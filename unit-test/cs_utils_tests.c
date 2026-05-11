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

#include "cs_utils.h"
#include "cs_msg.h"
#include "cs_msgdefs.h"
#include "cs_eventids.h"
#include "cs_version.h"
#include "cs_compute.h"
#include "cs_test_utils.h"
#include <unistd.h>
#include <stdlib.h>

/* UT includes */
#include "uttest.h"
#include "utassert.h"
#include "utstubs.h"

/*
 * Function Definitions
 */

void CS_UTILS_TEST_CS_ComputeHandler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    bool *DoneWithEntry = UT_Hook_GetArgValueByName(Context, "DoneWithEntry", bool *);

    *DoneWithEntry = true;
}

void CS_ZeroEepromTempValues_Test(void)
{
    UtAssert_VOIDCALL(CS_ZeroEepromTempValues());
}

void CS_ZeroMemoryTempValues_Test(void)
{
    UtAssert_VOIDCALL(CS_ZeroMemoryTempValues());
}

void CS_ZeroTablesTempValues_Test(void)
{
    UtAssert_VOIDCALL(CS_ZeroTablesTempValues());
}

void CS_ZeroAppTempValues_Test(void)
{
    UtAssert_VOIDCALL(CS_ZeroAppTempValues());
}

void CS_ZeroCfeCoreTempValues_Test(void)
{
    UtAssert_VOIDCALL(CS_ZeroCfeCoreTempValues());

    CS_AppData.Tbl[CS_ChecksumType_CFECORE].ResAddr = NULL;
    UtAssert_VOIDCALL(CS_ZeroCfeCoreTempValues());
}

void CS_ZeroOSTempValues_Test(void)
{
    UtAssert_VOIDCALL(CS_ZeroOSTempValues());

    CS_AppData.Tbl[CS_ChecksumType_OSCORE].ResAddr = NULL;
    UtAssert_VOIDCALL(CS_ZeroOSTempValues());
}

void CS_InitializeDefaultTables_Test(void)
{
    UtAssert_VOIDCALL(CS_InitializeDefaultTables());
}

void CS_GoToNextTable_Test(void)
{
    CS_AppData.HkPacket.Payload.CurrentCSTable = CS_NUM_TABLES - 2;

    /* increment once */
    CS_GoToNextTable();

    /* Verify results */
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.CurrentCSTable, CS_NUM_TABLES - 1);
    UtAssert_UINT32_EQ(CS_AppData.HkPacket.Payload.PassCounter, 0);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.CurrentEntryInTable, 0);

    /* Cause loop */
    CS_GoToNextTable();

    /* Verify results */
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.CurrentCSTable, 0);
    UtAssert_UINT32_EQ(CS_AppData.HkPacket.Payload.PassCounter, 1);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.CurrentEntryInTable, 0);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
}

void CS_GetTableResTblEntryByName_Test(void)
{
    CS_Res_Tables_Table_Entry_t *EntryPtr        = NULL;
    CS_TableWrapper_t           *tw              = &CS_AppData.Tbl[CS_ChecksumType_TABLES_TABLE];
    CS_Res_Tables_Table_Entry_t *ResTablesTblPtr = tw->ResAddr;

    /* Empty name, enabled state */
    ResTablesTblPtr[0].State = CS_ChecksumState_ENABLED;

    /* Execute and verify */
    UtAssert_BOOL_FALSE(CS_GetTableResTblEntryByName(&EntryPtr, "name"));
    UtAssert_ADDRESS_EQ(EntryPtr, NULL);

    /* Matching name, empty state */
    strncpy(ResTablesTblPtr[0].Name, "name", 10);
    ResTablesTblPtr[0].State = CS_ChecksumState_EMPTY;

    /* Execute and verify */
    UtAssert_BOOL_FALSE(CS_GetTableResTblEntryByName(&EntryPtr, "name"));
    UtAssert_ADDRESS_EQ(EntryPtr, NULL);

    /* Matching name, enabled */
    ResTablesTblPtr[0].State = CS_ChecksumState_ENABLED;

    /* Execute and verify */
    UtAssert_BOOL_TRUE(CS_GetTableResTblEntryByName(&EntryPtr, "name"));
    UtAssert_ADDRESS_EQ(EntryPtr, ResTablesTblPtr);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
}

void CS_GetTableDefTblEntryByName_Test(void)
{
    CS_Def_Tables_Table_Entry_t *EntryPtr        = NULL;
    CS_TableWrapper_t           *tw              = &CS_AppData.Tbl[CS_ChecksumType_TABLES_TABLE];
    CS_Def_Tables_Table_Entry_t *DefTablesTblPtr = tw->DefAddr;

    /* Empty name, enabled state */
    DefTablesTblPtr[0].State = CS_ChecksumState_ENABLED;

    /* Execute and verify */
    UtAssert_BOOL_FALSE(CS_GetTableDefTblEntryByName(&EntryPtr, "name"));
    UtAssert_ADDRESS_EQ(EntryPtr, NULL);

    /* Matching name, empty state */
    strncpy(DefTablesTblPtr[0].Name, "name", 10);
    DefTablesTblPtr[0].State = CS_ChecksumState_EMPTY;

    /* Execute and verify */
    UtAssert_BOOL_FALSE(CS_GetTableDefTblEntryByName(&EntryPtr, "name"));
    UtAssert_ADDRESS_EQ(EntryPtr, NULL);

    /* Matching name, enabled */
    DefTablesTblPtr[0].State = CS_ChecksumState_ENABLED;

    /* Execute and verify */
    UtAssert_BOOL_TRUE(CS_GetTableDefTblEntryByName(&EntryPtr, "name"));
    UtAssert_ADDRESS_EQ(EntryPtr, DefTablesTblPtr);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
}

