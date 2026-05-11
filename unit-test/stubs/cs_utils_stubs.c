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

/**
 * @file
 *
 * Auto-Generated stub implementations for functions defined in cs_utils header
 */

#include "cs_utils.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for CS_BackgroundApp()
 * ----------------------------------------------------
 */
bool CS_BackgroundApp(void)
{
    UT_GenStub_SetupReturnBuffer(CS_BackgroundApp, bool);

    UT_GenStub_Execute(CS_BackgroundApp, Basic, NULL);

    return UT_GenStub_GetReturnValue(CS_BackgroundApp, bool);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CS_BackgroundCfeCore()
 * ----------------------------------------------------
 */
bool CS_BackgroundCfeCore(void)
{
    UT_GenStub_SetupReturnBuffer(CS_BackgroundCfeCore, bool);

    UT_GenStub_Execute(CS_BackgroundCfeCore, Basic, NULL);

    return UT_GenStub_GetReturnValue(CS_BackgroundCfeCore, bool);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CS_BackgroundEeprom()
 * ----------------------------------------------------
 */
bool CS_BackgroundEeprom(void)
{
    UT_GenStub_SetupReturnBuffer(CS_BackgroundEeprom, bool);

    UT_GenStub_Execute(CS_BackgroundEeprom, Basic, NULL);

    return UT_GenStub_GetReturnValue(CS_BackgroundEeprom, bool);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CS_BackgroundMemory()
 * ----------------------------------------------------
 */
bool CS_BackgroundMemory(void)
{
    UT_GenStub_SetupReturnBuffer(CS_BackgroundMemory, bool);

    UT_GenStub_Execute(CS_BackgroundMemory, Basic, NULL);

    return UT_GenStub_GetReturnValue(CS_BackgroundMemory, bool);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CS_BackgroundOS()
 * ----------------------------------------------------
 */
bool CS_BackgroundOS(void)
{
    UT_GenStub_SetupReturnBuffer(CS_BackgroundOS, bool);

    UT_GenStub_Execute(CS_BackgroundOS, Basic, NULL);

    return UT_GenStub_GetReturnValue(CS_BackgroundOS, bool);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CS_BackgroundTables()
 * ----------------------------------------------------
 */
bool CS_BackgroundTables(void)
{
    UT_GenStub_SetupReturnBuffer(CS_BackgroundTables, bool);

    UT_GenStub_Execute(CS_BackgroundTables, Basic, NULL);

    return UT_GenStub_GetReturnValue(CS_BackgroundTables, bool);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CS_CheckDefTableNameMatch()
 * ----------------------------------------------------
 */
bool CS_CheckDefTableNameMatch(const char *Name, uint16 TableIdx)
{
    UT_GenStub_SetupReturnBuffer(CS_CheckDefTableNameMatch, bool);

    UT_GenStub_AddParam(CS_CheckDefTableNameMatch, const char *, Name);
    UT_GenStub_AddParam(CS_CheckDefTableNameMatch, uint16, TableIdx);

    UT_GenStub_Execute(CS_CheckDefTableNameMatch, Basic, NULL);

    return UT_GenStub_GetReturnValue(CS_CheckDefTableNameMatch, bool);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CS_CheckRecomputeOneshot()
 * ----------------------------------------------------
 */
bool CS_CheckRecomputeOneshot(void)
{
    UT_GenStub_SetupReturnBuffer(CS_CheckRecomputeOneshot, bool);

    UT_GenStub_Execute(CS_CheckRecomputeOneshot, Basic, NULL);

    return UT_GenStub_GetReturnValue(CS_CheckRecomputeOneshot, bool);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CS_CheckResTableNameMatch()
 * ----------------------------------------------------
 */
bool CS_CheckResTableNameMatch(const char *Name, uint16 TableIdx)
{
    UT_GenStub_SetupReturnBuffer(CS_CheckResTableNameMatch, bool);

    UT_GenStub_AddParam(CS_CheckResTableNameMatch, const char *, Name);
    UT_GenStub_AddParam(CS_CheckResTableNameMatch, uint16, TableIdx);

    UT_GenStub_Execute(CS_CheckResTableNameMatch, Basic, NULL);

    return UT_GenStub_GetReturnValue(CS_CheckResTableNameMatch, bool);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CS_FindEnabledAppEntry()
 * ----------------------------------------------------
 */
CS_Res_App_Table_Entry_t *CS_FindEnabledAppEntry(void)
{
    UT_GenStub_SetupReturnBuffer(CS_FindEnabledAppEntry, CS_Res_App_Table_Entry_t *);

    UT_GenStub_Execute(CS_FindEnabledAppEntry, Basic, NULL);

    return UT_GenStub_GetReturnValue(CS_FindEnabledAppEntry, CS_Res_App_Table_Entry_t *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CS_FindEnabledEepromEntry()
 * ----------------------------------------------------
 */
CS_Res_EepromMemory_Table_Entry_t *CS_FindEnabledEepromEntry(void)
{
    UT_GenStub_SetupReturnBuffer(CS_FindEnabledEepromEntry, CS_Res_EepromMemory_Table_Entry_t *);

    UT_GenStub_Execute(CS_FindEnabledEepromEntry, Basic, NULL);

    return UT_GenStub_GetReturnValue(CS_FindEnabledEepromEntry, CS_Res_EepromMemory_Table_Entry_t *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CS_FindEnabledMemoryEntry()
 * ----------------------------------------------------
 */
CS_Res_EepromMemory_Table_Entry_t *CS_FindEnabledMemoryEntry(void)
{
    UT_GenStub_SetupReturnBuffer(CS_FindEnabledMemoryEntry, CS_Res_EepromMemory_Table_Entry_t *);

    UT_GenStub_Execute(CS_FindEnabledMemoryEntry, Basic, NULL);

    return UT_GenStub_GetReturnValue(CS_FindEnabledMemoryEntry, CS_Res_EepromMemory_Table_Entry_t *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CS_FindEnabledTablesEntry()
 * ----------------------------------------------------
 */
CS_Res_Tables_Table_Entry_t *CS_FindEnabledTablesEntry(void)
{
    UT_GenStub_SetupReturnBuffer(CS_FindEnabledTablesEntry, CS_Res_Tables_Table_Entry_t *);

    UT_GenStub_Execute(CS_FindEnabledTablesEntry, Basic, NULL);

    return UT_GenStub_GetReturnValue(CS_FindEnabledTablesEntry, CS_Res_Tables_Table_Entry_t *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CS_GetAppDefTblEntryByName()
 * ----------------------------------------------------
 */
bool CS_GetAppDefTblEntryByName(CS_Def_App_Table_Entry_t **EntryPtr, const char *Name)
{
    UT_GenStub_SetupReturnBuffer(CS_GetAppDefTblEntryByName, bool);

    UT_GenStub_AddParam(CS_GetAppDefTblEntryByName, CS_Def_App_Table_Entry_t **, EntryPtr);
    UT_GenStub_AddParam(CS_GetAppDefTblEntryByName, const char *, Name);

    UT_GenStub_Execute(CS_GetAppDefTblEntryByName, Basic, NULL);

    return UT_GenStub_GetReturnValue(CS_GetAppDefTblEntryByName, bool);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CS_GetAppResTblEntryByName()
 * ----------------------------------------------------
 */
bool CS_GetAppResTblEntryByName(CS_Res_App_Table_Entry_t **EntryPtr, const char *Name)
{
    UT_GenStub_SetupReturnBuffer(CS_GetAppResTblEntryByName, bool);

    UT_GenStub_AddParam(CS_GetAppResTblEntryByName, CS_Res_App_Table_Entry_t **, EntryPtr);
    UT_GenStub_AddParam(CS_GetAppResTblEntryByName, const char *, Name);

    UT_GenStub_Execute(CS_GetAppResTblEntryByName, Basic, NULL);

    return UT_GenStub_GetReturnValue(CS_GetAppResTblEntryByName, bool);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CS_GetDefEntryAddr()
 * ----------------------------------------------------
 */
void *CS_GetDefEntryAddr(CS_TableWrapper_t *tw, uint16 EntryIdx)
{
    UT_GenStub_SetupReturnBuffer(CS_GetDefEntryAddr, void *);

    UT_GenStub_AddParam(CS_GetDefEntryAddr, CS_TableWrapper_t *, tw);
    UT_GenStub_AddParam(CS_GetDefEntryAddr, uint16, EntryIdx);

    UT_GenStub_Execute(CS_GetDefEntryAddr, Basic, NULL);

    return UT_GenStub_GetReturnValue(CS_GetDefEntryAddr, void *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CS_GetResEntryAddr()
 * ----------------------------------------------------
 */
void *CS_GetResEntryAddr(CS_TableWrapper_t *tw, uint16 EntryIdx)
{
    UT_GenStub_SetupReturnBuffer(CS_GetResEntryAddr, void *);

    UT_GenStub_AddParam(CS_GetResEntryAddr, CS_TableWrapper_t *, tw);
    UT_GenStub_AddParam(CS_GetResEntryAddr, uint16, EntryIdx);

    UT_GenStub_Execute(CS_GetResEntryAddr, Basic, NULL);

    return UT_GenStub_GetReturnValue(CS_GetResEntryAddr, void *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CS_GetTableDefTblEntryByName()
 * ----------------------------------------------------
 */
bool CS_GetTableDefTblEntryByName(CS_Def_Tables_Table_Entry_t **EntryPtr, const char *Name)
{
    UT_GenStub_SetupReturnBuffer(CS_GetTableDefTblEntryByName, bool);

    UT_GenStub_AddParam(CS_GetTableDefTblEntryByName, CS_Def_Tables_Table_Entry_t **, EntryPtr);
    UT_GenStub_AddParam(CS_GetTableDefTblEntryByName, const char *, Name);

    UT_GenStub_Execute(CS_GetTableDefTblEntryByName, Basic, NULL);

    return UT_GenStub_GetReturnValue(CS_GetTableDefTblEntryByName, bool);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CS_GetTableResTblEntryByName()
 * ----------------------------------------------------
 */
bool CS_GetTableResTblEntryByName(CS_Res_Tables_Table_Entry_t **EntryPtr, const char *Name)
{
    UT_GenStub_SetupReturnBuffer(CS_GetTableResTblEntryByName, bool);

    UT_GenStub_AddParam(CS_GetTableResTblEntryByName, CS_Res_Tables_Table_Entry_t **, EntryPtr);
    UT_GenStub_AddParam(CS_GetTableResTblEntryByName, const char *, Name);

    UT_GenStub_Execute(CS_GetTableResTblEntryByName, Basic, NULL);

    return UT_GenStub_GetReturnValue(CS_GetTableResTblEntryByName, bool);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CS_GetTableTypeAsString()
 * ----------------------------------------------------
 */
const char *CS_GetTableTypeAsString(const CS_TableWrapper_t *Tbl)
{
    UT_GenStub_SetupReturnBuffer(CS_GetTableTypeAsString, const char *);

    UT_GenStub_AddParam(CS_GetTableTypeAsString, const CS_TableWrapper_t *, Tbl);

    UT_GenStub_Execute(CS_GetTableTypeAsString, Basic, NULL);

    return UT_GenStub_GetReturnValue(CS_GetTableTypeAsString, const char *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CS_GoToNextTable()
 * ----------------------------------------------------
 */
void CS_GoToNextTable(void)
{
    UT_GenStub_Execute(CS_GoToNextTable, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CS_HandleRoutineTableUpdates()
 * ----------------------------------------------------
 */
CFE_Status_t CS_HandleRoutineTableUpdates(void)
{
    UT_GenStub_SetupReturnBuffer(CS_HandleRoutineTableUpdates, CFE_Status_t);

    UT_GenStub_Execute(CS_HandleRoutineTableUpdates, Basic, NULL);

    return UT_GenStub_GetReturnValue(CS_HandleRoutineTableUpdates, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CS_InitializeDefaultTables()
 * ----------------------------------------------------
 */
void CS_InitializeDefaultTables(void)
{
    UT_GenStub_Execute(CS_InitializeDefaultTables, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CS_ResetTablesTblResultEntry()
 * ----------------------------------------------------
 */
void CS_ResetTablesTblResultEntry(CS_Res_Tables_Table_Entry_t *TablesTblResultEntry)
{
    UT_GenStub_AddParam(CS_ResetTablesTblResultEntry, CS_Res_Tables_Table_Entry_t *, TablesTblResultEntry);

    UT_GenStub_Execute(CS_ResetTablesTblResultEntry, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CS_SetDefEntryState()
 * ----------------------------------------------------
 */
CS_ChecksumState_Enum_t CS_SetDefEntryState(CS_TableWrapper_t *tw, void *EntryPtr, CS_ChecksumState_Enum_t NewState)
{
    UT_GenStub_SetupReturnBuffer(CS_SetDefEntryState, CS_ChecksumState_Enum_t);

    UT_GenStub_AddParam(CS_SetDefEntryState, CS_TableWrapper_t *, tw);
    UT_GenStub_AddParam(CS_SetDefEntryState, void *, EntryPtr);
    UT_GenStub_AddParam(CS_SetDefEntryState, CS_ChecksumState_Enum_t, NewState);

    UT_GenStub_Execute(CS_SetDefEntryState, Basic, NULL);

    return UT_GenStub_GetReturnValue(CS_SetDefEntryState, CS_ChecksumState_Enum_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CS_ZeroAppTempValues()
 * ----------------------------------------------------
 */
void CS_ZeroAppTempValues(void)
{
    UT_GenStub_Execute(CS_ZeroAppTempValues, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CS_ZeroCfeCoreTempValues()
 * ----------------------------------------------------
 */
void CS_ZeroCfeCoreTempValues(void)
{
    UT_GenStub_Execute(CS_ZeroCfeCoreTempValues, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CS_ZeroEepromTempValues()
 * ----------------------------------------------------
 */
void CS_ZeroEepromTempValues(void)
{
    UT_GenStub_Execute(CS_ZeroEepromTempValues, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CS_ZeroMemoryTempValues()
 * ----------------------------------------------------
 */
void CS_ZeroMemoryTempValues(void)
{
    UT_GenStub_Execute(CS_ZeroMemoryTempValues, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CS_ZeroOSTempValues()
 * ----------------------------------------------------
 */
void CS_ZeroOSTempValues(void)
{
    UT_GenStub_Execute(CS_ZeroOSTempValues, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CS_ZeroTablesTempValues()
 * ----------------------------------------------------
 */
void CS_ZeroTablesTempValues(void)
{
    UT_GenStub_Execute(CS_ZeroTablesTempValues, Basic, NULL);
}
