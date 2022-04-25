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

/* cs_table_processing_tests globals */
uint8 call_count_CFE_EVS_SendEvent;

/*
 * Function Definitions
 */

void CS_TABLE_PROCESSING_TEST_CFE_ES_GetAppNameHandler1(void *UserObj, UT_EntryKey_t FuncKey,
                                                        const UT_StubContext_t *Context)
{
    char *AppName = (char *)UT_Hook_GetArgValueByName(Context, "AppName", char *);

    strncpy((char *)AppName, "CS", 3);
}

int32 CS_TABLE_PROCESSING_TEST_CFE_TBL_GetAddressHook(void *UserObj, int32 StubRetcode, uint32 CallCount,
                                                      const UT_StubContext_t *Context)
{
    return CFE_SUCCESS;
}

int32 CS_TABLE_PROCESSING_TEST_CFE_TBL_LoadHook(void *UserObj, int32 StubRetcode, uint32 CallCount,
                                                const UT_StubContext_t *Context)
{
    return CFE_SUCCESS;
}

void CS_ValidateEepromChecksumDefinitionTable_Test_Nominal(void)
{
    int32 Result;
    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS Eeprom Table verification results: good = %%d, bad = %%d, unused = %%d");

    CS_AppData.DefEepromTblPtr[0].State = CS_STATE_ENABLED; /* All other states are empty by default, and so this test
                                                               also covers CS_STATE_EMPTY branch */

    /* Execute the function being tested */
    Result = CS_ValidateEepromChecksumDefinitionTable(CS_AppData.DefEepromTblPtr);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_VAL_EEPROM_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_ValidateEepromChecksumDefinitionTable_Test_Nominal */

void CS_ValidateEepromChecksumDefinitionTable_Test_IllegalChecksumRangeStateEnabled(void)
{
    int32 Result;
    int32 strCmpResult;
    char  ExpectedEventString[2][CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString[0], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Eeprom Table Validate: Illegal checksum range found in Entry ID %%d, CFE_PSP_MemValidateRange returned: "
             "0x%%08X");

    snprintf(ExpectedEventString[1], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS Eeprom Table verification results: good = %%d, bad = %%d, unused = %%d");

    CS_AppData.DefEepromTblPtr[0].State = CS_STATE_ENABLED;

    /* Set to generate error message CS_VAL_EEPROM_RANGE_ERR_EID */
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_MemValidateRange), 1, -1);

    /* Execute the function being tested */
    Result = CS_ValidateEepromChecksumDefinitionTable(CS_AppData.DefEepromTblPtr);

    /* Verify results */

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_VAL_EEPROM_RANGE_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult =
        strncmp(ExpectedEventString[0], context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, CS_VAL_EEPROM_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult =
        strncmp(ExpectedEventString[1], context_CFE_EVS_SendEvent[1].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[1].Spec);

    UtAssert_True(Result == CS_TABLE_ERROR, "Result == CS_TABLE_ERROR");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 2, "CFE_EVS_SendEvent was called %u time(s), expected 2",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_ValidateEepromChecksumDefinitionTable_Test_IllegalChecksumRangeStateEnabled */

void CS_ValidateEepromChecksumDefinitionTable_Test_IllegalChecksumRangeStateDisabled(void)
{
    int32 Result;
    int32 strCmpResult;
    char  ExpectedEventString[2][CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString[0], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Eeprom Table Validate: Illegal checksum range found in Entry ID %%d, CFE_PSP_MemValidateRange returned: "
             "0x%%08X");

    snprintf(ExpectedEventString[1], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS Eeprom Table verification results: good = %%d, bad = %%d, unused = %%d");

    CS_AppData.DefEepromTblPtr[0].State = CS_STATE_DISABLED;

    /* Set to generate error message CS_VAL_EEPROM_RANGE_ERR_EID */
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_MemValidateRange), 1, -1);

    /* Execute the function being tested */
    Result = CS_ValidateEepromChecksumDefinitionTable(CS_AppData.DefEepromTblPtr);

    /* Verify results */

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_VAL_EEPROM_RANGE_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult =
        strncmp(ExpectedEventString[0], context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, CS_VAL_EEPROM_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult =
        strncmp(ExpectedEventString[1], context_CFE_EVS_SendEvent[1].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[1].Spec);

    UtAssert_True(Result == CS_TABLE_ERROR, "Result == CS_TABLE_ERROR");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 2, "CFE_EVS_SendEvent was called %u time(s), expected 2",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_ValidateEepromChecksumDefinitionTable_Test_IllegalChecksumRangeStateDisabled */

void CS_ValidateEepromChecksumDefinitionTable_Test_IllegalStateField(void)
{
    int32 Result;
    int32 strCmpResult;
    char  ExpectedEventString[2][CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString[0], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Eeprom Table Validate: Illegal State Field (0x%%04X) found in Entry ID %%d");

    snprintf(ExpectedEventString[1], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS Eeprom Table verification results: good = %%d, bad = %%d, unused = %%d");

    CS_AppData.DefEepromTblPtr[0].State = 0xFFFF;

    /* Execute the function being tested */
    Result = CS_ValidateEepromChecksumDefinitionTable(CS_AppData.DefEepromTblPtr);

    /* Verify results */

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_VAL_EEPROM_STATE_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult =
        strncmp(ExpectedEventString[0], context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, CS_VAL_EEPROM_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult =
        strncmp(ExpectedEventString[1], context_CFE_EVS_SendEvent[1].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[1].Spec);

    UtAssert_True(Result == CS_TABLE_ERROR, "Result == CS_TABLE_ERROR");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 2, "CFE_EVS_SendEvent was called %u time(s), expected 2",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_ValidateEepromChecksumDefinitionTable_Test_IllegalStateField */

void CS_ValidateEepromChecksumDefinitionTable_Test_TableErrorResult(void)
{
    int32 Result;
    int32 strCmpResult;
    char  ExpectedEventString[2][CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString[0], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Eeprom Table Validate: Illegal checksum range found in Entry ID %%d, CFE_PSP_MemValidateRange returned: "
             "0x%%08X");

    snprintf(ExpectedEventString[1], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS Eeprom Table verification results: good = %%d, bad = %%d, unused = %%d");

    CS_AppData.DefEepromTblPtr[0].State = CS_STATE_DISABLED;
    CS_AppData.DefEepromTblPtr[1].State = CS_STATE_DISABLED;

    /* Set to generate error message CS_VAL_EEPROM_RANGE_ERR_EID */
    UT_SetDefaultReturnValue(UT_KEY(CFE_PSP_MemValidateRange), -1);

    /* Execute the function being tested */
    Result = CS_ValidateEepromChecksumDefinitionTable(CS_AppData.DefEepromTblPtr);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_VAL_EEPROM_RANGE_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult =
        strncmp(ExpectedEventString[0], context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, CS_VAL_EEPROM_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult =
        strncmp(ExpectedEventString[1], context_CFE_EVS_SendEvent[1].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[1].Spec);

    UtAssert_True(Result == CS_TABLE_ERROR, "Result == CS_TABLE_ERROR");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 2, "CFE_EVS_SendEvent was called %u time(s), expected 2",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_ValidateEepromChecksumDefinitionTable_Test_TableErrorResult */

void CS_ValidateEepromChecksumDefinitionTable_Test_UndefTableErrorResult(void)
{
    int32 Result;
    int32 strCmpResult;
    char  ExpectedEventString[2][CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString[0], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Eeprom Table Validate: Illegal checksum range found in Entry ID %%d, CFE_PSP_MemValidateRange returned: "
             "0x%%08X");

    snprintf(ExpectedEventString[1], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS Eeprom Table verification results: good = %%d, bad = %%d, unused = %%d");

    CS_AppData.DefEepromTblPtr[0].State = CS_STATE_DISABLED;
    CS_AppData.DefEepromTblPtr[1].State = CS_STATE_UNDEFINED;

    /* Set to generate error message CS_VAL_EEPROM_RANGE_ERR_EID */
    UT_SetDefaultReturnValue(UT_KEY(CFE_PSP_MemValidateRange), -1);

    /* Execute the function being tested */
    Result = CS_ValidateEepromChecksumDefinitionTable(CS_AppData.DefEepromTblPtr);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_VAL_EEPROM_RANGE_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult =
        strncmp(ExpectedEventString[0], context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, CS_VAL_EEPROM_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult =
        strncmp(ExpectedEventString[1], context_CFE_EVS_SendEvent[1].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[1].Spec);

    UtAssert_True(Result == CS_TABLE_ERROR, "Result == CS_TABLE_ERROR");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 2, "CFE_EVS_SendEvent was called %u time(s), expected 2",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_ValidateEepromChecksumDefinitionTable_Test_UndefTableErrorResult */

void CS_ValidateMemoryChecksumDefinitionTable_Test_Nominal(void)
{
    int32 Result;
    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS Memory Table verification results: good = %%d, bad = %%d, unused = %%d");

    CS_AppData.DefMemoryTblPtr[0].State = CS_STATE_ENABLED; /* All other states are empty by default, and so this test
                                                               also covers CS_STATE_EMPTY branch */

    /* Execute the function being tested */
    Result = CS_ValidateMemoryChecksumDefinitionTable(CS_AppData.DefMemoryTblPtr);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_VAL_MEMORY_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_ValidateMemoryChecksumDefinitionTable_Test_Nominal */

void CS_ValidateMemoryChecksumDefinitionTable_Test_IllegalChecksumRangeStateEnabled(void)
{
    int32 Result;
    int32 strCmpResult;
    char  ExpectedEventString[2][CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString[0], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Memory Table Validate: Illegal checksum range found in Entry ID %%d, CFE_PSP_MemValidateRange returned: "
             "0x%%08X");

    snprintf(ExpectedEventString[1], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS Memory Table verification results: good = %%d, bad = %%d, unused = %%d");

    CS_AppData.DefMemoryTblPtr[0].State = CS_STATE_ENABLED;

    /* Set to generate error message CS_VAL_MEMORY_RANGE_ERR_EID */
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_MemValidateRange), 1, -1);

    /* Execute the function being tested */
    Result = CS_ValidateMemoryChecksumDefinitionTable(CS_AppData.DefMemoryTblPtr);

    /* Verify results */

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_VAL_MEMORY_RANGE_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult =
        strncmp(ExpectedEventString[0], context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, CS_VAL_MEMORY_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult =
        strncmp(ExpectedEventString[1], context_CFE_EVS_SendEvent[1].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[1].Spec);

    UtAssert_True(Result == CS_TABLE_ERROR, "Result == CS_TABLE_ERROR");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 2, "CFE_EVS_SendEvent was called %u time(s), expected 2",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_ValidateMemoryChecksumDefinitionTable_Test_IllegalChecksumRangeStateEnabled */

void CS_ValidateMemoryChecksumDefinitionTable_Test_IllegalChecksumRangeStateDisabled(void)
{
    int32 Result;
    int32 strCmpResult;
    char  ExpectedEventString[2][CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString[0], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Memory Table Validate: Illegal checksum range found in Entry ID %%d, CFE_PSP_MemValidateRange returned: "
             "0x%%08X");

    snprintf(ExpectedEventString[1], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS Memory Table verification results: good = %%d, bad = %%d, unused = %%d");

    CS_AppData.DefMemoryTblPtr[0].State = CS_STATE_DISABLED;

    /* Set to generate error message CS_VAL_MEMORY_RANGE_ERR_EID */
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_MemValidateRange), 1, -1);

    /* Execute the function being tested */
    Result = CS_ValidateMemoryChecksumDefinitionTable(CS_AppData.DefMemoryTblPtr);

    /* Verify results */

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_VAL_MEMORY_RANGE_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult =
        strncmp(ExpectedEventString[0], context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, CS_VAL_MEMORY_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult =
        strncmp(ExpectedEventString[1], context_CFE_EVS_SendEvent[1].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[1].Spec);

    UtAssert_True(Result == CS_TABLE_ERROR, "Result == CS_TABLE_ERROR");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 2, "CFE_EVS_SendEvent was called %u time(s), expected 2",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_ValidateMemoryChecksumDefinitionTable_Test_IllegalChecksumRangeStateDisabled */

void CS_ValidateMemoryChecksumDefinitionTable_Test_IllegalStateField(void)
{
    int32 Result;
    int32 strCmpResult;
    char  ExpectedEventString[2][CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString[0], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Memory Table Validate: Illegal State Field (0x%%04X) found in Entry ID %%d");

    snprintf(ExpectedEventString[1], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS Memory Table verification results: good = %%d, bad = %%d, unused = %%d");

    CS_AppData.DefMemoryTblPtr[0].State = 0xFFFF;

    /* Execute the function being tested */
    Result = CS_ValidateMemoryChecksumDefinitionTable(CS_AppData.DefMemoryTblPtr);

    /* Verify results */

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_VAL_MEMORY_STATE_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult =
        strncmp(ExpectedEventString[0], context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, CS_VAL_MEMORY_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult =
        strncmp(ExpectedEventString[1], context_CFE_EVS_SendEvent[1].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[1].Spec);

    UtAssert_True(Result == CS_TABLE_ERROR, "Result == CS_TABLE_ERROR");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 2, "CFE_EVS_SendEvent was called %u time(s), expected 2",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_ValidateMemoryChecksumDefinitionTable_Test_IllegalStateField */

void CS_ValidateMemoryChecksumDefinitionTable_Test_TableErrorResult(void)
{
    int32 Result;
    int32 strCmpResult;
    char  ExpectedEventString[2][CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString[0], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Memory Table Validate: Illegal checksum range found in Entry ID %%d, CFE_PSP_MemValidateRange returned: "
             "0x%%08X");

    snprintf(ExpectedEventString[1], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS Memory Table verification results: good = %%d, bad = %%d, unused = %%d");

    CS_AppData.DefMemoryTblPtr[0].State = CS_STATE_DISABLED;
    CS_AppData.DefMemoryTblPtr[1].State = CS_STATE_DISABLED;

    /* Set to generate error message CS_VAL_MEMORY_RANGE_ERR_EID */
    UT_SetDefaultReturnValue(UT_KEY(CFE_PSP_MemValidateRange), -1);

    /* Execute the function being tested */
    Result = CS_ValidateMemoryChecksumDefinitionTable(CS_AppData.DefMemoryTblPtr);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_VAL_MEMORY_RANGE_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult =
        strncmp(ExpectedEventString[0], context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, CS_VAL_MEMORY_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult =
        strncmp(ExpectedEventString[1], context_CFE_EVS_SendEvent[1].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[1].Spec);

    UtAssert_True(Result == CS_TABLE_ERROR, "Result == CS_TABLE_ERROR");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 2, "CFE_EVS_SendEvent was called %u time(s), expected 2",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_ValidateMemoryChecksumDefinitionTable_Test_TableErrorResult */

void CS_ValidateMemoryChecksumDefinitionTable_Test_UndefTableErrorResult(void)
{
    int32 Result;
    int32 strCmpResult;
    char  ExpectedEventString[2][CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString[0], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Memory Table Validate: Illegal checksum range found in Entry ID %%d, CFE_PSP_MemValidateRange returned: "
             "0x%%08X");

    snprintf(ExpectedEventString[1], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS Memory Table verification results: good = %%d, bad = %%d, unused = %%d");

    CS_AppData.DefMemoryTblPtr[0].State = CS_STATE_DISABLED;
    CS_AppData.DefMemoryTblPtr[1].State = CS_STATE_UNDEFINED;

    /* Set to generate error message CS_VAL_MEMORY_RANGE_ERR_EID */
    UT_SetDefaultReturnValue(UT_KEY(CFE_PSP_MemValidateRange), -1);

    /* Execute the function being tested */
    Result = CS_ValidateMemoryChecksumDefinitionTable(CS_AppData.DefMemoryTblPtr);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_VAL_MEMORY_RANGE_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult =
        strncmp(ExpectedEventString[0], context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, CS_VAL_MEMORY_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult =
        strncmp(ExpectedEventString[1], context_CFE_EVS_SendEvent[1].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[1].Spec);

    UtAssert_True(Result == CS_TABLE_ERROR, "Result == CS_TABLE_ERROR");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 2, "CFE_EVS_SendEvent was called %u time(s), expected 2",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_ValidateMemoryChecksumDefinitionTable_Test_UndefTableErrorResult */

void CS_ValidateTablesChecksumDefinitionTable_Test_Nominal(void)
{
    int32 Result;
    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS Tables Table verification results: good = %%d, bad = %%d, unused = %%d");

    CS_AppData.DefTablesTblPtr[0].State = CS_STATE_ENABLED; /* All other states are empty by default, and so this test
                                                               also covers CS_STATE_EMPTY branch */

    strncpy(CS_AppData.DefTablesTblPtr[0].Name, "name", 10);

    /* Execute the function being tested */
    Result = CS_ValidateTablesChecksumDefinitionTable(CS_AppData.DefTablesTblPtr);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_VAL_TABLES_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_ValidateTablesChecksumDefinitionTable_Test_Nominal */

void CS_ValidateTablesChecksumDefinitionTable_Test_DuplicateNameStateEmpty(void)
{
    int32 Result;
    int32 strCmpResult;
    char  ExpectedEventString[2][CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString[0], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS Tables Table Validate: Duplicate Name (%%s) found at entries %%d and %%d");

    snprintf(ExpectedEventString[1], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS Tables Table verification results: good = %%d, bad = %%d, unused = %%d");

    /* All states are empty by default */

    strncpy(CS_AppData.DefTablesTblPtr[0].Name, "name", 10);
    strncpy(CS_AppData.DefTablesTblPtr[1].Name, "name", 10);

    /* Execute the function being tested */
    Result = CS_ValidateTablesChecksumDefinitionTable(CS_AppData.DefTablesTblPtr);

    /* Verify results */

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_VAL_TABLES_DEF_TBL_DUPL_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult =
        strncmp(ExpectedEventString[0], context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, CS_VAL_TABLES_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult =
        strncmp(ExpectedEventString[1], context_CFE_EVS_SendEvent[1].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[1].Spec);

    UtAssert_True(Result == CS_TABLE_ERROR, "Result == CS_TABLE_ERROR");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 2, "CFE_EVS_SendEvent was called %u time(s), expected 2",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_ValidateTablesChecksumDefinitionTable_Test_DuplicateNameStateEmpty */

void CS_ValidateTablesChecksumDefinitionTable_Test_DuplicateNameStateEnabled(void)
{
    int32 Result;
    int32 strCmpResult;
    char  ExpectedEventString[2][CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString[0], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS Tables Table Validate: Duplicate Name (%%s) found at entries %%d and %%d");

    snprintf(ExpectedEventString[1], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS Tables Table verification results: good = %%d, bad = %%d, unused = %%d");

    CS_AppData.DefTablesTblPtr[0].State = CS_STATE_ENABLED; /* All other states are empty by default, and so this test
                                                               also covers CS_STATE_EMPTY branch */
    CS_AppData.DefTablesTblPtr[1].State = CS_STATE_ENABLED; /* All other states are empty by default, and so this test
                                                               also covers CS_STATE_EMPTY branch */

    strncpy(CS_AppData.DefTablesTblPtr[0].Name, "name", 10);
    strncpy(CS_AppData.DefTablesTblPtr[1].Name, "name", 10);

    /* Execute the function being tested */
    Result = CS_ValidateTablesChecksumDefinitionTable(CS_AppData.DefTablesTblPtr);

    /* Verify results */

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_VAL_TABLES_DEF_TBL_DUPL_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult =
        strncmp(ExpectedEventString[0], context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, CS_VAL_TABLES_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult =
        strncmp(ExpectedEventString[1], context_CFE_EVS_SendEvent[1].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[1].Spec);

    UtAssert_True(Result == CS_TABLE_ERROR, "Result == CS_TABLE_ERROR");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 2, "CFE_EVS_SendEvent was called %u time(s), expected 2",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_ValidateTablesChecksumDefinitionTable_Test_DuplicateNameStateEnabled */

void CS_ValidateTablesChecksumDefinitionTable_Test_DuplicateNameStateDisabled(void)
{
    int32 Result;
    int32 strCmpResult;
    char  ExpectedEventString[2][CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString[0], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS Tables Table Validate: Duplicate Name (%%s) found at entries %%d and %%d");

    snprintf(ExpectedEventString[1], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS Tables Table verification results: good = %%d, bad = %%d, unused = %%d");

    CS_AppData.DefTablesTblPtr[0].State = CS_STATE_DISABLED; /* All other states are empty by default, and so this test
                                                                also covers CS_STATE_EMPTY branch */
    CS_AppData.DefTablesTblPtr[1].State = CS_STATE_DISABLED; /* All other states are empty by default, and so this test
                                                                also covers CS_STATE_EMPTY branch */

    strncpy(CS_AppData.DefTablesTblPtr[0].Name, "name", 10);
    strncpy(CS_AppData.DefTablesTblPtr[1].Name, "name", 10);

    /* Execute the function being tested */
    Result = CS_ValidateTablesChecksumDefinitionTable(CS_AppData.DefTablesTblPtr);

    /* Verify results */

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_VAL_TABLES_DEF_TBL_DUPL_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult =
        strncmp(ExpectedEventString[0], context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, CS_VAL_TABLES_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult =
        strncmp(ExpectedEventString[1], context_CFE_EVS_SendEvent[1].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[1].Spec);

    UtAssert_True(Result == CS_TABLE_ERROR, "Result == CS_TABLE_ERROR");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 2, "CFE_EVS_SendEvent was called %u time(s), expected 2",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_ValidateTablesChecksumDefinitionTable_Test_DuplicateNameStateDisabled */

void CS_ValidateTablesChecksumDefinitionTable_Test_IllegalStateField(void)
{
    int32 Result;
    int32 strCmpResult;
    char  ExpectedEventString[2][CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString[0], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS Tables Table Validate: Illegal State Field (0x%%04X) found with name %%s");

    snprintf(ExpectedEventString[1], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS Tables Table verification results: good = %%d, bad = %%d, unused = %%d");

    CS_AppData.DefTablesTblPtr[0].State = 0xFFFF;

    strncpy(CS_AppData.DefTablesTblPtr[0].Name, "name", 10);

    /* Execute the function being tested */
    Result = CS_ValidateTablesChecksumDefinitionTable(CS_AppData.DefTablesTblPtr);

    /* Verify results */

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_VAL_TABLES_STATE_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult =
        strncmp(ExpectedEventString[0], context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, CS_VAL_TABLES_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult =
        strncmp(ExpectedEventString[1], context_CFE_EVS_SendEvent[1].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[1].Spec);

    UtAssert_True(Result == CS_TABLE_ERROR, "Result == CS_TABLE_ERROR");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 2, "CFE_EVS_SendEvent was called %u time(s), expected 2",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_ValidateTablesChecksumDefinitionTable_Test_IllegalStateField */

void CS_ValidateTablesChecksumDefinitionTable_Test_IllegalStateEmptyName(void)
{
    int32 Result;
    int32 strCmpResult;
    char  ExpectedEventString[2][CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString[0], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS Tables Table Validate: Illegal State (0x%%04X) with empty name at entry %%d");

    snprintf(ExpectedEventString[1], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS Tables Table verification results: good = %%d, bad = %%d, unused = %%d");

    CS_AppData.DefTablesTblPtr[0].State = CS_STATE_ENABLED;

    /* Execute the function being tested */
    Result = CS_ValidateTablesChecksumDefinitionTable(CS_AppData.DefTablesTblPtr);

    /* Verify results */

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_VAL_TABLES_DEF_TBL_ZERO_NAME_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult =
        strncmp(ExpectedEventString[0], context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, CS_VAL_TABLES_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult =
        strncmp(ExpectedEventString[1], context_CFE_EVS_SendEvent[1].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[1].Spec);

    UtAssert_True(Result == CS_TABLE_ERROR, "Result == CS_TABLE_ERROR");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 2, "CFE_EVS_SendEvent was called %u time(s), expected 2",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_ValidateTablesChecksumDefinitionTable_Test_IllegalStateEmptyName */

void CS_ValidateTablesChecksumDefinitionTable_Test_TableErrorResult(void)
{
    int32 Result;
    int32 strCmpResult;
    char  ExpectedEventString[2][CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString[0], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS Tables Table Validate: Duplicate Name (%%s) found at entries %%d and %%d");

    snprintf(ExpectedEventString[1], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS Tables Table verification results: good = %%d, bad = %%d, unused = %%d");

    /* All states are empty by default */
    CS_AppData.DefTablesTblPtr[0].State = CS_STATE_DISABLED;
    CS_AppData.DefTablesTblPtr[1].State = CS_STATE_DISABLED;
    CS_AppData.DefTablesTblPtr[2].State = CS_STATE_DISABLED;

    strncpy(CS_AppData.DefTablesTblPtr[0].Name, "name", 10);
    strncpy(CS_AppData.DefTablesTblPtr[1].Name, "name", 10);
    strncpy(CS_AppData.DefTablesTblPtr[2].Name, "name", 10);

    /* Execute the function being tested */
    Result = CS_ValidateTablesChecksumDefinitionTable(CS_AppData.DefTablesTblPtr);

    /* Verify results */

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_VAL_TABLES_DEF_TBL_DUPL_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult =
        strncmp(ExpectedEventString[0], context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, CS_VAL_TABLES_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult =
        strncmp(ExpectedEventString[1], context_CFE_EVS_SendEvent[1].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[1].Spec);

    UtAssert_True(Result == CS_TABLE_ERROR, "Result == CS_TABLE_ERROR");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 2, "CFE_EVS_SendEvent was called %u time(s), expected 2",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_ValidateTablesChecksumDefinitionTable_Test_TableErrorResult */

void CS_ValidateTablesChecksumDefinitionTable_Test_UndefTableErrorResult(void)
{
    int32 Result;
    int32 strCmpResult;
    char  ExpectedEventString[2][CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString[0], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS Tables Table Validate: Duplicate Name (%%s) found at entries %%d and %%d");

    snprintf(ExpectedEventString[1], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS Tables Table verification results: good = %%d, bad = %%d, unused = %%d");

    CS_AppData.DefTablesTblPtr[0].State = CS_STATE_DISABLED;
    CS_AppData.DefTablesTblPtr[1].State = CS_STATE_DISABLED;
    CS_AppData.DefTablesTblPtr[2].State = CS_STATE_UNDEFINED;

    strncpy(CS_AppData.DefTablesTblPtr[0].Name, "name", 10);
    strncpy(CS_AppData.DefTablesTblPtr[1].Name, "name", 10);
    strncpy(CS_AppData.DefTablesTblPtr[2].Name, "name", 10);

    /* Execute the function being tested */
    Result = CS_ValidateTablesChecksumDefinitionTable(CS_AppData.DefTablesTblPtr);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_VAL_TABLES_DEF_TBL_DUPL_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult =
        strncmp(ExpectedEventString[0], context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, CS_VAL_TABLES_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult =
        strncmp(ExpectedEventString[1], context_CFE_EVS_SendEvent[1].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[1].Spec);

    UtAssert_True(Result == CS_TABLE_ERROR, "Result == CS_TABLE_ERROR");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 2, "CFE_EVS_SendEvent was called %u time(s), expected 2",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_ValidateTablesChecksumDefinitionTable_Test_UndefState */

void CS_ValidateTablesChecksumDefinitionTable_Test_CsTableError(void)
{
    int32 Result;
    int32 strCmpResult;
    char  ExpectedEventString[2][CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString[0], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS Tables Table Validate: Duplicate Name (%%s) found at entries %%d and %%d");

    snprintf(ExpectedEventString[1], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS Tables Table verification results: good = %%d, bad = %%d, unused = %%d");

    CS_AppData.DefTablesTblPtr[0].State = CS_STATE_DISABLED;
    CS_AppData.DefTablesTblPtr[1].State = CS_STATE_DISABLED;
    CS_AppData.DefTablesTblPtr[2].State = CS_STATE_UNDEFINED;

    strncpy(CS_AppData.DefTablesTblPtr[0].Name, "name", 10);
    strncpy(CS_AppData.DefTablesTblPtr[1].Name, "name", 10);

    /* Execute the function being tested */
    Result = CS_ValidateTablesChecksumDefinitionTable(CS_AppData.DefTablesTblPtr);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_VAL_TABLES_DEF_TBL_DUPL_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult =
        strncmp(ExpectedEventString[0], context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, CS_VAL_TABLES_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult =
        strncmp(ExpectedEventString[1], context_CFE_EVS_SendEvent[1].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[1].Spec);

    UtAssert_True(Result == CS_TABLE_ERROR, "Result == CS_TABLE_ERROR");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 2, "CFE_EVS_SendEvent was called %u time(s), expected 2",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_ValidateTablesChecksumDefinitionTable_Test_CsTableError */

void CS_ValidateAppChecksumDefinitionTable_Test_Nominal(void)
{
    int32 Result;
    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS Apps Table verification results: good = %%d, bad = %%d, unused = %%d");

    CS_AppData.DefAppTblPtr[0].State = CS_STATE_ENABLED; /* All other states are empty by default, and so this test also
                                                            covers CS_STATE_EMPTY branch */

    strncpy(CS_AppData.DefAppTblPtr[0].Name, "name", 10);

    /* Execute the function being tested */
    Result = CS_ValidateAppChecksumDefinitionTable(CS_AppData.DefAppTblPtr);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_VAL_APP_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_ValidateAppChecksumDefinitionTable_Test_Nominal */

void CS_ValidateAppChecksumDefinitionTable_Test_DuplicateNameStateEmpty(void)
{
    int32 Result;
    int32 strCmpResult;
    char  ExpectedEventString[2][CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString[0], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS Apps Table Validate: Duplicate Name (%%s) found at entries %%d and %%d");

    snprintf(ExpectedEventString[1], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS Apps Table verification results: good = %%d, bad = %%d, unused = %%d");

    /* All states are empty by default */

    strncpy(CS_AppData.DefAppTblPtr[0].Name, "name", 10);
    strncpy(CS_AppData.DefAppTblPtr[1].Name, "name", 10);

    /* Execute the function being tested */
    Result = CS_ValidateAppChecksumDefinitionTable(CS_AppData.DefAppTblPtr);

    /* Verify results */

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_VAL_APP_DEF_TBL_DUPL_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult =
        strncmp(ExpectedEventString[0], context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, CS_VAL_APP_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult =
        strncmp(ExpectedEventString[1], context_CFE_EVS_SendEvent[1].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[1].Spec);

    UtAssert_True(Result == CS_TABLE_ERROR, "Result == CS_TABLE_ERROR");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 2, "CFE_EVS_SendEvent was called %u time(s), expected 2",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_ValidateAppChecksumDefinitionTable_Test_DuplicateNameStateEmpty */

void CS_ValidateAppChecksumDefinitionTable_Test_DuplicateNameStateEnabled(void)
{
    int32 Result;
    int32 strCmpResult;
    char  ExpectedEventString[2][CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString[0], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS Apps Table Validate: Duplicate Name (%%s) found at entries %%d and %%d");

    snprintf(ExpectedEventString[1], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS Apps Table verification results: good = %%d, bad = %%d, unused = %%d");

    CS_AppData.DefAppTblPtr[0].State = CS_STATE_ENABLED; /* All other states are empty by default, and so this test also
                                                            covers CS_STATE_EMPTY branch */
    CS_AppData.DefAppTblPtr[1].State = CS_STATE_ENABLED; /* All other states are empty by default, and so this test also
                                                            covers CS_STATE_EMPTY branch */

    strncpy(CS_AppData.DefAppTblPtr[0].Name, "name", 10);
    strncpy(CS_AppData.DefAppTblPtr[1].Name, "name", 10);

    /* Execute the function being tested */
    Result = CS_ValidateAppChecksumDefinitionTable(CS_AppData.DefAppTblPtr);

    /* Verify results */

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_VAL_APP_DEF_TBL_DUPL_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult =
        strncmp(ExpectedEventString[0], context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, CS_VAL_APP_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult =
        strncmp(ExpectedEventString[1], context_CFE_EVS_SendEvent[1].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[1].Spec);

    UtAssert_True(Result == CS_TABLE_ERROR, "Result == CS_TABLE_ERROR");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 2, "CFE_EVS_SendEvent was called %u time(s), expected 2",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_ValidateAppChecksumDefinitionTable_Test_DuplicateNameStateEnabled */

void CS_ValidateAppChecksumDefinitionTable_Test_DuplicateNameStateDisabled(void)
{
    int32 Result;
    int32 strCmpResult;
    char  ExpectedEventString[2][CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString[0], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS Apps Table Validate: Duplicate Name (%%s) found at entries %%d and %%d");

    snprintf(ExpectedEventString[1], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS Apps Table verification results: good = %%d, bad = %%d, unused = %%d");

    CS_AppData.DefAppTblPtr[0].State = CS_STATE_DISABLED; /* All other states are empty by default, and so this test
                                                             also covers CS_STATE_EMPTY branch */
    CS_AppData.DefAppTblPtr[1].State = CS_STATE_DISABLED; /* All other states are empty by default, and so this test
                                                             also covers CS_STATE_EMPTY branch */

    strncpy(CS_AppData.DefAppTblPtr[0].Name, "name", 10);
    strncpy(CS_AppData.DefAppTblPtr[1].Name, "name", 10);

    /* Execute the function being tested */
    Result = CS_ValidateAppChecksumDefinitionTable(CS_AppData.DefAppTblPtr);

    /* Verify results */

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_VAL_APP_DEF_TBL_DUPL_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult =
        strncmp(ExpectedEventString[0], context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, CS_VAL_APP_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult =
        strncmp(ExpectedEventString[1], context_CFE_EVS_SendEvent[1].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[1].Spec);

    UtAssert_True(Result == CS_TABLE_ERROR, "Result == CS_TABLE_ERROR");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 2, "CFE_EVS_SendEvent was called %u time(s), expected 2",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_ValidateAppChecksumDefinitionTable_Test_DuplicateNameStateDisabled */

void CS_ValidateAppChecksumDefinitionTable_Test_IllegalStateField(void)
{
    int32 Result;
    int32 strCmpResult;
    char  ExpectedEventString[2][CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString[0], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS Apps Table Validate: Illegal State Field (0x%%04X) found with name %%s");

    snprintf(ExpectedEventString[1], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS Apps Table verification results: good = %%d, bad = %%d, unused = %%d");

    CS_AppData.DefAppTblPtr[0].State = 0xFFFF;

    strncpy(CS_AppData.DefAppTblPtr[0].Name, "name", 10);

    /* Execute the function being tested */
    Result = CS_ValidateAppChecksumDefinitionTable(CS_AppData.DefAppTblPtr);

    /* Verify results */

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_VAL_APP_STATE_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult =
        strncmp(ExpectedEventString[0], context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, CS_VAL_APP_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult =
        strncmp(ExpectedEventString[1], context_CFE_EVS_SendEvent[1].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[1].Spec);

    UtAssert_True(Result == CS_TABLE_ERROR, "Result == CS_TABLE_ERROR");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 2, "CFE_EVS_SendEvent was called %u time(s), expected 2",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_ValidateAppChecksumDefinitionTable_Test_IllegalStateField */

void CS_ValidateAppChecksumDefinitionTable_Test_IllegalStateEmptyName(void)
{
    int32 Result;
    int32 strCmpResult;
    char  ExpectedEventString[2][CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString[0], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS Apps Table Validate: Illegal State (0x%%04X) with empty name at entry %%d");

    snprintf(ExpectedEventString[1], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS Apps Table verification results: good = %%d, bad = %%d, unused = %%d");

    CS_AppData.DefAppTblPtr[0].State = CS_STATE_ENABLED;

    /* Execute the function being tested */
    Result = CS_ValidateAppChecksumDefinitionTable(CS_AppData.DefAppTblPtr);

    /* Verify results */

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_VAL_APP_DEF_TBL_ZERO_NAME_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult =
        strncmp(ExpectedEventString[0], context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, CS_VAL_APP_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult =
        strncmp(ExpectedEventString[1], context_CFE_EVS_SendEvent[1].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[1].Spec);

    UtAssert_True(Result == CS_TABLE_ERROR, "Result == CS_TABLE_ERROR");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 2, "CFE_EVS_SendEvent was called %u time(s), expected 2",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_ValidateAppChecksumDefinitionTable_Test_IllegalStateEmptyName */

void CS_ValidateAppChecksumDefinitionTable_Test_TableErrorResult(void)
{
    int32 Result;
    int32 strCmpResult;
    char  ExpectedEventString[2][CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString[0], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS Apps Table Validate: Duplicate Name (%%s) found at entries %%d and %%d");

    snprintf(ExpectedEventString[1], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS Apps Table verification results: good = %%d, bad = %%d, unused = %%d");

    /* All states are empty by default */
    CS_AppData.DefAppTblPtr[0].State = CS_STATE_DISABLED;
    CS_AppData.DefAppTblPtr[1].State = CS_STATE_DISABLED;
    CS_AppData.DefAppTblPtr[2].State = CS_STATE_DISABLED;

    strncpy(CS_AppData.DefAppTblPtr[0].Name, "name", 10);
    strncpy(CS_AppData.DefAppTblPtr[1].Name, "name", 10);
    strncpy(CS_AppData.DefAppTblPtr[2].Name, "name", 10);

    /* Execute the function being tested */
    Result = CS_ValidateAppChecksumDefinitionTable(CS_AppData.DefAppTblPtr);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_VAL_APP_DEF_TBL_DUPL_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult =
        strncmp(ExpectedEventString[0], context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, CS_VAL_APP_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult =
        strncmp(ExpectedEventString[1], context_CFE_EVS_SendEvent[1].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[1].Spec);

    UtAssert_True(Result == CS_TABLE_ERROR, "Result == CS_TABLE_ERROR");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 2, "CFE_EVS_SendEvent was called %u time(s), expected 2",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_ValidateAppChecksumDefinitionTable_Test_TableErrorResult */

void CS_ValidateAppChecksumDefinitionTable_Test_UndefTableErrorResult(void)
{
    int32 Result;
    int32 strCmpResult;
    char  ExpectedEventString[2][CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString[0], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS Apps Table Validate: Duplicate Name (%%s) found at entries %%d and %%d");

    snprintf(ExpectedEventString[1], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS Apps Table verification results: good = %%d, bad = %%d, unused = %%d");

    CS_AppData.DefAppTblPtr[0].State = CS_STATE_DISABLED;
    CS_AppData.DefAppTblPtr[1].State = CS_STATE_DISABLED;
    CS_AppData.DefAppTblPtr[2].State = CS_STATE_UNDEFINED;

    strncpy(CS_AppData.DefAppTblPtr[0].Name, "name", 10);
    strncpy(CS_AppData.DefAppTblPtr[1].Name, "name", 10);
    strncpy(CS_AppData.DefAppTblPtr[2].Name, "name", 10);

    /* Execute the function being tested */
    Result = CS_ValidateAppChecksumDefinitionTable(CS_AppData.DefAppTblPtr);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_VAL_APP_DEF_TBL_DUPL_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult =
        strncmp(ExpectedEventString[0], context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, CS_VAL_APP_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult =
        strncmp(ExpectedEventString[1], context_CFE_EVS_SendEvent[1].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[1].Spec);

    UtAssert_True(Result == CS_TABLE_ERROR, "Result == CS_TABLE_ERROR");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 2, "CFE_EVS_SendEvent was called %u time(s), expected 2",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_ValidateAppChecksumDefinitionTable_Test_UndefTableErrorResult */

void CS_ValidateAppChecksumDefinitionTable_Test_EmptyNameTableResult(void)
{
    int32 Result;
    int32 strCmpResult;
    char  ExpectedEventString[2][CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString[0], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS Apps Table Validate: Illegal State (0x%%04X) with empty name at entry %%d");

    snprintf(ExpectedEventString[1], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS Apps Table verification results: good = %%d, bad = %%d, unused = %%d");

    CS_AppData.DefAppTblPtr[0].State = CS_STATE_ENABLED;
    CS_AppData.DefAppTblPtr[1].State = CS_STATE_ENABLED;
    CS_AppData.DefAppTblPtr[2].State = CS_STATE_ENABLED;

    /* Execute the function being tested */
    Result = CS_ValidateAppChecksumDefinitionTable(CS_AppData.DefAppTblPtr);

    /* Verify results */

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_VAL_APP_DEF_TBL_ZERO_NAME_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult =
        strncmp(ExpectedEventString[0], context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, CS_VAL_APP_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult =
        strncmp(ExpectedEventString[1], context_CFE_EVS_SendEvent[1].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[1].Spec);

    UtAssert_True(Result == CS_TABLE_ERROR, "Result == CS_TABLE_ERROR");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 2, "CFE_EVS_SendEvent was called %u time(s), expected 2",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_ValidateAppChecksumDefinitionTable_Test_EmptyNameTableResult */

void CS_ProcessNewEepromMemoryDefinitionTable_Test_EEPROMTableNominal(void)
{
    /* Handles both cases of "DefEntry -> State" */

    uint16 NumEntries = 1;
    uint16 Table      = CS_EEPROM_TABLE;

    CS_AppData.HkPacket.EepromCSState                = 99;
    CS_AppData.DefEepromTblPtr[0].State              = 1;
    CS_AppData.DefEepromTblPtr[0].NumBytesToChecksum = 2;
    CS_AppData.DefEepromTblPtr[0].StartAddress       = 3;

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just
       pointers */
    CS_ProcessNewEepromMemoryDefinitionTable((CS_Def_EepromMemory_Table_Entry_t *)&CS_AppData.DefEepromTblPtr,
                                             (CS_Res_EepromMemory_Table_Entry_t *)&CS_AppData.ResEepromTblPtr,
                                             NumEntries, Table);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.EepromCSState == 99, "CS_AppData.HkPacket.EepromCSState == 99");

    UtAssert_True(CS_AppData.ResEepromTblPtr[0].State == 1, "CS_AppData.ResEepromTblPtr[0].State == 1");
    UtAssert_True(CS_AppData.ResEepromTblPtr[0].ComputedYet == false,
                  "CS_AppData.ResEepromTblPtr[0].ComputedYet == false");
    UtAssert_True(CS_AppData.ResEepromTblPtr[0].NumBytesToChecksum == 2,
                  "CS_AppData.ResEepromTblPtr[0].NumBytesToChecksum == 2");
    UtAssert_True(CS_AppData.ResEepromTblPtr[0].ComparisonValue == 0,
                  "CS_AppData.ResEepromTblPtr[0].ComparisonValue == 0");
    UtAssert_True(CS_AppData.ResEepromTblPtr[0].ByteOffset == 0, "CS_AppData.ResEepromTblPtr[0].ByteOffset == 0");
    UtAssert_True(CS_AppData.ResEepromTblPtr[0].TempChecksumValue == 0,
                  "CS_AppData.ResEepromTblPtr[0].TempChecksumValue == 0");
    UtAssert_True(CS_AppData.ResEepromTblPtr[0].StartAddress == 3, "CS_AppData.ResEepromTblPtr[0].StartAddress == 3");

    UtAssert_True(CS_AppData.ResEepromTblPtr[1].State == CS_STATE_EMPTY,
                  "CS_AppData.ResEepromTblPtr[1].State == CS_STATE_EMPTY");
    UtAssert_True(CS_AppData.ResEepromTblPtr[1].ComputedYet == false,
                  "CS_AppData.ResEepromTblPtr[1].ComputedYet == false");
    UtAssert_True(CS_AppData.ResEepromTblPtr[1].NumBytesToChecksum == 0,
                  "CS_AppData.ResEepromTblPtr[1].NumBytesToChecksum == 0");
    UtAssert_True(CS_AppData.ResEepromTblPtr[1].ComparisonValue == 0,
                  "CS_AppData.ResEepromTblPtr[1].ComparisonValue == 0");
    UtAssert_True(CS_AppData.ResEepromTblPtr[1].ByteOffset == 0, "CS_AppData.ResEepromTblPtr[1].ByteOffset == 0");
    UtAssert_True(CS_AppData.ResEepromTblPtr[1].TempChecksumValue == 0,
                  "CS_AppData.ResEepromTblPtr[1].TempChecksumValue == 0");
    UtAssert_True(CS_AppData.ResEepromTblPtr[1].StartAddress == 0, "CS_AppData.ResEepromTblPtr[1].StartAddress == 0");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_ProcessNewEepromMemoryDefinitionTable_Test_EEPROMTableNominal */

void CS_ProcessNewEepromMemoryDefinitionTable_Test_MemoryTableNominal(void)
{
    /* Handles both cases of "DefEntry -> State" */

    uint16 NumEntries = 1;
    uint16 Table      = CS_MEMORY_TABLE;

    CS_AppData.HkPacket.MemoryCSState                = 99;
    CS_AppData.DefMemoryTblPtr[0].State              = 1;
    CS_AppData.DefMemoryTblPtr[0].NumBytesToChecksum = 2;
    CS_AppData.DefMemoryTblPtr[0].StartAddress       = 3;

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just
       pointers */
    CS_ProcessNewEepromMemoryDefinitionTable((CS_Def_EepromMemory_Table_Entry_t *)&CS_AppData.DefMemoryTblPtr,
                                             (CS_Res_EepromMemory_Table_Entry_t *)&CS_AppData.ResMemoryTblPtr,
                                             NumEntries, Table);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.MemoryCSState == 99, "CS_AppData.HkPacket.MemoryCSState == 99");

    UtAssert_True(CS_AppData.ResMemoryTblPtr[0].State == 1, "CS_AppData.ResMemoryTblPtr[0].State == 1");
    UtAssert_True(CS_AppData.ResMemoryTblPtr[0].ComputedYet == false,
                  "CS_AppData.ResMemoryTblPtr[0].ComputedYet == false");
    UtAssert_True(CS_AppData.ResMemoryTblPtr[0].NumBytesToChecksum == 2,
                  "CS_AppData.ResMemoryTblPtr[0].NumBytesToChecksum == 2");
    UtAssert_True(CS_AppData.ResMemoryTblPtr[0].ComparisonValue == 0,
                  "CS_AppData.ResMemoryTblPtr[0].ComparisonValue == 0");
    UtAssert_True(CS_AppData.ResMemoryTblPtr[0].ByteOffset == 0, "CS_AppData.ResMemoryTblPtr[0].ByteOffset == 0");
    UtAssert_True(CS_AppData.ResMemoryTblPtr[0].TempChecksumValue == 0,
                  "CS_AppData.ResMemoryTblPtr[0].TempChecksumValue == 0");
    UtAssert_True(CS_AppData.ResMemoryTblPtr[0].StartAddress == 3, "CS_AppData.ResMemoryTblPtr[0].StartAddress == 3");

    UtAssert_True(CS_AppData.ResMemoryTblPtr[1].State == CS_STATE_EMPTY,
                  "CS_AppData.ResMemoryTblPtr[1].State == CS_STATE_EMPTY");
    UtAssert_True(CS_AppData.ResMemoryTblPtr[1].ComputedYet == false,
                  "CS_AppData.ResMemoryTblPtr[1].ComputedYet == false");
    UtAssert_True(CS_AppData.ResMemoryTblPtr[1].NumBytesToChecksum == 0,
                  "CS_AppData.ResMemoryTblPtr[1].NumBytesToChecksum == 0");
    UtAssert_True(CS_AppData.ResMemoryTblPtr[1].ComparisonValue == 0,
                  "CS_AppData.ResMemoryTblPtr[1].ComparisonValue == 0");
    UtAssert_True(CS_AppData.ResMemoryTblPtr[1].ByteOffset == 0, "CS_AppData.ResMemoryTblPtr[1].ByteOffset == 0");
    UtAssert_True(CS_AppData.ResMemoryTblPtr[1].TempChecksumValue == 0,
                  "CS_AppData.ResMemoryTblPtr[1].TempChecksumValue == 0");
    UtAssert_True(CS_AppData.ResMemoryTblPtr[1].StartAddress == 0, "CS_AppData.ResMemoryTblPtr[1].StartAddress == 0");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_ProcessNewEepromMemoryDefinitionTable_Test_MemoryTableNominal */

void CS_ProcessNewEepromMemoryDefinitionTable_Test_EEPROMTableNoValidEntries(void)
{
    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "CS %%s Table: No valid entries in the table");

    /* Handles both cases of "DefEntry -> State" */

    uint16 NumEntries = 1;
    uint16 Table      = CS_EEPROM_TABLE;

    CS_AppData.HkPacket.MemoryCSState = 99;

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just
       pointers */
    CS_ProcessNewEepromMemoryDefinitionTable((CS_Def_EepromMemory_Table_Entry_t *)&CS_AppData.DefMemoryTblPtr,
                                             (CS_Res_EepromMemory_Table_Entry_t *)&CS_AppData.ResMemoryTblPtr,
                                             NumEntries, Table);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.MemoryCSState == 99, "CS_AppData.HkPacket.MemoryCSState == 99");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_PROCESS_EEPROM_MEMORY_NO_ENTRIES_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_ProcessNewEepromMemoryDefinitionTable_Test_EEPROMTableNoValidEntries */

void CS_ProcessNewEepromMemoryDefinitionTable_Test_MemoryTableNoValidEntries(void)
{
    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "CS %%s Table: No valid entries in the table");

    /* Handles both cases of "DefEntry -> State" */

    uint16 NumEntries = 1;
    uint16 Table      = CS_MEMORY_TABLE;

    CS_AppData.HkPacket.MemoryCSState = 99;

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just
       pointers */
    CS_ProcessNewEepromMemoryDefinitionTable((CS_Def_EepromMemory_Table_Entry_t *)&CS_AppData.DefMemoryTblPtr,
                                             (CS_Res_EepromMemory_Table_Entry_t *)&CS_AppData.ResMemoryTblPtr,
                                             NumEntries, Table);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.MemoryCSState == 99, "CS_AppData.HkPacket.MemoryCSState == 99");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_PROCESS_EEPROM_MEMORY_NO_ENTRIES_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_ProcessNewEepromMemoryDefinitionTable_Test_MemoryTableNoValidEntries */

void CS_ProcessNewTablesDefinitionTable_Test_DefEepromTableHandle(void)
{
    CS_AppData.HkPacket.TablesCSState   = 99;
    CS_AppData.DefTablesTblPtr[0].State = 88;

    strncpy(CS_AppData.DefTablesTblPtr[0].Name, "CS.DefEepromTbl", 20);

    CS_AppData.DefEepromTableHandle = 11;

    /* Sets AppName to "CS" */
    UT_SetHandlerFunction(UT_KEY(CFE_ES_GetAppName), CS_TABLE_PROCESSING_TEST_CFE_ES_GetAppNameHandler1, NULL);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just
       pointers */
    CS_ProcessNewTablesDefinitionTable((CS_Def_Tables_Table_Entry_t *)&CS_AppData.DefTablesTblPtr,
                                       (CS_Res_Tables_Table_Entry_t *)&CS_AppData.ResTablesTblPtr);

    /* Verify results */
    UtAssert_True(CS_AppData.MemResTablesTblPtr == NULL, "CS_AppData.MemResTablesTblPtr == NULL");
    UtAssert_True(CS_AppData.AppResTablesTblPtr == NULL, "CS_AppData.AppResTablesTblPtr == NULL");
    UtAssert_True(CS_AppData.TblResTablesTblPtr == NULL, "CS_AppData.TblResTablesTblPtr == NULL");

    UtAssert_True(CS_AppData.EepResTablesTblPtr == CS_AppData.ResTablesTblPtr,
                  "CS_AppData.EepResTablesTblPtr == CS_AppData.ResTablesTblPtr");

    UtAssert_True(CS_AppData.ResTablesTblPtr[0].State == 88, "CS_AppData.ResTablesTblPtr[0].State == 88");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].ComputedYet == false,
                  "CS_AppData.ResTablesTblPtr[0].ComputedYet == false");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].NumBytesToChecksum == 0,
                  "CS_AppData.ResTablesTblPtr[0].NumBytesToChecksum == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].ComparisonValue == 0,
                  "CS_AppData.ResTablesTblPtr[0].ComparisonValue == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].ByteOffset == 0, "CS_AppData.ResTablesTblPtr[0].ByteOffset == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].TempChecksumValue == 0,
                  "CS_AppData.ResTablesTblPtr[0].TempChecksumValue == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].StartAddress == 0, "CS_AppData.ResTablesTblPtr[0].StartAddress == 0");

    UtAssert_True(CS_AppData.ResTablesTblPtr[0].TblHandle == 11, "CS_AppData.ResTablesTblPtr[0].TblHandle == 11");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].IsCSOwner == true, "CS_AppData.ResTablesTblPtr[0].IsCSOwner == true");
    UtAssert_True(strncmp(CS_AppData.ResTablesTblPtr[0].Name, "CS.DefEepromTbl", 20) == 0,
                  "strncmp(CS_AppData.ResTablesTblPtr[0].Name, 'CS.DefEepromTbl', 20) == 0");

    UtAssert_True(CS_AppData.HkPacket.TablesCSState == 99, "CS_AppData.HkPacket.TablesCSState == 99");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_ProcessNewTablesDefinitionTable_Test_DefEepromTableHandle */

void CS_ProcessNewTablesDefinitionTable_Test_DefMemoryTableHandle(void)
{
    CS_AppData.HkPacket.TablesCSState   = 99;
    CS_AppData.DefTablesTblPtr[0].State = 88;

    strncpy(CS_AppData.DefTablesTblPtr[0].Name, "CS.DefMemoryTbl", 20);

    CS_AppData.DefMemoryTableHandle = 11;

    /* Sets AppName to "CS" */
    UT_SetHandlerFunction(UT_KEY(CFE_ES_GetAppName), CS_TABLE_PROCESSING_TEST_CFE_ES_GetAppNameHandler1, NULL);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just
       pointers */
    CS_ProcessNewTablesDefinitionTable((CS_Def_Tables_Table_Entry_t *)&CS_AppData.DefTablesTblPtr,
                                       (CS_Res_Tables_Table_Entry_t *)&CS_AppData.ResTablesTblPtr);

    /* Verify results */
    UtAssert_True(CS_AppData.EepResTablesTblPtr == NULL, "CS_AppData.EepResTablesTblPtr == NULL");
    UtAssert_True(CS_AppData.AppResTablesTblPtr == NULL, "CS_AppData.AppResTablesTblPtr == NULL");
    UtAssert_True(CS_AppData.TblResTablesTblPtr == NULL, "CS_AppData.TblResTablesTblPtr == NULL");

    UtAssert_True(CS_AppData.MemResTablesTblPtr == CS_AppData.ResTablesTblPtr,
                  "CS_AppData.MemResTablesTblPtr == CS_AppData.ResTablesTblPtr");

    UtAssert_True(CS_AppData.ResTablesTblPtr[0].State == 88, "CS_AppData.ResTablesTblPtr[0].State == 88");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].ComputedYet == false,
                  "CS_AppData.ResTablesTblPtr[0].ComputedYet == false");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].NumBytesToChecksum == 0,
                  "CS_AppData.ResTablesTblPtr[0].NumBytesToChecksum == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].ComparisonValue == 0,
                  "CS_AppData.ResTablesTblPtr[0].ComparisonValue == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].ByteOffset == 0, "CS_AppData.ResTablesTblPtr[0].ByteOffset == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].TempChecksumValue == 0,
                  "CS_AppData.ResTablesTblPtr[0].TempChecksumValue == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].StartAddress == 0, "CS_AppData.ResTablesTblPtr[0].StartAddress == 0");

    UtAssert_True(CS_AppData.ResTablesTblPtr[0].TblHandle == 11, "CS_AppData.ResTablesTblPtr[0].TblHandle == 11");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].IsCSOwner == true, "CS_AppData.ResTablesTblPtr[0].IsCSOwner == true");
    UtAssert_True(strncmp(CS_AppData.ResTablesTblPtr[0].Name, "CS.DefMemoryTbl", 20) == 0,
                  "strncmp(CS_AppData.ResTablesTblPtr[0].Name, 'CS.DefMemoryTbl', 20) == 0");

    UtAssert_True(CS_AppData.HkPacket.TablesCSState == 99, "CS_AppData.HkPacket.TablesCSState == 99");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_ProcessNewTablesDefinitionTable_Test_DefMemoryTableHandle */

void CS_ProcessNewTablesDefinitionTable_Test_DefTablesTableHandle(void)
{
    CS_AppData.HkPacket.TablesCSState   = 99;
    CS_AppData.DefTablesTblPtr[0].State = 88;

    strncpy(CS_AppData.DefTablesTblPtr[0].Name, "CS.DefTablesTbl", 20);

    CS_AppData.DefTablesTableHandle = 11;

    /* Sets AppName to "CS" */
    UT_SetHandlerFunction(UT_KEY(CFE_ES_GetAppName), CS_TABLE_PROCESSING_TEST_CFE_ES_GetAppNameHandler1, NULL);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just
       pointers */
    CS_ProcessNewTablesDefinitionTable((CS_Def_Tables_Table_Entry_t *)&CS_AppData.DefTablesTblPtr,
                                       (CS_Res_Tables_Table_Entry_t *)&CS_AppData.ResTablesTblPtr);

    /* Verify results */
    UtAssert_True(CS_AppData.EepResTablesTblPtr == NULL, "CS_AppData.EepResTablesTblPtr == NULL");
    UtAssert_True(CS_AppData.AppResTablesTblPtr == NULL, "CS_AppData.AppResTablesTblPtr == NULL");
    UtAssert_True(CS_AppData.MemResTablesTblPtr == NULL, "CS_AppData.MemResTablesTblPtr == NULL");

    UtAssert_True(CS_AppData.TblResTablesTblPtr == CS_AppData.ResTablesTblPtr,
                  "CS_AppData.TblResTablesTblPtr == CS_AppData.ResTablesTblPtr");

    UtAssert_True(CS_AppData.ResTablesTblPtr[0].State == 88, "CS_AppData.ResTablesTblPtr[0].State == 88");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].ComputedYet == false,
                  "CS_AppData.ResTablesTblPtr[0].ComputedYet == false");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].NumBytesToChecksum == 0,
                  "CS_AppData.ResTablesTblPtr[0].NumBytesToChecksum == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].ComparisonValue == 0,
                  "CS_AppData.ResTablesTblPtr[0].ComparisonValue == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].ByteOffset == 0, "CS_AppData.ResTablesTblPtr[0].ByteOffset == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].TempChecksumValue == 0,
                  "CS_AppData.ResTablesTblPtr[0].TempChecksumValue == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].StartAddress == 0, "CS_AppData.ResTablesTblPtr[0].StartAddress == 0");

    UtAssert_True(CS_AppData.ResTablesTblPtr[0].TblHandle == 11, "CS_AppData.ResTablesTblPtr[0].TblHandle == 11");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].IsCSOwner == true, "CS_AppData.ResTablesTblPtr[0].IsCSOwner == true");
    UtAssert_True(strncmp(CS_AppData.ResTablesTblPtr[0].Name, "CS.DefTablesTbl", 20) == 0,
                  "strncmp(CS_AppData.ResTablesTblPtr[0].Name, 'CS.DefTablesTbl', 20) == 0");

    UtAssert_True(CS_AppData.HkPacket.TablesCSState == 99, "CS_AppData.HkPacket.TablesCSState == 99");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_ProcessNewTablesDefinitionTable_Test_DefTablesTableHandle */

void CS_ProcessNewTablesDefinitionTable_Test_DefAppTableHandle(void)
{
    CS_AppData.HkPacket.TablesCSState   = 99;
    CS_AppData.DefTablesTblPtr[0].State = 88;

    strncpy(CS_AppData.DefTablesTblPtr[0].Name, "CS.DefAppTbl", 20);

    CS_AppData.DefAppTableHandle = 11;

    /* Sets AppName to "CS" */
    UT_SetHandlerFunction(UT_KEY(CFE_ES_GetAppName), CS_TABLE_PROCESSING_TEST_CFE_ES_GetAppNameHandler1, NULL);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just
       pointers */
    CS_ProcessNewTablesDefinitionTable((CS_Def_Tables_Table_Entry_t *)&CS_AppData.DefTablesTblPtr,
                                       (CS_Res_Tables_Table_Entry_t *)&CS_AppData.ResTablesTblPtr);

    /* Verify results */
    UtAssert_True(CS_AppData.EepResTablesTblPtr == NULL, "CS_AppData.EepResTablesTblPtr == NULL");
    UtAssert_True(CS_AppData.TblResTablesTblPtr == NULL, "CS_AppData.TblResTablesTblPtr == NULL");
    UtAssert_True(CS_AppData.MemResTablesTblPtr == NULL, "CS_AppData.MemResTablesTblPtr == NULL");

    UtAssert_True(CS_AppData.AppResTablesTblPtr == CS_AppData.ResTablesTblPtr,
                  "CS_AppData.AppResTablesTblPtr == CS_AppData.ResTablesTblPtr");

    UtAssert_True(CS_AppData.ResTablesTblPtr[0].State == 88, "CS_AppData.ResTablesTblPtr[0].State == 88");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].ComputedYet == false,
                  "CS_AppData.ResTablesTblPtr[0].ComputedYet == false");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].NumBytesToChecksum == 0,
                  "CS_AppData.ResTablesTblPtr[0].NumBytesToChecksum == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].ComparisonValue == 0,
                  "CS_AppData.ResTablesTblPtr[0].ComparisonValue == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].ByteOffset == 0, "CS_AppData.ResTablesTblPtr[0].ByteOffset == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].TempChecksumValue == 0,
                  "CS_AppData.ResTablesTblPtr[0].TempChecksumValue == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].StartAddress == 0, "CS_AppData.ResTablesTblPtr[0].StartAddress == 0");

    UtAssert_True(CS_AppData.ResTablesTblPtr[0].TblHandle == 11, "CS_AppData.ResTablesTblPtr[0].TblHandle == 11");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].IsCSOwner == true, "CS_AppData.ResTablesTblPtr[0].IsCSOwner == true");
    UtAssert_True(strncmp(CS_AppData.ResTablesTblPtr[0].Name, "CS.DefAppTbl", 20) == 0,
                  "strncmp(CS_AppData.ResTablesTblPtr[0].Name, 'CS.DefAppTbl', 20) == 0");

    UtAssert_True(CS_AppData.HkPacket.TablesCSState == 99, "CS_AppData.HkPacket.TablesCSState == 99");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_ProcessNewTablesDefinitionTable_Test_DefAppTableHandle */

void CS_ProcessNewTablesDefinitionTable_Test_ResEepromTableHandle(void)
{
    CS_AppData.HkPacket.TablesCSState   = 99;
    CS_AppData.DefTablesTblPtr[0].State = 88;

    strncpy(CS_AppData.DefTablesTblPtr[0].Name, "CS.ResEepromTbl", 20);

    CS_AppData.ResEepromTableHandle = 11;

    /* Sets AppName to "CS" */
    UT_SetHandlerFunction(UT_KEY(CFE_ES_GetAppName), CS_TABLE_PROCESSING_TEST_CFE_ES_GetAppNameHandler1, NULL);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just
       pointers */
    CS_ProcessNewTablesDefinitionTable((CS_Def_Tables_Table_Entry_t *)&CS_AppData.DefTablesTblPtr,
                                       (CS_Res_Tables_Table_Entry_t *)&CS_AppData.ResTablesTblPtr);

    /* Verify results */
    UtAssert_True(CS_AppData.EepResTablesTblPtr == NULL, "CS_AppData.EepResTablesTblPtr == NULL");
    UtAssert_True(CS_AppData.MemResTablesTblPtr == NULL, "CS_AppData.MemResTablesTblPtr == NULL");
    UtAssert_True(CS_AppData.AppResTablesTblPtr == NULL, "CS_AppData.AppResTablesTblPtr == NULL");
    UtAssert_True(CS_AppData.TblResTablesTblPtr == NULL, "CS_AppData.TblResTablesTblPtr == NULL");

    UtAssert_True(CS_AppData.ResTablesTblPtr[0].State == 88, "CS_AppData.ResTablesTblPtr[0].State == 88");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].ComputedYet == false,
                  "CS_AppData.ResTablesTblPtr[0].ComputedYet == false");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].NumBytesToChecksum == 0,
                  "CS_AppData.ResTablesTblPtr[0].NumBytesToChecksum == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].ComparisonValue == 0,
                  "CS_AppData.ResTablesTblPtr[0].ComparisonValue == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].ByteOffset == 0, "CS_AppData.ResTablesTblPtr[0].ByteOffset == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].TempChecksumValue == 0,
                  "CS_AppData.ResTablesTblPtr[0].TempChecksumValue == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].StartAddress == 0, "CS_AppData.ResTablesTblPtr[0].StartAddress == 0");

    UtAssert_True(CS_AppData.ResTablesTblPtr[0].TblHandle == 11, "CS_AppData.ResTablesTblPtr[0].TblHandle == 11");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].IsCSOwner == true, "CS_AppData.ResTablesTblPtr[0].IsCSOwner == true");
    UtAssert_True(strncmp(CS_AppData.ResTablesTblPtr[0].Name, "CS.ResEepromTbl", 20) == 0,
                  "strncmp(CS_AppData.ResTablesTblPtr[0].Name, 'CS.ResEepromTbl', 20) == 0");

    UtAssert_True(CS_AppData.HkPacket.TablesCSState == 99, "CS_AppData.HkPacket.TablesCSState == 99");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_ProcessNewTablesDefinitionTable_Test_ResEepromTableHandle */

void CS_ProcessNewTablesDefinitionTable_Test_ResMemoryTableHandle(void)
{
    CS_AppData.HkPacket.TablesCSState   = 99;
    CS_AppData.DefTablesTblPtr[0].State = 88;

    strncpy(CS_AppData.DefTablesTblPtr[0].Name, "CS.ResMemoryTbl", 20);

    CS_AppData.ResMemoryTableHandle = 11;

    /* Sets AppName to "CS" */
    UT_SetHandlerFunction(UT_KEY(CFE_ES_GetAppName), CS_TABLE_PROCESSING_TEST_CFE_ES_GetAppNameHandler1, NULL);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just
       pointers */
    CS_ProcessNewTablesDefinitionTable((CS_Def_Tables_Table_Entry_t *)&CS_AppData.DefTablesTblPtr,
                                       (CS_Res_Tables_Table_Entry_t *)&CS_AppData.ResTablesTblPtr);

    /* Verify results */
    UtAssert_True(CS_AppData.EepResTablesTblPtr == NULL, "CS_AppData.EepResTablesTblPtr == NULL");
    UtAssert_True(CS_AppData.MemResTablesTblPtr == NULL, "CS_AppData.MemResTablesTblPtr == NULL");
    UtAssert_True(CS_AppData.AppResTablesTblPtr == NULL, "CS_AppData.AppResTablesTblPtr == NULL");
    UtAssert_True(CS_AppData.TblResTablesTblPtr == NULL, "CS_AppData.TblResTablesTblPtr == NULL");

    UtAssert_True(CS_AppData.ResTablesTblPtr[0].State == 88, "CS_AppData.ResTablesTblPtr[0].State == 88");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].ComputedYet == false,
                  "CS_AppData.ResTablesTblPtr[0].ComputedYet == false");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].NumBytesToChecksum == 0,
                  "CS_AppData.ResTablesTblPtr[0].NumBytesToChecksum == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].ComparisonValue == 0,
                  "CS_AppData.ResTablesTblPtr[0].ComparisonValue == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].ByteOffset == 0, "CS_AppData.ResTablesTblPtr[0].ByteOffset == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].TempChecksumValue == 0,
                  "CS_AppData.ResTablesTblPtr[0].TempChecksumValue == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].StartAddress == 0, "CS_AppData.ResTablesTblPtr[0].StartAddress == 0");

    UtAssert_True(CS_AppData.ResTablesTblPtr[0].TblHandle == 11, "CS_AppData.ResTablesTblPtr[0].TblHandle == 11");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].IsCSOwner == true, "CS_AppData.ResTablesTblPtr[0].IsCSOwner == true");
    UtAssert_True(strncmp(CS_AppData.ResTablesTblPtr[0].Name, "CS.ResMemoryTbl", 20) == 0,
                  "strncmp(CS_AppData.ResTablesTblPtr[0].Name, 'CS.ResMemoryTbl', 20) == 0");

    UtAssert_True(CS_AppData.HkPacket.TablesCSState == 99, "CS_AppData.HkPacket.TablesCSState == 99");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_ProcessNewTablesDefinitionTable_Test_ResMemoryTableHandle */

void CS_ProcessNewTablesDefinitionTable_Test_ResTablesTableHandle(void)
{
    CS_AppData.HkPacket.TablesCSState   = 99;
    CS_AppData.DefTablesTblPtr[0].State = 88;

    strncpy(CS_AppData.DefTablesTblPtr[0].Name, "CS.ResTablesTbl", 20);

    CS_AppData.ResTablesTableHandle = 11;

    /* Sets AppName to "CS" */
    UT_SetHandlerFunction(UT_KEY(CFE_ES_GetAppName), CS_TABLE_PROCESSING_TEST_CFE_ES_GetAppNameHandler1, NULL);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just
       pointers */
    CS_ProcessNewTablesDefinitionTable((CS_Def_Tables_Table_Entry_t *)&CS_AppData.DefTablesTblPtr,
                                       (CS_Res_Tables_Table_Entry_t *)&CS_AppData.ResTablesTblPtr);

    /* Verify results */
    UtAssert_True(CS_AppData.EepResTablesTblPtr == NULL, "CS_AppData.EepResTablesTblPtr == NULL");
    UtAssert_True(CS_AppData.MemResTablesTblPtr == NULL, "CS_AppData.MemResTablesTblPtr == NULL");
    UtAssert_True(CS_AppData.AppResTablesTblPtr == NULL, "CS_AppData.AppResTablesTblPtr == NULL");
    UtAssert_True(CS_AppData.TblResTablesTblPtr == NULL, "CS_AppData.TblResTablesTblPtr == NULL");

    UtAssert_True(CS_AppData.ResTablesTblPtr[0].State == 88, "CS_AppData.ResTablesTblPtr[0].State == 88");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].ComputedYet == false,
                  "CS_AppData.ResTablesTblPtr[0].ComputedYet == false");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].NumBytesToChecksum == 0,
                  "CS_AppData.ResTablesTblPtr[0].NumBytesToChecksum == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].ComparisonValue == 0,
                  "CS_AppData.ResTablesTblPtr[0].ComparisonValue == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].ByteOffset == 0, "CS_AppData.ResTablesTblPtr[0].ByteOffset == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].TempChecksumValue == 0,
                  "CS_AppData.ResTablesTblPtr[0].TempChecksumValue == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].StartAddress == 0, "CS_AppData.ResTablesTblPtr[0].StartAddress == 0");

    UtAssert_True(CS_AppData.ResTablesTblPtr[0].TblHandle == 11, "CS_AppData.ResTablesTblPtr[0].TblHandle == 11");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].IsCSOwner == true, "CS_AppData.ResTablesTblPtr[0].IsCSOwner == true");
    UtAssert_True(strncmp(CS_AppData.ResTablesTblPtr[0].Name, "CS.ResTablesTbl", 20) == 0,
                  "strncmp(CS_AppData.ResTablesTblPtr[0].Name, 'CS.ResTablesTbl', 20) == 0");

    UtAssert_True(CS_AppData.HkPacket.TablesCSState == 99, "CS_AppData.HkPacket.TablesCSState == 99");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_ProcessNewTablesDefinitionTable_Test_ResTablesTableHandle */

void CS_ProcessNewTablesDefinitionTable_Test_ResAppTableHandle(void)
{
    CS_AppData.HkPacket.TablesCSState   = 99;
    CS_AppData.DefTablesTblPtr[0].State = 88;

    strncpy(CS_AppData.DefTablesTblPtr[0].Name, "CS.ResAppTbl", 20);

    CS_AppData.ResAppTableHandle = 11;

    /* Sets AppName to "CS" */
    UT_SetHandlerFunction(UT_KEY(CFE_ES_GetAppName), CS_TABLE_PROCESSING_TEST_CFE_ES_GetAppNameHandler1, NULL);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just
       pointers */
    CS_ProcessNewTablesDefinitionTable((CS_Def_Tables_Table_Entry_t *)&CS_AppData.DefTablesTblPtr,
                                       (CS_Res_Tables_Table_Entry_t *)&CS_AppData.ResTablesTblPtr);

    /* Verify results */
    UtAssert_True(CS_AppData.EepResTablesTblPtr == NULL, "CS_AppData.EepResTablesTblPtr == NULL");
    UtAssert_True(CS_AppData.MemResTablesTblPtr == NULL, "CS_AppData.MemResTablesTblPtr == NULL");
    UtAssert_True(CS_AppData.AppResTablesTblPtr == NULL, "CS_AppData.AppResTablesTblPtr == NULL");
    UtAssert_True(CS_AppData.TblResTablesTblPtr == NULL, "CS_AppData.TblResTablesTblPtr == NULL");

    UtAssert_True(CS_AppData.ResTablesTblPtr[0].State == 88, "CS_AppData.ResTablesTblPtr[0].State == 88");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].ComputedYet == false,
                  "CS_AppData.ResTablesTblPtr[0].ComputedYet == false");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].NumBytesToChecksum == 0,
                  "CS_AppData.ResTablesTblPtr[0].NumBytesToChecksum == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].ComparisonValue == 0,
                  "CS_AppData.ResTablesTblPtr[0].ComparisonValue == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].ByteOffset == 0, "CS_AppData.ResTablesTblPtr[0].ByteOffset == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].TempChecksumValue == 0,
                  "CS_AppData.ResTablesTblPtr[0].TempChecksumValue == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].StartAddress == 0, "CS_AppData.ResTablesTblPtr[0].StartAddress == 0");

    UtAssert_True(CS_AppData.ResTablesTblPtr[0].TblHandle == 11, "CS_AppData.ResTablesTblPtr[0].TblHandle == 11");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].IsCSOwner == true, "CS_AppData.ResTablesTblPtr[0].IsCSOwner == true");
    UtAssert_True(strncmp(CS_AppData.ResTablesTblPtr[0].Name, "CS.ResAppTbl", 20) == 0,
                  "strncmp(CS_AppData.ResTablesTblPtr[0].Name, 'CS.ResAppTbl', 20) == 0");

    UtAssert_True(CS_AppData.HkPacket.TablesCSState == 99, "CS_AppData.HkPacket.TablesCSState == 99");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_ProcessNewTablesDefinitionTable_Test_ResAppTableHandle */

void CS_ProcessNewTablesDefinitionTable_Test_StateEmptyNoValidEntries(void)
{
    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "CS Tables Table: No valid entries in the table");

    CS_AppData.HkPacket.TablesCSState   = 99;
    CS_AppData.DefTablesTblPtr[0].State = CS_STATE_EMPTY;

    /* Sets AppName to "CS" */
    UT_SetHandlerFunction(UT_KEY(CFE_ES_GetAppName), CS_TABLE_PROCESSING_TEST_CFE_ES_GetAppNameHandler1, NULL);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just
       pointers */
    CS_ProcessNewTablesDefinitionTable((CS_Def_Tables_Table_Entry_t *)&CS_AppData.DefTablesTblPtr,
                                       (CS_Res_Tables_Table_Entry_t *)&CS_AppData.ResTablesTblPtr);

    /* Verify results */
    UtAssert_True(CS_AppData.EepResTablesTblPtr == NULL, "CS_AppData.EepResTablesTblPtr == NULL");
    UtAssert_True(CS_AppData.MemResTablesTblPtr == NULL, "CS_AppData.MemResTablesTblPtr == NULL");
    UtAssert_True(CS_AppData.AppResTablesTblPtr == NULL, "CS_AppData.AppResTablesTblPtr == NULL");
    UtAssert_True(CS_AppData.TblResTablesTblPtr == NULL, "CS_AppData.TblResTablesTblPtr == NULL");

    UtAssert_True(CS_AppData.ResTablesTblPtr[0].State == CS_STATE_EMPTY,
                  "CS_AppData.ResTablesTblPtr[0].State == CS_STATE_EMPTY");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].ComputedYet == false,
                  "CS_AppData.ResTablesTblPtr[0].ComputedYet == false");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].NumBytesToChecksum == 0,
                  "CS_AppData.ResTablesTblPtr[0].NumBytesToChecksum == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].ComparisonValue == 0,
                  "CS_AppData.ResTablesTblPtr[0].ComparisonValue == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].ByteOffset == 0, "CS_AppData.ResTablesTblPtr[0].ByteOffset == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].TempChecksumValue == 0,
                  "CS_AppData.ResTablesTblPtr[0].TempChecksumValue == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].StartAddress == 0, "CS_AppData.ResTablesTblPtr[0].StartAddress == 0");

    UtAssert_True(CS_AppData.ResTablesTblPtr[0].TblHandle == CFE_TBL_BAD_TABLE_HANDLE,
                  "CS_AppData.ResTablesTblPtr[0].TblHandle == CFE_TBL_BAD_TABLE_HANDLE");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].IsCSOwner == false, "CS_AppData.ResTablesTblPtr[0].IsCSOwner == false");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].Name[0] == 0, "CS_AppData.ResTablesTblPtr[0].Name[0] == 0");

    UtAssert_True(CS_AppData.HkPacket.TablesCSState == 99, "CS_AppData.HkPacket.TablesCSState == 99");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_PROCESS_TABLES_NO_ENTRIES_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_ProcessNewTablesDefinitionTable_Test_StateEmptyNoValidEntries */

void CS_ProcessNewTablesDefinitionTable_Test_LimitApplicationNameLength(void)
{
    uint16     i;
    const char AppNameX[] = "X";

    CS_AppData.HkPacket.TablesCSState   = 99;
    CS_AppData.DefTablesTblPtr[0].State = 88;

    /* String name chosen to be of length OS_MAX_API_NAME in order to satisfy condition "AppNameIndex ==
     * OS_MAX_API_NAME" */
    /* If intended branch is reached, name length will be truncated to length OS_MAX_API_NAME - 1 */
    for (i = 0; i <= OS_MAX_API_NAME; i++)
    {
        strncat(CS_AppData.DefTablesTblPtr[0].Name, "x", OS_MAX_API_NAME);
    }

    strncat(CS_AppData.DefTablesTblPtr[0].Name, ".DefEepromTbl", OS_MAX_API_NAME);

    CS_AppData.DefEepromTableHandle = 11;

    /* Sets AppName to string of x's of length OS_MAX_API_NAME */
    UT_SetDataBuffer(UT_KEY(CFE_ES_GetAppName), (uint8 *)AppNameX, sizeof(AppNameX), false);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just
       pointers */
    CS_ProcessNewTablesDefinitionTable((CS_Def_Tables_Table_Entry_t *)&CS_AppData.DefTablesTblPtr,
                                       (CS_Res_Tables_Table_Entry_t *)&CS_AppData.ResTablesTblPtr);

    /* Verify results */
    /* Note: This test is a special case where we're only interested in seeing that one branch was taken: "if
     * (AppNameIndex == OS_MAX_API_NAME)" */
    /* If branch was taken, line "CS_AppData.EepResTablesTblPtr = ResultsEntry" will not be reached, and so the
     * following assert will pass */
    UtAssert_True(CS_AppData.EepResTablesTblPtr != CS_AppData.ResTablesTblPtr,
                  "CS_AppData.EepResTablesTblPtr != CS_AppData.ResTablesTblPtr");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_ProcessNewTablesDefinitionTable_Test_LimitApplicationNameLength */

void CS_ProcessNewTablesDefinitionTable_Test_LimitTableNameLength(void)
{
    uint16 i;

    CS_AppData.HkPacket.TablesCSState   = 99;
    CS_AppData.DefTablesTblPtr[0].State = 88;

    strncat(CS_AppData.DefTablesTblPtr[0].Name, "CS.", CFE_TBL_MAX_FULL_NAME_LEN);

    /* String name chosen to be of length CFE_TBL_MAX_FULL_NAME_LEN in order to satisfy condition "TableNameIndex ==
     * CFE_TBL_MAX_FULL_NAME_LEN" */
    /* If intended branch is reached, name length will be truncated to length CFE_TBL_MAX_FULL_NAME_LEN - 1 */
    for (i = 0; i <= CFE_TBL_MAX_FULL_NAME_LEN; i++)
    {
        strncat(CS_AppData.DefTablesTblPtr[0].Name, "x", CFE_TBL_MAX_FULL_NAME_LEN);
    }

    CS_AppData.DefEepromTableHandle = 11;

    /* Sets AppName to "CS" */
    UT_SetHandlerFunction(UT_KEY(CFE_ES_GetAppName), CS_TABLE_PROCESSING_TEST_CFE_ES_GetAppNameHandler1, NULL);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just
       pointers */
    CS_ProcessNewTablesDefinitionTable((CS_Def_Tables_Table_Entry_t *)&CS_AppData.DefTablesTblPtr,
                                       (CS_Res_Tables_Table_Entry_t *)&CS_AppData.ResTablesTblPtr);

    /* Verify results */
    /* Note: This test is a special case where we're only interested in seeing that one branch was taken: "if
     * (TableNameIndex == CFE_TBL_MAX_FULL_NAME_LEN)" */
    /* If branch was taken, line "CS_AppData.EepResTablesTblPtr = ResultsEntry" will not be reached, and so the
     * following assert will pass */
    UtAssert_True(CS_AppData.EepResTablesTblPtr != CS_AppData.ResTablesTblPtr,
                  "CS_AppData.EepResTablesTblPtr != CS_AppData.ResTablesTblPtr");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_ProcessNewTablesDefinitionTable_Test_LimitTableNameLength */

void CS_ProcessNewTablesDefinitionTable_Test_MaxTableNameLength(void)
{
    uint16 i;

    CS_AppData.HkPacket.TablesCSState   = 99;
    CS_AppData.DefTablesTblPtr[0].State = 88;

    strncat(CS_AppData.DefTablesTblPtr[0].Name, "CS", CFE_TBL_MAX_FULL_NAME_LEN);

    /* String name chosen to be of length CFE_TBL_MAX_FULL_NAME_LEN in order to satisfy condition "TableNameIndex ==
     * CFE_TBL_MAX_FULL_NAME_LEN" */
    /* If intended branch is reached, name length will be truncated to length CFE_TBL_MAX_FULL_NAME_LEN - 1 */
    for (i = 0; i <= CFE_TBL_MAX_FULL_NAME_LEN + 1; i++)
    {
        strncat(CS_AppData.DefTablesTblPtr[0].Name, "x", CFE_TBL_MAX_FULL_NAME_LEN);
    }

    CS_AppData.DefEepromTableHandle = 11;

    /* Sets AppName to "CS" */
    UT_SetHandlerFunction(UT_KEY(CFE_ES_GetAppName), CS_TABLE_PROCESSING_TEST_CFE_ES_GetAppNameHandler1, NULL);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just
       pointers */
    CS_ProcessNewTablesDefinitionTable((CS_Def_Tables_Table_Entry_t *)&CS_AppData.DefTablesTblPtr,
                                       (CS_Res_Tables_Table_Entry_t *)&CS_AppData.ResTablesTblPtr);

    /* Verify results */
    /* Note: This test is a special case where we're only interested in seeing that one branch was taken: "if
     * (TableNameIndex == CFE_TBL_MAX_FULL_NAME_LEN)" */
    /* If branch was taken, line "CS_AppData.EepResTablesTblPtr = ResultsEntry" will not be reached, and so the
     * following assert will pass */
    UtAssert_True(CS_AppData.EepResTablesTblPtr != CS_AppData.ResTablesTblPtr,
                  "CS_AppData.EepResTablesTblPtr != CS_AppData.ResTablesTblPtr");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_ProcessNewTablesDefinitionTable_Test_MaxTableNameLength */

void CS_ProcessNewAppDefinitionTable_Test_Nominal(void)
{
    CS_AppData.HkPacket.AppCSState   = 99;
    CS_AppData.DefAppTblPtr[0].State = 88;

    strncpy(CS_AppData.DefAppTblPtr[0].Name, "name", 20);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just
       pointers */
    CS_ProcessNewAppDefinitionTable((CS_Def_App_Table_Entry_t *)&CS_AppData.DefAppTblPtr,
                                    (CS_Res_App_Table_Entry_t *)&CS_AppData.ResAppTblPtr);

    /* Verify results */
    UtAssert_True(CS_AppData.ResAppTblPtr[0].State == 88, "CS_AppData.ResAppTblPtr[0].State == 88");
    UtAssert_True(CS_AppData.ResAppTblPtr[0].ComputedYet == false, "CS_AppData.ResAppTblPtr[0].ComputedYet == false");
    UtAssert_True(CS_AppData.ResAppTblPtr[0].NumBytesToChecksum == 0,
                  "CS_AppData.ResAppTblPtr[0].NumBytesToChecksum == 0");
    UtAssert_True(CS_AppData.ResAppTblPtr[0].ComparisonValue == 0, "CS_AppData.ResAppTblPtr[0].ComparisonValue == 0");
    UtAssert_True(CS_AppData.ResAppTblPtr[0].ByteOffset == 0, "CS_AppData.ResAppTblPtr[0].ByteOffset == 0");
    UtAssert_True(CS_AppData.ResAppTblPtr[0].TempChecksumValue == 0,
                  "CS_AppData.ResAppTblPtr[0].TempChecksumValue == 0");
    UtAssert_True(CS_AppData.ResAppTblPtr[0].StartAddress == 0, "CS_AppData.ResAppTblPtr[0].StartAddress == 0");
    UtAssert_True(strncmp(CS_AppData.ResAppTblPtr[0].Name, "name", 20) == 0,
                  "strncmp(CS_AppData.ResAppTblPtr[0].Name, 'name', 20) == 0");

    UtAssert_True(CS_AppData.HkPacket.AppCSState == 99, "CS_AppData.HkPacket.AppCSState == 99");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_ProcessNewAppDefinitionTable_Test_Nominal */

void CS_ProcessNewAppDefinitionTable_Test_StateEmptyNoValidEntries(void)
{
    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "CS Apps Table: No valid entries in the table");

    CS_AppData.HkPacket.AppCSState   = 99;
    CS_AppData.DefAppTblPtr[0].State = CS_STATE_EMPTY;

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just
       pointers */
    CS_ProcessNewAppDefinitionTable((CS_Def_App_Table_Entry_t *)&CS_AppData.DefAppTblPtr,
                                    (CS_Res_App_Table_Entry_t *)&CS_AppData.ResAppTblPtr);

    /* Verify results */
    UtAssert_True(CS_AppData.ResAppTblPtr[0].State == CS_STATE_EMPTY,
                  "CS_AppData.ResAppTblPtr[0].State == CS_STATE_EMPTY");
    UtAssert_True(CS_AppData.ResAppTblPtr[0].ComputedYet == false, "CS_AppData.ResAppTblPtr[0].ComputedYet == false");
    UtAssert_True(CS_AppData.ResAppTblPtr[0].NumBytesToChecksum == 0,
                  "CS_AppData.ResAppTblPtr[0].NumBytesToChecksum == 0");
    UtAssert_True(CS_AppData.ResAppTblPtr[0].ComparisonValue == 0, "CS_AppData.ResAppTblPtr[0].ComparisonValue == 0");
    UtAssert_True(CS_AppData.ResAppTblPtr[0].ByteOffset == 0, "CS_AppData.ResAppTblPtr[0].ByteOffset == 0");
    UtAssert_True(CS_AppData.ResAppTblPtr[0].TempChecksumValue == 0,
                  "CS_AppData.ResAppTblPtr[0].TempChecksumValue == 0");
    UtAssert_True(CS_AppData.ResAppTblPtr[0].StartAddress == 0, "CS_AppData.ResAppTblPtr[0].StartAddress == 0");
    UtAssert_True(CS_AppData.ResAppTblPtr[0].Name[0] == 0, "CS_AppData.ResAppTblPtr[0].Name[0] == 0");

    UtAssert_True(CS_AppData.HkPacket.AppCSState == 99, "CS_AppData.HkPacket.AppCSState == 99");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_PROCESS_APP_NO_ENTRIES_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_ProcessNewAppDefinitionTable_Test_StateEmptyNoValidEntries */

void CS_TableInit_Test_DefaultDefinitionTableLoadErrorEEPROM(void)
{
    int32            Result;
    CFE_TBL_Handle_t DefinitionTableHandle = 0;
    CFE_TBL_Handle_t ResultsTableHandle    = 0;
    int32            strCmpResult;
    char             ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS received error 0x%%08X initializing Definition table for %%s");

    /* Set to cause load from the default tables */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Load), -1);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just
       pointers */
    Result = CS_TableInit(&DefinitionTableHandle, &ResultsTableHandle, &CS_AppData.DefEepromTblPtr,
                          &CS_AppData.ResEepromTblPtr, CS_EEPROM_TABLE, CS_DEF_EEPROM_TABLE_NAME,
                          CS_RESULTS_EEPROM_TABLE_NAME, CS_MAX_NUM_EEPROM_TABLE_ENTRIES, CS_DEF_EEPROM_TABLE_FILENAME,
                          &CS_AppData.DefaultEepromDefTable, sizeof(CS_Def_EepromMemory_Table_Entry_t),
                          sizeof(CS_Res_EepromMemory_Table_Entry_t), NULL);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_TBL_INIT_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(Result == -1, "Result == -1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_TableInit_Test_DefaultDefinitionTableLoadErrorEEPROM */

void CS_TableInit_Test_DefinitionTableGetAddressErrorEEPROM(void)
{
    int32            Result;
    CFE_TBL_Handle_t DefinitionTableHandle = 0;
    CFE_TBL_Handle_t ResultsTableHandle    = 0;
    int32            strCmpResult;
    char             ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS received error 0x%%08X initializing Definition table for %%s");

    /* Set to prevent unintended errors */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Load), CFE_SUCCESS);

    /* Set to fail condition "Result >= CFE_SUCCESS" after 2nd call to GetAddress */
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_GetAddress), 2, -1);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just
       pointers */
    Result = CS_TableInit(&DefinitionTableHandle, &ResultsTableHandle, &CS_AppData.DefEepromTblPtr,
                          &CS_AppData.ResEepromTblPtr, CS_EEPROM_TABLE, CS_DEF_EEPROM_TABLE_NAME,
                          CS_RESULTS_EEPROM_TABLE_NAME, CS_MAX_NUM_EEPROM_TABLE_ENTRIES, CS_DEF_EEPROM_TABLE_FILENAME,
                          &CS_AppData.DefaultEepromDefTable, sizeof(CS_Def_EepromMemory_Table_Entry_t),
                          sizeof(CS_Res_EepromMemory_Table_Entry_t), NULL);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_TBL_INIT_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(Result == -1, "Result == -1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_TableInit_Test_DefinitionTableGetAddressErrorEEPROM */

void CS_TableInit_Test_DefinitionTableGetAddressErrorMemory(void)
{
    int32            Result;
    CFE_TBL_Handle_t DefinitionTableHandle = 0;
    CFE_TBL_Handle_t ResultsTableHandle    = 0;
    int32            strCmpResult;
    char             ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS received error 0x%%08X initializing Definition table for %%s");

    /* Set to prevent unintended errors */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Load), CFE_SUCCESS);

    /* Set to fail condition "Result >= CFE_SUCCESS" after 2nd call to GetAddress */
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_GetAddress), 2, -1);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just
       pointers */
    Result = CS_TableInit(&DefinitionTableHandle, &ResultsTableHandle, &CS_AppData.DefMemoryTblPtr,
                          &CS_AppData.ResMemoryTblPtr, CS_MEMORY_TABLE, CS_DEF_MEMORY_TABLE_NAME,
                          CS_RESULTS_MEMORY_TABLE_NAME, CS_MAX_NUM_MEMORY_TABLE_ENTRIES, CS_DEF_MEMORY_TABLE_FILENAME,
                          &CS_AppData.DefaultMemoryDefTable, sizeof(CS_Def_EepromMemory_Table_Entry_t),
                          sizeof(CS_Res_EepromMemory_Table_Entry_t), NULL);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_TBL_INIT_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(Result == -1, "Result == -1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_TableInit_Test_DefinitionTableGetAddressErrorMemory */

void CS_TableInit_Test_DefinitionTableGetAddressErrorTables(void)
{
    int32            Result;
    CFE_TBL_Handle_t DefinitionTableHandle = 0;
    CFE_TBL_Handle_t ResultsTableHandle    = 0;
    int32            strCmpResult;
    char             ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS received error 0x%%08X initializing Definition table for %%s");

    /* Set to prevent unintended errors */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Load), CFE_SUCCESS);

    /* Set to fail condition "Result >= CFE_SUCCESS" after 2nd call to GetAddress */
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_GetAddress), 2, -1);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just
       pointers */
    Result = CS_TableInit(&DefinitionTableHandle, &ResultsTableHandle, &CS_AppData.DefTablesTblPtr,
                          &CS_AppData.ResTablesTblPtr, CS_TABLES_TABLE, CS_DEF_TABLES_TABLE_NAME,
                          CS_RESULTS_TABLES_TABLE_NAME, CS_MAX_NUM_TABLES_TABLE_ENTRIES, CS_DEF_TABLES_TABLE_FILENAME,
                          &CS_AppData.DefaultTablesDefTable, sizeof(CS_Def_Tables_Table_Entry_t),
                          sizeof(CS_Res_Tables_Table_Entry_t), NULL);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_TBL_INIT_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(Result == -1, "Result == -1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_TableInit_Test_DefinitionTableGetAddressErrorTables */

void CS_TableInit_Test_DefinitionTableGetAddressErrorApps(void)
{
    int32            Result;
    CFE_TBL_Handle_t DefinitionTableHandle = 0;
    CFE_TBL_Handle_t ResultsTableHandle    = 0;
    int32            strCmpResult;
    char             ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS received error 0x%%08X initializing Definition table for %%s");

    /* Set to prevent unintended errors */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Load), CFE_SUCCESS);

    /* Set to fail condition "Result >= CFE_SUCCESS" after 2nd call to GetAddress */
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_GetAddress), 2, -1);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just
       pointers */
    Result = CS_TableInit(&DefinitionTableHandle, &ResultsTableHandle, &CS_AppData.DefAppTblPtr,
                          &CS_AppData.ResAppTblPtr, CS_APP_TABLE, CS_DEF_APP_TABLE_NAME, CS_RESULTS_APP_TABLE_NAME,
                          CS_MAX_NUM_APP_TABLE_ENTRIES, CS_DEF_APP_TABLE_FILENAME, &CS_AppData.DefaultAppDefTable,
                          sizeof(CS_Def_App_Table_Entry_t), sizeof(CS_Res_App_Table_Entry_t), NULL);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_TBL_INIT_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(Result == -1, "Result == -1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_TableInit_Test_DefinitionTableGetAddressErrorApps */

void CS_TableInit_Test_EepromTableAndNotLoadedFromMemory(void)
{
    int32            Result;
    CFE_TBL_Handle_t DefinitionTableHandle = 0;
    CFE_TBL_Handle_t ResultsTableHandle    = 0;

    /* Set to prevent unintended errors */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Load), CFE_SUCCESS);

    /* Set to satisfy condition "Result == CFE_TBL_INFO_UPDATED" */
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_GetAddress), 2, CFE_TBL_INFO_UPDATED);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just
       pointers */
    Result = CS_TableInit(&DefinitionTableHandle, &ResultsTableHandle, &CS_AppData.DefEepromTblPtr,
                          &CS_AppData.ResEepromTblPtr, CS_EEPROM_TABLE, CS_DEF_EEPROM_TABLE_NAME,
                          CS_RESULTS_EEPROM_TABLE_NAME, CS_MAX_NUM_EEPROM_TABLE_ENTRIES, CS_DEF_EEPROM_TABLE_FILENAME,
                          &CS_AppData.DefaultEepromDefTable, sizeof(CS_Def_EepromMemory_Table_Entry_t),
                          sizeof(CS_Res_EepromMemory_Table_Entry_t), NULL);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
    /* Generates 1 error message we don't care about in this test */

} /* end CS_TableInit_Test_EepromTableAndNotLoadedFromMemory */

void CS_TableInit_Test_EepromTableAndLoadedFromMemoryAfterResultsTableRegisterError(void)
{
    int32            Result;
    CFE_TBL_Handle_t DefinitionTableHandle = 0;
    CFE_TBL_Handle_t ResultsTableHandle    = 0;

    /* Set to satisfy condition "ResultFromLoad != CFE_SUCCESS" */
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_Register), 1, -1);

    /* Set to prevent unintended errors */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Load), CFE_SUCCESS);

    /* Set to prevent unintended errors */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_SUCCESS);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just
       pointers */
    Result = CS_TableInit(&DefinitionTableHandle, &ResultsTableHandle, &CS_AppData.DefEepromTblPtr,
                          &CS_AppData.ResEepromTblPtr, CS_EEPROM_TABLE, CS_DEF_EEPROM_TABLE_NAME,
                          CS_RESULTS_EEPROM_TABLE_NAME, CS_MAX_NUM_EEPROM_TABLE_ENTRIES, CS_DEF_EEPROM_TABLE_FILENAME,
                          &CS_AppData.DefaultEepromDefTable, sizeof(CS_Def_EepromMemory_Table_Entry_t),
                          sizeof(CS_Res_EepromMemory_Table_Entry_t), NULL);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.EepromCSState == CS_STATE_DISABLED,
                  "CS_AppData.HkPacket.EepromCSState == CS_STATE_DISABLED");

    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_TableInit_Test_EepromTableAndLoadedFromMemoryAfterResultsTableRegisterError */

void CS_TableInit_Test_EepromTableAndLoadedFromMemoryAfterResultsTableGetAddressError(void)
{
    int32            Result;
    CFE_TBL_Handle_t DefinitionTableHandle = 0;
    CFE_TBL_Handle_t ResultsTableHandle    = 0;

    /* Set to prevent unintended errors */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Load), CFE_SUCCESS);

    /* On 1st call, return -1 to set LoadedFromMemory.  On 2nd call, return CFE_SUCCESS to prevent error */
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_GetAddress), 1, -1);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just
       pointers */
    Result = CS_TableInit(&DefinitionTableHandle, &ResultsTableHandle, &CS_AppData.DefEepromTblPtr,
                          &CS_AppData.ResEepromTblPtr, CS_EEPROM_TABLE, CS_DEF_EEPROM_TABLE_NAME,
                          CS_RESULTS_EEPROM_TABLE_NAME, CS_MAX_NUM_EEPROM_TABLE_ENTRIES, CS_DEF_EEPROM_TABLE_FILENAME,
                          &CS_AppData.DefaultEepromDefTable, sizeof(CS_Def_EepromMemory_Table_Entry_t),
                          sizeof(CS_Res_EepromMemory_Table_Entry_t), NULL);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.EepromCSState == CS_STATE_DISABLED,
                  "CS_AppData.HkPacket.EepromCSState == CS_STATE_DISABLED");

    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_TableInit_Test_EepromTableAndLoadedFromMemoryAfterResultsTableGetAddressError */

void CS_TableInit_Test_EepromTableAndLoadedFromMemoryAfterDefinitionTableRegisterError(void)
{
    int32            Result;
    CFE_TBL_Handle_t DefinitionTableHandle = 0;
    CFE_TBL_Handle_t ResultsTableHandle    = 0;

    /* Set to satisfy condition "ResultFromLoad != CFE_SUCCESS" on 2nd call to TBL_Register */
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_Register), 2, -1);

    /* Set to prevent unintended errors */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Load), CFE_SUCCESS);

    /* Set to prevent unintended errors */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_SUCCESS);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just
       pointers */
    Result = CS_TableInit(&DefinitionTableHandle, &ResultsTableHandle, &CS_AppData.DefEepromTblPtr,
                          &CS_AppData.ResEepromTblPtr, CS_EEPROM_TABLE, CS_DEF_EEPROM_TABLE_NAME,
                          CS_RESULTS_EEPROM_TABLE_NAME, CS_MAX_NUM_EEPROM_TABLE_ENTRIES, CS_DEF_EEPROM_TABLE_FILENAME,
                          &CS_AppData.DefaultEepromDefTable, sizeof(CS_Def_EepromMemory_Table_Entry_t),
                          sizeof(CS_Res_EepromMemory_Table_Entry_t), NULL);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.EepromCSState == CS_STATE_DISABLED,
                  "CS_AppData.HkPacket.EepromCSState == CS_STATE_DISABLED");

    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_TableInit_Test_EepromTableAndLoadedFromMemoryAfterDefinitionTableRegisterError */

void CS_TableInit_Test_EepromTableAndLoadedFromMemoryAfterDefinitionTableFileLoadError(void)
{
    int32            Result;
    CFE_TBL_Handle_t DefinitionTableHandle = 0;
    CFE_TBL_Handle_t ResultsTableHandle    = 0;

    /* On 1st call, return -1 to satisfy condition "ResultFromLoad != CFE_SUCCESS".  On 2nd call, return CFE_SUCCESS to
     * prevent error */
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_Load), 1, -1);

    /* Set to prevent unintended errors */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_SUCCESS);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just
       pointers */
    Result = CS_TableInit(&DefinitionTableHandle, &ResultsTableHandle, &CS_AppData.DefEepromTblPtr,
                          &CS_AppData.ResEepromTblPtr, CS_EEPROM_TABLE, CS_DEF_EEPROM_TABLE_NAME,
                          CS_RESULTS_EEPROM_TABLE_NAME, CS_MAX_NUM_EEPROM_TABLE_ENTRIES, CS_DEF_EEPROM_TABLE_FILENAME,
                          &CS_AppData.DefaultEepromDefTable, sizeof(CS_Def_EepromMemory_Table_Entry_t),
                          sizeof(CS_Res_EepromMemory_Table_Entry_t), NULL);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.EepromCSState == CS_STATE_DISABLED,
                  "CS_AppData.HkPacket.EepromCSState == CS_STATE_DISABLED");

    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_TableInit_Test_EepromTableAndLoadedFromMemoryAfterDefinitionTableFileLoadError */

void CS_TableInit_Test_MemoryTableAndNotLoadedFromMemory(void)
{
    int32            Result;
    CFE_TBL_Handle_t DefinitionTableHandle = 0;
    CFE_TBL_Handle_t ResultsTableHandle    = 0;

    /* Set to prevent unintended errors */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Load), CFE_SUCCESS);

    /* Set to satisfy condition "Result == CFE_TBL_INFO_UPDATED" */
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_GetAddress), 2, CFE_TBL_INFO_UPDATED);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just
       pointers */
    Result = CS_TableInit(&DefinitionTableHandle, &ResultsTableHandle, &CS_AppData.DefMemoryTblPtr,
                          &CS_AppData.ResMemoryTblPtr, CS_MEMORY_TABLE, CS_DEF_MEMORY_TABLE_NAME,
                          CS_RESULTS_MEMORY_TABLE_NAME, CS_MAX_NUM_MEMORY_TABLE_ENTRIES, CS_DEF_MEMORY_TABLE_FILENAME,
                          &CS_AppData.DefaultMemoryDefTable, sizeof(CS_Def_EepromMemory_Table_Entry_t),
                          sizeof(CS_Res_EepromMemory_Table_Entry_t), NULL);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
    /* Generates 1 error message we don't care about in this test */

} /* end CS_TableInit_Test_MemoryTableAndNotLoadedFromMemory */

void CS_TableInit_Test_MemoryTableAndLoadedFromMemory(void)
{
    int32            Result;
    CFE_TBL_Handle_t DefinitionTableHandle = 0;
    CFE_TBL_Handle_t ResultsTableHandle    = 0;

    /* Set to satisfy condition "ResultFromLoad != CFE_SUCCESS" */
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_Register), 1, -1);

    /* Set to prevent unintended errors */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Load), CFE_SUCCESS);

    /* Set to prevent unintended errors */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_SUCCESS);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just
       pointers */
    Result = CS_TableInit(&DefinitionTableHandle, &ResultsTableHandle, &CS_AppData.DefMemoryTblPtr,
                          &CS_AppData.ResMemoryTblPtr, CS_MEMORY_TABLE, CS_DEF_MEMORY_TABLE_NAME,
                          CS_RESULTS_MEMORY_TABLE_NAME, CS_MAX_NUM_MEMORY_TABLE_ENTRIES, CS_DEF_MEMORY_TABLE_FILENAME,
                          &CS_AppData.DefaultMemoryDefTable, sizeof(CS_Def_EepromMemory_Table_Entry_t),
                          sizeof(CS_Res_EepromMemory_Table_Entry_t), NULL);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.MemoryCSState == CS_STATE_DISABLED,
                  "CS_AppData.HkPacket.MemoryCSState == CS_STATE_DISABLED");

    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_TableInit_Test_MemoryTableAndLoadedFromMemory */

void CS_TableInit_Test_AppTableAndNotLoadedFromMemory(void)
{
    int32            Result;
    CFE_TBL_Handle_t DefinitionTableHandle = 0;
    CFE_TBL_Handle_t ResultsTableHandle    = 0;

    /* Set to prevent unintended errors */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Load), CFE_SUCCESS);

    /* Set to satisfy condition "Result == CFE_TBL_INFO_UPDATED" */
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_GetAddress), 2, CFE_TBL_INFO_UPDATED);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just
       pointers */
    Result = CS_TableInit(&DefinitionTableHandle, &ResultsTableHandle, &CS_AppData.DefAppTblPtr,
                          &CS_AppData.ResAppTblPtr, CS_APP_TABLE, CS_DEF_APP_TABLE_NAME, CS_RESULTS_APP_TABLE_NAME,
                          CS_MAX_NUM_APP_TABLE_ENTRIES, CS_DEF_APP_TABLE_FILENAME, &CS_AppData.DefaultAppDefTable,
                          sizeof(CS_Def_App_Table_Entry_t), sizeof(CS_Res_App_Table_Entry_t), NULL);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
    /* Generates 1 event message we don't care about in this test */

} /* end CS_TableInit_Test_AppTableAndNotLoadedFromMemory */

void CS_TableInit_Test_AppTableAndLoadedFromMemory(void)
{
    int32            Result;
    CFE_TBL_Handle_t DefinitionTableHandle = 0;
    CFE_TBL_Handle_t ResultsTableHandle    = 0;

    /* Set to satisfy condition "ResultFromLoad != CFE_SUCCESS" */
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_Register), 1, -1);

    /* Set to prevent unintended errors */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Load), CFE_SUCCESS);

    /* Set to prevent unintended errors */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_SUCCESS);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just
       pointers */
    Result = CS_TableInit(&DefinitionTableHandle, &ResultsTableHandle, &CS_AppData.DefAppTblPtr,
                          &CS_AppData.ResAppTblPtr, CS_APP_TABLE, CS_DEF_APP_TABLE_NAME, CS_RESULTS_APP_TABLE_NAME,
                          CS_MAX_NUM_APP_TABLE_ENTRIES, CS_DEF_APP_TABLE_FILENAME, &CS_AppData.DefaultAppDefTable,
                          sizeof(CS_Def_App_Table_Entry_t), sizeof(CS_Res_App_Table_Entry_t), NULL);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.AppCSState == CS_STATE_DISABLED,
                  "CS_AppData.HkPacket.AppCSState == CS_STATE_DISABLED");

    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_TableInit_Test_AppTableAndLoadedFromMemory */

