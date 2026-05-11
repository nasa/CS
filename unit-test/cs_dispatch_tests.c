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

/*
 * Includes
 */

#include "cs_app.h"
#include "cs_app_cmds.h"
#include "cs_cmds.h"
#include "cs_eeprom_cmds.h"
#include "cs_memory_cmds.h"
#include "cs_table_cmds.h"
#include "cs_msg.h"
#include "cs_msgdefs.h"
#include "cs_eventids.h"
#include "cs_version.h"
#include "cs_init.h"
#include "cs_utils.h"
#include "cs_dispatch.h"
#include "cs_test_utils.h"
#include <unistd.h>
#include <stdlib.h>

/* UT includes */
#include "uttest.h"
#include "utassert.h"
#include "utstubs.h"

#define CS_NUM_DATA_STORE_STATES 6

/* Define a CDS ID that will return TRUE for CFE_RESOURCEID_TEST_DEFINED, basically nonzero */
#define UT_VALID_CDSID CFE_ES_CDSHANDLE_C(CFE_RESOURCEID_WRAP(1))

/* Command buffer typedef for any handler */
typedef union
{
    CFE_SB_Buffer_t Buf;

    CS_SendHkCmd_t                      SendHkCmd;
    CS_NoopCmd_t                        NoopCmd;
    CS_ResetCountersCmd_t               ResetCountersCmd;
    CS_BackgroundCheckCycleCmd_t        BackgroundCheckCycle;
    CS_DisableAllCSCmd_t                DisableAllCSCmd;
    CS_EnableAllCSCmd_t                 EnableAllCSCmd;
    CS_DisableCfeCoreCmd_t              DisableCfeCoreCmd;
    CS_EnableCfeCoreCmd_t               EnableCfeCoreCmd;
    CS_DisableOSCmd_t                   DisableOSCmd;
    CS_EnableOSCmd_t                    EnableOSCmd;
    CS_ReportBaselineCfeCoreCmd_t       ReportBaselineCfeCoreCmd;
    CS_ReportBaselineOSCmd_t            ReportBaselineOSCmd;
    CS_RecomputeBaselineCfeCoreCmd_t    RecomputeBaselineCfeCoreCmd;
    CS_RecomputeBaselineOSCmd_t         RecomputeBaselineOSCmd;
    CS_CancelOneShotCmd_t               CancelOneShotCmd;
    CS_DisableAppsCmd_t                 DisableAppsCmd;
    CS_EnableAppsCmd_t                  EnableAppsCmd;
    CS_DisableEepromCmd_t               DisableEepromCmd;
    CS_EnableEepromCmd_t                EnableEepromCmd;
    CS_DisableMemoryCmd_t               DisableMemoryCmd;
    CS_EnableMemoryCmd_t                EnableMemoryCmd;
    CS_DisableTablesCmd_t               DisableTablesCmd;
    CS_EnableTablesCmd_t                EnableTablesCmd;
    CS_GetEntryIDEepromCmd_t            GetEntryIDEepromCmd;
    CS_GetEntryIDMemoryCmd_t            GetEntryIDMemoryCmd;
    CS_ReportBaselineEntryIDEepromCmd_t ReportBaselineEntryIDEepromCmd;
    CS_DisableEntryIDEepromCmd_t        DisableEntryIDEepromCmd;
    CS_RecomputeBaselineEepromCmd_t     RecomputeBaselineEepromCmd;
    CS_EnableEntryIDEepromCmd_t         EnableEntryIDEepromCmd;
    CS_ReportBaselineEntryIDMemoryCmd_t ReportBaselineEntryIDMemoryCmd;
    CS_DisableEntryIDMemoryCmd_t        DisableEntryIDMemoryCmd;
    CS_RecomputeBaselineMemoryCmd_t     RecomputeBaselineMemoryCmd;
    CS_EnableEntryIDMemoryCmd_t         EnableEntryIDMemoryCmd;
    CS_ReportBaselineTableCmd_t         ReportBaselineTableCmd;
    CS_RecomputeBaselineTableCmd_t      RecomputeBaselineTableCmd;
    CS_DisableNameTableCmd_t            DisableNameTableCmd;
    CS_EnableNameTableCmd_t             EnableNameTableCmd;
    CS_ReportBaselineAppCmd_t           ReportBaselineAppCmd;
    CS_RecomputeBaselineAppCmd_t        RecomputeBaselineAppCmd;
    CS_DisableNameAppCmd_t              DisableNameAppCmd;
    CS_EnableNameAppCmd_t               EnableNameAppCmd;
    CS_OneShotCmd_t                     OneShotCmd;

} UT_CmdBuf_t;

