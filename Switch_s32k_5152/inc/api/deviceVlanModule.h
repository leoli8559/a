#pragma once

#include "umsdUtil.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ALL_PORT_PARAM  -1   //代表所有端口
#define ALL_VLAN_PAGE_PARMA -1 //include vlan  page 0 and page 1
#define ALLOW_OPERATION_MAX_VLAN_NUM  64  //可以最多设置出口端口的64个VID的出口方式（不是其成员，Unmodified,Tagged, UnTagged），但是不是说一个端口仅仅只能允许64个VID的帧可以从这个端口出口。
#define UNVALID_VID -1  //无效的VID
#define PORT_DISABLE_VLAN_VID 1//
#define PORT_DEFAULT_VID  2   //默认VID 也是允许操作的最小VID
#define MAX_OPERATION_VID 4095 //允许操作的最大VID
#define MAX_VID_VALUE   4096


/**
 * @brief VLAN_MODEL 启用vlan的方式
 */
typedef enum {
    VLANMODEL_DISABLE,/* 禁用802.1 标签 */
    VLANMODEL_VLAN,/*启用802.1单标签模式 */
    VLANMODEL_QINQ,/* 802.1 ad双标签模式*/
}VlanModel;

/**
 * @brief VlanEgressMode 指定了某个端口，VLAN的出口方式
 */
typedef enum {
    VLAN_EGRESS_MODE_ALLOW_ALL,  /*允许所有VLAN出口端口，当以这种方式指定时，在该端口上,任何VID的MSD_PORT_MEMBER_TAG都不能是MSD_NOT_A_MEMBER。*/
    VLAN_EGRESS_MODE_ALLOW_MEMBERSHIP,/* 允许VTU列表中指定的VLAN出口，该端口可以不是VTU列表中的VID的membership. this is the default value */
    VLAN_EGRESS_MODE_ALLOW_ALL_BUT_EXCLUSION_LIST/*明确排除VTU列表中的部分VID不从指定端口出口（不是membership），VTU列表中的其他VID可以出口，另外未指定VID，以Unmodified方式出口。*/
}VlanEgressMode;


/**
 * @brief FirVlanEntry Fir 类型的交换机的vlan条目结构
 */
typedef struct {
	MSD_VTU_ENTRY entry;//Marvell API提供的VLAN结构体
	//MSD_U8 page;//VTU条目的Page，88Q5152 Fir提供该字段。0/1,目前仅使用page 0，预留字段
}FirVlanEntry;

/**
 * @brief 每个vid的描述
 */
typedef struct{
    MSD_PORT_MEMBER_TAG memberTag[MSD_MAX_SWITCH_PORTS];/* 所有端口的memberTag */
    MSD_16 vid;/* vid,如果大于等于0，代表有效，-1为无效VID，使用-1代表无效的VTU条目，由于可能频繁的删除和添加，使用-1可以消除数组频繁的移动操作。*/
    MSD_BOOL isShow;/*是否前端显示，如果是通过MAC条目添加的VTU条目，不是由VLAN功能主动添加的，不会显示出来，或者本来被VLAN添加了，但是后续又被VLAN操作删除了，但MAC条目需要该条目*/
}VlanPerInfo;

/**
 * @brief 每个端口的出口描述
 */
typedef struct {
    MSD_U16 defaultVid;/*出口端口使用的默认VID*/
    VlanEgressMode egressMode;//端口的vlan出口方式
}VlanPerPortVlanInfo;

/**
 * @brief  EES交换机的方式实现VLAN功能的结构
 */
typedef struct {
    VlanPerPortVlanInfo portVlanInfo[MSD_MAX_SWITCH_PORTS];/*与端口相关的VLAN信息描述， 实际个数由交换机的Port数决定。序号和端口号一一对应，即下标为0的代表端口号0 */
    VlanPerInfo vidInfo[ALLOW_OPERATION_MAX_VLAN_NUM];/* 每个VID的描述 */
    MSD_U32 vidInfoSize;/* 所有操作vid条目的个数 */
}VlanMemberTagAndEgressModeInfo;

/**
 * @brief VLAN配置信息，当交换机重置或者开机时，会从交换机的默认设置或者配置文件中读取。
 */
