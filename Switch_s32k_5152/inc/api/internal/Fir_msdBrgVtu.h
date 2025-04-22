/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/*******************************************************************************
* Fir_msdBrgVtu.h
*
* DESCRIPTION:
*       API/Structure definitions for Marvell VTU functionality.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#ifndef Fir_msdBrgVtu_h
#define Fir_msdBrgVtu_h

#include "msdApiTypes.h"
#include "msdSysConfig.h"
#include "msdUtils.h"

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************/
/* Exported VTU Types			 			                                   */
/****************************************************************************/
/*
 *  typedef: struct FIR_MSD_VTU_EXT_INFO
 *
 *  Description: 
 *        Extanded VTU Entry information for Priority Override and Non Rate Limit.
 *        Frame Priority is used to as the tag's PRI bits if the frame egresses
 *        the switch tagged. The egresss queue the frame is switch into is not
 *        modified by the Frame Priority Override.
 *        Queue Priority is used to determine the egress queue the frame is
 *        switched into. If the frame egresses tagged, the priority in the frame 
 *        will not be modified by a Queue Priority Override.
 *        NonRateLimit for VID is used to indicate any frames associated with this 
 *        VID are to bypass ingress and egress rate limiting, if the ingress 
 *        port's VID NRateLimit is also enabled.
 *
 *  Fields:
 *       useVIDQPri - Use VID queue priority override. When this is MSD_TRUE and 
 *                     VIDQPriOverride of the ingress port of the frame is enabled,
 *                     vidQPri data is used to override the frame priority on
 *                     any frame associated with this VID.
 *       vidQPri    - VID queue priority data (0 ~ 7).
 *       useVIDFPri - Use VID frame priority override. When this is MSD_TRUE and 
 *                     VIDFPriOverride of the ingress port of the frame is enabled,
 *                     vidFPri data is used to override the frame priority on
 *                     any frame associated with this VID.
 *       vidFPri    - VID frame priority data (0 ~ 7).
 *       dontLearn  - Don's learn. When this bit is set to one and this VTU entry is 
 *					   accessed by a frame, then learning will not take place on this frame
 *       filterUC   - Filter Unicast frames.
 *       filterBC   - Filter Broadcast frames.
 *       filterMC   - Filter Multicast frames.
 *		 routeDis   - Non-Routable VID
 *	     mldSnoop   - IPv6 MLD Snoop
 *	     igmpSnoop  - IPv4 IGMP Snoop
 *		 vtuPage    - VTU Page
 *
 */
typedef struct
{
    MSD_BOOL           useVIDFPri;
    MSD_U8             vidFPri;
    MSD_BOOL           useVIDQPri;
    MSD_U8             vidQPri;
    MSD_BOOL           dontLearn;
    MSD_BOOL           filterUC;
    MSD_BOOL           filterBC;
    MSD_BOOL           filterMC;
	MSD_BOOL		   routeDis;
	MSD_BOOL	       mldSnoop;
	MSD_BOOL	       igmpSnoop;
    MSD_U8             vtuPage;
} FIR_MSD_VTU_EXT_INFO;

/*
 *  definition for MEMBER_TAG
 */
 typedef enum
 {
	 Fir_MEMBER_EGRESS_UNMODIFIED = 0,
	 Fir_MEMBER_EGRESS_UNTAGGED,
	 Fir_MEMBER_EGRESS_TAGGED,
	 Fir_NOT_A_MEMBER
 }FIR_MSD_MEMTAGP;

/*
 *  typedef: struct FIR_MSD_VTU_ENTRY
 *
 *  Description: VLAN tarnslaton unit Entry
 *        Each field in the structure is device specific, i.e., some fields may not
 *        be supported by the used switch device. In such case, those fields are
 *        ignored by the MSD driver. Please refer to the datasheet for the list of
 *        supported fields.
 *
 *  Fields:
 *      DBNum      - database number or FID (forwarding information database)
 *      vid        - VLAN ID 
 *      sid        - 802.1s Port State Database ID
 *		memberTagP - Membership and Egress Tagging
 *                   memberTagP[0] is for Port 0, MemberTagP[1] is for port 1, and so on 
 *      vidPolicy  - indicate that the frame with this VID uses VID Policy
 *                     (see Fir_gprtSetPolicy API).
 *      vidExInfo  - extanded information for VTU entry. If the device supports extanded 
 *                     information, vidPriorityOverride and vidPriority values are 
 *                     ignored.
 */
typedef struct
{
    MSD_U16        DBNum;
    MSD_U16        vid;
    MSD_U8         sid;
	FIR_MSD_MEMTAGP    memberTagP[MSD_MAX_SWITCH_PORTS];
    MSD_BOOL          vidPolicy;
    FIR_MSD_VTU_EXT_INFO    vidExInfo;
} FIR_MSD_VTU_ENTRY;

