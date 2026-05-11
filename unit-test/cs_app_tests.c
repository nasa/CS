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

/* cs_app_tests globals */
uint8 call_count_CFE_EVS_SendEvent;

/*
 * Function Definitions
 */

CFE_Status_t
CS_APP_TEST_CFE_ES_RunLoop_Hook(void *UserObj, int32 StubRetcode, uint32 CallCount, const UT_StubContext_t *Context)
{
    CS_AppData.RunStatus = CFE_ES_RunStatus_SYS_EXCEPTION;

    return false;
}

void CS_APP_TEST_CFE_ES_RestoreFromCDS_Handler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    uint8 *DataStoreBuffer = (uint8 *)UT_Hook_GetArgValueByName(Context, "RestoreToMemory", uint8 *);

    DataStoreBuffer[0] = CS_ChecksumState_ENABLED;
    DataStoreBuffer[1] = CS_ChecksumState_ENABLED;
    DataStoreBuffer[2] = CS_ChecksumState_ENABLED;
    DataStoreBuffer[3] = CS_ChecksumState_ENABLED;

    DataStoreBuffer[4] = CS_ChecksumState_ENABLED;
    DataStoreBuffer[5] = CS_ChecksumState_ENABLED;
}

void CS_App_TestCmdTlmAlign(void)
{
    /* Ensures the command and telemetry structures are padded to and aligned to 32-bits */
#define CMD_STRUCT_DATA_IS_32_ALIGNED(x) ((sizeof(x) - sizeof(CFE_MSG_CommandHeader_t)) % 4) == 0
#define TLM_STRUCT_DATA_IS_32_ALIGNED(x) ((sizeof(x) - sizeof(CFE_MSG_TelemetryHeader_t)) % 4) == 0

    UtAssert_True(TLM_STRUCT_DATA_IS_32_ALIGNED(CS_HkPacket_t), "CS_HkPacket_t is 32-bit aligned");
    UtAssert_True(CMD_STRUCT_DATA_IS_32_ALIGNED(CS_SendHkCmd_t), "CS_SendHkCmd_t is 32-bit aligned");
    UtAssert_True(CMD_STRUCT_DATA_IS_32_ALIGNED(CS_NoopCmd_t), "CS_NoopCmd_t is 32-bit aligned");
    UtAssert_True(CMD_STRUCT_DATA_IS_32_ALIGNED(CS_ResetCountersCmd_t), "CS_ResetCountersCmd_t is 32-bit aligned");
    UtAssert_True(CMD_STRUCT_DATA_IS_32_ALIGNED(CS_BackgroundCheckCycleCmd_t),
                  "CS_BackgroundCheckCycleCmd_t is 32-bit aligned");
    UtAssert_True(CMD_STRUCT_DATA_IS_32_ALIGNED(CS_DisableAllCSCmd_t), "CS_DisableAllCSCmd_t is 32-bit aligned");
    UtAssert_True(CMD_STRUCT_DATA_IS_32_ALIGNED(CS_EnableAllCSCmd_t), "CS_EnableAllCSCmd_t is 32-bit aligned");
    UtAssert_True(CMD_STRUCT_DATA_IS_32_ALIGNED(CS_DisableCfeCoreCmd_t), "CS_DisableCfeCoreCmd_t is 32-bit aligned");
    UtAssert_True(CMD_STRUCT_DATA_IS_32_ALIGNED(CS_EnableCfeCoreCmd_t), "CS_EnableCfeCoreCmd_t is 32-bit aligned");
    UtAssert_True(CMD_STRUCT_DATA_IS_32_ALIGNED(CS_DisableOSCmd_t), "CS_DisableOSCmd_t is 32-bit aligned");
    UtAssert_True(CMD_STRUCT_DATA_IS_32_ALIGNED(CS_EnableOSCmd_t), "CS_EnableOSCmd_t is 32-bit aligned");
    UtAssert_True(CMD_STRUCT_DATA_IS_32_ALIGNED(CS_ReportBaselineCfeCoreCmd_t),
                  "CS_ReportBaselineCfeCoreCmd_t is 32-bit aligned");
    UtAssert_True(CMD_STRUCT_DATA_IS_32_ALIGNED(CS_ReportBaselineOSCmd_t),
                  "CS_ReportBaselineOSCmd_t is 32-bit aligned");
    UtAssert_True(CMD_STRUCT_DATA_IS_32_ALIGNED(CS_RecomputeBaselineCfeCoreCmd_t),
                  "CS_RecomputeBaselineCfeCoreCmd_t is 32-bit aligned");
    UtAssert_True(CMD_STRUCT_DATA_IS_32_ALIGNED(CS_RecomputeBaselineOSCmd_t),
                  "CS_RecomputeBaselineOSCmd_t is 32-bit aligned");
    UtAssert_True(CMD_STRUCT_DATA_IS_32_ALIGNED(CS_CancelOneShotCmd_t), "CS_CancelOneShotCmd_t is 32-bit aligned");
    UtAssert_True(CMD_STRUCT_DATA_IS_32_ALIGNED(CS_DisableAppsCmd_t), "CS_DisableAppsCmd_t is 32-bit aligned");
    UtAssert_True(CMD_STRUCT_DATA_IS_32_ALIGNED(CS_EnableAppsCmd_t), "CS_EnableAppsCmd_t is 32-bit aligned");
    UtAssert_True(CMD_STRUCT_DATA_IS_32_ALIGNED(CS_DisableEepromCmd_t), "CS_DisableEepromCmd_t is 32-bit aligned");
    UtAssert_True(CMD_STRUCT_DATA_IS_32_ALIGNED(CS_EnableEepromCmd_t), "CS_EnableEepromCmd_t is 32-bit aligned");
    UtAssert_True(CMD_STRUCT_DATA_IS_32_ALIGNED(CS_DisableMemoryCmd_t), "CS_DisableMemoryCmd_t is 32-bit aligned");
    UtAssert_True(CMD_STRUCT_DATA_IS_32_ALIGNED(CS_EnableMemoryCmd_t), "CS_EnableMemoryCmd_t is 32-bit aligned");
    UtAssert_True(CMD_STRUCT_DATA_IS_32_ALIGNED(CS_DisableTablesCmd_t), "CS_DisableTablesCmd_t is 32-bit aligned");
    UtAssert_True(CMD_STRUCT_DATA_IS_32_ALIGNED(CS_EnableTablesCmd_t), "CS_EnableTablesCmd_t is 32-bit aligned");
    UtAssert_True(CMD_STRUCT_DATA_IS_32_ALIGNED(CS_GetEntryIDEepromCmd_t),
                  "CS_GetEntryIDEepromCmd_t is 32-bit aligned");
    UtAssert_True(CMD_STRUCT_DATA_IS_32_ALIGNED(CS_GetEntryIDMemoryCmd_t),
                  "CS_GetEntryIDMemoryCmd_t is 32-bit aligned");
    UtAssert_True(CMD_STRUCT_DATA_IS_32_ALIGNED(CS_ReportBaselineEntryIDEepromCmd_t),
                  "CS_ReportBaselineEntryIDEepromCmd_t is 32-bit aligned");
    UtAssert_True(CMD_STRUCT_DATA_IS_32_ALIGNED(CS_DisableEntryIDEepromCmd_t),
                  "CS_DisableEntryIDEepromCmd_t is 32-bit aligned");
    UtAssert_True(CMD_STRUCT_DATA_IS_32_ALIGNED(CS_RecomputeBaselineEepromCmd_t),
                  "CS_RecomputeBaselineEepromCmd_t is 32-bit aligned");
    UtAssert_True(CMD_STRUCT_DATA_IS_32_ALIGNED(CS_EnableEntryIDEepromCmd_t),
                  "CS_EnableEntryIDEepromCmd_t is 32-bit aligned");
    UtAssert_True(CMD_STRUCT_DATA_IS_32_ALIGNED(CS_ReportBaselineEntryIDMemoryCmd_t),
                  "CS_ReportBaselineEntryIDMemoryCmd_t is 32-bit aligned");
    UtAssert_True(CMD_STRUCT_DATA_IS_32_ALIGNED(CS_DisableEntryIDMemoryCmd_t),
                  "CS_DisableEntryIDMemoryCmd_t is 32-bit aligned");
    UtAssert_True(CMD_STRUCT_DATA_IS_32_ALIGNED(CS_RecomputeBaselineMemoryCmd_t),
                  "CS_RecomputeBaselineMemoryCmd_t is 32-bit aligned");
    UtAssert_True(CMD_STRUCT_DATA_IS_32_ALIGNED(CS_EnableEntryIDMemoryCmd_t),
                  "CS_EnableEntryIDMemoryCmd_t is 32-bit aligned");
    UtAssert_True(CMD_STRUCT_DATA_IS_32_ALIGNED(CS_ReportBaselineTableCmd_t),
                  "CS_ReportBaselineTableCmd_t is 32-bit aligned");
    UtAssert_True(CMD_STRUCT_DATA_IS_32_ALIGNED(CS_RecomputeBaselineTableCmd_t),
                  "CS_RecomputeBaselineTableCmd_t is 32-bit aligned");
    UtAssert_True(CMD_STRUCT_DATA_IS_32_ALIGNED(CS_DisableNameTableCmd_t),
                  "CS_DisableNameTableCmd_t is 32-bit aligned");
    UtAssert_True(CMD_STRUCT_DATA_IS_32_ALIGNED(CS_EnableNameTableCmd_t), "CS_EnableNameTableCmd_t is 32-bit aligned");
    UtAssert_True(CMD_STRUCT_DATA_IS_32_ALIGNED(CS_ReportBaselineAppCmd_t),
                  "CS_ReportBaselineAppCmd_t is 32-bit aligned");
    UtAssert_True(CMD_STRUCT_DATA_IS_32_ALIGNED(CS_RecomputeBaselineAppCmd_t),
                  "CS_RecomputeBaselineAppCmd_t is 32-bit aligned");
    UtAssert_True(CMD_STRUCT_DATA_IS_32_ALIGNED(CS_DisableNameAppCmd_t), "CS_DisableNameAppCmd_t is 32-bit aligned");
    UtAssert_True(CMD_STRUCT_DATA_IS_32_ALIGNED(CS_EnableNameAppCmd_t), "CS_EnableNameAppCmd_t is 32-bit aligned");
    UtAssert_True(CMD_STRUCT_DATA_IS_32_ALIGNED(CS_OneShotCmd_t), "CS_OneShotCmd_t is 32-bit aligned");
}

