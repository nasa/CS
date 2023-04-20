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

#include "cs_compute.h"

/* UT includes */
#include "uttest.h"
#include "utassert.h"
#include "utstubs.h"

CFE_Status_t CS_ComputeEepromMemory(CS_Res_EepromMemory_Table_Entry_t *ResultsEntry, uint32 *ComputedCSValue,
                                    bool *DoneWithEntry)
{
    UT_Stub_RegisterContext(UT_KEY(CS_ComputeEepromMemory), ResultsEntry);
    UT_Stub_RegisterContext(UT_KEY(CS_ComputeEepromMemory), ComputedCSValue);
    UT_Stub_RegisterContext(UT_KEY(CS_ComputeEepromMemory), DoneWithEntry);

    return UT_DEFAULT_IMPL(CS_ComputeEepromMemory);
}

CFE_Status_t CS_ComputeTables(CS_Res_Tables_Table_Entry_t *ResultsEntry, uint32 *ComputedCSValue, bool *DoneWithEntry)
{
    UT_Stub_RegisterContext(UT_KEY(CS_ComputeTables), ResultsEntry);
    UT_Stub_RegisterContext(UT_KEY(CS_ComputeTables), ComputedCSValue);
    UT_Stub_RegisterContext(UT_KEY(CS_ComputeTables), DoneWithEntry);

    return UT_DEFAULT_IMPL(CS_ComputeTables);
}

CFE_Status_t CS_ComputeApp(CS_Res_App_Table_Entry_t *ResultsEntry, uint32 *ComputedCSValue, bool *DoneWithEntry)
{
    UT_Stub_RegisterContext(UT_KEY(CS_ComputeApp), ResultsEntry);
    UT_Stub_RegisterContext(UT_KEY(CS_ComputeApp), ComputedCSValue);
    UT_Stub_RegisterContext(UT_KEY(CS_ComputeApp), DoneWithEntry);

    return UT_DEFAULT_IMPL(CS_ComputeApp);
}

void CS_RecomputeEepromMemoryChildTask(void)
{
    UT_DEFAULT_IMPL(CS_RecomputeEepromMemoryChildTask);
}

void CS_RecomputeTablesChildTask(void)
{
    UT_DEFAULT_IMPL(CS_RecomputeTablesChildTask);
}

void CS_RecomputeAppChildTask(void)
{
    UT_DEFAULT_IMPL(CS_RecomputeAppChildTask);
}

void CS_OneShotChildTask(void)
{
    UT_DEFAULT_IMPL(CS_OneShotChildTask);
}
