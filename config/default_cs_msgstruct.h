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
 *   Specification for the CFS Checksum command and telemetry
 *   messages.
 */
#ifndef DEFAULT_CS_MSGSTRUCT_H
#define DEFAULT_CS_MSGSTRUCT_H

#include "cs_msgdefs.h"
#include "cfe_msg_hdr.h"

/**
 * \defgroup cfscstlm CFS Checksum Telemetry
 * \{
 */

/**
 *  \brief Housekeeping Packet Structure
 */
typedef struct
{
    CFE_MSG_TelemetryHeader_t TelemetryHeader; /**< \brief cFE SB Tlm Msg Hdr */
    CS_HkPacket_Payload_t     Payload;         /**< \brief CS HK Payload */
} CS_HkPacket_t;

/**\}*/

/**
 * \defgroup cfscscmdstructs CFS Checksum Command Structures
 * \{
 */

typedef struct
{
    CFE_MSG_CommandHeader_t CommandHeader; /**< \brief Command header */
} CS_SendHkCmd_t;

/**
 * \brief No arguments command data type
 *
 *  For command details see #CS_NOOP_CC #CS_RESET_COUNTERS_CC, #CS_ENABLE_ALL_CS_CC,
 *  #CS_DISABLE_ALL_CS_CC, #CS_ENABLE_CFE_CORE_CC, #CS_DISABLE_CFE_CORE_CC, #CS_ENABLE_OS_CC,
 *  #CS_DISABLE_OS_CC, #CS_ENABLE_EEPROM_CC, #CS_DISABLE_EEPROM_CC, #CS_ENABLE_MEMORY_CC,
 *  #CS_DISABLE_MEMORY_CC, #CS_ENABLE_TABLES_CC, #CS_DISABLE_TABLES_CC
 *  #CS_ENABLE_APPS_CC, #CS_DISABLE_APPS_CC, #CS_CANCEL_ONE_SHOT_CC
 */
typedef struct
{
    CFE_MSG_CommandHeader_t CommandHeader;
} CS_NoopCmd_t;

/**
 * \brief No arguments command data type
 *
 *  For command details see #CS_NOOP_CC #CS_RESET_COUNTERS_CC, #CS_ENABLE_ALL_CS_CC,
 *  #CS_DISABLE_ALL_CS_CC, #CS_ENABLE_CFE_CORE_CC, #CS_DISABLE_CFE_CORE_CC, #CS_ENABLE_OS_CC,
 *  #CS_DISABLE_OS_CC, #CS_ENABLE_EEPROM_CC, #CS_DISABLE_EEPROM_CC, #CS_ENABLE_MEMORY_CC,
 *  #CS_DISABLE_MEMORY_CC, #CS_ENABLE_TABLES_CC, #CS_DISABLE_TABLES_CC
 *  #CS_ENABLE_APPS_CC, #CS_DISABLE_APPS_CC, #CS_CANCEL_ONE_SHOT_CC
 */
typedef struct
{
    CFE_MSG_CommandHeader_t CommandHeader;
} CS_ResetCountersCmd_t;

/**
 * \brief No arguments command data type
 *
 *  For command details see #CS_NOOP_CC #CS_RESET_COUNTERS_CC, #CS_ENABLE_ALL_CS_CC,
 *  #CS_DISABLE_ALL_CS_CC, #CS_ENABLE_CFE_CORE_CC, #CS_DISABLE_CFE_CORE_CC, #CS_ENABLE_OS_CC,
 *  #CS_DISABLE_OS_CC, #CS_ENABLE_EEPROM_CC, #CS_DISABLE_EEPROM_CC, #CS_ENABLE_MEMORY_CC,
 *  #CS_DISABLE_MEMORY_CC, #CS_ENABLE_TABLES_CC, #CS_DISABLE_TABLES_CC
 *  #CS_ENABLE_APPS_CC, #CS_DISABLE_APPS_CC, #CS_CANCEL_ONE_SHOT_CC
 */
typedef struct
{
    CFE_MSG_CommandHeader_t CommandHeader;
} CS_BackgroundCheckCycleCmd_t;