void CS_TableInit_Test_TablesTableAndNotLoadedFromMemory(void)
{
    int32            Result;
    CFE_TBL_Handle_t DefinitionTableHandle = 0;
    CFE_TBL_Handle_t ResultsTableHandle    = 0;

    /* Set to prevent unintended errors */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Load), CFE_SUCCESS);

    /* Set to satisfy condition "Result == CFE_TBL_INFO_UPDATED" */
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_GetAddress), 2, CFE_TBL_INFO_UPDATED);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just
       pointers */
    Result = CS_TableInit(&DefinitionTableHandle, &ResultsTableHandle, &CS_AppData.DefTablesTblPtr,
                          &CS_AppData.ResTablesTblPtr, CS_TABLES_TABLE, CS_DEF_TABLES_TABLE_NAME,
                          CS_RESULTS_TABLES_TABLE_NAME, CS_MAX_NUM_TABLES_TABLE_ENTRIES, CS_DEF_TABLES_TABLE_FILENAME,
                          &CS_AppData.DefaultTablesDefTable, sizeof(CS_Def_Tables_Table_Entry_t),
                          sizeof(CS_Res_Tables_Table_Entry_t), NULL);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
    /* Generates 1 error message we don't care about in this test */

} /* end CS_TableInit_Test_TablesTableAndNotLoadedFromMemory */

