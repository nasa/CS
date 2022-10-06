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

#include "cs_utils.h"
#include "cs_msg.h"
#include "cs_msgdefs.h"
#include "cs_events.h"
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
    CS_ZeroEepromTempValues();
}

void CS_ZeroMemoryTempValues_Test(void)
{
    CS_ZeroMemoryTempValues();
}

void CS_ZeroTablesTempValues_Test(void)
{
    CS_ZeroTablesTempValues();
}

void CS_ZeroAppTempValues_Test(void)
{
    CS_ZeroAppTempValues();
}

void CS_ZeroCfeCoreTempValues_Test(void)
{
    CS_ZeroCfeCoreTempValues();
}

void CS_ZeroOSTempValues_Test(void)
{
    CS_ZeroOSTempValues();
}

void CS_InitializeDefaultTables_Test(void)
{
    CS_InitializeDefaultTables();
}

void CS_GoToNextTable_Test(void)
{
    CS_AppData.HkPacket.CurrentCSTable = CS_NUM_TABLES - 2;

    /* increment once */
    CS_GoToNextTable();

    /* Verify results */
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.CurrentCSTable, CS_NUM_TABLES - 1);
    UtAssert_UINT32_EQ(CS_AppData.HkPacket.PassCounter, 0);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.CurrentEntryInTable, 0);

    /* Cause loop */
    CS_GoToNextTable();

    /* Verify results */
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.CurrentCSTable, 0);
    UtAssert_UINT32_EQ(CS_AppData.HkPacket.PassCounter, 1);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.CurrentEntryInTable, 0);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
}

void CS_GetTableResTblEntryByName_Test(void)
{
    CS_Res_Tables_Table_Entry_t *EntryPtr = NULL;

    /* Empty name, enabled state */
    CS_AppData.ResTablesTblPtr[0].State = CS_STATE_ENABLED;

    /* Execute and verify */
    UtAssert_BOOL_FALSE(CS_GetTableResTblEntryByName(&EntryPtr, "name"));
    UtAssert_ADDRESS_EQ(EntryPtr, NULL);

    /* Matching name, empty state */
    strncpy(CS_AppData.ResTablesTblPtr[0].Name, "name", 10);
    CS_AppData.ResTablesTblPtr[0].State = CS_STATE_EMPTY;

    /* Execute and verify */
    UtAssert_BOOL_FALSE(CS_GetTableResTblEntryByName(&EntryPtr, "name"));
    UtAssert_ADDRESS_EQ(EntryPtr, NULL);

    /* Matching name, enabled */
    CS_AppData.ResTablesTblPtr[0].State = CS_STATE_ENABLED;

    /* Execute and verify */
    UtAssert_BOOL_TRUE(CS_GetTableResTblEntryByName(&EntryPtr, "name"));
    UtAssert_ADDRESS_EQ(EntryPtr, CS_AppData.ResTablesTblPtr);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
}

void CS_GetTableDefTblEntryByName_Test(void)
{
    CS_Def_Tables_Table_Entry_t *EntryPtr = NULL;

    /* Empty name, enabled state */
    CS_AppData.DefTablesTblPtr[0].State = CS_STATE_ENABLED;

    /* Execute and verify */
    UtAssert_BOOL_FALSE(CS_GetTableDefTblEntryByName(&EntryPtr, "name"));
    UtAssert_ADDRESS_EQ(EntryPtr, NULL);

    /* Matching name, empty state */
    strncpy(CS_AppData.DefTablesTblPtr[0].Name, "name", 10);
    CS_AppData.DefTablesTblPtr[0].State = CS_STATE_EMPTY;

    /* Execute and verify */
    UtAssert_BOOL_FALSE(CS_GetTableDefTblEntryByName(&EntryPtr, "name"));
    UtAssert_ADDRESS_EQ(EntryPtr, NULL);

    /* Matching name, enabled */
    CS_AppData.DefTablesTblPtr[0].State = CS_STATE_ENABLED;

    /* Execute and verify */
    UtAssert_BOOL_TRUE(CS_GetTableDefTblEntryByName(&EntryPtr, "name"));
    UtAssert_ADDRESS_EQ(EntryPtr, CS_AppData.DefTablesTblPtr);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
}

