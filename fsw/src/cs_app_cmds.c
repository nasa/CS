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
 *   The CFS Checksum (CS) Application's commands for checking App
 */

/*************************************************************************
 **
 ** Include section
 **
 **************************************************************************/
#include "cfe.h"
#include "cs_app.h"
#include "cs_eventids.h"
#include "cs_utils.h"
#include "cs_compute.h"
#include "cs_app_cmds.h"
/**************************************************************************
 **
 ** Functions
 **
 **************************************************************************/

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Disable background checking of App command                   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CFE_Status_t CS_DisableAppsCmd(const CS_DisableAppsCmd_t *CmdPtr)
{
    if (CS_CheckRecomputeOneshot() == false)
    {
        CS_AppData.HkPacket.Payload.AppCSState = CS_ChecksumState_DISABLED;
        CS_ZeroAppTempValues();

#if (CS_PRESERVE_STATES_ON_PROCESSOR_RESET == true)
        CS_UpdateCDS();
#endif

        CFE_EVS_SendEvent(CS_DISABLE_APP_INF_EID, CFE_EVS_EventType_INFORMATION, "Checksumming of App is Disabled");
        CS_AppData.HkPacket.Payload.CommandCounter++;
    }

    return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Enable background checking of App command                    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CFE_Status_t CS_EnableAppsCmd(const CS_EnableAppsCmd_t *CmdPtr)
{
    if (CS_CheckRecomputeOneshot() == false)
    {
        CS_AppData.HkPacket.Payload.AppCSState = CS_ChecksumState_ENABLED;

#if (CS_PRESERVE_STATES_ON_PROCESSOR_RESET == true)
        CS_UpdateCDS();
#endif

        CFE_EVS_SendEvent(CS_ENABLE_APP_INF_EID, CFE_EVS_EventType_INFORMATION, "Checksumming of App is Enabled");
        CS_AppData.HkPacket.Payload.CommandCounter++;
    }

    return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Report the baseline checksum of an entry in the App table    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CFE_Status_t CS_ReportBaselineAppCmd(const CS_ReportBaselineAppCmd_t *CmdPtr)
{
    /* command verification variables */
    CS_Res_App_Table_Entry_t *ResultsEntry;
    uint32                    Baseline;
    char                      Name[OS_MAX_API_NAME];

    strncpy(Name, CmdPtr->Payload.Name, sizeof(Name) - 1);
    Name[sizeof(Name) - 1] = '\0';

    if (CS_GetAppResTblEntryByName(&ResultsEntry, Name))
    {
        if (ResultsEntry->ComputedYet == true)
        {
            Baseline = ResultsEntry->ComparisonValue;
            CFE_EVS_SendEvent(CS_BASELINE_APP_INF_EID,
                              CFE_EVS_EventType_INFORMATION,
                              "Report baseline of app %s is 0x%08X",
                              Name,
                              (unsigned int)Baseline);
        }
        else
        {
            CFE_EVS_SendEvent(CS_NO_BASELINE_APP_INF_EID,
                              CFE_EVS_EventType_INFORMATION,
                              "Report baseline of app %s has not been computed yet",
                              Name);
        }
        CS_AppData.HkPacket.Payload.CommandCounter++;
    }
    else
    {
        CFE_EVS_SendEvent(CS_BASELINE_INVALID_NAME_APP_ERR_EID,
                          CFE_EVS_EventType_ERROR,
                          "App report baseline failed, app %s not found",
                          Name);
        CS_AppData.HkPacket.Payload.CommandErrorCounter++;
    }

    return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Recompute the baseline of an entry in the App table cmd      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CFE_Status_t CS_RecomputeBaselineAppCmd(const CS_RecomputeBaselineAppCmd_t *CmdPtr)
{
    /* command verification variables */
    CFE_ES_TaskId_t           ChildTaskID;
    CFE_Status_t              Status;
    CS_Res_App_Table_Entry_t *ResultsEntry;
    char                      Name[OS_MAX_API_NAME];

    if (CS_AppData.HkPacket.Payload.RecomputeInProgress == false
        && CS_AppData.HkPacket.Payload.OneShotInProgress == false)
    {
        strncpy(Name, CmdPtr->Payload.Name, sizeof(Name) - 1);
        Name[sizeof(Name) - 1] = '\0';

        /* make sure the entry is a valid number and is defined in the table */
        if (CS_GetAppResTblEntryByName(&ResultsEntry, Name))
        {
            /* There is no child task running right now, we can use it*/
            CS_AppData.HkPacket.Payload.RecomputeInProgress = true;

            /* fill in child task variables */
            CS_AppData.ChildTaskTable = CS_ChecksumType_APP_TABLE;

            CS_AppData.RecomputeAppEntryPtr = ResultsEntry;

            Status = CFE_ES_CreateChildTask(&ChildTaskID,
                                            CS_RECOMP_APP_TASK_NAME,
                                            CS_RecomputeAppChildTask,
                                            NULL,
                                            CFE_PLATFORM_ES_DEFAULT_STACK_SIZE,
                                            CS_CHILD_TASK_PRIORITY,
                                            0);
            if (Status == CFE_SUCCESS)
            {
                CFE_EVS_SendEvent(CS_RECOMPUTE_APP_STARTED_DBG_EID,
                                  CFE_EVS_EventType_DEBUG,
                                  "Recompute baseline of app %s started",
                                  Name);
                CS_AppData.HkPacket.Payload.CommandCounter++;
            }
            else /* child task creation failed */
            {
                CFE_EVS_SendEvent(CS_RECOMPUTE_APP_CREATE_CHDTASK_ERR_EID,
                                  CFE_EVS_EventType_ERROR,
                                  "Recompute baseline of app %s failed, CFE_ES_CreateChildTask returned: 0x%08X",
                                  Name,
                                  (unsigned int)Status);
                CS_AppData.HkPacket.Payload.CommandErrorCounter++;
                CS_AppData.HkPacket.Payload.RecomputeInProgress = false;
            }
        }
        else
        {
            CFE_EVS_SendEvent(CS_RECOMPUTE_UNKNOWN_NAME_APP_ERR_EID,
                              CFE_EVS_EventType_ERROR,
                              "App recompute baseline failed, app %s not found",
                              Name);
            CS_AppData.HkPacket.Payload.CommandErrorCounter++;
        }
    }
    else
    {
        /*send event that we can't start another task right now */
        CFE_EVS_SendEvent(CS_RECOMPUTE_APP_CHDTASK_ERR_EID,
                          CFE_EVS_EventType_ERROR,
                          "App recompute baseline for app %s failed: child task in use",
                          Name);
        CS_AppData.HkPacket.Payload.CommandErrorCounter++;
    }

    return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Disable a specific entry in the App table command            */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CFE_Status_t CS_DisableNameAppCmd(const CS_DisableNameAppCmd_t *CmdPtr)
{
    /* command verification variables */
    CS_Res_App_Table_Entry_t *ResultsEntry;
    CS_Def_App_Table_Entry_t *DefinitionEntry;
    CS_TableWrapper_t        *tw = &CS_AppData.Tbl[CS_ChecksumType_APP_TABLE];
    char                      Name[OS_MAX_API_NAME];

    if (CS_CheckRecomputeOneshot() == false)
    {
        strncpy(Name, CmdPtr->Payload.Name, sizeof(Name) - 1);
        Name[sizeof(Name) - 1] = '\0';

        if (CS_GetAppResTblEntryByName(&ResultsEntry, Name))
        {
            ResultsEntry->State             = CS_ChecksumState_DISABLED;
            ResultsEntry->TempChecksumValue = 0;
            ResultsEntry->ByteOffset        = 0;

            CFE_EVS_SendEvent(CS_DISABLE_APP_NAME_INF_EID,
                              CFE_EVS_EventType_INFORMATION,
                              "Checksumming of app %s is Disabled",
                              Name);

            if (CS_GetAppDefTblEntryByName(&DefinitionEntry, Name))
            {
                CS_SetDefEntryState(tw, DefinitionEntry, CS_ChecksumState_DISABLED);
            }
            else
            {
                CFE_EVS_SendEvent(CS_DISABLE_APP_DEF_NOT_FOUND_DBG_EID,
                                  CFE_EVS_EventType_DEBUG,
                                  "CS unable to update apps definition table for entry %s",
                                  Name);
            }

            CS_AppData.HkPacket.Payload.CommandCounter++;
        }

        else
        {
            CFE_EVS_SendEvent(CS_DISABLE_APP_UNKNOWN_NAME_ERR_EID,
                              CFE_EVS_EventType_ERROR,
                              "App disable app command failed, app %s not found",
                              Name);
            CS_AppData.HkPacket.Payload.CommandErrorCounter++;
        }
    } /* end InProgress if */

    return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Enable a specific entry in the App table command             */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CFE_Status_t CS_EnableNameAppCmd(const CS_EnableNameAppCmd_t *CmdPtr)
{
    /* command verification variables */
    CS_Res_App_Table_Entry_t *ResultsEntry;
    CS_Def_App_Table_Entry_t *DefinitionEntry;
    CS_TableWrapper_t        *tw = &CS_AppData.Tbl[CS_ChecksumType_APP_TABLE];
    char                      Name[OS_MAX_API_NAME];

    if (CS_CheckRecomputeOneshot() == false)
    {
        strncpy(Name, CmdPtr->Payload.Name, sizeof(Name) - 1);
        Name[sizeof(Name) - 1] = '\0';

        if (CS_GetAppResTblEntryByName(&ResultsEntry, Name))
        {
            ResultsEntry->State = CS_ChecksumState_ENABLED;

            CFE_EVS_SendEvent(CS_ENABLE_APP_NAME_INF_EID,
                              CFE_EVS_EventType_INFORMATION,
                              "Checksumming of app %s is Enabled",
                              Name);

            if (CS_GetAppDefTblEntryByName(&DefinitionEntry, Name))
            {
                CS_SetDefEntryState(tw, DefinitionEntry, CS_ChecksumState_ENABLED);
            }
            else
            {
                CFE_EVS_SendEvent(CS_ENABLE_APP_DEF_NOT_FOUND_DBG_EID,
                                  CFE_EVS_EventType_DEBUG,
                                  "CS unable to update apps definition table for entry %s",
                                  Name);
            }

            CS_AppData.HkPacket.Payload.CommandCounter++;
        }
        else
        {
            CFE_EVS_SendEvent(CS_ENABLE_APP_UNKNOWN_NAME_ERR_EID,
                              CFE_EVS_EventType_ERROR,
                              "App enable app command failed, app %s not found",
                              Name);
            CS_AppData.HkPacket.Payload.CommandErrorCounter++;
        }
    } /* end InProgress if */

    return CFE_SUCCESS;
}
