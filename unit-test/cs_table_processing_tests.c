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

#define CS_UT_TABLE_HANDLE(x) CFE_TBL_HandleFromID(CFE_TBL_HANDLEID_C(CFE_ResourceId_FromInteger(x)))

/* cs_table_processing_tests globals */
uint8 call_count_CFE_EVS_SendEvent;

/*
 * Function Definitions
 */

void CS_TABLE_PROCESSING_TEST_CFE_ES_GetAppNameHandler1(void                   *UserObj,
                                                        UT_EntryKey_t           FuncKey,
                                                        const UT_StubContext_t *Context)
{
    char *AppName = (char *)UT_Hook_GetArgValueByName(Context, "AppName", char *);

    strncpy((char *)AppName, "CS", 3);
}

CFE_Status_t CS_TABLE_PROCESSING_TEST_CFE_TBL_GetAddressHook(void                   *UserObj,
                                                             int32                   StubRetcode,
                                                             uint32                  CallCount,
                                                             const UT_StubContext_t *Context)
{
    return CFE_SUCCESS;
}

CFE_Status_t CS_TABLE_PROCESSING_TEST_CFE_TBL_LoadHook(void                   *UserObj,
                                                       int32                   StubRetcode,
                                                       uint32                  CallCount,
                                                       const UT_StubContext_t *Context)
{
    return CFE_SUCCESS;
}

void Test_CS_ValidateEepromChecksumDefinitionTable(void)
{
    /* Test case for:
     * CFE_Status_t CS_ValidateEepromChecksumDefinitionTable(void *TblPtr);
     */

    /* Nominal, enabled + disabled, rest empty */
    CS_AppData.DefaultEepromDefTable[0].State = CS_ChecksumState_ENABLED;
    CS_AppData.DefaultEepromDefTable[1].State = CS_ChecksumState_DISABLED;
    UtAssert_INT32_EQ(CS_ValidateEepromChecksumDefinitionTable(CS_AppData.DefaultEepromDefTable), CFE_SUCCESS);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_VAL_EEPROM_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    /* Error in state */
    CS_Test_ResetEventCheck();
    CS_AppData.DefaultEepromDefTable[0].State = CS_ChecksumState_UNDEFINED;
    CS_AppData.DefaultEepromDefTable[1].State = CS_ChecksumState_UNDEFINED;
    UtAssert_INT32_EQ(CS_ValidateEepromChecksumDefinitionTable(CS_AppData.DefaultEepromDefTable), CS_TABLE_ERROR);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 2);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_VAL_EEPROM_STATE_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    /* Error in range */
    CS_Test_ResetEventCheck();
    CS_AppData.DefaultEepromDefTable[0].State = CS_ChecksumState_ENABLED;
    CS_AppData.DefaultEepromDefTable[1].State = CS_ChecksumState_DISABLED;
    UT_SetDefaultReturnValue(UT_KEY(CFE_PSP_MemValidateRange), -1);
    UtAssert_INT32_EQ(CS_ValidateEepromChecksumDefinitionTable(CS_AppData.DefaultEepromDefTable), CS_TABLE_ERROR);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 2);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_VAL_EEPROM_RANGE_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);
}

void Test_CS_ValidateMemoryChecksumDefinitionTable(void)
{
    /* Test case for:
     * CFE_Status_t CS_ValidateMemoryChecksumDefinitionTable(void *TblPtr);
     */

    /* Nominal, enabled + disabled, rest empty */
    CS_AppData.DefaultMemoryDefTable[0].State = CS_ChecksumState_ENABLED;
    CS_AppData.DefaultMemoryDefTable[1].State = CS_ChecksumState_DISABLED;
    UtAssert_INT32_EQ(CS_ValidateMemoryChecksumDefinitionTable(CS_AppData.DefaultMemoryDefTable), CFE_SUCCESS);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_VAL_MEMORY_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    /* Error in state */
    CS_Test_ResetEventCheck();
    CS_AppData.DefaultMemoryDefTable[0].State = CS_ChecksumState_UNDEFINED;
    CS_AppData.DefaultMemoryDefTable[1].State = CS_ChecksumState_UNDEFINED;
    UtAssert_INT32_EQ(CS_ValidateMemoryChecksumDefinitionTable(CS_AppData.DefaultMemoryDefTable), CS_TABLE_ERROR);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 2);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_VAL_MEMORY_STATE_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    /* Error in range */
    CS_Test_ResetEventCheck();
    CS_AppData.DefaultMemoryDefTable[0].State = CS_ChecksumState_ENABLED;
    CS_AppData.DefaultMemoryDefTable[1].State = CS_ChecksumState_DISABLED;
    UT_SetDefaultReturnValue(UT_KEY(CFE_PSP_MemValidateRange), -1);
    UtAssert_INT32_EQ(CS_ValidateMemoryChecksumDefinitionTable(CS_AppData.DefaultMemoryDefTable), CS_TABLE_ERROR);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 2);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_VAL_MEMORY_RANGE_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);
}

