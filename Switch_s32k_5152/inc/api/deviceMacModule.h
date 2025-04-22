#pragma once

#include "internal/umsdUtil.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief AtuEntryType 获取的ATU条目类型
 *
 */
typedef enum {
    ATU_ENTRY_TYPE_STATIC_MULTICAST = 1,/* 1-静态多播条目 */
    ATU_ENTRY_TYPE_STATIC_UNICAST = 2,/*2-静态单播条目*/
    ATU_ENTRY_TYPE_STATIC_BROCASTCAST = 3,/* 3-静态广播条目 */
    ATU_ENTRY_TYPE_AUTO = 4, /* 4-动态条目(仅仅单播的情况) */
    ATU_ENTRY_TYPE_UNUSED_OR_RESERVED_MULTICAST = 5,/*  5-未使用或保留的多播条目 */
    ATU_NETRY_TYPE_UNUSED_OR_RESERVED_UNICAST = 6, /*6-未使用或保留的单播条目 */
    ATU_NETRY_TYPE_UNUSED_OR_RESERVED_BROCASTCAST = 7,/* 7-未使用或保留的广播条目 */
    ATU_ENTRY_TYPE_ECID_MULTICAST = 8, /* 8- e-cid 多播条目 */
    ATU_ENTRY_TYPE_ECID_BROCASTCAST = 9, /*9- e-cid 广播条目 */
    ATU_ENTRY_TYPE_STATIC = 10, /* 10- 所有的静态ATU条目(1,2,3的组合) */
    ATU_ENTRY_TYPE_ALL = 11, /* 11- 所有的ATU条目 */
    ATU_ENTRY_TYPE_ALL_VALID = 12,/* 12- 所有的有效ATU条目(除5,6,7以外的条目) */
    ATU_ENTRY_TYPE_ALL_INVALID = 13,/* 13- 无效条目（5,6,7的组合） */
}AtuEntryType;

#define  ATU_ENTRY_TYPE_SIZE 13

#define  MAX_AGE_OUT_MINUTE  10 //静态条目的最大存活时间（minute）

#define  MAX_STATIC_ATU_ENTRIES   64   //交换机可以存储的最大静态ATU条目个数
#define  MAX_AUTO_ATU_ENTRIES  4096   //在没有静态ATU条目的情况下，交换机最多可以存储的动态ATU条目个数，Function 2.10.3.1中描述

#define MAX_FID_VALUE   4095

#define  ALL_FID_VALUE  -1  //ATU操作全部FID

/// <summary>
/// 存放MAC地址条目的结构,和交换机的MAC地址条目结构有很大不同
/// </summary>
typedef struct {
    MSD_U32 portVec;//端口vec，比如port 1和Port 2，那么port_vec为0x6.//最好用16进制表示，4位表示4个端口，好计算。
    MSD_U16 vid;//条目对应的VID.
    MSD_ETHERADDR address;//mac地址
//    MSD_BOOL isStatic;//如果为真，则为静态，否则为动态
//    MSD_BOOL isUnicast;//如果为真，为单播地址，否则为多播地址
//    MSD_BOOL isVaild;//是否有效的标志，如果无效，则数据属性可以被替换
    //int age;//动态条目的Age (1-7)
    MSD_U8  ageAndFlag;//bit 0:静态条目或者动态条目的标志（1，静态条目，0-动态条目），bit 1:单播或者多播（1-单播条目，0-多播条目），bit 2:是否有效的标志（1-有效，0-无效） bit 3 ~ bit 5(动态条目的age 1- 7)
}MacEntry;

typedef enum {
    USE_MAC_ENTRY_TYPE_STATIC_AND_DYNAMIC = 0,//允许端口自动学习，默认方式
    USE_MAC_ENTRY_TYPE_ONLY_STATIC, //仅仅使用静态条目，禁止端口自动学习
}UseMacEntryType;


