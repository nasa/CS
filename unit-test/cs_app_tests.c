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

/*
 * Includes
 */

#include "cs_app.h"
#include "cs_msg.h"
#include "cs_msgdefs.h"
#include "cs_events.h"
#include "cs_version.h"
#include "cs_init.h"
#include "cs_utils.h"
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
    CFE_SB_Buffer_t    Buf;
    CS_NoArgsCmd_t     NoArgsCmd;
    CS_GetEntryIDCmd_t GetEntryIDCmd;
    CS_EntryCmd_t      EntryCmd;
    CS_TableNameCmd_t  TableNameCmd;
    CS_AppNameCmd_t    AppNameCmd;
    CS_OneShotCmd_t    OneShotCmd;
} UT_CmdBuf_t;

/* cs_app_tests globals */
uint8 call_count_CFE_EVS_SendEvent;

/*
 * Function Definitions
 */

int32 CS_APP_TEST_CFE_ES_RunLoop_Hook(void *UserObj, int32 StubRetcode, uint32 CallCount,
                                      const UT_StubContext_t *Context)
{
    CS_AppData.RunStatus = CFE_ES_RunStatus_SYS_EXCEPTION;

    return false;
}

void CS_APP_TEST_CFE_ES_RestoreFromCDS_Handler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    uint8 *DataStoreBuffer = (uint8 *)UT_Hook_GetArgValueByName(Context, "RestoreToMemory", uint8 *);

    DataStoreBuffer[0] = CS_STATE_ENABLED;
    DataStoreBuffer[1] = CS_STATE_ENABLED;
    DataStoreBuffer[2] = CS_STATE_ENABLED;
    DataStoreBuffer[3] = CS_STATE_ENABLED;

    DataStoreBuffer[4] = CS_STATE_ENABLED;
    DataStoreBuffer[5] = CS_STATE_ENABLED;
}

int32 CS_AppInit(void);

int32 CS_AppPipe(CFE_SB_Buffer_t *BufPtr);

void CS_HousekeepingCmd(const CS_NoArgsCmd_t *CmdPtr);

int32 CS_CreateRestoreStatesFromCDS(void);

void CS_App_TestCmdTlmAlign(void)
{
    /* Ensures the command and telemetry structures are padded to and aligned to 32-bits */
#define CMD_STRUCT_DATA_IS_32_ALIGNED(x) ((sizeof(x) - sizeof(CFE_MSG_CommandHeader_t)) % 4) == 0
#define TLM_STRUCT_DATA_IS_32_ALIGNED(x) ((sizeof(x) - sizeof(CFE_MSG_TelemetryHeader_t)) % 4) == 0

    UtAssert_True(TLM_STRUCT_DATA_IS_32_ALIGNED(CS_HkPacket_t), "CS_HkPacket_t is 32-bit aligned");

    UtAssert_True(CMD_STRUCT_DATA_IS_32_ALIGNED(CS_NoArgsCmd_t), "CS_NoArgsCmd_t is 32-bit aligned");
    UtAssert_True(CMD_STRUCT_DATA_IS_32_ALIGNED(CS_GetEntryIDCmd_t), "CS_GetEntryIDCmd_t is 32-bit aligned");
    UtAssert_True(CMD_STRUCT_DATA_IS_32_ALIGNED(CS_EntryCmd_t), "CS_EntryCmd_t is 32-bit aligned");
    UtAssert_True(CMD_STRUCT_DATA_IS_32_ALIGNED(CS_TableNameCmd_t), "CS_TableNameCmd_t is 32-bit aligned");
    UtAssert_True(CMD_STRUCT_DATA_IS_32_ALIGNED(CS_AppNameCmd_t), "CS_AppNameCmd_t is 32-bit aligned");
    UtAssert_True(CMD_STRUCT_DATA_IS_32_ALIGNED(CS_OneShotCmd_t), "CS_OneShotCmd_t is 32-bit aligned");
}

void CS_AppMain_Test_Nominal(void)
{
    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
    char  ExpectedSysLogString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "App terminating, RunStatus:0x%%08X");

    snprintf(ExpectedSysLogString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
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

    UtAssert_True(call_count_CFE_EVS_SendEvent == 2, "CFE_EVS_SendEvent was called %u time(s), expected 2",
                  call_count_CFE_EVS_SendEvent);

    /* Generates 2 event messages we don't care about in this test */
} /* end CS_AppMain_Test_Nominal */

void CS_AppMain_Test_AppInitError(void)
{
    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
    char  ExpectedSysLogString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "App terminating, RunStatus:0x%%08X, RC:0x%%08X");

    snprintf(ExpectedSysLogString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
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

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
} /* end CS_AppMain_Test_AppInitError */

void CS_AppMain_Test_SysException(void)
{
    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
    char  ExpectedSysLogString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "App terminating, RunStatus:0x%%08X, RC:0x%%08X");

    snprintf(ExpectedSysLogString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
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

    UtAssert_True(call_count_CFE_EVS_SendEvent == 2, "CFE_EVS_SendEvent was called %u time(s), expected 2",
                  call_count_CFE_EVS_SendEvent);
    /* Generates 1 event message we don't care about in this test */
} /* end CS_AppMain_Test_SysException */

void CS_AppMain_Test_RcvMsgError(void)
{
    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
    char  ExpectedSysLogString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "App terminating, RunStatus:0x%%08X, RC:0x%%08X");

    snprintf(ExpectedSysLogString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
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

    UtAssert_True(call_count_CFE_EVS_SendEvent == 2, "CFE_EVS_SendEvent was called %u time(s), expected 2",
                  call_count_CFE_EVS_SendEvent);
    /* Generates 1 event message we don't care about in this test */
} /* end CS_AppMain_Test_RcvMsgError */

void CS_AppMain_Test_RcvMsgTimeout(void)
{
    CFE_SB_MsgId_t TestMsgId;
    size_t         MsgSize;
    uint8          Packet[4];
    int32          strCmpResult;
    char           ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
    char           ExpectedSysLogString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "App terminating, RunStatus:0x%%08X, RC:0x%%08X");

    snprintf(ExpectedSysLogString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS App terminating, RunStatus:0x%%08X, RC:0x%%08X\n");

    /* Set to make loop execute exactly once */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RunLoop), 1, true);

    TestMsgId = CFE_SB_ValueToMsgId(CS_SEND_HK_MID);
    MsgSize   = sizeof(CS_NoArgsCmd_t);
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

    UtAssert_True(call_count_CFE_EVS_SendEvent == 2, "CFE_EVS_SendEvent was called %u time(s), expected 2",
                  call_count_CFE_EVS_SendEvent);
    /* Generates 3 event messages we don't care about in this test */

    strCmpResult = strncmp(ExpectedSysLogString, context_CFE_ES_WriteToSysLog.Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Sys Log string matched expected result, '%s'", context_CFE_ES_WriteToSysLog.Spec);
} /* end CS_AppMain_Test_RcvMsgTimeout */