void CS_GetAppResTblEntryByName_Test(void)
{
    CS_Res_App_Table_Entry_t *EntryPtr = NULL;

    /* Empty name, enabled state */
    CS_AppData.ResAppTblPtr[0].State = CS_STATE_ENABLED;

    /* Execute and verify */
    UtAssert_BOOL_FALSE(CS_GetAppResTblEntryByName(&EntryPtr, "name"));
    UtAssert_ADDRESS_EQ(EntryPtr, NULL);

    /* Matching name, empty state */
    strncpy(CS_AppData.ResAppTblPtr[0].Name, "name", 10);
    CS_AppData.ResAppTblPtr[0].State = CS_STATE_EMPTY;

    /* Execute and verify */
    UtAssert_BOOL_FALSE(CS_GetAppResTblEntryByName(&EntryPtr, "name"));
    UtAssert_ADDRESS_EQ(EntryPtr, NULL);

    /* Matching name, enabled */
    CS_AppData.ResAppTblPtr[0].State = CS_STATE_ENABLED;

    /* Execute and verify */
    UtAssert_BOOL_TRUE(CS_GetAppResTblEntryByName(&EntryPtr, "name"));
    UtAssert_ADDRESS_EQ(EntryPtr, CS_AppData.ResAppTblPtr);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
}

void CS_GetAppDefTblEntryByName_Test(void)
{
    CS_Def_App_Table_Entry_t *EntryPtr = NULL;

    /* Empty name, enabled state */
    CS_AppData.DefAppTblPtr[0].State = CS_STATE_ENABLED;

    /* Execute and verify */
    UtAssert_BOOL_FALSE(CS_GetAppDefTblEntryByName(&EntryPtr, "name"));
    UtAssert_ADDRESS_EQ(EntryPtr, NULL);

    /* Matching name, empty state */
    strncpy(CS_AppData.DefAppTblPtr[0].Name, "name", 10);
    CS_AppData.DefAppTblPtr[0].State = CS_STATE_EMPTY;

    /* Execute and verify */
    UtAssert_BOOL_FALSE(CS_GetAppDefTblEntryByName(&EntryPtr, "name"));
    UtAssert_ADDRESS_EQ(EntryPtr, NULL);

    /* Matching name, enabled */
    CS_AppData.DefAppTblPtr[0].State = CS_STATE_ENABLED;

    /* Execute and verify */
    UtAssert_BOOL_TRUE(CS_GetAppDefTblEntryByName(&EntryPtr, "name"));
    UtAssert_ADDRESS_EQ(EntryPtr, CS_AppData.DefAppTblPtr);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
}

void CS_FindEnabledEepromEntry_Test(void)
{
    uint16 EnabledEntry = 0;

    /* Call with zeros */
    UtAssert_BOOL_FALSE(CS_FindEnabledEepromEntry(&EnabledEntry));
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.CurrentEntryInTable, CS_MAX_NUM_EEPROM_TABLE_ENTRIES);
    UtAssert_UINT16_EQ(EnabledEntry, CS_MAX_NUM_EEPROM_TABLE_ENTRIES);

    /* Set up to find last entry (skip first) */
    CS_AppData.ResEepromTblPtr[0].State                                   = CS_STATE_ENABLED;
    CS_AppData.ResEepromTblPtr[CS_MAX_NUM_EEPROM_TABLE_ENTRIES - 1].State = CS_STATE_ENABLED;
    CS_AppData.HkPacket.CurrentEntryInTable                               = 1;

    UtAssert_BOOL_TRUE(CS_FindEnabledEepromEntry(&EnabledEntry));
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.CurrentEntryInTable, CS_MAX_NUM_EEPROM_TABLE_ENTRIES - 1);
    UtAssert_UINT16_EQ(EnabledEntry, CS_MAX_NUM_EEPROM_TABLE_ENTRIES - 1);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
}

