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
 *   Specification for the CFS Checksum command and telemetry
 *   messages.
 */
#ifndef CS_MSGDEFS_H
#define CS_MSGDEFS_H

#include "cfe.h"

/**
 * \defgroup cfscscmdcodes CFS Checksum Command Codes
 * \{
 */

/**
 * \brief Noop
 *
 *  \par Description
 *       Implements the Noop command that insures the CS task is alive
 *
 *  \par Command Structure
 *       #CS_NoArgsCmd_t
 *
 *  \par Command Verification
 *       Successful execution of this command may be verified with
 *       the following telemetry:
 *       - #CS_HkPacket_t.CmdCounter will increment
 *       - The #CS_NOOP_INF_EID informational event message will be
 *         generated when the command is received
 *
 *  \par Error Conditions
 *       This command may fail for the following reason(s):
 *       - Command packet length not as expected
 *
 *  \par Evidence of failure may be found in the following telemetry:
 *       - #CS_HkPacket_t.CmdErrCounter will increment
 *       - Error specific event message #CS_LEN_ERR_EID
 *
 *  \par Criticality
 *       None
 *
 *  \sa #CS_RESET_CC
 */
#define CS_NOOP_CC 0

/**
 * \brief Reset Counters
 *
 *  \par Description
 *       Resets the CS housekeeping counters
 *
 *  \par Command Structure
 *       #CS_NoArgsCmd_t
 *
 *  \par Command Verification
 *       Successful execution of this command may be verified with
 *       the following telemetry:
 *       - #CS_HkPacket_t.CmdCounter will be cleared
 *       - #CS_HkPacket_t.CmdErrCounter will be cleared
 *       - The #CS_RESET_DBG_EID informational event message will be
 *         generated when the command is executed
 *
 *  \par Error Conditions
 *       This command may fail for the following reason(s):
 *       - Command packet length not as expected
 *
 *  \par Evidence of failure may be found in the following telemetry:
 *       - #CS_HkPacket_t.CmdErrCounter will increment
 *       - Error specific event message #CS_LEN_ERR_EID
 *
 *  \par Criticality
 *       None
 *
 *  \sa #CS_NOOP_CC
 */
#define CS_RESET_CC 1

/**
 * \brief Start One shot calculation
 *
 *  \par Description
 *         Computes a checksum on the command specified address
 *         and size of memory at the command specified rate.
 *         This command spawns a child task to complete the
 *         checksum.
 *
 *  \par Command Structure
 *       #CS_OneShotCmd_t
 *
 *  \par Command Verification
 *       Successful execution of this command may be verified with
 *       the following telemetry:
 *       - #CS_HkPacket_t.CmdCounter will increment
 *       - The #CS_ONESHOT_STARTED_DBG_EID debug event message will be
 *         generated when the command is received
 *       - The CS_ONESHOT_FINISHED_INF_EID informational message will
 *         be generated when the compuation finishes.
 *       - #CS_HkPacket_t.LastOneShotChecksum will be updated to the new value
 *
 *  \par Error Conditions
 *       This command may fail for the following reason(s):
 *       - Command packet length not as expected
 *       - The address and size cannot be validated
 *       - A child task (recompute baseline or one shot ) is
 *         already running, precluding starting another. Only one child
 *         task is allowed to run at any given time.
 *       - The child task failed to be created
 *
 *  \par Evidence of failure may be found in the following telemetry:
 *       - #CS_HkPacket_t.CmdErrCounter will increment
 *       - Error specific event message #CS_LEN_ERR_EID
 *       - Error specific event message #CS_ONESHOT_MEMVALIDATE_ERR_EID
 *       - Error specific event message #CS_ONESHOT_CHDTASK_ERR_EID
 *       - Error specific event message #CS_ONESHOT_CREATE_CHDTASK_ERR_EID
 *
 *  \par Criticality
 *       None
 *
 *  \sa #CS_CANCEL_ONESHOT_CC,
 *      #CS_RECOMPUTE_BASELINE_CFECORE_CC,
 *      #CS_RECOMPUTE_BASELINE_OS_CC,
 *      #CS_RECOMPUTE_BASELINE_EEPROM_CC,
 *      #CS_RECOMPUTE_BASELINE_MEMORY_CC,
 *      #CS_RECOMPUTE_BASELINE_TABLE_CC,
 *      #CS_RECOMPUTE_BASELINE_APP_CC
 */
#define CS_ONESHOT_CC 2

/**
 * \brief Cancel one shot
 *
 *  \par Description
 *       Cancels a one shot calculation that is already in progress.
 *
 *  \par Command Structure
 *       #CS_NoArgsCmd_t
 *
 *  \par Command Verification
 *       Successful execution of this command may be verified with
 *       the following telemetry:
 *       - #CS_HkPacket_t.CmdCounter will increment
 *       - The #CS_ONESHOT_CANCELLED_INF_EID informational event message will be
 *         generated when the command is received
 *
 *  \par Error Conditions
 *       This command may fail for the following reason(s):
 *       - Command packet length not as expected
 *       - A one shot calculation is not in progress
 *       - The child task could not be deleted
 *
 *  \par Evidence of failure may be found in the following telemetry:
 *       - #CS_HkPacket_t.CmdErrCounter will increment
 *       - Error specific event message #CS_LEN_ERR_EID
 *       - Error specific event message #CS_ONESHOT_CANCEL_NO_CHDTASK_ERR_EID
 *       - Error specific event message #CS_ONESHOT_CANCEL_DELETE_CHDTASK_ERR_EID
 *
 *  \par Criticality
 *       None
 *
 *  \sa #CS_ONESHOT_CC
 */
#define CS_CANCEL_ONESHOT_CC 3

/**
 * \brief Enable Global Checksumming
 *
 *  \par Description
 *       Allows CS to continue background checking
 *
 *  \par Command Structure
 *       #CS_NoArgsCmd_t
 *
 *  \par Command Verification
 *       Successful execution of this command may be verified with
 *       the following telemetry:
 *       - #CS_HkPacket_t.CmdCounter will increment
 *       - The #CS_ENABLE_ALL_INF_EID informational event message will be
 *         generated when the command is received
 *       - #CS_HkPacket_t.ChecksumState set to #CS_STATE_ENABLED
 *
 *  \par Error Conditions
 *       This command may fail for the following reason(s):
 *       - Command packet length not as expected
 *
 *  \par Evidence of failure may be found in the following telemetry:
 *       - #CS_HkPacket_t.CmdErrCounter will increment
 *       - Error specific event message #CS_LEN_ERR_EID
 *
 *  \par Criticality
 *       None
 *
 *  \sa #CS_DISABLE_ALL_CS_CC
 */
