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

#include "cs_eeprom_cmds.h"
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

/* cs_eeprom_cmds_tests globals */
uint8 call_count_CFE_EVS_SendEvent;

/*
 * Function Definitions
 */

void CS_DisableEepromCmd_Test(void)
{
    CS_NoArgsCmd_t CmdPacket;
    int32          strCmpResult;
    char           ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Checksumming of Eeprom is Disabled");

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);

    /* Execute the function being tested */
    CS_DisableEepromCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.EepromCSState == CS_STATE_DISABLED,
                  "CS_AppData.HkPacket.EepromCSState == CS_STATE_DISABLED");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_DISABLE_EEPROM_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.CmdCounter == 1, "CS_AppData.HkPacket.CmdCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_DisableEepromCmd_Test */

void CS_DisableEepromCmd_Test_VerifyError(void)
{
    CS_NoArgsCmd_t CmdPacket;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, false);

    /* Execute the function being tested */
    CS_DisableEepromCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.CmdCounter == 0, "CS_AppData.HkPacket.CmdCounter == 0");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_DisableEepromCmd_Test_VerifyError */

void CS_DisableEepromCmd_Test_OneShot(void)
{
    CS_NoArgsCmd_t CmdPacket;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);
    UT_SetDeferredRetcode(UT_KEY(CS_CheckRecomputeOneshot), 1, true);

    /* Execute the function being tested */
    CS_DisableEepromCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.CmdCounter == 0, "CS_AppData.HkPacket.CmdCounter == 0");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_DisableEepromCmd_Test_OneShot */

void CS_EnableEepromCmd_Test(void)
{
    CS_NoArgsCmd_t CmdPacket;
    int32          strCmpResult;
    char           ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Checksumming of Eeprom is Enabled");

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);

    /* Execute the function being tested */
    CS_EnableEepromCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.EepromCSState == CS_STATE_ENABLED,
                  "CS_AppData.HkPacket.EepromCSState == CS_STATE_ENABLED");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_ENABLE_EEPROM_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.CmdCounter == 1, "CS_AppData.HkPacket.CmdCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_EnableEepromCmd_Test */

void CS_EnableEepromCmd_Test_VerifyError(void)
{
    CS_NoArgsCmd_t CmdPacket;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, false);

    /* Execute the function being tested */
    CS_EnableEepromCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.CmdCounter == 0, "CS_AppData.HkPacket.CmdCounter == 0");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_EnableEepromCmd_Test_VerifyError */

void CS_EnableEepromCmd_Test_OneShot(void)
{
    CS_NoArgsCmd_t CmdPacket;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);
    UT_SetDeferredRetcode(UT_KEY(CS_CheckRecomputeOneshot), 1, true);

    /* Execute the function being tested */
    CS_EnableEepromCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.CmdCounter == 0, "CS_AppData.HkPacket.CmdCounter == 0");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_EnableEepromCmd_Test_OneShot */

void CS_ReportBaselineEntryIDEepromCmd_Test_Computed(void)
{
    CS_EntryCmd_t CmdPacket;
    int32         strCmpResult;
    char          ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Report baseline of Eeprom Entry %%d is 0x%%08X");

    CmdPacket.EntryID = 1;

    CS_AppData.ResEepromTblPtr[CmdPacket.EntryID].State           = 99;
    CS_AppData.ResEepromTblPtr[CmdPacket.EntryID].ComputedYet     = true;
    CS_AppData.ResEepromTblPtr[CmdPacket.EntryID].ComparisonValue = 1;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);

    /* Execute the function being tested */
    CS_ReportBaselineEntryIDEepromCmd(&CmdPacket);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_BASELINE_EEPROM_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.CmdCounter == 1, "CS_AppData.HkPacket.CmdCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_ReportBaselineEntryIDEepromCmd_Test_Computed */

void CS_ReportBaselineEntryIDEepromCmd_Test_NotYetComputed(void)
{
    CS_EntryCmd_t CmdPacket;
    int32         strCmpResult;
    char          ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Report baseline of Eeprom Entry %%d has not been computed yet");

    CmdPacket.EntryID = 1;

    CS_AppData.ResEepromTblPtr[CmdPacket.EntryID].State           = 99;
    CS_AppData.ResEepromTblPtr[CmdPacket.EntryID].ComputedYet     = false;
    CS_AppData.ResEepromTblPtr[CmdPacket.EntryID].ComparisonValue = 1;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);

    /* Execute the function being tested */
    CS_ReportBaselineEntryIDEepromCmd(&CmdPacket);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_NO_BASELINE_EEPROM_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.CmdCounter == 1, "CS_AppData.HkPacket.CmdCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_ReportBaselineEntryIDEepromCmd_Test_NotYetComputed */

