#ifndef _API_INIT_H
#define _API_INIT_H

//负责初始化访问API接口相关的函数


#ifdef __cplusplus
extern "C" {
#endif

#include "msdSysConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "umsdUtil.h"

typedef struct {
    MSD_U8 devNum;//设备编号，通常设置为0，目前freetos仅支持一个单芯片(不排除之后可能使用多芯片的情况，所有所有API接口都有一个devNum参数，代表要操作的设备芯片编号)
    SemaphoreHandle_t xMutex;//调用API的锁
    TaskHandle_t eventLoopHandle;//任务函数的句柄
    MSD_BOOL isCallAPI;//是否调用API
    MSD_BOOL isOpen;//是否打开设备的标志
}DeviceConfig;

 /****************************************************************************************
  * @brief init_open_bus_interface 打开设备
  * @return
  * MSD_OK:打开成功
  * MSD_FAILED:打开失败
  * MSD_BAD_PARAM:dev_num越界（1~31）
  *****************************************************************************************/
 MSD_STATUS initOpenDevice(IN MSD_U8 devNum);

 /****************************************************************************************
  * @brief initStartCallAPI 调用API之前需要调用该接口
  * @param devNum
  * @return
  * MSD_OK:success
  * MSD_FAIL:fail
  * MSD_BAD_PARAM:dev_num越界（1~31）
  ****************************************************************************************/
 MSD_STATUS initStartCallAPI(IN MSD_U8 devNum);

 /****************************************************************************************
  * @brief initStopCallAPI 结束调用API之后需要调用该接口
  * @param devNum
  * @return
  * MSD_OK:success
  * MSD_FAIL:fail
  * MSD_BAD_PARAM:dev_num越界（1~31）
  ****************************************************************************************/
 MSD_STATUS initStopCallAPI(IN MSD_U8 devNum);

 /*****************************************************************************************
  * @brief init_close_device 关闭设备
  *****************************************************************************************/
 void initCloseDevice(IN MSD_U8 devNum);

#ifdef __cplusplus
}
#endif


#endif


