#pragma once

#include "umsdUtil.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 交换机的segmenation的配置
 */
typedef struct {
    MSD_BOOL isSymmetrically;/*是否对称设置,如果设置了对称设置，比如如果前端设置了Port 1可以往Port 2发送数据包，但是前端没有设置Port2可以发送给port1发送数据包，则默认允许Port 2可以给Port 1发送数据包 */
    MSD_BOOL memPortSegmentationSet[MSD_MAX_SWITCH_PORTS][MSD_MAX_SWITCH_PORTS];/*端口的出口segmenation设置，比如mem_port_segmentation_set[0][1]为true，代表port 1可以给port 2发送数据，否则不可以,不保存port 0的设置*/
    MSD_U8 memPortSegmentationSetSize;/*mem_port_segmentation_set的大小，理应设置为交换机端口数-1，Port 0为内部端口，不进行设置,在设置时，如果传递的值小于端口数-1，则后续端口的mem_port_segmentation_set设置为ture*/
}PortSegmentationConfig;

/******************************************************************************************************************************************
 * @brief device_port_segmentation_module_set_enable_segmentation  设置是否启用segmentation模块
 * @param devNum 设备编号
 * @param is_enable 是否启用
 * @return
 *  MSD_OK  - on success
 *  MSD_BAD_PARAM - if invalid parameter is given
 ******************************************************************************************************************************************/
MSD_STATUS devicePortSegmentationModuleSetEnableSegmentation(IN MSD_U8 devNum, IN MSD_BOOL isEnable);

/******************************************************************************************************************************************
 * @brief device_port_segmentation_module_get_enable_segmentation 获取是否启用segmentation模块
 * @param devNum
 * @param isEnable
 * @return
 *  MSD_OK  - on success
 *  MSD_BAD_PARAM - if invalid parameter is given
 ******************************************************************************************************************************************/
MSD_STATUS devicePortSegmentationModuleGetEnableSegmentation(IN MSD_U8 devNum, OUT MSD_BOOL *isEnable);

