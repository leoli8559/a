#pragma once

#include "umsdUtil.h"

#ifdef __cplusplus
extern "C" {
#endif


#define  FILTER_NUM_NAME_MAX_LEN   16  //过滤器名称最大字符数
#define  FILTER_MAX_NUM   32 //最多可以支持filter的个数

/**
 * @brief TAG_MODEL_TYPE
 * QINQ标签模式需要入口端口为Provider Port，否则设置为QinQ标签模式无效。
 */
typedef enum {
    TAG_MODEL_TYPE_UNTAG_OR_TAG = 0, //过滤不带标签或者带标签的帧(default value)
	TAG_MODEL_TYPE_TAG, //过滤带VLAN标签的帧
	TAG_MODEL_TYPE_UNTAG,//过滤不带标签的帧
	TAG_MODEL_TYPE_QINQ_WITH_OR_WITHOUT_VLAN_TAG, //过滤QinQ标签的帧(不带VLAN标签或者带VLAN，即双标签或者仅有P标签),即只要P标签满足条件即可
	TAG_MODEL_TYPE_QINQ_WITH_VLAN_TAG, //过滤QinQ标签的帧(带VLAN，即双标签)，P标签和C标签同时满足条件。
	TAG_MODEL_TYPE_QINQ_WITHOUT_VLAN_TAG //过滤QinQ标签的帧(不带VLAN，即仅有P标签)，不能存在C标签
}TagModelType;

/**
 * @brief egress_type 出口类型
 *
 */
typedef enum {
    EGRESS_TYPE_MIRROR = 1, //映射到出口端口（如果和入口端口一致，则只发送一个包，不Mirror）
    EGRESS_TYPE_FORCE_EGRESS_PORT,//强制将帧转发到指定的出口端口
    EGRESS_TYPE_DROP,//丢弃
    EGRESS_TYPE_AVOID_FURTHRE_FILTER, //出口不做进一步处理(default value)
}EgressType;


//是否匹配第二层对应对应的字段的标志，用于设置second_layer_filer_param_t 类型的 check_ether_flag字段。如果相应标志被设置，就代表要匹配对应的字段。
#define CHECK_ETHER_DEST_MAC_FLAG    (1 << 0)   //匹配目标地址
#define CHECK_ETHER_SRC_MAC_FLAG      (1 << 1) //匹配源地址

/** 如果CHECK_ETHER_QINQ_VLAN_FLAG 和CHECK_ETHER_NOT_QINQ_VLAN_FALG均不设置与这2个标志同时设置，都认为该过滤器同时匹配具有QINQ标签和非QINQ标签的帧 */
#define CHECK_ETHER_QINQ_VLAN_FLAG  (1 << 2) //匹配具有外层标签的帧
#define CHECK_ETHER_NOT_QINQ_VLAN_FALG (1 << 3) //匹配没有外层标签的帧
/** 如果CHECK_ETHER_VLAN_FLAG 和CHECK_ETHER_NOT_VLAN_FALG均不设置与这2个标志同时设置，都认为该过滤器同时匹配具有VLAN标签和非VLAN标签的帧 */
#define CHECK_ETHER_VLAN_FLAG  (1 << 4) //匹配具有Vlan标签的帧(当CHECK_ETHER_NOT_VLAN_FALG设置时，则即匹配无标签帧，也匹配标签帧)
#define CHECK_ETHER_NOT_VLAN_FALG (1 << 5) //匹配没有vlanTag标签的帧(当CHECK_ETHER_VLAN_FLAG设置时，则即匹配无标签帧，也匹配标签帧)

#define CHECK_ETHER_QINQ_VLAN_PCP_FLAG (1 << 6)//匹配QINQ VLAN PCP
#define CHECK_ETHER_QINQ_VLAN_DEI_FLAG (1 << 7) //匹配QINQ Vlan  DEI
#define CHECK_ETHER_QINQ_VLAN_VID_FLAG (1 << 8) //匹配QINQ Vlan VID

#define CHECK_ETHER_VLAN_PCP_FLAG (1 << 9)//匹配Vlan PCP
#define CHECK_ETHER_VLAN_DEI_FLAG (1 << 10)//匹配Vlan DEI
#define CHECK_ETHER_VLAN_VID_FLAG (1 << 11)//匹配Vlan VID

/** 检测所有标志的组合, 即检测所有二层字段的标志*/
#define CHECK_ETHER_ALL_FLAG  (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4) | (1 << 5) | (1 << 6) | (1 << 7) | (1 << 8) | (1 << 9) | (1 << 10) | (1 << 11)

/**
 * 与入口过滤相关的EtherType（二层包）字段的Data和Mask。
 * Data与Mask共同决定是否帧是否满足过滤条件，Data与Mask相应位的设置如下
 * data和mask对应位同时为0，无论帧的内容是啥，都会发生TCAM命中
 * Mask为1，Data为0，则当帧的内容为0时，会发送TCAM命中
 * MASK为1，Data为1.当帧的内容为1时，才会发生TCAM命中
 * Mask为1，Data为0，TCAM表项不会发生任何TCAM命中
 * CHECK_ETHER_ALL_FLAG:可以决定是否进行相应的字段匹配(即相当于将data和mask设置为0,当data和mask同时为0时，也不会比较相应的字段)
 */
typedef struct {

    //qinq vlan 需要开启qinq tag模式
    MSD_U16 qinqVlanIdData; //qinq vlan id
    MSD_U16 qinqVlanIdMask; //qinq vlan mask
    MSD_U8 qinqVlanPcpData;//qinq vlan pcp
    MSD_U8 qinqVlanPcpMask;//qinq vlan pcp mask
    MSD_BOOL qinqVlanDeiData;//qinq vlan dei
    MSD_BOOL qinqVlanDeiMask;//qinq vlan dei

    MSD_U16 vlanIdData; //vlan id
    MSD_U16 vlanIdMask; //vlan id mask
    MSD_U8 vlanPcpData;//vlan pcp
    MSD_U8 vlanPcpMask;//vlan pcp
    MSD_BOOL vlanDeiData;//vlan dei
    MSD_BOOL vlanDeiMask;//vlan dei

    MSD_U8 destMacData[6]; //dest mac
    MSD_U8 destMacMask[6]; //dest mac mask
    MSD_U8 srcMacData[6]; //src mac
    MSD_U8 srcMacMask[6]; //src mac mask

    MSD_U16  checkEtherFlag; //CHECK_ETHER_XXX_FLAG 组合(|)

}SecondLayerFilerParam;

/**
 * @brief filter_type 过滤帧的类型
 * @note 如果仅仅匹配二层字段，最好考虑filter_type_only_second_param,
 * 比如考虑filter_type_ip_tcp_or_udp,但是不设置相应的三层字段（IP属性），
 * 那么会默认使用IP4/TCP，会将EtherType设置为0x0800以及将将四层的protocol设置为0x6.
 * 当不启用QinQ功能，但是传递一个双标签帧时，即使传递的是IPv4/TCP帧，也会匹配失败，
 * 这是因为双标签会多占用4个字段，会导致设置的字段发生错误,本来EtherType为第17个字节，但是双标签帧的时候
 * 就会占用第17个字节，导致匹配失败.
 * 如果仅对端口做相应的过滤，考虑使用filter_type_all
 */
typedef enum {
    FILTER_TYPE_IP_TCP_OR_UDP = 0, /** 过滤IP TCP(UDP) packet,device_filter_ip_tcp_or_udp_filter_t 结构体决定三层字段和四层字段*/
    FILTER_TYPE_ARP,   /** filter arp packet:DeviceFilterArpFilter,结构体决定其arp参数的比较*/
    /** add other type */
    FILTER_TYPE_SECOND_LAYER = FILTER_MAX_NUM - 1, /*过滤二层字段   */
    FILTER_TYPE_ALL = FILTER_MAX_NUM /** 过滤所有帧，不考虑二层和三层过滤以及以上的过滤条件*/
}FilterType;


/**
 * IPv4/IPv6 UDP/TCP 组合过滤器参数
 * 比如想比较IPv4,UDP包的TTL，则将pkt_type 设置为MSD_TCAM_TYPE_IPV4_UDP,mask ipv4's ttl 设置为0xff,data 的 ipv4's ttl 设置为想要的值
 * 如果不关心具体的字段（即匹配上），则不用设置该字段的mask和data
 */
typedef struct {
    MSD_TCAM_PKT_TYPE pktType;/** 用以决定选择哪种IP类型的包 */
    MSD_TCAM_ENT_PKT_FORMAT_UNT mask;/** mask */
    MSD_TCAM_ENT_PKT_FORMAT_UNT data;/** data */
}DeviceFilterIpTcpOrUdpFilter;

/**
 * @brief arp_operation: arp 协议类型
 */
typedef enum{
    ARP_OPERATION_REQUEST = 1, /** arp request */
    ARP_OPERATION_RESPONSE,   /** arp response */
    ARP_OPERATION_RARP_REQUEST,/** rarp request,目前不支持 */
    ARP_OPERATION_RARP_RESPONSE /** rarp response,目前不支持 */
}ArpOperation;

/**
 * @brief device_filter_arp_filter_t ARP过滤结构体参数
 */
typedef struct{
    ArpOperation operation;/** arp type */
    MSD_U8 senderMacAddressData[6];/** sender mac address data */
    MSD_U8 senderMacAddressMask[6];/** sender mac address mask */
    MSD_U8 targetMacAddressData[6];/** target mac address data */
    MSD_U8 targetMacAddressMask[6];/** target mac address mask */
    MSD_U8 senderIpAddressData[4];/** sender ip address data */
    MSD_U8 senderIpAddressMask[4];/** sender ip address mask */
    MSD_U8 targetIpAddressData[4];/** target ip address data */
    MSD_U8 targetIpAddressMask[4];/** target ip address mask */
}DeviceFilterArpFilter;

/**
 * 三层及其以上过滤条件，各个结构体的过滤是互斥 IPv4/IPv6, TCP/UDP packets
 * 其他类型的过滤条件，比如ARP等协议帧，暂时没有考虑
 */
typedef union {
    DeviceFilterIpTcpOrUdpFilter ipTcpOrUdpFilter;/** IPv4/IPv6, TCP/UDP packets fitler */
    DeviceFilterArpFilter arpFilter; /** arp filter */
}ThirdAndAboveLayerFilterParam;

/**
 * @brief 过滤参数
 */
typedef struct {
    SecondLayerFilerParam secondLayerParam;/** 二层过滤字段 */
    ThirdAndAboveLayerFilterParam thirdAndAboveLayerParam;/** 三层过滤参数  */
}FilterParam;

/**
 * @brief 过滤器
 */
typedef struct {
    char filterName[FILTER_NUM_NAME_MAX_LEN];//name
    MSD_U8 filterId;//id:
    MSD_U16 ingressPortVecBit;//入口端口集合,比如如果需要过滤入口端口port 1和port 2，则设置为0b00000110; 最低位到最高位分别代表port 0到port 最大值，设置为1，代表需要设置相应的入口
    EgressType etype;//出口端口类型，从指定的egress_port_vec_bit端口列表中做相应的操作。比如丢弃，mirror等
    MSD_U16 egressPortVecBit;//出口端口集合
    FilterParam filterParam;//过滤器参数，针对不同的帧类型有不同的值，如果为过滤所有帧，该参数无效
    FilterType ftype;//过滤类型
}DeviceFilter;

/**************************************************************************************************************************************************
  * @brief device_filter_module_set_is_enable_filter
  * 设置是否启用过滤，使用TCAM功能来实现过滤功能,如果开启，则可以使用过滤功能，否则不能使用
  * @param devNum 设备编号
  * @param isEnableFilter 是否启用过滤功能
  * @return
  * MSD_OK - on success.
  * MSD_FAIL - on error.
  * MSD_BAD_PARAM - if invalid parameter is given
  * MSD_FEATRUE_NOT_ALLOW - 如果vlan模式对于端口是不同的，则不允许开启过滤功能
  *************************************************************************************************************************************************/
 MSD_STATUS deviceFilterModuleSetIsEnableFilter(IN MSD_U8 devNum, IN MSD_BOOL isEnableFilter);

 /**************************************************************************************************************************************************
  * @brief device_filter_module_get_is_enable_filter
  * 获取是否启用过滤功能
  * @param devNum 设备编号
  * @param isenableFilter[OUT] 存储是否启用过滤的标志
  * @return
  *  MSD_OK - on success.
  *  MSD_FAIL - on error.
  *  MSD_BAD_PARAM - if invalid parameter is given
  *************************************************************************************************************************************************/
 MSD_STATUS deviceFilterModuleGetIsEnableFilter(IN MSD_U8 devNum, OUT MSD_BOOL* isEnableFilter);

 /*************************************************************************************************************************************************
  * @brief device_filter_module_add_filter 添加或者修改过滤器(只要filter_id或者filter_name存在于过滤器列表中,优先根据过滤器id)
  * @param dev_num 设备编号
  * @param filter_id 如果设置为0，为添加过滤器(如果filter_name不存在)，如果设置为大于0的值，则为要修改的过滤器编号(如果编号不存在，并且filter_name不存在于过滤器列表中,也为添加)
  * @note  如果根据filter_id找到，则filter_name不能和列表中的其他filter相同(即不能存在相同filter_name的过滤器)
  * @param filter_name 要添加的过滤器名称,如果过滤器名称存在，则为修改相应的过滤器(不管filter_id值是否是0)
  * @param ingress_port_vec_bit  要过滤的入口列表,比如如果需要过滤入口端口port 1和port 2，则设置为0b00000110
  * @param etype 出口的过滤行为：丢弃，mirror,forth to other port,不做进一步处理
  * @param egress_port_vec_bit  出口端口集合(如果etype为egress_type_avoid_future_filter,该参数无效)
  * @param f_type 过滤类型
  * @param filter_param 过滤器参数，针对不同的帧类型有不同的值，如果为过滤所有帧，该参数设置为Null
  * @return
  *  MSD_OK - on success.
  *  MSD_FAIL - on error.
  *  MSD_BAD_PARAM - if invalid parameter is given
  *  MSD_FEATRUE_NOT_ALLOW - 没有开启过滤器功能 ,或者检测双标签，但是没有开启QinQ功能(检测双标签需要开启QinQ vlan功能)
  *  MSD_ALREADY_EXIST - 条目已经存在（包括名称存在、过滤入口参数和端口相同的过滤器）
  *
  *************************************************************************************************************************************************/
 MSD_STATUS deviceFilterModuleAddFilter(IN MSD_U8 devNum,IN MSD_U8 filterId,IN const char *filterName,IN MSD_U16 ingressPortVecBit,
                                                       IN EgressType etype,IN MSD_U16 egressPortVecBit,IN FilterType fType,IN FilterParam *filterParam);

 /*************************************************************************************************************************************************
  * @brief device_filter_module_remove_filter_by_id 根据过滤器编号删除过滤器
  * @param dev_num 设备编号
  * @param filter_num 过滤器编号
  * @param is_found 如果要删除的过滤器不存在，则设置为MSD_FALSE,否则设置为MSD_TRUE
  * @return
  *  MSD_OK - on success.
  *  MSD_FAIL - on error.
  *  MSD_FEATRUE_NOT_ALLOW - 没有开启过滤器功能
  *  MSD_BAD_PARAM - if invalid parameter is given
  *************************************************************************************************************************************************/
 MSD_STATUS deviceFilterModuleRemoveFilterById(IN MSD_U8 devNum, IN MSD_U8 filterNum, OUT MSD_BOOL *isFound);

 /*************************************************************************************************************************************************
  * @brief device_filter_module_remove_filter_by_name 根据过滤器名称删除过滤器
  * @param dev_num 设备编号
  * @param filter_name 过滤器名称
  * @return
  *  MSD_OK - on success.
  *  MSD_FAIL - on error.
  *  MSD_FEATRUE_NOT_ALLOW - 没有开启过滤器功能
  *  MSD_BAD_PARAM - if invalid parameter is given
  *************************************************************************************************************************************************/
 MSD_STATUS deviceFilterModuleRemoveFilterByName(IN MSD_U8 devNum,IN const char *filterName,OUT MSD_BOOL *isFound);

 /*************************************************************************************************************************************************
  * @brief device_filter_module_clear_filters 清空所有的过滤器
  * @param dev_num 设备编号
  * @return
  *  MSD_OK - on success.
  *  MSD_FAIL - on error.
  *  MSD_FEATRUE_NOT_ALLOW - 没有开启过滤器功能
  *  MSD_BAD_PARAM - if invalid parameter is given
  *************************************************************************************************************************************************/
 MSD_STATUS deviceFilterModuleClearFilters(IN MSD_U8 devNum);


 /*************************************************************************************************************************************************
 * @brief device_filter_module_find_filter_by_filter_num
 * 根据过滤器id查询过滤器
 * @param dev_num 设备编号
 * @param filter_num 过滤器id
 * @param filter 存放对应的过滤器信息
 * @return
 *   MSD_BAD_PARAM - 参数错误
 *   MSD_FEATRUE_NOT_ALLOW - 没有启用filter功能
 *   MSD_OK  -success
 *************************************************************************************************************************************************/
MSD_STATUS deviceFilterModuleFindFilterByFilterNum(IN MSD_U8 devNum,IN MSD_U8 filterNum,OUT DeviceFilter *filter,OUT MSD_BOOL *isFound);

 /************************************************************************************************************************************************
 * @brief device_filter_module_find_filter_by_filter_name
 * 根据过滤器name查询过滤器
 * @param dev_num 设备编号
 * @param filter_name 过滤器name
 * @param filter 存放对应的过滤器信息
 * @return
 *   MSD_BAD_PARAM - 参数错误
 *   MSD_FEATRUE_NOT_ALLOW - 没有启用filter功能
 *   MSD_OK  -success
 ************************************************************************************************************************************************/
MSD_STATUS deviceFilterModuleFindFilterByFilterName(IN MSD_U8 devNum,IN const char *filterName,OUT DeviceFilter *filter,OUT MSD_BOOL *isFound);

/************************************************************************************************************************************************
  * @brief deviceFilterModuleGetAllFilters 得到所有的过滤器
  * @param dev_num 设备编号
  * @param filter 用以存放所有的的过滤器列表信息
  * @param filterSize filter的大小
  * @param filterCount filter的实际个数
  * @return
  *  MSD_BAD_PARAM - 参数错误
  *  MSD_FEATRUE_NOT_ALLOW - 没有启用filter功能
  *  MSD_NO_SPACE - filter的大小无法存放所有的filter
  *  MSD_OK  -success
  ***********************************************************************************************************************************************/
 MSD_STATUS deviceFilterModuleGetAllFilters(IN MSD_U8 devNum, OUT DeviceFilter *filter,IN int filterSize,OUT int* filterCount);

 /************************************************************************************************************************************************
  * @brief device_filter_module_is_equal_filter_param  该接口仅仅用于测试使用，不使用在生产环境中(比较过滤器参数是否相等)
  * @param param1 要比较的第一个过滤器参数
  * @param param2 要比较的第2个过滤器参数
  * @param filter_type  过滤器类型
  * @return
  * MSD_TRUE:2个过滤器参数相同
  * MSD_FALSE:2个过滤器参数不相同
  ************************************************************************************************************************************************/
 MSD_BOOL deviceFilterModuleIsEqualFilterParam(IN FilterParam *param1, IN FilterParam *param2,OUT FilterType filterType);
#ifdef __cplusplus
}
#endif

