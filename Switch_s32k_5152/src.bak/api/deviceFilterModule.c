#include <apiInit.h>
#include <deviceFilterModule.h>
#include <deviceVlanModule.h>
#include <string.h>
#include <stdlib.h>
#include "adlist.h"

#define FILTER_UNUSED_NUM -1 //无效的过滤器编号
#define TCAM_UNUSED_NUM -1 //无效的TCAM ENtry 编号

typedef struct {
    MSD_U8 isEnableFilter : 1;//是否设置过滤(使用 1 bit 保存)
    MSD_U8 filterIndex : 7;/* 使用高7位保存 用以记录filter的序号，添加时会使用将+1，并作为添加的过滤器 编号 */
}FilterModuleInitType;

static FilterModuleInitType s_filterModuleInit[MAX_SOHO_DEVICES];

// 每个过滤器需要最多的tcam entry 条目，功能说明书说支持144 byte的tcam entry，但是api中好像仅支持96个 byte的tcam entry（但是开启qinq情况，需要双倍的tcam entry，即4个）
#define  MAX_FILTER_ENTRY_SIZE  4  //
/**
 * @brief 用以记录过滤器对象和tcam entry的对应关系的对象
 */
typedef struct{
    DeviceFilter filter;//过滤器对象
    MSD_U8 tcamPointerSize;//tcamPointer 的实际大小
    //按tcam entry id排序
    MSD_U32 tcamPointer[MAX_FILTER_ENTRY_SIZE];//tcam entry pointer: 一个filter最多需要6个entry，当开启匹配144个字节时，并且启用QinQ时（需要同时使用frameType为0x2和0x0的条目）。
}FilterEntry;

static list *s_filters[MAX_SOHO_DEVICES] = {NULL};//存放所有的过滤器

static void deviceFilterFreeFilterNode(void *value){
    FilterEntry *entry = (FilterEntry *) value;
    vPortFree(entry);
    entry = NULL;
}

MSD_STATUS deviceFilterModuleInitial(void)
{
    msdMemSet(&s_filterModuleInit, 0, sizeof(FilterModuleInitType) * MAX_SOHO_DEVICES);
    for (MSD_U8 i = 0; i < MAX_SOHO_DEVICES; ++i) {
    	s_filterModuleInit[i].isEnableFilter = 0;
    	s_filterModuleInit[i].filterIndex = 0;
    }
    return MSD_OK;
}

MSD_STATUS deviceFilterModuleInitialFilterInfo(MSD_U8 devNum)
{
    MSD_STATUS ret = MSD_OK;
    ret = deviceFilterModuleSetIsEnableFilter(devNum, MSD_TRUE);//
    if (ret != MSD_OK) return ret;
    ret = msdTcamAllDelete(devNum);
    return ret;
}

MSD_STATUS deviceFilterModuleSetIsEnableFilter(MSD_U8 devNum, MSD_BOOL isEnableFilter)
{
	CHECK_DEV_NUM_IS_CORRECT;
    MSD_STATUS ret = MSD_OK;
	if (isEnableFilter != MSD_FALSE && isEnableFilter != MSD_TRUE) {
		isEnableFilter = MSD_TRUE;
	}
    MSD_BOOL isVlanGlobal;
    deviceVlanModuleGetQModeIsGlobal(devNum, &isVlanGlobal);//目前需要vlan是全局模式下才能使用该功能
    if(!isVlanGlobal){
        MSD_DBG_ERROR(("device_filter_module_set_is_enable_filter failed,the vlan module should be global!\n"));
        return MSD_FEATRUE_NOT_ALLOW;
    }
    MSD_TCAM_MODE tcamMode = MSD_TCAM_MODE_ENABLE_96;
    if (!isEnableFilter) {
        tcamMode = MSD_TCAM_MODE_DISABLE;
    }
    for (int i = 0; i < dev->numOfPorts; ++i) {
        ret = msdPortTcamModeSet(devNum, i, tcamMode);
        if (ret != MSD_OK){
            break;
        }
    }
    if (ret == MSD_OK) {//设置成功
        s_filterModuleInit[devNum].isEnableFilter = (MSD_U8)isEnableFilter;
        if(isEnableFilter){
        	s_filters[devNum] = listCreate();
			s_filters[devNum]->free = deviceFilterFreeFilterNode;
			s_filterModuleInit[devNum].filterIndex = 0;
        }else{
        	if(s_filters[devNum] != NULL){
        		listRelease(s_filters[devNum]);
        		s_filters[devNum] = NULL;
        	}
        }
    }
    return ret;
}

MSD_STATUS deviceFilterModuleGetIsEnableFilter(MSD_U8 devNum, MSD_BOOL* isEnableFilter)
{
	CHECK_DEV_NUM_IS_CORRECT;
    MSD_STATUS ret = MSD_OK;
    *isEnableFilter = (MSD_BOOL)s_filterModuleInit[devNum].isEnableFilter;
    MSD_TCAM_MODE tcamMode,tempTcamMode;
    if (!*isEnableFilter) {
        tcamMode = MSD_TCAM_MODE_DISABLE;
    }
    else {
        tcamMode = MSD_TCAM_MODE_ENABLE_96;
    }
    for (int i = 0; i < dev->numOfPorts; ++i) {
        ret = msdPortTcamModeGet(devNum, (MSD_LPORT)i, &tempTcamMode);
        if (ret != MSD_OK)
            break;
        if (tempTcamMode != tcamMode) {//模式不对，重新设置端口的TCAM
            ret = msdPortTcamModeSet(devNum, (MSD_LPORT)i, tcamMode);
            if (ret != MSD_OK)
                break;
        }
    }
    return ret;
}

/****************************************************************************************************
 * @brief deviceFilterGetUnusedTcamPointers 找到没有使用的tcam entry条目
 * @param devNum
 * @param tcam_pointer
 * @param tcamPointerSize 需要的tcma entry条数
 * @param isFound 如果找到指定未使用的tcam entry条目数，设置为msd_true
 * @return
 ****************************************************************************************************/
static MSD_STATUS deviceFilterGetUnusedTcamPointers(MSD_U8 devNum, MSD_U32 *tcam_pointer, int tcamPointerSize, MSD_BOOL *isFound)
{
    *isFound = MSD_FALSE;
    int count = 0;
    msdMemSet(tcam_pointer,0,sizeof(MSD_U32) * tcamPointerSize);
    MSD_TCAM_DATA data;
    MSD_STATUS status = MSD_OK;
    int size = FILTER_MAX_NUM * MAX_FILTER_ENTRY_SIZE;//最大过滤器 * 一个过滤器可能最多需要的tcam entry条目
    for (int i = 0; i < size;  ++i) {
        status = msdTcamEntryRead(devNum, i, &data);
        if (status != MSD_OK) break;
        if (data.frameTypeMask == 0x0) {//没有使用的tcam entry
            tcam_pointer[count++] = i;
            if(count == tcamPointerSize){
                *isFound = MSD_TRUE;
                break;
            }
        }
    }
    return status;
}


/**************************************************************************************************************************************************
 * @brief setDeviceFilterAllTcamPointer
 * 设置某个过滤器需要的tcam entry(均考虑到启用Qinq的情况，如果带标签帧(帧的TPID为寄存器adType的值)的时候，会匹配frameType为0x2的TCAM,否则就使用frameType为0x0的情况)
 * @param devNum  设备编号
 * @param filterEntry  过滤器条目
 * @param needTcamPointer 需要的TCAM Entry条目列表
 * @param needTcamPointerSize  need_tcam_pointer的大小
 * @param needTcamPointerCount 过滤器需要的TCAM Entry条数
 * @param releaseTcamPointer 需要释放的TCAM Entry条目列表
 * @param releaseTcamPointerSize release_tcam_pointer_size的大小
 * @param releaseTcamPointerCount 过滤器需要释放的TCAM Entry条数
 * @param ftype 过滤帧的类型
 * @param isNeedCheckQinqAndNotQinq 是否需要同时检测Qinq或者非QinQ标签帧
 * @return MSD_OK:success, others:error
 **************************************************************************************************************************************************/
static MSD_STATUS setDeviceFilterAllTcamPointer(MSD_U8 devNum, FilterEntry *filterEntry,
                                                MSD_U32 *needTcamPointer, int needTcamPointerSize,int *needTcamPointerCount,
                                                MSD_U32 *releaseTcamPointer, int releaseTcamPointerSize,int *releaseTcamPointerCount,
                                                FilterType ftype, MSD_BOOL isNeedCheckQinqAndNotQinq)
{
    msdMemSet(needTcamPointer,0,needTcamPointerSize * sizeof(MSD_U32));
    msdMemSet(releaseTcamPointer,0,releaseTcamPointerSize * sizeof(MSD_U32));
    *needTcamPointerCount = 0;
    *releaseTcamPointerCount = 0;
    MSD_STATUS ret = MSD_OK;
    int needTcamSize = 0;//
    int base = isNeedCheckQinqAndNotQinq ? 2:1;//如果需要检测非qinq和qinq，则需要2倍的TCAM ENTRY
    if(filterEntry != NULL){//修改过滤器的情况
        MSD_U8 tcamEntrySize = 0;
        tcamEntrySize = filterEntry->tcamPointerSize;//
        //有可能为负数，比如之前匹配使用了2个TCAM Entry，但是本次仅仅使用了一个，则为-1.即需要释放的条目
        if(ftype == FILTER_TYPE_ALL){
            //在qinq模式下，如果帧带有双标签，则需要tcam entry的frameType = 0x2,同时如果帧不带双标签，则frameType=0x0.因此我们这里统一设置为需要2个TCAM Entry
            needTcamSize = 2 - tcamEntrySize;

        }else if(ftype == FILTER_TYPE_IP_TCP_OR_UDP){// filter ip tcp/udp packet
            //如果要仅仅匹配帧的前48个字节，需要一个TCAM Entry足矣，EtherType:18 0ct(include vlan tag),IPv4 Header:20 byte(但是如果包括可选字段（options），那么头部的大小可以增加到60字节。)
            //IPv6 Header:40 byte,TCP header:20 byte, UDP HEADER:8 byte,但是msdTcamAdvConfig需要指定2个tcam entry编号,即使是ipv4 udp(最小为18 + 20 + 8 = 46)使用2个tcam entry
            //其余使用2个entry，但是可能实际就使用一个tcam entry.(qinq 需要考虑*2的情况)
            needTcamSize = 2 * base - tcamEntrySize;
        }else if(ftype == FILTER_TYPE_ARP){ //filter arp packet
            needTcamSize = base - tcamEntrySize; // arp packet use only one tcam entry
        }else if(ftype == FILTER_TYPE_SECOND_LAYER){//
            needTcamSize = base - tcamEntrySize;
        }
        else{//其他情况，暂不考虑

        }
        if(needTcamSize == 0){//不增不减的情况，就使用当前的tcam entry
            for(int i = 0; i < tcamEntrySize; ++i){ //重新赋值即可
                needTcamPointer[i] = filterEntry->tcamPointer[i];
            }
        }else if(needTcamSize > 0){//需要从未使用的tcam entry中获取
            for(int i = 0; i < tcamEntrySize; ++i){ //
                needTcamPointer[i] = filterEntry->tcamPointer[i];
            }
            MSD_BOOL isFound;
            ret = deviceFilterGetUnusedTcamPointers(devNum, needTcamPointer,needTcamSize,&isFound);
            if(ret != MSD_OK || !isFound){//无法获取到条目
                return MSD_FAIL;
            }
        }else{ //需要释放对应的tcam entry
            int releaseSize = -needTcamSize;//释放的条目数
            *releaseTcamPointerCount = releaseSize;
            int needSize = tcamEntrySize - releaseSize;//保留的条目
            //保留序号小的TCAM ENTRY
            for(int i = 0; i < needSize; ++i){
                needTcamPointer[i] = filterEntry->tcamPointer[i];
            }
            //释放序号大的TCAM ENTRY
            for(int i = 0; i < releaseSize; ++i){
                releaseTcamPointer[i] = filterEntry->tcamPointer[i + needSize];
            }
        }
        *needTcamPointerCount = tcamEntrySize + needTcamSize;
    }else{//新增过滤器,需要从未使用的tcam entry中获取
        if(ftype == FILTER_TYPE_ALL){
            //在qinq模式下，如果帧带有双标签，则需要tcam entry的frameType = 0x2,同时如果帧不带双标签，则frameType=0x0.因此我们这里统一设置为需要2个TCAM Entry
            needTcamSize = 2;

        }else if(ftype == FILTER_TYPE_IP_TCP_OR_UDP){//
            //如果要仅仅匹配帧的前48个字节，需要一个TCAM Entry足矣，EtherType:18 0ct(include vlan tag),IPv4 Header:20 byte(但是如果包括可选字段（options），那么头部的大小可以增加到60字节。)
            //IPv6 Header:40 byte,TCP header:20 byte, UDP HEADER:8 byte,但是msdTcamAdvConfig需要指定2个tcam entry编号,即使是ipv4 udp(最小为18 + 20 + 8 = 46)使用2个tcam entry
            //其余使用2个entry，但是可能实际就使用一个tcam entry.(qinq 需要考虑*2的情况)
            needTcamSize = 2 * base;
        }else if(ftype == FILTER_TYPE_ARP){//filter arp
            needTcamSize = base;
        }else if(ftype == FILTER_TYPE_SECOND_LAYER){
            needTcamSize = base;
        }
        else{//其他情况，暂不考虑

        }
        MSD_BOOL is_found;
        ret = deviceFilterGetUnusedTcamPointers(devNum, needTcamPointer,needTcamSize,&is_found);
        if(ret != MSD_OK || !is_found){//无法获取到条目
            return MSD_FAIL;
        }
        *needTcamPointerCount = needTcamSize;
    }
    return ret;
}

