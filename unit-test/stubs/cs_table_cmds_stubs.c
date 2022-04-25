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

#include "cs_table_cmds.h"

/* UT includes */
#include "uttest.h"
#include "utassert.h"
#include "utstubs.h"

void CS_DisableTablesCmd(const CS_NoArgsCmd_t *CmdPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_DisableTablesCmd), CmdPtr);
    UT_DEFAULT_IMPL(CS_DisableTablesCmd);
}

void CS_EnableTablesCmd(const CS_NoArgsCmd_t *CmdPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_EnableTablesCmd), CmdPtr);
    UT_DEFAULT_IMPL(CS_EnableTablesCmd);
}

void CS_ReportBaselineTablesCmd(const CS_TableNameCmd_t *CmdPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_ReportBaselineTablesCmd), CmdPtr);
    UT_DEFAULT_IMPL(CS_ReportBaselineTablesCmd);
}

void CS_RecomputeBaselineTablesCmd(const CS_TableNameCmd_t *CmdPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_RecomputeBaselineTablesCmd), CmdPtr);
    UT_DEFAULT_IMPL(CS_RecomputeBaselineTablesCmd);
}

void CS_DisableNameTablesCmd(const CS_TableNameCmd_t *CmdPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_DisableNameTablesCmd), CmdPtr);
    UT_DEFAULT_IMPL(CS_DisableNameTablesCmd);
}

void CS_EnableNameTablesCmd(const CS_TableNameCmd_t *CmdPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_EnableNameTablesCmd), CmdPtr);
    UT_DEFAULT_IMPL(CS_EnableNameTablesCmd);
}
