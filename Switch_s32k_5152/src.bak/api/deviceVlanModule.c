#include <apiInit.h>
#include <deviceVlanModule.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <smiasscess.h>
#include <Fir_msdApi.h> //使用上层API好像有点问题


#define MSD_8021Q_MODEL_DEFAULT MSD_8021Q_FALLBACK  //默认使用802.1Q Fallback，后续可以更改为其他默认值
#define DEFAULT_QINQ_TPID_VALUE 0x88a8  //默认的Provider Port的TPID值

#define QINQ_VLAN_VLAN_0_INGRESS_TCAM_POINTER 200 //QINQ VID 0的ingressTCAM

//qinq 默认启用IngressTCAM的开始下标号，使用连续的标号来设置对应VLAN的出口方式 功能需要使用IngressTCAM和EgressTCAM来完成帧带标签或者去掉标签功能
//注意，该值要足够大，因为TCAM查询时从下标号小的开始查询，由于小的编号用于实现过滤功能。需要留一些小的标号，如果需要支持更多的过滤器，则该编号需要设置更大一些
//无标签帧的入口条目在所有标签帧的序号之后，
#define QINQ_VLAN_INGRESS_START_TCAM_POINTER  201

#define QINQ_VLAN_FOR_TAG_FRAME_EGRESS_NOT_TAG_TCAM_POINTER  2 //QinQ模式下，带标签帧出口去除标签的的EgressTCAM的下标号
#define QINQ_VLAN_FOR_TAG_FRAME_EGRESS_NOT_A_MEMBER_TCAM_POINTER 3 //QinQ模式下， 带标签帧不允许出口的EgressTCAM的下标号

#define QINQ_VLAN_FOR_NO_TAG_FRAME_EGRESS_TCAM_POINTER 1 //QinQ模式下，无标签帧的EgressTCAM的出口条目TCAM ,其IngressTCAM的标号为265（201 + 64（VID的大小））

typedef struct {
    MSD_BOOL qModelIsGlobal;//所有端口是否使用同一种标签模式（MSD_TRUE:全部端口使用同一种模式，MSD_FALSE:端口使用不同的模式）
    VlanModel portQModel[MSD_MAX_SWITCH_PORTS];//端口的标签模式
    VlanModel globalQModel;//全局模式下的标签模式，如果使用全局模式（q_model_is_global为MSD_TRUE），则使用该Tag标签，目前仅仅支持该模式
    VlanMemberTagAndEgressModeInfo  portVlanMemberTagInfo;//这些设置是已经保存到内存中的VLAN信息。
    VlanConfigInfo vtuConfigInfo;//vlan 配置信息。全局配置下
    //MSD_BOOL isInitial;//是否初始化VLAN模块的标志
    MSD_BOOL isSetTagMode;//是否已经设置了tag_mode
}VtuModuleInitType;

static VtuModuleInitType s_vtuModuleInitType[MAX_SOHO_DEVICES] = { 0 };

extern DeviceConfig g_allDevicesConfig[MAX_SOHO_DEVICES];

extern MSD_STATUS checkVlanEntry(IN MSD_U8 devNum, IN MSD_U16 vid, OUT MSD_BOOL* isAdd);
extern MSD_STATUS setFidValue(IN MSD_U8 devNum, IN MSD_U16 fid);
extern MSD_STATUS deviceAtuModuleSetMacEntryVidFlagAndSize(IN MSD_U8 devNum, IN MSD_U16 vid_num, IN MSD_BOOL flag);


MSD_STATUS deviceVlanModuleGetVlanEgressMode(IN MSD_U8 devNum, OUT VlanEgressMode* vlanEgressMode) {
    for (size_t i = 0; i < MSD_MAX_SWITCH_PORTS; ++i) {
        vlanEgressMode[i] = s_vtuModuleInitType[devNum].portVlanMemberTagInfo.portVlanInfo[i].egressMode;
    }
    return MSD_OK;
}

MSD_STATUS deviceVlanModuleInitial(void)
{
    for (MSD_U8 i = 0; i < MAX_SOHO_DEVICES; ++i) {

        s_vtuModuleInitType[i].qModelIsGlobal = MSD_TRUE;
        s_vtuModuleInitType[i].globalQModel = VLANMODEL_DISABLE;

        for (size_t j = 0; j < MSD_MAX_SWITCH_PORTS; ++j) {
            s_vtuModuleInitType[i].portQModel[j] = VLANMODEL_DISABLE;
            s_vtuModuleInitType[i].portVlanMemberTagInfo.portVlanInfo[j].egressMode = VLAN_EGRESS_MODE_ALLOW_MEMBERSHIP;//允许指定的VLAN成员从端口出口
        }
        for (int j = 0; j < ALLOW_OPERATION_MAX_VLAN_NUM; ++j) {
            s_vtuModuleInitType[i].portVlanMemberTagInfo.vidInfo[j].vid = UNVALID_VID;
            s_vtuModuleInitType[i].portVlanMemberTagInfo.vidInfo[j].isShow = MSD_FALSE;
        }
        s_vtuModuleInitType[i].portVlanMemberTagInfo.vidInfoSize = 0;
    }
    return MSD_OK;
}

static void deviceVlanModuleInitialVlanConfigInfo(IN MSD_U8 devNum)
{
    msdMemSet(&s_vtuModuleInitType[devNum].vtuConfigInfo, 0, sizeof(VlanConfigInfo));
    s_vtuModuleInitType[devNum].vtuConfigInfo.vlanModel = VLANMODEL_DISABLE;
    s_vtuModuleInitType[devNum].vtuConfigInfo.qinqTpid = DEFAULT_QINQ_TPID_VALUE;
    for (size_t j = 0; j < MSD_MAX_SWITCH_PORTS; ++j) {
        s_vtuModuleInitType[devNum].vtuConfigInfo.portVlanMemberTagInfo.portVlanInfo[j].defaultVid = PORT_DEFAULT_VID;
        s_vtuModuleInitType[devNum].vtuConfigInfo.portVlanMemberTagInfo.portVlanInfo[j].egressMode = VLAN_EGRESS_MODE_ALLOW_MEMBERSHIP;//允许指定的VLAN成员从端口出口
    }
    for (int j = 0; j < ALLOW_OPERATION_MAX_VLAN_NUM; ++j) {
        s_vtuModuleInitType[devNum].vtuConfigInfo.portVlanMemberTagInfo.vidInfo[j].vid = UNVALID_VID;
        s_vtuModuleInitType[devNum].vtuConfigInfo.portVlanMemberTagInfo.vidInfo[j].isShow = MSD_FALSE;
    }
    s_vtuModuleInitType[devNum].vtuConfigInfo.portVlanMemberTagInfo.vidInfoSize = 0;
}

/**
 * @brief deviceVlanModuleResetVlanInfo
 * 重置VTU条目，默认添加VID 1和VID fff
 * @param devNum
 * @return
 */
static MSD_STATUS deviceVlanModuleResetVlanInfo(IN MSD_U8 devNum)
{
    MSD_STATUS status = deviceVlanModuleClearAllVlans(devNum);//清空所有VLAN
    if (status != MSD_OK) return status;
    //添加VID 1  VID fff(Page 0 和 Page 1)
    MSD_VTU_ENTRY vtuEntry;
    msdMemSet(&vtuEntry, 0, sizeof(MSD_VTU_ENTRY));
    vtuEntry.fid = (MSD_U16)1;
    vtuEntry.vid = (MSD_U16)1;
    status = msdVlanEntryAdd(devNum, &vtuEntry);
    if (status != MSD_OK) return status;
    //设置VLAN 1 默认不能删除
    //device_atu_module_set_mac_entry_vid_flag_and_size(dev_num, 1, MSD_TRUE);
    vtuEntry.fid = (MSD_U16)0xfff;
    vtuEntry.vid = (MSD_U16)0xfff;
    status = msdVlanEntryAdd(devNum, &vtuEntry);
    //暂时不考虑Page 1的VID fff的条目
    return status;
}

/**
 * @brief deviceVlanModuleInitialVlanInfo
 *  初始化VTU功能项目的所有模块
 * @param devNum 设备编号
 * @return
 */
MSD_STATUS deviceVlanModuleInitialVlanInfo(IN MSD_U8 devNum)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    deviceVlanModuleSetQMode(devNum, ALL_PORT_PARAM, VLANMODEL_DISABLE);//设置禁用Q标签。
    MSD_STATUS status = MSD_OK;
    for (int i = 0; i < dev->numOfPorts; ++i) {
        deviceVlanModuleSetDefaultVlanId(devNum, i, PORT_DISABLE_VLAN_VID);//不启用VLAN时，端口的默认VID为1.
        msdPortEtherTypeByTypeSet(devNum, i, MSD_ETHERTYPE_PROVIDER, DEFAULT_QINQ_TPID_VALUE);
    }
    status = deviceVlanModuleResetVlanInfo(devNum);
    if (status != MSD_OK) {
        return status;
    }
    //初始化vlan配置信息
    deviceVlanModuleInitialVlanConfigInfo(devNum);
    return MSD_OK;
}

/**
 * @brief checkVlanEntryIsValid 判断VTU条目是否符合要求
 * @param vtuEntry
 * @return
 */
static MSD_BOOL checkVlanEntryIsValid(MSD_VTU_ENTRY* vtuEntry)
{
    if (vtuEntry == NULL) return MSD_FALSE;
    if((vtuEntry->fid < PORT_DEFAULT_VID) || (vtuEntry->fid > MAX_OPERATION_VID)){
        MSD_DBG_ERROR(("check_vlan_entry_is_valid failed, the vtu entry fid range is %d ~ %d!\n", PORT_DEFAULT_VID, MAX_OPERATION_VID));
        return MSD_FALSE;
    }
    if((vtuEntry->vid < PORT_DEFAULT_VID) || (vtuEntry->vid > MAX_OPERATION_VID)){
        MSD_DBG_ERROR(("check_vlan_entry_is_valid failed, the vtu entry vid range is %d ~ %d!\n", PORT_DEFAULT_VID, MAX_OPERATION_VID));
        return MSD_FALSE;
    }
    for (size_t i = 0; i < MSD_MAX_SWITCH_PORTS; ++i) {
    	if(vtuEntry->memberTagP[i] < 0x0  || vtuEntry->memberTagP[i] > 0x3){
            return MSD_FALSE;
        }
    }
    if (vtuEntry->vidPolicy != MSD_FALSE && vtuEntry->vidPolicy != MSD_TRUE)
        return MSD_FALSE;
    if (vtuEntry->vidExInfo.dontLearn != MSD_FALSE && vtuEntry->vidExInfo.dontLearn != MSD_TRUE)
        return MSD_FALSE;
    if (vtuEntry->vidExInfo.filterBC != MSD_FALSE && vtuEntry->vidExInfo.filterBC != MSD_TRUE)
        return MSD_FALSE;
    if (vtuEntry->vidExInfo.filterMC != MSD_FALSE && vtuEntry->vidExInfo.filterMC != MSD_TRUE)
        return MSD_FALSE;
    if (vtuEntry->vidExInfo.filterUC != MSD_FALSE && vtuEntry->vidExInfo.filterUC != MSD_TRUE)
        return MSD_FALSE;
    if (vtuEntry->vidExInfo.igmpSnoop != MSD_FALSE && vtuEntry->vidExInfo.igmpSnoop != MSD_TRUE)
        return MSD_FALSE;
    if (vtuEntry->vidExInfo.mldSnoop != MSD_FALSE && vtuEntry->vidExInfo.mldSnoop != MSD_TRUE)
        return MSD_FALSE;
    if (vtuEntry->vidExInfo.routeDis != MSD_FALSE && vtuEntry->vidExInfo.routeDis != MSD_TRUE)
        return MSD_FALSE;
    if (vtuEntry->vidExInfo.useVIDFPri != MSD_FALSE && vtuEntry->vidExInfo.useVIDFPri != MSD_TRUE)
        return MSD_FALSE;
    if (vtuEntry->vidExInfo.useVIDQPri != MSD_FALSE && vtuEntry->vidExInfo.useVIDQPri != MSD_TRUE)
        return MSD_FALSE;
    if(vtuEntry->vidExInfo.vidFPri > 0x7)
        return MSD_FALSE;
    if (vtuEntry->vidExInfo.vidQPri > 0x7)
        return MSD_FALSE;
    return MSD_TRUE;
}

static void copyQVtuEntryToFirVtuEntry(IN FIR_MSD_VTU_ENTRY* firVtuEntry, IN FirVlanEntry* qVtuEntry)
{
    firVtuEntry->DBNum = qVtuEntry->entry.fid;
    firVtuEntry->sid = qVtuEntry->entry.sid;
    firVtuEntry->vid = qVtuEntry->entry.vid;
    firVtuEntry->vidPolicy = qVtuEntry->entry.vidPolicy;
    for (size_t i = 0; i < MSD_MAX_SWITCH_PORTS; ++i) {
        firVtuEntry->memberTagP[i] = (FIR_MSD_MEMTAGP)qVtuEntry->entry.memberTagP[i];
    }
    firVtuEntry->vidExInfo.dontLearn = qVtuEntry->entry.vidExInfo.dontLearn;
    firVtuEntry->vidExInfo.filterBC = qVtuEntry->entry.vidExInfo.filterBC;
    firVtuEntry->vidExInfo.filterMC = qVtuEntry->entry.vidExInfo.filterMC;
    firVtuEntry->vidExInfo.filterUC = qVtuEntry->entry.vidExInfo.filterUC;
    firVtuEntry->vidExInfo.igmpSnoop = qVtuEntry->entry.vidExInfo.igmpSnoop;
    firVtuEntry->vidExInfo.mldSnoop = qVtuEntry->entry.vidExInfo.mldSnoop;
    firVtuEntry->vidExInfo.routeDis = qVtuEntry->entry.vidExInfo.routeDis;
    firVtuEntry->vidExInfo.useVIDFPri = qVtuEntry->entry.vidExInfo.useVIDFPri;
    firVtuEntry->vidExInfo.useVIDQPri = qVtuEntry->entry.vidExInfo.useVIDQPri;
    firVtuEntry->vidExInfo.vidFPri = qVtuEntry->entry.vidExInfo.vidFPri;
    firVtuEntry->vidExInfo.vidQPri = qVtuEntry->entry.vidExInfo.vidQPri;
    firVtuEntry->vidExInfo.vtuPage = 0;
}

