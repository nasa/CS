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
#ifndef CS_MSG_H
#define CS_MSG_H

#include "cfe.h"

/**
 * \defgroup cfscstlm CFS Checksum Telemetry
 * \{
 */

/**
 *  \brief Housekeeping Packet Structure
 */
typedef struct
{
    CFE_MSG_TelemetryHeader_t TlmHeader; /**< \brief cFE SB Tlm Msg Hdr */

    uint8   CmdCounter;                  /**< \brief CS Application Command Counter */
    uint8   CmdErrCounter;               /**< \brief CS Application Command Error Counter */
    uint8   ChecksumState;               /**< \brief CS Application global checksum state */
    uint8   EepromCSState;               /**< \brief CS Eeprom table checksum state */
    uint8   MemoryCSState;               /**< \brief CS Memory table checksum state */
    uint8   AppCSState;                  /**< \brief CS App table checksum state */
    uint8   TablesCSState;               /**< \brief CS Tables table checksum state */
    uint8   OSCSState;                   /**< \brief OS code segment checksum state */
    uint8   CfeCoreCSState;              /**< \brief cFE Core code segment checksum stat e*/
    uint8   RecomputeInProgress;         /**< \brief CS "Recompute In Progress" flag */
    uint8   OneShotInProgress;           /**< \brief CS "OneShot In Progress" flag */
    uint8   Filler8;                     /**< \brief 8 bit padding */
    uint16  EepromCSErrCounter;          /**< \brief Eeprom miscompare counter */
    uint16  MemoryCSErrCounter;          /**< \brief Memory miscompare counter */
    uint16  AppCSErrCounter;             /**< \brief App miscompare counter */
    uint16  TablesCSErrCounter;          /**< \brief Tables miscompare counter */
    uint16  CfeCoreCSErrCounter;         /**< \brief cFE core miscompare counter */
    uint16  OSCSErrCounter;              /**< \brief OS code segment miscopmare counter */
    uint16  CurrentCSTable;              /**< \brief Current table being checksummed */
    uint16  CurrentEntryInTable;         /**< \brief Current entry ID in table being checksummed */
    uint32  EepromBaseline;              /**< \brief Baseline checksum for all of Eeprom */
    uint32  OSBaseline;                  /**< \brief Baseline checksum for the OS code segment */
    uint32  CfeCoreBaseline;             /**< \brief Basline checksum for the cFE core */
    cpuaddr LastOneShotAddress;          /**< \brief Address used in last one shot checksum command */
    uint32  LastOneShotSize;             /**< \brief Size used in the last one shot checksum command */
    uint32  LastOneShotMaxBytesPerCycle; /**< \brief Max bytes per cycle for last one shot checksum command */
    uint32  LastOneShotChecksum;         /**< \brief Checksum of the last one shot checksum command */
    uint32  PassCounter;                 /**< \brief Number of times CS has passed through all of its tables */
} CS_HkPacket_t;

/**\}*/

/**
 * \defgroup cfscscmdstructs CFS Checksum Command Structures
 * \{
 */

/**
 * \brief No arguments command data type
 *
 *  For command details see #CS_NOOP_CC #CS_RESET_CC, #CS_ENABLE_ALL_CS_CC,
 *  #CS_DISABLE_ALL_CS_CC, #CS_ENABLE_CFECORE_CC, #CS_DISABLE_CFECORE_CC, #CS_ENABLE_OS_CC,
 *  #CS_DISABLE_OS_CC, #CS_ENABLE_EEPROM_CC, #CS_DISABLE_EEPROM_CC, #CS_ENABLE_MEMORY_CC,
 *  #CS_DISABLE_MEMORY_CC, #CS_ENABLE_TABLES_CC, #CS_DISABLE_TABLES_CC
 *  #CS_ENABLE_APPS_CC, #CS_DISABLE_APPS_CC, #CS_CANCEL_ONESHOT_CC
 */
typedef struct
{
    CFE_MSG_CommandHeader_t CmdHeader;
} CS_NoArgsCmd_t;

/**
 * \brief Get entry ID command
 *
 *  For command details see CS_GET_ENTRY_ID_EEPROM_CC, #CS_GET_ENTRY_ID_MEMORY_CC
 */
typedef struct
{
    CFE_MSG_CommandHeader_t CmdHeader;
    cpuaddr                 Address; /**< \brief Address to get the ID for */
} CS_GetEntryIDCmd_t;

/**
 * \brief Command type for commands using Memory or Eeprom tables
 *
 *  For command details see #CS_ENABLE_ENTRY_EEPROM_CC, #CS_DISABLE_ENTRY_EEPROM_CC,
 * #CS_ENABLE_ENTRY_MEMORY_CC, #CS_DISABLE_ENTRY_MEMORY_CC
 */
typedef struct
{
    CFE_MSG_CommandHeader_t CmdHeader;
    uint32                  EntryID; /**< \brief EntryID to perform a command on */
} CS_EntryCmd_t;

/**
 * \brief Command type for commanding by table name
 *
 *  For command details see #CS_ENABLE_NAME_TABLE_CC, #CS_DISABLE_NAME_TABLE_CC,
 *  #CS_RECOMPUTE_BASELINE_TABLE_CC, #CS_REPORT_BASELINE_TABLE_CC
 */
typedef struct
{
    CFE_MSG_CommandHeader_t CmdHeader;
    char                    Name[CFE_TBL_MAX_FULL_NAME_LEN]; /**< \brief Table name to perform a command on */
} CS_TableNameCmd_t;

/**
 * \brief Command type for commanding by app name
 *
 *  For command details see e #CS_ENABLE_NAME_APP_CC, #CS_DISABLE_NAME_APP_CC,
 *  #CS_RECOMPUTE_BASELINE_APP_CC, #CS_REPORT_BASELINE_APP_CC
 */
typedef struct
{
    CFE_MSG_CommandHeader_t CmdHeader;
    char                    Name[OS_MAX_API_NAME]; /**< \brief App name to perform a command on */
} CS_AppNameCmd_t;

/**
 * \brief Command type for sending one shot calculation
 *
 *  For command details see #CS_ONESHOT_CC
 */
typedef struct
{
    CFE_MSG_CommandHeader_t CmdHeader;
    cpuaddr                 Address; /**< \brief Address to start checksum */
    uint32                  Size;    /**< \brief Number of bytes to checksum */
    uint32 MaxBytesPerCycle; /**< \brief Max Number of bytes to compute per cycle. Value of Zero to use platform config
                                value */
} CS_OneShotCmd_t;

/**\}*/

#endif
