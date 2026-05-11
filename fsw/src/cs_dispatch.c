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
 *   CFS Checksum (CS) Application provides the service of background
 *   checksumming user-defined objects in the CFS
 */
#include <string.h>
#include "cfe.h"
#include "cs_app.h"

#include "cs_platform_cfg.h"
#include "cs_eventids.h"
#include "cs_eeprom_cmds.h"
#include "cs_table_cmds.h"
#include "cs_memory_cmds.h"
#include "cs_app_cmds.h"
#include "cs_cmds.h"
#include "cs_dispatch.h"
#include "cs_utils.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Verify the length of the command                             */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
bool CS_VerifyCmdLength(const CFE_MSG_Message_t *msg, size_t ExpectedLength)
{
    CFE_SB_MsgId_t    MessageID    = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_FcnCode_t CommandCode  = 0;
    bool              Result       = true;
    size_t            ActualLength = 0;

    CFE_MSG_GetSize(msg, &ActualLength);

    /* Verify the command packet length */
    if (ExpectedLength != ActualLength)
    {
        CFE_MSG_GetMsgId(msg, &MessageID);
        CFE_MSG_GetFcnCode(msg, &CommandCode);

        CFE_EVS_SendEvent(CS_CMD_LEN_ERR_EID,
                          CFE_EVS_EventType_ERROR,
                          "Invalid msg length: ID = 0x%08lX, CC = %d, Len = %lu, Expected = %lu",
                          (unsigned long)CFE_SB_MsgIdToValue(MessageID),
                          CommandCode,
                          (unsigned long)ActualLength,
                          (unsigned long)ExpectedLength);
        Result = false;
        CS_AppData.HkPacket.Payload.CommandErrorCounter++;
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
    static CFE_SB_MsgId_t CMD_MID              = CFE_SB_MSGID_RESERVED;
    static CFE_SB_MsgId_t BACKGROUND_CYCLE_MID = CFE_SB_MSGID_RESERVED;
    static CFE_SB_MsgId_t SEND_HK_MID          = CFE_SB_MSGID_RESERVED;

    CFE_SB_MsgId_t MessageID = CFE_SB_INVALID_MSG_ID;

    /* cache the local MID Values here, this avoids repeat lookups */
    if (!CFE_SB_IsValidMsgId(CMD_MID))
    {
        CMD_MID              = CFE_SB_ValueToMsgId(CS_CMD_MID);
        SEND_HK_MID          = CFE_SB_ValueToMsgId(CS_SEND_HK_MID);
        BACKGROUND_CYCLE_MID = CFE_SB_ValueToMsgId(CS_BACKGROUND_CYCLE_MID);
    }

    CFE_MSG_GetMsgId(&BufPtr->Msg, &MessageID);

    /* Process all SB messages */
    if (CFE_SB_MsgId_Equal(MessageID, SEND_HK_MID))
    {
        /* Housekeeping telemetry request */
        if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_SendHkCmd_t)))
        {
            CS_SendHkCmd((const CS_SendHkCmd_t *)BufPtr);
        }
    }
    else if (CFE_SB_MsgId_Equal(MessageID, BACKGROUND_CYCLE_MID))
    {
        if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_BackgroundCheckCycleCmd_t)))
        {
            CS_BackgroundCheckCycleCmd((const CS_BackgroundCheckCycleCmd_t *)BufPtr);
        }
    }
    else if (CFE_SB_MsgId_Equal(MessageID, CMD_MID))
    {
        /* All CS Commands */
        CS_ProcessCmd(BufPtr);
    }
    else
    {
        /* Unknown command */
        CFE_EVS_SendEvent(CS_MID_ERR_EID,
                          CFE_EVS_EventType_ERROR,
                          "Invalid command pipe message ID: 0x%08lX",
                          (unsigned long)CFE_SB_MsgIdToValue(MessageID));

        CS_AppData.HkPacket.Payload.CommandErrorCounter++;
    }

    return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS application -- command packet processor                      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CS_ProcessCmd(const CFE_SB_Buffer_t *BufPtr)
{
    CFE_SB_MsgId_t MessageID   = CFE_SB_INVALID_MSG_ID;
    uint16         CommandCode = 0;

    CFE_MSG_GetFcnCode(&BufPtr->Msg, &CommandCode);

    switch (CommandCode)
    {
        /*  All CS Commands */
        case CS_NOOP_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_NoopCmd_t)))
            {
                CS_NoopCmd((const CS_NoopCmd_t *)BufPtr);
            }
            break;

        case CS_RESET_COUNTERS_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_ResetCountersCmd_t)))
            {
                CS_ResetCountersCmd((const CS_ResetCountersCmd_t *)BufPtr);
            }
            break;

        case CS_ONE_SHOT_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_OneShotCmd_t)))
            {
                CS_OneShotCmd((const CS_OneShotCmd_t *)BufPtr);
            }
            break;

        case CS_CANCEL_ONE_SHOT_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_CancelOneShotCmd_t)))
            {
                CS_CancelOneShotCmd((const CS_CancelOneShotCmd_t *)BufPtr);
            }
            break;

        case CS_ENABLE_ALL_CS_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_EnableAllCSCmd_t)))
            {
                CS_EnableAllCSCmd((const CS_EnableAllCSCmd_t *)BufPtr);
            }
            break;

        case CS_DISABLE_ALL_CS_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_DisableAllCSCmd_t)))
            {
                CS_DisableAllCSCmd((const CS_DisableAllCSCmd_t *)BufPtr);
            }
            break;

        /* cFE core Commands */
        case CS_ENABLE_CFE_CORE_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_EnableCfeCoreCmd_t)))
            {
                CS_EnableCfeCoreCmd((const CS_EnableCfeCoreCmd_t *)BufPtr);
            }
            break;

        case CS_DISABLE_CFE_CORE_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_DisableCfeCoreCmd_t)))
            {
                CS_DisableCfeCoreCmd((const CS_DisableCfeCoreCmd_t *)BufPtr);
            }
            break;

        case CS_REPORT_BASELINE_CFE_CORE_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_ReportBaselineCfeCoreCmd_t)))
            {
                CS_ReportBaselineCfeCoreCmd((const CS_ReportBaselineCfeCoreCmd_t *)BufPtr);
            }
            break;

        case CS_RECOMPUTE_BASELINE_CFE_CORE_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_RecomputeBaselineCfeCoreCmd_t)))
            {
                CS_RecomputeBaselineCfeCoreCmd((const CS_RecomputeBaselineCfeCoreCmd_t *)BufPtr);
            }
            break;

        /* OS Commands*/
        case CS_ENABLE_OS_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_EnableOSCmd_t)))
            {
                CS_EnableOSCmd((const CS_EnableOSCmd_t *)BufPtr);
            }
            break;

        case CS_DISABLE_OS_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_DisableOSCmd_t)))
            {
                CS_DisableOSCmd((const CS_DisableOSCmd_t *)BufPtr);
            }
            break;

        case CS_REPORT_BASELINE_OS_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_ReportBaselineOSCmd_t)))
            {
                CS_ReportBaselineOSCmd((const CS_ReportBaselineOSCmd_t *)BufPtr);
            }
            break;

        case CS_RECOMPUTE_BASELINE_OS_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_RecomputeBaselineOSCmd_t)))
            {
                CS_RecomputeBaselineOSCmd((const CS_RecomputeBaselineOSCmd_t *)BufPtr);
            }
            break;

        /* EEPROM Commands */
        case CS_ENABLE_EEPROM_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_EnableEepromCmd_t)))
            {
                CS_EnableEepromCmd((const CS_EnableEepromCmd_t *)BufPtr);
            }
            break;

        case CS_DISABLE_EEPROM_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_DisableEepromCmd_t)))
            {
                CS_DisableEepromCmd((const CS_DisableEepromCmd_t *)BufPtr);
            }
            break;

        case CS_REPORT_BASELINE_ENTRY_ID_EEPROM_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_ReportBaselineEntryIDEepromCmd_t)))
            {
                CS_ReportBaselineEntryIDEepromCmd((const CS_ReportBaselineEntryIDEepromCmd_t *)BufPtr);
            }
            break;

        case CS_RECOMPUTE_BASELINE_EEPROM_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_RecomputeBaselineEepromCmd_t)))
            {
                CS_RecomputeBaselineEepromCmd((const CS_RecomputeBaselineEepromCmd_t *)BufPtr);
            }
            break;

        case CS_ENABLE_ENTRY_ID_EEPROM_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_EnableEntryIDEepromCmd_t)))
            {
                CS_EnableEntryIDEepromCmd((const CS_EnableEntryIDEepromCmd_t *)BufPtr);
            }
            break;

        case CS_DISABLE_ENTRY_ID_EEPROM_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_DisableEntryIDEepromCmd_t)))
            {
                CS_DisableEntryIDEepromCmd((const CS_DisableEntryIDEepromCmd_t *)BufPtr);
            }
            break;

        case CS_GET_ENTRY_ID_EEPROM_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_GetEntryIDEepromCmd_t)))
            {
                CS_GetEntryIDEepromCmd((const CS_GetEntryIDEepromCmd_t *)BufPtr);
            }
            break;

        /*  Memory Commands */
        case CS_ENABLE_MEMORY_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_EnableMemoryCmd_t)))
            {
                CS_EnableMemoryCmd((const CS_EnableMemoryCmd_t *)BufPtr);
            }
            break;

        case CS_DISABLE_MEMORY_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_DisableMemoryCmd_t)))
            {
                CS_DisableMemoryCmd((const CS_DisableMemoryCmd_t *)BufPtr);
            }
            break;

        case CS_REPORT_BASELINE_ENTRY_ID_MEMORY_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_ReportBaselineEntryIDMemoryCmd_t)))
            {
                CS_ReportBaselineEntryIDMemoryCmd((const CS_ReportBaselineEntryIDMemoryCmd_t *)BufPtr);
            }
            break;

        case CS_RECOMPUTE_BASELINE_MEMORY_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_RecomputeBaselineMemoryCmd_t)))
            {
                CS_RecomputeBaselineMemoryCmd((const CS_RecomputeBaselineMemoryCmd_t *)BufPtr);
            }
            break;

        case CS_ENABLE_ENTRY_ID_MEMORY_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_EnableEntryIDMemoryCmd_t)))
            {
                CS_EnableEntryIDMemoryCmd((const CS_EnableEntryIDMemoryCmd_t *)BufPtr);
            }
            break;

        case CS_DISABLE_ENTRY_ID_MEMORY_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_DisableEntryIDMemoryCmd_t)))
            {
                CS_DisableEntryIDMemoryCmd((const CS_DisableEntryIDMemoryCmd_t *)BufPtr);
            }
            break;

        case CS_GET_ENTRY_ID_MEMORY_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_GetEntryIDMemoryCmd_t)))
            {
                CS_GetEntryIDMemoryCmd((const CS_GetEntryIDMemoryCmd_t *)BufPtr);
            }
            break;

        /* Tables Commands */
        case CS_ENABLE_TABLES_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_EnableTablesCmd_t)))
            {
                CS_EnableTablesCmd((const CS_EnableTablesCmd_t *)BufPtr);
            }
            break;

        case CS_DISABLE_TABLES_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_DisableTablesCmd_t)))
            {
                CS_DisableTablesCmd((const CS_DisableTablesCmd_t *)BufPtr);
            }
            break;

        case CS_REPORT_BASELINE_TABLE_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_ReportBaselineTableCmd_t)))
            {
                CS_ReportBaselineTableCmd((const CS_ReportBaselineTableCmd_t *)BufPtr);
            }
            break;

        case CS_RECOMPUTE_BASELINE_TABLE_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_RecomputeBaselineTableCmd_t)))
            {
                CS_RecomputeBaselineTableCmd((const CS_RecomputeBaselineTableCmd_t *)BufPtr);
            }
            break;

        case CS_ENABLE_NAME_TABLE_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_EnableNameTableCmd_t)))
            {
                CS_EnableNameTableCmd((const CS_EnableNameTableCmd_t *)BufPtr);
            }
            break;

        case CS_DISABLE_NAME_TABLE_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_DisableNameTableCmd_t)))
            {
                CS_DisableNameTableCmd((const CS_DisableNameTableCmd_t *)BufPtr);
            }
            break;

        /* App Commands */
        case CS_ENABLE_APPS_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_EnableAppsCmd_t)))
            {
                CS_EnableAppsCmd((const CS_EnableAppsCmd_t *)BufPtr);
            }
            break;

        case CS_DISABLE_APPS_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_DisableAppsCmd_t)))
            {
                CS_DisableAppsCmd((const CS_DisableAppsCmd_t *)BufPtr);
            }
            break;

        case CS_REPORT_BASELINE_APP_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_ReportBaselineAppCmd_t)))
            {
                CS_ReportBaselineAppCmd((const CS_ReportBaselineAppCmd_t *)BufPtr);
            }
            break;

        case CS_RECOMPUTE_BASELINE_APP_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_RecomputeBaselineAppCmd_t)))
            {
                CS_RecomputeBaselineAppCmd((const CS_RecomputeBaselineAppCmd_t *)BufPtr);
            }
            break;

        case CS_ENABLE_NAME_APP_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_EnableNameAppCmd_t)))
            {
                CS_EnableNameAppCmd((const CS_EnableNameAppCmd_t *)BufPtr);
            }
            break;

        case CS_DISABLE_NAME_APP_CC:
            if (CS_VerifyCmdLength(&BufPtr->Msg, sizeof(CS_DisableNameAppCmd_t)))
            {
                CS_DisableNameAppCmd((const CS_DisableNameAppCmd_t *)BufPtr);
            }
            break;

        default:
            CFE_MSG_GetMsgId(&BufPtr->Msg, &MessageID);
            CFE_EVS_SendEvent(CS_CC_ERR_EID,
                              CFE_EVS_EventType_ERROR,
                              "Invalid ground command code: ID = 0x%08lX, CC = %d",
                              (unsigned long)CFE_SB_MsgIdToValue(MessageID),
                              CommandCode);

            CS_AppData.HkPacket.Payload.CommandErrorCounter++;
            break;
    } /* end switch */
}