void CS_TableInit_Test_TablesTableAndLoadedFromMemory(void)
{
    int32            Result;
    CFE_TBL_Handle_t DefinitionTableHandle = 0;
    CFE_TBL_Handle_t ResultsTableHandle    = 0;

    /* Set to satisfy condition "ResultFromLoad != CFE_SUCCESS" */
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_Register), 1, -1);

    /* Set to prevent unintended errors */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Load), CFE_SUCCESS);

    /* Set to prevent unintended errors */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_SUCCESS);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just
       pointers */
    Result = CS_TableInit(&DefinitionTableHandle, &ResultsTableHandle, &CS_AppData.DefTablesTblPtr,
                          &CS_AppData.ResTablesTblPtr, CS_TABLES_TABLE, CS_DEF_TABLES_TABLE_NAME,
                          CS_RESULTS_TABLES_TABLE_NAME, CS_MAX_NUM_TABLES_TABLE_ENTRIES, CS_DEF_TABLES_TABLE_FILENAME,
                          &CS_AppData.DefaultTablesDefTable, sizeof(CS_Def_Tables_Table_Entry_t),
                          sizeof(CS_Res_Tables_Table_Entry_t), NULL);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.TablesCSState == CS_STATE_DISABLED,
                  "CS_AppData.HkPacket.TablesCSState == CS_STATE_DISABLED");

    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_TableInit_Test_TablesTableAndLoadedFromMemory */

