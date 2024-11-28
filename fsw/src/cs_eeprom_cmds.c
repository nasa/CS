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
 *   The CFS Checksum (CS) Application's commands for checking EEPROM
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
#include "cs_eeprom_cmds.h"
#include "cs_utils.h"

/**************************************************************************
 **
 ** Functions
 **
 **************************************************************************/

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Disable background checking of EEPROM command                */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_DisableEepromCmd(const CS_NoArgsCmd_t *CmdPtr)
{
    if (CS_CheckRecomputeOneshot() == false)
    {
        CS_AppData.HkPacket.Payload.EepromCSState = CS_STATE_DISABLED;
        CS_ZeroEepromTempValues();

#if (CS_PRESERVE_STATES_ON_PROCESSOR_RESET == true)
        CS_UpdateCDS();
#endif

        CFE_EVS_SendEvent(CS_DISABLE_EEPROM_INF_EID, CFE_EVS_EventType_INFORMATION,
                          "Checksumming of EEPROM is Disabled");

        CS_AppData.HkPacket.Payload.CmdCounter++;
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Enable background checking of EEPROM command                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_EnableEepromCmd(const CS_NoArgsCmd_t *CmdPtr)
{
    if (CS_CheckRecomputeOneshot() == false)
    {
        CS_AppData.HkPacket.Payload.EepromCSState = CS_STATE_ENABLED;

#if (CS_PRESERVE_STATES_ON_PROCESSOR_RESET == true)
        CS_UpdateCDS();
#endif

        CFE_EVS_SendEvent(CS_ENABLE_EEPROM_INF_EID, CFE_EVS_EventType_INFORMATION, "Checksumming of EEPROM is Enabled");

        CS_AppData.HkPacket.Payload.CmdCounter++;
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Report the baseline checksum of an entry in the EEPROM table */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_ReportBaselineEntryIDEepromCmd(const CS_EntryCmd_t *CmdPtr)
{
    /* command verification variables */
    uint32                            Baseline = 0;
    uint16                            EntryID  = 0;
    uint16                            State    = CS_STATE_EMPTY;
    CS_Res_EepromMemory_Table_Entry_t ResultsEntry;

    EntryID = CmdPtr->Payload.EntryID;

    if ((EntryID < CS_MAX_NUM_EEPROM_TABLE_ENTRIES) && (CS_AppData.ResEepromTblPtr[EntryID].State != CS_STATE_EMPTY))
    {
        ResultsEntry = CS_AppData.ResEepromTblPtr[EntryID];

        if (ResultsEntry.ComputedYet == true)
        {
            Baseline = ResultsEntry.ComparisonValue;

            CFE_EVS_SendEvent(CS_BASELINE_EEPROM_INF_EID, CFE_EVS_EventType_INFORMATION,
                              "Report baseline of EEPROM Entry %d is 0x%08X", EntryID, (unsigned int)Baseline);
        }
        else
        {
            CFE_EVS_SendEvent(CS_NO_BASELINE_EEPROM_INF_EID, CFE_EVS_EventType_INFORMATION,
                              "Report baseline of EEPROM Entry %d has not been computed yet", EntryID);
        }
        CS_AppData.HkPacket.Payload.CmdCounter++;
    }
    else
    {
        if (EntryID >= CS_MAX_NUM_EEPROM_TABLE_ENTRIES)
        {
            State = CS_STATE_UNDEFINED;
        }
        else
        {
            State = CS_AppData.ResEepromTblPtr[EntryID].State;
        }

        CFE_EVS_SendEvent(CS_BASELINE_INVALID_ENTRY_EEPROM_ERR_EID, CFE_EVS_EventType_ERROR,
                          "EEPROM report baseline failed, Entry ID invalid: %d, State: %d Max ID: %d", EntryID, State,
                          (CS_MAX_NUM_EEPROM_TABLE_ENTRIES - 1));
        CS_AppData.HkPacket.Payload.CmdErrCounter++;
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Recompute the baseline of an entry in the EEPROM table cmd   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_RecomputeBaselineEepromCmd(const CS_EntryCmd_t *CmdPtr)
{
    /* command verification variables */
    CFE_ES_TaskId_t ChildTaskID = CFE_ES_TASKID_UNDEFINED;
    CFE_Status_t    Status;
    uint16          EntryID;
    uint16          State = CS_STATE_EMPTY;

    EntryID = CmdPtr->Payload.EntryID;

    if (CS_AppData.HkPacket.Payload.RecomputeInProgress == false &&
        CS_AppData.HkPacket.Payload.OneShotInProgress == false)
    {
        /* make sure the entry is a valid number and is defined in the table */
        if ((EntryID < CS_MAX_NUM_EEPROM_TABLE_ENTRIES) &&
            (CS_AppData.ResEepromTblPtr[EntryID].State != CS_STATE_EMPTY))
        {
            /* There is no child task running right now, we can use it*/
            CS_AppData.HkPacket.Payload.RecomputeInProgress = true;

            /* fill in child task variables */
            CS_AppData.ChildTaskTable   = CS_EEPROM_TABLE;
            CS_AppData.ChildTaskEntryID = EntryID;

            CS_AppData.RecomputeEepromMemoryEntryPtr = &CS_AppData.ResEepromTblPtr[EntryID];

            Status = CFE_ES_CreateChildTask(&ChildTaskID, CS_RECOMP_EEPROM_TASK_NAME, CS_RecomputeEepromMemoryChildTask,
                                            NULL, CFE_PLATFORM_ES_DEFAULT_STACK_SIZE, CS_CHILD_TASK_PRIORITY, 0);
            if (Status == CFE_SUCCESS)
            {
                CFE_EVS_SendEvent(CS_RECOMPUTE_EEPROM_STARTED_DBG_EID, CFE_EVS_EventType_DEBUG,
                                  "Recompute baseline of EEPROM Entry ID %d started", EntryID);
                CS_AppData.HkPacket.Payload.CmdCounter++;
            }
            else /* child task creation failed */
            {
                CFE_EVS_SendEvent(
                    CS_RECOMPUTE_EEPROM_CREATE_CHDTASK_ERR_EID, CFE_EVS_EventType_ERROR,
                    "Recompute baseline of EEPROM Entry ID %d failed, CFE_ES_CreateChildTask returned:  0x%08X",
                    EntryID, (unsigned int)Status);
                CS_AppData.HkPacket.Payload.CmdErrCounter++;
                CS_AppData.HkPacket.Payload.RecomputeInProgress = false;
            }
        }
        else
        {
            if (EntryID >= CS_MAX_NUM_EEPROM_TABLE_ENTRIES)
            {
                State = CS_STATE_UNDEFINED;
            }
            else
            {
                State = CS_AppData.ResEepromTblPtr[EntryID].State;
            }

            CFE_EVS_SendEvent(CS_RECOMPUTE_INVALID_ENTRY_EEPROM_ERR_EID, CFE_EVS_EventType_ERROR,
                              "EEPROM recompute baseline of entry failed, Entry ID invalid: %d, State: %d, Max ID: %d",
                              EntryID, State, (CS_MAX_NUM_EEPROM_TABLE_ENTRIES - 1));

            CS_AppData.HkPacket.Payload.CmdErrCounter++;
        }
    }
    else
    {
        /*send event that we can't start another task right now */
        CFE_EVS_SendEvent(CS_RECOMPUTE_EEPROM_CHDTASK_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Recompute baseline of EEPROM Entry ID %d failed: child task in use", EntryID);
        CS_AppData.HkPacket.Payload.CmdErrCounter++;
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Enable a specific entry in the EEPROM table command          */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_EnableEntryIDEepromCmd(const CS_EntryCmd_t *CmdPtr)
{
    /* command verification variables */
    CS_Res_EepromMemory_Table_Entry_t *ResultsEntry = NULL;
    uint16                             EntryID;
    uint16                             State = CS_STATE_EMPTY;

    if (CS_CheckRecomputeOneshot() == false)
    {
        EntryID = CmdPtr->Payload.EntryID;

        if ((EntryID < CS_MAX_NUM_EEPROM_TABLE_ENTRIES) &&
            (CS_AppData.ResEepromTblPtr[EntryID].State != CS_STATE_EMPTY))
        {
            ResultsEntry = &CS_AppData.ResEepromTblPtr[EntryID];

            ResultsEntry->State = CS_STATE_ENABLED;

            CFE_EVS_SendEvent(CS_ENABLE_EEPROM_ENTRY_INF_EID, CFE_EVS_EventType_INFORMATION,
                              "Checksumming of EEPROM Entry ID %d is Enabled", EntryID);

            if (CS_AppData.DefEepromTblPtr[EntryID].State != CS_STATE_EMPTY)
            {
                CS_AppData.DefEepromTblPtr[EntryID].State = CS_STATE_ENABLED;
                CS_ResetTablesTblResultEntry(CS_AppData.EepResTablesTblPtr);
                CFE_TBL_Modified(CS_AppData.DefEepromTableHandle);
            }
            else
            {
                CFE_EVS_SendEvent(CS_ENABLE_EEPROM_DEF_EMPTY_DBG_EID, CFE_EVS_EventType_DEBUG,
                                  "CS unable to update EEPROM definition table for entry %d, State: %d", EntryID,
                                  State);
            }

            CS_AppData.HkPacket.Payload.CmdCounter++;
        }
        else
        {
            if (EntryID >= CS_MAX_NUM_EEPROM_TABLE_ENTRIES)
            {
                State = CS_STATE_UNDEFINED;
            }
            else
            {
                State = CS_AppData.ResEepromTblPtr[EntryID].State;
            }

            CFE_EVS_SendEvent(CS_ENABLE_EEPROM_INVALID_ENTRY_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Enable EEPROM entry failed, invalid Entry ID:  %d, State: %d, Max ID: %d", EntryID,
                              State, (CS_MAX_NUM_EEPROM_TABLE_ENTRIES - 1));
            CS_AppData.HkPacket.Payload.CmdErrCounter++;
        }
    } /* end InProgress if */
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Disable a specific entry in the EEPROM table command         */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_DisableEntryIDEepromCmd(const CS_EntryCmd_t *CmdPtr)
{
    /* command verification variables */
    CS_Res_EepromMemory_Table_Entry_t *ResultsEntry = NULL;
    uint16                             EntryID;
    uint16                             State = CS_STATE_EMPTY;

    if (CS_CheckRecomputeOneshot() == false)
    {
        EntryID = CmdPtr->Payload.EntryID;

        if ((EntryID < CS_MAX_NUM_EEPROM_TABLE_ENTRIES) &&
            (CS_AppData.ResEepromTblPtr[EntryID].State != CS_STATE_EMPTY))
        {
            ResultsEntry = &CS_AppData.ResEepromTblPtr[EntryID];

            ResultsEntry->State             = CS_STATE_DISABLED;
            ResultsEntry->TempChecksumValue = 0;
            ResultsEntry->ByteOffset        = 0;

            CFE_EVS_SendEvent(CS_DISABLE_EEPROM_ENTRY_INF_EID, CFE_EVS_EventType_INFORMATION,
                              "Checksumming of EEPROM Entry ID %d is Disabled", EntryID);

            if (CS_AppData.DefEepromTblPtr[EntryID].State != CS_STATE_EMPTY)
            {
                CS_AppData.DefEepromTblPtr[EntryID].State = CS_STATE_DISABLED;
                CS_ResetTablesTblResultEntry(CS_AppData.EepResTablesTblPtr);
                CFE_TBL_Modified(CS_AppData.DefEepromTableHandle);
            }
            else
            {
                CFE_EVS_SendEvent(CS_DISABLE_EEPROM_DEF_EMPTY_DBG_EID, CFE_EVS_EventType_DEBUG,
                                  "CS unable to update EEPROM definition table for entry %d, State: %d", EntryID,
                                  State);
            }

            CS_AppData.HkPacket.Payload.CmdCounter++;
        }
        else
        {
            if (EntryID >= CS_MAX_NUM_EEPROM_TABLE_ENTRIES)
            {
                State = CS_STATE_UNDEFINED;
            }
            else
            {
                State = CS_AppData.ResEepromTblPtr[EntryID].State;
            }

            CFE_EVS_SendEvent(CS_DISABLE_EEPROM_INVALID_ENTRY_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Disable EEPROM entry failed, invalid Entry ID:  %d, State: %d, Max ID: %d", EntryID,
                              State, (CS_MAX_NUM_EEPROM_TABLE_ENTRIES - 1));

            CS_AppData.HkPacket.Payload.CmdErrCounter++;
        }
    } /* end InProgress if */
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Retrieve an EntryID based on Address from EEPROM table cmd   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_GetEntryIDEepromCmd(const CS_GetEntryIDCmd_t *CmdPtr)
{
    /* command verification variables */
    CS_Res_EepromMemory_Table_Entry_t *StartOfResultsTable = NULL;
    uint16                             Loop;
    bool                               EntryFound = false;
    CS_Res_EepromMemory_Table_Entry_t  ResultsEntry;

    StartOfResultsTable = CS_AppData.ResEepromTblPtr;

    for (Loop = 0; Loop < CS_MAX_NUM_EEPROM_TABLE_ENTRIES; Loop++)
    {
        ResultsEntry = StartOfResultsTable[Loop];

        if ((ResultsEntry.StartAddress <= CmdPtr->Payload.Address) &&
            CmdPtr->Payload.Address <= (ResultsEntry.StartAddress + ResultsEntry.NumBytesToChecksum) &&
            ResultsEntry.State != CS_STATE_EMPTY)
        {
            CFE_EVS_SendEvent(CS_GET_ENTRY_ID_EEPROM_INF_EID, CFE_EVS_EventType_INFORMATION,
                              "EEPROM Found Address 0x%08X in Entry ID %d", (unsigned int)(CmdPtr->Payload.Address),
                              Loop);
            EntryFound = true;
        }
    }

    if (EntryFound == false)
    {
        CFE_EVS_SendEvent(CS_GET_ENTRY_ID_EEPROM_NOT_FOUND_INF_EID, CFE_EVS_EventType_INFORMATION,
                          "Address 0x%08X was not found in EEPROM table", (unsigned int)(CmdPtr->Payload.Address));
    }
    CS_AppData.HkPacket.Payload.CmdCounter++;
}
