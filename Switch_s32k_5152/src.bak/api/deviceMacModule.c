#include <apiInit.h>
#include <deviceMacModule.h>
#include <deviceVlanModule.h>
#include <string.h>
#include <stdlib.h>
#include "adlist.h"

#define  DEFAULT_FID_VALUE 1 //默认的操作的FID
#define  DEFAULT_AGE_TIME_MINUTE 5//默认超时时间
#define  VID_FLAG_ARRAY_SIZE 128

//每个设备最多支持512 * 8 = 4096个fid,如FID_SET_ARRAY[0][0]的bit0被设置为1，代表设备0的fid 0 存在MAC条目，如果为0，则代表fid 0不存在mac条目。加快查询所有FID的MAC地址条目速度
//#define  FID_ARRAY_LENGTH 512
//static MSD_U8 FID_SET_ARRAY[MAX_SOHO_DEVICES][FID_ARRAY_LENGTH];//sk32144不能这么做，没有这么大的内存可以，需要存放所有的fid。
static list *s_fids[MAX_SOHO_DEVICES];

/**
 * 用于描述ATU条目的相关属性状态
 */
typedef struct {
    AtuEntryType entryType;//细分了条目状态，也包括静态，动态，所有条目状态的地址
    MSD_BOOL  isVaild;//条目是否有效
    //MSD_BOOL is_static;//是否是静态条目地址
    //MSD_BOOL is_auto;//是否是动态地址
}AtuEntryStatus;

/**
 * ATU Module init type
 */
typedef struct {
    short fidNum;//设备使用的MAC -1:代表使用设备所有的fid，大于0的fid则使用指定的fid
    MacEntry  staticEntry[MAX_STATIC_ATU_ENTRIES];//用以存储所有有VID关联的静态MAC条目（最多允许的数据有限制）
    MSD_U8 staticEntryCount;//当前静态条目的条数
    MacEntry  autoEntry[MAX_AUTO_ATU_ENTRIES];//用以存储所有有VID关联的动态MAC条目
    MSD_U16  autoEntryCount;//当前动态条目的条数
    UseMacEntryType useEntryType;//使用条目的方式：使用静态条目，还是结合使用
    MSD_U32 vidArrayFlag[VID_FLAG_ARRAY_SIZE];//当对应的下标被设置为真时，有MAC条目的VID为该值，该VID条目不能被删除
    MSD_U16  allMacEntryVidSize;//当前所有MAC条目的所有VID集合,如果为0，则可以删除所有的VLAN条目。
}AtuModuleInitType;

static AtuModuleInitType s_atuModuleInitType[MAX_SOHO_DEVICES] = { 0 };

static AtuConfiguration s_atuConfiguration[MAX_SOHO_DEVICES] = { 0 };//atu 配置

extern DeviceConfig g_allDevicesConfig[MAX_SOHO_DEVICES];

#define FID_PARAM_CHECK(dev_num, fid) \
if ((fid < 0 || fid > MAX_FID_VALUE) || dev_num < 0 || dev_num >  MAX_SOHO_DEVICES - 1) \
        return MSD_BAD_PARAM;


extern MSD_STATUS deviceVlanModuleGetVlanEgressMode(IN MSD_U8 devNum, OUT VlanEgressMode* vlanEgressMode);

//pre define
static void deviceMacEntryAddToList(IN AtuEntryType entryType, IN MSD_ATU_ENTRY* checkAtuEntry, OUT MSD_ATU_ENTRY* atuEntry, OUT int* atuEntryCount);
static MSD_STATUS deviceAtuModuleSetUseMacEntryTypeToConfiguration(IN MSD_U8 devNum, IN UseMacEntryType useMacType);

/**
 * @brief vlanNumHasMacEntry 判断是否具有指定的VID的MAC条目，用于在VLAN执行删除操作时是否删除指定VLAN的目的
 * 注：如果VLAN执行删除VID条目时，如果还存在对应的MAC条目，则不执行VLAN条目的删除操作
 * @param devNum 设备编号
 * @param vidNum VID编号
 * @return
 */
static MSD_BOOL vlanNumHasMacEntry(MSD_U8 devNum, MSD_U16 vidNum) {
    for (int i = 0; i < MAX_AUTO_ATU_ENTRIES; ++i) {
        if (s_atuModuleInitType[devNum].autoEntry[i].vid == vidNum) {
            return MSD_TRUE;
        }
    }
    for (int i = 0; i < MAX_STATIC_ATU_ENTRIES; ++i) {
        if (s_atuModuleInitType[devNum].staticEntry[i].vid == vidNum) {
            return MSD_TRUE;
        }
    }
    return MSD_FALSE;
}

/**
 * @brief deviceAtuModuleSetMacEntryVidFlagAndSize 设置对应的VID条目为flag设置的标志，如果flag为真，即将其对应的VID条目标志设置为真。
 * @param devNum 设备编号
 * @param vidNum vid
 * @param flag 将VID条目设置为是否对应MAC条目的标志，如果设置为MSD_FALSE，告知VLAN条目操作可以删除对应的VTU
 * @return
 *   MSD_TRUE:当flag设置为TRUE，返回MSD_TRUE,或者vid对应的mac条目标志可以设置为FLASE时
 *   MSD_false:当vid对应的mac条目标志不能设置为FLASE时
 */
MSD_BOOL deviceAtuModuleSetMacEntryVidFlagAndSize(MSD_U8 devNum, MSD_U16 vidNum, MSD_BOOL flag)
{
	MSD_U16 arrayIndex = vidNum / 32;//在数组下标的位置
	MSD_U8 bitIndex = vidNum % 32;//在某个数下的坐标位置
	MSD_BOOL vidFlag = IS_BIT_SET(s_atuModuleInitType[devNum].vidArrayFlag[arrayIndex], bitIndex);
    if (flag == vidFlag)//要设置的vid标志和之前的一样，不做任何操作
        return MSD_TRUE;
    if (flag) {//如果设置为TRUE.
        s_atuModuleInitType[devNum].allMacEntryVidSize++;
        SET_BIT(s_atuModuleInitType[devNum].vidArrayFlag[arrayIndex], bitIndex);
        //s_atuModuleInitType[devNum].allMacEntryVid[vidNum] = MSD_TRUE;
        return MSD_TRUE;//
    }
    else {//设置为FLASE
        //判断是否可以设置为FALSE
        if (!vlanNumHasMacEntry(devNum, vidNum)) {//如果不存在对应的VID的相应MAC条目，则可以将其对应的VTU条目标志设置为FALSE.VLAN操作可以VTU条目删除
            s_atuModuleInitType[devNum].allMacEntryVidSize--;
            CLEAR_BIT(s_atuModuleInitType[devNum].vidArrayFlag[arrayIndex], bitIndex);
            //s_atuModuleInitType[devNum].allMacEntryVid[vidNum] = MSD_FALSE;
            return MSD_TRUE;
        }
        return MSD_FALSE;//不能设置为FALSE.
    }
}

/**
 * @brief deviceAtuModuleClearMacEntryVidFlag
 * 将所有的VID条目标志设置为FALSE.即VLAN操作可以清空所有的VLAN条目，否则VLAN操作不能执行删除所有的VLAN条目
 * @param devNum
 */
static void deviceAtuModuleClearMacEntryVidFlag(MSD_U8 devNum)
{
    for(int i = 0; i < VID_FLAG_ARRAY_SIZE; ++i){
    	s_atuModuleInitType[devNum].vidArrayFlag[i] = 0;
    }
    s_atuModuleInitType[devNum].allMacEntryVidSize = 0;
}


static inline int searchFid(void *listFidParam, void *findFidParam)
{
    MSD_U16 listFid = (MSD_U16)listFidParam;//地址直接转换为数字
    MSD_U16 findFid = (MSD_U16)findFidParam;
    return listFid == findFid;
}

static void initialFidArray(void)
{
    for (MSD_U8 j = 0; j < MAX_SOHO_DEVICES; ++j) {
    	s_fids[j] = listCreate();
    	listSetMatchMethod(s_fids[j],searchFid);//find id
    }
}

MSD_STATUS setFidValue(MSD_U8 devNum, IN MSD_U16 fid)
{

//	MSD_U16 *param = (MSD_U16 *)malloc(sizeof(MSD_U16));
//	*param = fid;
	if((listSearchKey(s_fids[devNum],(void *)fid)) == NULL){//不存在fid值，add
		if((listAddNodeHead(s_fids[devNum],(void *)fid)) == NULL){
			return MSD_FAIL;
		}
	}
    return MSD_OK;
}

static MSD_STATUS unsetFidValue(MSD_U8 devNum, IN MSD_U16 fid)
{
    if (fid == 0 || fid == 1)//FID 0和FID 1始终要查询
        return MSD_OK;
    listNode *fidNode = listSearchKey(s_fids[devNum],(void *)fid);//地址直接存储MSD_U16
    if(fidNode != NULL){//如果存在，则删除
    	listDelNode(s_fids[devNum],fidNode);
    }
    return MSD_OK;
}

static MSD_STATUS clearFidValues(MSD_U8 devNum)
{
    //FID 0 和 1 做特殊处理，不清空
    listEmpty(s_fids[devNum]);
    setFidValue(devNum,0);
    setFidValue(devNum,1);
    return MSD_OK;
}

void releaseAllFidValues(MSD_U8 devNum)
{
	listRelease(s_fids[devNum]);
}

MSD_STATUS deviceAtuModuleGetAllFidHasValues(IN MSD_U8 devNum, INOUT MSD_U16 fidArr[], IN int fidSize, OUT int* fidCount)
{
    *fidCount = 0;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (dev == NULL) {
        MSD_DBG_ERROR(("device_atu_module_set_fid failed, the dev_num's device is not open!\n"));
        return MSD_BAD_PARAM;
    }
    if (fidSize == 0 || fidArr == NULL)
        return MSD_BAD_PARAM;
    msdMemSet(fidArr, 0, fidSize * sizeof(MSD_U16));

    listIter * liter = listGetIterator(s_fids[devNum],AL_START_HEAD);
	listNode *node = NULL;
	int index = 0;
	while ((node = listNext(liter)) != NULL) {
		fidArr[index++] = (MSD_U16)node->value;
	}
	*fidCount = index;
	listReleaseIterator(liter);
    return MSD_OK;
}

/**
 * @brief getFirstAtuEntryInFid 获取某个FID的第一个ATU条目
 * @param devNum
 * @param fid
 * @param resultEntry
 * @return
 * MSD_OK  - on success
 * MSD_FAIL - on error
 * MSD_BAD_PARAM - if invalid parameter is given
 * MSD_NO_SUCH - no more entries.
 */
static MSD_STATUS getFirstAtuEntryInFid(IN MSD_U8 devNum, IN MSD_U32 fid, OUT MSD_ATU_ENTRY* resultEntry)
{
    msdMemSet(resultEntry, 0, sizeof(MSD_ATU_ENTRY));
    MSD_ETHERADDR  start_atu_entry_mac = { 0 };//开始查询的条目,从广播条目开始查询
    for (size_t i = 0; i < MSD_ETHERNET_HEADER_SIZE; ++i) {
        start_atu_entry_mac.arEther[i] = 0xff;
    }
    MSD_STATUS ret = msdFdbEntryNextGet(devNum, &start_atu_entry_mac, fid, resultEntry);
    return ret;
}