/******************************************************************************************************************************************
  * @brief device_port_segmentation_module_set_clear_segmentation_to_config
  * 清除所有的Port Segmentation,即任意端口之前都不能转发数据包
  * @note:配置仅仅是保存到内存，没有保存到交换机中，如需要保存到交换机中，需要调用save接口。
  * @param devNum 设备编号
  * @return
  * MSD_OK - On success
  * MSD_BAD_PARAM - If invalid parameter is given
  ******************************************************************************************************************************************/
 MSD_STATUS devicePortSegmentationModuleSetClearSegmentationToConfig(IN MSD_U8 devNum);

 /******************************************************************************************************************************************
  * @brief device_port_segmentation_module_reset_clear_segmentation
  * 重置segmentation,具体设置为端口除不能给自身发送数据包外，可以发送给其他端口(会保存到交换机中),对称性设置为false
  * @param devNum 设备编号
  * @return
  *  MSD_OK  - on success
  *  MSD_FAIL  - on error
  *  MSD_BAD_PARAM - if invalid parameter is given
  ******************************************************************************************************************************************/
 MSD_STATUS devicePortSegmentationModuleResetSegmentation(IN MSD_U8 devNum);

 /******************************************************************************************************************************************
  * @brief device_port_segmentation_module_set_all_segmentation_to_config
  * 设置所有端口的Port Segmentation。即任意端口之前都不能转发数据包。端口自身是否可以回发给自身，由is_forward_back参数决定
  * @param devNum 设备编号
  * @param isForwardBack 是否可以回发给端口自身的标志
  * @return
  * MSD_OK - On success
  * MSD_BAD_PARAM - If invalid parameter is given
  ******************************************************************************************************************************************/
 MSD_STATUS devicePortSegmentationModuleSetAllSegmentationToConfig(IN MSD_U8 devNum, IN MSD_BOOL isForwardBack);

 /******************************************************************************************************************************************
  * @brief device_port_segmentation_module_set_segmentation_to_config
  * 设置端口的segmentation，设置值不会保存到交换机中，仅仅保存到配置中，segmentation_config的mem_port_segmentation_set_size的值应等于交换机的端口数-1,
  * 注：port 0为交换机内部端口，不会进行设置。
  * @param devNum 设备编号
  * @param segmentationConfig 要设置的segmentation参数,如果其中的mem_port_segmentation_set_size,小于交换机的端口数-1时，则默认出口，
  * 即如果为mem_port_segmentation_set_size为0时，相当于设置所有端口都可以往其他端口(除了自身)转发数据（set_all_segmentation）
  * @return
  * MSD_OK - On success
  * MSD_BAD_PARAM - If invalid parameter is given
  * MSD_FEATRUE_NOT_ALLOW - 没有开启segmentation功能
  ******************************************************************************************************************************************/
 MSD_STATUS devicePortSegmentationModuleSetSegmentationToConfig(IN MSD_U8 devNum, IN PortSegmentationConfig* segmentationConfig);

 /******************************************************************************************************************************************
  * @brief device_port_segmentation_module_get_segmentation_from_config
  * 获取端口的segmentation的设置值，仅仅获取当前配置中值，不会获取交换机中的值
  * @param devNum 设备编号
  * @param segmentationConfig 端口的segmentation的配置值
  * @return
  * MSD_OK - On success
  * MSD_BAD_PARAM - If invalid parameter is given
  * MSD_FEATRUE_NOT_ALLOW - 没有开启segmentation功能
  ******************************************************************************************************************************************/
 MSD_STATUS devicePortSegmentationModuleGetSegmentationFromConfig(IN MSD_U8 devNum, OUT PortSegmentationConfig* segmentationConfig);

 /******************************************************************************************************************************************
  * @brief device_port_segmentation_module_refresh_port_segmentation
  * 从交换机中获取端口的Port Segmentation, 结果存放在segmentation_config mem_port_segmentation_set二元数组中，
  * 二元数组的行对应入口端口，列对应相应入口端口是否可以转发给相应的出口端口，比如segmentation_config mem_port_segmentation_set[1][2] = MSD_TRUE，代表Port 2可以出口到 PORT 3
  * 注意：Port 0为交换机内部端口，我们并不关心Port 0,因此并没有保存到结果中。
  * @param devNum 设备编号
  * @param segmentationConfig 存放结果的配置
  * @return
  * MSD_OK - On success
  * MSD_FAIL - On error
  * MSD_BAD_PARAM - If invalid parameter is given
  * MSD_FEATRUE_NOT_ALLOW - 没有开启segmentation功能
  ******************************************************************************************************************************************/
 MSD_STATUS devicePortSegmentationModuleRefreshSegmentation(IN MSD_U8 devNum,OUT PortSegmentationConfig* segmentationConfig);


 /******************************************************************************************************************************************
  * @brief device_port_segmentation_module_save_one_port_segmentation
  * 保存某个端口的segmentation的信息到交换机中
  * @param devNum 设备编号
  * @param portNum  端口编号
  * @param memPortSegmentationSet 指定端口的segmentation出口信息(包括port 0的设置,注意和其他接口的区别),可以为NULL,如果为NULL,则允许出口除自身以外的其他端口
  * @param memPortSegmentationSetSize memPortSegmentationSet的大小，如果memPortSegmentationSetSize小于port-1，则端口的后续出口端口（端口较大的编号）默认设置为true
  * @return
  *  MSD_OK  - on success
  *  MSD_FAIL  - on error
  *  MSD_BAD_PARAM - if invalid parameter is given
  *  MSD_FEATRUE_NOT_ALLOW - 没有开启segmentation功能
  ******************************************************************************************************************************************/
 MSD_STATUS devicePortSegmentationModuleSaveOnePortSegmentation(IN MSD_U8 devNum, IN int portNum,IN MSD_BOOL *memPortSegmentationSet,
                                                                       IN int memPortSegmentationSize);

 /******************************************************************************************************************************************
  * @brief device_port_segmentation_module_save_segmentation
  * 保存端口的Port Segmentation配置到交换机设备中
  * @param devNum 设备编号
  * @return
  * MSD_OK - On success
  * MSD_FAIL - On error
  * MSD_BAD_PARAM - If invalid parameter is given
  * MSD_FEATRUE_NOT_ALLOW - 没有开启segmentation功能
  ******************************************************************************************************************************************/
 MSD_STATUS devicePortSegmentationModuleSaveSegmentation(IN MSD_U8 devNum);

#ifdef __cplusplus
 }
#endif
