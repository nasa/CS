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

#include <string.h>
#include "cfe.h"
#include "cs_app.h"

#include "cs_platform_cfg.h"
#include "cs_eventids.h"
#include "cs_utils.h"
#include "cs_compute.h"
#include "cs_eeprom_cmds.h"
#include "cs_table_cmds.h"
#include "cs_memory_cmds.h"
#include "cs_app_cmds.h"
#include "cs_cmds.h"
#include "cs_init.h"
#include "cs_table_processing.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Software Bus Setup                                           */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CFE_Status_t CS_SbInit(void)
{
    CFE_Status_t Result = CFE_SUCCESS;

    /* Initialize housekeeping packet */
    CFE_MSG_Init(CFE_MSG_PTR(CS_AppData.HkPacket.TelemetryHeader),
                 CFE_SB_ValueToMsgId(CS_HK_TLM_MID),
                 sizeof(CS_HkPacket_t));

    /* Create Software Bus message pipe */
    Result = CFE_SB_CreatePipe(&CS_AppData.CmdPipe, CS_PIPE_DEPTH, CS_CMD_PIPE_NAME);
    if (Result != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(CS_CR_PIPE_ERR_EID,
                          CFE_EVS_EventType_ERROR,
                          "Software Bus Create Pipe for command returned: 0x%08X",
                          (unsigned int)Result);
    }
    else
    {
        /* Subscribe to Housekeeping request commands */
        Result = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(CS_SEND_HK_MID), CS_AppData.CmdPipe);

        if (Result != CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(CS_INIT_SB_SUBSCRIBE_HK_ERR_EID,
                              CFE_EVS_EventType_ERROR,
                              "Software Bus subscribe to housekeeping returned: 0x%08X",
                              (unsigned int)Result);
        }
        else
        {
            /* Subscribe to background checking schedule */
            Result = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(CS_BACKGROUND_CYCLE_MID), CS_AppData.CmdPipe);

            if (Result != CFE_SUCCESS)
            {
                CFE_EVS_SendEvent(CS_INIT_SB_SUBSCRIBE_BACK_ERR_EID,
                                  CFE_EVS_EventType_ERROR,
                                  "Software Bus subscribe to background cycle returned: 0x%08X",
                                  (unsigned int)Result);
            }
        }

        if (Result == CFE_SUCCESS)
        {
            /* Subscribe to CS Internal command packets */
            Result = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(CS_CMD_MID), CS_AppData.CmdPipe);
            if (Result != CFE_SUCCESS)
            {
                CFE_EVS_SendEvent(CS_INIT_SB_SUBSCRIBE_CMD_ERR_EID,
                                  CFE_EVS_EventType_ERROR,
                                  "Software Bus subscribe to command returned: 0x%08X",
                                  (unsigned int)Result);
            }
        }
    }
    return Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Table Initialization                                         */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CFE_Status_t CS_InitAllTables(void)
{
    static const CS_TableWrapper_t TABLE_INITIALIZERS[CS_NUM_TABLES] = {
        [CS_ChecksumType_CFECORE] =
            {
                .BaselineValue = &CS_AppData.HkPacket.Payload.CfeCoreBaseline,
                .ResEntrySize  = sizeof(CS_Res_EepromMemory_Table_Entry_t),
                .ResAddr       = &CS_AppData.CfeCoreCodeSeg,
                .NumEntries    = 1,
            },
        [CS_ChecksumType_OSCORE] =
            {
                .BaselineValue = &CS_AppData.HkPacket.Payload.OSBaseline,
                .ResEntrySize  = sizeof(CS_Res_EepromMemory_Table_Entry_t),
                .ResAddr       = &CS_AppData.OSCodeSeg,
                .NumEntries    = 1,
            },
        [CS_ChecksumType_EEPROM_TABLE] =
            {
                .DefTableName         = CS_DEF_EEPROM_TABLE_NAME,
                .ResTableName         = CS_RESULTS_EEPROM_TABLE_NAME,
                .GlobalState          = &CS_AppData.HkPacket.Payload.EepromCSState,
                .NumEntries           = CS_MAX_NUM_EEPROM_TABLE_ENTRIES,
                .DefEntrySize         = sizeof(CS_Def_EepromMemory_Table_Entry_t),
                .ResEntrySize         = sizeof(CS_Res_EepromMemory_Table_Entry_t),
                .DefStateOffset       = offsetof(CS_Def_EepromMemory_Table_Entry_t, State),
                .UpdateHandler        = CS_ProcessNewEepromMemoryDefinitionTable,
                .DefaultDefinitionPtr = CS_AppData.DefaultEepromDefTable,
            },
        [CS_ChecksumType_MEMORY_TABLE] =
            {
                .DefTableName         = CS_DEF_MEMORY_TABLE_NAME,
                .ResTableName         = CS_RESULTS_MEMORY_TABLE_NAME,
                .GlobalState          = &CS_AppData.HkPacket.Payload.MemoryCSState,
                .NumEntries           = CS_MAX_NUM_MEMORY_TABLE_ENTRIES,
                .DefEntrySize         = sizeof(CS_Def_EepromMemory_Table_Entry_t),
                .ResEntrySize         = sizeof(CS_Res_EepromMemory_Table_Entry_t),
                .DefStateOffset       = offsetof(CS_Def_EepromMemory_Table_Entry_t, State),
                .UpdateHandler        = CS_ProcessNewEepromMemoryDefinitionTable,
                .DefaultDefinitionPtr = CS_AppData.DefaultMemoryDefTable,
            },
        [CS_ChecksumType_TABLES_TABLE] =
            {
                .DefTableName         = CS_DEF_TABLES_TABLE_NAME,
                .ResTableName         = CS_RESULTS_TABLES_TABLE_NAME,
                .GlobalState          = &CS_AppData.HkPacket.Payload.TablesCSState,
                .NumEntries           = CS_MAX_NUM_TABLES_TABLE_ENTRIES,
                .DefEntrySize         = sizeof(CS_Def_Tables_Table_Entry_t),
                .ResEntrySize         = sizeof(CS_Res_Tables_Table_Entry_t),
                .DefStateOffset       = offsetof(CS_Def_Tables_Table_Entry_t, State),
                .UpdateHandler        = CS_ProcessNewTablesDefinitionTable,
                .DefaultDefinitionPtr = CS_AppData.DefaultTablesDefTable,
            },
        [CS_ChecksumType_APP_TABLE] = {
            .DefTableName         = CS_DEF_APP_TABLE_NAME,
            .ResTableName         = CS_RESULTS_APP_TABLE_NAME,
            .GlobalState          = &CS_AppData.HkPacket.Payload.AppCSState,
            .NumEntries           = CS_MAX_NUM_APP_TABLE_ENTRIES,
            .DefEntrySize         = sizeof(CS_Def_App_Table_Entry_t),
            .ResEntrySize         = sizeof(CS_Res_App_Table_Entry_t),
            .DefStateOffset       = offsetof(CS_Def_App_Table_Entry_t, State),
            .UpdateHandler        = CS_ProcessNewAppDefinitionTable,
            .DefaultDefinitionPtr = CS_AppData.DefaultAppDefTable,
        }};

    static const char *const TABLE_FILENAME_MAP[CS_NUM_TABLES] = {
        [CS_ChecksumType_EEPROM_TABLE] = CS_DEF_EEPROM_TABLE_FILENAME,
        [CS_ChecksumType_MEMORY_TABLE] = CS_DEF_MEMORY_TABLE_FILENAME,
        [CS_ChecksumType_TABLES_TABLE] = CS_DEF_TABLES_TABLE_FILENAME,
        [CS_ChecksumType_APP_TABLE]    = CS_DEF_APP_TABLE_FILENAME
    };

    static const CFE_TBL_CallbackFuncPtr_t TABLE_CALLBACK_MAP[CS_NUM_TABLES] = {
        [CS_ChecksumType_EEPROM_TABLE] = CS_ValidateEepromChecksumDefinitionTable,
        [CS_ChecksumType_MEMORY_TABLE] = CS_ValidateMemoryChecksumDefinitionTable,
        [CS_ChecksumType_TABLES_TABLE] = CS_ValidateTablesChecksumDefinitionTable,
        [CS_ChecksumType_APP_TABLE]    = CS_ValidateAppChecksumDefinitionTable
    };

    static const uint16 TABLE_EVENTID_MAP[CS_NUM_TABLES] = { [CS_ChecksumType_EEPROM_TABLE] = CS_INIT_EEPROM_ERR_EID,
                                                             [CS_ChecksumType_MEMORY_TABLE] = CS_INIT_MEMORY_ERR_EID,
                                                             [CS_ChecksumType_TABLES_TABLE] = CS_INIT_TABLES_ERR_EID,
                                                             [CS_ChecksumType_APP_TABLE]    = CS_INIT_APP_ERR_EID };

    CFE_Status_t              ResultAll;
    CFE_Status_t              Result;
    const char               *LoadFilename;
    CFE_TBL_CallbackFuncPtr_t ValidationFunc;
    CS_TableWrapper_t        *tw;
    CS_ChecksumType_Enum_t    TableId;
    uint16                    ErrorEventID;

    ResultAll = CFE_SUCCESS;
    tw        = CS_AppData.Tbl;

    for (TableId = 0; TableId < CS_NUM_TABLES; ++TableId)
    {
        *tw            = TABLE_INITIALIZERS[TableId];
        LoadFilename   = TABLE_FILENAME_MAP[TableId];
        ValidationFunc = TABLE_CALLBACK_MAP[TableId];
        ErrorEventID   = TABLE_EVENTID_MAP[TableId];

        /* If this table needs to be loaded via table services, do that now */
        Result = CS_TableInit(tw, LoadFilename, ValidationFunc);
        if (Result != CFE_SUCCESS)
        {
            /* If the init did not work then make sure the global status is set to disabled */
            if (tw->GlobalState)
            {
                *tw->GlobalState = CS_ChecksumState_DISABLED;
            }

            if (ErrorEventID != 0)
            {
                CFE_EVS_SendEvent(ErrorEventID,
                                  CFE_EVS_EventType_ERROR,
                                  "Table initialization failed for %s: 0x%08X",
                                  CS_GetTableTypeAsString(tw),
                                  (unsigned int)Result);
            }

            ResultAll = Result;
        }

        ++tw;
    }

    return ResultAll;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Segment Initialization                                       */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_InitSegments(void)
{
    int32   ResultSegment = OS_SUCCESS;
    uint32  CFESize;
    cpuaddr CFEAddress;
    uint32  KernelSize;
    cpuaddr KernelAddress;

    CS_Res_EepromMemory_Table_Entry_t *CodeSeg;

    /* Initalize the CFE core segments */
    ResultSegment = CFE_PSP_GetCFETextSegmentInfo(&CFEAddress, &CFESize);
    CodeSeg       = CS_GetCfeCoreCodeSegResTable();

    if (ResultSegment != OS_SUCCESS)
    {
        CodeSeg->StartAddress       = 0;
        CodeSeg->NumBytesToChecksum = 0;
        CodeSeg->ComputedYet        = false;
        CodeSeg->ComparisonValue    = 0;
        CodeSeg->ByteOffset         = 0;
        CodeSeg->TempChecksumValue  = 0;
        CodeSeg->State              = CS_ChecksumState_DISABLED;

        CFE_EVS_SendEvent(CS_CFE_TEXT_SEG_INF_EID, CFE_EVS_EventType_INFORMATION, "CFE Text Segment disabled");
    }
    else
    {
        CodeSeg->StartAddress       = CFEAddress;
        CodeSeg->NumBytesToChecksum = CFESize;
        CodeSeg->ComputedYet        = false;
        CodeSeg->ComparisonValue    = 0;
        CodeSeg->ByteOffset         = 0;
        CodeSeg->TempChecksumValue  = 0;
        CodeSeg->State              = CS_ChecksumState_ENABLED;
    }

    /* Initialize the OS Core code segment*/

    ResultSegment = CFE_PSP_GetKernelTextSegmentInfo(&KernelAddress, &KernelSize);
    CodeSeg       = CS_GetOSCodeSegResTable();

    if (ResultSegment != OS_SUCCESS)
    {
        CodeSeg->StartAddress       = 0;
        CodeSeg->NumBytesToChecksum = 0;
        CodeSeg->ComputedYet        = false;
        CodeSeg->ComparisonValue    = 0;
        CodeSeg->ByteOffset         = 0;
        CodeSeg->TempChecksumValue  = 0;
        CodeSeg->State              = CS_ChecksumState_DISABLED;

        CFE_EVS_SendEvent(CS_OS_TEXT_SEG_INF_EID,
                          CFE_EVS_EventType_INFORMATION,
                          "OS Text Segment disabled due to platform");
    }
    else
    {
        CodeSeg->StartAddress       = KernelAddress;
        CodeSeg->NumBytesToChecksum = KernelSize;
        CodeSeg->ComputedYet        = false;
        CodeSeg->ComparisonValue    = 0;
        CodeSeg->ByteOffset         = 0;
        CodeSeg->TempChecksumValue  = 0;
        CodeSeg->State              = CS_ChecksumState_ENABLED;
    }
}