static MSD_STATUS deviceMacEntryGetListInFid(IN MSD_U8 devNum, IN MSD_U32 fid, OUT int* atuEntryCount, IN AtuEntryType entryType, OUT MSD_ATU_ENTRY* atuEntries, IN int atuEntryMaxSize)
{
    MSD_ATU_ENTRY resultEntry;
    msdMemSet(&resultEntry, 0, sizeof(MSD_ATU_ENTRY));
    for (size_t i = 0; i < MSD_ETHERNET_HEADER_SIZE; ++i) {
        resultEntry.macAddr.arEther[i] = 0xff;
    }
    MSD_STATUS ret = MSD_OK;
    while (1) {
        //获取下一个条目
        ret = msdFdbEntryNextGet(devNum, &resultEntry.macAddr, fid, &resultEntry);
        if(ret != MSD_OK) {
            if (ret == MSD_NO_SUCH) {//无更多条目
                ret = MSD_OK;
            }
            break;
        }
        if ((*atuEntryCount) == atuEntryMaxSize) return MSD_NO_SPACE;
        deviceMacEntryAddToList(entryType, &resultEntry, atuEntries, atuEntryCount);
        if (MSD_IS_BROADCAST_MAC(resultEntry.macAddr)) {
            break;
        }
    }
    return ret;
}

MSD_STATUS deviceMacModuleInitialMacInfo(IN MSD_U8 devNum)
{
    MSD_STATUS status = deviceAtuModuleSetUseMacEntryTypeToConfiguration(devNum, USE_MAC_ENTRY_TYPE_STATIC_AND_DYNAMIC);//
    if (status != MSD_OK) return status;
    status = deviceAtuModuleFlushEntries(devNum, MSD_FLUSH_ALL_NONSTATIC);//清空所有的MAC地址条目
    if (status != MSD_OK) return status;
    // status = deviceMacEntrySetAtuOperation(devNum); //这里会导致失败，看看如何解决
    // if (status != MSD_OK) return MSD_FAIL;
    //默认情况下，清空条目之后，可能会存在FID 0 和FID 1的MAC地址条目的存在。这里可能是操作API引起的。
    //status = deviceAtuModuleRefreshAutoEntries(devNum);//这里刷新操作可能没有作用
    //if (status != MSD_OK) return status;
    status = deviceAtuModuleSetPortDefaultFid(devNum, ALL_PORT_PARAM, 1);//不启用VLAN的默认FID设置为1.这样也会将默认的VID视为1.
    if (status != MSD_OK) return status;
    status = deviceAtuModuleAgingTimeoutSet(devNum, DEFAULT_AGE_TIME_MINUTE);
    if (status != MSD_OK) return status;
    return status;
}

/**
 * @brief checkMacEntryIsValid 检测MAC Entry条目格式是否正确
 * @param devNum
 * @param macEntry
 * @return 成功返回1，否则返回0
 */
static MSD_BOOL checkMacEntryIsValid(IN MSD_U8 devNum, IN MSD_ATU_ENTRY* macEntry)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);

    //端口的PortVec和端口类型判断
    if (macEntry->trunkMemberOrLAG != 0 && macEntry->trunkMemberOrLAG != 1) {
        return MSD_FALSE;
    }
    MSD_U8 ports = dev->numOfPorts;//端口数
    MSD_U32 maxPortVector = (1 << ports) - 1;//最大的PortVec(为普通的端口时)
    if (macEntry->trunkMemberOrLAG == 1) {//LAG端口时，只使用低5位（88Q5152交换机是这样的，不知道其他交换机是否是这样）
        maxPortVector = (1 << 5) - 1;
    }
    if (macEntry->portVec > maxPortVector) {
        return MSD_FALSE;
    }
    //entrystate判断
    if (macEntry->entryState > 0xf || macEntry->entryState == 0x0) { //负数会转换为大于0xf.
        return MSD_FALSE;
    }
    if (macEntry->fid > MAX_FID_VALUE) {
        return MSD_FALSE;
    }
    if (macEntry->exPrio.macFPri > 7 || macEntry->exPrio.macQPri > 7) {
        return MSD_FALSE;
    }
    if (s_atuModuleInitType[devNum].fidNum != ALL_FID_VALUE && (macEntry->fid != s_atuModuleInitType[devNum].fidNum)) {//如果指定了操作的FID，则仅仅可以操作指定的FID
        MSD_DBG_ERROR(("Mac Entry Add Failed, Please check the added Mac Entry fid is correct!\n"));
        return MSD_FALSE;
    }
    return MSD_TRUE;
}

/**
 * @brief deviceAtuModuleInitial
 * atu模块初始化调用，在调用atu模块的其他API接口需要先调用该接口
 * 默认情况下，所有设备的ATU操作对整个数据库的条目生效，如果仅仅想指定操作的FID数据库，参考 deviceAtuModuleSetFid
 * @return
 */
MSD_STATUS deviceAtuModuleInitial(void)
{
    initialFidArray();//初始化ATU需要的FID设置

    for (MSD_U8 i = 0; i < MAX_SOHO_DEVICES; ++i) {

        msdMemSet(&s_atuConfiguration[i], 0, sizeof(AtuConfiguration));
        s_atuModuleInitType[i].autoEntryCount = 0;
        s_atuModuleInitType[i].staticEntryCount = 0;
        s_atuModuleInitType[i].allMacEntryVidSize = 0;
        s_atuModuleInitType[i].fidNum = ALL_FID_VALUE;
        for(int i = 0; i < VID_FLAG_ARRAY_SIZE; ++i){
           	s_atuModuleInitType[i].vidArrayFlag[i] = 0;
        }
        //set_fid_value(i, ALL_FID_VALUE);//默认检测该fid的值（）
        for (int j = 0; j < MAX_AUTO_ATU_ENTRIES; ++j) {
        	CLEAR_BIT(s_atuModuleInitType[i].autoEntry[j].ageAndFlag, 2);//设置为无效状态
        }
        for (int j = 0; j < MAX_STATIC_ATU_ENTRIES; ++j) {
        	CLEAR_BIT(s_atuModuleInitType[i].staticEntry[j].ageAndFlag, 2);//设置为无效状态
        }
        //默认需要访问FID 1和FID 0的MAC条目
        setFidValue(i, 1);
        setFidValue(i, 0);

        s_atuModuleInitType[i].useEntryType = USE_MAC_ENTRY_TYPE_STATIC_AND_DYNAMIC; // auto + static
        s_atuConfiguration[i].useType = USE_MAC_ENTRY_TYPE_STATIC_AND_DYNAMIC; // auto + static
    }
    return MSD_OK;
}

MSD_STATUS deviceAtuModuleSetFid(MSD_U8 devNum, IN short fid)
{
	CHECK_DEV_NUM_IS_CORRECT;
    if (fid < ALL_FID_VALUE || fid > MAX_FID_VALUE) {
        MSD_DBG_ERROR(("the fid range is not correct,please check!\n"));
        return MSD_BAD_PARAM;
    }
    s_atuModuleInitType[devNum].fidNum = fid;
    return MSD_OK;
}

MSD_STATUS deviceAtuModuleGetFid(MSD_U8 devNum, OUT MSD_U16* fid)
{
	CHECK_DEV_NUM_IS_CORRECT;
    *fid = (MSD_U16)s_atuModuleInitType[devNum].fidNum;
    return MSD_OK;
}

MSD_STATUS deviceAtuModuleSetPortDefaultFid(MSD_U8 devNum, IN int portNum, IN MSD_U16 fid)
{
	CHECK_DEV_NUM_IS_CORRECT;
    if (fid > MAX_FID_VALUE) {
        return MSD_BAD_PARAM;
    }
    if (portNum < ALL_PORT_PARAM || portNum > dev->numOfPorts - 1) {
        MSD_DBG_ERROR(("device_atu_module_set_port_default_fid failed,the fid range is not correct,please check!\n"));
        return MSD_BAD_PARAM;
    }
    MSD_STATUS ret = MSD_OK;
    if (portNum == ALL_PORT_PARAM) {
        for (MSD_U8 i = 0; i < dev->numOfPorts; ++i) {
            ret = msdPortDefaultFIDNumberSet(devNum, i, fid);
            if (ret != MSD_OK) return ret;
        }
    }
    else {
        ret = msdPortDefaultFIDNumberSet(devNum, (MSD_LPORT)portNum, fid);
    }
    return ret;
}

MSD_STATUS deviceAtuModuleGetPortDefaultFid(MSD_U8 devNum, IN MSD_U8 portNum, OUT MSD_U16* fid)
{
	CHECK_DEV_NUM_IS_CORRECT;
    if (portNum > dev->numOfPorts - 1) {
        MSD_DBG_ERROR(("device_atu_module_get_port_default_fid failed,the fid range is not correct,please check!\n"));
        return MSD_BAD_PARAM;
    }
    MSD_STATUS ret = msdPortDefaultFIDNumberGet(devNum, (MSD_LPORT)portNum, fid);
    return ret;
}


#define FIR_GLOBAL1_DEV_ADDRESS   0x1b
#define FIR_GLOBAL2_DEV_ADDRESS   0x1c

/**
 * @brief checkStaticEntryIsExist 根据提供的VID和MAC地址判断MAC条目是否存在于对应的ATU条目中
 * @param devNum 设备编号
 * @param address 查询的MAC地址
 * @param vid 查询的VID
 * @param index如果找到，则存储下标，否则存储-1
 * @return 如果找到返回MSD_TRUE,否则返回MSD_FALSE
 */
static MSD_BOOL checkStaticEntryIsExist(MSD_U8 devNum, IN MSD_ETHERADDR address, IN MSD_U32 vid, MSD_BOOL isStatic, int* index)
{
    *index = -1;
    if(isStatic){
        for (int i = 0; i < MAX_STATIC_ATU_ENTRIES; ++i) {
            //if (s_atuModuleInitType[devNum].staticEntry[i].isVaild) {
        	 if(IS_BIT_SET(s_atuModuleInitType[devNum].staticEntry[i].ageAndFlag,2)){
                if (s_atuModuleInitType[devNum].staticEntry[i].vid == vid && MAC_IS_EQUAL(address, s_atuModuleInitType[devNum].staticEntry[i].address)) {
                    *index = i;
                    break;
                }
            }
        }
    }else{
        for (int i = 0; i < MAX_AUTO_ATU_ENTRIES; ++i) {
        	if(IS_BIT_SET(s_atuModuleInitType[devNum].autoEntry[i].ageAndFlag,2)){
                if (s_atuModuleInitType[devNum].autoEntry[i].vid == vid && MAC_IS_EQUAL(address, s_atuModuleInitType[devNum].autoEntry[i].address)) {
                    *index = i;
                    break;
                }
            }
        }
    }
    if (*index == -1)
        return MSD_FALSE;
    return MSD_TRUE;
}

/**
 * @brief addEntryToModuleIndex 根据VID和MAC地址找到要插入MAC地址地址的下标
 * @param devNum 设备编号
 * @param address
 * @param vid 对应的VID值
 * @param portVec 出口vec
 * @param entryState 添加条目的entrystate
 */