typedef struct {
    VlanModel vlanModel;/* 是否启用VLAN，或者VLAN方式 */
    MSD_U16  qinqTpid;/* 当使用双标签模式时，会设置这个值，并且当没设置时，会使用默认的QINQ TPID 0x88a8(另外常见的外层标签还有0x9100,88Q5152的默认值也是0x9100) */
    VlanMemberTagAndEgressModeInfo  portVlanMemberTagInfo;/* VLAN功能的结构 */
}VlanConfigInfo;

/******************************************************************************************************************
  * @brief device_vlan_module_add_or_modify_vlan
  *  添加和删除vlan，当存在vlan时，为修改vlan信息
  * @param devNum 要添加设备的编号
  * @param vtuEntry 要添加或者修改的VTU条目
  * @return
  * MSD_OK - on success
  * MSD_FAIL - on error
  * MSD_BAD_PARAM - if invalid parameter is given
  * MSD_NOT_SUPPORTED - device not supported
  * MSD_FEATRUE_NOT_ALLOW - 已经添加到最大VID数目，无法继续添加
  ******************************************************************************************************************/
 MSD_STATUS deviceVlanModuleAddOrModifyVlan(IN MSD_U8 devNum, IN FirVlanEntry* vtuEntry);
 /******************************************************************************************************************
  * @brief device_vlan_module_del_vlan 删除对应id的VLAN条目
  * @param devNum 要删除设备的编号
  * @param vlanId 要删除的VLAN ID
  * @return
  * MSD_OK - on success
  * MSD_FAIL - on error
  * MSD_BAD_PARAM - if invalid parameter is given
  * MSD_NOT_SUPPORTED - device not supported
  * MSD_FEATRUE_NOT_ALLOW(暂时不考虑该返回值，备用) - 不允许删除对应的VLAN条目。可能是该VLAN条目绑定了指定的MAC条目（其VID为该VID值），
  *                         MAC条目可以根据该VID值找到对应的FID值，并找到对应的MAC条目，然后找到指定的出口，如果删除（对于静态条目而言）
  *                         该VID，具有指定VID的帧进入条目时会使用端口的默认的FID，这样查询的MAC地址条目就可能不对。从而导致出口端口错误
  ******************************************************************************************************************/
 MSD_STATUS deviceVlanModuleDelVlan(IN MSD_U8 devNum, IN MSD_U16 vlanId);
 /******************************************************************************************************************
  * @brief device_vlan_module_clear_all_vlans 清空数据库中的所有VLAN条目
  * @param devNum 设备编号
  * @return
  * MSD_OK - on success
  * MSD_FAIL - on error
  * MSD_BAD_PARAM - if invalid parameter is given
  * MSD_NOT_SUPPORTED - device not supported
  ******************************************************************************************************************/
 MSD_STATUS deviceVlanModuleClearAllVlans(IN MSD_U8 devNum);
 /********************************************************************************************************************
  * @brief device_vlan_module_get_all_vlans
  *  获取所有的VLAN条目，条目列表存在在vtu_entries参数中,page可以指定是否在指定的page中查询条目
  *  (目前仅使用Page 0的条目，因此也只显示Page 0的条目）
  * @param dev_num 设备编号
  * @param vtuEntries 要获取的VTU条目，由调用者开辟空间
  * @param vtuEntrySize tu_entries条目的最大可存放的条数
  * @param vtuEntryCount VTU条目的实际条数，存放在vtu_entries参数的0到vtu_entry_count-1下标的位置
  * @return
  *  MSD_OK - on success
  *  MSD_FAIL - on error
  *  MSD_BAD_PARAM - if invalid parameter is given
  *  MSD_NOT_SUPPORTED - device not supported
  ******************************************************************************************************************/
 MSD_STATUS deviceVlanModuleGetAllVlans(IN MSD_U8 devNum, /*IN int page,*/ OUT FirVlanEntry* vtuEntries, IN MSD_U16 vtuEntrySize, OUT MSD_U16* vtuEntryCount);

 /******************************************************************************************************************
  * @brief device_vlan_module_find_vlan 查询Page 0或者Page 1中具有VLANID的vtu条目
  * @param devNum 设备编号
  * @param vtuEntry 存储查询到的VTU条目,同时作为查询的输入条目
  * @param isFound 是否查询到的标志
  * @return
  * MSD_OK - on success.
  * MSD_FAIL - on error.
  * MSD_BAD_PARAM - if invalid parameter is given
  ******************************************************************************************************************/
 MSD_STATUS deviceVlanModuleFindVlan(IN MSD_U8 devNum, INOUT FirVlanEntry* vtuEntry, OUT MSD_BOOL* isFound);

 /******************************************************************************************************************
  * @brief device_vlan_module_set_q_mode_is_global
  *  设置Q_MODE是否为全局模式，只有全局模式时，才允许设置所有端口设置为相同对的Q Mode模式，否则只能对某个端口进行设置。
  *  如果isGlobal参数为MSD_TRUE,则将所有端口的Q_MODEL设置为model模式
  * @param devNum 要设置的设备编号
  * @param isGlobal 是否设置为全局模式
  * @param model 如果为全局模式，则需要指定设置的Q MODEL
  * @return
  * MSD_OK - on success
  * MSD_FAIL - on error
  * MSD_BAD_PARAM - if invalid parameter is given
  * MSD_NOT_SUPPORTED - device not supported
  * MSD_FEATRUE_NOT_ALLOW - model设置和is_global参数的混合设置不被允许,see device_vlan_module_set_q_mode
  *******************************************************************************************************************/
 MSD_STATUS deviceVlanModuleSetQModeIsGlobal(IN MSD_U8 devNum,IN MSD_BOOL isGlobal, IN VlanModel model);

 /*******************************************************************************************************************
  * @brief device_vlan_module_get_q_mode_is_global
  * 获取Q_MODE是否为全局模式，只有全局模式时，才允许设置所有端口设置为相同对的Q Mode模式，否则只能对某个端口进行设置。
  * @param devNum
  * @param isGlobal 存放是否为全局模式的标志
  *******************************************************************************************************************/
 MSD_STATUS deviceVlanModuleGetQModeIsGlobal(IN MSD_U8 devNum, OUT MSD_BOOL *isGlobal);

 /*******************************************************************************************************************
  * @brief device_vlan_module_set_q_mode
  * 设置交换机的VLAN模式，启用VLAN标签，双标签和禁用标签模式，88Q5152可以针对某个端口设置VLAN模式，也可以针对交换机所有端口设置
  * (port_num设置为-1，EES交换机目前就使用该模式),当port_num为-1时，所有端口都设置为同一种Q模式。如果port_num设置为具体的端口号，
  * 则设置为具体端口号的模式（仅仅在非全局模式下起作用）,注：如果mode = Q_QINQ_MODEL 默认会丢弃不满足Provider Tag条件的帧，
  * 如果想将帧发送给具体的端口，请调用device_vlan_module_set_provider_port_deal_with_no_provider_tag_frame
  * 如果在非全局模式下，端口类型仅可以是Q_QINQ_MODE与Q_DISABLE_MODE组合,Q_DISABLE_MODE与Q_VLAN_MODE组合,
  * 或者所有端口都是同一种模式，不允许其他混合模式的组合。
  * @param dev_num 设备编号
  * @param port_num 端口编号，设置为-1时，表示设置交换机的所有端口(需要在全局模式下),大于等于0时，设置具体端口(需要为非全局模式下)
  * @param model 要设置的模式
  * @return
  * MSD_OK - on success
  * MSD_FAIL - on error
  * MSD_BAD_PARAM - if invalid parameter is given
  * MSD_NOT_SUPPORTED - device not supported
  * MSD_FEATRUE_NOT_ALLOW - 操作不被允许(全局模式下，设置某个端口的Tag mode；端口模式下，设置所有端口的Tag mode),
  * 在端口模式下，端口可以使用不同模式(QinQ 和 Disable Mode组合，Disable Mode 和 VLAN Mode组合， 或者任意一种模式，其他混合模式都不允许)，
  * 因此任何导致这种违反的案例，都不允许将指定的port_num设置为相应的model模式。
  *******************************************************************************************************************/
 MSD_STATUS deviceVlanModuleSetQMode(IN MSD_U8 devNum,IN int portNum,IN VlanModel model);

 /******************************************************************************************************************
  * @brief device_vlan_module_get_q_mode
  * 获取交换机的VLAN模式：启用VLAN标签，双标签和禁用标签模式,如果为全局模式，则所有的端口model都相同，如果模式为非全局模式，portum应该大于0
  * @param devNum 设备编号
  * @param portNum 端口编号，设置为-1时，获取全局的Q标签模式，大于0时，则获取某个端口的标签模式
  * @param model 用于存储交换机的标签模式
  * @return
  * MSD_OK - on success
  * MSD_BAD_PARAM - if invalid parameter is given
  ******************************************************************************************************************/
 MSD_STATUS deviceVlanModuleGetQMode(IN MSD_U8 devNum, IN int portNum, OUT VlanModel* model);

 /******************************************************************************************************************
  * @brief device_vlan_module_set_tpid
  * 设置设备的各种EtherType值，EtherType类型由type参数决定，比如VLAN的EtherType,MSD_ETHERTYPE_PROVIDER等。
  * @param dev_num 要设置设备的Dev num
  * @param port_num 要设置的端口编号
  * @param type 要设置的ethertype类型
  * @param e_type 要设置EtherType的值
  * @return
  * MSD_OK - on success
  * MSD_FAIL - on error
  * MSD_BAD_PARAM - if invalid parameter is given
  * MSD_NOT_SUPPORTED - device not supported
  ******************************************************************************************************************/
 MSD_STATUS deviceVlanModuleSetTpid(IN MSD_U8 devNum, IN MSD_U8 portNum, IN  MSD_ETHERTYPE type, IN MSD_U16 eType);
 /******************************************************************************************************************
  * @brief device_vlan_module_get_tpid
  * 获取设备的各种EtherType值，EtherType类型由type参数决定，比如VLAN的EtherType,MSD_ETHERTYPE_PROVIDER等。
  * @param devNum 要获取设备的Dev num
  * @param portNum 要获取的端口编号
  * @param type 要获取的ethertype类型
  * @param eType 返回的EtherType类型值
  * @return
  * MSD_OK - on success
  * MSD_FAIL - on error
  * MSD_BAD_PARAM - if invalid parameter is given
  * MSD_NOT_SUPPORTED - device not supported
  ******************************************************************************************************************/
 MSD_STATUS deviceVlanModuleGetTpid(IN MSD_U8 devNum, IN MSD_U8 portNum, IN  MSD_ETHERTYPE type,  OUT MSD_U16* eType);

 /******************************************************************************************************************
  * @brief device_vlan_module_set_8021_mode
  *  设置设备端口的802.1 Q Mode
  *  如果port_num为-1，则设置所有端口的802.1 Q模式（需要VLAN 设置为全局模式），如果port_num为具体的端口，则设置某个端口的802.1 Q模式，需要VLAN 设置为非全局模式。
  *  设置是否成功以及副作用，请参考说明。
  *  注：1.在全局模式下(设置交换机的所有端口时)：
  *     1.1 要设置的mode_802.1为Disable,并且Q Mode为QinQ模式，则不允许设置
  *     1.2 要设置的mode_802.1为Disbale,并且Q Mode为Q_VLAN（单标签）模式，则会将所有的端口Q Mode设置为Q_DISABLE_MODEL
  *     1.3 要设置的mode_802.1为Disable,并且Q_Mode为Q_DISABLE_MODEL，不做任何操作
  *     1.4 要设置的mode_802.1为Fallback,Secure,Check任何一种时，并且Q Mode为Q_DISABLE_MODEL，则会将所有端口设置Q Mode设置为Q_VLAN_MODEL
  *     2.在设置某一个端口模式时：
  *     2.1 要设置的mode_802.1为Disable时，如果当前要设置的端口为Q Mode为Q_QinQ_MODE模式时，则不允许设置（QinQ默认需要使用802.1Q模式）
  *     2.2 要设置的mode_802.1为Disable时，如果当前要设置的端口为Q Mode为Q_VLAN_MODE模式时，则会将所有端口的Q_MODE设置为Q_DISABLE_MODEL
  *     2.3 要设置的mode_802.1为Disable时，如果当前要设置的端口为Q Mode为Q_DISABLE_MODE模式时,不做任何操作
  *     2.4 要设置的mode_802.1为Fallback,Secure,Check任何一种时，如果存在Provider Port时，并且要设置的端口不是Provider Port时，不允许设置802.1 Q Mode（有Provider Port时，非Provider Port不允许设置）
  *     2.5 要设置的mode_802.1为Fallback,Secure,Check任何一种时，如果不存在Provider Port时，并且要设置的端口的Q Mode为Q_DISABLE_MODEL，会将Q Mode设置为Q_VLAN_MODEL
  * @param devNum 设备编号
  * @param portNum 端口编号(0~9)
  * @param mode8021 要设置的802.1 Q Mode
  * @return
  * MSD_OK - on success
  * MSD_FAIL - on error
  * MSD_BAD_PARAM - if invalid parameter is given
  * MSD_NOT_SUPPORTED - device not supported
  * MSD_FEATRUE_NOT_ALLOW - 不允许设置802.1Q mode
  ******************************************************************************************************************/
 MSD_STATUS deviceVlanModuleSet8021Mode(IN MSD_U8 devNum, IN int portNum, IN MSD_8021Q_MODE mode8021);

 /******************************************************************************************************************
  * @brief device_vlan_module_get_8021_mode
  *  获取端口的802.1 Q Mode,一次获取一个端口的802.1 Q Mode.
  * @param devNum 设备编号
  * @param portNum 端口编号
  * @param mode8021 存放返回的802.1 Q Mode
  * @return
  * MSD_OK - on success
  * MSD_FAIL - on error
  * MSD_BAD_PARAM - if invalid parameter is given
  * MSD_NOT_SUPPORTED - device not supported
  ******************************************************************************************************************/
 MSD_STATUS deviceVlanModuleGet8021Mode(IN MSD_U8 devNum, IN MSD_U8 portNum, OUT MSD_8021Q_MODE *mode8021);
 /******************************************************************************************************************
  * @brief device_vlan_module_set_default_vlan_id
  * 设置端口的默认VID（不启用802.1Q或者帧不带标签时会使用该VID）
  * 默认VID,PRI字段，不需要支持802.1Q Mode，需要判断默认VID是否合法
  * @param devNum 设备编号
  * @param portNum 端口编号
  * @param defaultVid 默认vid
  * @return
  * MSD_OK - on success
  * MSD_FAIL - on error
  * MSD_BAD_PARAM - if invalid parameter is given
  * MSD_NOT_SUPPORTED - device not supported
  ******************************************************************************************************************/
 MSD_STATUS deviceVlanModuleSetDefaultVlanId(IN MSD_U8 devNum, IN MSD_U8 portNum,IN MSD_U16 defaultVid);

 /******************************************************************************************************************
  * @brief device_vlan_module_get_default_vlan_id 获取不带标签帧的默认VID
  * @param devNum 设备编号
  * @param portNum 端口编号
  * @param defaultVid 存放返回的默认VID值
  * @return
  * MSD_OK - on success
  * MSD_FAIL - on error
  * MSD_BAD_PARAM - if invalid parameter is given
  * MSD_NOT_SUPPORTED - device not supported
  ******************************************************************************************************************/
 MSD_STATUS deviceVlanModuleGetDefaultVlanId(IN MSD_U8 devNum, IN MSD_U8 portNum, OUT MSD_U16 *defaultVid);
 /******************************************************************************************************************
  * @brief device_vlan_module_set_default_pri
  * 设置不带标签帧的默认PRI 注：如果是不带标签的IPv4/IPv6，则由IPv4的DSCP或IPv6的TOS决定。
  * @param devNum 设备编号
  * @param portNum 端口编号
  * @param defaultPri 要设置的默认PRI值,range is 0x0 ~ 0x7
  * @return
  * MSD_OK - on success
  * MSD_FAIL - on error
  * MSD_BAD_PARAM - if invalid parameter is given
  * MSD_NOT_SUPPORTED - device not supported
  ******************************************************************************************************************/
 MSD_STATUS deviceVlanModuleSetDefaultPri(IN MSD_U8 devNum, IN MSD_U8 portNum,IN MSD_U8 defaultPri);
 /******************************************************************************************************************
  * @brief device_vlan_module_get_default_pri 获取不带标签帧的默认PRI
  * @param devNum 设备编号
  * @param portNum 端口编号
  * @param defaultPri 存放获取的默认PRI值
  * @return
  * MSD_OK - on success
  * MSD_FAIL - on error
  * MSD_BAD_PARAM - if invalid parameter is given
  * MSD_NOT_SUPPORTED - device not supported
  *****************************************************************************************************************/
 MSD_STATUS deviceVlanModuleGetDefaultPri(IN MSD_U8 devNum, IN MSD_U8 portNum, OUT MSD_U8* defaultPri);

 /*****************************************************************************************************************
  * @brief device_vlan_module_set_initailPri_and_TagIfBoth
  * 设置Qos的Initial Priority Selection,同device_vlan_module_set_default_pri一起,可以控制帧出口的优先级
  * @note 具体参考功能规范Qos 2.2.4.4
  * @param dev_num 设备编号
  * @param port_num 端口编号
  * @param initial_pri 使用何种初始的优先级
  * @param tag_if_both_is_set 优先级设置是否需要考虑Tag标签
  * @return
  * MSD_OK - on success
  * MSD_FAIL - on error
  * MSD_BAD_PARAM - if invalid parameter is given
  * MSD_NOT_SUPPORTED - device not supported
  *****************************************************************************************************************/
 MSD_STATUS deviceVlanModuleSetInitailPriAndTagIfBoth(IN MSD_U8 devNum, IN MSD_U8 portNum, IN MSD_INITIAL_PRI initialPri, IN MSD_BOOL tagIfBothIsSet);

 // @brief device_vlan_module_get_initailPri_and_TagIfBoth