static void copyFirVtuEntryToQVtuEntry(IN FirVlanEntry* qVtuEntry,IN FIR_MSD_VTU_ENTRY* firVtuEntry)
{
    qVtuEntry->entry.fid = firVtuEntry->DBNum;
    qVtuEntry->entry.sid = firVtuEntry->sid;
    qVtuEntry->entry.vid = firVtuEntry->vid;
    qVtuEntry->entry.vidPolicy = firVtuEntry->vidPolicy;
    for (size_t i = 0; i < MSD_MAX_SWITCH_PORTS; ++i) {
        qVtuEntry->entry.memberTagP[i] = ( MSD_PORT_MEMBER_TAG)firVtuEntry->memberTagP[i];
    }
    qVtuEntry->entry.vidExInfo.dontLearn = firVtuEntry->vidExInfo.dontLearn;
    qVtuEntry->entry.vidExInfo.filterBC = firVtuEntry->vidExInfo.filterBC;
    qVtuEntry->entry.vidExInfo.filterMC = firVtuEntry->vidExInfo.filterMC;
    qVtuEntry->entry.vidExInfo.filterUC = firVtuEntry->vidExInfo.filterUC;
    qVtuEntry->entry.vidExInfo.igmpSnoop = firVtuEntry->vidExInfo.igmpSnoop;
    qVtuEntry->entry.vidExInfo.mldSnoop = firVtuEntry->vidExInfo.mldSnoop;
    qVtuEntry->entry.vidExInfo.routeDis = firVtuEntry->vidExInfo.routeDis;
    qVtuEntry->entry.vidExInfo.useVIDFPri = firVtuEntry->vidExInfo.useVIDFPri;
    qVtuEntry->entry.vidExInfo.useVIDQPri = firVtuEntry->vidExInfo.useVIDQPri;
    qVtuEntry->entry.vidExInfo.vidFPri = firVtuEntry->vidExInfo.vidFPri;
    qVtuEntry->entry.vidExInfo.vidQPri = firVtuEntry->vidExInfo.vidQPri;
//    qVtuEntry->page = firVtuEntry->vidExInfo.vtuPage;
}

/**
 * @brief deviceVlanModuleAddOrModifyOneVidMemberTagInfo
 * 添加或者修改一个VTU条目的vid和memberTag到全局参数的相应下标中。
 * @param devNum 设备编号
 * @param vtuEntry 要添加的VTU条目
 * @return 如果设置成功返回MSD_TRUE,如果没有下标可以存储，则返回MSD_FALSE
 */
static void deviceVlanModuleAddOrModifyOneVidMemberTagInfo(IN MSD_U8 devNum, IN MSD_VTU_ENTRY* vtuEntry)
{
    int index = -1; //存储下标
    for (int i = 0; i < ALLOW_OPERATION_MAX_VLAN_NUM; ++i) {
        int tempVid = s_vtuModuleInitType[devNum].portVlanMemberTagInfo.vidInfo[i].vid;
        if (tempVid == vtuEntry->vid) {//存在，则修改
            index = i;
            break;
        }
    }
    if (index == -1) { //条目中不存在，则找到一个位置
        for (int i = 0; i < ALLOW_OPERATION_MAX_VLAN_NUM; ++i) {
            int tempVid = s_vtuModuleInitType[devNum].portVlanMemberTagInfo.vidInfo[i].vid;
            if (tempVid == UNVALID_VID) {
                s_vtuModuleInitType[devNum].portVlanMemberTagInfo.vidInfoSize++;
                index = i;
                break;
            }
        }
    }
    if(index != -1){//找到存储VID的下标
        for (size_t j = 0; j < MSD_MAX_SWITCH_PORTS; ++j) {
            s_vtuModuleInitType[devNum].portVlanMemberTagInfo.vidInfo[index].memberTag[j] = vtuEntry->memberTagP[j];
        }
        s_vtuModuleInitType[devNum].portVlanMemberTagInfo.vidInfo[index].vid = vtuEntry->vid;
        s_vtuModuleInitType[devNum].portVlanMemberTagInfo.vidInfo[index].isShow = MSD_TRUE;
    }
}

MSD_STATUS deviceVlanModuleAddOrModifyVlan(IN MSD_U8 devNum, IN FirVlanEntry* vtuEntry)
{
	CHECK_DEV_NUM_IS_CORRECT;
    if(vtuEntry == NULL){
        MSD_DBG_ERROR(("device_vlan_module_add_or_modify_vlan failed, because the vtu entry param is null!\n"));
        return MSD_BAD_PARAM;
    }
    MSD_VTU_ENTRY *entry = &vtuEntry->entry;
    entry->sid = 0;//force sid 0
    if (!checkVlanEntryIsValid(entry)) {
        MSD_DBG_ERROR(("device_vlan_module_add_or_modify_vlan failed, because vtu_entry is NULL or vtu_entry param is error\n"));
        return MSD_BAD_PARAM;
    }
    MSD_BOOL isExists;
    MSD_STATUS ret = msdVlanExists(devNum, entry->vid, &isExists);
    if(ret != MSD_OK){
        return ret;
    }
    if (!isExists && s_vtuModuleInitType[devNum].portVlanMemberTagInfo.vidInfoSize == ALLOW_OPERATION_MAX_VLAN_NUM) {
        MSD_DBG_ERROR(("device_vlan_module_add_or_modify_vlan failed, because operation vtu_entry numer is reach the max:%d\n", ALLOW_OPERATION_MAX_VLAN_NUM));
        return MSD_FEATRUE_NOT_ALLOW;
    }
    ret = msdVlanEntryAdd(devNum, entry);
    if (ret == MSD_OK) {
        setFidValue(devNum, entry->fid);//添加了指定VTU条目，其FID对应MAC条目很可能就存在对应的MAC条目。
        deviceVlanModuleAddOrModifyOneVidMemberTagInfo(devNum, entry);
    }
    return ret;
}

/**
 * @brief deviceVlanModuleRemoveOneVidMemberTagInfo
 *  从数据库中的member_tag_info中删除指定的vid的member_tag信息。找到并删除返回MSD_TRUE,否则返回MSD_FALSE.
 * @param dev_num 设备编号
 * @param vlan_id
 * @return
 */
static void deviceVlanModuleRemoveOneVidMemberTagInfo(IN int devNum, IN int vlanId)
{
    for (int i = 0; i < ALLOW_OPERATION_MAX_VLAN_NUM; ++i) {
        int tempVid = s_vtuModuleInitType[devNum].portVlanMemberTagInfo.vidInfo[i].vid;
        if (tempVid == vlanId) {//找到条目
            s_vtuModuleInitType[devNum].portVlanMemberTagInfo.vidInfo[i].vid = UNVALID_VID;//设置为无效项目
            s_vtuModuleInitType[devNum].portVlanMemberTagInfo.vidInfoSize--;
        }
    }
}

MSD_STATUS deviceVlanModuleDelVlan(IN MSD_U8 devNum, IN MSD_U16 vlanId)
{
	CHECK_DEV_NUM_IS_CORRECT;
    if(vlanId > MAX_VID_VALUE - 1){
        MSD_DBG_ERROR(("device_vlan_module_del_vlan failed, because vlanid param range is(%d,%d)\n", 0, MAX_VID_VALUE - 1));
        return MSD_BAD_PARAM;
    }
    MSD_STATUS ret = MSD_OK;
    MSD_BOOL isFound;
    MSD_VTU_ENTRY entry;
    ret = msdVlanEntryFind(devNum, vlanId, &entry, &isFound);
    if (ret != MSD_OK) return ret;
    if(!isFound) return MSD_OK;
    ret = msdVlanEntryDelete(devNum, vlanId); //只能删除Page 0的VLAN
    if (ret == MSD_OK) {
        deviceVlanModuleRemoveOneVidMemberTagInfo(devNum, vlanId);
    }
    return ret;
}

/**
 * @brief deviceVlanModuleResetAllVidMemberTagInfo
 * 清空所有端口所有VID的memberTag
 * @param devNum
 */
static void deviceVlanModuleResetAllVidMemberTagInfo(IN MSD_U8 devNum)
{
    for (int i = 0; i < ALLOW_OPERATION_MAX_VLAN_NUM; ++i) {
        s_vtuModuleInitType[devNum].portVlanMemberTagInfo.vidInfo[i].vid = UNVALID_VID;
        s_vtuModuleInitType[devNum].portVlanMemberTagInfo.vidInfo[i].isShow = MSD_FALSE;
        for (size_t j = 0; j < MSD_MAX_SWITCH_PORTS; ++j) {
            s_vtuModuleInitType[devNum].portVlanMemberTagInfo.vidInfo[i].memberTag[j] = MSD_MEMBER_EGRESS_UNMODIFIED;//默认帧不修改方式出口端口
        }
    }
    for (size_t i = 0; i < MSD_MAX_SWITCH_PORTS; ++i) {
        s_vtuModuleInitType[devNum].portVlanMemberTagInfo.portVlanInfo[i].egressMode = VLAN_EGRESS_MODE_ALLOW_MEMBERSHIP;//默认允许所有VID的帧出口该端口
        s_vtuModuleInitType[devNum].portVlanMemberTagInfo.portVlanInfo[i].defaultVid = PORT_DEFAULT_VID;
    }
    s_vtuModuleInitType[devNum].portVlanMemberTagInfo.vidInfoSize = 0;
}

MSD_STATUS deviceVlanModuleClearAllVlans(IN MSD_U8 devNum)
{
	CHECK_DEV_NUM_IS_CORRECT;
    MSD_STATUS ret = msdVlanAllDelete(devNum);
    if (ret == MSD_OK) {
        deviceVlanModuleResetAllVidMemberTagInfo(devNum);
    }
    return ret;
}
MSD_STATUS deviceVlanModuleGetAllVlans(IN MSD_U8 devNum, OUT FirVlanEntry* vtuEntries, IN MSD_U16 vtuEntrySize, OUT MSD_U16 *vtuEntryCount)
{
	CHECK_DEV_NUM_IS_CORRECT;
    if (vtuEntries == NULL) {
        return MSD_BAD_PARAM;
    }
    *vtuEntryCount = 0;
    msdMemSet(vtuEntries,0,sizeof(FirVlanEntry) * vtuEntrySize);
    MSD_STATUS status = MSD_OK;
    FIR_MSD_VTU_ENTRY entry;
    //vid在VTU表项中是先page 0,最小到page 0最大0xfff,然后page 1最小到最大0xfff的VID值排序。
    //startPage = 1 ，endPage = 1，查询page 0和page 1的所有VID
    //startPage = 0,endPage = 1，查询page 1的vid
    //startPage = 1, endPage = 0,查询page 0的vid
    MSD_U8 startPage = 1, endPage = 0;
    /*if (page == 1) {
        start_page = 0;
        end_page = 1;
    }
    else if (page == 0) {
        start_page = 1;
        end_page = 0;
    }*/
    msdMemSet(&entry, 0, sizeof(FIR_MSD_VTU_ENTRY));
    entry.vidExInfo.vtuPage = startPage; //从Page 1开始寻找
    entry.vid = (MSD_U16)0xfff;
    while (1) {
        status = Fir_gvtuGetEntryNext(dev, &entry);
        if(status == MSD_OK){
            if(entry.vid == 0 || entry.vid == 1){//vid 0 和vid 1 不显示出来
                continue;
            }
            copyFirVtuEntryToQVtuEntry(&vtuEntries[(*vtuEntryCount)++], &entry);
            if (*vtuEntryCount == vtuEntrySize) {//no more space to store entry
                break;
            }
            if ((entry.vid == (MSD_U16)0xfff) && (entry.vidExInfo.vtuPage == endPage)) {//结束
                break;
            }
        }else if(status == MSD_NO_SUCH){
            status = MSD_OK;
            break;
        }else{//error
            break;
        }
    }
    return status;
}

MSD_STATUS deviceVlanModuleFindVlan(IN MSD_U8 devNum, INOUT FirVlanEntry* vtuEntry, OUT MSD_BOOL* isFound)
{
	CHECK_DEV_NUM_IS_CORRECT;
    if(vtuEntry == NULL){
        MSD_DBG_ERROR(("device_vlan_module_find_vlan failed, because vtu_entry is NULL\n"));
        return MSD_BAD_PARAM;
    }

    FIR_MSD_VTU_ENTRY firEntry;
    msdMemSet(&firEntry, 0, sizeof(FIR_MSD_VTU_ENTRY));
    copyQVtuEntryToFirVtuEntry(&firEntry, vtuEntry);
    MSD_STATUS ret = Fir_gvtuFindVidEntry(dev, &firEntry, isFound);
    if (isFound) {
        copyFirVtuEntryToQVtuEntry(vtuEntry, &firEntry);
    }
    if(ret == MSD_NO_SUCH) ret = MSD_OK;
    return ret;
}



MSD_STATUS deviceVlanModuleSetQModeIsGlobal(IN MSD_U8 devNum, IN MSD_BOOL isGlobal, IN VlanModel model)
{
	CHECK_DEV_NUM_IS_CORRECT;
    if(model < VLANMODEL_DISABLE || model > VLANMODEL_QINQ)
    {
        return MSD_BAD_PARAM;
    }
    MSD_STATUS ret = MSD_OK;
    if (isGlobal) {//
        if(!s_vtuModuleInitType[devNum].qModelIsGlobal){
            s_vtuModuleInitType[devNum].qModelIsGlobal = MSD_TRUE;
        }
        ret = deviceVlanModuleSetQMode(devNum, ALL_PORT_PARAM, model);
        if(ret != MSD_OK) //设置失败
            s_vtuModuleInitType[devNum].qModelIsGlobal = MSD_FALSE;
    }

    if (ret == MSD_OK) {
        s_vtuModuleInitType[devNum].qModelIsGlobal= isGlobal;
    }
    return ret;
}

MSD_STATUS deviceVlanModuleGetQModeIsGlobal(IN MSD_U8 devNum, OUT MSD_BOOL* isGlobal)
{
	CHECK_DEV_NUM_IS_CORRECT;
    *isGlobal = s_vtuModuleInitType[devNum].qModelIsGlobal;
    return MSD_OK;
}

/**
 * @brief deviceVlanModuleHasTheModePort
 * 查询交换机是否具有指定的VLAN模式。如果不需要排除端口，则指定port为小于0的值(非交换机端口)
 * @param devNum 设备编号
 * @param portNum 要排除的端口号
 * @param qMode 要查询的Q 模式值
 * @return
 * MSD_TRUE:存在指定模式的VLAN模式的端口
 * MSD_FALSE:不存在指定模式的VLAN模式的端口
 */
static MSD_BOOL deviceVlanModuleHasTheModePort(IN MSD_U8 devNum,IN int portNum, VlanModel qMode)
{
    if(s_vtuModuleInitType[devNum].qModelIsGlobal && s_vtuModuleInitType[devNum].globalQModel == qMode) {//全局模式下，不用查询端口
        return MSD_TRUE;
    }
    MSD_BOOL ret = MSD_FALSE;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    for (int i = 0; i < dev->numOfPorts; ++i) {//
        if (portNum == i) {//
            continue;
        }
        if (s_vtuModuleInitType[devNum].portQModel[i] == qMode) {
            ret = MSD_TRUE;
            break;
        }
    }
    return ret;
}