static void addEntryToModuleIndex(IN MSD_U8 devNum, IN MSD_ETHERADDR address, IN MSD_U16 vid, MSD_U32 portVec,int entryState)
{
    int index = -1;
    MSD_BOOL isUnicast = IS_UNICAST_MAC(address.arEther);
    MSD_BOOL isStatic = MSD_TRUE;
    if(entryState > 0 && entryState <= 7 && isUnicast){//如果为单播地址，并且条目状态小于7大于0，则为动态条目
        isStatic = MSD_FALSE;
    }
    do {
        MSD_BOOL isExist = checkStaticEntryIsExist(devNum, address, vid,isStatic, &index);
        if (isExist)//找到对应的静态或者动态MAC条目,替换
            break;
        if(isStatic){
            for (int i = 0; i < MAX_STATIC_ATU_ENTRIES; ++i) {
                //if (!s_atuModuleInitType[devNum].staticEntry[i].isVaild) {//序号没有被使用,使用该序号
            	if(!IS_BIT_SET(s_atuModuleInitType[devNum].staticEntry[i].ageAndFlag,2)){
                    index = i;
                    break;
                }
            }
        }else{
            for(int i = 0; i < MAX_AUTO_ATU_ENTRIES; ++i) {
                //if (!s_atuModuleInitType[devNum].autoEntry[i].isVaild) {//序号没有被使用,使用该序号
            	if(!IS_BIT_SET(s_atuModuleInitType[devNum].autoEntry[i].ageAndFlag,2)){
                    index = i;
                    break;
                }
            }
        }
    } while (0);
    if (index != -1) {
        MSD_BOOL isMul = IS_MULTICAST_MAC(address.arEther);
        if(isStatic){ //static entry
            for (unsigned i = 0; i < MSD_ETHERNET_HEADER_SIZE; ++i) {
                s_atuModuleInitType[devNum].staticEntry[index].address.arEther[i] = address.arEther[i];
            }
//            s_atuModuleInitType[devNum].staticEntry[index].isStatic = MSD_TRUE;
//            s_atuModuleInitType[devNum].staticEntry[index].isUnicast = !isMul;
//            s_atuModuleInitType[devNum].staticEntry[index].isVaild = MSD_TRUE;
            SET_BIT(s_atuModuleInitType[devNum].staticEntry[index].ageAndFlag,0);
            if(isMul){//多播
            	CLEAR_BIT(s_atuModuleInitType[devNum].staticEntry[index].ageAndFlag,1);
            }else{//单播
            	SET_BIT(s_atuModuleInitType[devNum].staticEntry[index].ageAndFlag,1);
            }
            SET_BIT(s_atuModuleInitType[devNum].staticEntry[index].ageAndFlag,2);
            s_atuModuleInitType[devNum].staticEntry[index].portVec = portVec;
            s_atuModuleInitType[devNum].staticEntry[index].vid = vid;
            //s_atuModuleInitType[devNum].staticEntry[index].age = 0xfff;//静态条目不用管
            s_atuModuleInitType[devNum].staticEntryCount++;
        }else{//auto entry
            for (unsigned i = 0; i < MSD_ETHERNET_HEADER_SIZE; ++i) {
                s_atuModuleInitType[devNum].autoEntry[index].address.arEther[i] = address.arEther[i];
            }
            CLEAR_BIT(s_atuModuleInitType[devNum].autoEntry[index].ageAndFlag,0);
			if(isMul){//多播
				CLEAR_BIT(s_atuModuleInitType[devNum].autoEntry[index].ageAndFlag,1);
			}else{//单播
				SET_BIT(s_atuModuleInitType[devNum].autoEntry[index].ageAndFlag,1);
			}
			SET_BIT(s_atuModuleInitType[devNum].staticEntry[index].ageAndFlag,2);
            s_atuModuleInitType[devNum].autoEntry[index].portVec = portVec;
            s_atuModuleInitType[devNum].autoEntry[index].vid = vid;
            s_atuModuleInitType[devNum].autoEntry[index].ageAndFlag |= ((entryState & 0x7) << 3);//设置age
//            s_atuModuleInitType[devNum].autoEntry[index].age = entryState & 0x7;
            s_atuModuleInitType[devNum].autoEntryCount++;
        }
    }
}

/**
 * @brief removeStaticOrAutoMacEntryToModuleIndex 将某个地址和VID的MAC地址条目中从内存中删除
 * @param devNum 设备编号
 * @param address MAC地址
 * @param vid VID值
 * @param entryState 条目entryState(静态的需要设置为大于0x7的值)
 * @return
 */
static void removeStaticOrAutoMacEntryToModuleIndex(IN MSD_U8 devNum, IN MSD_ETHERADDR address, IN MSD_U32 vid, int entryState)
{
    MSD_BOOL isUnicast = IS_UNICAST_MAC(address.arEther);
    MSD_BOOL isStatic = MSD_TRUE;
    if(entryState > 0 && entryState <= 7 && isUnicast){//如果为单播地址，并且条目状态小于7大于0，则为动态条目
        isStatic = MSD_FALSE;
    }
    if (isStatic) {//静态条目
        for (int i = 0; i < MAX_STATIC_ATU_ENTRIES; ++i) {
//            if (s_atuModuleInitType[devNum].staticEntry[i].isVaild) {
        	  if(IS_BIT_SET(s_atuModuleInitType[devNum].staticEntry[i].ageAndFlag,2)){
                if (s_atuModuleInitType[devNum].staticEntry[i].vid == vid && MAC_IS_EQUAL(address, s_atuModuleInitType[devNum].staticEntry[i].address)) {
//                    s_atuModuleInitType[devNum].staticEntry[i].isVaild = MSD_FALSE;
                	CLEAR_BIT(s_atuModuleInitType[devNum].staticEntry[i].ageAndFlag,2);//not Valid
                    s_atuModuleInitType[devNum].staticEntryCount--;
                    break;
                }
            }
        }
    }
    else {
        for (int i = 0; i < MAX_AUTO_ATU_ENTRIES; ++i) {
//            if (s_atuModuleInitType[devNum].autoEntry[i].isVaild) {
        	  if(IS_BIT_SET(s_atuModuleInitType[devNum].autoEntry[i].ageAndFlag,2)){
                if (s_atuModuleInitType[devNum].autoEntry[i].vid == vid && MAC_IS_EQUAL(address, s_atuModuleInitType[devNum].autoEntry[i].address)) {
//                    s_atuModuleInitType[devNum].autoEntry[i].isVaild = MSD_FALSE;
                	CLEAR_BIT(s_atuModuleInitType[devNum].autoEntry[i].ageAndFlag,2);////not Valid
                    s_atuModuleInitType[devNum].autoEntryCount--;
                    break;
                }
            }
        }
    }
}

/**
 * @brief setStaticOrAutoMacEntryUnvalid
 * 设置某个VID或者所有VID（UNVALID_VID）的静态条目或者动态条目为无效(remove_static_or_auto_mac_entry_to_module_index是如果存在，则删除一个MAC条目)。可能会删除多个条目或者移除所有条目
 * @param devNum 设备编号
 * @param vid VID值
 * @param isStatic 是否是静态
 */
static void setStaticOrAutoMacEntryUnvalid(IN MSD_U8 devNum, IN int vid, MSD_BOOL isStatic)
{
    if (isStatic) {
        if (vid == UNVALID_VID) {//设置所有VID的静态条目的无效
            for (int i = 0; i < MAX_STATIC_ATU_ENTRIES; ++i) {
//                s_atuModuleInitType[devNum].staticEntry[i].isVaild = MSD_FALSE;
            	CLEAR_BIT(s_atuModuleInitType[devNum].staticEntry[i].ageAndFlag,2);//not Valid
            }
            s_atuModuleInitType[devNum].staticEntryCount = 0;
        }
        else {//设置指定VID的所有静态条目为无效
            for (int i = 0; i < MAX_STATIC_ATU_ENTRIES; ++i) {
                if ((s_atuModuleInitType[devNum].staticEntry[i].vid == vid) && IS_BIT_SET(s_atuModuleInitType[devNum].staticEntry[i].ageAndFlag,2)) {
//                    s_atuModuleInitType[devNum].staticEntry[i].isVaild = MSD_FALSE;
                	CLEAR_BIT(s_atuModuleInitType[devNum].staticEntry[i].ageAndFlag,2);////not Valid
                    s_atuModuleInitType[devNum].staticEntryCount--;
                }
            }
        }
    }
    else {
        if (vid == UNVALID_VID) {//设置所有VID的atuo条目无效
            for (int i = 0; i < MAX_AUTO_ATU_ENTRIES; ++i) {
                //s_atuModuleInitType[devNum].autoEntry[i].isVaild = MSD_FALSE;
                CLEAR_BIT(s_atuModuleInitType[devNum].autoEntry[i].ageAndFlag,2);////not Valid
            }
            s_atuModuleInitType[devNum].autoEntryCount = 0;
        }
        else {//设置指定VID的所有auto条目无效
            for (int i = 0; i < MAX_AUTO_ATU_ENTRIES; ++i) {
                if (s_atuModuleInitType[devNum].autoEntry[i].vid == vid && IS_BIT_SET(s_atuModuleInitType[devNum].autoEntry[i].ageAndFlag,2)) {
//                    s_atuModuleInitType[devNum].autoEntry[i].isVaild = MSD_FALSE;
                	CLEAR_BIT(s_atuModuleInitType[devNum].autoEntry[i].ageAndFlag,2);////not Valid
                    s_atuModuleInitType[devNum].autoEntryCount--;
                }
            }
        }
    }
}


/**
 * @brief checkVlanEntry 检测是否需要添加或者修改相应的VTU条目
 * @param devNum 设备编号
 * @param vid 要查询的VID
 * @param isAdd 是否添加VTU条目的标志
 * @return
 */
MSD_STATUS checkVlanEntry(IN MSD_U8 devNum, IN MSD_U16 vid, OUT MSD_BOOL* isAdd)
{
    //判断是否需要修改VID条目，默认情况下，88Q5152仅仅只有VID 1和VID fff，不存在其余VID条目，不存在的VID条目对应的端口memberTag为MSD_MEMBER_EGRESS_UNMODIFIED,即指定VID的帧可以从该端口出口
    //但是默认FID为Port 0ffset 0x5和0x6的FID bit决定，这样我们就需要修改其对应的FID条目
    *isAdd = MSD_FALSE;
    MSD_VTU_ENTRY vtuEntry;
    MSD_BOOL isFoundVtu;
    MSD_STATUS ret = msdVlanEntryFind(devNum, vid, &vtuEntry, &isFoundVtu);//先查询，如果查询到，则会使用到查询的VID条目信息，然后进行替换。
    if (ret != MSD_OK){
        return ret;
    }
    //是否需要添加或者修改对应VID的VTU条目,需要添加或者修改的VTU条目如下
    //1. VTU条目不存在，需要添加VTU条目 修改将FID值修改为对应的VID值
    MSD_BOOL isShouldAddOrModifyVtu = MSD_FALSE;//注意，是否需要显示已添加的VID条目，需要再VTU功能那边去判断，即前端不要求显示的VID条目不应该显示在页面上。
    if (!isFoundVtu) {//没有找到VTU的情况，默认FID为Port 0ffset 0x5和0x6的FID bit决定，需要修改相应的FID为VID。
        isShouldAddOrModifyVtu = MSD_TRUE;
        VlanEgressMode egressMode[MSD_MAX_SWITCH_PORTS] = { VLAN_EGRESS_MODE_ALLOW_ALL };
        deviceVlanModuleGetVlanEgressMode(devNum, egressMode);
        MSD_QD_DEV* dev = sohoDevGet(devNum);
        for (MSD_U8 i = 0; i < dev->numOfPorts; ++i) {
            if (egressMode[i] == VLAN_EGRESS_MODE_ALLOW_MEMBERSHIP) { //如果出口端口为允许指定的VLAN出口，则将该VLAN在其端口的memberTag设置为NOT A MEMBER
                vtuEntry.memberTagP[i] = MSD_NOT_A_MEMBER;
            }
        }
        *isAdd = MSD_TRUE;
    }
    else {//找到VTU条目
        if (vtuEntry.fid != vid) {//如果其fid和vid不同，则修改(如果在VLAN模块强制fid == VID，就不需要判断)
            isShouldAddOrModifyVtu = MSD_TRUE;
        }
    }
    if (isShouldAddOrModifyVtu) { //如果需要修改或者添加VTU条目
        vtuEntry.vid = vid;
        vtuEntry.fid = vid;//设置FID和VID相同
        ret = msdVlanEntryAdd(devNum, &vtuEntry);//通常情况下，如果不存在对应的VLAN条目，默认相当于添加MSD_MEMBER_EGRESS_UNMODIFIED方式的VLAN条目
        if (ret != MSD_OK){
            return ret;
        }

    }
    //设置VLAN有对应的MAC条目不能删除
    deviceAtuModuleSetMacEntryVidFlagAndSize(devNum, vid, MSD_TRUE);
    return MSD_OK;
}