void Test_CS_ValidateTablesChecksumDefinitionTable(void)
{
    /* Test case for:
     * CFE_Status_t CS_ValidateTablesChecksumDefinitionTable(void *TblPtr);
     */

    /* Nominal, enabled + disabled, rest empty */
    CS_AppData.DefaultTablesDefTable[0].State = CS_ChecksumState_ENABLED;
    strcpy(CS_AppData.DefaultTablesDefTable[0].Name, "UT0");
    CS_AppData.DefaultTablesDefTable[1].State = CS_ChecksumState_DISABLED;
    strcpy(CS_AppData.DefaultTablesDefTable[1].Name, "UT1");
    UtAssert_INT32_EQ(CS_ValidateTablesChecksumDefinitionTable(CS_AppData.DefaultTablesDefTable), CFE_SUCCESS);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_VAL_TABLES_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    /* Error in state */
    CS_Test_ResetEventCheck();
    CS_AppData.DefaultTablesDefTable[0].State = CS_ChecksumState_UNDEFINED;
    CS_AppData.DefaultTablesDefTable[1].State = CS_ChecksumState_UNDEFINED;
    UtAssert_INT32_EQ(CS_ValidateTablesChecksumDefinitionTable(CS_AppData.DefaultTablesDefTable), CS_TABLE_ERROR);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 2);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_VAL_TABLES_STATE_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    /* Error in name (zero) */
    CS_Test_ResetEventCheck();
    CS_AppData.DefaultTablesDefTable[0].State   = CS_ChecksumState_ENABLED;
    CS_AppData.DefaultTablesDefTable[0].Name[0] = 0;
    CS_AppData.DefaultTablesDefTable[1].State   = CS_ChecksumState_DISABLED;
    strcpy(CS_AppData.DefaultTablesDefTable[1].Name, "UT1");
    UtAssert_INT32_EQ(CS_ValidateTablesChecksumDefinitionTable(CS_AppData.DefaultTablesDefTable), CS_TABLE_ERROR);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 2);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_VAL_TABLES_DEF_TBL_ZERO_NAME_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    /* Error in name (long) */
    CS_Test_ResetEventCheck();
    CS_AppData.DefaultTablesDefTable[0].State = CS_ChecksumState_ENABLED;
    memset(CS_AppData.DefaultTablesDefTable[0].Name, 'x', sizeof(CS_AppData.DefaultTablesDefTable[0].Name));
    CS_AppData.DefaultTablesDefTable[1].State = CS_ChecksumState_DISABLED;
    strcpy(CS_AppData.DefaultTablesDefTable[1].Name, "UT1");
    UtAssert_INT32_EQ(CS_ValidateTablesChecksumDefinitionTable(CS_AppData.DefaultTablesDefTable), CS_TABLE_ERROR);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 2);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_VAL_TABLES_DEF_TBL_LONG_NAME_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    /* Error in name (duplicate) */
    CS_Test_ResetEventCheck();
    CS_AppData.DefaultTablesDefTable[0].State = CS_ChecksumState_ENABLED;
    strcpy(CS_AppData.DefaultTablesDefTable[0].Name, "UT1");
    CS_AppData.DefaultTablesDefTable[1].State = CS_ChecksumState_DISABLED;
    strcpy(CS_AppData.DefaultTablesDefTable[1].Name, "UT1");
    UtAssert_INT32_EQ(CS_ValidateTablesChecksumDefinitionTable(CS_AppData.DefaultTablesDefTable), CS_TABLE_ERROR);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 2);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_VAL_TABLES_DEF_TBL_DUPL_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);
}

