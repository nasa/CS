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
 *   The CFS Checksum (CS) Application's commands for checking Tables
 */

/*************************************************************************
 **
 ** Include section
 **
 **************************************************************************/
#include "cfe.h"
#include "cs_app.h"
#include "cs_events.h"
#include "cs_utils.h"
#include "cs_compute.h"
#include "cs_table_cmds.h"
/**************************************************************************
 **
 ** Functions
 **
 **************************************************************************/

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Disable background checking of Tables command                */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_DisableTablesCmd(const CS_NoArgsCmd_t *CmdPtr)
{
        if (CS_CheckRecomputeOneshot() == false)
        {
            CS_AppData.HkPacket.Payload.TablesCSState = CS_STATE_DISABLED;
            CS_ZeroTablesTempValues();

#if (CS_PRESERVE_STATES_ON_PROCESSOR_RESET == true)
            CS_UpdateCDS();
#endif

            CFE_EVS_SendEvent(CS_DISABLE_TABLES_INF_EID, CFE_EVS_EventType_INFORMATION,
                              "Checksumming of Tables is Disabled");
            CS_AppData.HkPacket.Payload.CmdCounter++;
        }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Enable background checking of Tables command                 */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_EnableTablesCmd(const CS_NoArgsCmd_t *CmdPtr)
{
        if (CS_CheckRecomputeOneshot() == false)
        {
            CS_AppData.HkPacket.Payload.TablesCSState = CS_STATE_ENABLED;

#if (CS_PRESERVE_STATES_ON_PROCESSOR_RESET == true)
            CS_UpdateCDS();
#endif

            CFE_EVS_SendEvent(CS_ENABLE_TABLES_INF_EID, CFE_EVS_EventType_INFORMATION,
                              "Checksumming of Tables is Enabled");
            CS_AppData.HkPacket.Payload.CmdCounter++;
        }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Report the baseline checksum of an entry in the Tables table */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_ReportBaselineTablesCmd(const CS_TableNameCmd_t *CmdPtr)
{
    CS_Res_Tables_Table_Entry_t *ResultsEntry;
    uint32                       Baseline;
    char                         Name[CFE_TBL_MAX_FULL_NAME_LEN];

        strncpy(Name, CmdPtr->Payload.Name, sizeof(Name) - 1);
        Name[sizeof(Name) - 1] = '\0';

        if (CS_GetTableResTblEntryByName(&ResultsEntry, Name))
        {
            if (ResultsEntry->ComputedYet == true)
            {
                Baseline = ResultsEntry->ComparisonValue;
                CFE_EVS_SendEvent(CS_BASELINE_TABLES_INF_EID, CFE_EVS_EventType_INFORMATION,
                                  "Report baseline of table %s is 0x%08X", Name, (unsigned int)Baseline);
            }
            else
            {
                CFE_EVS_SendEvent(CS_NO_BASELINE_TABLES_INF_EID, CFE_EVS_EventType_INFORMATION,
                                  "Report baseline of table %s has not been computed yet", Name);
            }
            CS_AppData.HkPacket.Payload.CmdCounter++;
        }
        else
        {
            CFE_EVS_SendEvent(CS_BASELINE_INVALID_NAME_TABLES_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Tables report baseline failed, table %s not found", Name);
            CS_AppData.HkPacket.Payload.CmdErrCounter++;
        }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Recompute the baseline of an entry in the Tables table cmd   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_RecomputeBaselineTablesCmd(const CS_TableNameCmd_t *CmdPtr)
{
    CFE_ES_TaskId_t              ChildTaskID;
    CFE_Status_t                 Status;
    CS_Res_Tables_Table_Entry_t *ResultsEntry;
    char                         Name[CFE_TBL_MAX_FULL_NAME_LEN];

        if (CS_AppData.HkPacket.Payload.RecomputeInProgress == false && CS_AppData.HkPacket.Payload.OneShotInProgress == false)
        {
            strncpy(Name, CmdPtr->Payload.Name, sizeof(Name) - 1);
            Name[sizeof(Name) - 1] = '\0';

            /* make sure the entry is a valid number and is defined in the table */
            if (CS_GetTableResTblEntryByName(&ResultsEntry, Name))
            {
                /* There is no child task running right now, we can use it*/
                CS_AppData.HkPacket.Payload.RecomputeInProgress = true;

                /* fill in child task variables */
                CS_AppData.ChildTaskTable = CS_TABLES_TABLE;

                CS_AppData.RecomputeTablesEntryPtr = ResultsEntry;

                Status = CFE_ES_CreateChildTask(&ChildTaskID, CS_RECOMP_TABLES_TASK_NAME, CS_RecomputeTablesChildTask,
                                                NULL, CFE_PLATFORM_ES_DEFAULT_STACK_SIZE, CS_CHILD_TASK_PRIORITY, 0);
                if (Status == CFE_SUCCESS)
                {
                    CFE_EVS_SendEvent(CS_RECOMPUTE_TABLES_STARTED_INF_EID, CFE_EVS_EventType_INFORMATION,
                                      "Recompute baseline of table %s started", Name);
                    CS_AppData.HkPacket.Payload.CmdCounter++;
                }
                else /* child task creation failed */
                {
                    CFE_EVS_SendEvent(CS_RECOMPUTE_TABLES_CREATE_CHDTASK_ERR_EID, CFE_EVS_EventType_ERROR,
                                      "Recompute baseline of table %s failed, CFE_ES_CreateChildTask returned: 0x%08X",
                                      Name, (unsigned int)Status);
                    CS_AppData.HkPacket.Payload.CmdErrCounter++;
                    CS_AppData.HkPacket.Payload.RecomputeInProgress = false;
                }
            }
            else
            {
                CFE_EVS_SendEvent(CS_RECOMPUTE_UNKNOWN_NAME_TABLES_ERR_EID, CFE_EVS_EventType_ERROR,
                                  "Tables recompute baseline failed, table %s not found", Name);
                CS_AppData.HkPacket.Payload.CmdErrCounter++;
            }
        }
        else
        {
            /*send event that we can't start another task right now */
            CFE_EVS_SendEvent(CS_RECOMPUTE_TABLES_CHDTASK_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Tables recompute baseline for table %s failed: child task in use", Name);
            CS_AppData.HkPacket.Payload.CmdErrCounter++;
        }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Disable a specific entry in the Tables table command         */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_DisableNameTablesCmd(const CS_TableNameCmd_t *CmdPtr)
{
    CS_Res_Tables_Table_Entry_t *ResultsEntry;
    CS_Def_Tables_Table_Entry_t *DefinitionEntry;
    char                         Name[CFE_TBL_MAX_FULL_NAME_LEN];

        if (CS_CheckRecomputeOneshot() == false)
        {
            strncpy(Name, CmdPtr->Payload.Name, sizeof(Name) - 1);
            Name[sizeof(Name) - 1] = '\0';

            if (CS_GetTableResTblEntryByName(&ResultsEntry, Name))
            {
                ResultsEntry->State             = CS_STATE_DISABLED;
                ResultsEntry->TempChecksumValue = 0;
                ResultsEntry->ByteOffset        = 0;

                CFE_EVS_SendEvent(CS_DISABLE_TABLES_NAME_INF_EID, CFE_EVS_EventType_INFORMATION,
                                  "Checksumming of table %s is Disabled", Name);

                if (CS_GetTableDefTblEntryByName(&DefinitionEntry, Name))
                {
                    DefinitionEntry->State = CS_STATE_DISABLED;
                    CS_ResetTablesTblResultEntry(CS_AppData.TblResTablesTblPtr);
                    CFE_TBL_Modified(CS_AppData.DefTablesTableHandle);
                }
                else
                {
                    CFE_EVS_SendEvent(CS_DISABLE_TABLE_DEF_NOT_FOUND_ERR_EID, CFE_EVS_EventType_ERROR,
                                      "CS unable to update tables definition table for entry %s", Name);
                }

                CS_AppData.HkPacket.Payload.CmdCounter++;
            }
            else
            {
                CFE_EVS_SendEvent(CS_DISABLE_TABLES_UNKNOWN_NAME_ERR_EID, CFE_EVS_EventType_ERROR,
                                  "Tables disable table command failed, table %s not found", Name);
                CS_AppData.HkPacket.Payload.CmdErrCounter++;
            }
        } /* end InProgress if */
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Enable a specific entry in the Tables table command          */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_EnableNameTablesCmd(const CS_TableNameCmd_t *CmdPtr)
{
    CS_Res_Tables_Table_Entry_t *ResultsEntry;
    CS_Def_Tables_Table_Entry_t *DefinitionEntry;
    char                         Name[CFE_TBL_MAX_FULL_NAME_LEN];

        if (CS_CheckRecomputeOneshot() == false)
        {
            strncpy(Name, CmdPtr->Payload.Name, sizeof(Name) - 1);
            Name[sizeof(Name) - 1] = '\0';

            if (CS_GetTableResTblEntryByName(&ResultsEntry, Name))
            {
                ResultsEntry->State = CS_STATE_ENABLED;

                CFE_EVS_SendEvent(CS_ENABLE_TABLES_NAME_INF_EID, CFE_EVS_EventType_INFORMATION,
                                  "Checksumming of table %s is Enabled", Name);

                if (CS_GetTableDefTblEntryByName(&DefinitionEntry, Name))
                {
                    DefinitionEntry->State = CS_STATE_ENABLED;
                    CS_ResetTablesTblResultEntry(CS_AppData.TblResTablesTblPtr);
                    CFE_TBL_Modified(CS_AppData.DefTablesTableHandle);
                }
                else
                {
                    CFE_EVS_SendEvent(CS_ENABLE_TABLE_DEF_NOT_FOUND_ERR_EID, CFE_EVS_EventType_ERROR,
                                      "CS unable to update tables definition table for entry %s", Name);
                }

                CS_AppData.HkPacket.Payload.CmdCounter++;
            }
            else
            {
                CFE_EVS_SendEvent(CS_ENABLE_TABLES_UNKNOWN_NAME_ERR_EID, CFE_EVS_EventType_ERROR,
                                  "Tables enable table command failed, table %s not found", Name);
                CS_AppData.HkPacket.Payload.CmdErrCounter++;
            }
        } /* end InProgress if */
}
