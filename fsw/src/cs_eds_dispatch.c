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

#include "cs_eds_dictionary.h"
#include "cs_eds_dispatcher.h"

static const EdsDispatchTable_EdsComponent_CS_Application_CFE_SB_Telecommand_t CS_TC_DISPATCH_TABLE = {
    .CMD =
        {
            .CancelOneShotCmd_indication               = CS_CancelOneShotCmd,
            .DisableAllCSCmd_indication                = CS_DisableAllCSCmd,
            .DisableAppsCmd_indication                 = CS_DisableAppsCmd,
            .DisableCfeCoreCmd_indication              = CS_DisableCfeCoreCmd,
            .DisableEepromCmd_indication               = CS_DisableEepromCmd,
            .DisableEntryIDEepromCmd_indication        = CS_DisableEntryIDEepromCmd,
            .DisableEntryIDMemoryCmd_indication        = CS_DisableEntryIDMemoryCmd,
            .DisableMemoryCmd_indication               = CS_DisableMemoryCmd,
            .DisableNameAppCmd_indication              = CS_DisableNameAppCmd,
            .DisableNameTableCmd_indication            = CS_DisableNameTableCmd,
            .DisableOSCmd_indication                   = CS_DisableOSCmd,
            .DisableTablesCmd_indication               = CS_DisableTablesCmd,
            .EnableAllCSCmd_indication                 = CS_EnableAllCSCmd,
            .EnableAppsCmd_indication                  = CS_EnableAppsCmd,
            .EnableCfeCoreCmd_indication               = CS_EnableCfeCoreCmd,
            .EnableEepromCmd_indication                = CS_EnableEepromCmd,
            .EnableEntryIDEepromCmd_indication         = CS_EnableEntryIDEepromCmd,
            .EnableEntryIDMemoryCmd_indication         = CS_EnableEntryIDMemoryCmd,
            .EnableMemoryCmd_indication                = CS_EnableMemoryCmd,
            .EnableNameAppCmd_indication               = CS_EnableNameAppCmd,
            .EnableNameTableCmd_indication             = CS_EnableNameTableCmd,
            .EnableOSCmd_indication                    = CS_EnableOSCmd,
            .EnableTablesCmd_indication                = CS_EnableTablesCmd,
            .GetEntryIDEepromCmd_indication            = CS_GetEntryIDEepromCmd,
            .GetEntryIDMemoryCmd_indication            = CS_GetEntryIDMemoryCmd,
            .NoopCmd_indication                        = CS_NoopCmd,
            .OneShotCmd_indication                     = CS_OneShotCmd,
            .RecomputeBaselineAppCmd_indication        = CS_RecomputeBaselineAppCmd,
            .RecomputeBaselineCfeCoreCmd_indication    = CS_RecomputeBaselineCfeCoreCmd,
            .RecomputeBaselineEepromCmd_indication     = CS_RecomputeBaselineEepromCmd,
            .RecomputeBaselineMemoryCmd_indication     = CS_RecomputeBaselineMemoryCmd,
            .RecomputeBaselineOSCmd_indication         = CS_RecomputeBaselineOSCmd,
            .RecomputeBaselineTableCmd_indication      = CS_RecomputeBaselineTableCmd,
            .ReportBaselineAppCmd_indication           = CS_ReportBaselineAppCmd,
            .ReportBaselineCfeCoreCmd_indication       = CS_ReportBaselineCfeCoreCmd,
            .ReportBaselineEntryIDEepromCmd_indication = CS_ReportBaselineEntryIDEepromCmd,
            .ReportBaselineEntryIDMemoryCmd_indication = CS_ReportBaselineEntryIDMemoryCmd,
            .ReportBaselineOSCmd_indication            = CS_ReportBaselineOSCmd,
            .ReportBaselineTableCmd_indication         = CS_ReportBaselineTableCmd,
            .ResetCountersCmd_indication                       = CS_ResetCountersCmd,
        },
    .SEND_HK          = {.indication = CS_SendHkCmd},
    .BACKGROUND_CYCLE = {.indication = CS_BackgroundCheckCycleCmd}};

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS_AppPipe() -- Process command pipe message           */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CFE_Status_t CS_AppPipe(const CFE_SB_Buffer_t *BufPtr)
{
    CFE_Status_t      status;
    CFE_SB_MsgId_t    MsgId;
    CFE_MSG_Size_t    MsgSize;
    CFE_MSG_FcnCode_t MsgFc;

    status = EdsDispatch_EdsComponent_CS_Application_Telecommand(BufPtr, &CS_TC_DISPATCH_TABLE);

    if (status != CFE_SUCCESS)
    {
        CFE_MSG_GetMsgId(&BufPtr->Msg, &MsgId);

        ++CS_AppData.HkPacket.Payload.CommandErrorCounter;

        if (status == CFE_STATUS_UNKNOWN_MSG_ID)
        {
            CFE_EVS_SendEvent(CS_MID_ERR_EID,
                              CFE_EVS_EventType_ERROR,
                              "Invalid command pipe message ID: 0x%08lX",
                              (unsigned long)CFE_SB_MsgIdToValue(MsgId));
        }
        else
        {
            CFE_MSG_GetFcnCode(&BufPtr->Msg, &MsgFc);
            if (status == CFE_STATUS_WRONG_MSG_LENGTH)
            {
                CFE_MSG_GetSize(&BufPtr->Msg, &MsgSize);
                CFE_EVS_SendEvent(CS_CMD_LEN_ERR_EID,
                                  CFE_EVS_EventType_ERROR,
                                  "Invalid msg length: ID = 0x%08lX, CC = %d, Len = %lu",
                                  (unsigned long)CFE_SB_MsgIdToValue(MsgId),
                                  MsgFc,
                                  (unsigned long)MsgSize);
            }
            else
            {
                CFE_EVS_SendEvent(CS_CC_ERR_EID,
                                  CFE_EVS_EventType_ERROR,
                                  "Invalid ground command code: ID = 0x%08lX, CC = %d",
                                  (unsigned long)CFE_SB_MsgIdToValue(MsgId),
                                  MsgFc);
            }
        }

        /* do not exit the app */
        status = CFE_SUCCESS;
    }

    return status;
}