#define CS_ENABLE_ALL_CS_CC 4

/**
 * \brief Disable Global Checksumming
 *
 *  \par Description
 *       Disables all background checking
 *
 *  \par Command Structure
 *       #CS_NoArgsCmd_t
 *
 *  \par Command Verification
 *       Successful execution of this command may be verified with
 *       the following telemetry:
 *       - #CS_HkPacket_t.CmdCounter will increment
 *       - The #CS_DISABLE_ALL_INF_EID informational event message will be
 *         generated when the command is received
 *       - #CS_HkPacket_t.ChecksumState set to #CS_STATE_DISABLED
 *
 *  \par Error Conditions
 *       This command may fail for the following reason(s):
 *       - Command packet length not as expected
 *
 *  \par Evidence of failure may be found in the following telemetry:
 *       - #CS_HkPacket_t.CmdErrCounter will increment
 *       - Error specific event message #CS_LEN_ERR_EID
 *
 *  \par Criticality
 *       None
 *
 *  \sa #CS_ENABLE_ALL_CS_CC
 */
#define CS_DISABLE_ALL_CS_CC 5

/**
 * \brief Enable checksumming of cFE core
 *
 *  \par Description
 *       Enables background checking on the cFE core code segment
 *
 *  \par Command Structure
 *       #CS_NoArgsCmd_t
 *
 *  \par Command Verification
 *       Successful execution of this command may be verified with
 *       the following telemetry:
 *       - #CS_HkPacket_t.CmdCounter will increment
 *       - The #CS_ENABLE_CFECORE_INF_EID informational event message will be
 *         generated when the command is received
 *       - #CS_HkPacket_t.CfeCoreCSState set to #CS_STATE_ENABLED
 *
 *  \par Error Conditions
 *       This command may fail for the following reason(s):
 *       - Command packet length not as expected
 *
 *  \par Evidence of failure may be found in the following telemetry:
 *       - #CS_HkPacket_t.CmdErrCounter will increment
 *       - Error specific event message #CS_LEN_ERR_EID
 *
 *  \par Criticality
 *       None
 *
 *  \sa #CS_DISABLE_CFECORE_CC
 */
#define CS_ENABLE_CFECORE_CC 6

/**
 * \brief Disable checksumming of cFE core
 *
 *  \par Description
 *       Disables background checking on the cFE core code segment
 *
 *  \par Command Structure
 *       #CS_NoArgsCmd_t
 *
 *  \par Command Verification
 *       Successful execution of this command may be verified with
 *       the following telemetry:
 *       - #CS_HkPacket_t.CmdCounter will increment
 *       - The #CS_DISABLE_CFECORE_INF_EID informational event message will be
 *         generated when the command is received
 *       - #CS_HkPacket_t.CfeCoreCSState set to #CS_STATE_DISABLED
 *
 *  \par Error Conditions
 *       This command may fail for the following reason(s):
 *       - Command packet length not as expected
 *
 *  \par Evidence of failure may be found in the following telemetry:
 *       - #CS_HkPacket_t.CmdErrCounter will increment
 *       - Error specific event message #CS_LEN_ERR_EID
 *
 *  \par Criticality
 *       None
 *
 *  \sa #CS_ENABLE_CFECORE_CC
 */
#define CS_DISABLE_CFECORE_CC 7

/**
 * \brief Report Baseline checksum of cFE core
 *
 *  \par Description
 *       Reports the baseline checksum of the cFE core
 *       that has already been calculated.
 *
 *  \par Command Structure
 *       #CS_NoArgsCmd_t
 *
 *  \par Command Verification
 *       Successful execution of this command may be verified with
 *       the following telemetry:
 *       - #CS_HkPacket_t.CmdCounter will increment
 *       - The #CS_BASELINE_CFECORE_INF_EID informational event message will be
 *         generated when the command is received, or
 *       - The #CS_NO_BASELINE_CFECORE_INF_EID informational event message will be
 *         generated when the command is received
 *
 *  \par Error Conditions
 *       This command may fail for the following reason(s):
 *       - Command packet length not as expected
 *
 *  \par Evidence of failure may be found in the following telemetry:
 *       - #CS_HkPacket_t.CmdErrCounter will increment
 *       - Error specific event message #CS_LEN_ERR_EID
 *
 *  \par Criticality
 *       None
 *
 */
#define CS_REPORT_BASELINE_CFECORE_CC 8

/**
 * \brief Recompute Baseline checksum of cFE core
 *
 *  \par Description
 *       Recomputesthe baseline checksum of the cFE core
 *       and use the new value as the baseline.
 *
 *  \par Command Structure
 *       #CS_NoArgsCmd_t
 *
 *  \par Command Verification
 *       Successful execution of this command may be verified with
 *       the following telemetry:
 *       - #CS_HkPacket_t.CmdCounter will increment
 *       - The #CS_RECOMPUTE_CFECORE_STARTED_DBG_EID debug event message will be
 *         generated when the command is received
 *
 *  \par Error Conditions
 *       This command may fail for the following reason(s):
 *       - Command packet length not as expected
 *       - A child task (recompute baseline or one shot ) is
 *         already running, precluding starting another. Only one child
 *         task is allowed to run at any given time.
 *       - The child task failed to be created by Executive Services (ES)
 *
 *  \par Evidence of failure may be found in the following telemetry:
 *       - #CS_HkPacket_t.CmdErrCounter will increment
 *       - Error specific event message #CS_LEN_ERR_EID
 *       - Error specific event message #CS_RECOMPUTE_CFECORE_CREATE_CHDTASK_ERR_EID
 *       - Error specific event message #CS_RECOMPUTE_CFECORE_CHDTASK_ERR_EID
 *
 *  \par Criticality
 *       None
 *
 *  \sa #CS_ONESHOT_CC,
 *      #CS_RECOMPUTE_BASELINE_OS_CC,
 *      #CS_RECOMPUTE_BASELINE_EEPROM_CC,
 *      #CS_RECOMPUTE_BASELINE_MEMORY_CC,
 *      #CS_RECOMPUTE_BASELINE_TABLE_CC,
 *      #CS_RECOMPUTE_BASELINE_APP_CC
 */
#define CS_RECOMPUTE_BASELINE_CFECORE_CC 9