void CS_AppMain_Test_RcvNoMsg(void)
{
    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
    char  ExpectedSysLogString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "App terminating, RunStatus:0x%%08X");

    snprintf(ExpectedSysLogString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
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

    UtAssert_True(call_count_CFE_EVS_SendEvent == 2, "CFE_EVS_SendEvent was called %u time(s), expected 2",
                  call_count_CFE_EVS_SendEvent);

    /* Generates 2 event messages we don't care about in this test */
} /* end CS_AppMain_Test_RcvNoMsg */

void CS_AppMain_Test_RcvNullBufPtr(void)
{
    CFE_SB_MsgId_t  TestMsgId;
    size_t          MsgSize;
    int32           strCmpResult;
    char            ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
    char            ExpectedSysLogString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
    CS_NoArgsCmd_t *Packet = NULL;

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "App terminating, RunStatus:0x%%08X, RC:0x%%08X");

    snprintf(ExpectedSysLogString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS App terminating, RunStatus:0x%%08X, RC:0x%%08X\n");

    /* Set to make loop execute exactly once */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RunLoop), 1, true);

    TestMsgId = CFE_SB_ValueToMsgId(CS_SEND_HK_MID);
    MsgSize   = sizeof(CS_NoArgsCmd_t);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &MsgSize, sizeof(MsgSize), false);
    UT_SetDataBuffer(UT_KEY(CFE_SB_ReceiveBuffer), &Packet, sizeof(Packet), false);

    /* Set to make CS_AppPipe return -1 */
    UT_SetDeferredRetcode(UT_KEY(CS_HandleRoutineTableUpdates), 1, -1);

    /* Set to satisfy subsequent condition "BufPtr == NULL" */
    UT_SetDataBuffer(UT_KEY(CFE_SB_ReceiveBuffer), Packet, sizeof(CS_NoArgsCmd_t), false);

    /* Execute the function being tested */
    CS_AppMain();

    /* Verify results */
    UtAssert_True(CS_AppData.RunStatus == 1, "CS_AppData.RunStatus == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 2, "CFE_EVS_SendEvent was called %u time(s), expected 2",
                  call_count_CFE_EVS_SendEvent);
    /* Generates 1 event messages we don't care about in this test */

    strCmpResult = strncmp(ExpectedSysLogString, context_CFE_ES_WriteToSysLog.Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Sys Log string matched expected result, '%s'", context_CFE_ES_WriteToSysLog.Spec);
} /* end CS_AppMain_Test_RcvNullBufPtr */

void CS_AppMain_Test_AppPipeError(void)
{
    CFE_SB_MsgId_t  TestMsgId;
    size_t          MsgSize;
    CS_NoArgsCmd_t  Packet;
    CS_NoArgsCmd_t *PacketPtr = &Packet;
    int32           strCmpResult;
    char            ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
    char            ExpectedSysLogString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "App terminating, RunStatus:0x%%08X, RC:0x%%08X");

    snprintf(ExpectedSysLogString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS App terminating, RunStatus:0x%%08X, RC:0x%%08X\n");

    /* Set to make loop execute exactly once */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RunLoop), 1, true);

    TestMsgId = CFE_SB_ValueToMsgId(CS_SEND_HK_MID);
    MsgSize   = sizeof(CS_NoArgsCmd_t);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &MsgSize, sizeof(MsgSize), false);
    UT_SetDataBuffer(UT_KEY(CFE_SB_ReceiveBuffer), &PacketPtr, sizeof(PacketPtr), false);

    /* Set to make CS_AppPipe return -1 */
    UT_SetDeferredRetcode(UT_KEY(CS_HandleRoutineTableUpdates), 1, -1);

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

    UtAssert_True(call_count_CFE_EVS_SendEvent == 2, "CFE_EVS_SendEvent was called %u time(s), expected 2",
                  call_count_CFE_EVS_SendEvent);
    /* Generates 3 event messages we don't care about in this test */

    strCmpResult = strncmp(ExpectedSysLogString, context_CFE_ES_WriteToSysLog.Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Sys Log string matched expected result, '%s'", context_CFE_ES_WriteToSysLog.Spec);
} /* end CS_AppMain_Test_AppPipeError */

void CS_AppInit_Test_Nominal(void)
{
    int32 Result;
    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

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

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
} /* end CS_AppInit_Test_Nominal */

void CS_AppInit_Test_NominalPowerOnReset(void)
{
    int32 Result;
    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "CS Initialized. Version %%d.%%d.%%d.%%d");

    CS_AppData.HkPacket.EepromCSState = 99;
    CS_AppData.HkPacket.MemoryCSState = 99;
    CS_AppData.HkPacket.AppCSState    = 99;
    CS_AppData.HkPacket.TablesCSState = 99;

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

    UtAssert_True(CS_AppData.HkPacket.EepromCSState == CS_EEPROM_TBL_POWERON_STATE,
                  "CS_AppData.HkPacket.EepromCSState == CS_EEPROM_TBL_POWERON_STATE");
    UtAssert_True(CS_AppData.HkPacket.MemoryCSState == CS_MEMORY_TBL_POWERON_STATE,
                  "CS_AppData.HkPacket.MemoryCSState == CS_MEMORY_TBL_POWERON_STATE");
    UtAssert_True(CS_AppData.HkPacket.AppCSState == CS_APPS_TBL_POWERON_STATE,
                  "CS_AppData.HkPacket.AppCSState    == CS_APPS_TBL_POWERON_STATE");
    UtAssert_True(CS_AppData.HkPacket.TablesCSState == CS_TABLES_TBL_POWERON_STATE,
                  "CS_AppData.HkPacket.TablesCSState == CS_TABLES_TBL_POWERON_STATE");

    UtAssert_True(CS_AppData.HkPacket.OSCSState == CS_OSCS_CHECKSUM_STATE,
                  "CS_AppData.HkPacket.OSCSState == CS_OSCS_CHECKSUM_STATE");
    UtAssert_True(CS_AppData.HkPacket.CfeCoreCSState == CS_CFECORE_CHECKSUM_STATE,
                  "CS_AppData.HkPacket.CfeCoreCSState == CS_CFECORE_CHECKSUM_STATE");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
} /* end CS_AppInit_Test_NominalPowerOnReset */

#if (CS_PRESERVE_STATES_ON_PROCESSOR_RESET == true)