const size_t UT_CMD_SIZE[] = {
    [CS_NOOP_CC]                            = sizeof(CS_NoopCmd_t),
    [CS_RESET_COUNTERS_CC]                  = sizeof(CS_ResetCountersCmd_t),
    [CS_ONE_SHOT_CC]                        = sizeof(CS_OneShotCmd_t),
    [CS_CANCEL_ONE_SHOT_CC]                 = sizeof(CS_CancelOneShotCmd_t),
    [CS_ENABLE_ALL_CS_CC]                   = sizeof(CS_EnableAllCSCmd_t),
    [CS_DISABLE_ALL_CS_CC]                  = sizeof(CS_DisableAllCSCmd_t),
    [CS_ENABLE_CFE_CORE_CC]                 = sizeof(CS_EnableCfeCoreCmd_t),
    [CS_DISABLE_CFE_CORE_CC]                = sizeof(CS_DisableCfeCoreCmd_t),
    [CS_REPORT_BASELINE_CFE_CORE_CC]        = sizeof(CS_ReportBaselineCfeCoreCmd_t),
    [CS_RECOMPUTE_BASELINE_CFE_CORE_CC]     = sizeof(CS_RecomputeBaselineCfeCoreCmd_t),
    [CS_ENABLE_OS_CC]                       = sizeof(CS_EnableOSCmd_t),
    [CS_DISABLE_OS_CC]                      = sizeof(CS_DisableOSCmd_t),
    [CS_REPORT_BASELINE_OS_CC]              = sizeof(CS_ReportBaselineOSCmd_t),
    [CS_RECOMPUTE_BASELINE_OS_CC]           = sizeof(CS_RecomputeBaselineOSCmd_t),
    [CS_ENABLE_EEPROM_CC]                   = sizeof(CS_EnableEepromCmd_t),
    [CS_DISABLE_EEPROM_CC]                  = sizeof(CS_DisableEepromCmd_t),
    [CS_REPORT_BASELINE_ENTRY_ID_EEPROM_CC] = sizeof(CS_ReportBaselineEntryIDEepromCmd_t),
    [CS_RECOMPUTE_BASELINE_EEPROM_CC]       = sizeof(CS_RecomputeBaselineEepromCmd_t),
    [CS_ENABLE_ENTRY_ID_EEPROM_CC]          = sizeof(CS_EnableEntryIDEepromCmd_t),
    [CS_DISABLE_ENTRY_ID_EEPROM_CC]         = sizeof(CS_DisableEntryIDEepromCmd_t),
    [CS_GET_ENTRY_ID_EEPROM_CC]             = sizeof(CS_GetEntryIDEepromCmd_t),
    [CS_ENABLE_MEMORY_CC]                   = sizeof(CS_EnableMemoryCmd_t),
    [CS_DISABLE_MEMORY_CC]                  = sizeof(CS_DisableMemoryCmd_t),
    [CS_REPORT_BASELINE_ENTRY_ID_MEMORY_CC] = sizeof(CS_ReportBaselineEntryIDMemoryCmd_t),
    [CS_RECOMPUTE_BASELINE_MEMORY_CC]       = sizeof(CS_RecomputeBaselineMemoryCmd_t),
    [CS_ENABLE_ENTRY_ID_MEMORY_CC]          = sizeof(CS_EnableEntryIDMemoryCmd_t),
    [CS_DISABLE_ENTRY_ID_MEMORY_CC]         = sizeof(CS_DisableEntryIDMemoryCmd_t),
    [CS_GET_ENTRY_ID_MEMORY_CC]             = sizeof(CS_GetEntryIDMemoryCmd_t),
    [CS_ENABLE_TABLES_CC]                   = sizeof(CS_EnableTablesCmd_t),
    [CS_DISABLE_TABLES_CC]                  = sizeof(CS_DisableTablesCmd_t),
    [CS_REPORT_BASELINE_TABLE_CC]           = sizeof(CS_ReportBaselineTableCmd_t),
    [CS_RECOMPUTE_BASELINE_TABLE_CC]        = sizeof(CS_RecomputeBaselineTableCmd_t),
    [CS_ENABLE_NAME_TABLE_CC]               = sizeof(CS_EnableNameTableCmd_t),
    [CS_DISABLE_NAME_TABLE_CC]              = sizeof(CS_DisableNameTableCmd_t),
    [CS_ENABLE_APPS_CC]                     = sizeof(CS_EnableAppsCmd_t),
    [CS_DISABLE_APPS_CC]                    = sizeof(CS_DisableAppsCmd_t),
    [CS_REPORT_BASELINE_APP_CC]             = sizeof(CS_ReportBaselineAppCmd_t),
    [CS_RECOMPUTE_BASELINE_APP_CC]          = sizeof(CS_RecomputeBaselineAppCmd_t),
    [CS_ENABLE_NAME_APP_CC]                 = sizeof(CS_EnableNameAppCmd_t),
    [CS_DISABLE_NAME_APP_CC]                = sizeof(CS_DisableNameAppCmd_t),
};

/* cs_app_tests globals */
uint8 call_count_CFE_EVS_SendEvent;

/*
 * Function Definitions
 */

void UT_CS_SetupDispatchCheck(CFE_SB_MsgId_t TestMsgId, CFE_MSG_FcnCode_t FcnCode, bool length_ok)
{
    CFE_MSG_Size_t MsgSize;
    CFE_SB_MsgId_t SEND_HK          = CFE_SB_ValueToMsgId(CS_SEND_HK_MID);
    CFE_SB_MsgId_t BACKGROUND_CYCLE = CFE_SB_ValueToMsgId(CS_BACKGROUND_CYCLE_MID);
    CFE_SB_MsgId_t CMD              = CFE_SB_ValueToMsgId(CS_CMD_MID);

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), true);

    if (CFE_SB_MsgId_Equal(TestMsgId, SEND_HK))
    {
        MsgSize = sizeof(CS_SendHkCmd_t);
    }
    else if (CFE_SB_MsgId_Equal(TestMsgId, BACKGROUND_CYCLE))
    {
        MsgSize = sizeof(CS_BackgroundCheckCycleCmd_t);
    }
    else if (CFE_SB_MsgId_Equal(TestMsgId, CMD))
    {
        UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), true);
        if (FcnCode < (sizeof(UT_CMD_SIZE) / sizeof(UT_CMD_SIZE[0])))
        {
            MsgSize = UT_CMD_SIZE[FcnCode];
        }
        else
        {
            UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), true);
            MsgSize = 0;
        }
    }
    else
    {
        MsgSize = 0;
    }
    if (!length_ok)
    {
        /* Make it wrong */
        ++MsgSize;

        /* The event report will call these again */
        UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), true);
        UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), true);
    }
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &MsgSize, sizeof(MsgSize), true);
}

void CS_AppPipe_Test_TableUpdateErrors(void)
{
    CFE_Status_t   Result;
    UT_CmdBuf_t    CmdBuf;
    CFE_SB_MsgId_t TestMsgId;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_SEND_HK_MID);
    UT_CS_SetupDispatchCheck(TestMsgId, 0, true);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0,
                  "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
    /* Generates 5 event messages we don't care about in this test */
}

