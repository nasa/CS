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
 *   Specification for the CFS generic cmds
 */
#ifndef CS_CMDS_H
#define CS_CMDS_H

/**************************************************************************
 **
 ** Include section
 **
 **************************************************************************/
#include "cfe.h"
#include "cs_msg.h"

/**
 * \brief Process housekeeping request
 *
 *  \par Description
 *       Processes an on-board housekeeping request message.
 *
 *  \par Assumptions, External Events, and Notes:
 *       This command does not affect the command execution counter
 *
 *  \param[in] CmdPtr Command pointer, verified non-null in CS_AppMain
 */
CFE_Status_t CS_SendHkCmd(const CS_SendHkCmd_t *CmdPtr);

/**
 * \brief Process noop command
 *
 *  \par Description
 *       Processes a noop ground command.
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param[in] CmdPtr Command pointer, verified non-null in CS_AppMain
 *
 *  \sa #CS_NOOP_CC
 */
CFE_Status_t CS_NoopCmd(const CS_NoopCmd_t *CmdPtr);

/**
 * \brief Process reset counters command
 *
 *  \par Description
 *       Processes a reset counters ground command which will reset
 *       the checksum commmand error and command execution counters
 *       to zero. It also resets all checksum error counters and
 *       the passes completed counter.
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param[in] CmdPtr Command pointer, verified non-null in CS_AppMain
 *
 *  \sa #CS_RESET_COUNTERS_CC
 */
CFE_Status_t CS_ResetCountersCmd(const CS_ResetCountersCmd_t *CmdPtr);

/**
 * \brief process a background checking cycle
 *
 *  \par Description
 *       Processes a background checking cycle when the scheduler
 *       tell CS.
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param[in] CmdPtr Command pointer, verified non-null in CS_AppMain
 */
CFE_Status_t CS_BackgroundCheckCycleCmd(const CS_BackgroundCheckCycleCmd_t *CmdPtr);

/**
 * \brief Process a disable overall background checking command
 *
 *  \par Description
 *     Disables all background checking in CS
 *
 *  \par Assumptions, External Events, and Notes:
 *
 *  \param[in] CmdPtr Command pointer, verified non-null in CS_AppMain
 *
 *  \sa #CS_DISABLE_ALL_CS_CC
 */
CFE_Status_t CS_DisableAllCSCmd(const CS_DisableAllCSCmd_t *CmdPtr);

/**
 * \brief Process an enable overall background checking command
 *
 *  \par Description
 *       Allows background checking to take place.
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param[in] CmdPtr Command pointer, verified non-null in CS_AppMain
 *
 *  \sa #CS_ENABLE_ALL_CS_CC
 */
CFE_Status_t CS_EnableAllCSCmd(const CS_EnableAllCSCmd_t *CmdPtr);

/**
 * \brief Process a disable background checking for the cFE core
 *        code segment command
 *
 *  \par Description
 *       Disables background checking for the cFE core code segment
 *
 *  \par Assumptions, External Events, and Notes:
 *       In order for background checking of individual areas
 *       to checksum (OS code segment, cFE core, EEPROM, Memory,
 *       Apps, and Tables) to occurr, the table must be enabled
 *       and overall checksumming must be enabled.
 *
 *  \param[in] CmdPtr Command pointer, verified non-null in CS_AppMain
 *
 *  \sa #CS_DISABLE_CFE_CORE_CC
 */
CFE_Status_t CS_DisableCfeCoreCmd(const CS_DisableCfeCoreCmd_t *CmdPtr);

/**
 * \brief Process an enable background checking for the cFE core code
 *        segment command
 *
 *  \par Description
 *       Allows the cFE Core code segment to be background checksummed.
 *
 *  \par Assumptions, External Events, and Notes:
 *       In order for background checking of individual areas
 *       to checksum (OS code segment, cFE core, EEPROM, Memory,
 *       Apps, and Tables) to occurr, the table must be enabled
 *       and overall checksumming must be enabled.
 *
 *  \param[in] CmdPtr Command pointer, verified non-null in CS_AppMain
 *
 *  \sa #CS_ENABLE_CFE_CORE_CC
 */
CFE_Status_t CS_EnableCfeCoreCmd(const CS_EnableCfeCoreCmd_t *CmdPtr);