void CS_ReportBaselineEntryIDEepromCmd_Test_InvalidEntryErrorEntryIDTooHigh(void)
{
    CS_EntryCmd_t CmdPacket;
    int32         strCmpResult;
    char          ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Eeprom report baseline failed, Entry ID invalid: %%d, State: %%d Max ID: %%d");

    CmdPacket.EntryID = CS_MAX_NUM_EEPROM_TABLE_ENTRIES;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);

    /* Execute the function being tested */
    CS_ReportBaselineEntryIDEepromCmd(&CmdPacket);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_BASELINE_INVALID_ENTRY_EEPROM_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.CmdErrCounter == 1, "CS_AppData.HkPacket.CmdErrCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_ReportBaselineEntryIDEepromCmd_Test_InvalidEntryErrorEntryIDTooHigh */

void CS_ReportBaselineEntryIDEepromCmd_Test_InvalidEntryErrorStateEmpty(void)
{
    CS_EntryCmd_t CmdPacket;
    int32         strCmpResult;
    char          ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Eeprom report baseline failed, Entry ID invalid: %%d, State: %%d Max ID: %%d");

    CmdPacket.EntryID = 1;

    CS_AppData.ResEepromTblPtr[CmdPacket.EntryID].State = CS_STATE_EMPTY;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);

    /* Execute the function being tested */
    CS_ReportBaselineEntryIDEepromCmd(&CmdPacket);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_BASELINE_INVALID_ENTRY_EEPROM_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.CmdErrCounter == 1, "CS_AppData.HkPacket.CmdErrCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_ReportBaselineEntryIDEepromCmd_Test_InvalidEntryErrorStateEmpty */

void CS_ReportBaselineEntryIDEepromCmd_Test_VerifyError(void)
{
    CS_EntryCmd_t CmdPacket;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, false);

    /* Execute the function being tested */
    CS_ReportBaselineEntryIDEepromCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.CmdCounter == 0, "CS_AppData.HkPacket.CmdCounter == 0");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_ReportBaselineEntryIDEepromCmd_Test_VerifyError */

void CS_RecomputeBaselineEepromCmd_Test_Nominal(void)
{
    CS_EntryCmd_t CmdPacket;
    int32         strCmpResult;
    char          ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Recompute baseline of Eeprom Entry ID %%d started");

    CmdPacket.EntryID = 1;

    CS_AppData.ResEepromTblPtr[CmdPacket.EntryID].State = 99;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);

    /* Execute the function being tested */
    CS_RecomputeBaselineEepromCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.RecomputeInProgress == true, "CS_AppData.HkPacket.RecomputeInProgress == true");
    UtAssert_True(CS_AppData.HkPacket.OneShotInProgress == false, "CS_AppData.HkPacket.OneShotInProgress == false");

    UtAssert_True(CS_AppData.ChildTaskTable == CS_EEPROM_TABLE, "CS_AppData.ChildTaskTable == CS_EEPROM_TABLE");
    UtAssert_True(CS_AppData.ChildTaskEntryID == CmdPacket.EntryID, "CS_AppData.ChildTaskEntryID == CmdPacket.EntryID");
    UtAssert_True(CS_AppData.RecomputeEepromMemoryEntryPtr == &CS_AppData.ResEepromTblPtr[CmdPacket.EntryID],
                  "CS_AppData.RecomputeEepromMemoryEntryPtr == &CS_AppData.ResEepromTblPtr[CmdPacket.EntryID]");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_RECOMPUTE_EEPROM_STARTED_DBG_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_DEBUG);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.CmdCounter == 1, "CS_AppData.HkPacket.CmdCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_RecomputeBaselineEepromCmd_Test_Nominal */

void CS_RecomputeBaselineEepromCmd_Test_CreateChildTaskError(void)
{
    CS_EntryCmd_t CmdPacket;
    int32         strCmpResult;
    char          ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Recompute baseline of Eeprom Entry ID %%d failed, CFE_ES_CreateChildTask returned:  0x%%08X");

    CmdPacket.EntryID = 1;

    CS_AppData.ResEepromTblPtr[CmdPacket.EntryID].State = 99;

    /* Set to generate error message CS_RECOMPUTE_EEPROM_CREATE_CHDTASK_ERR_EID */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_CreateChildTask), 1, -1);

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);

    /* Execute the function being tested */
    CS_RecomputeBaselineEepromCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.OneShotInProgress == false, "CS_AppData.HkPacket.OneShotInProgress == false");

    UtAssert_True(CS_AppData.ChildTaskTable == CS_EEPROM_TABLE, "CS_AppData.ChildTaskTable == CS_EEPROM_TABLE");
    UtAssert_True(CS_AppData.ChildTaskEntryID == CmdPacket.EntryID, "CS_AppData.ChildTaskEntryID == CmdPacket.EntryID");
    UtAssert_True(CS_AppData.RecomputeEepromMemoryEntryPtr == &CS_AppData.ResEepromTblPtr[CmdPacket.EntryID],
                  "CS_AppData.RecomputeEepromMemoryEntryPtr == &CS_AppData.ResEepromTblPtr[CmdPacket.EntryID]");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_RECOMPUTE_EEPROM_CREATE_CHDTASK_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.CmdErrCounter == 1, "CS_AppData.HkPacket.CmdErrCounter == 1");
    UtAssert_True(CS_AppData.HkPacket.RecomputeInProgress == false, "CS_AppData.HkPacket.RecomputeInProgress == false");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_RecomputeBaselineEepromCmd_Test_CreateChildTaskError */