void CS_TableInit_Test_DefaultAndLoadedFromMemory(void)
{
    int32            Result;
    CFE_TBL_Handle_t DefinitionTableHandle = 0;
    CFE_TBL_Handle_t ResultsTableHandle    = 0;

    /* Set to satisfy condition "ResultFromLoad != CFE_SUCCESS" */
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_Register), 1, -1);

    /* Set to prevent unintended errors */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Load), CFE_SUCCESS);

    /* Set to prevent unintended errors */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_SUCCESS);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just
       pointers */
    Result =
        CS_TableInit(&DefinitionTableHandle, &ResultsTableHandle, &CS_AppData.DefTablesTblPtr,
                     &CS_AppData.ResTablesTblPtr, 99, CS_DEF_TABLES_TABLE_NAME, CS_RESULTS_TABLES_TABLE_NAME,
                     CS_MAX_NUM_TABLES_TABLE_ENTRIES, CS_DEF_TABLES_TABLE_FILENAME, &CS_AppData.DefaultTablesDefTable,
                     sizeof(CS_Def_Tables_Table_Entry_t), sizeof(CS_Res_Tables_Table_Entry_t), NULL);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.TablesCSState == CS_STATE_EMPTY,
                  "CS_AppData.HkPacket.TablesCSState == CS_STATE_EMPTY");

    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_TableInit_Test_DefaultAndLoadedFromMemory */