/************************************************************************************************************************
 * @brief setEgressPortsAndFilterType
 * 将入口动作拷贝到相应的TCAM条目中
 * 目前仅做设置帧在端口上mirror(Drop,Force)操作。如果需要设置其他入口动作，比如修改VID等，可以继续添加（需要添加device_ingress_action_filter_t的相应参数）
 * @param isTcamEntry: 是tcam entry 还是action entry
 * @param tcam_data_pram: TCAM Entry or action entry data param
 * @param egressPorts  该过滤的出口端口列表bit
 * @param eType 出口端口时使用的出口动作
 ************************************************************************************************************************/
static void setEgressPortsAndFilterType(MSD_BOOL isTcamEntry, INOUT void *tcamDataParam, IN MSD_U16 egressPorts, IN EgressType eType)
{
    if(isTcamEntry){
        MSD_TCAM_DATA *tcamData = (MSD_TCAM_DATA *)tcamDataParam;
        tcamData->dpvData = (MSD_U32)egressPorts;
        if (eType == EGRESS_TYPE_MIRROR) { //mirror到指定端口列表
            tcamData->dpvMode = MSD_TCAM_DPV_MODE_OR_DPV;
        }
        else if (eType == EGRESS_TYPE_FORCE_EGRESS_PORT) {//强制将帧出口到指定的端口列表
            tcamData->dpvMode = MSD_TCAM_DPV_MODE_OVERRIDE_DPV;
        }
        else if (eType == EGRESS_TYPE_DROP) {//指定的端口列表会丢弃满足条件的帧
            tcamData->dpvMode = MSD_TCAM_DPV_MODE_AND_DPV;
            tcamData->dpvData = ~tcamData->dpvData;//设置的端口Dpv设置为0.
        }
        else { //Avoid future filter(出口不做任何过滤)
            tcamData->dpvMode = MSD_TCAM_DPV_MODE_DO_NOTHING;
        }
    }else{
        MSD_TCAM_ENT_ACT* tcamData = (MSD_TCAM_ENT_ACT*)tcamDataParam;
        //设置DPV数据和方式（可用于设置egress到哪些端口port）
        tcamData->dpvData = (MSD_U32)egressPorts;
        if (eType == EGRESS_TYPE_MIRROR) { //mirror到指定端口列表
            tcamData->dpvMode = MSD_TCAM_DPV_MODE_OR_DPV;
        }
        else if (eType == EGRESS_TYPE_FORCE_EGRESS_PORT) {//强制将帧出口到指定的端口列表
            tcamData->dpvMode = MSD_TCAM_DPV_MODE_OVERRIDE_DPV;
        }
        else if (eType == EGRESS_TYPE_DROP) {//指定的端口列表会丢弃满足条件的帧
            tcamData->dpvMode = MSD_TCAM_DPV_MODE_AND_DPV;
            tcamData->dpvData = ~tcamData->dpvData;//设置的端口Dpv设置为0.
        }
        else { //Avoid future filter
            tcamData->dpvMode = MSD_TCAM_DPV_MODE_DO_NOTHING;
        }
    }
}

static void copyIpv4Header(MSD_TCAM_ENT_IPV4_UNT* dest, MSD_TCAM_ENT_IPV4_UNT* src) {
    dest->version = src->version;
    dest->ihl = src->ihl;
    dest->tos = src->tos;
    dest->length = src->length;
    dest->id = src->id;
    dest->flag = src->flag;
    dest->fragOffset = src->fragOffset;
    dest->ttl = src->ttl;
    dest->protocol = src->protocol;
    dest->hdrChkSum = src->hdrChkSum;
    for (int i = 0; i < 4; ++i) {
        dest->sip[i] = src->sip[i];
        dest->dip[i] = src->dip[i];
    }
}

static void copyTcpHeader(MSD_TCAM_ENT_TCP_UNT* dest, MSD_TCAM_ENT_TCP_UNT* src)
{
    dest->srcPort = src->srcPort;
    dest->destPort = src->destPort;
    dest->seqNum = src->seqNum;
    dest->ackNum = src->ackNum;
    dest->offset = src->offset;
    dest->flags = src->flags;
    dest->windowSize = src->windowSize;
    dest->chkSum = src->chkSum;
    dest->urgPtr = src->urgPtr;
}

static void copyUdpHeader(MSD_TCAM_ENT_UDP_UNT* dest, MSD_TCAM_ENT_UDP_UNT* src) {
    dest->chkSum = src->chkSum;
    dest->destPort = src->destPort;
    dest->length = src->length;
    dest->srcPort = src->srcPort;
}


static void copyIpv6Header(MSD_TCAM_ENT_IPV6_UNT* dest, MSD_TCAM_ENT_IPV6_UNT* src)
{
    for (int i = 0; i < 8; i++) {
        dest->dip[i] = src->dip[i];
        dest->sip[i] = src->sip[i];
    }
    dest->version = src->version;
    dest->tc = src->tc;
    dest->flowLbl = src->flowLbl;
    dest->payloadLen = src->payloadLen;
    dest->nextHdr = src->nextHdr;
    dest->hopLimit = src->hopLimit;
}


static MSD_BOOL copyIpv4TcpPktFormatContent(MSD_TCAM_ENT_IPV4_TCP_UNT *src, MSD_TCAM_ENT_IPV4_TCP_UNT *dest)
{
    copyIpv4Header(&dest->ipv4, &src->ipv4);
    copyTcpHeader(&dest->tcp, &src->tcp);
    return MSD_TRUE;
}


static MSD_BOOL copyIpv4UdpPktFormatContent(MSD_TCAM_ENT_IPV4_UDP_UNT* src, MSD_TCAM_ENT_IPV4_UDP_UNT* dest)
{
    copyIpv4Header(&dest->ipv4, &src->ipv4);
    copyUdpHeader(&dest->udp, &src->udp);
    return MSD_TRUE;
}

static MSD_BOOL copyIpv6TcpPktFormatContent(MSD_TCAM_ENT_IPV6_TCP_UNT* src, MSD_TCAM_ENT_IPV6_TCP_UNT* dest)
{
    copyIpv6Header(&dest->ipv6, &src->ipv6);
    copyTcpHeader(&dest->tcp, &src->tcp);
    return MSD_TRUE;
}

static MSD_BOOL copyIpv6UdpPktFormatContent(MSD_TCAM_ENT_IPV6_UDP_UNT* src, MSD_TCAM_ENT_IPV6_UDP_UNT* dest)
{
    copyIpv6Header(&dest->ipv6, &src->ipv6);
    copyUdpHeader(&dest->udp, &src->udp);
    return MSD_TRUE;
}

//MSD_TCAM_ENT_PKT_FORMAT_UNT
static void handleIpTcpOrUdpPktFormatContent(OUT MSD_TCAM_ENT_PKT_FORMAT_UNT* dest,IN MSD_TCAM_ENT_PKT_FORMAT_UNT* src,IN MSD_TCAM_PKT_TYPE pktType)
{
    switch (pktType) {
    case MSD_TCAM_TYPE_IPV4_TCP:
        copyIpv4TcpPktFormatContent(&src->ipv4Tcp, &dest->ipv4Tcp);
        break;
    case MSD_TCAM_TYPE_IPV4_UDP:
        copyIpv4UdpPktFormatContent(&src->ipv4Udp, &dest->ipv4Udp);
        break;
    case MSD_TCAM_TYPE_IPV6_TCP:
        copyIpv6TcpPktFormatContent(&src->ipv6Tcp, &dest->ipv6Tcp);
        break;
    case MSD_TCAM_TYPE_IPV6_UDP:
        copyIpv6UdpPktFormatContent(&src->ipv6Udp, &dest->ipv6Udp);
        break;
    case MSD_TCAM_TYPE_ONLY_IPV4:
        copyIpv4Header(&dest->ipv4Only, &src->ipv4Only);
        break;
    case MSD_TCAM_TYPE_ONLY_IPV6:
        copyIpv6Header(&dest->ipv6Only, &src->ipv6Only);
        break;
    }
}
/********************************************************************************************************************************
 * @brief addOrModifyFilterEntryToList 添加或者修改过滤条目到列表中
 * @param filterEntry:过滤条目
 * @param devNum:设备编号
 * @param filterId:过滤id
 * @param filterName:过滤名称
 * @param ingressPortVecBit 入口端口bit集合
 * @param etype 出口类型
 * @param egressPortVecBit 出口端口bit集合
 * @param ftype 过滤类型
 * @param filterParam 过滤参数
 * @param tcamEntryPointers:tcam条目num编号
 * @param tcamEntrySize:tcamEntryPointers 的大小
 * @return
 *
 *********************************************************************************************************************************/