void Test_CS_ValidateAppChecksumDefinitionTable(void)
{
    /* Test case for:
     * CFE_Status_t CS_ValidateAppChecksumDefinitionTable(void *TblPtr);
     */

    /* Nominal, enabled + disabled, rest empty */
    CS_AppData.DefaultAppDefTable[0].State = CS_ChecksumState_ENABLED;
    strcpy(CS_AppData.DefaultAppDefTable[0].Name, "UT0");
    CS_AppData.DefaultAppDefTable[1].State = CS_ChecksumState_DISABLED;
    strcpy(CS_AppData.DefaultAppDefTable[1].Name, "UT1");
    UtAssert_INT32_EQ(CS_ValidateAppChecksumDefinitionTable(CS_AppData.DefaultAppDefTable), CFE_SUCCESS);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_VAL_APP_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    /* Error in state */
    CS_Test_ResetEventCheck();
    CS_AppData.DefaultAppDefTable[0].State = CS_ChecksumState_UNDEFINED;
    CS_AppData.DefaultAppDefTable[1].State = CS_ChecksumState_UNDEFINED;
    UtAssert_INT32_EQ(CS_ValidateAppChecksumDefinitionTable(CS_AppData.DefaultAppDefTable), CS_TABLE_ERROR);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 2);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_VAL_APP_STATE_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    /* Error in name (zero) */
    CS_Test_ResetEventCheck();
    CS_AppData.DefaultAppDefTable[0].State   = CS_ChecksumState_ENABLED;
    CS_AppData.DefaultAppDefTable[0].Name[0] = 0;
    CS_AppData.DefaultAppDefTable[1].State   = CS_ChecksumState_DISABLED;
    strcpy(CS_AppData.DefaultAppDefTable[1].Name, "UT1");
    UtAssert_INT32_EQ(CS_ValidateAppChecksumDefinitionTable(CS_AppData.DefaultAppDefTable), CS_TABLE_ERROR);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 2);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_VAL_APP_DEF_TBL_ZERO_NAME_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    /* Error in name (long) */
    CS_Test_ResetEventCheck();
    CS_AppData.DefaultAppDefTable[0].State = CS_ChecksumState_ENABLED;
    memset(CS_AppData.DefaultAppDefTable[0].Name, 'x', sizeof(CS_AppData.DefaultAppDefTable[0].Name));
    CS_AppData.DefaultAppDefTable[1].State = CS_ChecksumState_DISABLED;
    strcpy(CS_AppData.DefaultAppDefTable[1].Name, "UT1");
    UtAssert_INT32_EQ(CS_ValidateAppChecksumDefinitionTable(CS_AppData.DefaultAppDefTable), CS_TABLE_ERROR);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 2);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_VAL_APP_DEF_TBL_LONG_NAME_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    /* Error in name (duplicate) */
    CS_Test_ResetEventCheck();
    CS_AppData.DefaultAppDefTable[0].State = CS_ChecksumState_ENABLED;
    strcpy(CS_AppData.DefaultAppDefTable[0].Name, "UT1");
    CS_AppData.DefaultAppDefTable[1].State = CS_ChecksumState_DISABLED;
    strcpy(CS_AppData.DefaultAppDefTable[1].Name, "UT1");
    UtAssert_INT32_EQ(CS_ValidateAppChecksumDefinitionTable(CS_AppData.DefaultAppDefTable), CS_TABLE_ERROR);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 2);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_VAL_APP_DEF_TBL_DUPL_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);
}

void Test_CS_ProcessNewEepromMemoryDefinitionTable(void)
{
    /* Test case for:
     * void CS_ProcessNewEepromMemoryDefinitionTable(CS_TableWrapper_t *tw);
     */
    CS_TableWrapper_t *tw;

    /* path where all entries are empty */
    tw = &CS_AppData.Tbl[CS_ChecksumType_EEPROM_TABLE];
    UtAssert_VOIDCALL(CS_ProcessNewEepromMemoryDefinitionTable(tw));
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_PROCESS_EEPROM_MEMORY_NO_ENTRIES_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    /* path where some entries not empty */
    CS_Test_ResetEventCheck();
    CS_AppData.DefaultEepromDefTable[0].State = CS_ChecksumState_ENABLED;
    CS_AppData.DefaultEepromDefTable[1].State = CS_ChecksumState_DISABLED;
    UtAssert_VOIDCALL(CS_ProcessNewEepromMemoryDefinitionTable(tw));
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);

    /* Where ResultsEntry is null */
    CS_Test_ResetEventCheck();
    UT_SetDeferredRetcode(UT_KEY(CS_GetResEntryAddr), 1, 0);
    CS_AppData.DefaultEepromDefTable[0].State = CS_ChecksumState_ENABLED;
    CS_AppData.DefaultEepromDefTable[1].State = CS_ChecksumState_DISABLED;
    UtAssert_VOIDCALL(CS_ProcessNewEepromMemoryDefinitionTable(tw));
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_PROCESS_EEPROM_MEMORY_NO_ENTRIES_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    /* Where DefEntry is null */
    CS_Test_ResetEventCheck();
    UT_SetDeferredRetcode(UT_KEY(CS_GetDefEntryAddr), 1, 0);
    CS_AppData.DefaultEepromDefTable[0].State = CS_ChecksumState_ENABLED;
    CS_AppData.DefaultEepromDefTable[1].State = CS_ChecksumState_DISABLED;
    UtAssert_VOIDCALL(CS_ProcessNewEepromMemoryDefinitionTable(tw));
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_PROCESS_EEPROM_MEMORY_NO_ENTRIES_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    CS_Test_ResetEventCheck();
    CS_AppData.DefaultEepromDefTable[0].State = CS_ChecksumState_ENABLED;
    CS_AppData.DefaultEepromDefTable[1].State = CS_ChecksumState_DISABLED;
    UtAssert_VOIDCALL(CS_ProcessNewEepromMemoryDefinitionTable(tw));
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);

    /* exercise path where there is no global state */
    tw->GlobalState = NULL;
    CS_Test_ResetEventCheck();
    UtAssert_VOIDCALL(CS_ProcessNewEepromMemoryDefinitionTable(tw));
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
}