void CS_TableInit_Test_OpenCreateError(void)
{
    int32            Result;
    CFE_TBL_Handle_t DefinitionTableHandle = 0;
    CFE_TBL_Handle_t ResultsTableHandle    = 0;

    /* Set to satisfy condition "ResultFromLoad != CFE_SUCCESS" */
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_Register), 1, CFE_SUCCESS);

    /* Set to prevent unintended errors */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Load), CFE_SUCCESS);

    /* Set to prevent unintended errors */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_SUCCESS);

    UT_SetDefaultReturnValue(UT_KEY(OS_OpenCreate), -1);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just
       pointers */
    Result =
        CS_TableInit(&DefinitionTableHandle, &ResultsTableHandle, &CS_AppData.DefTablesTblPtr,
                     &CS_AppData.ResTablesTblPtr, 99, CS_DEF_TABLES_TABLE_NAME, CS_RESULTS_TABLES_TABLE_NAME,
                     CS_MAX_NUM_TABLES_TABLE_ENTRIES, CS_DEF_TABLES_TABLE_FILENAME, &CS_AppData.DefaultTablesDefTable,
                     sizeof(CS_Def_Tables_Table_Entry_t), sizeof(CS_Res_Tables_Table_Entry_t), NULL);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.TablesCSState == CS_STATE_EMPTY,
                  "CS_AppData.HkPacket.TablesCSState == CS_STATE_EMPTY");

    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_TableInit_Test_OpenCreateError */

