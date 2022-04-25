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
 *   The CFS Checksum (CS) Application's commands for checking Memory
 */

/**************************************************************************
 **
 ** Include section
 **
 **************************************************************************/
#include "cfe.h"
#include "cs_tbldefs.h"
#include "cs_app.h"
#include "cs_events.h"
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
void CS_DisableMemoryCmd(const CS_NoArgsCmd_t *CmdPtr)
{
    /* command verification variables */
    size_t ExpectedLength = sizeof(CS_NoArgsCmd_t);

    /* Verify command packet length */
    if (CS_VerifyCmdLength(&CmdPtr->CmdHeader.Msg, ExpectedLength))
    {
        if (CS_CheckRecomputeOneshot() == false)
        {

            CS_AppData.HkPacket.MemoryCSState = CS_STATE_DISABLED;
            CS_ZeroMemoryTempValues();

#if (CS_PRESERVE_STATES_ON_PROCESSOR_RESET == true)
            CS_UpdateCDS();
#endif

            CFE_EVS_SendEvent(CS_DISABLE_MEMORY_INF_EID, CFE_EVS_EventType_INFORMATION,
                              "Checksumming of Memory is Disabled");

            CS_AppData.HkPacket.CmdCounter++;
        }
    }
    return;
} /* End of CS_DisableMemoryCmd () */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Enable background checking of Memory command                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_EnableMemoryCmd(const CS_NoArgsCmd_t *CmdPtr)
{
    /* command verification variables */
    size_t ExpectedLength = sizeof(CS_NoArgsCmd_t);

    /* Verify command packet length */
    if (CS_VerifyCmdLength(&CmdPtr->CmdHeader.Msg, ExpectedLength))
    {

        if (CS_CheckRecomputeOneshot() == false)
        {

            CS_AppData.HkPacket.MemoryCSState = CS_STATE_ENABLED;

#if (CS_PRESERVE_STATES_ON_PROCESSOR_RESET == true)
            CS_UpdateCDS();
#endif

            CFE_EVS_SendEvent(CS_ENABLE_MEMORY_INF_EID, CFE_EVS_EventType_INFORMATION,
                              "Checksumming of Memory is Enabled");

            CS_AppData.HkPacket.CmdCounter++;
        }
    }
    return;
} /* End of CS_EnableMemoryCmd () */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Report the baseline checksum of an entry in the Memory table */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_ReportBaselineEntryIDMemoryCmd(const CS_EntryCmd_t *CmdPtr)
{
    /* command verification variables */
    size_t                             ExpectedLength = sizeof(CS_EntryCmd_t);
    CS_Res_EepromMemory_Table_Entry_t *ResultsEntry   = NULL;
    uint32                             Baseline       = 0;
    uint16                             EntryID        = 0;
    uint16                             State          = CS_STATE_EMPTY;

    /* Verify command packet length */
    if (CS_VerifyCmdLength(&CmdPtr->CmdHeader.Msg, ExpectedLength))
    {
        EntryID = CmdPtr->EntryID;

        if ((EntryID < CS_MAX_NUM_MEMORY_TABLE_ENTRIES) &&
            (CS_AppData.ResMemoryTblPtr[EntryID].State != CS_STATE_EMPTY))
        {
            ResultsEntry = &CS_AppData.ResMemoryTblPtr[EntryID];

            if (ResultsEntry->ComputedYet == true)
            {
                Baseline = ResultsEntry->ComparisonValue;

                CFE_EVS_SendEvent(CS_BASELINE_MEMORY_INF_EID, CFE_EVS_EventType_INFORMATION,
                                  "Report baseline of Memory Entry %d is 0x%08X", EntryID, (unsigned int)Baseline);
            }
            else
            {
                CFE_EVS_SendEvent(CS_NO_BASELINE_MEMORY_INF_EID, CFE_EVS_EventType_INFORMATION,
                                  "Report baseline of Memory Entry %d has not been computed yet", EntryID);
            }
            CS_AppData.HkPacket.CmdCounter++;
        }
        else
        {
            if (EntryID >= CS_MAX_NUM_MEMORY_TABLE_ENTRIES)
            {
                State = CS_STATE_UNDEFINED;
            }
            else
            {
                State = CS_AppData.ResMemoryTblPtr[EntryID].State;
            }

            CFE_EVS_SendEvent(CS_BASELINE_INVALID_ENTRY_MEMORY_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Memory report baseline failed, Entry ID invalid: %d, State: %d Max ID: %d", EntryID,
                              State, (CS_MAX_NUM_MEMORY_TABLE_ENTRIES - 1));
            CS_AppData.HkPacket.CmdErrCounter++;
        }
    }
    return;
} /* End of CS_ReportBaselineEntryIDCmd () */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Recompute the baseline of an entry in the Memory table cmd   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_RecomputeBaselineMemoryCmd(const CS_EntryCmd_t *CmdPtr)
{
    /* command verification variables */
    size_t          ExpectedLength = sizeof(CS_EntryCmd_t);
    CFE_ES_TaskId_t ChildTaskID    = CFE_ES_TASKID_UNDEFINED;
    int32           Status         = CS_ERROR;
    uint16          EntryID        = 0;
    uint16          State          = CS_STATE_EMPTY;

    /* Verify command packet length */
    if (CS_VerifyCmdLength(&CmdPtr->CmdHeader.Msg, ExpectedLength))
    {
        EntryID = CmdPtr->EntryID;

        if (CS_AppData.HkPacket.RecomputeInProgress == false && CS_AppData.HkPacket.OneShotInProgress == false)
        {

            /* make sure the entry is a valid number and is defined in the table */
            if ((EntryID < CS_MAX_NUM_MEMORY_TABLE_ENTRIES) &&
                (CS_AppData.ResMemoryTblPtr[EntryID].State != CS_STATE_EMPTY))
            {

                /* There is no child task running right now, we can use it*/
                CS_AppData.HkPacket.RecomputeInProgress = true;

                /* fill in child task variables */
                CS_AppData.ChildTaskTable   = CS_MEMORY_TABLE;
                CS_AppData.ChildTaskEntryID = EntryID;

                CS_AppData.RecomputeEepromMemoryEntryPtr = &CS_AppData.ResMemoryTblPtr[EntryID];

                Status =
                    CFE_ES_CreateChildTask(&ChildTaskID, CS_RECOMP_MEMORY_TASK_NAME, CS_RecomputeEepromMemoryChildTask,
                                           NULL, CFE_PLATFORM_ES_DEFAULT_STACK_SIZE, CS_CHILD_TASK_PRIORITY, 0);
                if (Status == CFE_SUCCESS)
                {
                    CFE_EVS_SendEvent(CS_RECOMPUTE_MEMORY_STARTED_DBG_EID, CFE_EVS_EventType_DEBUG,
                                      "Recompute baseline of Memory Entry ID %d started", EntryID);
                    CS_AppData.HkPacket.CmdCounter++;
                }
                else /* child task creation failed */
                {
                    CFE_EVS_SendEvent(
                        CS_RECOMPUTE_MEMORY_CREATE_CHDTASK_ERR_EID, CFE_EVS_EventType_ERROR,
                        "Recompute baseline of Memory Entry ID %d failed, ES_CreateChildTask returned:  0x%08X",
                        EntryID, (unsigned int)Status);
                    CS_AppData.HkPacket.CmdErrCounter++;
                    CS_AppData.HkPacket.RecomputeInProgress = false;
                }
            }
            else
            {
                if (EntryID >= CS_MAX_NUM_MEMORY_TABLE_ENTRIES)
                {
                    State = CS_STATE_UNDEFINED;
                }
                else
                {
                    State = CS_AppData.ResMemoryTblPtr[EntryID].State;
                }

                CFE_EVS_SendEvent(
                    CS_RECOMPUTE_INVALID_ENTRY_MEMORY_ERR_EID, CFE_EVS_EventType_ERROR,
                    "Memory recompute baseline of entry failed, Entry ID invalid: %d, State: %d, Max ID: %d", EntryID,
                    State, (CS_MAX_NUM_MEMORY_TABLE_ENTRIES - 1));

                CS_AppData.HkPacket.CmdErrCounter++;
            }
        }
        else
        {
            /*send event that we can't start another task right now */
            CFE_EVS_SendEvent(CS_RECOMPUTE_MEMORY_CHDTASK_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Recompute baseline of Memory Entry ID %d failed: child task in use", EntryID);
            CS_AppData.HkPacket.CmdErrCounter++;
        }
    }
    return;
} /* end CS_RecomputeBaselineMemoryCmd */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Enable a specific entry in the Memory table command          */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_EnableEntryIDMemoryCmd(const CS_EntryCmd_t *CmdPtr)
{
    /* command verification variables */
    size_t                             ExpectedLength = sizeof(CS_EntryCmd_t);
    CS_Res_EepromMemory_Table_Entry_t *ResultsEntry   = NULL;
    uint16                             EntryID        = 0;
    uint16                             State          = CS_STATE_EMPTY;

    /* Verify command packet length */
    if (CS_VerifyCmdLength(&CmdPtr->CmdHeader.Msg, ExpectedLength))
    {
        if (CS_CheckRecomputeOneshot() == false)
        {
            EntryID = CmdPtr->EntryID;

            if ((EntryID < CS_MAX_NUM_MEMORY_TABLE_ENTRIES) &&
                (CS_AppData.ResMemoryTblPtr[EntryID].State != CS_STATE_EMPTY))
            {
                ResultsEntry = &CS_AppData.ResMemoryTblPtr[EntryID];

                ResultsEntry->State = CS_STATE_ENABLED;

                CFE_EVS_SendEvent(CS_ENABLE_MEMORY_ENTRY_INF_EID, CFE_EVS_EventType_INFORMATION,
                                  "Checksumming of Memory Entry ID %d is Enabled", EntryID);

                if (CS_AppData.DefMemoryTblPtr[EntryID].State != CS_STATE_EMPTY)
                {
                    CS_AppData.DefMemoryTblPtr[EntryID].State = CS_STATE_ENABLED;
                    CS_ResetTablesTblResultEntry(CS_AppData.MemResTablesTblPtr);
                    CFE_TBL_Modified(CS_AppData.DefMemoryTableHandle);
                }
                else
                {
                    CFE_EVS_SendEvent(CS_ENABLE_MEMORY_DEF_EMPTY_DBG_EID, CFE_EVS_EventType_DEBUG,
                                      "CS unable to update memory definition table for entry %d, State: %d", EntryID,
                                      State);
                }

                CS_AppData.HkPacket.CmdCounter++;
            }
            else
            {
                if (EntryID >= CS_MAX_NUM_MEMORY_TABLE_ENTRIES)
                {
                    State = CS_STATE_UNDEFINED;
                }
                else
                {
                    State = CS_AppData.ResMemoryTblPtr[EntryID].State;
                }

                CFE_EVS_SendEvent(CS_ENABLE_MEMORY_INVALID_ENTRY_ERR_EID, CFE_EVS_EventType_ERROR,
                                  "Enable Memory entry failed, invalid Entry ID:  %d, State: %d, Max ID: %d", EntryID,
                                  State, (CS_MAX_NUM_MEMORY_TABLE_ENTRIES - 1));
                CS_AppData.HkPacket.CmdErrCounter++;
            }
        } /* end InProgress if */
    }
    return;
} /* End of CS_EnableCSEntryIDMemoryCmd () */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Disable a specific entry in the Memory table command         */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_DisableEntryIDMemoryCmd(const CS_EntryCmd_t *CmdPtr)
{
    /* command verification variables */
    size_t                             ExpectedLength = sizeof(CS_EntryCmd_t);
    CS_Res_EepromMemory_Table_Entry_t *ResultsEntry   = NULL;
    uint16                             EntryID        = 0;
    uint16                             State          = CS_STATE_EMPTY;

    /* Verify command packet length */
    if (CS_VerifyCmdLength(&CmdPtr->CmdHeader.Msg, ExpectedLength))
    {

        if (CS_CheckRecomputeOneshot() == false)
        {
            EntryID = CmdPtr->EntryID;

            if ((EntryID < CS_MAX_NUM_MEMORY_TABLE_ENTRIES) &&
                (CS_AppData.ResMemoryTblPtr[EntryID].State != CS_STATE_EMPTY))
            {
                ResultsEntry = &CS_AppData.ResMemoryTblPtr[EntryID];

                ResultsEntry->State             = CS_STATE_DISABLED;
                ResultsEntry->TempChecksumValue = 0;
                ResultsEntry->ByteOffset        = 0;

                CFE_EVS_SendEvent(CS_DISABLE_MEMORY_ENTRY_INF_EID, CFE_EVS_EventType_INFORMATION,
                                  "Checksumming of Memory Entry ID %d is Disabled", EntryID);

                if (CS_AppData.DefMemoryTblPtr[EntryID].State != CS_STATE_EMPTY)
                {
                    CS_AppData.DefMemoryTblPtr[EntryID].State = CS_STATE_DISABLED;
                    CS_ResetTablesTblResultEntry(CS_AppData.MemResTablesTblPtr);
                    CFE_TBL_Modified(CS_AppData.DefMemoryTableHandle);
                }
                else
                {
                    CFE_EVS_SendEvent(CS_DISABLE_MEMORY_DEF_EMPTY_DBG_EID, CFE_EVS_EventType_DEBUG,
                                      "CS unable to update memory definition table for entry %d, State: %d", EntryID,
                                      State);
                }

                CS_AppData.HkPacket.CmdCounter++;
            }
            else
            {
                if (EntryID >= CS_MAX_NUM_MEMORY_TABLE_ENTRIES)
                {
                    State = CS_STATE_UNDEFINED;
                }
                else
                {
                    State = CS_AppData.ResMemoryTblPtr[EntryID].State;
                }

                CFE_EVS_SendEvent(CS_DISABLE_MEMORY_INVALID_ENTRY_ERR_EID, CFE_EVS_EventType_ERROR,
                                  "Disable Memory entry failed, invalid Entry ID:  %d, State: %d, Max ID: %d", EntryID,
                                  State, (CS_MAX_NUM_MEMORY_TABLE_ENTRIES - 1));

                CS_AppData.HkPacket.CmdErrCounter++;
            }
        } /* end InProgress if */
    }
    return;
} /* End of CS_DisableCSEntryIDMemoryCmd () */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Retrieve an EntryID based on Address from Memory table cmd   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_GetEntryIDMemoryCmd(const CS_GetEntryIDCmd_t *CmdPtr)
{
    /* command verification variables */
    size_t                             ExpectedLength      = sizeof(CS_GetEntryIDCmd_t);
    CS_Res_EepromMemory_Table_Entry_t *StartOfResultsTable = NULL;
    CS_Res_EepromMemory_Table_Entry_t *ResultsEntry        = NULL;
    uint16                             Loop                = 0;
    bool                               EntryFound          = false;

    /* Verify command packet length */
    if (CS_VerifyCmdLength(&CmdPtr->CmdHeader.Msg, ExpectedLength))
    {
        StartOfResultsTable = CS_AppData.ResMemoryTblPtr;

        for (Loop = 0; Loop < CS_MAX_NUM_MEMORY_TABLE_ENTRIES; Loop++)
        {
            ResultsEntry = &StartOfResultsTable[Loop];

            if ((ResultsEntry->StartAddress <= CmdPtr->Address) &&
                CmdPtr->Address <= (ResultsEntry->StartAddress + ResultsEntry->NumBytesToChecksum) &&
                ResultsEntry->State != CS_STATE_EMPTY)
            {

                CFE_EVS_SendEvent(CS_GET_ENTRY_ID_MEMORY_INF_EID, CFE_EVS_EventType_INFORMATION,
                                  "Memory Found Address 0x%08X in Entry ID %d", (unsigned int)(CmdPtr->Address), Loop);
                EntryFound = true;
            }
        }

        if (EntryFound == false)
        {
            CFE_EVS_SendEvent(CS_GET_ENTRY_ID_MEMORY_NOT_FOUND_INF_EID, CFE_EVS_EventType_INFORMATION,
                              "Address 0x%08X was not found in Memory table", (unsigned int)(CmdPtr->Address));
        }
        CS_AppData.HkPacket.CmdCounter++;
    }
    return;

} /* End of CS_GetEntryIDMemoryCmd () */

/************************/
/*  End of File Comment */
/************************/