/**
 * \brief No arguments command data type
 *
 *  For command details see #CS_NOOP_CC #CS_RESET_COUNTERS_CC, #CS_ENABLE_ALL_CS_CC,
 *  #CS_DISABLE_ALL_CS_CC, #CS_ENABLE_CFE_CORE_CC, #CS_DISABLE_CFE_CORE_CC, #CS_ENABLE_OS_CC,
 *  #CS_DISABLE_OS_CC, #CS_ENABLE_EEPROM_CC, #CS_DISABLE_EEPROM_CC, #CS_ENABLE_MEMORY_CC,
 *  #CS_DISABLE_MEMORY_CC, #CS_ENABLE_TABLES_CC, #CS_DISABLE_TABLES_CC
 *  #CS_ENABLE_APPS_CC, #CS_DISABLE_APPS_CC, #CS_CANCEL_ONE_SHOT_CC
 */
typedef struct
{
    CFE_MSG_CommandHeader_t CommandHeader;
} CS_DisableAllCSCmd_t;

/**
 * \brief No arguments command data type
 *
 *  For command details see #CS_NOOP_CC #CS_RESET_COUNTERS_CC, #CS_ENABLE_ALL_CS_CC,
 *  #CS_DISABLE_ALL_CS_CC, #CS_ENABLE_CFE_CORE_CC, #CS_DISABLE_CFE_CORE_CC, #CS_ENABLE_OS_CC,
 *  #CS_DISABLE_OS_CC, #CS_ENABLE_EEPROM_CC, #CS_DISABLE_EEPROM_CC, #CS_ENABLE_MEMORY_CC,
 *  #CS_DISABLE_MEMORY_CC, #CS_ENABLE_TABLES_CC, #CS_DISABLE_TABLES_CC
 *  #CS_ENABLE_APPS_CC, #CS_DISABLE_APPS_CC, #CS_CANCEL_ONE_SHOT_CC
 */
typedef struct
{
    CFE_MSG_CommandHeader_t CommandHeader;
} CS_EnableAllCSCmd_t;

/**
 * \brief No arguments command data type
 *
 *  For command details see #CS_NOOP_CC #CS_RESET_COUNTERS_CC, #CS_ENABLE_ALL_CS_CC,
 *  #CS_DISABLE_ALL_CS_CC, #CS_ENABLE_CFE_CORE_CC, #CS_DISABLE_CFE_CORE_CC, #CS_ENABLE_OS_CC,
 *  #CS_DISABLE_OS_CC, #CS_ENABLE_EEPROM_CC, #CS_DISABLE_EEPROM_CC, #CS_ENABLE_MEMORY_CC,
 *  #CS_DISABLE_MEMORY_CC, #CS_ENABLE_TABLES_CC, #CS_DISABLE_TABLES_CC
 *  #CS_ENABLE_APPS_CC, #CS_DISABLE_APPS_CC, #CS_CANCEL_ONE_SHOT_CC
 */
typedef struct
{
    CFE_MSG_CommandHeader_t CommandHeader;
} CS_DisableCfeCoreCmd_t;

/**
 * \brief No arguments command data type
 *
 *  For command details see #CS_NOOP_CC #CS_RESET_COUNTERS_CC, #CS_ENABLE_ALL_CS_CC,
 *  #CS_DISABLE_ALL_CS_CC, #CS_ENABLE_CFE_CORE_CC, #CS_DISABLE_CFE_CORE_CC, #CS_ENABLE_OS_CC,
 *  #CS_DISABLE_OS_CC, #CS_ENABLE_EEPROM_CC, #CS_DISABLE_EEPROM_CC, #CS_ENABLE_MEMORY_CC,
 *  #CS_DISABLE_MEMORY_CC, #CS_ENABLE_TABLES_CC, #CS_DISABLE_TABLES_CC
 *  #CS_ENABLE_APPS_CC, #CS_DISABLE_APPS_CC, #CS_CANCEL_ONE_SHOT_CC
 */
typedef struct
{
    CFE_MSG_CommandHeader_t CommandHeader;
} CS_EnableCfeCoreCmd_t;