void CS_HandleTableUpdate_Test_ProcessNewTablesDefinitionTable(void)
{
    int32            Result;
    CFE_TBL_Handle_t DefinitionTableHandle = 0;
    CFE_TBL_Handle_t ResultsTableHandle    = 0;
    uint16           Table                 = CS_TABLES_TABLE;

    /* On 1st call, return CFE_SUCCESS to prevent error.  On 2nd call, return UT_CFE_TBL_GETADDRESS_INDEX to satisfy
     * condition "Result == CFE_TBL_INFO_UPDATED". */
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_GetAddress), 2, CFE_TBL_INFO_UPDATED);

    CS_AppData.ResTablesTblPtr[0].TblHandle = 99;
    CS_AppData.ResTablesTblPtr[0].IsCSOwner = false;

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just
       pointers */
    Result = CS_HandleTableUpdate(&CS_AppData.DefTablesTblPtr, &CS_AppData.ResTablesTblPtr, DefinitionTableHandle,
                                  ResultsTableHandle, Table, CS_MAX_NUM_TABLES_TABLE_ENTRIES);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
    /* Generates 1 event message we don't care about in this test */

} /* end CS_HandleTableUpdate_Test_ProcessNewTablesDefinitionTable */

void CS_HandleTableUpdate_Test_ProcessNewAppDefinitionTable(void)
{
    int32            Result;
    CFE_TBL_Handle_t DefinitionTableHandle = 0;
    CFE_TBL_Handle_t ResultsTableHandle    = 0;
    uint16           Table                 = CS_APP_TABLE;
    uint16           NumEntries            = 0;

    /* On 1st call, return CFE_SUCCESS to prevent error.  On 2nd call, return UT_CFE_TBL_GETADDRESS_INDEX to satisfy
     * condition "Result == CFE_TBL_INFO_UPDATED". */
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_GetAddress), 2, CFE_TBL_INFO_UPDATED);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just
       pointers */
    Result = CS_HandleTableUpdate(&CS_AppData.DefAppTblPtr, &CS_AppData.ResAppTblPtr, DefinitionTableHandle,
                                  ResultsTableHandle, Table, NumEntries);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
    /* Generates 1 error message we don't care about in this test */

} /* end CS_HandleTableUpdate_Test_ProcessNewAppDefinitionTable */

