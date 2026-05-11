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
 *   The CFS Checksum (CS) Application's commands for checking Memory
 */

/**************************************************************************
 **
 ** Include section
 **
 **************************************************************************/
#include "cfe.h"
#include "cs_tbl.h"
#include "cs_app.h"
#include "cs_eventids.h"
#include "cs_compute.h"
#include "cs_memory_cmds.h"
#include "cs_utils.h"

/**************************************************************************
 **
 ** Functions
 **
 **************************************************************************/

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Disable background checking of Memory command                */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CFE_Status_t CS_DisableMemoryCmd(const CS_DisableMemoryCmd_t *CmdPtr)
{
    if (CS_CheckRecomputeOneshot() == false)
    {
        CS_AppData.HkPacket.Payload.MemoryCSState = CS_ChecksumState_DISABLED;
        CS_ZeroMemoryTempValues();

#if (CS_PRESERVE_STATES_ON_PROCESSOR_RESET == true)
        CS_UpdateCDS();
#endif

        CFE_EVS_SendEvent(CS_DISABLE_MEMORY_INF_EID,
                          CFE_EVS_EventType_INFORMATION,
                          "Checksumming of Memory is Disabled");

        CS_AppData.HkPacket.Payload.CommandCounter++;
    }

    return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Enable background checking of Memory command                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CFE_Status_t CS_EnableMemoryCmd(const CS_EnableMemoryCmd_t *CmdPtr)
{
    if (CS_CheckRecomputeOneshot() == false)
    {
        CS_AppData.HkPacket.Payload.MemoryCSState = CS_ChecksumState_ENABLED;

#if (CS_PRESERVE_STATES_ON_PROCESSOR_RESET == true)
        CS_UpdateCDS();
#endif

        CFE_EVS_SendEvent(CS_ENABLE_MEMORY_INF_EID, CFE_EVS_EventType_INFORMATION, "Checksumming of Memory is Enabled");

        CS_AppData.HkPacket.Payload.CommandCounter++;
    }

    return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Report the baseline checksum of an entry in the Memory table */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CFE_Status_t CS_ReportBaselineEntryIDMemoryCmd(const CS_ReportBaselineEntryIDMemoryCmd_t *CmdPtr)
{
    /* command verification variables */
    CS_Res_EepromMemory_Table_Entry_t *ResultsEntry = NULL;
    uint32                             Baseline     = 0;
    uint16                             EntryID      = 0;
    CS_ChecksumState_Enum_t            State        = CS_ChecksumState_EMPTY;

    EntryID      = CmdPtr->Payload.EntryID;
    ResultsEntry = CS_GetMemoryResEntry(EntryID);

    if (ResultsEntry == NULL)
    {
        State = CS_ChecksumState_UNDEFINED;
    }
    else
    {
        State = ResultsEntry->State;
    }

    if (CS_StateValid(State))
    {
        if (ResultsEntry->ComputedYet == true)
        {
            Baseline = ResultsEntry->ComparisonValue;

            CFE_EVS_SendEvent(CS_BASELINE_MEMORY_INF_EID,
                              CFE_EVS_EventType_INFORMATION,
                              "Report baseline of Memory Entry %d is 0x%08X",
                              EntryID,
                              (unsigned int)Baseline);
        }
        else
        {
            CFE_EVS_SendEvent(CS_NO_BASELINE_MEMORY_INF_EID,
                              CFE_EVS_EventType_INFORMATION,
                              "Report baseline of Memory Entry %d has not been computed yet",
                              EntryID);
        }
        CS_AppData.HkPacket.Payload.CommandCounter++;
    }
    else
    {
        CFE_EVS_SendEvent(CS_BASELINE_INVALID_ENTRY_MEMORY_ERR_EID,
                          CFE_EVS_EventType_ERROR,
                          "Memory report baseline failed, Entry ID invalid: %d, State: %d Max ID: %d",
                          EntryID,
                          State,
                          (CS_MAX_NUM_MEMORY_TABLE_ENTRIES - 1));
        CS_AppData.HkPacket.Payload.CommandErrorCounter++;
    }

    return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Recompute the baseline of an entry in the Memory table cmd   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CFE_Status_t CS_RecomputeBaselineMemoryCmd(const CS_RecomputeBaselineMemoryCmd_t *CmdPtr)
{
    /* command verification variables */
    CS_Res_EepromMemory_Table_Entry_t *ResultsEntry = NULL;
    CFE_ES_TaskId_t                    ChildTaskID  = CFE_ES_TASKID_UNDEFINED;
    CFE_Status_t                       Status       = CS_ERROR;
    uint16                             EntryID      = 0;
    CS_ChecksumState_Enum_t            State        = CS_ChecksumState_EMPTY;

    EntryID = CmdPtr->Payload.EntryID;

    if (CS_AppData.HkPacket.Payload.RecomputeInProgress == false
        && CS_AppData.HkPacket.Payload.OneShotInProgress == false)
    {
        ResultsEntry = CS_GetMemoryResEntry(EntryID);

        if (ResultsEntry == NULL)
        {
            State = CS_ChecksumState_UNDEFINED;
        }
        else
        {
            State = ResultsEntry->State;
        }

        if (CS_StateValid(State))
        {
            /* There is no child task running right now, we can use it*/
            CS_AppData.HkPacket.Payload.RecomputeInProgress = true;

            /* fill in child task variables */
            CS_AppData.ChildTaskTable   = CS_ChecksumType_MEMORY_TABLE;
            CS_AppData.ChildTaskEntryID = EntryID;

            CS_AppData.RecomputeEepromMemoryEntryPtr = ResultsEntry;

            Status = CFE_ES_CreateChildTask(&ChildTaskID,
                                            CS_RECOMP_MEMORY_TASK_NAME,
                                            CS_RecomputeEepromMemoryChildTask,
                                            NULL,
                                            CFE_PLATFORM_ES_DEFAULT_STACK_SIZE,
                                            CS_CHILD_TASK_PRIORITY,
                                            0);
            if (Status == CFE_SUCCESS)
            {
                CFE_EVS_SendEvent(CS_RECOMPUTE_MEMORY_STARTED_DBG_EID,
                                  CFE_EVS_EventType_DEBUG,
                                  "Recompute baseline of Memory Entry ID %d started",
                                  EntryID);
                CS_AppData.HkPacket.Payload.CommandCounter++;
            }
            else /* child task creation failed */
            {
                CFE_EVS_SendEvent(
                    CS_RECOMPUTE_MEMORY_CREATE_CHDTASK_ERR_EID,
                    CFE_EVS_EventType_ERROR,
                    "Recompute baseline of Memory Entry ID %d failed, ES_CreateChildTask returned:  0x%08X",
                    EntryID,
                    (unsigned int)Status);
                CS_AppData.HkPacket.Payload.CommandErrorCounter++;
                CS_AppData.HkPacket.Payload.RecomputeInProgress = false;
            }
        }
        else
        {
            CFE_EVS_SendEvent(CS_RECOMPUTE_INVALID_ENTRY_MEMORY_ERR_EID,
                              CFE_EVS_EventType_ERROR,
                              "Memory recompute baseline of entry failed, Entry ID invalid: %d, State: %d, Max ID: %d",
                              EntryID,
                              State,
                              (CS_MAX_NUM_MEMORY_TABLE_ENTRIES - 1));

            CS_AppData.HkPacket.Payload.CommandErrorCounter++;
        }
    }
    else
    {
        /*send event that we can't start another task right now */
        CFE_EVS_SendEvent(CS_RECOMPUTE_MEMORY_CHDTASK_ERR_EID,
                          CFE_EVS_EventType_ERROR,
                          "Recompute baseline of Memory Entry ID %d failed: child task in use",
                          EntryID);
        CS_AppData.HkPacket.Payload.CommandErrorCounter++;
    }

    return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Enable a specific entry in the Memory table command          */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CFE_Status_t CS_EnableEntryIDMemoryCmd(const CS_EnableEntryIDMemoryCmd_t *CmdPtr)
{
    /* command verification variables */
    CS_Res_EepromMemory_Table_Entry_t *ResultsEntry = NULL;
    CS_Def_EepromMemory_Table_Entry_t *DefEntry     = NULL;
    CS_TableWrapper_t                 *tw           = &CS_AppData.Tbl[CS_ChecksumType_MEMORY_TABLE];
    uint16                             EntryID      = 0;
    CS_ChecksumState_Enum_t            State        = CS_ChecksumState_EMPTY;

    if (CS_CheckRecomputeOneshot() == false)
    {
        EntryID      = CmdPtr->Payload.EntryID;
        ResultsEntry = CS_GetMemoryResEntry(EntryID);

        if (ResultsEntry == NULL)
        {
            State = CS_ChecksumState_UNDEFINED;
        }
        else
        {
            State = ResultsEntry->State;
        }

        if (CS_StateValid(State))
        {
            DefEntry            = CS_GetMemoryDefEntry(EntryID);
            ResultsEntry->State = CS_ChecksumState_ENABLED;

            CFE_EVS_SendEvent(CS_ENABLE_MEMORY_ENTRY_INF_EID,
                              CFE_EVS_EventType_INFORMATION,
                              "Checksumming of Memory Entry ID %d is Enabled",
                              EntryID);

            State = CS_SetDefEntryState(tw, DefEntry, CS_ChecksumState_ENABLED);
            if (!CS_StateValid(State))
            {
                CFE_EVS_SendEvent(CS_ENABLE_MEMORY_DEF_EMPTY_DBG_EID,
                                  CFE_EVS_EventType_DEBUG,
                                  "CS unable to update memory definition table for entry %d, State: %d",
                                  EntryID,
                                  State);
            }

            CS_AppData.HkPacket.Payload.CommandCounter++;
        }
        else
        {
            CFE_EVS_SendEvent(CS_ENABLE_MEMORY_INVALID_ENTRY_ERR_EID,
                              CFE_EVS_EventType_ERROR,
                              "Enable Memory entry failed, invalid Entry ID:  %d, State: %d, Max ID: %d",
                              EntryID,
                              State,
                              (CS_MAX_NUM_MEMORY_TABLE_ENTRIES - 1));
            CS_AppData.HkPacket.Payload.CommandErrorCounter++;
        }
    } /* end InProgress if */

    return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Disable a specific entry in the Memory table command         */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CFE_Status_t CS_DisableEntryIDMemoryCmd(const CS_DisableEntryIDMemoryCmd_t *CmdPtr)
{
    /* command verification variables */
    CS_Res_EepromMemory_Table_Entry_t *ResultsEntry = NULL;
    CS_Def_EepromMemory_Table_Entry_t *DefEntry     = NULL;
    CS_TableWrapper_t                 *tw           = &CS_AppData.Tbl[CS_ChecksumType_MEMORY_TABLE];
    uint16                             EntryID      = 0;
    CS_ChecksumState_Enum_t            State        = CS_ChecksumState_EMPTY;

    if (CS_CheckRecomputeOneshot() == false)
    {
        EntryID      = CmdPtr->Payload.EntryID;
        ResultsEntry = CS_GetMemoryResEntry(EntryID);

        if (ResultsEntry == NULL)
        {
            State = CS_ChecksumState_UNDEFINED;
        }
        else
        {
            State = ResultsEntry->State;
        }

        if (CS_StateValid(State))
        {
            DefEntry = CS_GetMemoryDefEntry(EntryID);

            ResultsEntry->State             = CS_ChecksumState_DISABLED;
            ResultsEntry->TempChecksumValue = 0;
            ResultsEntry->ByteOffset        = 0;

            CFE_EVS_SendEvent(CS_DISABLE_MEMORY_ENTRY_INF_EID,
                              CFE_EVS_EventType_INFORMATION,
                              "Checksumming of Memory Entry ID %d is Disabled",
                              EntryID);

            State = CS_SetDefEntryState(tw, DefEntry, CS_ChecksumState_DISABLED);
            if (!CS_StateValid(State))
            {
                CFE_EVS_SendEvent(CS_DISABLE_MEMORY_DEF_EMPTY_DBG_EID,
                                  CFE_EVS_EventType_DEBUG,
                                  "CS unable to update memory definition table for entry %d, State: %d",
                                  EntryID,
                                  State);
            }

            CS_AppData.HkPacket.Payload.CommandCounter++;
        }
        else
        {
            CFE_EVS_SendEvent(CS_DISABLE_MEMORY_INVALID_ENTRY_ERR_EID,
                              CFE_EVS_EventType_ERROR,
                              "Disable Memory entry failed, invalid Entry ID:  %d, State: %d, Max ID: %d",
                              EntryID,
                              State,
                              (CS_MAX_NUM_MEMORY_TABLE_ENTRIES - 1));

            CS_AppData.HkPacket.Payload.CommandErrorCounter++;
        }
    } /* end InProgress if */

    return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Retrieve an EntryID based on Address from Memory table cmd   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CFE_Status_t CS_GetEntryIDMemoryCmd(const CS_GetEntryIDMemoryCmd_t *CmdPtr)
{
    /* command verification variables */
    CS_Res_EepromMemory_Table_Entry_t *ResultsEntry = NULL;
    uint16                             Loop         = 0;
    bool                               EntryFound   = false;

    Loop = 0;
    while (true)
    {
        ResultsEntry = CS_GetMemoryResEntry(Loop);
        if (ResultsEntry == NULL)
        {
            break;
        }

        if (ResultsEntry->State != CS_ChecksumState_EMPTY && ResultsEntry->StartAddress <= CmdPtr->Payload.Address
            && CmdPtr->Payload.Address < (ResultsEntry->StartAddress + ResultsEntry->NumBytesToChecksum))
        {
            CFE_EVS_SendEvent(CS_GET_ENTRY_ID_MEMORY_INF_EID,
                              CFE_EVS_EventType_INFORMATION,
                              "Memory Found Address 0x%08X in Entry ID %d",
                              (unsigned int)(CmdPtr->Payload.Address),
                              Loop);
            EntryFound = true;
        }

        ++Loop;
    }

    if (EntryFound == false)
    {
        CFE_EVS_SendEvent(CS_GET_ENTRY_ID_MEMORY_NOT_FOUND_INF_EID,
                          CFE_EVS_EventType_INFORMATION,
                          "Address 0x%08X was not found in Memory table",
                          (unsigned int)(CmdPtr->Payload.Address));
    }
    CS_AppData.HkPacket.Payload.CommandCounter++;

    return CFE_SUCCESS;
}
