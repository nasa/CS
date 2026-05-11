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
 * Auto-Generated stub implementations for functions defined in cs_table_processing header
 */

#include "cs_table_processing.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for CS_CallTableUpdateHandler()
 * ----------------------------------------------------
 */
void CS_CallTableUpdateHandler(CS_TableWrapper_t *tw)
{
    UT_GenStub_AddParam(CS_CallTableUpdateHandler, CS_TableWrapper_t *, tw);

    UT_GenStub_Execute(CS_CallTableUpdateHandler, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CS_HandleTableUpdate()
 * ----------------------------------------------------
 */
CFE_Status_t CS_HandleTableUpdate(CS_TableWrapper_t *tw)
{
    UT_GenStub_SetupReturnBuffer(CS_HandleTableUpdate, CFE_Status_t);

    UT_GenStub_AddParam(CS_HandleTableUpdate, CS_TableWrapper_t *, tw);

    UT_GenStub_Execute(CS_HandleTableUpdate, Basic, NULL);

    return UT_GenStub_GetReturnValue(CS_HandleTableUpdate, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CS_ProcessNewAppDefinitionTable()
 * ----------------------------------------------------
 */
void CS_ProcessNewAppDefinitionTable(CS_TableWrapper_t *tw)
{
    UT_GenStub_AddParam(CS_ProcessNewAppDefinitionTable, CS_TableWrapper_t *, tw);

    UT_GenStub_Execute(CS_ProcessNewAppDefinitionTable, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CS_ProcessNewEepromMemoryDefinitionTable()
 * ----------------------------------------------------
 */
void CS_ProcessNewEepromMemoryDefinitionTable(CS_TableWrapper_t *tw)
{
    UT_GenStub_AddParam(CS_ProcessNewEepromMemoryDefinitionTable, CS_TableWrapper_t *, tw);

    UT_GenStub_Execute(CS_ProcessNewEepromMemoryDefinitionTable, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CS_ProcessNewTablesDefinitionTable()
 * ----------------------------------------------------
 */
void CS_ProcessNewTablesDefinitionTable(CS_TableWrapper_t *tw)
{
    UT_GenStub_AddParam(CS_ProcessNewTablesDefinitionTable, CS_TableWrapper_t *, tw);

    UT_GenStub_Execute(CS_ProcessNewTablesDefinitionTable, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CS_TableInit()
 * ----------------------------------------------------
 */
CFE_Status_t
CS_TableInit(CS_TableWrapper_t *tw, const char *DefinitionTableFileName, CFE_TBL_CallbackFuncPtr_t CallBackFunction)
{
    UT_GenStub_SetupReturnBuffer(CS_TableInit, CFE_Status_t);

    UT_GenStub_AddParam(CS_TableInit, CS_TableWrapper_t *, tw);
    UT_GenStub_AddParam(CS_TableInit, const char *, DefinitionTableFileName);
    UT_GenStub_AddParam(CS_TableInit, CFE_TBL_CallbackFuncPtr_t, CallBackFunction);

    UT_GenStub_Execute(CS_TableInit, Basic, NULL);

    return UT_GenStub_GetReturnValue(CS_TableInit, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CS_ValidateAppChecksumDefinitionTable()
 * ----------------------------------------------------
 */
CFE_Status_t CS_ValidateAppChecksumDefinitionTable(void *TblPtr)
{
    UT_GenStub_SetupReturnBuffer(CS_ValidateAppChecksumDefinitionTable, CFE_Status_t);

    UT_GenStub_AddParam(CS_ValidateAppChecksumDefinitionTable, void *, TblPtr);

    UT_GenStub_Execute(CS_ValidateAppChecksumDefinitionTable, Basic, NULL);

    return UT_GenStub_GetReturnValue(CS_ValidateAppChecksumDefinitionTable, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CS_ValidateEepromChecksumDefinitionTable()
 * ----------------------------------------------------
 */
CFE_Status_t CS_ValidateEepromChecksumDefinitionTable(void *TblPtr)
{
    UT_GenStub_SetupReturnBuffer(CS_ValidateEepromChecksumDefinitionTable, CFE_Status_t);

    UT_GenStub_AddParam(CS_ValidateEepromChecksumDefinitionTable, void *, TblPtr);

    UT_GenStub_Execute(CS_ValidateEepromChecksumDefinitionTable, Basic, NULL);

    return UT_GenStub_GetReturnValue(CS_ValidateEepromChecksumDefinitionTable, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CS_ValidateMemoryChecksumDefinitionTable()
 * ----------------------------------------------------
 */
CFE_Status_t CS_ValidateMemoryChecksumDefinitionTable(void *TblPtr)
{
    UT_GenStub_SetupReturnBuffer(CS_ValidateMemoryChecksumDefinitionTable, CFE_Status_t);

    UT_GenStub_AddParam(CS_ValidateMemoryChecksumDefinitionTable, void *, TblPtr);

    UT_GenStub_Execute(CS_ValidateMemoryChecksumDefinitionTable, Basic, NULL);

    return UT_GenStub_GetReturnValue(CS_ValidateMemoryChecksumDefinitionTable, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CS_ValidateTablesChecksumDefinitionTable()
 * ----------------------------------------------------
 */
CFE_Status_t CS_ValidateTablesChecksumDefinitionTable(void *TblPtr)
{
    UT_GenStub_SetupReturnBuffer(CS_ValidateTablesChecksumDefinitionTable, CFE_Status_t);

    UT_GenStub_AddParam(CS_ValidateTablesChecksumDefinitionTable, void *, TblPtr);

    UT_GenStub_Execute(CS_ValidateTablesChecksumDefinitionTable, Basic, NULL);

    return UT_GenStub_GetReturnValue(CS_ValidateTablesChecksumDefinitionTable, CFE_Status_t);
}
