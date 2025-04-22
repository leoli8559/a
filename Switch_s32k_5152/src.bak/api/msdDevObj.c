/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/********************************************************************************
* msdDevObj.c
*
* DESCRIPTION:
*       API definitions for system configuration, and enabling.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*
*******************************************************************************/

#include <msdSysConfig.h>
#include <msdDevObj.h>
#include <msdHwAccess.h>
#include <msdUtils.h>
#include <Fir_msdApi.h>

/* static function declare*/
static MSD_STATUS InitObj(MSD_QD_DEV *dev);
static MSD_STATUS InitDevObj_Fir(MSD_QD_DEV *dev);

static MSD_STATUS InitDevObj_Fir(MSD_QD_DEV *dev)
{
    dev->SwitchDevObj.ATUObj.gfdbAddMacEntry = &Fir_gfdbAddMacEntryIntf;
    dev->SwitchDevObj.ATUObj.gfdbGetAtuEntryNext = &Fir_gfdbGetAtuEntryNextIntf;
    dev->SwitchDevObj.ATUObj.gfdbFlush = &Fir_gfdbFlushIntf;
    dev->SwitchDevObj.ATUObj.gfdbFlushInDB = &Fir_gfdbFlushInDBIntf;
    dev->SwitchDevObj.ATUObj.gfdbMove = &Fir_gfdbMoveIntf;
    dev->SwitchDevObj.ATUObj.gfdbMoveInDB = &Fir_gfdbMoveInDBIntf;
    dev->SwitchDevObj.ATUObj.gfdbPortRemove = &Fir_gfdbPortRemoveIntf;
    dev->SwitchDevObj.ATUObj.gfdbPortRemoveInDB = &Fir_gfdbPortRemoveInDBIntf;
    dev->SwitchDevObj.ATUObj.gfdbDelAtuEntry = &Fir_gfdbDelAtuEntry;
    dev->SwitchDevObj.ATUObj.gfdbGetViolation = &Fir_gfdbGetViolationIntf;
    dev->SwitchDevObj.ATUObj.gfdbFindAtuMacEntry = &Fir_gfdbFindAtuMacEntryIntf;
    dev->SwitchDevObj.ATUObj.gfdbSetAgingTimeout = &Fir_gfdbSetAgingTimeout;
    dev->SwitchDevObj.ATUObj.gfdbGetAgingTimeout = &Fir_gfdbGetAgingTimeout;
    dev->SwitchDevObj.ATUObj.gfdbSetLearn2All = &Fir_gfdbSetLearn2All;
    dev->SwitchDevObj.ATUObj.gfdbGetLearn2All = &Fir_gfdbGetLearn2All;
    dev->SwitchDevObj.ATUObj.gfdbSetPortLearnLimit = &Fir_gfdbSetPortLearnLimit;
    dev->SwitchDevObj.ATUObj.gfdbGetPortLearnLimit = &Fir_gfdbGetPortLearnLimit;
    dev->SwitchDevObj.ATUObj.gfdbGetPortLearnCount = &Fir_gfdbGetPortLearnCount;
    dev->SwitchDevObj.ATUObj.gfdbGetEntryCount = &Fir_gfdbGetEntryCount;
    dev->SwitchDevObj.ATUObj.gfdbGetEntryCountPerFid = &Fir_gfdbGetEntryCountPerFid;
    dev->SwitchDevObj.ATUObj.gfdbGetNonStaticEntryCount = &Fir_gfdbGetNonStaticEntryCount;
    dev->SwitchDevObj.ATUObj.gfdbGetNonStaticEntryCountPerFid = &Fir_gfdbGetNStaticEntryCountFid;
    dev->SwitchDevObj.ATUObj.gfdbDump = &Fir_gfdbDump;
//
//    dev->SwitchDevObj.ARPObj.garpLoadMcEntry = &Fir_garpLoadMcEntryIntf;
//    dev->SwitchDevObj.ARPObj.garpLoadUcEntry = &Fir_garpLoadUcEntryIntf;
//    dev->SwitchDevObj.ARPObj.garpFlushEntry = &Fir_garpFlushEntry;
//    dev->SwitchDevObj.ARPObj.garpFlushAll = &Fir_garpFlushAll;
//    dev->SwitchDevObj.ARPObj.garpReadEntry = &Fir_garpReadEntryIntf;
//    dev->SwitchDevObj.ARPObj.garpGetNextEntry = &Fir_garpGetNextEntryIntf;

//    dev->SwitchDevObj.QAVObj.gqavGetPortQpriXHiLimit = &Fir_gqavGetPortQpriXHiLimit;
//    dev->SwitchDevObj.QAVObj.gqavSetPortQpriXHiLimit = &Fir_gqavSetPortQpriXHiLimit;
//    dev->SwitchDevObj.QAVObj.gqavGetPortQpriXRate = &Fir_gqavGetPortQpriXRate;
//    dev->SwitchDevObj.QAVObj.gqavSetPortQpriXRate = &Fir_gqavSetPortQpriXRate;

//    dev->SwitchDevObj.PHYCTRLObj.serdesReset = &Fir_serdesReset;

//    dev->SwitchDevObj.PTPObj.gptpSetIntEnable = &Fir_gptpSetIntEnableIntf;
//    dev->SwitchDevObj.PTPObj.gptpGetIntEnable = &Fir_gptpGetIntEnableIntf;
//    dev->SwitchDevObj.PTPObj.gptpSetProtPTPEn = &Fir_gptpSetPortPTPEn;
//    dev->SwitchDevObj.PTPObj.gptpGetProtPTPEn = &Fir_gptpGetPortPTPEn;
//    dev->SwitchDevObj.PTPObj.gptpGetTimeStamp = &Fir_gptpGetTimeStampIntf;
//    dev->SwitchDevObj.PTPObj.gptpGetTSValidSt = &Fir_gptpGetTSValidSt;
//    dev->SwitchDevObj.PTPObj.gptpResetTimeStamp = &Fir_gptpResetTimeStamp;
//    dev->SwitchDevObj.PTPObj.gptpGetIntStatus = &Fir_gptpGetIntStatusIntf;
//    dev->SwitchDevObj.PTPObj.gptpSetMeanPathDelay = &Fir_gptpSetMeanPathDelay;
//    dev->SwitchDevObj.PTPObj.gptpGetMeanPathDelay = &Fir_gptpGetMeanPathDelay;
//    dev->SwitchDevObj.PTPObj.gptpSetIgrPathDelayAsym = &Fir_gptpSetIgrPathDelayAsym;
//    dev->SwitchDevObj.PTPObj.gptpGetIgrPathDelayAsym = &Fir_gptpGetIgrPathDelayAsym;
//    dev->SwitchDevObj.PTPObj.gptpSetEgrPathDelayAsym = &Fir_gptpSetEgrPathDelayAsym;
//    dev->SwitchDevObj.PTPObj.gptpGetEgrPathDelayAsym = &Fir_gptpGetEgrPathDelayAsym;
//    dev->SwitchDevObj.PTPObj.gptpGetTODBusyBitStatus = &Fir_gptpGetTODBusyBitStatus;
//    dev->SwitchDevObj.PTPObj.gptpTODStoreCompensation = &Fir_gptpTODStoreCompenIntf;
//    dev->SwitchDevObj.PTPObj.gptpTODStoreAll = &Fir_gptpTODStoreAllIntf;
//    dev->SwitchDevObj.PTPObj.gptpTODCaptureAll = &Fir_gptpTODCaptureAllIntf;
//    dev->SwitchDevObj.PTPObj.gptpSetPulse = &Fir_gptpSetPulseIntf;
//    dev->SwitchDevObj.PTPObj.gptpGetPulse = &Fir_gptpGetPulseIntf;
//    dev->SwitchDevObj.PTPObj.gtaiGetPTPGlobalTime = &Fir_gtaiGetPTPGlobalTime;
//    dev->SwitchDevObj.PTPObj.gptpGetPortReg = &Fir_gptpPortRegGet;
//    dev->SwitchDevObj.PTPObj.gptpSetPortReg = &Fir_gptpPortRegSet;
//    dev->SwitchDevObj.PTPObj.gptpGetGlobalReg = &Fir_gptpGlobalRegGet;
//    dev->SwitchDevObj.PTPObj.gptpSetGlobalReg = &Fir_gptpGlobalRegSet;
//    dev->SwitchDevObj.PTPObj.gptpGetTaiReg = &Fir_gTaiRegGet;
//    dev->SwitchDevObj.PTPObj.gptpSetTaiReg = &Fir_gTaiRegSet;
//    dev->SwitchDevObj.PTPObj.gptpSet1588v2Config = &Fir_gptp1588v2ConfigSetIntf;
//    dev->SwitchDevObj.PTPObj.gptpSet8021asConfig = &Fir_gptp8021asConfigSetIntf;
//    dev->SwitchDevObj.PTPObj.gptpSet8021asRevConfig = &Fir_gptp8021asRevConfigSetIntf;
//    dev->SwitchDevObj.PTPObj.gptpSetPreemptTsSel = &Fir_gptpPreemptTimestampSelSet;
//    dev->SwitchDevObj.PTPObj.gptpGetPreemptTsSel = &Fir_gptpPreemptTimestampSelGet;
//    dev->SwitchDevObj.PTPObj.gptpSetPreemptCtrl = &Fir_gptpPreemptCtrlSet;
//    dev->SwitchDevObj.PTPObj.gptpGetPreemptCtrl = &Fir_gptpPreemptCtrlGet;
//    dev->SwitchDevObj.PTPObj.gtaiSetTrigGenTimerSel = &Fir_gtaiTrigGenTimerSelSet;
//    dev->SwitchDevObj.PTPObj.gtaiGetTrigGenTimerSel = &Fir_gtaiTrigGenTimerSelGet;

    dev->SwitchDevObj.PORTCTRLObj.gprtSetIgnoreFcs = &Fir_gprtSetIgnoreFcs;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetIgnoreFcs = &Fir_gprtGetIgnoreFcs;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetAllowBadFcs = &Fir_gprtSetAllowBadFcs;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetAllowBadFcs = &Fir_gprtGetAllowBadFcs;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetPauseLimitOut = &Fir_gprtSetPauseLimitOut;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetPauseLimitOut = &Fir_gprtGetPauseLimitOut;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetPauseLimitIn = &Fir_gprtSetPauseLimitIn;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetPauseLimitIn = &Fir_gprtGetPauseLimitIn;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetQueueToPause = &Fir_gprtSetQueueToPause;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetQueueToPause = &Fir_gprtGetQueueToPause;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetRxPriorityFlowControlEnable = &Fir_gprtSetRxPriFlowCtrlEnable;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetRxPriorityFlowControlEnable = &Fir_gprtGetRxPriFlowCtrlEnable;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetRxPriorityFlowControlToQueue = &Fir_gprtSetRxPriFlowCtrlToQueue;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetRxPriorityFlowControlToQueue = &Fir_gprtGetRxPriFlowCtrlToQueue;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetTxPriorityFlowControlEnable = &Fir_gprtSetTxPriFlowCtrlEnable;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetTxPriorityFlowControlEnable = &Fir_gprtGetTxPriFlowCtrlEnable;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetOutQueueSize = &Fir_gprtGetOutQueueSize;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetMapDA = &Fir_gprtSetMapDA;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetMapDA = &Fir_gprtGetMapDA;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetPortLocked = &Fir_gprtSetPortLocked;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetPortLocked = &Fir_gprtGetPortLocked;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetHoldAt1 = &Fir_gprtSetHoldAt1;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetHoldAt1 = &Fir_gprtGetHoldAt1;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetInt0nAgeOut = &Fir_gprtSetInt0nAgeOut;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetInt0nAgeOut = &Fir_gprtGetInt0nAgeOut;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetRefreshLocked = &Fir_gprtSetRefreshLocked;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetRefreshLocked = &Fir_gprtGetRefreshLocked;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetIgnoreWrongData = &Fir_gprtSetIgnoreWrongData;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetIgnoreWrongData = &Fir_gprtGetIgnoreWrongData;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetARPMirror = &Fir_gprtSetARPMirror;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetARPMirror = &Fir_gprtGetARPMirror;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetForceDefaultVid = &Fir_gprtSetForceDefaultVid;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetForceDefaultVid = &Fir_gprtGetForceDefaultVid;
    dev->SwitchDevObj.PORTCTRLObj.gvlnSetPortVid = &Fir_gvlnSetPortVid;
    dev->SwitchDevObj.PORTCTRLObj.gvlnGetPortVid = &Fir_gvlnGetPortVid;
    dev->SwitchDevObj.PORTCTRLObj.gvlnSetPortVlanDot1qMode = &Fir_gvlnSetPortVlanQModeIntf;
    dev->SwitchDevObj.PORTCTRLObj.gvlnGetPortVlanDot1qMode = &Fir_gvlnGetPortVlanQModeIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetDiscardTagged = &Fir_gprtSetDiscardTaggedIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetDiscardTagged = &Fir_gprtGetDiscardTaggedIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetDiscardUntagged = &Fir_gprtSetDiscardUntaggedIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetDiscardUntagged = &Fir_gprtGetDiscardUntaggedIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetUnicastFloodBlock = &Fir_gprtSetUnicFloodBlockIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetUnicastFloodBlock = &Fir_gprtGetUnicFloodBlockIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetMulticastFloodBlock = &Fir_gprtSetMultiFloodBlockIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetMulticastFloodBlock = &Fir_gprtGetMultiFloodBlockIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetDiscardBCastMode = &Fir_gprtSetDiscardBCastModeIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetDiscardBCastMode = &Fir_gprtGetDiscardBCastModeIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetIGMPSnoopEnable = &Fir_gprtSetIGMPSnoopEnable;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetIGMPSnoopEnable = &Fir_gprtGetIGMPSnoopEnable;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetHeaderMode = &Fir_gprtSetHeaderMode;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetHeaderMode = &Fir_gprtGetHeaderMode;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetDropOnLock = &Fir_gprtSetDropOnLock;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetDropOnLock = &Fir_gprtGetDropOnLock;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetJumboMode = &Fir_gprtSetJumboModeIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetJumboMode = &Fir_gprtGetJumboModeIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetSpecifiedMTU = &Fir_gprtSetSpecifiedMTUIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetSpecifiedMTU = &Fir_gprtGetSpecifiedMTUIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetLearnEnable = &Fir_gprtSetLearnEnableIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetLearnEnable = &Fir_gprtGetLearnEnableIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetVTUPriorityOverride = &Fir_gprtSetVTUPriOverrideIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetVTUPriorityOverride = &Fir_gprtGetVTUPriOverrideIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetSAPriorityOverride = &Fir_gprtSetSAPriOverrideIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetSAPriorityOverride = &Fir_gprtGetSAPriOverrideIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetDAPriorityOverride = &Fir_gprtSetDAPriOverrideIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetDAPriorityOverride = &Fir_gprtGetDAPriOverrideIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetMessagePort = &Fir_gprtSetMessagePort;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetMessagePort = &Fir_gprtGetMessagePort;
    
	dev->SwitchDevObj.PORTCTRLObj.gprtSetEtherType = &Fir_gprtSetEtherType;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetEtherType = &Fir_gprtGetEtherType;

	dev->SwitchDevObj.PORTCTRLObj.gprtSetEtherTypeByType = &Fir_gprtSetEtherTypeByType;//Add 2024.7.18  for Fir (88Q5152) 
	dev->SwitchDevObj.PORTCTRLObj.gprtGetEtherTypeByType = &Fir_gprtGetEtherTypeByType;//Add 2024.7.18  for Fir (88Q5152) 
	
    dev->SwitchDevObj.PORTCTRLObj.gprtSetMirrorSAAndVTUMiss = &Fir_gprtSetMirrorSAAndVTUMissIntf;//Add 2024.8.23  for Fir (88Q5152)
    dev->SwitchDevObj.PORTCTRLObj.gprtGetMirrorSAAndVTUMiss = &Fir_gprtGetMirrorSAAndVTUMissIntf;//Add 2024.8.23  for Fir (88Q5152)
    dev->SwitchDevObj.PORTCTRLObj.gprtSetTrapSAAndDAMiss = &Fir_gprtSetTrapSAAndDAMissIntf;//Add 2024.8.23  for Fir (88Q5152)
    dev->SwitchDevObj.PORTCTRLObj.gprtGetTrapSAAndDAMiss = &Fir_gprtGetTrapSAAndDAMissIntf;//Add 2024.8.23  for Fir (88Q5152)
    dev->SwitchDevObj.PORTCTRLObj.gprtSetTrapVTUAndTCAMMiss = &Fir_gprtSetTrapVTUAndTCAMMissIntf;//Add 2024.8.23  for Fir (88Q5152)
    dev->SwitchDevObj.PORTCTRLObj.gprtGetTrapVTUAndTCAMMiss = &Fir_gprtGetTrapVTUAndTCAMMissIntf;//Add 2024.8.23  for Fir (88Q5152)
	dev->SwitchDevObj.PORTCTRLObj.gprtSetFIDNumber = &Fir_gprtSetFIDNumber;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetFIDNumber = &Fir_gprtGetFIDNumber;

    dev->SwitchDevObj.PORTCTRLObj.gprtSetAllowVidZero = &Fir_gprtSetAllowVidZero;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetAllowVidZero = &Fir_gprtGetAllowVidZero;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetFlowCtrl = &Fir_gprtSetFlowCtrlIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetFlowCtrl = &Fir_gprtGetFlowCtrlIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetEgressMonitorSource = &Fir_gprtSetEgressMonitorSource;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetEgressMonitorSource = &Fir_gprtGetEgressMonitorSource;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetIngressMonitorSource = &Fir_gprtSetIngressMonitorSource;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetIngressMonitorSource = &Fir_gprtGetIngressMonitorSource;
    dev->SwitchDevObj.PORTCTRLObj.grcSetEgressRate = &Fir_grcSetEgressRateIntf;
    dev->SwitchDevObj.PORTCTRLObj.grcGetEgressRate = &Fir_grcGetEgressRateIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetDuplexStatus = &Fir_gprtGetDuplexStatusIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetDuplex = &Fir_gprtSetDuplexIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetDuplex = &Fir_gprtGetDuplexIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetForceLink = &Fir_gprtGetForceLinkIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetForceLink = &Fir_gprtSetForceLinkIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetLinkState = &Fir_gprtGetLinkState;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetForceSpeed = &Fir_gprtSetForceSpeedIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetForceSpeed = &Fir_gprtGetForceSpeedIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetSpeed = &Fir_gprtGetSpeedIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetFrameMode = &Fir_gprtSetFrameModeIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetFrameMode = &Fir_gprtGetFrameModeIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetPortECID = &Fir_gprtSetPortECID;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetPortECID = &Fir_gprtGetPortECID;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetEgressMode = &Fir_gprtSetEgressModeIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetEgressMode = &Fir_gprtGetEgressModeIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetIgnoreETag = &Fir_gprtSetIgnoreETag;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetIgnoreETag = &Fir_gprtGetIgnoreETag;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetExtendedPortCtrlReg = &Fir_gprtSetExtendedPortCtrlReg;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetExtendedPortCtrlReg = &Fir_gprtGetExtendedPortCtrlReg;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetVlanPorts = &Fir_gprtSetVlanPorts;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetVlanPorts = &Fir_gprtGetVlanPorts;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetPortState = &Fir_gprtSetPortStateIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetPortState = &Fir_gprtGetPortStateIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetPreemptEnable = &Fir_gprtSetPreemptEnable;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetPreemptEnable = &Fir_gprtGetPreemptEnable;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetPreemptSize = &Fir_gprtSetPreemptSize;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetPreemptSize = &Fir_gprtGetPreemptSize;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetPreemptQueue = &Fir_gprtSetPreemptQueue;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetPreemptQueue = &Fir_gprtGetPreemptQueue;
    dev->SwitchDevObj.PORTCTRLObj.gprtRegDump = &Fir_gprtRegDump;

//    dev->SwitchDevObj.STUObj.gstuAddEntry = &Fir_gstuAddEntryIntf;
//    dev->SwitchDevObj.STUObj.gstuGetEntryNext = &Fir_gstuGetEntryNextIntf;
//    dev->SwitchDevObj.STUObj.gstuDelEntry = &Fir_gstuDelEntryIntf;
//    dev->SwitchDevObj.STUObj.gstuFindSidEntry = &Fir_gstuFindSidEntryIntf;
//    dev->SwitchDevObj.STUObj.gstuGetEntryCount = &Fir_gstuGetEntryCountIntf;
//    dev->SwitchDevObj.STUObj.gstuDump = &Fir_gstuDump;

    dev->SwitchDevObj.VTUObj.gvtuFindVidEntry = &Fir_gvtuFindVidEntryIntf;
    dev->SwitchDevObj.VTUObj.gvtuExistVidEntry = &Fir_gvtuExistVidEntryIntf;
    dev->SwitchDevObj.VTUObj.gvtuFlush = &Fir_gvtuFlushIntf;
    dev->SwitchDevObj.VTUObj.gvtuAddEntry = &Fir_gvtuAddEntryIntf;
    dev->SwitchDevObj.VTUObj.gvtuGetEntryNext = &Fir_gvtuGetEntryNextIntf;
    dev->SwitchDevObj.VTUObj.gvtuDelEntry = &Fir_gvtuDelEntryIntf;
    dev->SwitchDevObj.VTUObj.gvtuSetMode = &Fir_gvtuSetModeIntf;
    dev->SwitchDevObj.VTUObj.gvtuGetMode = &Fir_gvtuGetModeIntf;
    dev->SwitchDevObj.VTUObj.gvtuGetViolation = &Fir_gvtuGetViolationIntf;
    dev->SwitchDevObj.VTUObj.gvtuGetEntryCount = &Fir_gvtuGetEntryCountIntf;
    dev->SwitchDevObj.VTUObj.gvtuSetMemberTag = &Fir_gvtuSetMemberTagIntf;
    dev->SwitchDevObj.VTUObj.gvtuGetMemberTag = &Fir_gvtuGetMemberTagIntf;
    dev->SwitchDevObj.VTUObj.gvtuDump = &Fir_gvtuDump;

    dev->SwitchDevObj.TrunkObj.gprtSetTrunkPort = &Fir_gprtSetTrunkPort;
    dev->SwitchDevObj.TrunkObj.gprtGetTrunkPort = &Fir_gprtGetTrunkPort;
    dev->SwitchDevObj.TrunkObj.gsysSetTrunkMaskTable = &Fir_gsysSetTrunkMaskTable;
    dev->SwitchDevObj.TrunkObj.gsysGetTrunkMaskTable = &Fir_gsysGetTrunkMaskTable;
    dev->SwitchDevObj.TrunkObj.gsysSetHashTrunk = &Fir_gsysSetHashTrunk;
    dev->SwitchDevObj.TrunkObj.gsysGetHashTrunk = &Fir_gsysGetHashTrunk;
    dev->SwitchDevObj.TrunkObj.gsysSetTrunkRouting = &Fir_gsysSetTrunkRouting;
    dev->SwitchDevObj.TrunkObj.gsysGetTrunkRouting = &Fir_gsysGetTrunkRouting;

//    dev->SwitchDevObj.EEPROMObj.geepromWrite = &Fir_EEPROMWriteIntf;
//    dev->SwitchDevObj.EEPROMObj.geepromRead = &Fir_EEPROMReadIntf;
//    dev->SwitchDevObj.EEPROMObj.geepromSetMChipSel = &Fir_EEPROMChipSelSetIntf;
//    dev->SwitchDevObj.EEPROMObj.geepromGetMChipSel = &Fir_EEPROMChipSelGetIntf;

    dev->SwitchDevObj.INTObj.gdevIntEnableSet = &Fir_gsysDevIntEnableSet;
    dev->SwitchDevObj.INTObj.gdevIntEnableGet = &Fir_gsysDevIntEnableGet;
    dev->SwitchDevObj.INTObj.gdevIntStatusGet = &Fir_gsysDevIntStatusGet;
    dev->SwitchDevObj.INTObj.gactiveIntEnableSet = &Fir_gsysActiveIntEnableSet;
    dev->SwitchDevObj.INTObj.gactiveIntEnableGet = &Fir_gsysActiveIntEnableGet;
    dev->SwitchDevObj.INTObj.gactiveIntStatusGet = &Fir_gsysActiveIntStatusGet;

//    dev->SwitchDevObj.PIRLObj.gpirlInitialize = &Fir_gpirlInitializeIntf;
//    dev->SwitchDevObj.PIRLObj.gpirlInitResource = &Fir_gpirlInitResourceIntf;
//    dev->SwitchDevObj.PIRLObj.gpirlCustomSetup = &Fir_gpirlCustomSetup_sr2cIntf;
//    dev->SwitchDevObj.PIRLObj.gpirlReadResource = &Fir_gpirlReadResourceIntf;
//    dev->SwitchDevObj.PIRLObj.gpirlWriteResource = &Fir_gpirlWriteResourceIntf;
//    dev->SwitchDevObj.PIRLObj.gpirlGetResReg = &Fir_gpirlGetResRegIntf;
//    dev->SwitchDevObj.PIRLObj.gpirlSetResReg = &Fir_gpirlSetResRegIntf;
//    dev->SwitchDevObj.PIRLObj.gpirlAdvConfig = &Fir_gpirlAdvConfigIntf;
//    dev->SwitchDevObj.PIRLObj.gpirlAdvConfigDisable = &Fir_gpirlAdvConfigDisableIntf;

//    dev->SwitchDevObj.QOSMAPObj.gprtGetIEEEPrioMap = &Fir_gprtGetIEEEPrioMapIntf;
//    dev->SwitchDevObj.QOSMAPObj.gprtSetIEEEPrioMap = &Fir_gprtSetIEEEPrioMapIntf;
//    dev->SwitchDevObj.QOSMAPObj.gqosGetPrioMapRule = &Fir_gqosGetPrioMapRuleIntf;
//    dev->SwitchDevObj.QOSMAPObj.gprtGetInitialPri = &Fir_gprtGetInitialPriIntf;
//    dev->SwitchDevObj.QOSMAPObj.gprtSetDefQpri = &Fir_gprtSetDefQpriIntf;
//    dev->SwitchDevObj.QOSMAPObj.gprtGetDefQpri = &Fir_gprtGetDefQpriIntf;
//    dev->SwitchDevObj.QOSMAPObj.gprtSetDefFpri = &Fir_gprtSetDefFpri;
//    dev->SwitchDevObj.QOSMAPObj.gprtGetDefFpri = &Fir_gprtGetDefFpri;
//    dev->SwitchDevObj.QOSMAPObj.gprtSetInitialPri = &Fir_gprtSetInitialPriIntf;
//    dev->SwitchDevObj.QOSMAPObj.gqosSetPrioMapRule = &Fir_gqosSetPrioMapRuleIntf;
//    dev->SwitchDevObj.QOSMAPObj.gprtRewriteDscp = &Fir_gprtRewriteDscp;
//    dev->SwitchDevObj.QOSMAPObj.gprtSetCosToQueue = &Fir_gprtSetCosToQueue;
//    dev->SwitchDevObj.QOSMAPObj.gprtGetCosToQueue = &Fir_gprtGetCosToQueue;
//    dev->SwitchDevObj.QOSMAPObj.gprtSetCosToDscp = &Fir_gprtSetCosToDscp;
//    dev->SwitchDevObj.QOSMAPObj.gprtGetCosToDscp = &Fir_gprtGetCosToDscp;

//    dev->SwitchDevObj.PORTRMONObj.gstatsFlushAll = &Fir_gstatsFlushAll;
//    dev->SwitchDevObj.PORTRMONObj.gstatsFlushPort = &Fir_gstatsFlushPort;
//    dev->SwitchDevObj.PORTRMONObj.gstatsGetPortCounter = &Fir_gstatsGetPortCounterIntf;
//    dev->SwitchDevObj.PORTRMONObj.gstatsGetPortAllCounters = &Fir_gstatsGetPortAllCntersIntf;
//    dev->SwitchDevObj.PORTRMONObj.gstatsGetHistogramMode = &Fir_gstatsGetHistogramModeIntf;
//    dev->SwitchDevObj.PORTRMONObj.gstatsSetHistogramMode = &Fir_gstatsSetHistogramModeIntf;
//    dev->SwitchDevObj.PORTRMONObj.gmibDump = &Fir_gmibDump;

//    dev->SwitchDevObj.QUEUEObj.gprtSetQueueCtrl = &Fir_gprtSetQueueCtrlIntf;
//    dev->SwitchDevObj.QUEUEObj.gprtGetQueueCtrl = &Fir_gprtGetQueueCtrlIntf;
//    dev->SwitchDevObj.QUEUEObj.gprtSetPortSched = &Fir_gprtSetPortSchedIntf;
//    dev->SwitchDevObj.QUEUEObj.gprtGetPortSched = &Fir_gprtGetPortSchedIntf;
//    dev->SwitchDevObj.QUEUEObj.gprtSetQoSWeight = &Fir_gprtSetQoSWeightIntf;
//    dev->SwitchDevObj.QUEUEObj.gprtGetQoSWeight = &Fir_gprtGetQoSWeightIntf;

    dev->SwitchDevObj.SYSCTRLObj.gsysSetFloodBC = &Fir_gsysSetFloodBC;
    dev->SwitchDevObj.SYSCTRLObj.gsysGetFloodBC = &Fir_gsysGetFloodBC;
	
	dev->SwitchDevObj.SYSCTRLObj.gsysSetRemove1Ptag = &Fir_gsysSetRemove1Ptag;//add 2024.9.25
	dev->SwitchDevObj.SYSCTRLObj.gsysGetRemove1Ptag = &Fir_gsysGetRemove1Ptag;

    dev->SwitchDevObj.SYSCTRLObj.gsysSetEgressMonitorDest = &Fir_gsysSetEgressMonitorDest;
    dev->SwitchDevObj.SYSCTRLObj.gsysGetEgressMonitorDest = &Fir_gsysGetEgressMonitorDest;
    dev->SwitchDevObj.SYSCTRLObj.gsysSetSMISetup = &Fir_gsysSetSMISetup;
    dev->SwitchDevObj.SYSCTRLObj.gsysGetSMISetup = &Fir_gsysGetSMISetup;
    dev->SwitchDevObj.SYSCTRLObj.gsysGlobal1RegDump = &Fir_gsysGlobal1RegDump;
    dev->SwitchDevObj.SYSCTRLObj.gsysGlobal2RegDump = &Fir_gsysGlobal2RegDump;
    dev->SwitchDevObj.SYSCTRLObj.gsysSetRMUMode = &Fir_gsysSetRMUMode;
    dev->SwitchDevObj.SYSCTRLObj.gsysGetRMUMode = &Fir_gsysGetRMUMode;
    dev->SwitchDevObj.SYSCTRLObj.gsysQbvWrite = &Fir_gsysQbvWrite;
    dev->SwitchDevObj.SYSCTRLObj.gsysQbvRead = &Fir_gsysQbvRead;

//    dev->SwitchDevObj.ECIDObj.gBpeEnableSet = &Fir_gBpeEnableSet;
//    dev->SwitchDevObj.ECIDObj.gBpeEnableGet = &Fir_gBpeEnableGet;
//    dev->SwitchDevObj.ECIDObj.gecidAddEntry = &Fir_gecidAddEntryIntf;
//    dev->SwitchDevObj.ECIDObj.gecidDump = &Fir_gecidDump;
//    dev->SwitchDevObj.ECIDObj.gecidFindEcidEntry = &Fir_gecidFindEcidEntryIntf;
//    dev->SwitchDevObj.ECIDObj.gecidFlushAll = &Fir_gecidFlushAll;
//    dev->SwitchDevObj.ECIDObj.gecidFlushEntry = &Fir_gecidFlushEntry;
//    dev->SwitchDevObj.ECIDObj.gecidGetEcidEntryNext = &Fir_gecidGetEcidEntryNextIntf;
//    dev->SwitchDevObj.ECIDObj.gecidGetEntryCount = &Fir_gecidGetEntryCount;
//    dev->SwitchDevObj.ECIDObj.gecidMove = &Fir_gecidMove;

    dev->SwitchDevObj.TCAMObj.gtcamFlushAll = &Fir_gtcamFlushAll;
    dev->SwitchDevObj.TCAMObj.gtcamFlushEntry = &Fir_gtcamFlushEntry;
    dev->SwitchDevObj.TCAMObj.gtcamLoadEntry = &Fir_gtcamLoadEntryIntf;
    dev->SwitchDevObj.TCAMObj.gtcamReadTCAMData = &Fir_gtcamReadTCAMDataIntf;
    dev->SwitchDevObj.TCAMObj.gtcamGetNextTCAMData = &Fir_gtcamGetNextTCAMDataIntf;
    dev->SwitchDevObj.TCAMObj.gtcamFindEntry = &Fir_gtcamFindEntryIntf;
    dev->SwitchDevObj.TCAMObj.gtcamSetMode = &Fir_gtcamSetModeIntf;
    dev->SwitchDevObj.TCAMObj.gtcamGetMode = &Fir_gtcamGetModeIntf;
    dev->SwitchDevObj.TCAMObj.gtcamAddEntryPort = &Fir_gtcamAddEntryPort;
    dev->SwitchDevObj.TCAMObj.gtcamRemoveEntryPort = &Fir_gtcamRemoveEntryPort;
    dev->SwitchDevObj.TCAMObj.gtcamEgrFlushEntry = &Fir_gtcamEgrFlushEntryIntf;
    dev->SwitchDevObj.TCAMObj.gtcamEgrFlushEntryAllPorts = &Fir_gtcamEgrFlushAllPortsIntf;
    dev->SwitchDevObj.TCAMObj.gtcamEgrLoadEntry = &Fir_gtcamEgrLoadEntryIntf;
    dev->SwitchDevObj.TCAMObj.gtcamEgrGetNextTCAMData = &Fir_gtcamEgrGetNextTCAMDataIntf;
    dev->SwitchDevObj.TCAMObj.gtcamEgrReadTCAMData = &Fir_gtcamEgrReadTCAMDataIntf;
    dev->SwitchDevObj.TCAMObj.gtcamDump = &Fir_gtcamDump;
    dev->SwitchDevObj.TCAMObj.gtcamAdvConfig = &Fir_gtcamAdvConfigIntf;

//    dev->SwitchDevObj.TCAMRCObj.gtcamRCLoadEntry = &Fir_gtcamRCLoadEntryIntf;
//    dev->SwitchDevObj.TCAMRCObj.gtcamRCFlushAllEntry = &Fir_gtcamRCFlushAllEntry;
//    dev->SwitchDevObj.TCAMRCObj.gtcamRCFlushEntry = &Fir_gtcamRCFlushEntry;
//    dev->SwitchDevObj.TCAMRCObj.gtcamRCReadEntry = &Fir_gtcamRCReadEntryIntf;
//    dev->SwitchDevObj.TCAMRCObj.gtcamRCGetNextEntry = &Fir_gtcamRCGetNextEntryIntf;
//    dev->SwitchDevObj.TCAMRCObj.gtcamRCFindEntry = &Fir_gtcamRCFindEntryIntf;
//    dev->SwitchDevObj.TCAMRCObj.gtcamRCEntryDump = &Fir_gtcamRCEntryDump;
//
//    dev->SwitchDevObj.TCAMRCCompObj.gtcamRCCompLoad = &Fir_gtcamRCCompLoadIntf;
//    dev->SwitchDevObj.TCAMRCCompObj.gtcamRCCompFlushAll = &Fir_gtcamRCCompFlushAll;
//    dev->SwitchDevObj.TCAMRCCompObj.gtcamRCCompFlushPort = &Fir_gtcamRCCompFlushPort;
//    dev->SwitchDevObj.TCAMRCCompObj.gtcamRCCompRead = &Fir_gtcamRCCompReadIntf;

//    dev->SwitchDevObj.RMUObj.grmuTCAMDump = &Fir_msdRmuTCAMDumpIntf;
//    dev->SwitchDevObj.RMUObj.grmuTCAMRead = &Fir_msdRmuTCAMReadIntf;
//    dev->SwitchDevObj.RMUObj.grmuTCAMWrite = &Fir_msdRmuTCAMWriteIntf;
//    dev->SwitchDevObj.RMUObj.grmuEcidDump = &Fir_msdRmuEcidDumpIntf;
//
//    dev->SwitchDevObj.RMUObj.grmuGetID = &Fir_msdRmuGetIDIntf;
//    dev->SwitchDevObj.RMUObj.grmuAtuDump = &Fir_msdRmuAtuDumpIntf;
//    dev->SwitchDevObj.RMUObj.grmuMib2Dump = &Fir_msdRmuMib2DumpIntf;
//    dev->SwitchDevObj.RMUObj.grmuMultiRegAccess = &Fir_msdRmuMultiRegAccessIntf;
//    dev->SwitchDevObj.RMUObj.grmuRegDump = &Fir_msdRmuRegDump;
//
//    dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIC45PhyReg = &Fir_msdGetSMIC45PhyReg;
//    dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIC45PhyReg = &Fir_msdSetSMIC45PhyReg;
//    dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIPhyReg = &Fir_msdGetSMIPhyReg;
//    dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIPhyReg = &Fir_msdSetSMIPhyReg;
//    dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIPhyXMDIOReg = &Fir_msdSetSMIPhyXMDIOReg;
//    dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIPhyXMDIOReg = &Fir_msdGetSMIPhyXMDIOReg;
//    dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIExtPhyReg = &Fir_msdGetSMIExtPhyReg;
//    dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIExtPhyReg = &Fir_msdSetSMIExtPhyReg;
//    dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIExtPhyXMDIOReg = &Fir_msdSetSMIExtPhyXMDIOReg;
//    dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIExtPhyXMDIOReg = &Fir_msdGetSMIExtPhyXMDIOReg;

//    dev->SwitchDevObj.LEDObj.gledForceOn = &Fir_gledForceOnIntf;
//    dev->SwitchDevObj.LEDObj.gledForceOff = &Fir_gledForceOffIntf;
//    dev->SwitchDevObj.LEDObj.gledLinkAct = &Fir_gledLinkActIntf;
//    dev->SwitchDevObj.LEDObj.gledModeSet = &Fir_gledModeSetIntf;
//    dev->SwitchDevObj.LEDObj.gledModeGet = &Fir_gledModeGetIntf;
//    dev->SwitchDevObj.LEDObj.gledBlinkRateSet = &Fir_gledBlinkRateSetIntf;
//    dev->SwitchDevObj.LEDObj.gledBlinkRateGet = &Fir_gledBlinkRateGetIntf;
//    dev->SwitchDevObj.LEDObj.gledPulseStretchSet = &Fir_gledPulseStretchSetIntf;
//    dev->SwitchDevObj.LEDObj.gledPulseStretchGet = &Fir_gledPulseStretchGetIntf;
//    dev->SwitchDevObj.LEDObj.gledSpecialModeSet = &Fir_gledSpecialModeSetIntf;
//    dev->SwitchDevObj.LEDObj.gledSpecialModeGet = &Fir_gledSpecialModeGetIntf;
//    dev->SwitchDevObj.LEDObj.gledCtrlSet = &Fir_gledCtrlSetIntf;
//    dev->SwitchDevObj.LEDObj.gledCtrlGet = &Fir_gledCtrlGetIntf;
//    dev->SwitchDevObj.LEDObj.gledDump = &Fir_gledDump;

//    dev->SwitchDevObj.POLICYObj.gpolicySetMgmtCtrl = &Fir_gpolicySetMgmtCtrl;
//    dev->SwitchDevObj.POLICYObj.gpolicyGetMgmtCtrl = &Fir_gpolicyGetMgmtCtrl;
//    dev->SwitchDevObj.POLICYObj.gpolicySetCpuDest = &Fir_gpolicySetCpuDest;
//    dev->SwitchDevObj.POLICYObj.gpolicyGetCpuDest = &Fir_gpolicyGetCpuDest;
//    dev->SwitchDevObj.POLICYObj.gpolicySetIngressMonitorDest = &Fir_gpolicySetIgrMonitorDest;
//    dev->SwitchDevObj.POLICYObj.gpolicyGetIngressMonitorDest = &Fir_gpolicyGetIgrMonitorDest;
//    dev->SwitchDevObj.POLICYObj.gpolicySetRsvd2CpuEnables = &Fir_gpolicySetRsvd2CpuEnables;
//    dev->SwitchDevObj.POLICYObj.gpolicyGetRsvd2CpuEnables = &Fir_gpolicyGetRsvd2CpuEnables;
//    dev->SwitchDevObj.POLICYObj.gpolicySetMGMTPri = &Fir_gpolicySetMGMTPri;
//    dev->SwitchDevObj.POLICYObj.gpolicyGetMGMTPri = &Fir_gpolicyGetMGMTPri;
//    dev->SwitchDevObj.POLICYObj.gpolicySetRouteDest = &Fir_gpolicySetRouteDest;
//    dev->SwitchDevObj.POLICYObj.gpolicyGetRouteDest = &Fir_gpolicyGetRouteDest;
//    dev->SwitchDevObj.POLICYObj.gpolicySetMirrorDest = &Fir_gpolicySetMirrorDest;
//    dev->SwitchDevObj.POLICYObj.gpolicyGetMirrorDest = &Fir_gpolicyGetMirrorDest;
//    dev->SwitchDevObj.POLICYObj.gpolicySetIllegalIPAddr = &Fir_gpolicySetIllegalIPAddr;
//    dev->SwitchDevObj.POLICYObj.gpolicyGetIllegalIPAddr = &Fir_gpolicyGetIllegalIPAddr;
//    dev->SwitchDevObj.POLICYObj.gpolicySetDipDaMismatch = &Fir_gpolicySetDipDaMismatch;
//    dev->SwitchDevObj.POLICYObj.gpolicyGetDipDaMismatch = &Fir_gpolicyGetDipDaMismatch;
//    dev->SwitchDevObj.POLICYObj.gpolicySetZeroSIP = &Fir_gpolicySetZeroSIP;
//    dev->SwitchDevObj.POLICYObj.gpolicyGetZeroSIP = &Fir_gpolicyGetZeroSIP;
//    dev->SwitchDevObj.POLICYObj.gpolicySetMcTtlExceeded = &Fir_gpolicySetMcTtlExceeded;
//    dev->SwitchDevObj.POLICYObj.gpolicyGetMcTtlExceeded = &Fir_gpolicyGetMcTtlExceeded;
//    dev->SwitchDevObj.POLICYObj.gpolicySetUcTtlExceeded = &Fir_gpolicySetUcTtlExceeded;
//    dev->SwitchDevObj.POLICYObj.gpolicyGetUcTtlExceeded = &Fir_gpolicyGetUcTtlExceeded;
//    dev->SwitchDevObj.POLICYObj.gprtSetPortTtlThreshold = &Fir_gprtSetPortTtlThreshold;
//    dev->SwitchDevObj.POLICYObj.gprtGetPortTtlThreshold = &Fir_gprtGetPortTtlThreshold;
//    dev->SwitchDevObj.POLICYObj.gpolicySetIpHdrErr = &Fir_gpolicySetIpHdrErr;
//    dev->SwitchDevObj.POLICYObj.gpolicyGetIpHdrErr = &Fir_gpolicyGetIpHdrErr;
//    dev->SwitchDevObj.POLICYObj.gpolicySetTCPFinUrgPsh = &Fir_gpolicySetTCPFinUrgPsh;
//    dev->SwitchDevObj.POLICYObj.gpolicyGetTCPFinUrgPsh = &Fir_gpolicyGetTCPFinUrgPsh;
//    dev->SwitchDevObj.POLICYObj.gpolicySetTCPFlagsSeqNum = &Fir_gpolicySetTCPFlagsSeqNum;
//    dev->SwitchDevObj.POLICYObj.gpolicyGetTCPFlagsSeqNum = &Fir_gpolicyGetTCPFlagsSeqNum;
//    dev->SwitchDevObj.POLICYObj.gpolicySetTCPSYN = &Fir_gpolicySetTCPSYN;
//    dev->SwitchDevObj.POLICYObj.gpolicyGetTCPSYN = &Fir_gpolicyGetTCPSYN;
//    dev->SwitchDevObj.POLICYObj.gpolicySetTCPHSize = &Fir_gpolicySetTCPHSize;
//    dev->SwitchDevObj.POLICYObj.gpolicyGetTCPHSize = &Fir_gpolicyGetTCPHSize;
//    dev->SwitchDevObj.POLICYObj.gprtSetPortTCPMinHdrSize = &Fir_gprtSetPortTCPMinHdrSize;
//    dev->SwitchDevObj.POLICYObj.gprtGetPortTCPMinHdrSize = &Fir_gprtGetPortTCPMinHdrSize;
//    dev->SwitchDevObj.POLICYObj.gpolicySetICMPSize = &Fir_gpolicySetICMPSize;
//    dev->SwitchDevObj.POLICYObj.gpolicyGetICMPSize = &Fir_gpolicyGetICMPSize;
//    dev->SwitchDevObj.POLICYObj.gprtSetPortICMPMaxSize = &Fir_gprtSetPortICMPMaxSize;
//    dev->SwitchDevObj.POLICYObj.gprtGetPortICMPMaxSize = &Fir_gprtGetPortICMPMaxSize;
//    dev->SwitchDevObj.POLICYObj.gpolicySetUDPPort = &Fir_gpolicySetUDPPort;
//    dev->SwitchDevObj.POLICYObj.gpolicyGetUDPPort = &Fir_gpolicyGetUDPPort;
//    dev->SwitchDevObj.POLICYObj.gpolicySetTCPPort = &Fir_gpolicySetTCPPort;
//    dev->SwitchDevObj.POLICYObj.gpolicyGetTCPPort = &Fir_gpolicyGetTCPPort;
//    dev->SwitchDevObj.POLICYObj.gpolicySetTCPSynFin = &Fir_gpolicySetTCPSynFin;
//    dev->SwitchDevObj.POLICYObj.gpolicyGetTCPSynFin = &Fir_gpolicyGetTCPSynFin;
//    dev->SwitchDevObj.POLICYObj.gpolicySetTCPFrag = &Fir_gpolicySetTCPFrag;
//    dev->SwitchDevObj.POLICYObj.gpolicyGetTCPFrag = &Fir_gpolicyGetTCPFrag;
//    dev->SwitchDevObj.POLICYObj.gpolicySetNearNonTPMR = &Fir_gpolicySetNearNonTPMR;
//    dev->SwitchDevObj.POLICYObj.gpolicyGetNearNonTPMR = &Fir_gpolicyGetNearNonTPMR;
//    dev->SwitchDevObj.POLICYObj.gpolicySetETag = &Fir_gpolicySetETag;
//    dev->SwitchDevObj.POLICYObj.gpolicyGetETag = &Fir_gpolicyGetETag;
//    dev->SwitchDevObj.POLICYObj.gpolicySetNonETag = &Fir_gpolicySetNonETag;
//    dev->SwitchDevObj.POLICYObj.gpolicyGetNonETag = &Fir_gpolicyGetNonETag;
//    dev->SwitchDevObj.POLICYObj.gpolicyDump = &Fir_gpolicyDump;

//    dev->SwitchDevObj.FRERObj.gfrerSeqGenFlushAll = &Fir_gfrerSeqGenFlushAll;
//    dev->SwitchDevObj.FRERObj.gfrerSeqGenFlushEntry = &Fir_gfrerSeqGenFlushEntry;
//    dev->SwitchDevObj.FRERObj.gfrerSeqGenRead = &Fir_gfrerSeqGenReadIntf;
//    dev->SwitchDevObj.FRERObj.gfrerSeqGenLoad = &Fir_gfrerSeqGenLoadIntf;
//
//    dev->SwitchDevObj.FRERObj.gfrerIndvRcvyFlushAll = &Fir_gfrerIndvRcvyFlushAll;
//    dev->SwitchDevObj.FRERObj.gfrerIndvRcvyFlushEntry = &Fir_gfrerIndvRcvyFlushEntry;
//    dev->SwitchDevObj.FRERObj.gfrerIndvRcvyRead = &Fir_gfrerIndvRcvyReadIntf;
//	dev->SwitchDevObj.FRERObj.gfrerIndvRcvyLoad = &Fir_gfrerIndvRcvyLoadIntf;
//    dev->SwitchDevObj.FRERObj.gfrerIndvRcvyGetNext = &Fir_gfrerIndvRcvyGetNextIntf;
//    dev->SwitchDevObj.FRERObj.gfrerIndvRcvyReadClear = &Fir_gfrerIndvRcvyReadClearIntf;
//
//    dev->SwitchDevObj.FRERObj.gfrerSeqRcvyFlushAll = &Fir_gfrerSeqRcvyFlushAll;
//    dev->SwitchDevObj.FRERObj.gfrerSeqRcvyFlushEntry = &Fir_gfrerSeqRcvyFlushEntry;
//    dev->SwitchDevObj.FRERObj.gfrerSeqRcvyRead = &Fir_gfrerSeqRcvyReadIntf;
//    dev->SwitchDevObj.FRERObj.gfrerSeqRcvyLoad = &Fir_gfrerSeqRcvyLoadIntf;
//    dev->SwitchDevObj.FRERObj.gfrerSeqRcvyGetNext = &Fir_gfrerSeqRcvyGetNextIntf;
//    dev->SwitchDevObj.FRERObj.gfrerSeqRcvyGetViolation = &Fir_gfrerSeqRcvyGetViolationIntf;
//    dev->SwitchDevObj.FRERObj.gfrerSeqRcvyReadClear = &Fir_gfrerSeqRcvyReadClearIntf;

//    dev->SwitchDevObj.FRERObj.gfrerBank0SCtrFlushAll = &Fir_gfrerBank0SCtrFlushAll;
//    dev->SwitchDevObj.FRERObj.gfrerBank0SCtrFlushEntry = &Fir_gfrerBank0SCtrFlushCtr;
//    dev->SwitchDevObj.FRERObj.gfrerBank0SCtrRead = &Fir_gfrerBank0SCtrReadIntf;
//    dev->SwitchDevObj.FRERObj.gfrerBank0SCtrLoad = &Fir_gfrerBank0SCtrLoadIntf;
//    dev->SwitchDevObj.FRERObj.gfrerBank0SCtrGetNext = &Fir_gfrerBank0SCtrGetNextIntf;
//    dev->SwitchDevObj.FRERObj.gfrerBank0SCtrReadClear = &Fir_gfrerBank0SCtrReadClearIntf;
//
//    dev->SwitchDevObj.FRERObj.gfrerBank1SCtrFlushAll = &Fir_gfrerBank1SCtrFlushAll;
//    dev->SwitchDevObj.FRERObj.gfrerBank1SCtrFlushEntry = &Fir_gfrerBank1SCtrFlushCtr;
//    dev->SwitchDevObj.FRERObj.gfrerBank1SCtrRead = &Fir_gfrerBank1SCtrReadIntf;
//    dev->SwitchDevObj.FRERObj.gfrerBank1SCtrLoad = &Fir_gfrerBank1SCtrLoadIntf;
//    dev->SwitchDevObj.FRERObj.gfrerBank1SCtrGetNext = &Fir_gfrerBank1SCtrGetNextIntf;
//    dev->SwitchDevObj.FRERObj.gfrerBank1SCtrReadClear = &Fir_gfrerBank1SCtrReadClearIntf;
//
//    dev->SwitchDevObj.QCRObj.gqcrFlushAll = &Fir_gatsFlushAll;
//    dev->SwitchDevObj.QCRObj.gqcrFlushOne = &Fir_gatsFlushOne;
//    dev->SwitchDevObj.QCRObj.gqcrSetGrpId = &Fir_gatsSetGrpId;
//    dev->SwitchDevObj.QCRObj.gqcrGetGrpId = &Fir_gatsGetGrpId;
//    dev->SwitchDevObj.QCRObj.gqcrSetCbsLimit = &Fir_gatsSetCbsLimit;
//    dev->SwitchDevObj.QCRObj.gqcrGetCbsLimit = &Fir_gatsGetCbsLimit;
//    dev->SwitchDevObj.QCRObj.gqcrSetMaxResidenceTime = &Fir_gatsSetMaxResidenceTime;
//    dev->SwitchDevObj.QCRObj.gqcrGetMaxResidenceTime = &Fir_gatsGetMaxResidenceTime;
//    dev->SwitchDevObj.QCRObj.gqcrSetRateLimit = &Fir_gatsSetRateLimit;
//    dev->SwitchDevObj.QCRObj.gqcrGetRateLimit = &Fir_gatsGetRateLimit;
//    dev->SwitchDevObj.QCRObj.gqcrSetQueueEnables = &Fir_gatsSetQueueEnables;
//    dev->SwitchDevObj.QCRObj.gqcrGetQueueEnables = &Fir_gatsGetQueueEnables;

//	dev->SwitchDevObj.QCIObj.gqciStreamFiltFlushAll = &Fir_gqciStreamFiltFlushAll;
//	dev->SwitchDevObj.QCIObj.gqciStreamFiltSet = &Fir_gqciStreamFiltSet;
//	dev->SwitchDevObj.QCIObj.gqciStreamFiltBlockStatSet = &Fir_gqciStreamFiltBlockStatSet;
//	dev->SwitchDevObj.QCIObj.gqciStreamFiltBlockStatGet = &Fir_gqciStreamFiltBlockStatGet;
//	dev->SwitchDevObj.QCIObj.gqciStreamFiltEnAts = &Fir_gqciStreamFiltEnAts;
//	dev->SwitchDevObj.QCIObj.gqciStreamFiltEnGate = &Fir_gqciStreamFiltEnGate;
//	dev->SwitchDevObj.QCIObj.gqciStreamFiltMatchCtrGet = &Fir_gqciStreamFiltMatchCtrGet;
//	dev->SwitchDevObj.QCIObj.gqciStreamFiltFilterCtrGet = &Fir_gqciStreamFiltFilterCtrGet;
//	dev->SwitchDevObj.QCIObj.gqciStreamGateFiltCtrGet = &Fir_gqciStreamGateFiltCtrGet;
//	dev->SwitchDevObj.QCIObj.gqciFlowMeterFiltCtrGet = &Fir_gqciFlowMeterFiltCtrGet;
//
//	dev->SwitchDevObj.QCIObj.gqciStreamGateFlushAll = &Fir_gqciStreamGateFlushAll;
//	dev->SwitchDevObj.QCIObj.gqciStreamGateDefStateSet = &Fir_gqciStreamGateDefStateSet;
//	dev->SwitchDevObj.QCIObj.gqciStreamGateScaleSet = &Fir_gqciStreamGateScaleSet;
//	dev->SwitchDevObj.QCIObj.gqciStreamGateTimeGenIdSet = &Fir_gqciStreamGateTimeGenIdSet;
//	dev->SwitchDevObj.QCIObj.gqciStreamGateEntryAdd = &Fir_gqciStreamGateEntryAddIntf;
//
//	dev->SwitchDevObj.QCIObj.gqciStreamGateTimeGenFlush = &Fir_gqciStreamGateTimeGenFlush;
//	dev->SwitchDevObj.QCIObj.gqciStreamGateTimeGenSet = &Fir_gqciStreamGateTimeGenSet;

//	dev->SwitchDevObj.QCIObj.gqciFlowMeterFlushAll = &Fir_gqciFlowMeterFlushAll;
//	dev->SwitchDevObj.QCIObj.gqciFlowMeterCoupleFlagSet = &Fir_gqciFlowMeterCoupleFlagSet;
//	dev->SwitchDevObj.QCIObj.gqciFlowMeterCoupleFlagGet = &Fir_gqciFlowMeterCoupleFlagGet;
//	dev->SwitchDevObj.QCIObj.gqciFlowMeterDropOnYelSet = &Fir_gqciFlowMeterDropOnYelSet;
//	dev->SwitchDevObj.QCIObj.gqciFlowMeterDropOnYelGet = &Fir_gqciFlowMeterDropOnYelGet;
//	dev->SwitchDevObj.QCIObj.gqciFlowMeterRedBlockEnSet = &Fir_gqciFlowMeterRedBlockEnSet;
//	dev->SwitchDevObj.QCIObj.gqciFlowMeterRedBlockEnGet = &Fir_gqciFlowMeterRedBlockEnGet;
//	dev->SwitchDevObj.QCIObj.gqciFlowMeterRedBlkStatSet = &Fir_gqciFlowMeterRedBlkStatSet;
//	dev->SwitchDevObj.QCIObj.gqciFlowMeterRedBlkStatGet = &Fir_gqciFlowMeterRedBlkStatGet;
//	dev->SwitchDevObj.QCIObj.gqciFlowMeterAdvConfig = &Fir_gqciFlowMeterAdvConfig;

//	dev->SwitchDevObj.MACSECObj.gmacsecFlushAll = &Fir_gmacsecFlushAll;
//	dev->SwitchDevObj.MACSECObj.gmacsecCreateRule = &Fir_gmacsecCreateRuleIntf;
//	dev->SwitchDevObj.MACSECObj.gmacsecDeleteRule = &Fir_gmacsecDeleteRule;
//	dev->SwitchDevObj.MACSECObj.gmacsecGetRule = &Fir_gmacsecGetRuleIntf;
//	dev->SwitchDevObj.MACSECObj.gmacsecCreateTxSecY = &Fir_gmacsecCreateTxSecYIntf;
//	dev->SwitchDevObj.MACSECObj.gmacsecDeleteTxSecY = &Fir_gmacsecDeleteTxSecY;
//	dev->SwitchDevObj.MACSECObj.gmacsecGetTxSecY = &Fir_gmacsecGetTxSecYIntf;
//	dev->SwitchDevObj.MACSECObj.gmacsecCreateRxSecY = &Fir_gmacsecCreateRxSecYIntf;
//	dev->SwitchDevObj.MACSECObj.gmacsecDeleteRxSecY = &Fir_gmacsecDeleteRxSecY;
//	dev->SwitchDevObj.MACSECObj.gmacsecGetRxSecY = &Fir_gmacsecGetRxSecYIntf;
//	dev->SwitchDevObj.MACSECObj.gmacsecCreateRxSC = &Fir_gmacsecCreateRxSCIntf;
//	dev->SwitchDevObj.MACSECObj.gmacsecDeleteRxSC = &Fir_gmacsecDeleteRxSC;
//	dev->SwitchDevObj.MACSECObj.gmacsecGetRxSC = &Fir_gmacsecGetRxSCIntf;
//	dev->SwitchDevObj.MACSECObj.gmacsecCreateRxSA = &Fir_gmacsecCreateRxSAIntf;
//	dev->SwitchDevObj.MACSECObj.gmacsecDeleteRxSA = &Fir_gmacsecDeleteRxSA;
//	dev->SwitchDevObj.MACSECObj.gmacsecGetRxSA = &Fir_gmacsecGetRxSAIntf;
//	dev->SwitchDevObj.MACSECObj.gmacsecCreateTxSC = &Fir_gmacsecCreateTxSCIntf;
//	dev->SwitchDevObj.MACSECObj.gmacsecDeleteTxSC = &Fir_gmacsecDeleteTxSC;
//	dev->SwitchDevObj.MACSECObj.gmacsecGetTxSC = &Fir_gmacsecGetTxSCIntf;
//	dev->SwitchDevObj.MACSECObj.gmacsecCreateTxSA = &Fir_gmacsecCreateTxSAIntf;
//	dev->SwitchDevObj.MACSECObj.gmacsecDeleteTxSA = &Fir_gmacsecDeleteTxSA;
//	dev->SwitchDevObj.MACSECObj.gmacsecGetTxSA = &Fir_gmacsecGetTxSAIntf;
//	dev->SwitchDevObj.MACSECObj.gmacsecGetRXSCStates = &Fir_gmacsecGetRXSCStatesIntf;
//	dev->SwitchDevObj.MACSECObj.gmacsecGetRXSecYStates = &Fir_gmacsecGetRXSecYStatesIntf;
//	dev->SwitchDevObj.MACSECObj.gmacsecGetTXSCStates = &Fir_gmacsecGetTXSCStatesIntf;
//	dev->SwitchDevObj.MACSECObj.gmacsecGetTXSecYStates = &Fir_gmacsecGetTXSecYStatesIntf;
//	dev->SwitchDevObj.MACSECObj.gmacsecEnableEgressSet = &Fir_gmacsecEnableEgressSet;
//	dev->SwitchDevObj.MACSECObj.gmacsecEnableEgressGet = &Fir_gmacsecEnableEgressGet;
//	dev->SwitchDevObj.MACSECObj.gmacsecEnableIngressSet = &Fir_gmacsecEnableIngressSet;
//	dev->SwitchDevObj.MACSECObj.gmacsecEnableIngressGet = &Fir_gmacsecEnableIngressGet;
//	dev->SwitchDevObj.MACSECObj.gmacsecSetTxPNThreshold = &Fir_gmacsecSetTxPNThreshold;
//	dev->SwitchDevObj.MACSECObj.gmacsecGetTxPNThreshold = &Fir_gmacsecGetTxPNThreshold;
//	dev->SwitchDevObj.MACSECObj.gmacsecSetTxXPNThreshold = &Fir_gmacsecSetTxXPNThreshold;
//	dev->SwitchDevObj.MACSECObj.gmacsecGetTxXPNThreshold = &Fir_gmacsecGetTxXPNThreshold;
//	dev->SwitchDevObj.MACSECObj.gmacsecSetRxDefaultSCI = &Fir_gmacsecSetRxDefaultSCI;
//	dev->SwitchDevObj.MACSECObj.gmacsecGetRxDefaultSCI = &Fir_gmacsecGetRxDefaultSCI;
//	dev->SwitchDevObj.MACSECObj.gmacsecSetTxSCIntCtrl = &Fir_gmacsecSetTxSCIntCtrlIntf;
//	dev->SwitchDevObj.MACSECObj.gmacsecGetTxSCIntCtrl = &Fir_gmacsecGetTxSCIntCtrlIntf;
//	dev->SwitchDevObj.MACSECObj.gmacsecGetNextTxSCIntStatus = &Fir_gmacsecGetNextTxSCIntStatusIntf;
    return MSD_OK;
}