void Test_CS_ProcessNewTablesDefinitionTable(void)
{
    /* Test case for:
     * void CS_ProcessNewTablesDefinitionTable(CS_TableWrapper_t *tw);
     */

    CS_TableWrapper_t           *tw;
    CS_Res_Tables_Table_Entry_t *ResultsEntry;

    /* path where all entries are empty */
    tw = &CS_AppData.Tbl[CS_ChecksumType_TABLES_TABLE];
    UtAssert_VOIDCALL(CS_ProcessNewTablesDefinitionTable(tw));
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_PROCESS_TABLES_NO_ENTRIES_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    /* path where some entries not empty, table names only */
    ResultsEntry = tw->ResAddr;
    CS_Test_ResetEventCheck();
    CS_AppData.DefaultTablesDefTable[0].State = CS_ChecksumState_ENABLED;
    strcpy(CS_AppData.DefaultTablesDefTable[0].Name, "UT0");
    CS_AppData.DefaultTablesDefTable[1].State = CS_ChecksumState_DISABLED;
    strcpy(CS_AppData.DefaultTablesDefTable[1].Name, "UT1");
    UtAssert_VOIDCALL(CS_ProcessNewTablesDefinitionTable(tw));
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_BOOL_FALSE(ResultsEntry->IsCSOwner);

    /* Where ResultsEntry is null */
    CS_Test_ResetEventCheck();
    UT_SetDeferredRetcode(UT_KEY(CS_GetResEntryAddr), 1, 0);
    CS_AppData.DefaultTablesDefTable[0].State = CS_ChecksumState_ENABLED;
    strcpy(CS_AppData.DefaultTablesDefTable[0].Name, "UT0");
    CS_AppData.DefaultTablesDefTable[1].State = CS_ChecksumState_DISABLED;
    strcpy(CS_AppData.DefaultTablesDefTable[1].Name, "UT1");
    UtAssert_VOIDCALL(CS_ProcessNewTablesDefinitionTable(tw));
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_PROCESS_TABLES_NO_ENTRIES_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    /* Where DefEntry is null */
    CS_Test_ResetEventCheck();
    UT_SetDeferredRetcode(UT_KEY(CS_GetDefEntryAddr), 1, 0);
    CS_AppData.DefaultTablesDefTable[0].State = CS_ChecksumState_ENABLED;
    strcpy(CS_AppData.DefaultTablesDefTable[0].Name, "UT0");
    CS_AppData.DefaultTablesDefTable[1].State = CS_ChecksumState_DISABLED;
    strcpy(CS_AppData.DefaultTablesDefTable[1].Name, "UT1");
    UtAssert_VOIDCALL(CS_ProcessNewTablesDefinitionTable(tw));
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_PROCESS_TABLES_NO_ENTRIES_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    /* path where some entries not empty, qualified table names */
    ResultsEntry = tw->ResAddr;
    CS_Test_ResetEventCheck();
    CS_AppData.DefaultTablesDefTable[0].State = CS_ChecksumState_ENABLED;
    strcpy(CS_AppData.DefaultTablesDefTable[0].Name, "UT.UT0");
    CS_AppData.DefaultTablesDefTable[1].State = CS_ChecksumState_DISABLED;
    strcpy(CS_AppData.DefaultTablesDefTable[1].Name, "XY.UT1");
    UtAssert_VOIDCALL(CS_ProcessNewTablesDefinitionTable(tw));
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_BOOL_FALSE(ResultsEntry->IsCSOwner);

    /* path where some entries not empty, invalid table names */
    ResultsEntry = tw->ResAddr;
    CS_Test_ResetEventCheck();
    CS_AppData.DefaultTablesDefTable[0].State = CS_ChecksumState_ENABLED;
    memset(CS_AppData.DefaultTablesDefTable[0].Name, 'x', sizeof(CS_AppData.DefaultTablesDefTable[0].Name));
    CS_AppData.DefaultTablesDefTable[0].Name[sizeof(CS_AppData.DefaultTablesDefTable[0].Name) - 1] = '.';
    CS_AppData.DefaultTablesDefTable[1].State = CS_ChecksumState_DISABLED;
    memset(CS_AppData.DefaultTablesDefTable[1].Name, 'y', sizeof(CS_AppData.DefaultTablesDefTable[1].Name));
    CS_AppData.DefaultTablesDefTable[1].Name[0] = '.';
    UtAssert_VOIDCALL(CS_ProcessNewTablesDefinitionTable(tw));
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_BOOL_FALSE(ResultsEntry->IsCSOwner);

    /* path where matches a CS Def table */
    UT_SetDeferredRetcode(UT_KEY(CS_CheckDefTableNameMatch), 1, true);
    CS_AppData.DefaultTablesDefTable[0].State = CS_ChecksumState_ENABLED;
    strcpy(CS_AppData.DefaultTablesDefTable[0].Name, "UT.UT0");
    CS_AppData.DefaultTablesDefTable[1].State = CS_ChecksumState_DISABLED;
    strcpy(CS_AppData.DefaultTablesDefTable[1].Name, "XY.UT1");
    UtAssert_VOIDCALL(CS_ProcessNewTablesDefinitionTable(tw));
    UtAssert_BOOL_TRUE(ResultsEntry->IsCSOwner);

    /* path where matches a CS Res table */
    UT_SetDeferredRetcode(UT_KEY(CS_CheckResTableNameMatch), 1, true);
    CS_AppData.DefaultTablesDefTable[0].State = CS_ChecksumState_ENABLED;
    strcpy(CS_AppData.DefaultTablesDefTable[0].Name, "UT.UT0");
    CS_AppData.DefaultTablesDefTable[1].State = CS_ChecksumState_DISABLED;
    strcpy(CS_AppData.DefaultTablesDefTable[1].Name, "XY.UT1");
    UtAssert_VOIDCALL(CS_ProcessNewTablesDefinitionTable(tw));
    UtAssert_BOOL_TRUE(ResultsEntry->IsCSOwner);
}

