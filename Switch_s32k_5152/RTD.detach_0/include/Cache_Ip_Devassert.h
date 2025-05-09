/*==================================================================================================
*   Project              : RTD AUTOSAR 4.7
*   Platform             : CORTEXM
*   Peripheral           : DMA,CACHE,TRGMUX,LCU,EMIOS,FLEXIO
*   Dependencies         : none
*
*   Autosar Version      : 4.7.0
*   Autosar Revision     : ASR_REL_4_7_REV_0000
*   Autosar Conf.Variant :
*   SW Version           : 4.0.0
*   Build Version        : S32K3_RTD_4_0_0_D2311_ASR_REL_4_7_REV_0000_20231128
*
*   Copyright 2020 - 2023 NXP
*   
*
*   NXP Confidential. This software is owned or controlled by NXP and may only be
*   used strictly in accordance with the applicable license terms. By expressly
*   accepting such terms or by downloading, installing, activating and/or otherwise
*   using the software, you are agreeing that you have read, and that you agree to
*   comply with and are bound by, such license terms. If you do not agree to be
*   bound by the applicable license terms, then you may not retain, install,
*   activate or otherwise use the software.
==================================================================================================*/

/* Prevention from multiple including the same header */
#ifndef CACHE_IP_DEVASSERT_H_
#define CACHE_IP_DEVASSERT_H_

/**
*   @file    Cache_Ip_Devassert.h
*
*   @version 4.0.0
*
*   @brief   AUTOSAR Mcl - Cache Ip driver header file.
*   @details
*
*   @addtogroup CACHE_IP_DRIVER CACHE IP Driver
*   @{
*/

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
*                                          INCLUDE FILES
*  1) system and project includes
*  2) needed interfaces from external units
*  3) internal and external interfaces from this unit
==================================================================================================*/
#include "Cache_Ip_Types.h"

/*==================================================================================================
*                              SOURCE FILE VERSION INFORMATION
==================================================================================================*/
#define CACHE_IP_DEVASSERT_VENDOR_ID                       43
#define CACHE_IP_DEVASSERT_AR_RELEASE_MAJOR_VERSION        4
#define CACHE_IP_DEVASSERT_AR_RELEASE_MINOR_VERSION        7
#define CACHE_IP_DEVASSERT_AR_RELEASE_REVISION_VERSION     0
#define CACHE_IP_DEVASSERT_SW_MAJOR_VERSION                4
#define CACHE_IP_DEVASSERT_SW_MINOR_VERSION                0
#define CACHE_IP_DEVASSERT_SW_PATCH_VERSION                0

/*==================================================================================================
                                      FILE VERSION CHECKS
==================================================================================================*/
/* Check if header file and Cache_Ip_Types.h file are of the same vendor */
#if (CACHE_IP_DEVASSERT_VENDOR_ID != CACHE_IP_TYPES_VENDOR_ID)
    #error "Cache_Ip_Devassert.h and Cache_Ip_Types.h have different vendor ids"
#endif

/* Check if header file and Cache_Ip_Types.h file are of the same Autosar version */
#if ((CACHE_IP_DEVASSERT_AR_RELEASE_MAJOR_VERSION != CACHE_IP_TYPES_AR_RELEASE_MAJOR_VERSION) || \
     (CACHE_IP_DEVASSERT_AR_RELEASE_MINOR_VERSION != CACHE_IP_TYPES_AR_RELEASE_MINOR_VERSION) || \
     (CACHE_IP_DEVASSERT_AR_RELEASE_REVISION_VERSION != CACHE_IP_TYPES_AR_RELEASE_REVISION_VERSION) \
    )
    #error "AutoSar Version Numbers of Cache_Ip_Devassert.h and Cache_Ip_Types.h are different"
#endif

/* Check if header file and Cache_Ip_Types.h file are of the same Software version */
#if ((CACHE_IP_DEVASSERT_SW_MAJOR_VERSION != CACHE_IP_TYPES_SW_MAJOR_VERSION) || \
     (CACHE_IP_DEVASSERT_SW_MINOR_VERSION != CACHE_IP_TYPES_SW_MINOR_VERSION) || \
     (CACHE_IP_DEVASSERT_SW_PATCH_VERSION != CACHE_IP_TYPES_SW_PATCH_VERSION) \
    )
    #error "Software Version Numbers of Cache_Ip_Devassert.h and Cache_Ip_Types.h are different"
#endif

#if (STD_ON == CACHE_IP_IS_AVAILABLE)
/*==================================================================================================
*                                            CONSTANTS
==================================================================================================*/

/*==================================================================================================
*                                       DEFINES AND MACROS
==================================================================================================*/

/*==================================================================================================
*                                              ENUMS
==================================================================================================*/

/*==================================================================================================
*                                  STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/
#define MCL_START_SEC_CODE
#include "Mcl_MemMap.h"

#if (STD_ON == CACHE_IP_DEV_ERROR_DETECT)
    /* Implement default assert macro */
    static inline void Cache_Ip_DevAssert(volatile boolean x)
    {
        if(x) { } else { ASM_KEYWORD("BKPT #0"); for(;;) {} }
    }
    #define CACHE_IP_DEV_ASSERT(x) Cache_Ip_DevAssert(x)
#else
    /* Assert macro does nothing */
    #define CACHE_IP_DEV_ASSERT(x)
#endif /* STD_ON == CACHE_IP_DEV_ERROR_DETECT */

#define MCL_STOP_SEC_CODE
#include "Mcl_MemMap.h"

/*==================================================================================================
*                                  GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

/*==================================================================================================
*                                       FUNCTION PROTOTYPES
==================================================================================================*/

#endif /* #if (STD_ON == CACHE_IP_IS_AVAILABLE) */

#ifdef __cplusplus
}
#endif

/** @} */

#endif  /* #ifndef CACHE_IP_DEVASSERT_H_ */