void CS_HandleTableUpdate_Test_ProcessNewEepromMemoryDefinitionTable(void)
{
    int32            Result;
    CFE_TBL_Handle_t DefinitionTableHandle = 0;
    CFE_TBL_Handle_t ResultsTableHandle    = 0;
    uint16           Table                 = CS_EEPROM_TABLE;
    uint16           NumEntries            = CS_MAX_NUM_EEPROM_TABLE_ENTRIES;

    /* On 1st call, return CFE_SUCCESS to prevent error.  On 2nd call, return UT_CFE_TBL_GETADDRESS_INDEX to satisfy
     * condition "Result == CFE_TBL_INFO_UPDATED". */
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_GetAddress), 2, CFE_TBL_INFO_UPDATED);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just
       pointers */
    Result = CS_HandleTableUpdate(&CS_AppData.DefEepromTblPtr, &CS_AppData.ResEepromTblPtr, DefinitionTableHandle,
                                  ResultsTableHandle, Table, NumEntries);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
    /* Generates 1 error message we don't care about in this test */

} /* end CS_HandleTableUpdate_Test_ProcessNewEepromMemoryDefinitionTable */

void CS_HandleTableUpdate_Test_ResultsTableGetAddressErrorEEPROM(void)
{
    int32            Result;
    CFE_TBL_Handle_t DefinitionTableHandle = 0;
    CFE_TBL_Handle_t ResultsTableHandle    = 0;
    uint16           Table                 = CS_EEPROM_TABLE;
    uint16           NumEntries            = 0;
    int32            strCmpResult;
    char             ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH * 2];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH * 2,
             "CS had problems updating table. Res Release: 0x%%08X Def Release:0x%%08X Res Manage:0x%%08X Def Manage: "
             "0x%%08X Get:0x%%08X for table %%s");

    /* Set to generate error message CS_TBL_UPDATE_ERR_EID */
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_GetAddress), 1, -1);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just
       pointers */
    Result = CS_HandleTableUpdate(&CS_AppData.DefEepromTblPtr, &CS_AppData.ResEepromTblPtr, DefinitionTableHandle,
                                  ResultsTableHandle, Table, NumEntries);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_TBL_UPDATE_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    /* Ignore last char in case it was truncated */
    strCmpResult =
        strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH - 1);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(Result == -1, "Result == -1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
    /* Generates 1 error message we don't care about in this test */

} /* end CS_HandleTableUpdate_Test_ResultsTableGetAddressErrorEEPROM */

void CS_HandleTableUpdate_Test_DefinitionTableGetAddressErrorEEPROM(void)
{
    int32            Result;
    CFE_TBL_Handle_t DefinitionTableHandle = 0;
    CFE_TBL_Handle_t ResultsTableHandle    = 0;
    uint16           Table                 = CS_EEPROM_TABLE;
    uint16           NumEntries            = 0;
    int32            strCmpResult;
    char             ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH * 2];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH * 2,
             "CS had problems updating table. Res Release: 0x%%08X Def Release:0x%%08X Res Manage:0x%%08X Def Manage: "
             "0x%%08X Get:0x%%08X for table %%s");

    /* Set to generate error message CS_TBL_UPDATE_ERR_EID.  Also prevent issues by returning CFE_SUCCESS on all calls
     * except the 2nd. */
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_GetAddress), 2, -1);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just
       pointers */
    Result = CS_HandleTableUpdate(&CS_AppData.DefEepromTblPtr, &CS_AppData.ResEepromTblPtr, DefinitionTableHandle,
                                  ResultsTableHandle, Table, NumEntries);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_TBL_UPDATE_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    /* Ignore last char in case it was truncated */
    strCmpResult =
        strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH - 1);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(Result == -1, "Result == -1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
    /* Generates 1 error message we don't care about in this test */

} /* end CS_HandleTableUpdate_Test_DefinitionTableGetAddressErrorEEPROM */

void CS_HandleTableUpdate_Test_DefinitionTableGetAddressErrorMemory(void)
{
    int32            Result;
    CFE_TBL_Handle_t DefinitionTableHandle = 0;
    CFE_TBL_Handle_t ResultsTableHandle    = 0;
    uint16           Table                 = CS_MEMORY_TABLE;
    uint16           NumEntries            = 0;
    int32            strCmpResult;
    char             ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH * 2];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH * 2,
             "CS had problems updating table. Res Release: 0x%%08X Def Release:0x%%08X Res Manage:0x%%08X Def Manage: "
             "0x%%08X Get:0x%%08X for table %%s");

    /* Set to generate error message CS_TBL_UPDATE_ERR_EID.  Also prevent issues by returning CFE_SUCCESS on all calls
     * except the 2nd. */
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_GetAddress), 2, -1);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just
       pointers */
    Result = CS_HandleTableUpdate(&CS_AppData.DefMemoryTblPtr, &CS_AppData.ResMemoryTblPtr, DefinitionTableHandle,
                                  ResultsTableHandle, Table, NumEntries);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_TBL_UPDATE_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    /* Ignore last char in case it was truncated */
    strCmpResult =
        strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH - 1);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(Result == -1, "Result == -1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
    /* Generates 1 error message we don't care about in this test */

} /* end CS_HandleTableUpdate_Test_DefinitionTableGetAddressErrorMemory */

void CS_HandleTableUpdate_Test_DefinitionTableGetAddressErrorTables(void)
{
    int32            Result;
    CFE_TBL_Handle_t DefinitionTableHandle = 0;
    CFE_TBL_Handle_t ResultsTableHandle    = 0;
    uint16           Table                 = CS_TABLES_TABLE;
    uint16           NumEntries            = 0;
    int32            strCmpResult;
    char             ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH * 2];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH * 2,
             "CS had problems updating table. Res Release: 0x%%08X Def Release:0x%%08X Res Manage:0x%%08X Def Manage: "
             "0x%%08X Get:0x%%08X for table %%s");

    /* Set to generate error message CS_TBL_UPDATE_ERR_EID.  Also prevent issues by returning CFE_SUCCESS on all calls
     * except the 2nd. */
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_GetAddress), 2, -1);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just
       pointers */
    Result = CS_HandleTableUpdate(&CS_AppData.DefTablesTblPtr, &CS_AppData.ResTablesTblPtr, DefinitionTableHandle,
                                  ResultsTableHandle, Table, NumEntries);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_TBL_UPDATE_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    /* Ignore last char in case it was truncated */
    strCmpResult =
        strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH - 1);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(Result == -1, "Result == -1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
    /* Generates 1 error message we don't care about in this test */

} /* end CS_HandleTableUpdate_Test_DefinitionTableGetAddressErrorTables */

void CS_HandleTableUpdate_Test_DefinitionTableGetAddressErrorApps(void)
{
    int32            Result;
    CFE_TBL_Handle_t DefinitionTableHandle = 0;
    CFE_TBL_Handle_t ResultsTableHandle    = 0;
    uint16           Table                 = CS_APP_TABLE;
    uint16           NumEntries            = 0;
    int32            strCmpResult;
    char             ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH * 2];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH * 2,
             "CS had problems updating table. Res Release: 0x%%08X Def Release:0x%%08X Res Manage:0x%%08X Def Manage: "
             "0x%%08X Get:0x%%08X for table %%s");

    /* Set to generate error message CS_TBL_UPDATE_ERR_EID.  Also prevent issues by returning CFE_SUCCESS on all calls
     * except the 2nd. */
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_GetAddress), 2, -1);

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just
       pointers */
    Result = CS_HandleTableUpdate(&CS_AppData.DefAppTblPtr, &CS_AppData.ResAppTblPtr, DefinitionTableHandle,
                                  ResultsTableHandle, Table, NumEntries);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_TBL_UPDATE_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    /* Ignore last char in case it was truncated */
    strCmpResult =
        strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH - 1);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_True(Result == -1, "Result == -1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
    /* Generates 1 error message we don't care about in this test */

} /* end CS_HandleTableUpdate_Test_DefinitionTableGetAddressErrorApp */

void CS_HandleTableUpdate_Test_BadTableHandle(void)
{
    int32            Result;
    CFE_TBL_Handle_t DefinitionTableHandle = 0;
    CFE_TBL_Handle_t ResultsTableHandle    = 0;
    uint16           Table                 = CS_TABLES_TABLE;

    /* On 1st call, return CFE_SUCCESS to prevent error.  On 2nd call, return UT_CFE_TBL_GETADDRESS_INDEX to satisfy
     * condition "Result == CFE_TBL_INFO_UPDATED". */
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_GetAddress), 2, CFE_TBL_INFO_UPDATED);

    CS_AppData.ResTablesTblPtr[0].TblHandle = CFE_TBL_BAD_TABLE_HANDLE;
    CS_AppData.ResTablesTblPtr[0].IsCSOwner = true;

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just
       pointers */
    Result = CS_HandleTableUpdate(&CS_AppData.DefTablesTblPtr, &CS_AppData.ResTablesTblPtr, DefinitionTableHandle,
                                  ResultsTableHandle, Table, CS_MAX_NUM_TABLES_TABLE_ENTRIES);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
    /* Generates 1 event message we don't care about in this test */

} /* end CS_HandleTableUpdate_Test_BadTableHandle */

void CS_HandleTableUpdate_Test_CsOwner(void)
{
    int32            Result;
    CFE_TBL_Handle_t DefinitionTableHandle = 0;
    CFE_TBL_Handle_t ResultsTableHandle    = 0;
    uint16           Table                 = CS_TABLES_TABLE;

    /* On 1st call, return CFE_SUCCESS to prevent error.  On 2nd call, return UT_CFE_TBL_GETADDRESS_INDEX to satisfy
     * condition "Result == CFE_TBL_INFO_UPDATED". */
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_GetAddress), 2, CFE_TBL_INFO_UPDATED);

    CS_AppData.ResTablesTblPtr[0].TblHandle = 99;
    CS_AppData.ResTablesTblPtr[0].IsCSOwner = true;

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just
       pointers */
    Result = CS_HandleTableUpdate(&CS_AppData.DefTablesTblPtr, &CS_AppData.ResTablesTblPtr, DefinitionTableHandle,
                                  ResultsTableHandle, Table, CS_MAX_NUM_TABLES_TABLE_ENTRIES);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
    /* Generates 1 event message we don't care about in this test */

} /* end CS_HandleTableUpdate_Test_CsOwner */