static MSD_STATUS deviceVlanModuleSetQModelInPort(IN MSD_U8 devNum, IN int portNum, IN VlanModel model)
{

    MSD_STATUS ret = MSD_OK;
    MSD_BOOL isGlobal = s_vtuModuleInitType[devNum].qModelIsGlobal;
    do {
        MSD_8021Q_MODE mode8021;
        if (s_vtuModuleInitType[devNum].portVlanMemberTagInfo.portVlanInfo->egressMode == VLAN_EGRESS_MODE_ALLOW_MEMBERSHIP) {
            mode8021 = MSD_8021Q_CHECK;//VID必须包含在VTU中，否则丢弃帧，出口端口必须为其成员，才能从端口出口
        }
        else {//Allow All or All All but exclusion the list
            mode8021 = MSD_8021Q_FALLBACK;//VID不是必须在VTU列表中，如果VID包含在VTU列表中，出口必须为其成员
        }
        if (model == VLANMODEL_DISABLE || model == VLANMODEL_VLAN) { //设置端口为VLAN MODEL或者DISABLE MODE

            MSD_FRAME_MODE portFrameMode;
            ret = msdPortFrameModeGet(devNum, portNum, &portFrameMode);
            if (ret != MSD_OK){
                break;
            }
            if (portFrameMode == MSD_FRAME_MODE_PROVIDER) {//如果为QinQ模式，则设置为普通模式
                ret = msdPortFrameModeSet(devNum, portNum, MSD_FRAME_MODE_NORMAL);
                if (ret != MSD_OK){
                    break;
                }
            }
            if (model == VLANMODEL_DISABLE) {//禁用802.1 Q 模式
                ret = msdPortExtendedPortCtrlRegSet(devNum, portNum, 0x10, 0, 15, 0x20);//禁用VLAN时 不设置adDis bit和Qdis bit
                if (ret != MSD_OK){
                    break;
                }
                ret = msdPort8021qModeSet(devNum, portNum, MSD_8021Q_DISABLE);
                if (ret != MSD_OK){
                    break;
                }
                ret = msdPortDefaultVlanIdSet(devNum, portNum, PORT_DISABLE_VLAN_VID);//不启用VLAN的VID
                if (ret != MSD_OK){
                    break;
                }
            }
            else { //设置为启用802.1 Q模式
                //如果启用802.1 VLAN(普通端口),则需要将adDis bit设置为1(寄存器规范Tabble 73).这可以防止类似QinQ(EtherType = 0x9100)的帧被解析为带标签帧，即0x9100的帧会被认为是带标签帧。
                ret = msdPortExtendedPortCtrlRegSet(devNum, portNum, 0x10, 0, 15, 0x24);
                if (ret != MSD_OK) break;
            }
            //判断是否存在Provider Ports,如果存在，则参考功能文档2.3.7看Customer Port具有哪些限制
            if (!isGlobal && deviceVlanModuleHasTheModePort(devNum, portNum, VLANMODEL_QINQ)) {//如果不是全局模式，并且其他端口存在Provider Port。
                //1.Don’t enable any of the 802.1Q VLAN modes (Section 2.2.2.2). Use Port Based VLANs only.
                ret = msdPort8021qModeSet(devNum, portNum, MSD_8021Q_DISABLE);
                if (ret != MSD_OK) break;
                //2. Don’t use 802.1s Per VLAN Spanning Tree (Section 2.2.2.3).
                //3. Don’t use Layer 2 Priority Override using the frame’s Customer VID (Section 2.2.4.5).
                //4. Don’t use a Transmit Tagged or Transmit Untagging Egress Mode. Always use Transmit  Unmodified3 (Section 2.2.9.3).
                ret = msdPortEgressModeSet(devNum, portNum, MSD_EGRESS_MODE_UNMODIFIED);
                if (ret != MSD_OK) break;
            }
        }
        else { //QINQ
            ret = msdPortFrameModeSet(devNum, portNum, MSD_FRAME_MODE_PROVIDER);//
            if (ret != MSD_OK) break;
            //注：当端口可以使用不同模式时，可以是QinQ 和 Disable Mode组合， Disable Mode 和 VLAN Mode组合， 或者任意一种模式，其他混合模式都不允许
            //if (!is_global && device_vlan_module_has_the_mode_port(dev_num,port_num, Q_DISABLE_MODEL)) { //QinQ + Disable Model
            //	//交换机存在Provider Port and Disable VLAN Port，在功能文档里面有描述这种情况的限制
            //	//Provider Port 参考功能文档2.3.6看Provider Port具有哪些限制，功能文档介绍的Provider Port的限制一般是在存在Customer Port情况下的限制
            //	//1.Don’t use Layer 2 Policy Controls using the frame’s Ether type
            //	//2.Don’t use Frame Type Priority Override using the frame’s Ether type
            //	//3.Don’t use a Transmit Tagged or Transmit Untagging Egress Mode.Always use Transmit  Unmodified1(Section 2.2.9.3).
            //	ret = msdPortEgressModeSet(dev_num, port_num, MSD_EGRESS_MODE_UNMODIFIED);
            //	if (ret != MSD_OK) break;
            //}
            //else {//全局模式下(所有端口都是Provider Port) 或者非全局模式下所有端口都是Provider Port（包括自身在设置前为Disable Mode）
            //	//如果入口设置为MSD_EGRESS_MODE_TAGGED, TCAM 过滤不了双标签。
            //	ret = msdPortEgressModeSet(dev_num, port_num, MSD_EGRESS_MODE_TAGGED);//需要设置为Tagged，才能对VLAN的memberTag修改为Tagged或者UnTagged。
            //	if (ret != MSD_OK) break;
            //}
            //
            ret = msdPortEgressModeSet(devNum, portNum, MSD_EGRESS_MODE_UNMODIFIED);
            if (ret != MSD_OK) break;
            ret = msdSysRemove1PtagSet(devNum, MSD_TRUE);//可以防止多个QinQ TPID的后续TPID Tag被移除
            if (ret != MSD_OK) break;
            //寄存器Table 73,禁用Qdis（ Ingress Q Tag (Customer Tag) decode Disable），防止C-Tag对外层S-Tag memberTag设置的影响。
            ret = msdPortExtendedPortCtrlRegSet(devNum, portNum, 0x10, 0, 15, 0x26);
            if (ret != MSD_OK) break;

            //默认Provider Port丢弃不满足符合Provider Tag标签的帧，如果想转发到具体的端口，请调用device_vlan_module_set_provider_port_deal_with_no_provider_tag_frame接口
            /*ret = msdPortDiscardUntaggedSet(dev_num, port_num, MSD_TRUE);
            if (ret != MSD_OK) break;*/
            //4.Don’t used a Provider Port as a destination port for Mirrors2 (Section 2.2.1.3 for Layer 2 Mirrors  and Section 2.5.9 for Ingress and/or Egress Mirrors)
        }

        if (model != VLANMODEL_DISABLE) {//VLAN AND QINQ
            //VLAN模式开启802.1 Q ,建议使用Provider Port也使用802.1Q模式
            MSD_8021Q_MODE mode;
            ret = msdPort8021qModeGet(devNum, portNum, &mode);
            if (ret != MSD_OK) break;
            //如果没有启用802.1Q模式
            if (mode == MSD_8021Q_DISABLE) {
                //启用802.1Q功能
                ret = msdPort8021qModeSet(devNum, portNum, mode8021);
                if (ret != MSD_OK) break;
            }
            MSD_U16 vid;
            //如果启用了VLAN或者QinQ功能，还应将DefaultVID设置为不小于2的值
            ret = msdPortDefaultVlanIdGet(devNum, portNum, &vid);
            if (ret != MSD_OK) break;
            if (vid < PORT_DEFAULT_VID) {//如果小于2，则需要设置默认的VID。
                ret = msdPortDefaultVlanIdSet(devNum, portNum, PORT_DEFAULT_VID);
                if (ret != MSD_OK) break;
            }
            ret = msdPortAllowVidZeroSet(devNum, portNum, MSD_TRUE);//VLAN不变的情况下，设置允许VID 0的帧。
            if (ret != MSD_OK) break;
        }
    } while (0);
    s_vtuModuleInitType[devNum].portQModel[portNum] = model;
    return ret;
}

/**
 * @brief isCanSetPortQMode
 * 判断端口模式下，指定的端口是否可以设置为指定的Q MODE
 * 在设置之前，端口模式必须满足相应的标签组合或者单标签模式
 * 注：端口可以使用不同模式(QinQ 和 Disable Mode组合， Disable Mode 和 VLAN Mode组合， 或者任意一种模式，其他混合模式都不允许)
 * @param devNum 设备编号
 * @param portNum 端口编号
 * @param model 设置的Q Mode
 * @return
 *  MSD_TRUE: 可以将port_num设置为model设置的Tag模式
 *  MSD_FALSE: 不可以将port_num设置为model指定的Tag模式，如果当前设置的端口模式已经为model指定的参数，也会返回MSD_FALSE
 */
static MSD_BOOL isCanSetPortQMode(IN MSD_U8 devNum, IN int portNum, IN VlanModel model)
{
    int qinqModelPort[MSD_MAX_SWITCH_PORTS], vlanModelPort[MSD_MAX_SWITCH_PORTS], disableVlanModelPort[MSD_MAX_SWITCH_PORTS];
    int qinqPortCount, vlanPortCount, disableVlanPortCount;
    int index = -1;//是否在下标的标志，-1不在其数组中
    MSD_STATUS ret = deviceVlanModuleGetModePort(devNum, qinqModelPort, MSD_MAX_SWITCH_PORTS, &qinqPortCount, VLANMODEL_QINQ);
    if (ret != MSD_OK) {
        return MSD_FALSE;
    }
    ret = deviceVlanModuleGetModePort(devNum, vlanModelPort, MSD_MAX_SWITCH_PORTS, &vlanPortCount, VLANMODEL_VLAN);
    if (ret != MSD_OK) {
        return MSD_FALSE;
    }
    ret = deviceVlanModuleGetModePort(devNum, disableVlanModelPort, MSD_MAX_SWITCH_PORTS, &disableVlanPortCount, VLANMODEL_DISABLE);
    if (ret != MSD_OK) {
        return MSD_FALSE;
    }
    if (qinqPortCount != 0 && disableVlanPortCount != 0) {//设置之前存在QinQ,Disable组合端口的情况。
        if (qinqPortCount > 1 && disableVlanPortCount > 1) {
            if (model == VLANMODEL_VLAN) {//这种情况，会导致3种端口模式会同时存在，因此不允许设置
                return MSD_FALSE;
            }
            else { //允许设置
                return 	MSD_TRUE;
            }
        }
        else if (qinqPortCount == 1) {//QinQ端口个数为1，其余端口（大于1个端口）都禁用VLAN。
            //这种情况可以设置QinQ端口的模式为禁用VLAN或者启用VLAN(因为可以将端口模式转换为vlan端口与禁用端口端口组合)
            index = numberIsInArr(qinqModelPort, qinqPortCount, portNum);
            if (index != -1) {//当前设置的端口号是QinQ端口
                if (model == VLANMODEL_QINQ)//不允许设置为QinQ
                    return MSD_FALSE;
                return MSD_TRUE;//设置为VLAN和DISABLE VLAN都可以
            }
            else {//当前设置的端口不是QinQ端口,为DISABLE VLAN端口,允许设置为QINQ端口模式
                if (model == VLANMODEL_VLAN || model == VLANMODEL_DISABLE)//DISABLE 端口不允许设置为VLAN 模式 和 DISABLE 端口模式
                    return MSD_FALSE;
                return MSD_TRUE;//
            }
        }
        else {//禁用VLAN的端口个数为1，其他所有端口（大于1个端口）为QinQ模式
            index = numberIsInArr(disableVlanModelPort, disableVlanPortCount, portNum);
            if (index != -1) {//当前设置的端口号是Disable VLAN端口
                if (model == VLANMODEL_DISABLE || model == VLANMODEL_VLAN)//不允许设置为Q_DISABLE VLAN 和VLAN Mode
                    return MSD_FALSE;
                return MSD_TRUE; //可以设置为QinQ
            }
            else {//当前设置的端口不是DISABLE VLAN端口，为QINQ端口
                if (model == VLANMODEL_QINQ || model == VLANMODEL_VLAN)//DISABLE 端口不允许设置为VLAN 模式 和 QINQ 端口模式
                    return MSD_FALSE;
                return MSD_TRUE;//
            }
        }
    }
    else if (disableVlanPortCount != 0 && vlanPortCount != 0) {//设置之前为disable 和 vlan端口模式的组合。
        if (vlanPortCount > 1 && disableVlanPortCount > 1) { //端口都大于1
            if (model == VLANMODEL_QINQ) {//这种情况，会导致3种端口模式会同时存在，因此不允许设置
                return MSD_FALSE;
            }
            else { //允许vlan model 和disable vlan port相互设置
                return 	MSD_TRUE;
            }
        }
        else if (vlanPortCount == 1) { //disable vlan 端口大于1，vlan 端口数等于1
            index = numberIsInArr(vlanModelPort, vlanPortCount, portNum);
            if (index != -1) {//设置的端口恰好为VLAN模式
                if (model == VLANMODEL_VLAN) {//不允许模式不改变
                    return MSD_FALSE;
                }
                return MSD_TRUE;//可以设置为DISABLE 和 QINQ
            }
            else { //设置的端口为Disable Mode模式(VLAN 模式还存在)
                if (model == VLANMODEL_VLAN)//可以将Disable模式，修改为VLAN模式
                    return MSD_TRUE;
                return MSD_FALSE;//
            }
        }
        else {//disable vlan 端口等于1，vlan 端口数大于1
            index = numberIsInArr(disableVlanModelPort, disableVlanPortCount, portNum);
            if (index != -1) { //设置的端口恰好为DISABLE VLAN模式
                if (model == VLANMODEL_VLAN)//可以设置为VLAN 模式
                    return MSD_TRUE;
                return MSD_FALSE;
            }
            else {//设置的端口为VLAN Mode模式（还有一个端口为Disable Mode模式）
                if (model == VLANMODEL_DISABLE)
                    return MSD_TRUE;
                return MSD_FALSE;//
            }
        }
    }
    else if (qinqPortCount != 0) {//设置之前全部为QinQ Port
        if (model == VLANMODEL_DISABLE) //仅允许设置为DISABLE
            return MSD_TRUE;
        return MSD_FALSE;
    }
    else if (vlanPortCount != 0) { //设置之前全部为Vlan Port
        if (model == VLANMODEL_DISABLE) //仅允许设置为DISABLE
            return MSD_TRUE;
        return MSD_FALSE;
    }
    else if (disableVlanPortCount != 0) { //设置之前全部为disable vlan port
        if (model == VLANMODEL_DISABLE)
            return MSD_FALSE;
        return MSD_TRUE;
    }
    return MSD_FALSE;//其他情况都不允许设置（一般不会出现这种情况）
}