/**
 * \brief Enable checksumming of OS code segment
 *
 *  \par Description
 *       Enables background checking on the OS code segment
 *
 *  \par Command Structure
 *       #CS_NoArgsCmd_t
 *
 *  \par Command Verification
 *       Successful execution of this command may be verified with
 *       the following telemetry:
 *       - #CS_HkPacket_t.CmdCounter will increment
 *       - The #CS_ENABLE_OS_INF_EID informational event message will be
 *         generated when the command is received
 *
 *  \par Error Conditions
 *       This command may fail for the following reason(s):
 *       - Command packet length not as expected
 *
 *  \par Evidence of failure may be found in the following telemetry:
 *       - #CS_HkPacket_t.CmdErrCounter will increment
 *       - Error specific event message #CS_LEN_ERR_EID
 *       - #CS_HkPacket_t.OSCSState set to #CS_STATE_ENABLED
 *
 *  \par Criticality
 *       None
 *
 *  \sa #CS_DISABLE_OS_CC
 */
#define CS_ENABLE_OS_CC 10

/**
 * \brief Disable checksumming of OS code segment
 *
 *  \par Description
 *       Disables background checking on the OS code segment code segment
 *
 *  \par Command Structure
 *       #CS_NoArgsCmd_t
 *
 *  \par Command Verification
 *       Successful execution of this command may be verified with
 *       the following telemetry:
 *       - #CS_HkPacket_t.CmdCounter will increment
 *       - The #CS_DISABLE_OS_INF_EID informational event message will be
 *         generated when the command is received
 *       - #CS_HkPacket_t.OSCSState set to #CS_STATE_DISABLED
 *
 *  \par Error Conditions
 *       This command may fail for the following reason(s):
 *       - Command packet length not as expected
 *
 *  \par Evidence of failure may be found in the following telemetry:
 *       - #CS_HkPacket_t.CmdErrCounter will increment
 *       - Error specific event message #CS_LEN_ERR_EID
 *
 *  \par Criticality
 *       None
 *
 *  \sa #CS_ENABLE_OS_CC
 */
#define CS_DISABLE_OS_CC 11

/**
 * \brief Report Baseline checksum of OS code segment
 *
 *  \par Description
 *       Reports the baseline checksum of the OS code segment
 *       that has already been calculated.
 *
 *  \par Command Structure
 *       #CS_NoArgsCmd_t
 *
 *  \par Command Verification
 *       Successful execution of this command may be verified with
 *       the following telemetry:
 *       - #CS_HkPacket_t.CmdCounter will increment
 *       - The #CS_BASELINE_OS_INF_EID informational event message will be
 *         generated when the command is received, or
 *       - The #CS_NO_BASELINE_OS_INF_EID informational event message will be
 *         generated when the command is received
 *
 *  \par Error Conditions
 *       This command may fail for the following reason(s):
 *       - Command packet length not as expected
 *
 *  \par Evidence of failure may be found in the following telemetry:
 *       - #CS_HkPacket_t.CmdErrCounter will increment
 *       - Error specific event message #CS_LEN_ERR_EID
 *
 *  \par Criticality
 *       None
 */
#define CS_REPORT_BASELINE_OS_CC 12

/**
 * \brief Recompute Baseline checksum of OS code segment
 *
 *  \par Description
 *       Recomputesthe baseline checksum of the OS code segment
 *       and use the new value as the baseline.
 *
 *  \par Command Structure
 *       #CS_NoArgsCmd_t
 *
 *  \par Command Verification
 *       Successful execution of this command may be verified with
 *       the following telemetry:
 *       - #CS_HkPacket_t.CmdCounter will increment
 *       - The #CS_RECOMPUTE_OS_STARTED_DBG_EID debug event message will be
 *         generated when the command is received
 *
 *  \par Error Conditions
 *       This command may fail for the following reason(s):
 *       - Command packet length not as expected
 *       - A child task (recompute baseline or one shot ) is
 *         already running, precluding starting another. Only one child
 *         task is allowed to run at any given time.
 *       - The child task failed to be created by Executive Services (ES)
 *
 *  \par Evidence of failure may be found in the following telemetry:
 *       - #CS_HkPacket_t.CmdErrCounter will increment
 *       - Error specific event message #CS_LEN_ERR_EID
 *       - Error specific event message #CS_RECOMPUTE_OS_CREATE_CHDTASK_ERR_EID
 *       - Error specific event message #CS_RECOMPUTE_OS_CHDTASK_ERR_EID
 *
 *  \par Criticality
 *       None
 *  \sa #CS_ONESHOT_CC,
 *      #CS_RECOMPUTE_BASELINE_CFECORE_CC,
 *      #CS_RECOMPUTE_BASELINE_EEPROM_CC,
 *      #CS_RECOMPUTE_BASELINE_MEMORY_CC,
 *      #CS_RECOMPUTE_BASELINE_TABLE_CC,
 *      #CS_RECOMPUTE_BASELINE_APP_CC
 */
#define CS_RECOMPUTE_BASELINE_OS_CC 13

/**
 * \brief Enable checksumming for Eeprom table
 *
 *  \par Description
 *       Allow the Eeprom table to checksummed in the background
 *
 *  \par Command Structure
 *       #CS_NoArgsCmd_t
 *
 *  \par Command Verification
 *       Successful execution of this command may be verified with
 *       the following telemetry:
 *       - #CS_HkPacket_t.CmdCounter will increment
 *       - The #CS_ENABLE_EEPROM_INF_EID informational event message will be
 *         generated when the command is received
 *       - #CS_HkPacket_t.EepromCSState set to #CS_STATE_ENABLED
 *
 *  \par Error Conditions
 *       This command may fail for the following reason(s):
 *       - Command packet length not as expected
 *
 *  \par Evidence of failure may be found in the following telemetry:
 *       - #CS_HkPacket_t.CmdErrCounter will increment
 *       - Error specific event message #CS_LEN_ERR_EID
 *
 *  \par Criticality
 *       None
 *
 *  \sa #CS_DISABLE_EEPROM_CC
 */
#define CS_ENABLE_EEPROM_CC 14

/**
 * \brief Disable checksumming for Eeprom table
 *
 *  \par Description
 *       Disable the Eeprom table background checksumming
 *
 *  \par Command Structure
 *       #CS_NoArgsCmd_t
 *
 *  \par Command Verification
 *       Successful execution of this command may be verified with
 *       the following telemetry:
 *       - #CS_HkPacket_t.CmdCounter will increment
 *       - The #CS_DISABLE_EEPROM_INF_EID informational event message will be
 *         generated when the command is received
 *       - #CS_HkPacket_t.EepromCSState set to #CS_STATE_DISABLED
 *
 *  \par Error Conditions
 *       This command may fail for the following reason(s):
 *       - Command packet length not as expected
 *
 *  \par Evidence of failure may be found in the following telemetry:
 *       - #CS_HkPacket_t.CmdErrCounter will increment
 *       - Error specific event message #CS_LEN_ERR_EID
 *
 *  \par Criticality
 *       None
 *
 *  \sa #CS_ENABLE_EEPROM_CC
 */