void CS_AppInit_Test_NominalProcReset(void)
{
    CS_AppData.HkPacket.EepromCSState = 99;
    CS_AppData.HkPacket.MemoryCSState = 99;
    CS_AppData.HkPacket.AppCSState    = 99;
    CS_AppData.HkPacket.TablesCSState = 99;

    /* Execute the function being tested */
    UtAssert_INT32_EQ(CS_AppInit(), CFE_SUCCESS);

    /* Verify results */
    UtAssert_UINT32_EQ(CS_AppData.HkPacket.EepromCSState, CS_STATE_ENABLED);
    UtAssert_UINT32_EQ(CS_AppData.HkPacket.MemoryCSState, CS_STATE_ENABLED);
    UtAssert_UINT32_EQ(CS_AppData.HkPacket.AppCSState, CS_STATE_ENABLED);
    UtAssert_UINT32_EQ(CS_AppData.HkPacket.TablesCSState, CS_STATE_ENABLED);
    UtAssert_UINT32_EQ(CS_AppData.HkPacket.OSCSState, CS_STATE_ENABLED);
    UtAssert_UINT32_EQ(CS_AppData.HkPacket.CfeCoreCSState, CS_STATE_ENABLED);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_INIT_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);
} /* end CS_AppInit_Test_NominalProcReset */

void CS_CreateRestoreStatesFromCDS_Test_NoExistingCDS(void)
{
    int32 Result;

    CS_AppData.HkPacket.EepromCSState  = 99;
    CS_AppData.HkPacket.MemoryCSState  = 99;
    CS_AppData.HkPacket.AppCSState     = 99;
    CS_AppData.HkPacket.TablesCSState  = 99;
    CS_AppData.HkPacket.OSCSState      = 99;
    CS_AppData.HkPacket.CfeCoreCSState = 99;

    /* Execute the function being tested */
    Result = CS_CreateRestoreStatesFromCDS();

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True(CS_AppData.HkPacket.EepromCSState == 99, "CS_AppData.HkPacket.EepromCSState == CS_STATE_ENABLED");
    UtAssert_True(CS_AppData.HkPacket.MemoryCSState == 99, "CS_AppData.HkPacket.MemoryCSState == CS_STATE_ENABLED");
    UtAssert_True(CS_AppData.HkPacket.AppCSState == 99, "CS_AppData.HkPacket.AppCSState  == CS_STATE_ENABLED");
    UtAssert_True(CS_AppData.HkPacket.TablesCSState == 99, "CS_AppData.HkPacket.TablesCSState == CS_STATE_ENABLED");

    UtAssert_True(CS_AppData.HkPacket.OSCSState == 99, "CS_AppData.HkPacket.OSCSState == CS_OSCS_CHECKSUM_STATE");
    UtAssert_True(CS_AppData.HkPacket.CfeCoreCSState == 99,
                  "CS_AppData.HkPacket.CfeCoreCSState == CS_CFECORE_CHECKSUM_STATE");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
} /* end CS_CreateRestoreStatesFromCDS_Test_NoCDS */

void CS_CreateRestoreStatesFromCDS_Test_CDSSuccess(void)
{
    int32 Result;

    CS_AppData.HkPacket.EepromCSState  = 99;
    CS_AppData.HkPacket.MemoryCSState  = 99;
    CS_AppData.HkPacket.AppCSState     = 99;
    CS_AppData.HkPacket.TablesCSState  = 99;
    CS_AppData.HkPacket.OSCSState      = 99;
    CS_AppData.HkPacket.CfeCoreCSState = 99;

    /* Set CDS return calls */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RegisterCDS), 1, CFE_ES_CDS_ALREADY_EXISTS);

    UT_SetHandlerFunction(UT_KEY(CFE_ES_RestoreFromCDS), CS_APP_TEST_CFE_ES_RestoreFromCDS_Handler, NULL);

    /* Execute the function being tested */
    Result = CS_CreateRestoreStatesFromCDS();

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True(CS_AppData.HkPacket.EepromCSState == CS_STATE_ENABLED,
                  "CS_AppData.HkPacket.EepromCSState == CS_STATE_ENABLED");
    UtAssert_True(CS_AppData.HkPacket.MemoryCSState == CS_STATE_ENABLED,
                  "CS_AppData.HkPacket.MemoryCSState == CS_STATE_ENABLED");
    UtAssert_True(CS_AppData.HkPacket.AppCSState == CS_STATE_ENABLED,
                  "CS_AppData.HkPacket.AppCSState    == CS_STATE_ENABLED");
    UtAssert_True(CS_AppData.HkPacket.TablesCSState == CS_STATE_ENABLED,
                  "CS_AppData.HkPacket.TablesCSState == CS_STATE_ENABLED");

    UtAssert_True(CS_AppData.HkPacket.OSCSState == CS_STATE_ENABLED,
                  "CS_AppData.HkPacket.OSCSState == CS_STATE_ENABLED");
    UtAssert_True(CS_AppData.HkPacket.CfeCoreCSState == CS_STATE_ENABLED,
                  "CS_AppData.HkPacket.CfeCoreCSState == CS_STATE_ENABLED");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
} /* end CS_CreateRestoreStatesFromCDS_Test_CDSSuccess */

void CS_CreateRestoreStatesFromCDS_Test_CDSFail(void)
{
    int32 Result;
    uint8 DataStoreBuffer[CS_NUM_DATA_STORE_STATES];
    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Critical Data Store access error = 0x%%08X");

    CS_AppData.HkPacket.EepromCSState  = 99;
    CS_AppData.HkPacket.MemoryCSState  = 99;
    CS_AppData.HkPacket.AppCSState     = 99;
    CS_AppData.HkPacket.TablesCSState  = 99;
    CS_AppData.HkPacket.OSCSState      = 99;
    CS_AppData.HkPacket.CfeCoreCSState = 99;

    /* Set CDS return calls */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RegisterCDS), 1, CFE_ES_CDS_ALREADY_EXISTS);

    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RestoreFromCDS), 1, -1);
    UT_SetDataBuffer(UT_KEY(CFE_ES_RestoreFromCDS), &DataStoreBuffer, sizeof(DataStoreBuffer), false);

    /* Execute the function being tested */
    Result = CS_CreateRestoreStatesFromCDS();

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True(CS_AppData.HkPacket.EepromCSState == CS_EEPROM_TBL_POWERON_STATE,
                  "CS_AppData.HkPacket.EepromCSState == CS_EEPROM_TBL_POWERON_STATE");
    UtAssert_True(CS_AppData.HkPacket.MemoryCSState == CS_MEMORY_TBL_POWERON_STATE,
                  "CS_AppData.HkPacket.MemoryCSState == CS_MEMORY_TBL_POWERON_STATE");
    UtAssert_True(CS_AppData.HkPacket.AppCSState == CS_APPS_TBL_POWERON_STATE,
                  "CS_AppData.HkPacket.AppCSState    == CS_APPS_TBL_POWERON_STATE");
    UtAssert_True(CS_AppData.HkPacket.TablesCSState == CS_TABLES_TBL_POWERON_STATE,
                  "CS_AppData.HkPacket.TablesCSState == CS_TABLES_TBL_POWERON_STATE");

    UtAssert_True(CS_AppData.HkPacket.OSCSState == CS_OSCS_CHECKSUM_STATE,
                  "CS_AppData.HkPacket.OSCSState == CS_OSCS_CHECKSUM_STATE");
    UtAssert_True(CS_AppData.HkPacket.CfeCoreCSState == CS_CFECORE_CHECKSUM_STATE,
                  "CS_AppData.HkPacket.CfeCoreCSState == CS_CFECORE_CHECKSUM_STATE");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_CR_CDS_RES_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
} /* end CS_AppInit_Test_ProcResetRestoreCDSFail */