void Test_CS_ProcessNewAppDefinitionTable(void)
{
    /* Test case for:
     * void CS_ProcessNewAppDefinitionTable(CS_TableWrapper_t *tw);
     */
    CS_TableWrapper_t *tw;

    /* path where all entries are empty */
    tw = &CS_AppData.Tbl[CS_ChecksumType_APP_TABLE];
    UtAssert_VOIDCALL(CS_ProcessNewAppDefinitionTable(tw));
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_PROCESS_APP_NO_ENTRIES_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    /* path where some entries not empty */
    CS_Test_ResetEventCheck();
    CS_AppData.DefaultAppDefTable[0].State = CS_ChecksumState_ENABLED;
    strcpy(CS_AppData.DefaultAppDefTable[0].Name, "UT0");
    CS_AppData.DefaultAppDefTable[1].State = CS_ChecksumState_DISABLED;
    strcpy(CS_AppData.DefaultAppDefTable[1].Name, "UT1");
    UtAssert_VOIDCALL(CS_ProcessNewAppDefinitionTable(tw));
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);

    /* Where ResultsEntry is null */
    CS_Test_ResetEventCheck();
    UT_SetDeferredRetcode(UT_KEY(CS_GetResEntryAddr), 1, 0);
    CS_AppData.DefaultAppDefTable[0].State = CS_ChecksumState_ENABLED;
    strcpy(CS_AppData.DefaultAppDefTable[0].Name, "UT0");
    CS_AppData.DefaultAppDefTable[1].State = CS_ChecksumState_DISABLED;
    strcpy(CS_AppData.DefaultAppDefTable[1].Name, "UT1");
    UtAssert_VOIDCALL(CS_ProcessNewAppDefinitionTable(tw));
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_PROCESS_APP_NO_ENTRIES_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    /* Where DefEntry is null */
    CS_Test_ResetEventCheck();
    UT_SetDeferredRetcode(UT_KEY(CS_GetDefEntryAddr), 1, 0);
    CS_AppData.DefaultAppDefTable[0].State = CS_ChecksumState_ENABLED;
    strcpy(CS_AppData.DefaultAppDefTable[0].Name, "UT0");
    CS_AppData.DefaultAppDefTable[1].State = CS_ChecksumState_DISABLED;
    strcpy(CS_AppData.DefaultAppDefTable[1].Name, "UT1");
    UtAssert_VOIDCALL(CS_ProcessNewAppDefinitionTable(tw));
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_PROCESS_APP_NO_ENTRIES_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);
}