#define CS_DISABLE_EEPROM_CC 15

/**
 * \brief Report Baseline checksum of Eeprom Entry
 *
 *  \par Description
 *       Reports the baseline checksum of the Eeprom
 *       table entry that has already been calculated.
 *
 *  \par Command Structure
 *       #CS_EntryCmd_t
 *
 *  \par Command Verification
 *       Successful execution of this command may be verified with
 *       the following telemetry:
 *       - #CS_HkPacket_t.CmdCounter will increment
 *       - The #CS_BASELINE_EEPROM_INF_EID informational event message will be
 *         generated when the command is received, or
 *       - The #CS_NO_BASELINE_EEPROM_INF_EID informational event message will be
 *         generated when the command is received
 *
 *  \par Error Conditions
 *       This command may fail for the following reason(s):
 *       - Command packet length not as expected
 *       - The command specified Entry ID is invalid
 *
 *  \par Evidence of failure may be found in the following telemetry:
 *       - #CS_HkPacket_t.CmdErrCounter will increment
 *       - Error specific event message #CS_LEN_ERR_EID
 *       - Error specific event message #CS_BASELINE_INVALID_ENTRY_EEPROM_ERR_EID
 *
 *  \par Criticality
 *       None
 *
 */
#define CS_REPORT_BASELINE_EEPROM_CC 16

/**
 * \brief Recompute Baseline checksum of Eeprom Entry
 *
 *  \par Description
 *       Recompute the baseline checksum of the Eeprom
 *       table entry and use that value as the new baseline.
 *       This command spawns a child task to do the recompute.
 *
 *  \par Command Structure
 *       #CS_EntryCmd_t
 *
 *  \par Command Verification
 *       Successful execution of this command may be verified with
 *       the following telemetry:
 *       - #CS_HkPacket_t.CmdCounter will increment
 *       - The #CS_RECOMPUTE_EEPROM_STARTED_DBG_EID debug event
 *         message will be generated when the command is received
 *       - The #CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID informational event
 *         message will be generated when the recompute is finished
 *
 *  \par Error Conditions
 *       This command may fail for the following reason(s):
 *       - Command packet length not as expected
 *       - The command specified Entry ID is invalid
 *       - A child task (recompute baseline or one shot ) is
 *         already running, precluding starting another. Only one child
 *         task is allowed to run at any given time.
 *       - The child task failed to be created by Executive Services (ES)
 *
 *  \par Evidence of failure may be found in the following telemetry:
 *       - #CS_HkPacket_t.CmdErrCounter will increment
 *       - Error specific event message #CS_LEN_ERR_EID
 *       - Error specific event message #CS_RECOMPUTE_INVALID_ENTRY_EEPROM_ERR_EID
 *       - Error specific event message #CS_RECOMPUTE_EEPROM_CREATE_CHDTASK_ERR_EID
 *       - Error specific event message #CS_RECOMPUTE_EEPROM_CHDTASK_ERR_EID
 *
 *  \par Criticality
 *       None
 *
 *  \sa #CS_ONESHOT_CC,
 *      #CS_RECOMPUTE_BASELINE_CFECORE_CC,
 *      #CS_RECOMPUTE_BASELINE_OS_CC,
 *      #CS_RECOMPUTE_BASELINE_MEMORY_CC,
 *      #CS_RECOMPUTE_BASELINE_TABLE_CC,
 *      #CS_RECOMPUTE_BASELINE_APP_CC
 */
#define CS_RECOMPUTE_BASELINE_EEPROM_CC 17

/**
 * \brief Enable checksumming for an Eeprom entry
 *
 *  \par Description
 *       Allow the Eeprom entry to checksummed in the background
 *
 *  \par Command Structure
 *       #CS_EntryCmd_t
 *
 *  \par Command Verification
 *       Successful execution of this command may be verified with
 *       the following telemetry:
 *       - #CS_HkPacket_t.CmdCounter will increment
 *       - The #CS_ENABLE_EEPROM_ENTRY_INF_EID informational event message will be
 *         generated when the command is received
 *
 *  \par Error Conditions
 *       This command may fail for the following reason(s):
 *       - Command packet length not as expected
 *       - Command specified entry was invalid
 *
 *  \par Evidence of failure may be found in the following telemetry:
 *       - #CS_HkPacket_t.CmdErrCounter will increment
 *       - Error specific event message #CS_LEN_ERR_EID
 *       - Error specific event message #CS_ENABLE_EEPROM_INVALID_ENTRY_ERR_EID
 *
 *  \par Criticality
 *       None
 *
 *  \sa #CS_DISABLE_ENTRY_EEPROM_CC
 */
#define CS_ENABLE_ENTRY_EEPROM_CC 18

/**
 * \brief Disable checksumming for an Eeprom entry
 *
 *  \par Description
 *      Disable the Eeprom entry background checksumming
 *
 *  \par Command Structure
 *       #CS_EntryCmd_t
 *
 *  \par Command Verification
 *       Successful execution of this command may be verified with
 *       the following telemetry:
 *       - #CS_HkPacket_t.CmdCounter will increment
 *       - The #CS_DISABLE_EEPROM_ENTRY_INF_EID informational event message will be
 *         generated when the command is received
 *
 *  \par Error Conditions
 *       This command may fail for the following reason(s):
 *       - Command packet length not as expected
 *       - Command specified entry was invalid
 *
 *  \par Evidence of failure may be found in the following telemetry:
 *       - #CS_HkPacket_t.CmdErrCounter will increment
 *       - Error specific event message #CS_LEN_ERR_EID
 *       - Error specific event message #CS_DISABLE_EEPROM_INVALID_ENTRY_ERR_EID
 *
 *  \par Criticality
 *       None
 *
 *  \sa #CS_ENABLE_ENTRY_EEPROM_CC
 */
#define CS_DISABLE_ENTRY_EEPROM_CC 19

/**
 * \brief Get the Entry ID for a given Eeprom address
 *
 *  \par Description
 *     Gets the Entry ID of an Eeprom address to use in
 *     subsequent commands.
 *
 *  \par Command Structure
 *       #CS_GetEntryIDCmd_t
 *
 *  \par Command Verification
 *       Successful execution of this command may be verified with
 *       the following telemetry:
 *       - #CS_HkPacket_t.CmdCounter will increment
 *       - The #CS_GET_ENTRY_ID_EEPROM_INF_EID informational event message will be
 *         generated when the command is received
 *
 *  \par Error Conditions
 *       This command may fail for the following reason(s):
 *       - Command packet length not as expected
 *       - Command specified address was not found in the table
 *
 *  \par Evidence of failure may be found in the following telemetry:
 *       - #CS_HkPacket_t.CmdErrCounter will increment
 *       - Error specific event message #CS_LEN_ERR_EID
 *       - Error specific event message #CS_GET_ENTRY_ID_EEPROM_NOT_FOUND_INF_EID
 *
 *  \par Criticality
 *       None
 */