void CS_AppMain_Test_Nominal(void)
{
    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
    char  ExpectedSysLogString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "App terminating, RunStatus:0x%%08X");

    snprintf(ExpectedSysLogString,
             CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS App terminating, RunStatus:0x%%08X, RC:0x%%08X\n");

    /* Set to prevent segmentation fault */
    UT_SetDeferredRetcode(UT_KEY(CFE_MSG_GetMsgId), 1, 99);

    /* Set to prevent unintended error messages */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Load), CFE_SUCCESS);

    /* Set to make while-loop run exactly once */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RunLoop), 2, false);

    /* Set to satisfy subsequent condition "Result == CFE_SUCCESS" */
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_ReceiveBuffer), 1, CFE_SUCCESS);

    /* Execute the function being tested */
    CS_AppMain();

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, CS_EXIT_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[1].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[1].Spec);

    strCmpResult = strncmp(ExpectedSysLogString, context_CFE_ES_WriteToSysLog.Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Sys Log string matched expected result, '%s'", context_CFE_ES_WriteToSysLog.Spec);

    /* 2 Messages Tested elsewhere so we can ignore them here. INFO:CS Initialized  and ERROR:Invalid Command pipe  */
    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 2,
                  "CFE_EVS_SendEvent was called %u time(s), expected 2",
                  call_count_CFE_EVS_SendEvent);

    /* Generates 2 event messages we don't care about in this test */
}