void Test_CS_TableInit(void)
{
    /* Test case for:
     * CFE_Status_t CS_TableInit(CS_TableWrapper_t *tw, const char *DefinitionTableFileName, CFE_TBL_CallbackFuncPtr_t
     * CallBackFunction);
     */
    CS_TableWrapper_t *tw = &CS_AppData.Tbl[CS_ChecksumType_APP_TABLE];

    /* nominal */
    UT_SetDataBuffer(UT_KEY(CFE_TBL_GetAddress), &tw->ResAddr, sizeof(tw->ResAddr), false);
    UT_SetDataBuffer(UT_KEY(CFE_TBL_GetAddress), &tw->DefAddr, sizeof(tw->DefAddr), false);
    UtAssert_INT32_EQ(CS_TableInit(tw, "UT", NULL), CFE_SUCCESS);
    UtAssert_STUB_COUNT(CFE_TBL_Register, 2);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);

    /* No filename */
    CS_Test_ResetEventCheck();
    UT_ResetState(UT_KEY(CFE_TBL_Register));
    UT_ResetState(UT_KEY(CFE_TBL_GetAddress));
    UT_SetDataBuffer(UT_KEY(CFE_TBL_GetAddress), &tw->ResAddr, sizeof(tw->ResAddr), false);
    UT_SetDataBuffer(UT_KEY(CFE_TBL_GetAddress), &tw->DefAddr, sizeof(tw->DefAddr), false);
    UtAssert_INT32_EQ(CS_TableInit(tw, NULL, NULL), CFE_SUCCESS);
    UtAssert_STUB_COUNT(CFE_TBL_Register, 0);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);

    /* failure to register result table */
    CS_Test_ResetEventCheck();
    UT_ResetState(UT_KEY(CFE_TBL_Register));
    UT_ResetState(UT_KEY(CFE_TBL_GetAddress));
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_Register), 1, -123);
    UtAssert_INT32_EQ(CS_TableInit(tw, "UT", NULL), -123);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_TBL_INIT_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    /* failure to register definition table */
    CS_Test_ResetEventCheck();
    UT_ResetState(UT_KEY(CFE_TBL_Register));
    UT_ResetState(UT_KEY(CFE_TBL_GetAddress));
    UT_SetDataBuffer(UT_KEY(CFE_TBL_GetAddress), &tw->ResAddr, sizeof(tw->ResAddr), false);
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_Register), 2, -123);
    UtAssert_INT32_EQ(CS_TableInit(tw, "UT", NULL), -123);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_TBL_INIT_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    /* failure to get address result table */
    CS_Test_ResetEventCheck();
    UT_ResetState(UT_KEY(CFE_TBL_Register));
    UT_ResetState(UT_KEY(CFE_TBL_GetAddress));
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_GetAddress), 1, -123);
    UtAssert_INT32_EQ(CS_TableInit(tw, "UT", NULL), -123);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_TBL_INIT_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    /* failure to register definition table */
    CS_Test_ResetEventCheck();
    UT_ResetState(UT_KEY(CFE_TBL_Register));
    UT_ResetState(UT_KEY(CFE_TBL_GetAddress));
    UT_SetDataBuffer(UT_KEY(CFE_TBL_GetAddress), &tw->ResAddr, sizeof(tw->ResAddr), false);
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_GetAddress), 2, -123);
    UtAssert_INT32_EQ(CS_TableInit(tw, "UT", NULL), -123);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_TBL_INIT_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    /* failure to load definition table from file, but memory succeeds */
    CS_Test_ResetEventCheck();
    *tw->GlobalState = CS_ChecksumState_ENABLED;
    UT_ResetState(UT_KEY(CFE_TBL_Register));
    UT_ResetState(UT_KEY(CFE_TBL_GetAddress));
    UT_SetDataBuffer(UT_KEY(CFE_TBL_GetAddress), &tw->ResAddr, sizeof(tw->ResAddr), false);
    UT_SetDataBuffer(UT_KEY(CFE_TBL_GetAddress), &tw->DefAddr, sizeof(tw->DefAddr), false);
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_Load), 1, -456);
    UtAssert_INT32_EQ(CS_TableInit(tw, "UT", NULL), CFE_SUCCESS);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_UINT8_EQ(*tw->GlobalState, CS_ChecksumState_DISABLED);

    /* same but when there is no global staste */
    CS_Test_ResetEventCheck();
    tw->GlobalState = NULL;
    UT_ResetState(UT_KEY(CFE_TBL_Register));
    UT_ResetState(UT_KEY(CFE_TBL_GetAddress));
    UT_SetDataBuffer(UT_KEY(CFE_TBL_GetAddress), &tw->ResAddr, sizeof(tw->ResAddr), false);
    UT_SetDataBuffer(UT_KEY(CFE_TBL_GetAddress), &tw->DefAddr, sizeof(tw->DefAddr), false);
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_Load), 1, -456);
    UtAssert_INT32_EQ(CS_TableInit(tw, "UT", NULL), CFE_SUCCESS);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);

    /* failure to load definition table from anything */
    CS_Test_ResetEventCheck();
    UT_ResetState(UT_KEY(CFE_TBL_Register));
    UT_ResetState(UT_KEY(CFE_TBL_GetAddress));
    UT_SetDataBuffer(UT_KEY(CFE_TBL_GetAddress), &tw->ResAddr, sizeof(tw->ResAddr), false);
    UT_SetDataBuffer(UT_KEY(CFE_TBL_GetAddress), &tw->DefAddr, sizeof(tw->DefAddr), false);
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_Load), 1, -456);
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_Load), 1, -456);
    UtAssert_INT32_EQ(CS_TableInit(tw, "UT", NULL), -456);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_TBL_INIT_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);
}

