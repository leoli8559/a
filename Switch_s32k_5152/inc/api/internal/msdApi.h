/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/********************************************************************************
* msdApi.h
*
* DESCRIPTION:
*       API definitions for QuarterDeck Device
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*
*******************************************************************************/

#ifndef msdApi_h
#define msdApi_h

#ifdef __cplusplus
extern "C" {
#endif

/* minimum need header files*/
#include "msdApiTypes.h"
#include "msdHwAccess.h"
#include "msdSem.h"
#include "msdSysConfig.h"
#include "msdUtils.h"

/* add modules whatever you needed */
#include "msdBrgFdb.h"
#include "msdBrgVtu.h"
#include "msdPortCtrl.h"
#include "msdSysCtrl.h"
#include "msdTCAM.h"
#include "msdQosMap.h"
#ifdef __cplusplus
}
#endif

#endif /* msdApi_h */