void CS_HandleTableUpdate_Test_GetAddressError(void)
{
    int32            Result;
    CFE_TBL_Handle_t DefinitionTableHandle = 0;
    CFE_TBL_Handle_t ResultsTableHandle    = 0;
    uint16           Table                 = CS_TABLES_TABLE;

    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_GetAddress), 1, 1);

    CS_AppData.ResTablesTblPtr[0].TblHandle = 99;
    CS_AppData.ResTablesTblPtr[0].IsCSOwner = true;

    /* Execute the function being tested */
    /* Note: first 2 arguments are passed in as addresses of pointers in the source code, even though the variable
       types of the arguments are just pointers and the variable names of the arguments suggest that they're just
       pointers */
    Result = CS_HandleTableUpdate(&CS_AppData.DefTablesTblPtr, &CS_AppData.ResTablesTblPtr, DefinitionTableHandle,
                                  ResultsTableHandle, Table, CS_MAX_NUM_TABLES_TABLE_ENTRIES);

    /* Verify results */
    UtAssert_True(Result == 1, "Result == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_HandleTableUpdate_Test_GetAddressError */

void UtTest_Setup(void)
{
    UtTest_Add(CS_ValidateEepromChecksumDefinitionTable_Test_Nominal, CS_Test_Setup, CS_Test_TearDown,
               "CS_ValidateEepromChecksumDefinitionTable_Test_Nominal");
    UtTest_Add(CS_ValidateEepromChecksumDefinitionTable_Test_IllegalChecksumRangeStateEnabled, CS_Test_Setup,
               CS_Test_TearDown, "CS_ValidateEepromChecksumDefinitionTable_Test_IllegalChecksumRangeStateEnabled");
    UtTest_Add(CS_ValidateEepromChecksumDefinitionTable_Test_IllegalChecksumRangeStateDisabled, CS_Test_Setup,
               CS_Test_TearDown, "CS_ValidateEepromChecksumDefinitionTable_Test_IllegalChecksumRangeStateDisabled");
    UtTest_Add(CS_ValidateEepromChecksumDefinitionTable_Test_IllegalStateField, CS_Test_Setup, CS_Test_TearDown,
               "CS_ValidateEepromChecksumDefinitionTable_Test_IllegalStateField");
    UtTest_Add(CS_ValidateEepromChecksumDefinitionTable_Test_TableErrorResult, CS_Test_Setup, CS_Test_TearDown,
               "CS_ValidateEepromChecksumDefinitionTable_Test_TableErrorResult");
    UtTest_Add(CS_ValidateEepromChecksumDefinitionTable_Test_UndefTableErrorResult, CS_Test_Setup, CS_Test_TearDown,
               "CS_ValidateEepromChecksumDefinitionTable_Test_UndefTableErrorResult");

    UtTest_Add(CS_ValidateMemoryChecksumDefinitionTable_Test_Nominal, CS_Test_Setup, CS_Test_TearDown,
               "CS_ValidateMemoryChecksumDefinitionTable_Test_Nominal");
    UtTest_Add(CS_ValidateMemoryChecksumDefinitionTable_Test_IllegalChecksumRangeStateEnabled, CS_Test_Setup,
               CS_Test_TearDown, "CS_ValidateMemoryChecksumDefinitionTable_Test_IllegalChecksumRangeStateEnabled");
    UtTest_Add(CS_ValidateMemoryChecksumDefinitionTable_Test_IllegalChecksumRangeStateDisabled, CS_Test_Setup,
               CS_Test_TearDown, "CS_ValidateMemoryChecksumDefinitionTable_Test_IllegalChecksumRangeStateDisabled");
    UtTest_Add(CS_ValidateMemoryChecksumDefinitionTable_Test_IllegalStateField, CS_Test_Setup, CS_Test_TearDown,
               "CS_ValidateMemoryChecksumDefinitionTable_Test_IllegalStateField");
    UtTest_Add(CS_ValidateMemoryChecksumDefinitionTable_Test_TableErrorResult, CS_Test_Setup, CS_Test_TearDown,
               "CS_ValidateMemoryChecksumDefinitionTable_Test_TableErrorResult");
    UtTest_Add(CS_ValidateMemoryChecksumDefinitionTable_Test_UndefTableErrorResult, CS_Test_Setup, CS_Test_TearDown,
               "CS_ValidateMemoryChecksumDefinitionTable_Test_UndefTableErrorResult");

    UtTest_Add(CS_ValidateTablesChecksumDefinitionTable_Test_Nominal, CS_Test_Setup, CS_Test_TearDown,
               "CS_ValidateTablesChecksumDefinitionTable_Test_Nominal");
    UtTest_Add(CS_ValidateTablesChecksumDefinitionTable_Test_DuplicateNameStateEmpty, CS_Test_Setup, CS_Test_TearDown,
               "CS_ValidateTablesChecksumDefinitionTable_Test_DuplicateNameStateEmpty");
    UtTest_Add(CS_ValidateTablesChecksumDefinitionTable_Test_DuplicateNameStateEnabled, CS_Test_Setup, CS_Test_TearDown,
               "CS_ValidateTablesChecksumDefinitionTable_Test_DuplicateNameStateEnabled");
    UtTest_Add(CS_ValidateTablesChecksumDefinitionTable_Test_DuplicateNameStateDisabled, CS_Test_Setup,
               CS_Test_TearDown, "CS_ValidateTablesChecksumDefinitionTable_Test_DuplicateNameStateDisabled");
    UtTest_Add(CS_ValidateTablesChecksumDefinitionTable_Test_IllegalStateField, CS_Test_Setup, CS_Test_TearDown,
               "CS_ValidateTablesChecksumDefinitionTable_Test_IllegalStateField");
    UtTest_Add(CS_ValidateTablesChecksumDefinitionTable_Test_IllegalStateEmptyName, CS_Test_Setup, CS_Test_TearDown,
               "CS_ValidateTablesChecksumDefinitionTable_Test_IllegalStateEmptyName");
    UtTest_Add(CS_ValidateTablesChecksumDefinitionTable_Test_TableErrorResult, CS_Test_Setup, CS_Test_TearDown,
               "CS_ValidateTablesChecksumDefinitionTable_Test_TableErrorResult");
    UtTest_Add(CS_ValidateTablesChecksumDefinitionTable_Test_UndefTableErrorResult, CS_Test_Setup, CS_Test_TearDown,
               "CS_ValidateTablesChecksumDefinitionTable_Test_UndefTableErrorResult");
    UtTest_Add(CS_ValidateTablesChecksumDefinitionTable_Test_CsTableError, CS_Test_Setup, CS_Test_TearDown,
               "CS_ValidateTablesChecksumDefinitionTable_Test_CsTableError");

    UtTest_Add(CS_ValidateAppChecksumDefinitionTable_Test_Nominal, CS_Test_Setup, CS_Test_TearDown,
               "CS_ValidateAppChecksumDefinitionTable_Test_Nominal");
    UtTest_Add(CS_ValidateAppChecksumDefinitionTable_Test_DuplicateNameStateEmpty, CS_Test_Setup, CS_Test_TearDown,
               "CS_ValidateAppChecksumDefinitionTable_Test_DuplicateNameStateEmpty");
    UtTest_Add(CS_ValidateAppChecksumDefinitionTable_Test_DuplicateNameStateEnabled, CS_Test_Setup, CS_Test_TearDown,
               "CS_ValidateAppChecksumDefinitionTable_Test_DuplicateNameStateEnabled");
    UtTest_Add(CS_ValidateAppChecksumDefinitionTable_Test_DuplicateNameStateDisabled, CS_Test_Setup, CS_Test_TearDown,
               "CS_ValidateAppChecksumDefinitionTable_Test_DuplicateNameStateDisabled");
    UtTest_Add(CS_ValidateAppChecksumDefinitionTable_Test_IllegalStateField, CS_Test_Setup, CS_Test_TearDown,
               "CS_ValidateAppChecksumDefinitionTable_Test_IllegalStateField");
    UtTest_Add(CS_ValidateAppChecksumDefinitionTable_Test_IllegalStateEmptyName, CS_Test_Setup, CS_Test_TearDown,
               "CS_ValidateAppChecksumDefinitionTable_Test_IllegalStateEmptyName");
    UtTest_Add(CS_ValidateAppChecksumDefinitionTable_Test_TableErrorResult, CS_Test_Setup, CS_Test_TearDown,
               "CS_ValidateAppChecksumDefinitionTable_Test_TableErrorResult");
    UtTest_Add(CS_ValidateAppChecksumDefinitionTable_Test_UndefTableErrorResult, CS_Test_Setup, CS_Test_TearDown,
               "CS_ValidateAppChecksumDefinitionTable_Test_UndefTableErrorResult");
    UtTest_Add(CS_ValidateAppChecksumDefinitionTable_Test_EmptyNameTableResult, CS_Test_Setup, CS_Test_TearDown,
               "CS_ValidateAppChecksumDefinitionTable_Test_EmptyNameTableResult");

    UtTest_Add(CS_ProcessNewEepromMemoryDefinitionTable_Test_EEPROMTableNominal, CS_Test_Setup, CS_Test_TearDown,
               "CS_ProcessNewEepromMemoryDefinitionTable_Test_EEPROMTableNominal");
    UtTest_Add(CS_ProcessNewEepromMemoryDefinitionTable_Test_MemoryTableNominal, CS_Test_Setup, CS_Test_TearDown,
               "CS_ProcessNewEepromMemoryDefinitionTable_Test_MemoryTableNominal");
    UtTest_Add(CS_ProcessNewEepromMemoryDefinitionTable_Test_EEPROMTableNoValidEntries, CS_Test_Setup, CS_Test_TearDown,
               "CS_ProcessNewEepromMemoryDefinitionTable_Test_EEPROMTableNoValidEntries");
    UtTest_Add(CS_ProcessNewEepromMemoryDefinitionTable_Test_MemoryTableNoValidEntries, CS_Test_Setup, CS_Test_TearDown,
               "CS_ProcessNewEepromMemoryDefinitionTable_Test_MemoryTableNoValidEntries");

    UtTest_Add(CS_ProcessNewTablesDefinitionTable_Test_DefEepromTableHandle, CS_Test_Setup, CS_Test_TearDown,
               "CS_ProcessNewTablesDefinitionTable_Test_DefEepromTableHandle");
    UtTest_Add(CS_ProcessNewTablesDefinitionTable_Test_DefMemoryTableHandle, CS_Test_Setup, CS_Test_TearDown,
               "CS_ProcessNewTablesDefinitionTable_Test_DefMemoryTableHandle");
    UtTest_Add(CS_ProcessNewTablesDefinitionTable_Test_DefTablesTableHandle, CS_Test_Setup, CS_Test_TearDown,
               "CS_ProcessNewTablesDefinitionTable_Test_DefTablesTableHandle");
    UtTest_Add(CS_ProcessNewTablesDefinitionTable_Test_DefAppTableHandle, CS_Test_Setup, CS_Test_TearDown,
               "CS_ProcessNewTablesDefinitionTable_Test_DefAppTableHandle");
    UtTest_Add(CS_ProcessNewTablesDefinitionTable_Test_ResEepromTableHandle, CS_Test_Setup, CS_Test_TearDown,
               "CS_ProcessNewTablesDefinitionTable_Test_ResEepromTableHandle");
    UtTest_Add(CS_ProcessNewTablesDefinitionTable_Test_ResMemoryTableHandle, CS_Test_Setup, CS_Test_TearDown,
               "CS_ProcessNewTablesDefinitionTable_Test_ResMemoryTableHandle");
    UtTest_Add(CS_ProcessNewTablesDefinitionTable_Test_ResTablesTableHandle, CS_Test_Setup, CS_Test_TearDown,
               "CS_ProcessNewTablesDefinitionTable_Test_ResTablesTableHandle");
    UtTest_Add(CS_ProcessNewTablesDefinitionTable_Test_ResAppTableHandle, CS_Test_Setup, CS_Test_TearDown,
               "CS_ProcessNewTablesDefinitionTable_Test_ResAppTableHandle");
    UtTest_Add(CS_ProcessNewTablesDefinitionTable_Test_StateEmptyNoValidEntries, CS_Test_Setup, CS_Test_TearDown,
               "CS_ProcessNewTablesDefinitionTable_Test_StateEmptyNoValidEntries");
    UtTest_Add(CS_ProcessNewTablesDefinitionTable_Test_LimitApplicationNameLength, CS_Test_Setup, CS_Test_TearDown,
               "CS_ProcessNewTablesDefinitionTable_Test_LimitApplicationNameLength");
    UtTest_Add(CS_ProcessNewTablesDefinitionTable_Test_LimitTableNameLength, CS_Test_Setup, CS_Test_TearDown,
               "CS_ProcessNewTablesDefinitionTable_Test_LimitTableNameLength");
    UtTest_Add(CS_ProcessNewTablesDefinitionTable_Test_MaxTableNameLength, CS_Test_Setup, CS_Test_TearDown,
               "CS_ProcessNewTablesDefinitionTable_Test_MaxTableNameLength");

    UtTest_Add(CS_ProcessNewAppDefinitionTable_Test_Nominal, CS_Test_Setup, CS_Test_TearDown,
               "CS_ProcessNewAppDefinitionTable_Test_Nominal");
    UtTest_Add(CS_ProcessNewAppDefinitionTable_Test_StateEmptyNoValidEntries, CS_Test_Setup, CS_Test_TearDown,
               "CS_ProcessNewAppDefinitionTable_Test_StateEmptyNoValidEntries");

    UtTest_Add(CS_TableInit_Test_DefaultDefinitionTableLoadErrorEEPROM, CS_Test_Setup, CS_Test_TearDown,
               "CS_TableInit_Test_DefaultDefinitionTableLoadErrorEEPROM");
    UtTest_Add(CS_TableInit_Test_DefinitionTableGetAddressErrorEEPROM, CS_Test_Setup, CS_Test_TearDown,
               "CS_TableInit_Test_DefinitionTableGetAddressErrorEEPROM");
    UtTest_Add(CS_TableInit_Test_DefinitionTableGetAddressErrorMemory, CS_Test_Setup, CS_Test_TearDown,
               "CS_TableInit_Test_DefinitionTableGetAddressErrorMemory");
    UtTest_Add(CS_TableInit_Test_DefinitionTableGetAddressErrorTables, CS_Test_Setup, CS_Test_TearDown,
               "CS_TableInit_Test_DefinitionTableGetAddressErrorTables");
    UtTest_Add(CS_TableInit_Test_DefinitionTableGetAddressErrorApps, CS_Test_Setup, CS_Test_TearDown,
               "CS_TableInit_Test_DefinitionTableGetAddressErrorApps");
    UtTest_Add(CS_TableInit_Test_EepromTableAndNotLoadedFromMemory, CS_Test_Setup, CS_Test_TearDown,
               "CS_TableInit_Test_EepromTableAndNotLoadedFromMemory");
    UtTest_Add(CS_TableInit_Test_EepromTableAndLoadedFromMemoryAfterResultsTableRegisterError, CS_Test_Setup,
               CS_Test_TearDown, "CS_TableInit_Test_EepromTableAndLoadedFromMemoryAfterResultsTableRegisterError");
    UtTest_Add(CS_TableInit_Test_EepromTableAndLoadedFromMemoryAfterResultsTableGetAddressError, CS_Test_Setup,
               CS_Test_TearDown, "CS_TableInit_Test_EepromTableAndLoadedFromMemoryAfterResultsTableGetAddressError");
    UtTest_Add(CS_TableInit_Test_EepromTableAndLoadedFromMemoryAfterDefinitionTableRegisterError, CS_Test_Setup,
               CS_Test_TearDown, "CS_TableInit_Test_EepromTableAndLoadedFromMemoryAfterDefinitionTableRegisterError");
    UtTest_Add(CS_TableInit_Test_EepromTableAndLoadedFromMemoryAfterDefinitionTableFileLoadError, CS_Test_Setup,
               CS_Test_TearDown, "CS_TableInit_Test_EepromTableAndLoadedFromMemoryAfterDefinitionTableFileLoadError");
    UtTest_Add(CS_TableInit_Test_MemoryTableAndNotLoadedFromMemory, CS_Test_Setup, CS_Test_TearDown,
               "CS_TableInit_Test_MemoryTableAndNotLoadedFromMemory");
    UtTest_Add(CS_TableInit_Test_MemoryTableAndLoadedFromMemory, CS_Test_Setup, CS_Test_TearDown,
               "CS_TableInit_Test_MemoryTableAndLoadedFromMemory");
    UtTest_Add(CS_TableInit_Test_AppTableAndNotLoadedFromMemory, CS_Test_Setup, CS_Test_TearDown,
               "CS_TableInit_Test_AppTableAndNotLoadedFromMemory");
    UtTest_Add(CS_TableInit_Test_AppTableAndLoadedFromMemory, CS_Test_Setup, CS_Test_TearDown,
               "CS_TableInit_Test_AppTableAndLoadedFromMemory");
    UtTest_Add(CS_TableInit_Test_TablesTableAndNotLoadedFromMemory, CS_Test_Setup, CS_Test_TearDown,
               "CS_TableInit_Test_TablesTableAndNotLoadedFromMemory");
    UtTest_Add(CS_TableInit_Test_TablesTableAndLoadedFromMemory, CS_Test_Setup, CS_Test_TearDown,
               "CS_TableInit_Test_TablesTableAndLoadedFromMemory");
    UtTest_Add(CS_TableInit_Test_DefaultAndLoadedFromMemory, CS_Test_Setup, CS_Test_TearDown,
               "CS_TableInit_Test_DefaultAndLoadedFromMemory");
    UtTest_Add(CS_TableInit_Test_OpenCreateError, CS_Test_Setup, CS_Test_TearDown, "CS_TableInit_Test_OpenCreateError");

    UtTest_Add(CS_HandleTableUpdate_Test_ProcessNewTablesDefinitionTable, CS_Test_Setup, CS_Test_TearDown,
               "CS_HandleTableUpdate_Test_ProcessNewTablesDefinitionTable");
    UtTest_Add(CS_HandleTableUpdate_Test_ProcessNewAppDefinitionTable, CS_Test_Setup, CS_Test_TearDown,
               "CS_HandleTableUpdate_Test_ProcessNewAppDefinitionTable");
    UtTest_Add(CS_HandleTableUpdate_Test_ProcessNewEepromMemoryDefinitionTable, CS_Test_Setup, CS_Test_TearDown,
               "CS_HandleTableUpdate_Test_ProcessNewEepromMemoryDefinitionTable");
    UtTest_Add(CS_HandleTableUpdate_Test_ResultsTableGetAddressErrorEEPROM, CS_Test_Setup, CS_Test_TearDown,
               "CS_HandleTableUpdate_Test_ResultsTableGetAddressErrorEEPROM");
    UtTest_Add(CS_HandleTableUpdate_Test_DefinitionTableGetAddressErrorEEPROM, CS_Test_Setup, CS_Test_TearDown,
               "CS_HandleTableUpdate_Test_DefinitionTableGetAddressErrorEEPROM");
    UtTest_Add(CS_HandleTableUpdate_Test_DefinitionTableGetAddressErrorMemory, CS_Test_Setup, CS_Test_TearDown,
               "CS_HandleTableUpdate_Test_DefinitionTableGetAddressErrorMemory");
    UtTest_Add(CS_HandleTableUpdate_Test_DefinitionTableGetAddressErrorTables, CS_Test_Setup, CS_Test_TearDown,
               "CS_HandleTableUpdate_Test_DefinitionTableGetAddressErrorTables");
    UtTest_Add(CS_HandleTableUpdate_Test_DefinitionTableGetAddressErrorApps, CS_Test_Setup, CS_Test_TearDown,
               "CS_HandleTableUpdate_Test_DefinitionTableGetAddressErrorApps");
    UtTest_Add(CS_HandleTableUpdate_Test_BadTableHandle, CS_Test_Setup, CS_Test_TearDown,
               "CS_HandleTableUpdate_Test_BadTableHandle");
    UtTest_Add(CS_HandleTableUpdate_Test_CsOwner, CS_Test_Setup, CS_Test_TearDown, "CS_HandleTableUpdate_Test_CsOwner");
    UtTest_Add(CS_HandleTableUpdate_Test_GetAddressError, CS_Test_Setup, CS_Test_TearDown,
               "CS_HandleTableUpdate_Test_GetAddressError");

} /* end UtTest_Setup */

/************************/
/*  End of File Comment */
/************************/
