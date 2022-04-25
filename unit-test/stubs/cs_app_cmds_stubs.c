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

#include "cs_app_cmds.h"

/* UT includes */
#include "uttest.h"
#include "utassert.h"
#include "utstubs.h"

void CS_DisableAppCmd(const CS_NoArgsCmd_t *CmdPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_DisableAppCmd), CmdPtr);
    UT_DEFAULT_IMPL(CS_DisableAppCmd);
}

void CS_EnableAppCmd(const CS_NoArgsCmd_t *CmdPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_EnableAppCmd), CmdPtr);
    UT_DEFAULT_IMPL(CS_EnableAppCmd);
}

void CS_ReportBaselineAppCmd(const CS_AppNameCmd_t *CmdPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_ReportBaselineAppCmd), CmdPtr);
    UT_DEFAULT_IMPL(CS_ReportBaselineAppCmd);
}

void CS_RecomputeBaselineAppCmd(const CS_AppNameCmd_t *CmdPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_RecomputeBaselineAppCmd), CmdPtr);
    UT_DEFAULT_IMPL(CS_RecomputeBaselineAppCmd);
}

void CS_DisableNameAppCmd(const CS_AppNameCmd_t *CmdPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_DisableNameAppCmd), CmdPtr);
    UT_DEFAULT_IMPL(CS_DisableNameAppCmd);
}

void CS_EnableNameAppCmd(const CS_AppNameCmd_t *CmdPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_EnableNameAppCmd), CmdPtr);
    UT_DEFAULT_IMPL(CS_EnableNameAppCmd);
}
