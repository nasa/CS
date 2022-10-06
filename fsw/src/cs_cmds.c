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
#include "cs_events.h"
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
/* CS no operation command                                         */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_NoopCmd(const CS_NoArgsCmd_t *CmdPtr)
{
    /* command verification variables */
    size_t ExpectedLength = sizeof(CS_NoArgsCmd_t);

    /* Verify command packet length */
    if (CS_VerifyCmdLength(&CmdPtr->CmdHeader.Msg, ExpectedLength))
    {
        CS_AppData.HkPacket.CmdCounter++;

        CFE_EVS_SendEvent(CS_NOOP_INF_EID, CFE_EVS_EventType_INFORMATION, "No-op command. Version %d.%d.%d.%d",
                          CS_MAJOR_VERSION, CS_MINOR_VERSION, CS_REVISION, CS_MISSION_REV);
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Reset Application counters command                           */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_ResetCmd(const CS_NoArgsCmd_t *CmdPtr)
{
    /* command verification variables */
    size_t ExpectedLength = sizeof(CS_NoArgsCmd_t);

    /* Verify command packet length */
    if (CS_VerifyCmdLength(&CmdPtr->CmdHeader.Msg, ExpectedLength))
    {
        CS_AppData.HkPacket.CmdCounter    = 0;
        CS_AppData.HkPacket.CmdErrCounter = 0;

        CS_AppData.HkPacket.EepromCSErrCounter  = 0;
        CS_AppData.HkPacket.MemoryCSErrCounter  = 0;
        CS_AppData.HkPacket.TablesCSErrCounter  = 0;
        CS_AppData.HkPacket.AppCSErrCounter     = 0;
        CS_AppData.HkPacket.CfeCoreCSErrCounter = 0;
        CS_AppData.HkPacket.OSCSErrCounter      = 0;
        CS_AppData.HkPacket.PassCounter         = 0;

        CFE_EVS_SendEvent(CS_RESET_DBG_EID, CFE_EVS_EventType_DEBUG, "Reset Counters command recieved");
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS's background checksumming cycle                              */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_BackgroundCheckCycle(const CS_NoArgsCmd_t *CmdPtr)
{
    /* command verification variables */
    size_t            ExpectedLength = sizeof(CS_NoArgsCmd_t);
    bool              DoneWithCycle  = false;
    bool              EndOfList      = false;
    CFE_SB_MsgId_t    MessageID      = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_FcnCode_t CommandCode    = 0;
    size_t            ActualLength   = 0;

    CFE_MSG_GetSize(&CmdPtr->CmdHeader.Msg, &ActualLength);

    /* Verify the command packet length */
    if (ExpectedLength != ActualLength)
    {
        CFE_MSG_GetMsgId(&CmdPtr->CmdHeader.Msg, &MessageID);
        CFE_MSG_GetFcnCode(&CmdPtr->CmdHeader.Msg, &CommandCode);

        CFE_EVS_SendEvent(CS_LEN_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Invalid msg length: ID = 0x%08lX, CC = %d, Len = %lu, Expected = %lu",
                          (unsigned long)CFE_SB_MsgIdToValue(MessageID), CommandCode, (unsigned long)ActualLength,
                          (unsigned long)ExpectedLength);
    }
    else
    {
        if (CS_AppData.HkPacket.ChecksumState == CS_STATE_ENABLED)
        {
            DoneWithCycle = false;
            EndOfList     = false;

            /* Skip this background cycle if there's a recompute or one shot in
             * progress */
            if (CS_AppData.HkPacket.RecomputeInProgress == true || CS_AppData.HkPacket.OneShotInProgress == true)
            {
                CFE_EVS_SendEvent(CS_BKGND_COMPUTE_PROG_INF_EID, CFE_EVS_EventType_INFORMATION,
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
                if (CS_AppData.HkPacket.CurrentCSTable >= (CS_NUM_TABLES - 1))
                {
                    EndOfList = true;
                }

                /* Call the appropriate background function based on the current table
                   value.  The value is updated inside each function */
                switch (CS_AppData.HkPacket.CurrentCSTable)
                {
                    case (CS_CFECORE):
                        DoneWithCycle = CS_BackgroundCfeCore();
                        break;

                    case (CS_OSCORE):

                        DoneWithCycle = CS_BackgroundOS();
                        break;

                    case (CS_EEPROM_TABLE):
                        DoneWithCycle = CS_BackgroundEeprom();
                        break;

                    case (CS_MEMORY_TABLE):
                        DoneWithCycle = CS_BackgroundMemory();
                        break;

                    case (CS_TABLES_TABLE):
                        DoneWithCycle = CS_BackgroundTables();
                        break;

                    case (CS_APP_TABLE):

                        DoneWithCycle = CS_BackgroundApp();
                        break;

                        /* default case in case CS_AppData.HkPacket.CurrentCSTable is some random bad value */
                    default:

                        /* We are at the end of the line */
                        CS_AppData.HkPacket.CurrentCSTable      = 0;
                        CS_AppData.HkPacket.CurrentEntryInTable = 0;
                        CS_AppData.HkPacket.PassCounter++;
                        DoneWithCycle = true;
                        break;

                } /* end switch */
            }     /* end while */
        }
        else
        {
            /* CS is disabled, Application-wide */
        }
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Disable all background checksumming command                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_DisableAllCSCmd(const CS_NoArgsCmd_t *CmdPtr)
{
    /* command verification variables */
    size_t ExpectedLength = sizeof(CS_NoArgsCmd_t);

    /* Verify command packet length */
    if (CS_VerifyCmdLength(&CmdPtr->CmdHeader.Msg, ExpectedLength))
    {
        CS_AppData.HkPacket.ChecksumState = CS_STATE_DISABLED;

        /* zero out the temp values in all the tables
         so that when the checksums are re-enabled,
         they don't start with a half-old value */
        CS_ZeroEepromTempValues();
        CS_ZeroMemoryTempValues();
        CS_ZeroTablesTempValues();
        CS_ZeroAppTempValues();
        CS_ZeroCfeCoreTempValues();
        CS_ZeroOSTempValues();

        CS_AppData.HkPacket.CmdCounter++;

        CFE_EVS_SendEvent(CS_DISABLE_ALL_INF_EID, CFE_EVS_EventType_INFORMATION, "Background Checksumming Disabled");
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Enable all background checksumming command                   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_EnableAllCSCmd(const CS_NoArgsCmd_t *CmdPtr)
{
    /* command verification variables */
    size_t ExpectedLength = sizeof(CS_NoArgsCmd_t);

    /* Verify command packet length */
    if (CS_VerifyCmdLength(&CmdPtr->CmdHeader.Msg, ExpectedLength))
    {
        CS_AppData.HkPacket.ChecksumState = CS_STATE_ENABLED;

        CS_AppData.HkPacket.CmdCounter++;

        CFE_EVS_SendEvent(CS_ENABLE_ALL_INF_EID, CFE_EVS_EventType_INFORMATION, "Background Checksumming Enabled");
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Disable background checking of the cFE core command          */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_DisableCfeCoreCmd(const CS_NoArgsCmd_t *CmdPtr)
{
    /* command verification variables */
    size_t ExpectedLength = sizeof(CS_NoArgsCmd_t);

    /* Verify command packet length */
    if (CS_VerifyCmdLength(&CmdPtr->CmdHeader.Msg, ExpectedLength))
    {
        CS_AppData.HkPacket.CfeCoreCSState = CS_STATE_DISABLED;
        CS_ZeroCfeCoreTempValues();

#if (CS_PRESERVE_STATES_ON_PROCESSOR_RESET == true)
        CS_UpdateCDS();
#endif

        CFE_EVS_SendEvent(CS_DISABLE_CFECORE_INF_EID, CFE_EVS_EventType_INFORMATION,
                          "Checksumming of cFE Core is Disabled");

        CS_AppData.HkPacket.CmdCounter++;
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Enable background checking of the cFE core command           */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_EnableCfeCoreCmd(const CS_NoArgsCmd_t *CmdPtr)
{
    /* command verification variables */
    size_t ExpectedLength = sizeof(CS_NoArgsCmd_t);

    /* Verify command packet length */
    if (CS_VerifyCmdLength(&CmdPtr->CmdHeader.Msg, ExpectedLength))
    {
        CS_AppData.HkPacket.CfeCoreCSState = CS_STATE_ENABLED;

#if (CS_PRESERVE_STATES_ON_PROCESSOR_RESET == true)
        CS_UpdateCDS();
#endif

        CFE_EVS_SendEvent(CS_ENABLE_CFECORE_INF_EID, CFE_EVS_EventType_INFORMATION,
                          "Checksumming of cFE Core is Enabled");

        CS_AppData.HkPacket.CmdCounter++;
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Disable background checking of the OS code segment command   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_DisableOSCmd(const CS_NoArgsCmd_t *CmdPtr)
{
    /* command verification variables */
    size_t ExpectedLength = sizeof(CS_NoArgsCmd_t);

    /* Verify command packet length */
    if (CS_VerifyCmdLength(&CmdPtr->CmdHeader.Msg, ExpectedLength))
    {
        CS_AppData.HkPacket.OSCSState = CS_STATE_DISABLED;
        CS_ZeroOSTempValues();

#if (CS_PRESERVE_STATES_ON_PROCESSOR_RESET == true)
        CS_UpdateCDS();
#endif

        CFE_EVS_SendEvent(CS_DISABLE_OS_INF_EID, CFE_EVS_EventType_INFORMATION,
                          "Checksumming of OS code segment is Disabled");

        CS_AppData.HkPacket.CmdCounter++;
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Enable background checking of the OS code segment command    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_EnableOSCmd(const CS_NoArgsCmd_t *CmdPtr)
{
    /* command verification variables */
    size_t ExpectedLength = sizeof(CS_NoArgsCmd_t);

    /* Verify command packet length */
    if (CS_VerifyCmdLength(&CmdPtr->CmdHeader.Msg, ExpectedLength))
    {
        CS_AppData.HkPacket.OSCSState = CS_STATE_ENABLED;

#if (CS_PRESERVE_STATES_ON_PROCESSOR_RESET == true)
        CS_UpdateCDS();
#endif

        CFE_EVS_SendEvent(CS_ENABLE_OS_INF_EID, CFE_EVS_EventType_INFORMATION,
                          "Checksumming of OS code segment is Enabled");

        CS_AppData.HkPacket.CmdCounter++;
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Report the baseline checksum for the cFE core command        */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_ReportBaselineCfeCoreCmd(const CS_NoArgsCmd_t *CmdPtr)
{
    /* command verification variables */
    size_t ExpectedLength = sizeof(CS_NoArgsCmd_t);

    /* Verify command packet length */
    if (CS_VerifyCmdLength(&CmdPtr->CmdHeader.Msg, ExpectedLength))
    {
        if (CS_AppData.CfeCoreCodeSeg.ComputedYet == true)
        {
            CFE_EVS_SendEvent(CS_BASELINE_CFECORE_INF_EID, CFE_EVS_EventType_INFORMATION,
                              "Baseline of cFE Core is 0x%08X",
                              (unsigned int)CS_AppData.CfeCoreCodeSeg.ComparisonValue);
        }
        else
        {
            CFE_EVS_SendEvent(CS_NO_BASELINE_CFECORE_INF_EID, CFE_EVS_EventType_INFORMATION,
                              "Baseline of cFE Core has not been computed yet");
        }
        CS_AppData.HkPacket.CmdCounter++;
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Report the baseline checksum for the OS code segment command */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_ReportBaselineOSCmd(const CS_NoArgsCmd_t *CmdPtr)
{
    /* command verification variables */
    size_t ExpectedLength = sizeof(CS_NoArgsCmd_t);

    /* Verify command packet length */
    if (CS_VerifyCmdLength(&CmdPtr->CmdHeader.Msg, ExpectedLength))
    {
        if (CS_AppData.OSCodeSeg.ComputedYet == true)
        {
            CFE_EVS_SendEvent(CS_BASELINE_OS_INF_EID, CFE_EVS_EventType_INFORMATION,
                              "Baseline of OS code segment is 0x%08X",
                              (unsigned int)CS_AppData.OSCodeSeg.ComparisonValue);
        }
        else
        {
            CFE_EVS_SendEvent(CS_NO_BASELINE_OS_INF_EID, CFE_EVS_EventType_INFORMATION,
                              "Baseline of OS code segment has not been computed yet");
        }
        CS_AppData.HkPacket.CmdCounter++;
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Recompute the baseline checksum for the cFE core command     */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_RecomputeBaselineCfeCoreCmd(const CS_NoArgsCmd_t *CmdPtr)
{
    /* command verification variables */
    size_t          ExpectedLength = sizeof(CS_NoArgsCmd_t);
    CFE_ES_TaskId_t ChildTaskID;
    int32           Status;

    /* Verify command packet length... */
    if (CS_VerifyCmdLength(&CmdPtr->CmdHeader.Msg, ExpectedLength))
    {
        if (CS_AppData.HkPacket.RecomputeInProgress == false && CS_AppData.HkPacket.OneShotInProgress == false)
        {
            /* There is no child task running right now, we can use it*/
            CS_AppData.HkPacket.RecomputeInProgress = true;

            /* fill in child task variables */
            CS_AppData.ChildTaskTable                = CS_CFECORE;
            CS_AppData.ChildTaskEntryID              = 0;
            CS_AppData.RecomputeEepromMemoryEntryPtr = &CS_AppData.CfeCoreCodeSeg;

            Status =
                CFE_ES_CreateChildTask(&ChildTaskID, CS_RECOMP_CFECORE_TASK_NAME, CS_RecomputeEepromMemoryChildTask,
                                       NULL, CFE_PLATFORM_ES_DEFAULT_STACK_SIZE, CS_CHILD_TASK_PRIORITY, 0);

            if (Status == CFE_SUCCESS)
            {
                CFE_EVS_SendEvent(CS_RECOMPUTE_CFECORE_STARTED_DBG_EID, CFE_EVS_EventType_DEBUG,
                                  "Recompute of cFE core started");
                CS_AppData.HkPacket.CmdCounter++;
            }
            else /* child task creation failed */
            {
                CFE_EVS_SendEvent(CS_RECOMPUTE_CFECORE_CREATE_CHDTASK_ERR_EID, CFE_EVS_EventType_ERROR,
                                  "Recompute cFE core failed, CFE_ES_CreateChildTask returned: 0x%08X",
                                  (unsigned int)Status);
                CS_AppData.HkPacket.CmdErrCounter++;
                CS_AppData.HkPacket.RecomputeInProgress = false;
            }
        }
        else
        {
            /*send event that we can't start another task right now */
            CFE_EVS_SendEvent(CS_RECOMPUTE_CFECORE_CHDTASK_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Recompute cFE core failed: child task in use");
            CS_AppData.HkPacket.CmdErrCounter++;
        }
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Recompute the baseline checksum for the OS code seg command  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_RecomputeBaselineOSCmd(const CS_NoArgsCmd_t *CmdPtr)
{
    /* command verification variables */
    size_t          ExpectedLength = sizeof(CS_NoArgsCmd_t);
    CFE_ES_TaskId_t ChildTaskID;
    int32           Status;

    /* Verify command packet length... */
    if (CS_VerifyCmdLength(&CmdPtr->CmdHeader.Msg, ExpectedLength))
    {
        if (CS_AppData.HkPacket.RecomputeInProgress == false && CS_AppData.HkPacket.OneShotInProgress == false)
        {
            /* There is no child task running right now, we can use it*/
            CS_AppData.HkPacket.RecomputeInProgress = true;

            /* fill in child task variables */
            CS_AppData.ChildTaskTable                = CS_OSCORE;
            CS_AppData.ChildTaskEntryID              = 0;
            CS_AppData.RecomputeEepromMemoryEntryPtr = &CS_AppData.OSCodeSeg;

            Status = CFE_ES_CreateChildTask(&ChildTaskID, CS_RECOMP_OS_TASK_NAME, CS_RecomputeEepromMemoryChildTask,
                                            NULL, CFE_PLATFORM_ES_DEFAULT_STACK_SIZE, CS_CHILD_TASK_PRIORITY, 0);
            if (Status == CFE_SUCCESS)
            {
                CFE_EVS_SendEvent(CS_RECOMPUTE_OS_STARTED_DBG_EID, CFE_EVS_EventType_DEBUG,
                                  "Recompute of OS code segment started");
                CS_AppData.HkPacket.CmdCounter++;
            }
            else /* child task creation failed */
            {
                CFE_EVS_SendEvent(CS_RECOMPUTE_OS_CREATE_CHDTASK_ERR_EID, CFE_EVS_EventType_ERROR,
                                  "Recompute OS code segment failed, CFE_ES_CreateChildTask returned: 0x%08X",
                                  (unsigned int)Status);
                CS_AppData.HkPacket.CmdErrCounter++;
                CS_AppData.HkPacket.RecomputeInProgress = false;
            }
        }
        else
        {
            /*send event that we can't start another task right now */
            CFE_EVS_SendEvent(CS_RECOMPUTE_OS_CHDTASK_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Recompute OS code segment failed: child task in use");
            CS_AppData.HkPacket.CmdErrCounter++;
        }
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Compute the OneShot checksum command                         */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_OneShotCmd(const CS_OneShotCmd_t *CmdPtr)
{
    /* command verification variables */
    size_t          ExpectedLength = sizeof(CS_OneShotCmd_t);
    CFE_ES_TaskId_t ChildTaskID;
    int32           Status;

    /* Verify command packet length... */
    if (CS_VerifyCmdLength(&CmdPtr->CmdHeader.Msg, ExpectedLength))
    {
        /* validate size and address */
        Status = CFE_PSP_MemValidateRange(CmdPtr->Address, CmdPtr->Size, CFE_PSP_MEM_ANY);

        if (Status == CFE_SUCCESS)
        {
            if (CS_AppData.HkPacket.RecomputeInProgress == false && CS_AppData.HkPacket.OneShotInProgress == false)
            {
                /* There is no child task running right now, we can use it*/
                CS_AppData.HkPacket.RecomputeInProgress = false;
                CS_AppData.HkPacket.OneShotInProgress   = true;

                CS_AppData.HkPacket.LastOneShotAddress = CmdPtr->Address;
                CS_AppData.HkPacket.LastOneShotSize    = CmdPtr->Size;
                if (CmdPtr->MaxBytesPerCycle == 0)
                {
                    CS_AppData.HkPacket.LastOneShotMaxBytesPerCycle = CS_AppData.MaxBytesPerCycle;
                }
                else
                {
                    CS_AppData.HkPacket.LastOneShotMaxBytesPerCycle = CmdPtr->MaxBytesPerCycle;
                }

                CS_AppData.HkPacket.LastOneShotChecksum = 0;

                Status = CFE_ES_CreateChildTask(&ChildTaskID, CS_ONESHOT_TASK_NAME, CS_OneShotChildTask, NULL,
                                                CFE_PLATFORM_ES_DEFAULT_STACK_SIZE, CS_CHILD_TASK_PRIORITY, 0);
                if (Status == CFE_SUCCESS)
                {
                    CFE_EVS_SendEvent(CS_ONESHOT_STARTED_DBG_EID, CFE_EVS_EventType_DEBUG,
                                      "OneShot checksum started on address: 0x%08X, size: %d",
                                      (unsigned int)(CmdPtr->Address), (int)(CmdPtr->Size));

                    CS_AppData.ChildTaskID = ChildTaskID;
                    CS_AppData.HkPacket.CmdCounter++;
                }
                else /* child task creation failed */
                {
                    CFE_EVS_SendEvent(CS_ONESHOT_CREATE_CHDTASK_ERR_EID, CFE_EVS_EventType_ERROR,
                                      "OneShot checkum failed, CFE_ES_CreateChildTask returned: 0x%08X",
                                      (unsigned int)Status);

                    CS_AppData.HkPacket.CmdErrCounter++;
                    CS_AppData.HkPacket.RecomputeInProgress = false;
                    CS_AppData.HkPacket.OneShotInProgress   = false;
                }
            }
            else
            {
                /*send event that we can't start another task right now */
                CFE_EVS_SendEvent(CS_ONESHOT_CHDTASK_ERR_EID, CFE_EVS_EventType_ERROR,
                                  "OneShot checksum failed: child task in use");

                CS_AppData.HkPacket.CmdErrCounter++;
            }
        } /* end if CFE_PSP_MemValidateRange */
        else
        {
            CFE_EVS_SendEvent(CS_ONESHOT_MEMVALIDATE_ERR_EID, CFE_EVS_EventType_ERROR,
                              "OneShot checksum failed, CFE_PSP_MemValidateRange returned: 0x%08X",
                              (unsigned int)Status);

            CS_AppData.HkPacket.CmdErrCounter++;
        }
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Cancel the OneShot checksum command                          */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_CancelOneShotCmd(const CS_NoArgsCmd_t *CmdPtr)
{
    /* command verification variables */
    size_t ExpectedLength = sizeof(CS_NoArgsCmd_t);
    int32  Status;

    /* Verify command packet length... */
    if (CS_VerifyCmdLength(&CmdPtr->CmdHeader.Msg, ExpectedLength))
    {
        /* Make sure there is a OneShot command in use */
        if (CS_AppData.HkPacket.RecomputeInProgress == false && CS_AppData.HkPacket.OneShotInProgress == true)
        {
            Status = CFE_ES_DeleteChildTask(CS_AppData.ChildTaskID);

            if (Status == CFE_SUCCESS)
            {
                CS_AppData.ChildTaskID                  = CFE_ES_TASKID_UNDEFINED;
                CS_AppData.HkPacket.RecomputeInProgress = false;
                CS_AppData.HkPacket.OneShotInProgress   = false;
                CS_AppData.HkPacket.CmdCounter++;
                CFE_EVS_SendEvent(CS_ONESHOT_CANCELLED_INF_EID, CFE_EVS_EventType_INFORMATION,
                                  "OneShot checksum calculation has been cancelled");
            }
            else
            {
                CFE_EVS_SendEvent(CS_ONESHOT_CANCEL_DELETE_CHDTASK_ERR_EID, CFE_EVS_EventType_ERROR,
                                  "Cancel OneShot checksum failed, CFE_ES_DeleteChildTask returned:  0x%08X",
                                  (unsigned int)Status);
                CS_AppData.HkPacket.CmdErrCounter++;
            }
        }
        else
        {
            CFE_EVS_SendEvent(CS_ONESHOT_CANCEL_NO_CHDTASK_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Cancel OneShot checksum failed. No OneShot active");
            CS_AppData.HkPacket.CmdErrCounter++;
        }
    }
}