void Test_CS_HandleTableUpdate(void)
{
    /* Test case for:
     * CFE_Status_t CS_HandleTableUpdate(CS_TableWrapper_t *tw);
     */
    CS_Res_Tables_Table_Entry_t *ResultsEntry;
    CS_TableWrapper_t           *tw;

    tw = &CS_AppData.Tbl[CS_ChecksumType_TABLES_TABLE];

    /* nominal */
    UT_SetDataBuffer(UT_KEY(CFE_TBL_GetAddress), &tw->ResAddr, sizeof(tw->ResAddr), false);
    UT_SetDataBuffer(UT_KEY(CFE_TBL_GetAddress), &tw->DefAddr, sizeof(tw->DefAddr), false);
    UtAssert_INT32_EQ(CS_HandleTableUpdate(tw), CFE_SUCCESS);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);

    /* failure to get result address */
    CS_Test_ResetEventCheck();
    UT_ResetState(UT_KEY(CFE_TBL_GetAddress));
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_GetAddress), 1, -123);
    UtAssert_INT32_EQ(CS_HandleTableUpdate(tw), -123);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_TBL_UPDATE_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    /* failure to get definition address */
    CS_Test_ResetEventCheck();
    UT_ResetState(UT_KEY(CFE_TBL_GetAddress));
    UT_SetDataBuffer(UT_KEY(CFE_TBL_GetAddress), &tw->ResAddr, sizeof(tw->ResAddr), false);
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_GetAddress), 2, -123);
    UtAssert_INT32_EQ(CS_HandleTableUpdate(tw), -123);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_TBL_UPDATE_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    /* CFE_TBL_INFO_UPDATED on definition address, not the tables table */
    tw = &CS_AppData.Tbl[CS_ChecksumType_APP_TABLE];
    CS_Test_ResetEventCheck();
    UT_ResetState(UT_KEY(CFE_TBL_GetAddress));
    UT_SetDataBuffer(UT_KEY(CFE_TBL_GetAddress), &tw->ResAddr, sizeof(tw->ResAddr), false);
    UT_SetDataBuffer(UT_KEY(CFE_TBL_GetAddress), &tw->DefAddr, sizeof(tw->DefAddr), false);
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_GetAddress), 2, CFE_TBL_INFO_UPDATED);
    UtAssert_INT32_EQ(CS_HandleTableUpdate(tw), CFE_SUCCESS);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);

    /* CFE_TBL_INFO_UPDATED on definition address, on the tables table, owned by CS */
    tw           = &CS_AppData.Tbl[CS_ChecksumType_TABLES_TABLE];
    ResultsEntry = tw->ResAddr;
    CS_Test_ResetEventCheck();
    UT_ResetState(UT_KEY(CFE_TBL_GetAddress));
    UT_SetDataBuffer(UT_KEY(CFE_TBL_GetAddress), &tw->ResAddr, sizeof(tw->ResAddr), false);
    UT_SetDataBuffer(UT_KEY(CFE_TBL_GetAddress), &tw->DefAddr, sizeof(tw->DefAddr), false);
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_GetAddress), 2, CFE_TBL_INFO_UPDATED);
    ResultsEntry->TblHandleID = CFE_TBL_HANDLEID_C(CFE_ResourceId_FromInteger(15));
    ResultsEntry->IsCSOwner   = true;
    UtAssert_INT32_EQ(CS_HandleTableUpdate(tw), CFE_SUCCESS);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_STUB_COUNT(CFE_TBL_Unregister, 0);

    /* CFE_TBL_INFO_UPDATED on definition address, on the tables table, not owned by CS */
    CS_Test_ResetEventCheck();
    UT_ResetState(UT_KEY(CFE_TBL_GetAddress));
    UT_SetDataBuffer(UT_KEY(CFE_TBL_GetAddress), &tw->ResAddr, sizeof(tw->ResAddr), false);
    UT_SetDataBuffer(UT_KEY(CFE_TBL_GetAddress), &tw->DefAddr, sizeof(tw->DefAddr), false);
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_GetAddress), 2, CFE_TBL_INFO_UPDATED);
    ResultsEntry->TblHandleID = CFE_TBL_HANDLEID_C(CFE_ResourceId_FromInteger(15));
    ResultsEntry->IsCSOwner   = false;
    UtAssert_INT32_EQ(CS_HandleTableUpdate(tw), CFE_SUCCESS);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_STUB_COUNT(CFE_TBL_Unregister, 1);
}