#define MAC_IS_EQUAL(mac1,mac2) (((mac1).arEther[0] == (mac2).arEther[0] ) &&  \
((mac1).arEther[1] == (mac2).arEther[1]) && \
((mac1).arEther[2] == (mac2).arEther[2]) && \
((mac1).arEther[3] == (mac2).arEther[3]) && \
((mac1).arEther[4] == (mac2).arEther[4]) && \
((mac1).arEther[5] == (mac2).arEther[5]))

/**
 * ATU配置结构信息
 */
typedef struct {
    UseMacEntryType useType;/* 使用地址条目的类型 */
    MSD_U8 ageOutMinute;//超时时间（以min为单位），最大支持10分钟
    MacEntry staticMacEntry[MAX_STATIC_ATU_ENTRIES];//静态MAC条目
    MSD_U8 staticMacEntryCount;//static_mac_entry的实际个数
}AtuConfiguration;

/********************************************************************************
  * @brief device_atu_module_set_fid
  * 设置MAC条目使用的FID，如果FID设置为ALL_FID_VALUE，则可以使用多FID（默认值），
  * 如果FID大于0，则使用指定的FID.
  * @Note: 如果在atu操作之前，调用了该接口,建议在具体的操作之后，再次将FID设置为ALL_FID_VALUE
  * @param dev_num 要设置的设备编号
  * @param fid 要使用的FID值
  * @return MSD_OK,设置成功，MSD_BAD_PARAM:dev_num和fid值参数错误
  *******************************************************************************/
 MSD_STATUS deviceAtuModuleSetFid(IN MSD_U8 devNum,IN short fid);

 /********************************************************************************
  * @brief device_atu_module_get_fid
  * 获取MAC条目使用的FID
  * @param dev_num 要设置的设备编号
  * @param fid 使用的FID值
  * @return
  * MSD_OK:success
  * MSD_BAD_PARAM:dev_num参数错误
  ********************************************************************************/
 MSD_STATUS deviceAtuModuleGetFid(IN MSD_U8 devNum,IN MSD_U16 *fid);

 /********************************************************************************
  * @brief deviceAtuModuleSetPortDefaultFid 设置端口号的默认FID Number,如果port_num为-1.则设置所有的端口的FID值
  * @param devNum 设备编号
  * @param portNum 端口编号，-1为设置所有端口的FID值为相同的FID值
  * @param fid FID值
  * @return
  * MSD_OK - On success
  * MSD_FAIL - On error
  * MSD_BAD_PARAM - If invalid parameter is given
  * MSD_NOT_SUPPORTED - Device not support
  ********************************************************************************/
 MSD_STATUS deviceAtuModuleSetPortDefaultFid(IN MSD_U8 devNum,IN int portNum,IN MSD_U16 fid);

 /********************************************************************************
  * @brief deviceAtuModuleGetPortDefaultFid 获取端口号的默认FID Number
  * @param dev_num 设备编号
  * @param port_num 端口编号
  * @param fid FID值
  * @return
  * MSD_OK - On success
  * MSD_FAIL - On error
  * MSD_BAD_PARAM - If invalid parameter is given
  ********************************************************************************/
 MSD_STATUS deviceAtuModuleGetPortDefaultFid(IN MSD_U8 devNum,IN MSD_U8 portNum,OUT MSD_U16* fid);

 /********************************************************************************
  * @brief device_atu_module_add_entry
  * 手动添加MAC地址条目,通常指添加静态条目，但是也可以手动添加动态条目(一般为测试使用，一段时间会从自动从数据库中删除)
  * 这里添加的MAC地址条目不会涉及到VID相关的操作。添加的FID决定数据库属于哪个多数据库编号，查询时也要指定相应的FID
  * 来进行查询。在开启VLAN功能时，会根据帧的VID查询VTU条目，决定相应VTU条目的FID值，不存在相应的VID条目时，
  * 则会使用端口Port offset 0x5 & 0x6的FID值来决定查询哪个FID的数据库。如果不存在时，则会泛洪到除入口端口的其他
  * 所有端口，如果存在，则会发送到MAC条目指定的端口列表。使用device_atu_module_add_vid_static_entry将VID和MAC地址进行关联，
  * 即同一个MAC地址，但是不同的VID被视为不同的MAC Entry条目，这是将VID条目的FID设置为VID值，然后在添加对应FID值的MAC条目。
  * 这样，就能优先根据VID值决定使用哪个FID的MAC地址条目。
  * @note: 这里添加的静态条目不会添加到配置信息中。
  * @param devNum 设备Num,从0开始
  * @param macEntry 要添加的MAC地址条目
  * @return
  * MSD_OK - On success
  * MSD_FAIL - On error
  * MSD_BAD_PARAM - If invalid parameter is given
  * MSD_NOT_SUPPORTED - Device not support
  ********************************************************************************/
 MSD_STATUS deviceAtuModuleAddEntry(IN MSD_U8 devNum,IN MSD_ATU_ENTRY* macEntry);

 /********************************************************************************
  * @brief device_atu_module_aging_timeout_set
  * 设置动态条目的超时时间,最小值为0(动态条目永久不删除)，最大值为MAX_AGE_OUT_MINUTE
  * @param devNum 设备Num
  * @param ageTimeMinute 超时时间,以分钟为单位(最大值为10min)
  * @return
  * MSD_OK - On success
  * MSD_FAIL - On error
  * MSD_BAD_PARAM - If invalid parameter is given
  * MSD_NOT_SUPPORTED - Device not support
  ********************************************************************************/
 MSD_STATUS deviceAtuModuleAgingTimeoutSet(IN MSD_U8 devNum,IN MSD_U8 ageTimeMinute);

 /********************************************************************************
  * @brief device_atu_module_aging_timeout_get
  * 获取动态条目的超时时间
  * @param devNum 设备Num
  * @param ageTimeMinute 超时时间,以分钟为单位
  * @return
  * MSD_OK - On success
  * MSD_FAIL - On error
  * MSD_BAD_PARAM - If invalid parameter is given
  * MSD_NOT_SUPPORTED - Device not support
  ********************************************************************************/
 MSD_STATUS deviceAtuModuleAgingTimeoutGet(IN MSD_U8 devNum,OUT MSD_U8* ageTimeMinute);

 /************************************************************************************************
  * @brief device_atu_module_delete_entry 根据mac地址(macAddr)删除地址条目
  * @param devNum 设备Num
  * @param macAddr 要删除的地址条目
  * @return
  * MSD_OK  - on success
  * MSD_FAIL - on error
  * MSD_BAD_PARAM - if invalid parameter is given
  * MSD_NOT_SUPPORTED - device not support
  ************************************************************************************************/
 MSD_STATUS deviceAtuModuleDeleteEntry(IN MSD_U8 devNum, IN MSD_ETHERADDR macAddr);

 /************************************************************************************************
  * @brief device_atu_module_flush_entries
  *  根据flushCmd的值删除地址数据库中指定的地址条目类型，如果指定设备的fid值为-1，则删除地址条目中指定的地址条目类型。
  *  如果fid的值大于等于0，则删除指定数据库(fid)的地址条目,device_atu_module_set_fid 接口可以设置要操作的FID值
  *  注：如果flush STATIC条目，则需要指定fid值(fid > 0)，由于Marvell没有提供单独删除所有静态地址条目记录的API接口。
  *  思路是调用msdFdbEntryNextGet接口获取某个fid的下一个ATU条目，在判断是否是静态条目，如果是则删除指定条目.
  *  如果是使用ＲＭＵ协议，会比较耗时
  * @param devNum 设备Num
  * @param flushCmd 要删除地址条目的类型
  * @return
  *  MSD_OK  - on success
  *  MSD_FAIL - on error
  *  MSD_BAD_PARAM - if invalid parameter is given
  *  MSD_NOT_SUPPORTED - device not support
  ************************************************************************************************/
 MSD_STATUS deviceAtuModuleFlushEntries(IN MSD_U8 devNum, IN MSD_FLUSH_CMD flushCmd);

 /************************************************************************************************
  * @brief device_atu_module_get_list
  * 获取MAC地址条目列表，结果存放在atu_entry 二元数组中，用户指定atu_entry的空间。entry_type获取指定的MAC地址列表类型
  * @param dev_num 设备Num
  * @param entry_type 要获取的ATU条目类型（可以优化，这里可以根据单播，多播，广播，
  * 然后在根据是否有效，是否有策略等进行分类。这里并没有这么细分）
  * @param atuEntry 存放返回的MAC地址条目列表
  * @param atuEntryMaxSize atuEntry的最大条数，最大值可以为MAX_ATU_ENTRIES
  * @param atuEntryCount 实际返回的MAC条目列表数据
  * @return
  * MSD_OK - on success
  * MSD_FAIL - on error
  * MSD_BAD_PARAM - if invalid parameter is given
  * MSD_NOT_SUPPORTED - device not support
  * ATU条目已达分配的最大值，考虑最近atu entry max size的大小，并分配相应的条目空间存储
  ************************************************************************************************/
 MSD_STATUS deviceAtuModuleGetList(IN MSD_U8 devNum,IN AtuEntryType entryType, OUT MSD_ATU_ENTRY *atuEntry,IN int atuEntryMaxSize,  OUT int* atuEntryCount);

 /************************************************************************************************
  * @brief device_atu_module_find_entry 查询某地址条目是否存在，如果存在，则条目内容存放在atu_entry
  * @param devNum 要查询的设备编号
  * @param fid 要查询的fid，大于0,返回值仅仅返回一条数据
  * @param macAddr 要查询的MAC地址
  * @param atuEntry 如果找到，则用于存放找到的条目内容
  * @param isFound 存放是否找到的标志
  * @return
  * MSD_OK - On success
  * MSD_FAIL - On error
  * MSD_BAD_PARAM - If invalid parameter is given
  * MSD_NOT_SUPPORTED - Device not support
  ************************************************************************************************/
 MSD_STATUS deviceAtuModuleFindEntry(IN MSD_U8 devNum, IN int fid, IN  MSD_ETHERADDR* macAddr, OUT MSD_ATU_ENTRY* atuEntry, OUT MSD_BOOL* isFound);

 /************************************************************************************************
  * @brief device_atu_module_get_all_fid_has_values 获取某个设备所有使用的FID值
  * @param dev_num
  * @param fid_arr 存储设备中的FID集合
  * @param fid_size fid_arr的大小
  * @param fid_count 实际获取的FID数目数
  * @return
  ************************************************************************************************/
 MSD_STATUS deviceAtuModuleGetAllFidHasValues(IN MSD_U8 devNum, INOUT MSD_U16 fidArr[], IN int fidSize, OUT int* fidCount);


