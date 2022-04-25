
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
#include <sys/fcntl.h>
#include <unistd.h>
#include <stdlib.h>

/* UT includes */
#include "uttest.h"
#include "utassert.h"
#include "utstubs.h"

/* cs_utils_tests globals */
uint8 call_count_CFE_EVS_SendEvent;

/*
 * Function Definitions
 */

void CS_UTILS_TEST_CS_ComputeEepromMemoryHandler1(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    bool *DoneWithEntry = (bool *)UT_Hook_GetArgValueByName(Context, "DoneWithEntry", bool *);

    *DoneWithEntry = true;
}

void CS_UTILS_TEST_CS_ComputeAppHandler1(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    bool *DoneWithEntry = (bool *)UT_Hook_GetArgValueByName(Context, "DoneWithEntry", bool *);

    *DoneWithEntry = true;
}

void CS_UTILS_TEST_CS_ComputeTablesHandler1(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    bool *DoneWithEntry = (bool *)UT_Hook_GetArgValueByName(Context, "DoneWithEntry", bool *);

    *DoneWithEntry = true;
}

void CS_ZeroEepromTempValues_Test(void)
{
    CS_AppData.ResEepromTblPtr = (CS_Res_EepromMemory_Table_Entry_t *)CS_AppData.DefaultEepromDefTable;

    /* Execute the function being tested */
    CS_ZeroEepromTempValues();

    /* Verify results */
    UtAssert_True(CS_AppData.ResEepromTblPtr[0].TempChecksumValue == 0,
                  "CS_AppData.ResEepromTblPtr[0].TempChecksumValue == 0");
    UtAssert_True(CS_AppData.ResEepromTblPtr[0].ByteOffset == 0, "CS_AppData.ResEepromTblPtr[0].ByteOffset == 0");
    UtAssert_True(CS_AppData.ResEepromTblPtr[CS_MAX_NUM_EEPROM_TABLE_ENTRIES / 2].TempChecksumValue == 0,
                  "CS_AppData.ResEepromTblPtr[CS_MAX_NUM_EEPROM_TABLE_ENTRIES/2].TempChecksumValue == 0");
    UtAssert_True(CS_AppData.ResEepromTblPtr[CS_MAX_NUM_EEPROM_TABLE_ENTRIES / 2].ByteOffset == 0,
                  "CS_AppData.ResEepromTblPtr[CS_MAX_NUM_EEPROM_TABLE_ENTRIES/2].ByteOffset == 0");
    UtAssert_True(CS_AppData.ResEepromTblPtr[CS_MAX_NUM_EEPROM_TABLE_ENTRIES - 1].TempChecksumValue == 0,
                  "CS_AppData.ResEepromTblPtr[CS_MAX_NUM_EEPROM_TABLE_ENTRIES-1].TempChecksumValue == 0");
    UtAssert_True(CS_AppData.ResEepromTblPtr[CS_MAX_NUM_EEPROM_TABLE_ENTRIES - 1].ByteOffset == 0,
                  "CS_AppData.ResEepromTblPtr[CS_MAX_NUM_EEPROM_TABLE_ENTRIES-1].ByteOffset == 0");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_ZeroEepromTempValues_Test */

void CS_ZeroMemoryTempValues_Test(void)
{
    CS_AppData.ResMemoryTblPtr = (CS_Res_EepromMemory_Table_Entry_t *)CS_AppData.DefaultMemoryDefTable;

    /* Execute the function being tested */
    CS_ZeroMemoryTempValues();

    /* Verify results */
    UtAssert_True(CS_AppData.ResMemoryTblPtr[0].TempChecksumValue == 0,
                  "CS_AppData.ResMemoryTblPtr[0].TempChecksumValue == 0");
    UtAssert_True(CS_AppData.ResMemoryTblPtr[0].ByteOffset == 0, "CS_AppData.ResMemoryTblPtr[0].ByteOffset == 0");
    UtAssert_True(CS_AppData.ResMemoryTblPtr[CS_MAX_NUM_MEMORY_TABLE_ENTRIES / 2].TempChecksumValue == 0,
                  "CS_AppData.ResMemoryTblPtr[CS_MAX_NUM_MEMORY_TABLE_ENTRIES/2].TempChecksumValue == 0");
    UtAssert_True(CS_AppData.ResMemoryTblPtr[CS_MAX_NUM_MEMORY_TABLE_ENTRIES / 2].ByteOffset == 0,
                  "CS_AppData.ResMemoryTblPtr[CS_MAX_NUM_MEMORY_TABLE_ENTRIES/2].ByteOffset == 0");
    UtAssert_True(CS_AppData.ResMemoryTblPtr[CS_MAX_NUM_MEMORY_TABLE_ENTRIES - 1].TempChecksumValue == 0,
                  "CS_AppData.ResMemoryTblPtr[CS_MAX_NUM_MEMORY_TABLE_ENTRIES-1].TempChecksumValue == 0");
    UtAssert_True(CS_AppData.ResMemoryTblPtr[CS_MAX_NUM_MEMORY_TABLE_ENTRIES - 1].ByteOffset == 0,
                  "CS_AppData.ResMemoryTblPtr[CS_MAX_NUM_MEMORY_TABLE_ENTRIES-1].ByteOffset == 0");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_ZeroMemoryTempValues_Test */

void CS_ZeroTablesTempValues_Test(void)
{
    CS_AppData.ResTablesTblPtr = (CS_Res_Tables_Table_Entry_t *)CS_AppData.DefaultTablesDefTable;

    /* Execute the function being tested */
    CS_ZeroTablesTempValues();

    /* Verify results */
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].TempChecksumValue == 0,
                  "CS_AppData.ResTablesTblPtr[0].TempChecksumValue == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].ByteOffset == 0, "CS_AppData.ResTablesTblPtr[0].ByteOffset == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[CS_MAX_NUM_TABLES_TABLE_ENTRIES / 2].TempChecksumValue == 0,
                  "CS_AppData.ResTablesTblPtr[CS_MAX_NUM_TABLES_TABLE_ENTRIES/2].TempChecksumValue == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[CS_MAX_NUM_TABLES_TABLE_ENTRIES / 2].ByteOffset == 0,
                  "CS_AppData.ResTablesTblPtr[CS_MAX_NUM_TABLES_TABLE_ENTRIES/2].ByteOffset == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[CS_MAX_NUM_TABLES_TABLE_ENTRIES - 1].TempChecksumValue == 0,
                  "CS_AppData.ResTablesTblPtr[CS_MAX_NUM_TABLES_TABLE_ENTRIES-1].TempChecksumValue == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[CS_MAX_NUM_TABLES_TABLE_ENTRIES - 1].ByteOffset == 0,
                  "CS_AppData.ResTablesTblPtr[CS_MAX_NUM_TABLES_TABLE_ENTRIES-1].ByteOffset == 0");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_ZeroTablesTempValues_Test */

void CS_ZeroAppTempValues_Test(void)
{
    CS_AppData.ResAppTblPtr = (CS_Res_App_Table_Entry_t *)CS_AppData.DefaultAppDefTable;

    /* Execute the function being tested */
    CS_ZeroAppTempValues();

    /* Verify results */
    UtAssert_True(CS_AppData.ResAppTblPtr[0].TempChecksumValue == 0,
                  "CS_AppData.ResAppTblPtr[0].TempChecksumValue == 0");
    UtAssert_True(CS_AppData.ResAppTblPtr[0].ByteOffset == 0, "CS_AppData.ResAppTblPtr[0].ByteOffset == 0");
    UtAssert_True(CS_AppData.ResAppTblPtr[CS_MAX_NUM_APP_TABLE_ENTRIES / 2].TempChecksumValue == 0,
                  "CS_AppData.ResAppTblPtr[CS_MAX_NUM_APP_TABLE_ENTRIES/2].TempChecksumValue == 0");
    UtAssert_True(CS_AppData.ResAppTblPtr[CS_MAX_NUM_APP_TABLE_ENTRIES / 2].ByteOffset == 0,
                  "CS_AppData.ResAppTblPtr[CS_MAX_NUM_APP_TABLE_ENTRIES/2].ByteOffset == 0");
    UtAssert_True(CS_AppData.ResAppTblPtr[CS_MAX_NUM_APP_TABLE_ENTRIES - 1].TempChecksumValue == 0,
                  "CS_AppData.ResAppTblPtr[CS_MAX_NUM_APP_TABLE_ENTRIES-1].TempChecksumValue == 0");
    UtAssert_True(CS_AppData.ResAppTblPtr[CS_MAX_NUM_APP_TABLE_ENTRIES - 1].ByteOffset == 0,
                  "CS_AppData.ResAppTblPtr[CS_MAX_NUM_APP_TABLE_ENTRIES-1].ByteOffset == 0");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_ZeroAppTempValues_Test */

void CS_ZeroCfeCoreTempValues_Test(void)
{

    /* Execute the function being tested */
    CS_ZeroCfeCoreTempValues();

    /* Verify results */
    UtAssert_True(CS_AppData.CfeCoreCodeSeg.TempChecksumValue == 0, "CS_AppData.CfeCoreCodeSeg.TempChecksumValue == 0");
    UtAssert_True(CS_AppData.CfeCoreCodeSeg.ByteOffset == 0, "CS_AppData.CfeCoreCodeSeg.ByteOffset == 0");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_ZeroCfeCoreTempValues_Test */

void CS_ZeroOSTempValues_Test(void)
{

    /* Execute the function being tested */
    CS_ZeroOSTempValues();

    /* Verify results */
    UtAssert_True(CS_AppData.OSCodeSeg.TempChecksumValue == 0, "CS_AppData.OSCodeSeg.TempChecksumValue == 0");
    UtAssert_True(CS_AppData.OSCodeSeg.ByteOffset == 0, "CS_AppData.OSCodeSeg.ByteOffset == 0");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_ZeroOSTempValues_Test */

void CS_InitializeDefaultTables_Test(void)
{

    /* Execute the function being tested */
    CS_InitializeDefaultTables();

    /* Verify results */
    UtAssert_True(CS_AppData.DefaultEepromDefTable[0].State == CS_STATE_EMPTY,
                  "CS_AppData.DefaultEepromDefTable[0].State == CS_STATE_EMPTY");
    UtAssert_True(CS_AppData.DefaultEepromDefTable[0].NumBytesToChecksum == 0,
                  "CS_AppData.DefaultEepromDefTable[0].NumBytesToChecksum == 0");
    UtAssert_True(CS_AppData.DefaultEepromDefTable[0].StartAddress == 0,
                  "CS_AppData.DefaultEepromDefTable[0].StartAddress == 0");

    UtAssert_True(CS_AppData.DefaultEepromDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES / 2].State == CS_STATE_EMPTY,
                  "CS_AppData.DefaultEepromDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES/2].State == CS_STATE_EMPTY");
    UtAssert_True(CS_AppData.DefaultEepromDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES / 2].NumBytesToChecksum == 0,
                  "CS_AppData.DefaultEepromDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES/2].NumBytesToChecksum == 0");
    UtAssert_True(CS_AppData.DefaultEepromDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES / 2].StartAddress == 0,
                  "CS_AppData.DefaultEepromDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES/2].StartAddress == 0");

    UtAssert_True(CS_AppData.DefaultEepromDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES - 1].State == CS_STATE_EMPTY,
                  "CS_AppData.DefaultEepromDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES-1].State == CS_STATE_EMPTY");
    UtAssert_True(CS_AppData.DefaultEepromDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES - 1].NumBytesToChecksum == 0,
                  "CS_AppData.DefaultEepromDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES-1].NumBytesToChecksum == 0");
    UtAssert_True(CS_AppData.DefaultEepromDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES - 1].StartAddress == 0,
                  "CS_AppData.DefaultEepromDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES-1].StartAddress == 0");

    UtAssert_True(CS_AppData.DefaultMemoryDefTable[0].State == CS_STATE_EMPTY,
                  "CS_AppData.DefaultMemoryDefTable[0].State == CS_STATE_EMPTY");
    UtAssert_True(CS_AppData.DefaultMemoryDefTable[0].NumBytesToChecksum == 0,
                  "CS_AppData.DefaultMemoryDefTable[0].NumBytesToChecksum == 0");
    UtAssert_True(CS_AppData.DefaultMemoryDefTable[0].StartAddress == 0,
                  "CS_AppData.DefaultMemoryDefTable[0].StartAddress == 0");

    UtAssert_True(CS_AppData.DefaultMemoryDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES / 2].State == CS_STATE_EMPTY,
                  "CS_AppData.DefaultMemoryDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES/2].State == CS_STATE_EMPTY");
    UtAssert_True(CS_AppData.DefaultMemoryDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES / 2].NumBytesToChecksum == 0,
                  "CS_AppData.DefaultMemoryDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES/2].NumBytesToChecksum == 0");
    UtAssert_True(CS_AppData.DefaultMemoryDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES / 2].StartAddress == 0,
                  "CS_AppData.DefaultMemoryDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES/2].StartAddress == 0");

    UtAssert_True(CS_AppData.DefaultMemoryDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES - 1].State == CS_STATE_EMPTY,
                  "CS_AppData.DefaultMemoryDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES-1].State == CS_STATE_EMPTY");
    UtAssert_True(CS_AppData.DefaultMemoryDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES - 1].NumBytesToChecksum == 0,
                  "CS_AppData.DefaultMemoryDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES-1].NumBytesToChecksum == 0");
    UtAssert_True(CS_AppData.DefaultMemoryDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES - 1].StartAddress == 0,
                  "CS_AppData.DefaultMemoryDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES-1].StartAddress == 0");

    UtAssert_True(CS_AppData.DefaultAppDefTable[0].State == CS_STATE_EMPTY,
                  "CS_AppData.DefaultAppDefTable[0].State == CS_STATE_EMPTY");
    UtAssert_True(CS_AppData.DefaultAppDefTable[0].Name[0] == 0, "CS_AppData.DefaultAppDefTable[0].Name[0] == 0");

    UtAssert_True(CS_AppData.DefaultAppDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES / 2].State == CS_STATE_EMPTY,
                  "CS_AppData.DefaultAppDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES/2].State == CS_STATE_EMPTY");
    UtAssert_True(CS_AppData.DefaultAppDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES / 2].Name[0] == 0,
                  "CS_AppData.DefaultAppDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES/2].Name[0] == 0");

    UtAssert_True(CS_AppData.DefaultAppDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES - 1].State == CS_STATE_EMPTY,
                  "CS_AppData.DefaultAppDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES-1].State == CS_STATE_EMPTY");
    UtAssert_True(CS_AppData.DefaultAppDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES - 1].Name[0] == 0,
                  "CS_AppData.DefaultAppDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES-1].Name[0] == 0");

    UtAssert_True(CS_AppData.DefaultTablesDefTable[0].State == CS_STATE_EMPTY,
                  "CS_AppData.DefaultTablesDefTable[0].State == CS_STATE_EMPTY");
    UtAssert_True(CS_AppData.DefaultTablesDefTable[0].Name[0] == 0, "CS_AppData.DefaultTablesDefTable[0].Name[0] == 0");

    UtAssert_True(CS_AppData.DefaultTablesDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES / 2].State == CS_STATE_EMPTY,
                  "CS_AppData.DefaultTablesDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES/2].State == CS_STATE_EMPTY");
    UtAssert_True(CS_AppData.DefaultTablesDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES / 2].Name[0] == 0,
                  "CS_AppData.DefaultTablesDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES/2].Name[0] == 0");

    UtAssert_True(CS_AppData.DefaultTablesDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES - 1].State == CS_STATE_EMPTY,
                  "CS_AppData.DefaultTablesDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES-1].State == CS_STATE_EMPTY");
    UtAssert_True(CS_AppData.DefaultTablesDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES - 1].Name[0] == 0,
                  "CS_AppData.DefaultTablesDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES-1].Name[0] == 0");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_InitializeDefaultTables_Test */