static MSD_BOOL addOrModifyFilterEntryToList(FilterEntry *filterEntry,MSD_U8 devNum,MSD_U8 filterId,
                                                    const char *filterName, MSD_U16 ingressPortVecBit,
                                                    EgressType etype,MSD_U16 egressPortVecBit, FilterType ftype, FilterParam *filterParam,
                                                    MSD_U32 *tcamEntryPointers,MSD_U8 tcamEntrySize)
{
    FilterEntry *tempFilterEntry = NULL;
    if(filterEntry == NULL){//add filter
        tempFilterEntry = (FilterEntry *)pvPortMalloc(sizeof(FilterEntry));
        if(tempFilterEntry == NULL){
        	return MSD_FALSE;
        }
    }else{//modify filter
        tempFilterEntry = filterEntry;
    }
    tempFilterEntry->filter.filterId = filterId;
    msdMemSet(tempFilterEntry->filter.filterName,0,sizeof(tempFilterEntry->filter.filterName));
    memcpy(tempFilterEntry->filter.filterName,filterName,strlen(filterName));
    tempFilterEntry->filter.egressPortVecBit = egressPortVecBit;
    tempFilterEntry->filter.ingressPortVecBit = ingressPortVecBit;
    tempFilterEntry->filter.etype = etype;
    if(ftype != FILTER_TYPE_ALL){
        tempFilterEntry->filter.filterParam.secondLayerParam = filterParam->secondLayerParam;
        tempFilterEntry->filter.filterParam.thirdAndAboveLayerParam = filterParam->thirdAndAboveLayerParam;
    }
    tempFilterEntry->filter.ftype = ftype;
    tempFilterEntry->tcamPointerSize = tcamEntrySize;
    msdMemSet(tempFilterEntry->tcamPointer,0,sizeof(tempFilterEntry->tcamPointer));
    for(int i = 0; i < tcamEntrySize; ++i){
        tempFilterEntry->tcamPointer[i] = tcamEntryPointers[i];
    }
    if(filterEntry == NULL){ //add
        if(listAddNodeHead(s_filters[devNum],tempFilterEntry) != NULL)
        	return MSD_TRUE;
        return MSD_FALSE;
    }
    return MSD_TRUE;
}


static MSD_BOOL compareIpv4Field(MSD_TCAM_ENT_IPV4_UNT *p1, MSD_TCAM_ENT_IPV4_UNT *p2)
{
    return (
        p1->version == p2->version &&
        p1->ihl == p2->ihl &&
        p1->tos == p2->tos &&
        p1->length == p2->length &&
        p1->id == p2->id &&
        p1->flag == p2->flag &&
        p1->fragOffset == p2->fragOffset &&
        p1->ttl == p2->ttl &&
        p1->protocol == p2->protocol &&
        p1->hdrChkSum == p2->hdrChkSum &&
        memcmp(p1->sip, p2->sip, 4) == 0 &&
        memcmp(p1->dip, p2->dip, 4) == 0
        );
}

static MSD_BOOL compareIpv6Field(MSD_TCAM_ENT_IPV6_UNT *p1, MSD_TCAM_ENT_IPV6_UNT *p2)
{
    return (
        p1->version == p2->version &&
        p1->tc == p2->tc &&
        p1->flowLbl == p2->flowLbl &&
        p1->payloadLen == p2->payloadLen &&
        p1->nextHdr == p2->nextHdr &&
        p1->hopLimit == p2->hopLimit &&
        memcmp(p1->sip, p2->sip, 8) == 0 &&
        memcmp(p1->dip, p2->dip, 8) == 0);
}


static MSD_BOOL compareTcpField(MSD_TCAM_ENT_TCP_UNT *a, MSD_TCAM_ENT_TCP_UNT *b){
    if(a->srcPort!= b->srcPort
        || a->destPort!= b->destPort
        || a->seqNum!= b->seqNum
        || a->ackNum!= b->ackNum
        || a->offset!= b->offset
        || a->flags!= b->flags
        || a->windowSize!= b->windowSize
        || a->chkSum!= b->chkSum
        || a->urgPtr!= b->urgPtr)
        return MSD_FALSE;
    else
        return MSD_TRUE;
}

static MSD_BOOL compareUdpField(MSD_TCAM_ENT_UDP_UNT *a, MSD_TCAM_ENT_UDP_UNT *b)
{
    return (a->srcPort == b->srcPort)
    && (a->destPort == b->destPort)
        && (a->length == b->length)
        && (a->chkSum == b->chkSum);
}



static MSD_BOOL compareArpFilterField(DeviceFilterArpFilter *filter1, DeviceFilterArpFilter *filter2)
{
    if(filter1->operation != filter2->operation) return MSD_FALSE;
    if(memcmp(filter1->senderMacAddressData, filter2->senderMacAddressData, 6) != 0) return MSD_FALSE;
    if(memcmp(filter1->senderMacAddressMask, filter2->senderMacAddressMask, 6) != 0) return MSD_FALSE;
    if(memcmp(filter1->targetMacAddressData, filter2->targetMacAddressData, 6) != 0) return MSD_FALSE;
    if(memcmp(filter1->targetMacAddressMask, filter2->targetMacAddressMask, 6) != 0) return MSD_FALSE;
    if(memcmp(filter1->senderIpAddressData, filter2->senderIpAddressData, 4) != 0) return MSD_FALSE;
    if(memcmp(filter1->senderIpAddressMask, filter2->senderIpAddressMask, 4) != 0) return MSD_FALSE;
    if(memcmp(filter1->targetIpAddressData, filter2->targetIpAddressData, 4) != 0) return MSD_FALSE;
    if(memcmp(filter1->targetIpAddressMask, filter2->targetIpAddressMask, 4) != 0) return MSD_FALSE;
    return MSD_TRUE;
}