#define CS_GET_ENTRY_ID_EEPROM_CC 20

/**
 * \brief Enable checksumming for Memory table
 *
 *  \par Description
 *       Allow the Memory table to checksummed in the background
 *
 *  \par Command Structure
 *       #CS_NoArgsCmd_t
 *
 *  \par Command Verification
 *       Successful execution of this command may be verified with
 *       the following telemetry:
 *       - #CS_HkPacket_t.CmdCounter will increment
 *       - The #CS_ENABLE_MEMORY_INF_EID informational event message will be
 *         generated when the command is received
 *       - #CS_HkPacket_t.MemoryCSState set to #CS_STATE_ENABLED
 *
 *  \par Error Conditions
 *       This command may fail for the following reason(s):
 *       - Command packet length not as expected
 *
 *  \par Evidence of failure may be found in the following telemetry:
 *       - #CS_HkPacket_t.CmdErrCounter will increment
 *       - Error specific event message #CS_LEN_ERR_EID
 *
 *  \par Criticality
 *       None
 *
 *  \sa #CS_DISABLE_MEMORY_CC
 */
#define CS_ENABLE_MEMORY_CC 21

/**
 * \brief Disable checksumming for Memory table
 *
 *  \par Description
 *       Disable the Memory table background checksumming
 *
 *  \par Command Structure
 *       #CS_NoArgsCmd_t
 *
 *  \par Command Verification
 *       Successful execution of this command may be verified with
 *       the following telemetry:
 *       - #CS_HkPacket_t.CmdCounter will increment
 *       - The #CS_DISABLE_MEMORY_INF_EID informational event message will be
 *         generated when the command is received
 *       - #CS_HkPacket_t.MemoryCSState set to #CS_STATE_DISABLED
 *
 *  \par Error Conditions
 *       This command may fail for the following reason(s):
 *       - Command packet length not as expected
 *
 *  \par Evidence of failure may be found in the following telemetry:
 *       - #CS_HkPacket_t.CmdErrCounter will increment
 *       - Error specific event message #CS_LEN_ERR_EID
 *
 *  \par Criticality
 *       None
 *
 *  \sa #CS_ENABLE_MEMORY_CC
 */
#define CS_DISABLE_MEMORY_CC 22

/**
 * \brief Report Baseline checksum of Memory Entry
 *
 *  \par Description
 *       Reports the baseline checksum of the Memory
 *       table entry that has already been calculated.
 *
 *  \par Command Structure
 *       #CS_EntryCmd_t
 *
 *  \par Command Verification
 *       Successful execution of this command may be verified with
 *       the following telemetry:
 *       - #CS_HkPacket_t.CmdCounter will increment
 *       - The #CS_BASELINE_MEMORY_INF_EID informational event message will be
 *         generated when the command is received, or
 *       - The #CS_NO_BASELINE_MEMORY_INF_EID informational event message will be
 *         generated when the command is received
 *
 *  \par Error Conditions
 *       This command may fail for the following reason(s):
 *       - Command packet length not as expected
 *       - The command specified Entry ID is invalid
 *
 *  \par Evidence of failure may be found in the following telemetry:
 *       - #CS_HkPacket_t.CmdErrCounter will increment
 *       - Error specific event message #CS_LEN_ERR_EID
 *       - Error specific event message #CS_BASELINE_INVALID_ENTRY_MEMORY_ERR_EID
 *
 *  \par Criticality
 *       None
 */
#define CS_REPORT_BASELINE_MEMORY_CC 23

/**
 * \brief Recompute Baseline checksum of Memory Entry
 *
 *  \par Description
 *       Recompute the baseline checksum of the Memory
 *       table entry and use that value as the new baseline.
 *       This command spawns a child task to do the recompute.
 *
 *  \par Command Structure
 *       #CS_EntryCmd_t
 *
 *  \par Command Verification
 *       Successful execution of this command may be verified with
 *       the following telemetry:
 *       - #CS_HkPacket_t.CmdCounter will increment
 *       - The #CS_RECOMPUTE_MEMORY_STARTED_DBG_EID debug event
 *         message will be generated when the command is received
 *       - The #CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID informational event
 *         message will be generated when the recompute is finished
 *
 *  \par Error Conditions
 *       This command may fail for the following reason(s):
 *       - Command packet length not as expected
 *       - The command specified Entry ID is invalid
 *       - A child task (recompute baseline or one shot ) is
 *         already running, precluding starting another. Only one child
 *         task is allowed to run at any given time.
 *       - The child task failed to be created by Executive Services (ES)
 *
 *  \par Evidence of failure may be found in the following telemetry:
 *       - #CS_HkPacket_t.CmdErrCounter will increment
 *       - Error specific event message #CS_LEN_ERR_EID
 *       - Error specific event message #CS_RECOMPUTE_INVALID_ENTRY_MEMORY_ERR_EID
 *       - Error specific event message #CS_RECOMPUTE_MEMORY_CREATE_CHDTASK_ERR_EID
 *       - Error specific event message #CS_RECOMPUTE_MEMORY_CHDTASK_ERR_EID
 *
 *  \par Criticality
 *       None
 *
 *  \sa #CS_ONESHOT_CC,
 *      #CS_RECOMPUTE_BASELINE_CFECORE_CC,
 *      #CS_RECOMPUTE_BASELINE_OS_CC,
 *      #CS_RECOMPUTE_BASELINE_EEPROM_CC,
 *      #CS_RECOMPUTE_BASELINE_TABLE_CC,
 *      #CS_RECOMPUTE_BASELINE_APP_CC
 */
#define CS_RECOMPUTE_BASELINE_MEMORY_CC 24