void CS_GoToNextTable_Test_Nominal(void)
{
    CS_AppData.HkPacket.CurrentCSTable = CS_NUM_TABLES - 2;

    /* Execute the function being tested */
    CS_GoToNextTable();

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.CurrentCSTable == CS_NUM_TABLES - 1,
                  "CS_AppData.HkPacket.CurrentCSTable == CS_NUM_TABLES - 1");
    UtAssert_True(CS_AppData.HkPacket.CurrentEntryInTable == 0, "CS_AppData.HkPacket.CurrentEntryInTable == 0");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_GoToNextTable_Test_Nominal */

void CS_GoToNextTable_Test_UpdatePassCounter(void)
{
    CS_AppData.HkPacket.CurrentCSTable = CS_NUM_TABLES - 1;

    /* Execute the function being tested */
    CS_GoToNextTable();

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.CurrentCSTable == 0, "CS_AppData.HkPacket.CurrentCSTable == 0");
    UtAssert_True(CS_AppData.HkPacket.PassCounter == 1, "CS_AppData.HkPacket.PassCounter == 1");
    UtAssert_True(CS_AppData.HkPacket.CurrentEntryInTable == 0, "CS_AppData.HkPacket.CurrentEntryInTable == 0");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_GoToNextTable_Test_UpdatePassCounter */

