#pragma once

#include "msdApi.h"

#ifdef __cplusplus
extern "C" {
#endif


#define CHECK_DEV_NUM_IS_CORRECT  MSD_QD_DEV* dev = sohoDevGet(devNum); \
								  if(dev == NULL) return MSD_BAD_PARAM;

/**
 *根据pcp，dei，vid,tpid构造VLANTag标签字段
 */
#define CREATE_VLAN_TAG(TPID,PCP,DEI,VID)  (((TPID & 0xFFFF) << 16) | ((PCP & 0x07 ) << 13) | ((DEI & 0x01) << 12) | (VID & 0x0FFF))

/**
 * 判断num数的第bit位是否设置。
 */
#define IS_BIT_SET(num,bitIndex) \
((num & (1 << bitIndex)) != 0)

///设置数的某一位
#define SET_BIT(num, bitIndex) ((num) |= (1U << (bitIndex)))

//清空数的某一位
#define CLEAR_BIT(num, bitIndex) ((num) = ((num) & ~(1 << (bitIndex))))

/**
 * 获取数据中从指定位置开始的指定长度的位段
 * data:要操作的数
 * startBit:位段的起始位置（从0开始计数)
 * numBits: 位段的长度
 */
#define GET_CONTINUOUS_BITS(data, startBit, numBits) (((data) >> (startBit)) & ((1 << (numBits)) - 1))

// 宏定义，用于设置从startBit位开始连续numBits位为1
#define SET_CONTINUOUS_BITS_TO_1(num, startBit, numBits) \
((num) | (((1 << (numBits)) - 1) << (startBit)))

// 宏定义，用于设置从startBit位开始连续numBits位为0
#define SET_CONTINUOUS_BITS_TO_0(num, startBit, numBits) \
((num) & ~(((1 << (numBits)) - 1) << (startBit)))

//是否只有一位被设置为1
#define HAS_ONLY_ONE_BIT_SET(num) ((num != 0) && ((num & (num - 1)) == 0))

//判断MSD_U8 *mac参数是否是多播地址(包含广播地址)
#define IS_MULTICAST_MAC(mac) ((mac[0] & 0x01) != 0)
#define IS_UNICAST_MAC(mac) ((mac[0] & 0x01) == 0)
#define IS_BROADCAST_MAC(mac) ((mac[0] == 0xFF) && (mac[1] == 0xFF) && (mac[2] == 0xFF) && (mac[3] == 0xFF) && (mac[4] == 0xFF) && (mac[5] == 0xFF))

/******************************************************************************
 * @brief boolarrayToInt
 * 将bool数组转换为int整数存储
 * @param array
 * @param size
 * @return 返回转换后的整数
 *******************************************************************************/
int boolarrayToInt(MSD_BOOL array[], int size);

/******************************************************************************
 * @brief numberIsInArr 判断数字是否在数组中
 * @param arr
 * @param arrSize
 * @param num 要查询的数字
 * @return
 *   -1：不存在
 *   >0 ：下标值
******************************************************************************/
int numberIsInArr(int* arr, int arrSize, int num);





#ifdef __cplusplus
}
#endif