MSD_STATUS deviceVlanModuleSetQMode(IN MSD_U8 devNum, IN int portNum, IN VlanModel model)
{
	CHECK_DEV_NUM_IS_CORRECT;
    if (portNum < ALL_PORT_PARAM || portNum > dev->numOfPorts - 1) {
        MSD_DBG_ERROR(("device_vlan_module_set_q_mode failed，port num param is out of range,should be %d or less then the dev numofPorts param!\n", ALL_PORT_PARAM));
        return MSD_BAD_PARAM;
    }
    if (!s_vtuModuleInitType[devNum].qModelIsGlobal && portNum == ALL_PORT_PARAM) {
        MSD_DBG_ERROR(("device_vlan_module_set_q_mode failed，the dev num:%d device q model is not global,the port_num param should be equal or greater than 0!\n", devNum));
        return MSD_BAD_PARAM;
    }
    if (s_vtuModuleInitType[devNum].qModelIsGlobal && portNum != ALL_PORT_PARAM) {
        MSD_DBG_ERROR(("device_vlan_module_set_q_mode failed，the dev num:%d device q model is global,the port_num param should be equal %d !\n", devNum, ALL_PORT_PARAM));
        return MSD_BAD_PARAM;
    }
    if (model < VLANMODEL_DISABLE || model > VLANMODEL_QINQ) {
        MSD_DBG_ERROR(("device_vlan_module_set_q_mode failed，the Q model param is out range!\n"));
        return MSD_BAD_PARAM;
    }
    MSD_STATUS ret = MSD_OK;
    if (portNum == ALL_PORT_PARAM) { //所有端口都设置为同一种标签模式
        if (!s_vtuModuleInitType[devNum].isSetTagMode)	//启动API时 ，默认Q_DISABLE,也可以判断是否是QinQ
        {
            for (int i = 0; i < dev->numOfPorts; ++i) {
                ret = deviceVlanModuleSetQModelInPort(devNum, i, model);
                if (ret != MSD_OK) {
                    break;
                }
            }
            s_vtuModuleInitType[devNum].globalQModel = model;
            s_vtuModuleInitType[devNum].isSetTagMode = MSD_TRUE;
        }
        else {
            //本次设置值和之前设置的值不同时设置
            if (model != s_vtuModuleInitType[devNum].globalQModel) {
                for (int i = 0; i < dev->numOfPorts; ++i) {
                    ret = deviceVlanModuleSetQModelInPort(devNum, i, model);
                    if (ret != MSD_OK) {
                        break;
                    }
                }
                s_vtuModuleInitType[devNum].globalQModel = model;
            }
        }
    }
    else {//端口可以使用不同模式(QinQ 和 Disable Mode组合， Disable Mode 和 VLAN Mode组合， 或者任意一种模式，其他混合模式都不允许)
        if (s_vtuModuleInitType[devNum].portQModel[portNum] != model) {
            if (isCanSetPortQMode(devNum, portNum, model)) {	//查询当前端口是否可以设置为指定的model
                ret = deviceVlanModuleSetQModelInPort(devNum, portNum, model);
            }
            else {
                ret = MSD_FEATRUE_NOT_ALLOW;
            }
        }
    }
    //设置为DISABLE VLAN时，发送SIGUSR2，不去检测vlan，发送SIGUSR1，检测vlan
    // if(ret == MSD_OK){
    //     static MSD_BOOL isUSR2 = MSD_FALSE;
    //     static MSD_BOOL isUSR1 = MSD_FALSE;
    //     if(model == VLANMODEL_DISABLE){
    //         if(g_allDevicesConfig[devNum].vtuThreadSuccess && !isUSR2){
    //             isUSR2 = MSD_TRUE;
    //             isUSR1 = MSD_FALSE;
    //             pthread_kill(g_allDevicesConfig[devNum].vtuOperationPtheread, SIGUSR2);// 不检测vlan信息
    //         }
    //     }else{
    //         if(g_allDevicesConfig[devNum].vtuThreadSuccess && !isUSR1){
    //             isUSR1 = MSD_TRUE;
    //             isUSR2 = MSD_FALSE;
    //             pthread_kill(g_allDevicesConfig[devNum].vtuOperationPtheread, SIGUSR1);//检测vlan信息
    //         }
    //     }
    // }
    return ret;
}

MSD_STATUS deviceVlanModuleGetQMode(IN MSD_U8 devNum, IN int portNum, OUT VlanModel* model)
{
	CHECK_DEV_NUM_IS_CORRECT;
    if (portNum < ALL_PORT_PARAM || portNum > dev->numOfPorts - 1) {
        MSD_DBG_ERROR(("port num param is out of range,should be %d or less then the dev numofPorts param!\n", ALL_PORT_PARAM));
        return MSD_BAD_PARAM;
    }
    if (!s_vtuModuleInitType[devNum].qModelIsGlobal && portNum == ALL_PORT_PARAM) {
        MSD_DBG_ERROR(("the dev num:%d device q model is not global,the port num param should be equal or greater than 0!\n", devNum));
        return MSD_BAD_PARAM;
    }
    if (s_vtuModuleInitType[devNum].qModelIsGlobal && portNum != ALL_PORT_PARAM) {
        MSD_DBG_ERROR(("the dev num:%d device q model is global,the port_num param should be %d!\n", devNum, ALL_PORT_PARAM));
        return MSD_BAD_PARAM;
    }
    if (portNum == ALL_PORT_PARAM) {//全部端口都设置为同一种模式
        *model = s_vtuModuleInitType[devNum].globalQModel;
    }
    else { //获取某个端口的Q Model模式
        *model = s_vtuModuleInitType[devNum].portQModel[portNum];
    }
    return MSD_OK;
}

MSD_STATUS deviceVlanModuleSetTpid(IN MSD_U8 devNum, IN MSD_U8 portNum, IN MSD_ETHERTYPE type, IN MSD_U16 eType)
{
	CHECK_DEV_NUM_IS_CORRECT;
    if (type > MSD_ETHERTYPE_RTYPE || type <  MSD_ETHERTYPE_Q) {
        MSD_DBG_ERROR(("device_vlan_module_set_tpid failed,the ethertype param is not correct!\n"));
        return MSD_BAD_PARAM;
    }
    if (portNum > dev->numOfPorts - 1) {
        MSD_DBG_ERROR(("device_vlan_module_set_tpid failed,the port num param is not correct!\n"));
        return MSD_BAD_PARAM;
    }
    MSD_STATUS ret = msdPortEtherTypeByTypeSet(devNum, portNum, type, eType);
    return ret;
}

MSD_STATUS deviceVlanModuleGetTpid(IN MSD_U8 devNum, IN MSD_U8 portNum, IN MSD_ETHERTYPE type, OUT MSD_U16* eType)
{
    *eType = 0;
    CHECK_DEV_NUM_IS_CORRECT;
    if (type > MSD_ETHERTYPE_RTYPE || type <  MSD_ETHERTYPE_Q) {
        MSD_DBG_ERROR(("device_vlan_module_get_tpid failed,the ethertype param is not correct!\n"));
        return MSD_BAD_PARAM;
    }
    if (portNum > dev->numOfPorts - 1) {
        MSD_DBG_ERROR(("device_vlan_module_get_tpid failed,the port num param is not correct!\n"));
        return MSD_BAD_PARAM;
    }
    MSD_STATUS ret = msdPortEtherTypeByTypeGet(devNum, portNum, type, eType);
    return ret;
}

MSD_STATUS deviceVlanModuleSet8021Mode(IN MSD_U8 devNum, IN int portNum, IN MSD_8021Q_MODE mode8021)
{
	CHECK_DEV_NUM_IS_CORRECT;
    if (mode8021 > MSD_8021Q_SECURE || mode8021 < MSD_8021Q_DISABLE) {
        MSD_DBG_ERROR(("device_vlan_module_set_8021_mode 802.1 Q Model param is out of range!\n"));
        return MSD_BAD_PARAM;
    }
    if (portNum > dev->numOfPorts - 1 || portNum < ALL_PORT_PARAM) {
        MSD_DBG_ERROR(("device_vlan_module_set_8021_mode port param is out of range!\n"));
        return MSD_BAD_PARAM;
    }
    if (portNum == ALL_PORT_PARAM && !s_vtuModuleInitType[devNum].qModelIsGlobal) {
        MSD_DBG_ERROR(("device_vlan_module_set_8021_mode port param is %d,but the q model is not global mode !\n", ALL_PORT_PARAM));
        return MSD_BAD_PARAM;
    }
    //设置
    MSD_STATUS ret = MSD_OK;
    do {
        if (mode8021 == MSD_8021Q_DISABLE) {
            if (s_vtuModuleInitType[devNum].qModelIsGlobal) {//全局标签模式
                if (s_vtuModuleInitType[devNum].globalQModel == VLANMODEL_VLAN) {//设备使用单标签，则设置所有端口禁用标签模式
                    ret = deviceVlanModuleSetQMode(devNum, ALL_PORT_PARAM, VLANMODEL_DISABLE);
                }
                //如果是全局双标签模式，则必须使用802.1 Q ,这是为了简单，强制双标签使用802.1 Q模式
                else if (s_vtuModuleInitType[devNum].globalQModel == VLANMODEL_QINQ) {
                    ret = MSD_FEATRUE_NOT_ALLOW;
                }
                //Q_DISABLE_MODEL 不做任何操作
            }
            else {//非全局标签模式
                if (s_vtuModuleInitType[devNum].portQModel[portNum] == VLANMODEL_QINQ) {//该端口为双标签（需要设置802.1Q 模式），不能进行操作
                    ret = MSD_FEATRUE_NOT_ALLOW;
                }
                else if (s_vtuModuleInitType[devNum].portQModel[portNum] == VLANMODEL_VLAN) {//单标签,设置为禁用标签模式，设置802.1 Q为禁用VLAN
                    ret = deviceVlanModuleSetQModelInPort(devNum, portNum, VLANMODEL_DISABLE);
                }
            }
        }
        else {//802.1Q
            if (s_vtuModuleInitType[devNum].qModelIsGlobal) {//全局标签模式
                if (s_vtuModuleInitType[devNum].globalQModel == VLANMODEL_DISABLE) {//如果设备禁用标签模式，则将设备所有端口设置为启用802.1Q
                    for (int i = 0; i < dev->numOfPorts; ++i) {

                        ret = msdPort8021qModeSet(devNum, i, mode8021);//先设置为具体的某个标签模式
                        if (ret != MSD_OK) break;
                    }
                    if (ret != MSD_OK) break;
                    ret = deviceVlanModuleSetQMode(devNum, ALL_PORT_PARAM, VLANMODEL_VLAN);
                }
                else { //设备使用标签模式或者QinQ
                    for (int i = 0; i < dev->numOfPorts; ++i) {
                        ret = msdPort8021qModeSet(devNum, i, mode8021);
                        if (ret != MSD_OK) break;
                    }
                }
            }
            else {//非全局标签模式
                if (deviceVlanModuleHasTheModePort(devNum, ALL_PORT_PARAM, VLANMODEL_QINQ)) {//端口中有Provider Port(包括自身)
                    if (s_vtuModuleInitType[devNum].portQModel[portNum] != VLANMODEL_QINQ) {//自身不是Provider Port，不允许设置802.1 Q 模式。
                        ret = MSD_FEATRUE_NOT_ALLOW;
                    }
                    else {//自身是Provider端口，可以设置802.1 Q模式
                        ret = msdPort8021qModeSet(devNum, portNum, mode8021);//
                    }
                }
                else {//不存在Provider Port，所有端口都为VLAN Model或者Q
                    ret = msdPort8021qModeSet(devNum, portNum, mode8021);//
                    if (ret != MSD_OK) break;
                    if (s_vtuModuleInitType[devNum].portQModel[portNum] == VLANMODEL_DISABLE) {//如果没启用VLAN Model，则会同时开启VLAN模式
                        ret = deviceVlanModuleSetQModelInPort(devNum, portNum, VLANMODEL_VLAN);
                    }
                }
            }
        }
    } while (0);
    return ret;
}

MSD_STATUS deviceVlanModuleGet8021Mode(IN MSD_U8 devNum, IN MSD_U8 portNum, OUT MSD_8021Q_MODE* mode8021)
{
	CHECK_DEV_NUM_IS_CORRECT;
    if (portNum > dev->numOfPorts - 1) {
        MSD_DBG_ERROR(("device_vlan_module_get_8021_mode failed,the port param is out of range!\n"));
        return MSD_BAD_PARAM;
    }
    MSD_STATUS ret = msdPort8021qModeGet(devNum, portNum, mode8021);
    return ret;
}

MSD_STATUS deviceVlanModuleSetDefaultVlanId(IN MSD_U8 devNum, IN MSD_U8 portNum, IN MSD_U16 defaultVid)
{
	CHECK_DEV_NUM_IS_CORRECT;
    if (portNum > dev->numOfPorts - 1) {
        MSD_DBG_ERROR(("device_vlan_module_set_default_vlan_id failed,the port param is out of range!\n"));
        return MSD_BAD_PARAM;
    }
    if (defaultVid > MAX_OPERATION_VID || defaultVid < PORT_DISABLE_VLAN_VID) { // 1 :不使用vlan时设置
        MSD_DBG_ERROR(("device_vlan_module_set_default_vlan_id failed,the default_vid param range is %d ~ %d !\n", PORT_DEFAULT_VID, MAX_OPERATION_VID));
        return MSD_BAD_PARAM;
    }
    MSD_STATUS ret = msdPortDefaultVlanIdSet(devNum, portNum, defaultVid);
    if (ret == MSD_OK) {
        s_vtuModuleInitType[devNum].portVlanMemberTagInfo.portVlanInfo[portNum].defaultVid = defaultVid;
    }
    return ret;
}

MSD_STATUS deviceVlanModuleGetDefaultVlanId(IN MSD_U8 devNum, IN MSD_U8 portNum, OUT MSD_U16* defaultVid)
{
    *defaultVid = 0;
    CHECK_DEV_NUM_IS_CORRECT;
    if (portNum > dev->numOfPorts - 1) {
        MSD_DBG_ERROR(("device_vlan_module_get_default_vlan_id failed,the port param is out of range!\n"));
        return MSD_BAD_PARAM;
    }
    MSD_STATUS ret = msdPortDefaultVlanIdGet(devNum, portNum, defaultVid);
    if (ret == MSD_OK) {
        s_vtuModuleInitType[devNum].portVlanMemberTagInfo.portVlanInfo[portNum].defaultVid = *defaultVid;
    }
    return ret;
}

MSD_STATUS deviceVlanModuleSetDefaultPri(IN MSD_U8 devNum, IN MSD_U8 portNum, IN MSD_U8 defaultPri)
{
	CHECK_DEV_NUM_IS_CORRECT;
    if(portNum > dev->numOfPorts-1){
        return MSD_BAD_PARAM;
    }
    if(defaultPri > 0x7){
        return MSD_BAD_PARAM;
    }
    MSD_STATUS ret = msdPortDefFPriSet(devNum, portNum, defaultPri);
    if (ret != MSD_OK)
        return ret;
    //ret = msdPortDefQPriSet(dev_num, port_num, default_pri);//Q_PRI设置为Default_pri一致。可选操作
    return ret;
}