void CS_AppMain_Test_AppInitError(void)
{
    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
    char  ExpectedSysLogString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "App terminating, RunStatus:0x%%08X, RC:0x%%08X");

    snprintf(ExpectedSysLogString,
             CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS App terminating, RunStatus:0x%%08X, RC:0x%%08X\n");

    /* Set to make subfunction CS_AppInit return -1 */
    UT_SetDeferredRetcode(UT_KEY(CS_SbInit), 1, -1);

    /* Execute the function being tested */
    CS_AppMain();

    /* Verify results */
    UtAssert_True(CS_AppData.RunStatus == CFE_ES_RunStatus_APP_ERROR,
                  "CS_AppData.RunStatus == CFE_ES_RunStatus_APP_ERROR");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_EXIT_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    strCmpResult = strncmp(ExpectedSysLogString, context_CFE_ES_WriteToSysLog.Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Sys Log string matched expected result, '%s'", context_CFE_ES_WriteToSysLog.Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1,
                  "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_AppMain_Test_SysException(void)
{
    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
    char  ExpectedSysLogString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "App terminating, RunStatus:0x%%08X, RC:0x%%08X");

    snprintf(ExpectedSysLogString,
             CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS App terminating, RunStatus:0x%%08X, RC:0x%%08X\n");

    /* Set to make loop not execute */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RunLoop), 1, false);

    /* Set to prevent unintended error messages */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Load), CFE_SUCCESS);

    /* Set to satisfy subsequent condition "Result != CFE_SUCCESS" */
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_ReceiveBuffer), 1, -1);

    /* Set to make CS_AppData.RunStatus == CFE_ES_RunStatus_SYS_EXCEPTION */
    UT_SetHookFunction(UT_KEY(CFE_ES_RunLoop), CS_APP_TEST_CFE_ES_RunLoop_Hook, NULL);

    /* Execute the function being tested */
    CS_AppMain();

    /* Verify results */
    UtAssert_True(CS_AppData.RunStatus == CFE_ES_RunStatus_SYS_EXCEPTION,
                  "CS_AppData.RunStatus == CFE_ES_RunStatus_SYS_EXCEPTION");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, CS_EXIT_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[1].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[1].Spec);

    strCmpResult = strncmp(ExpectedSysLogString, context_CFE_ES_WriteToSysLog.Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Sys Log string matched expected result, '%s'", context_CFE_ES_WriteToSysLog.Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 2,
                  "CFE_EVS_SendEvent was called %u time(s), expected 2",
                  call_count_CFE_EVS_SendEvent);
    /* Generates 1 event message we don't care about in this test */
}

void CS_AppMain_Test_RcvMsgError(void)
{
    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
    char  ExpectedSysLogString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "App terminating, RunStatus:0x%%08X, RC:0x%%08X");

    snprintf(ExpectedSysLogString,
             CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS App terminating, RunStatus:0x%%08X, RC:0x%%08X\n");

    /* Set to make loop execute exactly once */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RunLoop), 1, true);

    /* Set to prevent unintended error messages */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Load), CFE_SUCCESS);

    /* Set to satisfy subsequent condition "Result != CFE_SUCCESS" */
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_ReceiveBuffer), 1, -1);

    /* Execute the function being tested */
    CS_AppMain();

    /* Verify results */
    UtAssert_True(CS_AppData.RunStatus == CFE_ES_RunStatus_APP_ERROR,
                  "CS_AppData.RunStatus == CFE_ES_RunStatus_APP_ERROR");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, CS_EXIT_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[1].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[1].Spec);

    strCmpResult = strncmp(ExpectedSysLogString, context_CFE_ES_WriteToSysLog.Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Sys Log string matched expected result, '%s'", context_CFE_ES_WriteToSysLog.Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 2,
                  "CFE_EVS_SendEvent was called %u time(s), expected 2",
                  call_count_CFE_EVS_SendEvent);
    /* Generates 1 event message we don't care about in this test */
}

void CS_AppMain_Test_RcvMsgTimeout(void)
{
    CFE_SB_MsgId_t TestMsgId;
    size_t         MsgSize;
    uint8          Packet[4];
    int32          strCmpResult;
    char           ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
    char           ExpectedSysLogString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "App terminating, RunStatus:0x%%08X, RC:0x%%08X");

    snprintf(ExpectedSysLogString,
             CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS App terminating, RunStatus:0x%%08X, RC:0x%%08X\n");

    /* Set to make loop execute exactly once */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RunLoop), 1, true);

    TestMsgId = CFE_SB_ValueToMsgId(CS_SEND_HK_MID);
    MsgSize   = sizeof(Packet);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &MsgSize, sizeof(MsgSize), false);
    UT_SetDataBuffer(UT_KEY(CFE_SB_ReceiveBuffer), &Packet, sizeof(Packet), false);

    /* Set to make CS_AppPipe return -1 */
    UT_SetDeferredRetcode(UT_KEY(CS_HandleRoutineTableUpdates), 1, -1);

    /* Set to prevent unintended error messages */

    /* Set to satisfy subsequent condition "Result == CFE_SB_TIME_OUT" */
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_ReceiveBuffer), 1, CFE_SB_TIME_OUT);

    /* Execute the function being tested */
    CS_AppMain();

    /* Verify results */
    UtAssert_True(CS_AppData.RunStatus == CFE_ES_RunStatus_APP_ERROR,
                  "CS_AppData.RunStatus == CFE_ES_RunStatus_APP_ERROR");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, CS_EXIT_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[1].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[1].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 2,
                  "CFE_EVS_SendEvent was called %u time(s), expected 2",
                  call_count_CFE_EVS_SendEvent);
    /* Generates 3 event messages we don't care about in this test */

    strCmpResult = strncmp(ExpectedSysLogString, context_CFE_ES_WriteToSysLog.Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Sys Log string matched expected result, '%s'", context_CFE_ES_WriteToSysLog.Spec);
}

