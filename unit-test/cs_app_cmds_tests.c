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

#include "cs_app_cmds.h"
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

/* cs_app_cmds_tests globals */
uint8 call_count_CFE_EVS_SendEvent;

/*
 * Function Definitions
 */

void CS_APP_CMDS_TEST_CS_GetAppResTblEntryByNameHandler1(void *UserObj, UT_EntryKey_t FuncKey,
                                                         const UT_StubContext_t *Context)
{
    CS_Res_App_Table_Entry_t **EntryPtr =
        (CS_Res_App_Table_Entry_t **)UT_Hook_GetArgValueByName(Context, "EntryPtr", CS_Res_App_Table_Entry_t **);

    *EntryPtr = CS_AppData.ResAppTblPtr;

    CS_AppData.ResAppTblPtr->ComputedYet = true;
}

void CS_APP_CMDS_TEST_CS_GetAppResTblEntryByNameHandler2(void *UserObj, UT_EntryKey_t FuncKey,
                                                         const UT_StubContext_t *Context)
{
    CS_Res_App_Table_Entry_t **EntryPtr =
        (CS_Res_App_Table_Entry_t **)UT_Hook_GetArgValueByName(Context, "EntryPtr", CS_Res_App_Table_Entry_t **);

    *EntryPtr = CS_AppData.ResAppTblPtr;

    CS_AppData.ResAppTblPtr->ComputedYet = false;
}

void CS_APP_CMDS_TEST_CS_GetAppDefTblEntryByNameHandler1(void *UserObj, UT_EntryKey_t FuncKey,
                                                         const UT_StubContext_t *Context)
{
    CS_Def_Tables_Table_Entry_t **EntryPtr =
        (CS_Def_Tables_Table_Entry_t **)UT_Hook_GetArgValueByName(Context, "EntryPtr", CS_Def_Tables_Table_Entry_t **);

    *EntryPtr = CS_AppData.DefTablesTblPtr;
}