void CS_GetTableResTblEntryByName_Test(void)
{
    bool Result;

    CS_Res_Tables_Table_Entry_t *EntryPtr;

    CS_AppData.ResTablesTblPtr = (CS_Res_Tables_Table_Entry_t *)CS_AppData.DefaultTablesDefTable;

    strncpy(CS_AppData.ResTablesTblPtr[0].Name, "name", 10);

    CS_AppData.ResTablesTblPtr[0].State = 99;

    /* Execute the function being tested */
    Result = CS_GetTableResTblEntryByName(&EntryPtr, "name");

    /* Verify results */
    UtAssert_True(EntryPtr == CS_AppData.ResTablesTblPtr, "EntryPtr == CS_AppData.ResTablesTblPtr");
    UtAssert_True(Result == true, "Result == true");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_GetTableResTblEntryByName_Test */

void CS_GetTableResTblEntryByName_Test_EmptyName(void)
{
    bool Result;

    CS_Res_Tables_Table_Entry_t *EntryPtr;

    CS_AppData.ResTablesTblPtr = (CS_Res_Tables_Table_Entry_t *)CS_AppData.DefaultTablesDefTable;

    CS_AppData.ResTablesTblPtr[0].State = CS_STATE_EMPTY;

    /* Execute the function being tested */
    Result = CS_GetTableResTblEntryByName(&EntryPtr, "name");

    /* Verify results */
    UtAssert_True(Result == false, "Result == false");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_GetTableResTblEntryByName_Test_EmptyName */

void CS_GetTableResTblEntryByName_Test_EmptyState(void)
{
    bool Result;

    CS_Res_Tables_Table_Entry_t *EntryPtr;

    CS_AppData.ResTablesTblPtr = (CS_Res_Tables_Table_Entry_t *)CS_AppData.DefaultTablesDefTable;

    strncpy(CS_AppData.ResTablesTblPtr[0].Name, "name", 10);

    CS_AppData.ResTablesTblPtr[0].State = CS_STATE_EMPTY;

    /* Execute the function being tested */
    Result = CS_GetTableResTblEntryByName(&EntryPtr, "name");

    /* Verify results */
    UtAssert_True(Result == false, "Result == false");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_GetTableResTblEntryByName_Test_EmptyState */

void CS_GetTableDefTblEntryByName_Test(void)
{
    bool Result;

    CS_Def_Tables_Table_Entry_t *EntryPtr;

    strncpy(CS_AppData.DefTablesTblPtr[0].Name, "name", 10);

    CS_AppData.DefTablesTblPtr[0].State = 99;

    /* Execute the function being tested */
    Result = CS_GetTableDefTblEntryByName(&EntryPtr, "name");

    /* Verify results */
    UtAssert_True(EntryPtr == CS_AppData.DefTablesTblPtr, "EntryPtr == CS_AppData.DefTablesTblPtr");
    UtAssert_True(Result == true, "Result == true");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_GetTableDefTblEntryByName_Test */

void CS_GetTableDefTblEntryByName_Test_EmptyName(void)
{
    bool Result;

    CS_Def_Tables_Table_Entry_t *EntryPtr;

    CS_AppData.DefTablesTblPtr[0].State = CS_STATE_EMPTY;

    /* Execute the function being tested */
    Result = CS_GetTableDefTblEntryByName(&EntryPtr, "name");

    /* Verify results */
    UtAssert_True(Result == false, "Result == false");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_GetTableDefTblEntryByName_Test_EmptyName */

void CS_GetTableDefTblEntryByName_Test_EmptyState(void)
{
    bool Result;

    CS_Def_Tables_Table_Entry_t *EntryPtr;

    strncpy(CS_AppData.DefTablesTblPtr[0].Name, "name", 10);

    CS_AppData.DefTablesTblPtr[0].State = CS_STATE_EMPTY;

    /* Execute the function being tested */
    Result = CS_GetTableDefTblEntryByName(&EntryPtr, "name");

    /* Verify results */
    UtAssert_True(Result == false, "Result == false");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_GetTableDefTblEntryByName_Test_EmptyState */

void CS_GetAppResTblEntryByName_Test(void)
{
    bool Result;

    CS_Res_App_Table_Entry_t *EntryPtr;

    strncpy(CS_AppData.ResAppTblPtr[0].Name, "name", 10);

    CS_AppData.ResAppTblPtr[0].State = 99;

    /* Execute the function being tested */
    Result = CS_GetAppResTblEntryByName(&EntryPtr, "name");

    /* Verify results */
    UtAssert_True(EntryPtr == CS_AppData.ResAppTblPtr, "EntryPtr == CS_AppData.ResAppTblPtr");
    UtAssert_True(Result == true, "Result == true");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_GetAppResTblEntryByName_Test */

void CS_GetAppResTblEntryByName_Test_EmptyName(void)
{
    bool Result;

    CS_Res_App_Table_Entry_t *EntryPtr;

    CS_AppData.ResAppTblPtr[0].State = CS_STATE_EMPTY;

    /* Execute the function being tested */
    Result = CS_GetAppResTblEntryByName(&EntryPtr, "name");

    /* Verify results */
    UtAssert_True(Result == false, "Result == false");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_GetAppResTblEntryByName_Test_EmptyName */

void CS_GetAppResTblEntryByName_Test_EmptyState(void)
{
    bool Result;

    CS_Res_App_Table_Entry_t *EntryPtr;

    strncpy(CS_AppData.ResAppTblPtr[0].Name, "name", 10);

    CS_AppData.ResAppTblPtr[0].State = CS_STATE_EMPTY;

    /* Execute the function being tested */
    Result = CS_GetAppResTblEntryByName(&EntryPtr, "name");

    /* Verify results */
    UtAssert_True(Result == false, "Result == false");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_GetAppResTblEntryByName_Test_EmptyState */

void CS_GetAppDefTblEntryByName_Test(void)
{
    bool Result;

    CS_Def_App_Table_Entry_t *EntryPtr;

    strncpy(CS_AppData.DefAppTblPtr[0].Name, "name", 10);

    CS_AppData.DefAppTblPtr[0].State = 99;

    /* Execute the function being tested */
    Result = CS_GetAppDefTblEntryByName(&EntryPtr, "name");

    /* Verify results */
    UtAssert_True(EntryPtr == CS_AppData.DefAppTblPtr, "EntryPtr == CS_AppData.DefAppTblPtr");
    UtAssert_True(Result == true, "Result == true");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_GetAppDefTblEntryByName_Test */

void CS_GetAppDefTblEntryByName_Test_EmptyName(void)
{
    bool Result;

    CS_Def_App_Table_Entry_t *EntryPtr;

    CS_AppData.DefAppTblPtr[0].State = CS_STATE_EMPTY;

    /* Execute the function being tested */
    Result = CS_GetAppDefTblEntryByName(&EntryPtr, "name");

    /* Verify results */
    UtAssert_True(Result == false, "Result == false");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_GetAppDefTblEntryByName_Test_EmptyName */

void CS_GetAppDefTblEntryByName_Test_EmptyState(void)
{
    bool Result;

    CS_Def_App_Table_Entry_t *EntryPtr;

    strncpy(CS_AppData.DefAppTblPtr[0].Name, "name", 10);

    CS_AppData.DefAppTblPtr[0].State = CS_STATE_EMPTY;

    /* Execute the function being tested */
    Result = CS_GetAppDefTblEntryByName(&EntryPtr, "name");

    /* Verify results */
    UtAssert_True(Result == false, "Result == false");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_GetAppDefTblEntryByName_Test_EmptyState */

void CS_FindEnabledEepromEntry_Test(void)
{
    bool   Result;
    uint16 EnabledEntry;

    CS_AppData.HkPacket.CurrentEntryInTable = 0;

    CS_AppData.ResEepromTblPtr = (CS_Res_EepromMemory_Table_Entry_t *)CS_AppData.DefaultEepromDefTable;
    CS_AppData.ResEepromTblPtr[CS_MAX_NUM_EEPROM_TABLE_ENTRIES].State = CS_STATE_ENABLED;

    /* Execute the function being tested */
    Result = CS_FindEnabledEepromEntry(&EnabledEntry);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.CurrentEntryInTable == 16, "CS_AppData.HkPacket.CurrentEntryInTable == 16");

    UtAssert_True(memcmp(&CS_AppData.ResEepromTblPtr[0], &CS_AppData.ResEepromTblPtr[15],
                         CS_MAX_NUM_EEPROM_TABLE_ENTRIES * sizeof(CS_Res_EepromMemory_Table_Entry_t)),
                  "memcmp(&CS_AppData.ResEepromTblPtr[0], &CS_AppData.ResEepromTblPtr[15], "
                  "CS_MAX_NUM_EEPROM_TABLE_ENTRIES*sizeof(CS_Res_EepromMemory_Table_Entry_t))");
    UtAssert_True(EnabledEntry == 16, "EnabledEntry == 16");

    UtAssert_True(Result == false, "Result == false");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_FindEnabledEepromEntry_Test */

void CS_FindEnabledMemoryEntry_Test(void)
{
    bool   Result;
    uint16 EnabledEntry;

    CS_AppData.HkPacket.CurrentEntryInTable = 0;

    CS_AppData.ResMemoryTblPtr = (CS_Res_EepromMemory_Table_Entry_t *)CS_AppData.DefaultMemoryDefTable;
    CS_AppData.ResMemoryTblPtr[CS_MAX_NUM_MEMORY_TABLE_ENTRIES].State = CS_STATE_ENABLED;

    /* Execute the function being tested */
    Result = CS_FindEnabledMemoryEntry(&EnabledEntry);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.CurrentEntryInTable == 16, "CS_AppData.HkPacket.CurrentEntryInTable == 16");

    UtAssert_True(memcmp(&CS_AppData.ResMemoryTblPtr[0], &CS_AppData.ResMemoryTblPtr[15],
                         CS_MAX_NUM_MEMORY_TABLE_ENTRIES * sizeof(CS_Res_EepromMemory_Table_Entry_t)),
                  "memcmp(&CS_AppData.ResMemoryTblPtr[0], &CS_AppData.ResMemoryTblPtr[15], "
                  "CS_MAX_NUM_MEMORY_TABLE_ENTRIES*sizeof(CS_Res_EepromMemory_Table_Entry_t))");
    UtAssert_True(EnabledEntry == 16, "EnabledEntry == 16");

    UtAssert_True(Result == false, "Result == false");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_FindEnabledMemoryEntry_Test */

void CS_FindEnabledTablesEntry_Test(void)
{
    bool   Result;
    uint16 EnabledEntry;

    CS_AppData.HkPacket.CurrentEntryInTable = 0;

    CS_AppData.ResTablesTblPtr = (CS_Res_Tables_Table_Entry_t *)CS_AppData.DefaultTablesDefTable;
    CS_AppData.ResTablesTblPtr[CS_MAX_NUM_TABLES_TABLE_ENTRIES].State = CS_STATE_ENABLED;

    /* Execute the function being tested */
    Result = CS_FindEnabledTablesEntry(&EnabledEntry);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.CurrentEntryInTable == 24, "CS_AppData.HkPacket.CurrentEntryInTable == 24");

    UtAssert_True(memcmp(&CS_AppData.ResTablesTblPtr[0], &CS_AppData.ResTablesTblPtr[23],
                         CS_MAX_NUM_TABLES_TABLE_ENTRIES * sizeof(CS_Res_Tables_Table_Entry_t)),
                  "memcmp(&CS_AppData.ResTablesTblPtr[0], &CS_AppData.ResTablesTblPtr[23], "
                  "CS_MAX_NUM_TABLES_TABLE_ENTRIES*sizeof(CS_Res_Tables_Table_Entry_t))");
    UtAssert_True(EnabledEntry == 24, "EnabledEntry == 24");

    UtAssert_True(Result == false, "Result == false");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_FindEnabledTablesEntry_Test */

void CS_FindEnabledAppEntry_Test(void)
{
    bool   Result;
    uint16 EnabledEntry;

    CS_AppData.HkPacket.CurrentEntryInTable = 0;

    CS_AppData.ResAppTblPtr = (CS_Res_App_Table_Entry_t *)CS_AppData.DefaultAppDefTable;
    CS_AppData.ResAppTblPtr[CS_MAX_NUM_APP_TABLE_ENTRIES].State = CS_STATE_ENABLED;

    /* Execute the function being tested */
    Result = CS_FindEnabledAppEntry(&EnabledEntry);

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.CurrentEntryInTable == 24, "CS_AppData.HkPacket.CurrentEntryInTable == 24");

    UtAssert_True(memcmp(&CS_AppData.ResAppTblPtr[0], &CS_AppData.ResAppTblPtr[23],
                         CS_MAX_NUM_APP_TABLE_ENTRIES * sizeof(CS_Res_App_Table_Entry_t)),
                  "memcmp(&CS_AppData.ResAppTblPtr[0], &CS_AppData.ResAppTblPtr[23], "
                  "CS_MAX_NUM_APP_TABLE_ENTRIES*sizeof(CS_Res_App_Table_Entry_t))");
    UtAssert_True(EnabledEntry == 24, "EnabledEntry == 24");

    UtAssert_True(Result == false, "Result == false");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_FindEnabledAppEntry_Test */

void CS_VerifyCmdLength_Test_Nominal(void)
{
    bool              Result;
    CS_OneShotCmd_t   CmdPacket;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;
    size_t            MsgSize;

    CFE_MSG_Init((CFE_MSG_Message_t *)&CmdPacket, CS_CMD_MID, sizeof(CS_OneShotCmd_t));
    TestMsgId = CS_CMD_MID;
    MsgSize   = sizeof(CS_OneShotCmd_t);
    FcnCode   = 99;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &MsgSize, sizeof(MsgSize), false);

    CS_AppData.HkPacket.CurrentEntryInTable = 0;

    CS_AppData.ResAppTblPtr = (CS_Res_App_Table_Entry_t *)CS_AppData.DefaultAppDefTable;
    CS_AppData.ResAppTblPtr[CS_MAX_NUM_APP_TABLE_ENTRIES].State = CS_STATE_ENABLED;

    /* Execute the function being tested */
    Result = CS_VerifyCmdLength((CFE_MSG_Message_t *)(&CmdPacket), sizeof(CS_OneShotCmd_t));

    /* Verify results */
    UtAssert_True(Result == true, "Result == true");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_VerifyCmdLength_Test_Nominal */

void CS_VerifyCmdLength_Test_InvalidMsgLength(void)
{
    bool              Result;
    CS_OneShotCmd_t   CmdPacket;
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;
    size_t            MsgSize;
    int32             strCmpResult;
    char              ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;
    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), UT_Utils_stub_reporter_hook, &context_CFE_EVS_SendEvent);

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Invalid msg length: ID = 0x%%08X, CC = %%d, Len = %%lu, Expected = %%lu");

    CFE_MSG_Init((CFE_MSG_Message_t *)&CmdPacket, CS_CMD_MID, sizeof(CS_OneShotCmd_t));
    CFE_MSG_SetFcnCode((CFE_MSG_Message_t *)&CmdPacket, 88);
    TestMsgId = CS_CMD_MID;
    MsgSize   = sizeof(CS_OneShotCmd_t);
    FcnCode   = 88;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &MsgSize, sizeof(MsgSize), false);

    CS_AppData.HkPacket.CurrentEntryInTable = 0;

    CS_AppData.ResAppTblPtr = (CS_Res_App_Table_Entry_t *)CS_AppData.DefaultAppDefTable;
    CS_AppData.ResAppTblPtr[CS_MAX_NUM_APP_TABLE_ENTRIES].State = CS_STATE_ENABLED;

    /* Execute the function being tested */
    Result = CS_VerifyCmdLength((CFE_MSG_Message_t *)(&CmdPacket), 99);

    /* Verify results */

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventID, CS_LEN_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent.Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent.Spec);

    UtAssert_True(CS_AppData.HkPacket.CmdErrCounter == 1, "CS_AppData.HkPacket.CmdErrCounter == 1");

    UtAssert_True(Result == false, "Result == false");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_VerifyCmdLength_Test_InvalidMsgLength */

void CS_BackgroundCfeCore_Test_Nominal(void)
{
    bool Result;

    CS_AppData.HkPacket.CfeCoreCSState        = CS_STATE_ENABLED;
    CS_AppData.CfeCoreCodeSeg.ComparisonValue = 99;
    CS_AppData.CfeCoreCodeSeg.State           = CS_STATE_ENABLED;

    /* Execute the function being tested */
    Result = CS_BackgroundCfeCore();

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.CurrentEntryInTable == 0,
                  "CS_AppData.HkPacket.CurrentEntryInTable == 0"); /* Reset to 0 by CS_GoToNextTable */
    UtAssert_True(CS_AppData.HkPacket.CfeCoreBaseline == 0,
                  "CS_AppData.HkPacket.CfeCoreBaseline == 0"); /* Reset to 0 by CS_ComputeEepromMemory */

    UtAssert_True(Result == true, "Result == true");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_BackgroundCfeCore_Test_Nominal */

void CS_BackgroundCfeCore_Test_Miscompare(void)
{
    bool  Result;
    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;
    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), UT_Utils_stub_reporter_hook, &context_CFE_EVS_SendEvent);

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Checksum Failure: cFE Core, Expected: 0x%%08X, Calculated: 0x%%08X");

    CS_AppData.HkPacket.CfeCoreCSState        = CS_STATE_ENABLED;
    CS_AppData.CfeCoreCodeSeg.ComparisonValue = 99;
    CS_AppData.CfeCoreCodeSeg.State           = CS_STATE_ENABLED;

    /* The following section is just to make CS_ComputeEepromMemory return error */
    CS_AppData.CfeCoreCodeSeg.ByteOffset         = 0;
    CS_AppData.CfeCoreCodeSeg.NumBytesToChecksum = 1;
    CS_AppData.MaxBytesPerCycle                  = 2;
    CS_AppData.CfeCoreCodeSeg.ComputedYet        = true;
    CS_AppData.CfeCoreCodeSeg.ComparisonValue    = 5;
    UT_SetDeferredRetcode(UT_KEY(CS_ComputeEepromMemory), 1, CS_ERROR);

    /* Set to satisfy condition "NewChecksumValue != ResultsEntry -> ComparisonValue" */

    /* End of section to make CS_ComputeEepromMemory return error */

    UT_SetHandlerFunction(UT_KEY(CS_ComputeEepromMemory), CS_UTILS_TEST_CS_ComputeEepromMemoryHandler1, NULL);

    /* Execute the function being tested */
    Result = CS_BackgroundCfeCore();

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.CfeCoreCSErrCounter == 1, "CS_AppData.HkPacket.CfeCoreCSErrCounter == 1");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventID, CS_CFECORE_MISCOMPARE_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent.Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent.Spec);

    UtAssert_True(CS_AppData.HkPacket.CurrentEntryInTable == 0,
                  "CS_AppData.HkPacket.CurrentEntryInTable == 0"); /* Reset to 0 by CS_GoToNextTable */
    UtAssert_True(CS_AppData.HkPacket.CfeCoreBaseline == 5, "CS_AppData.HkPacket.CfeCoreBaseline == 5");

    UtAssert_True(Result == true, "Result == true");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_BackgroundCfeCore_Test_Miscompare */

void CS_BackgroundCfeCore_Test_ResultsEntryDisabled(void)
{
    bool Result;

    CS_AppData.HkPacket.CfeCoreCSState = CS_STATE_ENABLED;
    CS_AppData.CfeCoreCodeSeg.State    = CS_STATE_DISABLED;

    /* Execute the function being tested */
    Result = CS_BackgroundCfeCore();

    /* Verify results */
    UtAssert_True(Result == false, "Result == false");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_BackgroundCfeCore_Test_ResultsEntryDisabled */

void CS_BackgroundCfeCore_Test_CfeCoreCSStateDisabled(void)
{
    bool Result;

    CS_AppData.HkPacket.CfeCoreCSState = CS_STATE_DISABLED;

    /* Execute the function being tested */
    Result = CS_BackgroundCfeCore();

    /* Verify results */
    UtAssert_True(Result == false, "Result == false");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_BackgroundCfeCore_Test_CfeCoreCSStateDisabled */

void CS_BackgroundOS_Test_Nominal(void)
{
    bool Result;

    CS_AppData.HkPacket.OSCSState        = CS_STATE_ENABLED;
    CS_AppData.OSCodeSeg.ComparisonValue = 99;
    CS_AppData.OSCodeSeg.State           = CS_STATE_ENABLED;

    /* Execute the function being tested */
    Result = CS_BackgroundOS();

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.CurrentEntryInTable == 0,
                  "CS_AppData.HkPacket.CurrentEntryInTable == 0"); /* Reset to 0 by CS_GoToNextTable */
    UtAssert_True(CS_AppData.HkPacket.OSBaseline == 0,
                  "CS_AppData.HkPacket.OSBaseline == 0"); /* Reset to 0 by CS_ComputeEepromMemory */

    UtAssert_True(Result == true, "Result == true");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_BackgroundOS_Test_Nominal */

void CS_BackgroundOS_Test_Miscompare(void)
{
    bool  Result;
    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;
    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), UT_Utils_stub_reporter_hook, &context_CFE_EVS_SendEvent);

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Checksum Failure: OS code segment, Expected: 0x%%08X, Calculated: 0x%%08X");

    CS_AppData.HkPacket.OSCSState        = CS_STATE_ENABLED;
    CS_AppData.OSCodeSeg.ComparisonValue = 99;
    CS_AppData.OSCodeSeg.State           = CS_STATE_ENABLED;

    /* The following section is just to make CS_ComputeEepromMemory return error */
    CS_AppData.OSCodeSeg.ByteOffset         = 0;
    CS_AppData.OSCodeSeg.NumBytesToChecksum = 1;
    CS_AppData.MaxBytesPerCycle             = 2;
    CS_AppData.OSCodeSeg.ComputedYet        = true;
    CS_AppData.OSCodeSeg.ComparisonValue    = 5;
    UT_SetDeferredRetcode(UT_KEY(CS_ComputeEepromMemory), 1, CS_ERROR);

    /* Set to satisfy condition "NewChecksumValue != ResultsEntry -> ComparisonValue" */
    /* End of section to make CS_ComputeEepromMemory return error */

    UT_SetHandlerFunction(UT_KEY(CS_ComputeEepromMemory), CS_UTILS_TEST_CS_ComputeEepromMemoryHandler1, NULL);

    /* Execute the function being tested */
    Result = CS_BackgroundOS();

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.OSCSErrCounter == 1, "CS_AppData.HkPacket.OSCSErrCounter == 1");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventID, CS_OS_MISCOMPARE_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent.Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent.Spec);

    UtAssert_True(CS_AppData.HkPacket.CurrentEntryInTable == 0,
                  "CS_AppData.HkPacket.CurrentEntryInTable == 0"); /* Reset to 0 by CS_GoToNextTable */
    UtAssert_True(CS_AppData.HkPacket.OSBaseline == 5, "CS_AppData.HkPacket.OSBaseline == 5");

    UtAssert_True(Result == true, "Result == true");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_BackgroundOS_Test_Miscompare */

void CS_BackgroundOS_Test_ResultsEntryDisabled(void)
{
    bool Result;

    CS_AppData.HkPacket.OSCSState = CS_STATE_ENABLED;
    CS_AppData.OSCodeSeg.State    = CS_STATE_DISABLED;

    /* Execute the function being tested */
    Result = CS_BackgroundOS();

    /* Verify results */
    UtAssert_True(Result == false, "Result == false");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_BackgroundOS_Test_ResultsEntryDisabled */

void CS_BackgroundOS_Test_OSCSStateDisabled(void)
{
    bool Result;

    CS_AppData.HkPacket.OSCSState = CS_STATE_DISABLED;

    /* Execute the function being tested */
    Result = CS_BackgroundOS();

    /* Verify results */
    UtAssert_True(Result == false, "Result == false");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_BackgroundOS_Test_OSCSStateDisabled */

void CS_BackgroundEeprom_Test_Nominal(void)
{
    bool Result;

    CS_AppData.HkPacket.CurrentEntryInTable = CS_MAX_NUM_EEPROM_TABLE_ENTRIES;

    CS_AppData.HkPacket.EepromCSState = CS_STATE_ENABLED;
    CS_AppData.ResEepromTblPtr        = (CS_Res_EepromMemory_Table_Entry_t *)CS_AppData.DefaultEepromDefTable;
    CS_AppData.ResEepromTblPtr[0].ComparisonValue = 99;
    CS_AppData.ResEepromTblPtr[0].State           = CS_STATE_ENABLED;

    CS_AppData.ResEepromTblPtr[0].ComparisonValue = 1;

    UT_SetHandlerFunction(UT_KEY(CS_ComputeEepromMemory), CS_UTILS_TEST_CS_ComputeEepromMemoryHandler1, NULL);

    /* Execute the function being tested */
    Result = CS_BackgroundEeprom();

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.CurrentEntryInTable == 0,
                  "CS_AppData.HkPacket.CurrentEntryInTable == 0"); /* Reset to 0 by CS_GoToNextTable */
    UtAssert_True(CS_AppData.HkPacket.EepromBaseline == 1, "CS_AppData.HkPacket.EepromBaseline == 1");
    UtAssert_True(Result == false, "Result == false");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_BackgroundEeprom_Test_Nominal */

void CS_BackgroundEeprom_Test_Miscompare(void)
{
    bool  Result;
    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;
    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), UT_Utils_stub_reporter_hook, &context_CFE_EVS_SendEvent);

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Checksum Failure: Entry %%d in Eeprom Table, Expected: 0x%%08X, Calculated: 0x%%08X");

    CS_AppData.HkPacket.CurrentEntryInTable = CS_MAX_NUM_EEPROM_TABLE_ENTRIES;

    CS_AppData.HkPacket.EepromCSState = CS_STATE_ENABLED;
    CS_AppData.ResEepromTblPtr        = (CS_Res_EepromMemory_Table_Entry_t *)CS_AppData.DefaultEepromDefTable;
    CS_AppData.ResEepromTblPtr[CS_MAX_NUM_EEPROM_TABLE_ENTRIES].ComparisonValue = 99;
    CS_AppData.ResEepromTblPtr[CS_MAX_NUM_EEPROM_TABLE_ENTRIES].State           = CS_STATE_ENABLED;

    /* The following section is just to make CS_ComputeEepromMemory return error */
    CS_AppData.ResEepromTblPtr[CS_MAX_NUM_EEPROM_TABLE_ENTRIES].ByteOffset         = 0;
    CS_AppData.ResEepromTblPtr[CS_MAX_NUM_EEPROM_TABLE_ENTRIES].NumBytesToChecksum = 1;
    CS_AppData.MaxBytesPerCycle                                                    = 2;
    CS_AppData.ResEepromTblPtr[CS_MAX_NUM_EEPROM_TABLE_ENTRIES].ComputedYet        = true;
    CS_AppData.ResEepromTblPtr[CS_MAX_NUM_EEPROM_TABLE_ENTRIES].ComparisonValue    = 5;
    CS_AppData.ResEepromTblPtr[0].ComparisonValue =
        5; /* ResEepromTblPtr[0] is added to EntireEepromCS, ResEepromTblPtr[16] is not, because it's outside the valid
              range */
    UT_SetDeferredRetcode(UT_KEY(CS_ComputeEepromMemory), 1, CS_ERROR);

    /* Set to satisfy condition "NewChecksumValue != ResultsEntry -> ComparisonValue" */

    /* End of section to make CS_ComputeEepromMemory return error */

    UT_SetHandlerFunction(UT_KEY(CS_ComputeEepromMemory), CS_UTILS_TEST_CS_ComputeEepromMemoryHandler1, NULL);

    /* Execute the function being tested */
    Result = CS_BackgroundEeprom();

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.EepromCSErrCounter == 1, "CS_AppData.HkPacket.EepromCSErrCounter == 1");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventID, CS_EEPROM_MISCOMPARE_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent.Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent.Spec);

    UtAssert_True(CS_AppData.HkPacket.CurrentEntryInTable == 0,
                  "CS_AppData.HkPacket.CurrentEntryInTable == 0"); /* Reset to 0 by CS_GoToNextTable */
    UtAssert_True(CS_AppData.HkPacket.EepromBaseline == 5, "CS_AppData.HkPacket.EepromBaseline == 5");

    UtAssert_True(Result == true, "Result == true");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_BackgroundEeprom_Test_Miscompare */

void CS_BackgroundEeprom_Test_FindEnabledEepromEntryFalse(void)
{
    bool Result;

    CS_AppData.HkPacket.EepromCSState = CS_STATE_ENABLED;

    /* Execute the function being tested */
    Result = CS_BackgroundEeprom();

    /* Verify results */
    UtAssert_True(Result == false, "Result == false");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_BackgroundEeprom_Test_FindEnabledEepromEntryFalse */

void CS_BackgroundEeprom_Test_EepromCSStateDisabled(void)
{
    bool Result;

    CS_AppData.HkPacket.EepromCSState = CS_STATE_DISABLED;

    /* Execute the function being tested */
    Result = CS_BackgroundEeprom();

    /* Verify results */
    UtAssert_True(Result == false, "Result == false");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_BackgroundEeprom_Test_EepromCSStateDisabled */

void CS_BackgroundEeprom_Test_ComputeEeprom(void)
{
    bool Result;

    CS_AppData.HkPacket.CurrentEntryInTable = 0;

    CS_AppData.HkPacket.EepromCSState = CS_STATE_ENABLED;
    CS_AppData.ResEepromTblPtr        = (CS_Res_EepromMemory_Table_Entry_t *)CS_AppData.DefaultEepromDefTable;
    CS_AppData.ResEepromTblPtr[0].ComparisonValue = 99;
    CS_AppData.ResEepromTblPtr[0].State           = CS_STATE_ENABLED;

    CS_AppData.ResEepromTblPtr[0].ComparisonValue = 1;

    /* Execute the function being tested */
    Result = CS_BackgroundEeprom();

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.CurrentEntryInTable == 0,
                  "CS_AppData.HkPacket.CurrentEntryInTable == 0"); /* Reset to 0 by CS_GoToNextTable */
    UtAssert_True(CS_AppData.HkPacket.EepromBaseline == 0, "CS_AppData.HkPacket.EepromBaseline == 0");
    UtAssert_True(Result == true, "Result == true");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_BackgroundEeprom_Test_ComputeEeprom */

void CS_BackgroundMemory_Test_Nominal(void)
{
    bool Result;

    CS_AppData.HkPacket.CurrentEntryInTable = CS_MAX_NUM_MEMORY_TABLE_ENTRIES;

    CS_AppData.HkPacket.MemoryCSState = CS_STATE_ENABLED;
    CS_AppData.ResMemoryTblPtr        = (CS_Res_EepromMemory_Table_Entry_t *)CS_AppData.DefaultMemoryDefTable;
    CS_AppData.ResMemoryTblPtr[CS_MAX_NUM_MEMORY_TABLE_ENTRIES].ComparisonValue = 99;
    CS_AppData.ResMemoryTblPtr[CS_MAX_NUM_MEMORY_TABLE_ENTRIES].State           = CS_STATE_ENABLED;

    CS_AppData.ResMemoryTblPtr[CS_MAX_NUM_MEMORY_TABLE_ENTRIES].ComparisonValue = 1;

    UT_SetHandlerFunction(UT_KEY(CS_ComputeEepromMemory), CS_UTILS_TEST_CS_ComputeEepromMemoryHandler1, NULL);

    /* Execute the function being tested */
    Result = CS_BackgroundMemory();

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.CurrentEntryInTable == 0,
                  "CS_AppData.HkPacket.CurrentEntryInTable == 0"); /* Reset to 0 by CS_GoToNextTable */
    UtAssert_True(Result == true, "Result == true");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_BackgroundMemory_Test_Nominal */

void CS_BackgroundMemory_Test_Miscompare(void)
{
    bool  Result;
    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;
    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), UT_Utils_stub_reporter_hook, &context_CFE_EVS_SendEvent);

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Checksum Failure: Entry %%d in Memory Table, Expected: 0x%%08X, Calculated: 0x%%08X");

    CS_AppData.HkPacket.CurrentEntryInTable = CS_MAX_NUM_MEMORY_TABLE_ENTRIES;

    CS_AppData.HkPacket.MemoryCSState = CS_STATE_ENABLED;
    CS_AppData.ResMemoryTblPtr        = (CS_Res_EepromMemory_Table_Entry_t *)CS_AppData.DefaultMemoryDefTable;
    CS_AppData.ResMemoryTblPtr[CS_MAX_NUM_MEMORY_TABLE_ENTRIES].ComparisonValue = 99;
    CS_AppData.ResMemoryTblPtr[CS_MAX_NUM_MEMORY_TABLE_ENTRIES].State           = CS_STATE_ENABLED;

    /* The following section is just to make CS_ComputeEepromMemory return error */
    CS_AppData.ResMemoryTblPtr[CS_MAX_NUM_MEMORY_TABLE_ENTRIES].ByteOffset         = 0;
    CS_AppData.ResMemoryTblPtr[CS_MAX_NUM_MEMORY_TABLE_ENTRIES].NumBytesToChecksum = 1;
    CS_AppData.MaxBytesPerCycle                                                    = 2;
    CS_AppData.ResMemoryTblPtr[CS_MAX_NUM_MEMORY_TABLE_ENTRIES].ComputedYet        = true;
    CS_AppData.ResMemoryTblPtr[CS_MAX_NUM_MEMORY_TABLE_ENTRIES].ComparisonValue    = 5;
    CS_AppData.ResMemoryTblPtr[0].ComparisonValue =
        5; /* ResMemoryTblPtr[0] is added to EntireMemoryCS, ResMemoryTblPtr[16] is not, because it's outside the valid
              range */
    UT_SetDeferredRetcode(UT_KEY(CS_ComputeEepromMemory), 1, CS_ERROR);

    /* Set to satisfy condition "NewChecksumValue != ResultsEntry -> ComparisonValue" */

    /* End of section to make CS_ComputeEepromMemory return error */

    /* Execute the function being tested */
    Result = CS_BackgroundMemory();

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.MemoryCSErrCounter == 1, "CS_AppData.HkPacket.MemoryCSErrCounter == 1");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventID, CS_MEMORY_MISCOMPARE_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent.Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent.Spec);

    UtAssert_True(CS_AppData.HkPacket.CurrentEntryInTable == 0,
                  "CS_AppData.HkPacket.CurrentEntryInTable == 0"); /* Reset to 0 by CS_GoToNextTable */

    UtAssert_True(Result == true, "Result == true");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_BackgroundMemory_Test_Miscompare */

void CS_BackgroundMemory_Test_FindEnabledMemoryEntryFalse(void)
{
    bool Result;

    CS_AppData.HkPacket.MemoryCSState = CS_STATE_ENABLED;

    /* Execute the function being tested */
    Result = CS_BackgroundMemory();

    /* Verify results */
    UtAssert_True(Result == false, "Result == false");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_BackgroundMemory_Test_FindEnabledMemoryEntryFalse */

void CS_BackgroundMemory_Test_MemoryCSStateDisabled(void)
{
    bool Result;

    CS_AppData.HkPacket.MemoryCSState = CS_STATE_DISABLED;

    /* Execute the function being tested */
    Result = CS_BackgroundMemory();

    /* Verify results */
    UtAssert_True(Result == false, "Result == false");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_BackgroundMemory_Test_MemoryCSStateDisabled */

void CS_BackgroundMemory_Test_CurrentEntry(void)
{
    bool Result;

    CS_AppData.HkPacket.CurrentEntryInTable = 0;

    CS_AppData.HkPacket.MemoryCSState = CS_STATE_ENABLED;
    CS_AppData.ResMemoryTblPtr        = (CS_Res_EepromMemory_Table_Entry_t *)CS_AppData.DefaultMemoryDefTable;
    CS_AppData.ResMemoryTblPtr[1].ComparisonValue = 99;
    CS_AppData.ResMemoryTblPtr[1].State           = CS_STATE_ENABLED;

    CS_AppData.ResMemoryTblPtr[CS_MAX_NUM_MEMORY_TABLE_ENTRIES].ComparisonValue = 1;

    /* Execute the function being tested */
    Result = CS_BackgroundMemory();

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.CurrentEntryInTable == 1, "CS_AppData.HkPacket.CurrentEntryInTable == 1");
    UtAssert_True(Result == true, "Result == true");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_BackgroundMemory_Test_CurrentEntry */

void CS_BackgroundTables_Test_Nominal(void)
{
    bool Result;

    CS_AppData.HkPacket.CurrentEntryInTable = CS_MAX_NUM_TABLES_TABLE_ENTRIES;

    CS_AppData.HkPacket.TablesCSState = CS_STATE_ENABLED;
    CS_AppData.ResMemoryTblPtr        = (CS_Res_EepromMemory_Table_Entry_t *)CS_AppData.DefaultMemoryDefTable;
    CS_AppData.ResTablesTblPtr[CS_MAX_NUM_TABLES_TABLE_ENTRIES].ComparisonValue = 99;
    CS_AppData.ResTablesTblPtr[CS_MAX_NUM_TABLES_TABLE_ENTRIES].State           = CS_STATE_ENABLED;

    CS_AppData.ResTablesTblPtr[CS_MAX_NUM_TABLES_TABLE_ENTRIES].ComparisonValue = 1;

    /* Set to prevent CS_ComputeTables from returning an error */

    UT_SetHandlerFunction(UT_KEY(CS_ComputeTables), CS_UTILS_TEST_CS_ComputeTablesHandler1, NULL);

    /* Execute the function being tested */
    Result = CS_BackgroundTables();

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.CurrentEntryInTable == 0,
                  "CS_AppData.HkPacket.CurrentEntryInTable == 0"); /* Reset to 0 by CS_GoToNextTable */
    UtAssert_True(Result == true, "Result == true");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_BackgroundTables_Test_Nominal */

void CS_BackgroundTables_Test_Miscompare(void)
{
    bool  Result;
    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;
    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), UT_Utils_stub_reporter_hook, &context_CFE_EVS_SendEvent);

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Checksum Failure: Table %%s, Expected: 0x%%08X, Calculated: 0x%%08X");

    CS_AppData.HkPacket.CurrentEntryInTable = CS_MAX_NUM_TABLES_TABLE_ENTRIES;

    CS_AppData.HkPacket.TablesCSState = CS_STATE_ENABLED;
    CS_AppData.ResAppTblPtr           = (CS_Res_App_Table_Entry_t *)CS_AppData.DefaultAppDefTable;
    CS_AppData.ResTablesTblPtr[CS_MAX_NUM_TABLES_TABLE_ENTRIES].State = CS_STATE_ENABLED;

    strncpy(CS_AppData.ResTablesTblPtr[CS_MAX_NUM_TABLES_TABLE_ENTRIES].Name, "name", 10);

    /* The following section is just to make CS_ComputeTables return error */

    CS_AppData.ResTablesTblPtr[CS_MAX_NUM_TABLES_TABLE_ENTRIES].TblHandle = 99;

    CS_AppData.ResTablesTblPtr[CS_MAX_NUM_TABLES_TABLE_ENTRIES].ByteOffset = 0;
    CS_AppData.MaxBytesPerCycle                                            = 5;

    CS_AppData.ResTablesTblPtr[CS_MAX_NUM_TABLES_TABLE_ENTRIES].ComputedYet = true;

    CS_AppData.ResTablesTblPtr[CS_MAX_NUM_TABLES_TABLE_ENTRIES].ComparisonValue = 1;

    /* Set to satisfy condition "Result == CFE_SUCCESS" and to fail other conditions that check for other values of
     * Result */

    /* Set to fail condition "NewChecksumValue != ResultsEntry -> ComparisonValue" */

    UT_SetDeferredRetcode(UT_KEY(CS_ComputeTables), 1, CS_ERROR);
    /* End of section to make CS_ComputeTables return error */

    /* Execute the function being tested */
    Result = CS_BackgroundTables();

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.TablesCSErrCounter == 1, "CS_AppData.HkPacket.TablesCSErrCounter == 1");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventID, CS_TABLES_MISCOMPARE_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent.Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent.Spec);

    UtAssert_True(CS_AppData.HkPacket.CurrentEntryInTable == 0,
                  "CS_AppData.HkPacket.CurrentEntryInTable == 0"); /* Reset to 0 by CS_GoToNextTable */

    UtAssert_True(Result == true, "Result == true");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_BackgroundTables_Test_Miscompare */

void CS_BackgroundTables_Test_TablesNotFound(void)
{
    bool  Result;
    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;
    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), UT_Utils_stub_reporter_hook, &context_CFE_EVS_SendEvent);

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Tables table computing: Table %%s could not be found, skipping");

    CS_AppData.HkPacket.TablesCSState       = CS_STATE_ENABLED;
    CS_AppData.HkPacket.CurrentEntryInTable = 0;
    CS_AppData.ResTablesTblPtr              = (CS_Res_Tables_Table_Entry_t *)CS_AppData.DefaultTablesDefTable;
    CS_AppData.ResTablesTblPtr[0].State     = CS_STATE_ENABLED;
    strncpy(CS_AppData.ResTablesTblPtr[0].Name, "name", 10);

    /* Set to make ComputeTables return CS_ERR_NOT_FOUND */
    UT_SetDeferredRetcode(UT_KEY(CS_ComputeTables), 1, CS_ERR_NOT_FOUND);

    /* Execute the function being tested */
    Result = CS_BackgroundTables();

    /* Verify results */

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventID, CS_COMPUTE_TABLES_NOT_FOUND_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent.Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent.Spec);

    UtAssert_True(CS_AppData.HkPacket.CurrentEntryInTable == 1, "CS_AppData.HkPacket.CurrentEntryInTable == 1");

    UtAssert_True(Result == true, "Result == true");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
    /* Generates 1 event message we don't care about in this test */

} /* end CS_BackgroundTables_Test_TablesNotFound */