static MSD_STATUS InitObj(MSD_QD_DEV *dev)
{
	dev->SwitchDevObj.ATUObj.gfdbAddMacEntry = NULL;
	dev->SwitchDevObj.ATUObj.gfdbGetAtuEntryNext = NULL;
	dev->SwitchDevObj.ATUObj.gfdbFlush = NULL;
	dev->SwitchDevObj.ATUObj.gfdbMove = NULL;
	dev->SwitchDevObj.ATUObj.gfdbMoveInDB = NULL;
	dev->SwitchDevObj.ATUObj.gfdbPortRemove = NULL;
	dev->SwitchDevObj.ATUObj.gfdbPortRemoveInDB = NULL;
	dev->SwitchDevObj.ATUObj.gfdbDelAtuEntry = NULL;
	dev->SwitchDevObj.ATUObj.gfdbGetViolation = NULL;
	dev->SwitchDevObj.ATUObj.gfdbFindAtuMacEntry = NULL;
	dev->SwitchDevObj.ATUObj.gfdbSetAgingTimeout = NULL;
	dev->SwitchDevObj.ATUObj.gfdbGetAgingTimeout = NULL;
	dev->SwitchDevObj.ATUObj.gfdbSetLearn2All = NULL;
	dev->SwitchDevObj.ATUObj.gfdbGetLearn2All = NULL;
	dev->SwitchDevObj.ATUObj.gfdbSetPortLearnLimit = NULL;
	dev->SwitchDevObj.ATUObj.gfdbGetPortLearnLimit = NULL;
	dev->SwitchDevObj.ATUObj.gfdbGetPortLearnCount = NULL;
	dev->SwitchDevObj.ATUObj.gfdbGetEntryCount = NULL;
	dev->SwitchDevObj.ATUObj.gfdbGetEntryCountPerFid = NULL;
	dev->SwitchDevObj.ATUObj.gfdbGetNonStaticEntryCount = NULL;
	dev->SwitchDevObj.ATUObj.gfdbGetNonStaticEntryCountPerFid = NULL;
	dev->SwitchDevObj.ATUObj.gfdbDump = NULL;

	dev->SwitchDevObj.ECIDObj.gecidAddEntry = NULL;
	dev->SwitchDevObj.ECIDObj.gecidFlushAll = NULL;
	dev->SwitchDevObj.ECIDObj.gecidFlushEntry = NULL;
	dev->SwitchDevObj.ECIDObj.gecidMove = NULL;
	dev->SwitchDevObj.ECIDObj.gecidGetEcidEntryNext = NULL;
	dev->SwitchDevObj.ECIDObj.gecidFindEcidEntry = NULL;
	dev->SwitchDevObj.ECIDObj.gecidGetEntryCount = NULL;
	dev->SwitchDevObj.ECIDObj.gecidDump = NULL;

	dev->SwitchDevObj.PORTCTRLObj.gprtSetCutThrouthEnable = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetCutThrouthEnable = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetIgnoreFcs = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetIgnoreFcs = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetAllowBadFcs = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetAllowBadFcs = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPauseLimitOut = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPauseLimitOut = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPauseLimitIn = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPauseLimitIn = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetQueueToPause = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetQueueToPause = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetRxPriorityFlowControlEnable = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetRxPriorityFlowControlEnable = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetRxPriorityFlowControlToQueue = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetRxPriorityFlowControlToQueue = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetTxPriorityFlowControlEnable = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetTxPriorityFlowControlEnable = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetOutQueueSize = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetMapDA = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetMapDA = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPortLocked = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPortLocked = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetHoldAt1 = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetHoldAt1 = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetInt0nAgeOut = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetInt0nAgeOut = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetRefreshLocked = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetRefreshLocked = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetIgnoreWrongData = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetIgnoreWrongData = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetARPMirror = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetARPMirror = NULL;

	dev->SwitchDevObj.POLICYObj.gpolicySetMgmtCtrl = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicyGetMgmtCtrl = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicySetCpuDest = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicyGetCpuDest = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicySetIngressMonitorDest = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicyGetIngressMonitorDest = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicySetRsvd2CpuEnables = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicyGetRsvd2CpuEnables = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicySetMGMTPri = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicyGetMGMTPri = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicySetRouteDest = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicyGetRouteDest = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicySetMirrorDest = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicyGetMirrorDest = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicySetIllegalIPAddr = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicyGetIllegalIPAddr = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicySetDipDaMismatch = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicyGetDipDaMismatch = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicySetZeroSIP = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicyGetZeroSIP = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicySetMcTtlExceeded = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicyGetMcTtlExceeded = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicySetUcTtlExceeded = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicyGetUcTtlExceeded = NULL;
	dev->SwitchDevObj.POLICYObj.gprtSetPortTtlThreshold = NULL;
	dev->SwitchDevObj.POLICYObj.gprtGetPortTtlThreshold = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicySetIpHdrErr = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicyGetIpHdrErr = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicySetTCPFinUrgPsh = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicyGetTCPFinUrgPsh = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicySetTCPFlagsSeqNum = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicyGetTCPFlagsSeqNum = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicySetTCPSYN = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicyGetTCPSYN = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicySetTCPHSize = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicyGetTCPHSize = NULL;
	dev->SwitchDevObj.POLICYObj.gprtSetPortTCPMinHdrSize = NULL;
	dev->SwitchDevObj.POLICYObj.gprtGetPortTCPMinHdrSize = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicySetICMPSize = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicyGetICMPSize = NULL;
	dev->SwitchDevObj.POLICYObj.gprtSetPortICMPMaxSize = NULL;
	dev->SwitchDevObj.POLICYObj.gprtGetPortICMPMaxSize = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicySetUDPPort = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicyGetUDPPort = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicySetTCPPort = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicyGetTCPPort = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicySetTCPSynFin = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicyGetTCPSynFin = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicySetTCPFrag = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicyGetTCPFrag = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicySetNearNonTPMR = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicyGetNearNonTPMR = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicySetETag = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicyGetETag = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicySetNonETag = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicyGetNonETag = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicyDump = NULL;

	dev->SwitchDevObj.VCTObj.gvctGetAdvCableDiag = NULL;
	dev->SwitchDevObj.VCTObj.gvctGetAdvExtendedStatus = NULL;

	dev->SwitchDevObj.STUObj.gstuAddEntry = NULL;
	dev->SwitchDevObj.STUObj.gstuGetEntryNext = NULL;
	dev->SwitchDevObj.STUObj.gstuFindSidEntry = NULL;
	dev->SwitchDevObj.STUObj.gstuGetEntryCount = NULL;
	dev->SwitchDevObj.STUObj.gstuDump = NULL;

	dev->SwitchDevObj.VTUObj.gvtuFindVidEntry = NULL;
	dev->SwitchDevObj.VTUObj.gvtuExistVidEntry = NULL;
	dev->SwitchDevObj.VTUObj.gvtuFlush = NULL;
	dev->SwitchDevObj.VTUObj.gvtuAddEntry = NULL;
	dev->SwitchDevObj.VTUObj.gvtuGetEntryNext = NULL;
	dev->SwitchDevObj.VTUObj.gvtuDelEntry = NULL;
	dev->SwitchDevObj.VTUObj.gvtuSetMode = NULL;
	dev->SwitchDevObj.VTUObj.gvtuGetMode = NULL;
	dev->SwitchDevObj.VTUObj.gvtuGetViolation = NULL;
	dev->SwitchDevObj.VTUObj.gvtuGetEntryCount = NULL;
	dev->SwitchDevObj.VTUObj.gvtuSetMemberTag = NULL;
	dev->SwitchDevObj.VTUObj.gvtuGetMemberTag = NULL;
	dev->SwitchDevObj.VTUObj.gvtuDump = NULL;

	dev->SwitchDevObj.IMPObj.impRun = NULL;
	dev->SwitchDevObj.IMPObj.impReset = NULL;
	dev->SwitchDevObj.IMPObj.impWriteEEPROM = NULL;
	dev->SwitchDevObj.IMPObj.impLoadToEEPROM = NULL;
	dev->SwitchDevObj.IMPObj.impLoadToRAM = NULL;
	dev->SwitchDevObj.IMPObj.impStop = NULL;

	dev->SwitchDevObj.EEPROMObj.geepromWrite = NULL;
	dev->SwitchDevObj.EEPROMObj.geepromRead = NULL;
	dev->SwitchDevObj.EEPROMObj.geepromSetMChipSel = NULL;
	dev->SwitchDevObj.EEPROMObj.geepromGetMChipSel = NULL;

	dev->SwitchDevObj.PHYCTRLObj.gphyReset = NULL;
    dev->SwitchDevObj.PHYCTRLObj.serdesReset = NULL;
	dev->SwitchDevObj.PHYCTRLObj.gphySetPortLoopback = NULL;
	dev->SwitchDevObj.PHYCTRLObj.gphySetPortSpeed = NULL;
	dev->SwitchDevObj.PHYCTRLObj.gphyPortPowerDown = NULL;
	dev->SwitchDevObj.PHYCTRLObj.gphySetPortDuplexMode = NULL;
	dev->SwitchDevObj.PHYCTRLObj.gphySetPortAutoMode = NULL;
	dev->SwitchDevObj.PHYCTRLObj.gphySetEEE = NULL;
	dev->SwitchDevObj.PHYCTRLObj.gphySetFlowControlEnable = NULL;
	dev->SwitchDevObj.PHYCTRLObj.gphySetIntEnable = NULL;
	dev->SwitchDevObj.PHYCTRLObj.gphyGetIntEnable = NULL;
	dev->SwitchDevObj.PHYCTRLObj.gphyGetIntStatus = NULL;

	dev->SwitchDevObj.PIRLObj.gpirlInitialize = NULL;
	dev->SwitchDevObj.PIRLObj.gpirlInitResource = NULL;
	dev->SwitchDevObj.PIRLObj.gpirlCustomSetup = NULL;
	dev->SwitchDevObj.PIRLObj.gpirlCustomSetup = NULL;
	dev->SwitchDevObj.PIRLObj.gpirlReadResource = NULL;
	dev->SwitchDevObj.PIRLObj.gpirlWriteResource = NULL;
	dev->SwitchDevObj.PIRLObj.gpirlGetResReg = NULL;
	dev->SwitchDevObj.PIRLObj.gpirlSetResReg = NULL;
	dev->SwitchDevObj.PIRLObj.gpirlAdvConfig = NULL;
	dev->SwitchDevObj.PIRLObj.gpirlAdvConfigDisable = NULL;

	dev->SwitchDevObj.PORTCTRLObj.gvlnSetPortVid = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gvlnGetPortVid = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gvlnSetPortVlanDot1qMode = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gvlnGetPortVlanDot1qMode = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDiscardTagged = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDiscardTagged = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDiscardUntagged = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDiscardUntagged = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetUnicastFloodBlock = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetUnicastFloodBlock = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetMulticastFloodBlock = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetMulticastFloodBlock = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDiscardBCastMode = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDiscardBCastMode = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetIGMPSnoopEnable = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetIGMPSnoopEnable = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetHeaderMode = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetHeaderMode = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDropOnLock = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDropOnLock = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetJumboMode = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetJumboMode = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetSpecifiedMTU = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetSpecifiedMTU = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetLearnEnable = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetLearnEnable = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetVTUPriorityOverride = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetVTUPriorityOverride = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetSAPriorityOverride = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetSAPriorityOverride = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDAPriorityOverride = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDAPriorityOverride = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetMessagePort = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetMessagePort = NULL;
	
	dev->SwitchDevObj.PORTCTRLObj.gprtSetEtherType = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetEtherType = NULL;

	dev->SwitchDevObj.PORTCTRLObj.gprtSetEtherTypeByType = NULL;//Add 2024.7.18  for Fir (88Q5152) 
	dev->SwitchDevObj.PORTCTRLObj.gprtGetEtherTypeByType = NULL;//Add 2024.7.18  for Fir (88Q5152) 

	dev->SwitchDevObj.PORTCTRLObj.gprtSetAllowVidZero = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetAllowVidZero = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetFlowCtrl = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetFlowCtrl = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetEgressMonitorSource = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetEgressMonitorSource = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetIngressMonitorSource = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetIngressMonitorSource = NULL;
	dev->SwitchDevObj.PORTCTRLObj.grcSetEgressRate = NULL;
	dev->SwitchDevObj.PORTCTRLObj.grcGetEgressRate = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDuplex = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDuplex = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDuplex = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetForceLink = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetForceLink = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetLinkState = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetForceSpeed = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetForceSpeed = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetSpeed = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetFrameMode = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetFrameMode = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPortECID = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPortECID = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetEgressMode = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetEgressMode = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetIgnoreETag = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetIgnoreETag = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetExtendedPortCtrlReg = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetExtendedPortCtrlReg = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetVlanPorts = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetVlanPorts = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetForceDefaultVid = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetForceDefaultVid = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPortState = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPortState = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPreemptEnable = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPreemptEnable = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPreemptSize = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPreemptSize = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPreemptQueue = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPreemptQueue = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtRegDump = NULL;

	dev->SwitchDevObj.TrunkObj.gprtSetTrunkPort = NULL;
	dev->SwitchDevObj.TrunkObj.gprtGetTrunkPort = NULL;
	dev->SwitchDevObj.TrunkObj.gsysSetTrunkMaskTable = NULL;
	dev->SwitchDevObj.TrunkObj.gsysGetTrunkMaskTable = NULL;
	dev->SwitchDevObj.TrunkObj.gsysSetHashTrunk = NULL;
	dev->SwitchDevObj.TrunkObj.gsysGetHashTrunk = NULL;
	dev->SwitchDevObj.TrunkObj.gsysSetTrunkRouting = NULL;
	dev->SwitchDevObj.TrunkObj.gsysGetTrunkRouting = NULL;

	dev->SwitchDevObj.PORTRMONObj.gstatsFlushAll = NULL;
	dev->SwitchDevObj.PORTRMONObj.gstatsFlushPort = NULL;
	dev->SwitchDevObj.PORTRMONObj.gstatsGetPortCounter = NULL;
	dev->SwitchDevObj.PORTRMONObj.gstatsGetPortAllCounters = NULL;
	dev->SwitchDevObj.PORTRMONObj.gstatsGetHistogramMode = NULL;
	dev->SwitchDevObj.PORTRMONObj.gstatsSetHistogramMode = NULL;
	dev->SwitchDevObj.PORTRMONObj.gmibDump = NULL;

	dev->SwitchDevObj.QUEUEObj.gprtSetQueueCtrl = NULL;
	dev->SwitchDevObj.QUEUEObj.gprtGetQueueCtrl = NULL;
	dev->SwitchDevObj.QUEUEObj.gprtSetPortSched = NULL;
	dev->SwitchDevObj.QUEUEObj.gprtGetPortSched = NULL;
	dev->SwitchDevObj.QUEUEObj.gsysSetQoSWeight = NULL;
	dev->SwitchDevObj.QUEUEObj.gsysGetQoSWeight = NULL;
	dev->SwitchDevObj.QUEUEObj.gprtSetQoSWeight = NULL;
	dev->SwitchDevObj.QUEUEObj.gprtGetQoSWeight = NULL;

	dev->SwitchDevObj.SYSCTRLObj.gsysSetFloodBC = NULL;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetFloodBC = NULL;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetMonitorMgmtCtrl = NULL;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetMonitorMgmtCtrl = NULL;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetCPUDest = NULL;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetCPUDest = NULL;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetIngressMonitorDest = NULL;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetIngressMonitorDest = NULL;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetEgressMonitorDest = NULL;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetEgressMonitorDest = NULL;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetRsvd2CpuEnables = NULL;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetRsvd2CpuEnables = NULL;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetMGMTPri = NULL;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetMGMTPri = NULL;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetSMISetup = NULL;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetSMISetup = NULL;
	dev->SwitchDevObj.SYSCTRLObj.gsysGlobal1RegDump = NULL;
	dev->SwitchDevObj.SYSCTRLObj.gsysGlobal2RegDump = NULL;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetRMUMode = NULL;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetRMUMode = NULL;
	dev->SwitchDevObj.SYSCTRLObj.gsysQbvWrite = NULL;
	dev->SwitchDevObj.SYSCTRLObj.gsysQbvRead = NULL;

	dev->SwitchDevObj.QOSMAPObj.gprtGetIEEEPrioMap = NULL;
	dev->SwitchDevObj.QOSMAPObj.gprtSetIEEEPrioMap = NULL;
	dev->SwitchDevObj.QOSMAPObj.gqosGetPrioMapRule = NULL;
	dev->SwitchDevObj.QOSMAPObj.gprtGetInitialPri = NULL;
	dev->SwitchDevObj.QOSMAPObj.gprtSetDefQpri = NULL;
	dev->SwitchDevObj.QOSMAPObj.gprtGetDefQpri = NULL;
	dev->SwitchDevObj.QOSMAPObj.gprtSetInitialPri = NULL;
	dev->SwitchDevObj.QOSMAPObj.gqosSetPrioMapRule = NULL;
	dev->SwitchDevObj.QOSMAPObj.gprtRewriteDscp = NULL;
	dev->SwitchDevObj.QOSMAPObj.gprtSetCosToQueue = NULL;
	dev->SwitchDevObj.QOSMAPObj.gprtGetCosToQueue = NULL;
	dev->SwitchDevObj.QOSMAPObj.gprtSetCosToDscp = NULL;
	dev->SwitchDevObj.QOSMAPObj.gprtGetCosToDscp = NULL;

	dev->SwitchDevObj.QAVObj.gqavGetPortQpriXHiLimit = NULL;
	dev->SwitchDevObj.QAVObj.gqavSetPortQpriXHiLimit = NULL;
	dev->SwitchDevObj.QAVObj.gqavGetPortQpriXRate = NULL;
	dev->SwitchDevObj.QAVObj.gqavSetPortQpriXRate = NULL;

	dev->SwitchDevObj.PTPObj.gptpSetIntEnable = NULL;
	dev->SwitchDevObj.PTPObj.gptpGetIntEnable = NULL;
	dev->SwitchDevObj.PTPObj.gptpSetProtPTPEn = NULL;
	dev->SwitchDevObj.PTPObj.gptpGetProtPTPEn = NULL;
	dev->SwitchDevObj.PTPObj.gptpSetPortConfig = NULL;
	dev->SwitchDevObj.PTPObj.gptpGetPortConfig = NULL;
	dev->SwitchDevObj.PTPObj.gptpGetTimeStamp = NULL;
	dev->SwitchDevObj.PTPObj.gptpGetTSValidSt = NULL;
	dev->SwitchDevObj.PTPObj.gptpResetTimeStamp = NULL;
	dev->SwitchDevObj.PTPObj.gptpSetGlobalConfig = NULL;
	dev->SwitchDevObj.PTPObj.gptpGetGlobalConfig = NULL;
	dev->SwitchDevObj.PTPObj.gptpGetIntStatus = NULL;
	dev->SwitchDevObj.PTPObj.gptpSetMeanPathDelay = NULL;
	dev->SwitchDevObj.PTPObj.gptpGetMeanPathDelay = NULL;
	dev->SwitchDevObj.PTPObj.gptpSetIgrPathDelayAsym = NULL;
	dev->SwitchDevObj.PTPObj.gptpGetIgrPathDelayAsym = NULL;
	dev->SwitchDevObj.PTPObj.gptpSetEgrPathDelayAsym = NULL;
	dev->SwitchDevObj.PTPObj.gptpGetEgrPathDelayAsym = NULL;
	dev->SwitchDevObj.PTPObj.gptpSetMode = NULL;
	dev->SwitchDevObj.PTPObj.gptpGetMode = NULL;
	dev->SwitchDevObj.PTPObj.gptpSetMasterSlave = NULL;
	dev->SwitchDevObj.PTPObj.gptpGetMasterSlave = NULL;
	dev->SwitchDevObj.PTPObj.gptpGetTODBusyBitStatus = NULL;
	dev->SwitchDevObj.PTPObj.gptpTODStoreCompensation = NULL;
	dev->SwitchDevObj.PTPObj.gptpTODStoreAll = NULL;
	dev->SwitchDevObj.PTPObj.gptpTODCaptureAll = NULL;

	dev->SwitchDevObj.PTPObj.gptpSetPulse = NULL;
	dev->SwitchDevObj.PTPObj.gptpGetPulse = NULL;
	dev->SwitchDevObj.PTPObj.gtaiGetPTPGlobalTime = NULL;
    dev->SwitchDevObj.PTPObj.gptpGetPortReg = NULL;
    dev->SwitchDevObj.PTPObj.gptpSetPortReg = NULL;
    dev->SwitchDevObj.PTPObj.gptpGetGlobalReg = NULL;
    dev->SwitchDevObj.PTPObj.gptpSetGlobalReg = NULL;
    dev->SwitchDevObj.PTPObj.gptpGetTaiReg = NULL;
    dev->SwitchDevObj.PTPObj.gptpSetTaiReg = NULL;
    dev->SwitchDevObj.PTPObj.gptpSet1588v2Config = NULL;
    dev->SwitchDevObj.PTPObj.gptpSet8021asConfig = NULL;
    dev->SwitchDevObj.PTPObj.gptpSet8021asRevConfig = NULL;
    dev->SwitchDevObj.PTPObj.gptpSetPreemptTsSel = NULL;
    dev->SwitchDevObj.PTPObj.gptpGetPreemptTsSel = NULL;
    dev->SwitchDevObj.PTPObj.gptpSetPreemptCtrl = NULL;
    dev->SwitchDevObj.PTPObj.gptpGetPreemptCtrl = NULL;
	dev->SwitchDevObj.PTPObj.gtaiSetTrigGenTimerSel = NULL;
	dev->SwitchDevObj.PTPObj.gtaiGetTrigGenTimerSel = NULL;

	dev->SwitchDevObj.TCAMObj.gtcamFlushAll = NULL;
	dev->SwitchDevObj.TCAMObj.gtcamFlushEntry = NULL;
	dev->SwitchDevObj.TCAMObj.gtcamLoadEntry = NULL;
	dev->SwitchDevObj.TCAMObj.gtcamReadTCAMData = NULL;
	dev->SwitchDevObj.TCAMObj.gtcamGetNextTCAMData = NULL;
	dev->SwitchDevObj.TCAMObj.gtcamFindEntry = NULL;
	dev->SwitchDevObj.TCAMObj.gtcamSetMode = NULL;
	dev->SwitchDevObj.TCAMObj.gtcamGetMode = NULL;
	dev->SwitchDevObj.TCAMObj.gtcamAddEntryPort = NULL;
	dev->SwitchDevObj.TCAMObj.gtcamRemoveEntryPort = NULL;
	dev->SwitchDevObj.TCAMObj.gtcamEgrFlushEntry = NULL;
	dev->SwitchDevObj.TCAMObj.gtcamEgrFlushEntryAllPorts = NULL;
	dev->SwitchDevObj.TCAMObj.gtcamEgrLoadEntry = NULL;
	dev->SwitchDevObj.TCAMObj.gtcamEgrGetNextTCAMData = NULL;
	dev->SwitchDevObj.TCAMObj.gtcamEgrReadTCAMData = NULL;
	dev->SwitchDevObj.TCAMObj.gtcamDump = NULL;
	dev->SwitchDevObj.TCAMObj.gtcamAdvConfig = NULL;

	dev->SwitchDevObj.TCAMRCObj.gtcamRCLoadEntry = NULL;
	dev->SwitchDevObj.TCAMRCObj.gtcamRCFlushEntry = NULL;
	dev->SwitchDevObj.TCAMRCObj.gtcamRCReadEntry = NULL;
	dev->SwitchDevObj.TCAMRCObj.gtcamRCGetNextEntry = NULL;
	dev->SwitchDevObj.TCAMRCObj.gtcamRCFindEntry = NULL;
	dev->SwitchDevObj.TCAMRCObj.gtcamRCEntryDump = NULL;

	dev->SwitchDevObj.TCAMRCCompObj.gtcamRCCompLoad = NULL;
	dev->SwitchDevObj.TCAMRCCompObj.gtcamRCCompFlushAll = NULL;
	dev->SwitchDevObj.TCAMRCCompObj.gtcamRCCompFlushPort = NULL;
	dev->SwitchDevObj.TCAMRCCompObj.gtcamRCCompRead = NULL;

	dev->SwitchDevObj.ARPObj.garpLoadMcEntry = NULL;
	dev->SwitchDevObj.ARPObj.garpLoadUcEntry = NULL;
	dev->SwitchDevObj.ARPObj.garpFlushEntry = NULL;
	dev->SwitchDevObj.ARPObj.garpFlushAll = NULL;
	dev->SwitchDevObj.ARPObj.garpReadEntry = NULL;
	dev->SwitchDevObj.ARPObj.garpGetNextEntry = NULL;

	dev->SwitchDevObj.RMUObj.grmuGetID = NULL;
	dev->SwitchDevObj.RMUObj.grmuAtuDump = NULL;
	dev->SwitchDevObj.RMUObj.grmuEcidDump = NULL;
	dev->SwitchDevObj.RMUObj.grmuMibDump = NULL;
	dev->SwitchDevObj.RMUObj.grmuMib2Dump = NULL;
	dev->SwitchDevObj.RMUObj.grmuTCAMDump = NULL;
	dev->SwitchDevObj.RMUObj.grmuTCAMRead = NULL;
	dev->SwitchDevObj.RMUObj.grmuTCAMWrite = NULL;
	dev->SwitchDevObj.RMUObj.grmuMultiRegAccess = NULL;
    dev->SwitchDevObj.RMUObj.grmuRegDump = NULL;

	dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIC45PhyReg = NULL;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIC45PhyReg = NULL;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIPhyReg = NULL;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIPhyReg = NULL;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIPhyXMDIOReg = NULL;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIPhyXMDIOReg = NULL;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIExtPhyReg = NULL;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIExtPhyReg = NULL;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIExtPhyXMDIOReg = NULL;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIExtPhyXMDIOReg = NULL;

	dev->SwitchDevObj.LEDObj.gledForceOn = NULL;
	dev->SwitchDevObj.LEDObj.gledForceOff = NULL;
	dev->SwitchDevObj.LEDObj.gledLinkAct = NULL;
	dev->SwitchDevObj.LEDObj.gledModeSet = NULL;
	dev->SwitchDevObj.LEDObj.gledModeGet = NULL;
	dev->SwitchDevObj.LEDObj.gledBlinkRateSet = NULL;
	dev->SwitchDevObj.LEDObj.gledBlinkRateGet = NULL;
	dev->SwitchDevObj.LEDObj.gledPulseStretchSet = NULL;
	dev->SwitchDevObj.LEDObj.gledPulseStretchGet = NULL;
	dev->SwitchDevObj.LEDObj.gledSpecialModeSet = NULL;
	dev->SwitchDevObj.LEDObj.gledSpecialModeGet = NULL;
	dev->SwitchDevObj.LEDObj.gledCtrlSet = NULL;
	dev->SwitchDevObj.LEDObj.gledCtrlGet = NULL;
	dev->SwitchDevObj.LEDObj.gledDump = NULL;

	dev->SwitchDevObj.INTObj.gdevIntEnableSet = NULL;
	dev->SwitchDevObj.INTObj.gdevIntEnableGet = NULL;
	dev->SwitchDevObj.INTObj.gdevIntStatusGet = NULL;
	dev->SwitchDevObj.INTObj.gactiveIntEnableSet = NULL;
	dev->SwitchDevObj.INTObj.gactiveIntEnableGet = NULL;
	dev->SwitchDevObj.INTObj.gactiveIntStatusGet = NULL;

    dev->SwitchDevObj.QCRObj.gqcrFlushAll = NULL;
    dev->SwitchDevObj.QCRObj.gqcrFlushOne = NULL;
    dev->SwitchDevObj.QCRObj.gqcrSetGrpId = NULL;
    dev->SwitchDevObj.QCRObj.gqcrGetGrpId = NULL;
    dev->SwitchDevObj.QCRObj.gqcrSetCbsLimit = NULL;
    dev->SwitchDevObj.QCRObj.gqcrGetCbsLimit = NULL;
    dev->SwitchDevObj.QCRObj.gqcrSetMaxResidenceTime = NULL;
    dev->SwitchDevObj.QCRObj.gqcrGetMaxResidenceTime = NULL;
    dev->SwitchDevObj.QCRObj.gqcrSetRateLimit = NULL;
    dev->SwitchDevObj.QCRObj.gqcrGetRateLimit = NULL;

    dev->SwitchDevObj.FRERObj.gfrerSeqGenFlushAll = NULL;
    dev->SwitchDevObj.FRERObj.gfrerSeqGenFlushEntry = NULL;
    dev->SwitchDevObj.FRERObj.gfrerSeqGenRead = NULL;
    dev->SwitchDevObj.FRERObj.gfrerSeqGenLoad = NULL;

    dev->SwitchDevObj.FRERObj.gfrerIndvRcvyFlushAll = NULL;
    dev->SwitchDevObj.FRERObj.gfrerIndvRcvyFlushEntry = NULL;
    dev->SwitchDevObj.FRERObj.gfrerIndvRcvyRead = NULL;
    dev->SwitchDevObj.FRERObj.gfrerIndvRcvyGetNext = NULL;
    dev->SwitchDevObj.FRERObj.gfrerIndvRcvyReadClear = NULL;

    dev->SwitchDevObj.FRERObj.gfrerSeqRcvyFlushAll = NULL;
    dev->SwitchDevObj.FRERObj.gfrerSeqRcvyFlushEntry = NULL;
    dev->SwitchDevObj.FRERObj.gfrerSeqRcvyRead = NULL;
    dev->SwitchDevObj.FRERObj.gfrerSeqRcvyLoad = NULL;
    dev->SwitchDevObj.FRERObj.gfrerSeqRcvyGetNext = NULL;
    dev->SwitchDevObj.FRERObj.gfrerSeqRcvyGetViolation = NULL;
    dev->SwitchDevObj.FRERObj.gfrerSeqRcvyReadClear = NULL;

    dev->SwitchDevObj.FRERObj.gfrerBank0SCtrFlushAll = NULL;
    dev->SwitchDevObj.FRERObj.gfrerBank0SCtrFlushEntry = NULL;
    dev->SwitchDevObj.FRERObj.gfrerBank0SCtrRead = NULL;
    dev->SwitchDevObj.FRERObj.gfrerBank0SCtrLoad = NULL;
    dev->SwitchDevObj.FRERObj.gfrerBank0SCtrGetNext = NULL;
    dev->SwitchDevObj.FRERObj.gfrerBank0SCtrReadClear = NULL;

    dev->SwitchDevObj.FRERObj.gfrerBank1SCtrFlushAll = NULL;
    dev->SwitchDevObj.FRERObj.gfrerBank1SCtrFlushEntry = NULL;
    dev->SwitchDevObj.FRERObj.gfrerBank1SCtrRead = NULL;
    dev->SwitchDevObj.FRERObj.gfrerBank1SCtrLoad = NULL;
    dev->SwitchDevObj.FRERObj.gfrerBank1SCtrGetNext = NULL;
    dev->SwitchDevObj.FRERObj.gfrerBank1SCtrReadClear = NULL;

	dev->SwitchDevObj.QCIObj.gqciStreamFiltFlushAll = NULL;
	dev->SwitchDevObj.QCIObj.gqciStreamFiltSet = NULL;
	dev->SwitchDevObj.QCIObj.gqciStreamFiltBlockStatSet = NULL;
	dev->SwitchDevObj.QCIObj.gqciStreamFiltBlockStatGet = NULL;
	dev->SwitchDevObj.QCIObj.gqciStreamFiltEnAts = NULL;
	dev->SwitchDevObj.QCIObj.gqciStreamFiltEnGate = NULL;
	dev->SwitchDevObj.QCIObj.gqciStreamFiltMatchCtrGet = NULL;
	dev->SwitchDevObj.QCIObj.gqciStreamFiltFilterCtrGet = NULL;
	dev->SwitchDevObj.QCIObj.gqciStreamGateFiltCtrGet = NULL;
	dev->SwitchDevObj.QCIObj.gqciFlowMeterFiltCtrGet = NULL;

	dev->SwitchDevObj.QCIObj.gqciStreamGateFlushAll = NULL;
	dev->SwitchDevObj.QCIObj.gqciStreamGateDefStateSet = NULL;
	dev->SwitchDevObj.QCIObj.gqciStreamGateScaleSet = NULL;
	dev->SwitchDevObj.QCIObj.gqciStreamGateTimeGenIdSet = NULL;
	dev->SwitchDevObj.QCIObj.gqciStreamGateEntryAdd = NULL;

	dev->SwitchDevObj.QCIObj.gqciStreamGateTimeGenFlush = NULL;
	dev->SwitchDevObj.QCIObj.gqciStreamGateTimeGenSet = NULL;

	dev->SwitchDevObj.QCIObj.gqciFlowMeterFlushAll = NULL;
	dev->SwitchDevObj.QCIObj.gqciFlowMeterCoupleFlagSet = NULL;
	dev->SwitchDevObj.QCIObj.gqciFlowMeterCoupleFlagGet = NULL;
	dev->SwitchDevObj.QCIObj.gqciFlowMeterDropOnYelSet = NULL;
	dev->SwitchDevObj.QCIObj.gqciFlowMeterDropOnYelGet = NULL;
	dev->SwitchDevObj.QCIObj.gqciFlowMeterRedBlockEnSet = NULL;
	dev->SwitchDevObj.QCIObj.gqciFlowMeterRedBlockEnGet = NULL;
	dev->SwitchDevObj.QCIObj.gqciFlowMeterRedBlkStatSet = NULL;
	dev->SwitchDevObj.QCIObj.gqciFlowMeterRedBlkStatGet = NULL;
	dev->SwitchDevObj.QCIObj.gqciFlowMeterAdvConfig = NULL;

	dev->SwitchDevObj.MACSECObj.gmacsecFlushAll = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecCreateRule = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecDeleteRule = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecGetRule = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecCreateTxSecY = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecDeleteTxSecY = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecGetTxSecY = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecCreateRxSecY = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecDeleteRxSecY = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecGetRxSecY = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecCreateRxSC = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecDeleteRxSC = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecGetRxSC = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecCreateRxSA = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecDeleteRxSA = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecGetRxSA = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecCreateTxSC = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecDeleteTxSC = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecGetTxSC = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecCreateTxSA = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecDeleteTxSA = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecGetTxSA = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecGetRXSCStates = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecGetRXSecYStates = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecGetTXSCStates = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecGetTXSecYStates = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecEnableEgressSet = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecEnableEgressGet = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecEnableIngressSet = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecEnableIngressGet = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecSetTxPNThreshold = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecGetTxPNThreshold = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecSetTxXPNThreshold = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecGetTxXPNThreshold = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecSetRxDefaultSCI = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecGetRxDefaultSCI = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecSetTxSCIntCtrl = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecGetTxSCIntCtrl = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecGetNextTxSCIntStatus = NULL;

	return MSD_OK;
}