void CS_AppMain_Test_RcvNoMsg(void)
{
    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
    char  ExpectedSysLogString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "App terminating, RunStatus:0x%%08X");

    snprintf(ExpectedSysLogString,
             CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS App terminating, RunStatus:0x%%08X, RC:0x%%08X\n");

    /* Set to prevent segmentation fault */
    UT_SetDeferredRetcode(UT_KEY(CFE_MSG_GetMsgId), 1, 99);

    /* Set to prevent unintended error messages */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Load), CFE_SUCCESS);

    /* Set to make while-loop run exactly once */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RunLoop), 1, true);

    /* Set to satisfy subsequent condition "Result == CFE_SB_NO_MESSAGE" */
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_ReceiveBuffer), 1, CFE_SB_NO_MESSAGE);

    /* Execute the function being tested */
    CS_AppMain();

    /* Verify results */

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, CS_EXIT_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[1].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[1].Spec);

    strCmpResult = strncmp(ExpectedSysLogString, context_CFE_ES_WriteToSysLog.Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Sys Log string matched expected result, '%s'", context_CFE_ES_WriteToSysLog.Spec);

    /* 2 Messages Tested elsewhere so we can ignore them here. INFO:CS Initialized  and ERROR:Invalid Command pipe  */
    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 2,
                  "CFE_EVS_SendEvent was called %u time(s), expected 2",
                  call_count_CFE_EVS_SendEvent);

    /* Generates 2 event messages we don't care about in this test */
}

void CS_AppMain_Test_RcvNullBufPtr(void)
{
    CFE_SB_MsgId_t TestMsgId;
    size_t         MsgSize;
    int32          strCmpResult;
    char           ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
    char           ExpectedSysLogString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
    CS_NoopCmd_t  *PacketPtr = NULL;

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "App terminating, RunStatus:0x%%08X, RC:0x%%08X");

    snprintf(ExpectedSysLogString,
             CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS App terminating, RunStatus:0x%%08X, RC:0x%%08X\n");

    /* Set to make loop execute exactly once */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RunLoop), 1, true);

    TestMsgId = CFE_SB_ValueToMsgId(CS_SEND_HK_MID);
    MsgSize   = sizeof(*PacketPtr);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &MsgSize, sizeof(MsgSize), false);
    UT_SetDataBuffer(UT_KEY(CFE_SB_ReceiveBuffer), &PacketPtr, sizeof(PacketPtr), false);

    /* Set to make CS_AppPipe return -1 */
    UT_SetDeferredRetcode(UT_KEY(CS_HandleRoutineTableUpdates), 1, -1);

    /* Set to satisfy subsequent condition "BufPtr == NULL" */
    UT_SetDataBuffer(UT_KEY(CFE_SB_ReceiveBuffer), PacketPtr, sizeof(*PacketPtr), false);

    /* Execute the function being tested */
    CS_AppMain();

    /* Verify results */
    UtAssert_True(CS_AppData.RunStatus == 1, "CS_AppData.RunStatus == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 2,
                  "CFE_EVS_SendEvent was called %u time(s), expected 2",
                  call_count_CFE_EVS_SendEvent);
    /* Generates 1 event messages we don't care about in this test */

    strCmpResult = strncmp(ExpectedSysLogString, context_CFE_ES_WriteToSysLog.Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Sys Log string matched expected result, '%s'", context_CFE_ES_WriteToSysLog.Spec);
}

void CS_AppMain_Test_AppPipeError(void)
{
    CS_NoopCmd_t  Packet;
    CS_NoopCmd_t *PacketPtr = &Packet;
    int32         strCmpResult;
    char          ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
    char          ExpectedSysLogString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "App terminating, RunStatus:0x%%08X, RC:0x%%08X");

    snprintf(ExpectedSysLogString,
             CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS App terminating, RunStatus:0x%%08X, RC:0x%%08X\n");

    /* Set to make loop execute exactly once */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RunLoop), 1, true);
    UT_SetDataBuffer(UT_KEY(CFE_SB_ReceiveBuffer), &PacketPtr, sizeof(PacketPtr), false);

    /* Set to make CS_AppPipe return -1 */
    UT_SetDeferredRetcode(UT_KEY(CS_AppPipe), 1, -1);

    /* Set to prevent unintended error messages */

    /* Set to satisfy subsequent condition "Result == CFE_SUCCESS" */
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_ReceiveBuffer), 1, CFE_SUCCESS);

    /* Execute the function being tested */
    CS_AppMain();

    /* Verify results */
    UtAssert_UINT32_EQ(CS_AppData.RunStatus, CFE_ES_RunStatus_APP_ERROR);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, CS_EXIT_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[1].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[1].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 2,
                  "CFE_EVS_SendEvent was called %u time(s), expected 2",
                  call_count_CFE_EVS_SendEvent);
    /* Generates 3 event messages we don't care about in this test */

    strCmpResult = strncmp(ExpectedSysLogString, context_CFE_ES_WriteToSysLog.Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Sys Log string matched expected result, '%s'", context_CFE_ES_WriteToSysLog.Spec);
}