/**
 * \brief No arguments command data type
 *
 *  For command details see #CS_NOOP_CC #CS_RESET_COUNTERS_CC, #CS_ENABLE_ALL_CS_CC,
 *  #CS_DISABLE_ALL_CS_CC, #CS_ENABLE_CFE_CORE_CC, #CS_DISABLE_CFE_CORE_CC, #CS_ENABLE_OS_CC,
 *  #CS_DISABLE_OS_CC, #CS_ENABLE_EEPROM_CC, #CS_DISABLE_EEPROM_CC, #CS_ENABLE_MEMORY_CC,
 *  #CS_DISABLE_MEMORY_CC, #CS_ENABLE_TABLES_CC, #CS_DISABLE_TABLES_CC
 *  #CS_ENABLE_APPS_CC, #CS_DISABLE_APPS_CC, #CS_CANCEL_ONE_SHOT_CC
 */
typedef struct
{
    CFE_MSG_CommandHeader_t CommandHeader;
} CS_DisableOSCmd_t;

/**
 * \brief No arguments command data type
 *
 *  For command details see #CS_NOOP_CC #CS_RESET_COUNTERS_CC, #CS_ENABLE_ALL_CS_CC,
 *  #CS_DISABLE_ALL_CS_CC, #CS_ENABLE_CFE_CORE_CC, #CS_DISABLE_CFE_CORE_CC, #CS_ENABLE_OS_CC,
 *  #CS_DISABLE_OS_CC, #CS_ENABLE_EEPROM_CC, #CS_DISABLE_EEPROM_CC, #CS_ENABLE_MEMORY_CC,
 *  #CS_DISABLE_MEMORY_CC, #CS_ENABLE_TABLES_CC, #CS_DISABLE_TABLES_CC
 *  #CS_ENABLE_APPS_CC, #CS_DISABLE_APPS_CC, #CS_CANCEL_ONE_SHOT_CC
 */
typedef struct
{
    CFE_MSG_CommandHeader_t CommandHeader;
} CS_EnableOSCmd_t;

/**
 * \brief No arguments command data type
 *
 *  For command details see #CS_NOOP_CC #CS_RESET_COUNTERS_CC, #CS_ENABLE_ALL_CS_CC,
 *  #CS_DISABLE_ALL_CS_CC, #CS_ENABLE_CFE_CORE_CC, #CS_DISABLE_CFE_CORE_CC, #CS_ENABLE_OS_CC,
 *  #CS_DISABLE_OS_CC, #CS_ENABLE_EEPROM_CC, #CS_DISABLE_EEPROM_CC, #CS_ENABLE_MEMORY_CC,
 *  #CS_DISABLE_MEMORY_CC, #CS_ENABLE_TABLES_CC, #CS_DISABLE_TABLES_CC
 *  #CS_ENABLE_APPS_CC, #CS_DISABLE_APPS_CC, #CS_CANCEL_ONE_SHOT_CC
 */
typedef struct
{
    CFE_MSG_CommandHeader_t CommandHeader;
} CS_ReportBaselineCfeCoreCmd_t;

/**
 * \brief No arguments command data type
 *
 *  For command details see #CS_NOOP_CC #CS_RESET_COUNTERS_CC, #CS_ENABLE_ALL_CS_CC,
 *  #CS_DISABLE_ALL_CS_CC, #CS_ENABLE_CFE_CORE_CC, #CS_DISABLE_CFE_CORE_CC, #CS_ENABLE_OS_CC,
 *  #CS_DISABLE_OS_CC, #CS_ENABLE_EEPROM_CC, #CS_DISABLE_EEPROM_CC, #CS_ENABLE_MEMORY_CC,
 *  #CS_DISABLE_MEMORY_CC, #CS_ENABLE_TABLES_CC, #CS_DISABLE_TABLES_CC
 *  #CS_ENABLE_APPS_CC, #CS_DISABLE_APPS_CC, #CS_CANCEL_ONE_SHOT_CC
 */
typedef struct
{
    CFE_MSG_CommandHeader_t CommandHeader;
} CS_ReportBaselineOSCmd_t;

