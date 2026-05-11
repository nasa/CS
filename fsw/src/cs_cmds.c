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
 *   The CFS Checksum (CS) Application's commands for OS code segement,
 *   the cFE core code segment, and for CS in general
 */

/**************************************************************************
 **
 ** Include section
 **
 **************************************************************************/
#include "cfe.h"
#include "cs_app.h"
#include "cs_eventids.h"
#include "cs_cmds.h"
#include "cs_utils.h"
#include "cs_compute.h"

/**************************************************************************
 **
 ** Function Prototypes
 **
 **************************************************************************/

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Housekeeping command                                         */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CFE_Status_t CS_SendHkCmd(const CS_SendHkCmd_t *CmdPtr)
{
    /* Send housekeeping telemetry packet */
    CFE_SB_TimeStampMsg(CFE_MSG_PTR(CS_AppData.HkPacket.TelemetryHeader));
    CFE_SB_TransmitMsg(CFE_MSG_PTR(CS_AppData.HkPacket.TelemetryHeader), true);

    /* update each table if there is no recompute happening on that table */
    CS_HandleRoutineTableUpdates();

    return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS no operation command                                         */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CFE_Status_t CS_NoopCmd(const CS_NoopCmd_t *CmdPtr)
{
    CS_AppData.HkPacket.Payload.CommandCounter++;

    CFE_EVS_SendEvent(CS_NOOP_INF_EID,
                      CFE_EVS_EventType_INFORMATION,
                      "No-op command. Version %d.%d.%d.%d",
                      CS_MAJOR_VERSION,
                      CS_MINOR_VERSION,
                      CS_REVISION,
                      CS_MISSION_REV);

    return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Reset Application counters command                           */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CFE_Status_t CS_ResetCountersCmd(const CS_ResetCountersCmd_t *CmdPtr)
{
    CS_AppData.HkPacket.Payload.CommandCounter      = 0;
    CS_AppData.HkPacket.Payload.CommandErrorCounter = 0;

    CS_AppData.HkPacket.Payload.EepromCSErrCounter  = 0;
    CS_AppData.HkPacket.Payload.MemoryCSErrCounter  = 0;
    CS_AppData.HkPacket.Payload.TablesCSErrCounter  = 0;
    CS_AppData.HkPacket.Payload.AppCSErrCounter     = 0;
    CS_AppData.HkPacket.Payload.CfeCoreCSErrCounter = 0;
    CS_AppData.HkPacket.Payload.OSCSErrCounter      = 0;
    CS_AppData.HkPacket.Payload.PassCounter         = 0;

    CFE_EVS_SendEvent(CS_RESET_INF_EID, CFE_EVS_EventType_DEBUG, "Reset Counters command recieved");

    return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS's background checksumming cycle                              */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CFE_Status_t CS_BackgroundCheckCycleCmd(const CS_BackgroundCheckCycleCmd_t *CmdPtr)
{
    /* command verification variables */
    bool DoneWithCycle = false;
    bool EndOfList     = false;

    if (CS_AppData.HkPacket.Payload.ChecksumState == CS_ChecksumState_ENABLED)
    {
        DoneWithCycle = false;
        EndOfList     = false;

        /* Skip this background cycle if there's a recompute or one shot in
         * progress */
        if (CS_AppData.HkPacket.Payload.RecomputeInProgress == true
            || CS_AppData.HkPacket.Payload.OneShotInProgress == true)
        {
            CFE_EVS_SendEvent(CS_BKGND_COMPUTE_PROG_INF_EID,
                              CFE_EVS_EventType_INFORMATION,
                              "Skipping background cycle. Recompute or oneshot in progress.");
            DoneWithCycle = true;
        }

        /* We check for end-of-list because we don't necessarily know the
            order in which the table entries are defined, and we don't
            want to keep looping through the list */

        while ((DoneWithCycle != true) && (EndOfList != true))
        {
            /* We need to check the current table value here because
                it is updated (and possibly reset to zero) inside each
                function called */
            if (CS_AppData.HkPacket.Payload.CurrentCSTable >= (CS_NUM_TABLES - 1))
            {
                EndOfList = true;
            }

            /* Call the appropriate background function based on the current table
                value.  The value is updated inside each function */
            switch (CS_AppData.HkPacket.Payload.CurrentCSTable)
            {
                case (CS_ChecksumType_CFECORE):
                    DoneWithCycle = CS_BackgroundCfeCore();
                    break;

                case (CS_ChecksumType_OSCORE):

                    DoneWithCycle = CS_BackgroundOS();
                    break;

                case (CS_ChecksumType_EEPROM_TABLE):
                    DoneWithCycle = CS_BackgroundEeprom();
                    break;

                case (CS_ChecksumType_MEMORY_TABLE):
                    DoneWithCycle = CS_BackgroundMemory();
                    break;

                case (CS_ChecksumType_TABLES_TABLE):
                    DoneWithCycle = CS_BackgroundTables();
                    break;

                case (CS_ChecksumType_APP_TABLE):

                    DoneWithCycle = CS_BackgroundApp();
                    break;

                    /* default case in case CS_AppData.HkPacket.Payload.CurrentCSTable is some random bad value */
                default:

                    /* We are at the end of the line */
                    CS_AppData.HkPacket.Payload.CurrentCSTable      = 0;
                    CS_AppData.HkPacket.Payload.CurrentEntryInTable = 0;
                    CS_AppData.HkPacket.Payload.PassCounter++;
                    DoneWithCycle = true;
                    break;

            } /* end switch */
        } /* end while */
    }

    return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Disable all background checksumming command                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CFE_Status_t CS_DisableAllCSCmd(const CS_DisableAllCSCmd_t *CmdPtr)
{
    CS_AppData.HkPacket.Payload.ChecksumState = CS_ChecksumState_DISABLED;

    /* zero out the temp values in all the tables
     so that when the checksums are re-enabled,
     they don't start with a half-old value */
    CS_ZeroEepromTempValues();
    CS_ZeroMemoryTempValues();
    CS_ZeroTablesTempValues();
    CS_ZeroAppTempValues();
    CS_ZeroCfeCoreTempValues();
    CS_ZeroOSTempValues();

    CS_AppData.HkPacket.Payload.CommandCounter++;

    CFE_EVS_SendEvent(CS_DISABLE_ALL_INF_EID, CFE_EVS_EventType_INFORMATION, "Background Checksumming Disabled");

    return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Enable all background checksumming command                   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CFE_Status_t CS_EnableAllCSCmd(const CS_EnableAllCSCmd_t *CmdPtr)
{
    CS_AppData.HkPacket.Payload.ChecksumState = CS_ChecksumState_ENABLED;

    CS_AppData.HkPacket.Payload.CommandCounter++;

    CFE_EVS_SendEvent(CS_ENABLE_ALL_INF_EID, CFE_EVS_EventType_INFORMATION, "Background Checksumming Enabled");

    return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Disable background checking of the cFE core command          */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CFE_Status_t CS_DisableCfeCoreCmd(const CS_DisableCfeCoreCmd_t *CmdPtr)
{
    CS_AppData.HkPacket.Payload.CfeCoreCSState = CS_ChecksumState_DISABLED;
    CS_ZeroCfeCoreTempValues();

#if (CS_PRESERVE_STATES_ON_PROCESSOR_RESET == true)
    CS_UpdateCDS();
#endif

    CFE_EVS_SendEvent(CS_DISABLE_CFECORE_INF_EID,
                      CFE_EVS_EventType_INFORMATION,
                      "Checksumming of cFE Core is Disabled");

    CS_AppData.HkPacket.Payload.CommandCounter++;

    return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Enable background checking of the cFE core command           */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CFE_Status_t CS_EnableCfeCoreCmd(const CS_EnableCfeCoreCmd_t *CmdPtr)
{
    CS_AppData.HkPacket.Payload.CfeCoreCSState = CS_ChecksumState_ENABLED;

#if (CS_PRESERVE_STATES_ON_PROCESSOR_RESET == true)
    CS_UpdateCDS();
#endif

    CFE_EVS_SendEvent(CS_ENABLE_CFECORE_INF_EID, CFE_EVS_EventType_INFORMATION, "Checksumming of cFE Core is Enabled");

    CS_AppData.HkPacket.Payload.CommandCounter++;

    return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Disable background checking of the OS code segment command   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CFE_Status_t CS_DisableOSCmd(const CS_DisableOSCmd_t *CmdPtr)
{
    CS_AppData.HkPacket.Payload.OSCSState = CS_ChecksumState_DISABLED;
    CS_ZeroOSTempValues();

#if (CS_PRESERVE_STATES_ON_PROCESSOR_RESET == true)
    CS_UpdateCDS();
#endif

    CFE_EVS_SendEvent(CS_DISABLE_OS_INF_EID,
                      CFE_EVS_EventType_INFORMATION,
                      "Checksumming of OS code segment is Disabled");

    CS_AppData.HkPacket.Payload.CommandCounter++;

    return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Enable background checking of the OS code segment command    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CFE_Status_t CS_EnableOSCmd(const CS_EnableOSCmd_t *CmdPtr)
{
    CS_AppData.HkPacket.Payload.OSCSState = CS_ChecksumState_ENABLED;

#if (CS_PRESERVE_STATES_ON_PROCESSOR_RESET == true)
    CS_UpdateCDS();
#endif

    CFE_EVS_SendEvent(CS_ENABLE_OS_INF_EID,
                      CFE_EVS_EventType_INFORMATION,
                      "Checksumming of OS code segment is Enabled");

    CS_AppData.HkPacket.Payload.CommandCounter++;

    return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Report the baseline checksum for the cFE core command        */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CFE_Status_t CS_ReportBaselineCfeCoreCmd(const CS_ReportBaselineCfeCoreCmd_t *CmdPtr)
{
    CS_Res_EepromMemory_Table_Entry_t *CfeCoreCodeSeg = CS_GetCfeCoreCodeSegResTable();

    if (CfeCoreCodeSeg->ComputedYet == true)
    {
        CFE_EVS_SendEvent(CS_BASELINE_CFECORE_INF_EID,
                          CFE_EVS_EventType_INFORMATION,
                          "Baseline of cFE Core is 0x%08X",
                          (unsigned int)CfeCoreCodeSeg->ComparisonValue);
    }
    else
    {
        CFE_EVS_SendEvent(CS_NO_BASELINE_CFECORE_INF_EID,
                          CFE_EVS_EventType_INFORMATION,
                          "Baseline of cFE Core has not been computed yet");
    }
    CS_AppData.HkPacket.Payload.CommandCounter++;

    return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Report the baseline checksum for the OS code segment command */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CFE_Status_t CS_ReportBaselineOSCmd(const CS_ReportBaselineOSCmd_t *CmdPtr)
{
    CS_Res_EepromMemory_Table_Entry_t *OSCodeSeg = CS_GetOSCodeSegResTable();

    if (OSCodeSeg->ComputedYet == true)
    {
        CFE_EVS_SendEvent(CS_BASELINE_OS_INF_EID,
                          CFE_EVS_EventType_INFORMATION,
                          "Baseline of OS code segment is 0x%08X",
                          (unsigned int)OSCodeSeg->ComparisonValue);
    }
    else
    {
        CFE_EVS_SendEvent(CS_NO_BASELINE_OS_INF_EID,
                          CFE_EVS_EventType_INFORMATION,
                          "Baseline of OS code segment has not been computed yet");
    }
    CS_AppData.HkPacket.Payload.CommandCounter++;

    return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Recompute the baseline checksum for the cFE core command     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CFE_Status_t CS_RecomputeBaselineCfeCoreCmd(const CS_RecomputeBaselineCfeCoreCmd_t *CmdPtr)
{
    /* command verification variables */
    CFE_ES_TaskId_t ChildTaskID;
    CFE_Status_t    Status;

    if (CS_AppData.HkPacket.Payload.RecomputeInProgress == false
        && CS_AppData.HkPacket.Payload.OneShotInProgress == false)
    {
        /* There is no child task running right now, we can use it*/
        CS_AppData.HkPacket.Payload.RecomputeInProgress = true;

        /* fill in child task variables */
        CS_AppData.ChildTaskTable                = CS_ChecksumType_CFECORE;
        CS_AppData.ChildTaskEntryID              = 0;
        CS_AppData.RecomputeEepromMemoryEntryPtr = CS_GetCfeCoreCodeSegResTable();

        Status = CFE_ES_CreateChildTask(&ChildTaskID,
                                        CS_RECOMP_CFECORE_TASK_NAME,
                                        CS_RecomputeEepromMemoryChildTask,
                                        NULL,
                                        CFE_PLATFORM_ES_DEFAULT_STACK_SIZE,
                                        CS_CHILD_TASK_PRIORITY,
                                        0);

        if (Status == CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(CS_RECOMPUTE_CFECORE_STARTED_DBG_EID,
                              CFE_EVS_EventType_DEBUG,
                              "Recompute of cFE core started");
            CS_AppData.HkPacket.Payload.CommandCounter++;
        }
        else /* child task creation failed */
        {
            CFE_EVS_SendEvent(CS_RECOMPUTE_CFECORE_CREATE_CHDTASK_ERR_EID,
                              CFE_EVS_EventType_ERROR,
                              "Recompute cFE core failed, CFE_ES_CreateChildTask returned: 0x%08X",
                              (unsigned int)Status);
            CS_AppData.HkPacket.Payload.CommandErrorCounter++;
            CS_AppData.HkPacket.Payload.RecomputeInProgress = false;
        }
    }
    else
    {
        /*send event that we can't start another task right now */
        CFE_EVS_SendEvent(CS_RECOMPUTE_CFECORE_CHDTASK_ERR_EID,
                          CFE_EVS_EventType_ERROR,
                          "Recompute cFE core failed: child task in use");
        CS_AppData.HkPacket.Payload.CommandErrorCounter++;
    }

    return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Recompute the baseline checksum for the OS code seg command  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CFE_Status_t CS_RecomputeBaselineOSCmd(const CS_RecomputeBaselineOSCmd_t *CmdPtr)
{
    /* command verification variables */
    CFE_ES_TaskId_t ChildTaskID;
    CFE_Status_t    Status;

    if (CS_AppData.HkPacket.Payload.RecomputeInProgress == false
        && CS_AppData.HkPacket.Payload.OneShotInProgress == false)
    {
        /* There is no child task running right now, we can use it*/
        CS_AppData.HkPacket.Payload.RecomputeInProgress = true;

        /* fill in child task variables */
        CS_AppData.ChildTaskTable                = CS_ChecksumType_OSCORE;
        CS_AppData.ChildTaskEntryID              = 0;
        CS_AppData.RecomputeEepromMemoryEntryPtr = CS_GetOSCodeSegResTable();

        Status = CFE_ES_CreateChildTask(&ChildTaskID,
                                        CS_RECOMP_OS_TASK_NAME,
                                        CS_RecomputeEepromMemoryChildTask,
                                        NULL,
                                        CFE_PLATFORM_ES_DEFAULT_STACK_SIZE,
                                        CS_CHILD_TASK_PRIORITY,
                                        0);
        if (Status == CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(CS_RECOMPUTE_OS_STARTED_DBG_EID,
                              CFE_EVS_EventType_DEBUG,
                              "Recompute of OS code segment started");
            CS_AppData.HkPacket.Payload.CommandCounter++;
        }
        else /* child task creation failed */
        {
            CFE_EVS_SendEvent(CS_RECOMPUTE_OS_CREATE_CHDTASK_ERR_EID,
                              CFE_EVS_EventType_ERROR,
                              "Recompute OS code segment failed, CFE_ES_CreateChildTask returned: 0x%08X",
                              (unsigned int)Status);
            CS_AppData.HkPacket.Payload.CommandErrorCounter++;
            CS_AppData.HkPacket.Payload.RecomputeInProgress = false;
        }
    }
    else
    {
        /*send event that we can't start another task right now */
        CFE_EVS_SendEvent(CS_RECOMPUTE_OS_CHDTASK_ERR_EID,
                          CFE_EVS_EventType_ERROR,
                          "Recompute OS code segment failed: child task in use");
        CS_AppData.HkPacket.Payload.CommandErrorCounter++;
    }

    return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Compute the OneShot checksum command                         */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CFE_Status_t CS_OneShotCmd(const CS_OneShotCmd_t *CmdPtr)
{
    /* command verification variables */
    CFE_ES_TaskId_t ChildTaskID;
    CFE_Status_t    Status;

    /* validate size and address */
    Status = CFE_PSP_MemValidateRange(CmdPtr->Payload.Address, CmdPtr->Payload.Size, CFE_PSP_MEM_ANY);

    if (Status == CFE_SUCCESS)
    {
        if (CS_AppData.HkPacket.Payload.RecomputeInProgress == false
            && CS_AppData.HkPacket.Payload.OneShotInProgress == false)
        {
            /* There is no child task running right now, we can use it*/
            CS_AppData.HkPacket.Payload.RecomputeInProgress = false;
            CS_AppData.HkPacket.Payload.OneShotInProgress   = true;

            CS_AppData.HkPacket.Payload.LastOneShotAddress = CmdPtr->Payload.Address;
            CS_AppData.HkPacket.Payload.LastOneShotSize    = CmdPtr->Payload.Size;
            if (CmdPtr->Payload.MaxBytesPerCycle == 0)
            {
                CS_AppData.HkPacket.Payload.LastOneShotMaxBytesPerCycle = CS_AppData.MaxBytesPerCycle;
            }
            else
            {
                CS_AppData.HkPacket.Payload.LastOneShotMaxBytesPerCycle = CmdPtr->Payload.MaxBytesPerCycle;
            }

            CS_AppData.HkPacket.Payload.LastOneShotChecksum = 0;

            Status = CFE_ES_CreateChildTask(&ChildTaskID,
                                            CS_ONESHOT_TASK_NAME,
                                            CS_OneShotChildTask,
                                            NULL,
                                            CFE_PLATFORM_ES_DEFAULT_STACK_SIZE,
                                            CS_CHILD_TASK_PRIORITY,
                                            0);
            if (Status == CFE_SUCCESS)
            {
                CFE_EVS_SendEvent(CS_ONESHOT_STARTED_DBG_EID,
                                  CFE_EVS_EventType_DEBUG,
                                  "OneShot checksum started on address: 0x%08X, size: %d",
                                  (unsigned int)(CmdPtr->Payload.Address),
                                  (int)(CmdPtr->Payload.Size));

                CS_AppData.ChildTaskID = ChildTaskID;
                CS_AppData.HkPacket.Payload.CommandCounter++;
            }
            else /* child task creation failed */
            {
                CFE_EVS_SendEvent(CS_ONESHOT_CREATE_CHDTASK_ERR_EID,
                                  CFE_EVS_EventType_ERROR,
                                  "OneShot checkum failed, CFE_ES_CreateChildTask returned: 0x%08X",
                                  (unsigned int)Status);

                CS_AppData.HkPacket.Payload.CommandErrorCounter++;
                CS_AppData.HkPacket.Payload.RecomputeInProgress = false;
                CS_AppData.HkPacket.Payload.OneShotInProgress   = false;
            }
        }
        else
        {
            /*send event that we can't start another task right now */
            CFE_EVS_SendEvent(CS_ONESHOT_CHDTASK_ERR_EID,
                              CFE_EVS_EventType_ERROR,
                              "OneShot checksum failed: child task in use");

            CS_AppData.HkPacket.Payload.CommandErrorCounter++;
        }
    } /* end if CFE_PSP_MemValidateRange */
    else
    {
        CFE_EVS_SendEvent(CS_ONESHOT_MEMVALIDATE_ERR_EID,
                          CFE_EVS_EventType_ERROR,
                          "OneShot checksum failed, CFE_PSP_MemValidateRange returned: 0x%08X",
                          (unsigned int)Status);

        CS_AppData.HkPacket.Payload.CommandErrorCounter++;
    }

    return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Cancel the OneShot checksum command                          */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CFE_Status_t CS_CancelOneShotCmd(const CS_CancelOneShotCmd_t *CmdPtr)
{
    /* command verification variables */
    CFE_Status_t Status;

    /* Make sure there is a OneShot command in use */
    if (CS_AppData.HkPacket.Payload.RecomputeInProgress == false
        && CS_AppData.HkPacket.Payload.OneShotInProgress == true)
    {
        Status = CFE_ES_DeleteChildTask(CS_AppData.ChildTaskID);

        if (Status == CFE_SUCCESS)
        {
            CS_AppData.ChildTaskID                          = CFE_ES_TASKID_UNDEFINED;
            CS_AppData.HkPacket.Payload.RecomputeInProgress = false;
            CS_AppData.HkPacket.Payload.OneShotInProgress   = false;
            CS_AppData.HkPacket.Payload.CommandCounter++;
            CFE_EVS_SendEvent(CS_ONESHOT_CANCELLED_INF_EID,
                              CFE_EVS_EventType_INFORMATION,
                              "OneShot checksum calculation has been cancelled");
        }
        else
        {
            CFE_EVS_SendEvent(CS_ONESHOT_CANCEL_DELETE_CHDTASK_ERR_EID,
                              CFE_EVS_EventType_ERROR,
                              "Cancel OneShot checksum failed, CFE_ES_DeleteChildTask returned:  0x%08X",
                              (unsigned int)Status);
            CS_AppData.HkPacket.Payload.CommandErrorCounter++;
        }
    }
    else
    {
        CFE_EVS_SendEvent(CS_ONESHOT_CANCEL_NO_CHDTASK_ERR_EID,
                          CFE_EVS_EventType_ERROR,
                          "Cancel OneShot checksum failed. No OneShot active");
        CS_AppData.HkPacket.Payload.CommandErrorCounter++;
    }

    return CFE_SUCCESS;
}
