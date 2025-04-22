#pragma once

#include "umsdUtil.h"

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************************************************************
 * @brief deviceinfoGetDeviceNumAndRevByTypename 根据设备id获取设备编号和Rev值
 * @param deviceTypename 设备设备类型名称值
 * @return 成功返回设备编号和Rev值，否则返回0
 ****************************************************************************************************************************/
MSD_U16 deviceinfoGetDeviceNumAndRevByTypename(IN const char* deviceTypename);

/****************************************************************************************************************************
 * @brief deviceinfoGetDeviceNameByDeviceType 根据设备类型获取设备名称，比如88Q5152为Fir
 * @param deviceType 设备类型
 * @return
 ****************************************************************************************************************************/
char* deviceinfoGetDeviceNameByDeviceType(IN int deviceType);




#ifdef __cplusplus
}
#endif