static void CS_UT_UpdateHandler(CS_TableWrapper_t *tw)
{
    UT_DEFAULT_IMPL(CS_UT_UpdateHandler);
}

void Test_CS_CallTableUpdateHandler(void)
{
    CS_TableWrapper_t *tw;

    tw = &CS_AppData.Tbl[CS_ChecksumType_EEPROM_TABLE];

    /* nominal */
    tw->UpdateHandler = CS_UT_UpdateHandler;
    UtAssert_VOIDCALL(CS_CallTableUpdateHandler(tw));
    UtAssert_STUB_COUNT(CS_UT_UpdateHandler, 1);

    /* unset */
    tw->UpdateHandler = NULL;
    UtAssert_VOIDCALL(CS_CallTableUpdateHandler(tw));
    UtAssert_STUB_COUNT(CS_UT_UpdateHandler, 1);
}

void UtTest_Setup(void)
{
    UtTest_Add(Test_CS_ValidateEepromChecksumDefinitionTable,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ValidateEepromChecksumDefinitionTable");
    UtTest_Add(Test_CS_ValidateMemoryChecksumDefinitionTable,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ValidateMemoryChecksumDefinitionTable");
    UtTest_Add(Test_CS_ValidateTablesChecksumDefinitionTable,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ValidateTablesChecksumDefinitionTable");
    UtTest_Add(Test_CS_ValidateAppChecksumDefinitionTable,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ValidateAppChecksumDefinitionTable");
    UtTest_Add(Test_CS_ProcessNewEepromMemoryDefinitionTable,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessNewEepromMemoryDefinitionTable");
    UtTest_Add(Test_CS_ProcessNewTablesDefinitionTable,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessNewTablesDefinitionTable");
    UtTest_Add(Test_CS_ProcessNewAppDefinitionTable,
               CS_Test_Setup,
               CS_Test_TearDown,
               "CS_ProcessNewAppDefinitionTable");
    UtTest_Add(Test_CS_TableInit, CS_Test_Setup, CS_Test_TearDown, "CS_TableInit");
    UtTest_Add(Test_CS_HandleTableUpdate, CS_Test_Setup, CS_Test_TearDown, "CS_HandleTableUpdate");
    UtTest_Add(Test_CS_CallTableUpdateHandler, CS_Test_Setup, CS_Test_TearDown, "CS_CallTableUpdateHandler");
}