void CS_FindEnabledMemoryEntry_Test(void)
{
    uint16 EnabledEntry = 0;

    /* Call with zeros */
    UtAssert_BOOL_FALSE(CS_FindEnabledMemoryEntry(&EnabledEntry));
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.CurrentEntryInTable, CS_MAX_NUM_MEMORY_TABLE_ENTRIES);
    UtAssert_UINT16_EQ(EnabledEntry, CS_MAX_NUM_MEMORY_TABLE_ENTRIES);

    /* Set up to find last entry (skip first) */
    CS_AppData.ResMemoryTblPtr[0].State                                   = CS_STATE_ENABLED;
    CS_AppData.ResMemoryTblPtr[CS_MAX_NUM_MEMORY_TABLE_ENTRIES - 1].State = CS_STATE_ENABLED;
    CS_AppData.HkPacket.CurrentEntryInTable                               = 1;

    UtAssert_BOOL_TRUE(CS_FindEnabledMemoryEntry(&EnabledEntry));
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.CurrentEntryInTable, CS_MAX_NUM_MEMORY_TABLE_ENTRIES - 1);
    UtAssert_UINT16_EQ(EnabledEntry, CS_MAX_NUM_MEMORY_TABLE_ENTRIES - 1);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
}

void CS_FindEnabledTablesEntry_Test(void)
{
    uint16 EnabledEntry = 0;

    /* Call with zeros */
    UtAssert_BOOL_FALSE(CS_FindEnabledTablesEntry(&EnabledEntry));
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.CurrentEntryInTable, CS_MAX_NUM_TABLES_TABLE_ENTRIES);
    UtAssert_UINT16_EQ(EnabledEntry, CS_MAX_NUM_TABLES_TABLE_ENTRIES);

    /* Set up to find last entry (skip first) */
    CS_AppData.ResTablesTblPtr[0].State                                   = CS_STATE_ENABLED;
    CS_AppData.ResTablesTblPtr[CS_MAX_NUM_TABLES_TABLE_ENTRIES - 1].State = CS_STATE_ENABLED;
    CS_AppData.HkPacket.CurrentEntryInTable                               = 1;

    UtAssert_BOOL_TRUE(CS_FindEnabledTablesEntry(&EnabledEntry));
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.CurrentEntryInTable, CS_MAX_NUM_TABLES_TABLE_ENTRIES - 1);
    UtAssert_UINT16_EQ(EnabledEntry, CS_MAX_NUM_TABLES_TABLE_ENTRIES - 1);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
}

void CS_FindEnabledAppEntry_Test(void)
{
    uint16 EnabledEntry = 0;

    /* Call with zeros */
    UtAssert_BOOL_FALSE(CS_FindEnabledAppEntry(&EnabledEntry));
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.CurrentEntryInTable, CS_MAX_NUM_APP_TABLE_ENTRIES);
    UtAssert_UINT16_EQ(EnabledEntry, CS_MAX_NUM_APP_TABLE_ENTRIES);

    /* Set up to find last entry (skip first) */
    CS_AppData.ResAppTblPtr[0].State                                = CS_STATE_ENABLED;
    CS_AppData.ResAppTblPtr[CS_MAX_NUM_APP_TABLE_ENTRIES - 1].State = CS_STATE_ENABLED;
    CS_AppData.HkPacket.CurrentEntryInTable                         = 1;

    UtAssert_BOOL_TRUE(CS_FindEnabledAppEntry(&EnabledEntry));
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.CurrentEntryInTable, CS_MAX_NUM_APP_TABLE_ENTRIES - 1);
    UtAssert_UINT16_EQ(EnabledEntry, CS_MAX_NUM_APP_TABLE_ENTRIES - 1);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
}