void CS_GetAppResTblEntryByName_Test(void)
{
    CS_Res_App_Table_Entry_t *EntryPtr     = NULL;
    CS_TableWrapper_t        *tw           = &CS_AppData.Tbl[CS_ChecksumType_APP_TABLE];
    CS_Res_App_Table_Entry_t *ResAppTblPtr = tw->ResAddr;

    /* Empty name, enabled state */
    ResAppTblPtr[0].State = CS_ChecksumState_ENABLED;

    /* Execute and verify */
    UtAssert_BOOL_FALSE(CS_GetAppResTblEntryByName(&EntryPtr, "name"));
    UtAssert_ADDRESS_EQ(EntryPtr, NULL);

    /* Matching name, empty state */
    strncpy(ResAppTblPtr[0].Name, "name", 10);
    ResAppTblPtr[0].State = CS_ChecksumState_EMPTY;

    /* Execute and verify */
    UtAssert_BOOL_FALSE(CS_GetAppResTblEntryByName(&EntryPtr, "name"));
    UtAssert_ADDRESS_EQ(EntryPtr, NULL);

    /* Matching name, enabled */
    ResAppTblPtr[0].State = CS_ChecksumState_ENABLED;

    /* Execute and verify */
    UtAssert_BOOL_TRUE(CS_GetAppResTblEntryByName(&EntryPtr, "name"));
    UtAssert_ADDRESS_EQ(EntryPtr, ResAppTblPtr);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
}

void CS_GetAppDefTblEntryByName_Test(void)
{
    CS_Def_App_Table_Entry_t *EntryPtr     = NULL;
    CS_TableWrapper_t        *tw           = &CS_AppData.Tbl[CS_ChecksumType_APP_TABLE];
    CS_Def_App_Table_Entry_t *DefAppTblPtr = tw->DefAddr;

    /* Empty name, enabled state */
    DefAppTblPtr[0].State = CS_ChecksumState_ENABLED;

    /* Execute and verify */
    UtAssert_BOOL_FALSE(CS_GetAppDefTblEntryByName(&EntryPtr, "name"));
    UtAssert_ADDRESS_EQ(EntryPtr, NULL);

    /* Matching name, empty state */
    strncpy(DefAppTblPtr[0].Name, "name", 10);
    DefAppTblPtr[0].State = CS_ChecksumState_EMPTY;

    /* Execute and verify */
    UtAssert_BOOL_FALSE(CS_GetAppDefTblEntryByName(&EntryPtr, "name"));
    UtAssert_ADDRESS_EQ(EntryPtr, NULL);

    /* Matching name, enabled */
    DefAppTblPtr[0].State = CS_ChecksumState_ENABLED;

    /* Execute and verify */
    UtAssert_BOOL_TRUE(CS_GetAppDefTblEntryByName(&EntryPtr, "name"));
    UtAssert_ADDRESS_EQ(EntryPtr, DefAppTblPtr);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
}

void CS_FindEnabledEepromEntry_Test(void)
{
    CS_TableWrapper_t                 *tw              = &CS_AppData.Tbl[CS_ChecksumType_EEPROM_TABLE];
    CS_Res_EepromMemory_Table_Entry_t *ResEepromTblPtr = tw->ResAddr;

    /* Call with zeros */
    UtAssert_NULL(CS_FindEnabledEepromEntry());
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.CurrentEntryInTable, CS_MAX_NUM_EEPROM_TABLE_ENTRIES);

    /* Set up to find last entry (skip first) */
    ResEepromTblPtr[0].State                                   = CS_ChecksumState_ENABLED;
    ResEepromTblPtr[CS_MAX_NUM_EEPROM_TABLE_ENTRIES - 1].State = CS_ChecksumState_ENABLED;
    CS_AppData.HkPacket.Payload.CurrentEntryInTable            = 1;

    UtAssert_NOT_NULL(CS_FindEnabledEepromEntry());
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.CurrentEntryInTable, CS_MAX_NUM_EEPROM_TABLE_ENTRIES - 1);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
}

void CS_FindEnabledMemoryEntry_Test(void)
{
    CS_TableWrapper_t                 *tw              = &CS_AppData.Tbl[CS_ChecksumType_MEMORY_TABLE];
    CS_Res_EepromMemory_Table_Entry_t *ResMemoryTblPtr = tw->ResAddr;

    /* Call with zeros */
    UtAssert_NULL(CS_FindEnabledMemoryEntry());
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.CurrentEntryInTable, CS_MAX_NUM_MEMORY_TABLE_ENTRIES);

    /* Set up to find last entry (skip first) */
    ResMemoryTblPtr[0].State                                   = CS_ChecksumState_ENABLED;
    ResMemoryTblPtr[CS_MAX_NUM_MEMORY_TABLE_ENTRIES - 1].State = CS_ChecksumState_ENABLED;
    CS_AppData.HkPacket.Payload.CurrentEntryInTable            = 1;

    UtAssert_NOT_NULL(CS_FindEnabledMemoryEntry());
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.CurrentEntryInTable, CS_MAX_NUM_MEMORY_TABLE_ENTRIES - 1);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
}