/**
 * \brief No arguments command data type
 *
 *  For command details see #CS_NOOP_CC #CS_RESET_COUNTERS_CC, #CS_ENABLE_ALL_CS_CC,
 *  #CS_DISABLE_ALL_CS_CC, #CS_ENABLE_CFE_CORE_CC, #CS_DISABLE_CFE_CORE_CC, #CS_ENABLE_OS_CC,
 *  #CS_DISABLE_OS_CC, #CS_ENABLE_EEPROM_CC, #CS_DISABLE_EEPROM_CC, #CS_ENABLE_MEMORY_CC,
 *  #CS_DISABLE_MEMORY_CC, #CS_ENABLE_TABLES_CC, #CS_DISABLE_TABLES_CC
 *  #CS_ENABLE_APPS_CC, #CS_DISABLE_APPS_CC, #CS_CANCEL_ONE_SHOT_CC
 */
typedef struct
{
    CFE_MSG_CommandHeader_t CommandHeader;
} CS_RecomputeBaselineCfeCoreCmd_t;

/**
 * \brief No arguments command data type
 *
 *  For command details see #CS_NOOP_CC #CS_RESET_COUNTERS_CC, #CS_ENABLE_ALL_CS_CC,
 *  #CS_DISABLE_ALL_CS_CC, #CS_ENABLE_CFE_CORE_CC, #CS_DISABLE_CFE_CORE_CC, #CS_ENABLE_OS_CC,
 *  #CS_DISABLE_OS_CC, #CS_ENABLE_EEPROM_CC, #CS_DISABLE_EEPROM_CC, #CS_ENABLE_MEMORY_CC,
 *  #CS_DISABLE_MEMORY_CC, #CS_ENABLE_TABLES_CC, #CS_DISABLE_TABLES_CC
 *  #CS_ENABLE_APPS_CC, #CS_DISABLE_APPS_CC, #CS_CANCEL_ONE_SHOT_CC
 */
typedef struct
{
    CFE_MSG_CommandHeader_t CommandHeader;
} CS_RecomputeBaselineOSCmd_t;

/**
 * \brief No arguments command data type
 *
 *  For command details see #CS_NOOP_CC #CS_RESET_COUNTERS_CC, #CS_ENABLE_ALL_CS_CC,
 *  #CS_DISABLE_ALL_CS_CC, #CS_ENABLE_CFE_CORE_CC, #CS_DISABLE_CFE_CORE_CC, #CS_ENABLE_OS_CC,
 *  #CS_DISABLE_OS_CC, #CS_ENABLE_EEPROM_CC, #CS_DISABLE_EEPROM_CC, #CS_ENABLE_MEMORY_CC,
 *  #CS_DISABLE_MEMORY_CC, #CS_ENABLE_TABLES_CC, #CS_DISABLE_TABLES_CC
 *  #CS_ENABLE_APPS_CC, #CS_DISABLE_APPS_CC, #CS_CANCEL_ONE_SHOT_CC
 */
typedef struct
{
    CFE_MSG_CommandHeader_t CommandHeader;
} CS_CancelOneShotCmd_t;

/**
 * \brief No arguments command data type
 *
 *  For command details see #CS_NOOP_CC #CS_RESET_COUNTERS_CC, #CS_ENABLE_ALL_CS_CC,
 *  #CS_DISABLE_ALL_CS_CC, #CS_ENABLE_CFE_CORE_CC, #CS_DISABLE_CFE_CORE_CC, #CS_ENABLE_OS_CC,
 *  #CS_DISABLE_OS_CC, #CS_ENABLE_EEPROM_CC, #CS_DISABLE_EEPROM_CC, #CS_ENABLE_MEMORY_CC,
 *  #CS_DISABLE_MEMORY_CC, #CS_ENABLE_TABLES_CC, #CS_DISABLE_TABLES_CC
 *  #CS_ENABLE_APPS_CC, #CS_DISABLE_APPS_CC, #CS_CANCEL_ONE_SHOT_CC
 */
typedef struct
{
    CFE_MSG_CommandHeader_t CommandHeader;
} CS_DisableAppsCmd_t;