MSD_STATUS deviceVlanModuleGetDefaultPri(IN MSD_U8 devNum, IN MSD_U8 portNum, OUT MSD_U8* defaultPri)
{
	CHECK_DEV_NUM_IS_CORRECT;
    if(portNum > dev->numOfPorts-1){
        return MSD_BAD_PARAM;
    }
    MSD_STATUS ret = msdPortDefFPriGet(devNum, portNum, defaultPri);
    return ret;
}

MSD_STATUS deviceVlanModuleSetInitailPriAndTagIfBoth(IN MSD_U8 devNum, IN MSD_U8 portNum, IN MSD_INITIAL_PRI initialPri, IN MSD_BOOL tagIfBothIsSet)
{
	CHECK_DEV_NUM_IS_CORRECT;
    if (portNum > dev->numOfPorts - 1 ||
        initialPri <  MSD_INITIAL_PRI_DEFAULT || initialPri > MSD_INITIAL_PRI_TAG_IP ||
        (tagIfBothIsSet != MSD_FALSE && tagIfBothIsSet != MSD_TRUE)) { //
        return MSD_BAD_PARAM;
    }
    MSD_STATUS ret = msdPortInitialPriSet(devNum, portNum, initialPri);
    if (ret != MSD_OK) return ret;
    ret = msdPortPrioRuleSet(devNum, portNum, tagIfBothIsSet);
    return ret;
}

MSD_STATUS deviceVlanModuleGetInitailPriAndTagIfBoth(IN MSD_U8 devNum, IN MSD_U8 portNum, OUT MSD_INITIAL_PRI* initialPri, OUT MSD_BOOL* tagIfBothIsSet)
{
	CHECK_DEV_NUM_IS_CORRECT;
    if (portNum > dev->numOfPorts - 1) { //
        return MSD_BAD_PARAM;
    }
    MSD_STATUS ret = msdPortInitialPriGet(devNum, portNum, initialPri);
    if (ret != MSD_OK) return ret;
    ret = msdPortPrioRuleGet(devNum, portNum, tagIfBothIsSet);
    return ret;
}
/**
 * @brief copyValidPortVlanMemberTagInfo
 * 将src中有效的VID memberTag条目拷贝到dest中的最前面，即最前面都是有效的VID对应的memberTag。
 * @param dest
 * @param src
 */
static void copyValidPortVlanMemberTagInfo(OUT VlanMemberTagAndEgressModeInfo* dest, IN VlanMemberTagAndEgressModeInfo* src)
{
    msdMemSet(dest, 0, sizeof(VlanMemberTagAndEgressModeInfo));
    dest->vidInfoSize = src->vidInfoSize;
    for (size_t i = 0; i < MSD_MAX_SWITCH_PORTS; ++i) {
        dest->portVlanInfo[i].egressMode = src->portVlanInfo[i].egressMode;
        dest->portVlanInfo[i].defaultVid = src->portVlanInfo[i].defaultVid;
    }
    for (int i = 0; i < ALLOW_OPERATION_MAX_VLAN_NUM; ++i) {
        dest->vidInfo[i].vid = src->vidInfo[i].vid;
        for (size_t j = 0; j < MSD_MAX_SWITCH_PORTS; j++) {
            dest->vidInfo[i].memberTag[j] = src->vidInfo[i].memberTag[j];
        }
        dest->vidInfo[i].isShow = src->vidInfo[i].isShow;
    }
}
/**
 * @brief isVidTagInfoHasExist 判断member_tag信息表中是否存在指定的vid的member_tag信息
 * @param operationVidMemberTag member_tag信息表
 * @param size member_tag信息表的长度
 * @param vid 要查询的vid
 * @param index 返回查询的vid在信息表中的下标，如果不存在则设置为-1
 * @return
 */
// static MSD_BOOL isVidTagInfoHasExist(VlanPerInfo operationVidMemberTag[], int size, int vid, OUT int* index) {
// 	*index = -1;
// 	for (int i = 0; i < size; i++) {
//         if (operationVidMemberTag[i].vid == vid) {
// 			*index = i;
// 			return MSD_TRUE;
// 		}
// 	}
// 	return MSD_FALSE;
// }

/**
 * @brief combineVlanMemberTag
 * 合并tagInfo1和tagInfo2的MemberTag到result_tag_info中，如果vid相同，则使用tagInfo2的VID的memberTag信息（因此tagInfo2的信息应该比tagInfo1里面的memberTag信息新）
 * @param tagInfo1 tagInfo1的有效memberTag数据都存储在前vidSize下标中
 * @param tagInfo2 tagInfo2的有效memberTag数据都存储在前vidSize下标中
 * @param resultTagInfo
 * @return
 */
// static MSD_BOOL combineVlanMemberTag(IN VlanMemberTagAndEgressModeInfo* tagInfo1, IN VlanMemberTagAndEgressModeInfo* tagInfo2, OUT VlanMemberTagAndEgressModeInfo* resultTagInfo)
// {
//     copyValidPortVlanMemberTagInfo(resultTagInfo, tagInfo2);//先将tag_info2的数据拷贝到结果表中,拷贝到result_tag_info的数组的最前面
//     for (size_t i = 0; i < tagInfo1->vidInfoSize; ++i) {//tag_info1
// 		int index;
//         isVidTagInfoHasExist(resultTagInfo->vidInfo, resultTagInfo->vidInfoSize, tagInfo1->vidInfo[i].vid, &index);
// 		if (index == -1) {//不存在，直接添加。如果存在，不替换。不操作
//             resultTagInfo->vidInfo[resultTagInfo->vidInfoSize].vid = tagInfo1->vidInfo[i].vid;
//             resultTagInfo->vidInfo[resultTagInfo->vidInfoSize].isShow = tagInfo1->vidInfo[i].isShow;
//             for (size_t j = 0; j < MSD_MAX_SWITCH_PORTS; j++) {
//                 resultTagInfo->vidInfo[resultTagInfo->vidInfoSize].memberTag[j] = tagInfo1->vidInfo[i].memberTag[j];
// 			}
//             resultTagInfo->vidInfoSize++;
// 		}
// 	}
// 	return MSD_TRUE;
// }

static MSD_BOOL checkEesVlanInfoIsValid(IN int portNum, IN VlanMemberTagAndEgressModeInfo* allPortVlanMemberTag) {
    if(allPortVlanMemberTag == NULL) return MSD_FALSE;
    int vidSize = allPortVlanMemberTag->vidInfoSize;
    if(vidSize > ALLOW_OPERATION_MAX_VLAN_NUM || vidSize < 0) return MSD_FALSE; //vid size 为0 可以
    for (int i = 1; i < portNum; ++i) {
        if (allPortVlanMemberTag->portVlanInfo[i].defaultVid < PORT_DEFAULT_VID || allPortVlanMemberTag->portVlanInfo[i].defaultVid > MAX_OPERATION_VID) {
            MSD_DBG_ERROR(("check_ees_vlan_info_is_valid failed,the default_vid param is out of range!\n"));
            return MSD_FALSE;
        }
        if (allPortVlanMemberTag->portVlanInfo[i].egressMode < VLAN_EGRESS_MODE_ALLOW_ALL || allPortVlanMemberTag->portVlanInfo[i].egressMode > VLAN_EGRESS_MODE_ALLOW_ALL_BUT_EXCLUSION_LIST) {
            MSD_DBG_ERROR(("check_ees_vlan_info_is_valid failed,the vlan_egress_mode param is out of range!\n"));
            return MSD_FALSE;
        }
    }

    for (int i = 0; i < vidSize; ++i) {
        if (allPortVlanMemberTag->vidInfo[i].vid < PORT_DEFAULT_VID || allPortVlanMemberTag->vidInfo[i].vid > MAX_OPERATION_VID) {
            MSD_DBG_ERROR(("check_ees_vlan_info_is_valid failed,the vid param is out of range!\n"));
            return MSD_FALSE;
        }
        for (int j = 0; j < portNum; ++j) {
            if (allPortVlanMemberTag->vidInfo[i].memberTag[j] < MSD_MEMBER_EGRESS_UNMODIFIED || allPortVlanMemberTag->vidInfo[i].memberTag[j] > MSD_NOT_A_MEMBER) {
                MSD_DBG_ERROR(("check_ees_vlan_info_is_valid failed,the member_tag param is out of range!\n"));
                return MSD_FALSE;
            }
        }
    }
    return MSD_TRUE;
}

/**
 * @brief deviceVlanModuleClearQinqVlanIngressAndEgressTcam
 *  清除所有与QinQ VLAN相关的IngressTCAM和EgressTCAM条目
 * @param devNum
 * @return
 */
static MSD_STATUS deviceVlanModuleClearQinqVlanIngressAndEgressTcam(IN MSD_U8 devNum)
{
    int end = QINQ_VLAN_VLAN_0_INGRESS_TCAM_POINTER + ALLOW_OPERATION_MAX_VLAN_NUM + 1;//前面用于表示所有VLAN相关(包括VID 0的优先级帧)的IngressTCAM,最后一条为qinq模式下，无标签帧匹配的IngressTCAM
    MSD_STATUS ret = MSD_OK;
    for (int i = QINQ_VLAN_VLAN_0_INGRESS_TCAM_POINTER; i <= end; ++i) {
        ret = msdTcamEntryDelete(devNum, i);
        if (ret != MSD_OK) break;
    }
    //删除出口条目
    ret = msdEgrTcamEntryAllPortsDelete(devNum, QINQ_VLAN_FOR_NO_TAG_FRAME_EGRESS_TCAM_POINTER);
    if (ret != MSD_OK) return ret;
    ret = msdEgrTcamEntryAllPortsDelete(devNum, QINQ_VLAN_FOR_TAG_FRAME_EGRESS_NOT_A_MEMBER_TCAM_POINTER);
    if (ret != MSD_OK) return ret;
    ret = msdEgrTcamEntryAllPortsDelete(devNum, QINQ_VLAN_FOR_TAG_FRAME_EGRESS_NOT_TAG_TCAM_POINTER);
    return ret;
}

/**
 * @brief deviceVlanModuleSetQinqVlanTagFramHandleTag
 * 通过IngressTCAM和EgressTCAM实现移除标签帧的标签。QinQ不能通过设置VLAN memberTag来移除标签
 * 注：默认Provider Port在出口是带标签的。即使不带标签帧进入时出口时也是带DefaultVID的标签
 * @param devNum 设备编号
 * @param vid 要设置的VID
 * @param tcamNumber IngressTCAM pointer
 * @param memberTag 指定VID 某端口的memberTag
 * @param egressPorts
 * @return
 */
static MSD_STATUS deviceVlanModuleSetQinqVlanTagFramHandleTag(IN MSD_U8 devNum,IN MSD_U16 vid, IN int tcamNumber, MSD_PORT_MEMBER_TAG memberTag, IN MSD_BOOL *egressPorts)
{
    MSD_TCAM_DATA tcamData;
    msdMemSet(&tcamData, 0, sizeof(MSD_TCAM_DATA));
    MSD_BOOL isFound = MSD_FALSE;
    MSD_STATUS  ret = msdTcamEntryFind(devNum, tcamNumber, &tcamData, &isFound);
    if (ret != MSD_OK) return ret;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    int num = dev->numOfPorts;

    int egressTcamPointer = 0;//默认不设置出口

    if (memberTag == MSD_NOT_A_MEMBER) {
        egressTcamPointer = QINQ_VLAN_FOR_TAG_FRAME_EGRESS_NOT_A_MEMBER_TCAM_POINTER;
    }
    else if(memberTag == MSD_MEMBER_EGRESS_UNTAGGED){
        egressTcamPointer = QINQ_VLAN_FOR_TAG_FRAME_EGRESS_NOT_TAG_TCAM_POINTER;//出口不带标签处理
    }

    if (!isFound) {
        MSD_BOOL ingressPorts[MSD_MAX_SWITCH_PORTS] = { MSD_FALSE };
        for (int i = 1; i < num; ++i) {//从Port 1开始设置，Port 0暂时不设置
            ingressPorts[i] = MSD_TRUE;
        }
        MSD_U16 ingressPortVec = (MSD_U16)boolarrayToInt(ingressPorts, num);//
        tcamData.frameTypeMask = 0x3;//启用
        tcamData.spvMask = (MSD_U16)~ingressPortVec;
        tcamData.frameType = 0x2;//Provider Tag Frame
        tcamData.pvid = vid;
        tcamData.pvidMask = (MSD_U16)0xffff;

        tcamData.egActPoint = egressTcamPointer;//设置入口条目的出口指针

        ret = msdTcamEntryAdd(devNum, tcamNumber, &tcamData);
    }
    else {

    }
    if (ret != MSD_OK) return ret;


    MSD_TCAM_EGR_DATA egressTcamData;
    msdMemSet(&egressTcamData, 0, sizeof(MSD_TCAM_EGR_DATA));
    egressTcamData.frameModeOverride = MSD_TRUE;//覆盖出口模式，frameMode为0x0时，则为Normal  Network
    egressTcamData.tagModeOverride = MSD_TRUE;//覆盖出口标签模式，tagMode为0x0时，Egress the frame unmodified.
    egressTcamData.tagMode = (MSD_U8)memberTag;
    for (int i = 1; i < num; ++i) {
        if (egressPorts[i]) {//
            ret = msdEgrTcamEntryAdd(devNum, i, egressTcamPointer, &egressTcamData);
        }
    }
    return ret;
}

/**
 * @brief deviceVlanModuleSetQinqVlanUntagFramHandleTag
 * 无标签帧，使用IngressTCAM和EgressTCAM实现VLAN出口的处理，如果之前已存在相应的IngresTCAM和EgressTCAM条目，则直接替换。
 * @param devNum 设备编号
 * @param memberTag 不带标签出口以何种方式（带标签，不带标签，不变化）
 * @param egressPorts 出口条目的使用的端口列表集合
 * @return
 */
