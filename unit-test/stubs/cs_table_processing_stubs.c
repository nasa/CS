/************************************************************************
 * NASA Docket No. GSC-18,915-1, and identified as “cFS Checksum
 * Application version 2.5.0”
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

#include "cs_tbldefs.h"

/* UT includes */
#include "uttest.h"
#include "utassert.h"
#include "utstubs.h"

int32 CS_ValidateEepromChecksumDefinitionTable(void *TblPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_ValidateEepromChecksumDefinitionTable), TblPtr);

    return UT_DEFAULT_IMPL(CS_ValidateEepromChecksumDefinitionTable);
}

int32 CS_ValidateMemoryChecksumDefinitionTable(void *TblPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_ValidateMemoryChecksumDefinitionTable), TblPtr);

    return UT_DEFAULT_IMPL(CS_ValidateMemoryChecksumDefinitionTable);
}

int32 CS_ValidateTablesChecksumDefinitionTable(void *TblPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_ValidateTablesChecksumDefinitionTable), TblPtr);

    return UT_DEFAULT_IMPL(CS_ValidateTablesChecksumDefinitionTable);
}

int32 CS_ValidateAppChecksumDefinitionTable(void *TblPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_ValidateAppChecksumDefinitionTable), TblPtr);

    return UT_DEFAULT_IMPL(CS_ValidateAppChecksumDefinitionTable);
}

void CS_ProcessNewEepromMemoryDefinitionTable(const CS_Def_EepromMemory_Table_Entry_t *DefinitionTblPtr,
                                              const CS_Res_EepromMemory_Table_Entry_t *ResultsTblPtr,
                                              const uint16 NumEntries, const uint16 Table)
{
    UT_Stub_RegisterContext(UT_KEY(CS_ProcessNewEepromMemoryDefinitionTable), DefinitionTblPtr);
    UT_Stub_RegisterContext(UT_KEY(CS_ProcessNewEepromMemoryDefinitionTable), ResultsTblPtr);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CS_ProcessNewEepromMemoryDefinitionTable), Table);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CS_ProcessNewEepromMemoryDefinitionTable), NumEntries);

    UT_DEFAULT_IMPL(CS_ProcessNewEepromMemoryDefinitionTable);
}

void CS_ProcessNewTablesDefinitionTable(const CS_Def_Tables_Table_Entry_t *DefinitionTblPtr,
                                        const CS_Res_Tables_Table_Entry_t *ResultsTblPtr)

{
    UT_Stub_RegisterContext(UT_KEY(CS_ProcessNewTablesDefinitionTable), DefinitionTblPtr);
    UT_Stub_RegisterContext(UT_KEY(CS_ProcessNewTablesDefinitionTable), ResultsTblPtr);

    UT_DEFAULT_IMPL(CS_ProcessNewTablesDefinitionTable);
}

void CS_ProcessNewAppDefinitionTable(const CS_Def_App_Table_Entry_t *DefinitionTblPtr,
                                     const CS_Res_App_Table_Entry_t *ResultsTblPtr)

{
    UT_Stub_RegisterContext(UT_KEY(CS_ProcessNewAppDefinitionTable), DefinitionTblPtr);
    UT_Stub_RegisterContext(UT_KEY(CS_ProcessNewAppDefinitionTable), ResultsTblPtr);

    UT_DEFAULT_IMPL(CS_ProcessNewAppDefinitionTable);
}

int32 CS_TableInit(CFE_TBL_Handle_t *DefinitionTableHandle, CFE_TBL_Handle_t *ResultsTableHandle,
                   void *DefinitionTblPtr, void *ResultsTblPtr, const uint16 Table, const char *DefinitionTableName,
                   const char *ResultsTableName, const uint16 NumEntries, const char *DefinitionTableFileName,
                   const void *DefaultDefTableAddress, const uint16 SizeofDefinitionTableEntry,
                   const uint16 SizeofResultsTableEntry, const CFE_TBL_CallbackFuncPtr_t CallBackFunction)

{
    UT_Stub_RegisterContext(UT_KEY(CS_TableInit), DefinitionTableHandle);
    UT_Stub_RegisterContext(UT_KEY(CS_TableInit), ResultsTableHandle);
    UT_Stub_RegisterContext(UT_KEY(CS_TableInit), DefinitionTblPtr);
    UT_Stub_RegisterContext(UT_KEY(CS_TableInit), ResultsTblPtr);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CS_TableInit), Table);
    UT_Stub_RegisterContext(UT_KEY(CS_TableInit), DefinitionTableName);
    UT_Stub_RegisterContext(UT_KEY(CS_TableInit), ResultsTableName);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CS_TableInit), NumEntries);
    UT_Stub_RegisterContext(UT_KEY(CS_TableInit), DefinitionTableFileName);
    UT_Stub_RegisterContext(UT_KEY(CS_TableInit), DefaultDefTableAddress);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CS_TableInit), SizeofDefinitionTableEntry);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CS_TableInit), SizeofResultsTableEntry);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CS_TableInit), CallBackFunction);

    return UT_DEFAULT_IMPL(CS_TableInit);
}

int32 CS_HandleTableUpdate(void *DefinitionTblPtr, void *ResultsTblPtr, const CFE_TBL_Handle_t DefinitionTableHandle,
                           const CFE_TBL_Handle_t ResultsTableHandle, const uint16 Table, const uint16 NumEntries)
{
    UT_Stub_RegisterContext(UT_KEY(CS_HandleTableUpdate), DefinitionTblPtr);
    UT_Stub_RegisterContext(UT_KEY(CS_HandleTableUpdate), ResultsTblPtr);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CS_HandleTableUpdate), DefinitionTableHandle);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CS_HandleTableUpdate), ResultsTableHandle);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CS_HandleTableUpdate), Table);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CS_HandleTableUpdate), NumEntries);

    return UT_DEFAULT_IMPL(CS_HandleTableUpdate);
}