MSD_STATUS deviceAtuModuleAddEntry(MSD_U8 devNum, IN MSD_ATU_ENTRY* macEntry)
{
	CHECK_DEV_NUM_IS_CORRECT;
    //only check static entry
    if (s_atuModuleInitType[devNum].staticEntryCount == MAX_STATIC_ATU_ENTRIES) {
        MSD_DBG_ERROR(("device_atu_module_add_without_vid_entry failed,static mac entry is alreay reached to the max config value!\n"));
        return MSD_NO_SPACE;
    }
    if (macEntry == NULL) {
        MSD_DBG_ERROR(("device_atu_module_add Failed,the added Mac Entry is NULL!\n"));
        return MSD_BAD_PARAM;
    }
    if (!checkMacEntryIsValid(devNum, macEntry)) {
        MSD_DBG_ERROR(("device_atu_module_add Failed Failed, Please check the added Mac Entry param!\n"));
        return MSD_BAD_PARAM;
    }
    MSD_STATUS ret = msdFdbMacEntryAdd(devNum, macEntry);
    if (ret != MSD_OK) return ret;
    setFidValue(devNum, macEntry->fid);//添加成功,将指定的fid加入到查询的数据库中

    //这里添加mac条目的fid就相当于和vid值一一对应。即该条目和VID值为mac_entry->fid的VTU条目对应
    addEntryToModuleIndex(devNum, macEntry->macAddr, macEntry->fid, macEntry->portVec,macEntry->entryState);
    //removeStaticOrAutoMacEntryToModuleIndex(devNum, macEntry->macAddr, macEntry->fid, macEntry->entryState);//如果添加了相同的静态地址和VID，则将动态条目移除

    //虽然没有指定VID参数，但是由于使用了多FID，如果不设置VID值，会默认使用端口的默认FID，然后去查询对应的条目，这不是我们想要的，为了和添加与VID相关的API对应，这里将MAC条目的FID来决定是否要添加对应的VID条目。
    //这里当指定VID的帧到达时，就会和FID对应的MAC条目一一对应。
    MSD_BOOL isAdd;
    ret = checkVlanEntry(devNum, macEntry->fid, &isAdd);//
    return MSD_OK;
}

MSD_STATUS deviceAtuModuleAgingTimeoutSet(MSD_U8 devNum, MSD_U8 ageTimeMinute)
{
	CHECK_DEV_NUM_IS_CORRECT;
    if (ageTimeMinute > MAX_AGE_OUT_MINUTE) {
        MSD_DBG_ERROR(("Set Aging Timeout Failed,the time unit param range is between 0 and %d.\n", MAX_AGE_OUT_MINUTE));
        return MSD_BAD_PARAM;
    }
    MSD_U32 timeout = ageTimeMinute * 3750 * 16; //以毫秒为单位
    MSD_STATUS status = msdFdbAgingTimeoutSet(devNum, timeout);
    if (status == MSD_OK) {//设置OK
        s_atuConfiguration[devNum].ageOutMinute = ageTimeMinute;
    }
    return status;
}

MSD_STATUS deviceAtuModuleAgingTimeoutGet(MSD_U8 devNum, OUT MSD_U8* ageTimeMinute)
{
    *ageTimeMinute = 0;
    CHECK_DEV_NUM_IS_CORRECT;
    MSD_U32 ageTimeout;
    MSD_STATUS status = msdFdbAgingTimeoutGet(devNum, &ageTimeout);
    if (status != MSD_OK) return status;
    *ageTimeMinute = (MSD_U8)ageTimeout / (3750 * 16);
    s_atuConfiguration[devNum].ageOutMinute = *ageTimeMinute;
    return status;
}

MSD_STATUS deviceAtuModuleDeleteEntry(MSD_U8 devNum, IN MSD_ETHERADDR macAddr)
{
	CHECK_DEV_NUM_IS_CORRECT;
    int fid = s_atuModuleInitType[devNum].fidNum; //fid为-1，删除所有数据库的fid，否则仅仅删除指定fid的数据库
    MSD_STATUS ret = 0;
    MSD_ATU_ENTRY atuEntry = { 0 };
    MSD_BOOL isFound;
    if (fid >= 0) {//大于0，删除指定的fid的地址条目
        ret = msdFdbMacEntryFind(devNum,&macAddr,fid,&atuEntry,&isFound);
        if(ret != MSD_OK || !isFound){//没有找到，也直接返回
            return ret;
        }
        ret = msdFdbMacEntryDelete(devNum, &macAddr, fid);

        if (ret == MSD_OK) {
            MSD_U16 vid = fid;
            removeStaticOrAutoMacEntryToModuleIndex(devNum, macAddr, vid, atuEntry.entryState);
            deviceAtuModuleSetMacEntryVidFlagAndSize(devNum, vid, MSD_FALSE);
            MSD_ATU_ENTRY resultEntry;
            ret = getFirstAtuEntryInFid(devNum, fid, &resultEntry);
            if (ret == MSD_NO_SUCH) {//删除之后该fid不存在对应的MAC条目，将fid删除
                unsetFidValue(devNum, fid);
                ret = MSD_OK;
            }
            else if (ret != MSD_OK) {
                return ret;
            }
        }
        else {
            return ret;
        }
    }
    else { //删除所有fid的条目
        int fidSize = MAX_FID_VALUE + 1;
        MSD_U16* fidArray = (MSD_U16*)calloc(fidSize, sizeof(MSD_U16));
        if (fidArray == NULL) return MSD_NO_SPACE;
        int fidCount = 0;
        deviceAtuModuleGetAllFidHasValues(devNum, fidArray, fidSize, &fidCount);
        for (int i = 0; i < fidCount; ++i) {
            int fid = fidArray[i];
            ret = msdFdbMacEntryFind(devNum,&macAddr,fid,&atuEntry,&isFound);
            if(ret != MSD_OK){
                return ret;
            }
            if(!isFound){
                continue;
            }
            ret = msdFdbMacEntryDelete(devNum, &macAddr, fid);
            if (ret == MSD_OK) {
                removeStaticOrAutoMacEntryToModuleIndex(devNum, macAddr, fid, atuEntry.entryState);
                MSD_BOOL flag = deviceAtuModuleSetMacEntryVidFlagAndSize(devNum, fid, MSD_FALSE);
                if (flag) {//可以删除对应的VTU条目
                    /*ret = msdVlanEntryDelete(dev_num, vid);
                    if (ret != MSD_OK)
                        return ret;*/
                }
                MSD_ATU_ENTRY result_entry;
                msdMemSet(&result_entry, 0, sizeof(MSD_ATU_ENTRY));
                ret = getFirstAtuEntryInFid(devNum, fidArray[i], &result_entry);
                if (ret == MSD_NO_SUCH) {//删除之后该fid不存在对应的MAC条目，将fid删除
                    unsetFidValue(devNum, fidArray[i]);
                    ret = MSD_OK;
                }
                else if (ret != MSD_OK) {
                    break;
                }
            }
            else {
                break;
            }

        }
        vPortFree(fidArray);
        fidArray = NULL;
    }
    return ret;
}

/**
 * @brief checkMacEntryIsStaticOrDynamicMac 判断条目是静态条目还是动态条目
 * @param atuEntry 要判断的ATU条目
 * @param entryStatus 返回结果，1-静态条目，2-动态条目
 */
static inline void checkMacEntryIsStaticOrDynamicMac(IN MSD_ATU_ENTRY* atuEntry, OUT AtuEntryStatus* entryStatus)
{
    entryStatus->isVaild = MSD_TRUE;//默认有效
    if (atuEntry->macAddr.arEther[0] == 0xff && atuEntry->macAddr.arEther[1] == 0xff
        && atuEntry->macAddr.arEther[2] == 0xff && atuEntry->macAddr.arEther[3] == 0xff
        && atuEntry->macAddr.arEther[4] == 0xff && atuEntry->macAddr.arEther[5] == 0xff) {//广播条目
        if (atuEntry->entryState == 0x0 || atuEntry->entryState == 0x2 || atuEntry->entryState == 0x3 ||
            atuEntry->entryState == 0x8 || atuEntry->entryState == 0xA || atuEntry->entryState == 0xB) {//未使用或者保留
            entryStatus->entryType = ATU_NETRY_TYPE_UNUSED_OR_RESERVED_BROCASTCAST;
            entryStatus->isVaild = MSD_FALSE;
        }
        else if ((atuEntry->entryState == 0x1) || (atuEntry->entryState == 0x9)) { //E-CID条目
            entryStatus->entryType = ATU_ENTRY_TYPE_ECID_BROCASTCAST;
        }
        else {//静态条目
            entryStatus->entryType = ATU_ENTRY_TYPE_STATIC_BROCASTCAST;
        }
    }
    else { //单播或者多播
        if (atuEntry->macAddr.arEther[0] % 2 == 0) {//单播
            /* if (atuEntry->entryState == 0x0) {//未使用
                entryStatus->entryType = ATU_NETRY_TYPE_UNUSED_OR_RESERVED_UNICAST;
                entryStatus->isVaild = MSD_FALSE;
            }
            else*/
            if (atuEntry->entryState <= 0x7) {//动态条目
                entryStatus->entryType = ATU_ENTRY_TYPE_AUTO;
            }
            else { //静态条目
                entryStatus->entryType = ATU_ENTRY_TYPE_STATIC_UNICAST;
            }
        }
        else { //多播
            if (atuEntry->entryState == 0x0 || atuEntry->entryState == 0x2 || atuEntry->entryState == 0x3 ||
                atuEntry->entryState == 0x8 || atuEntry->entryState == 0xA || atuEntry->entryState == 0xB) {//未使用或者保留
                entryStatus->entryType = ATU_ENTRY_TYPE_UNUSED_OR_RESERVED_MULTICAST;
                entryStatus->isVaild = MSD_FALSE;
            }
            else if ((atuEntry->entryState == 0x1) || (atuEntry->entryState == 0x9)) { //E-CID条目
                entryStatus->entryType = ATU_ENTRY_TYPE_ECID_MULTICAST;
            }
            else {//静态条目
                entryStatus->entryType = ATU_ENTRY_TYPE_STATIC_MULTICAST;
            }
        }
    }
}

/**
 * @brief deviceMacEntryFlushStaticInFid 清空指定fid的所有静态条目
 * @param devNum
 * @param fid
 * @return
 */