// 获取Qos的Initial Priority Selection,具体作用，参考device_vlan_module_set_initailPri_and_TagIfBoth
// @param devNum 设备编号
// @param portNum 端口编号
// @param initialPri 存储使用何种初始的优先级
// @param tagIfBothisSet 存储优先级设置是否需要考虑Tag标签
// @return
// MSD_OK - on success
// MSD_FAIL - on error
// MSD_BAD_PARAM - if invalid parameter is given
// MSD_NOT_SUPPORTED - device not supported
 MSD_STATUS deviceVlanModuleGetInitailPriAndTagIfBoth(IN MSD_U8 devNum, IN MSD_U8 portNum, OUT MSD_INITIAL_PRI* initialPri, OUT MSD_BOOL* tagIfBothIsSet);
 /*****************************************************************************************************************
  * @brief device_vlan_module_set_vlan_egress_mode_and_member_tag_info
  * 设置端口VID的memberTag信息及其端口的vlan出口方式和端口的默认VID
  * 端口对vlan的处理方式如下：
  * 1：allow all vid:所有vid都允许出口，但是最多允许对ALLOW_OPERATION_MAX_VLAN_NUM VID 添加到VTU数据库中，其他没有添加到VTU条目中都以unmodified方式传输
  * 2：allow memberlist:允许对添加的vid做操作（不允许出口和以各种方式(unmodified,untagged,tagged)出口）
  * 3: allow all but not exclusion:允许所有vid，但是可以排除指定的vid列表。
  *  注：如果VID还不属于VTU条目，在还没达到最多VID条目列表时，会添加到VTU条目中
  * @param dev_num 设备编号
  * @param port_vlan_member_tag 本次要修改的所有端口及其所有vid的memberTag信息
  * @return
  *  MSD_OK - on success
  *  MSD_FAIL - on error
  *  MSD_BAD_PARAM - if invalid parameter is given
  *  MSD_NOT_SUPPORTED - device not supported
  *  MSD_FEATRUE_NOT_ALLOW:
  *     (1)已经添加到最大VID数目，无法继续添加
  *     (2)当前处于非全局模式下，该API目前仅仅支持全局模式下的设置
  *     (3)全局模式下，Q Mode is Disable VLAN
  *****************************************************************************************************************/
 MSD_STATUS deviceVlanModuleSetVlanEgressModeAndMemberTagInfo(IN MSD_U8 devNum, IN VlanMemberTagAndEgressModeInfo* portVlanMemberTag/*, MSD_BOOL is_reset*/);
 /*****************************************************************************************************************
  * @brief device_vlan_module_get_vlan_egress_mode_member_tag_info
  * 获取端口VID的memberTag信息及其端口的vlan处理方式，参考device_vlan_module_set_member_tag_info的描述。
  * @param devNum 设备编号
  * @param allPortVlanMemberTag 所有端口的所有VID的memberTag信息及其端口的VLAN处理方式
  * @return
  * MSD_OK - on success
  * MSD_BAD_PARAM - if invalid parameter is given
  ******************************************************************************************************************/
 MSD_STATUS deviceVlanModuleGetVlanEgressModeMemberTagInfo(IN MSD_U8 devNum, OUT VlanMemberTagAndEgressModeInfo* allPortVlanMemberTag);

 /******************************************************************************************************************
  * @brief deviceVlanModuleSetProviderPortDealWithNoProviderTagFrame
  *  该接口目前没有进行测试，不知道是否需要，默认情况下，provider port 不会对非provider 帧做任何处理
  *  目前API代码已注释，如需要，自行打开
  * 设置Provider Port 处理非Provider Tag帧的处理方式，当ports_size为0或者ports为NULL时，直接丢弃帧。否则port_num的帧仅可能发送给ports指定的端口列表，如果的确该发送给这些端口(根据MAC地址表)。
  * @param devNum 设备编号
  * @param portNum 要设置的Provider Port端口号，如果不是Provider Port，则不做任何处理
  * @param fid 操作的MAC的FID，一般情况下，由device_atu_module_get_fid获取大于0的fid，如果获取为-1，则由用户指定
  * @param ports 端口号列表（比如需要发送给Port 1和Port 2，则列表为1和2）
  * @param portsSize ports参数的个数
  * @return
  * MSD_FEATRUE_NOT_ALLOW: 不允许的操作，比如port_num不是Provider Port
  * MSD_OK - on success
  * MSD_FAIL - on error
  * MSD_BAD_PARAM - if invalid parameter is given
  *******************************************************************************************************************/
 //MSD_STATUS deviceVlanModuleSetProviderPortDealWithNoProviderTagFrame(IN MSD_U8 devNum, IN int portNum, IN int fid, IN MSD_LPORT *ports, IN int portsSize);

 /******************************************************************************************************************
  * @brief device_vlan_module_get_mode_port
  * 根据某个Q Model的所有端口号，结果存储在ports参数中，端口个数存放在ports_count中
  * @param devNum 设备编号
  * @param ports 存放端口号列表
  * @param portsSize ports的空间大小
  * @param portsCount ports的实际个数
  * @param model 要查询的Q Model
  * @return
  *  MSD_BAD_PARAM:参数错误
  *  MSD_OK:success
  *  MSD_NO_SPACE:ports的空间不足以容纳所有的端口号信息
  ******************************************************************************************************************/
 MSD_STATUS deviceVlanModuleGetModePort(IN MSD_U8 devNum, OUT int* ports, IN int portsSize, OUT int *portsCount, IN VlanModel model);