void CS_BackgroundTables_Test_FindEnabledTablesEntryFalse(void)
{
    bool Result;

    CS_AppData.HkPacket.TablesCSState = CS_STATE_ENABLED;

    /* Execute the function being tested */
    Result = CS_BackgroundTables();

    /* Verify results */
    UtAssert_True(Result == false, "Result == false");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_BackgroundTables_Test_FindEnabledTablesEntryFalse */

void CS_BackgroundTables_Test_TablesCSStateDisabled(void)
{
    bool Result;

    CS_AppData.HkPacket.TablesCSState = CS_STATE_DISABLED;

    /* Execute the function being tested */
    Result = CS_BackgroundTables();

    /* Verify results */
    UtAssert_True(Result == false, "Result == false");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_BackgroundTables_Test_TablesCSStateDisabled */

void CS_BackgroundApp_Test_Nominal(void)
{
    bool Result;

    CS_AppData.HkPacket.AppCSState          = CS_STATE_ENABLED;
    CS_AppData.HkPacket.CurrentEntryInTable = CS_MAX_NUM_APP_TABLE_ENTRIES;

    CS_AppData.ResAppTblPtr = (CS_Res_App_Table_Entry_t *)CS_AppData.DefaultAppDefTable;
    CS_AppData.ResAppTblPtr[CS_MAX_NUM_APP_TABLE_ENTRIES].ComparisonValue = 99;
    CS_AppData.ResAppTblPtr[CS_MAX_NUM_APP_TABLE_ENTRIES].State           = CS_STATE_ENABLED;

    CS_AppData.ResAppTblPtr[CS_MAX_NUM_APP_TABLE_ENTRIES].ComparisonValue = 1;

    /* Set to prevent CS_ComputeApp from returning an error */

    UT_SetHandlerFunction(UT_KEY(CS_ComputeApp), CS_UTILS_TEST_CS_ComputeAppHandler1, NULL);

    /* Execute the function being tested */
    Result = CS_BackgroundApp();

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.CurrentEntryInTable == 0,
                  "CS_AppData.HkPacket.CurrentEntryInTable == 0"); /* Reset to 0 by CS_GoToNextTable */
    UtAssert_True(Result == true, "Result == true");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_BackgroundApp_Test_Nominal */

void CS_BackgroundApp_Test_NextTable(void)
{
    bool Result;

    CS_AppData.HkPacket.AppCSState          = CS_STATE_ENABLED;
    CS_AppData.HkPacket.CurrentEntryInTable = CS_MAX_NUM_APP_TABLE_ENTRIES;

    CS_AppData.ResAppTblPtr = (CS_Res_App_Table_Entry_t *)CS_AppData.DefaultAppDefTable;
    CS_AppData.ResAppTblPtr[CS_MAX_NUM_APP_TABLE_ENTRIES].ComparisonValue = 99;
    CS_AppData.ResAppTblPtr[CS_MAX_NUM_APP_TABLE_ENTRIES].State           = CS_STATE_DISABLED;

    CS_AppData.ResAppTblPtr[CS_MAX_NUM_APP_TABLE_ENTRIES].ComparisonValue = 1;

    /* Set to prevent CS_ComputeApp from returning an error */

    UT_SetHandlerFunction(UT_KEY(CS_ComputeApp), CS_UTILS_TEST_CS_ComputeAppHandler1, NULL);

    /* Execute the function being tested */
    Result = CS_BackgroundApp();

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.CurrentEntryInTable == 0,
                  "CS_AppData.HkPacket.CurrentEntryInTable == 0"); /* Reset to 0 by CS_GoToNextTable */
    UtAssert_True(Result == false, "Result == false");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_BackgroundApp_Test_NextTable */

void CS_BackgroundApp_Test_Miscompare(void)
{
    bool  Result;
    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;
    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), UT_Utils_stub_reporter_hook, &context_CFE_EVS_SendEvent);

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Checksum Failure: Application %%s, Expected: 0x%%08X, Calculated: 0x%%08X");

    CS_AppData.HkPacket.AppCSState          = CS_STATE_ENABLED;
    CS_AppData.HkPacket.CurrentEntryInTable = CS_MAX_NUM_APP_TABLE_ENTRIES;

    CS_AppData.ResAppTblPtr = (CS_Res_App_Table_Entry_t *)CS_AppData.DefaultAppDefTable;
    CS_AppData.ResAppTblPtr[CS_MAX_NUM_APP_TABLE_ENTRIES].State = CS_STATE_ENABLED;

    strncpy(CS_AppData.ResAppTblPtr[CS_MAX_NUM_APP_TABLE_ENTRIES].Name, "name", 10);

    /* The following section is just to make CS_ComputeApp return error */

    CS_AppData.ResAppTblPtr[CS_MAX_NUM_APP_TABLE_ENTRIES].ByteOffset = 0;
    CS_AppData.MaxBytesPerCycle                                      = 5;

    CS_AppData.ResAppTblPtr[CS_MAX_NUM_APP_TABLE_ENTRIES].ComputedYet = true;

    CS_AppData.ResAppTblPtr[CS_MAX_NUM_APP_TABLE_ENTRIES].ComparisonValue = 3;

    /* Sets AppInfo.CodeSize = 5, sets AppInfo.CodeAddress = 1, AppInfo.AddressesAreValid = true, and returns
     * CFE_SUCCESS */

    /* Set to fail condition "NewChecksumValue != ResultsEntry -> ComparisonValue" */

    UT_SetDeferredRetcode(UT_KEY(CS_ComputeApp), 1, CS_ERROR);
    /* End of section to make CS_ComputeApp return error */

    /* Execute the function being tested */
    Result = CS_BackgroundApp();

    /* Verify results */
    UtAssert_True(CS_AppData.HkPacket.AppCSErrCounter == 1, "CS_AppData.HkPacket.AppCSErrCounter == 1");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventID, CS_APP_MISCOMPARE_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent.Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent.Spec);

    UtAssert_True(CS_AppData.HkPacket.CurrentEntryInTable == 0,
                  "CS_AppData.HkPacket.CurrentEntryInTable == 0"); /* Reset to 0 by CS_GoToNextTable */

    UtAssert_True(Result == true, "Result == true");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_BackgroundApp_Test_Miscompare */