static MSD_STATUS deviceMacEntryFlushStaticInFid(IN MSD_U8 devNum, IN MSD_U32 fid)
{
    //MSD_BOOL is_find_broadcast = MSD_FALSE;//需要判断广播地址是否在地址表中，如果存在，使用msdFdbEntryNextGet找到最后一个MAC地址时，再次调用时，会一直返回MSD_OK，个人认为这是API的一个BUG。这里根据地址相同来判断是否是最后一个地址

    MSD_ATU_ENTRY resultEntry;
    MSD_STATUS ret = getFirstAtuEntryInFid(devNum, fid, &resultEntry);
    if (ret != MSD_OK) //数据库条目为空或者错误
    {
        if (ret == MSD_NO_SUCH) {
            unsetFidValue(devNum, fid);
            ret = MSD_OK;
        }
        return ret;
    }
    MSD_ETHERADDR  prevEntryMac;//保存上一条MAC地址条目的MAC地址，用于判断是否找到最后一个地址。
    msdMemCpy(prevEntryMac.arEther, resultEntry.macAddr.arEther, MSD_ETHERNET_HEADER_SIZE);
    MSD_DBG_INFO(("device_mac_entry_flush_static_in_fid MAC:%02x:%02x:%02x:%02x:%02x:%02x\n", resultEntry.macAddr.arEther[0], resultEntry.macAddr.arEther[1],
                  resultEntry.macAddr.arEther[2], resultEntry.macAddr.arEther[3], resultEntry.macAddr.arEther[4], resultEntry.macAddr.arEther[5]));
    AtuEntryStatus atuEntryStatus;
    while (1) {
        //判断单播还是多播（arEther[0]为偶数则为单播，如果为奇数则为多播）
        checkMacEntryIsStaticOrDynamicMac(&resultEntry,&atuEntryStatus);
        if (atuEntryStatus.entryType == ATU_ENTRY_TYPE_STATIC_MULTICAST ||
            atuEntryStatus.entryType == ATU_ENTRY_TYPE_STATIC_BROCASTCAST ||
            atuEntryStatus.entryType == ATU_ENTRY_TYPE_STATIC_UNICAST) {
            ret = msdFdbMacEntryDelete(devNum, &resultEntry.macAddr, fid);
            if (ret != MSD_OK)
                break;
        }
        ret = msdFdbEntryNextGet(devNum, &resultEntry.macAddr, fid, &resultEntry);
        MSD_DBG_INFO(("device_mac_entry_flush_static_in_fid MAC:%02x:%02x:%02x:%02x:%02x:%02x\n", resultEntry.macAddr.arEther[0], resultEntry.macAddr.arEther[1],
                      resultEntry.macAddr.arEther[2], resultEntry.macAddr.arEther[3], resultEntry.macAddr.arEther[4], resultEntry.macAddr.arEther[5]));
        if (ret != MSD_OK || MAC_IS_EQUAL(prevEntryMac, resultEntry.macAddr)) {
            if (ret == MSD_NO_SUCH) {//fid中所有ATU项都删除成功
                ret = MSD_OK;
            }
            break;
        }
        msdMemCpy(prevEntryMac.arEther, resultEntry.macAddr.arEther, MSD_ETHERNET_HEADER_SIZE);
    }
    //要判断当前fid是否还存在条目，如果不存在，应该删除对应fid，否则调用显示列表，不会显示任何数据。
    msdMemSet(&resultEntry, 0, sizeof(MSD_ATU_ENTRY));
    ret = getFirstAtuEntryInFid(devNum, fid, &resultEntry);
    //if (ret == MSD_NO_SUCH && is_unset_fid) {//删除之后该fid不存在对应的MAC条目（注意广播条目需要单独判断），将fid删除
    if (ret == MSD_NO_SUCH) {
        ret = MSD_OK;
        unsetFidValue(devNum, fid);
    }
    return ret;
}

MSD_STATUS deviceAtuModuleFlushEntries(MSD_U8 devNum, IN MSD_FLUSH_CMD flushCmd)
{
	CHECK_DEV_NUM_IS_CORRECT;
    if (flushCmd < MSD_FLUSH_ALL || flushCmd > MSD_FLUSH_ALL_STATIC) {
        MSD_DBG_ERROR(("delete atu entry failed,because the flush cmd param is out of range,please check!\n"));
        return MSD_BAD_PARAM;
    }
    int fid = s_atuModuleInitType[devNum].fidNum;
    int fidArraySize = MAX_FID_VALUE + 1;
    MSD_U16* fidArray = (MSD_U16*)calloc(fidArraySize, sizeof(MSD_U16));
    int fidCount = 0;
    if (fidArray == NULL) {
        return MSD_NO_SPACE;
    }
    MSD_STATUS ret = MSD_OK;
    if (flushCmd == MSD_FLUSH_ALL_STATIC) { //删除静态条目
        if (fid == ALL_FID_VALUE) //删除所有的fid的静态条目
        {
            deviceAtuModuleGetAllFidHasValues(devNum, fidArray, fidArraySize, &fidCount);
            for (int i = 0; i < fidCount; ++i) { //
                ret = deviceMacEntryFlushStaticInFid(devNum,fidArray[i]);
                if (ret != MSD_OK)
                    break;
                deviceAtuModuleSetMacEntryVidFlagAndSize(devNum, fidArray[i], MSD_FALSE);//如果vid还存在对应的MAC条目，则不能设置成功
            }
            setStaticOrAutoMacEntryUnvalid(devNum, UNVALID_VID, MSD_TRUE);
        }
        else {//删除指定的FID的静态条目
            deviceMacEntryFlushStaticInFid(devNum, fid);
            if (ret == MSD_OK) {
                setStaticOrAutoMacEntryUnvalid(devNum, fid, MSD_TRUE);
            }
            deviceAtuModuleSetMacEntryVidFlagAndSize(devNum, fid, MSD_FALSE);
        }
    }
    else { //刷新所有条目 或者非静态条目
        if (fid == ALL_FID_VALUE) {
            ret = msdFdbAllDelete(devNum, flushCmd);
            if (ret == MSD_OK) {
                deviceAtuModuleGetAllFidHasValues(devNum, fidArray, fidArraySize, &fidCount);
                if (flushCmd == MSD_FLUSH_ALL_NONSTATIC) {//清空所有fid的非静态条目
                    for (int i = 0; i < fidCount; ++i) {
                        MSD_ATU_ENTRY result_entry;
                        msdMemSet(&result_entry, 0, sizeof(MSD_ATU_ENTRY));
                        ret = getFirstAtuEntryInFid(devNum, fidArray[i], &result_entry);//
                        if (ret == MSD_NO_SUCH) {//删除之后该fid不存在对应的MAC条目，将fid删除
                            unsetFidValue(devNum, fidArray[i]);
                            ret = MSD_OK;
                        }
                        deviceAtuModuleSetMacEntryVidFlagAndSize(devNum, fidArray[i], MSD_FALSE);
                    }
                    setStaticOrAutoMacEntryUnvalid(devNum, UNVALID_VID, MSD_FALSE);
                }
                else { //删除所有fid的所有条目
                    //清空所有的内存条目信息
                    clearFidValues(devNum);
                    setStaticOrAutoMacEntryUnvalid(devNum, UNVALID_VID, MSD_TRUE);
                    setStaticOrAutoMacEntryUnvalid(devNum, UNVALID_VID, MSD_FALSE);
                    deviceAtuModuleClearMacEntryVidFlag(devNum);//
                }
            }
        }
        else {//Flush某个FID的所有条目
            ret = msdFdbAllDeleteInDB(devNum, flushCmd, fid);
            int vid = fid;
            if (ret == MSD_OK) {
                if (flushCmd == MSD_FLUSH_ALL) {//清空某个fid的所有条目
                    unsetFidValue(devNum, fid);
                    setStaticOrAutoMacEntryUnvalid(devNum, vid, MSD_TRUE);
                    setStaticOrAutoMacEntryUnvalid(devNum, vid, MSD_FALSE);
                }
                else {//清空某个fid的非静态条目
                    MSD_ATU_ENTRY resultEntry;
                    msdMemSet(&resultEntry, 0, sizeof(MSD_ATU_ENTRY));
                    ret = getFirstAtuEntryInFid(devNum, fid, &resultEntry);
                    if (ret == MSD_NO_SUCH) {//删除之后该fid不存在对应的MAC条目，将fid删除
                        unsetFidValue(devNum, fid);
                        ret = MSD_OK;
                    }
                    setStaticOrAutoMacEntryUnvalid(devNum, vid, MSD_FALSE);
                }
                MSD_BOOL flag = deviceAtuModuleSetMacEntryVidFlagAndSize(devNum, vid, MSD_FALSE);
                if (flag) {//可以删除对应的VTU条目
                    /*ret = msdVlanEntryDelete(dev_num, vid);
                    if (ret != MSD_OK)
                        return ret;*/
                }
            }
        }
    }
    vPortFree(fidArray);
    fidArray = NULL;
    return ret;
}

/**********************************************************************************************
 * @brief deviceMacEntryAddToList 判断条目是否可以加入到指定的列表中
 * @param entryType 要添加的类型
 * @param checkAtuEntry  要判断的条目
 * @param atuEntries 存放条目列表
 * @param atuEntryCount
 ***********************************************************************************************/
static void deviceMacEntryAddToList(IN AtuEntryType entryType, IN MSD_ATU_ENTRY* checkAtuEntry, OUT MSD_ATU_ENTRY* atuEntries, OUT int* atuEntryCount) {
    //判断条目类型
    MSD_BOOL isAddResult = MSD_FALSE;
    if (entryType == ATU_ENTRY_TYPE_ALL) {
        isAddResult = MSD_TRUE;
    }
    else { //如果不是获取所有条目，则判断条目状态
        //静态条目，是否有效等。
        AtuEntryStatus atuEntryStatus;
        checkMacEntryIsStaticOrDynamicMac(checkAtuEntry, &atuEntryStatus);
        if ((entryType == ATU_ENTRY_TYPE_STATIC &&
             (atuEntryStatus.entryType == ATU_ENTRY_TYPE_STATIC_MULTICAST ||
              atuEntryStatus.entryType == ATU_ENTRY_TYPE_STATIC_UNICAST ||
              atuEntryStatus.entryType == ATU_ENTRY_TYPE_STATIC_BROCASTCAST)) ||
            (entryType == atuEntryStatus.entryType) ||
            (entryType == ATU_ENTRY_TYPE_ALL_VALID && atuEntryStatus.isVaild) ||
            (entryType == ATU_ENTRY_TYPE_ALL_INVALID && (!atuEntryStatus.isVaild))) {
            isAddResult = MSD_TRUE;
        }
    }
    if (isAddResult) {
        int index = *atuEntryCount;
        msdMemCpy(atuEntries[index].macAddr.arEther, checkAtuEntry->macAddr.arEther, MSD_ETHERNET_HEADER_SIZE);
        atuEntries[index].entryState = checkAtuEntry->entryState;
        atuEntries[index].exPrio.macFPri = checkAtuEntry->exPrio.macFPri;
        atuEntries[index].exPrio.macQPri = checkAtuEntry->exPrio.macQPri;
        atuEntries[index].fid = checkAtuEntry->fid;
        atuEntries[index].portVec = checkAtuEntry->portVec;
        atuEntries[index].trunkMemberOrLAG = checkAtuEntry->trunkMemberOrLAG;
        (*atuEntryCount)++;
    }
}