void CS_FindEnabledTablesEntry_Test(void)
{
    CS_TableWrapper_t           *tw              = &CS_AppData.Tbl[CS_ChecksumType_TABLES_TABLE];
    CS_Res_Tables_Table_Entry_t *ResTablesTblPtr = tw->ResAddr;

    /* Call with zeros */
    UtAssert_NULL(CS_FindEnabledTablesEntry());
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.CurrentEntryInTable, CS_MAX_NUM_TABLES_TABLE_ENTRIES);

    /* Set up to find last entry (skip first) */
    ResTablesTblPtr[0].State                                   = CS_ChecksumState_ENABLED;
    ResTablesTblPtr[CS_MAX_NUM_TABLES_TABLE_ENTRIES - 1].State = CS_ChecksumState_ENABLED;
    CS_AppData.HkPacket.Payload.CurrentEntryInTable            = 1;

    UtAssert_NOT_NULL(CS_FindEnabledTablesEntry());
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.CurrentEntryInTable, CS_MAX_NUM_TABLES_TABLE_ENTRIES - 1);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
}

void CS_FindEnabledAppEntry_Test(void)
{
    CS_TableWrapper_t        *tw           = &CS_AppData.Tbl[CS_ChecksumType_APP_TABLE];
    CS_Res_App_Table_Entry_t *ResAppTblPtr = tw->ResAddr;

    /* Call with zeros */
    UtAssert_NULL(CS_FindEnabledAppEntry());
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.CurrentEntryInTable, CS_MAX_NUM_APP_TABLE_ENTRIES);

    /* Set up to find last entry (skip first) */
    ResAppTblPtr[0].State                                = CS_ChecksumState_ENABLED;
    ResAppTblPtr[CS_MAX_NUM_APP_TABLE_ENTRIES - 1].State = CS_ChecksumState_ENABLED;
    CS_AppData.HkPacket.Payload.CurrentEntryInTable      = 1;

    UtAssert_NOT_NULL(CS_FindEnabledAppEntry());
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.CurrentEntryInTable, CS_MAX_NUM_APP_TABLE_ENTRIES - 1);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
}

void CS_BackgroundCfeCore_Test(void)
{
    CS_Res_EepromMemory_Table_Entry_t *CfeCoreCodeSeg = &CS_AppData.CfeCoreCodeSeg;

    /* Entirely disabled */
    CS_AppData.HkPacket.Payload.CfeCoreCSState = CS_ChecksumState_DISABLED;
    UtAssert_BOOL_FALSE(CS_BackgroundCfeCore());
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.CurrentEntryInTable, 0);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.CurrentCSTable, 1);

    /* Segment disabled */
    CS_AppData.HkPacket.Payload.CfeCoreCSState = CS_ChecksumState_ENABLED;
    CfeCoreCodeSeg->State                      = CS_ChecksumState_DISABLED;
    UtAssert_BOOL_FALSE(CS_BackgroundCfeCore());
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.CurrentEntryInTable, 0);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.CurrentCSTable, 2);

    /* Enabled, miscompare, not done with entry */
    CfeCoreCodeSeg->State = CS_ChecksumState_ENABLED;
    UT_SetDeferredRetcode(UT_KEY(CS_ComputeEepromMemory), 1, CS_ERROR);
    UtAssert_BOOL_TRUE(CS_BackgroundCfeCore());
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.CfeCoreCSErrCounter, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_CFECORE_MISCOMPARE_ERR_EID);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.CurrentEntryInTable, 0);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.CurrentCSTable, 2);

    /* Enabled, compares, done with entry */
    UT_SetHandlerFunction(UT_KEY(CS_ComputeEepromMemory), CS_UTILS_TEST_CS_ComputeHandler, NULL);
    UtAssert_BOOL_TRUE(CS_BackgroundCfeCore());
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.CfeCoreCSErrCounter, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.CurrentEntryInTable, 0);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.CurrentCSTable, 3);
}

void CS_BackgroundOS_Test(void)
{
    CS_Res_EepromMemory_Table_Entry_t *OSCodeSeg = &CS_AppData.OSCodeSeg;

    /* Entirely disabled */
    CS_AppData.HkPacket.Payload.OSCSState = CS_ChecksumState_DISABLED;
    UtAssert_BOOL_FALSE(CS_BackgroundOS());
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.CurrentEntryInTable, 0);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.CurrentCSTable, 1);

    /* Segment disabled */
    CS_AppData.HkPacket.Payload.OSCSState = CS_ChecksumState_ENABLED;
    OSCodeSeg->State                      = CS_ChecksumState_DISABLED;
    UtAssert_BOOL_FALSE(CS_BackgroundOS());
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.CurrentEntryInTable, 0);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.CurrentCSTable, 2);

    /* Enabled, miscompare, not done with entry */
    OSCodeSeg->State = CS_ChecksumState_ENABLED;
    UT_SetDeferredRetcode(UT_KEY(CS_ComputeEepromMemory), 1, CS_ERROR);
    UtAssert_BOOL_TRUE(CS_BackgroundOS());
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.OSCSErrCounter, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_OS_MISCOMPARE_ERR_EID);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.CurrentEntryInTable, 0);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.CurrentCSTable, 2);

    /* Enabled, compares, done with entry */
    UT_SetHandlerFunction(UT_KEY(CS_ComputeEepromMemory), CS_UTILS_TEST_CS_ComputeHandler, NULL);
    UtAssert_BOOL_TRUE(CS_BackgroundOS());
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.OSCSErrCounter, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.CurrentEntryInTable, 0);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.CurrentCSTable, 3);
}