/**
 * \brief Enable checksumming for a Memory entry
 *
 *  \par Description
 *       Allow the Memory entry to checksummed in the background
 *
 *  \par Command Structure
 *       #CS_EntryCmd_t
 *
 *  \par Command Verification
 *       Successful execution of this command may be verified with
 *       the following telemetry:
 *       - #CS_HkPacket_t.CmdCounter will increment
 *       - The #CS_ENABLE_MEMORY_ENTRY_INF_EID informational event message will be
 *         generated when the command is received
 *
 *  \par Error Conditions
 *       This command may fail for the following reason(s):
 *       - Command packet length not as expected
 *       - Command specified entry was invalid
 *
 *  \par Evidence of failure may be found in the following telemetry:
 *       - #CS_HkPacket_t.CmdErrCounter will increment
 *       - Error specific event message #CS_LEN_ERR_EID
 *       - Error specific event message #CS_ENABLE_MEMORY_INVALID_ENTRY_ERR_EID
 *
 *  \par Criticality
 *       None
 *
 *  \sa #CS_DISABLE_ENTRY_MEMORY_CC
 */
#define CS_ENABLE_ENTRY_MEMORY_CC 25

/**
 * \brief Disable checksumming for a Memory entry
 *
 *  \par Description
 *      Disable the Memory entry background checksumming
 *
 *  \par Command Structure
 *       #CS_EntryCmd_t
 *
 *  \par Command Verification
 *       Successful execution of this command may be verified with
 *       the following telemetry:
 *       - #CS_HkPacket_t.CmdCounter will increment
 *       - The #CS_DISABLE_MEMORY_ENTRY_INF_EID informational event message will be
 *         generated when the command is received
 *
 *  \par Error Conditions
 *       This command may fail for the following reason(s):
 *       - Command packet length not as expected
 *       - Command specified entry was invalid
 *
 *  \par Evidence of failure may be found in the following telemetry:
 *       - #CS_HkPacket_t.CmdErrCounter will increment
 *       - Error specific event message #CS_LEN_ERR_EID
 *       - Error specific event message #CS_DISABLE_MEMORY_INVALID_ENTRY_ERR_EID
 *
 *  \par Criticality
 *       None
 *
 *  \sa #CS_ENABLE_ENTRY_MEMORY_CC
 */
#define CS_DISABLE_ENTRY_MEMORY_CC 26

/**
 * \brief Get the Entry ID for a given Memory address
 *
 *  \par Description
 *     Gets the Entry ID of a Memory address to use in
 *     subsequent commands.
 *
 *  \par Command Structure
 *       #CS_GetEntryIDCmd_t
 *
 *  \par Command Verification
 *       Successful execution of this command may be verified with
 *       the following telemetry:
 *       - #CS_HkPacket_t.CmdCounter will increment
 *       - The #CS_GET_ENTRY_ID_MEMORY_INF_EID informational event message will be
 *         generated when the command is received
 *
 *  \par Error Conditions
 *       This command may fail for the following reason(s):
 *       - Command packet length not as expected
 *       - Command specified address was not found in the table
 *
 *  \par Evidence of failure may be found in the following telemetry:
 *       - #CS_HkPacket_t.CmdErrCounter will increment
 *       - Error specific event message #CS_LEN_ERR_EID
 *       - Error specific event message #CS_GET_ENTRY_ID_MEMORY_NOT_FOUND_INF_EID
 *
 *  \par Criticality
 *       None
 *
 */
#define CS_GET_ENTRY_ID_MEMORY_CC 27

/**
 * \brief Enable checksumming for Tables table
 *
 *  \par Description
 *       Allow the Tables table to checksummed in the background
 *
 *  \par Command Structure
 *       #CS_NoArgsCmd_t
 *
 *  \par Command Verification
 *       Successful execution of this command may be verified with
 *       the following telemetry:
 *       - #CS_HkPacket_t.CmdCounter will increment
 *       - The #CS_ENABLE_TABLES_INF_EID informational event message will be
 *         generated when the command is received
 *       - #CS_HkPacket_t.TablesCSState set to #CS_STATE_ENABLED
 *
 *  \par Error Conditions
 *       This command may fail for the following reason(s):
 *       - Command packet length not as expected
 *
 *  \par Evidence of failure may be found in the following telemetry:
 *       - #CS_HkPacket_t.CmdErrCounter will increment
 *       - Error specific event message #CS_LEN_ERR_EID
 *
 *  \par Criticality
 *       None
 *
 *  \sa #CS_DISABLE_TABLES_CC
 */
#define CS_ENABLE_TABLES_CC 28

/**
 * \brief Disable checksumming for Tables table
 *
 *  \par Description
 *       Disable the Tables table background checksumming
 *
 *  \par Command Structure
 *       #CS_NoArgsCmd_t
 *
 *  \par Command Verification
 *       Successful execution of this command may be verified with
 *       the following telemetry:
 *       - #CS_HkPacket_t.CmdCounter will increment
 *       - The #CS_DISABLE_TABLES_INF_EID informational event message will be
 *         generated when the command is received
 *       - #CS_HkPacket_t.TablesCSState set to #CS_STATE_DISABLED
 *
 *  \par Error Conditions
 *       This command may fail for the following reason(s):
 *       - Command packet length not as expected
 *
 *  \par Evidence of failure may be found in the following telemetry:
 *       - #CS_HkPacket_t.CmdErrCounter will increment
 *       - Error specific event message #CS_LEN_ERR_EID
 *
 *  \par Criticality
 *       None
 *
 *  \sa #CS_ENABLE_TABLES_CC
 */
#define CS_DISABLE_TABLES_CC 29

/**
 * \brief Report Baseline checksum of a table
 *
 *  \par Description
 *       Reports the baseline checksum of the
 *       table that has already been calculated.
 *
 *  \par Command Structure
 *       #CS_TableNameCmd_t
 *
 *  \par Command Verification
 *       Successful execution of this command may be verified with
 *       the following telemetry:
 *       - #CS_HkPacket_t.CmdCounter will increment
 *       - The #CS_BASELINE_TABLES_INF_EID informational event message will be
 *         generated when the command is received, or
 *       - The #CS_NO_BASELINE_TABLES_INF_EID informational event message will be
 *         generated when the command is received
 *
 *  \par Error Conditions
 *       This command may fail for the following reason(s):
 *       - Command packet length not as expected
 *       - The command specified able name is invalid
 *
 *  \par Evidence of failure may be found in the following telemetry:
 *       - #CS_HkPacket_t.CmdErrCounter will increment
 *       - Error specific event message #CS_LEN_ERR_EID
 *       - Error specific event message #CS_BASELINE_INVALID_NAME_TABLES_ERR_EID
 *
 *  \par Criticality
 *       None
 */
#define CS_REPORT_BASELINE_TABLE_CC 30