void CS_AppPipe_Test_BackgroundCycle(void)
{
    CFE_Status_t   Result;
    UT_CmdBuf_t    CmdBuf;
    CFE_SB_MsgId_t TestMsgId;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_BACKGROUND_CYCLE_MID);
    UT_CS_SetupDispatchCheck(TestMsgId, 0, true);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0,
                  "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void CS_AppPipe_Test_NoopCmd(void)
{
    CFE_Status_t      Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_NOOP_CC;
    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0,
                  "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void CS_AppPipe_Test_ResetCountersCmd(void)
{
    CFE_Status_t      Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_RESET_COUNTERS_CC;
    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0,
                  "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void CS_AppPipe_Test_OneShotCmd(void)
{
    CFE_Status_t      Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_ONE_SHOT_CC;
    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0,
                  "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void CS_AppPipe_Test_CancelOneShotCmd(void)
{
    CFE_Status_t      Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_CANCEL_ONE_SHOT_CC;
    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0,
                  "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void CS_AppPipe_Test_EnableAllCSCmd(void)
{
    CFE_Status_t      Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_ENABLE_ALL_CS_CC;
    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0,
                  "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void CS_AppPipe_Test_DisableAllCSCmd(void)
{
    CFE_Status_t      Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_DISABLE_ALL_CS_CC;
    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0,
                  "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void CS_AppPipe_Test_EnableCfeCoreCmd(void)
{
    CFE_Status_t      Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_ENABLE_CFE_CORE_CC;
    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0,
                  "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void CS_AppPipe_Test_DisableCfeCoreCmd(void)
{
    CFE_Status_t      Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_DISABLE_CFE_CORE_CC;
    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0,
                  "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void CS_AppPipe_Test_ReportBaselineCfeCoreCmd(void)
{
    CFE_Status_t      Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_REPORT_BASELINE_CFE_CORE_CC;
    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0,
                  "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void CS_AppPipe_Test_RecomputeBaselineCfeCoreCmd(void)
{
    CFE_Status_t      Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_RECOMPUTE_BASELINE_CFE_CORE_CC;
    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0,
                  "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void CS_AppPipe_Test_EnableOSCmd(void)
{
    CFE_Status_t      Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_ENABLE_OS_CC;
    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0,
                  "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void CS_AppPipe_Test_DisableOSCmd(void)
{
    CFE_Status_t      Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_DISABLE_OS_CC;
    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0,
                  "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void CS_AppPipe_Test_ReportBaselineOSCmd(void)
{
    CFE_Status_t      Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_REPORT_BASELINE_OS_CC;
    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0,
                  "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void CS_AppPipe_Test_RecomputeBaselineOSCmd(void)
{
    CFE_Status_t      Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_RECOMPUTE_BASELINE_OS_CC;
    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0,
                  "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void CS_AppPipe_Test_EnableEepromCmd(void)
{
    CFE_Status_t      Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_ENABLE_EEPROM_CC;
    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0,
                  "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void CS_AppPipe_Test_DisableEepromCmd(void)
{
    CFE_Status_t      Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_DISABLE_EEPROM_CC;
    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0,
                  "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void CS_AppPipe_Test_ReportBaselineEntryIDEepromCmd(void)
{
    CFE_Status_t      Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_REPORT_BASELINE_ENTRY_ID_EEPROM_CC;
    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0,
                  "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void CS_AppPipe_Test_RecomputeBaselineEepromCmd(void)
{
    CFE_Status_t      Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_RECOMPUTE_BASELINE_EEPROM_CC;
    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0,
                  "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void CS_AppPipe_Test_EnableEntryIDEepromCmd(void)
{
    CFE_Status_t      Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_ENABLE_ENTRY_ID_EEPROM_CC;
    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0,
                  "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void CS_AppPipe_Test_DisableEntryIDEepromCmd(void)
{
    CFE_Status_t      Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_DISABLE_ENTRY_ID_EEPROM_CC;
    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0,
                  "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void CS_AppPipe_Test_GetEntryIDEepromCmd(void)
{
    CFE_Status_t      Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_GET_ENTRY_ID_EEPROM_CC;
    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0,
                  "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void CS_AppPipe_Test_EnableMemoryCmd(void)
{
    CFE_Status_t      Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_ENABLE_MEMORY_CC;
    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0,
                  "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void CS_AppPipe_Test_DisableMemoryCmd(void)
{
    CFE_Status_t      Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_DISABLE_MEMORY_CC;
    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0,
                  "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void CS_AppPipe_Test_ReportBaselineEntryIDMemoryCmd(void)
{
    CFE_Status_t      Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_REPORT_BASELINE_ENTRY_ID_MEMORY_CC;
    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0,
                  "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void CS_AppPipe_Test_RecomputeBaselineMemoryCmd(void)
{
    CFE_Status_t      Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_RECOMPUTE_BASELINE_MEMORY_CC;
    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0,
                  "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void CS_AppPipe_Test_EnableEntryIDMemoryCmd(void)
{
    CFE_Status_t      Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_ENABLE_ENTRY_ID_MEMORY_CC;
    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0,
                  "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void CS_AppPipe_Test_DisableEntryIDMemoryCmd(void)
{
    CFE_Status_t      Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_DISABLE_ENTRY_ID_MEMORY_CC;
    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0,
                  "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void CS_AppPipe_Test_GetEntryIDMemoryCmd(void)
{
    CFE_Status_t      Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_GET_ENTRY_ID_MEMORY_CC;
    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0,
                  "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void CS_AppPipe_Test_EnableTablesCmd(void)
{
    CFE_Status_t      Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_ENABLE_TABLES_CC;
    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0,
                  "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void CS_AppPipe_Test_DisableTablesCmd(void)
{
    CFE_Status_t      Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_DISABLE_TABLES_CC;
    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0,
                  "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void CS_AppPipe_Test_ReportBaselineTableCmd(void)
{
    CFE_Status_t      Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_REPORT_BASELINE_TABLE_CC;
    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0,
                  "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void CS_AppPipe_Test_RecomputeBaselineTableCmd(void)
{
    CFE_Status_t      Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_RECOMPUTE_BASELINE_TABLE_CC;
    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0,
                  "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void CS_AppPipe_Test_EnableNameTableCmd(void)
{
    CFE_Status_t      Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_ENABLE_NAME_TABLE_CC;
    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0,
                  "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void CS_AppPipe_Test_DisableNameTableCmd(void)
{
    CFE_Status_t      Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_DISABLE_NAME_TABLE_CC;
    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0,
                  "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void CS_AppPipe_Test_EnableAppsCmd(void)
{
    CFE_Status_t      Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_ENABLE_APPS_CC;
    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0,
                  "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void CS_AppPipe_Test_DisableAppsCmd(void)
{
    CFE_Status_t      Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_DISABLE_APPS_CC;
    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0,
                  "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void CS_AppPipe_Test_ReportBaselineAppCmd(void)
{
    CFE_Status_t      Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_REPORT_BASELINE_APP_CC;
    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0,
                  "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void CS_AppPipe_Test_RecomputeBaselineAppCmd(void)
{
    CFE_Status_t      Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_RECOMPUTE_BASELINE_APP_CC;
    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0,
                  "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void CS_AppPipe_Test_EnableNameAppCmd(void)
{
    CFE_Status_t      Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_ENABLE_NAME_APP_CC;
    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0,
                  "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void CS_AppPipe_Test_DisableNameAppCmd(void)
{
    CFE_Status_t      Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_DISABLE_NAME_APP_CC;
    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0,
                  "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void CS_AppPipe_Test_InvalidCCError(void)
{
    CFE_Status_t      Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;
    int32             strCmpResult;
    char              ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    snprintf(ExpectedEventString,
             CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Invalid ground command code: ID = 0x%%08lX, CC = %%d");

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = 99;
    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_CC_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1,
                  "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_AppPipe_Test_InvalidMIDError(void)
{
    CFE_Status_t   Result;
    UT_CmdBuf_t    CmdBuf;
    CFE_SB_MsgId_t TestMsgId;
    int32          strCmpResult;
    char           ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Invalid command pipe message ID: 0x%%08lX");

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_INVALID_MSG_ID;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_MID_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1,
                  "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_AppPipe_Test_SendHkCmd(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    TestMsgId = CFE_SB_ValueToMsgId(CS_SEND_HK_MID);
    FcnCode   = 99;
    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);
    /* Execute the function being tested */
    CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_SendHkCmd, 1);
}

void CS_AppPipe_Test_SendHkVerifyError(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    TestMsgId = CFE_SB_ValueToMsgId(CS_SEND_HK_MID);
    FcnCode   = 99;
    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, false);
    /* Execute the function being tested */
    CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_SendHkCmd, 0);
}

void CS_AppPipe_Test_BackgroundCheckCycleCmd(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    TestMsgId = CFE_SB_ValueToMsgId(CS_BACKGROUND_CYCLE_MID);
    FcnCode   = 99;
    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);
    /* Execute the function being tested */
    CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_BackgroundCheckCycleCmd, 1);
}

void CS_AppPipe_Test_BackgroundCheckCycleVerifyError(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    TestMsgId = CFE_SB_ValueToMsgId(CS_BACKGROUND_CYCLE_MID);
    FcnCode   = 99;
    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, false);
    /* Execute the function being tested */
    CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_BackgroundCheckCycleCmd, 0);
}

void CS_ProcessCmd_NoopCmd_Test(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_NOOP_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_NoopCmd, 1);
}

void CS_ProcessCmd_ResetCountersCmd_Test(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_RESET_COUNTERS_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_ResetCountersCmd, 1);
}

void CS_ProcessCmd_DisableAllCSCmd_Test(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_DISABLE_ALL_CS_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_DisableAllCSCmd, 1);
}

void CS_ProcessCmd_EnableAllCSCmd_Test(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_ENABLE_ALL_CS_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_EnableAllCSCmd, 1);
}

void CS_ProcessCmd_DisableCfeCoreCmd_Test(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_DISABLE_CFE_CORE_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_DisableCfeCoreCmd, 1);
}

void CS_ProcessCMD_EnableCfeCoreCmd_Test(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_ENABLE_CFE_CORE_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_EnableCfeCoreCmd, 1);
}

void CS_ProcessCmd_DisableOSCmd_Test(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_DISABLE_OS_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_DisableOSCmd, 1);
}

void CS_ProcessCmd_EnableOSCmd_Test(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_ENABLE_OS_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_EnableOSCmd, 1);
}

void CS_ProcessCmd_ReportBaselineCfeCoreCmd_Test(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_REPORT_BASELINE_CFE_CORE_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_ReportBaselineCfeCoreCmd, 1);
}

void CS_ProcessCmd_ReportBaselineOSCmd_Test(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_REPORT_BASELINE_OS_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_ReportBaselineOSCmd, 1);
}

void CS_ProcessCmd_RecomputeBaselineCfeCoreCmd_Test(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_RECOMPUTE_BASELINE_CFE_CORE_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_RecomputeBaselineCfeCoreCmd, 1);
}

void CS_ProcessCmd_RecomputeBaselineOSCmd_Test(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_RECOMPUTE_BASELINE_OS_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_RecomputeBaselineOSCmd, 1);
}

void CS_ProcessCmd_OneShotCmd_Test(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_ONE_SHOT_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_OneShotCmd, 1);
}

void CS_ProcessCmd_CancelOneShotCmd_Test(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_CANCEL_ONE_SHOT_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_CancelOneShotCmd, 1);
}

void CS_ProcessCmd_DisableTablesCmd_Test(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_DISABLE_TABLES_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_DisableTablesCmd, 1);
}

void CS_ProcessCmd_EnableTablesCmd_Test(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_ENABLE_TABLES_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_EnableTablesCmd, 1);
}

void CS_ProcessCmd_ReportBaselineTableCmd_Test(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_REPORT_BASELINE_TABLE_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_ReportBaselineTableCmd, 1);
}

void CS_ProcessCmd_RecomputeBaselineTableCmd_Test(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_RECOMPUTE_BASELINE_TABLE_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_RecomputeBaselineTableCmd, 1);
}

void CS_ProcessCmd_DisableNameTableCmd_Test(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_DISABLE_NAME_TABLE_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_DisableNameTableCmd, 1);
}

void CS_ProcessCmd_EnableNameTableCmd_Test(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_ENABLE_NAME_TABLE_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_EnableNameTableCmd, 1);
}

void CS_ProcessCmd_DisableAppsCmd_Test(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_DISABLE_APPS_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_DisableAppsCmd, 1);
}

void CS_ProcessCmd_EnableAppsCmd_Test(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_ENABLE_APPS_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_EnableAppsCmd, 1);
}

void CS_ProcessCmd_ReportBaselineAppCmd_Test(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_REPORT_BASELINE_APP_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_ReportBaselineAppCmd, 1);
}

void CS_ProcessCmd_RecomputeBaselineAppCmd_Test(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_RECOMPUTE_BASELINE_APP_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_RecomputeBaselineAppCmd, 1);
}

void CS_ProcessCmd_DisableNameAppCmd_Test(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_DISABLE_NAME_APP_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_DisableNameAppCmd, 1);
}

void CS_ProcessCmd_EnableNameAppCmd_Test(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_ENABLE_NAME_APP_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_EnableNameAppCmd, 1);
}

void CS_ProcessCmd_DisableMemoryCmd_Test(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_DISABLE_MEMORY_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_DisableMemoryCmd, 1);
}

void CS_ProcessCmd_EnableMemoryCmd_Test(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_ENABLE_MEMORY_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_EnableMemoryCmd, 1);
}

void CS_ProcessCmd_ReportBaselineEntryIDMemoryCmd_Test(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_REPORT_BASELINE_ENTRY_ID_MEMORY_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_ReportBaselineEntryIDMemoryCmd, 1);
}

void CS_ProcessCmd_RecomputeBaselineMemoryCmd_Test(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_RECOMPUTE_BASELINE_MEMORY_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_RecomputeBaselineMemoryCmd, 1);
}

void CS_ProcessCmd_EnableEntryIDMemoryCmd_Test(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_ENABLE_ENTRY_ID_MEMORY_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_EnableEntryIDMemoryCmd, 1);
}

void CS_ProcessCmd_DisableEntryIDMemoryCmd_Test(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_DISABLE_ENTRY_ID_MEMORY_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_DisableEntryIDMemoryCmd, 1);
}

void CS_ProcessCmd_GetEntryIDMemoryCmd_Test(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_GET_ENTRY_ID_MEMORY_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_GetEntryIDMemoryCmd, 1);
}

void CS_ProcessCmd_DisableEepromCmd_Test(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_DISABLE_EEPROM_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_DisableEepromCmd, 1);
}