void CS_BackgroundApp_Test_AppNotFound(void)
{
    bool  Result;
    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;
    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), UT_Utils_stub_reporter_hook, &context_CFE_EVS_SendEvent);

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "App table computing: App %%s could not be found, skipping");

    CS_AppData.HkPacket.AppCSState          = CS_STATE_ENABLED;
    CS_AppData.HkPacket.CurrentEntryInTable = 0;

    CS_AppData.ResAppTblPtr          = (CS_Res_App_Table_Entry_t *)CS_AppData.DefaultAppDefTable;
    CS_AppData.ResAppTblPtr[0].State = CS_STATE_ENABLED;
    strncpy(CS_AppData.ResAppTblPtr[0].Name, "name", 10);

    /* Set to make ComputeApp return CS_ERR_NOT_FOUND */
    UT_SetDeferredRetcode(UT_KEY(CS_ComputeApp), 1, CS_ERR_NOT_FOUND);

    /* Execute the function being tested */
    Result = CS_BackgroundApp();

    /* Verify results */

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventID, CS_COMPUTE_APP_NOT_FOUND_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent.Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent.Spec);

    UtAssert_True(CS_AppData.HkPacket.CurrentEntryInTable == 1, "CS_AppData.HkPacket.CurrentEntryInTable == 1");

    UtAssert_True(Result == true, "Result == true");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
    /* Generates 1 event message we don't care about in this test */

} /* end CS_BackgroundApp_Test_AppNotFound */