/**
 * \brief Recompute Baseline checksum of a table
 *
 *  \par Description
 *       Recompute the baseline checksum of the
 *       table and use that value as the new baseline.
 *       This command spawns a child task to do the recompute.
 *
 *  \par Command Structure
 *       #CS_TableNameCmd_t
 *
 *  \par Command Verification
 *       Successful execution of this command may be verified with
 *       the following telemetry:
 *       - #CS_HkPacket_t.CmdCounter will increment
 *       - The #CS_RECOMPUTE_TABLES_STARTED_DBG_EID debug event
 *         message will be generated when the command is received
 *       - The #CS_RECOMPUTE_FINISH_TABLES_INF_EID informational event
 *         message will be generated when the recompute is finished.
 *         However, for the CS Table Definitions Table only, the checksum
 *         value will be incorrect. This is because all entries in this
 *         table are disabled while being recomputed and disabling the
 *         entry for itself modifies the contents of the table being
 *         recomputed. Thus, recomputing the CS Tables Definition Table
 *         checksum is not recommended.
 *
 *  \par Error Conditions
 *       This command may fail for the following reason(s):
 *       - Command packet length not as expected
 *       - The command specified table name is invalid
 *       - A child task (recompute baseline or one shot ) is
 *         already running, precluding starting another. Only one child
 *         task is allowed to run at any given time.
 *       - The child task failed to be created by Executive Services (ES)
 *
 *  \par Evidence of failure may be found in the following telemetry:
 *       - #CS_HkPacket_t.CmdErrCounter will increment
 *       - Error specific event message #CS_LEN_ERR_EID
 *       - Error specific event message #CS_RECOMPUTE_UNKNOWN_NAME_TABLES_ERR_EID
 *       - Error specific event message #CS_RECOMPUTE_TABLES_CREATE_CHDTASK_ERR_EID
 *       - Error specific event message #CS_RECOMPUTE_TABLES_CHDTASK_ERR_EID
 *
 *  \par Criticality
 *       None
 *
 *  \sa #CS_ONESHOT_CC,
 *      #CS_RECOMPUTE_BASELINE_CFECORE_CC,
 *      #CS_RECOMPUTE_BASELINE_OS_CC,
 *      #CS_RECOMPUTE_BASELINE_EEPROM_CC,
 *      #CS_RECOMPUTE_BASELINE_MEMORY_CC,
 *      #CS_RECOMPUTE_BASELINE_APP_CC
 */
#define CS_RECOMPUTE_BASELINE_TABLE_CC 31

/**
 * \brief Enable checksumming for a table
 *
 *  \par Description
 *       Allow the table to checksummed in the background
 *
 *  \par Command Structure
 *       #CS_TableNameCmd_t
 *
 *  \par Command Verification
 *       Successful execution of this command may be verified with
 *       the following telemetry:
 *       - #CS_HkPacket_t.CmdCounter will increment
 *       - The #CS_ENABLE_TABLES_NAME_INF_EID informational event message will be
 *         generated when the command is received
 *
 *  \par Error Conditions
 *       This command may fail for the following reason(s):
 *       - Command packet length not as expected
 *       - Command specified table name was invalid
 *
 *  \par Evidence of failure may be found in the following telemetry:
 *       - #CS_HkPacket_t.CmdErrCounter will increment
 *       - Error specific event message #CS_LEN_ERR_EID
 *       - Error specific event message #CS_ENABLE_TABLES_UNKNOWN_NAME_ERR_EID
 *
 *  \par Criticality
 *       None
 *
 *  \sa #CS_DISABLE_NAME_TABLE_CC
 */
#define CS_ENABLE_NAME_TABLE_CC 32

/**
 * \brief Disable checksumming for a table
 *
 *  \par Description
 *       Disable background checking of the table
 *
 *  \par Command Structure
 *       #CS_TableNameCmd_t
 *
 *  \par Command Verification
 *       Successful execution of this command may be verified with
 *       the following telemetry:
 *       - #CS_HkPacket_t.CmdCounter will increment
 *       - The #CS_DISABLE_TABLES_NAME_INF_EID informational event message will be
 *         generated when the command is received
 *
 *  \par Error Conditions
 *       This command may fail for the following reason(s):
 *       - Command packet length not as expected
 *       - Command specified table name was invalid
 *
 *  \par Evidence of failure may be found in the following telemetry:
 *       - #CS_HkPacket_t.CmdErrCounter will increment
 *       - Error specific event message #CS_DISABLE_TABLES_NAME_INF_EID
 *       - Error specific event message #CS_DISABLE_TABLES_UNKNOWN_NAME_ERR_EID
 *
 *  \par Criticality
 *       None
 *
 *  \sa #CS_DISABLE_NAME_TABLE_CC
 */
#define CS_DISABLE_NAME_TABLE_CC 33

/**
 * \brief Enable checksumming for App table
 *
 *  \par Description
 *       Allow the App table to checksummed in the background
 *
 *  \par Command Structure
 *       #CS_NoArgsCmd_t
 *
 *  \par Command Verification
 *       Successful execution of this command may be verified with
 *       the following telemetry:
 *       - #CS_HkPacket_t.CmdCounter will increment
 *       - The #CS_ENABLE_APP_INF_EID informational event message will be
 *         generated when the command is received
 *       - #CS_HkPacket_t.AppCSState set to #CS_STATE_ENABLED
 *
 *  \par Error Conditions
 *       This command may fail for the following reason(s):
 *       - Command packet length not as expected
 *
 *  \par Evidence of failure may be found in the following telemetry:
 *       - #CS_HkPacket_t.CmdErrCounter will increment
 *       - Error specific event message #CS_LEN_ERR_EID
 *
 *  \par Criticality
 *       None
 *
 *  \sa #CS_DISABLE_APPS_CC
 */
#define CS_ENABLE_APPS_CC 34

/**
 * \brief Disable checksumming for App table
 *
 *  \par Description
 *       Disable the App table background checksumming
 *
 *  \par Command Structure
 *       #CS_NoArgsCmd_t
 *
 *  \par Command Verification
 *       Successful execution of this command may be verified with
 *       the following telemetry:
 *       - #CS_HkPacket_t.CmdCounter will increment
 *       - The #CS_DISABLE_APP_INF_EID informational event message will be
 *         generated when the command is received
 *       - #CS_HkPacket_t.AppCSState set to #CS_STATE_DISABLED
 *
 *  \par Error Conditions
 *       This command may fail for the following reason(s):
 *       - Command packet length not as expected
 *
 *  \par Evidence of failure may be found in the following telemetry:
 *       - #CS_HkPacket_t.CmdErrCounter will increment
 *       - Error specific event message #CS_LEN_ERR_EID
 *
 *  \par Criticality
 *       None
 *
 *  \sa #CS_ENABLE_APPS_CC
 */
#define CS_DISABLE_APPS_CC 35

