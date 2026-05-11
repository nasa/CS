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
 * Auto-Generated stub implementations for functions defined in cs_compute header
 */

#include "cs_compute.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for CS_ComputeApp()
 * ----------------------------------------------------
 */
CFE_Status_t CS_ComputeApp(CS_Res_App_Table_Entry_t *ResultsEntry, uint32 *ComputedCSValue, bool *DoneWithEntry)
{
    UT_GenStub_SetupReturnBuffer(CS_ComputeApp, CFE_Status_t);

    UT_GenStub_AddParam(CS_ComputeApp, CS_Res_App_Table_Entry_t *, ResultsEntry);
    UT_GenStub_AddParam(CS_ComputeApp, uint32 *, ComputedCSValue);
    UT_GenStub_AddParam(CS_ComputeApp, bool *, DoneWithEntry);

    UT_GenStub_Execute(CS_ComputeApp, Basic, NULL);

    return UT_GenStub_GetReturnValue(CS_ComputeApp, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CS_ComputeEepromMemory()
 * ----------------------------------------------------
 */
CFE_Status_t
CS_ComputeEepromMemory(CS_Res_EepromMemory_Table_Entry_t *ResultsEntry, uint32 *ComputedCSValue, bool *DoneWithEntry)
{
    UT_GenStub_SetupReturnBuffer(CS_ComputeEepromMemory, CFE_Status_t);

    UT_GenStub_AddParam(CS_ComputeEepromMemory, CS_Res_EepromMemory_Table_Entry_t *, ResultsEntry);
    UT_GenStub_AddParam(CS_ComputeEepromMemory, uint32 *, ComputedCSValue);
    UT_GenStub_AddParam(CS_ComputeEepromMemory, bool *, DoneWithEntry);

    UT_GenStub_Execute(CS_ComputeEepromMemory, Basic, NULL);

    return UT_GenStub_GetReturnValue(CS_ComputeEepromMemory, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CS_ComputeTables()
 * ----------------------------------------------------
 */
CFE_Status_t CS_ComputeTables(CS_Res_Tables_Table_Entry_t *ResultsEntry, uint32 *ComputedCSValue, bool *DoneWithEntry)
{
    UT_GenStub_SetupReturnBuffer(CS_ComputeTables, CFE_Status_t);

    UT_GenStub_AddParam(CS_ComputeTables, CS_Res_Tables_Table_Entry_t *, ResultsEntry);
    UT_GenStub_AddParam(CS_ComputeTables, uint32 *, ComputedCSValue);
    UT_GenStub_AddParam(CS_ComputeTables, bool *, DoneWithEntry);

    UT_GenStub_Execute(CS_ComputeTables, Basic, NULL);

    return UT_GenStub_GetReturnValue(CS_ComputeTables, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CS_OneShotChildTask()
 * ----------------------------------------------------
 */
void CS_OneShotChildTask(void)
{
    UT_GenStub_Execute(CS_OneShotChildTask, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CS_RecomputeAppChildTask()
 * ----------------------------------------------------
 */
void CS_RecomputeAppChildTask(void)
{
    UT_GenStub_Execute(CS_RecomputeAppChildTask, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CS_RecomputeEepromMemoryChildTask()
 * ----------------------------------------------------
 */
void CS_RecomputeEepromMemoryChildTask(void)
{
    UT_GenStub_Execute(CS_RecomputeEepromMemoryChildTask, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CS_RecomputeTablesChildTask()
 * ----------------------------------------------------
 */
void CS_RecomputeTablesChildTask(void)
{
    UT_GenStub_Execute(CS_RecomputeTablesChildTask, Basic, NULL);
}