void CS_VerifyCmdLength_Test(void)
{
    CS_OneShotCmd_t   CmdPacket;
    CFE_SB_MsgId_t    TestMsgId = CFE_SB_MSGID_WRAP_VALUE(CS_CMD_MID);
    CFE_MSG_FcnCode_t FcnCode   = 99;
    size_t            MsgSize   = sizeof(CmdPacket);

    /* Execute with passing size */
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &MsgSize, sizeof(MsgSize), false);

    UtAssert_BOOL_TRUE(CS_VerifyCmdLength((CFE_MSG_Message_t *)(&CmdPacket), MsgSize));

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);

    /* Execute with failing size */
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &MsgSize, sizeof(MsgSize), false);

    UtAssert_BOOL_FALSE(CS_VerifyCmdLength((CFE_MSG_Message_t *)(&CmdPacket), MsgSize - 1));

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_LEN_ERR_EID);
}

void CS_BackgroundCfeCore_Test(void)
{
    /* Entirely disabled */
    CS_AppData.HkPacket.CfeCoreCSState = CS_STATE_DISABLED;
    UtAssert_BOOL_FALSE(CS_BackgroundCfeCore());
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.CurrentEntryInTable, 0);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.CurrentCSTable, 1);

    /* Segment disabled */
    CS_AppData.HkPacket.CfeCoreCSState = CS_STATE_ENABLED;
    CS_AppData.CfeCoreCodeSeg.State    = CS_STATE_DISABLED;
    UtAssert_BOOL_FALSE(CS_BackgroundCfeCore());
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.CurrentEntryInTable, 0);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.CurrentCSTable, 2);

    /* Enabled, miscompare, not done with entry */
    CS_AppData.CfeCoreCodeSeg.State = CS_STATE_ENABLED;
    UT_SetDeferredRetcode(UT_KEY(CS_ComputeEepromMemory), 1, CS_ERROR);
    UtAssert_BOOL_TRUE(CS_BackgroundCfeCore());
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.CfeCoreCSErrCounter, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_CFECORE_MISCOMPARE_ERR_EID);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.CurrentEntryInTable, 0);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.CurrentCSTable, 2);

    /* Enabled, compares, done with entry */
    UT_SetHandlerFunction(UT_KEY(CS_ComputeEepromMemory), CS_UTILS_TEST_CS_ComputeHandler, NULL);
    UtAssert_BOOL_TRUE(CS_BackgroundCfeCore());
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.CfeCoreCSErrCounter, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.CurrentEntryInTable, 0);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.CurrentCSTable, 3);
}

void CS_BackgroundOS_Test(void)
{
    /* Entirely disabled */
    CS_AppData.HkPacket.OSCSState = CS_STATE_DISABLED;
    UtAssert_BOOL_FALSE(CS_BackgroundOS());
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.CurrentEntryInTable, 0);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.CurrentCSTable, 1);

    /* Segment disabled */
    CS_AppData.HkPacket.OSCSState = CS_STATE_ENABLED;
    CS_AppData.OSCodeSeg.State    = CS_STATE_DISABLED;
    UtAssert_BOOL_FALSE(CS_BackgroundOS());
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.CurrentEntryInTable, 0);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.CurrentCSTable, 2);

    /* Enabled, miscompare, not done with entry */
    CS_AppData.OSCodeSeg.State = CS_STATE_ENABLED;
    UT_SetDeferredRetcode(UT_KEY(CS_ComputeEepromMemory), 1, CS_ERROR);
    UtAssert_BOOL_TRUE(CS_BackgroundOS());
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.OSCSErrCounter, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_OS_MISCOMPARE_ERR_EID);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.CurrentEntryInTable, 0);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.CurrentCSTable, 2);

    /* Enabled, compares, done with entry */
    UT_SetHandlerFunction(UT_KEY(CS_ComputeEepromMemory), CS_UTILS_TEST_CS_ComputeHandler, NULL);
    UtAssert_BOOL_TRUE(CS_BackgroundOS());
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.OSCSErrCounter, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.CurrentEntryInTable, 0);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.CurrentCSTable, 3);
}