void CS_CreateRestoreStatesFromCDS_Test_CopyToCDSFail(void)
{
    int32 Result;
    uint8 DataStoreBuffer[CS_NUM_DATA_STORE_STATES];
    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Critical Data Store access error = 0x%%08X");

    CS_AppData.HkPacket.EepromCSState  = 99;
    CS_AppData.HkPacket.MemoryCSState  = 99;
    CS_AppData.HkPacket.AppCSState     = 99;
    CS_AppData.HkPacket.TablesCSState  = 99;
    CS_AppData.HkPacket.OSCSState      = 99;
    CS_AppData.HkPacket.CfeCoreCSState = 99;

    /* Set CDS return calls */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RegisterCDS), 1, CFE_SUCCESS);

    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RestoreFromCDS), 1, -1);
    UT_SetDataBuffer(UT_KEY(CFE_ES_RestoreFromCDS), &DataStoreBuffer, sizeof(DataStoreBuffer), false);

    UT_SetDeferredRetcode(UT_KEY(CFE_ES_CopyToCDS), 1, -1);

    /* Execute the function being tested */
    Result = CS_CreateRestoreStatesFromCDS();

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True(CS_AppData.HkPacket.EepromCSState == CS_EEPROM_TBL_POWERON_STATE,
                  "CS_AppData.HkPacket.EepromCSState == CS_EEPROM_TBL_POWERON_STATE");
    UtAssert_True(CS_AppData.HkPacket.MemoryCSState == CS_MEMORY_TBL_POWERON_STATE,
                  "CS_AppData.HkPacket.MemoryCSState == CS_MEMORY_TBL_POWERON_STATE");
    UtAssert_True(CS_AppData.HkPacket.AppCSState == CS_APPS_TBL_POWERON_STATE,
                  "CS_AppData.HkPacket.AppCSState    == CS_APPS_TBL_POWERON_STATE");
    UtAssert_True(CS_AppData.HkPacket.TablesCSState == CS_TABLES_TBL_POWERON_STATE,
                  "CS_AppData.HkPacket.TablesCSState == CS_TABLES_TBL_POWERON_STATE");

    UtAssert_True(CS_AppData.HkPacket.OSCSState == CS_OSCS_CHECKSUM_STATE,
                  "CS_AppData.HkPacket.OSCSState == CS_OSCS_CHECKSUM_STATE");
    UtAssert_True(CS_AppData.HkPacket.CfeCoreCSState == CS_CFECORE_CHECKSUM_STATE,
                  "CS_AppData.HkPacket.CfeCoreCSState == CS_CFECORE_CHECKSUM_STATE");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_CR_CDS_CPY_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
} /* end CS_CreateRestoreStatesFromCDS_Test_CopyToCDSFail */

void CS_CreateRestoreStatesFromCDS_Test_RegisterCDSFail(void)
{
    int32 Result;
    uint8 DataStoreBuffer[CS_NUM_DATA_STORE_STATES];
    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Critical Data Store access error = 0x%%08X");

    CS_AppData.HkPacket.EepromCSState  = 99;
    CS_AppData.HkPacket.MemoryCSState  = 99;
    CS_AppData.HkPacket.AppCSState     = 99;
    CS_AppData.HkPacket.TablesCSState  = 99;
    CS_AppData.HkPacket.OSCSState      = 99;
    CS_AppData.HkPacket.CfeCoreCSState = 99;

    /* Set CDS return calls */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RegisterCDS), 1, -1);

    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RestoreFromCDS), 1, -1);
    UT_SetDataBuffer(UT_KEY(CFE_ES_RestoreFromCDS), &DataStoreBuffer, sizeof(DataStoreBuffer), false);

    UT_SetDeferredRetcode(UT_KEY(CFE_ES_CopyToCDS), 1, -1);

    /* Execute the function being tested */
    Result = CS_CreateRestoreStatesFromCDS();

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True(CS_AppData.HkPacket.EepromCSState == CS_EEPROM_TBL_POWERON_STATE,
                  "CS_AppData.HkPacket.EepromCSState == CS_EEPROM_TBL_POWERON_STATE");
    UtAssert_True(CS_AppData.HkPacket.MemoryCSState == CS_MEMORY_TBL_POWERON_STATE,
                  "CS_AppData.HkPacket.MemoryCSState == CS_MEMORY_TBL_POWERON_STATE");
    UtAssert_True(CS_AppData.HkPacket.AppCSState == CS_APPS_TBL_POWERON_STATE,
                  "CS_AppData.HkPacket.AppCSState    == CS_APPS_TBL_POWERON_STATE");
    UtAssert_True(CS_AppData.HkPacket.TablesCSState == CS_TABLES_TBL_POWERON_STATE,
                  "CS_AppData.HkPacket.TablesCSState == CS_TABLES_TBL_POWERON_STATE");

    UtAssert_True(CS_AppData.HkPacket.OSCSState == CS_OSCS_CHECKSUM_STATE,
                  "CS_AppData.HkPacket.OSCSState == CS_OSCS_CHECKSUM_STATE");
    UtAssert_True(CS_AppData.HkPacket.CfeCoreCSState == CS_CFECORE_CHECKSUM_STATE,
                  "CS_AppData.HkPacket.CfeCoreCSState == CS_CFECORE_CHECKSUM_STATE");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_CR_CDS_REG_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
} /* end CS_CreateRestoreStatesFromCDS_Test_RegisterCDSFail */

#endif /* #if (CS_PRESERVE_STATES_ON_PROCESSOR_RESET == true) */

void CS_AppInit_Test_EVSRegisterError(void)
{
    int32 Result;
    int32 strCmpResult;
    char  ExpectedSysLogString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedSysLogString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS App: Error Registering For Event Services, RC = 0x%%08X\n");

    /* Set CFE_EVS_Register to return -1 in order to reach call to CFE_ES_WriteToSysLog */
    UT_SetDeferredRetcode(UT_KEY(CFE_EVS_Register), 1, -1);

    /* Execute the function being tested */
    Result = CS_AppInit();

    /* Verify results */
    UtAssert_True(Result == -1, "Result == -1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

    strCmpResult = strncmp(ExpectedSysLogString, context_CFE_ES_WriteToSysLog.Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Sys Log string matched expected result, '%s'", context_CFE_ES_WriteToSysLog.Spec);
} /* end CS_AppInit_Test_EVSRegisterError */

void CS_AppPipe_Test_TableUpdateErrors(void)
{
    int32          Result;
    UT_CmdBuf_t    CmdBuf;
    CFE_SB_MsgId_t TestMsgId;
    size_t         MsgSize;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_SEND_HK_MID);
    MsgSize   = sizeof(CS_NoArgsCmd_t);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &MsgSize, sizeof(MsgSize), false);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
    /* Generates 5 event messages we don't care about in this test */
} /* end CS_AppPipe_Test_TableUpdateErrors */