static MSD_STATUS deviceVlanModuleSetQinqVlanUntagFramHandleTag(IN MSD_U8 devNum, MSD_PORT_MEMBER_TAG memberTag,MSD_BOOL *egressPorts)
{
    //应该一个端口一个端口的设置，3个IngressTCAM对应3个EgressTCAM，
    MSD_TCAM_DATA tcamData;
    msdMemSet(&tcamData, 0, sizeof(MSD_TCAM_DATA));
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    int num = dev->numOfPorts;
    MSD_BOOL ingressPorts[MSD_MAX_SWITCH_PORTS] = { MSD_FALSE };
    for (int i = 1; i < num; ++i) {//从Port 1开始设置，Port 0暂时不设置
        ingressPorts[i] = MSD_TRUE;
    }
    MSD_U16 ingressPortVec = (MSD_U16)boolarrayToInt(ingressPorts, num);//
    tcamData.frameTypeMask = 0x3;//启用
    tcamData.spvMask = (MSD_U16)~ingressPortVec;

    int egressTcamPointer = QINQ_VLAN_FOR_NO_TAG_FRAME_EGRESS_TCAM_POINTER;
    int ingressTcamPointer = QINQ_VLAN_INGRESS_START_TCAM_POINTER + ALLOW_OPERATION_MAX_VLAN_NUM;
    tcamData.egActPoint = egressTcamPointer;//设置入口条目的出口指针

    MSD_STATUS ret = msdTcamEntryAdd(devNum, ingressTcamPointer, &tcamData);
    if (ret != MSD_OK) return ret;
    MSD_TCAM_EGR_DATA egressTcamData;
    msdMemSet(&egressTcamData, 0, sizeof(MSD_TCAM_EGR_DATA));
    if (memberTag == MSD_MEMBER_EGRESS_TAGGED) { //显示S-TAG标签
        egressTcamData.frameModeOverride = MSD_TRUE;
        egressTcamData.frameMode = 0x2;//Provider_Tag模式
        //保证出口的Pri为0，如果不覆盖，则为C-TAG的Pri
        egressTcamData.egfpriModeOverride = MSD_TRUE;
        egressTcamData.egfpriMode = 0x2;
    }
    else { //不显示标签的处理
        egressTcamData.frameModeOverride = MSD_TRUE;//覆盖出口模式，frameMode为0x0时，则为Normal  Network
        egressTcamData.tagModeOverride = MSD_TRUE;//覆盖出口标签模式，tagMode为0x0时，Egress the frame unmodified.
        if (memberTag == MSD_MEMBER_EGRESS_UNTAGGED) {//移除标签的处理
            egressTcamData.tagMode = 0x1;
        }
    }
    for (int i = 1; i < num; ++i) {
        if (egressPorts[i]) {
            ret = msdEgrTcamEntryAdd(devNum, i, egressTcamPointer, &egressTcamData);
            if (ret != MSD_OK)
                return ret;
        }
    }
    return ret;
}

MSD_STATUS deviceVlanModuleSetVlanEgressModeAndMemberTagInfo(IN MSD_U8 devNum, IN VlanMemberTagAndEgressModeInfo* portVlanMemberTag/*, MSD_BOOL is_reset*/)
{
	CHECK_DEV_NUM_IS_CORRECT;
    if (!s_vtuModuleInitType[devNum].qModelIsGlobal) {
        MSD_DBG_ERROR(("device_vlan_module_set_vlan_egress_mode_and_member_tag_info failed,the q module shoule be global!\n"));
        return MSD_FEATRUE_NOT_ALLOW;
    }

    if(s_vtuModuleInitType[devNum].globalQModel == VLANMODEL_DISABLE){
        MSD_DBG_ERROR(("device_vlan_module_set_vlan_egress_mode_and_member_tag_info failed,the q module shoule not diable vlan!\n"));
        return MSD_FEATRUE_NOT_ALLOW;
    }

    if (!checkEesVlanInfoIsValid(dev->numOfPorts,portVlanMemberTag)) {
        return MSD_BAD_PARAM;
    }

    VlanMemberTagAndEgressModeInfo beforeSetPortVlanMemberTag;//已经存在的,使用外面传递的参数(以修改值为准)。
    MSD_STATUS ret = deviceVlanModuleGetVlanEgressModeMemberTagInfo(devNum, &beforeSetPortVlanMemberTag);//
    if (ret != MSD_OK) return ret;

    ret = deviceVlanModuleClearQinqVlanIngressAndEgressTcam(devNum);//清除与QinQ相关的所有的IngressTCAM项
    if (ret != MSD_OK) return ret;

    ret = deviceVlanModuleResetVlanInfo(devNum); //保留VID 1和VID 0xfff
    if (ret != MSD_OK) return ret;

    VlanMemberTagAndEgressModeInfo tempVlanMemberTag;
    msdMemSet(&tempVlanMemberTag, 0, sizeof(VlanMemberTagAndEgressModeInfo));
    for (int i = 0; i < ALLOW_OPERATION_MAX_VLAN_NUM; ++i) {
        tempVlanMemberTag.vidInfo[i].vid = UNVALID_VID;
    }
    //    MSD_BOOL isReset = MSD_TRUE;//这里先代替参数。
    //    if (!isReset) { //和之前的VLAN项目进行整合
    //        combineVlanMemberTag(&beforeSetPortVlanMemberTag, portVlanMemberTag, &tempVlanMemberTag);
    // }
    // else {//替换之前的VLAN条目
    copyValidPortVlanMemberTagInfo(&tempVlanMemberTag, portVlanMemberTag);//
    // }
    FirVlanEntry vtuEntry;
    MSD_BOOL isFound;
    int portNum = dev->numOfPorts;

    int allPortDefaultVids[MSD_MAX_SWITCH_PORTS] = { 0 };
    for (int j = 0; j < portNum; ++j) {//

        //如果所有端口都是Allow All(即允许所有VLAN出口)，所有出口端口都为 MSD_MEMBER_EGRESS_UNMODIFIED，则可以不用添加该VID条目(这里不做任何判断)
        VlanEgressMode egressMode = tempVlanMemberTag.portVlanInfo[j].egressMode;
        s_vtuModuleInitType[devNum].portVlanMemberTagInfo.portVlanInfo[j].egressMode = egressMode;
        MSD_8021Q_MODE mode8021;
        if (egressMode == VLAN_EGRESS_MODE_ALLOW_MEMBERSHIP) {
            mode8021 = MSD_8021Q_CHECK;//VID必须包含在VTU中，否则丢弃帧，出口端口必须为其成员，才能从端口出口
        }
        else {//Allow All or All All but exclusion the list
            mode8021 = MSD_8021Q_FALLBACK;//VID不是必须在VTU列表中，如果VID包含在VTU列表中，出口必须为其成员
        }
        ret = deviceVlanModuleSet8021Mode(devNum, j, mode8021);//设置802.1 FallBack模式
        if (ret != MSD_OK) return ret;
        //设置DefaultVID
        MSD_U16 defaultVid = tempVlanMemberTag.portVlanInfo[j].defaultVid;
        allPortDefaultVids[j] = defaultVid;
        s_vtuModuleInitType[devNum].portVlanMemberTagInfo.portVlanInfo[j].defaultVid = defaultVid;
        ret = msdPortDefaultVlanIdSet(devNum, j, defaultVid);//设置默认VID
        if (ret != MSD_OK) return ret;
    }

    //添加VID条目
    MSD_BOOL isPortDefaultVidUnset[MSD_MAX_SWITCH_PORTS];//记录端口是否被设置了默认VID
    VlanModel vlanModel = s_vtuModuleInitType[devNum].globalQModel;
    MSD_BOOL isAddVtu0 = MSD_FALSE;
    for (int i = 0; i < portNum; i++) {
        isPortDefaultVidUnset[i] = MSD_TRUE;//所有端口都没有设置默认VID

        //如果为ALL_MEMBER_SHIP并且为Q_QINQ模式，添加VID 0
        if (!isAddVtu0) {
            if (vlanModel == VLANMODEL_QINQ && tempVlanMemberTag.portVlanInfo[i].egressMode == VLAN_EGRESS_MODE_ALLOW_MEMBERSHIP) {
                MSD_VTU_ENTRY vtuEntry;
                MSD_BOOL isFound;
                msdMemSet(&vtuEntry, 0, sizeof(MSD_VTU_ENTRY));
                ret = msdVlanEntryFind(devNum, 0, &vtuEntry, &isFound);
                if (ret != MSD_OK) return ret;
                if (!isFound) {
                    ret = msdVlanEntryAdd(devNum, &vtuEntry);
                    if (ret != MSD_OK) return ret;
                    isAddVtu0 = MSD_TRUE;
                }
            }
        }
    }
    MSD_BOOL qinqModelUntagFrameTaggedMemberPorts[MSD_MAX_SWITCH_PORTS] = { MSD_FALSE}; //qinq 无标签帧带标签出口的集合
    MSD_BOOL qinqModelUntagFrameUntaggedMemberPorts[MSD_MAX_SWITCH_PORTS] = {MSD_FALSE}; //qinq 无标签帧移除标签出口的集合
    MSD_BOOL isSetUntaggedMember = MSD_FALSE;
    MSD_BOOL qinqModelUntagFrameUnmodifiedMemberPorts[MSD_MAX_SWITCH_PORTS] = { MSD_FALSE };//qinq 无标签帧标签出口的集合(和untagged处理有一定的区别)

    int ingressTcamIndex = -1;
    MSD_BOOL qinqModelDefaultTagFrameNotMemberPorts[MSD_MAX_SWITCH_PORTS] = { MSD_FALSE };//QINQ 默认VID，出口非成员的设置
    MSD_BOOL isSetDefaultVidTagFrameNotMember = MSD_FALSE; //QINQ 默认VID，出口是否有非成员的设置

    for (size_t i = 0; i < tempVlanMemberTag.vidInfoSize; ++i) {//vlan table 行
        //       if (tempVlanMemberTag.vidInfo[i].vid == UNVALID_VID) { //没有设置
        // 	continue;
        // }
        int vid = tempVlanMemberTag.vidInfo[i].vid;
        MSD_BOOL qinqModelTagFrameUntaggedMemberPorts[MSD_MAX_SWITCH_PORTS] = { MSD_FALSE };//用以描述某个VID的标签帧出口是否取消标签
        MSD_BOOL qinqModelTagFrameHasUntaggedMemberPort = MSD_FALSE;
        msdMemSet(&vtuEntry, 0, sizeof(FirVlanEntry));
        ret = msdVlanEntryFind(devNum, tempVlanMemberTag.vidInfo[i].vid, &vtuEntry.entry, &isFound);
        if (ret != MSD_OK) break;
        vtuEntry.entry.vid = vid;
        vtuEntry.entry.fid = vid;//使用多数据库，fid设置为VID
        for (int j = 1; j < portNum; ++j) {//vlan table 列
            //如果Port j允许所有VLAN帧出口，则需要将其非成员设置不加修改的方式传输帧
            MSD_PORT_MEMBER_TAG memberTag = tempVlanMemberTag.vidInfo[i].memberTag[j];
            if (memberTag == MSD_NOT_A_MEMBER
                && tempVlanMemberTag.portVlanInfo[j].egressMode == VLAN_EGRESS_MODE_ALLOW_ALL) {
                memberTag = MSD_MEMBER_EGRESS_UNMODIFIED;
            }

            if (vlanModel == VLANMODEL_QINQ) {
                if (memberTag == MSD_MEMBER_EGRESS_UNTAGGED) {//如果Port j 指定VID的memberTag为Untagged，qinq则需要移除标签。
                    qinqModelTagFrameUntaggedMemberPorts[j] = MSD_TRUE;
                    qinqModelTagFrameHasUntaggedMemberPort = MSD_TRUE;
                }
                //添加Provider Port 无标签帧的处理
                if (vid == allPortDefaultVids[j]) {//Port j端口的默认VID被设置。
                    isPortDefaultVidUnset[j] = MSD_FALSE;
                    //如果端口的默认VID被设置为带标签，默认情况下，不带标签进入Provider Port会添加一个QinQ Tag,添加0x88a8(根据TPID的设置)，
                    //优先级和DEI都配置为C-TAG的优先级和DEI(如果存在)，不存在都设置为0,EES交换机的做法如果默认DefaultVID被设置，并且出口带标签的
                    //情况下，不管帧是否带C-TAG，其S-TAG的PCP和DEI都设置为0，这里按照EES的做法，将无标签的帧的PCP，DEI都设置为0
                    if (memberTag == MSD_MEMBER_EGRESS_TAGGED) {
                        qinqModelUntagFrameTaggedMemberPorts[j] = MSD_TRUE;
                        //如果端口的默认VID被设置，则不允许VID 0出口（EES）
                        ret = msdPortAllowVidZeroSet(devNum, (MSD_LPORT)j, MSD_FALSE);
                        if (ret != MSD_OK)
                            return ret;
                    }
                    //无标签帧在出口UNMODIFIED和UNTAGGED的情况下，都统一将其S-TAG移除（默认情况下，会带一个标签）。
                    else if (memberTag == MSD_MEMBER_EGRESS_UNMODIFIED || memberTag == MSD_MEMBER_EGRESS_UNTAGGED) {
                        ret = msdPortAllowVidZeroSet(devNum, (MSD_LPORT)j, MSD_TRUE);//允许
                        if (ret != MSD_OK)
                            return ret;
                        //如果端口的默认VID被设置，则允许VID 0出口（EES）
                        if (memberTag == MSD_MEMBER_EGRESS_UNMODIFIED) {
                            qinqModelUntagFrameUnmodifiedMemberPorts[j] = MSD_TRUE;
                        }
                        else {//MSD_MEMBER_EGRESS_UNTAGGED
                            qinqModelUntagFrameUntaggedMemberPorts[j] = MSD_TRUE;
                            isSetUntaggedMember = MSD_TRUE;
                            ////EES:如果默认VID出口删除标签，则VID 0的帧的S-TAG被移除。
                            //ret = device_vlan_module_set_qinq_vlan_tag_fram_handle_tag(dev_num, 0, QINQ_VLAN_VLAN_0_INGRESS_TCAM_POINTER, j, member_tag);
                            //if (ret != MSD_OK) return ret;
                            if (tempVlanMemberTag.portVlanInfo[j].egressMode == VLAN_EGRESS_MODE_ALLOW_MEMBERSHIP) {
                                ret = msdVlanMemberTagSet(devNum, 0, j, MSD_MEMBER_EGRESS_UNTAGGED);//虽然双标签 UNTAGGED设置无效，但是利于观察。设置一下标签模式
                                if (ret != MSD_OK) return ret;
                            }

                        }
                    }
                    else {//not a member
                        qinqModelDefaultTagFrameNotMemberPorts[j] = MSD_TRUE;//某个端口的默认VID的memberTag设置为not a member,
                        isSetDefaultVidTagFrameNotMember = MSD_TRUE;
                    }
                }
            }
            vtuEntry.entry.memberTagP[j] = memberTag;
        }
        if (vlanModel == VLANMODEL_QINQ) {
            //指定VID，设置Port的memberTag为UNtagged的处理
            if (qinqModelTagFrameHasUntaggedMemberPort) {
                ingressTcamIndex++;
                int tcam_pointer = QINQ_VLAN_INGRESS_START_TCAM_POINTER + ingressTcamIndex;
                ret = deviceVlanModuleSetQinqVlanTagFramHandleTag(devNum, vid, tcam_pointer, MSD_MEMBER_EGRESS_UNTAGGED, qinqModelTagFrameUntaggedMemberPorts);
                if (ret != MSD_OK) return ret;
            }
        }

        ret = deviceVlanModuleAddOrModifyVlan(devNum, &vtuEntry);
        if (ret != MSD_OK) return ret;
    }
    //QinQ: 端口被设置为Provider Port的情况下
    if (vlanModel == VLANMODEL_QINQ) {
        //在没有设置端口的DefaultVID时候(以MSD_MEMBER_EGRESS_UNMODIFIED转发)
        for (int i = 1; i < portNum; ++i) {
            if (isPortDefaultVidUnset[i]) {//端口没有设置默认VID
                qinqModelUntagFrameUnmodifiedMemberPorts[i] = MSD_TRUE;
            }
        }

        ret = deviceVlanModuleSetQinqVlanUntagFramHandleTag(devNum, MSD_MEMBER_EGRESS_UNMODIFIED, qinqModelUntagFrameUnmodifiedMemberPorts);
        if (ret != MSD_OK) return ret;
        ret = deviceVlanModuleSetQinqVlanUntagFramHandleTag(devNum, MSD_MEMBER_EGRESS_UNTAGGED, qinqModelUntagFrameUntaggedMemberPorts);
        if (ret != MSD_OK) return ret;
        int vid0TcamPointer = QINQ_VLAN_VLAN_0_INGRESS_TCAM_POINTER;
        //EES:如果默认VID出口删除标签，则VID 0的帧的S-TAG被移除。
        if (isSetUntaggedMember) {//如果有端口的默认VID被设置Untagged，则需要将这些端口的VID 0出口时设置为不带Tag。
            ret = deviceVlanModuleSetQinqVlanTagFramHandleTag(devNum, 0, vid0TcamPointer, MSD_MEMBER_EGRESS_UNTAGGED, qinqModelUntagFrameUntaggedMemberPorts);
            if (ret != MSD_OK) return ret;
        }
        if (isSetDefaultVidTagFrameNotMember) {//端口的默认VID不允许出口
            ret = deviceVlanModuleSetQinqVlanTagFramHandleTag(devNum, 0, vid0TcamPointer, MSD_NOT_A_MEMBER, qinqModelDefaultTagFrameNotMemberPorts);
            if (ret != MSD_OK) return ret;
        }
        ret = deviceVlanModuleSetQinqVlanUntagFramHandleTag(devNum, MSD_MEMBER_EGRESS_TAGGED, qinqModelUntagFrameTaggedMemberPorts);
        if (ret != MSD_OK) return ret;
    }
    return ret;
}