void CS_BackgroundApp_Test_FindEnabledAppEntryFalse(void)
{
    bool Result;

    /* Execute the function being tested */
    Result = CS_BackgroundApp();

    /* Verify results */
    UtAssert_True(Result == false, "Result == false");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_BackgroundApp_Test_FindEnabledAppEntryFalse */

void CS_BackgroundApp_Test_AppCSStateDisabled(void)
{
    bool Result;

    /* Execute the function being tested */
    Result = CS_BackgroundApp();

    /* Verify results */
    UtAssert_True(Result == false, "Result == false");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_BackgroundApp_Test_AppCSStateDisabled */

void CS_ResetTablesTblResultEntry_Test(void)
{
    /* Execute the function being tested */
    CS_AppData.ResTablesTblPtr = (CS_Res_Tables_Table_Entry_t *)CS_AppData.DefaultTablesDefTable;
    CS_ResetTablesTblResultEntry(CS_AppData.ResTablesTblPtr);

    /* Verify results */
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].ByteOffset == 0, "CS_AppData.ResTablesTblPtr[0].ByteOffset == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].TempChecksumValue == 0,
                  "CS_AppData.ResTablesTblPtr[0].TempChecksumValue == 0");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].ComputedYet == false,
                  "CS_AppData.ResTablesTblPtr[0].ComputedYet == false");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_ResetTablesTblResultEntry_Test */