/**
 * \brief No arguments command data type
 *
 *  For command details see #CS_NOOP_CC #CS_RESET_COUNTERS_CC, #CS_ENABLE_ALL_CS_CC,
 *  #CS_DISABLE_ALL_CS_CC, #CS_ENABLE_CFE_CORE_CC, #CS_DISABLE_CFE_CORE_CC, #CS_ENABLE_OS_CC,
 *  #CS_DISABLE_OS_CC, #CS_ENABLE_EEPROM_CC, #CS_DISABLE_EEPROM_CC, #CS_ENABLE_MEMORY_CC,
 *  #CS_DISABLE_MEMORY_CC, #CS_ENABLE_TABLES_CC, #CS_DISABLE_TABLES_CC
 *  #CS_ENABLE_APPS_CC, #CS_DISABLE_APPS_CC, #CS_CANCEL_ONE_SHOT_CC
 */
typedef struct
{
    CFE_MSG_CommandHeader_t CommandHeader;
} CS_EnableAppsCmd_t;

/**
 * \brief No arguments command data type
 *
 *  For command details see #CS_NOOP_CC #CS_RESET_COUNTERS_CC, #CS_ENABLE_ALL_CS_CC,
 *  #CS_DISABLE_ALL_CS_CC, #CS_ENABLE_CFE_CORE_CC, #CS_DISABLE_CFE_CORE_CC, #CS_ENABLE_OS_CC,
 *  #CS_DISABLE_OS_CC, #CS_ENABLE_EEPROM_CC, #CS_DISABLE_EEPROM_CC, #CS_ENABLE_MEMORY_CC,
 *  #CS_DISABLE_MEMORY_CC, #CS_ENABLE_TABLES_CC, #CS_DISABLE_TABLES_CC
 *  #CS_ENABLE_APPS_CC, #CS_DISABLE_APPS_CC, #CS_CANCEL_ONE_SHOT_CC
 */
typedef struct
{
    CFE_MSG_CommandHeader_t CommandHeader;
} CS_DisableEepromCmd_t;

/**
 * \brief No arguments command data type
 *
 *  For command details see #CS_NOOP_CC #CS_RESET_COUNTERS_CC, #CS_ENABLE_ALL_CS_CC,
 *  #CS_DISABLE_ALL_CS_CC, #CS_ENABLE_CFE_CORE_CC, #CS_DISABLE_CFE_CORE_CC, #CS_ENABLE_OS_CC,
 *  #CS_DISABLE_OS_CC, #CS_ENABLE_EEPROM_CC, #CS_DISABLE_EEPROM_CC, #CS_ENABLE_MEMORY_CC,
 *  #CS_DISABLE_MEMORY_CC, #CS_ENABLE_TABLES_CC, #CS_DISABLE_TABLES_CC
 *  #CS_ENABLE_APPS_CC, #CS_DISABLE_APPS_CC, #CS_CANCEL_ONE_SHOT_CC
 */
typedef struct
{
    CFE_MSG_CommandHeader_t CommandHeader;
} CS_EnableEepromCmd_t;

/**
 * \brief No arguments command data type
 *
 *  For command details see #CS_NOOP_CC #CS_RESET_COUNTERS_CC, #CS_ENABLE_ALL_CS_CC,
 *  #CS_DISABLE_ALL_CS_CC, #CS_ENABLE_CFE_CORE_CC, #CS_DISABLE_CFE_CORE_CC, #CS_ENABLE_OS_CC,
 *  #CS_DISABLE_OS_CC, #CS_ENABLE_EEPROM_CC, #CS_DISABLE_EEPROM_CC, #CS_ENABLE_MEMORY_CC,
 *  #CS_DISABLE_MEMORY_CC, #CS_ENABLE_TABLES_CC, #CS_DISABLE_TABLES_CC
 *  #CS_ENABLE_APPS_CC, #CS_DISABLE_APPS_CC, #CS_CANCEL_ONE_SHOT_CC
 */
typedef struct
{
    CFE_MSG_CommandHeader_t CommandHeader;
} CS_DisableMemoryCmd_t;

