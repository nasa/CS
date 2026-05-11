/************************************************************************
 * NASA Docket No. GSC-19,200-1, and identified as "cFS Draco"
 *
 * Copyright (c) 2023 United States Government as represented by the
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
#include "cs_eventids.h"
#include "cs_compute.h"
#include "cs_utils.h"

/*
 * Local helper structure to keep state of checksum calculation
 * This allows the checksum calculation to be consolidated into a
 * common helper function for memory, apps, and tables.
 */
typedef struct CS_LocalChecksumState
{
    const uint8 *BufferAddr;
    size_t       TotalSize;
    size_t       CurrOffset;
    uint32       ResultBuffer;
} CS_LocalChecksumState_t;

/**************************************************************************
 **
 ** Functions
 **
 **************************************************************************/

/*----------------------------------------------------------------
 *
 * Local helper function
 *
 *-----------------------------------------------------------------*/
static inline bool CS_IsChecksumComplete(const CS_LocalChecksumState_t *State)
{
    return (State->CurrOffset >= State->TotalSize);
}

/*----------------------------------------------------------------
 *
 * Local helper function
 *
 *-----------------------------------------------------------------*/
void CS_DoChecksumChunk(CS_LocalChecksumState_t *State)
{
    size_t NumBytesThisCycle;
    uint32 NewChecksumValue;

    if (State->CurrOffset >= State->TotalSize)
    {
        /* Start again at beginning */
        State->CurrOffset   = 0;
        State->ResultBuffer = 0;
    }

    NumBytesThisCycle = State->TotalSize - State->CurrOffset;

    /* Limit to the configured max */
    if (NumBytesThisCycle > CS_AppData.MaxBytesPerCycle)
    {
        NumBytesThisCycle = CS_AppData.MaxBytesPerCycle;
    }

    NewChecksumValue = CFE_ES_CalculateCRC(State->BufferAddr + State->CurrOffset,
                                           NumBytesThisCycle,
                                           State->ResultBuffer,
                                           CS_DEFAULT_ALGORITHM);

    /* Export all data */
    State->CurrOffset   = State->CurrOffset + NumBytesThisCycle;
    State->ResultBuffer = NewChecksumValue;
}

