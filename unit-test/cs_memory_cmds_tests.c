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

#include "cs_memory_cmds.h"
#include "cs_msg.h"
#include "cs_msgdefs.h"
#include "cs_events.h"
#include "cs_version.h"
#include "cs_utils.h"
#include "cs_test_utils.h"
#include <unistd.h>
#include <stdlib.h>

/* UT includes */
#include "uttest.h"
#include "utassert.h"
#include "utstubs.h"

/* cs_memory_cmds_tests globals */
uint8 call_count_CFE_EVS_SendEvent;

/*
 * Function Definitions
 */

void CS_DisableMemoryCmd_Test(void)
{
    CS_NoArgsCmd_t CmdPacket;
    int32          strCmpResult;
    char           ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Checksumming of Memory is Disabled");

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);

    /* Execute the function being tested */
    CS_DisableMemoryCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.Payload.MemoryCSState == CS_STATE_DISABLED,
                  "CS_AppData.HkPacket.Payload.MemoryCSState = CS_STATE_DISABLED");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_DISABLE_MEMORY_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.Payload.CmdCounter == 1, "CS_AppData.HkPacket.Payload.CmdCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_DisableMemoryCmd_Test_OneShot(void)
{
    CS_NoArgsCmd_t CmdPacket;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);
    UT_SetDeferredRetcode(UT_KEY(CS_CheckRecomputeOneshot), 1, true);

    /* Execute the function being tested */
    CS_DisableMemoryCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.Payload.CmdCounter == 0, "CS_AppData.HkPacket.Payload.CmdCounter == 0");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void CS_EnableMemoryCmd_Test(void)
{
    CS_NoArgsCmd_t CmdPacket;
    int32          strCmpResult;
    char           ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Checksumming of Memory is Enabled");

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);

    /* Execute the function being tested */
    CS_EnableMemoryCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.Payload.MemoryCSState == CS_STATE_ENABLED,
                  "CS_AppData.HkPacket.Payload.MemoryCSState = CS_STATE_ENABLED");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_ENABLE_MEMORY_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.Payload.CmdCounter == 1, "CS_AppData.HkPacket.Payload.CmdCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_EnableMemoryCmd_Test_OneShot(void)
{
    CS_NoArgsCmd_t CmdPacket;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);
    UT_SetDeferredRetcode(UT_KEY(CS_CheckRecomputeOneshot), 1, true);

    /* Execute the function being tested */
    CS_EnableMemoryCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.Payload.CmdCounter == 0, "CS_AppData.HkPacket.Payload.CmdCounter == 0");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void CS_ReportBaselineEntryIDMemoryCmd_Test_Computed(void)
{
    CS_EntryCmd_t CmdPacket;
    int32         strCmpResult;
    char          ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Report baseline of Memory Entry %%d is 0x%%08X");

    CmdPacket.Payload.EntryID = 1;

    CS_AppData.ResMemoryTblPtr[CmdPacket.Payload.EntryID].State           = 99;
    CS_AppData.ResMemoryTblPtr[CmdPacket.Payload.EntryID].ComputedYet     = true;
    CS_AppData.ResMemoryTblPtr[CmdPacket.Payload.EntryID].ComparisonValue = 1;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);

    /* Execute the function being tested */
    CS_ReportBaselineEntryIDMemoryCmd(&CmdPacket);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_BASELINE_MEMORY_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.Payload.CmdCounter == 1, "CS_AppData.HkPacket.Payload.CmdCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_ReportBaselineEntryIDMemoryCmd_Test_NotYetComputed(void)
{
    CS_EntryCmd_t CmdPacket;
    int32         strCmpResult;
    char          ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Report baseline of Memory Entry %%d has not been computed yet");

    CmdPacket.Payload.EntryID = 1;

    CS_AppData.ResMemoryTblPtr[CmdPacket.Payload.EntryID].State           = 99;
    CS_AppData.ResMemoryTblPtr[CmdPacket.Payload.EntryID].ComputedYet     = false;
    CS_AppData.ResMemoryTblPtr[CmdPacket.Payload.EntryID].ComparisonValue = 1;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);

    /* Execute the function being tested */
    CS_ReportBaselineEntryIDMemoryCmd(&CmdPacket);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_NO_BASELINE_MEMORY_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.Payload.CmdCounter == 1, "CS_AppData.HkPacket.Payload.CmdCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_ReportBaselineEntryIDMemoryCmd_Test_InvalidEntryErrorEntryIDTooHigh(void)
{
    CS_EntryCmd_t CmdPacket;
    int32         strCmpResult;
    char          ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Memory report baseline failed, Entry ID invalid: %%d, State: %%d Max ID: %%d");

    CmdPacket.Payload.EntryID = CS_MAX_NUM_MEMORY_TABLE_ENTRIES;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);

    /* Execute the function being tested */
    CS_ReportBaselineEntryIDMemoryCmd(&CmdPacket);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_BASELINE_INVALID_ENTRY_MEMORY_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.Payload.CmdErrCounter == 1, "CS_AppData.HkPacket.Payload.CmdErrCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_ReportBaselineEntryIDMemoryCmd_Test_InvalidEntryErrorStateEmpty(void)
{
    CS_EntryCmd_t CmdPacket;
    int32         strCmpResult;
    char          ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Memory report baseline failed, Entry ID invalid: %%d, State: %%d Max ID: %%d");

    CmdPacket.Payload.EntryID = 1;

    CS_AppData.ResMemoryTblPtr[CmdPacket.Payload.EntryID].State = CS_STATE_EMPTY;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);

    /* Execute the function being tested */
    CS_ReportBaselineEntryIDMemoryCmd(&CmdPacket);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_BASELINE_INVALID_ENTRY_MEMORY_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.Payload.CmdErrCounter == 1, "CS_AppData.HkPacket.Payload.CmdErrCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_RecomputeBaselineMemoryCmd_Test_Nominal(void)
{
    CS_EntryCmd_t CmdPacket;
    int32         strCmpResult;
    char          ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Recompute baseline of Memory Entry ID %%d started");

    CmdPacket.Payload.EntryID = 1;

    CS_AppData.ResMemoryTblPtr[CmdPacket.Payload.EntryID].State = 99;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);

    /* Execute the function being tested */
    CS_RecomputeBaselineMemoryCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.Payload.RecomputeInProgress == true, "CS_AppData.HkPacket.Payload.RecomputeInProgress == true");
    UtAssert_True(CS_AppData.HkPacket.Payload.OneShotInProgress == false, "CS_AppData.HkPacket.Payload.OneShotInProgress == false");

    UtAssert_True(CS_AppData.ChildTaskTable == CS_MEMORY_TABLE, "CS_AppData.ChildTaskTable == CS_MEMORY_TABLE");
    UtAssert_True(CS_AppData.ChildTaskEntryID == CmdPacket.Payload.EntryID, "CS_AppData.ChildTaskEntryID == CmdPacket.Payload.EntryID");
    UtAssert_True(CS_AppData.RecomputeEepromMemoryEntryPtr == &CS_AppData.ResMemoryTblPtr[CmdPacket.Payload.EntryID],
                  "CS_AppData.RecomputeEepromMemoryEntryPtr == &CS_AppData.ResMemoryTblPtr[CmdPacket.Payload.EntryID]");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_RECOMPUTE_MEMORY_STARTED_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.Payload.CmdCounter == 1, "CS_AppData.HkPacket.Payload.CmdCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_RecomputeBaselineMemoryCmd_Test_CreateChildTaskError(void)
{
    CS_EntryCmd_t CmdPacket;
    int32         strCmpResult;
    char          ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Recompute baseline of Memory Entry ID %%d failed, ES_CreateChildTask returned:  0x%%08X");

    CmdPacket.Payload.EntryID = 1;

    CS_AppData.ResMemoryTblPtr[CmdPacket.Payload.EntryID].State = 99;

    /* Set to generate error message CS_RECOMPUTE_MEMORY_CREATE_CHDTASK_ERR_EID */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_CreateChildTask), 1, -1);

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);

    /* Execute the function being tested */
    CS_RecomputeBaselineMemoryCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.Payload.OneShotInProgress == false, "CS_AppData.HkPacket.Payload.OneShotInProgress == false");

    UtAssert_True(CS_AppData.ChildTaskTable == CS_MEMORY_TABLE, "CS_AppData.ChildTaskTable == CS_MEMORY_TABLE");
    UtAssert_True(CS_AppData.ChildTaskEntryID == CmdPacket.Payload.EntryID, "CS_AppData.ChildTaskEntryID == CmdPacket.Payload.EntryID");
    UtAssert_True(CS_AppData.RecomputeEepromMemoryEntryPtr == &CS_AppData.ResMemoryTblPtr[CmdPacket.Payload.EntryID],
                  "CS_AppData.RecomputeEepromMemoryEntryPtr == &CS_AppData.ResMemoryTblPtr[CmdPacket.Payload.EntryID]");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_RECOMPUTE_MEMORY_CREATE_CHDTASK_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.Payload.CmdErrCounter == 1, "CS_AppData.HkPacket.Payload.CmdErrCounter == 1");
    UtAssert_True(CS_AppData.HkPacket.Payload.RecomputeInProgress == false, "CS_AppData.HkPacket.Payload.RecomputeInProgress == false");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_RecomputeBaselineMemoryCmd_Test_InvalidEntryErrorEntryIDTooHigh(void)
{
    CS_EntryCmd_t CmdPacket;
    int32         strCmpResult;
    char          ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Memory recompute baseline of entry failed, Entry ID invalid: %%d, State: %%d, Max ID: %%d");

    CmdPacket.Payload.EntryID = CS_MAX_NUM_MEMORY_TABLE_ENTRIES;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);

    /* Execute the function being tested */
    CS_RecomputeBaselineMemoryCmd(&CmdPacket);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_RECOMPUTE_INVALID_ENTRY_MEMORY_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.Payload.CmdErrCounter == 1, "CS_AppData.HkPacket.Payload.CmdErrCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_RecomputeBaselineMemoryCmd_Test_InvalidEntryErrorStateEmpty(void)
{
    CS_EntryCmd_t CmdPacket;
    int32         strCmpResult;
    char          ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Memory recompute baseline of entry failed, Entry ID invalid: %%d, State: %%d, Max ID: %%d");

    CmdPacket.Payload.EntryID = 1;

    CS_AppData.ResMemoryTblPtr[CmdPacket.Payload.EntryID].State = CS_STATE_EMPTY;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);

    /* Execute the function being tested */
    CS_RecomputeBaselineMemoryCmd(&CmdPacket);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_RECOMPUTE_INVALID_ENTRY_MEMORY_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.Payload.CmdErrCounter == 1, "CS_AppData.HkPacket.Payload.CmdErrCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_RecomputeBaselineMemoryCmd_Test_RecomputeInProgress(void)
{
    CS_EntryCmd_t CmdPacket;
    int32         strCmpResult;
    char          ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Recompute baseline of Memory Entry ID %%d failed: child task in use");

    CmdPacket.Payload.EntryID = 1;

    CS_AppData.HkPacket.Payload.RecomputeInProgress = true;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);

    /* Execute the function being tested */
    CS_RecomputeBaselineMemoryCmd(&CmdPacket);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_RECOMPUTE_MEMORY_CHDTASK_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.Payload.CmdErrCounter == 1, "CS_AppData.HkPacket.Payload.CmdErrCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_RecomputeBaselineMemoryCmd_Test_OneShot(void)
{
    CS_EntryCmd_t CmdPacket;
    int32         strCmpResult;
    char          ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Recompute baseline of Memory Entry ID %%d failed: child task in use");

    CmdPacket.Payload.EntryID = 1;

    CS_AppData.HkPacket.Payload.RecomputeInProgress = false;
    CS_AppData.HkPacket.Payload.OneShotInProgress   = true;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);

    /* Execute the function being tested */
    CS_RecomputeBaselineMemoryCmd(&CmdPacket);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_RECOMPUTE_MEMORY_CHDTASK_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.Payload.CmdErrCounter == 1, "CS_AppData.HkPacket.Payload.CmdErrCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_EnableEntryIDMemoryCmd_Test_Nominal(void)
{
    CS_EntryCmd_t CmdPacket;
    int32         strCmpResult;
    char          ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Checksumming of Memory Entry ID %%d is Enabled");

    CmdPacket.Payload.EntryID = 1;

    CS_AppData.ResMemoryTblPtr[CmdPacket.Payload.EntryID].State = 99;
    CS_AppData.DefMemoryTblPtr[CmdPacket.Payload.EntryID].State = 99;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);

    /* Execute the function being tested */
    CS_EnableEntryIDMemoryCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.ResMemoryTblPtr[CmdPacket.Payload.EntryID].State == CS_STATE_ENABLED,
                  "CS_AppData.ResMemoryTblPtr[CmdPacket.Payload.EntryID].State == CS_STATE_ENABLED");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_ENABLE_MEMORY_ENTRY_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.DefMemoryTblPtr[CmdPacket.Payload.EntryID].State == CS_STATE_ENABLED,
                  "CS_AppData.DefMemoryTblPtr[CmdPacket.Payload.EntryID].State == CS_STATE_ENABLED");

    UtAssert_True(CS_AppData.HkPacket.Payload.CmdCounter == 1, "CS_AppData.HkPacket.Payload.CmdCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_EnableEntryIDMemoryCmd_Test_DefMemoryTblPtrStateEmpty(void)
{
    CS_EntryCmd_t CmdPacket;
    int32         strCmpResult;
    char          ExpectedEventString[2][CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString[0], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Checksumming of Memory Entry ID %%d is Enabled");

    snprintf(ExpectedEventString[1], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS unable to update memory definition table for entry %%d, State: %%d");

    CmdPacket.Payload.EntryID = 1;

    CS_AppData.ResMemoryTblPtr[CmdPacket.Payload.EntryID].State = 99;
    CS_AppData.DefMemoryTblPtr[CmdPacket.Payload.EntryID].State = CS_STATE_EMPTY;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);

    /* Execute the function being tested */
    CS_EnableEntryIDMemoryCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.ResMemoryTblPtr[CmdPacket.Payload.EntryID].State == CS_STATE_ENABLED,
                  "CS_AppData.ResMemoryTblPtr[CmdPacket.Payload.EntryID].State == CS_STATE_ENABLED");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_ENABLE_MEMORY_ENTRY_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult =
        strncmp(ExpectedEventString[0], context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, CS_ENABLE_MEMORY_DEF_EMPTY_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult =
        strncmp(ExpectedEventString[1], context_CFE_EVS_SendEvent[1].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[1].Spec);

    UtAssert_True(CS_AppData.HkPacket.Payload.CmdCounter == 1, "CS_AppData.HkPacket.Payload.CmdCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 2, "CFE_EVS_SendEvent was called %u time(s), expected 2",
                  call_count_CFE_EVS_SendEvent);
}

void CS_EnableEntryIDMemoryCmd_Test_InvalidEntryErrorEntryIDTooHigh(void)
{
    CS_EntryCmd_t CmdPacket;
    int32         strCmpResult;
    char          ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Enable Memory entry failed, invalid Entry ID:  %%d, State: %%d, Max ID: %%d");

    CmdPacket.Payload.EntryID = CS_MAX_NUM_MEMORY_TABLE_ENTRIES;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);

    /* Execute the function being tested */
    CS_EnableEntryIDMemoryCmd(&CmdPacket);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_ENABLE_MEMORY_INVALID_ENTRY_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.Payload.CmdErrCounter == 1, "CS_AppData.HkPacket.Payload.CmdErrCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_EnableEntryIDMemoryCmd_Test_InvalidEntryErrorStateEmpty(void)
{
    CS_EntryCmd_t CmdPacket;
    int32         strCmpResult;
    char          ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Enable Memory entry failed, invalid Entry ID:  %%d, State: %%d, Max ID: %%d");

    CmdPacket.Payload.EntryID = 1;

    CS_AppData.ResMemoryTblPtr[CmdPacket.Payload.EntryID].State = CS_STATE_EMPTY;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);

    /* Execute the function being tested */
    CS_EnableEntryIDMemoryCmd(&CmdPacket);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_ENABLE_MEMORY_INVALID_ENTRY_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.Payload.CmdErrCounter == 1, "CS_AppData.HkPacket.Payload.CmdErrCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_EnableEntryIDMemoryCmd_Test_OneShot(void)
{
    CS_EntryCmd_t CmdPacket;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);
    UT_SetDeferredRetcode(UT_KEY(CS_CheckRecomputeOneshot), 1, true);

    /* Execute the function being tested */
    CS_EnableEntryIDMemoryCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.Payload.CmdCounter == 0, "CS_AppData.HkPacket.Payload.CmdCounter == 0");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void CS_DisableEntryIDMemoryCmd_Test_Nominal(void)
{
    CS_EntryCmd_t CmdPacket;
    int32         strCmpResult;
    char          ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Checksumming of Memory Entry ID %%d is Disabled");

    CmdPacket.Payload.EntryID = 1;

    CS_AppData.ResMemoryTblPtr[CmdPacket.Payload.EntryID].State = 99;
    CS_AppData.DefMemoryTblPtr[CmdPacket.Payload.EntryID].State = 99;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);

    /* Execute the function being tested */
    CS_DisableEntryIDMemoryCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.ResMemoryTblPtr[CmdPacket.Payload.EntryID].State == CS_STATE_DISABLED,
                  "CS_AppData.ResMemoryTblPtr[CmdPacket.Payload.EntryID].State == CS_STATE_DISABLED");
    UtAssert_True(CS_AppData.ResMemoryTblPtr[CmdPacket.Payload.EntryID].TempChecksumValue == 0,
                  "CS_AppData.ResMemoryTblPtr[CmdPacket.Payload.EntryID].TempChecksumValue == 0");
    UtAssert_True(CS_AppData.ResMemoryTblPtr[CmdPacket.Payload.EntryID].ByteOffset == 0,
                  "CS_AppData.ResMemoryTblPtr[CmdPacket.Payload.EntryID].ByteOffset == 0");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_DISABLE_MEMORY_ENTRY_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.DefMemoryTblPtr[CmdPacket.Payload.EntryID].State == CS_STATE_DISABLED,
                  "CS_AppData.DefMemoryTblPtr[CmdPacket.Payload.EntryID].State == CS_STATE_DISABLED");

    UtAssert_True(CS_AppData.HkPacket.Payload.CmdCounter == 1, "CS_AppData.HkPacket.Payload.CmdCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_DisableEntryIDMemoryCmd_Test_DefMemoryTblPtrStateEmpty(void)
{
    CS_EntryCmd_t CmdPacket;
    int32         strCmpResult;
    char          ExpectedEventString[2][CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString[0], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Checksumming of Memory Entry ID %%d is Disabled");

    snprintf(ExpectedEventString[1], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS unable to update memory definition table for entry %%d, State: %%d");

    CmdPacket.Payload.EntryID = 1;

    CS_AppData.ResMemoryTblPtr[CmdPacket.Payload.EntryID].State = 99;
    CS_AppData.DefMemoryTblPtr[CmdPacket.Payload.EntryID].State = CS_STATE_EMPTY;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);

    /* Execute the function being tested */
    CS_DisableEntryIDMemoryCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.ResMemoryTblPtr[CmdPacket.Payload.EntryID].State == CS_STATE_DISABLED,
                  "CS_AppData.ResMemoryTblPtr[CmdPacket.Payload.EntryID].State == CS_STATE_DISABLED");
    UtAssert_True(CS_AppData.ResMemoryTblPtr[CmdPacket.Payload.EntryID].TempChecksumValue == 0,
                  "CS_AppData.ResMemoryTblPtr[CmdPacket.Payload.EntryID].TempChecksumValue == 0");
    UtAssert_True(CS_AppData.ResMemoryTblPtr[CmdPacket.Payload.EntryID].ByteOffset == 0,
                  "CS_AppData.ResMemoryTblPtr[CmdPacket.Payload.EntryID].ByteOffset == 0");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_DISABLE_MEMORY_ENTRY_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult =
        strncmp(ExpectedEventString[0], context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, CS_DISABLE_MEMORY_DEF_EMPTY_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult =
        strncmp(ExpectedEventString[1], context_CFE_EVS_SendEvent[1].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[1].Spec);

    UtAssert_True(CS_AppData.HkPacket.Payload.CmdCounter == 1, "CS_AppData.HkPacket.Payload.CmdCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 2, "CFE_EVS_SendEvent was called %u time(s), expected 2",
                  call_count_CFE_EVS_SendEvent);
}

void CS_DisableEntryIDMemoryCmd_Test_InvalidEntryErrorEntryIDTooHigh(void)
{
    CS_EntryCmd_t CmdPacket;
    int32         strCmpResult;
    char          ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Disable Memory entry failed, invalid Entry ID:  %%d, State: %%d, Max ID: %%d");

    CmdPacket.Payload.EntryID = CS_MAX_NUM_MEMORY_TABLE_ENTRIES;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);

    /* Execute the function being tested */
    CS_DisableEntryIDMemoryCmd(&CmdPacket);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_DISABLE_MEMORY_INVALID_ENTRY_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.Payload.CmdErrCounter == 1, "CS_AppData.HkPacket.Payload.CmdErrCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_DisableEntryIDMemoryCmd_Test_InvalidEntryErrorStateEmpty(void)
{
    CS_EntryCmd_t CmdPacket;
    int32         strCmpResult;
    char          ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Disable Memory entry failed, invalid Entry ID:  %%d, State: %%d, Max ID: %%d");

    CmdPacket.Payload.EntryID = 1;

    CS_AppData.ResMemoryTblPtr[CmdPacket.Payload.EntryID].State = CS_STATE_EMPTY;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);

    /* Execute the function being tested */
    CS_DisableEntryIDMemoryCmd(&CmdPacket);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_DISABLE_MEMORY_INVALID_ENTRY_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.Payload.CmdErrCounter == 1, "CS_AppData.HkPacket.Payload.CmdErrCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_DisableEntryIDMemoryCmd_Test_OneShot(void)
{
    CS_EntryCmd_t CmdPacket;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);
    UT_SetDeferredRetcode(UT_KEY(CS_CheckRecomputeOneshot), 1, true);

    /* Execute the function being tested */
    CS_DisableEntryIDMemoryCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.Payload.CmdCounter == 0, "CS_AppData.HkPacket.Payload.CmdCounter == 0");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void CS_GetEntryIDMemoryCmd_Test_Nominal(void)
{
    CS_GetEntryIDCmd_t CmdPacket;
    int32              strCmpResult;
    char               ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Memory Found Address 0x%%08X in Entry ID %%d");

    int16 EntryID = 1;

    CS_AppData.ResMemoryTblPtr[EntryID].StartAddress       = 1;
    CmdPacket.Payload.Address                                      = 1;
    CS_AppData.ResMemoryTblPtr[EntryID].NumBytesToChecksum = 0;
    CS_AppData.ResMemoryTblPtr[EntryID].State              = 99;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);

    /* Execute the function being tested */
    CS_GetEntryIDMemoryCmd(&CmdPacket);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_GET_ENTRY_ID_MEMORY_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.Payload.CmdCounter == 1, "CS_AppData.HkPacket.Payload.CmdCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_GetEntryIDMemoryCmd_Test_AddressNotFound(void)
{
    CS_GetEntryIDCmd_t CmdPacket;
    int32              strCmpResult;
    char               ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Address 0x%%08X was not found in Memory table");

    CmdPacket.Payload.Address = 0xFFFFFFFF;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);

    /* Execute the function being tested */
    CS_GetEntryIDMemoryCmd(&CmdPacket);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_GET_ENTRY_ID_MEMORY_NOT_FOUND_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.Payload.CmdCounter == 1, "CS_AppData.HkPacket.Payload.CmdCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_GetEntryIDMemoryCmd_Test_AddressPtr(void)
{
    CS_GetEntryIDCmd_t CmdPacket;
    int32              strCmpResult;
    char               ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Address 0x%%08X was not found in Memory table");

    int16 EntryID = 1;

    CS_AppData.ResMemoryTblPtr[EntryID].StartAddress       = 2;
    CmdPacket.Payload.Address                                      = 1;
    CS_AppData.ResMemoryTblPtr[EntryID].NumBytesToChecksum = 0;
    CS_AppData.ResMemoryTblPtr[EntryID].State              = 99;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);

    /* Execute the function being tested */
    CS_GetEntryIDMemoryCmd(&CmdPacket);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_GET_ENTRY_ID_MEMORY_NOT_FOUND_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.Payload.CmdCounter == 1, "CS_AppData.HkPacket.Payload.CmdCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_GetEntryIDMemoryCmd_Test_State(void)
{
    CS_GetEntryIDCmd_t CmdPacket;
    int32              strCmpResult;
    char               ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Address 0x%%08X was not found in Memory table");

    int16 EntryID = 1;

    CS_AppData.ResMemoryTblPtr[EntryID].StartAddress       = 1;
    CmdPacket.Payload.Address                                      = 1;
    CS_AppData.ResMemoryTblPtr[EntryID].NumBytesToChecksum = 0;
    CS_AppData.ResMemoryTblPtr[EntryID].State              = CS_STATE_EMPTY;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);

    /* Execute the function being tested */
    CS_GetEntryIDMemoryCmd(&CmdPacket);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_GET_ENTRY_ID_MEMORY_NOT_FOUND_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.Payload.CmdCounter == 1, "CS_AppData.HkPacket.Payload.CmdCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void UtTest_Setup(void)
{
    UtTest_Add(CS_DisableMemoryCmd_Test, CS_Test_Setup, CS_Test_TearDown, "CS_DisableMemoryCmd_Test");
    UtTest_Add(CS_DisableMemoryCmd_Test_OneShot, CS_Test_Setup, CS_Test_TearDown, "CS_DisableMemoryCmd_Test_OneShot");

    UtTest_Add(CS_EnableMemoryCmd_Test, CS_Test_Setup, CS_Test_TearDown, "CS_EnableMemoryCmd_Test");
    UtTest_Add(CS_EnableMemoryCmd_Test_OneShot, CS_Test_Setup, CS_Test_TearDown, "CS_EnableMemoryCmd_Test_OneShot");

    UtTest_Add(CS_ReportBaselineEntryIDMemoryCmd_Test_Computed, CS_Test_Setup, CS_Test_TearDown,
               "CS_ReportBaselineEntryIDMemoryCmd_Test_Computed");
    UtTest_Add(CS_ReportBaselineEntryIDMemoryCmd_Test_NotYetComputed, CS_Test_Setup, CS_Test_TearDown,
               "CS_ReportBaselineEntryIDMemoryCmd_Test_NotYetComputed");
    UtTest_Add(CS_ReportBaselineEntryIDMemoryCmd_Test_InvalidEntryErrorEntryIDTooHigh, CS_Test_Setup, CS_Test_TearDown,
               "CS_ReportBaselineEntryIDMemoryCmd_Test_InvalidEntryErrorEntryIDTooHigh");
    UtTest_Add(CS_ReportBaselineEntryIDMemoryCmd_Test_InvalidEntryErrorStateEmpty, CS_Test_Setup, CS_Test_TearDown,
               "CS_ReportBaselineEntryIDMemoryCmd_Test_InvalidEntryErrorStateEmpty");

    UtTest_Add(CS_RecomputeBaselineMemoryCmd_Test_Nominal, CS_Test_Setup, CS_Test_TearDown,
               "CS_RecomputeBaselineMemoryCmd_Test_Nominal");
    UtTest_Add(CS_RecomputeBaselineMemoryCmd_Test_CreateChildTaskError, CS_Test_Setup, CS_Test_TearDown,
               "CS_RecomputeBaselineMemoryCmd_Test_CreateChildTaskError");
    UtTest_Add(CS_RecomputeBaselineMemoryCmd_Test_InvalidEntryErrorEntryIDTooHigh, CS_Test_Setup, CS_Test_TearDown,
               "CS_RecomputeBaselineMemoryCmd_Test_InvalidEntryErrorEntryIDTooHigh");
    UtTest_Add(CS_RecomputeBaselineMemoryCmd_Test_InvalidEntryErrorStateEmpty, CS_Test_Setup, CS_Test_TearDown,
               "CS_RecomputeBaselineMemoryCmd_Test_InvalidEntryErrorStateEmpty");
    UtTest_Add(CS_RecomputeBaselineMemoryCmd_Test_RecomputeInProgress, CS_Test_Setup, CS_Test_TearDown,
               "CS_RecomputeBaselineMemoryCmd_Test_RecomputeInProgress");
    UtTest_Add(CS_RecomputeBaselineMemoryCmd_Test_OneShot, CS_Test_Setup, CS_Test_TearDown,
               "CS_RecomputeBaselineMemoryCmd_Test_OneShot");

    UtTest_Add(CS_EnableEntryIDMemoryCmd_Test_Nominal, CS_Test_Setup, CS_Test_TearDown,
               "CS_EnableEntryIDMemoryCmd_Test_Nominal");
    UtTest_Add(CS_EnableEntryIDMemoryCmd_Test_DefMemoryTblPtrStateEmpty, CS_Test_Setup, CS_Test_TearDown,
               "CS_EnableEntryIDMemoryCmd_Test_DefMemoryTblPtrStateEmpty");
    UtTest_Add(CS_EnableEntryIDMemoryCmd_Test_InvalidEntryErrorEntryIDTooHigh, CS_Test_Setup, CS_Test_TearDown,
               "CS_EnableEntryIDMemoryCmd_Test_InvalidEntryErrorEntryIDTooHigh");
    UtTest_Add(CS_EnableEntryIDMemoryCmd_Test_InvalidEntryErrorStateEmpty, CS_Test_Setup, CS_Test_TearDown,
               "CS_EnableEntryIDMemoryCmd_Test_InvalidEntryErrorStateEmpty");
    UtTest_Add(CS_EnableEntryIDMemoryCmd_Test_OneShot, CS_Test_Setup, CS_Test_TearDown,
               "CS_EnableEntryIDMemoryCmd_Test_OneShots");

    UtTest_Add(CS_DisableEntryIDMemoryCmd_Test_Nominal, CS_Test_Setup, CS_Test_TearDown,
               "CS_DisableEntryIDMemoryCmd_Test_Nominal");
    UtTest_Add(CS_DisableEntryIDMemoryCmd_Test_DefMemoryTblPtrStateEmpty, CS_Test_Setup, CS_Test_TearDown,
               "CS_DisableEntryIDMemoryCmd_Test_DefMemoryTblPtrStateEmpty");
    UtTest_Add(CS_DisableEntryIDMemoryCmd_Test_InvalidEntryErrorEntryIDTooHigh, CS_Test_Setup, CS_Test_TearDown,
               "CS_DisableEntryIDMemoryCmd_Test_InvalidEntryErrorEntryIDTooHigh");
    UtTest_Add(CS_DisableEntryIDMemoryCmd_Test_InvalidEntryErrorStateEmpty, CS_Test_Setup, CS_Test_TearDown,
               "CS_DisableEntryIDMemoryCmd_Test_InvalidEntryErrorStateEmpty");
    UtTest_Add(CS_DisableEntryIDMemoryCmd_Test_OneShot, CS_Test_Setup, CS_Test_TearDown,
               "CS_DisableEntryIDMemoryCmd_Test_OneShot");

    UtTest_Add(CS_GetEntryIDMemoryCmd_Test_Nominal, CS_Test_Setup, CS_Test_TearDown,
               "CS_GetEntryIDMemoryCmd_Test_Nominal");
    UtTest_Add(CS_GetEntryIDMemoryCmd_Test_AddressNotFound, CS_Test_Setup, CS_Test_TearDown,
               "CS_GetEntryIDMemoryCmd_Test_AddressNotFound");
    UtTest_Add(CS_GetEntryIDMemoryCmd_Test_AddressPtr, CS_Test_Setup, CS_Test_TearDown,
               "CS_GetEntryIDMemoryCmd_Test_AddressPtr");
    UtTest_Add(CS_GetEntryIDMemoryCmd_Test_State, CS_Test_Setup, CS_Test_TearDown, "CS_GetEntryIDMemoryCmd_Test_State");
}