MSD_STATUS deviceAtuModuleGetList(MSD_U8 devNum, AtuEntryType entryType, OUT MSD_ATU_ENTRY* atuEntry, IN int atuEntryMaxSize, OUT int* atuEntryCount)
{
    *atuEntryCount = 0;
    CHECK_DEV_NUM_IS_CORRECT;
    if (entryType < 1 || entryType > ATU_ENTRY_TYPE_SIZE) {
        MSD_DBG_ERROR(("Get mac entry list failed,because the mac entry type param is out of range,please check!\n"));
        return MSD_BAD_PARAM;
    }
    if (atuEntryMaxSize <= 0) {
        MSD_DBG_ERROR(("Get mac entry list failed,because the atu_entry_max_size param is should be greater than 0!\n"));
        return MSD_BAD_PARAM;
    }
    if (atuEntry == NULL) {
        MSD_DBG_ERROR(("Get mac entry list failed,,because the atu enty param is NULL,please check!\n"));
        return MSD_BAD_PARAM;
    }
    msdMemSet(atuEntry,0,sizeof(MSD_ATU_ENTRY) * atuEntryMaxSize);
    int fid = s_atuModuleInitType[devNum].fidNum;
    MSD_STATUS ret = MSD_OK;
    if (fid != ALL_FID_VALUE) {//查询某个FID的条目
        ret = deviceMacEntryGetListInFid(devNum, fid, atuEntryCount, entryType, atuEntry, atuEntryMaxSize);
    }
    else { //查询所有FID的条目
        int fidSize = MAX_FID_VALUE + 1;
        MSD_U16* fidArray = (MSD_U16*)pvPortMalloc(sizeof(MSD_U16) * fidSize);
        if (fidArray == NULL) return MSD_NO_SPACE;
        msdMemSet(fidArray, 0, sizeof(int) * fidSize);
        int fidCount = 0;
        deviceAtuModuleGetAllFidHasValues(devNum, fidArray, fidSize, &fidCount);
        for (int i = 0; i < fidCount; ++i) { //
            ret = deviceMacEntryGetListInFid(devNum, fidArray[i], atuEntryCount, entryType, atuEntry, atuEntryMaxSize);
            if (ret != MSD_OK){
                break;
            }

        }
        vPortFree(fidArray);
        fidArray = NULL;
    }
    return ret;
}

MSD_STATUS deviceAtuModuleFindEntry(IN MSD_U8 devNum, IN int fid, IN  MSD_ETHERADDR* macAddr, OUT MSD_ATU_ENTRY* atuEntry, OUT MSD_BOOL* isFound)
{
	CHECK_DEV_NUM_IS_CORRECT;
    if (fid < 0 || fid > MAX_FID_VALUE) {
        MSD_DBG_ERROR(("Find mac entry failed,because the fid range is not correct,please check!\n"));
        return MSD_BAD_PARAM;
    }
    if (atuEntry == NULL) {
        MSD_DBG_ERROR(("Find mac entry failed,,because the atu enty param is NULL,please check!\n"));
        return MSD_BAD_PARAM;
    }
    if (macAddr == NULL) {
        MSD_DBG_ERROR(("Find mac entry failed,,because the mac address is NULL,please check!\n"));
        return MSD_BAD_PARAM;
    }
    msdMemSet(atuEntry, 0, sizeof(MSD_ATU_ENTRY));
    MSD_STATUS ret = 0;
    ret = msdFdbMacEntryFind(devNum, macAddr, fid, atuEntry, isFound);
    return ret;
}

#define UNICAST_MAC_ENTRY_STATE 0xE //单播静态地址EntryState
#define MULTICAST_MAC_ENTRY_STATE 0x7  //多播静态地址EntryState


//转发规则：优先确定指定的VID帧是否可以转发给指定端口。如果不可以，则该端口直接。 如果可以，在根据MAC地址决定是否传输给指定的端口
//思路1：将帧捕获到CPU端口，CPU解析帧，得到帧的VID，循环msdVlanMemberTagGet得到帧可以出口的端口。在根据帧的目标地址列表和VID得到帧的出口，然后将帧出口到相应的端口。
//（肯定不对，这样就需要将静态条目中涉及到VID的所有帧监控到CPU端口，肯定不行）
//思路2：使用多FID：FID和VID一一对应。比如添加VID为2时，添加一条VID = 2,FID = 2的VTU条目，当发送VID为2的帧时，得到的FID就为2，然后在根据FID的值去查询指定数据库中的MAC地址的条目。

/**
 * @brief addOneVidStaticEntry 添加一个静态vid条目
 * @param devNum
 * @param address
 * @param vid
 * @param portVec
 * @return
 */
static MSD_STATUS addOneVidStaticEntry(IN MSD_U8 devNum, IN MSD_ETHERADDR address, IN int vid, MSD_U32 portVec)
{
    MSD_BOOL isMul = IS_MULTICAST_MAC(address.arEther);
    MSD_STATUS ret = MSD_OK;
    MSD_ATU_ENTRY macEntry;
    msdMemSet(&macEntry, 0, sizeof(MSD_ATU_ENTRY));
    int fid = vid;//这里检查指定的fid的MAC地址条目是否存在，这样一个FID对应的VID就能和相同的MAC条目对应。
    MSD_BOOL isFoundMac;
    ret = msdFdbMacEntryFind(devNum, &address, fid, &macEntry, &isFoundMac);
    if (ret != MSD_OK)
        return ret;
    //设置传递的参数，如果存在，则是修改，如果不存在，则是添加。
    if (!isMul) {
        macEntry.entryState = (MSD_U8)UNICAST_MAC_ENTRY_STATE;
    }
    else {
        macEntry.entryState = (MSD_U8)MULTICAST_MAC_ENTRY_STATE;
    }
    macEntry.fid = fid; //注意这里不能使用默认值，否则就不支持同一MAC条目，不同VID。
    for (unsigned i = 0; i < MSD_ETHERNET_HEADER_SIZE; ++i) {
        macEntry.macAddr.arEther[i] = address.arEther[i];
    }
    macEntry.portVec = portVec;
    ret = msdFdbMacEntryAdd(devNum, &macEntry);
    if (ret != MSD_OK) return ret;
    setFidValue(devNum, macEntry.fid);
    MSD_BOOL isAdd;
    ret = checkVlanEntry(devNum, (MSD_U16)vid, &isAdd);
    if (ret != MSD_OK) return ret;
    //设置静态条目属性
    addEntryToModuleIndex(devNum, address, (MSD_U16)vid, portVec,UNICAST_MAC_ENTRY_STATE);//将entryState设置为大于7即可
    removeStaticOrAutoMacEntryToModuleIndex(devNum, address, vid, 0x7);//如果添加了相同的静态地址和VID，则将动态条目移除(将entryState设置为0x7)
    return MSD_OK;
}

/**
 * @brief checkStaticMacEntryIsInConfiguration 判断具有指定的MAC地址和VID的MAC条目是否在配置中
 * @param devNum 设备编号
 * @param address 地址
 * @param vid vid值
 * @param index 下标值（根据vid和mac地址找到的下标，如果不存在返回-1）
 * @return
 */
static MSD_BOOL checkStaticMacEntryIsInConfiguration(IN MSD_U8 devNum, IN MSD_ETHERADDR address, IN MSD_U16 vid, OUT int* index)
{
    *index = -1;
    for (int i = 0; i < MAX_STATIC_ATU_ENTRIES; ++i) {
        //if (s_atuConfiguration[devNum].staticMacEntry[i].isVaild) {
          if(IS_BIT_SET(s_atuConfiguration[devNum].staticMacEntry[i].ageAndFlag,2)){
            if (s_atuConfiguration[devNum].staticMacEntry[i].vid == vid && MAC_IS_EQUAL(address, s_atuConfiguration[devNum].staticMacEntry[i].address)) {
                *index = i;
                return MSD_TRUE;
            }
        }
    }
    return MSD_FALSE;
}

static MSD_BOOL addStaticEntryToConfigurationIndex(IN MSD_U8 devNum, IN MSD_ETHERADDR address, IN MSD_U16 vid, IN MSD_U32 portVec)
{
    int index = -1;
    do {
        MSD_BOOL isExist = checkStaticMacEntryIsInConfiguration(devNum, address, vid, &index);
        if (isExist)//找到对应的静态配置条目
            break;
        for (int i = 0; i < MAX_STATIC_ATU_ENTRIES; ++i) {
          //  if (!s_atuConfiguration[devNum].staticMacEntry[i].isVaild) {//无效条目，直接使用
        	if(!IS_BIT_SET(s_atuConfiguration[devNum].staticMacEntry[i].ageAndFlag,2)){
                index = i;// 新的条目下标
                break;
            }
        }
    } while (0);
    if (index != -1) {//交换机中不存在静态条目
        MSD_BOOL isMul = IS_MULTICAST_MAC(address.arEther);
        for (size_t i = 0; i < MSD_ETHERNET_HEADER_SIZE; ++i) {
            s_atuConfiguration[devNum].staticMacEntry[index].address.arEther[i] = address.arEther[i];
        }
        //s_atuConfiguration[devNum].staticMacEntry[index].age = -1;//静态条目不考虑age字段
        SET_BIT(s_atuConfiguration[devNum].staticMacEntry[index].ageAndFlag,0);
	   if(isMul){//多播
		  CLEAR_BIT(s_atuConfiguration[devNum].staticMacEntry[index].ageAndFlag,1);
	   }else{//单播
		  SET_BIT(s_atuConfiguration[devNum].staticMacEntry[index].ageAndFlag,1);
	   }
	   SET_BIT(s_atuConfiguration[devNum].staticMacEntry[index].ageAndFlag,2);
//        s_atuConfiguration[devNum].staticMacEntry[index].isStatic = MSD_TRUE;
//        s_atuConfiguration[devNum].staticMacEntry[index].isUnicast = !isMul;
//        s_atuConfiguration[devNum].staticMacEntry[index].isVaild = MSD_TRUE;
        s_atuConfiguration[devNum].staticMacEntry[index].portVec = portVec;
        s_atuConfiguration[devNum].staticMacEntry[index].vid = vid;
        s_atuConfiguration[devNum].staticMacEntryCount++;
        return MSD_TRUE;
    }
    return MSD_FALSE;//no space
}

static MSD_BOOL removeStaticEntryFromConfigurationIndex(IN MSD_U8 devNum, IN MSD_ETHERADDR address, IN MSD_U16 vid)
{
    int index = 0;
    MSD_BOOL isExist = checkStaticMacEntryIsInConfiguration(devNum, address, vid, &index);
    if (isExist) {
//        s_atuConfiguration[devNum].staticMacEntry[index].isVaild = MSD_FALSE;
        CLEAR_BIT(s_atuConfiguration[devNum].staticMacEntry[index].ageAndFlag,2);
        s_atuConfiguration[devNum].staticMacEntryCount--;
        return MSD_TRUE;
    }
    return MSD_FALSE;
}


MSD_STATUS deviceAtuModuleAddMacStaticEntryToConfiguration(IN MSD_U8 devNum, IN MSD_ETHERADDR address, IN int vid, MSD_U32 portVec)
{
	CHECK_DEV_NUM_IS_CORRECT;
    if (vid > 0xfff || vid < 1) {
        MSD_DBG_ERROR(("device_atu_module_add_vid_static_entry failed,vid param is out of range!\n"));
        return MSD_BAD_PARAM;
    }
    MSD_U8 ports = dev->numOfPorts;//端口数
    MSD_U32 maxPortVector = (1 << ports) - 1;//最大的PortVec(为普通的端口时)
    if (portVec > maxPortVector) {
        MSD_DBG_ERROR(("device_atu_module_add_vid_static_entry failed, port_vec param is out of range!\n"));
        return MSD_BAD_PARAM;
    }
    MSD_BOOL isMul = IS_MULTICAST_MAC(address.arEther);
    if (!isMul && !HAS_ONLY_ONE_BIT_SET(portVec)) {//单播地址，仅仅只允许设置一个端口
        MSD_DBG_ERROR(("device_atu_module_add_vid_static_entry failed, address is unicast mac entry, only one egress port can be set!\n"));
        return MSD_BAD_PARAM;
    }
    if (!addStaticEntryToConfigurationIndex(devNum, address, vid, portVec)) {
        MSD_DBG_ERROR(("device_atu_module_add_vid_static_entry failed,static mac entry is alreay reached to the max config value!\n"));
        return MSD_NO_SPACE;
    }
    return MSD_OK;
}