/*----------------------------------------------------------------
 *
 * Local helper function
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CS_CheckOrUpdateReference(CS_LocalChecksumState_t *State, uint16 *RefValid, uint32 *CompareBuffer)
{
    CFE_Status_t Status;

    Status = CFE_SUCCESS;

    if (CS_IsChecksumComplete(State))
    {
        if (*RefValid)
        {
            /* Comparison value was computed before, so check against it */
            if (*CompareBuffer != State->ResultBuffer)
            {
                /* Not Matching is an error */
                Status = CS_ERROR;
            }
        }
        else
        {
            /* First time, so update the reference value for future use */
            *CompareBuffer = State->ResultBuffer;
            *RefValid      = true;
        }
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Local helper function
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CS_GetTableAddr(CS_LocalChecksumState_t *State, CFE_TBL_Handle_t LocalHandle)
{
    CFE_Status_t Status;
    void        *Addr;

    Status = CFE_TBL_GetAddress(&Addr, LocalHandle);

    if (Status >= CFE_SUCCESS)
    {
        State->BufferAddr = Addr;
    }
    else
    {
        State->BufferAddr = NULL;
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Local helper function
 *
 *-----------------------------------------------------------------*/
void CS_ReleaseTableAddr(CFE_TBL_HandleId_t LocalId, const char *Name)
{
    CFE_Status_t     Result;
    CFE_TBL_Handle_t LocalHandle;

    LocalHandle = CFE_TBL_HandleFromID(LocalId);

    Result = CFE_TBL_ReleaseAddress(LocalHandle);
    if (Result != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(CS_COMPUTE_TABLES_RELEASE_ERR_EID,
                          CFE_EVS_EventType_ERROR,
                          "CS Tables: Could not release addresss for table %s, returned: 0x%08X",
                          Name,
                          (unsigned int)Result);
    }
}

/*----------------------------------------------------------------
 *
 * Local helper function
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CS_RefreshTableSize(CS_LocalChecksumState_t *State, const char *Name)
{
    CFE_TBL_Info_t TblInfo;
    CFE_Status_t   Result;

    /* First do GetInfo, this looks up the table by name */
    Result = CFE_TBL_GetInfo(&TblInfo, Name);
    if (Result >= CFE_SUCCESS)
    {
        State->TotalSize = TblInfo.Size;
        Result           = CFE_SUCCESS;
    }
    else
    {
        State->TotalSize = 0;
        Result           = CS_ERR_NOT_FOUND;
    }

    return Result;
}

/*----------------------------------------------------------------
 *
 * Local helper function
 *
 *-----------------------------------------------------------------*/
CFE_Status_t
CS_RefreshTableHandleAndAddress(CS_LocalChecksumState_t *State, CFE_TBL_Handle_t *LocalHandle, const char *Name)
{
    CFE_Status_t Result;

    /* This means our local handle went stale, unregister it before getting a new one */
    if (CFE_TBL_HANDLE_IS_VALID(*LocalHandle))
    {
        CFE_TBL_Unregister(*LocalHandle);
    }

    Result = CFE_TBL_Share(LocalHandle, Name);
    if (Result < CFE_SUCCESS)
    {
        *LocalHandle = CFE_TBL_BAD_TABLE_HANDLE;
    }
    else
    {
        /* Try getting an address again, we still do not have one */
        Result = CS_GetTableAddr(State, *LocalHandle);
    }

    if (Result >= CFE_SUCCESS)
    {
        Result = CFE_SUCCESS;
    }

    return Result;
}

/*----------------------------------------------------------------
 *
 * Local helper function
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CS_RefreshTableData(CS_LocalChecksumState_t *State, CFE_TBL_HandleId_t *LocalId, const char *Name)
{
    CFE_Status_t     Status;
    CFE_TBL_Handle_t LocalHandle;

    Status = CFE_SUCCESS;

    /* Always refresh the address (we do not hold it between cycles, but we do cache it) */
    /* Notably if this returns CFE_SUCCESS then it means there are no known changes
     * since the last time we checked this table.  */
    if (State->TotalSize != 0)
    {
        LocalHandle = CFE_TBL_HandleFromID(*LocalId);
        Status      = CS_GetTableAddr(State, LocalHandle);
    }
    else
    {
        LocalHandle = CFE_TBL_BAD_TABLE_HANDLE;
    }

    /* NOTE: This intentionally is bypassed only in plain CFE_SUCCESS, meaning that info
     * codes like CFE_TBL_INFO_UPDATED will also trigger refreshing the size info
     * The intent is to save the cost of refreshing the size info if the value we have
     * is still valid. */
    if (Status != CFE_SUCCESS || State->TotalSize == 0)
    {
        Status = CS_RefreshTableSize(State, Name);
    }

    /* Check if the table handle needs a refresh */
    if (Status == CFE_SUCCESS && State->BufferAddr == NULL)
    {
        Status = CS_RefreshTableHandleAndAddress(State, &LocalHandle, Name);
    }

    if (Status == CFE_SUCCESS)
    {
        *LocalId = CFE_TBL_HandleToID(LocalHandle);
    }
    else
    {
        *LocalId = CFE_TBL_HANDLEID_UNDEFINED;
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Local helper function
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CS_RefreshAppSizeAndAddr(CS_LocalChecksumState_t *State, const char *AppName)
{
    CFE_ResourceId_t ResourceID;
    CFE_ES_AppInfo_t AppInfo;
    CFE_Status_t     Result;

    AppInfo.AddressesAreValid = false;
    ResourceID                = CFE_RESOURCEID_UNDEFINED;

    Result = CFE_ES_GetAppIDByName((CFE_ES_AppId_t *)&ResourceID, AppName);
    if (Result == CFE_ES_ERR_NAME_NOT_FOUND)
    {
        /* Also check for a matching library name */
        Result = CFE_ES_GetLibIDByName((CFE_ES_LibId_t *)&ResourceID, AppName);
    }

    if (Result == CFE_SUCCESS)
    {
        /* We got a valid ResourceID, so get the Resource info */
        Result = CFE_ES_GetModuleInfo(&AppInfo, ResourceID);

        /* We got a valid ResourceID and good App info, so check the for valid addresses */
        if (Result == CFE_SUCCESS && !AppInfo.AddressesAreValid)
        {
            CFE_EVS_SendEvent(CS_COMPUTE_APP_PLATFORM_DBG_EID,
                              CFE_EVS_EventType_DEBUG,
                              "CS cannot get a valid address for %s, due to the platform",
                              AppName);

            Result = CS_ERROR;
        }
    }

    if (Result == CFE_SUCCESS)
    {
        /* Push in the data from the module info */
        State->TotalSize  = CFE_ES_MEMOFFSET_TO_SIZET(AppInfo.CodeSize);
        State->BufferAddr = CFE_ES_MEMADDRESS_TO_PTR(AppInfo.CodeAddress);
        State->CurrOffset = 0;
    }
    else
    {
        /* Something failed -- either invalid ResourceID, bad App info, or invalid addresses, so notify ground */
        CFE_EVS_SendEvent(CS_COMPUTE_APP_ERR_EID,
                          CFE_EVS_EventType_ERROR,
                          "CS Apps: Problems getting module %s info, Result: 0x%08X, AddressValid: %d",
                          AppName,
                          (unsigned int)Result,
                          (unsigned int)AppInfo.AddressesAreValid);

        Result = CS_ERR_NOT_FOUND;
    }

    return Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS function that computes the checksum for EEPROM, Memory, OS   */
/* and cFE core code segments                                      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CFE_Status_t
CS_ComputeEepromMemory(CS_Res_EepromMemory_Table_Entry_t *ResultsEntry, uint32 *ComputedCSValue, bool *DoneWithEntry)
{
    CFE_Status_t            Status;
    CS_LocalChecksumState_t State;

    Status = CFE_SUCCESS;

    memset(&State, 0, sizeof(State));

    State.BufferAddr = CFE_ES_MEMADDRESS_TO_PTR(ResultsEntry->StartAddress);
    State.TotalSize  = ResultsEntry->NumBytesToChecksum;
    State.CurrOffset = ResultsEntry->ByteOffset;

    if (!CS_IsChecksumComplete(&State))
    {
        /* resume where left off */
        State.ResultBuffer = ResultsEntry->TempChecksumValue;
    }

    CS_DoChecksumChunk(&State);

    /* Export all values to resume next time */
    ResultsEntry->TempChecksumValue  = State.ResultBuffer;
    ResultsEntry->StartAddress       = (cpuaddr)State.BufferAddr;
    ResultsEntry->ByteOffset         = State.CurrOffset;
    ResultsEntry->NumBytesToChecksum = State.TotalSize;

    /* mirror the value to the supplied register */
    *ComputedCSValue = State.ResultBuffer;

    /* Check against the previous value, if applicable */
    Status = CS_CheckOrUpdateReference(&State, &ResultsEntry->ComputedYet, &ResultsEntry->ComparisonValue);

    *DoneWithEntry = CS_IsChecksumComplete(&State);

    return Status;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS function that computes the checksum for Tables               */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CFE_Status_t CS_ComputeTables(CS_Res_Tables_Table_Entry_t *ResultsEntry, uint32 *ComputedCSValue, bool *DoneWithEntry)
{
    CFE_Status_t            Status;
    CS_LocalChecksumState_t State;

    memset(&State, 0, sizeof(State));

    /* For tables, This size will be nonzero if we have been here before */
    /* Note we do NOT used the cached address, only the cached size.  The address
     * needs to come from tbl services every cycle, since it can be updated */
    State.TotalSize = ResultsEntry->NumBytesToChecksum;

    /* By the time we get here, we know we have an enabled entry */
    Status = CS_RefreshTableData(&State, &ResultsEntry->TblHandleID, ResultsEntry->Name);

    if (Status != CFE_SUCCESS || State.BufferAddr == NULL)
    {
        CFE_EVS_SendEvent(CS_COMPUTE_TABLES_ERR_EID,
                          CFE_EVS_EventType_ERROR,
                          "CS Tables: Problem Getting table %s: Status=0x%08X",
                          ResultsEntry->Name,
                          (unsigned int)Status);

        memset(&State, 0, sizeof(State));

        /* pass generic error to the caller, since the underlying one was logged already */
        Status = CS_ERR_NOT_FOUND;
    }
    else
    {
        /* If the address and size has NOT changed since our last visit, resume checksumming */
        if (ResultsEntry->StartAddress == (cpuaddr)State.BufferAddr
            && ResultsEntry->NumBytesToChecksum == State.TotalSize)
        {
            State.CurrOffset   = ResultsEntry->ByteOffset;
            State.ResultBuffer = ResultsEntry->TempChecksumValue;
        }

        CS_DoChecksumChunk(&State);

        /* mirror the value to the supplied register */
        *ComputedCSValue = State.ResultBuffer;

        /* Check against the previous value, if applicable */
        Status = CS_CheckOrUpdateReference(&State, &ResultsEntry->ComputedYet, &ResultsEntry->ComparisonValue);
    }

    /* If we got a table address, we need to release it (always) */
    if (State.BufferAddr != NULL)
    {
        /* We are done with the table for this cycle, so we need to release the address */
        CS_ReleaseTableAddr(ResultsEntry->TblHandleID, ResultsEntry->Name);
    }

    /* Export all values to resume next time */
    ResultsEntry->TempChecksumValue  = State.ResultBuffer;
    ResultsEntry->StartAddress       = (cpuaddr)State.BufferAddr;
    ResultsEntry->ByteOffset         = State.CurrOffset;
    ResultsEntry->NumBytesToChecksum = State.TotalSize;

    *DoneWithEntry = CS_IsChecksumComplete(&State);

    return Status;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS function that computes the checksum for Apps                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CFE_Status_t CS_ComputeApp(CS_Res_App_Table_Entry_t *ResultsEntry, uint32 *ComputedCSValue, bool *DoneWithEntry)
{
    CFE_Status_t            Status;
    CS_LocalChecksumState_t State;

    Status = CFE_SUCCESS;

    memset(&State, 0, sizeof(State));

    State.BufferAddr = CFE_ES_MEMADDRESS_TO_PTR(ResultsEntry->StartAddress);
    State.TotalSize  = ResultsEntry->NumBytesToChecksum;
    State.CurrOffset = ResultsEntry->ByteOffset;

    if (State.BufferAddr == NULL || CS_IsChecksumComplete(&State))
    {
        Status = CS_RefreshAppSizeAndAddr(&State, ResultsEntry->Name);
    }
    else
    {
        /* Already got address and size, resume where left off */
        State.ResultBuffer = ResultsEntry->TempChecksumValue;
        Status             = CFE_SUCCESS;
    }

    if (Status == CFE_SUCCESS)
    {
        CS_DoChecksumChunk(&State);

        /* Export all values to resume next time */
        ResultsEntry->TempChecksumValue  = State.ResultBuffer;
        ResultsEntry->StartAddress       = (cpuaddr)State.BufferAddr;
        ResultsEntry->ByteOffset         = State.CurrOffset;
        ResultsEntry->NumBytesToChecksum = State.TotalSize;

        /* mirror the value to the supplied register */
        *ComputedCSValue = State.ResultBuffer;

        /* Check against the previous value, if applicable */
        Status = CS_CheckOrUpdateReference(&State, &ResultsEntry->ComputedYet, &ResultsEntry->ComparisonValue);
    } /* end if got module id ok */

    *DoneWithEntry = CS_IsChecksumComplete(&State);

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
    CS_Res_EepromMemory_Table_Entry_t *ResultsEntry     = NULL;
    uint16                             EntryID          = 0;
    CS_ChecksumState_Enum_t            PreviousState    = CS_ChecksumState_EMPTY;
    bool                               DoneWithEntry    = false;
    CS_ChecksumState_Enum_t            PreviousDefState = CS_ChecksumState_EMPTY;
    CS_Def_EepromMemory_Table_Entry_t *DefEntry         = NULL;
    CS_TableWrapper_t                 *tw;

    tw           = &CS_AppData.Tbl[CS_AppData.ChildTaskTable];
    EntryID      = CS_AppData.ChildTaskEntryID;
    ResultsEntry = CS_AppData.RecomputeEepromMemoryEntryPtr;

    /* we want to  make sure that the entry isn't being checksummed in the
     background at the same time we are recomputing */
    PreviousState       = ResultsEntry->State;
    ResultsEntry->State = CS_ChecksumState_DISABLED;

    /* Set entry as if this is the first time we are computing the checksum,
       since we want the entry to take on the new value */

    ResultsEntry->ByteOffset        = 0;
    ResultsEntry->TempChecksumValue = 0;
    ResultsEntry->ComputedYet       = false;

    /* Update the definition table entry as well.  We need to determine which memory type is
       being updated as well as which entry in the table is being updated. */
    if (CS_CheckTableId(tw, CS_ChecksumType_EEPROM_TABLE) || CS_CheckTableId(tw, CS_ChecksumType_MEMORY_TABLE))
    {
        DefEntry = CS_GetDefEntryAddr(tw, EntryID);
    }

    if (DefEntry != NULL && ResultsEntry->StartAddress == DefEntry->StartAddress)
    {
        PreviousDefState = CS_SetDefEntryState(tw, DefEntry, CS_ChecksumState_DISABLED);
    }
    else
    {
        DefEntry = NULL;
    }

    while (!DoneWithEntry)
    {
        CS_ComputeEepromMemory(ResultsEntry, &NewChecksumValue, &DoneWithEntry);

        OS_TaskDelay(CS_CHILD_TASK_DELAY);
    }

    /* The new checksum value is stored in the table by the above functions */

    /* restore the entry's previous state */
    ResultsEntry->State = PreviousState;

    /* Restore the definition table if we found one earlier */
    if (DefEntry != NULL)
    {
        CS_SetDefEntryState(tw, DefEntry, PreviousDefState);
    }

    /* Update reported value in HK TLM, if applicable */
    if (tw->BaselineValue)
    {
        *tw->BaselineValue = NewChecksumValue;
    }

    /* send event message */
    CFE_EVS_SendEvent(CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID,
                      CFE_EVS_EventType_INFORMATION,
                      "%s entry %d recompute finished. New baseline is 0X%08X",
                      CS_GetTableTypeAsString(tw),
                      EntryID,
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
    CS_ChecksumState_Enum_t   PreviousState    = CS_ChecksumState_EMPTY;
    bool                      DoneWithEntry    = false;
    CFE_Status_t              Status           = CS_ERROR;
    CS_ChecksumState_Enum_t   PreviousDefState = CS_ChecksumState_EMPTY;
    CS_Def_App_Table_Entry_t *DefTblEntry      = NULL;

    CS_TableWrapper_t *tw = &CS_AppData.Tbl[CS_ChecksumType_APP_TABLE];

    /* Get the variables to use from the global data */
    ResultsEntry = CS_AppData.RecomputeAppEntryPtr;

    /* we want to  make sure that the entry isn't being checksummed in the
     background at the same time we are recomputing */

    PreviousState       = ResultsEntry->State;
    ResultsEntry->State = CS_ChecksumState_DISABLED;

    /* Set entry as if this is the first time we are computing the checksum,
       since we want the entry to take on the new value */

    ResultsEntry->ByteOffset        = 0;
    ResultsEntry->TempChecksumValue = 0;
    ResultsEntry->ComputedYet       = false;

    /* Update the definition table entry as well.  We need to determine which memory type is
       being updated as well as which entry in the table is being updated. */

    CS_GetAppDefTblEntryByName(&DefTblEntry, ResultsEntry->Name);

    if (DefTblEntry != NULL)
    {
        PreviousDefState = CS_SetDefEntryState(tw, DefTblEntry, CS_ChecksumState_DISABLED);
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
    if (DefTblEntry != NULL)
    {
        CS_SetDefEntryState(tw, DefTblEntry, PreviousDefState);
    }

    if (Status == CS_ERR_NOT_FOUND)
    {
        CFE_EVS_SendEvent(CS_RECOMPUTE_ERROR_APP_ERR_EID,
                          CFE_EVS_EventType_ERROR,
                          "App %s recompute failed. Could not get address",
                          ResultsEntry->Name);
    }
    else
    {
        /* send event message */
        CFE_EVS_SendEvent(CS_RECOMPUTE_FINISH_APP_INF_EID,
                          CFE_EVS_EventType_INFORMATION,
                          "App %s recompute finished. New baseline is 0x%08X",
                          ResultsEntry->Name,
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
    CS_ChecksumState_Enum_t      PreviousState    = CS_ChecksumState_EMPTY;
    bool                         DoneWithEntry    = false;
    CFE_Status_t                 Status           = CS_ERROR;
    CS_ChecksumState_Enum_t      PreviousDefState = CS_ChecksumState_EMPTY;
    CS_Def_Tables_Table_Entry_t *DefTblEntry      = NULL;

    CS_TableWrapper_t *tw = &CS_AppData.Tbl[CS_ChecksumType_TABLES_TABLE];

    /* Get the variables to use from the global data */
    ResultsEntry = CS_AppData.RecomputeTablesEntryPtr;

    /* we want to  make sure that the entry isn't being checksummed in the
     background at the same time we are recomputing */

    PreviousState       = ResultsEntry->State;
    ResultsEntry->State = CS_ChecksumState_DISABLED;

    /* Set entry as if this is the first time we are computing the checksum,
     since we want the entry to take on the new value */

    ResultsEntry->ByteOffset        = 0;
    ResultsEntry->TempChecksumValue = 0;
    ResultsEntry->ComputedYet       = false;

    /* Update the definition table entry as well.  We need to determine which memory type is
     being updated as well as which entry in the table is being updated. */
    CS_GetTableDefTblEntryByName(&DefTblEntry, ResultsEntry->Name);
    if (DefTblEntry != NULL)
    {
        PreviousDefState = CS_SetDefEntryState(tw, DefTblEntry, CS_ChecksumState_DISABLED);
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
        CFE_EVS_SendEvent(CS_RECOMPUTE_ERROR_TABLES_ERR_EID,
                          CFE_EVS_EventType_ERROR,
                          "Table %s recompute failed. Could not get address",
                          ResultsEntry->Name);
    }
    else
    {
        /* send event message */
        CFE_EVS_SendEvent(CS_RECOMPUTE_FINISH_TABLES_INF_EID,
                          CFE_EVS_EventType_INFORMATION,
                          "Table %s recompute finished. New baseline is 0x%08X",
                          ResultsEntry->Name,
                          (unsigned int)NewChecksumValue);
    }

    /* restore the entry's state */
    ResultsEntry->State = PreviousState;

    /* Restore the definition table if we found one earlier */
    if (DefTblEntry != NULL)
    {
        CS_SetDefEntryState(tw, DefTblEntry, PreviousDefState);
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
    uint32  NewChecksumValue        = 0;
    size_t  NumBytesRemainingCycles = 0;
    size_t  NumBytesThisCycle       = 0;
    cpuaddr FirstAddrThisCycle      = 0;
    size_t  MaxBytesPerCycle        = 0;

    NewChecksumValue        = 0;
    NumBytesRemainingCycles = CS_AppData.HkPacket.Payload.LastOneShotSize;
    FirstAddrThisCycle      = CS_AppData.HkPacket.Payload.LastOneShotAddress;
    MaxBytesPerCycle        = CS_AppData.HkPacket.Payload.LastOneShotMaxBytesPerCycle;

    while (NumBytesRemainingCycles > 0)
    {
        NumBytesThisCycle = ((MaxBytesPerCycle < NumBytesRemainingCycles) ? MaxBytesPerCycle : NumBytesRemainingCycles);

        NewChecksumValue = CFE_ES_CalculateCRC((void *)(FirstAddrThisCycle),
                                               NumBytesThisCycle,
                                               NewChecksumValue,
                                               CS_DEFAULT_ALGORITHM);

        /* Update the remainders for the next cycle */
        FirstAddrThisCycle      += NumBytesThisCycle;
        NumBytesRemainingCycles -= NumBytesThisCycle;

        OS_TaskDelay(CS_CHILD_TASK_DELAY);
    }

    /*Checksum Calculation is done! */

    /* put the new checksum value in the baseline */
    CS_AppData.HkPacket.Payload.LastOneShotChecksum = NewChecksumValue;

    /* send event message */
    CFE_EVS_SendEvent(CS_ONESHOT_FINISHED_INF_EID,
                      CFE_EVS_EventType_INFORMATION,
                      "OneShot checksum on Address: 0x%08X, size %d completed. Checksum =  0x%08X",
                      (unsigned int)(CS_AppData.HkPacket.Payload.LastOneShotAddress),
                      (unsigned int)(CS_AppData.HkPacket.Payload.LastOneShotSize),
                      (unsigned int)(CS_AppData.HkPacket.Payload.LastOneShotChecksum));

    CS_AppData.HkPacket.Payload.OneShotInProgress = false;
    CS_AppData.ChildTaskID                        = CFE_ES_TASKID_UNDEFINED;

    CFE_ES_ExitChildTask();
}