void CS_BackgroundEeprom_Test(void)
{
    /* Entirely disabled */
    CS_AppData.HkPacket.EepromCSState = CS_STATE_DISABLED;
    UtAssert_BOOL_FALSE(CS_BackgroundEeprom());
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.CurrentEntryInTable, 0);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.CurrentCSTable, 1);

    /* All entries disabled */
    CS_AppData.HkPacket.EepromCSState             = CS_STATE_ENABLED;
    CS_AppData.ResEepromTblPtr[0].ComparisonValue = 1;
    CS_AppData.ResEepromTblPtr[1].ComparisonValue = 2;
    UtAssert_BOOL_FALSE(CS_BackgroundEeprom());
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.CurrentEntryInTable, 0);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.CurrentCSTable, 2);
    UtAssert_UINT32_EQ(CS_AppData.HkPacket.EepromBaseline, 3);

    /* Enabled, miscompare, not done with entry */
    CS_AppData.ResEepromTblPtr[0].State = CS_STATE_ENABLED;
    UT_SetDeferredRetcode(UT_KEY(CS_ComputeEepromMemory), 1, CS_ERROR);
    UtAssert_BOOL_TRUE(CS_BackgroundEeprom());
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.EepromCSErrCounter, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_EEPROM_MISCOMPARE_ERR_EID);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.CurrentEntryInTable, 0);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.CurrentCSTable, 2);

    /* Last entry, Enabled, compares, done with entry */
    CS_AppData.HkPacket.CurrentEntryInTable                                   = CS_MAX_NUM_EEPROM_TABLE_ENTRIES - 1;
    CS_AppData.ResEepromTblPtr[CS_AppData.HkPacket.CurrentEntryInTable].State = CS_STATE_ENABLED;
    CS_AppData.ResEepromTblPtr[CS_AppData.HkPacket.CurrentEntryInTable].ComparisonValue = 3;
    UT_SetHandlerFunction(UT_KEY(CS_ComputeEepromMemory), CS_UTILS_TEST_CS_ComputeHandler, NULL);
    UtAssert_BOOL_TRUE(CS_BackgroundEeprom());
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.EepromCSErrCounter, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.CurrentEntryInTable, 0);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.CurrentCSTable, 3);
    UtAssert_UINT32_EQ(CS_AppData.HkPacket.EepromBaseline, 6);
}

void CS_BackgroundMemory_Test(void)
{
    /* Entirely disabled */
    CS_AppData.HkPacket.MemoryCSState = CS_STATE_DISABLED;
    UtAssert_BOOL_FALSE(CS_BackgroundMemory());
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.CurrentEntryInTable, 0);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.CurrentCSTable, 1);

    /* All entries disabled */
    CS_AppData.HkPacket.MemoryCSState = CS_STATE_ENABLED;
    UtAssert_BOOL_FALSE(CS_BackgroundMemory());
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.CurrentEntryInTable, 0);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.CurrentCSTable, 2);

    /* Enabled, miscompare, not done with entry */
    CS_AppData.ResMemoryTblPtr[0].State = CS_STATE_ENABLED;
    UT_SetDeferredRetcode(UT_KEY(CS_ComputeEepromMemory), 1, CS_ERROR);
    UtAssert_BOOL_TRUE(CS_BackgroundMemory());
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.MemoryCSErrCounter, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_MEMORY_MISCOMPARE_ERR_EID);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.CurrentEntryInTable, 0);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.CurrentCSTable, 2);

    /* Last entry, Enabled, compares, done with entry */
    CS_AppData.HkPacket.CurrentEntryInTable                                   = CS_MAX_NUM_MEMORY_TABLE_ENTRIES - 1;
    CS_AppData.ResMemoryTblPtr[CS_AppData.HkPacket.CurrentEntryInTable].State = CS_STATE_ENABLED;
    UT_SetHandlerFunction(UT_KEY(CS_ComputeEepromMemory), CS_UTILS_TEST_CS_ComputeHandler, NULL);
    UtAssert_BOOL_TRUE(CS_BackgroundMemory());
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.MemoryCSErrCounter, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.CurrentEntryInTable, 0);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.CurrentCSTable, 3);
}