MSD_STATUS deviceAtuModuleRemoveMacStaticEntryFromConfiguration(IN MSD_U8 devNum, IN MSD_ETHERADDR address, IN int vid,OUT MSD_BOOL *found)
{
	CHECK_DEV_NUM_IS_CORRECT;
    if (vid > 0xfff || vid < 1) {
        MSD_DBG_ERROR(("device_atu_module_add_vid_static_entry failed,vid param is out of range!\n"));
        return MSD_BAD_PARAM;
    }
    *found = MSD_TRUE;
    if(!removeStaticEntryFromConfigurationIndex(devNum, address, vid)){
        *found = MSD_FALSE;
    }
    return MSD_OK;
}


MSD_STATUS deviceAtuModuleClearAllVidStaticEntriesFromConfiguration(IN MSD_U8 devNum)
{
	CHECK_DEV_NUM_IS_CORRECT;
    //清空配置
    s_atuConfiguration[devNum].staticMacEntryCount = 0;
    msdMemSet(&s_atuConfiguration[devNum].staticMacEntry, 0, sizeof(MacEntry) * MAX_STATIC_ATU_ENTRIES);

    return MSD_OK;
}

//static void copyMacEntry(OUT MacEntry* destMacEntry, IN MacEntry* srcMacEntry)
//{
//    destMacEntry->isStatic = srcMacEntry->isStatic;
//    destMacEntry->isUnicast = srcMacEntry->isUnicast;
//    destMacEntry->isVaild = srcMacEntry->isVaild;
//
//    destMacEntry->portVec = srcMacEntry->portVec;
//    destMacEntry->vid = srcMacEntry->vid;
//    for (size_t i = 0; i < MSD_ETHERNET_HEADER_SIZE; ++i) {
//        destMacEntry->address.arEther[i] = srcMacEntry->address.arEther[i];
//    }
//    destMacEntry->age = srcMacEntry->age;
//}

MSD_STATUS deviceAtuModuleGetAllVidStaticEntriesFromConfiguration(IN MSD_U8 devNum, OUT MacEntry* macEntries,
                                                                  IN MSD_U32 macEntrySize, OUT MSD_U32* macEntryCount)
{
	CHECK_DEV_NUM_IS_CORRECT;
    if (macEntries == NULL) {
        MSD_DBG_ERROR(("device_atu_module_get_all_vid_static_entries failed,the mac_entries param is NULL!\n"));
        return MSD_BAD_PARAM;
    }
    if (macEntrySize == 0) {
        MSD_DBG_ERROR(("device_atu_module_get_all_vid_static_entries failed,the mac_entry_size param is 0!\n"));
        return MSD_BAD_PARAM;
    }
    *macEntryCount = 0;
    msdMemSet(macEntries, 0, sizeof(MacEntry) * macEntrySize);
    MSD_U32 index = 0;
    for (int j = 0; j < MAX_STATIC_ATU_ENTRIES; ++j) {
        //if (s_atuConfiguration[devNum].staticMacEntry[j].isVaild) {//如果有效
    	if(IS_BIT_SET(s_atuConfiguration[devNum].staticMacEntry[j].ageAndFlag,2)){//如果有效
            //copyMacEntry(&macEntries[index], &s_atuConfiguration[devNum].staticMacEntry[j]);
        	macEntries[index] = s_atuConfiguration[devNum].staticMacEntry[j];
            if ((++index) == macEntrySize) break;//如果达到存储空间的个数
        }
        if (index == s_atuModuleInitType[devNum].staticEntryCount) break;//如果达到所有的静态条目数
    }
    *macEntryCount = index;
    return MSD_OK;
}

MSD_STATUS deviceAtuModuleRefreshAllVidStaticEntriesToConfiguration(IN MSD_U8 devNum)
{
	CHECK_DEV_NUM_IS_CORRECT;
    MSD_STATUS ret = MSD_OK;
    int fidSize = MAX_FID_VALUE + 1;
    MSD_U16* fidArray = (MSD_U16*)pvPortMalloc(sizeof(MSD_U16) * fidSize);
    if (fidArray == NULL) return MSD_NO_SPACE;
    int fidCount = 0;
    deviceAtuModuleGetAllFidHasValues(devNum, fidArray, fidSize, &fidCount);//获取所有有MAC条目的FID

    //清空之前的静态条目
    for (int j = 0; j < MAX_AUTO_ATU_ENTRIES; ++j) {
        //s_atuModuleInitType[devNum].staticEntry[j].isVaild = MSD_FALSE;
    	CLEAR_BIT(s_atuModuleInitType[devNum].staticEntry[j].ageAndFlag,2);
    }
    s_atuModuleInitType[devNum].staticEntryCount = 0;

    //清空所有的静态配置条目
    s_atuConfiguration[devNum].staticMacEntryCount = 0;
    msdMemSet(&s_atuConfiguration[devNum].staticMacEntry, 0, sizeof(MacEntry) * MAX_STATIC_ATU_ENTRIES);

    MSD_ATU_ENTRY* atuEntry = (MSD_ATU_ENTRY*)pvPortMalloc(sizeof(MSD_ATU_ENTRY) * MAX_AUTO_ATU_ENTRIES);
    if (atuEntry == NULL) {
    	vPortFree(fidArray);
        return MSD_NO_SPACE;
    }
    msdMemSet(atuEntry, 0, sizeof(MSD_ATU_ENTRY) * MAX_AUTO_ATU_ENTRIES);
    do {
        int atuEntryCount = 0;//MAC条目个数
        for (int i = 0; i < fidCount; ++i) { //
            ret = deviceMacEntryGetListInFid(devNum, fidArray[i], &atuEntryCount, ATU_ENTRY_TYPE_STATIC, atuEntry, MAX_AUTO_ATU_ENTRIES);
            if (ret != MSD_OK)
                break;
        }
        s_atuConfiguration[devNum].staticMacEntryCount = atuEntryCount;
        s_atuModuleInitType[devNum].staticEntryCount = atuEntryCount;
        for (int i = 0; i < atuEntryCount; ++i) {

            MSD_BOOL is_uni = IS_UNICAST_MAC(atuEntry[i].macAddr.arEther);

            for (size_t j = 0; j < MSD_ETHERNET_HEADER_SIZE; ++j) {

                s_atuModuleInitType[devNum].staticEntry[i].address.arEther[j] = atuEntry[i].macAddr.arEther[j];

                s_atuConfiguration[devNum].staticMacEntry[i].address.arEther[j] = atuEntry[i].macAddr.arEther[j];
            }

            //s_atuModuleInitType[devNum].staticEntry[i].age = -1;
//            s_atuModuleInitType[devNum].staticEntry[i].isStatic = MSD_TRUE;
//            s_atuModuleInitType[devNum].staticEntry[i].isUnicast = is_uni;
//            s_atuModuleInitType[devNum].staticEntry[i].isVaild = MSD_TRUE;

            SET_BIT(s_atuModuleInitType[devNum].staticEntry[i].ageAndFlag,0);
		    if(!is_uni){//多播
			CLEAR_BIT(s_atuModuleInitType[devNum].staticEntry[i].ageAndFlag,1);
		    }else{//单播
			SET_BIT(s_atuModuleInitType[devNum].staticEntry[i].ageAndFlag,1);
		    }
		    SET_BIT(s_atuModuleInitType[devNum].staticEntry[i].ageAndFlag,2);

            s_atuModuleInitType[devNum].staticEntry[i].portVec = atuEntry[i].portVec;
            s_atuModuleInitType[devNum].staticEntry[i].vid = atuEntry[i].fid;//vid为其fid值
            //设置静态配置条目
            //s_atuConfiguration[devNum].staticMacEntry[i].age = -1;

//            s_atuConfiguration[devNum].staticMacEntry[i].isStatic = MSD_TRUE;
//            s_atuConfiguration[devNum].staticMacEntry[i].isUnicast = is_uni;
//            s_atuConfiguration[devNum].staticMacEntry[i].isVaild = MSD_TRUE;

            SET_BIT(s_atuConfiguration[devNum].staticMacEntry[i].ageAndFlag,0);
			if(!is_uni){//多播
			CLEAR_BIT(s_atuConfiguration[devNum].staticMacEntry[i].ageAndFlag,1);
			}else{//单播
			SET_BIT(s_atuConfiguration[devNum].staticMacEntry[i].ageAndFlag,1);
			}
			SET_BIT(s_atuConfiguration[devNum].staticMacEntry[i].ageAndFlag,2);


            s_atuConfiguration[devNum].staticMacEntry[i].portVec = atuEntry[i].portVec;
            s_atuConfiguration[devNum].staticMacEntry[i].vid = atuEntry[i].fid;

        }
    } while (0);

    vPortFree(fidArray);
    fidArray = NULL;
    vPortFree(atuEntry);
    atuEntry = NULL;
    return ret;
}

MSD_STATUS deviceAtuModuleRemoveMacAutoEntryFromConfiguration(IN MSD_U8 devNum, IN MSD_ETHERADDR address, IN int vid)
{
	CHECK_DEV_NUM_IS_CORRECT;
    if (vid > 0xfff || vid < 1) {
        MSD_DBG_ERROR(("device_atu_module_add_vid_static_entry failed,vid param is out of range!\n"));
        return MSD_BAD_PARAM;
    }
    int fid = s_atuModuleInitType[devNum].fidNum; //保留之前的设置
    s_atuModuleInitType[devNum].fidNum = vid; //设置为删除的fid
    MSD_STATUS ret = deviceAtuModuleDeleteEntry(devNum, address);
    s_atuModuleInitType[devNum].fidNum = fid;
    return ret;
}

/**
 * @brief deviceAtuModuleSetUseMacEntryTypeToConfiguration 设置使用MAC条目的方式到配置文件中
 * @param devNum 设备编号
 * @param useMacType 使用MAC条目的方式（动态，静态，还是结合使用）
 * @return
 * MSD_OK  - on success
 * MSD_FAIL - on error
 * MSD_BAD_PARAM - if invalid parameter is given
 */
static MSD_STATUS deviceAtuModuleSetUseMacEntryTypeToConfiguration(IN MSD_U8 devNum, IN UseMacEntryType useMacType)
{
    MSD_STATUS ret = MSD_OK;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (useMacType == s_atuModuleInitType[devNum].useEntryType) {
        s_atuConfiguration[devNum].useType = useMacType;
        return MSD_OK;
    }
    if (useMacType == USE_MAC_ENTRY_TYPE_STATIC_AND_DYNAMIC) { //动态和静态条目结合
        for (int i = 0; i < dev->numOfPorts; ++i) {
            ret = msdPortLearnEnableSet(devNum, i, MSD_TRUE);
            if (ret != MSD_OK) break;
        }
    }
    else {//USE_ONLY_STATIC_MAC_ENTRY_TYPE 静态
        int fid = s_atuModuleInitType[devNum].fidNum;
        s_atuModuleInitType[devNum].fidNum = ALL_FID_VALUE;
        ret = deviceAtuModuleFlushEntries(devNum, MSD_FLUSH_ALL_NONSTATIC);//删除所有动态条目
        s_atuModuleInitType[devNum].fidNum = fid;
        //不允许自动学习
        for (int i = 0; i < dev->numOfPorts; ++i) {
            ret = msdPortLearnEnableSet(devNum, i, MSD_FALSE);
            if (ret != MSD_OK) break;
        }
    }
    s_atuModuleInitType[devNum].useEntryType = useMacType;
    s_atuConfiguration[devNum].useType = useMacType;
    return ret;
}