//memcmp:如果一字节对齐，可以直接使用该函数比较,这里逐个字段比较
MSD_BOOL deviceFilterModuleIsEqualFilterParam(FilterParam *param1, FilterParam *param2,FilterType filterType)
{
    //如果要检测的二层字段不同，则认为过滤器不同
    if(param1->secondLayerParam.checkEtherFlag != param2->secondLayerParam.checkEtherFlag){
        return MSD_FALSE;
    }
    if(param1->secondLayerParam.checkEtherFlag != 0){//先判断是否需要检测二层字段
        if(param1->secondLayerParam.checkEtherFlag & CHECK_ETHER_DEST_MAC_FLAG){//仅仅在需要检测DEST MAC字段的时候比较
            for(int i =0; i < 6; ++i){
                if(param1->secondLayerParam.destMacData[i] != param2->secondLayerParam.destMacData[i]){
                    return MSD_FALSE;
                }
                if(param1->secondLayerParam.destMacMask[i] != param2->secondLayerParam.destMacMask[i]){
                    return MSD_FALSE;
                }
            }
        }
        if(param1->secondLayerParam.checkEtherFlag & CHECK_ETHER_SRC_MAC_FLAG){//仅仅在需要检测SRC MAC字段的时候比较
            for(int i =0; i < 6; ++i){
                if(param1->secondLayerParam.srcMacData[i] != param2->secondLayerParam.srcMacData[i]){
                    return MSD_FALSE;
                }
                if(param1->secondLayerParam.srcMacMask[i] != param2->secondLayerParam.srcMacMask[i]){
                    return MSD_FALSE;
                }
            }
        }
        if(param1->secondLayerParam.checkEtherFlag & CHECK_ETHER_QINQ_VLAN_FLAG){
            if(param1->secondLayerParam.checkEtherFlag & CHECK_ETHER_QINQ_VLAN_DEI_FLAG){
                if(param1->secondLayerParam.qinqVlanDeiData !=  param2->secondLayerParam.qinqVlanDeiData){
                    return MSD_FALSE;
                }
                if(param1->secondLayerParam.qinqVlanDeiMask !=  param2->secondLayerParam.qinqVlanDeiMask){
                    return MSD_FALSE;
                }
            }
            if(param1->secondLayerParam.checkEtherFlag & CHECK_ETHER_QINQ_VLAN_PCP_FLAG){
                if(param1->secondLayerParam.qinqVlanPcpData !=  param2->secondLayerParam.qinqVlanPcpData){
                    return MSD_FALSE;
                }
                if(param1->secondLayerParam.qinqVlanPcpMask !=  param2->secondLayerParam.qinqVlanPcpMask){
                    return MSD_FALSE;
                }
            }
            if(param1->secondLayerParam.checkEtherFlag & CHECK_ETHER_QINQ_VLAN_VID_FLAG){
                if(param1->secondLayerParam.qinqVlanIdData !=  param2->secondLayerParam.qinqVlanIdData){
                    return MSD_FALSE;
                }
                if(param1->secondLayerParam.qinqVlanIdMask !=  param2->secondLayerParam.qinqVlanIdMask){
                    return MSD_FALSE;
                }
            }
        }
        if(param1->secondLayerParam.checkEtherFlag & CHECK_ETHER_VLAN_FLAG){

            if(param1->secondLayerParam.checkEtherFlag & CHECK_ETHER_VLAN_DEI_FLAG){
                if(param1->secondLayerParam.vlanDeiData !=  param2->secondLayerParam.vlanDeiData){
                    return MSD_FALSE;
                }
                if(param1->secondLayerParam.vlanDeiMask  !=  param2->secondLayerParam.vlanDeiMask ){
                    return MSD_FALSE;
                }
            }
            if(param1->secondLayerParam.checkEtherFlag & CHECK_ETHER_VLAN_PCP_FLAG){
                if(param1->secondLayerParam.vlanPcpData !=  param2->secondLayerParam.vlanPcpData){
                    return MSD_FALSE;
                }
                if(param1->secondLayerParam.vlanPcpMask  !=  param2->secondLayerParam.vlanPcpMask ){
                    return MSD_FALSE;
                }
            }
            if(param1->secondLayerParam.checkEtherFlag & CHECK_ETHER_VLAN_VID_FLAG){
                if(param1->secondLayerParam.vlanIdData !=  param2->secondLayerParam.vlanIdData){
                    return MSD_FALSE;
                }
                if(param1->secondLayerParam.vlanIdMask  !=  param2->secondLayerParam.vlanIdMask ){
                    return MSD_FALSE;
                }
            }
        }
    }
    if(filterType == FILTER_TYPE_IP_TCP_OR_UDP){ //ip tcp/udp
        if(param1->thirdAndAboveLayerParam.ipTcpOrUdpFilter.pktType != param2->thirdAndAboveLayerParam.ipTcpOrUdpFilter.pktType){
            return MSD_FALSE;
        }
        if(param1->thirdAndAboveLayerParam.ipTcpOrUdpFilter.pktType == MSD_TCAM_TYPE_IPV4_TCP){
            MSD_TCAM_ENT_IPV4_UNT *ipv4_param1_data = &param1->thirdAndAboveLayerParam.ipTcpOrUdpFilter.data.ipv4Tcp.ipv4;
            MSD_TCAM_ENT_IPV4_UNT *ipv4_param2_data = &param2->thirdAndAboveLayerParam.ipTcpOrUdpFilter.data.ipv4Tcp.ipv4;
            MSD_TCAM_ENT_IPV4_UNT *ipv4_param1_mask = &param1->thirdAndAboveLayerParam.ipTcpOrUdpFilter.mask.ipv4Tcp.ipv4;
            MSD_TCAM_ENT_IPV4_UNT *ipv4_param2_mask = &param2->thirdAndAboveLayerParam.ipTcpOrUdpFilter.mask.ipv4Tcp.ipv4;
            MSD_TCAM_ENT_TCP_UNT *tcp_param1_data = &param1->thirdAndAboveLayerParam.ipTcpOrUdpFilter.data.ipv4Tcp.tcp;
            MSD_TCAM_ENT_TCP_UNT *tcp_param2_data = &param2->thirdAndAboveLayerParam.ipTcpOrUdpFilter.data.ipv4Tcp.tcp;
            MSD_TCAM_ENT_TCP_UNT *tcp_param1_mask = &param1->thirdAndAboveLayerParam.ipTcpOrUdpFilter.mask.ipv4Tcp.tcp;
            MSD_TCAM_ENT_TCP_UNT *tcp_param2_mask = &param2->thirdAndAboveLayerParam.ipTcpOrUdpFilter.mask.ipv4Tcp.tcp;
            if(!compareIpv4Field(ipv4_param1_data,ipv4_param2_data)){
                return MSD_FALSE;
            }
            if(!compareIpv4Field(ipv4_param1_mask,ipv4_param2_mask)){
                return MSD_FALSE;
            }
            if(!compareTcpField(tcp_param1_data,tcp_param2_data)){
                return MSD_FALSE;
            }
            if(!compareTcpField(tcp_param1_mask,tcp_param2_mask)){
                return MSD_FALSE;
            }
        }else if(param1->thirdAndAboveLayerParam.ipTcpOrUdpFilter.pktType == MSD_TCAM_TYPE_IPV4_UDP){
            MSD_TCAM_ENT_IPV4_UNT *ipv4_param1_data = &param1->thirdAndAboveLayerParam.ipTcpOrUdpFilter.data.ipv4Udp.ipv4;
            MSD_TCAM_ENT_IPV4_UNT *ipv4_param2_data = &param2->thirdAndAboveLayerParam.ipTcpOrUdpFilter.data.ipv4Udp.ipv4;
            MSD_TCAM_ENT_IPV4_UNT *ipv4_param1_mask = &param1->thirdAndAboveLayerParam.ipTcpOrUdpFilter.mask.ipv4Udp.ipv4;
            MSD_TCAM_ENT_IPV4_UNT *ipv4_param2_mask = &param2->thirdAndAboveLayerParam.ipTcpOrUdpFilter.mask.ipv4Udp.ipv4;
            MSD_TCAM_ENT_UDP_UNT *udp_param1_data = &param1->thirdAndAboveLayerParam.ipTcpOrUdpFilter.data.ipv4Udp.udp;
            MSD_TCAM_ENT_UDP_UNT *udp_param2_data = &param2->thirdAndAboveLayerParam.ipTcpOrUdpFilter.data.ipv4Udp.udp;
            MSD_TCAM_ENT_UDP_UNT *udp_param1_mask = &param1->thirdAndAboveLayerParam.ipTcpOrUdpFilter.mask.ipv4Udp.udp;
            MSD_TCAM_ENT_UDP_UNT *udp_param2_mask = &param2->thirdAndAboveLayerParam.ipTcpOrUdpFilter.mask.ipv4Udp.udp;
            if(!compareIpv4Field(ipv4_param1_data,ipv4_param2_data)){
                return MSD_FALSE;//1
            }
            if(!compareIpv4Field(ipv4_param1_mask,ipv4_param2_mask)){
                return MSD_FALSE;
            }
            if(!compareUdpField(udp_param1_data,udp_param2_data)){
                return MSD_FALSE;
            }
            if(!compareUdpField(udp_param1_mask,udp_param2_mask)){
                return MSD_FALSE;
            }
        }else if(param1->thirdAndAboveLayerParam.ipTcpOrUdpFilter.pktType == MSD_TCAM_TYPE_IPV6_TCP){
            MSD_TCAM_ENT_IPV6_UNT *ipv6_param1_data = &param1->thirdAndAboveLayerParam.ipTcpOrUdpFilter.data.ipv6Tcp.ipv6;
            MSD_TCAM_ENT_IPV6_UNT *ipv6_param2_data = &param2->thirdAndAboveLayerParam.ipTcpOrUdpFilter.data.ipv6Tcp.ipv6;
            MSD_TCAM_ENT_IPV6_UNT *ipv6_param1_mask = &param1->thirdAndAboveLayerParam.ipTcpOrUdpFilter.mask.ipv6Tcp.ipv6;
            MSD_TCAM_ENT_IPV6_UNT *ipv6_param2_mask = &param2->thirdAndAboveLayerParam.ipTcpOrUdpFilter.mask.ipv6Tcp.ipv6;
            MSD_TCAM_ENT_TCP_UNT *tcp_param1_data = &param1->thirdAndAboveLayerParam.ipTcpOrUdpFilter.data.ipv6Tcp.tcp;
            MSD_TCAM_ENT_TCP_UNT *tcp_param2_data = &param2->thirdAndAboveLayerParam.ipTcpOrUdpFilter.data.ipv6Tcp.tcp;
            MSD_TCAM_ENT_TCP_UNT *tcp_param1_mask = &param1->thirdAndAboveLayerParam.ipTcpOrUdpFilter.mask.ipv6Tcp.tcp;
            MSD_TCAM_ENT_TCP_UNT *tcp_param2_mask = &param2->thirdAndAboveLayerParam.ipTcpOrUdpFilter.mask.ipv6Tcp.tcp;
            if(!compareIpv6Field(ipv6_param1_data,ipv6_param2_data)){
                return MSD_FALSE;
            }
            if(!compareIpv6Field(ipv6_param1_mask,ipv6_param2_mask)){
                return MSD_FALSE;
            }
            if(!compareTcpField(tcp_param1_data,tcp_param2_data)){
                return MSD_FALSE;
            }
            if(!compareTcpField(tcp_param1_mask,tcp_param2_mask)){
                return MSD_FALSE;
            }
        } else{
            MSD_TCAM_ENT_IPV6_UNT *ipv6_param1_data = &param1->thirdAndAboveLayerParam.ipTcpOrUdpFilter.data.ipv6Udp.ipv6;
            MSD_TCAM_ENT_IPV6_UNT *ipv6_param2_data = &param2->thirdAndAboveLayerParam.ipTcpOrUdpFilter.data.ipv6Udp.ipv6;
            MSD_TCAM_ENT_IPV6_UNT *ipv6_param1_mask = &param1->thirdAndAboveLayerParam.ipTcpOrUdpFilter.mask.ipv6Udp.ipv6;
            MSD_TCAM_ENT_IPV6_UNT *ipv6_param2_mask = &param2->thirdAndAboveLayerParam.ipTcpOrUdpFilter.mask.ipv6Udp.ipv6;
            MSD_TCAM_ENT_UDP_UNT *udp_param1_data = &param1->thirdAndAboveLayerParam.ipTcpOrUdpFilter.data.ipv6Udp.udp;
            MSD_TCAM_ENT_UDP_UNT *udp_param2_data = &param2->thirdAndAboveLayerParam.ipTcpOrUdpFilter.data.ipv6Udp.udp;
            MSD_TCAM_ENT_UDP_UNT *udp_param1_mask = &param1->thirdAndAboveLayerParam.ipTcpOrUdpFilter.mask.ipv6Udp.udp;;
            MSD_TCAM_ENT_UDP_UNT *udp_param2_mask = &param2->thirdAndAboveLayerParam.ipTcpOrUdpFilter.mask.ipv6Udp.udp;;
            if(!compareIpv6Field(ipv6_param1_data,ipv6_param2_data)){
                return MSD_FALSE;
            }
            if(!compareIpv6Field(ipv6_param1_mask,ipv6_param2_mask)){
                return MSD_FALSE;
            }
            if(!compareUdpField(udp_param1_data,udp_param2_data)){
                return MSD_FALSE;
            }
            if(!compareUdpField(udp_param1_mask,udp_param2_mask)){
                return MSD_FALSE;
            }
        }
    }else if(filterType == FILTER_TYPE_ARP){//arp
        return compareArpFilterField(&param1->thirdAndAboveLayerParam.arpFilter,&param2->thirdAndAboveLayerParam.arpFilter);
    }
    return MSD_TRUE;
}

/**
 * @brief searchFilterByName
 * list比较函数，根据filter_name，从filter_list查询是否存在指定的过滤器
 * @param filterParam
 * @param filterNameParam
 * @return 1-当前传入的filter为我们要查找的filter, 0-不是我们要查询的filter
 */
static inline int searchFilterByName(void *filterParam, void *filterNameParam)
{
    FilterEntry *filterEntry = (FilterEntry *)filterParam;
    char *filterName = (char *)filterNameParam;
    return !strcmp(filterEntry->filter.filterName,filterName);
}

/**
 * @brief searchFilterById 判断过滤器id判断过滤器是否存在
 * @param filterParam
 * @param idParam
 * @return 1- 从列表中查询成功，0-查询失败
 */
static inline int searchFilterById(void *filterParam, void *idParam)
{
    FilterEntry *filterEntry = (FilterEntry *)filterParam;
    MSD_U8 *fid = (MSD_U8*)idParam;
    return filterEntry->filter.filterId == (*fid);
}


/**
 * @brief searchFitlerByNqidAndEqname  从filter列表中查询是否存在相同的名称而filter_id不同的过滤器
 * @param filterParam 过滤器的每一个过滤器对象参数
 * @param findFilterParam 要查询的过滤器对象（封装查询的filter_id和filter_name）
 * @return 1- 从列表中查询成功，0-查询失败
 */
static int searchFitlerByNqidAndEqname(void *filterParam, void *findFilterParam)
{
    FilterEntry *filterEntry = (FilterEntry *)filterParam;
    FilterEntry *findEntry = (FilterEntry *)findFilterParam;
    int eqName = strcmp(filterEntry->filter.filterName,findEntry->filter.filterName);
    if((filterEntry->filter.filterId != findEntry->filter.filterId) && (eqName == 0)){//过滤器名称相同，id不同
        return 1;
    }
    return 0;
}

