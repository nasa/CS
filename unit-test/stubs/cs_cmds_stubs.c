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

#include "cfe.h"
#include "cs_cmds.h"

/* UT includes */
#include "uttest.h"
#include "utassert.h"
#include "utstubs.h"

void CS_NoopCmd(const CS_NoArgsCmd_t *CmdPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_NoopCmd), CmdPtr);
    UT_DEFAULT_IMPL(CS_NoopCmd);
}

void CS_ResetCmd(const CS_NoArgsCmd_t *CmdPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_ResetCmd), CmdPtr);
    UT_DEFAULT_IMPL(CS_ResetCmd);
}

void CS_BackgroundCheckCycle(const CS_NoArgsCmd_t *CmdPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_BackgroundCheckCycle), CmdPtr);
    UT_DEFAULT_IMPL(CS_BackgroundCheckCycle);
}

void CS_DisableAllCSCmd(const CS_NoArgsCmd_t *CmdPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_DisableAllCSCmd), CmdPtr);
    UT_DEFAULT_IMPL(CS_DisableAllCSCmd);
}

void CS_EnableAllCSCmd(const CS_NoArgsCmd_t *CmdPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_EnableAllCSCmd), CmdPtr);
    UT_DEFAULT_IMPL(CS_EnableAllCSCmd);
}

void CS_DisableCfeCoreCmd(const CS_NoArgsCmd_t *CmdPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_DisableCfeCoreCmd), CmdPtr);
    UT_DEFAULT_IMPL(CS_DisableCfeCoreCmd);
}

void CS_EnableCfeCoreCmd(const CS_NoArgsCmd_t *CmdPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_EnableCfeCoreCmd), CmdPtr);
    UT_DEFAULT_IMPL(CS_EnableCfeCoreCmd);
}

void CS_DisableOSCmd(const CS_NoArgsCmd_t *CmdPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_DisableOSCmd), CmdPtr);
    UT_DEFAULT_IMPL(CS_DisableOSCmd);
}

void CS_EnableOSCmd(const CS_NoArgsCmd_t *CmdPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_EnableOSCmd), CmdPtr);
    UT_DEFAULT_IMPL(CS_EnableOSCmd);
}

void CS_ReportBaselineCfeCoreCmd(const CS_NoArgsCmd_t *CmdPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_ReportBaselineCfeCoreCmd), CmdPtr);
    UT_DEFAULT_IMPL(CS_ReportBaselineCfeCoreCmd);
}

void CS_ReportBaselineOSCmd(const CS_NoArgsCmd_t *CmdPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_ReportBaselineOSCmd), CmdPtr);
    UT_DEFAULT_IMPL(CS_ReportBaselineOSCmd);
}

void CS_RecomputeBaselineCfeCoreCmd(const CS_NoArgsCmd_t *CmdPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_RecomputeBaselineCfeCoreCmd), CmdPtr);
    UT_DEFAULT_IMPL(CS_RecomputeBaselineCfeCoreCmd);
}

void CS_RecomputeBaselineOSCmd(const CS_NoArgsCmd_t *CmdPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_RecomputeBaselineOSCmd), CmdPtr);
    UT_DEFAULT_IMPL(CS_RecomputeBaselineOSCmd);
}

void CS_OneShotCmd(const CS_OneShotCmd_t *CmdPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_OneShotCmd), CmdPtr);
    UT_DEFAULT_IMPL(CS_OneShotCmd);
}

void CS_CancelOneShotCmd(const CS_NoArgsCmd_t *CmdPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_CancelOneShotCmd), CmdPtr);
    UT_DEFAULT_IMPL(CS_CancelOneShotCmd);
}