void CS_ProcessCmd_EnableEepromCmd_Test(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_ENABLE_EEPROM_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_EnableEepromCmd, 1);
}

void CS_ProcessCmd_ReportBaselineEntryIDEepromCmd_Test(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_REPORT_BASELINE_ENTRY_ID_EEPROM_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_ReportBaselineEntryIDEepromCmd, 1);
}

void CS_ProcessCmd_RecomputeBaselineEepromCmd_Test(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_RECOMPUTE_BASELINE_EEPROM_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_RecomputeBaselineEepromCmd, 1);
}

void CS_ProcessCmd_EnableEntryIDEepromCmd_Test(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_ENABLE_ENTRY_ID_EEPROM_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_EnableEntryIDEepromCmd, 1);
}

void CS_ProcessCmd_DisableEntryIDEepromCmd_Test(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_DISABLE_ENTRY_ID_EEPROM_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_DisableEntryIDEepromCmd, 1);
}

void CS_ProcessCmd_GetEntryIDEepromCmd_Test(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_GET_ENTRY_ID_EEPROM_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, true);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_GetEntryIDEepromCmd, 1);
}

void CS_ProcessCmd_NoopCmd_Test_VerifyError(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_NOOP_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, false);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_NoopCmd, 0);
}

void CS_ProcessCmd_ResetCountersCmd_Test_VerifyError(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_RESET_COUNTERS_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, false);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_ResetCountersCmd, 0);
}