void CS_BackgroundEeprom_Test(void)
{
    CS_TableWrapper_t                 *tw              = &CS_AppData.Tbl[CS_ChecksumType_EEPROM_TABLE];
    CS_Res_EepromMemory_Table_Entry_t *ResEepromTblPtr = tw->ResAddr;

    /* Entirely disabled */
    CS_AppData.HkPacket.Payload.EepromCSState = CS_ChecksumState_DISABLED;
    UtAssert_BOOL_FALSE(CS_BackgroundEeprom());
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.CurrentEntryInTable, 0);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.CurrentCSTable, 1);

    /* All entries disabled */
    CS_AppData.HkPacket.Payload.EepromCSState = CS_ChecksumState_ENABLED;
    ResEepromTblPtr[0].ComparisonValue        = 1;
    ResEepromTblPtr[1].ComparisonValue        = 2;
    UtAssert_BOOL_FALSE(CS_BackgroundEeprom());
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.CurrentEntryInTable, 0);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.CurrentCSTable, 2);
    UtAssert_UINT32_EQ(CS_AppData.HkPacket.Payload.EepromBaseline, 3);

    /* Enabled, miscompare, not done with entry */
    ResEepromTblPtr[0].State = CS_ChecksumState_ENABLED;
    UT_SetDeferredRetcode(UT_KEY(CS_ComputeEepromMemory), 1, CS_ERROR);
    UtAssert_BOOL_TRUE(CS_BackgroundEeprom());
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.EepromCSErrCounter, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_EEPROM_MISCOMPARE_ERR_EID);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.CurrentEntryInTable, 0);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.CurrentCSTable, 2);

    /* Last entry, Enabled, compares, done with entry */
    CS_AppData.HkPacket.Payload.CurrentEntryInTable                        = CS_MAX_NUM_EEPROM_TABLE_ENTRIES - 1;
    ResEepromTblPtr[CS_AppData.HkPacket.Payload.CurrentEntryInTable].State = CS_ChecksumState_ENABLED;
    ResEepromTblPtr[CS_AppData.HkPacket.Payload.CurrentEntryInTable].ComparisonValue = 3;
    UT_SetHandlerFunction(UT_KEY(CS_ComputeEepromMemory), CS_UTILS_TEST_CS_ComputeHandler, NULL);
    UtAssert_BOOL_TRUE(CS_BackgroundEeprom());
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.EepromCSErrCounter, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.CurrentEntryInTable, 0);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.CurrentCSTable, 3);
    UtAssert_UINT32_EQ(CS_AppData.HkPacket.Payload.EepromBaseline, 6);
}

void CS_BackgroundMemory_Test(void)
{
    CS_TableWrapper_t                 *tw              = &CS_AppData.Tbl[CS_ChecksumType_MEMORY_TABLE];
    CS_Res_EepromMemory_Table_Entry_t *ResMemoryTblPtr = tw->ResAddr;

    /* Entirely disabled */
    CS_AppData.HkPacket.Payload.MemoryCSState = CS_ChecksumState_DISABLED;
    UtAssert_BOOL_FALSE(CS_BackgroundMemory());
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.CurrentEntryInTable, 0);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.CurrentCSTable, 1);

    /* All entries disabled */
    CS_AppData.HkPacket.Payload.MemoryCSState = CS_ChecksumState_ENABLED;
    UtAssert_BOOL_FALSE(CS_BackgroundMemory());
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.CurrentEntryInTable, 0);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.CurrentCSTable, 2);

    /* Enabled, miscompare, not done with entry */
    ResMemoryTblPtr[0].State = CS_ChecksumState_ENABLED;
    UT_SetDeferredRetcode(UT_KEY(CS_ComputeEepromMemory), 1, CS_ERROR);
    UtAssert_BOOL_TRUE(CS_BackgroundMemory());
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.MemoryCSErrCounter, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_MEMORY_MISCOMPARE_ERR_EID);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.CurrentEntryInTable, 0);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.CurrentCSTable, 2);

    /* Last entry, Enabled, compares, done with entry */
    CS_AppData.HkPacket.Payload.CurrentEntryInTable                        = CS_MAX_NUM_MEMORY_TABLE_ENTRIES - 1;
    ResMemoryTblPtr[CS_AppData.HkPacket.Payload.CurrentEntryInTable].State = CS_ChecksumState_ENABLED;
    UT_SetHandlerFunction(UT_KEY(CS_ComputeEepromMemory), CS_UTILS_TEST_CS_ComputeHandler, NULL);
    UtAssert_BOOL_TRUE(CS_BackgroundMemory());
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.MemoryCSErrCounter, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.CurrentEntryInTable, 0);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.CurrentCSTable, 3);
}

void CS_BackgroundTables_Test(void)
{
    CS_TableWrapper_t           *tw              = &CS_AppData.Tbl[CS_ChecksumType_TABLES_TABLE];
    CS_Res_Tables_Table_Entry_t *ResTablesTblPtr = tw->ResAddr;

    /* Entirely disabled */
    CS_AppData.HkPacket.Payload.TablesCSState = CS_ChecksumState_DISABLED;
    UtAssert_BOOL_FALSE(CS_BackgroundTables());
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.CurrentEntryInTable, 0);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.CurrentCSTable, 1);

    /* All entries disabled */
    CS_AppData.HkPacket.Payload.TablesCSState = CS_ChecksumState_ENABLED;
    UtAssert_BOOL_FALSE(CS_BackgroundTables());
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.CurrentEntryInTable, 0);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.CurrentCSTable, 2);

    /* Enabled, miscompare, not done with entry */
    ResTablesTblPtr[0].State = CS_ChecksumState_ENABLED;
    UT_SetDeferredRetcode(UT_KEY(CS_ComputeTables), 1, CS_ERROR);
    UtAssert_BOOL_TRUE(CS_BackgroundTables());
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.TablesCSErrCounter, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_TABLES_MISCOMPARE_ERR_EID);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.CurrentEntryInTable, 0);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.CurrentCSTable, 2);

    /* Enabled, not found, not done with entry */
    UT_SetDeferredRetcode(UT_KEY(CS_ComputeTables), 1, CS_ERR_NOT_FOUND);
    UtAssert_BOOL_TRUE(CS_BackgroundTables());
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.TablesCSErrCounter, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 2);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, CS_COMPUTE_TABLES_NOT_FOUND_ERR_EID);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.CurrentEntryInTable, 1);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.CurrentCSTable, 2);

    /* Last entry, Enabled, compares, done with entry */
    CS_AppData.HkPacket.Payload.CurrentEntryInTable                        = CS_MAX_NUM_TABLES_TABLE_ENTRIES - 1;
    ResTablesTblPtr[CS_AppData.HkPacket.Payload.CurrentEntryInTable].State = CS_ChecksumState_ENABLED;
    UT_SetHandlerFunction(UT_KEY(CS_ComputeTables), CS_UTILS_TEST_CS_ComputeHandler, NULL);
    UtAssert_BOOL_TRUE(CS_BackgroundTables());
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.TablesCSErrCounter, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 2);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.CurrentEntryInTable, 0);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.CurrentCSTable, 3);
}