/**
 * @brief searchFilterByFilterParam
 * 判断入口过滤条件是否条件相同的条目是否存在，如果存在，则不添加过滤器或者修改过滤器
 * @param filterParam
 * @param findFilterParam
 * @return 列表中存在指定入口条件的，则返回1，否则返回0
 */
static int searchFilterByFilterParam(void *filterParam, void *findFilterParam)
{
    FilterEntry *filterEntry = (FilterEntry *)filterParam;
    FilterEntry *find_entry = (FilterEntry *)findFilterParam;
    FilterType filterType = find_entry->filter.ftype;
    if(filterEntry->filter.ftype != filterType){ //filter type is not the same
        return 0;
    }
    if(filterEntry->filter.ingressPortVecBit != find_entry->filter.ingressPortVecBit){
        return 0;
    }
    //filter all type is only compare the ingress port vec
    if(filterType != FILTER_TYPE_ALL){
        FilterParam *listParam = &filterEntry->filter.filterParam;
        FilterParam *findParam = &find_entry->filter.filterParam;
        return deviceFilterModuleIsEqualFilterParam(listParam,findParam,filterType);
    }
    return 1;
}

static MSD_STATUS checkDevNumAndIsEnable(MSD_U8 devNum)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (dev == NULL) {
        MSD_DBG_ERROR(("device_filter_module_add_filter failed,the dev is not open!\n"));
        return MSD_BAD_PARAM;
    }
    if(!s_filterModuleInit[devNum].isEnableFilter){//没有启用filter功能
        return MSD_FEATRUE_NOT_ALLOW;
    }
    return MSD_OK;
}