void CS_ProcessCmd_DisableAllCSCmd_Test_VerifyError(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_DISABLE_ALL_CS_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, false);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_DisableAllCSCmd, 0);
}

void CS_ProcessCmd_EnableAllCSCmd_Test_VerifyError(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_ENABLE_ALL_CS_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, false);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_EnableAllCSCmd, 0);
}

void CS_ProcessCmd_DisableCfeCoreCmd_Test_VerifyError(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_DISABLE_CFE_CORE_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, false);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_DisableCfeCoreCmd, 0);
}

void CS_ProcessCMD_EnableCfeCoreCmd_Test_VerifyError(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_ENABLE_CFE_CORE_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, false);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_EnableCfeCoreCmd, 0);
}

void CS_ProcessCmd_DisableOSCmd_Test_VerifyError(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_DISABLE_OS_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, false);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_DisableOSCmd, 0);
}

void CS_ProcessCmd_EnableOSCmd_Test_VerifyError(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_ENABLE_OS_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, false);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_EnableOSCmd, 0);
}

void CS_ProcessCmd_ReportBaselineCfeCoreCmd_Test_VerifyError(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_REPORT_BASELINE_CFE_CORE_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, false);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_ReportBaselineCfeCoreCmd, 0);
}

void CS_ProcessCmd_ReportBaselineOSCmd_Test_VerifyError(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_REPORT_BASELINE_OS_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, false);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_ReportBaselineOSCmd, 0);
}

void CS_ProcessCmd_RecomputeBaselineCfeCoreCmd_Test_VerifyError(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_RECOMPUTE_BASELINE_CFE_CORE_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, false);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_RecomputeBaselineCfeCoreCmd, 0);
}

void CS_ProcessCmd_RecomputeBaselineOSCmd_Test_VerifyError(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_RECOMPUTE_BASELINE_OS_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, false);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_RecomputeBaselineOSCmd, 0);
}

void CS_ProcessCmd_OneShotCmd_Test_VerifyError(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_ONE_SHOT_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, false);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_OneShotCmd, 0);
}

void CS_ProcessCmd_CancelOneShotCmd_Test_VerifyError(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_CANCEL_ONE_SHOT_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, false);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_CancelOneShotCmd, 0);
}

void CS_ProcessCmd_DisableTablesCmd_Test_VerifyError(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_DISABLE_TABLES_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, false);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_DisableTablesCmd, 0);
}

void CS_ProcessCmd_EnableTablesCmd_Test_VerifyError(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_ENABLE_TABLES_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, false);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_EnableTablesCmd, 0);
}

void CS_ProcessCmd_ReportBaselineTableCmd_Test_VerifyError(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_REPORT_BASELINE_TABLE_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, false);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_ReportBaselineTableCmd, 0);
}

void CS_ProcessCmd_RecomputeBaselineTableCmd_Test_VerifyError(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_RECOMPUTE_BASELINE_TABLE_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, false);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_RecomputeBaselineTableCmd, 0);
}

void CS_ProcessCmd_DisableNameTableCmd_Test_VerifyError(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_DISABLE_NAME_TABLE_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, false);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_DisableNameTableCmd, 0);
}

void CS_ProcessCmd_EnableNameTableCmd_Test_VerifyError(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_ENABLE_NAME_TABLE_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, false);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_EnableNameTableCmd, 0);
}

void CS_ProcessCmd_DisableAppsCmd_Test_VerifyError(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_DISABLE_APPS_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, false);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_DisableAppsCmd, 0);
}

void CS_ProcessCmd_EnableAppsCmd_Test_VerifyError(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_ENABLE_APPS_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, false);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_EnableAppsCmd, 0);
}

void CS_ProcessCmd_ReportBaselineAppCmd_Test_VerifyError(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_REPORT_BASELINE_APP_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, false);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_ReportBaselineAppCmd, 0);
}

void CS_ProcessCmd_RecomputeBaselineAppCmd_Test_VerifyError(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_RECOMPUTE_BASELINE_APP_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, false);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_RecomputeBaselineAppCmd, 0);
}

void CS_ProcessCmd_DisableNameAppCmd_Test_VerifyError(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_DISABLE_NAME_APP_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, false);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_DisableNameAppCmd, 0);
}

void CS_ProcessCmd_EnableNameAppCmd_Test_VerifyError(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_ENABLE_NAME_APP_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, false);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_EnableNameAppCmd, 0);
}

void CS_ProcessCmd_DisableMemoryCmd_Test_VerifyError(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_DISABLE_MEMORY_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, false);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_DisableMemoryCmd, 0);
}

void CS_ProcessCmd_EnableMemoryCmd_Test_VerifyError(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_ENABLE_MEMORY_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, false);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_EnableMemoryCmd, 0);
}

void CS_ProcessCmd_ReportBaselineEntryIDMemoryCmd_Test_VerifyError(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_REPORT_BASELINE_ENTRY_ID_MEMORY_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, false);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_ReportBaselineEntryIDMemoryCmd, 0);
}

void CS_ProcessCmd_RecomputeBaselineMemoryCmd_Test_VerifyError(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_RECOMPUTE_BASELINE_MEMORY_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, false);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_RecomputeBaselineMemoryCmd, 0);
}

void CS_ProcessCmd_EnableEntryIDMemoryCmd_Test_VerifyError(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_ENABLE_ENTRY_ID_MEMORY_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, false);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_EnableEntryIDMemoryCmd, 0);
}

void CS_ProcessCmd_DisableEntryIDMemoryCmd_Test_VerifyError(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_DISABLE_ENTRY_ID_MEMORY_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, false);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_DisableEntryIDMemoryCmd, 0);
}

void CS_ProcessCmd_GetEntryIDMemoryCmd_Test_VerifyError(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_GET_ENTRY_ID_MEMORY_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, false);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_GetEntryIDMemoryCmd, 0);
}

void CS_ProcessCmd_DisableEepromCmd_Test_VerifyError(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_DISABLE_EEPROM_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, false);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_DisableEepromCmd, 0);
}

void CS_ProcessCmd_EnableEepromCmd_Test_VerifyError(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_ENABLE_EEPROM_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, false);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_EnableEepromCmd, 0);
}

void CS_ProcessCmd_ReportBaselineEntryIDEepromCmd_Test_VerifyError(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_REPORT_BASELINE_ENTRY_ID_EEPROM_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, false);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_ReportBaselineEntryIDEepromCmd, 0);
}