void CS_BackgroundTables_Test(void)
{
    /* Entirely disabled */
    CS_AppData.HkPacket.TablesCSState = CS_STATE_DISABLED;
    UtAssert_BOOL_FALSE(CS_BackgroundTables());
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.CurrentEntryInTable, 0);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.CurrentCSTable, 1);

    /* All entries disabled */
    CS_AppData.HkPacket.TablesCSState = CS_STATE_ENABLED;
    UtAssert_BOOL_FALSE(CS_BackgroundTables());
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.CurrentEntryInTable, 0);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.CurrentCSTable, 2);

    /* Enabled, miscompare, not done with entry */
    CS_AppData.ResTablesTblPtr[0].State = CS_STATE_ENABLED;
    UT_SetDeferredRetcode(UT_KEY(CS_ComputeTables), 1, CS_ERROR);
    UtAssert_BOOL_TRUE(CS_BackgroundTables());
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.TablesCSErrCounter, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_TABLES_MISCOMPARE_ERR_EID);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.CurrentEntryInTable, 0);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.CurrentCSTable, 2);

    /* Enabled, not found, not done with entry */
    UT_SetDeferredRetcode(UT_KEY(CS_ComputeTables), 1, CS_ERR_NOT_FOUND);
    UtAssert_BOOL_TRUE(CS_BackgroundTables());
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.TablesCSErrCounter, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 2);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, CS_COMPUTE_TABLES_NOT_FOUND_ERR_EID);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.CurrentEntryInTable, 1);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.CurrentCSTable, 2);

    /* Last entry, Enabled, compares, done with entry */
    CS_AppData.HkPacket.CurrentEntryInTable                                   = CS_MAX_NUM_TABLES_TABLE_ENTRIES - 1;
    CS_AppData.ResTablesTblPtr[CS_AppData.HkPacket.CurrentEntryInTable].State = CS_STATE_ENABLED;
    UT_SetHandlerFunction(UT_KEY(CS_ComputeTables), CS_UTILS_TEST_CS_ComputeHandler, NULL);
    UtAssert_BOOL_TRUE(CS_BackgroundTables());
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.TablesCSErrCounter, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 2);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.CurrentEntryInTable, 0);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.CurrentCSTable, 3);
}