void CS_AppPipe_Test_BackgroundCycle(void)
{
    int32          Result;
    UT_CmdBuf_t    CmdBuf;
    CFE_SB_MsgId_t TestMsgId;
    size_t         MsgSize;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_BACKGROUND_CYCLE_MID);
    MsgSize   = sizeof(CS_NoArgsCmd_t);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &MsgSize, sizeof(MsgSize), false);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
} /* end CS_AppPipe_Test_BackgroundCycle */

void CS_AppPipe_Test_NoopCmd(void)
{
    int32             Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_NOOP_CC;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
} /* end CS_AppPipe_Test_NoopCmd */

void CS_AppPipe_Test_ResetCmd(void)
{
    int32             Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_RESET_CC;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
} /* end CS_AppPipe_Test_ResetCmd */

void CS_AppPipe_Test_OneShotCmd(void)
{
    int32             Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_ONESHOT_CC;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
} /* end CS_AppPipe_Test_OneShotCmd */

void CS_AppPipe_Test_CancelOneShotCmd(void)
{
    int32             Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_CANCEL_ONESHOT_CC;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
} /* end CS_AppPipe_Test_CancelOneShotCmd */

void CS_AppPipe_Test_EnableAllCSCmd(void)
{
    int32             Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_ENABLE_ALL_CS_CC;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
} /* end CS_AppPipe_Test_EnableAllCSCmd */

void CS_AppPipe_Test_DisableAllCSCmd(void)
{
    int32             Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_DISABLE_ALL_CS_CC;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
} /* end CS_AppPipe_Test_DisableAllCSCmd */

void CS_AppPipe_Test_EnableCfeCoreCmd(void)
{
    int32             Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_ENABLE_CFECORE_CC;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
} /* end CS_AppPipe_Test_EnableCfeCoreCmd */

void CS_AppPipe_Test_DisableCfeCoreCmd(void)
{
    int32             Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_DISABLE_CFECORE_CC;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
} /* end CS_AppPipe_Test_DisableCfeCoreCmd */

void CS_AppPipe_Test_ReportBaselineCfeCoreCmd(void)
{
    int32             Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_REPORT_BASELINE_CFECORE_CC;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
} /* end CS_AppPipe_Test_ReportBaselineCfeCoreCmd */

void CS_AppPipe_Test_RecomputeBaselineCfeCoreCmd(void)
{
    int32             Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_RECOMPUTE_BASELINE_CFECORE_CC;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
} /* end CS_AppPipe_Test_RecomputeBaselineCfeCoreCmd */

void CS_AppPipe_Test_EnableOSCmd(void)
{
    int32             Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_ENABLE_OS_CC;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
} /* end CS_AppPipe_Test_EnableOSCmd */

void CS_AppPipe_Test_DisableOSCmd(void)
{
    int32             Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_DISABLE_OS_CC;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
} /* end CS_AppPipe_Test_DisableOSCmd */

void CS_AppPipe_Test_ReportBaselineOSCmd(void)
{
    int32             Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_REPORT_BASELINE_OS_CC;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
} /* end CS_AppPipe_Test_ReportBaselineOSCmd */

void CS_AppPipe_Test_RecomputeBaselineOSCmd(void)
{
    int32             Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_RECOMPUTE_BASELINE_OS_CC;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
} /* end CS_AppPipe_Test_RecomputeBaselineOSCmd */

void CS_AppPipe_Test_EnableEepromCmd(void)
{
    int32             Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_ENABLE_EEPROM_CC;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
} /* end CS_AppPipe_Test_EnableEepromCmd */

void CS_AppPipe_Test_DisableEepromCmd(void)
{
    int32             Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_DISABLE_EEPROM_CC;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
} /* end CS_AppPipe_Test_DisableEepromCmd */

void CS_AppPipe_Test_ReportBaselineEntryIDEepromCmd(void)
{
    int32             Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_REPORT_BASELINE_EEPROM_CC;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
} /* end CS_AppPipe_Test_ReportBaselineEntryIDEepromCmd */

void CS_AppPipe_Test_RecomputeBaselineEepromCmd(void)
{
    int32             Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_RECOMPUTE_BASELINE_EEPROM_CC;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
} /* end CS_AppPipe_Test_RecomputeBaselineEepromCmd */

void CS_AppPipe_Test_EnableEntryIDEepromCmd(void)
{
    int32             Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_ENABLE_ENTRY_EEPROM_CC;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
} /* end CS_AppPipe_Test_EnableEntryIDEepromCmd */

void CS_AppPipe_Test_DisableEntryIDEepromCmd(void)
{
    int32             Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_DISABLE_ENTRY_EEPROM_CC;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
} /* end CS_AppPipe_Test_DisableEntryIDEepromCmd */

void CS_AppPipe_Test_GetEntryIDEepromCmd(void)
{
    int32             Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_GET_ENTRY_ID_EEPROM_CC;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
} /* end CS_AppPipe_Test_GetEntryIDEepromCmd */

void CS_AppPipe_Test_EnableMemoryCmd(void)
{
    int32             Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_ENABLE_MEMORY_CC;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
} /* end CS_AppPipe_Test_EnableMemoryCmd */

void CS_AppPipe_Test_DisableMemoryCmd(void)
{
    int32             Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_DISABLE_MEMORY_CC;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
} /* end CS_AppPipe_Test_DisableMemoryCmd */

void CS_AppPipe_Test_ReportBaselineEntryIDMemoryCmd(void)
{
    int32             Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_REPORT_BASELINE_MEMORY_CC;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
} /* end CS_AppPipe_Test_ReportBaselineEntryIDMemoryCmd */

void CS_AppPipe_Test_RecomputeBaselineMemoryCmd(void)
{
    int32             Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_RECOMPUTE_BASELINE_MEMORY_CC;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
} /* end CS_AppPipe_Test_RecomputeBaselineMemoryCmd */

void CS_AppPipe_Test_EnableEntryIDMemoryCmd(void)
{
    int32             Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_ENABLE_ENTRY_MEMORY_CC;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
} /* end CS_AppPipe_Test_EnableEntryIDMemoryCmd */

void CS_AppPipe_Test_DisableEntryIDMemoryCmd(void)
{
    int32             Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_DISABLE_ENTRY_MEMORY_CC;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
} /* end CS_AppPipe_Test_DisableEntryIDMemoryCmd */

void CS_AppPipe_Test_GetEntryIDMemoryCmd(void)
{
    int32             Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_GET_ENTRY_ID_MEMORY_CC;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
} /* end CS_AppPipe_Test_GetEntryIDMemoryCmd */