///ees交换机接口类型 start  
//ees交换机所有端口都使用相同的VLAN模式，即要么都使用VLAN(QinQ),要么都不使用VLAN
 /******************************************************************************************************************
  * @brief device_vlan_module_add_or_modify_vlan_with_vid_and_member_tag_to_config
  *  使用VID和memberTag信息添加VTU条目到VLAN配置中
  *  注：该设置并不会网交换机中保存VTU条目，仅仅保存到配置中
  *  如果 memberTagSize小于交换机的端口数，则vid的memberTag默认为MSD_MEMBER_EGRESS_UNMODIFIED
  * @param devNum 设备编号
  * @param vid 条目的VID
  * @param memberTag 条目的memberTag列表，如果为null，则将所有的memebrTag设置为MSD_MEMBER_EGRESS_UNMODIFIED
  * @param memberTagSize: member_tag size
  * @return
  * MSD_OK - on success
  * MSD_FEATRUE_NOT_ALLOW - 配置中的VTU条目达到最大值，不允许添加VTU条目
  * MSD_BAD_PARAM - if invalid parameter is given
  ******************************************************************************************************************/
 MSD_STATUS deviceVlanModuleAddOrModifyVlanWithVidAndMemberTagToConfig(IN MSD_U8 devNum, IN MSD_U16 vid, IN MSD_PORT_MEMBER_TAG memberTag[MSD_MAX_SWITCH_PORTS],
                                                                                    IN MSD_U8 memberTagSize);
 /******************************************************************************************************************
  * @brief device_vlan_module_remove_vlan_from_config
  * 将指定的VID从VID 条目中从VTU 配置中移除。
  * @param devNum 设备编号
  * @param vid vid值
  * @return
  * MSD_OK - on success
  * MSD_BAD_PARAM - if invalid parameter is given
  ******************************************************************************************************************/
 MSD_STATUS deviceVlanModuleRemoveVlanFromConfig(IN MSD_U8 devNum, IN MSD_U16 vid);
 /******************************************************************************************************************
  * @brief device_vlan_module_clean_all_vlan_and_member_tag_from_config
  *  清空缓存中VTU配置
  * @param devNum 设备编号
  * @param isResetDefaultVid 是否重置默认的VID(默认VID为2),如果传递了一个错误值，则默认为MSD_TRUE
  * @param isResetVlanEgressMode 是否重置默认的VLAN EGRESS MODE(默认为Allow membership),如果传递了一个错误值，则默认为MSD_TRUE
  * @return
  * MSD_OK - on success
  * MSD_BAD_PARAM - if invalid parameter is given
  ******************************************************************************************************************/
 MSD_STATUS deviceVlanModuleCleanAllVlanAndMemberTagFromConfig(IN MSD_U8 devNum, MSD_BOOL isResetDefaultVid, MSD_BOOL isResetVlanEgressMode);

 /******************************************************************************************************************
  * @brief device_vlan_module_set_q_mode_and_qinq_tpid_to_config
  * 设置QINQ TPID和VLAN模式到VLAN配置中
  * @param devNum 设备编号
  * @param qinqTpid QINQ TPID VALUE
  * @param vlanModel 标签模式
  * @return
  * MSD_OK - on success
  * MSD_BAD_PARAM - if invalid parameter is given
  ******************************************************************************************************************/
 MSD_STATUS deviceVlanModuleSetQModeAndQinqTpidToConfig(IN MSD_U8 devNum, IN MSD_U16 qinqTpid, IN VlanModel vlanModel);

 /******************************************************************************************************************
  * @brief device_vlan_module_export_all_vlan_and_member_tag
  * 获取所有的VID信息和所有的端口出口方式及其默认VID
  * @param devNum 设备编号
  * @param vlanInfo 存储导出的VLAN及其端口出口方式
  * @return
  * MSD_OK - on success
  * MSD_BAD_PARAM - if invalid parameter is given
  ******************************************************************************************************************/
 MSD_STATUS deviceVlanModuleExportAllVlanAndMemberTag(IN MSD_U8 devNum, OUT VlanMemberTagAndEgressModeInfo *vlanInfo);

 // @brief device_vlan_module_import_all_vlan_and_member_tag