void CS_BackgroundApp_Test(void)
{
    /* Entirely disabled */
    CS_AppData.HkPacket.AppCSState = CS_STATE_DISABLED;
    UtAssert_BOOL_FALSE(CS_BackgroundApp());
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.CurrentEntryInTable, 0);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.CurrentCSTable, 1);

    /* All entries disabled */
    CS_AppData.HkPacket.AppCSState = CS_STATE_ENABLED;
    UtAssert_BOOL_FALSE(CS_BackgroundApp());
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.CurrentEntryInTable, 0);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.CurrentCSTable, 2);

    /* Enabled, miscompare, not done with entry */
    CS_AppData.ResAppTblPtr[0].State = CS_STATE_ENABLED;
    UT_SetDeferredRetcode(UT_KEY(CS_ComputeApp), 1, CS_ERROR);
    UtAssert_BOOL_TRUE(CS_BackgroundApp());
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.AppCSErrCounter, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_APP_MISCOMPARE_ERR_EID);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.CurrentEntryInTable, 0);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.CurrentCSTable, 2);

    /* Enabled, not found, not done with entry */
    UT_SetDeferredRetcode(UT_KEY(CS_ComputeApp), 1, CS_ERR_NOT_FOUND);
    UtAssert_BOOL_TRUE(CS_BackgroundApp());
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.AppCSErrCounter, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 2);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, CS_COMPUTE_APP_NOT_FOUND_ERR_EID);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.CurrentEntryInTable, 1);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.CurrentCSTable, 2);

    /* Last entry, Enabled, compares, done with entry */
    CS_AppData.HkPacket.CurrentEntryInTable                                = CS_MAX_NUM_APP_TABLE_ENTRIES - 1;
    CS_AppData.ResAppTblPtr[CS_AppData.HkPacket.CurrentEntryInTable].State = CS_STATE_ENABLED;
    UT_SetHandlerFunction(UT_KEY(CS_ComputeApp), CS_UTILS_TEST_CS_ComputeHandler, NULL);
    UtAssert_BOOL_TRUE(CS_BackgroundApp());
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.AppCSErrCounter, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 2);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.CurrentEntryInTable, 0);
    UtAssert_UINT16_EQ(CS_AppData.HkPacket.CurrentCSTable, 3);
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
    uint16 ChildTaskTable[] = {CS_CFECORE, CS_EEPROM_TABLE, CS_MEMORY_TABLE, CS_APP_TABLE, CS_TABLES_TABLE};
    uint16 TblMax           = sizeof(ChildTaskTable) / sizeof(ChildTaskTable[0]);
    uint16 i;

    /* Cycle through each all true case */
    CS_AppData.HkPacket.RecomputeInProgress = true;
    CS_AppData.HkPacket.OneShotInProgress   = false;
    for (i = 0; i < TblMax; i++)
    {
        CS_AppData.ChildTaskTable = ChildTaskTable[i];

        UT_ResetState(UT_KEY(CS_HandleTableUpdate));
        UtAssert_INT32_EQ(CS_HandleRoutineTableUpdates(), CFE_SUCCESS);

        /* Confirm stub call count based on selected table */
        if (CS_AppData.ChildTaskTable == CS_CFECORE)
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
    CS_AppData.HkPacket.OneShotInProgress = true;
    for (i = 0; i < TblMax; i++)
    {
        CS_AppData.ChildTaskTable = ChildTaskTable[i];

        UT_ResetState(UT_KEY(CS_HandleTableUpdate));
        UtAssert_INT32_EQ(CS_HandleRoutineTableUpdates(), CFE_SUCCESS);
        UtAssert_STUB_COUNT(CS_HandleTableUpdate, 4);
        UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    }

    /* Recompute false will update all */
    CS_AppData.HkPacket.RecomputeInProgress = false;
    CS_AppData.HkPacket.OneShotInProgress   = false;
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

    /* Force send event errors */
    UT_ResetState(UT_KEY(CS_HandleTableUpdate));
    UT_ResetState(UT_KEY(CFE_EVS_SendEvent));
    UT_SetDefaultReturnValue(UT_KEY(CS_HandleTableUpdate), -1);
    UT_SetDefaultReturnValue(UT_KEY(CFE_EVS_SendEvent), -2);
    UtAssert_INT32_EQ(CS_HandleRoutineTableUpdates(), -2);
    UtAssert_STUB_COUNT(CS_HandleTableUpdate, 4);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 4);
}