MSD_STATUS deviceFilterModuleAddFilter(IN MSD_U8 devNum, IN MSD_U8 filterId, const char *filterName, MSD_U16 ingressPortVecBit,
                                       EgressType etype, MSD_U16 egressPortVecBit, FilterType ftype, FilterParam *filterParam)
{
    //check the param is legal
    MSD_STATUS ret = checkDevNumAndIsEnable(devNum);
    if(ret != MSD_OK) return ret;
    if(filterName == NULL || strlen(filterName) == 0){
        return MSD_BAD_PARAM;
    }
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    MSD_U8 portCount = dev->numOfPorts;
    MSD_U32 portVecMax = (1 << portCount) - 1;//
    if(ingressPortVecBit < 2 || ingressPortVecBit > portVecMax){//如果小于2，代表仅有端口0或者没有端口被设置
        return MSD_BAD_PARAM;
    }
    if(etype < EGRESS_TYPE_MIRROR || etype > EGRESS_TYPE_AVOID_FURTHRE_FILTER){
        return MSD_BAD_PARAM;
    }
    if(etype != EGRESS_TYPE_AVOID_FURTHRE_FILTER){
        if(egressPortVecBit < 2 || egressPortVecBit > portVecMax){//如果小于2，代表仅有端口0或者没有端口被设置
            return MSD_BAD_PARAM;
        }
    }
    if(ftype < 0 || ftype > FILTER_TYPE_ALL){
        MSD_DBG_ERROR(("device_filter_module_add_filter failed,the ftype is not the correct param!\n"));
        return MSD_BAD_PARAM;
    }
    if(ftype != FILTER_TYPE_ALL && filterParam == NULL){
        MSD_DBG_ERROR(("device_filter_module_add_filter failed,if the ftype is not filter_type_all, the filter_param should be not null!\n"));
        return MSD_BAD_PARAM;
    }

    MSD_BOOL isCheckQinqVlan = MSD_FALSE;
    MSD_BOOL isCheckNotQinqVlan = MSD_FALSE;
    MSD_BOOL isCheckQinqPcp = MSD_FALSE;
    MSD_BOOL isCheckQinqDei = MSD_FALSE;
    MSD_BOOL isCheckQinqVid = MSD_FALSE;
    MSD_U8 qinqPcpData = 0;
    MSD_U8 qinqPcpMask = 0;
    MSD_BOOL qinqDeiData = MSD_FALSE;
    MSD_BOOL qinqDeiMask = MSD_FALSE;
    MSD_U16 qinqVidData = 0;
    MSD_U16 qinqVidMask = 0;

    MSD_BOOL isCheckVlan = MSD_FALSE; //is check vlan tag?
    MSD_BOOL isCheckNotVlan = MSD_FALSE;//is check no vlan tag ?
    MSD_U16 vlanTpidMask = 0xffff;//default value
    MSD_U16 vlanTpidData = 0x8100;//default tpid value
    MSD_BOOL vlanDeiMask = MSD_FALSE;//
    MSD_BOOL vlanDeiData = MSD_FALSE;
    MSD_U8  vlanPcpMask = 0x0; //
    MSD_U8  vlanPcpData = 0x0;
    MSD_U16 vlanVidMask = 0x000;//
    MSD_U16 vlanVidData = 0x000;//
    MSD_U32 vlanTagData = 0;
    MSD_U32 vlanTagMask = 0;
    if(ftype != FILTER_TYPE_ALL){
        if(filterParam->secondLayerParam.checkEtherFlag & CHECK_ETHER_QINQ_VLAN_FLAG){
            isCheckQinqVlan = MSD_TRUE;
        }
        if(filterParam->secondLayerParam.checkEtherFlag & CHECK_ETHER_NOT_QINQ_VLAN_FALG){
            isCheckNotQinqVlan = MSD_TRUE;
        }
        if(filterParam->secondLayerParam.checkEtherFlag & CHECK_ETHER_VLAN_FLAG){
            isCheckVlan = MSD_TRUE;
        }
        if(filterParam->secondLayerParam.checkEtherFlag & CHECK_ETHER_NOT_VLAN_FALG){
            isCheckNotVlan = MSD_TRUE;
        }
        if(isCheckQinqVlan){//只要检测QinQ标签帧(不管是否检测非Qinq)都要进行设置
            isCheckQinqPcp = (filterParam->secondLayerParam.checkEtherFlag & CHECK_ETHER_QINQ_VLAN_PCP_FLAG) == CHECK_ETHER_QINQ_VLAN_PCP_FLAG?MSD_TRUE:MSD_FALSE;//is check qinq vlan pcp
            isCheckQinqDei = (filterParam->secondLayerParam.checkEtherFlag & CHECK_ETHER_QINQ_VLAN_DEI_FLAG) == CHECK_ETHER_QINQ_VLAN_DEI_FLAG?MSD_TRUE:MSD_FALSE;// is check qinq vlan dei
            isCheckQinqVid = (filterParam->secondLayerParam.checkEtherFlag & CHECK_ETHER_QINQ_VLAN_VID_FLAG) == CHECK_ETHER_QINQ_VLAN_VID_FLAG?MSD_TRUE:MSD_FALSE;; //is check qinq vlan vid
            qinqPcpData = filterParam->secondLayerParam.qinqVlanPcpData; //
            qinqPcpMask = filterParam->secondLayerParam.qinqVlanPcpMask;
            qinqDeiData = filterParam->secondLayerParam.qinqVlanDeiData;
            qinqDeiMask = filterParam->secondLayerParam.qinqVlanDeiMask;
            qinqVidData = filterParam->secondLayerParam.qinqVlanIdData;
            qinqVidMask = filterParam->secondLayerParam.qinqVlanIdMask;
        }
        if(isCheckVlan && !isCheckNotVlan){//仅匹配标签帧，不匹配非标签帧
            ret = deviceVlanModuleGetTpid(devNum,1,MSD_ETHERTYPE_Q,&vlanTpidData);//检测tpid值字段应该设置为多少
            if(ret != MSD_OK) return ret;
            if(filterParam->secondLayerParam.checkEtherFlag & CHECK_ETHER_VLAN_PCP_FLAG){//check inner vlan tpid
                vlanPcpMask = filterParam->secondLayerParam.vlanPcpMask & (MSD_U8)0x7;
                vlanPcpData = filterParam->secondLayerParam.vlanPcpData & (MSD_U8)0x7;
            }
            if(filterParam->secondLayerParam.checkEtherFlag & CHECK_ETHER_VLAN_DEI_FLAG){//check inner vlan dei
                vlanDeiMask = filterParam->secondLayerParam.vlanDeiMask &(MSD_U8) 0x1;
                vlanDeiData = filterParam->secondLayerParam.vlanDeiData & (MSD_U8)0x1;
            }
            if(filterParam->secondLayerParam.checkEtherFlag & CHECK_ETHER_VLAN_VID_FLAG){ //check inner vlan vid
                vlanVidMask = filterParam->secondLayerParam.vlanIdMask & (MSD_U16)0xfff;
                vlanVidData = filterParam->secondLayerParam.vlanIdData & (MSD_U16)0xfff;
            }
            vlanTagMask = CREATE_VLAN_TAG(vlanTpidMask, vlanPcpMask, vlanDeiMask, vlanVidMask);
            vlanTagData = CREATE_VLAN_TAG(vlanTpidData, vlanPcpData, vlanDeiData, vlanVidData);
        } else if(!isCheckVlan && isCheckNotVlan){//仅匹配未标签帧,不匹配标签帧(vlan tag mask设置为1，data设置为0 )
            vlanTagMask = CREATE_VLAN_TAG(0xffff,0, 0, 0);
        }
    }

    VlanModel vlanModel;
    deviceVlanModuleGetQMode(devNum,ALL_PORT_PARAM,&vlanModel);//
    if(filterParam != NULL){
        if(vlanModel != VLANMODEL_QINQ && isCheckQinqVlan){//没有开启qinq vlan功能，并且需要检测外层标签，这是不允许的。
            return MSD_FEATRUE_NOT_ALLOW;
        }
    }

    MSD_U8 filterNum = 0;//默认根据没有使用的filter编号找到对应的tcam pointer num
    //check the filter is exist
    listSetMatchMethod(s_filters[devNum],searchFilterById);//根据id查找
    listNode *filterNode = listSearchKey(s_filters[devNum],(void *)&filterId);
    if(filterNode == NULL){ //id没有查询到，在看名称是否存在,
        listSetMatchMethod(s_filters[devNum],searchFilterByName);//根据名称查找
        filterNode = listSearchKey(s_filters[devNum],(void *)filterName);
    }else{//过滤器,根据id查询到，则需要判断是否存在相同的过滤器名称
        listSetMatchMethod(s_filters[devNum],searchFitlerByNqidAndEqname);
        FilterEntry tempFilterEntry;
        msdMemSet(&tempFilterEntry,0,sizeof(FilterEntry));
        tempFilterEntry.filter.filterId = filterId;
        memcpy(tempFilterEntry.filter.filterName,filterName,strlen(filterName));
        listNode *tempNode = listSearchKey(s_filters[devNum],(void *)&tempFilterEntry);
        if(tempNode != NULL){
            MSD_DBG_ERROR(("device_filter_module_add_filter failed,the filter_name is exist in the other filter!\n"));
            return MSD_ALREADY_EXIST;
        }
    }
    //check filter ingress action is exist()
    listSetMatchMethod(s_filters[devNum],searchFilterByFilterParam);//
    FilterEntry tempFilterEntry;
    msdMemSet(&tempFilterEntry,0,sizeof(FilterEntry));
    tempFilterEntry.filter.ftype = ftype;
    if(filterParam != NULL){
        tempFilterEntry.filter.filterParam = *filterParam;
    }
    tempFilterEntry.filter.ingressPortVecBit = ingressPortVecBit;
    listNode *tempNode = listSearchKey(s_filters[devNum],(void *)&tempFilterEntry);
    if(tempNode != NULL && tempNode != filterNode){//temp_node == filter_node的时候是修改
        MSD_DBG_ERROR(("device_filter_module_add_filter failed,the filter ingress action is exist in the other filter!\n"));
        return MSD_ALREADY_EXIST;
    }
    //根据过滤器是否存在，做相应的初始化操作
    FilterEntry *filterEntry = NULL;
    if(filterNode != NULL){ //修改filter，则根据filter找到对应的tcam pointer num，然后在执行相应的修改操作
        filterEntry = (FilterEntry *)filterNode->value;
        filterNum = filterEntry->filter.filterId;
    }else{//添加filter,不允许相同的入口过滤的过滤器存在，相同过滤条件的过滤器被认为是同一种过滤器
        if(listLength(s_filters[devNum]) >= FILTER_MAX_NUM){//判定是否达到最大值，如果达到最大值就直接返回错误
            return MSD_NO_SPACE;
        }
        filterNum = ++s_filterModuleInit[devNum].filterIndex;//如果为添加，则需要生产添加filter的id值
    }
    MSD_U32  needTcamEntryPointer[MAX_FILTER_ENTRY_SIZE] = {0};//保存本次过滤条目对应的tcam entry编号
    int needTcamEntrySize;
    MSD_U32 releaseTcamEntryPointer[MAX_FILTER_ENTRY_SIZE] = {0};
    int releaseTcamEntrySize;
    //如果添加了其他类型的过滤器，则从以下代码开始修改
    MSD_BOOL isNeedCheckQinqAndNotQinq = MSD_FALSE;
    if(isCheckNotQinqVlan == isCheckQinqVlan){//当这2个标志同时设置或者同时没有设置的时候即代表要同时检测双标签
        isNeedCheckQinqAndNotQinq = MSD_TRUE;
    }
    ret =  setDeviceFilterAllTcamPointer(devNum,filterEntry,needTcamEntryPointer,MAX_FILTER_ENTRY_SIZE,&needTcamEntrySize,
                                        releaseTcamEntryPointer,MAX_FILTER_ENTRY_SIZE,&releaseTcamEntrySize,ftype,isNeedCheckQinqAndNotQinq);
    if(ret != MSD_OK) return ret;
    if(ftype != FILTER_TYPE_IP_TCP_OR_UDP){// 非IP帧或者过滤所有帧,依然可以过滤ip帧,但是如果需要过滤ip字段或者tcp/udp，则使用非ip过滤器类型，则不能对其进行过滤
        MSD_TCAM_DATA tcamData1,tcamData2;
        msdMemSet(&tcamData1, 0, sizeof(MSD_TCAM_DATA));
        msdMemSet(&tcamData2, 0, sizeof(MSD_TCAM_DATA));
        tcamData1.frameTypeMask = 0x3; //有效的TCAM ，Mask这2位必须为1
        tcamData1.spv = 0x0;//设置为全0，spvMask相应位设置为0，则代表相应端口需要做入口过滤
        tcamData1.spvMask = ~ingressPortVecBit;
        tcamData1.pvidMask = 0x1FF;//对于所有 non-Provider TCAM项，Index位的mask必须始终是0x1FF。
        setEgressPortsAndFilterType(MSD_TRUE, &tcamData1,egressPortVecBit,etype);//将设置过滤的转发端口和端口的动作设置到TCAM Entry 1
        if(ftype == FILTER_TYPE_ALL){ //filter all
            ret = msdTcamEntryAdd(devNum, needTcamEntryPointer[0], &tcamData1);//add tcam entry 1
            if(ret != MSD_OK) return ret;
            tcamData2.frameType = 0x2;//provider port shoud be set 0x2,netowrk port should be set 0x0
            tcamData2.frameTypeMask = 0x3; //有效的TCAM ，Mask这2位必须为1
            tcamData2.spv = 0x0;//设置为全0，spvMask相应位设置为0，则代表相应端口需要做入口过滤
            tcamData2.spvMask = ~ingressPortVecBit;
            setEgressPortsAndFilterType(MSD_TRUE, &tcamData2,egressPortVecBit,etype);//将设置过滤的转发端口和端口的动作设置到TCAM Entry 2
            ret = msdTcamEntryAdd(devNum, needTcamEntryPointer[1], &tcamData2);//add tcam entry 2
            if(ret != MSD_OK) return ret;
        }else { //
            if(isCheckQinqVlan && !isCheckNotQinqVlan){//匹配Qinq标签帧，不匹配非QinQ标签帧
                tcamData1.frameType = 0x2;//provider port shoud be set 0x2,netowrk port should be set 0x0
                tcamData1.frameTypeMask = 0x3; //有效的TCAM ，Mask这2位必须为1
                tcamData1.spv = 0x0;//设置为全0，spvMask相应位设置为0，则代表相应端口需要做入口过滤
                tcamData1.spvMask = ~ingressPortVecBit;
                setEgressPortsAndFilterType(MSD_TRUE, &tcamData1,egressPortVecBit,etype);//将设置过滤的转发端口和端口的动作设置到TCAM Entry 2
                if(isCheckQinqPcp && isCheckQinqDei){//check outer pcp and outer dei
                    tcamData1.ppri = (MSD_U8)((MSD_U8)((qinqPcpData & 0x7) << 1)
                                               | (MSD_U8)(qinqDeiData & 0x1));
                    tcamData1.ppriMask = (MSD_U8)((MSD_U8)((qinqPcpMask & 0x7) << 1)
                                                   | (MSD_U8)(qinqDeiMask & 0x1));
                }else if(!isCheckQinqPcp && isCheckQinqDei){ //check dei and not check pcp
                    tcamData1.ppri = (MSD_U8)(qinqDeiData & 0x1);
                    tcamData1.ppriMask  = (MSD_U8)(qinqDeiMask & 0x1);
                }else if(isCheckQinqPcp && !isCheckQinqDei){ //check pcp and not check dei
                    tcamData1.ppri = (MSD_U8)((qinqPcpData & 0x7) << 1);
                    tcamData1.ppriMask = (MSD_U8)((qinqPcpMask & 0x7) << 1);
                }

                if(isCheckQinqVid){//check outer vid
                    tcamData1.pvid = (MSD_U16)(qinqVidData & (MSD_U16)0xfff);
                    tcamData1.pvidMask = (MSD_U16)(qinqVidMask & (MSD_U16)0xfff);
                }
            }
            if(filterParam->secondLayerParam.checkEtherFlag & CHECK_ETHER_DEST_MAC_FLAG){//check dest mac
                for (int i = 0; i < 6; ++i) { //octet 1 ~ octet 6 is dest mac
                    tcamData1.frameOctet[i] = filterParam->secondLayerParam.destMacData[i];
                    tcamData1.frameOctetMask[i] = filterParam->secondLayerParam.destMacMask[i];
                }
            }
            if(filterParam->secondLayerParam.checkEtherFlag & CHECK_ETHER_SRC_MAC_FLAG){//check src mac
                for (int i = 0; i < 6; ++i) { //octet 7 ~ ocet 12 is src mac
                    tcamData1.frameOctet[i + 6] = filterParam->secondLayerParam.srcMacData[i];
                    tcamData1.frameOctetMask[i + 6] = filterParam->secondLayerParam.srcMacMask[i];
                }
            }
            if(isCheckVlan && !isCheckNotVlan){//filter tag frame and not filter untag frame
                tcamData1.frameOctet[12] = GET_CONTINUOUS_BITS(vlanTpidData,8,8);//get tpid's the high 8 bits
                tcamData1.frameOctet[13] = GET_CONTINUOUS_BITS(vlanTpidData,0,8);//get tpid's the lower bit 8 bits
                tcamData1.frameOctet[14] = (GET_CONTINUOUS_BITS(vlanPcpData,0,3) << 5) | (vlanDeiData << 4) | GET_CONTINUOUS_BITS(vlanVidData,8,4);
                tcamData1.frameOctet[15] = GET_CONTINUOUS_BITS(vlanVidData,0,8);
                tcamData1.frameOctetMask[12] = GET_CONTINUOUS_BITS(vlanTpidMask,8,8);//get tpid's the high 8 bits
                tcamData1.frameOctetMask[13] = GET_CONTINUOUS_BITS(vlanTpidMask,0,8);//get tpid's the lower bit 8 bits
                tcamData1.frameOctetMask[14] = (GET_CONTINUOUS_BITS(vlanPcpMask,0,3) << 5) | (vlanDeiMask << 4) | GET_CONTINUOUS_BITS(vlanVidMask,8,4);
                tcamData1.frameOctetMask[15] = GET_CONTINUOUS_BITS(vlanVidMask,0,8);
            } else if(!isCheckVlan && isCheckNotVlan){//仅匹配未标签帧
                tcamData1.frameOctetMask[12] = 0xff;
                tcamData1.frameOctetMask[13] = 0xff;
            }
            if(ftype == FILTER_TYPE_ARP){ //arp filter
                //16 ~ 17: ethertype
                tcamData1.frameOctet[16] = 0x08; //ethertype 0x0806
                tcamData1.frameOctet[17] = 0x06;
                tcamData1.frameOctetMask[16] = 0xff; //ethertype 0x0806
                tcamData1.frameOctetMask[17] = 0xff;
                    //18 ~ 19 : Hardware type : 1
                tcamData1.frameOctet[18] = 0x00;
                tcamData1.frameOctet[19] = 0x01;
                tcamData1.frameOctetMask[18] = 0xff;
                tcamData1.frameOctetMask[19] = 0xff;
                    //20 ~ 21 : Protocol type : 0x0800
                tcamData1.frameOctet[20] = 0x08;
                tcamData1.frameOctet[21] = 0x00;
                tcamData1.frameOctetMask[20] = 0xff;
                tcamData1.frameOctetMask[21] = 0xff;
                    //22: Hardware size : 6
                    //23: protocol size : 4
                tcamData1.frameOctet[22] = 0x06;
                tcamData1.frameOctet[23] = 0x04;
                tcamData1.frameOctetMask[22] = 0xff;
                tcamData1.frameOctetMask[23] = 0xff;
                    //24 ~ 25 : opcode(request = 1,response = 2,not support other code)
                tcamData1.frameOctet[24] = 0x00;
                tcamData1.frameOctet[25] = filterParam->thirdAndAboveLayerParam.arpFilter.operation;
                tcamData1.frameOctetMask[24] = 0xff;
                tcamData1.frameOctetMask[25] = 0xff;
                //26 ~ 31 : sender mac address
                for(int i = 26; i <= 31; ++i){
                    tcamData1.frameOctet[i] = filterParam->thirdAndAboveLayerParam.arpFilter.senderMacAddressData[i- 26];
                    tcamData1.frameOctetMask[i] = filterParam->thirdAndAboveLayerParam.arpFilter.senderMacAddressMask[i - 26];
                }
                //32 ~ 35 : sender ip address
                for(int i = 32; i <= 35; ++i){
                    tcamData1.frameOctet[i] = filterParam->thirdAndAboveLayerParam.arpFilter.senderIpAddressData[i - 32];
                    tcamData1.frameOctetMask[i] = filterParam->thirdAndAboveLayerParam.arpFilter.senderIpAddressMask[i - 32];
                }
                //36 ~ 41 : target mac address
                for(int i = 36; i <= 41; ++i){
                    tcamData1.frameOctet[i] = filterParam->thirdAndAboveLayerParam.arpFilter.targetMacAddressData[i - 36];
                    tcamData1.frameOctetMask[i] = filterParam->thirdAndAboveLayerParam.arpFilter.targetMacAddressMask[i - 36];
                }
                //42 ~ 45 : target ip address
                for(int i = 42; i <= 45; ++i){
                    tcamData1.frameOctet[i] = filterParam->thirdAndAboveLayerParam.arpFilter.targetIpAddressData[i - 42];
                    tcamData1.frameOctetMask[i] = filterParam->thirdAndAboveLayerParam.arpFilter.targetIpAddressMask[i - 42];
                }
            }
            ret = msdTcamEntryAdd(devNum, needTcamEntryPointer[0], &tcamData1);
            if(ret != MSD_OK) return ret;
            if(isNeedCheckQinqAndNotQinq){//如果既需要匹配非QinQ，又要匹配QinQ时,在使用一个TCAM ENTRY用以匹配QinQ标签
                tcamData1.frameType = 0x2;//provider port shoud be set 0x2,netowrk port should be set 0x0
                tcamData1.frameTypeMask = 0x3; //有效的TCAM ，Mask这2位必须为1
                tcamData1.spv = 0x0;//设置为全0，spvMask相应位设置为0，则代表相应端口需要做入口过滤
                tcamData1.spvMask = ~ingressPortVecBit;
                if(isCheckQinqPcp && isCheckQinqDei){//check outer pcp and outer dei
                    tcamData1.ppri = (MSD_U8)((MSD_U8)((qinqPcpData & 0x7) << 1)
                                               | (MSD_U8)( qinqDeiData & 0x1));
                    tcamData1.ppriMask = (MSD_U8)((MSD_U8)((qinqPcpMask & 0x7) << 1)
                                                   | (MSD_U8)(qinqDeiMask & 0x1));
                }else if(!isCheckQinqPcp && isCheckQinqDei){ //check dei and not check pcp
                    tcamData1.ppri = (MSD_U8)( qinqDeiData & 0x1);
                    tcamData1.ppriMask  = (MSD_U8)(qinqDeiMask & 0x1);
                }else if(isCheckQinqPcp && !isCheckQinqDei){ //check pcp and not check dei
                    tcamData1.ppri = (MSD_U8)((qinqPcpData & 0x7) << 1);
                    tcamData1.ppriMask = (MSD_U8)((qinqPcpMask & 0x7) << 1);
                }
                if(isCheckQinqVid){//check outer vid
                    tcamData1.pvid = (MSD_U16)(qinqVidData & (MSD_U16)0xfff);
                    tcamData1.pvidMask = (MSD_U16)(qinqVidMask & (MSD_U16)0xfff);
                }
                ret = msdTcamEntryAdd(devNum, needTcamEntryPointer[1], &tcamData1);
                if(ret != MSD_OK) return ret;
            }
        }
    }else{ //IPv4/IPv6， UDP /TCP
        MSD_TCAM_ENT_KEY keyMaskPtr,keyPtr;
        msdMemSet(&keyMaskPtr, 0, sizeof(MSD_TCAM_ENT_KEY));
        msdMemSet(&keyPtr, 0, sizeof(MSD_TCAM_ENT_KEY));
        keyMaskPtr.frameType = 0x3;//有效的TCAM ，Mask这2位必须为1
        keyMaskPtr.spv = (MSD_U16)~ingressPortVecBit;
        keyMaskPtr.pvid = 0x1FF;//对于所有 non-Provider TCAM项，Index位的mask必须始终是0x1FF。

        keyPtr.spv = 0x0;//
        keyPtr.frameType = 0x0;//

        if(isCheckQinqVlan && !isCheckNotQinqVlan){//匹配Qinq标签帧，不匹配非QinQ标签帧
            keyPtr.frameType = 0x2;
            if(isCheckQinqPcp && isCheckQinqDei){//check outer pcp and outer dei
                keyPtr.ppri = (MSD_U8)((MSD_U8)((qinqPcpData & 0x7) << 1)
                                        | (MSD_U8)(qinqDeiData & 0x1));
                keyMaskPtr.ppri = (MSD_U8)((MSD_U8)((qinqPcpMask & 0x7) << 1)
                                            | (MSD_U8)(qinqDeiMask & 0x1));
            }else if(!isCheckQinqPcp && isCheckQinqDei){ //check dei and not check pcp
                keyPtr.ppri = (MSD_U8)(qinqDeiData & 0x1);
                keyMaskPtr.ppri = (MSD_U8)(qinqDeiMask & 0x1);
            }else if(isCheckQinqPcp && !isCheckQinqDei){ //check pcp and not check dei
                keyPtr.ppri = (MSD_U8)((qinqPcpData & 0x7) << 1);
                keyMaskPtr.ppri = (MSD_U8)((qinqPcpMask & 0x7) << 1);
            }
            if(isCheckQinqVid){//check outer vid
                keyPtr.pvid = (MSD_U16)(qinqVidData & (MSD_U16)0xfff);
                keyMaskPtr.pvid = (MSD_U16)(qinqVidMask & (MSD_U16)0xfff);
            }
        }
        //ethertype layer param set
        MSD_TCAM_ENT_PKT_UNT maskPtr;
        msdMemSet(&maskPtr, 0, sizeof(MSD_TCAM_ENT_PKT_UNT));

        MSD_TCAM_ENT_PKT_UNT patternPtr;
        msdMemSet(&patternPtr, 0, sizeof(MSD_TCAM_ENT_PKT_UNT));
        if(filterParam->secondLayerParam.checkEtherFlag & CHECK_ETHER_DEST_MAC_FLAG){//check dest mac
            for (int i = 0; i < 6; ++i) {
                maskPtr.da.arEther[i] = filterParam->secondLayerParam.destMacMask[i];
                patternPtr.da.arEther[i] = filterParam->secondLayerParam.destMacData[i];
            }
        }
        if(filterParam->secondLayerParam.checkEtherFlag & CHECK_ETHER_SRC_MAC_FLAG){//check src mac
            for (int i = 0; i < 6; ++i) {
                maskPtr.sa.arEther[i] = filterParam->secondLayerParam.srcMacMask[i];
                patternPtr.sa.arEther[i] = filterParam->secondLayerParam.srcMacData[i];
            }
        }
        if(isCheckVlan && !isCheckNotVlan){//filter tag frame and not filter untag frame
            maskPtr.vlanTag = vlanTagMask;
            patternPtr.vlanTag = vlanTagData;
        } else if(!isCheckVlan && isCheckNotVlan){//仅匹配未标签帧
            maskPtr.vlanTag = vlanTagMask;
        }
        else{//match inner tag or untag(不做任何操作)
            maskPtr.vlanTag = vlanTagMask;
        }

        //set the ip and udp/tcp param
        MSD_TCAM_PKT_TYPE pktType = filterParam->thirdAndAboveLayerParam.ipTcpOrUdpFilter.pktType;
        MSD_TCAM_ENT_PKT_FORMAT_UNT mask = filterParam->thirdAndAboveLayerParam.ipTcpOrUdpFilter.mask;
        MSD_TCAM_ENT_PKT_FORMAT_UNT data = filterParam->thirdAndAboveLayerParam.ipTcpOrUdpFilter.data;
        handleIpTcpOrUdpPktFormatContent(&maskPtr.pktUnit, &mask, pktType);
        handleIpTcpOrUdpPktFormatContent(&patternPtr.pktUnit, &data, pktType);
        //force the ethertype and ip protocol and version according to the pkt_type,if the frame is not Meet this condition,the filter is will not work
        switch (pktType) {
        case MSD_TCAM_TYPE_ONLY_IPV4:
            patternPtr.etherType = (MSD_U16)0x0800;//ethertype:ipv4
            maskPtr.etherType = (MSD_U16)0xffff;//
            patternPtr.pktUnit.ipv4Only.version = (MSD_U8)0x04;//version:ipv4
            maskPtr.pktUnit.ipv4Only.version = (MSD_U8)0xff;//
            break;
        case MSD_TCAM_TYPE_ONLY_IPV6:
            patternPtr.etherType = (MSD_U16)0x86dd;
            maskPtr.etherType = (MSD_U16)0xffff;//
            patternPtr.pktUnit.ipv6Only.version = (MSD_U8)0x6;//ipv6
            maskPtr.pktUnit.ipv6Only.version = (MSD_U8)0xff;//
            break;
        case MSD_TCAM_TYPE_IPV4_TCP://
            patternPtr.etherType = (MSD_U16)0x0800;//ethertype:ipv4
            maskPtr.etherType = (MSD_U16)0xffff;//
            patternPtr.pktUnit.ipv4Tcp.ipv4.version = (MSD_U8)0x04;//version:ipv4
            maskPtr.pktUnit.ipv4Tcp.ipv4.version = (MSD_U8)0xff;//
            patternPtr.pktUnit.ipv4Tcp.ipv4.protocol = (MSD_U8)0x6;//protocol:TCP
            maskPtr.pktUnit.ipv4Tcp.ipv4.protocol = (MSD_U8)0xff;
            break;
        case MSD_TCAM_TYPE_IPV4_UDP:
            patternPtr.etherType = (MSD_U16)0x0800;
            maskPtr.etherType = (MSD_U16)0xffff;//
            patternPtr.pktUnit.ipv4Udp.ipv4.version = (MSD_U8)0x04;//ipv4
            maskPtr.pktUnit.ipv4Udp.ipv4.version = (MSD_U8)0xff;//
            patternPtr.pktUnit.ipv4Udp.ipv4.protocol = (MSD_U8)0x11;//UDP
            maskPtr.pktUnit.ipv4Udp.ipv4.protocol = (MSD_U8)0xff;
            break;
        case MSD_TCAM_TYPE_IPV6_TCP:
            patternPtr.etherType = (MSD_U16)0x86dd;
            maskPtr.etherType = (MSD_U16)0xffff;//
            patternPtr.pktUnit.ipv6Tcp.ipv6.version = (MSD_U8)0x6;//ipv6
            maskPtr.pktUnit.ipv6Tcp.ipv6.version = (MSD_U8)0xff;//
            patternPtr.pktUnit.ipv6Tcp.ipv6.nextHdr = (MSD_U8)0x06;//TCP
            maskPtr.pktUnit.ipv6Tcp.ipv6.nextHdr = (MSD_U8)0xff;
            break;
        case MSD_TCAM_TYPE_IPV6_UDP:
            patternPtr.etherType = (MSD_U16)0x86dd;
            maskPtr.etherType = (MSD_U16)0xffff;//
            patternPtr.pktUnit.ipv6Tcp.ipv6.version = (MSD_U8)0x6;//ipv6
            maskPtr.pktUnit.ipv6Tcp.ipv6.version = (MSD_U8)0xff;//
            patternPtr.pktUnit.ipv6Tcp.ipv6.nextHdr = (MSD_U8)0x011;//UDP
            maskPtr.pktUnit.ipv6Tcp.ipv6.nextHdr = (MSD_U8)0xff;
            break;
        }
        MSD_TCAM_ENT_ACT actionPtr;
        msdMemSet(&actionPtr, 0, sizeof(MSD_TCAM_ENT_ACT));
        setEgressPortsAndFilterType(MSD_FALSE,&actionPtr,egressPortVecBit,etype);
        MSD_BOOL entry2Used = MSD_FALSE;

        ret = msdTcamAdvConfig(devNum, pktType, needTcamEntryPointer[0], needTcamEntryPointer[1], &keyMaskPtr, &keyPtr,
                               &maskPtr, &patternPtr, &actionPtr, &entry2Used);
        if(ret != MSD_OK) return ret;
        if(!entry2Used){//only use one tcam entry
            needTcamEntrySize = 1;
            // ret = msdTcamEntryDelete(dev_num,need_tcam_entry_pointer[1]);//释放第二个tcam entry
            // if(ret != MSD_OK)
            //     return ret;
        }else{
            needTcamEntrySize = 2;
        }
        if(isNeedCheckQinqAndNotQinq){//如果既需要匹配非QinQ，又要匹配QinQ时,在使用一个TCAM ENTRY用以匹配QinQ标签
            keyPtr.frameType = 0x2;
            if(isCheckQinqPcp && isCheckQinqDei){//check outer pcp and outer dei
                keyPtr.ppri = (MSD_U8)((MSD_U8)((qinqPcpData & 0x7) << 1)
                                        | (MSD_U8)( qinqDeiData & 0x1));
                keyMaskPtr.ppri = (MSD_U8)((MSD_U8)((qinqPcpMask & 0x7) << 1)
                                            | (MSD_U8)(qinqDeiMask & 0x1));
            }else if(!isCheckQinqPcp && isCheckQinqDei){ //check dei and not check pcp
                keyPtr.ppri = (MSD_U8)( qinqDeiData & 0x1);
                keyMaskPtr.ppri = (MSD_U8)(qinqDeiMask & 0x1);
            }else if(isCheckQinqPcp && !isCheckQinqDei){ //check pcp and not check dei
                keyPtr.ppri = (MSD_U8)((qinqPcpData & 0x7) << 1);
                keyMaskPtr.ppri = (MSD_U8)((qinqPcpMask & 0x7) << 1);
            }
            if(isCheckQinqVid){//check outer vid
                keyPtr.pvid = (MSD_U16)(qinqVidData & (MSD_U16)0xfff);
                keyMaskPtr.pvid = (MSD_U16)(qinqVidMask & (MSD_U16)0xfff);
            }

            int tcamEntryPointer1 = needTcamEntryPointer[1];//默认没有使用第二个，则一个过滤器需要2个TCAM Entry
            int tcamEntryPointer2 = needTcamEntryPointer[2];
            if(entry2Used){//需要使用4个tcam entry
                tcamEntryPointer1 = needTcamEntryPointer[2];
                tcamEntryPointer2 = needTcamEntryPointer[3];
            }
            ret = msdTcamAdvConfig(devNum, pktType, tcamEntryPointer1, tcamEntryPointer2, &keyMaskPtr, &keyPtr,
                                   &maskPtr, &patternPtr, &actionPtr, &entry2Used);
            if(ret != MSD_OK) return ret;
            if(!entry2Used){
                needTcamEntrySize = 2;
            }else{
                needTcamEntrySize = 4;
            }
        }
    }
    for(int i = 0; i < releaseTcamEntrySize; ++i){
        ret = msdTcamEntryDelete(devNum,releaseTcamEntryPointer[i]);
        if(ret != MSD_OK)
            return ret;
    }
    MSD_BOOL isSuccess = addOrModifyFilterEntryToList(filterEntry,devNum,filterNum,filterName,ingressPortVecBit,
                                                      etype,egressPortVecBit,ftype,filterParam,needTcamEntryPointer,needTcamEntrySize);//
    if(isSuccess)
        return MSD_OK;
    return MSD_FAIL;
}