void CS_RecomputeBaselineEepromCmd_Test_InvalidEntryErrorEntryIDTooHigh(void)
{
    CS_EntryCmd_t CmdPacket;
    int32         strCmpResult;
    char          ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Eeprom recompute baseline of entry failed, Entry ID invalid: %%d, State: %%d, Max ID: %%d");

    CmdPacket.EntryID = CS_MAX_NUM_EEPROM_TABLE_ENTRIES;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);

    /* Execute the function being tested */
    CS_RecomputeBaselineEepromCmd(&CmdPacket);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_RECOMPUTE_INVALID_ENTRY_EEPROM_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.CmdErrCounter == 1, "CS_AppData.HkPacket.CmdErrCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_RecomputeBaselineEepromCmd_Test_InvalidEntryErrorEntryIDTooHigh */

void CS_RecomputeBaselineEepromCmd_Test_InvalidEntryErrorStateEmpty(void)
{
    CS_EntryCmd_t CmdPacket;
    int32         strCmpResult;
    char          ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Eeprom recompute baseline of entry failed, Entry ID invalid: %%d, State: %%d, Max ID: %%d");

    CmdPacket.EntryID = 1;

    CS_AppData.ResEepromTblPtr[CmdPacket.EntryID].State = CS_STATE_EMPTY;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);

    /* Execute the function being tested */
    CS_RecomputeBaselineEepromCmd(&CmdPacket);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_RECOMPUTE_INVALID_ENTRY_EEPROM_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.CmdErrCounter == 1, "CS_AppData.HkPacket.CmdErrCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_RecomputeBaselineEepromCmd_Test_InvalidEntryErrorStateEmpty */

void CS_RecomputeBaselineEepromCmd_Test_RecomputeInProgress(void)
{
    CS_EntryCmd_t CmdPacket;
    int32         strCmpResult;
    char          ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Recompute baseline of Eeprom Entry ID %%d failed: child task in use");

    CmdPacket.EntryID = 1;

    CS_AppData.HkPacket.RecomputeInProgress = true;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);

    /* Execute the function being tested */
    CS_RecomputeBaselineEepromCmd(&CmdPacket);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_RECOMPUTE_EEPROM_CHDTASK_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.CmdErrCounter == 1, "CS_AppData.HkPacket.CmdErrCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_RecomputeBaselineEepromCmd_Test_RecomputeInProgress */

void CS_RecomputeBaselineEepromCmd_Test_VerifyError(void)
{
    CS_EntryCmd_t CmdPacket;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, false);

    /* Execute the function being tested */
    CS_RecomputeBaselineEepromCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.CmdCounter == 0, "CS_AppData.HkPacket.CmdCounter == 0");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_RecomputeBaselineEepromCmd_Test_VerifyError */

void CS_RecomputeBaselineEepromCmd_Test_OneShot(void)
{
    CS_EntryCmd_t CmdPacket;
    int32         strCmpResult;
    char          ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Recompute baseline of Eeprom Entry ID %%d failed: child task in use");

    CmdPacket.EntryID = 1;

    CS_AppData.HkPacket.RecomputeInProgress = false;
    CS_AppData.HkPacket.OneShotInProgress   = true;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);

    /* Execute the function being tested */
    CS_RecomputeBaselineEepromCmd(&CmdPacket);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_RECOMPUTE_EEPROM_CHDTASK_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.CmdErrCounter == 1, "CS_AppData.HkPacket.CmdErrCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_RecomputeBaselineEepromCmd_Test_OneShot */