/*
 *  typedef: struct FIR_MSD_VTU_INT_STATUS
 *
 *  Description: VLAN tarnslaton unit interrupt status
 *
 *  Fields:
 *      intCause  - VTU Interrupt Cause
 *                    FIR_MSD_VTU_FULL_VIOLATION,MSD_MEMEBER_VIOLATION,or
 *                    MSD_MISS_VIOLATION
 *      SPID      - source port number
 *                     if intCause is FIR_MSD_VTU_FULL_VIOLATION, it means nothing
 *      vid       - VLAN ID 
 *                     if intCause is FIR_MSD_VTU_FULL_VIOLATION, it means nothing
 *					   vid[12] is the vtuPage, vid[11:0] is the original vid
 */
typedef struct
{
    MSD_U16   vtuIntCause;
    MSD_U8    spid;
    MSD_U16   vid;
} FIR_MSD_VTU_INT_STATUS;

/*
 *  typedef: enum FIR_MSD_VTU_MODE
 *
 *  Description: VLAN tarnslaton unit mode
 *
 *  Fields:
 *      VTU_MODE_0   - 4K vtu, page should be 0 unless overridden by TCAM 
 *      VTU_MODE_1,  - 8K vtu with vtu  page being  0 if a frame is a  unicast frame 
 *      VTU_MODE_2,  - use 8K vtu with vtu page being = to the source 
 *                     port锟斤拷s VTU page bit(port offset 0x05) 
 *      VTU_MODE_3,  - use 8K vtu with vtu page being a one if frame is not                          
 *	                   properly Provider tagged entering a Provider port 
 */
typedef enum
{
	Fir_VTU_MODE_0 = 0,
	Fir_VTU_MODE_1,
	Fir_VTU_MODE_2,
	Fir_VTU_MODE_3,
}FIR_MSD_VTU_MODE;


/*
 * Bit Vector Definition for VTU Violation
 */
#define FIR_MSD_VTU_MEMBER_VIOLATION            0x4U
#define FIR_MSD_VTU_MISS_VIOLATION              0x2U
#define FIR_MSD_VTU_FULL_VIOLATION       	 	0x1U       

/*
 *  typedef: struct MSD_STU_ENTRY
 *
 *  Description: 802.1s Port State Information Database (STU) Entry
 *
 *  Fields:
 *      sid       - STU ID 
 *      portState - Per VLAN Port States for each port.
 */

/****************************************************************************/
/* Exported VTU Functions		 			                                   */
/****************************************************************************/

/*******************************************************************************
* Fir_gvtuGetEntryNext
*
* DESCRIPTION:
*       Gets next valid VTU entry from the specified VID.
*
* INPUTS:
*       vtuEntry - the VID and VTUPage to start the search.
*
* OUTPUTS:
*       vtuEntry - match VTU  entry.
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_NO_SUCH - no more entries.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       Search starts from the VID and VTU Page (13th vid) in the vtuEntry specified by the user.
*
*******************************************************************************/
MSD_STATUS Fir_gvtuGetEntryNext
(
    IN  MSD_QD_DEV         *dev,
    INOUT FIR_MSD_VTU_ENTRY    *vtuEntry
);
MSD_STATUS Fir_gvtuGetEntryNextIntf
(
    IN  MSD_QD_DEV       *dev,
    INOUT MSD_VTU_ENTRY  *vtuEntry
);