void CS_AppPipe_Test_EnableTablesCmd(void)
{
    int32             Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_ENABLE_TABLES_CC;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
} /* end CS_AppPipe_Test_EnableTablesCmd */

void CS_AppPipe_Test_DisableTablesCmd(void)
{
    int32             Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_DISABLE_TABLES_CC;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
} /* end CS_AppPipe_Test_DisableTablesCmd */

void CS_AppPipe_Test_ReportBaselineTablesCmd(void)
{
    int32             Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_REPORT_BASELINE_TABLE_CC;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
} /* end CS_AppPipe_Test_ReportBaselineTablesCmd */

void CS_AppPipe_Test_RecomputeBaselineTablesCmd(void)
{
    int32             Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_RECOMPUTE_BASELINE_TABLE_CC;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
} /* end CS_AppPipe_Test_RecomputeBaselineTablesCmd */

void CS_AppPipe_Test_EnableNameTablesCmd(void)
{
    int32             Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_ENABLE_NAME_TABLE_CC;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
} /* end CS_AppPipe_Test_EnableNameTablesCmd */

void CS_AppPipe_Test_DisableNameTablesCmd(void)
{
    int32             Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_DISABLE_NAME_TABLE_CC;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
} /* end CS_AppPipe_Test_DisableNameTablesCmd */

void CS_AppPipe_Test_EnableAppCmd(void)
{
    int32             Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_ENABLE_APPS_CC;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
} /* end CS_AppPipe_Test_EnableAppCmd */

void CS_AppPipe_Test_DisableAppCmd(void)
{
    int32             Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_DISABLE_APPS_CC;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
} /* end CS_AppPipe_Test_DisableAppCmd */

void CS_AppPipe_Test_ReportBaselineAppCmd(void)
{
    int32             Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_REPORT_BASELINE_APP_CC;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
} /* end CS_AppPipe_Test_ReportBaselineAppCmd */

void CS_AppPipe_Test_RecomputeBaselineAppCmd(void)
{
    int32             Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_RECOMPUTE_BASELINE_APP_CC;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
} /* end CS_AppPipe_Test_RecomputeBaselineAppCmd */

void CS_AppPipe_Test_EnableNameAppCmd(void)
{
    int32             Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_ENABLE_NAME_APP_CC;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
} /* end CS_AppPipe_Test_EnableNameAppCmd */

void CS_AppPipe_Test_DisableNameAppCmd(void)
{
    int32             Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = CS_DISABLE_NAME_APP_CC;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
} /* end CS_AppPipe_Test_DisableNameAppCmd */

void CS_AppPipe_Test_InvalidCCError(void)
{
    int32             Result;
    UT_CmdBuf_t       CmdBuf;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;
    int32             strCmpResult;
    char              ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Invalid ground command code: ID = 0x%%08lX, CC = %%d");

    CS_AppData.ChildTaskTable = -1;

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    FcnCode   = 99;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    /* Execute the function being tested */
    Result = CS_AppPipe(&CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_CC1_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
} /* end CS_AppPipe_Test_InvalidCCError */

void CS_AppPipe_Test_InvalidMIDError(void)
{
    int32          Result;
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

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
} /* end CS_AppPipe_Test_InvalidMIDError */

void CS_HousekeepingCmd_Test_Nominal(void)
{
    CS_NoArgsCmd_t    CmdPacket;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;
    size_t            MsgSize;

    memset(&CmdPacket, 0, sizeof(CmdPacket));

    TestMsgId = CFE_SB_ValueToMsgId(CS_SEND_HK_MID);
    MsgSize   = sizeof(CS_NoArgsCmd_t);
    FcnCode   = 99;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &MsgSize, sizeof(MsgSize), false);

    CS_AppData.HkPacket.CmdCounter          = 1;
    CS_AppData.HkPacket.CmdErrCounter       = 2;
    CS_AppData.HkPacket.ChecksumState       = 3;
    CS_AppData.HkPacket.EepromCSState       = 4;
    CS_AppData.HkPacket.MemoryCSState       = 5;
    CS_AppData.HkPacket.AppCSState          = 6;
    CS_AppData.HkPacket.TablesCSState       = 7;
    CS_AppData.HkPacket.OSCSState           = 8;
    CS_AppData.HkPacket.CfeCoreCSState      = 9;
    CS_AppData.HkPacket.RecomputeInProgress = 10;
    CS_AppData.HkPacket.OneShotInProgress   = 11;
    CS_AppData.HkPacket.EepromCSErrCounter  = 12;
    CS_AppData.HkPacket.MemoryCSErrCounter  = 13;
    CS_AppData.HkPacket.AppCSErrCounter     = 14;
    CS_AppData.HkPacket.TablesCSErrCounter  = 15;
    CS_AppData.HkPacket.CfeCoreCSErrCounter = 16;
    CS_AppData.HkPacket.OSCSErrCounter      = 17;
    CS_AppData.HkPacket.CurrentCSTable      = 18;
    CS_AppData.HkPacket.CurrentEntryInTable = 19;
    CS_AppData.HkPacket.EepromBaseline      = 20;
    CS_AppData.HkPacket.OSBaseline          = 21;
    CS_AppData.HkPacket.CfeCoreBaseline     = 22;
    CS_AppData.HkPacket.LastOneShotAddress  = 23;
    CS_AppData.HkPacket.LastOneShotSize     = 24;
    CS_AppData.HkPacket.LastOneShotChecksum = 25;
    CS_AppData.HkPacket.PassCounter         = 26;

    /* Execute the function being tested */
    CS_HousekeepingCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.CmdCounter == 1, "CS_AppData.HkPacket.CmdCounter == 1");
    UtAssert_True(CS_AppData.HkPacket.CmdErrCounter == 2, "CS_AppData.HkPacket.CmdErrCounter == 2");
    UtAssert_True(CS_AppData.HkPacket.ChecksumState == 3, "CS_AppData.HkPacket.ChecksumState == 3");
    UtAssert_True(CS_AppData.HkPacket.EepromCSState == 4, "CS_AppData.HkPacket.EepromCSState == 4");
    UtAssert_True(CS_AppData.HkPacket.MemoryCSState == 5, "CS_AppData.HkPacket.MemoryCSState == 5");
    UtAssert_True(CS_AppData.HkPacket.AppCSState == 6, "CS_AppData.HkPacket.AppCSState == 6");
    UtAssert_True(CS_AppData.HkPacket.TablesCSState == 7, "CS_AppData.HkPacket.TablesCSState == 7");
    UtAssert_True(CS_AppData.HkPacket.OSCSState == 8, "CS_AppData.HkPacket.OSCSState == 8");
    UtAssert_True(CS_AppData.HkPacket.CfeCoreCSState == 9, "CS_AppData.HkPacket.CfeCoreCSState == 9");
    UtAssert_True(CS_AppData.HkPacket.RecomputeInProgress == 10, "CS_AppData.HkPacket.ChildTaskInUse == 10");
    UtAssert_True(CS_AppData.HkPacket.OneShotInProgress == 11, "CS_AppData.HkPacket.OneShotInProgress == 11");
    UtAssert_True(CS_AppData.HkPacket.EepromCSErrCounter == 12, "CS_AppData.HkPacket.EepromCSErrCounter == 12");
    UtAssert_True(CS_AppData.HkPacket.MemoryCSErrCounter == 13, "CS_AppData.HkPacket.MemoryCSErrCounter == 13");
    UtAssert_True(CS_AppData.HkPacket.AppCSErrCounter == 14, "CS_AppData.HkPacket.AppCSErrCounter == 14");
    UtAssert_True(CS_AppData.HkPacket.TablesCSErrCounter == 15, "CS_AppData.HkPacket.TablesCSErrCounter == 15");
    UtAssert_True(CS_AppData.HkPacket.CfeCoreCSErrCounter == 16, "CS_AppData.HkPacket.CfeCoreCSErrCounter == 16");
    UtAssert_True(CS_AppData.HkPacket.OSCSErrCounter == 17, "CS_AppData.HkPacket.OSCSErrCounter == 17");
    UtAssert_True(CS_AppData.HkPacket.CurrentCSTable == 18, "CS_AppData.HkPacket.CurrentCSTable == 18");
    UtAssert_True(CS_AppData.HkPacket.CurrentEntryInTable == 19, "CS_AppData.HkPacket.CurrentEntryInTable == 19");
    UtAssert_True(CS_AppData.HkPacket.EepromBaseline == 20, "CS_AppData.HkPacket.EepromBaseline == 20");
    UtAssert_True(CS_AppData.HkPacket.OSBaseline == 21, "CS_AppData.HkPacket.OSBaseline == 21");
    UtAssert_True(CS_AppData.HkPacket.CfeCoreBaseline == 22, "CS_AppData.HkPacket.CfeCoreBaseline == 22");
    UtAssert_True(CS_AppData.HkPacket.LastOneShotAddress == 23, "CS_AppData.HkPacket.LastOneShotAddress == 23");
    UtAssert_True(CS_AppData.HkPacket.LastOneShotSize == 24, "CS_AppData.HkPacket.LastOneShotSize == 24");
    UtAssert_True(CS_AppData.HkPacket.LastOneShotChecksum == 25, "CS_AppData.HkPacket.LastOneShotChecksum == 25");
    UtAssert_True(CS_AppData.HkPacket.PassCounter == 26, "CS_AppData.HkPacket.PassCounter == 26");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
} /* end CS_HousekeepingCmd_Test_Nominal */