MSD_STATUS deviceVlanModuleGetVlanEgressModeMemberTagInfo(IN MSD_U8 devNum, OUT VlanMemberTagAndEgressModeInfo* allPortVlanMemberTag)
{
	CHECK_DEV_NUM_IS_CORRECT;
    if (allPortVlanMemberTag == NULL) {
        MSD_DBG_ERROR(("device_vlan_module_get_member_tag_info failed,the all_port_vlan_member_tag param is NULL!\n"));
        return MSD_BAD_PARAM;
    }
    copyValidPortVlanMemberTagInfo(allPortVlanMemberTag, &s_vtuModuleInitType[devNum].portVlanMemberTagInfo);
    return MSD_OK;
}

#define PROVIDER_PORT_DEAL_WITH_NO_PROVIDER_TAG_VID   0 //Provider Port处理非Provider Tag帧的默认VID

//MSD_STATUS deviceVlanModuleSetProviderPortDealWithNoProviderTagFrame(IN MSD_U8 devNum, IN int portNum, IN int fid, IN MSD_LPORT* ports, IN int portsSize)
//{
    //    MSD_QD_DEV* dev = sohoDevGet(devNum);
    //    if (dev == NULL) {
    // 	MSD_DBG_ERROR(("device_vlan_module_set_provider_port_deal_with_no_provider_tag_frame failed,the dev num param is not open or the dev num param is out of range!\n"));
    // 	return MSD_BAD_PARAM;
    // }
    //    if (portNum < 0 || portNum > dev->numOfPorts - 1) {
    //        MSD_DBG_ERROR(("device_vlan_module_set_provider_port_deal_with_no_provider_tag_frame failed,the port_num param range is 0 to %d\n", dev->numOfPorts - 1));
    // 	return MSD_BAD_PARAM;
    // }
    //    if (s_vtuModuleInitType.[devNum]portQModel[devNum][portNum] != VLANMODEL_QINQ) {//端口不是Provider Port 操作不被允许
    // 	MSD_DBG_ERROR(("device_vlan_module_set_provider_port_deal_with_no_provider_tag_frame failed,the port is not provider port!\n"));
    // 	return MSD_FEATRUE_NOT_ALLOW;
    // }
    // if (fid < 0 || fid > 0xfff) {
    // 	MSD_DBG_ERROR(("device_vlan_module_set_provider_port_deal_with_no_provider_tag_frame failed,the fid param is not correct!\n"));
    // 	return MSD_BAD_PARAM;
    // }
    //    for (int i = 0; i < portsSize; ++i) {
    //        if (ports[i] > dev->numOfPorts - 1) {
    // 		MSD_DBG_ERROR(("device_vlan_module_set_provider_port_deal_with_no_provider_tag_frame failed,the ports param is not correct!\n"));
    // 		return MSD_BAD_PARAM;
    // 	}
    // }
    // MSD_STATUS ret = MSD_OK;
    // do {
    //        if (ports == NULL || portsSize == 0) {
    //            ret = msdPortDiscardUntaggedSet(devNum, portNum, MSD_TRUE);
    // 		if (ret != MSD_OK) break;
    //            ret = msdPortDefaultVlanIdSet(devNum, portNum, PORT_DEFAULT_VID);//设置DefaultVID为1
    // 		if (ret != MSD_OK) break;
    // 	}
    // 	else {
    //            FirVlanEntry vtuEntry;
    //            msdMemSet(&vtuEntry, 0, sizeof(FirVlanEntry));
    //            MSD_BOOL isFoundVid;
    //            vtuEntry.entry.fid = fid;
    //            vtuEntry.page = 0;
    //            vtuEntry.entry.vid = PROVIDER_PORT_DEAL_WITH_NO_PROVIDER_TAG_VID;
    //            ret = deviceVlanModuleFindVlan(devNum, &vtuEntry, &isFoundVid);
    //            if (ret != MSD_OK || !isFoundVid) break;

    //            //Provider Port VTU条目的MemberTag目前仅支持2种模式：要么端口不是其VLAN成员，要么以不加修改的方式传输帧。
    //            for (size_t i = 0; i < MSD_MAX_SWITCH_PORTS; ++i) {
    //                vtuEntry.entry.memberTagP[i] = MSD_NOT_A_MEMBER;
    // 		}
    //            for (int i = 0; i < portsSize; ++i) {
    //                vtuEntry.entry.memberTagP[ports[i]] = MSD_MEMBER_EGRESS_UNMODIFIED;
    // 		}
    //            ret = deviceVlanModuleAddOrModifyVlan(devNum, &vtuEntry);
    // 		if (ret != MSD_OK) break;
    //            ret = msdPortDefaultVlanIdSet(devNum, portNum, PROVIDER_PORT_DEAL_WITH_NO_PROVIDER_TAG_VID);
    // 		if (ret != MSD_OK) break;
    //            ret = msdPortForceDefaultVlanIdSet(devNum, portNum, MSD_TRUE);//强制使用默认VID 0
    // 		if (ret != MSD_OK) break;
    //            ret = msdPortDiscardUntaggedSet(devNum, portNum, MSD_FALSE);
    // 		if (ret != MSD_OK) break;
    // 	}
    // } while (0);
    // return ret;
//}

MSD_STATUS deviceVlanModuleGetModePort(IN MSD_U8 devNum, OUT int* ports, IN int portsSize, OUT int* portsCount, IN VlanModel model)
{
    *portsCount = 0;
    CHECK_DEV_NUM_IS_CORRECT;
    if (model < VLANMODEL_DISABLE || model > VLANMODEL_QINQ) {
        return MSD_BAD_PARAM;
    }
    if (portsSize == 0 || ports == NULL) {
        return MSD_BAD_PARAM;
    }
    for (int i = 0; i < portsSize; i++) {
        ports[i] = -1;//初始化
    }
    MSD_STATUS ret = MSD_OK;
    if (s_vtuModuleInitType[devNum].qModelIsGlobal) { //全局模式
        if (model == s_vtuModuleInitType[devNum].globalQModel) {//模式相同
            *portsCount = dev->numOfPorts;
            if (portsSize < *portsCount) {
                return MSD_NO_SPACE;
            }
            for (int i = 0; i < *portsCount; ++i) {//从Port 1开始
                ports[i] = i;
            }
        }
    }
    else {
        for (int i = 0; i < dev->numOfPorts; ++i) {
            if (s_vtuModuleInitType[devNum].portQModel[i] == model) {
                ports[*portsCount] = i;
                (*portsCount)++;
                if (portsSize < *portsCount) {
                    return MSD_NO_SPACE;
                }
            }
        }
    }
    return ret;
}

/**
 * @brief deviceVlanModuleFindVlanConfigIndex
 *  查询是否可以将VID存储到相应的VLAN配置文件的相应下标中，如果当前配置的VLAN条目已达到ALLOW_OPERATION_MAX_VLAN_NUM，则返回MSD_FALSE
 * @param devNum 设备编号
 * @param vid 要查询的VID
 * @param index 存储的下标中
 * @param isExist 如果为真，则配置中存在查询的VID，否则将VLAN 配置中不存在查询的VID
 * @return
 */
static MSD_BOOL deviceVlanModuleFindVlanConfigIndex(IN MSD_U8 devNum, IN MSD_U16 vid, OUT int* index, OUT MSD_BOOL* isExist)
{
    (*index) = -1;
    *isExist = MSD_FALSE;
    int temp = -1;
    for (int i = 0; i < ALLOW_OPERATION_MAX_VLAN_NUM; ++i) {
        MSD_16 tempVid = s_vtuModuleInitType[devNum].vtuConfigInfo.portVlanMemberTagInfo.vidInfo[i].vid;
        if (tempVid == vid) {//存在对应的vid
            *index = i;
            temp = i;
            *isExist = MSD_TRUE;
            break;
        }
    }
    if (temp == -1) { //条目中不存在，则找到一个没有使用的位置
        for (int i = 0; i < ALLOW_OPERATION_MAX_VLAN_NUM; ++i) {
            MSD_16 tempVid = s_vtuModuleInitType[devNum].vtuConfigInfo.portVlanMemberTagInfo.vidInfo[i].vid;
            if (tempVid == UNVALID_VID) {
                (*index) = i;
                temp = i;
                break;
            }
        }
    }
    if (temp != -1)//找到合适的位置
        return MSD_TRUE;
    return MSD_FALSE;//没有找到
}

MSD_STATUS deviceVlanModuleAddOrModifyVlanWithVidAndMemberTagToConfig(IN MSD_U8 devNum, IN MSD_U16 vid, IN MSD_PORT_MEMBER_TAG memberTag[MSD_MAX_SWITCH_PORTS], IN MSD_U8 memberTagSize)
{
    MSD_STATUS ret = MSD_OK;
    CHECK_DEV_NUM_IS_CORRECT;
    if(memberTag != NULL){
        for (int i = 0; i < memberTagSize; ++i) {
            if (memberTag[i] < MSD_MEMBER_EGRESS_UNMODIFIED || memberTag[i] > MSD_NOT_A_MEMBER) {
                MSD_DBG_ERROR(("device_vlan_module_add_or_modify_vlan_with_vid_and_member_tag_to_config failed,the member tag param is out of range!\n"));
                return MSD_BAD_PARAM;
            }
        }
    }
    if (vid < PORT_DEFAULT_VID || vid > MAX_OPERATION_VID) {
        MSD_DBG_ERROR(("device_vlan_module_add_or_modify_vlan_with_vid_and_member_tag failed,the default_vid param range is %d ~ %d !\n", PORT_DEFAULT_VID, MAX_OPERATION_VID));
        return MSD_BAD_PARAM;
    }
    int index;
    MSD_BOOL isExist;
    deviceVlanModuleFindVlanConfigIndex(devNum, vid, &index, &isExist);
    if (index != -1) {
        s_vtuModuleInitType[devNum].vtuConfigInfo.portVlanMemberTagInfo.vidInfo[index].vid = vid;
        s_vtuModuleInitType[devNum].vtuConfigInfo.portVlanMemberTagInfo.vidInfo[index].isShow = MSD_TRUE;
        if(memberTag == NULL) memberTagSize = 0;
        if (memberTagSize < dev->numOfPorts) {
            //如果member_tag的参数的个数小于端口数，则将后续的端口memberTag设置为MSD_MEMBER_EGRESS_UNMODIFIED
            for (int i = memberTagSize; i < dev->numOfPorts; ++i) {
                s_vtuModuleInitType[devNum].vtuConfigInfo.portVlanMemberTagInfo.vidInfo[index].memberTag[i] = MSD_MEMBER_EGRESS_UNMODIFIED;
            }
        }
        for (int i = 0; i < memberTagSize; ++i) {
            s_vtuModuleInitType[devNum].vtuConfigInfo.portVlanMemberTagInfo.vidInfo[index].memberTag[i] = memberTag[i];
        }
        if (!isExist) {//如果VLAN不存在，则将其操作值+1
            s_vtuModuleInitType[devNum].vtuConfigInfo.portVlanMemberTagInfo.vidInfoSize++;
        }
    }
    else {//条目已达最大值，不能添加VLAN了。
        MSD_DBG_ERROR(("device_vlan_module_add_or_modify_vlan_with_vid_and_member_tag_to_config failed, because vtu config's vtu numer is reach the max:%d\n", ALLOW_OPERATION_MAX_VLAN_NUM));
        return MSD_FEATRUE_NOT_ALLOW;
    }
    return ret;
}

MSD_STATUS deviceVlanModuleRemoveVlanFromConfig(IN MSD_U8 devNum, IN MSD_U16 vid)
{
	CHECK_DEV_NUM_IS_CORRECT;
    if (vid < PORT_DEFAULT_VID || vid > MAX_OPERATION_VID) {
        MSD_DBG_ERROR(("device_vlan_module_remove_vlan_from_config failed,the vid param shoule between %d and %d!\n",PORT_DEFAULT_VID,MAX_OPERATION_VID));
        return MSD_BAD_PARAM;
    }
    int index;
    MSD_BOOL isExist;
    deviceVlanModuleFindVlanConfigIndex(devNum, vid, &index, &isExist);
    if (isExist) {//如果配置中存在对应的VID
        s_vtuModuleInitType[devNum].vtuConfigInfo.portVlanMemberTagInfo.vidInfo[index].vid = UNVALID_VID;
        s_vtuModuleInitType[devNum].vtuConfigInfo.portVlanMemberTagInfo.vidInfo[index].isShow = MSD_FALSE;
        s_vtuModuleInitType[devNum].vtuConfigInfo.portVlanMemberTagInfo.vidInfoSize--;
    }
    return MSD_OK;
}