/****************************************************************************************************************
 * @brief removeFilterByNameOrId 根据id或者name删除过滤器
 * @param devNum
 * @param isFilterId 是否是设备id，如果是，则根据id删除，否则根据name删除
 * @param param : filter_id or filter_name
 * @param isFound
 * @return
 ****************************************************************************************************************/
static MSD_STATUS removeFilterByNameOrId(MSD_U8 devNum, MSD_BOOL isFilterId, void *param, MSD_BOOL *isFound)
{
    *isFound = MSD_FALSE;
    MSD_STATUS ret = checkDevNumAndIsEnable(devNum);
    if(ret != MSD_OK) return ret;
    //search filter list if has the filter_num filter
    if(isFilterId){
        listSetMatchMethod(s_filters[devNum],searchFilterById);//根据id查找
    }else{
        listSetMatchMethod(s_filters[devNum],searchFilterByName);//根据name查找
    }
    listNode *filterNode = listSearchKey(s_filters[devNum],param);
    if(filterNode == NULL){ //id没有查询到
        return MSD_OK;//没有查询到，直接返回0K
    }
    *isFound = MSD_TRUE;
    //delete the tcam entry num
    FilterEntry *filterEntry = (FilterEntry *)filterNode->value;
    MSD_U32 *tcamPointer = filterEntry->tcamPointer;
    MSD_U8 tcamSize = filterEntry->tcamPointerSize;
    for(int i = 0; i < tcamSize; ++i){
        ret = msdTcamEntryDelete(devNum, tcamPointer[i]);
        if(ret != MSD_OK) return ret;
    }
    listDelNode(s_filters[devNum],filterNode);
    return ret;
}