void CS_AttemptTableReshare_Test(void)
{
    CS_Res_Tables_Table_Entry_t TblEntry;
    CFE_TBL_Handle_t            LocalTblHandle = CFE_TBL_BAD_TABLE_HANDLE;
    CFE_TBL_Info_t              TblInfo;
    cpuaddr                     LocalAddress;
    int32                       ResultGetInfo = -1;

    memset(&TblEntry, 0, sizeof(TblEntry));

    /* Execute the function being tested */
    UtAssert_INT32_EQ(CS_AttemptTableReshare(&TblEntry, &LocalTblHandle, &TblInfo, &LocalAddress, &ResultGetInfo),
                      CFE_SUCCESS);
    UtAssert_UINT32_EQ(TblEntry.TblHandle, LocalTblHandle);
    UtAssert_INT32_EQ(ResultGetInfo, CFE_SUCCESS);
    UtAssert_STUB_COUNT(CFE_TBL_ReleaseAddress, 0);

    /* Table never loaded to cause address release */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_TBL_ERR_NEVER_LOADED);
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetInfo), -1);
    UtAssert_INT32_EQ(CS_AttemptTableReshare(&TblEntry, &LocalTblHandle, &TblInfo, &LocalAddress, &ResultGetInfo),
                      CFE_TBL_ERR_NEVER_LOADED);
    UtAssert_UINT32_EQ(TblEntry.TblHandle, LocalTblHandle);
    UtAssert_INT32_EQ(ResultGetInfo, -1);
    UtAssert_STUB_COUNT(CFE_TBL_ReleaseAddress, 1);

    /* Failed share */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Share), -2);
    UtAssert_INT32_EQ(CS_AttemptTableReshare(&TblEntry, &LocalTblHandle, &TblInfo, &LocalAddress, &ResultGetInfo), -2);
}

void CS_CheckRecomputeOneShot_Test(void)
{
    /* Set up for false return */
    CS_AppData.HkPacket.RecomputeInProgress = false;
    CS_AppData.HkPacket.OneShotInProgress   = false;

    UtAssert_BOOL_FALSE(CS_CheckRecomputeOneshot());
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_UINT8_EQ(CS_AppData.HkPacket.CmdErrCounter, 0);

    /* One shot in progress */
    CS_AppData.HkPacket.OneShotInProgress = true;
    UtAssert_BOOL_TRUE(CS_CheckRecomputeOneshot());
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_CMD_COMPUTE_PROG_ERR_EID);
    UtAssert_UINT8_EQ(CS_AppData.HkPacket.CmdErrCounter, 1);

    /* Recompute in progress */
    CS_AppData.HkPacket.RecomputeInProgress = true;
    CS_AppData.HkPacket.OneShotInProgress   = false;
    UtAssert_BOOL_TRUE(CS_CheckRecomputeOneshot());
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 2);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, CS_CMD_COMPUTE_PROG_ERR_EID);
    UtAssert_UINT8_EQ(CS_AppData.HkPacket.CmdErrCounter, 2);
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
    UtTest_Add(CS_VerifyCmdLength_Test, CS_Test_Setup, CS_Test_TearDown, "CS_VerifyCmdLength_Test");
    UtTest_Add(CS_BackgroundCfeCore_Test, CS_Test_Setup, CS_Test_TearDown, "CS_BackgroundCfeCore_Test");
    UtTest_Add(CS_BackgroundOS_Test, CS_Test_Setup, CS_Test_TearDown, "CS_BackgroundOS_Test");
    UtTest_Add(CS_BackgroundEeprom_Test, CS_Test_Setup, CS_Test_TearDown, "CS_BackgroundEeprom_Test");
    UtTest_Add(CS_BackgroundMemory_Test, CS_Test_Setup, CS_Test_TearDown, "CS_BackgroundMemory_Test");
    UtTest_Add(CS_BackgroundTables_Test, CS_Test_Setup, CS_Test_TearDown, "CS_BackgroundTables_Test");
    UtTest_Add(CS_BackgroundApp_Test, CS_Test_Setup, CS_Test_TearDown, "CS_BackgroundApp_Test");
    UtTest_Add(CS_ResetTablesTblResultEntry_Test, CS_Test_Setup, CS_Test_TearDown, "CS_ResetTablesTblResultEntry_Test");
    UtTest_Add(CS_HandleRoutineTableUpdates_Test, CS_Test_Setup, CS_Test_TearDown, "CS_HandleRoutineTableUpdates_Test");
    UtTest_Add(CS_AttemptTableReshare_Test, CS_Test_Setup, CS_Test_TearDown, "CS_AttemptTableReshare_Test");
    UtTest_Add(CS_CheckRecomputeOneShot_Test, CS_Test_Setup, CS_Test_TearDown, "CS_CheckRecomputeOneShot_Test");
}