void CS_ProcessCmd_RecomputeBaselineEepromCmd_Test_VerifyError(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_RECOMPUTE_BASELINE_EEPROM_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, false);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_RecomputeBaselineEepromCmd, 0);
}

void CS_ProcessCmd_EnableEntryIDEepromCmd_Test_VerifyError(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_ENABLE_ENTRY_ID_EEPROM_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, false);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_EnableEntryIDEepromCmd, 0);
}

void CS_ProcessCmd_DisableEntryIDEepromCmd_Test_VerifyError(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_DISABLE_ENTRY_ID_EEPROM_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, false);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_DisableEntryIDEepromCmd, 0);
}

void CS_ProcessCmd_GetEntryIDEepromCmd_Test_VerifyError(void)
{
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = CS_GET_ENTRY_ID_EEPROM_CC;

    UT_CS_SetupDispatchCheck(TestMsgId, FcnCode, false);
    /* Execute the function being tested */
    CS_ProcessCmd(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_STUB_COUNT(CS_GetEntryIDEepromCmd, 0);
}

void UtTest_Setup(void)
{
    UtTest_Add(CS_AppPipe_Test_TableUpdateErrors, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_TableUpdateErrors");
    UtTest_Add(CS_AppPipe_Test_BackgroundCycle, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_BackgroundCycle");
    UtTest_Add(CS_AppPipe_Test_NoopCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_NoopCmd");
    UtTest_Add(CS_AppPipe_Test_ResetCountersCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_ResetCountersCmd");
    UtTest_Add(CS_AppPipe_Test_OneShotCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_OneShotCmd");
    UtTest_Add(CS_AppPipe_Test_CancelOneShotCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_CancelOneShotCmd");
    UtTest_Add(CS_AppPipe_Test_EnableAllCSCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_EnableAllCSCmd");
    UtTest_Add(CS_AppPipe_Test_DisableAllCSCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_DisableAllCSCmd");
    UtTest_Add(CS_AppPipe_Test_EnableCfeCoreCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_EnableCfeCoreCmd");
    UtTest_Add(CS_AppPipe_Test_DisableCfeCoreCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_DisableCfeCoreCmd");
    UtTest_Add(CS_AppPipe_Test_ReportBaselineCfeCoreCmd,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_AppPipe_Test_ReportBaselineCfeCoreCmd");
    UtTest_Add(CS_AppPipe_Test_RecomputeBaselineCfeCoreCmd,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_AppPipe_Test_RecomputeBaselineCfeCoreCmd");
    UtTest_Add(CS_AppPipe_Test_EnableOSCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_EnableOSCmd");
    UtTest_Add(CS_AppPipe_Test_DisableOSCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_DisableOSCmd");
    UtTest_Add(CS_AppPipe_Test_ReportBaselineOSCmd,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_AppPipe_Test_ReportBaselineOSCmd");
    UtTest_Add(CS_AppPipe_Test_RecomputeBaselineOSCmd,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_AppPipe_Test_RecomputeBaselineOSCmd");
    UtTest_Add(CS_AppPipe_Test_EnableEepromCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_EnableEepromCmd");
    UtTest_Add(CS_AppPipe_Test_DisableEepromCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_DisableEepromCmd");
    UtTest_Add(CS_AppPipe_Test_ReportBaselineEntryIDEepromCmd,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_AppPipe_Test_ReportBaselineEntryIDEepromCmd");
    UtTest_Add(CS_AppPipe_Test_RecomputeBaselineEepromCmd,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_AppPipe_Test_RecomputeBaselineEepromCmd");
    UtTest_Add(CS_AppPipe_Test_EnableEntryIDEepromCmd,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_AppPipe_Test_EnableEntryIDEepromCmd");
    UtTest_Add(CS_AppPipe_Test_DisableEntryIDEepromCmd,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_AppPipe_Test_DisableEntryIDEepromCmd");
    UtTest_Add(CS_AppPipe_Test_GetEntryIDEepromCmd,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_AppPipe_Test_GetEntryIDEepromCmd");
    UtTest_Add(CS_AppPipe_Test_EnableMemoryCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_EnableMemoryCmd");
    UtTest_Add(CS_AppPipe_Test_DisableMemoryCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_DisableMemoryCmd");
    UtTest_Add(CS_AppPipe_Test_ReportBaselineEntryIDMemoryCmd,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_AppPipe_Test_ReportBaselineEntryIDMemoryCmd");
    UtTest_Add(CS_AppPipe_Test_RecomputeBaselineMemoryCmd,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_AppPipe_Test_RecomputeBaselineMemoryCmd");
    UtTest_Add(CS_AppPipe_Test_EnableEntryIDMemoryCmd,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_AppPipe_Test_EnableEntryIDMemoryCmd");
    UtTest_Add(CS_AppPipe_Test_DisableEntryIDMemoryCmd,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_AppPipe_Test_DisableEntryIDMemoryCmd");
    UtTest_Add(CS_AppPipe_Test_GetEntryIDMemoryCmd,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_AppPipe_Test_GetEntryIDMemoryCmd");
    UtTest_Add(CS_AppPipe_Test_EnableTablesCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_EnableTablesCmd");
    UtTest_Add(CS_AppPipe_Test_DisableTablesCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_DisableTablesCmd");
    UtTest_Add(CS_AppPipe_Test_ReportBaselineTableCmd,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_AppPipe_Test_ReportBaselineTableCmd");
    UtTest_Add(CS_AppPipe_Test_RecomputeBaselineTableCmd,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_AppPipe_Test_RecomputeBaselineTableCmd");
    UtTest_Add(CS_AppPipe_Test_EnableNameTableCmd,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_AppPipe_Test_EnableNameTableCmd");
    UtTest_Add(CS_AppPipe_Test_DisableNameTableCmd,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_AppPipe_Test_DisableNameTableCmd");
    UtTest_Add(CS_AppPipe_Test_EnableAppsCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_EnableAppsCmd");
    UtTest_Add(CS_AppPipe_Test_DisableAppsCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_DisableAppsCmd");
    UtTest_Add(CS_AppPipe_Test_ReportBaselineAppCmd,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_AppPipe_Test_ReportBaselineAppCmd");
    UtTest_Add(CS_AppPipe_Test_RecomputeBaselineAppCmd,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_AppPipe_Test_RecomputeBaselineAppCmd");
    UtTest_Add(CS_AppPipe_Test_EnableNameAppCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_EnableNameAppCmd");
    UtTest_Add(CS_AppPipe_Test_DisableNameAppCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_DisableNameAppCmd");
    UtTest_Add(CS_AppPipe_Test_InvalidCCError, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_InvalidCCError");
    UtTest_Add(CS_AppPipe_Test_InvalidMIDError, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_InvalidMIDError");

    UtTest_Add(CS_AppPipe_Test_SendHkCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_SendHkVerifyError");
    UtTest_Add(CS_AppPipe_Test_SendHkVerifyError, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_SendHkVerifyError");
    UtTest_Add(CS_AppPipe_Test_BackgroundCheckCycleCmd,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_AppPipe_Test_BackgroundCheckCycleCmd");
    UtTest_Add(CS_AppPipe_Test_BackgroundCheckCycleVerifyError,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_AppPipe_Test_BackgroundCheckCycleVerifyError");

    UtTest_Add(CS_ProcessCmd_NoopCmd_Test, CS_Test_Setup, CS_Test_TearDown, "CS_ProcessCmd_NoopCmd_Test");
    UtTest_Add(CS_ProcessCmd_ResetCountersCmd_Test,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCmd_ResetCountersCmd_Test");
    UtTest_Add(CS_ProcessCmd_DisableAllCSCmd_Test,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCmd_DisableAllCSCmd_Test");
    UtTest_Add(CS_ProcessCmd_EnableAllCSCmd_Test, CS_Test_Setup, CS_Test_TearDown, "CS_ProcessCmd_EnableAllCSCmd_Test");
    UtTest_Add(CS_ProcessCmd_DisableCfeCoreCmd_Test,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCmd_DisableCfeCoreCmd_Test");
    UtTest_Add(CS_ProcessCMD_EnableCfeCoreCmd_Test,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCMD_EnableCfeCoreCmd_Test");
    UtTest_Add(CS_ProcessCmd_DisableOSCmd_Test, CS_Test_Setup, CS_Test_TearDown, "CS_ProcessCmd_DisableOSCmd_Test");
    UtTest_Add(CS_ProcessCmd_EnableOSCmd_Test, CS_Test_Setup, CS_Test_TearDown, "CS_ProcessCmd_EnableOSCmd_Test");
    UtTest_Add(CS_ProcessCmd_ReportBaselineCfeCoreCmd_Test,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCmd_ReportBaselineCfeCoreCmd_Test");
    UtTest_Add(CS_ProcessCmd_ReportBaselineOSCmd_Test,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCmd_ReportBaselineOSCmd_Test");
    UtTest_Add(CS_ProcessCmd_RecomputeBaselineCfeCoreCmd_Test,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCmd_RecomputeBaselineCfeCoreCmd_Test");
    UtTest_Add(CS_ProcessCmd_RecomputeBaselineOSCmd_Test,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCmd_RecomputeBaselineOSCmd_Test");
    UtTest_Add(CS_ProcessCmd_OneShotCmd_Test, CS_Test_Setup, CS_Test_TearDown, "CS_ProcessCmd_OneShotCmd_Test");
    UtTest_Add(CS_ProcessCmd_CancelOneShotCmd_Test,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCmd_CancelOneShotCmd_Test");
    UtTest_Add(CS_ProcessCmd_DisableTablesCmd_Test,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCmd_DisableTablesCmd_Test");
    UtTest_Add(CS_ProcessCmd_EnableTablesCmd_Test,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCmd_EnableTablesCmd_Test");
    UtTest_Add(CS_ProcessCmd_ReportBaselineTableCmd_Test,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCmd_ReportBaselineTableCmd_Test");
    UtTest_Add(CS_ProcessCmd_RecomputeBaselineTableCmd_Test,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCmd_RecomputeBaselineTableCmd_Test");
    UtTest_Add(CS_ProcessCmd_DisableNameTableCmd_Test,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCmd_DisableNameTableCmd_Test");
    UtTest_Add(CS_ProcessCmd_EnableNameTableCmd_Test,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCmd_EnableNameTableCmd_Test");
    UtTest_Add(CS_ProcessCmd_DisableAppsCmd_Test, CS_Test_Setup, CS_Test_TearDown, "CS_ProcessCmd_DisableAppsCmd_Test");
    UtTest_Add(CS_ProcessCmd_EnableAppsCmd_Test, CS_Test_Setup, CS_Test_TearDown, "CS_ProcessCmd_EnableAppsCmd_Test");
    UtTest_Add(CS_ProcessCmd_ReportBaselineAppCmd_Test,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCmd_ReportBaselineAppCmd_Test");
    UtTest_Add(CS_ProcessCmd_RecomputeBaselineAppCmd_Test,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCmd_RecomputeBaselineAppCmd_Test");
    UtTest_Add(CS_ProcessCmd_DisableNameAppCmd_Test,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCmd_DisableNameAppCmd_Test");
    UtTest_Add(CS_ProcessCmd_EnableNameAppCmd_Test,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCmd_EnableNameAppCmd_Test");
    UtTest_Add(CS_ProcessCmd_DisableMemoryCmd_Test,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCmd_DisableMemoryCmd_Test");
    UtTest_Add(CS_ProcessCmd_EnableMemoryCmd_Test,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCmd_EnableMemoryCmd_Test");
    UtTest_Add(CS_ProcessCmd_ReportBaselineEntryIDMemoryCmd_Test,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCmd_ReportBaselineEntryIDMemoryCmd_Test");
    UtTest_Add(CS_ProcessCmd_RecomputeBaselineMemoryCmd_Test,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCmd_RecomputeBaselineMemoryCmd_Test");
    UtTest_Add(CS_ProcessCmd_EnableEntryIDMemoryCmd_Test,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCmd_EnableEntryIDMemoryCmd_Test");
    UtTest_Add(CS_ProcessCmd_DisableEntryIDMemoryCmd_Test,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCmd_DisableEntryIDMemoryCmd_Test");
    UtTest_Add(CS_ProcessCmd_GetEntryIDMemoryCmd_Test,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCmd_GetEntryIDMemoryCmd_Test");
    UtTest_Add(CS_ProcessCmd_DisableEepromCmd_Test,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCmd_DisableEepromCmd_Test");
    UtTest_Add(CS_ProcessCmd_EnableEepromCmd_Test,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCmd_EnableEepromCmd_Test");
    UtTest_Add(CS_ProcessCmd_ReportBaselineEntryIDEepromCmd_Test,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCmd_ReportBaselineEntryIDEepromCmd_Test");
    UtTest_Add(CS_ProcessCmd_RecomputeBaselineEepromCmd_Test,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCmd_RecomputeBaselineEepromCmd_Test");
    UtTest_Add(CS_ProcessCmd_EnableEntryIDEepromCmd_Test,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCmd_EnableEntryIDEepromCmd_Test");
    UtTest_Add(CS_ProcessCmd_DisableEntryIDEepromCmd_Test,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCmd_DisableEntryIDEepromCmd_Test");
    UtTest_Add(CS_ProcessCmd_GetEntryIDEepromCmd_Test,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCmd_GetEntryIDEepromCmd_Test");

    UtTest_Add(CS_ProcessCmd_NoopCmd_Test_VerifyError,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCmd_NoopCmd_Test_VerifyError");
    UtTest_Add(CS_ProcessCmd_ResetCountersCmd_Test_VerifyError,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCmd_ResetCountersCmd_Test_VerifyError");
    UtTest_Add(CS_ProcessCmd_DisableAllCSCmd_Test_VerifyError,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCmd_DisableAllCSCmd_Test_VerifyError");
    UtTest_Add(CS_ProcessCmd_EnableAllCSCmd_Test_VerifyError,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCmd_EnableAllCSCmd_Test_VerifyError");
    UtTest_Add(CS_ProcessCmd_DisableCfeCoreCmd_Test_VerifyError,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCmd_DisableCfeCoreCmd_Test_VerifyError");
    UtTest_Add(CS_ProcessCMD_EnableCfeCoreCmd_Test_VerifyError,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCMD_EnableCfeCoreCmd_Test_VerifyError");
    UtTest_Add(CS_ProcessCmd_DisableOSCmd_Test_VerifyError,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCmd_DisableOSCmd_Test_VerifyError");
    UtTest_Add(CS_ProcessCmd_EnableOSCmd_Test_VerifyError,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCmd_EnableOSCmd_Test_VerifyError");
    UtTest_Add(CS_ProcessCmd_ReportBaselineCfeCoreCmd_Test_VerifyError,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCmd_ReportBaselineCfeCoreCmd_Test_VerifyError");
    UtTest_Add(CS_ProcessCmd_ReportBaselineOSCmd_Test_VerifyError,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCmd_ReportBaselineOSCmd_Test_VerifyError");
    UtTest_Add(CS_ProcessCmd_RecomputeBaselineCfeCoreCmd_Test_VerifyError,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCmd_RecomputeBaselineCfeCoreCmd_Test_VerifyError");
    UtTest_Add(CS_ProcessCmd_RecomputeBaselineOSCmd_Test_VerifyError,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCmd_RecomputeBaselineOSCmd_Test_VerifyError");
    UtTest_Add(CS_ProcessCmd_OneShotCmd_Test_VerifyError,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCmd_OneShotCmd_Test_VerifyError");
    UtTest_Add(CS_ProcessCmd_CancelOneShotCmd_Test_VerifyError,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCmd_CancelOneShotCmd_Test_VerifyError");
    UtTest_Add(CS_ProcessCmd_DisableTablesCmd_Test_VerifyError,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCmd_DisableTablesCmd_Test_VerifyError");
    UtTest_Add(CS_ProcessCmd_EnableTablesCmd_Test_VerifyError,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCmd_EnableTablesCmd_Test_VerifyError");
    UtTest_Add(CS_ProcessCmd_ReportBaselineTableCmd_Test_VerifyError,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCmd_ReportBaselineTableCmd_Test_VerifyError");
    UtTest_Add(CS_ProcessCmd_RecomputeBaselineTableCmd_Test_VerifyError,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCmd_RecomputeBaselineTableCmd_Test_VerifyError");
    UtTest_Add(CS_ProcessCmd_DisableNameTableCmd_Test_VerifyError,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCmd_DisableNameTableCmd_Test_VerifyError");
    UtTest_Add(CS_ProcessCmd_EnableNameTableCmd_Test_VerifyError,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCmd_EnableNameTableCmd_Test_VerifyError");
    UtTest_Add(CS_ProcessCmd_DisableAppsCmd_Test_VerifyError,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCmd_DisableAppsCmd_Test_VerifyError");
    UtTest_Add(CS_ProcessCmd_EnableAppsCmd_Test_VerifyError,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCmd_EnableAppsCmd_Test_VerifyError");
    UtTest_Add(CS_ProcessCmd_ReportBaselineAppCmd_Test_VerifyError,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCmd_ReportBaselineAppCmd_Test_VerifyError");
    UtTest_Add(CS_ProcessCmd_RecomputeBaselineAppCmd_Test_VerifyError,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCmd_RecomputeBaselineAppCmd_Test_VerifyError");
    UtTest_Add(CS_ProcessCmd_DisableNameAppCmd_Test_VerifyError,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCmd_DisableNameAppCmd_Test_VerifyError");
    UtTest_Add(CS_ProcessCmd_EnableNameAppCmd_Test_VerifyError,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCmd_EnableNameAppCmd_Test_VerifyError");
    UtTest_Add(CS_ProcessCmd_DisableMemoryCmd_Test_VerifyError,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCmd_DisableMemoryCmd_Test_VerifyError");
    UtTest_Add(CS_ProcessCmd_EnableMemoryCmd_Test_VerifyError,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCmd_EnableMemoryCmd_Test_VerifyError");
    UtTest_Add(CS_ProcessCmd_ReportBaselineEntryIDMemoryCmd_Test_VerifyError,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCmd_ReportBaselineEntryIDMemoryCmd_Test_VerifyError");
    UtTest_Add(CS_ProcessCmd_RecomputeBaselineMemoryCmd_Test_VerifyError,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCmd_RecomputeBaselineMemoryCmd_Test_VerifyError");
    UtTest_Add(CS_ProcessCmd_EnableEntryIDMemoryCmd_Test_VerifyError,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCmd_EnableEntryIDMemoryCmd_Test_VerifyError");
    UtTest_Add(CS_ProcessCmd_DisableEntryIDMemoryCmd_Test_VerifyError,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCmd_DisableEntryIDMemoryCmd_Test_VerifyError");
    UtTest_Add(CS_ProcessCmd_GetEntryIDMemoryCmd_Test_VerifyError,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCmd_GetEntryIDMemoryCmd_Test_VerifyError");
    UtTest_Add(CS_ProcessCmd_DisableEepromCmd_Test_VerifyError,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCmd_DisableEepromCmd_Test_VerifyError");
    UtTest_Add(CS_ProcessCmd_EnableEepromCmd_Test_VerifyError,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCmd_EnableEepromCmd_Test_VerifyError");
    UtTest_Add(CS_ProcessCmd_ReportBaselineEntryIDEepromCmd_Test_VerifyError,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCmd_ReportBaselineEntryIDEepromCmd_Test_VerifyError");
    UtTest_Add(CS_ProcessCmd_RecomputeBaselineEepromCmd_Test_VerifyError,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCmd_RecomputeBaselineEepromCmd_Test_VerifyError");
    UtTest_Add(CS_ProcessCmd_EnableEntryIDEepromCmd_Test_VerifyError,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCmd_EnableEntryIDEepromCmd_Test_VerifyError");
    UtTest_Add(CS_ProcessCmd_DisableEntryIDEepromCmd_Test_VerifyError,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCmd_DisableEntryIDEepromCmd_Test_VerifyError");
    UtTest_Add(CS_ProcessCmd_GetEntryIDEepromCmd_Test_VerifyError,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessCmd_GetEntryIDEepromCmd_Test_VerifyError");
}