////EES 交换机API相关类型的接口 start
 /*************************************************************************************************
  * @brief device_atu_module_add_mac_static_entry_to_configuration
  * 添加带有VID的静态ATU条目到配置中
  * 该静态条目用于指定具有指定VID和目标地址为该地址的帧应该从这个(这些)端口转发出去。
  * 注：该API接口的参数为EES交换机添加静态条目的参数，如果想设置其他参数，需进行修改。
  * 该接口在交换机重启时，需要从配置文件(如果可以，可以添加一个读取配置文件的API接口，
  * 也可以在web server端去实现)读取并导入到交换机中。
  * @param devNum 设备编码
  * @param address 要添加的MAC地址
  * @param vid  MAC条目的VID值
  * @param portVec 该ＭＡＣ条目的转发端口列表(例如二进制数0000000010代表该条目可以从端口１进行转发)
  * @return
  * MSD_OK - On success
  * MSD_NO_SPACE - 当已达到静态条目最大限制时，不允许添加条目
  * MSD_BAD_PARAM - If invalid parameter is given
  *************************************************************************************************/
 MSD_STATUS deviceAtuModuleAddMacStaticEntryToConfiguration(IN MSD_U8 devNum, IN MSD_ETHERADDR address, IN int vid, MSD_U32 portVec);//1

 /*************************************************************************************************
  * @brief device_atu_module_remove_mac_static_entry_from_configuration
  * 根据mac地址和VID值从配置中删除静态条目,并不会从交换机中移除静态条目
  * @param devNum 设备编号
  * @param address MAC地址
  * @param vid VID值
  * @param found:如果要删除的条目不存在，found设置为MSD_FALSE,否则返回ＭＳＤ_TRUE
  * @return
  * MSD_OK - On success
  * MSD_BAD_PARAM - If invalid parameter is given
  *************************************************************************************************/
 MSD_STATUS deviceAtuModuleRemoveMacStaticEntryFromConfiguration(IN MSD_U8 devNum, IN MSD_ETHERADDR address, IN int vid, OUT MSD_BOOL *found);//8

 /*************************************************************************************************
  * @brief device_atu_module_clear_all_vid_static_entries_from_configuration
  * 从配置信息中清空静态条目
  * @param dev_num 设备编号
  * @return
  * MSD_OK - On success
  * MSD_BAD_PARAM - If invalid parameter is given
  *************************************************************************************************/
 MSD_STATUS deviceAtuModuleClearAllVidStaticEntriesFromConfiguration(IN MSD_U8 devNum); //7
 /*************************************************************************************************
  * @brief device_atu_module_get_all_vid_static_entries_from_configuration
  * 从配置信息中获取mac entries条目信息
  * @param devNum 设备编号
  * @param macEntries 存储获取到的静态条目
  * @param macEntrySize macEntries的最大存储条数
  * @param macEntryCount macEntries的实际条数
  * @return
  * MSD_OK - On success
  * MSD_BAD_PARAM - If invalid parameter is given
  *************************************************************************************************/
 MSD_STATUS deviceAtuModuleGetAllVidStaticEntriesFromConfiguration(IN MSD_U8 devNum, OUT MacEntry *macEntries, IN MSD_U32 macEntrySize, OUT MSD_U32 *macEntryCount);//3

 /*************************************************************************************************
  * @brief device_atu_module_refresh_all_vid_static_entries_to_configuration
  *  刷新所有的静态 MAC VID条目。该条目会从交换机芯片中获取已有的静态条目，并将其已有静态条目保存到配置中
  * @param dev_num 设备编号
  * @return
  * MSD_OK  - on success
  * MSD_FAIL  - on error
  * MSD_BAD_PARAM - if invalid parameter is given
  * MSD_NOT_SUPPORTED - device not support
  * MSD_NO_SPACE - memory malloc failed
  *************************************************************************************************/
 MSD_STATUS deviceAtuModuleRefreshAllVidStaticEntriesToConfiguration(IN MSD_U8 devNum);//2

 /************************************************************************************************
  * @brief device_atu_module_remove_mac_auto_entry_from_configuration
  * 从交换机中删除指定VID和address组合的动态MAC条目
  * @param dev_num 设备编号
  * @param address MAC地址
  * @param vid VID值
  * @return
  * MSD_OK  - on success
  * MSD_FAIL - on error
  * MSD_BAD_PARAM - if invalid parameter is given
  * MSD_NOT_SUPPORTED - device not support
  *************************************************************************************************/
 MSD_STATUS deviceAtuModuleRemoveMacAutoEntryFromConfiguration(IN MSD_U8 devNum, IN MSD_ETHERADDR address, IN int vid);//9

 /*************************************************************************************************
  * @brief device_atu_module_clear_all_vid_auto_entries_from_configuration
  *  清空交换机所有FID的动态ATU条目,其动态条目配置也会被清空
  * @param devNum 设备编号
  * @return
  * MSD_OK  - on success
  * MSD_FAIL - on error
  * MSD_BAD_PARAM - if invalid parameter is given
  * MSD_NOT_SUPPORTED - device not support
  *************************************************************************************************/
 MSD_STATUS deviceAtuModuleClearAllVidAutoEntriesFromConfiguration(IN MSD_U8 devNum);//6

 /*************************************************************************************************
  * @brief device_atu_module_get_all_vid_auto_entries_from_configuration
  * 从配置条目中获取所有动态MAC条目（并不会主动刷新数据，如果想获取最新的动态MAC条目，
  * 请执行device_atu_module_refresh_all_vid_auto_entries）
  * @param dev_num 设备编号
  * @param macEntries 存储动态MAC条目
  * @param macEntrySize mac_entries的最大可存储个数
  * @param macEntryCount 交换机mac_entry的实际个数
  * @return
  * MSD_OK - On success
  * MSD_BAD_PARAM - If invalid parameter is given
  *************************************************************************************************/
 MSD_STATUS deviceAtuModuleGetAllVidAutoEntriesFromConfiguration(IN MSD_U8 devNum, OUT MacEntry* macEntries, IN MSD_U32 macEntrySize, OUT MSD_U32 *macEntryCount);//5

 /*************************************************************************************************
  * @brief device_atu_module_refresh_all_vid_auto_entries_to_configuration
  *  刷新内存中的动态ATU条目，会从交换机芯片中去获取最新的ATU条目列表
  * @param dev_num 设备编号
  * @return
  * MSD_OK - on success
  * MSD_FAIL - on error
  * MSD_BAD_PARAM - if invalid parameter is given
  * MSD_NOT_SUPPORTED - device not support
  * MSD_NO_SPACE - malloc memory space failed.
  *************************************************************************************************/
 MSD_STATUS deviceAtuModuleRefreshAllVidAutoEntriesToConfiguration(IN MSD_U8 devNum);//4

 /***************************************************************************************************
  * @brief device_atu_module_get_atu_configuration
  * 从配置中获取当前保存的ATU配置信息
  * @param dev_num 设备编号
  * @param configuration 保存获取到的配置信息
  * @return
  * MSD_OK - on success
  * MSD_BAD_PARAM - if invalid parameter is given
  ***************************************************************************************************/
 MSD_STATUS deviceAtuModuleGetAtuConfiguration(IN MSD_U8 devNum, OUT AtuConfiguration* configuration);
 /***************************************************************************************************
  * @brief device_atu_module_set_atu_configuration
  * 设置当前保存的ATU配置信息
  * 包括静态条目，使用条目方式，Agetime，会保存到配置信息中，不会存储到交换机芯片中
  * 建议先调用device_atu_module_get_atu_configuration,然后在进行相应的设置
  * @param devNum
  * @param configuration 配置信息
  * @return
  * MSD_OK - on success
  * MSD_BAD_PARAM - if invalid parameter is given
  **************************************************************************************************/
 MSD_STATUS deviceAtuModuleSetAtuConfiguration(IN MSD_U8 devNum, IN AtuConfiguration* configuration);
 /**************************************************************************************************
  * @brief device_atu_module_save_atu_configuration
  * 保存ATU配置:包括静态条目，使用条目方式，Agetime，会保存到交换机芯片中
  * @param devNum
  * @return
  * MSD_OK - On success
  * MSD_FAIL - On error
  * MSD_BAD_PARAM - If invalid parameter is given
  * MSD_NOT_SUPPORTED - Device not support</returns>
  **************************************************************************************************/
 MSD_STATUS deviceAtuModuleSaveAtuConfiguration(IN MSD_U8 devNum);


////EES 交换机API相关类型的接口 end

#ifdef __cplusplus
}
#endif