/**
 * \brief No arguments command data type
 *
 *  For command details see #CS_NOOP_CC #CS_RESET_COUNTERS_CC, #CS_ENABLE_ALL_CS_CC,
 *  #CS_DISABLE_ALL_CS_CC, #CS_ENABLE_CFE_CORE_CC, #CS_DISABLE_CFE_CORE_CC, #CS_ENABLE_OS_CC,
 *  #CS_DISABLE_OS_CC, #CS_ENABLE_EEPROM_CC, #CS_DISABLE_EEPROM_CC, #CS_ENABLE_MEMORY_CC,
 *  #CS_DISABLE_MEMORY_CC, #CS_ENABLE_TABLES_CC, #CS_DISABLE_TABLES_CC
 *  #CS_ENABLE_APPS_CC, #CS_DISABLE_APPS_CC, #CS_CANCEL_ONE_SHOT_CC
 */
typedef struct
{
    CFE_MSG_CommandHeader_t CommandHeader;
} CS_EnableMemoryCmd_t;

/**
 * \brief No arguments command data type
 *
 *  For command details see #CS_NOOP_CC #CS_RESET_COUNTERS_CC, #CS_ENABLE_ALL_CS_CC,
 *  #CS_DISABLE_ALL_CS_CC, #CS_ENABLE_CFE_CORE_CC, #CS_DISABLE_CFE_CORE_CC, #CS_ENABLE_OS_CC,
 *  #CS_DISABLE_OS_CC, #CS_ENABLE_EEPROM_CC, #CS_DISABLE_EEPROM_CC, #CS_ENABLE_MEMORY_CC,
 *  #CS_DISABLE_MEMORY_CC, #CS_ENABLE_TABLES_CC, #CS_DISABLE_TABLES_CC
 *  #CS_ENABLE_APPS_CC, #CS_DISABLE_APPS_CC, #CS_CANCEL_ONE_SHOT_CC
 */
typedef struct
{
    CFE_MSG_CommandHeader_t CommandHeader;
} CS_DisableTablesCmd_t;

/**
 * \brief No arguments command data type
 *
 *  For command details see #CS_NOOP_CC #CS_RESET_COUNTERS_CC, #CS_ENABLE_ALL_CS_CC,
 *  #CS_DISABLE_ALL_CS_CC, #CS_ENABLE_CFE_CORE_CC, #CS_DISABLE_CFE_CORE_CC, #CS_ENABLE_OS_CC,
 *  #CS_DISABLE_OS_CC, #CS_ENABLE_EEPROM_CC, #CS_DISABLE_EEPROM_CC, #CS_ENABLE_MEMORY_CC,
 *  #CS_DISABLE_MEMORY_CC, #CS_ENABLE_TABLES_CC, #CS_DISABLE_TABLES_CC
 *  #CS_ENABLE_APPS_CC, #CS_DISABLE_APPS_CC, #CS_CANCEL_ONE_SHOT_CC
 */
typedef struct
{
    CFE_MSG_CommandHeader_t CommandHeader;
} CS_EnableTablesCmd_t;

/**
 * \brief Get entry ID command
 *
 *  For command details see CS_GET_ENTRY_ID_EEPROM_CC, #CS_GET_ENTRY_ID_MEMORY_CC
 */
typedef struct
{
    CFE_MSG_CommandHeader_t    CommandHeader;
    CS_GetEntryIDCmd_Payload_t Payload;
} CS_GetEntryIDEepromCmd_t;

/**
 * \brief Get entry ID command
 *
 *  For command details see CS_GET_ENTRY_ID_EEPROM_CC, #CS_GET_ENTRY_ID_MEMORY_CC
 */
typedef struct
{
    CFE_MSG_CommandHeader_t    CommandHeader;
    CS_GetEntryIDCmd_Payload_t Payload;
} CS_GetEntryIDMemoryCmd_t;

/**
 * \brief Command type for commands using Memory or EEPROM tables
 *
 *  For command details see #CS_ENABLE_ENTRY_ID_EEPROM_CC, #CS_DISABLE_ENTRY_ID_EEPROM_CC,
 * #CS_ENABLE_ENTRY_ID_MEMORY_CC, #CS_DISABLE_ENTRY_ID_MEMORY_CC
 */
typedef struct
{
    CFE_MSG_CommandHeader_t CommandHeader;
    CS_EntryCmd_Payload_t   Payload;
} CS_ReportBaselineEntryIDEepromCmd_t;