void CS_BackgroundApp_Test(void)
{
    CS_TableWrapper_t        *tw           = &CS_AppData.Tbl[CS_ChecksumType_APP_TABLE];
    CS_Res_App_Table_Entry_t *ResAppTblPtr = tw->ResAddr;

    /* Entirely disabled */
    CS_AppData.HkPacket.Payload.AppCSState = CS_ChecksumState_DISABLED;
    UtAssert_BOOL_FALSE(CS_BackgroundApp());
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.CurrentEntryInTable, 0);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.CurrentCSTable, 1);

    /* All entries disabled */
    CS_AppData.HkPacket.Payload.AppCSState = CS_ChecksumState_ENABLED;
    UtAssert_BOOL_FALSE(CS_BackgroundApp());
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.CurrentEntryInTable, 0);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.CurrentCSTable, 2);

    /* Enabled, miscompare, not done with entry */
    ResAppTblPtr[0].State = CS_ChecksumState_ENABLED;
    UT_SetDeferredRetcode(UT_KEY(CS_ComputeApp), 1, CS_ERROR);
    UtAssert_BOOL_TRUE(CS_BackgroundApp());
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.AppCSErrCounter, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_APP_MISCOMPARE_ERR_EID);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.CurrentEntryInTable, 0);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.CurrentCSTable, 2);

    /* Enabled, not found, not done with entry */
    UT_SetDeferredRetcode(UT_KEY(CS_ComputeApp), 1, CS_ERR_NOT_FOUND);
    UtAssert_BOOL_TRUE(CS_BackgroundApp());
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.AppCSErrCounter, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 2);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, CS_COMPUTE_APP_NOT_FOUND_ERR_EID);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.CurrentEntryInTable, 1);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.CurrentCSTable, 2);

    /* Last entry, Enabled, compares, done with entry */
    CS_AppData.HkPacket.Payload.CurrentEntryInTable                     = CS_MAX_NUM_APP_TABLE_ENTRIES - 1;
    ResAppTblPtr[CS_AppData.HkPacket.Payload.CurrentEntryInTable].State = CS_ChecksumState_ENABLED;
    UT_SetHandlerFunction(UT_KEY(CS_ComputeApp), CS_UTILS_TEST_CS_ComputeHandler, NULL);
    UtAssert_BOOL_TRUE(CS_BackgroundApp());
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.AppCSErrCounter, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 2);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.CurrentEntryInTable, 0);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.Payload.CurrentCSTable, 3);
}

void CS_ResetTablesTblResultEntry_Test(void)
{
    CS_Res_Tables_Table_Entry_t ResTablesTbl;

    /* Null check */
    UtAssert_VOIDCALL(CS_ResetTablesTblResultEntry(NULL));

    /* Check it clears entry */
    memset(&ResTablesTbl, 0xFF, sizeof(ResTablesTbl));
    UtAssert_VOIDCALL(CS_ResetTablesTblResultEntry(&ResTablesTbl));

    /* Verify results */
    UtAssert_UINT32_EQ(ResTablesTbl.ByteOffset, 0);
    UtAssert_UINT32_EQ(ResTablesTbl.TempChecksumValue, 0);
    UtAssert_UINT16_EQ(ResTablesTbl.ComputedYet, false);
}