/*******************************************************************************
* Fir_gvtuFindVidEntry
*
* DESCRIPTION:
*       Find VTU entry for a specific VID and VTU Page, it will return the entry, if found, 
*       along with its associated data 
*
* INPUTS:
*       vtuEntry - contains the VID and VTU Page to search for.
*
* OUTPUTS:
*       found    - MSD_TRUE, if the appropriate entry exists.
*       vtuEntry - the entry parameters.
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error or entry does not exist.
*       MSD_NO_SUCH - no more entries.
*
* COMMENTS:
*
*
*******************************************************************************/
MSD_STATUS Fir_gvtuFindVidEntry
(
    IN MSD_QD_DEV         *dev,
    INOUT FIR_MSD_VTU_ENTRY  *vtuEntry,
    OUT MSD_BOOL         *found
);
MSD_STATUS Fir_gvtuFindVidEntryIntf
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_U16    vlanId,
    OUT MSD_VTU_ENTRY *entry,
    OUT MSD_BOOL *found
);
MSD_STATUS Fir_gvtuExistVidEntryIntf
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_U16  vlanId,
    OUT MSD_BOOL *isExists
);
/*******************************************************************************
* Fir_gvtuFlush
*
* DESCRIPTION:
*       This routine removes all entries from VTU Table.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL- on error
*
* COMMENTS:
*		None
*
*******************************************************************************/
MSD_STATUS Fir_gvtuFlush
(
    IN MSD_QD_DEV *dev
);
MSD_STATUS Fir_gvtuFlushIntf
(
    IN  MSD_QD_DEV       *dev
);
/*******************************************************************************
* Fir_gvtuAddEntry
*
* DESCRIPTION:
*       Creates the new entry in VTU table based on user input.
*
* INPUTS:
*       vtuEntry - vtu entry to insert to the VTU.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL- on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Fir_gvtuAddEntry
(
    IN MSD_QD_DEV     *dev,
    IN FIR_MSD_VTU_ENTRY *vtuEntry
);
MSD_STATUS Fir_gvtuAddEntryIntf
(
    IN  MSD_QD_DEV   *dev,
    IN MSD_VTU_ENTRY *vtuEntry
);
/*******************************************************************************
* Fir_gvtuDelEntry
*
* DESCRIPTION:
*       Deletes VTU entry specified by user.
*
* INPUTS:
*       vtuEntry - the VTU entry to be deleted 
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Fir_gvtuDelEntry
(
    IN MSD_QD_DEV     *dev,
    IN FIR_MSD_VTU_ENTRY  *vtuEntry
);
MSD_STATUS Fir_gvtuDelEntryIntf
(
    IN  MSD_QD_DEV   *dev,
    IN  MSD_U16 vlanId
);
/*******************************************************************************
* Fir_gvtuSetMode
*
* DESCRIPTION:
*       Set VTU Mode
*
* INPUTS:
*       FIR_MSD_VTU_MODE - VTU Mode
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL- on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Fir_gvtuSetMode
(
    IN  MSD_QD_DEV      *dev,
    IN  FIR_MSD_VTU_MODE    mode
);
MSD_STATUS Fir_gvtuSetModeIntf
(
    IN  MSD_QD_DEV      *dev,
    IN  MSD_VTU_MODE    mode
);
/*******************************************************************************
* Fir_gvtuGetViolation
*
* DESCRIPTION:
*       Get VTU Mode
*
* INPUTS:
*       None
*
* OUTPUTS:
*       FIR_MSD_VTU_MODE - VTU Mode
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL- on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Fir_gvtuGetMode
(
    IN  MSD_QD_DEV     *dev,
    OUT FIR_MSD_VTU_MODE   *mode
);
MSD_STATUS Fir_gvtuGetModeIntf
(
    IN  MSD_QD_DEV     *dev,
    OUT MSD_VTU_MODE   *mode
);
/*******************************************************************************
* Fir_gvtuGetViolation
*
* DESCRIPTION:
*       Get VTU Violation data
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       vtuIntStatus - interrupt cause, source portID, and vid.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL- on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Fir_gvtuGetViolation
(
    IN  MSD_QD_DEV         *dev,
    OUT FIR_MSD_VTU_INT_STATUS *vtuIntStatus
);
MSD_STATUS Fir_gvtuGetViolationIntf
(
    IN  MSD_QD_DEV         *dev,
    OUT MSD_VTU_INT_STATUS *vtuIntStatus
);
/*******************************************************************************
* Fir_gvtuGetEntryCountIntf
*
* DESCRIPTION:
*       Get number of valid entries in the VTU table
*
* INPUTS:
*       devNum      - physical device number
*
* OUTPUTS:
*       numEntries  - number of valid entries
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*       none
*
*******************************************************************************/
MSD_STATUS Fir_gvtuGetEntryCountIntf
(
    IN  MSD_QD_DEV *dev,
    OUT MSD_U16    *count
);
/*******************************************************************************
* Fir_gvtuSetMemberTagIntf
*
* DESCRIPTION:
*       If the entry with this vlan id exist in the VTU table, update the memberTag
*       to Allow/Block vlan on the port. If it not exist, return MSD_NO_SUCH
*
* INPUTS:
*       devNum  - physical device number
*       vlanId  - vlan id
*       portNum - port number
*       memberTag - enum element of MSD_PORT_MEMBER_TAG
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NO_SUCH   - the entry with this valn id not exist
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*		None
*
*******************************************************************************/
MSD_STATUS Fir_gvtuSetMemberTagIntf
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_U16  vlanId,
    IN  MSD_LPORT  portNum,
    IN  MSD_PORT_MEMBER_TAG memberTag
);
/*******************************************************************************
* Fir_gvtuGetMemberTagIntf
*
* DESCRIPTION:
*       If the entry with this vlan id exist in the VTU table, get the memberTag
*       on the port. If it not exist, return MSD_NO_SUCH.
*
* INPUTS:
*       devNum  - physical device number
*       vlanId  - vlan id
*       portNum - port number
*
* OUTPUTS:
*       memberTag - enum element of MSD_VLAN_MEMBER_TAG
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NO_SUCH   - the entry with this valn id not exist
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*		None
*
*******************************************************************************/
MSD_STATUS Fir_gvtuGetMemberTagIntf
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_U16  vlanId,
    IN  MSD_LPORT  portNum,
    OUT MSD_PORT_MEMBER_TAG *memberTag
);
MSD_STATUS Fir_gvtuDump
(
IN MSD_QD_DEV    *dev
);
#ifdef __cplusplus
}
#endif

#endif /* __Fir_msdBrgVtu_h */
