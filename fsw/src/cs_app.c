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
 *   CFS Checksum (CS) Application provides the service of background
 *   checksumming user-defined objects in the CFS
 */
#include <string.h>
#include "cfe.h"
#include "cs_app.h"

#include "cs_platform_cfg.h"
#include "cs_events.h"
#include "cs_utils.h"
#include "cs_compute.h"
#include "cs_eeprom_cmds.h"
#include "cs_table_cmds.h"
#include "cs_memory_cmds.h"
#include "cs_app_cmds.h"
#include "cs_cmds.h"
#include "cs_init.h"

/*************************************************************************
**
** Macro definitions
**
**************************************************************************/
#define CS_NUM_DATA_STORE_STATES 6 /* 4 tables + OS CS + cFE core number of checksum states for CDS */

/*************************************************************************
**
** CS global application data
**
**************************************************************************/
CS_AppData_t CS_AppData;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS application entry point and main process loop                */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_AppMain(void)
{
    CFE_Status_t     Result;
    CFE_SB_Buffer_t *BufPtr = NULL;

    /* Performance Log (start time counter) */
    CFE_ES_PerfLogEntry(CS_APPMAIN_PERF_ID);

    /* Perform application-specific initialization */
    Result = CS_AppInit();

    /* Check for start-up error */
    if (Result != CFE_SUCCESS)
    {
        /* Set request to terminate main loop */
        CS_AppData.RunStatus = CFE_ES_RunStatus_APP_ERROR;
    }

    CFE_ES_WaitForStartupSync(CS_STARTUP_TIMEOUT);

    /* Main process loop */
    while (CFE_ES_RunLoop(&CS_AppData.RunStatus))
    {
        /* Performance Log (stop time counter) */
        CFE_ES_PerfLogExit(CS_APPMAIN_PERF_ID);

        /* Wait for the next Software Bus message */
        Result = CFE_SB_ReceiveBuffer(&BufPtr, CS_AppData.CmdPipe, CS_WAKEUP_TIMEOUT);

        /* Performance Log (start time counter)  */
        CFE_ES_PerfLogEntry(CS_APPMAIN_PERF_ID);

        if ((Result == CFE_SUCCESS) && (BufPtr != NULL))
        {
            /* Process Software Bus message */
            Result = CS_AppPipe(BufPtr);
        }
        else if ((Result == CFE_SB_TIME_OUT) || (Result == CFE_SB_NO_MESSAGE))
        {
            Result = CS_HandleRoutineTableUpdates();
        }
        else
        {
            /* All other cases are caught by the following condition
               Result != CFE_SUCCESS */
        }

        /*
         ** Note: If there were some reason to exit the task
         **       normally (without error) then we would set
         **       RunStatus = CFE_ES_APP_EXIT
         */
        if (Result != CFE_SUCCESS)
        {
            /* Set request to terminate main loop */
            CS_AppData.RunStatus = CFE_ES_RunStatus_APP_ERROR;
        }
    } /* end run loop */

    /* Check for "fatal" process error */
    if (CS_AppData.RunStatus == CFE_ES_RunStatus_APP_ERROR || CS_AppData.RunStatus == CFE_ES_RunStatus_SYS_EXCEPTION)
    {
        /* Send an error event with run status and result */
        CFE_EVS_SendEvent(CS_EXIT_ERR_EID, CFE_EVS_EventType_ERROR, "App terminating, RunStatus:0x%08X, RC:0x%08X",
                          (unsigned int)CS_AppData.RunStatus, (unsigned int)Result);
    }
    else
    {
        /* Send an informational event describing the reason for the termination */
        CFE_EVS_SendEvent(CS_EXIT_INF_EID, CFE_EVS_EventType_INFORMATION, "App terminating, RunStatus:0x%08X",
                          (unsigned int)CS_AppData.RunStatus);
    }

    /* In case cFE Event Services is not working */
    CFE_ES_WriteToSysLog("CS App terminating, RunStatus:0x%08X, RC:0x%08X\n", (unsigned int)CS_AppData.RunStatus,
                         (unsigned int)Result);

    /* Performance Log (stop time counter) */
    CFE_ES_PerfLogExit(CS_APPMAIN_PERF_ID);

    /* Let cFE kill the task (and child task) */
    CFE_ES_ExitApp(CS_AppData.RunStatus);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Application initialization function                          */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CFE_Status_t CS_AppInit(void)
{
    CFE_Status_t Result;

    /* Register for event services */
    Result = CFE_EVS_Register(NULL, 0, 0);

    if (Result != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("CS App: Error Registering For Event Services, RC = 0x%08X\n", (unsigned int)Result);
    }
    else
    {
        /* Zero out all data in CS_AppData, including the housekeeping data*/
        memset(&CS_AppData, 0, sizeof(CS_AppData));

        CS_AppData.RunStatus = CFE_ES_RunStatus_APP_RUN;

        Result = CS_SbInit();
    }

    if (Result == CFE_SUCCESS)
    {
        /* Set up default tables in memory */
        CS_InitializeDefaultTables();

        CS_AppData.HkPacket.Payload.EepromCSState = CS_EEPROM_TBL_POWERON_STATE;
        CS_AppData.HkPacket.Payload.MemoryCSState = CS_MEMORY_TBL_POWERON_STATE;
        CS_AppData.HkPacket.Payload.AppCSState    = CS_APPS_TBL_POWERON_STATE;
        CS_AppData.HkPacket.Payload.TablesCSState = CS_TABLES_TBL_POWERON_STATE;

        CS_AppData.HkPacket.Payload.OSCSState      = CS_OSCS_CHECKSUM_STATE;
        CS_AppData.HkPacket.Payload.CfeCoreCSState = CS_CFECORE_CHECKSUM_STATE;

#if (CS_PRESERVE_STATES_ON_PROCESSOR_RESET == true)
        Result = CS_CreateRestoreStatesFromCDS();
#endif
    }

    if (Result == CFE_SUCCESS)
    {
        Result = CS_InitAllTables();
    }

    if (Result == CFE_SUCCESS)
    {
        CS_InitSegments();

        /* initialize the place to ostart background checksumming */
        CS_AppData.HkPacket.Payload.CurrentCSTable      = 0;
        CS_AppData.HkPacket.Payload.CurrentEntryInTable = 0;

        /* Initial settings for the CS Application */
        /* the rest of the tables are initialized in CS_TableInit */
        CS_AppData.HkPacket.Payload.ChecksumState = CS_STATE_ENABLED;

        CS_AppData.HkPacket.Payload.RecomputeInProgress = false;
        CS_AppData.HkPacket.Payload.OneShotInProgress   = false;

        CS_AppData.MaxBytesPerCycle = CS_DEFAULT_BYTES_PER_CYCLE;

        /* Application startup event message */
        Result =
            CFE_EVS_SendEvent(CS_INIT_INF_EID, CFE_EVS_EventType_INFORMATION, "CS Initialized. Version %d.%d.%d.%d",
                              CS_MAJOR_VERSION, CS_MINOR_VERSION, CS_REVISION, CS_MISSION_REV);
    }
    return Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS's command pipe processing                                    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CFE_Status_t CS_AppPipe(const CFE_SB_Buffer_t *BufPtr)
{
    CFE_SB_MsgId_t MessageID = CFE_SB_INVALID_MSG_ID;
    CFE_Status_t   Result    = CFE_SUCCESS;

    CFE_MSG_GetMsgId(&BufPtr->Msg, &MessageID);

    switch (CFE_SB_MsgIdToValue(MessageID))
    {
            /* Housekeeping telemetry request */
        case CS_SEND_HK_MID:
            CS_HousekeepingCmd((CS_NoArgsCmd_t *)BufPtr);

            /* update each table if there is no recompute happening on that table */
            Result = CS_HandleRoutineTableUpdates();

            break;

        case CS_BACKGROUND_CYCLE_MID:
            CS_BackgroundCheckCycle((CS_NoArgsCmd_t *)BufPtr);
            break;
        /* All CS Commands */
        case CS_CMD_MID:
            CS_ProcessCmd(BufPtr);
            break;

        default:
            CFE_EVS_SendEvent(CS_MID_ERR_EID, CFE_EVS_EventType_ERROR, "Invalid command pipe message ID: 0x%08lX",
                              (unsigned long)CFE_SB_MsgIdToValue(MessageID));

            CS_AppData.HkPacket.Payload.CmdErrCounter++;
            break;
    }

    return Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS application -- command packet processor                      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CS_ProcessCmd(const CFE_SB_Buffer_t *BufPtr)
{
    CFE_SB_MsgId_t    MessageID   = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_FcnCode_t CommandCode = 0;

    CFE_MSG_GetMsgId(&BufPtr->Msg, &MessageID);

    CFE_MSG_GetFcnCode(&BufPtr->Msg, &CommandCode);

    switch (CommandCode)
    {
        /*  All CS Commands */
        case CS_NOOP_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_NoArgsCmd_t)))
            {
                CS_NoopCmd((CS_NoArgsCmd_t *)BufPtr);
            }
            break;

        case CS_RESET_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_NoArgsCmd_t)))
            {
                CS_ResetCmd((CS_NoArgsCmd_t *)BufPtr);
            }
            break;

        case CS_ONE_SHOT_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_OneShotCmd_t)))
            {
                CS_OneShotCmd((CS_OneShotCmd_t *)BufPtr);
            }
            break;

        case CS_CANCEL_ONE_SHOT_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_NoArgsCmd_t)))
            {
                CS_CancelOneShotCmd((CS_NoArgsCmd_t *)BufPtr);
            }
            break;

        case CS_ENABLE_ALL_CS_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_NoArgsCmd_t)))
            {
                CS_EnableAllCSCmd((CS_NoArgsCmd_t *)BufPtr);
            }
            break;

        case CS_DISABLE_ALL_CS_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_NoArgsCmd_t)))
            {
                CS_DisableAllCSCmd((CS_NoArgsCmd_t *)BufPtr);
            }
            break;

        /* cFE core Commands */
        case CS_ENABLE_CFE_CORE_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_NoArgsCmd_t)))
            {
                CS_EnableCfeCoreCmd((CS_NoArgsCmd_t *)BufPtr);
            }
            break;

        case CS_DISABLE_CFE_CORE_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_NoArgsCmd_t)))
            {
                CS_DisableCfeCoreCmd((CS_NoArgsCmd_t *)BufPtr);
            }
            break;

        case CS_REPORT_BASELINE_CFE_CORE_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_NoArgsCmd_t)))
            {
                CS_ReportBaselineCfeCoreCmd((CS_NoArgsCmd_t *)BufPtr);
            }
            break;

        case CS_RECOMPUTE_BASELINE_CFE_CORE_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_NoArgsCmd_t)))
            {
                CS_RecomputeBaselineCfeCoreCmd((CS_NoArgsCmd_t *)BufPtr);
            }
            break;

        /* OS Commands*/
        case CS_ENABLE_OS_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_NoArgsCmd_t)))
            {
                CS_EnableOSCmd((CS_NoArgsCmd_t *)BufPtr);
            }
            break;

        case CS_DISABLE_OS_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_NoArgsCmd_t)))
            {
                CS_DisableOSCmd((CS_NoArgsCmd_t *)BufPtr);
            }
            break;

        case CS_REPORT_BASELINE_OS_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_NoArgsCmd_t)))
            {
                CS_ReportBaselineOSCmd((CS_NoArgsCmd_t *)BufPtr);
            }
            break;

        case CS_RECOMPUTE_BASELINE_OS_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_NoArgsCmd_t)))
            {
                CS_RecomputeBaselineOSCmd((CS_NoArgsCmd_t *)BufPtr);
            }
            break;

        /* EEPROM Commands */
        case CS_ENABLE_EEPROM_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_NoArgsCmd_t)))
            {
                CS_EnableEepromCmd((CS_NoArgsCmd_t *)BufPtr);
            }
            break;

        case CS_DISABLE_EEPROM_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_NoArgsCmd_t)))
            {
                CS_DisableEepromCmd((CS_NoArgsCmd_t *)BufPtr);
            }
            break;

        case CS_REPORT_BASELINE_EEPROM_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_EntryCmd_t)))
            {
                CS_ReportBaselineEntryIDEepromCmd((CS_EntryCmd_t *)BufPtr);
            }
            break;

        case CS_RECOMPUTE_BASELINE_EEPROM_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_EntryCmd_t)))
            {
                CS_RecomputeBaselineEepromCmd((CS_EntryCmd_t *)BufPtr);
            }
            break;

        case CS_ENABLE_ENTRY_EEPROM_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_EntryCmd_t)))
            {
                CS_EnableEntryIDEepromCmd((CS_EntryCmd_t *)BufPtr);
            }
            break;

        case CS_DISABLE_ENTRY_EEPROM_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_EntryCmd_t)))
            {
                CS_DisableEntryIDEepromCmd((CS_EntryCmd_t *)BufPtr);
            }
            break;

        case CS_GET_ENTRY_ID_EEPROM_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_GetEntryIDCmd_t)))
            {
                CS_GetEntryIDEepromCmd((CS_GetEntryIDCmd_t *)BufPtr);
            }
            break;

        /*  Memory Commands */
        case CS_ENABLE_MEMORY_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_NoArgsCmd_t)))
            {
                CS_EnableMemoryCmd((CS_NoArgsCmd_t *)BufPtr);
            }
            break;

        case CS_DISABLE_MEMORY_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_NoArgsCmd_t)))
            {
                CS_DisableMemoryCmd((CS_NoArgsCmd_t *)BufPtr);
            }
            break;

        case CS_REPORT_BASELINE_MEMORY_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_EntryCmd_t)))
            {
                CS_ReportBaselineEntryIDMemoryCmd((CS_EntryCmd_t *)BufPtr);
            }
            break;

        case CS_RECOMPUTE_BASELINE_MEMORY_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_EntryCmd_t)))
            {
                CS_RecomputeBaselineMemoryCmd((CS_EntryCmd_t *)BufPtr);
            }
            break;

        case CS_ENABLE_ENTRY_MEMORY_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_EntryCmd_t)))
            {
                CS_EnableEntryIDMemoryCmd((CS_EntryCmd_t *)BufPtr);
            }
            break;

        case CS_DISABLE_ENTRY_MEMORY_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_EntryCmd_t)))
            {
                CS_DisableEntryIDMemoryCmd((CS_EntryCmd_t *)BufPtr);
            }
            break;

        case CS_GET_ENTRY_ID_MEMORY_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_GetEntryIDCmd_t)))
            {
                CS_GetEntryIDMemoryCmd((CS_GetEntryIDCmd_t *)BufPtr);
            }
            break;

        /* Tables Commands */
        case CS_ENABLE_TABLES_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_NoArgsCmd_t)))
            {
                CS_EnableTablesCmd((CS_NoArgsCmd_t *)BufPtr);
            }
            break;

        case CS_DISABLE_TABLES_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_NoArgsCmd_t)))
            {
                CS_DisableTablesCmd((CS_NoArgsCmd_t *)BufPtr);
            }
            break;

        case CS_REPORT_BASELINE_TABLE_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_TableNameCmd_t)))
            {
                CS_ReportBaselineTablesCmd((CS_TableNameCmd_t *)BufPtr);
            }
            break;

        case CS_RECOMPUTE_BASELINE_TABLE_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_TableNameCmd_t)))
            {
                CS_RecomputeBaselineTablesCmd((CS_TableNameCmd_t *)BufPtr);
            }
            break;

        case CS_ENABLE_NAME_TABLE_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_TableNameCmd_t)))
            {
                CS_EnableNameTablesCmd((CS_TableNameCmd_t *)BufPtr);
            }
            break;

        case CS_DISABLE_NAME_TABLE_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_TableNameCmd_t)))
            {
                CS_DisableNameTablesCmd((CS_TableNameCmd_t *)BufPtr);
            }
            break;

        /* App Commands */
        case CS_ENABLE_APPS_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_NoArgsCmd_t)))
            {
                CS_EnableAppCmd((CS_NoArgsCmd_t *)BufPtr);
            }
            break;

        case CS_DISABLE_APPS_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_NoArgsCmd_t)))
            {
                CS_DisableAppCmd((CS_NoArgsCmd_t *)BufPtr);
            }
            break;

        case CS_REPORT_BASELINE_APP_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_AppNameCmd_t)))
            {
                CS_ReportBaselineAppCmd((CS_AppNameCmd_t *)BufPtr);
            }
            break;

        case CS_RECOMPUTE_BASELINE_APP_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_AppNameCmd_t)))
            {
                CS_RecomputeBaselineAppCmd((CS_AppNameCmd_t *)BufPtr);
            }
            break;

        case CS_ENABLE_NAME_APP_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_AppNameCmd_t)))
            {
                CS_EnableNameAppCmd((CS_AppNameCmd_t *)BufPtr);
            }
            break;

        case CS_DISABLE_NAME_APP_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_AppNameCmd_t)))
            {
                CS_DisableNameAppCmd((CS_AppNameCmd_t *)BufPtr);
            }
            break;

        default:
            CFE_EVS_SendEvent(CS_CC_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Invalid ground command code: ID = 0x%08lX, CC = %d",
                              (unsigned long)CFE_SB_MsgIdToValue(MessageID), CommandCode);

            CS_AppData.HkPacket.Payload.CmdErrCounter++;
            break;
    } /* end switch */
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Housekeeping command                                         */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_HousekeepingCmd(const CS_NoArgsCmd_t *CmdPtr)
{
    /* command verification variables */
    size_t            ExpectedLength = sizeof(CS_NoArgsCmd_t);
    CFE_SB_MsgId_t    MessageID      = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_FcnCode_t CommandCode    = 0;
    size_t            ActualLength   = 0;

    CFE_MSG_GetSize(CFE_MSG_PTR(CmdPtr->CommandHeader), &ActualLength);

    /* Verify the command packet length */
    if (ExpectedLength != ActualLength)
    {
        CFE_MSG_GetMsgId(CFE_MSG_PTR(CmdPtr->CommandHeader), &MessageID);
        CFE_MSG_GetFcnCode(CFE_MSG_PTR(CmdPtr->CommandHeader), &CommandCode);

        CFE_EVS_SendEvent(CS_CMD_LEN_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Invalid msg length: ID = 0x%08lX, CC = %d, Len = %lu, Expected = %lu",
                          (unsigned long)CFE_SB_MsgIdToValue(MessageID), CommandCode, (unsigned long)ActualLength,
                          (unsigned long)ExpectedLength);
    }
    else
    {
        /* Send housekeeping telemetry packet */
        CFE_SB_TimeStampMsg(CFE_MSG_PTR(CS_AppData.HkPacket.TelemetryHeader));
        CFE_SB_TransmitMsg(CFE_MSG_PTR(CS_AppData.HkPacket.TelemetryHeader), true);
    }
}