/**
 * \brief Command type for commands using Memory or EEPROM tables
 *
 *  For command details see #CS_ENABLE_ENTRY_ID_EEPROM_CC, #CS_DISABLE_ENTRY_ID_EEPROM_CC,
 * #CS_ENABLE_ENTRY_ID_MEMORY_CC, #CS_DISABLE_ENTRY_ID_MEMORY_CC
 */
typedef struct
{
    CFE_MSG_CommandHeader_t CommandHeader;
    CS_EntryCmd_Payload_t   Payload;
} CS_DisableEntryIDEepromCmd_t;

/**
 * \brief Command type for commands using Memory or EEPROM tables
 *
 *  For command details see #CS_ENABLE_ENTRY_ID_EEPROM_CC, #CS_DISABLE_ENTRY_ID_EEPROM_CC,
 * #CS_ENABLE_ENTRY_ID_MEMORY_CC, #CS_DISABLE_ENTRY_ID_MEMORY_CC
 */
typedef struct
{
    CFE_MSG_CommandHeader_t CommandHeader;
    CS_EntryCmd_Payload_t   Payload;
} CS_RecomputeBaselineEepromCmd_t;

/**
 * \brief Command type for commands using Memory or EEPROM tables
 *
 *  For command details see #CS_ENABLE_ENTRY_ID_EEPROM_CC, #CS_DISABLE_ENTRY_ID_EEPROM_CC,
 * #CS_ENABLE_ENTRY_ID_MEMORY_CC, #CS_DISABLE_ENTRY_ID_MEMORY_CC
 */
typedef struct
{
    CFE_MSG_CommandHeader_t CommandHeader;
    CS_EntryCmd_Payload_t   Payload;
} CS_EnableEntryIDEepromCmd_t;

/**
 * \brief Command type for commands using Memory or EEPROM tables
 *
 *  For command details see #CS_ENABLE_ENTRY_ID_EEPROM_CC, #CS_DISABLE_ENTRY_ID_EEPROM_CC,
 * #CS_ENABLE_ENTRY_ID_MEMORY_CC, #CS_DISABLE_ENTRY_ID_MEMORY_CC
 */
typedef struct
{
    CFE_MSG_CommandHeader_t CommandHeader;
    CS_EntryCmd_Payload_t   Payload;
} CS_ReportBaselineEntryIDMemoryCmd_t;

/**
 * \brief Command type for commands using Memory or EEPROM tables
 *
 *  For command details see #CS_ENABLE_ENTRY_ID_EEPROM_CC, #CS_DISABLE_ENTRY_ID_EEPROM_CC,
 * #CS_ENABLE_ENTRY_ID_MEMORY_CC, #CS_DISABLE_ENTRY_ID_MEMORY_CC
 */
typedef struct
{
    CFE_MSG_CommandHeader_t CommandHeader;
    CS_EntryCmd_Payload_t   Payload;
} CS_DisableEntryIDMemoryCmd_t;

/**
 * \brief Command type for commands using Memory or EEPROM tables
 *
 *  For command details see #CS_ENABLE_ENTRY_ID_EEPROM_CC, #CS_DISABLE_ENTRY_ID_EEPROM_CC,
 * #CS_ENABLE_ENTRY_ID_MEMORY_CC, #CS_DISABLE_ENTRY_ID_MEMORY_CC
 */
typedef struct
{
    CFE_MSG_CommandHeader_t CommandHeader;
    CS_EntryCmd_Payload_t   Payload;
} CS_RecomputeBaselineMemoryCmd_t;

/**
 * \brief Command type for commands using Memory or EEPROM tables
 *
 *  For command details see #CS_ENABLE_ENTRY_ID_EEPROM_CC, #CS_DISABLE_ENTRY_ID_EEPROM_CC,
 * #CS_ENABLE_ENTRY_ID_MEMORY_CC, #CS_DISABLE_ENTRY_ID_MEMORY_CC
 */
typedef struct
{
    CFE_MSG_CommandHeader_t CommandHeader;
    CS_EntryCmd_Payload_t   Payload;
} CS_EnableEntryIDMemoryCmd_t;

/**
 * \brief Command type for commanding by table name
 *
 *  For command details see #CS_ENABLE_NAME_TABLE_CC, #CS_DISABLE_NAME_TABLE_CC,
 *  #CS_RECOMPUTE_BASELINE_TABLE_CC, #CS_REPORT_BASELINE_TABLE_CC
 */