void CS_AppInit_Test_Nominal(void)
{
    CFE_Status_t Result;
    int32        strCmpResult;
    char         ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "CS Initialized. Version %%d.%%d.%%d.%%d");

    /* Set to prevent segmentation fault */
    UT_SetDeferredRetcode(UT_KEY(CFE_MSG_GetMsgId), 1, 99);

    /* Set to prevent unintended error messages */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Load), CFE_SUCCESS);

    /* Execute the function being tested */
    Result = CS_AppInit();

    /* Verify results */

    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_INIT_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1,
                  "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_AppInit_Test_NominalPowerOnReset(void)
{
    CFE_Status_t Result;
    int32        strCmpResult;
    char         ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "CS Initialized. Version %%d.%%d.%%d.%%d");

    CS_AppData.HkPacket.Payload.EepromCSState = 99;
    CS_AppData.HkPacket.Payload.MemoryCSState = 99;
    CS_AppData.HkPacket.Payload.AppCSState    = 99;
    CS_AppData.HkPacket.Payload.TablesCSState = 99;

    /* Set to prevent segmentation fault */
    UT_SetDeferredRetcode(UT_KEY(CFE_MSG_GetMsgId), 1, 99);

    /* Set to prevent unintended error messages */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Load), CFE_SUCCESS);

    /* Execute the function being tested */
    Result = CS_AppInit();

    /* Verify results */

    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_INIT_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.Payload.EepromCSState == CS_EEPROM_TBL_POWERON_STATE,
                  "CS_AppData.HkPacket.Payload.EepromCSState == CS_EEPROM_TBL_POWERON_STATE");
    UtAssert_True(CS_AppData.HkPacket.Payload.MemoryCSState == CS_MEMORY_TBL_POWERON_STATE,
                  "CS_AppData.HkPacket.Payload.MemoryCSState == CS_MEMORY_TBL_POWERON_STATE");
    UtAssert_True(CS_AppData.HkPacket.Payload.AppCSState == CS_APPS_TBL_POWERON_STATE,
                  "CS_AppData.HkPacket.Payload.AppCSState    == CS_APPS_TBL_POWERON_STATE");
    UtAssert_True(CS_AppData.HkPacket.Payload.TablesCSState == CS_TABLES_TBL_POWERON_STATE,
                  "CS_AppData.HkPacket.Payload.TablesCSState == CS_TABLES_TBL_POWERON_STATE");

    UtAssert_True(CS_AppData.HkPacket.Payload.OSCSState == CS_OSCS_CHECKSUM_STATE,
                  "CS_AppData.HkPacket.Payload.OSCSState == CS_OSCS_CHECKSUM_STATE");
    UtAssert_True(CS_AppData.HkPacket.Payload.CfeCoreCSState == CS_CFECORE_CHECKSUM_STATE,
                  "CS_AppData.HkPacket.Payload.CfeCoreCSState == CS_CFECORE_CHECKSUM_STATE");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1,
                  "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

#if (CS_PRESERVE_STATES_ON_PROCESSOR_RESET == true)

void CS_AppInit_Test_NominalProcReset(void)
{
    CS_AppData.HkPacket.Payload.EepromCSState = 99;
    CS_AppData.HkPacket.Payload.MemoryCSState = 99;
    CS_AppData.HkPacket.Payload.AppCSState    = 99;
    CS_AppData.HkPacket.Payload.TablesCSState = 99;

    /* Execute the function being tested */
    UtAssert_INT32_EQ(CS_AppInit(), CFE_SUCCESS);

    /* Verify results */
    UtAssert_UINT32_EQ(CS_AppData.HkPacket.Payload.EepromCSState, CS_ChecksumState_ENABLED);
    UtAssert_UINT32_EQ(CS_AppData.HkPacket.Payload.MemoryCSState, CS_ChecksumState_ENABLED);
    UtAssert_UINT32_EQ(CS_AppData.HkPacket.Payload.AppCSState, CS_ChecksumState_ENABLED);
    UtAssert_UINT32_EQ(CS_AppData.HkPacket.Payload.TablesCSState, CS_ChecksumState_ENABLED);
    UtAssert_UINT32_EQ(CS_AppData.HkPacket.Payload.OSCSState, CS_ChecksumState_ENABLED);
    UtAssert_UINT32_EQ(CS_AppData.HkPacket.Payload.CfeCoreCSState, CS_ChecksumState_ENABLED);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_INIT_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);
}