void CS_ResetTablesTblResultEntry_Test_NullEntry(void)
{
    CS_Res_Tables_Table_Entry_t *ResTablesTblPtr = NULL;

    /* Execute the function being tested */
    CS_ResetTablesTblResultEntry(ResTablesTblPtr);

    /* Verify results */
    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_ResetTablesTblResultEntry_Test_NullEntry */

void CS_HandleRoutineTableUpdates_Test_Nominal(void)
{
    int32 Result;

    CS_AppData.HkPacket.RecomputeInProgress = false;
    CS_AppData.HkPacket.OneShotInProgress   = true;
    CS_AppData.ChildTaskTable               = CS_CFECORE;

    /* Execute the function being tested */
    Result = CS_HandleRoutineTableUpdates();

    UtAssert_True(Result == CS_SUCCESS, "Result == CS_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_HandleRoutineTableUpdates_Test_Nominal */

void CS_HandleRoutineTableUpdates_Test_UpdateError(void)
{
    int32 Result;

    CS_AppData.HkPacket.RecomputeInProgress = false;
    CS_AppData.HkPacket.OneShotInProgress   = true;
    CS_AppData.ChildTaskTable               = CS_CFECORE;

    UT_SetDefaultReturnValue(UT_KEY(CS_HandleTableUpdate), -1);

    /* Execute the function being tested */
    Result = CS_HandleRoutineTableUpdates();

    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 4, "CFE_EVS_SendEvent was called %u time(s), expected 4",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_HandleRoutineTableUpdates_Test_UpateError */

void CS_HandleRoutineTableUpdates_Test_Eeprom(void)
{
    int32 Result;

    CS_AppData.HkPacket.RecomputeInProgress = true;
    CS_AppData.HkPacket.OneShotInProgress   = false;
    CS_AppData.ChildTaskTable               = CS_EEPROM_TABLE;

    /* Execute the function being tested */
    Result = CS_HandleRoutineTableUpdates();

    UtAssert_True(Result == CS_SUCCESS, "Result == CS_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_HandleRoutineTableUpdates_Test_Eeprom */

void CS_HandleRoutineTableUpdates_Test_Memory(void)
{
    int32 Result;

    CS_AppData.HkPacket.RecomputeInProgress = true;
    CS_AppData.HkPacket.OneShotInProgress   = false;
    CS_AppData.ChildTaskTable               = CS_MEMORY_TABLE;

    /* Execute the function being tested */
    Result = CS_HandleRoutineTableUpdates();

    UtAssert_True(Result == CS_SUCCESS, "Result == CS_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_HandleRoutineTableUpdates_Test_Memory */

void CS_HandleRoutineTableUpdates_Test_OneShot(void)
{
    int32 Result;

    CS_AppData.HkPacket.RecomputeInProgress = true;
    CS_AppData.HkPacket.OneShotInProgress   = true;
    CS_AppData.ChildTaskTable               = CS_CFECORE;

    /* Execute the function being tested */
    Result = CS_HandleRoutineTableUpdates();

    UtAssert_True(Result == CS_SUCCESS, "Result == CS_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_HandleRoutineTableUpdates_Test_OneShot */

void CS_HandleRoutineTableUpdates_Test_App(void)
{
    int32 Result;

    CS_AppData.HkPacket.RecomputeInProgress = true;
    CS_AppData.HkPacket.OneShotInProgress   = false;
    CS_AppData.ChildTaskTable               = CS_APP_TABLE;

    /* Execute the function being tested */
    Result = CS_HandleRoutineTableUpdates();

    UtAssert_True(Result == CS_SUCCESS, "Result == CS_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_HandleRoutineTableUpdates_Test_App */

void CS_HandleRoutineTableUpdates_Test_Tables(void)
{
    int32 Result;

    CS_AppData.HkPacket.RecomputeInProgress = true;
    CS_AppData.HkPacket.OneShotInProgress   = false;
    CS_AppData.ChildTaskTable               = CS_TABLES_TABLE;

    /* Execute the function being tested */
    Result = CS_HandleRoutineTableUpdates();

    UtAssert_True(Result == CS_SUCCESS, "Result == CS_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_HandleRoutineTableUpdates_Test_Tables */

void CS_AttemptTableReshare_Test_Nominal(void)
{
    int32            Result;
    CFE_TBL_Handle_t LocalTblHandle;
    CFE_TBL_Info_t   TblInfo;
    uint32           LocalAddress;
    int32            ResultGetInfo;

    CS_AppData.ResTablesTblPtr = (CS_Res_Tables_Table_Entry_t *)CS_AppData.DefaultTablesDefTable;

    /* Execute the function being tested */
    Result =
        CS_AttemptTableReshare(CS_AppData.ResTablesTblPtr, &LocalTblHandle, &TblInfo, &LocalAddress, &ResultGetInfo);

    /* Verify results */
    UtAssert_True(Result == CS_SUCCESS, "Result == CS_SUCCESS");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].TblHandle == LocalTblHandle,
                  "CS_AppData.ResTablesTblPtr[0].TblHandle == LocalTblHandle");
    UtAssert_True(ResultGetInfo == CS_SUCCESS, "Result == CS_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_AttemptTableReshare_Test_Nominal */

void CS_AttemptTableReshare_Test_NeverLoaded(void)
{
    int32            Result;
    CFE_TBL_Handle_t LocalTblHandle;
    CFE_TBL_Info_t   TblInfo;
    uint32           LocalAddress;
    int32            ResultGetInfo;

    CS_AppData.ResTablesTblPtr = (CS_Res_Tables_Table_Entry_t *)CS_AppData.DefaultTablesDefTable;

    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_GetAddress), 1, CFE_TBL_ERR_NEVER_LOADED);

    /* Execute the function being tested */
    Result =
        CS_AttemptTableReshare(CS_AppData.ResTablesTblPtr, &LocalTblHandle, &TblInfo, &LocalAddress, &ResultGetInfo);

    /* Verify results */
    UtAssert_True(Result == CFE_TBL_ERR_NEVER_LOADED, "Result == CFE_TBL_ERR_NEVER_LOADED");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].TblHandle == LocalTblHandle,
                  "CS_AppData.ResTablesTblPtr[0].TblHandle == LocalTblHandle");
    UtAssert_True(ResultGetInfo == CS_SUCCESS, "ResultGetInfo == CS_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_AttemptTableReshare_Test_NeverLoaded */

void CS_AttemptTableReshare_Test_NoTable(void)
{
    int32            Result;
    CFE_TBL_Handle_t LocalTblHandle;
    CFE_TBL_Info_t   TblInfo;
    uint32           LocalAddress;
    int32            ResultGetInfo = 0;

    CS_AppData.ResTablesTblPtr = (CS_Res_Tables_Table_Entry_t *)CS_AppData.DefaultTablesDefTable;

    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_Share), 1, -1);

    /* Execute the function being tested */
    Result =
        CS_AttemptTableReshare(CS_AppData.ResTablesTblPtr, &LocalTblHandle, &TblInfo, &LocalAddress, &ResultGetInfo);

    /* Verify results */
    UtAssert_True(Result == -1, "Result == -1");
    UtAssert_True(CS_AppData.ResTablesTblPtr[0].TblHandle == LocalTblHandle,
                  "CS_AppData.ResTablesTblPtr[0].TblHandle == LocalTblHandle");
    UtAssert_True(ResultGetInfo == 0, "ResultGetInfo == 0");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_AttemptTableReshare_Test_NoTable */

void CS_CheckRecomputeOneShot_Test_OneShotInProgress(void)
{
    bool  Result;
    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;
    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), UT_Utils_stub_reporter_hook, &context_CFE_EVS_SendEvent);

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Cannot perform command. Recompute or oneshot in progress.");

    CS_AppData.HkPacket.OneShotInProgress = true;

    /* Execute the function being tested */
    Result = CS_CheckRecomputeOneshot();

    /* Verify results */
    UtAssert_True(Result == true, "Result == true");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventID, CS_CMD_COMPUTE_PROG_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent.Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent.Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_CheckRecomputeOneShot_Test_OneShotInProgress */

void CS_CheckRecomputeOneShot_Test_RecomputeInProgress(void)
{
    bool  Result;
    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;
    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), UT_Utils_stub_reporter_hook, &context_CFE_EVS_SendEvent);

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Cannot perform command. Recompute or oneshot in progress.");

    CS_AppData.HkPacket.RecomputeInProgress = true;

    /* Execute the function being tested */
    Result = CS_CheckRecomputeOneshot();

    /* Verify results */
    UtAssert_True(Result == true, "Result == true");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventID, CS_CMD_COMPUTE_PROG_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent.Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent.Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_CheckRecomputeOneShot_Test_RecomputeInProgress */

void CS_CheckRecomputeOneShot_Test_NoInProgress(void)
{
    bool Result;

    /* Execute the function being tested */
    Result = CS_CheckRecomputeOneshot();

    /* Verify results */
    UtAssert_True(Result == false, "Result == false");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_CheckRecomputeOneShot_Test_NoInProgress */

void UtTest_Setup(void)
{
    UtTest_Add(CS_ZeroEepromTempValues_Test, CS_Test_Setup, CS_Test_TearDown, "CS_ZeroEepromTempValues_Test");

    UtTest_Add(CS_ZeroMemoryTempValues_Test, CS_Test_Setup, CS_Test_TearDown, "CS_ZeroMemoryTempValues_Test");

    UtTest_Add(CS_ZeroTablesTempValues_Test, CS_Test_Setup, CS_Test_TearDown, "CS_ZeroTablesTempValues_Test");

    UtTest_Add(CS_ZeroAppTempValues_Test, CS_Test_Setup, CS_Test_TearDown, "CS_ZeroAppTempValues_Test");

    UtTest_Add(CS_ZeroCfeCoreTempValues_Test, CS_Test_Setup, CS_Test_TearDown, "CS_ZeroCfeCoreTempValues_Test");

    UtTest_Add(CS_ZeroOSTempValues_Test, CS_Test_Setup, CS_Test_TearDown, "CS_ZeroOSTempValues_Test");

    UtTest_Add(CS_InitializeDefaultTables_Test, CS_Test_Setup, CS_Test_TearDown, "CS_InitializeDefaultTables_Test");

    UtTest_Add(CS_GoToNextTable_Test_Nominal, CS_Test_Setup, CS_Test_TearDown, "CS_GoToNextTable_Test_Nominal");
    UtTest_Add(CS_GoToNextTable_Test_UpdatePassCounter, CS_Test_Setup, CS_Test_TearDown,
               "CS_GoToNextTable_Test_UpdatePassCounter");

    UtTest_Add(CS_GetTableResTblEntryByName_Test, CS_Test_Setup, CS_Test_TearDown, "CS_GetTableResTblEntryByName_Test");
    UtTest_Add(CS_GetTableResTblEntryByName_Test_EmptyName, CS_Test_Setup, CS_Test_TearDown,
               "CS_GetTableResTblEntryByName_Test_EmptyName");
    UtTest_Add(CS_GetTableResTblEntryByName_Test_EmptyState, CS_Test_Setup, CS_Test_TearDown,
               "CS_GetTableResTblEntryByName_Test_EmptyState");

    UtTest_Add(CS_GetTableDefTblEntryByName_Test, CS_Test_Setup, CS_Test_TearDown, "CS_GetTableDefTblEntryByName_Test");
    UtTest_Add(CS_GetTableDefTblEntryByName_Test_EmptyName, CS_Test_Setup, CS_Test_TearDown,
               "CS_GetTableDefTblEntryByName_Test_EmptyName");
    UtTest_Add(CS_GetTableDefTblEntryByName_Test_EmptyState, CS_Test_Setup, CS_Test_TearDown,
               "CS_GetTableDefTblEntryByName_Test_EmptyState");

    UtTest_Add(CS_GetAppResTblEntryByName_Test, CS_Test_Setup, CS_Test_TearDown, "CS_GetAppResTblEntryByName_Test");
    UtTest_Add(CS_GetAppResTblEntryByName_Test_EmptyName, CS_Test_Setup, CS_Test_TearDown,
               "CS_GetAppResTblEntryByName_Test_EmptyName");
    UtTest_Add(CS_GetAppResTblEntryByName_Test_EmptyState, CS_Test_Setup, CS_Test_TearDown,
               "CS_GetAppResTblEntryByName_Test_EmptyState");

    UtTest_Add(CS_GetAppDefTblEntryByName_Test, CS_Test_Setup, CS_Test_TearDown, "CS_GetAppDefTblEntryByName_Test");
    UtTest_Add(CS_GetAppDefTblEntryByName_Test_EmptyName, CS_Test_Setup, CS_Test_TearDown,
               "CS_GetAppDefTblEntryByName_Test_EmptyName");
    UtTest_Add(CS_GetAppDefTblEntryByName_Test_EmptyState, CS_Test_Setup, CS_Test_TearDown,
               "CS_GetAppDefTblEntryByName_Test_EmptyState");

    UtTest_Add(CS_FindEnabledEepromEntry_Test, CS_Test_Setup, CS_Test_TearDown, "CS_FindEnabledEepromEntry_Test");

    UtTest_Add(CS_FindEnabledMemoryEntry_Test, CS_Test_Setup, CS_Test_TearDown, "CS_FindEnabledMemoryEntry_Test");

    UtTest_Add(CS_FindEnabledTablesEntry_Test, CS_Test_Setup, CS_Test_TearDown, "CS_FindEnabledTablesEntry_Test");

    UtTest_Add(CS_FindEnabledAppEntry_Test, CS_Test_Setup, CS_Test_TearDown, "CS_FindEnabledAppEntry_Test");

    UtTest_Add(CS_VerifyCmdLength_Test_Nominal, CS_Test_Setup, CS_Test_TearDown, "CS_VerifyCmdLength_Test_Nominal");
    UtTest_Add(CS_VerifyCmdLength_Test_InvalidMsgLength, CS_Test_Setup, CS_Test_TearDown,
               "CS_VerifyCmdLength_Test_InvalidMsgLength");

    UtTest_Add(CS_BackgroundCfeCore_Test_Nominal, CS_Test_Setup, CS_Test_TearDown, "CS_BackgroundCfeCore_Test_Nominal");
    UtTest_Add(CS_BackgroundCfeCore_Test_Miscompare, CS_Test_Setup, CS_Test_TearDown,
               "CS_BackgroundCfeCore_Test_Miscompare");
    UtTest_Add(CS_BackgroundCfeCore_Test_ResultsEntryDisabled, CS_Test_Setup, CS_Test_TearDown,
               "CS_BackgroundCfeCore_Test_ResultsEntryDisabled");
    UtTest_Add(CS_BackgroundCfeCore_Test_CfeCoreCSStateDisabled, CS_Test_Setup, CS_Test_TearDown,
               "CS_BackgroundCfeCore_Test_CfeCoreCSStateDisabled");

    UtTest_Add(CS_BackgroundOS_Test_Nominal, CS_Test_Setup, CS_Test_TearDown, "CS_BackgroundOS_Test_Nominal");
    UtTest_Add(CS_BackgroundOS_Test_Miscompare, CS_Test_Setup, CS_Test_TearDown, "CS_BackgroundOS_Test_Miscompare");
    UtTest_Add(CS_BackgroundOS_Test_ResultsEntryDisabled, CS_Test_Setup, CS_Test_TearDown,
               "CS_BackgroundOS_Test_ResultsEntryDisabled");
    UtTest_Add(CS_BackgroundOS_Test_OSCSStateDisabled, CS_Test_Setup, CS_Test_TearDown,
               "CS_BackgroundOS_Test_OSCSStateDisabled");

    UtTest_Add(CS_BackgroundEeprom_Test_Nominal, CS_Test_Setup, CS_Test_TearDown, "CS_BackgroundEeprom_Test_Nominal");
    UtTest_Add(CS_BackgroundEeprom_Test_Miscompare, CS_Test_Setup, CS_Test_TearDown,
               "CS_BackgroundEeprom_Test_Miscompare");
    UtTest_Add(CS_BackgroundEeprom_Test_FindEnabledEepromEntryFalse, CS_Test_Setup, CS_Test_TearDown,
               "CS_BackgroundEeprom_Test_FindEnabledEepromEntryFalse");
    UtTest_Add(CS_BackgroundEeprom_Test_EepromCSStateDisabled, CS_Test_Setup, CS_Test_TearDown,
               "CS_BackgroundEeprom_Test_EepromCSStateDisabled");
    UtTest_Add(CS_BackgroundEeprom_Test_ComputeEeprom, CS_Test_Setup, CS_Test_TearDown,
               "CS_BackgroundEeprom_Test_ComputeEeprom");

    UtTest_Add(CS_BackgroundMemory_Test_Nominal, CS_Test_Setup, CS_Test_TearDown, "CS_BackgroundMemory_Test_Nominal");
    UtTest_Add(CS_BackgroundMemory_Test_Miscompare, CS_Test_Setup, CS_Test_TearDown,
               "CS_BackgroundMemory_Test_Miscompare");
    UtTest_Add(CS_BackgroundMemory_Test_FindEnabledMemoryEntryFalse, CS_Test_Setup, CS_Test_TearDown,
               "CS_BackgroundMemory_Test_FindEnabledMemoryEntryFalse");
    UtTest_Add(CS_BackgroundMemory_Test_MemoryCSStateDisabled, CS_Test_Setup, CS_Test_TearDown,
               "CS_BackgroundMemory_Test_MemoryCSStateDisabled");
    UtTest_Add(CS_BackgroundMemory_Test_CurrentEntry, CS_Test_Setup, CS_Test_TearDown,
               "CS_BackgroundMemory_Test_CurrentEntry");

    UtTest_Add(CS_BackgroundTables_Test_Nominal, CS_Test_Setup, CS_Test_TearDown, "CS_BackgroundTables_Test_Nominal");
    UtTest_Add(CS_BackgroundTables_Test_Miscompare, CS_Test_Setup, CS_Test_TearDown,
               "CS_BackgroundTables_Test_Miscompare");
    UtTest_Add(CS_BackgroundTables_Test_TablesNotFound, CS_Test_Setup, CS_Test_TearDown,
               "CS_BackgroundTables_Test_TablesNotFound");
    UtTest_Add(CS_BackgroundTables_Test_FindEnabledTablesEntryFalse, CS_Test_Setup, CS_Test_TearDown,
               "CS_BackgroundTables_Test_FindEnabledTablesEntryFalse");
    UtTest_Add(CS_BackgroundTables_Test_TablesCSStateDisabled, CS_Test_Setup, CS_Test_TearDown,
               "CS_BackgroundTables_Test_TablesCSStateDisabled");

    UtTest_Add(CS_BackgroundApp_Test_Nominal, CS_Test_Setup, CS_Test_TearDown, "CS_BackgroundApp_Test_Nominal");
    UtTest_Add(CS_BackgroundApp_Test_NextTable, CS_Test_Setup, CS_Test_TearDown, "CS_BackgroundApp_Test_NextTable");
    UtTest_Add(CS_BackgroundApp_Test_Miscompare, CS_Test_Setup, CS_Test_TearDown, "CS_BackgroundApp_Test_Miscompare");
    UtTest_Add(CS_BackgroundApp_Test_AppNotFound, CS_Test_Setup, CS_Test_TearDown, "CS_BackgroundApp_Test_AppNotFound");
    UtTest_Add(CS_BackgroundApp_Test_FindEnabledAppEntryFalse, CS_Test_Setup, CS_Test_TearDown,
               "CS_BackgroundApp_Test_FindEnabledAppEntryFalse");
    UtTest_Add(CS_BackgroundApp_Test_AppCSStateDisabled, CS_Test_Setup, CS_Test_TearDown,
               "CS_BackgroundApp_Test_AppCSStateDisabled");

    UtTest_Add(CS_ResetTablesTblResultEntry_Test, CS_Test_Setup, CS_Test_TearDown, "CS_ResetTablesTblResultEntry_Test");
    UtTest_Add(CS_ResetTablesTblResultEntry_Test_NullEntry, CS_Test_Setup, CS_Test_TearDown,
               "CS_ResetTablesTblResultEntry_Test_NullEntry");

    UtTest_Add(CS_HandleRoutineTableUpdates_Test_Nominal, CS_Test_Setup, CS_Test_TearDown,
               "CS_HandleRoutineTableUpdates_Test_Nominal");
    UtTest_Add(CS_HandleRoutineTableUpdates_Test_UpdateError, CS_Test_Setup, CS_Test_TearDown,
               "CS_HandleRoutineTableUpdates_Test_UpdateError");
    UtTest_Add(CS_HandleRoutineTableUpdates_Test_Eeprom, CS_Test_Setup, CS_Test_TearDown,
               "CS_HandleRoutineTableUpdates_Test_Eeprom");
    UtTest_Add(CS_HandleRoutineTableUpdates_Test_Memory, CS_Test_Setup, CS_Test_TearDown,
               "CS_HandleRoutineTableUpdates_Test_Memory");
    UtTest_Add(CS_HandleRoutineTableUpdates_Test_App, CS_Test_Setup, CS_Test_TearDown,
               "CS_HandleRoutineTableUpdates_Test_App");
    UtTest_Add(CS_HandleRoutineTableUpdates_Test_Tables, CS_Test_Setup, CS_Test_TearDown,
               "CS_HandleRoutineTableUpdates_Test_Tables");
    UtTest_Add(CS_HandleRoutineTableUpdates_Test_OneShot, CS_Test_Setup, CS_Test_TearDown,
               "CS_HandleRoutineTableUpdates_Test_OneShot");

    UtTest_Add(CS_AttemptTableReshare_Test_Nominal, CS_Test_Setup, CS_Test_TearDown,
               "CS_AttemptTableReshare_Test_Nominal");
    UtTest_Add(CS_AttemptTableReshare_Test_NeverLoaded, CS_Test_Setup, CS_Test_TearDown,
               "CS_AttemptTableReshare_Test_NeverLoaded");
    UtTest_Add(CS_AttemptTableReshare_Test_NoTable, CS_Test_Setup, CS_Test_TearDown,
               "CS_AttemptTableReshare_Test_NoTable");

    UtTest_Add(CS_CheckRecomputeOneShot_Test_OneShotInProgress, CS_Test_Setup, CS_Test_TearDown,
               "CS_CheckRecomputeOneShot_Test_OneShotInProgress");
    UtTest_Add(CS_CheckRecomputeOneShot_Test_RecomputeInProgress, CS_Test_Setup, CS_Test_TearDown,
               "CS_CheckRecomputeOneShot_Test_RecomputeInProgress");
    UtTest_Add(CS_CheckRecomputeOneShot_Test_NoInProgress, CS_Test_Setup, CS_Test_TearDown,
               "CS_CheckRecomputeOneShot_Test_NoInProgress");

} /* end UtTest_Setup */

/************************/
/*  End of File Comment */
/************************/