void CS_HousekeepingCmd_Test_InvalidMsgLength(void)
{
    CS_NoArgsCmd_t    CmdPacket;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;
    size_t            MsgSize;
    int32             strCmpResult;
    char              ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    memset(&CmdPacket, 0, sizeof(CmdPacket));

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Invalid msg length: ID = 0x%%08lX, CC = %%d, Len = %%lu, Expected = %%lu");

    TestMsgId = CFE_SB_ValueToMsgId(CS_SEND_HK_MID);
    MsgSize   = 0;
    FcnCode   = 99;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &MsgSize, sizeof(MsgSize), false);

    /* Execute the function being tested */
    CS_HousekeepingCmd(&CmdPacket);

    /* Verify results */

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_LEN_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
} /* end CS_HousekeepingCmd_Test_InvalidMsgLength */

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

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
} /* end CS_UpdateCDS_Test_Nominal */

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

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
} /* end CS_UpdateCDS_Test_CopyToCDSFail */

void CS_UpdateCDS_Test_NullCDSHandle(void)
{
    CS_AppData.DataStoreHandle = CFE_ES_CDS_BAD_HANDLE;

    /* Execute the function being tested */
    CS_UpdateCDS();

    /* Verify results */
    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
} /* end CS_UpdateCDS_Test_NullCDSHandle */
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

    UtTest_Add(CS_AppPipe_Test_TableUpdateErrors, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_TableUpdateErrors");
    UtTest_Add(CS_AppPipe_Test_BackgroundCycle, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_BackgroundCycle");
    UtTest_Add(CS_AppPipe_Test_NoopCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_NoopCmd");
    UtTest_Add(CS_AppPipe_Test_ResetCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_ResetCmd");
    UtTest_Add(CS_AppPipe_Test_OneShotCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_OneShotCmd");
    UtTest_Add(CS_AppPipe_Test_CancelOneShotCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_CancelOneShotCmd");
    UtTest_Add(CS_AppPipe_Test_EnableAllCSCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_EnableAllCSCmd");
    UtTest_Add(CS_AppPipe_Test_DisableAllCSCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_DisableAllCSCmd");
    UtTest_Add(CS_AppPipe_Test_EnableCfeCoreCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_EnableCfeCoreCmd");
    UtTest_Add(CS_AppPipe_Test_DisableCfeCoreCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_DisableCfeCoreCmd");
    UtTest_Add(CS_AppPipe_Test_ReportBaselineCfeCoreCmd, CS_Test_Setup, CS_Test_TearDown,
               "CS_AppPipe_Test_ReportBaselineCfeCoreCmd");
    UtTest_Add(CS_AppPipe_Test_RecomputeBaselineCfeCoreCmd, CS_Test_Setup, CS_Test_TearDown,
               "CS_AppPipe_Test_RecomputeBaselineCfeCoreCmd");
    UtTest_Add(CS_AppPipe_Test_EnableOSCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_EnableOSCmd");
    UtTest_Add(CS_AppPipe_Test_DisableOSCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_DisableOSCmd");
    UtTest_Add(CS_AppPipe_Test_ReportBaselineOSCmd, CS_Test_Setup, CS_Test_TearDown,
               "CS_AppPipe_Test_ReportBaselineOSCmd");
    UtTest_Add(CS_AppPipe_Test_RecomputeBaselineOSCmd, CS_Test_Setup, CS_Test_TearDown,
               "CS_AppPipe_Test_RecomputeBaselineOSCmd");
    UtTest_Add(CS_AppPipe_Test_EnableEepromCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_EnableEepromCmd");
    UtTest_Add(CS_AppPipe_Test_DisableEepromCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_DisableEepromCmd");
    UtTest_Add(CS_AppPipe_Test_ReportBaselineEntryIDEepromCmd, CS_Test_Setup, CS_Test_TearDown,
               "CS_AppPipe_Test_ReportBaselineEntryIDEepromCmd");
    UtTest_Add(CS_AppPipe_Test_RecomputeBaselineEepromCmd, CS_Test_Setup, CS_Test_TearDown,
               "CS_AppPipe_Test_RecomputeBaselineEepromCmd");
    UtTest_Add(CS_AppPipe_Test_EnableEntryIDEepromCmd, CS_Test_Setup, CS_Test_TearDown,
               "CS_AppPipe_Test_EnableEntryIDEepromCmd");
    UtTest_Add(CS_AppPipe_Test_DisableEntryIDEepromCmd, CS_Test_Setup, CS_Test_TearDown,
               "CS_AppPipe_Test_DisableEntryIDEepromCmd");
    UtTest_Add(CS_AppPipe_Test_GetEntryIDEepromCmd, CS_Test_Setup, CS_Test_TearDown,
               "CS_AppPipe_Test_GetEntryIDEepromCmd");
    UtTest_Add(CS_AppPipe_Test_EnableMemoryCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_EnableMemoryCmd");
    UtTest_Add(CS_AppPipe_Test_DisableMemoryCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_DisableMemoryCmd");
    UtTest_Add(CS_AppPipe_Test_ReportBaselineEntryIDMemoryCmd, CS_Test_Setup, CS_Test_TearDown,
               "CS_AppPipe_Test_ReportBaselineEntryIDMemoryCmd");
    UtTest_Add(CS_AppPipe_Test_RecomputeBaselineMemoryCmd, CS_Test_Setup, CS_Test_TearDown,
               "CS_AppPipe_Test_RecomputeBaselineMemoryCmd");
    UtTest_Add(CS_AppPipe_Test_EnableEntryIDMemoryCmd, CS_Test_Setup, CS_Test_TearDown,
               "CS_AppPipe_Test_EnableEntryIDMemoryCmd");
    UtTest_Add(CS_AppPipe_Test_DisableEntryIDMemoryCmd, CS_Test_Setup, CS_Test_TearDown,
               "CS_AppPipe_Test_DisableEntryIDMemoryCmd");
    UtTest_Add(CS_AppPipe_Test_GetEntryIDMemoryCmd, CS_Test_Setup, CS_Test_TearDown,
               "CS_AppPipe_Test_GetEntryIDMemoryCmd");
    UtTest_Add(CS_AppPipe_Test_EnableTablesCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_EnableTablesCmd");
    UtTest_Add(CS_AppPipe_Test_DisableTablesCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_DisableTablesCmd");
    UtTest_Add(CS_AppPipe_Test_ReportBaselineTablesCmd, CS_Test_Setup, CS_Test_TearDown,
               "CS_AppPipe_Test_ReportBaselineTablesCmd");
    UtTest_Add(CS_AppPipe_Test_RecomputeBaselineTablesCmd, CS_Test_Setup, CS_Test_TearDown,
               "CS_AppPipe_Test_RecomputeBaselineTablesCmd");
    UtTest_Add(CS_AppPipe_Test_EnableNameTablesCmd, CS_Test_Setup, CS_Test_TearDown,
               "CS_AppPipe_Test_EnableNameTablesCmd");
    UtTest_Add(CS_AppPipe_Test_DisableNameTablesCmd, CS_Test_Setup, CS_Test_TearDown,
               "CS_AppPipe_Test_DisableNameTablesCmd");
    UtTest_Add(CS_AppPipe_Test_EnableAppCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_EnableAppCmd");
    UtTest_Add(CS_AppPipe_Test_DisableAppCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_DisableAppCmd");
    UtTest_Add(CS_AppPipe_Test_ReportBaselineAppCmd, CS_Test_Setup, CS_Test_TearDown,
               "CS_AppPipe_Test_ReportBaselineAppCmd");
    UtTest_Add(CS_AppPipe_Test_RecomputeBaselineAppCmd, CS_Test_Setup, CS_Test_TearDown,
               "CS_AppPipe_Test_RecomputeBaselineAppCmd");
    UtTest_Add(CS_AppPipe_Test_EnableNameAppCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_EnableNameAppCmd");
    UtTest_Add(CS_AppPipe_Test_DisableNameAppCmd, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_DisableNameAppCmd");
    UtTest_Add(CS_AppPipe_Test_InvalidCCError, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_InvalidCCError");
    UtTest_Add(CS_AppPipe_Test_InvalidMIDError, CS_Test_Setup, CS_Test_TearDown, "CS_AppPipe_Test_InvalidMIDError");

    UtTest_Add(CS_HousekeepingCmd_Test_Nominal, CS_Test_Setup, CS_Test_TearDown, "CS_HousekeepingCmd_Test_Nominal");
    UtTest_Add(CS_HousekeepingCmd_Test_InvalidMsgLength, CS_Test_Setup, CS_Test_TearDown,
               "CS_HousekeepingCmd_Test_InvalidMsgLength");

    UtTest_Add(CS_AppInit_Test_NominalPowerOnReset, CS_Test_Setup, CS_Test_TearDown,
               "CS_AppInit_Test_NominalPowerOnReset");
#if (CS_PRESERVE_STATES_ON_PROCESSOR_RESET == true)
    UtTest_Add(CS_AppInit_Test_NominalProcReset, CS_Test_Setup, CS_Test_TearDown, "CS_AppInit_Test_NominalProcReset");

    UtTest_Add(CS_CreateRestoreStatesFromCDS_Test_NoExistingCDS, CS_Test_Setup, CS_Test_TearDown,
               "CS_CreateRestoreStatesFromCDS_Test_NoExistingCDS");
    UtTest_Add(CS_CreateRestoreStatesFromCDS_Test_CDSSuccess, CS_Test_Setup, CS_Test_TearDown,
               "CS_CreateRestoreStatesFromCDS_Test_CDSSuccess");
    UtTest_Add(CS_CreateRestoreStatesFromCDS_Test_CDSFail, CS_Test_Setup, CS_Test_TearDown,
               "CS_CreateRestoreStatesFromCDS_Test_CDSFail");
    UtTest_Add(CS_CreateRestoreStatesFromCDS_Test_CopyToCDSFail, CS_Test_Setup, CS_Test_TearDown,
               "CS_CreateRestoreStatesFromCDS_Test_CopyToCDSFail");
    UtTest_Add(CS_CreateRestoreStatesFromCDS_Test_RegisterCDSFail, CS_Test_Setup, CS_Test_TearDown,
               "CS_CreateRestoreStatesFromCDS_Test_RegisterCDSFail");

    UtTest_Add(CS_UpdateCDS_Test_Nominal, CS_Test_Setup, CS_Test_TearDown, "CS_UpdateCDS_Test_Nominal");
    UtTest_Add(CS_UpdateCDS_Test_CopyToCDSFail, CS_Test_Setup, CS_Test_TearDown, "CS_UpdateCDS_Test_CopyToCDSFail");
    UtTest_Add(CS_UpdateCDS_Test_NullCDSHandle, CS_Test_Setup, CS_Test_TearDown, "CS_UpdateCDS_Test_NullCDSHandle");
#endif /* #if (CS_PRESERVE_STATES_ON_PROCESSOR_RESET == true) */
} /* end UtTest_Setup */

/************************/
/*  End of File Comment */
/************************/