void CS_CreateRestoreStatesFromCDS_Test_NoExistingCDS(void)
{
    CFE_Status_t Result;

    CS_AppData.HkPacket.Payload.EepromCSState  = 99;
    CS_AppData.HkPacket.Payload.MemoryCSState  = 99;
    CS_AppData.HkPacket.Payload.AppCSState     = 99;
    CS_AppData.HkPacket.Payload.TablesCSState  = 99;
    CS_AppData.HkPacket.Payload.OSCSState      = 99;
    CS_AppData.HkPacket.Payload.CfeCoreCSState = 99;

    /* Execute the function being tested */
    Result = CS_CreateRestoreStatesFromCDS();

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True(CS_AppData.HkPacket.Payload.EepromCSState == 99,
                  "CS_AppData.HkPacket.Payload.EepromCSState == CS_ChecksumState_ENABLED");
    UtAssert_True(CS_AppData.HkPacket.Payload.MemoryCSState == 99,
                  "CS_AppData.HkPacket.Payload.MemoryCSState == CS_ChecksumState_ENABLED");
    UtAssert_True(CS_AppData.HkPacket.Payload.AppCSState == 99,
                  "CS_AppData.HkPacket.Payload.AppCSState  == CS_ChecksumState_ENABLED");
    UtAssert_True(CS_AppData.HkPacket.Payload.TablesCSState == 99,
                  "CS_AppData.HkPacket.Payload.TablesCSState == CS_ChecksumState_ENABLED");

    UtAssert_True(CS_AppData.HkPacket.Payload.OSCSState == 99,
                  "CS_AppData.HkPacket.Payload.OSCSState == CS_OSCS_CHECKSUM_STATE");
    UtAssert_True(CS_AppData.HkPacket.Payload.CfeCoreCSState == 99,
                  "CS_AppData.HkPacket.Payload.CfeCoreCSState == CS_CFECORE_CHECKSUM_STATE");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0,
                  "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void CS_CreateRestoreStatesFromCDS_Test_CDSSuccess(void)
{
    CFE_Status_t Result;

    CS_AppData.HkPacket.Payload.EepromCSState  = 99;
    CS_AppData.HkPacket.Payload.MemoryCSState  = 99;
    CS_AppData.HkPacket.Payload.AppCSState     = 99;
    CS_AppData.HkPacket.Payload.TablesCSState  = 99;
    CS_AppData.HkPacket.Payload.OSCSState      = 99;
    CS_AppData.HkPacket.Payload.CfeCoreCSState = 99;

    /* Set CDS return calls */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RegisterCDS), 1, CFE_ES_CDS_ALREADY_EXISTS);

    UT_SetHandlerFunction(UT_KEY(CFE_ES_RestoreFromCDS), CS_APP_TEST_CFE_ES_RestoreFromCDS_Handler, NULL);

    /* Execute the function being tested */
    Result = CS_CreateRestoreStatesFromCDS();

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True(CS_AppData.HkPacket.Payload.EepromCSState == CS_ChecksumState_ENABLED,
                  "CS_AppData.HkPacket.Payload.EepromCSState == CS_ChecksumState_ENABLED");
    UtAssert_True(CS_AppData.HkPacket.Payload.MemoryCSState == CS_ChecksumState_ENABLED,
                  "CS_AppData.HkPacket.Payload.MemoryCSState == CS_ChecksumState_ENABLED");
    UtAssert_True(CS_AppData.HkPacket.Payload.AppCSState == CS_ChecksumState_ENABLED,
                  "CS_AppData.HkPacket.Payload.AppCSState    == CS_ChecksumState_ENABLED");
    UtAssert_True(CS_AppData.HkPacket.Payload.TablesCSState == CS_ChecksumState_ENABLED,
                  "CS_AppData.HkPacket.Payload.TablesCSState == CS_ChecksumState_ENABLED");

    UtAssert_True(CS_AppData.HkPacket.Payload.OSCSState == CS_ChecksumState_ENABLED,
                  "CS_AppData.HkPacket.Payload.OSCSState == CS_ChecksumState_ENABLED");
    UtAssert_True(CS_AppData.HkPacket.Payload.CfeCoreCSState == CS_ChecksumState_ENABLED,
                  "CS_AppData.HkPacket.Payload.CfeCoreCSState == CS_ChecksumState_ENABLED");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0,
                  "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void CS_CreateRestoreStatesFromCDS_Test_CDSFail(void)
{
    CFE_Status_t Result;
    uint8        DataStoreBuffer[CS_NUM_DATA_STORE_STATES];
    int32        strCmpResult;
    char         ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Critical Data Store access error = 0x%%08X");

    CS_AppData.HkPacket.Payload.EepromCSState  = 99;
    CS_AppData.HkPacket.Payload.MemoryCSState  = 99;
    CS_AppData.HkPacket.Payload.AppCSState     = 99;
    CS_AppData.HkPacket.Payload.TablesCSState  = 99;
    CS_AppData.HkPacket.Payload.OSCSState      = 99;
    CS_AppData.HkPacket.Payload.CfeCoreCSState = 99;

    /* Set CDS return calls */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RegisterCDS), 1, CFE_ES_CDS_ALREADY_EXISTS);

    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RestoreFromCDS), 1, -1);
    UT_SetDataBuffer(UT_KEY(CFE_ES_RestoreFromCDS), &DataStoreBuffer, sizeof(DataStoreBuffer), false);

    /* Execute the function being tested */
    Result = CS_CreateRestoreStatesFromCDS();

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True(CS_AppData.HkPacket.Payload.EepromCSState == CS_EEPROM_TBL_POWERON_STATE,
                  "CS_AppData.HkPacket.Payload.EepromCSState == CS_EEPROM_TBL_POWERON_STATE");
    UtAssert_True(CS_AppData.HkPacket.Payload.MemoryCSState == CS_MEMORY_TBL_POWERON_STATE,
                  "CS_AppData.HkPacket.Payload.MemoryCSState == CS_MEMORY_TBL_POWERON_STATE");
    UtAssert_True(CS_AppData.HkPacket.Payload.AppCSState == CS_APPS_TBL_POWERON_STATE,
                  "CS_AppData.HkPacket.Payload.AppCSState    == CS_APPS_TBL_POWERON_STATE");
    UtAssert_True(CS_AppData.HkPacket.Payload.TablesCSState == CS_TABLES_TBL_POWERON_STATE,
                  "CS_AppData.HkPacket.Payload.TablesCSState == CS_TABLES_TBL_POWERON_STATE");

    UtAssert_True(CS_AppData.HkPacket.Payload.OSCSState == CS_OSCS_CHECKSUM_STATE,
                  "CS_AppData.HkPacket.Payload.OSCSState == CS_OSCS_CHECKSUM_STATE");
    UtAssert_True(CS_AppData.HkPacket.Payload.CfeCoreCSState == CS_CFECORE_CHECKSUM_STATE,
                  "CS_AppData.HkPacket.Payload.CfeCoreCSState == CS_CFECORE_CHECKSUM_STATE");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_CR_CDS_RES_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1,
                  "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_CreateRestoreStatesFromCDS_Test_CopyToCDSFail(void)
{
    CFE_Status_t Result;
    uint8        DataStoreBuffer[CS_NUM_DATA_STORE_STATES];
    int32        strCmpResult;
    char         ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Critical Data Store access error = 0x%%08X");

    CS_AppData.HkPacket.Payload.EepromCSState  = 99;
    CS_AppData.HkPacket.Payload.MemoryCSState  = 99;
    CS_AppData.HkPacket.Payload.AppCSState     = 99;
    CS_AppData.HkPacket.Payload.TablesCSState  = 99;
    CS_AppData.HkPacket.Payload.OSCSState      = 99;
    CS_AppData.HkPacket.Payload.CfeCoreCSState = 99;

    /* Set CDS return calls */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RegisterCDS), 1, CFE_SUCCESS);

    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RestoreFromCDS), 1, -1);
    UT_SetDataBuffer(UT_KEY(CFE_ES_RestoreFromCDS), &DataStoreBuffer, sizeof(DataStoreBuffer), false);

    UT_SetDeferredRetcode(UT_KEY(CFE_ES_CopyToCDS), 1, -1);

    /* Execute the function being tested */
    Result = CS_CreateRestoreStatesFromCDS();

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True(CS_AppData.HkPacket.Payload.EepromCSState == CS_EEPROM_TBL_POWERON_STATE,
                  "CS_AppData.HkPacket.Payload.EepromCSState == CS_EEPROM_TBL_POWERON_STATE");
    UtAssert_True(CS_AppData.HkPacket.Payload.MemoryCSState == CS_MEMORY_TBL_POWERON_STATE,
                  "CS_AppData.HkPacket.Payload.MemoryCSState == CS_MEMORY_TBL_POWERON_STATE");
    UtAssert_True(CS_AppData.HkPacket.Payload.AppCSState == CS_APPS_TBL_POWERON_STATE,
                  "CS_AppData.HkPacket.Payload.AppCSState    == CS_APPS_TBL_POWERON_STATE");
    UtAssert_True(CS_AppData.HkPacket.Payload.TablesCSState == CS_TABLES_TBL_POWERON_STATE,
                  "CS_AppData.HkPacket.Payload.TablesCSState == CS_TABLES_TBL_POWERON_STATE");

    UtAssert_True(CS_AppData.HkPacket.Payload.OSCSState == CS_OSCS_CHECKSUM_STATE,
                  "CS_AppData.HkPacket.Payload.OSCSState == CS_OSCS_CHECKSUM_STATE");
    UtAssert_True(CS_AppData.HkPacket.Payload.CfeCoreCSState == CS_CFECORE_CHECKSUM_STATE,
                  "CS_AppData.HkPacket.Payload.CfeCoreCSState == CS_CFECORE_CHECKSUM_STATE");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_CR_CDS_CPY_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1,
                  "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_CreateRestoreStatesFromCDS_Test_RegisterCDSFail(void)
{
    CFE_Status_t Result;
    uint8        DataStoreBuffer[CS_NUM_DATA_STORE_STATES];
    int32        strCmpResult;
    char         ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Critical Data Store access error = 0x%%08X");

    CS_AppData.HkPacket.Payload.EepromCSState  = 99;
    CS_AppData.HkPacket.Payload.MemoryCSState  = 99;
    CS_AppData.HkPacket.Payload.AppCSState     = 99;
    CS_AppData.HkPacket.Payload.TablesCSState  = 99;
    CS_AppData.HkPacket.Payload.OSCSState      = 99;
    CS_AppData.HkPacket.Payload.CfeCoreCSState = 99;

    /* Set CDS return calls */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RegisterCDS), 1, -1);

    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RestoreFromCDS), 1, -1);
    UT_SetDataBuffer(UT_KEY(CFE_ES_RestoreFromCDS), &DataStoreBuffer, sizeof(DataStoreBuffer), false);

    UT_SetDeferredRetcode(UT_KEY(CFE_ES_CopyToCDS), 1, -1);

    /* Execute the function being tested */
    Result = CS_CreateRestoreStatesFromCDS();

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True(CS_AppData.HkPacket.Payload.EepromCSState == CS_EEPROM_TBL_POWERON_STATE,
                  "CS_AppData.HkPacket.Payload.EepromCSState == CS_EEPROM_TBL_POWERON_STATE");
    UtAssert_True(CS_AppData.HkPacket.Payload.MemoryCSState == CS_MEMORY_TBL_POWERON_STATE,
                  "CS_AppData.HkPacket.Payload.MemoryCSState == CS_MEMORY_TBL_POWERON_STATE");
    UtAssert_True(CS_AppData.HkPacket.Payload.AppCSState == CS_APPS_TBL_POWERON_STATE,
                  "CS_AppData.HkPacket.Payload.AppCSState    == CS_APPS_TBL_POWERON_STATE");
    UtAssert_True(CS_AppData.HkPacket.Payload.TablesCSState == CS_TABLES_TBL_POWERON_STATE,
                  "CS_AppData.HkPacket.Payload.TablesCSState == CS_TABLES_TBL_POWERON_STATE");

    UtAssert_True(CS_AppData.HkPacket.Payload.OSCSState == CS_OSCS_CHECKSUM_STATE,
                  "CS_AppData.HkPacket.Payload.OSCSState == CS_OSCS_CHECKSUM_STATE");
    UtAssert_True(CS_AppData.HkPacket.Payload.CfeCoreCSState == CS_CFECORE_CHECKSUM_STATE,
                  "CS_AppData.HkPacket.Payload.CfeCoreCSState == CS_CFECORE_CHECKSUM_STATE");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_CR_CDS_REG_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1,
                  "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