MSD_STATUS deviceVlanModuleCleanAllVlanAndMemberTagFromConfig(IN MSD_U8 devNum, MSD_BOOL isResetDefaultVid, MSD_BOOL isResetVlanEgressMode)
{
	CHECK_DEV_NUM_IS_CORRECT;
    if(isResetDefaultVid != MSD_TRUE && isResetDefaultVid != MSD_FALSE){
        isResetDefaultVid = MSD_TRUE;
    }
    if(isResetVlanEgressMode != MSD_TRUE && isResetVlanEgressMode != MSD_FALSE){
        isResetVlanEgressMode = MSD_TRUE;
    }
    s_vtuModuleInitType[devNum].vtuConfigInfo.portVlanMemberTagInfo.vidInfoSize = 0;
    for (int i = 0; i < ALLOW_OPERATION_MAX_VLAN_NUM; ++i) {
        s_vtuModuleInitType[devNum].vtuConfigInfo.portVlanMemberTagInfo.vidInfo[i].vid = UNVALID_VID;
        s_vtuModuleInitType[devNum].vtuConfigInfo.portVlanMemberTagInfo.vidInfo[i].isShow = MSD_FALSE;
    }
    if (isResetDefaultVid) {
        for (size_t i = 0; i < MSD_MAX_SWITCH_PORTS; i++) {
            s_vtuModuleInitType[devNum].vtuConfigInfo.portVlanMemberTagInfo.portVlanInfo[i].defaultVid = PORT_DEFAULT_VID;
        }
    }
    if (isResetVlanEgressMode) {
        for (size_t i = 0; i < MSD_MAX_SWITCH_PORTS; i++) {
            s_vtuModuleInitType[devNum].vtuConfigInfo.portVlanMemberTagInfo.portVlanInfo[i].egressMode = VLAN_EGRESS_MODE_ALLOW_MEMBERSHIP;
        }
    }
    return MSD_OK;
}

MSD_STATUS deviceVlanModuleSetQModeAndQinqTpidToConfig(IN MSD_U8 devNum, IN MSD_U16 qinqTpid, IN VlanModel vlanModel)
{
	CHECK_DEV_NUM_IS_CORRECT;
    if (vlanModel < VLANMODEL_DISABLE || vlanModel > VLANMODEL_QINQ) {
        MSD_DBG_ERROR(("device_vlan_module_set_q_mode_and_qinq_tpid_to_config failed,the vlan model param is out of range!\n"));
        return MSD_BAD_PARAM;
    }
    s_vtuModuleInitType[devNum].vtuConfigInfo.qinqTpid = qinqTpid;
    s_vtuModuleInitType[devNum].vtuConfigInfo.vlanModel = vlanModel;
    return MSD_OK;
}

/**
 * @brief deviceVlanModuleGetAllVlanInfo
 * 找到VLAN的EES VLAN 表格描述，VLAN vid信息存储在 vlan_info参数的前vlan_info->all_operation_vid_info_size参数中
 * @param devNum
 * @param vlanInfo
 */
static void  deviceVlanModuleGetAllVlanInfo(IN MSD_U8 devNum, OUT VlanMemberTagAndEgressModeInfo* vlanInfo)
{
    msdMemSet(vlanInfo, 0, sizeof(VlanMemberTagAndEgressModeInfo));
    vlanInfo->vidInfoSize = s_vtuModuleInitType[devNum].vtuConfigInfo.portVlanMemberTagInfo.vidInfoSize;
    for (size_t i = 0; i < MSD_MAX_SWITCH_PORTS; i++) {
        vlanInfo->portVlanInfo[i].defaultVid = s_vtuModuleInitType[devNum].vtuConfigInfo.portVlanMemberTagInfo.portVlanInfo[i].defaultVid;
        vlanInfo->portVlanInfo[i].egressMode = s_vtuModuleInitType[devNum].vtuConfigInfo.portVlanMemberTagInfo.portVlanInfo[i].egressMode;
    }
    int index = 0;
    for (int i = 0; i < ALLOW_OPERATION_MAX_VLAN_NUM; ++i) {
        MSD_16 tempVid = s_vtuModuleInitType[devNum].vtuConfigInfo.portVlanMemberTagInfo.vidInfo[i].vid;
        if (tempVid != UNVALID_VID) {
            vlanInfo->vidInfo[index].isShow = s_vtuModuleInitType[devNum].vtuConfigInfo.portVlanMemberTagInfo.vidInfo[i].isShow;
            vlanInfo->vidInfo[index].vid = tempVid;
            for (size_t j = 0; j < MSD_MAX_SWITCH_PORTS; j++) {
                vlanInfo->vidInfo[index].memberTag[j] = s_vtuModuleInitType[devNum].vtuConfigInfo.portVlanMemberTagInfo.vidInfo[i].memberTag[j];
            }
            index++;
        }
        if ((MSD_U32)index == vlanInfo->vidInfoSize) break;
    }

}

MSD_STATUS deviceVlanModuleExportAllVlanAndMemberTag(IN MSD_U8 devNum, OUT VlanMemberTagAndEgressModeInfo* vlanInfo)
{
    MSD_STATUS ret = MSD_OK;
    CHECK_DEV_NUM_IS_CORRECT;
    if (vlanInfo == NULL) {
        MSD_DBG_ERROR(("device_vlan_module_export_all_vlan_and_member_tag failed,the vlan_info param is null!\n"));
        return MSD_BAD_PARAM;
    }
    deviceVlanModuleGetAllVlanInfo(devNum, vlanInfo);
    return ret;
}

MSD_STATUS deviceVlanModuleImportAllVlanAndMemberTag(IN MSD_U8 devNum, IN VlanMemberTagAndEgressModeInfo* vlanInfo)
{
	CHECK_DEV_NUM_IS_CORRECT;
    if (!checkEesVlanInfoIsValid(dev->numOfPorts, vlanInfo)) {
        return MSD_BAD_PARAM;
    }
    deviceVlanModuleCleanAllVlanAndMemberTagFromConfig(devNum, MSD_TRUE, MSD_TRUE);

    s_vtuModuleInitType[devNum].vtuConfigInfo.portVlanMemberTagInfo.vidInfoSize = vlanInfo->vidInfoSize;
    for (size_t i = 0; i < vlanInfo->vidInfoSize; ++i) {
        s_vtuModuleInitType[devNum].vtuConfigInfo.portVlanMemberTagInfo.vidInfo[i].vid = vlanInfo->vidInfo[i].vid;
        s_vtuModuleInitType[devNum].vtuConfigInfo.portVlanMemberTagInfo.vidInfo[i].isShow = vlanInfo->vidInfo[i].isShow;
        for (size_t j = 0; j < MSD_MAX_SWITCH_PORTS; j++) {
            s_vtuModuleInitType[devNum].vtuConfigInfo.portVlanMemberTagInfo.vidInfo[i].memberTag[j] = vlanInfo->vidInfo[i].memberTag[j];
        }
    }
    for (size_t i = 0; i < MSD_MAX_SWITCH_PORTS; i++) {
        s_vtuModuleInitType[devNum].vtuConfigInfo.portVlanMemberTagInfo.portVlanInfo[i].defaultVid = vlanInfo->portVlanInfo[i].defaultVid;
    }

    for (size_t i = 0; i < MSD_MAX_SWITCH_PORTS; i++) {
        s_vtuModuleInitType[devNum].vtuConfigInfo.portVlanMemberTagInfo.portVlanInfo[i].egressMode = vlanInfo->portVlanInfo[i].egressMode;
    }
    return MSD_OK;
}



MSD_STATUS deviceVlanModuleGetVlanConfigInfo(IN MSD_U8 devNum, OUT VlanConfigInfo* vlanConfigInfo)
{
    MSD_STATUS ret = MSD_OK;
    if (vlanConfigInfo == NULL) {
        MSD_DBG_ERROR(("device_vlan_module_get_vlan_config_info failed,the vlan_config_infi param is NULL!\n"));
        return MSD_BAD_PARAM;
    }
    CHECK_DEV_NUM_IS_CORRECT;
    msdMemSet(vlanConfigInfo, 0, sizeof(VlanConfigInfo));
    ret = deviceVlanModuleExportAllVlanAndMemberTag(devNum, &vlanConfigInfo->portVlanMemberTagInfo);
    vlanConfigInfo->qinqTpid = s_vtuModuleInitType[devNum].vtuConfigInfo.qinqTpid;
    vlanConfigInfo->vlanModel = s_vtuModuleInitType[devNum].vtuConfigInfo.vlanModel;
    return ret;
}

MSD_STATUS deviceVlanModuleRefreshVlanConfigInfo(IN MSD_U8 devNum, OUT VlanConfigInfo* vlanConfigInfo)
{
	CHECK_DEV_NUM_IS_CORRECT;
    MSD_STATUS ret = MSD_OK;
    if (vlanConfigInfo == NULL) {
        MSD_DBG_ERROR(("device_vlan_module_refresh_vlan_config_info failed,the vlan_config_infi param is NULL!\n"));
        return MSD_BAD_PARAM;
    }
    msdMemSet(vlanConfigInfo, 0, sizeof(VlanConfigInfo));
    if(!s_vtuModuleInitType[devNum].qModelIsGlobal){ //非全局模式
        MSD_DBG_ERROR(("device_vlan_module_refresh_vlan_config_info failed, the vlan model is not global model"));
        return MSD_FEATRUE_NOT_ALLOW;
    }
    deviceVlanModuleCleanAllVlanAndMemberTagFromConfig(devNum, MSD_TRUE, MSD_TRUE);


    for (size_t i = 0; i < MSD_MAX_SWITCH_PORTS; ++i) {
        vlanConfigInfo->portVlanMemberTagInfo.portVlanInfo[i].defaultVid = s_vtuModuleInitType[devNum].portVlanMemberTagInfo.portVlanInfo[i].defaultVid;
        vlanConfigInfo->portVlanMemberTagInfo.portVlanInfo[i].egressMode = s_vtuModuleInitType[devNum].portVlanMemberTagInfo.portVlanInfo[i].egressMode;
    }
    vlanConfigInfo->portVlanMemberTagInfo.vidInfoSize = s_vtuModuleInitType[devNum].portVlanMemberTagInfo.vidInfoSize;
    s_vtuModuleInitType[devNum].vtuConfigInfo.portVlanMemberTagInfo.vidInfoSize = s_vtuModuleInitType[devNum].portVlanMemberTagInfo.vidInfoSize;
    size_t index = 0;
    for (int i = 0; i < ALLOW_OPERATION_MAX_VLAN_NUM; ++i) {
        MSD_16 tempVid = s_vtuModuleInitType[devNum].portVlanMemberTagInfo.vidInfo[i].vid;
        if (tempVid != UNVALID_VID) {
            vlanConfigInfo->portVlanMemberTagInfo.vidInfo[index].vid = tempVid;
            s_vtuModuleInitType[devNum].vtuConfigInfo.portVlanMemberTagInfo.vidInfo[index].vid = tempVid;
            MSD_BOOL isShow = s_vtuModuleInitType[devNum].portVlanMemberTagInfo.vidInfo[i].isShow;
            vlanConfigInfo->portVlanMemberTagInfo.vidInfo[index].isShow = isShow;
            s_vtuModuleInitType[devNum].vtuConfigInfo.portVlanMemberTagInfo.vidInfo[index].isShow = isShow;
            for (size_t j = 0; j < MSD_MAX_SWITCH_PORTS; ++j) {
                MSD_PORT_MEMBER_TAG memberTag = s_vtuModuleInitType[devNum].portVlanMemberTagInfo.vidInfo[i].memberTag[j];
                vlanConfigInfo->portVlanMemberTagInfo.vidInfo[index].memberTag[j] = memberTag;
                s_vtuModuleInitType[devNum].vtuConfigInfo.portVlanMemberTagInfo.vidInfo[index].memberTag[j] = memberTag;
            }
            index++;
        }
        if (index == vlanConfigInfo->portVlanMemberTagInfo.vidInfoSize) {//检测完毕
            break;
        }
    }
    MSD_U16 eType;
    ret = msdPortEtherTypeByTypeGet(devNum, 1, MSD_ETHERTYPE_PROVIDER, &eType);//EES模式需要再全局模式下，因此Port 1的TPID就是所有端口的TPID。
    if (ret == MSD_OK) {
        vlanConfigInfo->qinqTpid = eType;
        s_vtuModuleInitType[devNum].vtuConfigInfo.qinqTpid = eType;
        vlanConfigInfo->vlanModel = s_vtuModuleInitType[devNum].globalQModel;
        s_vtuModuleInitType[devNum].vtuConfigInfo.vlanModel = s_vtuModuleInitType[devNum].globalQModel;
    }
    return ret;
}

MSD_STATUS deviceVlanModuleSaveVlanConfigInfo(IN MSD_U8 devNum)
{
	CHECK_DEV_NUM_IS_CORRECT;
    MSD_BOOL isGlobal = s_vtuModuleInitType[devNum].qModelIsGlobal;
    //需要以全局模式工作
    s_vtuModuleInitType[devNum].qModelIsGlobal = MSD_TRUE;
    MSD_STATUS ret = MSD_OK;
    do {
        VlanModel vlanModel = s_vtuModuleInitType[devNum].vtuConfigInfo.vlanModel;
        ret = deviceVlanModuleSetQMode(devNum, ALL_PORT_PARAM, vlanModel);//设置VLAN 模式
        if (ret != MSD_OK) break;
        MSD_U16 qinqTpid = s_vtuModuleInitType[devNum].vtuConfigInfo.qinqTpid;//?
        for (int i = 0; i < dev->numOfPorts; ++i) {
            MSD_U16 defaultVid = s_vtuModuleInitType[devNum].vtuConfigInfo.portVlanMemberTagInfo.portVlanInfo[i].defaultVid;
            ret = msdPortDefaultVlanIdSet(devNum, i, defaultVid);//设置端口的默认VID
            if (ret != MSD_OK) {
                break;
            }
            s_vtuModuleInitType[devNum].portVlanMemberTagInfo.portVlanInfo[i].defaultVid = defaultVid;

            ret = msdPortEtherTypeByTypeSet(devNum, i, MSD_ETHERTYPE_PROVIDER, qinqTpid);//设置端口的QinQ TPID值
            if (ret != MSD_OK) {
                break;
            }
        }
        if (ret != MSD_OK) break;
        //保存VID及其VLAN 出口方式
        ret = deviceVlanModuleSetVlanEgressModeAndMemberTagInfo(devNum, &s_vtuModuleInitType[devNum].vtuConfigInfo.portVlanMemberTagInfo);
        if (ret != MSD_OK) break;
    } while (0);
    s_vtuModuleInitType[devNum].qModelIsGlobal = isGlobal;//恢复之前的模式(全局或者端口设置)
    return ret;
}