void CS_HandleRoutineTableUpdates_Test(void)
{
    uint16 ChildTaskTable[] = { CS_ChecksumType_CFECORE,
                                CS_ChecksumType_EEPROM_TABLE,
                                CS_ChecksumType_MEMORY_TABLE,
                                CS_ChecksumType_APP_TABLE,
                                CS_ChecksumType_TABLES_TABLE };
    uint16 TblMax           = sizeof(ChildTaskTable) / sizeof(ChildTaskTable[0]);
    uint16 i;

    for (i = 0; i < TblMax; i++)
    {
        if (CS_AppData.Tbl[ChildTaskTable[i]].GlobalState)
        {
            *CS_AppData.Tbl[ChildTaskTable[i]].GlobalState = CS_ChecksumState_ENABLED;
        }
    }

    UtAssert_UINT8_EQ(CS_AppData.HkPacket.Payload.AppCSState, CS_ChecksumState_ENABLED);
    UtAssert_UINT8_EQ(CS_AppData.HkPacket.Payload.TablesCSState, CS_ChecksumState_ENABLED);
    UtAssert_UINT8_EQ(CS_AppData.HkPacket.Payload.MemoryCSState, CS_ChecksumState_ENABLED);
    UtAssert_UINT8_EQ(CS_AppData.HkPacket.Payload.EepromCSState, CS_ChecksumState_ENABLED);

    CS_AppData.Tbl[CS_ChecksumType_EEPROM_TABLE].GlobalState = NULL;

    /* Cycle through each all true case */
    CS_AppData.HkPacket.Payload.RecomputeInProgress = true;
    CS_AppData.HkPacket.Payload.OneShotInProgress   = false;
    for (i = 0; i < TblMax; i++)
    {
        CS_AppData.ChildTaskTable = ChildTaskTable[i];

        UT_ResetState(UT_KEY(CS_HandleTableUpdate));
        UtAssert_INT32_EQ(CS_HandleRoutineTableUpdates(), CFE_SUCCESS);

        /* Confirm stub call count based on selected table */
        if (CS_AppData.ChildTaskTable == CS_ChecksumType_CFECORE)
        {
            UtAssert_STUB_COUNT(CS_HandleTableUpdate, 4);
        }
        else
        {
            UtAssert_STUB_COUNT(CS_HandleTableUpdate, 3);
        }
        UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    }

    /* OneShotInProgress true case will update all */
    CS_AppData.HkPacket.Payload.OneShotInProgress = true;
    for (i = 0; i < TblMax; i++)
    {
        CS_AppData.ChildTaskTable = ChildTaskTable[i];

        UT_ResetState(UT_KEY(CS_HandleTableUpdate));
        UtAssert_INT32_EQ(CS_HandleRoutineTableUpdates(), CFE_SUCCESS);
        UtAssert_STUB_COUNT(CS_HandleTableUpdate, 4);
        UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    }

    /* Recompute false will update all */
    CS_AppData.HkPacket.Payload.RecomputeInProgress = false;
    CS_AppData.HkPacket.Payload.OneShotInProgress   = false;
    for (i = 0; i < TblMax; i++)
    {
        CS_AppData.ChildTaskTable = ChildTaskTable[i];

        UT_ResetState(UT_KEY(CS_HandleTableUpdate));
        UtAssert_INT32_EQ(CS_HandleRoutineTableUpdates(), CFE_SUCCESS);
        UtAssert_STUB_COUNT(CS_HandleTableUpdate, 4);
        UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    }

    /* Force each update error */
    UT_ResetState(UT_KEY(CS_HandleTableUpdate));
    UT_SetDefaultReturnValue(UT_KEY(CS_HandleTableUpdate), -1);
    UtAssert_INT32_EQ(CS_HandleRoutineTableUpdates(), CFE_SUCCESS);
    UtAssert_STUB_COUNT(CS_HandleTableUpdate, 4);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 4);

    UtAssert_UINT8_EQ(CS_AppData.HkPacket.Payload.AppCSState, CS_ChecksumState_DISABLED);
    UtAssert_UINT8_EQ(CS_AppData.HkPacket.Payload.TablesCSState, CS_ChecksumState_DISABLED);
    UtAssert_UINT8_EQ(CS_AppData.HkPacket.Payload.MemoryCSState, CS_ChecksumState_DISABLED);
    UtAssert_UINT8_EQ(CS_AppData.HkPacket.Payload.EepromCSState, CS_ChecksumState_ENABLED);

    /* Force send event errors */
    UT_ResetState(UT_KEY(CS_HandleTableUpdate));
    UT_ResetState(UT_KEY(CFE_EVS_SendEvent));
    UT_SetDefaultReturnValue(UT_KEY(CS_HandleTableUpdate), -1);
    UT_SetDefaultReturnValue(UT_KEY(CFE_EVS_SendEvent), -2);
    UtAssert_INT32_EQ(CS_HandleRoutineTableUpdates(), CFE_SUCCESS);
    UtAssert_STUB_COUNT(CS_HandleTableUpdate, 4);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 4);
}

void CS_CheckRecomputeOneShot_Test(void)
{
    /* Set up for false return */
    CS_AppData.HkPacket.Payload.RecomputeInProgress = false;
    CS_AppData.HkPacket.Payload.OneShotInProgress   = false;

    UtAssert_BOOL_FALSE(CS_CheckRecomputeOneshot());
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_UINT8_EQ(CS_AppData.HkPacket.Payload.CommandErrorCounter, 0);

    /* One shot in progress */
    CS_AppData.HkPacket.Payload.OneShotInProgress = true;
    UtAssert_BOOL_TRUE(CS_CheckRecomputeOneshot());
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_CMD_COMPUTE_PROG_ERR_EID);
    UtAssert_UINT8_EQ(CS_AppData.HkPacket.Payload.CommandErrorCounter, 1);

    /* Recompute in progress */
    CS_AppData.HkPacket.Payload.RecomputeInProgress = true;
    CS_AppData.HkPacket.Payload.OneShotInProgress   = false;
    UtAssert_BOOL_TRUE(CS_CheckRecomputeOneshot());
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 2);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, CS_CMD_COMPUTE_PROG_ERR_EID);
    UtAssert_UINT8_EQ(CS_AppData.HkPacket.Payload.CommandErrorCounter, 2);
}

void CS_GetTableTypeAsString_Test(void)
{
    uint32 i;

    /* Just call the function with every table type, it should never return NULL */
    for (i = 0; i <= CS_NUM_TABLES; ++i)
    {
        UtAssert_NOT_NULL(CS_GetTableTypeAsString(&CS_AppData.Tbl[i]));
    }
}