void CS_EnableEntryIDEepromCmd_Test_Nominal(void)
{
    CS_EntryCmd_t CmdPacket;
    int32         strCmpResult;
    char          ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Checksumming of Eeprom Entry ID %%d is Enabled");

    CmdPacket.EntryID = 1;

    CS_AppData.ResEepromTblPtr[CmdPacket.EntryID].State = 99;
    CS_AppData.DefEepromTblPtr[CmdPacket.EntryID].State = 99;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);

    /* Execute the function being tested */
    CS_EnableEntryIDEepromCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.ResEepromTblPtr[CmdPacket.EntryID].State == CS_STATE_ENABLED,
                  "CS_AppData.ResEepromTblPtr[CmdPacket.EntryID].State == CS_STATE_ENABLED");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_ENABLE_EEPROM_ENTRY_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.DefEepromTblPtr[CmdPacket.EntryID].State == CS_STATE_ENABLED,
                  "CS_AppData.DefEepromTblPtr[CmdPacket.EntryID].State == CS_STATE_ENABLED");

    UtAssert_True(CS_AppData.HkPacket.CmdCounter == 1, "CS_AppData.HkPacket.CmdCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_EnableEntryIDEepromCmd_Test_Nominal */

void CS_EnableEntryIDEepromCmd_Test_DefEepromTblPtrStateEmpty(void)
{
    CS_EntryCmd_t CmdPacket;
    int32         strCmpResult;
    char          ExpectedEventString[2][CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString[0], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Checksumming of Eeprom Entry ID %%d is Enabled");

    snprintf(ExpectedEventString[1], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS unable to update Eeprom definition table for entry %%d, State: %%d");

    CmdPacket.EntryID = 1;

    CS_AppData.ResEepromTblPtr[CmdPacket.EntryID].State = 99;
    CS_AppData.DefEepromTblPtr[CmdPacket.EntryID].State = CS_STATE_EMPTY;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);

    /* Execute the function being tested */
    CS_EnableEntryIDEepromCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.ResEepromTblPtr[CmdPacket.EntryID].State == CS_STATE_ENABLED,
                  "CS_AppData.ResEepromTblPtr[CmdPacket.EntryID].State == CS_STATE_ENABLED");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_ENABLE_EEPROM_ENTRY_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult =
        strncmp(ExpectedEventString[0], context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, CS_ENABLE_EEPROM_DEF_EMPTY_DBG_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_DEBUG);

    strCmpResult =
        strncmp(ExpectedEventString[1], context_CFE_EVS_SendEvent[1].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[1].Spec);

    UtAssert_True(CS_AppData.HkPacket.CmdCounter == 1, "CS_AppData.HkPacket.CmdCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 2, "CFE_EVS_SendEvent was called %u time(s), expected 2",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_EnableEntryIDEepromCmd_Test_DefEepromTblPtrStateEmpty */

void CS_EnableEntryIDEepromCmd_Test_InvalidEntryErrorEntryIDTooHigh(void)
{
    CS_EntryCmd_t CmdPacket;
    int32         strCmpResult;
    char          ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Enable Eeprom entry failed, invalid Entry ID:  %%d, State: %%d, Max ID: %%d");

    CmdPacket.EntryID = CS_MAX_NUM_EEPROM_TABLE_ENTRIES;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);

    /* Execute the function being tested */
    CS_EnableEntryIDEepromCmd(&CmdPacket);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_ENABLE_EEPROM_INVALID_ENTRY_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.CmdErrCounter == 1, "CS_AppData.HkPacket.CmdErrCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_EnableEntryIDEepromCmd_Test_InvalidEntryErrorEntryIDTooHigh */

void CS_EnableEntryIDEepromCmd_Test_InvalidEntryErrorStateEmpty(void)
{
    CS_EntryCmd_t CmdPacket;
    int32         strCmpResult;
    char          ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Enable Eeprom entry failed, invalid Entry ID:  %%d, State: %%d, Max ID: %%d");

    CmdPacket.EntryID = 1;

    CS_AppData.ResEepromTblPtr[CmdPacket.EntryID].State = CS_STATE_EMPTY;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);

    /* Execute the function being tested */
    CS_EnableEntryIDEepromCmd(&CmdPacket);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_ENABLE_EEPROM_INVALID_ENTRY_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.CmdErrCounter == 1, "CS_AppData.HkPacket.CmdErrCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_EnableEntryIDEepromCmd_Test_InvalidEntryErrorStateEmpty */

void CS_EnableEntryIDEepromCmd_Test_VerifyError(void)
{
    CS_EntryCmd_t CmdPacket;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, false);

    /* Execute the function being tested */
    CS_EnableEntryIDEepromCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.CmdCounter == 0, "CS_AppData.HkPacket.CmdCounter == 0");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_EnableEntryIDEepromCmd_Test_VerifyError */

void CS_EnableEntryIDEepromCmd_Test_OneShot(void)
{
    CS_EntryCmd_t CmdPacket;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);
    UT_SetDeferredRetcode(UT_KEY(CS_CheckRecomputeOneshot), 1, true);

    /* Execute the function being tested */
    CS_EnableEntryIDEepromCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.CmdCounter == 0, "CS_AppData.HkPacket.CmdCounter == 0");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_EnableEntryIDEepromCmd_Test_OneShot */

void CS_DisableEntryIDEepromCmd_Test_Nominal(void)
{
    CS_EntryCmd_t CmdPacket;
    int32         strCmpResult;
    char          ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Checksumming of Eeprom Entry ID %%d is Disabled");

    CmdPacket.EntryID = 1;

    CS_AppData.ResEepromTblPtr[CmdPacket.EntryID].State = 99;
    CS_AppData.DefEepromTblPtr[CmdPacket.EntryID].State = 99;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);

    /* Execute the function being tested */
    CS_DisableEntryIDEepromCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.ResEepromTblPtr[CmdPacket.EntryID].State == CS_STATE_DISABLED,
                  "CS_AppData.ResEepromTblPtr[CmdPacket.EntryID].State == CS_STATE_DISABLED");
    UtAssert_True(CS_AppData.ResEepromTblPtr[CmdPacket.EntryID].TempChecksumValue == 0,
                  "CS_AppData.ResEepromTblPtr[CmdPacket.EntryID].TempChecksumValue == 0");
    UtAssert_True(CS_AppData.ResEepromTblPtr[CmdPacket.EntryID].ByteOffset == 0,
                  "CS_AppData.ResEepromTblPtr[CmdPacket.EntryID].ByteOffset == 0");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_DISABLE_EEPROM_ENTRY_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.DefEepromTblPtr[CmdPacket.EntryID].State == CS_STATE_DISABLED,
                  "CS_AppData.DefEepromTblPtr[CmdPacket.EntryID].State == CS_STATE_DISABLED");

    UtAssert_True(CS_AppData.HkPacket.CmdCounter == 1, "CS_AppData.HkPacket.CmdCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_DisableEntryIDEepromCmd_Test_Nominal */

void CS_DisableEntryIDEepromCmd_Test_DefEepromTblPtrStateEmpty(void)
{
    CS_EntryCmd_t CmdPacket;
    int32         strCmpResult;
    char          ExpectedEventString[2][CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString[0], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Checksumming of Eeprom Entry ID %%d is Disabled");

    snprintf(ExpectedEventString[1], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS unable to update Eeprom definition table for entry %%d, State: %%d");

    CmdPacket.EntryID = 1;

    CS_AppData.ResEepromTblPtr[CmdPacket.EntryID].State = 99;
    CS_AppData.DefEepromTblPtr[CmdPacket.EntryID].State = CS_STATE_EMPTY;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);

    /* Execute the function being tested */
    CS_DisableEntryIDEepromCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.ResEepromTblPtr[CmdPacket.EntryID].State == CS_STATE_DISABLED,
                  "CS_AppData.ResEepromTblPtr[CmdPacket.EntryID].State == CS_STATE_DISABLED");
    UtAssert_True(CS_AppData.ResEepromTblPtr[CmdPacket.EntryID].TempChecksumValue == 0,
                  "CS_AppData.ResEepromTblPtr[CmdPacket.EntryID].TempChecksumValue == 0");
    UtAssert_True(CS_AppData.ResEepromTblPtr[CmdPacket.EntryID].ByteOffset == 0,
                  "CS_AppData.ResEepromTblPtr[CmdPacket.EntryID].ByteOffset == 0");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_DISABLE_EEPROM_ENTRY_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult =
        strncmp(ExpectedEventString[0], context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, CS_DISABLE_EEPROM_DEF_EMPTY_DBG_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_DEBUG);

    strCmpResult =
        strncmp(ExpectedEventString[1], context_CFE_EVS_SendEvent[1].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[1].Spec);

    UtAssert_True(CS_AppData.HkPacket.CmdCounter == 1, "CS_AppData.HkPacket.CmdCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 2, "CFE_EVS_SendEvent was called %u time(s), expected 2",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_DisableEntryIDEepromCmd_Test_DefEepromTblPtrStateEmpty */

void CS_DisableEntryIDEepromCmd_Test_InvalidEntryErrorEntryIDTooHigh(void)
{
    CS_EntryCmd_t CmdPacket;
    int32         strCmpResult;
    char          ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Disable Eeprom entry failed, invalid Entry ID:  %%d, State: %%d, Max ID: %%d");

    CmdPacket.EntryID = CS_MAX_NUM_EEPROM_TABLE_ENTRIES;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);

    /* Execute the function being tested */
    CS_DisableEntryIDEepromCmd(&CmdPacket);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_DISABLE_EEPROM_INVALID_ENTRY_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.CmdErrCounter == 1, "CS_AppData.HkPacket.CmdErrCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_DisableEntryIDEepromCmd_Test_InvalidEntryErrorEntryIDTooHigh */

void CS_DisableEntryIDEepromCmd_Test_InvalidEntryErrorStateEmpty(void)
{
    CS_EntryCmd_t CmdPacket;
    int32         strCmpResult;
    char          ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Disable Eeprom entry failed, invalid Entry ID:  %%d, State: %%d, Max ID: %%d");

    CmdPacket.EntryID = 1;

    CS_AppData.ResEepromTblPtr[CmdPacket.EntryID].State = CS_STATE_EMPTY;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);

    /* Execute the function being tested */
    CS_DisableEntryIDEepromCmd(&CmdPacket);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_DISABLE_EEPROM_INVALID_ENTRY_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.CmdErrCounter == 1, "CS_AppData.HkPacket.CmdErrCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_DisableEntryIDEepromCmd_Test_InvalidEntryErrorStateEmpty */

void CS_DisableEntryIDEepromCmd_Test_VerifyError(void)
{
    CS_EntryCmd_t CmdPacket;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, false);

    /* Execute the function being tested */
    CS_DisableEntryIDEepromCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.CmdCounter == 0, "CS_AppData.HkPacket.CmdCounter == 0");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_DisableEntryIDEepromCmd_Test_VerifyError */

void CS_DisableEntryIDEepromCmd_Test_OneShot(void)
{
    CS_EntryCmd_t CmdPacket;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);
    UT_SetDeferredRetcode(UT_KEY(CS_CheckRecomputeOneshot), 1, true);

    /* Execute the function being tested */
    CS_DisableEntryIDEepromCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.CmdCounter == 0, "CS_AppData.HkPacket.CmdCounter == 0");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_DisableEntryIDEepromCmd_Test_OneShot */

void CS_GetEntryIDEepromCmd_Test_Nominal(void)
{
    CS_GetEntryIDCmd_t CmdPacket;
    int32              strCmpResult;
    char               ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Eeprom Found Address 0x%%08X in Entry ID %%d");

    int16 EntryID = 1;

    CS_AppData.ResEepromTblPtr[EntryID].StartAddress       = 1;
    CmdPacket.Address                                      = 1;
    CS_AppData.ResEepromTblPtr[EntryID].NumBytesToChecksum = 0;
    CS_AppData.ResEepromTblPtr[EntryID].State              = 99;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);

    /* Execute the function being tested */
    CS_GetEntryIDEepromCmd(&CmdPacket);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_GET_ENTRY_ID_EEPROM_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.CmdCounter == 1, "CS_AppData.HkPacket.CmdCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_GetEntryIDEepromCmd_Test_Nominal */

void CS_GetEntryIDEepromCmd_Test_AddressNotFound(void)
{
    CS_GetEntryIDCmd_t CmdPacket;
    int32              strCmpResult;
    char               ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Address 0x%%08X was not found in Eeprom table");

    CmdPacket.Address = 0xFFFFFFFF;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);

    /* Execute the function being tested */
    CS_GetEntryIDEepromCmd(&CmdPacket);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_GET_ENTRY_ID_EEPROM_NOT_FOUND_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.CmdCounter == 1, "CS_AppData.HkPacket.CmdCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_GetEntryIDEepromCmd_Test_AddressNotFound */

void CS_GetEntryIDEepromCmd_Test_AddressPtr(void)
{
    CS_GetEntryIDCmd_t CmdPacket;
    int32              strCmpResult;
    char               ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Address 0x%%08X was not found in Eeprom table");

    int16 EntryID = 1;

    CS_AppData.ResEepromTblPtr[EntryID].StartAddress       = 2;
    CmdPacket.Address                                      = 1;
    CS_AppData.ResEepromTblPtr[EntryID].NumBytesToChecksum = 0;
    CS_AppData.ResEepromTblPtr[EntryID].State              = 99;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);

    /* Execute the function being tested */
    CS_GetEntryIDEepromCmd(&CmdPacket);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_GET_ENTRY_ID_EEPROM_NOT_FOUND_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.CmdCounter == 1, "CS_AppData.HkPacket.CmdCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_GetEntryIDEepromCmd_Test_AddressPtr */

void CS_GetEntryIDEepromCmd_Test_State(void)
{
    CS_GetEntryIDCmd_t CmdPacket;
    int32              strCmpResult;
    char               ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Address 0x%%08X was not found in Eeprom table");

    int16 EntryID = 1;

    CS_AppData.ResEepromTblPtr[EntryID].StartAddress       = 1;
    CmdPacket.Address                                      = 1;
    CS_AppData.ResEepromTblPtr[EntryID].NumBytesToChecksum = 0;
    CS_AppData.ResEepromTblPtr[EntryID].State              = CS_STATE_EMPTY;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);

    /* Execute the function being tested */
    CS_GetEntryIDEepromCmd(&CmdPacket);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_GET_ENTRY_ID_EEPROM_NOT_FOUND_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.CmdCounter == 1, "CS_AppData.HkPacket.CmdCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_GetEntryIDEepromCmd_Test_State */

void CS_GetEntryIDEepromCmd_Test_VerifyError(void)
{
    CS_GetEntryIDCmd_t CmdPacket;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, false);

    /* Execute the function being tested */
    CS_GetEntryIDEepromCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.CmdCounter == 0, "CS_AppData.HkPacket.CmdCounter == 0");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_GetEntryIDEepromCmd_Test_VerifyError */

