/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/********************************************************************************
* Fir_msdBrgFdbIntf.c
*
* DESCRIPTION:
*       API definitions for Multiple Forwarding Databases 
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include "Fir_msdBrgFdb.h"
#include "Fir_msdApiInternal.h"

/*******************************************************************************
* gfdbAddMacEntry
*
* DESCRIPTION:
*       Creates the new entry in MAC address table.
*
* INPUTS:
*       macEntry    - mac address entry to insert to the ATU.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       DBNum in macEntry -
*            ATU MAC Address Database number. If multiple address 
*            databases are not being used, DBNum should be zero.
*            If multiple address databases are being used, this value
*            should be set to the desired address database number.
*
*******************************************************************************/
MSD_STATUS Fir_gfdbAddMacEntryIntf
(
    IN MSD_QD_DEV    *dev,
    IN MSD_ATU_ENTRY *macEntry
)
{
	FIR_MSD_ATU_ENTRY entry;

	if (NULL == macEntry)
	{
		MSD_DBG_ERROR(("Input param MSD_ATU_ENTRY in Fir_gfdbAddMacEntryIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}

    entry.DBNum = macEntry->fid;
    entry.portVec = macEntry->portVec;
    entry.entryState = macEntry->entryState;
    entry.exPrio.macFPri = macEntry->exPrio.macFPri;
    entry.exPrio.macQPri = macEntry->exPrio.macQPri;
    entry.LAG = macEntry->trunkMemberOrLAG;

    msdMemCpy(entry.macAddr.arEther, macEntry->macAddr.arEther, 6);
    
    return Fir_gfdbAddMacEntry(dev, &entry);
}


/*******************************************************************************
* gfdbGetAtuEntryNext
*
* DESCRIPTION:
*       Gets next lexicographic MAC address starting from the specified Mac Addr 
*		in a particular ATU database (DBNum or FID).
*
* INPUTS:
*       atuEntry - the Mac Address to start the search.
*
* OUTPUTS:
*       atuEntry - match Address translate unit entry.
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_NO_SUCH - no more entries.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       Search starts from atu.macAddr[xx:xx:xx:xx:xx:xx] specified by the
*       user.
*
*       DBNum in atuEntry -
*            ATU MAC Address Database number. If multiple address 
*            databases are not being used, DBNum should be zero.
*            If multiple address databases are being used, this value
*            should be set to the desired address database number.
*
*******************************************************************************/
MSD_STATUS Fir_gfdbGetAtuEntryNextIntf
(
IN MSD_QD_DEV    *dev,
INOUT MSD_ATU_ENTRY *atuEntry
)
{
	FIR_MSD_ATU_ENTRY entry;
	MSD_STATUS retVal;

	if (NULL == atuEntry)
	{
		MSD_DBG_ERROR(("Input param MSD_ATU_ENTRY in Fir_gfdbGetAtuEntryNextIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}

	entry.DBNum = atuEntry->fid;
	entry.portVec = atuEntry->portVec;
	entry.entryState = atuEntry->entryState;
	entry.exPrio.macFPri = atuEntry->exPrio.macFPri;
	entry.exPrio.macQPri = atuEntry->exPrio.macQPri;
	entry.LAG = atuEntry->trunkMemberOrLAG;

	msdMemCpy(entry.macAddr.arEther, atuEntry->macAddr.arEther, 6);

#ifdef COUNT_TIME
    clock_t start, end;
    double cpu_time_used;
    start = clock();
    
    retVal = Fir_gfdbGetAtuEntryNext(dev, &entry);

    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Fir_gfdbGetAtuEntryNextIntf  Fir_gfdbGetAtuEntryNextִ��ʱ��: %f ��\n", cpu_time_used);
#else
    retVal = Fir_gfdbGetAtuEntryNext(dev, &entry);
#endif
	
  
    if (MSD_OK != retVal)
	{
		return retVal;
	}

	msdMemSet((void*)atuEntry, 0, sizeof(MSD_ATU_ENTRY));
	atuEntry->fid = entry.DBNum;
	atuEntry->portVec = entry.portVec;
	atuEntry->entryState = entry.entryState;
	atuEntry->exPrio.macFPri = entry.exPrio.macFPri;
	atuEntry->exPrio.macQPri = entry.exPrio.macQPri;
	atuEntry->trunkMemberOrLAG = entry.LAG;
	msdMemCpy(atuEntry->macAddr.arEther, entry.macAddr.arEther, 6);

	return retVal;
}


/*******************************************************************************
* Fir_gfdbFlush
*
* DESCRIPTION:
*       This routine flush all or all non-static addresses from the MAC Address
*       Table.
*
* INPUTS:
*       flushCmd - the flush operation type.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL- on error
*
* COMMENTS:
*		none
*
*******************************************************************************/
MSD_STATUS Fir_gfdbFlushIntf
(
IN MSD_QD_DEV    *dev,
IN MSD_FLUSH_CMD flushCmd
)
{
	return Fir_gfdbFlush(dev, (FIR_MSD_FLUSH_CMD)flushCmd);
}

/*******************************************************************************
* Fir_gfdbFlushInDB
*
* DESCRIPTION:
*       This routine flush all or all non-static addresses from the particular
*       ATU Database (DBNum). If multiple address databases are being used, this
*       API can be used to flush entries in a particular DBNum database.
*
* INPUTS:
*       flushCmd  - the flush operation type.
*       DBNum     - ATU MAC Address Database Number.
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
*		none
*
*******************************************************************************/
MSD_STATUS Fir_gfdbFlushInDBIntf
(
IN MSD_QD_DEV    *dev,
IN MSD_FLUSH_CMD flushCmd,
IN MSD_U32 DBNum
)
{
	return Fir_gfdbFlushInDB(dev, (FIR_MSD_FLUSH_CMD)flushCmd, DBNum);
}

/*******************************************************************************
* Fir_gfdbMove
*
* DESCRIPTION:
*        This routine moves all or all non-static addresses from a port to another.
*
* INPUTS:
*        moveCmd  - the move operation type.
*        moveFrom - port where moving from
*        moveTo   - port where moving to
*
* OUTPUTS:
*        None
*
* RETURNS:
*        MSD_OK  - on success
*        MSD_FAIL- on error
*        MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*        none
*
*******************************************************************************/
MSD_STATUS Fir_gfdbMoveIntf
(
IN MSD_QD_DEV    *dev,
IN MSD_MOVE_CMD  moveCmd,
IN MSD_LPORT     moveFrom,
IN MSD_LPORT     moveTo
)
{
	return Fir_gfdbMove(dev, (FIR_MSD_MOVE_CMD)moveCmd, moveFrom, moveTo);
}

/*******************************************************************************
* Fir_gfdbMoveInDB
*
* DESCRIPTION:
*        This routine move all or all non-static addresses which are in the particular
*        ATU Database (DBNum) from a port to another.
*
* INPUTS:
*        moveCmd  - the move operation type.
*        DBNum    - ATU MAC Address Database Number.
*        moveFrom - port where moving from
*        moveTo   - port where moving to
*
* OUTPUTS:
*     None
*
* RETURNS:
*        MSD_OK   - on success
*        MSD_FAIL - on error
*        MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		 none
*
*******************************************************************************/
MSD_STATUS Fir_gfdbMoveInDBIntf
(
IN MSD_QD_DEV     *dev,
IN MSD_MOVE_CMD   moveCmd,
IN MSD_U32        DBNum,
IN MSD_LPORT      moveFrom,
IN MSD_LPORT      moveTo
)
{
	return Fir_gfdbMoveInDB(dev, (FIR_MSD_MOVE_CMD)moveCmd, DBNum, moveFrom, moveTo);
}

/*******************************************************************************
* Fir_gfdbPortRemove
*
* DESCRIPTION:
*        This routine remove all or all non-static addresses from a port.
*
* INPUTS:
*        moveCmd  - the move operation type.
*        portNum  - logical port number
*
* OUTPUTS:
*        None
*
* RETURNS:
*        MSD_OK  - on success
*        MSD_FAIL- on error
*        MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*        none
*
*******************************************************************************/
MSD_STATUS Fir_gfdbPortRemoveIntf
(
    IN  MSD_QD_DEV  *dev,
    IN  MSD_MOVE_CMD  moveCmd,
    IN  MSD_LPORT portNum
)
{
    FIR_MSD_MOVE_CMD pmoveCmd;

    switch (moveCmd)
    {
    case MSD_MOVE_ALL:
        pmoveCmd = FIR_MSD_MOVE_ALL;
        break;
    case MSD_MOVE_ALL_NONSTATIC:
        pmoveCmd = FIR_MSD_MOVE_ALL_NONSTATIC;
        break;
    default:
        return MSD_BAD_PARAM;
		break;
    }

    return Fir_gfdbMove(dev, pmoveCmd, portNum, (MSD_U32)0x1F);
}

/*******************************************************************************
* Fir_gfdbPortRemoveInDB
*
* DESCRIPTION:
*        This routine remove all or all non-static addresses from a port in the
*        specified ATU DBNum.
*
* INPUTS:
*        moveCmd  - the move operation type.
*        portNum  - logical port number
*        DBNum    - fid
*
* OUTPUTS:
*        None
*
* RETURNS:
*        MSD_OK  - on success
*        MSD_FAIL- on error
*        MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*        none
*
*******************************************************************************/
MSD_STATUS Fir_gfdbPortRemoveInDBIntf
(
    IN  MSD_QD_DEV  *dev,
    IN  MSD_MOVE_CMD  moveCmd,
    IN  MSD_U32  DBNum,
    IN  MSD_LPORT  portNum
)
{
    FIR_MSD_MOVE_CMD pmoveCmd;

    switch (moveCmd)
    {
    case MSD_MOVE_ALL:
        pmoveCmd = FIR_MSD_MOVE_ALL;
        break;
    case MSD_MOVE_ALL_NONSTATIC:
        pmoveCmd = FIR_MSD_MOVE_ALL_NONSTATIC;
        break;
    default:
        return MSD_BAD_PARAM;
		break;
    }

    return Fir_gfdbMoveInDB(dev, pmoveCmd, DBNum, portNum, (MSD_LPORT)0x1F);
}

/*******************************************************************************
* Fir_gfdbGetViolation
*
* DESCRIPTION:
*       Get ATU Violation data
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       atuIntStatus - interrupt cause, source portID, dbNum and mac address.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       none
*
*******************************************************************************/
MSD_STATUS Fir_gfdbGetViolationIntf
(
IN  MSD_QD_DEV         *dev,
OUT MSD_ATU_INT_STATUS *atuIntStatus
)
{
	FIR_MSD_ATU_INT_STATUS atuInt;
	MSD_STATUS       retVal;

	msdMemSet((void*)&atuInt, 0, sizeof(FIR_MSD_ATU_INT_STATUS));
	retVal = Fir_gfdbGetViolation(dev, &atuInt);
	if (MSD_OK != retVal)
	{
		return retVal;
	}

	if (NULL == atuIntStatus)
	{
		MSD_DBG_ERROR(("Input param MSD_ATU_INT_STATUS in Fir_gfdbGetViolationIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}
	msdMemSet((void*)atuIntStatus, 0, sizeof(MSD_ATU_INT_STATUS));
    atuIntStatus->atuIntCause.ageOutVio = ((atuInt.atuIntCause & FIR_MSD_ATU_AGE_OUT_VIOLATION) == 0U) ? MSD_FALSE : MSD_TRUE;
    atuIntStatus->atuIntCause.memberVio = ((atuInt.atuIntCause & FIR_MSD_ATU_MEMBER_VIOLATION) == 0U) ? MSD_FALSE : MSD_TRUE;
    atuIntStatus->atuIntCause.missVio = ((atuInt.atuIntCause & FIR_MSD_ATU_MISS_VIOLATION) == 0U) ? MSD_FALSE : MSD_TRUE;
    atuIntStatus->atuIntCause.fullVio = ((atuInt.atuIntCause & FIR_MSD_ATU_FULL_VIOLATION) == 0U) ? MSD_FALSE : MSD_TRUE;
	atuIntStatus->fid = atuInt.dbNum;
	atuIntStatus->spid = atuInt.spid;
	msdMemCpy(atuIntStatus->macAddr.arEther, atuInt.macAddr.arEther, 6);
	return retVal;
}

/*******************************************************************************
* Fir_gfdbFindAtuMacEntry
*
* DESCRIPTION:
*       Find FDB entry for specific MAC address from the ATU.
*
* INPUTS:
*       macAddr - the Mac address to search.
*       fid     - DBNum used to search
*
* OUTPUTS:
*       found    - MSD_TRUE, if the appropriate entry exists.
*       atuEntry - the entry parameters.
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_BAD_PARAM  - on bad parameter
*       MSD_NO_SUCH   - not find the correct entry  //add 2024.7.24
* COMMENTS:
*        DBNum in atuEntry -
*            ATU MAC Address Database number. If multiple address
*            databases are not being used, DBNum should be zero.
*            If multiple address databases are being used, this value
*            should be set to the desired address database number.
*
*******************************************************************************/
MSD_STATUS Fir_gfdbFindAtuMacEntryIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_ETHERADDR *macAddr,
IN  MSD_U32       fid,
OUT MSD_ATU_ENTRY *atuEntry,
OUT MSD_BOOL      *found
)
{
	FIR_MSD_ATU_ENTRY entry;
	MSD_STATUS       retVal;

    if (NULL == macAddr)
    {
        MSD_DBG_ERROR(("Input param MSD_ETHERADDR in Fir_gfdbFindAtuMacEntryIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

	if (NULL == atuEntry)
	{
		MSD_DBG_ERROR(("Input param MSD_ATU_ENTRY in Fir_gfdbFindAtuMacEntryIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}
    
    msdMemSet((void*)&entry, 0, sizeof(FIR_MSD_ATU_ENTRY));
    entry.DBNum = (MSD_U16)fid;
	msdMemCpy(entry.macAddr.arEther, macAddr, 6);

	retVal = Fir_gfdbFindAtuMacEntry(dev, &entry, found);
	if ((MSD_OK != retVal) || (MSD_FALSE == *found))
	{
		return retVal;
	}

	msdMemSet((void*)atuEntry, 0, sizeof(MSD_ATU_ENTRY));
	atuEntry->fid = entry.DBNum;
	atuEntry->portVec = entry.portVec;
	atuEntry->entryState = entry.entryState;
	atuEntry->exPrio.macFPri = entry.exPrio.macFPri;
	atuEntry->exPrio.macQPri = entry.exPrio.macQPri;
	atuEntry->trunkMemberOrLAG = entry.LAG;
	msdMemCpy(atuEntry->macAddr.arEther, entry.macAddr.arEther, 6);
	return retVal;
}


/*******************************************************************************
* gfdbDump
*
* DESCRIPTION:
*       Find all MAC address in the specified fid and print it out.
*
* INPUTS:
*       fid    - ATU MAC Address Database Number.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Fir_gfdbDump
(
    IN MSD_QD_DEV    *dev,
    IN MSD_U32       fid
)
{
    MSD_STATUS status;
    MSD_ATU_ENTRY entry;
    MSD_16 temp;

    msdMemSet(&entry, 0, sizeof(MSD_ATU_ENTRY));
    
    entry.macAddr.arEther[0] = (MSD_U8)0xff;
    entry.macAddr.arEther[1] = (MSD_U8)0xff;
    entry.macAddr.arEther[2] = (MSD_U8)0xff;
    entry.macAddr.arEther[3] = (MSD_U8)0xff;
    entry.macAddr.arEther[4] = (MSD_U8)0xff;
    entry.macAddr.arEther[5] = (MSD_U8)0xff;
    entry.fid = (MSD_U16)fid;

    /* Print out ATU entry field */
    MSG(("\n--------------------------------------------------------\n"));
    MSG(("FID    MAC          LAG   PORTVEC      STATE  FPRI  QPRI\n"));
    MSG(("--------------------------------------------------------\n"));

#ifdef COUNT_TIME
    clock_t start, end;
    double cpu_time_used;
#endif

    temp = 1;
    while (1 == temp)
    {
#ifdef COUNT_TIME
        start = clock();

        status = Fir_gfdbGetAtuEntryNextIntf(dev, &entry);
        
        end = clock();
        cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
        MSG(("Fir_gfdbDump Fir_gfdbGetAtuEntryNextIntfִ��ʱ��: %f ��\n", cpu_time_used));
#else 
        status = Fir_gfdbGetAtuEntryNextIntf(dev, &entry);
#endif	

        if (MSD_NO_SUCH == status)
		{
			break;
		}

        if (MSD_OK != status)
        {
            return status;
        }

        MSG(("0x%-5x%02x%02x%02x%02x%02x%02x  %-5x%x%x%x%x%x%x%x%x%x%x%x%x  %-7x%-6x%-4x\n", entry.fid, entry.macAddr.arEther[0], entry.macAddr.arEther[1], entry.macAddr.arEther[2],
                                                                    entry.macAddr.arEther[3], entry.macAddr.arEther[4], entry.macAddr.arEther[5], entry.trunkMemberOrLAG,
																	((entry.portVec & 0x800) ? 1 : 0), ((entry.portVec & 0x400) ? 1 : 0), ((entry.portVec & 0x200) ? 1 : 0), ((entry.portVec & 0x100) ? 1 : 0),
                                                                    ((entry.portVec & 0x80) ? 1 : 0), ((entry.portVec & 0x40) ? 1 : 0), ((entry.portVec & 0x20) ? 1 : 0),
                                                                    ((entry.portVec & 0x10) ? 1 : 0), ((entry.portVec & 0x8) ? 1 : 0), ((entry.portVec & 0x4) ? 1 : 0),
                                                                    ((entry.portVec & 0x2) ? 1 : 0), ((entry.portVec & 0x1) ? 1 : 0),
                                                                    entry.entryState, entry.exPrio.macFPri, entry.exPrio.macQPri));

        if (FIR_IS_BROADCAST_MAC(entry.macAddr))
        {
            break;
        }

    }

    MSG(("\n"));
    return MSD_OK;
}
