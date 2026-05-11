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

#include "cs_cmds.h"
#include "cs_msg.h"
#include "cs_msgdefs.h"
#include "cs_eventids.h"
#include "cs_version.h"
#include "cs_utils.h"
#include "cs_test_utils.h"
#include <unistd.h>
#include <stdlib.h>

/* UT includes */
#include "uttest.h"
#include "utassert.h"
#include "utstubs.h"

/* cs_cmds_tests globals */
uint8 call_count_CFE_EVS_SendEvent;

/*
 * Function Definitions
 */

CFE_Status_t CS_CMDS_TEST_CFE_ES_CreateChildTaskHook(uint32                       *TaskIdPtr,
                                                     const char                   *TaskName,
                                                     CFE_ES_ChildTaskMainFuncPtr_t FunctionPtr,
                                                     uint32                       *StackPtr,
                                                     uint32                        StackSize,
                                                     uint32                        Priority,
                                                     uint32                        Flags)
{
    *TaskIdPtr = 5;

    return CFE_SUCCESS;
}

void CS_SendHkCmd_Test_Nominal(void)
{
    CS_SendHkCmd_t    CmdPacket;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;
    size_t            MsgSize;

    memset(&CmdPacket, 0, sizeof(CmdPacket));

    TestMsgId = CFE_SB_ValueToMsgId(CS_SEND_HK_MID);
    MsgSize   = sizeof(CmdPacket);
    FcnCode   = 99;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &MsgSize, sizeof(MsgSize), false);

    CS_AppData.HkPacket.Payload.CommandCounter      = 1;
    CS_AppData.HkPacket.Payload.CommandErrorCounter = 2;
    CS_AppData.HkPacket.Payload.ChecksumState       = 3;
    CS_AppData.HkPacket.Payload.EepromCSState       = 4;
    CS_AppData.HkPacket.Payload.MemoryCSState       = 5;
    CS_AppData.HkPacket.Payload.AppCSState          = 6;
    CS_AppData.HkPacket.Payload.TablesCSState       = 7;
    CS_AppData.HkPacket.Payload.OSCSState           = 8;
    CS_AppData.HkPacket.Payload.CfeCoreCSState      = 9;
    CS_AppData.HkPacket.Payload.RecomputeInProgress = 10;
    CS_AppData.HkPacket.Payload.OneShotInProgress   = 11;
    CS_AppData.HkPacket.Payload.EepromCSErrCounter  = 12;
    CS_AppData.HkPacket.Payload.MemoryCSErrCounter  = 13;
    CS_AppData.HkPacket.Payload.AppCSErrCounter     = 14;
    CS_AppData.HkPacket.Payload.TablesCSErrCounter  = 15;
    CS_AppData.HkPacket.Payload.CfeCoreCSErrCounter = 16;
    CS_AppData.HkPacket.Payload.OSCSErrCounter      = 17;
    CS_AppData.HkPacket.Payload.CurrentCSTable      = 18;
    CS_AppData.HkPacket.Payload.CurrentEntryInTable = 19;
    CS_AppData.HkPacket.Payload.EepromBaseline      = 20;
    CS_AppData.HkPacket.Payload.OSBaseline          = 21;
    CS_AppData.HkPacket.Payload.CfeCoreBaseline     = 22;
    CS_AppData.HkPacket.Payload.LastOneShotAddress  = 23;
    CS_AppData.HkPacket.Payload.LastOneShotSize     = 24;
    CS_AppData.HkPacket.Payload.LastOneShotChecksum = 25;
    CS_AppData.HkPacket.Payload.PassCounter         = 26;

    /* Execute the function being tested */
    CS_SendHkCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.Payload.CommandCounter == 1, "CS_AppData.HkPacket.Payload.CommandCounter == 1");
    UtAssert_True(CS_AppData.HkPacket.Payload.CommandErrorCounter == 2,
                  "CS_AppData.HkPacket.Payload.CommandErrorCounter == 2");
    UtAssert_True(CS_AppData.HkPacket.Payload.ChecksumState == 3, "CS_AppData.HkPacket.Payload.ChecksumState == 3");
    UtAssert_True(CS_AppData.HkPacket.Payload.EepromCSState == 4, "CS_AppData.HkPacket.Payload.EepromCSState == 4");
    UtAssert_True(CS_AppData.HkPacket.Payload.MemoryCSState == 5, "CS_AppData.HkPacket.Payload.MemoryCSState == 5");
    UtAssert_True(CS_AppData.HkPacket.Payload.AppCSState == 6, "CS_AppData.HkPacket.Payload.AppCSState == 6");
    UtAssert_True(CS_AppData.HkPacket.Payload.TablesCSState == 7, "CS_AppData.HkPacket.Payload.TablesCSState == 7");
    UtAssert_True(CS_AppData.HkPacket.Payload.OSCSState == 8, "CS_AppData.HkPacket.Payload.OSCSState == 8");
    UtAssert_True(CS_AppData.HkPacket.Payload.CfeCoreCSState == 9, "CS_AppData.HkPacket.Payload.CfeCoreCSState == 9");
    UtAssert_True(CS_AppData.HkPacket.Payload.RecomputeInProgress == 10,
                  "CS_AppData.HkPacket.Payload.ChildTaskInUse == 10");
    UtAssert_True(CS_AppData.HkPacket.Payload.OneShotInProgress == 11,
                  "CS_AppData.HkPacket.Payload.OneShotInProgress == 11");
    UtAssert_True(CS_AppData.HkPacket.Payload.EepromCSErrCounter == 12,
                  "CS_AppData.HkPacket.Payload.EepromCSErrCounter == 12");
    UtAssert_True(CS_AppData.HkPacket.Payload.MemoryCSErrCounter == 13,
                  "CS_AppData.HkPacket.Payload.MemoryCSErrCounter == 13");
    UtAssert_True(CS_AppData.HkPacket.Payload.AppCSErrCounter == 14,
                  "CS_AppData.HkPacket.Payload.AppCSErrCounter == 14");
    UtAssert_True(CS_AppData.HkPacket.Payload.TablesCSErrCounter == 15,
                  "CS_AppData.HkPacket.Payload.TablesCSErrCounter == 15");
    UtAssert_True(CS_AppData.HkPacket.Payload.CfeCoreCSErrCounter == 16,
                  "CS_AppData.HkPacket.Payload.CfeCoreCSErrCounter == 16");
    UtAssert_True(CS_AppData.HkPacket.Payload.OSCSErrCounter == 17, "CS_AppData.HkPacket.Payload.OSCSErrCounter == 17");
    UtAssert_True(CS_AppData.HkPacket.Payload.CurrentCSTable == 18, "CS_AppData.HkPacket.Payload.CurrentCSTable == 18");
    UtAssert_True(CS_AppData.HkPacket.Payload.CurrentEntryInTable == 19,
                  "CS_AppData.HkPacket.Payload.CurrentEntryInTable == 19");
    UtAssert_True(CS_AppData.HkPacket.Payload.EepromBaseline == 20, "CS_AppData.HkPacket.Payload.EepromBaseline == 20");
    UtAssert_True(CS_AppData.HkPacket.Payload.OSBaseline == 21, "CS_AppData.HkPacket.Payload.OSBaseline == 21");
    UtAssert_True(CS_AppData.HkPacket.Payload.CfeCoreBaseline == 22,
                  "CS_AppData.HkPacket.Payload.CfeCoreBaseline == 22");
    UtAssert_True(CS_AppData.HkPacket.Payload.LastOneShotAddress == 23,
                  "CS_AppData.HkPacket.Payload.LastOneShotAddress == 23");
    UtAssert_True(CS_AppData.HkPacket.Payload.LastOneShotSize == 24,
                  "CS_AppData.HkPacket.Payload.LastOneShotSize == 24");
    UtAssert_True(CS_AppData.HkPacket.Payload.LastOneShotChecksum == 25,
                  "CS_AppData.HkPacket.Payload.LastOneShotChecksum == 25");
    UtAssert_True(CS_AppData.HkPacket.Payload.PassCounter == 26, "CS_AppData.HkPacket.Payload.PassCounter == 26");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0,
                  "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void CS_NoopCmd_Test(void)
{
    CS_NoopCmd_t CmdPacket;
    int32        strCmpResult;
    char         ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "No-op command. Version %%d.%%d.%%d.%%d");

    /* Execute the function being tested */
    CS_NoopCmd(&CmdPacket);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_NOOP_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1,
                  "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_ResetCountersCmd_Test(void)
{
    CS_ResetCountersCmd_t CmdPacket;
    int32                 strCmpResult;
    char                  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    memset(&CmdPacket, 0, sizeof(CmdPacket));
    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Reset Counters command recieved");

    CS_AppData.HkPacket.Payload.CommandCounter      = 1;
    CS_AppData.HkPacket.Payload.CommandErrorCounter = 2;
    CS_AppData.HkPacket.Payload.EepromCSErrCounter  = 3;
    CS_AppData.HkPacket.Payload.MemoryCSErrCounter  = 4;
    CS_AppData.HkPacket.Payload.TablesCSErrCounter  = 5;
    CS_AppData.HkPacket.Payload.AppCSErrCounter     = 6;
    CS_AppData.HkPacket.Payload.CfeCoreCSErrCounter = 7;
    CS_AppData.HkPacket.Payload.OSCSErrCounter      = 8;
    CS_AppData.HkPacket.Payload.PassCounter         = 9;

    /* Execute the function being tested */
    CS_ResetCountersCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.Payload.CommandCounter == 0, "CS_AppData.HkPacket.Payload.CommandCounter == 0");
    UtAssert_True(CS_AppData.HkPacket.Payload.CommandErrorCounter == 0,
                  "CS_AppData.HkPacket.Payload.CommandErrorCounter == 0");
    UtAssert_True(CS_AppData.HkPacket.Payload.EepromCSErrCounter == 0,
                  "CS_AppData.HkPacket.Payload.EepromCSErrCounter == 0");
    UtAssert_True(CS_AppData.HkPacket.Payload.MemoryCSErrCounter == 0,
                  "CS_AppData.HkPacket.Payload.MemoryCSErrCounter == 0");
    UtAssert_True(CS_AppData.HkPacket.Payload.TablesCSErrCounter == 0,
                  "CS_AppData.HkPacket.Payload.TablesCSErrCounter == 0");
    UtAssert_True(CS_AppData.HkPacket.Payload.AppCSErrCounter == 0, "CS_AppData.HkPacket.Payload.AppCSErrCounter == 0");
    UtAssert_True(CS_AppData.HkPacket.Payload.CfeCoreCSErrCounter == 0,
                  "CS_AppData.HkPacket.Payload.CfeCoreCSErrCounter == 0");
    UtAssert_True(CS_AppData.HkPacket.Payload.OSCSErrCounter == 0, "CS_AppData.HkPacket.Payload.OSCSErrCounter == 0");
    UtAssert_True(CS_AppData.HkPacket.Payload.PassCounter == 0, "CS_AppData.HkPacket.Payload.PassCounter == 0");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_RESET_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_DEBUG);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1,
                  "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_BackgroundCheckCycleCmd_Test_BackgroundInProgress(void)
{
    CS_BackgroundCheckCycleCmd_t CmdPacket;
    CFE_SB_MsgId_t               TestMsgId;
    CFE_MSG_FcnCode_t            FcnCode;
    size_t                       MsgSize;
    int32                        strCmpResult;
    char                         ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    memset(&CmdPacket, 0, sizeof(CmdPacket));
    snprintf(ExpectedEventString,
             CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Skipping background cycle. Recompute or oneshot in progress.");

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    MsgSize   = sizeof(CmdPacket);
    FcnCode   = 1;

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &MsgSize, sizeof(MsgSize), false);

    CS_AppData.HkPacket.Payload.ChecksumState  = CS_ChecksumState_ENABLED;
    CS_AppData.HkPacket.Payload.CurrentCSTable = CS_ChecksumType_CFECORE;
    CS_AppData.HkPacket.Payload.CfeCoreCSState = CS_ChecksumState_ENABLED;
    CS_AppData.CfeCoreCodeSeg.State            = CS_ChecksumState_ENABLED;

    CS_AppData.HkPacket.Payload.RecomputeInProgress = true;

    UT_SetDeferredRetcode(UT_KEY(CS_BackgroundCfeCore), 1, true);

    /* Execute the function being tested */
    CS_BackgroundCheckCycleCmd(&CmdPacket);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_BKGND_COMPUTE_PROG_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1,
                  "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_BackgroundCheckCycleCmd_Test_BackgroundCfeCore(void)
{
    CS_BackgroundCheckCycleCmd_t CmdPacket;
    CFE_SB_MsgId_t               TestMsgId;
    CFE_MSG_FcnCode_t            FcnCode;
    size_t                       MsgSize;

    memset(&CmdPacket, 0, sizeof(CmdPacket));
    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    MsgSize   = sizeof(CmdPacket);
    FcnCode   = 1;

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &MsgSize, sizeof(MsgSize), false);

    CS_AppData.HkPacket.Payload.ChecksumState  = CS_ChecksumState_ENABLED;
    CS_AppData.HkPacket.Payload.CurrentCSTable = CS_ChecksumType_CFECORE;
    CS_AppData.HkPacket.Payload.CfeCoreCSState = CS_ChecksumState_ENABLED;
    CS_AppData.CfeCoreCodeSeg.State            = CS_ChecksumState_ENABLED;

    UT_SetDeferredRetcode(UT_KEY(CS_BackgroundCfeCore), 1, true);

    /* Execute the function being tested */
    CS_BackgroundCheckCycleCmd(&CmdPacket);

    /* Verify results */
    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0,
                  "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void CS_BackgroundCheckCycleCmd_Test_BackgroundOS(void)
{
    CS_BackgroundCheckCycleCmd_t CmdPacket;
    CFE_SB_MsgId_t               TestMsgId;
    CFE_MSG_FcnCode_t            FcnCode;
    size_t                       MsgSize;

    memset(&CmdPacket, 0, sizeof(CmdPacket));
    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    MsgSize   = sizeof(CmdPacket);
    FcnCode   = 1;

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &MsgSize, sizeof(MsgSize), false);

    CS_AppData.HkPacket.Payload.ChecksumState  = CS_ChecksumState_ENABLED;
    CS_AppData.HkPacket.Payload.CurrentCSTable = CS_ChecksumType_OSCORE;
    CS_AppData.HkPacket.Payload.OSCSState      = CS_ChecksumState_ENABLED;
    CS_AppData.OSCodeSeg.State                 = CS_ChecksumState_ENABLED;

    UT_SetDeferredRetcode(UT_KEY(CS_BackgroundOS), 1, true);

    /* Execute the function being tested */
    CS_BackgroundCheckCycleCmd(&CmdPacket);

    /* Verify results */
    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0,
                  "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void CS_BackgroundCheckCycleCmd_Test_BackgroundEeprom(void)
{
    CS_BackgroundCheckCycleCmd_t CmdPacket;
    CFE_SB_MsgId_t               TestMsgId;
    CFE_MSG_FcnCode_t            FcnCode;
    size_t                       MsgSize;

    memset(&CmdPacket, 0, sizeof(CmdPacket));
    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    MsgSize   = sizeof(CmdPacket);
    FcnCode   = 99;

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &MsgSize, sizeof(MsgSize), false);

    CS_AppData.HkPacket.Payload.ChecksumState  = CS_ChecksumState_ENABLED;
    CS_AppData.HkPacket.Payload.CurrentCSTable = CS_ChecksumType_EEPROM_TABLE;
    CS_AppData.HkPacket.Payload.EepromCSState  = CS_ChecksumState_ENABLED;

    UT_SetDeferredRetcode(UT_KEY(CS_BackgroundEeprom), 1, true);

    /* Execute the function being tested */
    CS_BackgroundCheckCycleCmd(&CmdPacket);

    /* Verify results */
    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0,
                  "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void CS_BackgroundCheckCycleCmd_Test_BackgroundMemory(void)
{
    CS_BackgroundCheckCycleCmd_t CmdPacket;
    CFE_SB_MsgId_t               TestMsgId;
    CFE_MSG_FcnCode_t            FcnCode;
    size_t                       MsgSize;

    memset(&CmdPacket, 0, sizeof(CmdPacket));
    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    MsgSize   = sizeof(CmdPacket);
    FcnCode   = 99;

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &MsgSize, sizeof(MsgSize), false);

    CS_AppData.HkPacket.Payload.ChecksumState  = CS_ChecksumState_ENABLED;
    CS_AppData.HkPacket.Payload.CurrentCSTable = CS_ChecksumType_MEMORY_TABLE;
    CS_AppData.HkPacket.Payload.MemoryCSState  = CS_ChecksumState_ENABLED;

    UT_SetDeferredRetcode(UT_KEY(CS_BackgroundMemory), 1, true);

    /* Execute the function being tested */
    CS_BackgroundCheckCycleCmd(&CmdPacket);

    /* Verify results */
    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0,
                  "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void CS_BackgroundCheckCycleCmd_Test_BackgroundTables(void)
{
    CS_BackgroundCheckCycleCmd_t CmdPacket;
    CFE_SB_MsgId_t               TestMsgId;
    CFE_MSG_FcnCode_t            FcnCode;
    size_t                       MsgSize;

    memset(&CmdPacket, 0, sizeof(CmdPacket));
    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    MsgSize   = sizeof(CmdPacket);
    FcnCode   = 1;

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &MsgSize, sizeof(MsgSize), false);

    CS_AppData.HkPacket.Payload.ChecksumState  = CS_ChecksumState_ENABLED;
    CS_AppData.HkPacket.Payload.CurrentCSTable = CS_ChecksumType_TABLES_TABLE;
    CS_AppData.HkPacket.Payload.TablesCSState  = CS_ChecksumState_ENABLED;

    UT_SetDeferredRetcode(UT_KEY(CS_BackgroundTables), 1, true);

    /* Execute the function being tested */
    CS_BackgroundCheckCycleCmd(&CmdPacket);

    /* Verify results */
    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0,
                  "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void CS_BackgroundCheckCycleCmd_Test_BackgroundApp(void)
{
    CS_BackgroundCheckCycleCmd_t CmdPacket;
    CFE_SB_MsgId_t               TestMsgId;
    CFE_MSG_FcnCode_t            FcnCode;
    size_t                       MsgSize;

    memset(&CmdPacket, 0, sizeof(CmdPacket));
    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    MsgSize   = sizeof(CmdPacket);
    FcnCode   = 1;

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &MsgSize, sizeof(MsgSize), false);

    CS_AppData.HkPacket.Payload.ChecksumState  = CS_ChecksumState_ENABLED;
    CS_AppData.HkPacket.Payload.CurrentCSTable = CS_ChecksumType_APP_TABLE;
    CS_AppData.HkPacket.Payload.AppCSState     = CS_ChecksumState_ENABLED;

    UT_SetDeferredRetcode(UT_KEY(CS_BackgroundApp), 1, true);

    /* Execute the function being tested */
    CS_BackgroundCheckCycleCmd(&CmdPacket);

    /* Verify results */
    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0,
                  "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void CS_BackgroundCheckCycleCmd_Test_Default(void)
{
    CS_BackgroundCheckCycleCmd_t CmdPacket;
    CFE_SB_MsgId_t               TestMsgId;
    CFE_MSG_FcnCode_t            FcnCode;
    size_t                       MsgSize;

    memset(&CmdPacket, 0, sizeof(CmdPacket));
    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    MsgSize   = sizeof(CmdPacket);
    FcnCode   = 1;

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &MsgSize, sizeof(MsgSize), false);

    CS_AppData.HkPacket.Payload.ChecksumState  = CS_ChecksumState_ENABLED;
    CS_AppData.HkPacket.Payload.CurrentCSTable = 99;

    /* Execute the function being tested */
    CS_BackgroundCheckCycleCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.Payload.CurrentCSTable == 0, "CS_AppData.HkPacket.Payload.CurrentCSTable == 0");
    UtAssert_True(CS_AppData.HkPacket.Payload.CurrentEntryInTable == 0,
                  "CS_AppData.HkPacket.Payload.CurrentEntryInTable == 0");
    UtAssert_True(CS_AppData.HkPacket.Payload.PassCounter == 1, "CS_AppData.HkPacket.Payload.PassCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0,
                  "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void CS_BackgroundCheckCycleCmd_Test_Disabled(void)
{
    CS_BackgroundCheckCycleCmd_t CmdPacket;
    CFE_SB_MsgId_t               TestMsgId;
    CFE_MSG_FcnCode_t            FcnCode;
    size_t                       MsgSize;

    memset(&CmdPacket, 0, sizeof(CmdPacket));
    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    MsgSize   = sizeof(CmdPacket);
    FcnCode   = 1;

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &MsgSize, sizeof(MsgSize), false);

    CS_AppData.HkPacket.Payload.ChecksumState = CS_ChecksumState_DISABLED;

    /* Execute the function being tested */
    CS_BackgroundCheckCycleCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.Payload.PassCounter == 0, "CS_AppData.HkPacket.Payload.PassCounter == 0");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0,
                  "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void CS_BackgroundCheckCycleCmd_Test_OneShot(void)
{
    CS_BackgroundCheckCycleCmd_t CmdPacket;
    CFE_SB_MsgId_t               TestMsgId;
    CFE_MSG_FcnCode_t            FcnCode;
    size_t                       MsgSize;
    int32                        strCmpResult;
    char                         ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    memset(&CmdPacket, 0, sizeof(CmdPacket));
    snprintf(ExpectedEventString,
             CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Skipping background cycle. Recompute or oneshot in progress.");

    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    MsgSize   = sizeof(CmdPacket);
    FcnCode   = 1;

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &MsgSize, sizeof(MsgSize), false);

    CS_AppData.HkPacket.Payload.ChecksumState  = CS_ChecksumState_ENABLED;
    CS_AppData.HkPacket.Payload.CurrentCSTable = CS_ChecksumType_CFECORE;
    CS_AppData.HkPacket.Payload.CfeCoreCSState = CS_ChecksumState_ENABLED;
    CS_AppData.CfeCoreCodeSeg.State            = CS_ChecksumState_ENABLED;

    CS_AppData.HkPacket.Payload.RecomputeInProgress = false;
    CS_AppData.HkPacket.Payload.OneShotInProgress   = true;

    UT_SetDeferredRetcode(UT_KEY(CS_BackgroundCfeCore), 1, true);

    /* Execute the function being tested */
    CS_BackgroundCheckCycleCmd(&CmdPacket);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_BKGND_COMPUTE_PROG_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1,
                  "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_BackgroundCheckCycleCmd_Test_EndOfList(void)
{
    CS_BackgroundCheckCycleCmd_t CmdPacket;
    CFE_SB_MsgId_t               TestMsgId;
    CFE_MSG_FcnCode_t            FcnCode;
    size_t                       MsgSize;

    memset(&CmdPacket, 0, sizeof(CmdPacket));
    TestMsgId = CFE_SB_ValueToMsgId(CS_CMD_MID);
    MsgSize   = sizeof(CmdPacket);
    FcnCode   = 1;

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &MsgSize, sizeof(MsgSize), false);

    CS_AppData.HkPacket.Payload.ChecksumState  = CS_ChecksumState_ENABLED;
    CS_AppData.HkPacket.Payload.CurrentCSTable = CS_NUM_TABLES - 1;
    CS_AppData.HkPacket.Payload.CfeCoreCSState = CS_ChecksumState_ENABLED;
    CS_AppData.CfeCoreCodeSeg.State            = CS_ChecksumState_ENABLED;

    CS_AppData.HkPacket.Payload.RecomputeInProgress = false;
    CS_AppData.HkPacket.Payload.OneShotInProgress   = false;

    UT_SetDeferredRetcode(UT_KEY(CS_BackgroundCfeCore), 1, true);

    /* Execute the function being tested */
    CS_BackgroundCheckCycleCmd(&CmdPacket);

    /* Verify results */
    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0,
                  "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void CS_DisableAllCSCmd_Test(void)
{
    CS_DisableAllCSCmd_t CmdPacket;
    int32                strCmpResult;
    char                 ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    memset(&CmdPacket, 0, sizeof(CmdPacket));
    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Background Checksumming Disabled");

    /* Execute the function being tested */
    CS_DisableAllCSCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.Payload.ChecksumState == CS_ChecksumState_DISABLED,
                  "CS_AppData.HkPacket.Payload.ChecksumState == CS_ChecksumState_DISABLED");
    UtAssert_True(CS_AppData.HkPacket.Payload.CommandCounter == 1, "CS_AppData.HkPacket.Payload.CommandCounter == 1");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_DISABLE_ALL_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1,
                  "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_EnableAllCSCmd_Test(void)
{
    CS_EnableAllCSCmd_t CmdPacket;
    int32               strCmpResult;
    char                ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    memset(&CmdPacket, 0, sizeof(CmdPacket));
    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Background Checksumming Enabled");

    /* Execute the function being tested */
    CS_EnableAllCSCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.Payload.ChecksumState == CS_ChecksumState_ENABLED,
                  "CS_AppData.HkPacket.Payload.ChecksumState == CS_ChecksumState_ENABLED");
    UtAssert_True(CS_AppData.HkPacket.Payload.CommandCounter == 1, "CS_AppData.HkPacket.Payload.CommandCounter == 1");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_ENABLE_ALL_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1,
                  "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_DisableCfeCoreCmd_Test(void)
{
    CS_DisableCfeCoreCmd_t CmdPacket;
    int32                  strCmpResult;
    char                   ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    memset(&CmdPacket, 0, sizeof(CmdPacket));
    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Checksumming of cFE Core is Disabled");

    /* Execute the function being tested */
    CS_DisableCfeCoreCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.Payload.CfeCoreCSState == CS_ChecksumState_DISABLED,
                  "CS_AppData.HkPacket.Payload.CfeCoreCSState == CS_ChecksumState_DISABLED");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_DISABLE_CFECORE_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.Payload.CommandCounter == 1, "CS_AppData.HkPacket.Payload.CommandCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1,
                  "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_EnableCfeCoreCmd_Test(void)
{
    CS_EnableCfeCoreCmd_t CmdPacket;
    int32                 strCmpResult;
    char                  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    memset(&CmdPacket, 0, sizeof(CmdPacket));
    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Checksumming of cFE Core is Enabled");

    /* Execute the function being tested */
    CS_EnableCfeCoreCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.Payload.CfeCoreCSState == CS_ChecksumState_ENABLED,
                  "CS_AppData.HkPacket.Payload.CfeCoreCSState == CS_ChecksumState_ENABLED");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_ENABLE_CFECORE_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.Payload.CommandCounter == 1, "CS_AppData.HkPacket.Payload.CommandCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1,
                  "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_DisableOSCmd_Test(void)
{
    CS_DisableOSCmd_t CmdPacket;
    int32             strCmpResult;
    char              ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    memset(&CmdPacket, 0, sizeof(CmdPacket));
    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Checksumming of OS code segment is Disabled");

    /* Execute the function being tested */
    CS_DisableOSCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.Payload.OSCSState == CS_ChecksumState_DISABLED,
                  "CS_AppData.HkPacket.Payload.OSCSState == CS_ChecksumState_DISABLED");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_DISABLE_OS_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.Payload.CommandCounter == 1, "CS_AppData.HkPacket.Payload.CommandCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1,
                  "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_EnableOSCmd_Test(void)
{
    CS_EnableOSCmd_t CmdPacket;
    int32            strCmpResult;
    char             ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    memset(&CmdPacket, 0, sizeof(CmdPacket));
    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Checksumming of OS code segment is Enabled");

    /* Execute the function being tested */
    CS_EnableOSCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.Payload.OSCSState == CS_ChecksumState_ENABLED,
                  "CS_AppData.HkPacket.Payload.OSCSState == CS_ChecksumState_ENABLED");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_ENABLE_OS_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.Payload.CommandCounter == 1, "CS_AppData.HkPacket.Payload.CommandCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1,
                  "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_ReportBaselineCfeCoreCmd_Test_Nominal(void)
{
    CS_ReportBaselineCfeCoreCmd_t CmdPacket;
    int32                         strCmpResult;
    char                          ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    memset(&CmdPacket, 0, sizeof(CmdPacket));
    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Baseline of cFE Core is 0x%%08X");

    CS_AppData.CfeCoreCodeSeg.ComputedYet     = true;
    CS_AppData.CfeCoreCodeSeg.ComparisonValue = -1;

    /* Execute the function being tested */
    CS_ReportBaselineCfeCoreCmd(&CmdPacket);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_BASELINE_CFECORE_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.Payload.CommandCounter == 1, "CS_AppData.HkPacket.Payload.CommandCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1,
                  "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_ReportBaselineCfeCoreCmd_Test_NotComputedYet(void)
{
    CS_ReportBaselineCfeCoreCmd_t CmdPacket;
    int32                         strCmpResult;
    char                          ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    memset(&CmdPacket, 0, sizeof(CmdPacket));
    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Baseline of cFE Core has not been computed yet");

    CS_AppData.CfeCoreCodeSeg.ComputedYet = false;

    /* Execute the function being tested */
    CS_ReportBaselineCfeCoreCmd(&CmdPacket);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_NO_BASELINE_CFECORE_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.Payload.CommandCounter == 1, "CS_AppData.HkPacket.Payload.CommandCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1,
                  "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_ReportBaselineOSCmd_Test_Nominal(void)
{
    CS_ReportBaselineOSCmd_t CmdPacket;
    int32                    strCmpResult;
    char                     ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    memset(&CmdPacket, 0, sizeof(CmdPacket));
    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Baseline of OS code segment is 0x%%08X");

    CS_AppData.OSCodeSeg.ComputedYet     = true;
    CS_AppData.OSCodeSeg.ComparisonValue = -1;

    /* Execute the function being tested */
    CS_ReportBaselineOSCmd(&CmdPacket);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_BASELINE_OS_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.Payload.CommandCounter == 1, "CS_AppData.HkPacket.Payload.CommandCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1,
                  "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_ReportBaselineOSCmd_Test_NotComputedYet(void)
{
    CS_ReportBaselineOSCmd_t CmdPacket;
    int32                    strCmpResult;
    char                     ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    memset(&CmdPacket, 0, sizeof(CmdPacket));
    snprintf(ExpectedEventString,
             CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Baseline of OS code segment has not been computed yet");

    CS_AppData.OSCodeSeg.ComputedYet = false;

    /* Execute the function being tested */
    CS_ReportBaselineOSCmd(&CmdPacket);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_NO_BASELINE_OS_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.Payload.CommandCounter == 1, "CS_AppData.HkPacket.Payload.CommandCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1,
                  "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_RecomputeBaselineCfeCoreCmd_Test_Nominal(void)
{
    CS_RecomputeBaselineCfeCoreCmd_t CmdPacket;
    int32                            strCmpResult;
    char                             ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    memset(&CmdPacket, 0, sizeof(CmdPacket));
    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Recompute of cFE core started");

    CS_AppData.HkPacket.Payload.RecomputeInProgress = false;

    /* Execute the function being tested */
    CS_RecomputeBaselineCfeCoreCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.Payload.RecomputeInProgress == true,
                  "CS_AppData.HkPacket.Payload.RecomputeInProgress == true");
    UtAssert_True(CS_AppData.HkPacket.Payload.OneShotInProgress == false,
                  "CS_AppData.HkPacket.Payload.OneShotInProgress == false");
    UtAssert_True(CS_AppData.ChildTaskTable == CS_ChecksumType_CFECORE,
                  "CS_AppData.ChildTaskTable == CS_ChecksumType_CFECORE");
    UtAssert_True(CS_AppData.ChildTaskEntryID == 0, "CS_AppData.ChildTaskEntryID == 0");
    UtAssert_True(CS_AppData.RecomputeEepromMemoryEntryPtr == &CS_AppData.CfeCoreCodeSeg,
                  "CS_AppData.RecomputeEepromMemoryEntryPtr == &CS_AppData.CfeCoreCodeSeg");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_RECOMPUTE_CFECORE_STARTED_DBG_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_DEBUG);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.Payload.CommandCounter == 1, "CS_AppData.HkPacket.Payload.CommandCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1,
                  "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_RecomputeBaselineCfeCoreCmd_Test_CreateChildTaskError(void)
{
    CS_RecomputeBaselineCfeCoreCmd_t CmdPacket;
    int32                            strCmpResult;
    char                             ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    memset(&CmdPacket, 0, sizeof(CmdPacket));
    snprintf(ExpectedEventString,
             CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Recompute cFE core failed, CFE_ES_CreateChildTask returned: 0x%%08X");

    CS_AppData.HkPacket.Payload.RecomputeInProgress = false;

    /* Set to generate error message CS_RECOMPUTE_CFECORE_CREATE_CHDTASK_ERR_EID */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_CreateChildTask), 1, -1);

    /* Execute the function being tested */
    CS_RecomputeBaselineCfeCoreCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.Payload.OneShotInProgress == false,
                  "CS_AppData.HkPacket.Payload.OneShotInProgress == false");
    UtAssert_True(CS_AppData.ChildTaskTable == CS_ChecksumType_CFECORE,
                  "CS_AppData.ChildTaskTable == CS_ChecksumType_CFECORE");
    UtAssert_True(CS_AppData.ChildTaskEntryID == 0, "CS_AppData.ChildTaskEntryID == 0");
    UtAssert_True(CS_AppData.RecomputeEepromMemoryEntryPtr == &CS_AppData.CfeCoreCodeSeg,
                  "CS_AppData.RecomputeEepromMemoryEntryPtr == &CS_AppData.CfeCoreCodeSeg");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_RECOMPUTE_CFECORE_CREATE_CHDTASK_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.Payload.CommandErrorCounter == 1,
                  "CS_AppData.HkPacket.Payload.CommandErrorCounter == 1");
    UtAssert_True(CS_AppData.HkPacket.Payload.RecomputeInProgress == false,
                  "CS_AppData.HkPacket.Payload.RecomputeInProgress == false");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1,
                  "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_RecomputeBaselineCfeCoreCmd_Test_ChildTaskError(void)
{
    CS_RecomputeBaselineCfeCoreCmd_t CmdPacket;
    int32                            strCmpResult;
    char                             ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    memset(&CmdPacket, 0, sizeof(CmdPacket));
    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Recompute cFE core failed: child task in use");

    CS_AppData.HkPacket.Payload.RecomputeInProgress = true;

    /* Execute the function being tested */
    CS_RecomputeBaselineCfeCoreCmd(&CmdPacket);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_RECOMPUTE_CFECORE_CHDTASK_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.Payload.CommandErrorCounter == 1,
                  "CS_AppData.HkPacket.Payload.CommandErrorCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1,
                  "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_RecomputeBaselineCfeCoreCmd_Test_OneShot(void)
{
    CS_RecomputeBaselineCfeCoreCmd_t CmdPacket;
    int32                            strCmpResult;
    char                             ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    memset(&CmdPacket, 0, sizeof(CmdPacket));
    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Recompute cFE core failed: child task in use");

    CS_AppData.HkPacket.Payload.RecomputeInProgress = false;

    CS_AppData.HkPacket.Payload.OneShotInProgress = true;

    /* Execute the function being tested */
    CS_RecomputeBaselineCfeCoreCmd(&CmdPacket);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_RECOMPUTE_CFECORE_CHDTASK_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.Payload.CommandErrorCounter == 1,
                  "CS_AppData.HkPacket.Payload.CommandErrorCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1,
                  "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_RecomputeBaselineOSCmd_Test_Nominal(void)
{
    CS_RecomputeBaselineOSCmd_t CmdPacket;
    int32                       strCmpResult;
    char                        ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    memset(&CmdPacket, 0, sizeof(CmdPacket));
    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Recompute of OS code segment started");

    CS_AppData.HkPacket.Payload.RecomputeInProgress = false;

    /* Execute the function being tested */
    CS_RecomputeBaselineOSCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.Payload.RecomputeInProgress == true,
                  "CS_AppData.HkPacket.Payload.RecomputeInProgress == true");
    UtAssert_True(CS_AppData.HkPacket.Payload.OneShotInProgress == false,
                  "CS_AppData.HkPacket.Payload.OneShotInProgress == false");
    UtAssert_True(CS_AppData.ChildTaskTable == CS_ChecksumType_OSCORE,
                  "CS_AppData.ChildTaskTable == CS_ChecksumType_OSCORE");
    UtAssert_True(CS_AppData.ChildTaskEntryID == 0, "CS_AppData.OSCodeSeg == 0");
    UtAssert_True(CS_AppData.RecomputeEepromMemoryEntryPtr == &CS_AppData.OSCodeSeg,
                  "CS_AppData.RecomputeEepromMemoryEntryPtr == &CS_AppData.OSCodeSeg");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_RECOMPUTE_OS_STARTED_DBG_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_DEBUG);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.Payload.CommandCounter == 1, "CS_AppData.HkPacket.Payload.CommandCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1,
                  "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_RecomputeBaselineOSCmd_Test_CreateChildTaskError(void)
{
    CS_RecomputeBaselineOSCmd_t CmdPacket;
    int32                       strCmpResult;
    char                        ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    memset(&CmdPacket, 0, sizeof(CmdPacket));
    snprintf(ExpectedEventString,
             CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Recompute OS code segment failed, CFE_ES_CreateChildTask returned: 0x%%08X");

    CS_AppData.HkPacket.Payload.RecomputeInProgress = false;

    /* Set to generate error message CS_RECOMPUTE_OS_CREATE_CHDTASK_ERR_EID */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_CreateChildTask), 1, -1);

    /* Execute the function being tested */
    CS_RecomputeBaselineOSCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.Payload.OneShotInProgress == false,
                  "CS_AppData.HkPacket.Payload.OneShotInProgress == false");
    UtAssert_True(CS_AppData.ChildTaskTable == CS_ChecksumType_OSCORE,
                  "CS_AppData.ChildTaskTable == CS_ChecksumType_OSCORE");
    UtAssert_True(CS_AppData.ChildTaskEntryID == 0, "CS_AppData.OSCodeSeg == 0");
    UtAssert_True(CS_AppData.RecomputeEepromMemoryEntryPtr == &CS_AppData.OSCodeSeg,
                  "CS_AppData.RecomputeEepromMemoryEntryPtr == &CS_AppData.OSCodeSeg");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_RECOMPUTE_OS_CREATE_CHDTASK_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.Payload.CommandErrorCounter == 1,
                  "CS_AppData.HkPacket.Payload.CommandErrorCounter == 1");
    UtAssert_True(CS_AppData.HkPacket.Payload.RecomputeInProgress == false,
                  "CS_AppData.HkPacket.Payload.RecomputeInProgress == false");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1,
                  "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_RecomputeBaselineOSCmd_Test_ChildTaskError(void)
{
    CS_RecomputeBaselineOSCmd_t CmdPacket;
    int32                       strCmpResult;
    char                        ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    memset(&CmdPacket, 0, sizeof(CmdPacket));
    snprintf(ExpectedEventString,
             CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Recompute OS code segment failed: child task in use");

    CS_AppData.HkPacket.Payload.RecomputeInProgress = true;

    /* Execute the function being tested */
    CS_RecomputeBaselineOSCmd(&CmdPacket);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_RECOMPUTE_OS_CHDTASK_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.Payload.CommandErrorCounter == 1,
                  "CS_AppData.HkPacket.Payload.CommandErrorCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1,
                  "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_RecomputeBaselineOSCmd_Test_OneShot(void)
{
    CS_RecomputeBaselineOSCmd_t CmdPacket;
    int32                       strCmpResult;
    char                        ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    memset(&CmdPacket, 0, sizeof(CmdPacket));
    snprintf(ExpectedEventString,
             CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Recompute OS code segment failed: child task in use");

    CS_AppData.HkPacket.Payload.RecomputeInProgress = false;

    CS_AppData.HkPacket.Payload.OneShotInProgress = true;

    /* Execute the function being tested */
    CS_RecomputeBaselineOSCmd(&CmdPacket);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_RECOMPUTE_OS_CHDTASK_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.Payload.CommandErrorCounter == 1,
                  "CS_AppData.HkPacket.Payload.CommandErrorCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1,
                  "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_OneShotCmd_Test_Nominal(void)
{
    CS_OneShotCmd_t CmdPacket;
    int32           strCmpResult;
    char            ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    memset(&CmdPacket, 0, sizeof(CmdPacket));
    snprintf(ExpectedEventString,
             CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "OneShot checksum started on address: 0x%%08X, size: %%d");

    CmdPacket.Payload.Address          = 0x00000001;
    CmdPacket.Payload.Size             = 2;
    CmdPacket.Payload.MaxBytesPerCycle = 0;

    CS_AppData.HkPacket.Payload.RecomputeInProgress = false;
    CS_AppData.MaxBytesPerCycle                     = 8;

    /* Execute the function being tested */
    CS_OneShotCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.Payload.RecomputeInProgress == false,
                  "CS_AppData.HkPacket.Payload.RecomputeInProgress == false");
    UtAssert_True(CS_AppData.HkPacket.Payload.OneShotInProgress == true,
                  "CS_AppData.HkPacket.Payload.OneShotInProgress == true");

    UtAssert_True(CS_AppData.HkPacket.Payload.LastOneShotAddress == CmdPacket.Payload.Address,
                  "CS_AppData.HkPacket.Payload.LastOneShotAddress == CmdPacket.Payload.Address");
    UtAssert_True(CS_AppData.HkPacket.Payload.LastOneShotSize == CmdPacket.Payload.Size,
                  "CS_AppData.HkPacket.Payload.LastOneShotSize == CmdPacket.Payload.Size");
    UtAssert_True(CS_AppData.HkPacket.Payload.LastOneShotChecksum == 0,
                  "CS_AppData.HkPacket.Payload.LastOneShotChecksum == 0");
    UtAssert_True(CS_AppData.HkPacket.Payload.LastOneShotMaxBytesPerCycle == CS_AppData.MaxBytesPerCycle,
                  "CS_AppData.HkPacket.Payload.LastOneShotMaxBytesPerCycle == CS_AppData.MaxBytesPerCycle");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_ONESHOT_STARTED_DBG_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_DEBUG);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_BOOL_TRUE(CFE_RESOURCEID_TEST_DEFINED(CS_AppData.ChildTaskID));
    UtAssert_True(CS_AppData.HkPacket.Payload.CommandCounter == 1, "CS_AppData.HkPacket.Payload.CommandCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1,
                  "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_OneShotCmd_Test_MaxBytesPerCycleNonZero(void)
{
    CS_OneShotCmd_t CmdPacket;
    int32           strCmpResult;
    char            ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    memset(&CmdPacket, 0, sizeof(CmdPacket));
    snprintf(ExpectedEventString,
             CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "OneShot checksum started on address: 0x%%08X, size: %%d");

    CmdPacket.Payload.Address          = 0x00000001;
    CmdPacket.Payload.Size             = 2;
    CmdPacket.Payload.MaxBytesPerCycle = 1;

    CS_AppData.HkPacket.Payload.RecomputeInProgress = false;
    CS_AppData.MaxBytesPerCycle                     = 8;

    /* Execute the function being tested */
    CS_OneShotCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.Payload.RecomputeInProgress == false,
                  "CS_AppData.HkPacket.Payload.RecomputeInProgress == false");
    UtAssert_True(CS_AppData.HkPacket.Payload.OneShotInProgress == true,
                  "CS_AppData.HkPacket.Payload.OneShotInProgress == true");

    UtAssert_True(CS_AppData.HkPacket.Payload.LastOneShotAddress == CmdPacket.Payload.Address,
                  "CS_AppData.HkPacket.Payload.LastOneShotAddress == CmdPacket.Payload.Address");
    UtAssert_True(CS_AppData.HkPacket.Payload.LastOneShotSize == CmdPacket.Payload.Size,
                  "CS_AppData.HkPacket.Payload.LastOneShotSize == CmdPacket.Payload.Size");
    UtAssert_True(CS_AppData.HkPacket.Payload.LastOneShotChecksum == 0,
                  "CS_AppData.HkPacket.Payload.LastOneShotChecksum == 0");
    UtAssert_True(CS_AppData.HkPacket.Payload.LastOneShotMaxBytesPerCycle == CmdPacket.Payload.MaxBytesPerCycle,
                  "CS_AppData.HkPacket.Payload.LastOneShotMaxBytesPerCycle == CmdPacket.Payload.MaxBytesPerCycle");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_ONESHOT_STARTED_DBG_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_DEBUG);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_BOOL_TRUE(CFE_RESOURCEID_TEST_DEFINED(CS_AppData.ChildTaskID));
    UtAssert_True(CS_AppData.HkPacket.Payload.CommandCounter == 1, "CS_AppData.HkPacket.Payload.CommandCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1,
                  "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_OneShotCmd_Test_CreateChildTaskError(void)
{
    CS_OneShotCmd_t CmdPacket;
    int32           strCmpResult;
    char            ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    memset(&CmdPacket, 0, sizeof(CmdPacket));

    snprintf(ExpectedEventString,
             CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "OneShot checkum failed, CFE_ES_CreateChildTask returned: 0x%%08X");

    CS_AppData.HkPacket.Payload.RecomputeInProgress = false;

    /* Set to generate error message CS_RECOMPUTE_OS_CREATE_CHDTASK_ERR_EID */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_CreateChildTask), 1, -1);

    /* Execute the function being tested */
    CS_OneShotCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.Payload.LastOneShotAddress == CmdPacket.Payload.Address,
                  "CS_AppData.HkPacket.Payload.LastOneShotAddress == CmdPacket.Payload.Address");
    UtAssert_True(CS_AppData.HkPacket.Payload.LastOneShotSize == CmdPacket.Payload.Size,
                  "CS_AppData.HkPacket.Payload.LastOneShotSize == CmdPacket.Payload.Size");
    UtAssert_True(CS_AppData.HkPacket.Payload.LastOneShotChecksum == 0,
                  "CS_AppData.HkPacket.Payload.LastOneShotChecksum == 0");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_ONESHOT_CREATE_CHDTASK_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.Payload.CommandErrorCounter == 1,
                  "CS_AppData.HkPacket.Payload.CommandErrorCounter == 1");
    UtAssert_True(CS_AppData.HkPacket.Payload.RecomputeInProgress == false,
                  "CS_AppData.HkPacket.Payload.RecomputeInProgress == false");
    UtAssert_True(CS_AppData.HkPacket.Payload.OneShotInProgress == false,
                  "CS_AppData.HkPacket.Payload.OneShotInProgress == false");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1,
                  "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_OneShotCmd_Test_ChildTaskError(void)
{
    CS_OneShotCmd_t CmdPacket;
    int32           strCmpResult;
    char            ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    memset(&CmdPacket, 0, sizeof(CmdPacket));
    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "OneShot checksum failed: child task in use");

    CS_AppData.HkPacket.Payload.RecomputeInProgress = true;

    /* Execute the function being tested */
    CS_OneShotCmd(&CmdPacket);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_ONESHOT_CHDTASK_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.Payload.CommandErrorCounter == 1,
                  "CS_AppData.HkPacket.Payload.CommandErrorCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1,
                  "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_OneShotCmd_Test_MemValidateRangeError(void)
{
    CS_OneShotCmd_t CmdPacket;
    int32           strCmpResult;
    char            ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    memset(&CmdPacket, 0, sizeof(CmdPacket));
    snprintf(ExpectedEventString,
             CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "OneShot checksum failed, CFE_PSP_MemValidateRange returned: 0x%%08X");

    CS_AppData.HkPacket.Payload.RecomputeInProgress = true;

    /* Set to generate error message CS_ONESHOT_MEMVALIDATE_ERR_EID */
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_MemValidateRange), 1, -1);

    /* Execute the function being tested */
    CS_OneShotCmd(&CmdPacket);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_ONESHOT_MEMVALIDATE_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.Payload.CommandErrorCounter == 1,
                  "CS_AppData.HkPacket.Payload.CommandErrorCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1,
                  "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_OneShotCmd_Test_OneShot(void)
{
    CS_OneShotCmd_t CmdPacket;
    int32           strCmpResult;
    char            ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    memset(&CmdPacket, 0, sizeof(CmdPacket));
    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "OneShot checksum failed: child task in use");

    CS_AppData.HkPacket.Payload.RecomputeInProgress = false;

    CS_AppData.HkPacket.Payload.OneShotInProgress = true;

    /* Execute the function being tested */
    CS_OneShotCmd(&CmdPacket);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_ONESHOT_CHDTASK_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.Payload.CommandErrorCounter == 1,
                  "CS_AppData.HkPacket.Payload.CommandErrorCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1,
                  "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_CancelOneShotCmd_Test_Nominal(void)
{
    CS_CancelOneShotCmd_t CmdPacket;
    int32                 strCmpResult;
    char                  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    memset(&CmdPacket, 0, sizeof(CmdPacket));
    snprintf(ExpectedEventString,
             CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "OneShot checksum calculation has been cancelled");

    CS_AppData.HkPacket.Payload.RecomputeInProgress = false;
    CS_AppData.HkPacket.Payload.OneShotInProgress   = true;

    /* Execute the function being tested */
    CS_CancelOneShotCmd(&CmdPacket);

    /* Verify results */
    UtAssert_BOOL_FALSE(CFE_RESOURCEID_TEST_DEFINED(CS_AppData.ChildTaskID));
    UtAssert_True(CS_AppData.HkPacket.Payload.RecomputeInProgress == false,
                  "CS_AppData.HkPacket.Payload.RecomputeInProgress == false");
    UtAssert_True(CS_AppData.HkPacket.Payload.OneShotInProgress == false,
                  "CS_AppData.HkPacket.Payload.OneShotInProgress == false");
    UtAssert_True(CS_AppData.HkPacket.Payload.CommandCounter == 1, "CS_AppData.HkPacket.Payload.CommandCounter == 1");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_ONESHOT_CANCELLED_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1,
                  "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_CancelOneShotCmd_Test_DeleteChildTaskError(void)
{
    CS_CancelOneShotCmd_t CmdPacket;
    int32                 strCmpResult;
    char                  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    memset(&CmdPacket, 0, sizeof(CmdPacket));
    snprintf(ExpectedEventString,
             CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Cancel OneShot checksum failed, CFE_ES_DeleteChildTask returned:  0x%%08X");

    CS_AppData.HkPacket.Payload.RecomputeInProgress = false;
    CS_AppData.HkPacket.Payload.OneShotInProgress   = true;

    /* Set to generate error message CS_ONESHOT_CANCEL_DELETE_CHDTASK_ERR_EID */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_DeleteChildTask), 1, -1);

    /* Execute the function being tested */
    CS_CancelOneShotCmd(&CmdPacket);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_ONESHOT_CANCEL_DELETE_CHDTASK_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.Payload.CommandErrorCounter == 1,
                  "CS_AppData.HkPacket.Payload.CommandErrorCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1,
                  "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_CancelOneShotCmd_Test_NoChildTaskError(void)
{
    CS_CancelOneShotCmd_t CmdPacket;
    int32                 strCmpResult;
    char                  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    memset(&CmdPacket, 0, sizeof(CmdPacket));
    snprintf(ExpectedEventString,
             CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Cancel OneShot checksum failed. No OneShot active");

    CS_AppData.HkPacket.Payload.RecomputeInProgress = true;
    CS_AppData.HkPacket.Payload.OneShotInProgress   = false;

    /* Set to generate error message CS_ONESHOT_CANCEL_NO_CHDTASK_ERR_EID */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_DeleteChildTask), 1, -1);

    /* Execute the function being tested */
    CS_CancelOneShotCmd(&CmdPacket);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_ONESHOT_CANCEL_NO_CHDTASK_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.Payload.CommandErrorCounter == 1,
                  "CS_AppData.HkPacket.Payload.CommandErrorCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1,
                  "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_CancelOneShotCmd_Test_OneShot(void)
{
    CS_CancelOneShotCmd_t CmdPacket;
    int32                 strCmpResult;
    char                  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    memset(&CmdPacket, 0, sizeof(CmdPacket));
    snprintf(ExpectedEventString,
             CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Cancel OneShot checksum failed. No OneShot active");

    CS_AppData.HkPacket.Payload.RecomputeInProgress = false;
    CS_AppData.HkPacket.Payload.OneShotInProgress   = false;

    /* Set to generate error message CS_ONESHOT_CANCEL_NO_CHDTASK_ERR_EID */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_DeleteChildTask), 1, -1);

    /* Execute the function being tested */
    CS_CancelOneShotCmd(&CmdPacket);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_ONESHOT_CANCEL_NO_CHDTASK_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.Payload.CommandErrorCounter == 1,
                  "CS_AppData.HkPacket.Payload.CommandErrorCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1,
                  "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void UtTest_Setup(void)
{
    UtTest_Add(CS_SendHkCmd_Test_Nominal, CS_Test_Setup, CS_Test_TearDown, "CS_SendHkCmd_Test_Nominal");

    UtTest_Add(CS_NoopCmd_Test, CS_Test_Setup, CS_Test_TearDown, "CS_NoopCmd_Test");

    UtTest_Add(CS_ResetCountersCmd_Test, CS_Test_Setup, CS_Test_TearDown, "CS_ResetCountersCmd_Test");

    UtTest_Add(CS_BackgroundCheckCycleCmd_Test_BackgroundInProgress,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_BackgroundCheckCycleCmd_Test_BackgroundInProgress");
    UtTest_Add(CS_BackgroundCheckCycleCmd_Test_BackgroundCfeCore,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_BackgroundCheckCycleCmd_Test_BackgroundCfeCore");
    UtTest_Add(CS_BackgroundCheckCycleCmd_Test_BackgroundOS,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_BackgroundCheckCycleCmd_Test_BackgroundOS");
    UtTest_Add(CS_BackgroundCheckCycleCmd_Test_BackgroundEeprom,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_BackgroundCheckCycleCmd_Test_BackgroundEeprom");
    UtTest_Add(CS_BackgroundCheckCycleCmd_Test_BackgroundMemory,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_BackgroundCheckCycleCmd_Test_BackgroundMemory");
    UtTest_Add(CS_BackgroundCheckCycleCmd_Test_BackgroundTables,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_BackgroundCheckCycleCmd_Test_BackgroundTables");
    UtTest_Add(CS_BackgroundCheckCycleCmd_Test_BackgroundApp,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_BackgroundCheckCycleCmd_Test_BackgroundApp");
    UtTest_Add(CS_BackgroundCheckCycleCmd_Test_Default,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_BackgroundCheckCycleCmd_Test_Default");
    UtTest_Add(CS_BackgroundCheckCycleCmd_Test_Disabled,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_BackgroundCheckCycleCmd_Test_Disabled");
    UtTest_Add(CS_BackgroundCheckCycleCmd_Test_OneShot,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_BackgroundCheckCycleCmd_Test_OneShot");
    UtTest_Add(CS_BackgroundCheckCycleCmd_Test_EndOfList,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_BackgroundCheckCycleCmd_Test_EndOfList");

    UtTest_Add(CS_DisableAllCSCmd_Test, CS_Test_Setup, CS_Test_TearDown, "CS_DisableAllCSCmd_Test");

    UtTest_Add(CS_EnableAllCSCmd_Test, CS_Test_Setup, CS_Test_TearDown, "CS_EnableAllCSCmd_Test");

    UtTest_Add(CS_DisableCfeCoreCmd_Test, CS_Test_Setup, CS_Test_TearDown, "CS_DisableCfeCoreCmd_Test");

    UtTest_Add(CS_EnableCfeCoreCmd_Test, CS_Test_Setup, CS_Test_TearDown, "CS_EnableCfeCoreCmd_Test");

    UtTest_Add(CS_DisableOSCmd_Test, CS_Test_Setup, CS_Test_TearDown, "CS_DisableOSCmd_Test");

    UtTest_Add(CS_EnableOSCmd_Test, CS_Test_Setup, CS_Test_TearDown, "CS_EnableOSCmd_Test");

    UtTest_Add(CS_ReportBaselineCfeCoreCmd_Test_Nominal,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ReportBaselineCfeCoreCmd_Test_Nominal");
    UtTest_Add(CS_ReportBaselineCfeCoreCmd_Test_NotComputedYet,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ReportBaselineCfeCoreCmd_Test_NotComputedYet");

    UtTest_Add(CS_ReportBaselineOSCmd_Test_Nominal,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ReportBaselineOSCmd_Test_Nominal");
    UtTest_Add(CS_ReportBaselineOSCmd_Test_NotComputedYet,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ReportBaselineOSCmd_Test_NotComputedYet");

    UtTest_Add(CS_RecomputeBaselineCfeCoreCmd_Test_Nominal,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_RecomputeBaselineCfeCoreCmd_Test_Nominal");
    UtTest_Add(CS_RecomputeBaselineCfeCoreCmd_Test_CreateChildTaskError,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_RecomputeBaselineCfeCoreCmd_Test_CreateChildTaskError");
    UtTest_Add(CS_RecomputeBaselineCfeCoreCmd_Test_ChildTaskError,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_RecomputeBaselineCfeCoreCmd_Test_ChildTaskError");
    UtTest_Add(CS_RecomputeBaselineCfeCoreCmd_Test_OneShot,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_RecomputeBaselineCfeCoreCmd_Test_OneShot");

    UtTest_Add(CS_RecomputeBaselineOSCmd_Test_Nominal,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_RecomputeBaselineOSCmd_Test_Nominal");
    UtTest_Add(CS_RecomputeBaselineOSCmd_Test_CreateChildTaskError,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_RecomputeBaselineOSCmd_Test_CreateChildTaskError");
    UtTest_Add(CS_RecomputeBaselineOSCmd_Test_ChildTaskError,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_RecomputeBaselineOSCmd_Test_ChildTaskError");
    UtTest_Add(CS_RecomputeBaselineOSCmd_Test_OneShot,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_RecomputeBaselineOSCmd_Test_OneShot");

    UtTest_Add(CS_OneShotCmd_Test_Nominal, CS_Test_Setup, CS_Test_TearDown, "CS_OneShotCmd_Test_Nominal");
    UtTest_Add(CS_OneShotCmd_Test_MaxBytesPerCycleNonZero,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_OneShotCmd_Test_MaxBytesPerCycleNonZero");
    UtTest_Add(CS_OneShotCmd_Test_CreateChildTaskError,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_OneShotCmd_Test_CreateChildTaskError");
    UtTest_Add(CS_OneShotCmd_Test_ChildTaskError, CS_Test_Setup, CS_Test_TearDown, "CS_OneShotCmd_Test_ChildTaskError");
    UtTest_Add(CS_OneShotCmd_Test_MemValidateRangeError,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_OneShotCmd_Test_MemValidateRangeError");
    UtTest_Add(CS_OneShotCmd_Test_OneShot, CS_Test_Setup, CS_Test_TearDown, "CS_OneShotCmd_Test_OneShot");

    UtTest_Add(CS_CancelOneShotCmd_Test_Nominal, CS_Test_Setup, CS_Test_TearDown, "CS_CancelOneShotCmd_Test_Nominal");
    UtTest_Add(CS_CancelOneShotCmd_Test_DeleteChildTaskError,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_CancelOneShotCmd_Test_DeleteChildTaskError");
    UtTest_Add(CS_CancelOneShotCmd_Test_NoChildTaskError,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_CancelOneShotCmd_Test_NoChildTaskError");
    UtTest_Add(CS_CancelOneShotCmd_Test_OneShot, CS_Test_Setup, CS_Test_TearDown, "CS_CancelOneShotCmd_Test_OneShot");
}