MSD_STATUS InitDevObj(MSD_QD_DEV *dev)
{
	MSD_STATUS retVal;
	retVal = InitObj(dev);
	retVal = InitDevObj_Fir(dev);
	return retVal;
}

/*******************************************************************************
* msdReserveFreeQueueSize
*
* DESCRIPTION:
*       workaround for QC WatchDog trigger issue: reserve 16 free queue size, total 1024. Set threshold 
*       for the free pool FIFO to 0x10. This means the recycle pointer will not be reused immediately, 
*       16 pointers will be given to per-port module before this pointer can be used.
*
* INPUTS:
*        dev - pointer to the structure for device.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       MSD_OK, if input is valid. MSD_FAIL, otherwise.
*
*******************************************************************************/
static MSD_STATUS msdReserveFreeQueueSize(MSD_QD_DEV *qdev)
{
	MSD_U16 reg27, tempData;
	MSD_STATUS retVal;

	/*read back G1 offset 0x1b register value*/
	retVal = msdGetAnyReg(qdev->devNum, (MSD_U8)0x1B, (MSD_U8)0x1B, &reg27);
	if (retVal != 0)
	{
		MSD_DBG_ERROR(("msdGetAnyReg return failed, retval = %d.\n", retVal));
		return MSD_FAIL;
	}
	/*Change the hidden block to QC Hidden registers, Hidden Block: 0x2*/
	retVal = msdSetAnyReg(qdev->devNum, 0x1B, 0x1B, 0x2000);
	if (retVal != 0)
	{
		MSD_DBG_ERROR(("msdSetAnyReg return failed, retval = %d.\n", retVal));
		return MSD_FAIL;
	}
	/*wait QCHiddenCtrl register busy bit self clear*/
	do
	{
		retVal = msdGetAnyReg(qdev->devNum, 0x1B, 0x1B, &tempData);
		if (retVal != 0)
		{
			MSD_DBG_ERROR(("msdGetAnyReg return failed, retval = %d.\n", retVal));
			return MSD_FAIL;
		}
	} while ((tempData & 0x8000) == 0x8000);

	/*write data to QC Hidden Data, Device Offset 0x1B, Register Offset: 0x1A, reserve 16 free queue size*/
	retVal = msdSetAnyReg(qdev->devNum, 0x1B, 0x1A, 0x8400);
	if (retVal != 0)
	{
		MSD_DBG_ERROR(("msdSetAnyReg return failed, retval = %d.\n", retVal));
		return MSD_FAIL;
	}
	/*write cmd register, QC Hidden Control, Device Offset 0x1B, Register Offset: 0x19*/
	tempData = (0xE000 | (0x1F << 8) | 0xC);

	retVal = msdSetAnyReg(qdev->devNum, 0x1B, 0x19, tempData);
	if (retVal != 0)
	{
		MSD_DBG_ERROR(("msdSetAnyReg return failed, retval = %d.\n", retVal));
		return MSD_FAIL;
	}

	/*restore the Hidden block*/
	retVal = msdSetAnyReg(qdev->devNum, 0x1B, 0x1B, reg27);
	if (retVal != 0)
	{
		MSD_DBG_ERROR(("msdSetAnyReg return failed, retval = %d.\n", retVal));
		return MSD_FAIL;
	}

	return MSD_OK;
}