// 导入所有的VID信息和所有的端口出口方式及其默认VID
// 清空已有的vid配置，并将配置替换为vlan_info提供的参数
// @param devNum 设备编号
// @param vlanInfo VLAN描述信息
// @return
// MSD_OK - on success
// MSD_BAD_PARAM - if invalid parameter is given
 MSD_STATUS deviceVlanModuleImportAllVlanAndMemberTag(IN MSD_U8 devNum, IN VlanMemberTagAndEgressModeInfo* vlanInfo);

 /******************************************************************************************************************
  * @brief device_vlan_module_get_vlan_config_info
  * 获取当前交换机的VLAN配置信息，结果保存到vlan_config_info参数中, vid参数存储在前all_operation_vid_info_size的下标中
  *  注：初始化时会获取初始化的VLAN配置。即使vlan信息的添加，比如添加Vlan，修改qinq tpid也不会更新
  *  如果想获取最新的交换机的实际配置(该接口可用于获取临时保存的vlan配置信息)，可以调用device_vlan_module_refresh_vlan_config_info
  * @param devNum 设备编号
  * @param vlanConfigInfo 用于保存当前的VLAN配置信息
  * @return
  ******************************************************************************************************************/
 MSD_STATUS deviceVlanModuleGetVlanConfigInfo(IN MSD_U8 devNum, OUT VlanConfigInfo *vlanConfigInfo);

 /******************************************************************************************************************
  * @brief device_vlan_module_refresh_vlan_config_info
  * 获取当前交换机的VLAN配置信息.与device_vlan_module_get_vlan_config_info的区别是会从保存的配置中读取。
  * @param devNum  设备编号
  * @param vlanConfigInfo Vlan配置信息
  * @return
  * MSD_OK - on success
  * MSD_BAD_PARAM - if invalid parameter is given
  * MSD_FEATRUE_NOT_ALLOW - vlan model为非全局模式，操作不被允许
  * MSD_FAIL  - on error
******************************************************************************************************************/
 MSD_STATUS deviceVlanModuleRefreshVlanConfigInfo(IN MSD_U8 devNum, OUT VlanConfigInfo* vlanConfigInfo);

 /******************************************************************************************************************
  * @brief device_vlan_module_save_vlan_config_info 将交换机的配置信息保存到交换机中。
  * @note 如果交换机不是全局模式，会先被设置为全局模式，将所有端口都设置为同一Vlan模式
  * @param devNum 设备编号
  * @return
  *  MSD_OK - on success
  *  MSD_FAIL - on error
  *  MSD_BAD_PARAM - if invalid parameter is given
  *  MSD_NOT_SUPPORTED - device not supported
  *  MSD_FEATRUE_NOT_ALLOW:
  *     (1)已经添加到最大VID数目，无法继续添加
  *     (2)当前处于非全局模式下，该API目前仅仅支持全局模式下的设置
  *     (3)全局模式下，Q Mode is Disable VLAN
  ******************************************************************************************************************/
 MSD_STATUS deviceVlanModuleSaveVlanConfigInfo(IN MSD_U8 devNum);

/// ees交换机接口类型 end

#ifdef __cplusplus
}
#endif

