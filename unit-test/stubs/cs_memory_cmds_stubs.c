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

#include "cs_memory_cmds.h"

/* UT includes */
#include "uttest.h"
#include "utassert.h"
#include "utstubs.h"

void CS_DisableMemoryCmd(const CS_NoArgsCmd_t *CmdPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_DisableMemoryCmd), CmdPtr);
    UT_DEFAULT_IMPL(CS_DisableMemoryCmd);
}

void CS_EnableMemoryCmd(const CS_NoArgsCmd_t *CmdPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_EnableMemoryCmd), CmdPtr);
    UT_DEFAULT_IMPL(CS_EnableMemoryCmd);
}

void CS_ReportBaselineEntryIDMemoryCmd(const CS_EntryCmd_t *CmdPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_ReportBaselineEntryIDMemoryCmd), CmdPtr);
    UT_DEFAULT_IMPL(CS_ReportBaselineEntryIDMemoryCmd);
}

void CS_DisableEntryIDMemoryCmd(const CS_EntryCmd_t *CmdPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_DisableEntryIDMemoryCmd), CmdPtr);
    UT_DEFAULT_IMPL(CS_DisableEntryIDMemoryCmd);
}

void CS_RecomputeBaselineMemoryCmd(const CS_EntryCmd_t *CmdPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_RecomputeBaselineMemoryCmd), CmdPtr);
    UT_DEFAULT_IMPL(CS_RecomputeBaselineMemoryCmd);
}

void CS_EnableEntryIDMemoryCmd(const CS_EntryCmd_t *CmdPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_EnableEntryIDMemoryCmd), CmdPtr);
    UT_DEFAULT_IMPL(CS_EnableEntryIDMemoryCmd);
}

void CS_GetEntryIDMemoryCmd(const CS_GetEntryIDCmd_t *CmdPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_GetEntryIDMemoryCmd), CmdPtr);
    UT_DEFAULT_IMPL(CS_GetEntryIDMemoryCmd);
}