#if (CS_PRESERVE_STATES_ON_PROCESSOR_RESET == true)

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS_CreateCDS() - create CS storage area in CDS                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

CFE_Status_t CS_CreateRestoreStatesFromCDS(void)
{
    /* Store task ena/dis state of tables in CDS */
    uint8        DataStoreBuffer[CS_NUM_DATA_STORE_STATES];
    CFE_Status_t Result;
    int32        EventId = 0;

    memset(DataStoreBuffer, 0, sizeof(DataStoreBuffer));

    /*
    ** Request for CDS area from cFE Executive Services...
    */
    Result = CFE_ES_RegisterCDS(&CS_AppData.DataStoreHandle, sizeof(DataStoreBuffer), CS_CDS_NAME);

    if (Result == CFE_SUCCESS)
    {
        /*
        ** New CDS area - write to Critical Data Store...
        */
        DataStoreBuffer[0] = CS_AppData.HkPacket.Payload.EepromCSState;
        DataStoreBuffer[1] = CS_AppData.HkPacket.Payload.MemoryCSState;
        DataStoreBuffer[2] = CS_AppData.HkPacket.Payload.AppCSState;
        DataStoreBuffer[3] = CS_AppData.HkPacket.Payload.TablesCSState;

        DataStoreBuffer[4] = CS_AppData.HkPacket.Payload.OSCSState;
        DataStoreBuffer[5] = CS_AppData.HkPacket.Payload.CfeCoreCSState;

        Result = CFE_ES_CopyToCDS(CS_AppData.DataStoreHandle, DataStoreBuffer);

        if (Result != CFE_SUCCESS)
        {
            EventId = CS_CR_CDS_CPY_ERR_EID;
        }
    }
    else if (Result == CFE_ES_CDS_ALREADY_EXISTS)
    {
        /*
        ** Pre-existing CDS area - read from Critical Data Store...
        */
        Result = CFE_ES_RestoreFromCDS(DataStoreBuffer, CS_AppData.DataStoreHandle);

        if (Result == CFE_SUCCESS)
        {
            CS_AppData.HkPacket.Payload.EepromCSState = DataStoreBuffer[0];
            CS_AppData.HkPacket.Payload.MemoryCSState = DataStoreBuffer[1];
            CS_AppData.HkPacket.Payload.AppCSState    = DataStoreBuffer[2];
            CS_AppData.HkPacket.Payload.TablesCSState = DataStoreBuffer[3];

            CS_AppData.HkPacket.Payload.OSCSState      = DataStoreBuffer[4];
            CS_AppData.HkPacket.Payload.CfeCoreCSState = DataStoreBuffer[5];
        }
        else
        {
            EventId = CS_CR_CDS_RES_ERR_EID;
        }
    }
    else
    {
        EventId = CS_CR_CDS_REG_ERR_EID;
    }

    if (Result != CFE_SUCCESS)
    {
        /*
        ** CDS is broken - prevent further errors...
        */
        CS_AppData.DataStoreHandle = CFE_ES_CDS_BAD_HANDLE;

        /* Use states from platform configuration */
        CS_AppData.HkPacket.Payload.EepromCSState = CS_EEPROM_TBL_POWERON_STATE;
        CS_AppData.HkPacket.Payload.MemoryCSState = CS_MEMORY_TBL_POWERON_STATE;
        CS_AppData.HkPacket.Payload.AppCSState    = CS_APPS_TBL_POWERON_STATE;
        CS_AppData.HkPacket.Payload.TablesCSState = CS_TABLES_TBL_POWERON_STATE;

        CS_AppData.HkPacket.Payload.OSCSState      = CS_OSCS_CHECKSUM_STATE;
        CS_AppData.HkPacket.Payload.CfeCoreCSState = CS_CFECORE_CHECKSUM_STATE;

        CFE_EVS_SendEvent(EventId, CFE_EVS_EventType_ERROR, "Critical Data Store access error = 0x%08X",
                          (unsigned int)Result);
        /*
        ** CDS errors are not fatal - CS can still run...
        */
        Result = CFE_SUCCESS;
    }

    return Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS_UpdateCDS() - update DS storage area in CDS                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CS_UpdateCDS(void)
{
    /* Store table ena/dis state in CDS */
    uint8        DataStoreBuffer[CS_NUM_DATA_STORE_STATES];
    CFE_Status_t Result;

    /*
    ** Handle is defined when CDS is active...
    */
    if (CFE_RESOURCEID_TEST_DEFINED(CS_AppData.DataStoreHandle))
    {
        /*
        ** Copy ena/dis states of tables to the data array...
        */
        DataStoreBuffer[0] = CS_AppData.HkPacket.Payload.EepromCSState;
        DataStoreBuffer[1] = CS_AppData.HkPacket.Payload.MemoryCSState;
        DataStoreBuffer[2] = CS_AppData.HkPacket.Payload.AppCSState;
        DataStoreBuffer[3] = CS_AppData.HkPacket.Payload.TablesCSState;

        DataStoreBuffer[4] = CS_AppData.HkPacket.Payload.OSCSState;
        DataStoreBuffer[5] = CS_AppData.HkPacket.Payload.CfeCoreCSState;

        /*
        ** Update CS portion of Critical Data Store...
        */
        Result = CFE_ES_CopyToCDS(CS_AppData.DataStoreHandle, DataStoreBuffer);

        if (Result != CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(CS_UPDATE_CDS_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Critical Data Store access error = 0x%08X", (unsigned int)Result);
            /*
            ** CDS is broken - prevent further errors...
            */
            CS_AppData.DataStoreHandle = CFE_ES_CDS_BAD_HANDLE;
        }
    }
}
#endif /* #if (CS_PRESERVE_STATES_ON_PROCESSOR_RESET == true   ) */
