/************************************************************************
 * NASA Docket No. GSC-18,915-1, and identified as “cFS Checksum
 * Application version 2.5.1”
 *
 * Copyright (c) 2021 United States Government as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ************************************************************************/

/**
 * @file
 *   The CFS Checksum (CS) Application's computing checksum functions
 */

/**************************************************************************
 **
 ** Include section
 **
 **************************************************************************/
#include "cfe.h"
#include "cs_app.h"
#include <string.h>
#include "cs_events.h"
#include "cs_compute.h"
#include "cs_utils.h"
/**************************************************************************
 **
 ** Functions
 **
 **************************************************************************/

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS function that computes the checksum for EEPROM, Memory, OS   */
/* and cFE core code segments                                      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CFE_Status_t CS_ComputeEepromMemory(CS_Res_EepromMemory_Table_Entry_t *ResultsEntry, uint32 *ComputedCSValue,
                                    bool *DoneWithEntry)
{
    uint32       OffsetIntoCurrEntry     = 0;
    cpuaddr      FirstAddrThisCycle      = 0;
    uint32       NumBytesThisCycle       = 0;
    int32        NumBytesRemainingCycles = 0;
    uint32       NewChecksumValue        = 0;
    CFE_Status_t Status                  = CFE_SUCCESS;
    *DoneWithEntry                       = false;

    /* By the time we get here, we know we have an enabled entry */

    OffsetIntoCurrEntry     = ResultsEntry->ByteOffset;
    FirstAddrThisCycle      = ResultsEntry->StartAddress + OffsetIntoCurrEntry;
    NumBytesRemainingCycles = ResultsEntry->NumBytesToChecksum - OffsetIntoCurrEntry;

    NumBytesThisCycle = ((CS_AppData.MaxBytesPerCycle < NumBytesRemainingCycles) ? CS_AppData.MaxBytesPerCycle
                                                                                 : NumBytesRemainingCycles);

    NewChecksumValue = CFE_ES_CalculateCRC((void *)(FirstAddrThisCycle), NumBytesThisCycle,
                                           ResultsEntry->TempChecksumValue, CS_DEFAULT_ALGORITHM);

    NumBytesRemainingCycles -= NumBytesThisCycle;

    if (NumBytesRemainingCycles <= 0)
    {
        /* We are finished CS'ing all of the parts for this Entry */
        *DoneWithEntry = true;

        if (ResultsEntry->ComputedYet == true)
        {
            /* This is NOT the first time through this Entry.
             We have already computed a CS value for this Entry */

            if (NewChecksumValue != ResultsEntry->ComparisonValue)
            {
                /* If the just-computed value differ from the saved value */
                Status = CS_ERROR;
            }
            else
            {
                /* The checksum passes the test. */
            }
        }
        else
        {
            /* This is the first time through this Entry */
            ResultsEntry->ComputedYet     = true;
            ResultsEntry->ComparisonValue = NewChecksumValue;
        }

        *ComputedCSValue                = NewChecksumValue;
        ResultsEntry->ByteOffset        = 0;
        ResultsEntry->TempChecksumValue = 0;
    }
    else
    {
        /* We not finished this Entry.  Will try to finish during next wakeup */
        ResultsEntry->ByteOffset += NumBytesThisCycle;
        ResultsEntry->TempChecksumValue = NewChecksumValue;
    }

    return Status;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS function that computes the checksum for Tables               */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CFE_Status_t CS_ComputeTables(CS_Res_Tables_Table_Entry_t *ResultsEntry, uint32 *ComputedCSValue, bool *DoneWithEntry)
{
    uint32       OffsetIntoCurrEntry     = 0;
    cpuaddr      FirstAddrThisCycle      = 0;
    uint32       NumBytesThisCycle       = 0;
    int32        NumBytesRemainingCycles = 0;
    uint32       NewChecksumValue        = 0;
    CFE_Status_t Status                  = CFE_SUCCESS;
    CFE_Status_t Result                  = CFE_SUCCESS;
    CFE_Status_t ResultShare      = 0;
    CFE_Status_t ResultGetInfo    = 0;
    CFE_Status_t ResultGetAddress = 0;

    /* variables to get the table address */
    CFE_TBL_Handle_t LocalTblHandle = CFE_TBL_BAD_TABLE_HANDLE;
    cpuaddr          LocalAddress   = 0;
    CFE_TBL_Info_t   TblInfo;

    /* By the time we get here, we know we have an enabled entry */

    /* set the done flag to false originally */
    *DoneWithEntry = false;

    /* Handshake with Table Services to get address and size of table */

    /* if we already have a table handle for this table, don't get a new one */
    if (ResultsEntry->TblHandle == CFE_TBL_BAD_TABLE_HANDLE)
    {
        ResultShare = CFE_TBL_Share(&LocalTblHandle, ResultsEntry->Name);
        Result      = ResultShare;

        if (Result == CFE_SUCCESS)
        {
            ResultsEntry->TblHandle = LocalTblHandle;
        }
    }
    else
    {
        LocalTblHandle = ResultsEntry->TblHandle;
    }

    if (Result == CFE_SUCCESS)
    {
        ResultGetInfo = CFE_TBL_GetInfo(&TblInfo, ResultsEntry->Name);

        /* We want to try to get the address even if the GetInfo fails. This
        provides the CFE_TBL_UNREGISTERED if the table has gone away */
        ResultGetAddress = CFE_TBL_GetAddress((void *)&LocalAddress, LocalTblHandle);
        Result           = ResultGetAddress;
    }

    /* if the table was never loaded, release the address to prevent the table from being
       locked by CS, which would prevent the owner app from updating it*/
    if (ResultGetAddress == CFE_TBL_ERR_NEVER_LOADED)
    {
        CFE_TBL_ReleaseAddress(LocalTblHandle);
    }

    /* The table has dissapeared since the last time CS looked.
       We are checking to see if the table came back */
    if (Result == CFE_TBL_ERR_UNREGISTERED)
    {
        /* unregister the old handle */
        CFE_TBL_Unregister(LocalTblHandle);

        /* reset the stored  data in the results table since the
           table went away */
        ResultsEntry->TblHandle = CFE_TBL_BAD_TABLE_HANDLE;
        CS_ResetTablesTblResultEntry(ResultsEntry);
        ResultsEntry->ComparisonValue    = 0;
        ResultsEntry->StartAddress       = 0;
        ResultsEntry->NumBytesToChecksum = 0;

        /* Maybe the table came back, try and reshare it */
        Result = CS_AttemptTableReshare(ResultsEntry, &LocalTblHandle, &TblInfo, &LocalAddress, &ResultGetInfo);
    }

    if (Result == CFE_SUCCESS || Result == CFE_TBL_INFO_UPDATED)
    {
        /* push in the get data from the table info */
        if (ResultGetInfo == CFE_SUCCESS)
        {
            ResultsEntry->NumBytesToChecksum = TblInfo.Size;
        }
        ResultsEntry->StartAddress = LocalAddress;

        /* if the table has been updated since the last time we
         looked at it, we need to start over again. We can also
         use the new value as a baseline checksum */
        if (Result == CFE_TBL_INFO_UPDATED)
        {
            CS_ResetTablesTblResultEntry(ResultsEntry);
        }

        OffsetIntoCurrEntry     = ResultsEntry->ByteOffset;
        FirstAddrThisCycle      = ResultsEntry->StartAddress + OffsetIntoCurrEntry;
        NumBytesRemainingCycles = ResultsEntry->NumBytesToChecksum - OffsetIntoCurrEntry;

        NumBytesThisCycle = ((CS_AppData.MaxBytesPerCycle < NumBytesRemainingCycles) ? CS_AppData.MaxBytesPerCycle
                                                                                     : NumBytesRemainingCycles);

        NewChecksumValue = CFE_ES_CalculateCRC((void *)(FirstAddrThisCycle), NumBytesThisCycle,
                                               ResultsEntry->TempChecksumValue, CS_DEFAULT_ALGORITHM);

        NumBytesRemainingCycles -= NumBytesThisCycle;

        /* Have we finished all of the parts for this Entry */
        if (NumBytesRemainingCycles <= 0)
        {
            /* Start over if an update occurred after we started the last part */
            CFE_TBL_ReleaseAddress(LocalTblHandle);
            Result = CFE_TBL_GetAddress((void *)&LocalAddress, LocalTblHandle);
            if (Result == CFE_TBL_INFO_UPDATED)
            {
                *ComputedCSValue                = 0;
                ResultsEntry->ComputedYet       = false;
                ResultsEntry->ComparisonValue   = 0;
                ResultsEntry->ByteOffset        = 0;
                ResultsEntry->TempChecksumValue = 0;
            }
            else
            {
                /* No last second updates, post the result for this table */
                *DoneWithEntry = true;

                if (ResultsEntry->ComputedYet == true)
                {
                    /* This is NOT the first time through this Entry.
                       We have already computed a CS value for this Entry */
                    if (NewChecksumValue != ResultsEntry->ComparisonValue)
                    {
                        /* If the just-computed value differ from the saved value */
                        Status = CS_ERROR;
                    }
                    else
                    {
                        /* The checksum passes the test. */
                    }
                }
                else
                {
                    /* This is the first time through this Entry */
                    ResultsEntry->ComputedYet     = true;
                    ResultsEntry->ComparisonValue = NewChecksumValue;
                }

                *ComputedCSValue                = NewChecksumValue;
                ResultsEntry->ByteOffset        = 0;
                ResultsEntry->TempChecksumValue = 0;
            }
        }
        else
        {
            /* We have  not finished this Entry.  Will try to finish during next wakeup */
            ResultsEntry->ByteOffset += NumBytesThisCycle;
            ResultsEntry->TempChecksumValue = NewChecksumValue;
            *ComputedCSValue                = NewChecksumValue;
        }

        /* We are done with the table for this cycle, so we need to release the address */

        Result = CFE_TBL_ReleaseAddress(LocalTblHandle);
        if (Result != CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(CS_COMPUTE_TABLES_RELEASE_ERR_EID, CFE_EVS_EventType_ERROR,
                              "CS Tables: Could not release addresss for table %s, returned: 0x%08X",
                              ResultsEntry->Name, (unsigned int)Result);
        }

    } /* end if tabled was success or updated */
    else
    {
        CFE_EVS_SendEvent(CS_COMPUTE_TABLES_ERR_EID, CFE_EVS_EventType_ERROR,
                          "CS Tables: Problem Getting table %s info Share: 0x%08X, GetInfo: 0x%08X, GetAddress: 0x%08X",
                          ResultsEntry->Name, (unsigned int)ResultShare, (unsigned int)ResultGetInfo,
                          (unsigned int)ResultGetAddress);

        Status = CS_ERR_NOT_FOUND;
    }

    return Status;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS function that computes the checksum for Apps                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CFE_Status_t CS_ComputeApp(CS_Res_App_Table_Entry_t *ResultsEntry, uint32 *ComputedCSValue, bool *DoneWithEntry)
{
    uint32       OffsetIntoCurrEntry     = 0;
    cpuaddr      FirstAddrThisCycle      = 0;
    uint32       NumBytesThisCycle       = 0;
    int32        NumBytesRemainingCycles = 0;
    uint32       NewChecksumValue        = 0;
    CFE_Status_t Status                  = CFE_SUCCESS;
    CFE_Status_t Result;
    CFE_Status_t ResultGetResourceID;
    CFE_Status_t ResultGetResourceInfo = CS_ERROR;
    bool         ResultAddressValid    = false;

    /* variables to get applications address */
    CFE_ResourceId_t ResourceID = CFE_RESOURCEID_UNDEFINED;
    CFE_ES_AppInfo_t AppInfo;

    /* By the time we get here, we know we have an enabled entry */

    /* set the done flag to false originally */
    *DoneWithEntry = false;

    ResultGetResourceID = CFE_ES_GetAppIDByName((CFE_ES_AppId_t *)&ResourceID, ResultsEntry->Name);
    if (ResultGetResourceID == CFE_ES_ERR_NAME_NOT_FOUND)
    {
        /* Also check for a matching library name */
        ResultGetResourceID = CFE_ES_GetLibIDByName((CFE_ES_LibId_t *)&ResourceID, ResultsEntry->Name);
    }
    Result = ResultGetResourceID;

    if (Result == CFE_SUCCESS)
    {
        /* We got a valid ResourceID, so get the Resource info */

        ResultGetResourceInfo = CFE_ES_GetModuleInfo(&AppInfo, ResourceID);
        Result                = ResultGetResourceInfo;
    }

    if (Result == CFE_SUCCESS)
    {
        /* We got a valid ResourceID and good App info, so check the for valid addresses */

        if (AppInfo.AddressesAreValid == false)
        {
            CFE_EVS_SendEvent(CS_COMPUTE_APP_PLATFORM_DBG_EID, CFE_EVS_EventType_DEBUG,
                              "CS cannot get a valid address for %s, due to the platform", ResultsEntry->Name);
            Result = CS_ERROR;
        }
        else
        {
            /* Push in the data from the module info */
            ResultsEntry->NumBytesToChecksum = AppInfo.CodeSize;
            ResultsEntry->StartAddress       = AppInfo.CodeAddress;
            Result                           = CFE_SUCCESS;
            ResultAddressValid               = true;
        }
    }

    if (Result == CFE_SUCCESS)
    {
        /* We got valid ResourceID, good info, and valid addresses, so run the checksum */

        OffsetIntoCurrEntry     = ResultsEntry->ByteOffset;
        FirstAddrThisCycle      = ResultsEntry->StartAddress + OffsetIntoCurrEntry;
        NumBytesRemainingCycles = ResultsEntry->NumBytesToChecksum - OffsetIntoCurrEntry;

        NumBytesThisCycle = ((CS_AppData.MaxBytesPerCycle < NumBytesRemainingCycles) ? CS_AppData.MaxBytesPerCycle
                                                                                     : NumBytesRemainingCycles);

        NewChecksumValue = CFE_ES_CalculateCRC((void *)(FirstAddrThisCycle), NumBytesThisCycle,
                                               ResultsEntry->TempChecksumValue, CS_DEFAULT_ALGORITHM);

        NumBytesRemainingCycles -= NumBytesThisCycle;

        if (NumBytesRemainingCycles <= 0)
        {
            /* We are finished CS'ing all of the parts for this Entry */
            *DoneWithEntry = true;

            if (ResultsEntry->ComputedYet == true)
            {
                /* This is NOT the first time through this Entry.
                 We have already computed a CS value for this Entry */
                if (NewChecksumValue != ResultsEntry->ComparisonValue)
                {
                    /* If the just-computed value differ from the saved value */
                    Status = CS_ERROR;
                }
                else
                {
                    /* The checksum passes the test. */
                }
            }
            else
            {
                /* This is the first time through this Entry */
                ResultsEntry->ComputedYet     = true;
                ResultsEntry->ComparisonValue = NewChecksumValue;
            }

            *ComputedCSValue                = NewChecksumValue;
            ResultsEntry->ByteOffset        = 0;
            ResultsEntry->TempChecksumValue = 0;
        }
        else
        {
            /* We have not finished this Entry.  Will try to finish during next wakeup */
            ResultsEntry->ByteOffset += NumBytesThisCycle;
            ResultsEntry->TempChecksumValue = NewChecksumValue;
            *ComputedCSValue                = NewChecksumValue;
        }
    } /* end if got module id ok */
    else
    {
        /* Something failed -- either invalid ResourceID, bad App info, or invalid addresses, so notify ground */
        CFE_EVS_SendEvent(
            CS_COMPUTE_APP_ERR_EID, CFE_EVS_EventType_ERROR,
            "CS Apps: Problems getting module %s info, GetResourceID: 0x%08X, GetModuleInfo: 0x%08X, AddressValid: %d",
            ResultsEntry->Name, (unsigned int)ResultGetResourceID, (unsigned int)ResultGetResourceInfo,
            (unsigned int)ResultAddressValid);

        Status = CS_ERR_NOT_FOUND;
    }

    return Status;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS child task for recomputing EEPROM and Memory entry baselines */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_RecomputeEepromMemoryChildTask(void)
{
    uint32                             NewChecksumValue = 0;
    char                               TableType[CS_TABLETYPE_NAME_SIZE];
    CS_Res_EepromMemory_Table_Entry_t *ResultsEntry         = NULL;
    uint16                             Table                = 0;
    uint16                             EntryID              = 0;
    uint16                             PreviousState        = CS_STATE_EMPTY;
    bool                               DoneWithEntry        = false;
    uint16                             PreviousDefState     = CS_STATE_EMPTY;
    bool                               DefEntryFound        = false;
    uint16                             DefEntryID           = 0;
    CS_Def_EepromMemory_Table_Entry_t *DefTblPtr            = NULL;
    uint16                             MaxDefEntries        = 0;
    CFE_TBL_Handle_t                   DefTblHandle         = CFE_TBL_BAD_TABLE_HANDLE;
    CS_Res_Tables_Table_Entry_t *      TablesTblResultEntry = NULL;

    strncpy(TableType, "Undef Tbl", CS_TABLETYPE_NAME_SIZE); /* Initialize table type string */

    Table        = CS_AppData.ChildTaskTable;
    EntryID      = CS_AppData.ChildTaskEntryID;
    ResultsEntry = CS_AppData.RecomputeEepromMemoryEntryPtr;

    /* we want to  make sure that the entry isn't being checksummed in the
     background at the same time we are recomputing */
    PreviousState       = ResultsEntry->State;
    ResultsEntry->State = CS_STATE_DISABLED;

    /* Set entry as if this is the first time we are computing the checksum,
       since we want the entry to take on the new value */

    ResultsEntry->ByteOffset        = 0;
    ResultsEntry->TempChecksumValue = 0;
    ResultsEntry->ComputedYet       = false;

    /* Update the definition table entry as well.  We need to determine which memory type is
       being updated as well as which entry in the table is being updated. */
    if ((Table != CS_OSCORE) && (Table != CS_CFECORE))
    {
        if (Table == CS_EEPROM_TABLE)
        {
            DefTblPtr            = CS_AppData.DefEepromTblPtr;
            MaxDefEntries        = CS_MAX_NUM_EEPROM_TABLE_ENTRIES;
            DefTblHandle         = CS_AppData.DefEepromTableHandle;
            TablesTblResultEntry = CS_AppData.EepResTablesTblPtr;
        }
        else
        {
            DefTblPtr            = CS_AppData.DefMemoryTblPtr;
            MaxDefEntries        = CS_MAX_NUM_MEMORY_TABLE_ENTRIES;
            DefTblHandle         = CS_AppData.DefMemoryTableHandle;
            TablesTblResultEntry = CS_AppData.MemResTablesTblPtr;
        }

        if (EntryID < MaxDefEntries)
        {
            /* This assumes that the definition table entries are in the same order as the
               results table entries, which should be a safe assumption. */
            if ((ResultsEntry->StartAddress == DefTblPtr[EntryID].StartAddress) &&
                (DefTblPtr[EntryID].State != CS_STATE_EMPTY))
            {
                DefEntryFound            = true;
                PreviousDefState         = DefTblPtr[EntryID].State;
                DefTblPtr[EntryID].State = CS_STATE_DISABLED;
                DefEntryID               = EntryID;
                CS_ResetTablesTblResultEntry(TablesTblResultEntry);
                CFE_TBL_Modified(DefTblHandle);
            }
        }
    }

    while (!DoneWithEntry)
    {
        CS_ComputeEepromMemory(ResultsEntry, &NewChecksumValue, &DoneWithEntry);

        OS_TaskDelay(CS_CHILD_TASK_DELAY);
    }

    /* The new checksum value is stored in the table by the above functions */

    /* reset the entry's variables for a newly computed value */
    ResultsEntry->TempChecksumValue = 0;
    ResultsEntry->ByteOffset        = 0;
    ResultsEntry->ComputedYet       = true;

    /* restore the entry's previous state */
    ResultsEntry->State = PreviousState;

    /* Restore the definition table if we found one earlier */
    if (DefEntryFound)
    {
        DefTblPtr[DefEntryID].State = PreviousDefState;
        CS_ResetTablesTblResultEntry(TablesTblResultEntry);
        CFE_TBL_Modified(DefTblHandle);
    }

    /* send event message */

    if (Table == CS_EEPROM_TABLE)
    {
        snprintf(TableType, CS_TABLETYPE_NAME_SIZE, "%s", "EEPROM");
    }
    if (Table == CS_MEMORY_TABLE)
    {
        snprintf(TableType, CS_TABLETYPE_NAME_SIZE, "%s", "Memory");
    }
    if (Table == CS_CFECORE)
    {
        snprintf(TableType, CS_TABLETYPE_NAME_SIZE, "%s", "cFE Core");
        CS_AppData.HkPacket.Payload.CfeCoreBaseline = NewChecksumValue;
    }
    if (Table == CS_OSCORE)
    {
        snprintf(TableType, CS_TABLETYPE_NAME_SIZE, "%s", "OS");
        CS_AppData.HkPacket.Payload.OSBaseline = NewChecksumValue;
    }

    CFE_EVS_SendEvent(CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID, CFE_EVS_EventType_INFORMATION,
                      "%s entry %d recompute finished. New baseline is 0X%08X", TableType, EntryID,
                      (unsigned int)NewChecksumValue);

    CS_AppData.HkPacket.Payload.RecomputeInProgress = false;
    CFE_ES_ExitChildTask();
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS child task for recomputing baselines for Apps                */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_RecomputeAppChildTask(void)
{
    uint32                    NewChecksumValue = 0;
    CS_Res_App_Table_Entry_t *ResultsEntry     = NULL;
    uint16                    PreviousState    = CS_STATE_EMPTY;
    bool                      DoneWithEntry    = false;
    CFE_Status_t              Status           = CS_ERROR;
    uint16                    PreviousDefState = CS_STATE_EMPTY;
    bool                      DefEntryFound    = false;
    uint16                    DefEntryID       = 0;
    CS_Def_App_Table_Entry_t *DefTblPtr        = NULL;
    uint16                    MaxDefEntries    = 0;
    CFE_TBL_Handle_t          DefTblHandle     = CFE_TBL_BAD_TABLE_HANDLE;

    /* Get the variables to use from the global data */
    ResultsEntry = CS_AppData.RecomputeAppEntryPtr;

    /* we want to  make sure that the entry isn't being checksummed in the
     background at the same time we are recomputing */

    PreviousState       = ResultsEntry->State;
    ResultsEntry->State = CS_STATE_DISABLED;

    /* Set entry as if this is the first time we are computing the checksum,
       since we want the entry to take on the new value */

    ResultsEntry->ByteOffset        = 0;
    ResultsEntry->TempChecksumValue = 0;
    ResultsEntry->ComputedYet       = false;

    /* Update the definition table entry as well.  We need to determine which memory type is
       being updated as well as which entry in the table is being updated. */
    DefTblPtr     = CS_AppData.DefAppTblPtr;
    MaxDefEntries = CS_MAX_NUM_APP_TABLE_ENTRIES;
    DefTblHandle  = CS_AppData.DefAppTableHandle;

    DefEntryID = 0;

    while ((!DefEntryFound) && (DefEntryID < MaxDefEntries))
    {
        if ((strncmp(ResultsEntry->Name, DefTblPtr[DefEntryID].Name, OS_MAX_API_NAME) == 0) &&
            (DefTblPtr[DefEntryID].State != CS_STATE_EMPTY))
        {
            DefEntryFound               = true;
            PreviousDefState            = DefTblPtr[DefEntryID].State;
            DefTblPtr[DefEntryID].State = CS_STATE_DISABLED;
            CS_ResetTablesTblResultEntry(CS_AppData.AppResTablesTblPtr);
            CFE_TBL_Modified(DefTblHandle);
        }
        else
        {
            DefEntryID++;
        }
    }

    while (!DoneWithEntry)
    {
        Status = CS_ComputeApp(ResultsEntry, &NewChecksumValue, &DoneWithEntry);

        if (Status == CS_ERR_NOT_FOUND)
        {
            break;
        }

        OS_TaskDelay(CS_CHILD_TASK_DELAY);
    }
    /* The new checksum value is stored in the table by the above functions */

    /* restore the entry's state */
    ResultsEntry->State = PreviousState;

    /* Restore the definition table if we found one earlier */
    if (DefEntryFound)
    {
        DefTblPtr[DefEntryID].State = PreviousDefState;
        CS_ResetTablesTblResultEntry(CS_AppData.AppResTablesTblPtr);
        CFE_TBL_Modified(DefTblHandle);
    }

    if (Status == CS_ERR_NOT_FOUND)
    {
        CFE_EVS_SendEvent(CS_RECOMPUTE_ERROR_APP_ERR_EID, CFE_EVS_EventType_ERROR,
                          "App %s recompute failed. Could not get address", ResultsEntry->Name);
    }
    else
    {
        /* reset the entry's variables for a newly computed value */
        ResultsEntry->TempChecksumValue = 0;
        ResultsEntry->ByteOffset        = 0;
        ResultsEntry->ComputedYet       = true;

        /* send event message */
        CFE_EVS_SendEvent(CS_RECOMPUTE_FINISH_APP_INF_EID, CFE_EVS_EventType_INFORMATION,
                          "App %s recompute finished. New baseline is 0x%08X", ResultsEntry->Name,
                          (unsigned int)NewChecksumValue);
    }

    CS_AppData.HkPacket.Payload.RecomputeInProgress = false;
    CFE_ES_ExitChildTask();
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS child task for recomputing baselines for Tables              */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_RecomputeTablesChildTask(void)
{
    uint32                       NewChecksumValue = 0;
    CS_Res_Tables_Table_Entry_t *ResultsEntry     = NULL;
    uint16                       PreviousState    = CS_STATE_EMPTY;
    bool                         DoneWithEntry    = false;
    CFE_Status_t                 Status           = CS_ERROR;
    uint16                       PreviousDefState = CS_STATE_EMPTY;
    bool                         DefEntryFound    = false;
    uint16                       DefEntryID       = 0;
    CS_Def_Tables_Table_Entry_t *DefTblPtr        = NULL;
    uint16                       MaxDefEntries    = 0;
    CFE_TBL_Handle_t             DefTblHandle     = CFE_TBL_BAD_TABLE_HANDLE;

    /* Get the variables to use from the global data */
    ResultsEntry = CS_AppData.RecomputeTablesEntryPtr;

    /* we want to  make sure that the entry isn't being checksummed in the
     background at the same time we are recomputing */

    PreviousState       = ResultsEntry->State;
    ResultsEntry->State = CS_STATE_DISABLED;

    /* Set entry as if this is the first time we are computing the checksum,
     since we want the entry to take on the new value */

    ResultsEntry->ByteOffset        = 0;
    ResultsEntry->TempChecksumValue = 0;
    ResultsEntry->ComputedYet       = false;

    /* Update the definition table entry as well.  We need to determine which memory type is
     being updated as well as which entry in the table is being updated. */
    DefTblPtr     = CS_AppData.DefTablesTblPtr;
    MaxDefEntries = CS_MAX_NUM_TABLES_TABLE_ENTRIES;
    DefTblHandle  = CS_AppData.DefTablesTableHandle;

    DefEntryID = 0;

    while ((!DefEntryFound) && (DefEntryID < MaxDefEntries))
    {
        if ((strncmp(ResultsEntry->Name, DefTblPtr[DefEntryID].Name, CFE_TBL_MAX_FULL_NAME_LEN) == 0) &&
            (DefTblPtr[DefEntryID].State != CS_STATE_EMPTY))
        {
            DefEntryFound               = true;
            PreviousDefState            = DefTblPtr[DefEntryID].State;
            DefTblPtr[DefEntryID].State = CS_STATE_DISABLED;
            CS_ResetTablesTblResultEntry(CS_AppData.TblResTablesTblPtr);
            CFE_TBL_Modified(DefTblHandle);
        }
        else
        {
            DefEntryID++;
        }
    }

    while (!DoneWithEntry)
    {
        Status = CS_ComputeTables(ResultsEntry, &NewChecksumValue, &DoneWithEntry);

        if (Status == CS_ERR_NOT_FOUND)
        {
            break;
        }

        OS_TaskDelay(CS_CHILD_TASK_DELAY);
    }

    /* The new checksum value is stored in the table by the above functions */
    if (Status == CS_ERR_NOT_FOUND)
    {
        CFE_EVS_SendEvent(CS_RECOMPUTE_ERROR_TABLES_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Table %s recompute failed. Could not get address", ResultsEntry->Name);
    }
    else
    {
        /* reset the entry's variables for a newly computed value */
        ResultsEntry->TempChecksumValue = 0;
        ResultsEntry->ByteOffset        = 0;
        ResultsEntry->ComputedYet       = true;

        /* send event message */
        CFE_EVS_SendEvent(CS_RECOMPUTE_FINISH_TABLES_INF_EID, CFE_EVS_EventType_INFORMATION,
                          "Table %s recompute finished. New baseline is 0x%08X", ResultsEntry->Name,
                          (unsigned int)NewChecksumValue);
    }

    /* restore the entry's state */
    ResultsEntry->State = PreviousState;

    /* Restore the definition table if we found one earlier */
    if (DefEntryFound)
    {
        DefTblPtr[DefEntryID].State = PreviousDefState;
        CS_ResetTablesTblResultEntry(CS_AppData.TblResTablesTblPtr);
        CFE_TBL_Modified(DefTblHandle);
    }

    CS_AppData.HkPacket.Payload.RecomputeInProgress = false;
    CFE_ES_ExitChildTask();
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS child task for getting the checksum on an area of memory      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_OneShotChildTask(void)
{
    uint32  NewChecksumValue;
    uint32  NumBytesRemainingCycles;
    uint32  NumBytesThisCycle;
    cpuaddr FirstAddrThisCycle;
    uint32  MaxBytesPerCycle;

    NewChecksumValue        = 0;
    NumBytesRemainingCycles = CS_AppData.HkPacket.Payload.LastOneShotSize;
    FirstAddrThisCycle      = CS_AppData.HkPacket.Payload.LastOneShotAddress;
    MaxBytesPerCycle        = CS_AppData.HkPacket.Payload.LastOneShotMaxBytesPerCycle;

    while (NumBytesRemainingCycles > 0)
    {
        NumBytesThisCycle = ((MaxBytesPerCycle < NumBytesRemainingCycles) ? MaxBytesPerCycle : NumBytesRemainingCycles);

        NewChecksumValue = CFE_ES_CalculateCRC((void *)(FirstAddrThisCycle), NumBytesThisCycle, NewChecksumValue,
                                               CS_DEFAULT_ALGORITHM);

        /* Update the remainders for the next cycle */
        FirstAddrThisCycle += NumBytesThisCycle;
        NumBytesRemainingCycles -= NumBytesThisCycle;

        OS_TaskDelay(CS_CHILD_TASK_DELAY);
    }

    /*Checksum Calculation is done! */

    /* put the new checksum value in the baseline */
    CS_AppData.HkPacket.Payload.LastOneShotChecksum = NewChecksumValue;

    /* send event message */
    CFE_EVS_SendEvent(CS_ONESHOT_FINISHED_INF_EID, CFE_EVS_EventType_INFORMATION,
                      "OneShot checksum on Address: 0x%08X, size %d completed. Checksum =  0x%08X",
                      (unsigned int)(CS_AppData.HkPacket.Payload.LastOneShotAddress),
                      (unsigned int)(CS_AppData.HkPacket.Payload.LastOneShotSize),
                      (unsigned int)(CS_AppData.HkPacket.Payload.LastOneShotChecksum));

    CS_AppData.HkPacket.Payload.OneShotInProgress = false;
    CS_AppData.ChildTaskID                = CFE_ES_TASKID_UNDEFINED;

    CFE_ES_ExitChildTask();
}