MSD_STATUS deviceFilterModuleRemoveFilterById(MSD_U8 devNum, MSD_U8 filterNum, MSD_BOOL *isFound)
{
    return removeFilterByNameOrId(devNum,MSD_TRUE,(void *)&filterNum,isFound);
}

MSD_STATUS deviceFilterModuleRemoveFilterByName(MSD_U8 devNum, const char *filterName,  MSD_BOOL *isFound)
{
    return removeFilterByNameOrId(devNum,MSD_FALSE,(void *)filterName,isFound);
}

MSD_STATUS deviceFilterModuleClearFilters(MSD_U8 devNum)
{
    MSD_STATUS ret = checkDevNumAndIsEnable(devNum);
    if(ret != MSD_OK) return ret;
    ret = msdTcamAllDelete(devNum);
    if(ret == MSD_OK){
        s_filterModuleInit[devNum].filterIndex = 0;//reset filter index
        listEmpty(s_filters[devNum]);
    }
    return ret;
}



void copyDeviceFilterParam(DeviceFilter *src, DeviceFilter *dest)
{
    msdMemSet(dest, 0, sizeof(DeviceFilter));
    msdMemCpy(dest->filterName,src->filterName,strlen(src->filterName));
    dest->filterId = src->filterId;
    dest->ingressPortVecBit = src->ingressPortVecBit;
    dest->etype = src->etype;
    dest->egressPortVecBit = src->egressPortVecBit;
    dest->ftype = src->ftype;
    dest->filterParam.secondLayerParam = src->filterParam.secondLayerParam;
    dest->filterParam.thirdAndAboveLayerParam = src->filterParam.thirdAndAboveLayerParam;
}

static MSD_STATUS findFilterByNameOrId(MSD_U8 devNum, MSD_BOOL isFilterId, void *param, DeviceFilter *filter, MSD_BOOL *isFound)
{
    *isFound = MSD_FALSE;
    msdMemSet(filter,0,sizeof(DeviceFilter));
    MSD_STATUS ret = checkDevNumAndIsEnable(devNum);
    if(ret != MSD_OK) return ret;
    //search filter list if has the filter_num filter
    if(isFilterId){
        listSetMatchMethod(s_filters[devNum],searchFilterById);//根据id查找
    }else{
        listSetMatchMethod(s_filters[devNum],searchFilterByName);//根据name查找
    }

    listNode *filterNode = listSearchKey(s_filters[devNum],param);
    if(filterNode == NULL){ //id没有查询到
        return MSD_OK;//没有查询到，直接返回0K
    }
    FilterEntry *filter_entry = (FilterEntry *)filterNode->value;
    copyDeviceFilterParam(&filter_entry->filter,filter);
    *isFound = MSD_TRUE;
    return MSD_OK;
}

MSD_STATUS deviceFilterModuleFindFilterByFilterNum(MSD_U8 devNum, MSD_U8 filterNum, DeviceFilter* filter, MSD_BOOL *isFound)
{
    MSD_STATUS ret = findFilterByNameOrId(devNum,MSD_TRUE,(void *)&filterNum,filter, isFound);
    return ret;
}

MSD_STATUS deviceFilterModuleFindFilterByFilterName(MSD_U8 devNum, const char *filterName, DeviceFilter *filter,MSD_BOOL *isFound)
{
    MSD_STATUS ret = findFilterByNameOrId(devNum,MSD_FALSE,(void *)filterName,filter, isFound);
    return ret;
}

MSD_STATUS deviceFilterModuleGetAllFilters(MSD_U8 devNum, DeviceFilter* filter, int filterSize, int* filterCount)
{
    *filterCount = 0;
    MSD_STATUS ret = checkDevNumAndIsEnable(devNum);
    if(ret != MSD_OK) return ret;
    if(filterSize <= 0 || filter == NULL){
        MSD_DBG_ERROR(("device_filter_module_get_all_filters failed,the filer param is null or the size is 0!\n"));
        return MSD_BAD_PARAM;
    }
    listIter * liter = listGetIterator(s_filters[devNum],AL_START_HEAD);
    listNode *node = NULL;
    FilterEntry *filterEntry = NULL;
    int index = 0;
    while ((node = listNext(liter)) != NULL) {
        filterEntry = (FilterEntry *)node->value;
        copyDeviceFilterParam(&filterEntry->filter, &filter[index++]);
        if(index > filterSize){ //no more space to stor data
            MSD_DBG_ERROR(("there is no more space to store data"));
            ret = MSD_NO_SPACE;
            break;
        }
    }
    *filterCount = index;
    listReleaseIterator(liter);
    return ret;
}