void UtTest_Setup(void)
{
    UtTest_Add(CS_DisableEepromCmd_Test, CS_Test_Setup, CS_Test_TearDown, "CS_DisableEepromCmd_Test");
    UtTest_Add(CS_DisableEepromCmd_Test_VerifyError, CS_Test_Setup, CS_Test_TearDown,
               "CS_DisableEepromCmd_Test_VerifyError");
    UtTest_Add(CS_DisableEepromCmd_Test_OneShot, CS_Test_Setup, CS_Test_TearDown, "CS_DisableEepromCmd_Test_OneShot");

    UtTest_Add(CS_EnableEepromCmd_Test, CS_Test_Setup, CS_Test_TearDown, "CS_EnableEepromCmd_Test");
    UtTest_Add(CS_EnableEepromCmd_Test_VerifyError, CS_Test_Setup, CS_Test_TearDown,
               "CS_EnableEepromCmd_Test_VerifyError");
    UtTest_Add(CS_EnableEepromCmd_Test_OneShot, CS_Test_Setup, CS_Test_TearDown, "CS_EnableEepromCmd_Test_OneShot");

    UtTest_Add(CS_ReportBaselineEntryIDEepromCmd_Test_Computed, CS_Test_Setup, CS_Test_TearDown,
               "CS_ReportBaselineEntryIDEepromCmd_Test_Computed");
    UtTest_Add(CS_ReportBaselineEntryIDEepromCmd_Test_NotYetComputed, CS_Test_Setup, CS_Test_TearDown,
               "CS_ReportBaselineEntryIDEepromCmd_Test_NotYetComputed");
    UtTest_Add(CS_ReportBaselineEntryIDEepromCmd_Test_InvalidEntryErrorEntryIDTooHigh, CS_Test_Setup, CS_Test_TearDown,
               "CS_ReportBaselineEntryIDEepromCmd_Test_InvalidEntryErrorEntryIDTooHigh");
    UtTest_Add(CS_ReportBaselineEntryIDEepromCmd_Test_InvalidEntryErrorStateEmpty, CS_Test_Setup, CS_Test_TearDown,
               "CS_ReportBaselineEntryIDEepromCmd_Test_InvalidEntryErrorStateEmpty");
    UtTest_Add(CS_ReportBaselineEntryIDEepromCmd_Test_VerifyError, CS_Test_Setup, CS_Test_TearDown,
               "CS_ReportBaselineEntryIDEepromCmd_Test_VerifyError");

    UtTest_Add(CS_RecomputeBaselineEepromCmd_Test_Nominal, CS_Test_Setup, CS_Test_TearDown,
               "CS_RecomputeBaselineEepromCmd_Test_Nominal");
    UtTest_Add(CS_RecomputeBaselineEepromCmd_Test_CreateChildTaskError, CS_Test_Setup, CS_Test_TearDown,
               "CS_RecomputeBaselineEepromCmd_Test_CreateChildTaskError");
    UtTest_Add(CS_RecomputeBaselineEepromCmd_Test_InvalidEntryErrorEntryIDTooHigh, CS_Test_Setup, CS_Test_TearDown,
               "CS_RecomputeBaselineEepromCmd_Test_InvalidEntryErrorEntryIDTooHigh");
    UtTest_Add(CS_RecomputeBaselineEepromCmd_Test_InvalidEntryErrorStateEmpty, CS_Test_Setup, CS_Test_TearDown,
               "CS_RecomputeBaselineEepromCmd_Test_InvalidEntryErrorStateEmpty");
    UtTest_Add(CS_RecomputeBaselineEepromCmd_Test_RecomputeInProgress, CS_Test_Setup, CS_Test_TearDown,
               "CS_RecomputeBaselineEepromCmd_Test_RecomputeInProgress");
    UtTest_Add(CS_RecomputeBaselineEepromCmd_Test_VerifyError, CS_Test_Setup, CS_Test_TearDown,
               "CS_RecomputeBaselineEepromCmd_Test_VerifyError");
    UtTest_Add(CS_RecomputeBaselineEepromCmd_Test_OneShot, CS_Test_Setup, CS_Test_TearDown,
               "CS_RecomputeBaselineEepromCmd_Test_OneShot");

    UtTest_Add(CS_EnableEntryIDEepromCmd_Test_Nominal, CS_Test_Setup, CS_Test_TearDown,
               "CS_EnableEntryIDEepromCmd_Test_Nominal");
    UtTest_Add(CS_EnableEntryIDEepromCmd_Test_DefEepromTblPtrStateEmpty, CS_Test_Setup, CS_Test_TearDown,
               "CS_EnableEntryIDEepromCmd_Test_DefEepromTblPtrStateEmpty");
    UtTest_Add(CS_EnableEntryIDEepromCmd_Test_InvalidEntryErrorEntryIDTooHigh, CS_Test_Setup, CS_Test_TearDown,
               "CS_EnableEntryIDEepromCmd_Test_InvalidEntryErrorEntryIDTooHigh");
    UtTest_Add(CS_EnableEntryIDEepromCmd_Test_InvalidEntryErrorStateEmpty, CS_Test_Setup, CS_Test_TearDown,
               "CS_EnableEntryIDEepromCmd_Test_InvalidEntryErrorStateEmpty");
    UtTest_Add(CS_EnableEntryIDEepromCmd_Test_VerifyError, CS_Test_Setup, CS_Test_TearDown,
               "CS_EnableEntryIDEepromCmd_Test_VerifyError");
    UtTest_Add(CS_EnableEntryIDEepromCmd_Test_OneShot, CS_Test_Setup, CS_Test_TearDown,
               "CS_EnableEntryIDEepromCmd_Test_OneShot");

    UtTest_Add(CS_DisableEntryIDEepromCmd_Test_Nominal, CS_Test_Setup, CS_Test_TearDown,
               "CS_DisableEntryIDEepromCmd_Test_Nominal");
    UtTest_Add(CS_DisableEntryIDEepromCmd_Test_DefEepromTblPtrStateEmpty, CS_Test_Setup, CS_Test_TearDown,
               "CS_DisableEntryIDEepromCmd_Test_DefEepromTblPtrStateEmpty");
    UtTest_Add(CS_DisableEntryIDEepromCmd_Test_InvalidEntryErrorEntryIDTooHigh, CS_Test_Setup, CS_Test_TearDown,
               "CS_DisableEntryIDEepromCmd_Test_InvalidEntryErrorEntryIDTooHigh");
    UtTest_Add(CS_DisableEntryIDEepromCmd_Test_InvalidEntryErrorStateEmpty, CS_Test_Setup, CS_Test_TearDown,
               "CS_DisableEntryIDEepromCmd_Test_InvalidEntryErrorStateEmpty");
    UtTest_Add(CS_DisableEntryIDEepromCmd_Test_VerifyError, CS_Test_Setup, CS_Test_TearDown,
               "CS_DisableEntryIDEepromCmd_Test_VerifyError");
    UtTest_Add(CS_DisableEntryIDEepromCmd_Test_OneShot, CS_Test_Setup, CS_Test_TearDown,
               "CS_DisableEntryIDEepromCmd_Test_OneShot");

    UtTest_Add(CS_GetEntryIDEepromCmd_Test_Nominal, CS_Test_Setup, CS_Test_TearDown,
               "CS_GetEntryIDEepromCmd_Test_Nominal");
    UtTest_Add(CS_GetEntryIDEepromCmd_Test_AddressNotFound, CS_Test_Setup, CS_Test_TearDown,
               "CS_GetEntryIDEepromCmd_Test_AddressNotFound");
    UtTest_Add(CS_GetEntryIDEepromCmd_Test_AddressPtr, CS_Test_Setup, CS_Test_TearDown,
               "CS_GetEntryIDEepromCmd_Test_AddressPtr");
    UtTest_Add(CS_GetEntryIDEepromCmd_Test_State, CS_Test_Setup, CS_Test_TearDown, "CS_GetEntryIDEepromCmd_Test_State");
    UtTest_Add(CS_GetEntryIDEepromCmd_Test_VerifyError, CS_Test_Setup, CS_Test_TearDown,
               "CS_GetEntryIDEepromCmd_Test_VerifyError");

} /* end UtTest_Setup */

/************************/
/*  End of File Comment */
/************************/