typedef struct
{
    CFE_MSG_CommandHeader_t   CommandHeader;
    CS_TableNameCmd_Payload_t Payload;
} CS_ReportBaselineTableCmd_t;

/**
 * \brief Command type for commanding by table name
 *
 *  For command details see #CS_ENABLE_NAME_TABLE_CC, #CS_DISABLE_NAME_TABLE_CC,
 *  #CS_RECOMPUTE_BASELINE_TABLE_CC, #CS_REPORT_BASELINE_TABLE_CC
 */
typedef struct
{
    CFE_MSG_CommandHeader_t   CommandHeader;
    CS_TableNameCmd_Payload_t Payload;
} CS_RecomputeBaselineTableCmd_t;

/**
 * \brief Command type for commanding by table name
 *
 *  For command details see #CS_ENABLE_NAME_TABLE_CC, #CS_DISABLE_NAME_TABLE_CC,
 *  #CS_RECOMPUTE_BASELINE_TABLE_CC, #CS_REPORT_BASELINE_TABLE_CC
 */
typedef struct
{
    CFE_MSG_CommandHeader_t   CommandHeader;
    CS_TableNameCmd_Payload_t Payload;
} CS_DisableNameTableCmd_t;

/**
 * \brief Command type for commanding by table name
 *
 *  For command details see #CS_ENABLE_NAME_TABLE_CC, #CS_DISABLE_NAME_TABLE_CC,
 *  #CS_RECOMPUTE_BASELINE_TABLE_CC, #CS_REPORT_BASELINE_TABLE_CC
 */
typedef struct
{
    CFE_MSG_CommandHeader_t   CommandHeader;
    CS_TableNameCmd_Payload_t Payload;
} CS_EnableNameTableCmd_t;

/**
 * \brief Command type for commanding by app name
 *
 *  For command details see e #CS_ENABLE_NAME_APP_CC, #CS_DISABLE_NAME_APP_CC,
 *  #CS_RECOMPUTE_BASELINE_APP_CC, #CS_REPORT_BASELINE_APP_CC
 */
typedef struct
{
    CFE_MSG_CommandHeader_t CommandHeader;
    CS_AppNameCmd_Payload_t Payload;
} CS_ReportBaselineAppCmd_t;

/**
 * \brief Command type for commanding by app name
 *
 *  For command details see e #CS_ENABLE_NAME_APP_CC, #CS_DISABLE_NAME_APP_CC,
 *  #CS_RECOMPUTE_BASELINE_APP_CC, #CS_REPORT_BASELINE_APP_CC
 */
typedef struct
{
    CFE_MSG_CommandHeader_t CommandHeader;
    CS_AppNameCmd_Payload_t Payload;
} CS_RecomputeBaselineAppCmd_t;

/**
 * \brief Command type for commanding by app name
 *
 *  For command details see e #CS_ENABLE_NAME_APP_CC, #CS_DISABLE_NAME_APP_CC,
 *  #CS_RECOMPUTE_BASELINE_APP_CC, #CS_REPORT_BASELINE_APP_CC
 */
typedef struct
{
    CFE_MSG_CommandHeader_t CommandHeader;
    CS_AppNameCmd_Payload_t Payload;
} CS_DisableNameAppCmd_t;

/**
 * \brief Command type for commanding by app name
 *
 *  For command details see e #CS_ENABLE_NAME_APP_CC, #CS_DISABLE_NAME_APP_CC,
 *  #CS_RECOMPUTE_BASELINE_APP_CC, #CS_REPORT_BASELINE_APP_CC
 */
typedef struct
{
    CFE_MSG_CommandHeader_t CommandHeader;
    CS_AppNameCmd_Payload_t Payload;
} CS_EnableNameAppCmd_t;

/**
 * \brief Command type for sending one shot calculation
 *
 *  For command details see #CS_ONE_SHOT_CC
 */
typedef struct
{
    CFE_MSG_CommandHeader_t CommandHeader;
    CS_OneShotCmd_Payload_t Payload;
} CS_OneShotCmd_t;

/**\}*/

#endif