MSD_STATUS deviceAtuModuleClearAllVidAutoEntriesFromConfiguration(MSD_U8 devNum)
{
	CHECK_DEV_NUM_IS_CORRECT;
    int fid = s_atuModuleInitType[devNum].fidNum;
    s_atuModuleInitType[devNum].fidNum = ALL_FID_VALUE;//删除所有条目的FID
    UseMacEntryType useType = s_atuModuleInitType[devNum].useEntryType;
    MSD_STATUS ret = MSD_OK;
    do {
        ret = deviceAtuModuleSetUseMacEntryTypeToConfiguration(devNum, USE_MAC_ENTRY_TYPE_ONLY_STATIC);//先禁止自动学习
        if (ret != MSD_OK) break;
        ret = deviceAtuModuleFlushEntries(devNum, MSD_FLUSH_ALL_NONSTATIC);//清除所有动态条目
        if (ret != MSD_OK) break;
        ret = deviceAtuModuleSetUseMacEntryTypeToConfiguration(devNum, useType);//
    } while (0);
    s_atuModuleInitType[devNum].fidNum = fid;//恢复之前操作的FID值。
    return ret;
}

MSD_STATUS deviceAtuModuleGetAllVidAutoEntriesFromConfiguration(MSD_U8 devNum, OUT MacEntry* macEntries, IN MSD_U32 macEntrySize, OUT MSD_U32* macEntryCount)
{
	CHECK_DEV_NUM_IS_CORRECT;
    if (macEntries == NULL) {
        MSD_DBG_ERROR(("device_atu_module_get_all_vid_auto_entries failed,the mac_entries param is NULL!\n"));
        return MSD_BAD_PARAM;
    }
    if (macEntrySize == 0) {
        MSD_DBG_ERROR(("device_atu_module_get_all_vid_auto_entries failed,the mac_entry_size param is 0!\n"));
        return MSD_BAD_PARAM;
    }
    *macEntryCount = 0;
    msdMemSet(macEntries,0,sizeof(MacEntry) * (macEntrySize));
    MSD_U32 temp = 0;
    for (int j = 0; j < MAX_AUTO_ATU_ENTRIES; ++j) {
        //if (s_atuModuleInitType[devNum].autoEntry[j].isVaild) {
    	if(IS_BIT_SET(s_atuModuleInitType[devNum].autoEntry[j].ageAndFlag,2)){
            //copyMacEntry(&macEntries[temp], &s_atuModuleInitType[devNum].autoEntry[j]);
    		macEntries[temp] = s_atuModuleInitType[devNum].autoEntry[j];
            if ((++temp) == macEntrySize) break;
            if (temp == s_atuModuleInitType[devNum].autoEntryCount) break;
        }
    }
    *macEntryCount = temp;
    return MSD_OK;
}


MSD_STATUS deviceAtuModuleRefreshAllVidAutoEntriesToConfiguration(IN MSD_U8 devNum)
{
	CHECK_DEV_NUM_IS_CORRECT;
    int fidSize = MAX_FID_VALUE + 1;
    MSD_U16* fidArray = (MSD_U16*)pvPortMalloc(sizeof(MSD_U16) * fidSize);
    if (fidArray == NULL) return MSD_NO_SPACE;
    msdMemSet(fidArray, 0, sizeof(int) * fidSize);
    int fidCount = 0;
    deviceAtuModuleGetAllFidHasValues(devNum, fidArray, fidSize, &fidCount);//获取所有有MAC条目的FID
    MSD_STATUS ret = MSD_OK;

    int atuEntryCount = 0;//MAC条目个数
    MSD_ATU_ENTRY* atuEntry = (MSD_ATU_ENTRY*)pvPortMalloc(MAX_AUTO_ATU_ENTRIES * sizeof(MSD_ATU_ENTRY));
    if (atuEntry == NULL) {
    	vPortFree(fidArray);
        return MSD_NO_SPACE;
    }
    msdMemSet(atuEntry, 0, sizeof(MSD_ATU_ENTRY) * MAX_AUTO_ATU_ENTRIES);

    for (int i = 0; i < fidCount; ++i) { //
        ret = deviceMacEntryGetListInFid(devNum, fidArray[i], &atuEntryCount, ATU_ENTRY_TYPE_AUTO, atuEntry, MAX_AUTO_ATU_ENTRIES);
        if (ret != MSD_OK)
            break;
    }

    //清空之前的动态条目
    msdMemSet(&s_atuModuleInitType[devNum].autoEntry, 0, sizeof(MacEntry) * MAX_AUTO_ATU_ENTRIES);

    s_atuModuleInitType[devNum].autoEntryCount = atuEntryCount;
    for (int i = 0; i < atuEntryCount; ++i) {

        for (size_t j = 0; j < MSD_ETHERNET_HEADER_SIZE; ++j) {
            s_atuModuleInitType[devNum].autoEntry[i].address.arEther[j] = atuEntry[i].macAddr.arEther[j];
        }
//        s_atuModuleInitType[devNum].autoEntry[i].age = atuEntry[i].entryState;
//        s_atuModuleInitType[devNum].autoEntry[i].isStatic = MSD_FALSE;
//        s_atuModuleInitType[devNum].autoEntry[i].isUnicast = MSD_TRUE;
//        s_atuModuleInitType[devNum].autoEntry[i].isVaild = MSD_TRUE;
        s_atuModuleInitType[devNum].autoEntry[i].ageAndFlag |= ((atuEntry[i].entryState & 0x7) << 3);//设置age
        CLEAR_BIT(s_atuModuleInitType[devNum].autoEntry[i].ageAndFlag,0);
		SET_BIT(s_atuModuleInitType[devNum].autoEntry[i].ageAndFlag,1);
		SET_BIT(s_atuModuleInitType[devNum].autoEntry[i].ageAndFlag,2);

        s_atuModuleInitType[devNum].autoEntry[i].portVec = atuEntry[i].portVec;
        s_atuModuleInitType[devNum].autoEntry[i].vid = atuEntry[i].fid;//vid为其fid值
    }

    vPortFree(fidArray);
    fidArray = NULL;
    vPortFree(atuEntry);
    atuEntry = NULL;
    return ret;
}


MSD_STATUS deviceAtuModuleGetAtuConfiguration(MSD_U8 devNum, OUT AtuConfiguration* configuration)
{
	CHECK_DEV_NUM_IS_CORRECT;
    msdMemSet(configuration, 0, sizeof(configuration));
    configuration->ageOutMinute = s_atuConfiguration[devNum].ageOutMinute;
    configuration->staticMacEntryCount = s_atuConfiguration[devNum].staticMacEntryCount;
    configuration->useType = s_atuConfiguration[devNum].useType;
    int tempCount = 0;
    for (int i = 0; i < MAX_STATIC_ATU_ENTRIES; ++i) {
//        if (s_atuConfiguration[devNum].staticMacEntry[i].isVaild) {
    	 if(IS_BIT_SET(s_atuConfiguration[devNum].staticMacEntry[i].ageAndFlag,2)){
            //configuration->staticMacEntry[i].isVaild = MSD_TRUE;
            SET_BIT(configuration->staticMacEntry[i].ageAndFlag,2);
            configuration->staticMacEntry[i].portVec = s_atuConfiguration[devNum].staticMacEntry[i].portVec;
            configuration->staticMacEntry[i].vid = s_atuConfiguration[devNum].staticMacEntry[i].vid;
            for (size_t j = 0; j < MSD_ETHERNET_HEADER_SIZE; ++j) {
                configuration->staticMacEntry[i].address.arEther[j] = s_atuConfiguration[devNum].staticMacEntry[i].address.arEther[j];
            }
            tempCount++;
        }
        if (tempCount == configuration->staticMacEntryCount)//查询完成
            break;
    }
    return MSD_OK;
}

MSD_STATUS deviceAtuModuleSetAtuConfiguration(MSD_U8 devNum, IN AtuConfiguration* configuration)
{
	CHECK_DEV_NUM_IS_CORRECT;
    if (configuration->ageOutMinute > MAX_AGE_OUT_MINUTE) {
        MSD_DBG_ERROR(("device_atu_module_set_atu_configuration failed,the age out minute is greater than the %d!\n", MAX_AGE_OUT_MINUTE));
        return MSD_BAD_PARAM;
    }
    if (configuration->useType < USE_MAC_ENTRY_TYPE_STATIC_AND_DYNAMIC || configuration->useType > USE_MAC_ENTRY_TYPE_ONLY_STATIC) {
        MSD_DBG_ERROR(("device_atu_module_set_atu_configuration failed,the use mac entry type is out of range!\n"));
        return MSD_BAD_PARAM;
    }
    if(configuration->staticMacEntryCount > MAX_STATIC_ATU_ENTRIES){
        MSD_DBG_ERROR(("device_atu_module_set_atu_configuration failed,the configuration static_mac_entry_count is out of range!\n"));
        return MSD_BAD_PARAM;
    }
    s_atuConfiguration[devNum].ageOutMinute = configuration->ageOutMinute;
    s_atuConfiguration[devNum].useType = configuration->useType;
    s_atuConfiguration[devNum].staticMacEntryCount = configuration->staticMacEntryCount;
    msdMemSet(s_atuConfiguration[devNum].staticMacEntry, 0, sizeof(MacEntry) * MAX_STATIC_ATU_ENTRIES);
    for (int i = 0; i < configuration->staticMacEntryCount; ++i) {
        s_atuConfiguration[devNum].staticMacEntry[i].portVec = configuration->staticMacEntry[i].portVec;
        s_atuConfiguration[devNum].staticMacEntry[i].vid = configuration->staticMacEntry[i].vid;
        //s_atuConfiguration[devNum].staticMacEntry[i].isVaild = MSD_TRUE;
        SET_BIT(s_atuConfiguration[devNum].staticMacEntry[i].ageAndFlag,2);
        for (size_t j = 0; j < MSD_ETHERNET_HEADER_SIZE; ++j) {
            s_atuConfiguration[devNum].staticMacEntry[i].address.arEther[j] = configuration->staticMacEntry[i].address.arEther[j];
        }
    }
    return MSD_OK;
}

MSD_STATUS deviceAtuModuleSaveAtuConfiguration(MSD_U8 devNum)
{
	CHECK_DEV_NUM_IS_CORRECT;
    MSD_STATUS ret = MSD_OK;
    ret = deviceAtuModuleAgingTimeoutSet(devNum, s_atuConfiguration[devNum].ageOutMinute);
    if (ret != MSD_OK) return ret;

    UseMacEntryType useType = s_atuConfiguration[devNum].useType;
    ret = deviceAtuModuleSetUseMacEntryTypeToConfiguration(devNum, useType);
    if (ret != MSD_OK) return ret;

    //先清空所有的静态条目
    int fid = s_atuModuleInitType[devNum].fidNum;
    s_atuModuleInitType[devNum].fidNum = ALL_FID_VALUE;
    ret = deviceAtuModuleFlushEntries(devNum, MSD_FLUSH_ALL_STATIC);
    if (ret != MSD_OK) return ret;
    s_atuModuleInitType[devNum].fidNum = fid;

    int macCount = s_atuConfiguration[devNum].staticMacEntryCount;
    for (int i = 0; i < macCount; ++i) {
        ret = addOneVidStaticEntry(devNum, s_atuConfiguration[devNum].staticMacEntry[i].address, s_atuConfiguration[devNum].staticMacEntry[i].vid,
                                   s_atuConfiguration[devNum].staticMacEntry[i].portVec);
        if (ret != MSD_OK) return ret;
    }

    return ret;
}
