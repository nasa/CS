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

#include "cs_table_cmds.h"
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

/* cs_table_cmds_tests globals */
uint8 call_count_CFE_EVS_SendEvent;

CS_Res_Tables_Table_Entry_t ResTblEntryDummy;

CS_Def_Tables_Table_Entry_t DefTblEntryDummy;
/*
 * Function Definitions
 */
void                        CS_APP_CMDS_TEST_CS_GetTableResTblEntryByNameHandler1(void                   *UserObj,
                                                                                  UT_EntryKey_t           FuncKey,
                                                                                  const UT_StubContext_t *Context)
{
    CS_Res_Tables_Table_Entry_t **EntryPtr =
        (CS_Res_Tables_Table_Entry_t **)UT_Hook_GetArgValueByName(Context, "EntryPtr", CS_Res_Tables_Table_Entry_t **);

    *EntryPtr = &ResTblEntryDummy;

    ResTblEntryDummy.ComputedYet = true;
}

void CS_APP_CMDS_TEST_CS_GetTableResTblEntryByNameHandler2(void                   *UserObj,
                                                           UT_EntryKey_t           FuncKey,
                                                           const UT_StubContext_t *Context)
{
    CS_Res_Tables_Table_Entry_t **EntryPtr =
        (CS_Res_Tables_Table_Entry_t **)UT_Hook_GetArgValueByName(Context, "EntryPtr", CS_Res_Tables_Table_Entry_t **);

    *EntryPtr = &ResTblEntryDummy;

    ResTblEntryDummy.ComputedYet = false;
}

void CS_APP_CMDS_TEST_CS_GetTableDefTblEntryByNameHandler1(void                   *UserObj,
                                                           UT_EntryKey_t           FuncKey,
                                                           const UT_StubContext_t *Context)
{
    CS_Def_Tables_Table_Entry_t **EntryPtr =
        (CS_Def_Tables_Table_Entry_t **)UT_Hook_GetArgValueByName(Context, "EntryPtr", CS_Def_Tables_Table_Entry_t **);

    *EntryPtr = &DefTblEntryDummy;
}

