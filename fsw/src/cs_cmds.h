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
void CS_NoopCmd(const CS_NoArgsCmd_t *CmdPtr);

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
 *  \sa #CS_RESET_CC
 */
void CS_ResetCmd(const CS_NoArgsCmd_t *CmdPtr);

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
void CS_BackgroundCheckCycle(const CS_NoArgsCmd_t *CmdPtr);

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
void CS_DisableAllCSCmd(const CS_NoArgsCmd_t *CmdPtr);

/**
 * \brief Process a enable overall background checking command
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
void CS_EnableAllCSCmd(const CS_NoArgsCmd_t *CmdPtr);

/**
 * \brief Process a disable background checking for the cFE core
 *        code segment command
 *
 *  \par Description
 *       Disables background checking for the cFE core code segment
 *
 *  \par Assumptions, External Events, and Notes:
 *       In order for background checking of individual areas
 *       to checksum (OS code segment, cFE core, Eeprom, Memory,
 *       Apps, and Tables) to occurr, the table must be enabled
 *       and overall checksumming must be enabled.
 *
 *  \param[in] CmdPtr Command pointer, verified non-null in CS_AppMain
 *
 *  \sa #CS_DISABLE_CFECORE_CC
 */
void CS_DisableCfeCoreCmd(const CS_NoArgsCmd_t *CmdPtr);

/**
 * \brief Process a enable background checking for the cFE core code
 *        segment command
 *
 *  \par Description
 *       Allows the cFE Core code segment to be background checksummed.
 *
 *  \par Assumptions, External Events, and Notes:
 *       In order for background checking of individual areas
 *       to checksum (OS code segment, cFE core, Eeprom, Memory,
 *       Apps, and Tables) to occurr, the table must be enabled
 *       and overall checksumming must be enabled.
 *
 *  \param[in] CmdPtr Command pointer, verified non-null in CS_AppMain
 *
 *  \sa #CS_ENABLE_CFECORE_CC
 */
void CS_EnableCfeCoreCmd(const CS_NoArgsCmd_t *CmdPtr);

/**
 * \brief Process a disable background checking for the OS code
 *        segment command
 *
 *  \par Description
 *       Disables background checking for the OS code segment
 *
 *  \par Assumptions, External Events, and Notes:
 *       In order for background checking of individual areas
 *       to checksum (OS code segment, cFE core, Eeprom, Memory,
 *       Apps, and Tables) to occurr, the table must be enabled
 *       and overall checksumming must be enabled.
 *
 *  \param[in] CmdPtr Command pointer, verified non-null in CS_AppMain
 *
 *  \sa #CS_DISABLE_OS_CC
 */
void CS_DisableOSCmd(const CS_NoArgsCmd_t *CmdPtr);

/**
 * \brief Process a enable background checking for the OS code
 *        segment command
 *
 *  \par Description
 *       Allows the OS code segment to be background checksummed.
 *
 *  \par Assumptions, External Events, and Notes:
 *       In order for background checking of individual areas
 *       to checksum (OS code segment, cFE core, Eeprom, Memory,
 *       Apps, and Tables) to occurr, the table must be enabled
 *       and overall checksumming must be enabled.
 *
 *  \param[in] CmdPtr Command pointer, verified non-null in CS_AppMain
 *
 *  \sa #CS_ENABLE_OS_CC
 */
void CS_EnableOSCmd(const CS_NoArgsCmd_t *CmdPtr);

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
 *  \sa #CS_REPORT_BASELINE_CFECORE_CC
 */
void CS_ReportBaselineCfeCoreCmd(const CS_NoArgsCmd_t *CmdPtr);

/**
 * \brief Proccess a report baseline of the OS command
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
void CS_ReportBaselineOSCmd(const CS_NoArgsCmd_t *CmdPtr);

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
 *  \sa #CS_RECOMPUTE_BASELINE_CFECORE_CC
 */
void CS_RecomputeBaselineCfeCoreCmd(const CS_NoArgsCmd_t *CmdPtr);

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
void CS_RecomputeBaselineOSCmd(const CS_NoArgsCmd_t *CmdPtr);

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
 *  \sa #CS_ONESHOT_CC
 */
void CS_OneShotCmd(const CS_OneShotCmd_t *CmdPtr);

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
 *  \sa #CS_CANCEL_ONESHOT_CC
 */
void CS_CancelOneShotCmd(const CS_NoArgsCmd_t *CmdPtr);

#endif