void Test_CS_SetDefEntryState(void)
{
    /* Test case for:
     * uint16 CS_SetDefEntryState(CS_TableWrapper_t *tw, void *EntryPtr, uint16 NewState);
     */
    CS_TableWrapper_t                 *tw;
    CS_Def_EepromMemory_Table_Entry_t *DefEntry;

    tw       = &CS_AppData.Tbl[CS_ChecksumType_MEMORY_TABLE];
    DefEntry = tw->DefAddr;

    tw->DefStateOffset = offsetof(CS_Def_EepromMemory_Table_Entry_t, State);

    DefEntry->State = CS_ChecksumState_UNDEFINED;
    UtAssert_UINT16_EQ(CS_SetDefEntryState(tw, DefEntry, CS_ChecksumState_ENABLED), CS_ChecksumState_UNDEFINED);
    UtAssert_STUB_COUNT(CFE_TBL_Modified, 0);
    UtAssert_UINT16_EQ(DefEntry->State, CS_ChecksumState_UNDEFINED);

    DefEntry->State = CS_ChecksumState_ENABLED;
    UtAssert_UINT16_EQ(CS_SetDefEntryState(tw, DefEntry, CS_ChecksumState_DISABLED), CS_ChecksumState_ENABLED);
    UtAssert_STUB_COUNT(CFE_TBL_Modified, 1);
    UtAssert_UINT16_EQ(DefEntry->State, CS_ChecksumState_DISABLED);
    UtAssert_UINT16_EQ(CS_SetDefEntryState(tw, DefEntry, CS_ChecksumState_DISABLED), CS_ChecksumState_DISABLED);
    UtAssert_STUB_COUNT(CFE_TBL_Modified, 1);
    UtAssert_UINT16_EQ(DefEntry->State, CS_ChecksumState_DISABLED);
    UtAssert_UINT16_EQ(CS_SetDefEntryState(tw, DefEntry, CS_ChecksumState_ENABLED), CS_ChecksumState_DISABLED);
    UtAssert_STUB_COUNT(CFE_TBL_Modified, 2);
    UtAssert_UINT16_EQ(DefEntry->State, CS_ChecksumState_ENABLED);

    UtAssert_UINT16_EQ(CS_SetDefEntryState(tw, DefEntry, CS_ChecksumState_EMPTY), CS_ChecksumState_UNDEFINED);
    UtAssert_STUB_COUNT(CFE_TBL_Modified, 2);
    UtAssert_UINT16_EQ(DefEntry->State, CS_ChecksumState_ENABLED);
}

void Test_CS_CheckDefTableNameMatch(void)
{
    /* Test case for:
     * bool CS_CheckDefTableNameMatch(const char *Name, uint16 TableIdx);
     */
    CS_TableWrapper_t *tw;

    tw = &CS_AppData.Tbl[CS_ChecksumType_MEMORY_TABLE];

    tw->DefTableName = NULL;
    UtAssert_BOOL_FALSE(CS_CheckDefTableNameMatch("UT", CS_ChecksumType_MEMORY_TABLE));
    UtAssert_BOOL_FALSE(CS_CheckDefTableNameMatch(NULL, CS_ChecksumType_MEMORY_TABLE));

    tw->DefTableName = "UT";
    UtAssert_BOOL_TRUE(CS_CheckDefTableNameMatch("UT", CS_ChecksumType_MEMORY_TABLE));
    UtAssert_BOOL_FALSE(CS_CheckDefTableNameMatch("X", CS_ChecksumType_MEMORY_TABLE));
    UtAssert_BOOL_FALSE(CS_CheckDefTableNameMatch(NULL, CS_ChecksumType_MEMORY_TABLE));
}

void Test_CS_CheckResTableNameMatch(void)
{
    /* Test case for:
     * bool CS_CheckResTableNameMatch(const char *Name, uint16 TableIdx);
     */
    CS_TableWrapper_t *tw;

    tw = &CS_AppData.Tbl[CS_ChecksumType_MEMORY_TABLE];

    tw->ResTableName = NULL;
    UtAssert_BOOL_FALSE(CS_CheckResTableNameMatch("UT", CS_ChecksumType_MEMORY_TABLE));
    UtAssert_BOOL_FALSE(CS_CheckResTableNameMatch(NULL, CS_ChecksumType_MEMORY_TABLE));

    tw->ResTableName = "UT";
    UtAssert_BOOL_TRUE(CS_CheckResTableNameMatch("UT", CS_ChecksumType_MEMORY_TABLE));
    UtAssert_BOOL_FALSE(CS_CheckResTableNameMatch("X", CS_ChecksumType_MEMORY_TABLE));
    UtAssert_BOOL_FALSE(CS_CheckResTableNameMatch(NULL, CS_ChecksumType_MEMORY_TABLE));
}

void Test_CS_GetDefEntryAddr(void)
{
    /* Test case for:
     * void *CS_GetDefEntryAddr(CS_TableWrapper_t *tw, uint16 EntryIdx);
     */

    CS_TableWrapper_t                 *tw;
    CS_Def_EepromMemory_Table_Entry_t *DefEntry;

    tw       = &CS_AppData.Tbl[CS_ChecksumType_MEMORY_TABLE];
    DefEntry = tw->DefAddr;

    UtAssert_ADDRESS_EQ(CS_GetDefEntryAddr(tw, 0), DefEntry);
    UtAssert_ADDRESS_EQ(CS_GetDefEntryAddr(tw, 3), &DefEntry[3]);
    UtAssert_NULL(CS_GetDefEntryAddr(tw, tw->NumEntries + 1));

    tw->DefAddr = NULL;
    UtAssert_NULL(CS_GetDefEntryAddr(tw, tw->NumEntries - 1));
}

void Test_CS_GetResEntryAddr(void)
{
    /* Test case for:
     * void *CS_GetResEntryAddr(CS_TableWrapper_t *tw, uint16 EntryIdx);
     */

    CS_TableWrapper_t                 *tw;
    CS_Res_EepromMemory_Table_Entry_t *ResEntry;

    tw       = &CS_AppData.Tbl[CS_ChecksumType_MEMORY_TABLE];
    ResEntry = tw->ResAddr;

    UtAssert_ADDRESS_EQ(CS_GetResEntryAddr(tw, 0), ResEntry);
    UtAssert_ADDRESS_EQ(CS_GetResEntryAddr(tw, 3), &ResEntry[3]);
    UtAssert_NULL(CS_GetResEntryAddr(tw, tw->NumEntries + 1));

    tw->ResAddr = NULL;
    UtAssert_NULL(CS_GetResEntryAddr(tw, tw->NumEntries - 1));
}