/**
 * \brief Report Baseline checksum of a app
 *
 *  \par Description
 *       Reports the baseline checksum of the
 *       app that has already been calculated.
 *
 *  \par Command Structure
 *       #CS_AppNameCmd_t
 *
 *  \par Command Verification
 *       Successful execution of this command may be verified with
 *       the following telemetry:
 *       - #CS_HkPacket_t.CmdCounter will increment
 *       - The #CS_BASELINE_APP_INF_EID informational event message will be
 *         generated when the command is received, or
 *       - The #CS_NO_BASELINE_APP_INF_EID informational event message will be
 *         generated when the command is received
 *
 *  \par Error Conditions
 *       This command may fail for the following reason(s):
 *       - Command packet length not as expected
 *       - The command specified able name is invalid
 *
 *  \par Evidence of failure may be found in the following telemetry:
 *       - #CS_HkPacket_t.CmdErrCounter will increment
 *       - Error specific event message #CS_LEN_ERR_EID
 *       - Error specific event message #CS_BASELINE_INVALID_NAME_APP_ERR_EID
 *
 *  \par Criticality
 *       None
 */
#define CS_REPORT_BASELINE_APP_CC 36

/**
 * \brief Recompute Baseline checksum of a app
 *
 *  \par Description
 *       Recompute the baseline checksum of the
 *       app and use that value as the new baseline.
 *       This command spawns a child task to do the recompute.
 *
 *  \par Command Structure
 *       #CS_AppNameCmd_t
 *
 *  \par Command Verification
 *       Successful execution of this command may be verified with
 *       the following telemetry:
 *       - #CS_HkPacket_t.CmdCounter will increment
 *       - The #CS_RECOMPUTE_APP_STARTED_DBG_EID debug event
 *         message will be generated when the command is received
 *       - The #CS_RECOMPUTE_FINISH_APP_INF_EID informational event
 *         message will be generated when the recompute is finished
 *
 *  \par Error Conditions
 *       This command may fail for the following reason(s):
 *       - Command packet length not as expected
 *       - The command specified app name is invalid
 *       - A child task (recompute baseline or one shot ) is
 *         already running, precluding starting another. Only one child
 *         task is allowed to run at any given time.
 *       - The child task failed to be created by Executive Services (ES)
 *
 *  \par Evidence of failure may be found in the following telemetry:
 *       - #CS_HkPacket_t.CmdErrCounter will increment
 *       - Error specific event message #CS_LEN_ERR_EID
 *       - Error specific event message #CS_RECOMPUTE_UNKNOWN_NAME_APP_ERR_EID
 *       - Error specific event message #CS_RECOMPUTE_APP_CREATE_CHDTASK_ERR_EID
 *       - Error specific event message #CS_RECOMPUTE_APP_CHDTASK_ERR_EID
 *
 *  \par Criticality
 *       None
 *
 *  \sa #CS_ONESHOT_CC,
 *      #CS_RECOMPUTE_BASELINE_CFECORE_CC,
 *      #CS_RECOMPUTE_BASELINE_OS_CC,
 *      #CS_RECOMPUTE_BASELINE_EEPROM_CC,
 *      #CS_RECOMPUTE_BASELINE_MEMORY_CC,
 *      #CS_RECOMPUTE_BASELINE_TABLE_CC
 */
#define CS_RECOMPUTE_BASELINE_APP_CC 37

/**
 * \brief Enable checksumming for a app
 *
 *  \par Description
 *       Allow the app to checksummed in the background
 *
 *  \par Command Structure
 *       #CS_AppNameCmd_t
 *
 *  \par Command Verification
 *       Successful execution of this command may be verified with
 *       the following telemetry:
 *       - #CS_HkPacket_t.CmdCounter will increment
 *       - The #CS_ENABLE_APP_NAME_INF_EID informational event message will be
 *         generated when the command is received
 *
 *  \par Error Conditions
 *       This command may fail for the following reason(s):
 *       - Command packet length not as expected
 *       - Command specified app name was invalid
 *
 *  \par Evidence of failure may be found in the following telemetry:
 *       - #CS_HkPacket_t.CmdErrCounter will increment
 *       - Error specific event message #CS_LEN_ERR_EID
 *       - Error specific event message #CS_ENABLE_APP_UNKNOWN_NAME_ERR_EID
 *
 *  \par Criticality
 *       None
 *
 *  \sa #CS_DISABLE_NAME_APP_CC
 */
#define CS_ENABLE_NAME_APP_CC 38

/**
 * \brief Disable checksumming for a app
 *
 *  \par Description
 *       Disable background checking of the app
 *
 *  \par Command Structure
 *       #CS_AppNameCmd_t
 *
 *  \par Command Verification
 *       Successful execution of this command may be verified with
 *       the following telemetry:
 *       - #CS_HkPacket_t.CmdCounter will increment
 *       - The #CS_DISABLE_APP_NAME_INF_EID informational event message will be
 *         generated when the command is received
 *
 *  \par Error Conditions
 *       This command may fail for the following reason(s):
 *       - Command packet length not as expected
 *       - Command specified app name was invalid
 *
 *  \par Evidence of failure may be found in the following telemetry:
 *       - #CS_HkPacket_t.CmdErrCounter will increment
 *       - Error specific event message #CS_DISABLE_APP_NAME_INF_EID
 *       - Error specific event message #CS_DISABLE_APP_UNKNOWN_NAME_ERR_EID
 *
 *  \par Criticality
 *       None
 *
 *  \sa #CS_DISABLE_NAME_APP_CC
 */
#define CS_DISABLE_NAME_APP_CC 39

/**\}*/

/**
 * \name CS Checksum Type Number Definitions
 * \{
 */
#define CS_CFECORE      0 /**< \brief cFE Core checksum */
#define CS_OSCORE       1 /**< \brief OS Core checksum */
#define CS_EEPROM_TABLE 2 /**< \brief EEPROM checksum */
#define CS_MEMORY_TABLE 3 /**< \brief Memory checksum */
#define CS_TABLES_TABLE 4 /**< \brief Tables checksum */
#define CS_APP_TABLE    5 /**< \brief App checksum */
#define CS_NUM_TABLES   6 /**< \brief Number of checksum types*/
/**\}*/

/**
 * \name CS Checkum States
 * \{
 */
#define CS_STATE_EMPTY     0x00 /**< \brief Entry unused */
#define CS_STATE_ENABLED   0x01 /**< \brief Entry or table enabled */
#define CS_STATE_DISABLED  0x02 /**< \brief Entry or table disabled */
#define CS_STATE_UNDEFINED 0x03 /**< \brief Entry not found state undefined */
/**\}*/

#endif
