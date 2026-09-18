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
 *   Specification for the CFS Checksum tables.
 */
#ifndef DEFAULT_CS_TBLDEFS_H
#define DEFAULT_CS_TBLDEFS_H

/**************************************************************************
 **
 ** Include section
 **
 **************************************************************************/
#include "cfe_mission_cfg.h"
#include "cfe_es_extern_typedefs.h"
#include "cfe_tbl_extern_typedefs.h"
#include "cs_extern_typedefs.h"

/**************************************************************************
 **
 ** Type definitions
 **
 **************************************************************************/

/**
 * \brief Data structure for the EEPROM or Memory definition table
 */
typedef struct
{
    CFE_ES_MemAddress_t StartAddress;       /**< \brief The Start address to Checksum */
    uint16              State;              /**< \brief Uses the CS_STATE_... defines from above */
    uint16              Filler16;           /** <\brief Padding */
    uint32              NumBytesToChecksum; /**< \brief The number of Bytes to Checksum */
} CS_Def_EepromMemory_Table_Entry_t;

/**
 * \brief Data structure for the EEPROM or Memory results table
 */
typedef struct
{
    CFE_ES_MemAddress_t StartAddress;       /**< \brief The Start address to Checksum */
    uint16              State;              /**< \brief Uses the CS_STATE_... defines from above */
    uint16              ComputedYet;        /**< \brief Have we computed an Integrity value yet */
    uint32              NumBytesToChecksum; /**< \brief The number of Bytes to Checksum */
    uint32              ComparisonValue;    /**< \brief The Memory Integrity Value */
    uint32              ByteOffset;         /**< \brief Where a previous unfinished calc left off */
    uint32              TempChecksumValue;  /**< \brief The unfinished calculation */
    uint32              Filler32;           /**< \brief Padding */
} CS_Res_EepromMemory_Table_Entry_t;

/**
 * \brief Data structure for the Tables definition table
 */
typedef struct
{
    uint16 State;                                   /**< \brief Uses the CS_STATE_... defines from above */
    char   Name[CFE_MISSION_TBL_MAX_FULL_NAME_LEN]; /**< \brief name of the table */
} CS_Def_Tables_Table_Entry_t;

/**
 * \brief Data structure for the App definition table
 */
typedef struct
{
    uint16 State;                         /**< \brief Uses the CS_STATE_... defines from above */
    char   Name[CFE_MISSION_MAX_API_LEN]; /**< \brief name of the app */
} CS_Def_App_Table_Entry_t;

/**
 * \brief Data structure for the Tables result table
 */
typedef struct
{
    CFE_ES_MemAddress_t StartAddress;       /**< \brief The Start address to Checksum */
    uint16              State;              /**< \brief Uses the CS_STATE_... defines from above */
    uint16              ComputedYet;        /**< \brief Have we computed an Integrity value yet */
    uint32              NumBytesToChecksum; /**< \brief The number of Bytes to Checksum */
    uint32              ComparisonValue;    /**< \brief The Memory Integrity Value */
    uint32              ByteOffset;         /**< \brief Where a previous unfinished calc left off */
    uint32              TempChecksumValue;  /**< \brief The unfinished calculation */
    CFE_TBL_HandleId_t  TblHandleID;        /**< \brief handle received from CFE_TBL */
    bool                IsCSOwner;          /**< \brief Is CS the original owner of this table */
    bool                Filler8;            /**< \brief Padding */
    char                Name[CFE_MISSION_TBL_MAX_FULL_NAME_LEN]; /**< \brief name of the table */
} CS_Res_Tables_Table_Entry_t;

/**
 * \brief Data structure for the app result table
 */
typedef struct
{
    CFE_ES_MemAddress_t StartAddress;                  /**< \brief The Start address to Checksum */
    uint16              State;                         /**< \brief Uses the CS_STATE_... defines from above */
    uint16              ComputedYet;                   /**< \brief Have we computed an Integrity value yet */
    uint32              NumBytesToChecksum;            /**< \brief The number of Bytes to Checksum */
    uint32              ComparisonValue;               /**< \brief The Memory Integrity Value */
    uint32              ByteOffset;                    /**< \brief Where a previous unfinished calc left off */
    uint32              TempChecksumValue;             /**< \brief The unfinished calculation */
    char                Name[CFE_MISSION_MAX_API_LEN]; /**< \brief name of the app */
} CS_Res_App_Table_Entry_t;

#endif