void CS_DisableTablesCmd_Test(void)
{
    CS_DisableTablesCmd_t CmdPacket;
    int32                 strCmpResult;
    char                  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Checksumming of Tables is Disabled");

    UT_SetDeferredRetcode(UT_KEY(CS_CheckRecomputeOneshot), 1, false);

    /* Execute the function being tested */
    CS_DisableTablesCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.Payload.TablesCSState == CS_ChecksumState_DISABLED,
                  "CS_AppData.HkPacket.Payload.TablesCSState = CS_ChecksumState_DISABLED");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_DISABLE_TABLES_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.Payload.CommandCounter == 1, "CS_AppData.HkPacket.Payload.CommandCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1,
                  "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_DisableTablesCmd_Test_OneShot(void)
{
    CS_DisableTablesCmd_t CmdPacket;

    UT_SetDeferredRetcode(UT_KEY(CS_CheckRecomputeOneshot), 1, true);

    /* Execute the function being tested */
    CS_DisableTablesCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.Payload.CommandCounter == 0, "CS_AppData.HkPacket.Payload.CommandCounter == 0");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0,
                  "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void CS_EnableTablesCmd_Test(void)
{
    CS_EnableTablesCmd_t CmdPacket;
    int32                strCmpResult;
    char                 ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Checksumming of Tables is Enabled");

    UT_SetDeferredRetcode(UT_KEY(CS_CheckRecomputeOneshot), 1, false);

    /* Execute the function being tested */
    CS_EnableTablesCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.Payload.TablesCSState == CS_ChecksumState_ENABLED,
                  "CS_AppData.HkPacket.Payload.TablesCSState = CS_ChecksumState_ENABLED");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_ENABLE_TABLES_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.Payload.CommandCounter == 1, "CS_AppData.HkPacket.Payload.CommandCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1,
                  "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_EnableTablesCmd_Test_OneShot(void)
{
    CS_EnableTablesCmd_t CmdPacket;

    UT_SetDeferredRetcode(UT_KEY(CS_CheckRecomputeOneshot), 1, true);

    /* Execute the function being tested */
    CS_EnableTablesCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.Payload.CommandCounter == 0, "CS_AppData.HkPacket.Payload.CommandCounter == 0");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0,
                  "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void CS_ReportBaselineTableCmd_Test_Computed(void)
{
    CS_ReportBaselineTableCmd_t  CmdPacket;
    int32                        strCmpResult;
    char                         ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
    CS_Res_Tables_Table_Entry_t *ResTablesTblPtr = CS_AppData.Tbl[CS_ChecksumType_TABLES_TABLE].ResAddr;

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Report baseline of table %%s is 0x%%08X");

    ResTablesTblPtr[0].ComputedYet     = true;
    ResTablesTblPtr[0].ComparisonValue = 1;

    strncpy(ResTablesTblPtr[0].Name, "name", 10);
    strncpy(CmdPacket.Payload.Name, "name", 10);

    ResTablesTblPtr[0].State = 99; /* Needed to make CS_GetTableResTblEntryByName return correct results */

    UT_SetHandlerFunction(UT_KEY(CS_GetTableResTblEntryByName),
                          CS_APP_CMDS_TEST_CS_GetTableResTblEntryByNameHandler1,
                          NULL);
    UT_SetDeferredRetcode(UT_KEY(CS_GetTableResTblEntryByName), 1, true);

    /* Execute the function being tested */
    CS_ReportBaselineTableCmd(&CmdPacket);

    /* Verify results */

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_BASELINE_TABLE_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.Payload.CommandCounter == 1, "CS_AppData.HkPacket.Payload.CommandCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1,
                  "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_ReportBaselineTableCmd_Test_NotYetComputed(void)
{
    CS_ReportBaselineTableCmd_t  CmdPacket;
    int32                        strCmpResult;
    char                         ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
    CS_Res_Tables_Table_Entry_t *ResTablesTblPtr = CS_AppData.Tbl[CS_ChecksumType_TABLES_TABLE].ResAddr;

    snprintf(ExpectedEventString,
             CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Report baseline of table %%s has not been computed yet");

    ResTablesTblPtr[0].ComputedYet = false;

    strncpy(ResTablesTblPtr[0].Name, "name", 10);
    strncpy(CmdPacket.Payload.Name, "name", 10);

    ResTablesTblPtr[0].State = 99; /* Needed to make CS_GetTableResTblEntryByName return correct results */

    UT_SetHandlerFunction(UT_KEY(CS_GetTableResTblEntryByName),
                          CS_APP_CMDS_TEST_CS_GetTableResTblEntryByNameHandler2,
                          NULL);
    UT_SetDeferredRetcode(UT_KEY(CS_GetTableResTblEntryByName), 1, true);

    /* Execute the function being tested */
    CS_ReportBaselineTableCmd(&CmdPacket);

    /* Verify results */

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_NO_BASELINE_TABLE_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.Payload.CommandCounter == 1, "CS_AppData.HkPacket.Payload.CommandCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1,
                  "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_ReportBaselineTableCmd_Test_TableNotFound(void)
{
    CS_ReportBaselineTableCmd_t  CmdPacket;
    int32                        strCmpResult;
    char                         ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
    CS_Res_Tables_Table_Entry_t *ResTablesTblPtr = CS_AppData.Tbl[CS_ChecksumType_TABLES_TABLE].ResAddr;

    snprintf(ExpectedEventString,
             CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Tables report baseline failed, table %%s not found");

    strncpy(ResTablesTblPtr[0].Name, "name1", 10);
    strncpy(CmdPacket.Payload.Name, "name2", 10);

    UT_SetDeferredRetcode(UT_KEY(CS_GetTableResTblEntryByName), 1, false);

    /* Execute the function being tested */
    CS_ReportBaselineTableCmd(&CmdPacket);

    /* Verify results */

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_BASELINE_INVALID_NAME_TABLES_ERR_EID);
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

void CS_RecomputeBaselineTableCmd_Test_Nominal(void)
{
    CS_RecomputeBaselineTableCmd_t CmdPacket;
    int32                          strCmpResult;
    char                           ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
    CS_Res_Tables_Table_Entry_t   *ResTablesTblPtr = CS_AppData.Tbl[CS_ChecksumType_TABLES_TABLE].ResAddr;

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Recompute baseline of table %%s started");

    strncpy(ResTablesTblPtr[0].Name, "name", 10);
    strncpy(CmdPacket.Payload.Name, "name", 10);

    ResTablesTblPtr[0].State = 99; /* Needed to make CS_GetTableResTblEntryByName return correct results */

    UT_SetDeferredRetcode(UT_KEY(CS_CheckRecomputeOneshot), 1, false);
    UT_SetHandlerFunction(UT_KEY(CS_GetTableResTblEntryByName),
                          CS_APP_CMDS_TEST_CS_GetTableResTblEntryByNameHandler1,
                          NULL);
    UT_SetDeferredRetcode(UT_KEY(CS_GetTableResTblEntryByName), 1, true);

    /* Execute the function being tested */
    CS_RecomputeBaselineTableCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.Payload.RecomputeInProgress == true,
                  "CS_AppData.HkPacket.Payload.RecomputeInProgress == true");
    UtAssert_True(CS_AppData.HkPacket.Payload.OneShotInProgress == false,
                  "CS_AppData.HkPacket.Payload.OneShotInProgress == false");

    UtAssert_True(CS_AppData.ChildTaskTable == CS_ChecksumType_TABLES_TABLE,
                  "CS_AppData.ChildTaskTable == CS_ChecksumType_TABLES_TABLE");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_RECOMPUTE_TABLES_STARTED_DBG_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_DEBUG);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.Payload.CommandCounter == 1, "CS_AppData.HkPacket.Payload.CommandCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1,
                  "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_RecomputeBaselineTableCmd_Test_CreateChildTaskError(void)
{
    CS_RecomputeBaselineTableCmd_t CmdPacket;
    int32                          strCmpResult;
    char                           ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
    CS_Res_Tables_Table_Entry_t   *ResTablesTblPtr = CS_AppData.Tbl[CS_ChecksumType_TABLES_TABLE].ResAddr;

    snprintf(ExpectedEventString,
             CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Recompute baseline of table %%s failed, CFE_ES_CreateChildTask returned: 0x%%08X");

    strncpy(ResTablesTblPtr[0].Name, "name", 10);
    strncpy(CmdPacket.Payload.Name, "name", 10);

    ResTablesTblPtr[0].State = 99; /* Needed to make CS_GetTableResTblEntryByName return correct results */

    /* Set to generate error message CS_RECOMPUTE_TABLES_CREATE_CHDTASK_ERR_EID */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_CreateChildTask), 1, -1);

    UT_SetDeferredRetcode(UT_KEY(CS_CheckRecomputeOneshot), 1, false);
    UT_SetHandlerFunction(UT_KEY(CS_GetTableResTblEntryByName),
                          CS_APP_CMDS_TEST_CS_GetTableResTblEntryByNameHandler1,
                          NULL);
    UT_SetDeferredRetcode(UT_KEY(CS_GetTableResTblEntryByName), 1, true);

    /* Execute the function being tested */
    CS_RecomputeBaselineTableCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.Payload.OneShotInProgress == false,
                  "CS_AppData.HkPacket.Payload.OneShotInProgress == false");

    UtAssert_True(CS_AppData.ChildTaskTable == CS_ChecksumType_TABLES_TABLE,
                  "CS_AppData.ChildTaskTable == CS_ChecksumType_TABLES_TABLE");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_RECOMPUTE_TABLES_CREATE_CHDTASK_ERR_EID);
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

void CS_RecomputeBaselineTableCmd_Test_TableNotFound(void)
{
    CS_RecomputeBaselineTableCmd_t CmdPacket;
    int32                          strCmpResult;
    char                           ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString,
             CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Tables recompute baseline failed, table %%s not found");

    strncpy(CmdPacket.Payload.Name, "name", 10);

    UT_SetDeferredRetcode(UT_KEY(CS_CheckRecomputeOneshot), 1, false);

    /* Execute the function being tested */
    CS_RecomputeBaselineTableCmd(&CmdPacket);

    /* Verify results */

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_RECOMPUTE_UNKNOWN_NAME_TABLES_ERR_EID);
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

void CS_RecomputeBaselineTableCmd_Test_RecomputeInProgress(void)
{
    CS_RecomputeBaselineTableCmd_t CmdPacket;
    int32                          strCmpResult;
    char                           ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString,
             CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Tables recompute baseline for table %%s failed: child task in use");

    strncpy(CmdPacket.Payload.Name, "name", 10);

    CS_AppData.HkPacket.Payload.RecomputeInProgress = true;

    UT_SetDeferredRetcode(UT_KEY(CS_CheckRecomputeOneshot), 1, false);

    /* Execute the function being tested */
    CS_RecomputeBaselineTableCmd(&CmdPacket);

    /* Verify results */

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_RECOMPUTE_TABLES_CHDTASK_ERR_EID);
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

void CS_RecomputeBaselineTableCmd_Test_OneShot(void)
{
    CS_RecomputeBaselineTableCmd_t CmdPacket;
    int32                          strCmpResult;
    char                           ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString,
             CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Tables recompute baseline for table %%s failed: child task in use");

    strncpy(CmdPacket.Payload.Name, "name", 10);

    CS_AppData.HkPacket.Payload.RecomputeInProgress = false;
    CS_AppData.HkPacket.Payload.OneShotInProgress   = true;

    UT_SetDeferredRetcode(UT_KEY(CS_CheckRecomputeOneshot), 1, false);

    /* Execute the function being tested */
    CS_RecomputeBaselineTableCmd(&CmdPacket);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_RECOMPUTE_TABLES_CHDTASK_ERR_EID);
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

void CS_DisableNameTableCmd_Test_Nominal(void)
{
    CS_DisableNameTableCmd_t     CmdPacket;
    int32                        strCmpResult;
    char                         ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
    CS_Def_Tables_Table_Entry_t *DefTablesTblPtr = CS_AppData.Tbl[CS_ChecksumType_TABLES_TABLE].DefAddr;
    CS_Res_Tables_Table_Entry_t *ResTablesTblPtr = CS_AppData.Tbl[CS_ChecksumType_TABLES_TABLE].ResAddr;

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Checksumming of table %%s is Disabled");

    strncpy(ResTablesTblPtr[0].Name, "name", 10);
    strncpy(CmdPacket.Payload.Name, "name", 10);

    ResTablesTblPtr[0].State = 99; /* Needed to make CS_GetTableResTblEntryByName return correct results */

    strncpy(DefTablesTblPtr[0].Name, "name", 10);

    DefTablesTblPtr[0].State = 99; /* Needed to make CS_GetTableDefTblEntryByName return correct results */

    UT_SetDeferredRetcode(UT_KEY(CS_CheckRecomputeOneshot), 1, false);
    UT_SetHandlerFunction(UT_KEY(CS_GetTableResTblEntryByName),
                          CS_APP_CMDS_TEST_CS_GetTableResTblEntryByNameHandler1,
                          NULL);
    UT_SetDeferredRetcode(UT_KEY(CS_GetTableResTblEntryByName), 1, true);
    UT_SetHandlerFunction(UT_KEY(CS_GetTableDefTblEntryByName),
                          CS_APP_CMDS_TEST_CS_GetTableDefTblEntryByNameHandler1,
                          NULL);
    UT_SetDeferredRetcode(UT_KEY(CS_GetTableDefTblEntryByName), 1, true);

    /* Execute the function being tested */
    CS_DisableNameTableCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(ResTablesTblPtr[0].TempChecksumValue == 0, "ResTablesTblPtr[0].TempChecksumValue == 0");
    UtAssert_True(ResTablesTblPtr[0].ByteOffset == 0, "ResTablesTblPtr[0].ByteOffset == 0");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_DISABLE_TABLES_NAME_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.Payload.CommandCounter == 1, "CS_AppData.HkPacket.Payload.CommandCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1,
                  "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_DisableNameTableCmd_Test_TableDefNotFound(void)
{
    CS_DisableNameTableCmd_t     CmdPacket;
    int32                        strCmpResult;
    char                         ExpectedEventString[2][CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
    CS_Res_Tables_Table_Entry_t *ResTablesTblPtr = CS_AppData.Tbl[CS_ChecksumType_TABLES_TABLE].ResAddr;

    snprintf(ExpectedEventString[0], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Checksumming of table %%s is Disabled");

    snprintf(ExpectedEventString[1],
             CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS unable to update tables definition table for entry %%s");

    strncpy(ResTablesTblPtr[0].Name, "name", 10);
    strncpy(CmdPacket.Payload.Name, "name", 10);

    ResTablesTblPtr[0].State = 99; /* Needed to make CS_GetTableResTblEntryByName return correct results */

    UT_SetDeferredRetcode(UT_KEY(CS_CheckRecomputeOneshot), 1, false);
    UT_SetHandlerFunction(UT_KEY(CS_GetTableResTblEntryByName),
                          CS_APP_CMDS_TEST_CS_GetTableResTblEntryByNameHandler1,
                          NULL);
    UT_SetDeferredRetcode(UT_KEY(CS_GetTableResTblEntryByName), 1, true);
    UT_SetDeferredRetcode(UT_KEY(CS_GetTableDefTblEntryByName), 1, false);

    /* Execute the function being tested */
    CS_DisableNameTableCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(ResTablesTblPtr[0].TempChecksumValue == 0, "ResTablesTblPtr[0].TempChecksumValue == 0");
    UtAssert_True(ResTablesTblPtr[0].ByteOffset == 0, "ResTablesTblPtr[0].ByteOffset == 0");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_DISABLE_TABLES_NAME_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult =
        strncmp(ExpectedEventString[0], context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, CS_DISABLE_TABLE_DEF_NOT_FOUND_DBG_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_DEBUG);

    strCmpResult =
        strncmp(ExpectedEventString[1], context_CFE_EVS_SendEvent[1].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[1].Spec);

    UtAssert_True(CS_AppData.HkPacket.Payload.CommandCounter == 1, "CS_AppData.HkPacket.Payload.CommandCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 2,
                  "CFE_EVS_SendEvent was called %u time(s), expected 2",
                  call_count_CFE_EVS_SendEvent);
}

void CS_DisableNameTableCmd_Test_TableNotFound(void)
{
    CS_DisableNameTableCmd_t CmdPacket;
    int32                    strCmpResult;
    char                     ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString,
             CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Tables disable table command failed, table %%s not found");

    strncpy(CmdPacket.Payload.Name, "name", 10);

    UT_SetDeferredRetcode(UT_KEY(CS_CheckRecomputeOneshot), 1, false);
    UT_SetDeferredRetcode(UT_KEY(CS_GetTableResTblEntryByName), 1, false);

    /* Execute the function being tested */
    CS_DisableNameTableCmd(&CmdPacket);

    /* Verify results */

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_DISABLE_TABLES_UNKNOWN_NAME_ERR_EID);
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

void CS_DisableNameTableCmd_Test_OneShot(void)
{
    CS_DisableNameTableCmd_t CmdPacket;

    UT_SetDeferredRetcode(UT_KEY(CS_CheckRecomputeOneshot), 1, true);

    /* Execute the function being tested */
    CS_DisableNameTableCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.Payload.CommandCounter == 0, "CS_AppData.HkPacket.Payload.CommandCounter == 0");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0,
                  "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void CS_EnableNameTableCmd_Test_Nominal(void)
{
    CS_EnableNameTableCmd_t      CmdPacket;
    int32                        strCmpResult;
    char                         ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
    CS_Def_Tables_Table_Entry_t *DefTablesTblPtr = CS_AppData.Tbl[CS_ChecksumType_TABLES_TABLE].DefAddr;
    CS_Res_Tables_Table_Entry_t *ResTablesTblPtr = CS_AppData.Tbl[CS_ChecksumType_TABLES_TABLE].ResAddr;

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Checksumming of table %%s is Enabled");

    strncpy(ResTablesTblPtr[0].Name, "name", 10);
    strncpy(CmdPacket.Payload.Name, "name", 10);

    ResTablesTblPtr[0].State = 99; /* Needed to make CS_GetTableResTblEntryByName return correct results */

    strncpy(DefTablesTblPtr[0].Name, "name", 10);

    DefTablesTblPtr[0].State = 99; /* Needed to make CS_GetTableDefTblEntryByName return correct results */

    UT_SetDeferredRetcode(UT_KEY(CS_CheckRecomputeOneshot), 1, false);
    UT_SetHandlerFunction(UT_KEY(CS_GetTableResTblEntryByName),
                          CS_APP_CMDS_TEST_CS_GetTableResTblEntryByNameHandler1,
                          NULL);
    UT_SetDeferredRetcode(UT_KEY(CS_GetTableResTblEntryByName), 1, true);
    UT_SetHandlerFunction(UT_KEY(CS_GetTableDefTblEntryByName),
                          CS_APP_CMDS_TEST_CS_GetTableDefTblEntryByNameHandler1,
                          NULL);
    UT_SetDeferredRetcode(UT_KEY(CS_GetTableDefTblEntryByName), 1, true);

    /* Execute the function being tested */
    CS_EnableNameTableCmd(&CmdPacket);

    /* Verify results */

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_ENABLE_TABLES_NAME_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(CS_AppData.HkPacket.Payload.CommandCounter == 1, "CS_AppData.HkPacket.Payload.CommandCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1,
                  "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void CS_EnableNameTableCmd_Test_TableDefNotFound(void)
{
    CS_EnableNameTableCmd_t      CmdPacket;
    int32                        strCmpResult;
    char                         ExpectedEventString[2][CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
    CS_Res_Tables_Table_Entry_t *ResTablesTblPtr = CS_AppData.Tbl[CS_ChecksumType_TABLES_TABLE].ResAddr;

    snprintf(ExpectedEventString[0], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Checksumming of table %%s is Enabled");

    snprintf(ExpectedEventString[1],
             CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS unable to update tables definition table for entry %%s");

    strncpy(ResTablesTblPtr[0].Name, "name", 10);
    strncpy(CmdPacket.Payload.Name, "name", 10);

    ResTablesTblPtr[0].State = 99; /* Needed to make CS_GetTableResTblEntryByName return correct results */

    UT_SetDeferredRetcode(UT_KEY(CS_CheckRecomputeOneshot), 1, false);
    UT_SetHandlerFunction(UT_KEY(CS_GetTableResTblEntryByName),
                          CS_APP_CMDS_TEST_CS_GetTableResTblEntryByNameHandler1,
                          NULL);
    UT_SetDeferredRetcode(UT_KEY(CS_GetTableResTblEntryByName), 1, true);
    UT_SetDeferredRetcode(UT_KEY(CS_GetTableDefTblEntryByName), 1, false);

    /* Execute the function being tested */
    CS_EnableNameTableCmd(&CmdPacket);

    /* Verify results */

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_ENABLE_TABLES_NAME_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult =
        strncmp(ExpectedEventString[0], context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, CS_ENABLE_TABLE_DEF_NOT_FOUND_DBG_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_DEBUG);

    strCmpResult =
        strncmp(ExpectedEventString[1], context_CFE_EVS_SendEvent[1].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[1].Spec);

    UtAssert_True(CS_AppData.HkPacket.Payload.CommandCounter == 1, "CS_AppData.HkPacket.Payload.CommandCounter == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 2,
                  "CFE_EVS_SendEvent was called %u time(s), expected 2",
                  call_count_CFE_EVS_SendEvent);
}

void CS_EnableNameTableCmd_Test_TableNotFound(void)
{
    CS_EnableNameTableCmd_t CmdPacket;
    int32                   strCmpResult;
    char                    ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString,
             CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Tables enable table command failed, table %%s not found");

    strncpy(CmdPacket.Payload.Name, "name", 10);

    UT_SetDeferredRetcode(UT_KEY(CS_CheckRecomputeOneshot), 1, false);
    UT_SetDeferredRetcode(UT_KEY(CS_GetTableResTblEntryByName), 1, false);

    /* Execute the function being tested */
    CS_EnableNameTableCmd(&CmdPacket);

    /* Verify results */

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_ENABLE_TABLES_UNKNOWN_NAME_ERR_EID);
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

void CS_EnableNameTableCmd_Test_OneShot(void)
{
    CS_EnableNameTableCmd_t CmdPacket;

    UT_SetDeferredRetcode(UT_KEY(CS_CheckRecomputeOneshot), 1, true);

    /* Execute the function being tested */
    CS_EnableNameTableCmd(&CmdPacket);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.Payload.CommandCounter == 0, "CS_AppData.HkPacket.Payload.CommandCounter == 0");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0,
                  "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void UtTest_Setup(void)
{
    UtTest_Add(CS_DisableTablesCmd_Test, CS_Test_Setup, CS_Test_TearDown, "CS_DisableTablesCmd_Test");
    UtTest_Add(CS_DisableTablesCmd_Test_OneShot, CS_Test_Setup, CS_Test_TearDown, "CS_DisableTablesCmd_Test_OneShot");

    UtTest_Add(CS_EnableTablesCmd_Test, CS_Test_Setup, CS_Test_TearDown, "CS_EnableTablesCmd_Test");
    UtTest_Add(CS_EnableTablesCmd_Test_OneShot, CS_Test_Setup, CS_Test_TearDown, "CS_EnableTablesCmd_Test_OneShot");

    UtTest_Add(CS_ReportBaselineTableCmd_Test_Computed,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ReportBaselineTableCmd_Test_Computed");
    UtTest_Add(CS_ReportBaselineTableCmd_Test_NotYetComputed,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ReportBaselineTableCmd_Test_NotYetComputed");
    UtTest_Add(CS_ReportBaselineTableCmd_Test_TableNotFound,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ReportBaselineTableCmd_Test_TableNotFound");

    UtTest_Add(CS_RecomputeBaselineTableCmd_Test_Nominal,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_RecomputeBaselineTableCmd_Test_Nominal");
    UtTest_Add(CS_RecomputeBaselineTableCmd_Test_CreateChildTaskError,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_RecomputeBaselineTableCmd_Test_CreateChildTaskError");
    UtTest_Add(CS_RecomputeBaselineTableCmd_Test_TableNotFound,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_RecomputeBaselineTableCmd_Test_TableNotFound");
    UtTest_Add(CS_RecomputeBaselineTableCmd_Test_RecomputeInProgress,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_RecomputeBaselineTableCmd_Test_RecomputeInProgress");
    UtTest_Add(CS_RecomputeBaselineTableCmd_Test_OneShot,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_RecomputeBaselineTableCmd_Test_OneShot");

    UtTest_Add(CS_DisableNameTableCmd_Test_Nominal,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_DisableNameTableCmd_Test_Nominal");
    UtTest_Add(CS_DisableNameTableCmd_Test_TableDefNotFound,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_DisableNameTableCmd_Test_TableDefNotFound");
    UtTest_Add(CS_DisableNameTableCmd_Test_TableNotFound,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_DisableNameTableCmd_Test_TableNotFound");
    UtTest_Add(CS_DisableNameTableCmd_Test_OneShot,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_DisableNameTableCmd_Test_OneShot");

    UtTest_Add(CS_EnableNameTableCmd_Test_Nominal,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_EnableNameTableCmd_Test_Nominal");
    UtTest_Add(CS_EnableNameTableCmd_Test_TableDefNotFound,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_EnableNameTableCmd_Test_TableDefNotFound");
    UtTest_Add(CS_EnableNameTableCmd_Test_TableNotFound,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_EnableNameTableCmd_Test_TableNotFound");
    UtTest_Add(CS_EnableNameTableCmd_Test_OneShot,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_EnableNameTableCmd_Test_OneShot");
}