void CS_DisableAppCmd_Test(void)
{
    CS_NoArgsCmd_t CmdPacket;
    int32          strCmpResult;
    char           ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Checksumming of App is Disabled");

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);

    /* Execute the function being tested */
    CS_DisableAppCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.AppCSState == CS_STATE_DISABLED,
                  "CS_AppData.HkPacket.AppCSState == CS_STATE_DISABLED");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_DISABLE_APP_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.CmdCounter == 1, "CS_AppData.HkPacket.CmdCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_DisableAppCmd_Test_VerifyError(void)
{
    CS_NoArgsCmd_t CmdPacket;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, false);

    /* Execute the function being tested */
    CS_DisableAppCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.CmdCounter == 0, "CS_AppData.HkPacket.CmdCounter == 0");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void CS_DisableAppCmd_Test_OneShot(void)
{
    CS_NoArgsCmd_t CmdPacket;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);

    UT_SetDeferredRetcode(UT_KEY(CS_CheckRecomputeOneshot), 1, true);

    /* Execute the function being tested */
    CS_DisableAppCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.CmdCounter == 0, "CS_AppData.HkPacket.CmdCounter == 0");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void CS_EnableAppCmd_Test(void)
{
    CS_NoArgsCmd_t CmdPacket;
    int32          strCmpResult;
    char           ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Checksumming of App is Enabled");

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);

    /* Execute the function being tested */
    CS_EnableAppCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.AppCSState == CS_STATE_ENABLED,
                  "CS_AppData.HkPacket.AppCSState == CS_STATE_ENABLED");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_ENABLE_APP_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.CmdCounter == 1, "CS_AppData.HkPacket.CmdCounter == 1");

    UtAssert_True(CS_AppData.HkPacket.CmdCounter == 1, "CS_AppData.HkPacket.CmdCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_EnableAppCmd_Test_VerifyError(void)
{
    CS_NoArgsCmd_t CmdPacket;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, false);

    /* Execute the function being tested */
    CS_EnableAppCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.CmdCounter == 0, "CS_AppData.HkPacket.CmdCounter == 0");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void CS_EnableAppCmd_Test_OneShot(void)
{
    CS_NoArgsCmd_t CmdPacket;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);

    UT_SetDeferredRetcode(UT_KEY(CS_CheckRecomputeOneshot), 1, true);

    /* Execute the function being tested */
    CS_EnableAppCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.CmdCounter == 0, "CS_AppData.HkPacket.CmdCounter == 0");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void CS_ReportBaselineAppCmd_Test_Baseline(void)
{
    CS_AppNameCmd_t CmdPacket;
    int32           strCmpResult;
    char            ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Report baseline of app %%s is 0x%%08X");

    strncpy(CmdPacket.Name, "App1", OS_MAX_API_NAME);
    strncpy(CS_AppData.ResAppTblPtr->Name, "App1", OS_MAX_API_NAME);

    /* Needed to make subfunction CS_GetAppResTblEntryByName behave properly */
    CS_AppData.ResAppTblPtr->State           = 1;
    CS_AppData.ResAppTblPtr->ComputedYet     = true;
    CS_AppData.ResAppTblPtr->ComparisonValue = 1;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);
    UT_SetDeferredRetcode(UT_KEY(CS_GetAppResTblEntryByName), 1, true);
    UT_SetHandlerFunction(UT_KEY(CS_GetAppResTblEntryByName), CS_APP_CMDS_TEST_CS_GetAppResTblEntryByNameHandler1,
                          NULL);

    /* Execute the function being tested */
    CS_ReportBaselineAppCmd(&CmdPacket);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_BASELINE_APP_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.CmdCounter == 1, "CS_AppData.HkPacket.CmdCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_ReportBaselineAppCmd_Test_NoBaseline(void)
{
    CS_AppNameCmd_t CmdPacket;
    int32           strCmpResult;
    char            ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Report baseline of app %%s has not been computed yet");

    strncpy(CmdPacket.Name, "App1", OS_MAX_API_NAME);
    strncpy(CS_AppData.ResAppTblPtr->Name, "App1", OS_MAX_API_NAME);

    /* Needed to make subfunction CS_GetAppResTblEntryByName behave properly */
    CS_AppData.ResAppTblPtr->State       = 1;
    CS_AppData.ResAppTblPtr->ComputedYet = false;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);
    UT_SetDeferredRetcode(UT_KEY(CS_GetAppResTblEntryByName), 1, true);
    UT_SetHandlerFunction(UT_KEY(CS_GetAppResTblEntryByName), CS_APP_CMDS_TEST_CS_GetAppResTblEntryByNameHandler2,
                          NULL);

    /* Execute the function being tested */
    CS_ReportBaselineAppCmd(&CmdPacket);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_NO_BASELINE_APP_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.CmdCounter == 1, "CS_AppData.HkPacket.CmdCounter == 1");

    UtAssert_True(CS_AppData.HkPacket.CmdCounter == 1, "CS_AppData.HkPacket.CmdCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_ReportBaselineAppCmd_Test_BaselineInvalidName(void)
{
    CS_AppNameCmd_t CmdPacket;
    int32           strCmpResult;
    char            ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "App report baseline failed, app %%s not found");

    strncpy(CmdPacket.Name, "App1", OS_MAX_API_NAME);
    strncpy(CS_AppData.ResAppTblPtr->Name, "App2", OS_MAX_API_NAME);

    /* Needed to make subfunction CS_GetAppResTblEntryByName behave properly */
    CS_AppData.ResAppTblPtr->State = 1;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);
    UT_SetDeferredRetcode(UT_KEY(CS_GetAppResTblEntryByName), 1, false);

    /* Execute the function being tested */
    CS_ReportBaselineAppCmd(&CmdPacket);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_BASELINE_INVALID_NAME_APP_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.CmdErrCounter == 1, "CS_AppData.HkPacket.CmdErrCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_ReportBaselineAppCmd_Test_VerifyError(void)
{
    CS_AppNameCmd_t CmdPacket;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, false);

    /* Execute the function being tested */
    CS_ReportBaselineAppCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.CmdCounter == 0, "CS_AppData.HkPacket.CmdCounter == 0");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void CS_ReportBaselineAppCmd_Test_OneShot(void)
{
    CS_AppNameCmd_t CmdPacket;
    int32           strCmpResult;
    char            ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "App recompute baseline for app %%s failed: child task in use");

    strncpy(CmdPacket.Name, "App1", OS_MAX_API_NAME);
    strncpy(CS_AppData.ResAppTblPtr->Name, "App2", OS_MAX_API_NAME);

    CS_AppData.HkPacket.RecomputeInProgress = false;
    CS_AppData.HkPacket.OneShotInProgress   = true;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);

    /* Execute the function being tested */
    CS_RecomputeBaselineAppCmd(&CmdPacket);
    UT_SetDeferredRetcode(UT_KEY(CS_GetAppResTblEntryByName), 1, true);
    UT_SetHandlerFunction(UT_KEY(CS_GetAppResTblEntryByName), CS_APP_CMDS_TEST_CS_GetAppResTblEntryByNameHandler1,
                          NULL);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_RECOMPUTE_APP_CHDTASK_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.CmdErrCounter == 1, "CS_AppData.HkPacket.CmdErrCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_RecomputeBaselineAppCmd_Test_Nominal(void)
{
    CS_AppNameCmd_t CmdPacket;
    int32           strCmpResult;
    char            ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Recompute baseline of app %%s started");

    strncpy(CmdPacket.Name, "App1", OS_MAX_API_NAME);
    strncpy(CS_AppData.ResAppTblPtr->Name, "App1", OS_MAX_API_NAME);

    CS_AppData.HkPacket.RecomputeInProgress = false;

    /* Needed to make subfunction CS_GetAppResTblEntryByName behave properly */
    CS_AppData.ResAppTblPtr->State = 1;

    /* Set to generate event message CS_RECOMPUTE_APP_STARTED_DBG_EID */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_CreateChildTask), 1, CFE_SUCCESS);

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);
    UT_SetDeferredRetcode(UT_KEY(CS_GetAppResTblEntryByName), 1, true);
    UT_SetHandlerFunction(UT_KEY(CS_GetAppResTblEntryByName), CS_APP_CMDS_TEST_CS_GetAppResTblEntryByNameHandler1,
                          NULL);

    /* Execute the function being tested */
    CS_RecomputeBaselineAppCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.ResAppTblPtr == CS_AppData.RecomputeAppEntryPtr,
                  "CS_AppData.ResAppTblPtr == CS_AppData.RecomputeAppEntryPtr");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_RECOMPUTE_APP_STARTED_DBG_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_DEBUG);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.CmdCounter == 1, "CS_AppData.HkPacket.CmdCounter == 1");

    UtAssert_True(CS_AppData.HkPacket.CmdCounter == 1, "CS_AppData.HkPacket.CmdCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_RecomputeBaselineAppCmd_Test_CreateChildTaskError(void)
{
    CS_AppNameCmd_t CmdPacket;
    int32           strCmpResult;
    char            ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Recompute baseline of app %%s failed, CFE_ES_CreateChildTask returned: 0x%%08X");

    strncpy(CmdPacket.Name, "App1", OS_MAX_API_NAME);
    strncpy(CS_AppData.ResAppTblPtr->Name, "App1", OS_MAX_API_NAME);

    CS_AppData.HkPacket.RecomputeInProgress = false;

    /* Needed to make subfunction CS_GetAppResTblEntryByName behave properly */
    CS_AppData.ResAppTblPtr->State = 1;

    /* Set to generate event message CS_RECOMPUTE_APP_CREATE_CHDTASK_ERR_EID */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_CreateChildTask), 1, -1);

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);
    UT_SetDeferredRetcode(UT_KEY(CS_GetAppResTblEntryByName), 1, true);
    UT_SetHandlerFunction(UT_KEY(CS_GetAppResTblEntryByName), CS_APP_CMDS_TEST_CS_GetAppResTblEntryByNameHandler1,
                          NULL);

    /* Execute the function being tested */
    CS_RecomputeBaselineAppCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.ResAppTblPtr == CS_AppData.RecomputeAppEntryPtr,
                  "CS_AppData.ResAppTblPtr == CS_AppData.RecomputeAppEntryPtr");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_RECOMPUTE_APP_CREATE_CHDTASK_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.CmdErrCounter == 1, "CS_AppData.HkPacket.CmdErrCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_RecomputeBaselineAppCmd_Test_UnknownNameError(void)
{
    CS_AppNameCmd_t CmdPacket;
    int32           strCmpResult;
    char            ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "App recompute baseline failed, app %%s not found");

    strncpy(CmdPacket.Name, "App1", OS_MAX_API_NAME);
    strncpy(CS_AppData.ResAppTblPtr->Name, "App2", OS_MAX_API_NAME);

    CS_AppData.HkPacket.RecomputeInProgress = false;

    /* Needed to make subfunction CS_GetAppResTblEntryByName behave properly */
    CS_AppData.ResAppTblPtr->State = 1;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);
    UT_SetDeferredRetcode(UT_KEY(CS_GetAppResTblEntryByName), 1, false);

    /* Execute the function being tested */
    CS_RecomputeBaselineAppCmd(&CmdPacket);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_RECOMPUTE_UNKNOWN_NAME_APP_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.CmdErrCounter == 1, "CS_AppData.HkPacket.CmdErrCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_RecomputeBaselineAppCmd_Test_RecomputeInProgress(void)
{
    CS_AppNameCmd_t CmdPacket;
    int32           strCmpResult;
    char            ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "App recompute baseline for app %%s failed: child task in use");

    strncpy(CmdPacket.Name, "App1", OS_MAX_API_NAME);
    strncpy(CS_AppData.ResAppTblPtr->Name, "App2", OS_MAX_API_NAME);

    CS_AppData.HkPacket.RecomputeInProgress = true;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);

    /* Execute the function being tested */
    CS_RecomputeBaselineAppCmd(&CmdPacket);
    UT_SetDeferredRetcode(UT_KEY(CS_GetAppResTblEntryByName), 1, true);
    UT_SetHandlerFunction(UT_KEY(CS_GetAppResTblEntryByName), CS_APP_CMDS_TEST_CS_GetAppResTblEntryByNameHandler1,
                          NULL);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_RECOMPUTE_APP_CHDTASK_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.CmdErrCounter == 1, "CS_AppData.HkPacket.CmdErrCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_RecomputeBaselineAppCmd_Test_VerifyError(void)
{
    CS_AppNameCmd_t CmdPacket;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, false);

    /* Execute the function being tested */
    CS_RecomputeBaselineAppCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.CmdCounter == 0, "CS_AppData.HkPacket.CmdCounter == 0");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void CS_DisableNameAppCmd_Test_Nominal(void)
{
    CS_AppNameCmd_t CmdPacket;
    int32           strCmpResult;
    char            ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Checksumming of app %%s is Disabled");

    strncpy(CmdPacket.Name, "App1", OS_MAX_API_NAME);
    strncpy(CS_AppData.ResAppTblPtr->Name, "App1", OS_MAX_API_NAME);
    strncpy(CS_AppData.DefAppTblPtr->Name, "App1", OS_MAX_API_NAME);

    /* Needed to make subfunction CS_GetAppDefTblEntryByName behave properly */
    CS_AppData.ResAppTblPtr->State = 1;

    /* Needed to make subfunction CS_GetAppDefTblEntryByName behave properly */
    CS_AppData.DefAppTblPtr->State = 1;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);
    UT_SetDeferredRetcode(UT_KEY(CS_CheckRecomputeOneshot), 1, false);
    UT_SetDeferredRetcode(UT_KEY(CS_GetAppResTblEntryByName), 1, true);
    UT_SetHandlerFunction(UT_KEY(CS_GetAppResTblEntryByName), CS_APP_CMDS_TEST_CS_GetAppResTblEntryByNameHandler1,
                          NULL);
    UT_SetDeferredRetcode(UT_KEY(CS_GetAppDefTblEntryByName), 1, true);
    UT_SetHandlerFunction(UT_KEY(CS_GetAppDefTblEntryByName), CS_APP_CMDS_TEST_CS_GetAppDefTblEntryByNameHandler1,
                          NULL);

    /* Execute the function being tested */
    CS_DisableNameAppCmd(&CmdPacket);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_DISABLE_APP_NAME_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.CmdCounter == 1, "CS_AppData.HkPacket.CmdCounter == 1");

    UtAssert_True(CS_AppData.HkPacket.CmdCounter == 1, "CS_AppData.HkPacket.CmdCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_DisableNameAppCmd_Test_UpdateAppsDefinitionTableError(void)
{
    CS_AppNameCmd_t CmdPacket;
    int32           strCmpResult;
    char            ExpectedEventString[2][CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString[0], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Checksumming of app %%s is Disabled");

    snprintf(ExpectedEventString[1], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS unable to update apps definition table for entry %%s");

    strncpy(CmdPacket.Name, "App1", OS_MAX_API_NAME);
    strncpy(CS_AppData.ResAppTblPtr->Name, "App1", OS_MAX_API_NAME);
    strncpy(CS_AppData.DefAppTblPtr->Name, "App1", OS_MAX_API_NAME);

    /* Needed to make subfunction CS_GetAppDefTblEntryByName behave properly */
    CS_AppData.ResAppTblPtr->State = 1;

    /* Set to make subfunction CS_GetAppDefTblEntryByName return false */
    CS_AppData.DefAppTblPtr->State = CS_STATE_EMPTY;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);
    UT_SetDeferredRetcode(UT_KEY(CS_GetAppResTblEntryByName), 1, true);
    UT_SetHandlerFunction(UT_KEY(CS_GetAppResTblEntryByName), CS_APP_CMDS_TEST_CS_GetAppResTblEntryByNameHandler1,
                          NULL);

    /* Execute the function being tested */
    CS_DisableNameAppCmd(&CmdPacket);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_DISABLE_APP_NAME_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult =
        strncmp(ExpectedEventString[0], context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, CS_DISABLE_APP_DEF_NOT_FOUND_DBG_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_DEBUG);

    strCmpResult =
        strncmp(ExpectedEventString[1], context_CFE_EVS_SendEvent[1].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[1].Spec);

    UtAssert_True(CS_AppData.HkPacket.CmdCounter == 1, "CS_AppData.HkPacket.CmdCounter == 1");

    UtAssert_True(CS_AppData.HkPacket.CmdCounter == 1, "CS_AppData.HkPacket.CmdCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 2, "CFE_EVS_SendEvent was called %u time(s), expected 2",
                  call_count_CFE_EVS_SendEvent);
}

void CS_DisableNameAppCmd_Test_UnknownNameError(void)
{
    CS_AppNameCmd_t CmdPacket;
    int32           strCmpResult;
    char            ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "App disable app command failed, app %%s not found");

    strncpy(CmdPacket.Name, "App1", OS_MAX_API_NAME);
    strncpy(CS_AppData.ResAppTblPtr->Name, "App1", OS_MAX_API_NAME);
    strncpy(CS_AppData.DefAppTblPtr->Name, "App1", OS_MAX_API_NAME);

    /* Set to make subfunction CS_GetAppResTblEntryByName return false */
    CS_AppData.ResAppTblPtr->State = CS_STATE_EMPTY;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);
    UT_SetDeferredRetcode(UT_KEY(CS_GetAppResTblEntryByName), 1, false);

    /* Execute the function being tested */
    CS_DisableNameAppCmd(&CmdPacket);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_DISABLE_APP_UNKNOWN_NAME_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.CmdErrCounter == 1, "CS_AppData.HkPacket.CmdErrCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_DisableNameAppCmd_Test_VerifyError(void)
{
    CS_AppNameCmd_t CmdPacket;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, false);

    /* Execute the function being tested */
    CS_DisableNameAppCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.CmdCounter == 0, "CS_AppData.HkPacket.CmdCounter == 0");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void CS_DisableNameAppCmd_Test_OneShot(void)
{
    CS_AppNameCmd_t CmdPacket;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);

    UT_SetDeferredRetcode(UT_KEY(CS_CheckRecomputeOneshot), 1, true);

    /* Execute the function being tested */
    CS_DisableNameAppCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.CmdCounter == 0, "CS_AppData.HkPacket.CmdCounter == 0");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void CS_EnableNameAppCmd_Test_Nominal(void)
{
    CS_AppNameCmd_t CmdPacket;
    int32           strCmpResult;
    char            ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Checksumming of app %%s is Enabled");

    strncpy(CmdPacket.Name, "App1", OS_MAX_API_NAME);
    strncpy(CS_AppData.ResAppTblPtr->Name, "App1", OS_MAX_API_NAME);
    strncpy(CS_AppData.DefAppTblPtr->Name, "App1", OS_MAX_API_NAME);

    /* Needed to make subfunction CS_GetAppDefTblEntryByName behave properly */
    CS_AppData.ResAppTblPtr->State = 1;

    /* Needed to make subfunction CS_GetAppDefTblEntryByName behave properly */
    CS_AppData.DefAppTblPtr->State = 1;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);
    UT_SetDeferredRetcode(UT_KEY(CS_CheckRecomputeOneshot), 1, false);
    UT_SetDeferredRetcode(UT_KEY(CS_GetAppResTblEntryByName), 1, true);
    UT_SetHandlerFunction(UT_KEY(CS_GetAppResTblEntryByName), CS_APP_CMDS_TEST_CS_GetAppResTblEntryByNameHandler1,
                          NULL);
    UT_SetDeferredRetcode(UT_KEY(CS_GetAppDefTblEntryByName), 1, true);
    UT_SetHandlerFunction(UT_KEY(CS_GetAppDefTblEntryByName), CS_APP_CMDS_TEST_CS_GetAppDefTblEntryByNameHandler1,
                          NULL);

    /* Execute the function being tested */
    CS_EnableNameAppCmd(&CmdPacket);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_ENABLE_APP_NAME_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.CmdCounter == 1, "CS_AppData.HkPacket.CmdCounter == 1");

    UtAssert_True(CS_AppData.HkPacket.CmdCounter == 1, "CS_AppData.HkPacket.CmdCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_EnableNameAppCmd_Test_UpdateAppsDefinitionTableError(void)
{
    CS_AppNameCmd_t CmdPacket;
    int32           strCmpResult;
    char            ExpectedEventString[2][CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString[0], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Checksumming of app %%s is Enabled");

    snprintf(ExpectedEventString[1], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS unable to update apps definition table for entry %%s");

    strncpy(CmdPacket.Name, "App1", OS_MAX_API_NAME);
    strncpy(CS_AppData.ResAppTblPtr->Name, "App1", OS_MAX_API_NAME);
    strncpy(CS_AppData.DefAppTblPtr->Name, "App1", OS_MAX_API_NAME);

    /* Needed to make subfunction CS_GetAppDefTblEntryByName behave properly */
    CS_AppData.ResAppTblPtr->State = 1;

    /* Set to make subfunction CS_GetAppDefTblEntryByName return false */
    CS_AppData.DefAppTblPtr->State = CS_STATE_EMPTY;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);
    UT_SetDeferredRetcode(UT_KEY(CS_GetAppResTblEntryByName), 1, true);
    UT_SetHandlerFunction(UT_KEY(CS_GetAppResTblEntryByName), CS_APP_CMDS_TEST_CS_GetAppResTblEntryByNameHandler1,
                          NULL);

    /* Execute the function being tested */
    CS_EnableNameAppCmd(&CmdPacket);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_ENABLE_APP_NAME_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult =
        strncmp(ExpectedEventString[0], context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, CS_ENABLE_APP_DEF_NOT_FOUND_DBG_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_DEBUG);

    strCmpResult =
        strncmp(ExpectedEventString[1], context_CFE_EVS_SendEvent[1].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[1].Spec);

    UtAssert_True(CS_AppData.HkPacket.CmdCounter == 1, "CS_AppData.HkPacket.CmdCounter == 1");

    UtAssert_True(CS_AppData.HkPacket.CmdCounter == 1, "CS_AppData.HkPacket.CmdCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 2, "CFE_EVS_SendEvent was called %u time(s), expected 2",
                  call_count_CFE_EVS_SendEvent);
}

void CS_EnableNameAppCmd_Test_UnknownNameError(void)
{
    CS_AppNameCmd_t CmdPacket;
    int32           strCmpResult;
    char            ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "App enable app command failed, app %%s not found");

    strncpy(CmdPacket.Name, "App1", OS_MAX_API_NAME);
    strncpy(CS_AppData.ResAppTblPtr->Name, "App1", OS_MAX_API_NAME);
    strncpy(CS_AppData.DefAppTblPtr->Name, "App1", OS_MAX_API_NAME);

    /* Set to make subfunction CS_GetAppResTblEntryByName return false */
    CS_AppData.ResAppTblPtr->State = CS_STATE_EMPTY;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);
    UT_SetDeferredRetcode(UT_KEY(CS_CheckRecomputeOneshot), 1, false);
    UT_SetDeferredRetcode(UT_KEY(CS_GetAppResTblEntryByName), 1, false);

    /* Execute the function being tested */
    CS_EnableNameAppCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.CmdErrCounter == 1, "CS_AppData.HkPacket.CmdErrCounter == 1");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_ENABLE_APP_UNKNOWN_NAME_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_EnableNameAppCmd_Test_VerifyError(void)
{
    CS_AppNameCmd_t CmdPacket;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, false);

    /* Execute the function being tested */
    CS_EnableNameAppCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.CmdCounter == 0, "CS_AppData.HkPacket.CmdCounter == 0");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void CS_EnableNameAppCmd_Test_OneShot(void)
{
    CS_AppNameCmd_t CmdPacket;

    UT_SetDeferredRetcode(UT_KEY(CS_VerifyCmdLength), 1, true);

    UT_SetDeferredRetcode(UT_KEY(CS_CheckRecomputeOneshot), 1, true);

    /* Execute the function being tested */
    CS_EnableNameAppCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.CmdCounter == 0, "CS_AppData.HkPacket.CmdCounter == 0");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void UtTest_Setup(void)
{
    UtTest_Add(CS_DisableAppCmd_Test, CS_Test_Setup, CS_Test_TearDown, "CS_DisableAppCmd_Test");
    UtTest_Add(CS_DisableAppCmd_Test_VerifyError, CS_Test_Setup, CS_Test_TearDown, "CS_DisableAppCmd_Test_VerifyError");
    UtTest_Add(CS_DisableAppCmd_Test_OneShot, CS_Test_Setup, CS_Test_TearDown, "CS_DisableAppCmd_Test_OneShot");

    UtTest_Add(CS_EnableAppCmd_Test, CS_Test_Setup, CS_Test_TearDown, "CS_EnableAppCmd_Test");
    UtTest_Add(CS_EnableAppCmd_Test_VerifyError, CS_Test_Setup, CS_Test_TearDown, "CS_EnableAppCmd_Test_VerifyError");
    UtTest_Add(CS_EnableAppCmd_Test_OneShot, CS_Test_Setup, CS_Test_TearDown, "CS_EnableAppCmd_Test_OneShot");

    UtTest_Add(CS_ReportBaselineAppCmd_Test_Baseline, CS_Test_Setup, CS_Test_TearDown,
               "CS_ReportBaselineAppCmd_Test_Baseline");
    UtTest_Add(CS_ReportBaselineAppCmd_Test_NoBaseline, CS_Test_Setup, CS_Test_TearDown,
               "CS_ReportBaselineAppCmd_Test_NoBaseline");
    UtTest_Add(CS_ReportBaselineAppCmd_Test_BaselineInvalidName, CS_Test_Setup, CS_Test_TearDown,
               "CS_ReportBaselineAppCmd_Test_BaselineInvalidName");
    UtTest_Add(CS_ReportBaselineAppCmd_Test_VerifyError, CS_Test_Setup, CS_Test_TearDown,
               "CS_ReportBaselineAppCmd_Test_VerifyError");
    UtTest_Add(CS_ReportBaselineAppCmd_Test_OneShot, CS_Test_Setup, CS_Test_TearDown,
               "CS_ReportBaselineAppCmd_Test_OneShot");

    UtTest_Add(CS_RecomputeBaselineAppCmd_Test_Nominal, CS_Test_Setup, CS_Test_TearDown,
               "CS_RecomputeBaselineAppCmd_Test_Nominal");
    UtTest_Add(CS_RecomputeBaselineAppCmd_Test_CreateChildTaskError, CS_Test_Setup, CS_Test_TearDown,
               "CS_RecomputeBaselineAppCmd_Test_CreateChildTaskError");
    UtTest_Add(CS_RecomputeBaselineAppCmd_Test_UnknownNameError, CS_Test_Setup, CS_Test_TearDown,
               "CS_RecomputeBaselineAppCmd_Test_UnknownNameError");
    UtTest_Add(CS_RecomputeBaselineAppCmd_Test_RecomputeInProgress, CS_Test_Setup, CS_Test_TearDown,
               "CS_RecomputeBaselineAppCmd_Test_RecomputeInProgress");
    UtTest_Add(CS_RecomputeBaselineAppCmd_Test_VerifyError, CS_Test_Setup, CS_Test_TearDown,
               "CS_RecomputeBaselineAppCmd_Test_VerifyError");

    UtTest_Add(CS_DisableNameAppCmd_Test_Nominal, CS_Test_Setup, CS_Test_TearDown, "CS_DisableNameAppCmd_Test_Nominal");
    UtTest_Add(CS_DisableNameAppCmd_Test_UpdateAppsDefinitionTableError, CS_Test_Setup, CS_Test_TearDown,
               "CS_DisableNameAppCmd_Test_UpdateAppsDefinitionTableError");
    UtTest_Add(CS_DisableNameAppCmd_Test_UnknownNameError, CS_Test_Setup, CS_Test_TearDown,
               "CS_DisableNameAppCmd_Test_UnknownNameError");
    UtTest_Add(CS_DisableNameAppCmd_Test_VerifyError, CS_Test_Setup, CS_Test_TearDown,
               "CS_DisableNameAppCmd_Test_VerifyError");
    UtTest_Add(CS_DisableNameAppCmd_Test_OneShot, CS_Test_Setup, CS_Test_TearDown, "CS_DisableNameAppCmd_Test_OneShot");

    UtTest_Add(CS_EnableNameAppCmd_Test_Nominal, CS_Test_Setup, CS_Test_TearDown, "CS_EnableNameAppCmd_Test_Nominal");
    UtTest_Add(CS_EnableNameAppCmd_Test_UpdateAppsDefinitionTableError, CS_Test_Setup, CS_Test_TearDown,
               "CS_EnableNameAppCmd_Test_UpdateAppsDefinitionTableError");
    UtTest_Add(CS_EnableNameAppCmd_Test_UnknownNameError, CS_Test_Setup, CS_Test_TearDown,
               "CS_EnableNameAppCmd_Test_UnknownNameError");
    UtTest_Add(CS_EnableNameAppCmd_Test_VerifyError, CS_Test_Setup, CS_Test_TearDown,
               "CS_EnableNameAppCmd_Test_VerifyError");
    UtTest_Add(CS_EnableNameAppCmd_Test_OneShot, CS_Test_Setup, CS_Test_TearDown, "CS_EnableNameAppCmd_Test_OneShot");
}
