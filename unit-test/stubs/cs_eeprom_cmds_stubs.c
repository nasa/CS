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

#include "cs_eeprom_cmds.h"

/* UT includes */
#include "uttest.h"
#include "utassert.h"
#include "utstubs.h"

void CS_DisableEepromCmd(const CS_NoArgsCmd_t *CmdPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_DisableEepromCmd), CmdPtr);
    UT_DEFAULT_IMPL(CS_DisableEepromCmd);
}

void CS_EnableEepromCmd(const CS_NoArgsCmd_t *CmdPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_EnableEepromCmd), CmdPtr);
    UT_DEFAULT_IMPL(CS_EnableEepromCmd);
}

void CS_ReportBaselineEntryIDEepromCmd(const CS_EntryCmd_t *CmdPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_ReportBaselineEntryIDEepromCmd), CmdPtr);
    UT_DEFAULT_IMPL(CS_ReportBaselineEntryIDEepromCmd);
}

void CS_DisableEntryIDEepromCmd(const CS_EntryCmd_t *CmdPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_DisableEntryIDEepromCmd), CmdPtr);
    UT_DEFAULT_IMPL(CS_DisableEntryIDEepromCmd);
}

void CS_RecomputeBaselineEepromCmd(const CS_EntryCmd_t *CmdPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_RecomputeBaselineEepromCmd), CmdPtr);
    UT_DEFAULT_IMPL(CS_RecomputeBaselineEepromCmd);
}

void CS_EnableEntryIDEepromCmd(const CS_EntryCmd_t *CmdPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_EnableEntryIDEepromCmd), CmdPtr);
    UT_DEFAULT_IMPL(CS_EnableEntryIDEepromCmd);
}

void CS_GetEntryIDEepromCmd(const CS_GetEntryIDCmd_t *CmdPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_GetEntryIDEepromCmd), CmdPtr);
    UT_DEFAULT_IMPL(CS_GetEntryIDEepromCmd);
}