/**
 * \brief Process a disable background checking for the OS code
 *        segment command
 *
 *  \par Description
 *       Disables background checking for the OS code segment
 *
 *  \par Assumptions, External Events, and Notes:
 *       In order for background checking of individual areas
 *       to checksum (OS code segment, cFE core, EEPROM, Memory,
 *       Apps, and Tables) to occurr, the table must be enabled
 *       and overall checksumming must be enabled.
 *
 *  \param[in] CmdPtr Command pointer, verified non-null in CS_AppMain
 *
 *  \sa #CS_DISABLE_OS_CC
 */
CFE_Status_t CS_DisableOSCmd(const CS_DisableOSCmd_t *CmdPtr);

/**
 * \brief Process an enable background checking for the OS code
 *        segment command
 *
 *  \par Description
 *       Allows the OS code segment to be background checksummed.
 *
 *  \par Assumptions, External Events, and Notes:
 *       In order for background checking of individual areas
 *       to checksum (OS code segment, cFE core, EEPROM, Memory,
 *       Apps, and Tables) to occurr, the table must be enabled
 *       and overall checksumming must be enabled.
 *
 *  \param[in] CmdPtr Command pointer, verified non-null in CS_AppMain
 *
 *  \sa #CS_ENABLE_OS_CC
 */
CFE_Status_t CS_EnableOSCmd(const CS_EnableOSCmd_t *CmdPtr);

/**
 * \brief Process a report baseline of the cFE Core command
 *
 *  \par Description
 *        Reports the baseline checksum of the cFE core code segment
 *        if it has already been computed
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param[in] CmdPtr Command pointer, verified non-null in CS_AppMain
 *
 *  \sa #CS_REPORT_BASELINE_CFE_CORE_CC
 */
CFE_Status_t CS_ReportBaselineCfeCoreCmd(const CS_ReportBaselineCfeCoreCmd_t *CmdPtr);

/**
 * \brief Process a report baseline of the OS command
 *
 *  \par Description
 *        Reports the baseline checksum of the OS code segment
 *        if it has already been computed
 *
 *  \par Assumptions, External Events, and Notes:
 *        None
 *
 *  \param[in] CmdPtr Command pointer, verified non-null in CS_AppMain
 *
 *  \sa #CS_REPORT_BASELINE_OS_CC
 */
CFE_Status_t CS_ReportBaselineOSCmd(const CS_ReportBaselineOSCmd_t *CmdPtr);

/**
 * \brief Process a recopmute baseline of the cFE core code segment command
 *
 *  \par Description
 *        Recomputes the checksum of the cFE core code segment and use that
 *        value as the new baseline for the cFE core.
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param[in] CmdPtr Command pointer, verified non-null in CS_AppMain
 *
 *  \sa #CS_RECOMPUTE_BASELINE_CFE_CORE_CC
 */
CFE_Status_t CS_RecomputeBaselineCfeCoreCmd(const CS_RecomputeBaselineCfeCoreCmd_t *CmdPtr);

/**
 * \brief Process a recopmute baseline of the OS command
 *
 *  \par Description
 *        Recomputes the checksum of the OS code segment and use that
 *        value as the new baseline for the OS.
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param[in] CmdPtr Command pointer, verified non-null in CS_AppMain
 *
 *  \sa #CS_RECOMPUTE_BASELINE_OS_CC
 */
CFE_Status_t CS_RecomputeBaselineOSCmd(const CS_RecomputeBaselineOSCmd_t *CmdPtr);

/**
 * \brief Process a start a one shot checksum command
 *
 *  \par Description
 *        Starts a one shot checksum on given address and size,
 *        and reports checksum in telemetry and an event message.
 *
 *  \par Assumptions, External Events, and Notes:
 *
 *  \param[in] CmdPtr Command pointer, verified non-null in CS_AppMain
 *
 *  \sa #CS_ONE_SHOT_CC
 */
CFE_Status_t CS_OneShotCmd(const CS_OneShotCmd_t *CmdPtr);

/**
 * \brief Process a cancel one shot checksum command
 *
 *  \par Description
 *        Cancel a one shot command, if a one shot calculation is
 *        taking place
 *
 *  \par Assumptions, External Events, and Notes:
 *
 *  \param[in] CmdPtr Command pointer, verified non-null in CS_AppMain
 *
 *  \sa #CS_CANCEL_ONE_SHOT_CC
 */
CFE_Status_t CS_CancelOneShotCmd(const CS_CancelOneShotCmd_t *CmdPtr);

#endif
