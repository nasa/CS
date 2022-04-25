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

/* UT includes */
#include "uttest.h"
#include "utassert.h"
#include "utstubs.h"

void CS_ZeroEepromTempValues(void)
{
    UT_DEFAULT_IMPL(CS_ZeroEepromTempValues);
}

void CS_ZeroMemoryTempValues(void)
{
    UT_DEFAULT_IMPL(CS_ZeroMemoryTempValues);
}

void CS_ZeroTablesTempValues(void)
{
    UT_DEFAULT_IMPL(CS_ZeroTablesTempValues);
}

void CS_ZeroAppTempValues(void)
{
    UT_DEFAULT_IMPL(CS_ZeroAppTempValues);
}

void CS_ZeroCfeCoreTempValues(void)
{
    UT_DEFAULT_IMPL(CS_ZeroCfeCoreTempValues);
}

void CS_ZeroOSTempValues(void)
{
    UT_DEFAULT_IMPL(CS_ZeroOSTempValues);
}

void CS_InitializeDefaultTables(void)
{
    UT_DEFAULT_IMPL(CS_InitializeDefaultTables);
}

void CS_GoToNextTable(void)
{
    UT_DEFAULT_IMPL(CS_GoToNextTable);
}

bool CS_GetTableResTblEntryByName(CS_Res_Tables_Table_Entry_t **EntryPtr, const char *Name)
{
    UT_Stub_RegisterContext(UT_KEY(CS_GetTableResTblEntryByName), EntryPtr);
    UT_Stub_RegisterContext(UT_KEY(CS_GetTableResTblEntryByName), Name);

    return UT_DEFAULT_IMPL(CS_GetTableResTblEntryByName);
}

bool CS_GetTableDefTblEntryByName(CS_Def_Tables_Table_Entry_t **EntryPtr, const char *Name)
{
    UT_Stub_RegisterContext(UT_KEY(CS_GetTableDefTblEntryByName), EntryPtr);
    UT_Stub_RegisterContext(UT_KEY(CS_GetTableDefTblEntryByName), Name);

    return UT_DEFAULT_IMPL(CS_GetTableDefTblEntryByName);
}

bool CS_GetAppResTblEntryByName(CS_Res_App_Table_Entry_t **EntryPtr, const char *Name)
{
    UT_Stub_RegisterContext(UT_KEY(CS_GetAppResTblEntryByName), EntryPtr);
    UT_Stub_RegisterContext(UT_KEY(CS_GetAppResTblEntryByName), Name);

    return UT_DEFAULT_IMPL(CS_GetAppResTblEntryByName);
}

bool CS_GetAppDefTblEntryByName(CS_Def_App_Table_Entry_t **EntryPtr, const char *Name)
{
    UT_Stub_RegisterContext(UT_KEY(CS_GetAppDefTblEntryByName), EntryPtr);
    UT_Stub_RegisterContext(UT_KEY(CS_GetAppDefTblEntryByName), Name);

    return UT_DEFAULT_IMPL(CS_GetAppDefTblEntryByName);
}

bool CS_FindEnabledEepromEntry(uint16 *EnabledEntry)
{
    UT_Stub_RegisterContext(UT_KEY(CS_FindEnabledEepromEntry), EnabledEntry);
    return UT_DEFAULT_IMPL(CS_FindEnabledEepromEntry);
}

bool CS_FindEnabledMemoryEntry(uint16 *EnabledEntry)
{
    UT_Stub_RegisterContext(UT_KEY(CS_FindEnabledMemoryEntry), EnabledEntry);
    return UT_DEFAULT_IMPL(CS_FindEnabledMemoryEntry);
}

bool CS_FindEnabledTablesEntry(uint16 *EnabledEntry)
{
    UT_Stub_RegisterContext(UT_KEY(CS_FindEnabledTablesEntry), EnabledEntry);
    return UT_DEFAULT_IMPL(CS_FindEnabledTablesEntry);
}

bool CS_FindEnabledAppEntry(uint16 *EnabledEntry)
{
    UT_Stub_RegisterContext(UT_KEY(CS_FindEnabledAppEntry), EnabledEntry);
    return UT_DEFAULT_IMPL(CS_FindEnabledAppEntry);
}

bool CS_VerifyCmdLength(const CFE_MSG_Message_t *Msg, size_t ExpectedLength)
{
    UT_Stub_RegisterContext(UT_KEY(CS_VerifyCmdLength), Msg);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CS_VerifyCmdLength), ExpectedLength);

    return UT_DEFAULT_IMPL(CS_VerifyCmdLength);
}

bool CS_BackgroundOS(void)
{
    return UT_DEFAULT_IMPL(CS_BackgroundOS);
}

bool CS_BackgroundCfeCore(void)
{
    return UT_DEFAULT_IMPL(CS_BackgroundCfeCore);
}

bool CS_BackgroundEeprom(void)
{
    return UT_DEFAULT_IMPL(CS_BackgroundEeprom);
}

bool CS_BackgroundMemory(void)
{
    return UT_DEFAULT_IMPL(CS_BackgroundMemory);
}

bool CS_BackgroundTables(void)
{
    return UT_DEFAULT_IMPL(CS_BackgroundTables);
}

bool CS_BackgroundApp(void)
{
    return UT_DEFAULT_IMPL(CS_BackgroundApp);
}

void CS_ResetTablesTblResultEntry(CS_Res_Tables_Table_Entry_t *TablesTblResultEntry)
{
    UT_Stub_RegisterContext(UT_KEY(CS_ResetTablesTblResultEntry), TablesTblResultEntry);

    UT_DEFAULT_IMPL(CS_ResetTablesTblResultEntry);
}

int32 CS_HandleRoutineTableUpdates(void)
{
    return UT_DEFAULT_IMPL(CS_HandleRoutineTableUpdates);
}

int32 CS_AttemptTableReshare(CS_Res_Tables_Table_Entry_t *ResultsEntry, CFE_TBL_Handle_t *LocalTblHandle,
                             CFE_TBL_Info_t *TblInfo, cpuaddr *LocalAddress, int32 *ResultGetInfo)
{
    UT_Stub_RegisterContext(UT_KEY(CS_AttemptTableReshare), ResultsEntry);
    UT_Stub_RegisterContext(UT_KEY(CS_AttemptTableReshare), LocalTblHandle);
    UT_Stub_RegisterContext(UT_KEY(CS_AttemptTableReshare), TblInfo);
    UT_Stub_RegisterContext(UT_KEY(CS_AttemptTableReshare), LocalAddress);
    UT_Stub_RegisterContext(UT_KEY(CS_AttemptTableReshare), ResultGetInfo);

    return UT_DEFAULT_IMPL(CS_AttemptTableReshare);
}

bool CS_CheckRecomputeOneshot(void)
{
    return UT_DEFAULT_IMPL(CS_CheckRecomputeOneshot);
}
