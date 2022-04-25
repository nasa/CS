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
 *   Contains CFS Checksum macros that run preprocessor checks
 *   on mission configurable parameters
 */
#ifndef CS_VERIFY_H
#define CS_VERIFY_H

/*************************************************************************
** Includes
*************************************************************************/
#include "cs_platform_cfg.h"
#include "cs_mission_cfg.h"
#include <stdint.h>

/*************************************************************************
** Macro Definitions
*************************************************************************/

#if CS_MAX_NUM_EEPROM_TABLE_ENTRIES > 65535
#error CS_MAX_NUM_EEPROM_TABLE_ENTRIES cannot be greater than 65535!
#endif

#if CS_MAX_NUM_MEMORY_TABLE_ENTRIES > 65535
#error CS_MAX_NUM_MEMORY_TABLE_ENTRIES cannot be greater than 65535!
#endif

/*
 * JPH 2015-06-29 - Removed checks of:
 *  CS_MAX_NUM_APP_TABLE_ENTRIES > CFE_ES_MAX_APPLICATIONS
 *  CS_MAX_NUM_TABLES_TABLE_ENTRIES > CFE_TBL_MAX_NUM_TABLES
 *
 * These are not valid checks anymore, as the CS app does not have knowledge
 * of either CFE_ES_MAX_APPLICATIONS nor CFE_TBL_MAX_NUM_TABLES.  Also, if
 * these actually violate the rule, this will either show up as an obvious
 * run-time error OR it will still work perfectly fine.
 */

#if (CS_MAX_NUM_EEPROM_TABLE_ENTRIES < 1)
#error CS_MAX_NUM_EEPROM_TABLE_ENTRIES must be at least 1!
#endif

#if (CS_MAX_NUM_MEMORY_TABLE_ENTRIES < 1)
#error CS_MAX_NUM_MEMORY_TABLE_ENTRIES must be at least 1!
#endif

#if (CS_MAX_NUM_TABLES_TABLE_ENTRIES < 1)
#error CS_MAX_NUM_TABLES_TABLE_ENTRIES must be at least 1!
#endif

#if (CS_MAX_NUM_APP_TABLE_ENTRIES < 1)
#error CS_MAX_NUM_APP_TABLE_ENTRIES must be at least 1!
#endif

#if (CS_DEFAULT_BYTES_PER_CYCLE > 0xFFFFFFFF)
#error CS_DEFAULT_BYTES_PER_CYCLE cannot be greater than 0xFFFFFFFF!
#endif

#if (CS_DEFAULT_BYTES_PER_CYCLE < 0)
#error CS_DEFAULT_BYTES_PER_CYCLE cannot be less than 0!
#endif

#if (CS_CHILD_TASK_PRIORITY < 1)
#error CS_CHILD_TASK_PRIORITY must be greater than 0!
#endif

#if (CS_CHILD_TASK_PRIORITY > 255)
#error CS_CHILD_TASK_PRIORITY cannot be greater than 255!
#endif

#if (CS_DEFAULT_ALGORITHM != CFE_MISSION_ES_DEFAULT_CRC && CS_DEFAULT_ALGORITHM != CFE_ES_CRC_16)
#error CS_DEFAULT_ALGORITHM is limited to either CFE_MISSION_ES_DEFAULT_CRC or CFE_ES_CRC_16!
#endif

#endif