#endif /* #if (CS_PRESERVE_STATES_ON_PROCESSOR_RESET == true) */

void CS_AppInit_Test_EVSRegisterError(void)
{
    CFE_Status_t Result;
    int32        strCmpResult;
    char         ExpectedSysLogString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedSysLogString,
             CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS App: Error Registering For Event Services, RC = 0x%%08X\n");

    /* Set CFE_EVS_Register to return -1 in order to reach call to CFE_ES_WriteToSysLog */
    UT_SetDeferredRetcode(UT_KEY(CFE_EVS_Register), 1, -1);

    /* Execute the function being tested */
    Result = CS_AppInit();

    /* Verify results */
    UtAssert_True(Result == -1, "Result == -1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0,
                  "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

    strCmpResult = strncmp(ExpectedSysLogString, context_CFE_ES_WriteToSysLog.Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Sys Log string matched expected result, '%s'", context_CFE_ES_WriteToSysLog.Spec);
}

#if (CS_PRESERVE_STATES_ON_PROCESSOR_RESET == true)

void CS_UpdateCDS_Test_Nominal(void)
{
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_CopyToCDS), 1, CFE_SUCCESS);

    /* Set as valid CDS handle */
    CS_AppData.DataStoreHandle = UT_VALID_CDSID;

    /* Execute the function being tested */
    CS_UpdateCDS();

    /* Verify results */
    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0,
                  "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void CS_UpdateCDS_Test_CopyToCDSFail(void)
{
    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Critical Data Store access error = 0x%%08X");

    UT_SetDeferredRetcode(UT_KEY(CFE_ES_CopyToCDS), 1, -1);

    /* Set a valid CDS handle */
    CS_AppData.DataStoreHandle = UT_VALID_CDSID;

    /* Execute the function being tested */
    CS_UpdateCDS();

    /* Verify results */

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_UPDATE_CDS_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1,
                  "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_UpdateCDS_Test_NullCDSHandle(void)
{
    CS_AppData.DataStoreHandle = CFE_ES_CDS_BAD_HANDLE;

    /* Execute the function being tested */
    CS_UpdateCDS();

    /* Verify results */
    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0,
                  "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}
#endif /* #if (CS_PRESERVE_STATES_ON_PROCESSOR_RESET == true) */

void UtTest_Setup(void)
{
    UtTest_Add(CS_App_TestCmdTlmAlign, CS_Test_Setup, CS_Test_TearDown, "CS_App_TestCmdTlmAlign");

    UtTest_Add(CS_AppMain_Test_Nominal, CS_Test_Setup, CS_Test_TearDown, "CS_AppMain_Test_Nominal");
    UtTest_Add(CS_AppMain_Test_AppInitError, CS_Test_Setup, CS_Test_TearDown, "CS_AppMain_Test_AppInitError");
    UtTest_Add(CS_AppMain_Test_SysException, CS_Test_Setup, CS_Test_TearDown, "CS_AppMain_Test_SysException");
    UtTest_Add(CS_AppMain_Test_RcvMsgError, CS_Test_Setup, CS_Test_TearDown, "CS_AppMain_Test_RcvMsgError");
    UtTest_Add(CS_AppMain_Test_RcvMsgTimeout, CS_Test_Setup, CS_Test_TearDown, "CS_AppMain_Test_RcvMsgTimeout");
    UtTest_Add(CS_AppMain_Test_RcvNoMsg, CS_Test_Setup, CS_Test_TearDown, "CS_AppMain_Test_RcvNoMsg");
    UtTest_Add(CS_AppMain_Test_RcvNullBufPtr, CS_Test_Setup, CS_Test_TearDown, "CS_AppMain_Test_RcvNullBufPtr");
    UtTest_Add(CS_AppMain_Test_AppPipeError, CS_Test_Setup, CS_Test_TearDown, "CS_AppMain_Test_AppPipeError");

    UtTest_Add(CS_AppInit_Test_Nominal, CS_Test_Setup, CS_Test_TearDown, "CS_AppInit_Test_Nominal");
    UtTest_Add(CS_AppInit_Test_EVSRegisterError, CS_Test_Setup, CS_Test_TearDown, "CS_AppInit_Test_EVSRegisterError");
    UtTest_Add(CS_AppInit_Test_NominalPowerOnReset,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_AppInit_Test_NominalPowerOnReset");

#if (CS_PRESERVE_STATES_ON_PROCESSOR_RESET == true)
    UtTest_Add(CS_AppInit_Test_NominalProcReset, CS_Test_Setup, CS_Test_TearDown, "CS_AppInit_Test_NominalProcReset");

    UtTest_Add(CS_CreateRestoreStatesFromCDS_Test_NoExistingCDS,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_CreateRestoreStatesFromCDS_Test_NoExistingCDS");
    UtTest_Add(CS_CreateRestoreStatesFromCDS_Test_CDSSuccess,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_CreateRestoreStatesFromCDS_Test_CDSSuccess");
    UtTest_Add(CS_CreateRestoreStatesFromCDS_Test_CDSFail,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_CreateRestoreStatesFromCDS_Test_CDSFail");
    UtTest_Add(CS_CreateRestoreStatesFromCDS_Test_CopyToCDSFail,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_CreateRestoreStatesFromCDS_Test_CopyToCDSFail");
    UtTest_Add(CS_CreateRestoreStatesFromCDS_Test_RegisterCDSFail,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_CreateRestoreStatesFromCDS_Test_RegisterCDSFail");

    UtTest_Add(CS_UpdateCDS_Test_Nominal, CS_Test_Setup, CS_Test_TearDown, "CS_UpdateCDS_Test_Nominal");
    UtTest_Add(CS_UpdateCDS_Test_CopyToCDSFail, CS_Test_Setup, CS_Test_TearDown, "CS_UpdateCDS_Test_CopyToCDSFail");
    UtTest_Add(CS_UpdateCDS_Test_NullCDSHandle, CS_Test_Setup, CS_Test_TearDown, "CS_UpdateCDS_Test_NullCDSHandle");
#endif /* #if (CS_PRESERVE_STATES_ON_PROCESSOR_RESET == true) */
}