void UtTest_Setup(void)
{
    UtTest_Add(CS_ZeroEepromTempValues_Test, CS_Test_Setup, CS_Test_TearDown, "CS_ZeroEepromTempValues_Test");
    UtTest_Add(CS_ZeroMemoryTempValues_Test, CS_Test_Setup, CS_Test_TearDown, "CS_ZeroMemoryTempValues_Test");
    UtTest_Add(CS_ZeroTablesTempValues_Test, CS_Test_Setup, CS_Test_TearDown, "CS_ZeroTablesTempValues_Test");
    UtTest_Add(CS_ZeroAppTempValues_Test, CS_Test_Setup, CS_Test_TearDown, "CS_ZeroAppTempValues_Test");
    UtTest_Add(CS_ZeroCfeCoreTempValues_Test, CS_Test_Setup, CS_Test_TearDown, "CS_ZeroCfeCoreTempValues_Test");
    UtTest_Add(CS_ZeroOSTempValues_Test, CS_Test_Setup, CS_Test_TearDown, "CS_ZeroOSTempValues_Test");
    UtTest_Add(CS_InitializeDefaultTables_Test, CS_Test_Setup, CS_Test_TearDown, "CS_InitializeDefaultTables_Test");
    UtTest_Add(CS_GoToNextTable_Test, CS_Test_Setup, CS_Test_TearDown, "CS_GoToNextTable_Test");
    UtTest_Add(CS_GetTableResTblEntryByName_Test, CS_Test_Setup, CS_Test_TearDown, "CS_GetTableResTblEntryByName_Test");
    UtTest_Add(CS_GetTableDefTblEntryByName_Test, CS_Test_Setup, CS_Test_TearDown, "CS_GetTableDefTblEntryByName_Test");
    UtTest_Add(CS_GetAppResTblEntryByName_Test, CS_Test_Setup, CS_Test_TearDown, "CS_GetAppResTblEntryByName_Test");
    UtTest_Add(CS_GetAppDefTblEntryByName_Test, CS_Test_Setup, CS_Test_TearDown, "CS_GetAppDefTblEntryByName_Test");
    UtTest_Add(CS_FindEnabledEepromEntry_Test, CS_Test_Setup, CS_Test_TearDown, "CS_FindEnabledEepromEntry_Test");
    UtTest_Add(CS_FindEnabledMemoryEntry_Test, CS_Test_Setup, CS_Test_TearDown, "CS_FindEnabledMemoryEntry_Test");
    UtTest_Add(CS_FindEnabledTablesEntry_Test, CS_Test_Setup, CS_Test_TearDown, "CS_FindEnabledTablesEntry_Test");
    UtTest_Add(CS_FindEnabledAppEntry_Test, CS_Test_Setup, CS_Test_TearDown, "CS_FindEnabledAppEntry_Test");
    UtTest_Add(CS_BackgroundCfeCore_Test, CS_Test_Setup, CS_Test_TearDown, "CS_BackgroundCfeCore_Test");
    UtTest_Add(CS_BackgroundOS_Test, CS_Test_Setup, CS_Test_TearDown, "CS_BackgroundOS_Test");
    UtTest_Add(CS_BackgroundEeprom_Test, CS_Test_Setup, CS_Test_TearDown, "CS_BackgroundEeprom_Test");
    UtTest_Add(CS_BackgroundMemory_Test, CS_Test_Setup, CS_Test_TearDown, "CS_BackgroundMemory_Test");
    UtTest_Add(CS_BackgroundTables_Test, CS_Test_Setup, CS_Test_TearDown, "CS_BackgroundTables_Test");
    UtTest_Add(CS_BackgroundApp_Test, CS_Test_Setup, CS_Test_TearDown, "CS_BackgroundApp_Test");
    UtTest_Add(CS_ResetTablesTblResultEntry_Test, CS_Test_Setup, CS_Test_TearDown, "CS_ResetTablesTblResultEntry_Test");
    UtTest_Add(CS_HandleRoutineTableUpdates_Test, CS_Test_Setup, CS_Test_TearDown, "CS_HandleRoutineTableUpdates_Test");
    UtTest_Add(CS_CheckRecomputeOneShot_Test, CS_Test_Setup, CS_Test_TearDown, "CS_CheckRecomputeOneShot_Test");
    UtTest_Add(CS_GetTableTypeAsString_Test, CS_Test_Setup, CS_Test_TearDown, "CS_GetTableTypeAsString_Test");

    UtTest_Add(Test_CS_SetDefEntryState, CS_Test_Setup, CS_Test_TearDown, "CS_SetDefEntryState");
    UtTest_Add(Test_CS_CheckDefTableNameMatch, CS_Test_Setup, CS_Test_TearDown, "CS_CheckDefTableNameMatch");
    UtTest_Add(Test_CS_CheckResTableNameMatch, CS_Test_Setup, CS_Test_TearDown, "CS_CheckResTableNameMatch");
    UtTest_Add(Test_CS_GetDefEntryAddr, CS_Test_Setup, CS_Test_TearDown, "CS_GetDefEntryAddr");
    UtTest_Add(Test_CS_GetResEntryAddr, CS_Test_Setup, CS_Test_TearDown, "CS_GetResEntryAddr");
}
