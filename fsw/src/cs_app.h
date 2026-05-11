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
 *   Unit specification for the Core Flight System (CFS)
 *   Checksum (CS) Application.
 */
#ifndef CS_APP_H
#define CS_APP_H

/**************************************************************************
**
** Include section
**
**************************************************************************/

#include "cfe.h"
#include "cs_tbl.h"
#include "cs_msg.h"
#include "cs_platform_cfg.h"
#include "cs_mission_cfg.h"
#include "cs_msgids.h"
#include "cs_perfids.h"
#include "cs_verify.h"
#include "cs_version.h"
#include "cs_table_processing.h"

/**************************************************************************
 **
 ** Macro definitions
 **
 **************************************************************************/

/**
 * \name CS Error Codes
 * \{
 */
#define CS_ERROR         (-1) /**< \brief Error code returned when a checksum compare failed */
#define CS_ERR_NOT_FOUND (-2) /**< \brief Error code returned the app or table requested could not be found */
#define CS_TABLE_ERROR   (-3) /**< \brief Error code returned on table validation error */
/**\}*/

/**
 * \name CS Command Pipe Parameters
 * \{
 */
#define CS_CMD_PIPE_NAME "CS_CMD_PIPE"
/**\}*/

/**
 * \name CS Child Task Names
 * \{
 */
#define CS_RECOMP_OS_TASK_NAME      "CS_RecmpOSTsk"
#define CS_RECOMP_CFECORE_TASK_NAME "CS_RecmpCfeCoreTsk"
#define CS_RECOMP_MEMORY_TASK_NAME  "CS_RecmpMemoryTsk"
#define CS_RECOMP_EEPROM_TASK_NAME  "CS_RecmpEepromTsk"
#define CS_RECOMP_APP_TASK_NAME     "CS_RecmpAppTsk"
#define CS_RECOMP_TABLES_TASK_NAME  "CS_RecmpTableTsk"
#define CS_ONESHOT_TASK_NAME        "CS_OneShotTask"
/**\}*/

/**
 * \brief Wakeup for CS
 *
 * \par Description
 *      Wakes up CS every 1 second for routine maintenance whether a
 *      message was received or not.
 */
#define CS_WAKEUP_TIMEOUT 1000

/*************************************************************************
 **
 ** Type definitions
 **
 **************************************************************************/

/**
 *  \brief CS global data structure
 */
typedef struct
{
    CS_HkPacket_t HkPacket; /**< \brief Housekeeping telemetry packet */

    CS_ChecksumType_Enum_t ChildTaskTable;   /**< \brief Table for the child task to process */
    uint16                 ChildTaskEntryID; /**< \brief Entry in table for child task to process */
    CFE_ES_TaskId_t        ChildTaskID;      /**< \brief Task ID for the child task */

    size_t MaxBytesPerCycle; /**< \brief Max number of bytes to process in a cycle */
    uint32 RunStatus;        /**< \brief Application run status */

    CS_Res_EepromMemory_Table_Entry_t *RecomputeEepromMemoryEntryPtr; /**< \brief Pointer to an entry to recompute in
                                                                         the EEPROM or Memory table */

    CS_Res_App_Table_Entry_t *RecomputeAppEntryPtr; /**< \brief Pointer to an entry to recompute in the
                                                                Application table */

    CS_Res_Tables_Table_Entry_t *RecomputeTablesEntryPtr; /**< \brief Pointer to an entry to recompute in the
                                                                      Tables table */

    CFE_SB_PipeId_t CmdPipe; /**< \brief Command pipe ID */

    CS_TableWrapper_t Tbl[CS_NUM_TABLES];

    CS_Res_EepromMemory_Table_Entry_t OSCodeSeg;      /**< \brief OS code segment 'table' */
    CS_Res_EepromMemory_Table_Entry_t CfeCoreCodeSeg; /**< \brief cFE core code segment 'table' */

    CS_Def_EepromMemory_Table_Entry_t
        DefaultEepromDefTable[CS_MAX_NUM_EEPROM_TABLE_ENTRIES]; /**< \brief Default EEPROM definition table */
    CS_Def_EepromMemory_Table_Entry_t
        DefaultMemoryDefTable[CS_MAX_NUM_MEMORY_TABLE_ENTRIES]; /**< \brief Default Memory definition table */
    CS_Def_Tables_Table_Entry_t
        DefaultTablesDefTable[CS_MAX_NUM_TABLES_TABLE_ENTRIES]; /**< \brief Default Tables definition table */
    CS_Def_App_Table_Entry_t
        DefaultAppDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES]; /**< \brief Default Apps definition table */

    /* The following pointers locate the results for CS tables that get checksummed because they are listed in the CS
     * Tables table */

#if (CS_PRESERVE_STATES_ON_PROCESSOR_RESET == true)
    CFE_ES_CDSHandle_t DataStoreHandle; /**< \brief Handle to critical data store created by CS */
#endif
} CS_AppData_t;

/**************************************************************************
 **
 ** Exported data
 **
 **************************************************************************/

/**
 * \brief Extern the CS_AppData so all CS files can use it
 */
extern CS_AppData_t CS_AppData;

/**************************************************************************
 **
 **  Function Prototypes
 **
 **************************************************************************/

/**
 * \brief Method to get the table ID from the wrapper pointer
 *
 * \returns ID of table
 */
static inline CS_ChecksumType_Enum_t CS_GetTableId(const CS_TableWrapper_t *tw)
{
    return (tw - CS_AppData.Tbl);
}

/**
 * \brief Checks if the table wrapper refers to the given table ID
 * \param tw Pointer to table wrapper struct
 * \param TableId Identifier of table to check if match to
 * \retval true if match
 * \retval false if not a match
 */
static inline bool CS_CheckTableId(const CS_TableWrapper_t *tw, CS_ChecksumType_Enum_t TableId)
{
    return (tw == &CS_AppData.Tbl[TableId]);
}

/**
 * \brief Gets the definition table handle for the given table ID
 * \param TableId Identifier of table
 * \returns Definition table handle
 */
static inline CFE_TBL_Handle_t CS_GetDefHandle(CS_ChecksumType_Enum_t TableId)
{
    return CS_AppData.Tbl[TableId].DefHandle;
}

/**
 * \brief Gets the result table handle for the given table ID
 * \param TableId Identifier of table
 * \returns Result table handle
 */
static inline CFE_TBL_Handle_t CS_GetResHandle(CS_ChecksumType_Enum_t TableId)
{
    return CS_AppData.Tbl[TableId].ResHandle;
}

/**
 * \brief Gets the definition table name handle for the given table ID
 * \param TableId Identifier of table
 * \returns Pointer to definition table name
 * \retval  NULL if no name defined
 */
static inline const char *CS_GetDefName(CS_ChecksumType_Enum_t TableId)
{
    return CS_AppData.Tbl[TableId].DefTableName;
}

/**
 * \brief Gets the result table name handle for the given table ID
 * \param TableId Identifier of table
 * \returns Pointer to result table name
 * \retval  NULL if no name defined
 */
static inline const char *CS_GetResName(CS_ChecksumType_Enum_t TableId)
{
    return CS_AppData.Tbl[TableId].ResTableName;
}

/**
 * \brief Gets the definition table pointer for the given table ID
 * \param TableId Identifier of table
 * \returns Pointer to definition data
 * \retval  NULL if no definition table registered or loaded
 */
static inline const void *CS_GetDefAddr(CS_ChecksumType_Enum_t TableId)
{
    return CS_AppData.Tbl[TableId].DefAddr;
}

/**
 * \brief Gets the result table pointer for the given table ID
 * \param TableId Identifier of table
 * \returns Pointer to result data
 * \retval  NULL if no result table registered
 */
static inline void *CS_GetResAddr(CS_ChecksumType_Enum_t TableId)
{
    return CS_AppData.Tbl[TableId].ResAddr;
}

/**
 * \brief Gets the result table pointer for OSAL code segment
 * \returns Pointer to OSAL segment result data
 * \retval  NULL if no OSAL segment result registered
 */
static inline CS_Res_EepromMemory_Table_Entry_t *CS_GetOSCodeSegResTable(void)
{
    return (CS_Res_EepromMemory_Table_Entry_t *)CS_GetResAddr(CS_ChecksumType_OSCORE);
}

/**
 * \brief Gets the result table pointer for CFE core code segment
 * \returns Pointer to CFE core segment result data
 * \retval  NULL if no CFE core segment result registered
 */
static inline CS_Res_EepromMemory_Table_Entry_t *CS_GetCfeCoreCodeSegResTable(void)
{
    return (CS_Res_EepromMemory_Table_Entry_t *)CS_GetResAddr(CS_ChecksumType_CFECORE);
}

/**
 * \brief Gets the definition table pointer for EEPROM checksums
 * \returns Pointer to EEPROM checksum definition data
 * \retval  NULL if no EEPROM checksum definition registered
 */
static inline CS_Def_EepromMemory_Table_Entry_t *CS_GetEepromDefTable(void)
{
    return (CS_Def_EepromMemory_Table_Entry_t *)CS_GetDefAddr(CS_ChecksumType_EEPROM_TABLE);
}

/**
 * \brief Gets the result table pointer for EEPROM checksums
 * \returns Pointer to EEPROM checksum result data
 * \retval  NULL if no EEPROM checksum result registered
 */
static inline CS_Res_EepromMemory_Table_Entry_t *CS_GetEepromResTable(void)
{
    return (CS_Res_EepromMemory_Table_Entry_t *)CS_GetResAddr(CS_ChecksumType_EEPROM_TABLE);
}

/**
 * \brief Gets the definition table pointer for Memory checksums
 * \returns Pointer to Memory checksum definition data
 * \retval  NULL if no Memory checksum definition registered
 */
static inline CS_Def_EepromMemory_Table_Entry_t *CS_GetMemoryDefTable(void)
{
    return (CS_Def_EepromMemory_Table_Entry_t *)CS_GetDefAddr(CS_ChecksumType_MEMORY_TABLE);
}

/**
 * \brief Gets the result table pointer for Memory checksums
 * \returns Pointer to Memory checksum result data
 * \retval  NULL if no Memory checksum result registered
 */
static inline CS_Res_EepromMemory_Table_Entry_t *CS_GetMemoryResTable(void)
{
    return (CS_Res_EepromMemory_Table_Entry_t *)CS_GetResAddr(CS_ChecksumType_MEMORY_TABLE);
}

/**
 * \brief Gets the definition table pointer for Table checksums
 * \returns Pointer to Table checksum definition data
 * \retval  NULL if no Table checksum definition registered
 */
static inline CS_Def_Tables_Table_Entry_t *CS_GetTablesDefTable(void)
{
    return (CS_Def_Tables_Table_Entry_t *)CS_GetDefAddr(CS_ChecksumType_TABLES_TABLE);
}

/**
 * \brief Gets the result table pointer for Table checksums
 * \returns Pointer to Table checksum result data
 * \retval  NULL if no Table checksum result registered
 */
static inline CS_Res_Tables_Table_Entry_t *CS_GetTablesResTable(void)
{
    return (CS_Res_Tables_Table_Entry_t *)CS_GetResAddr(CS_ChecksumType_TABLES_TABLE);
}

/**
 * \brief Gets the definition table pointer for App checksums
 * \returns Pointer to App checksum definition data
 * \retval  NULL if no App checksum definition registered
 */
static inline CS_Def_App_Table_Entry_t *CS_GetAppDefTable(void)
{
    return (CS_Def_App_Table_Entry_t *)CS_GetDefAddr(CS_ChecksumType_APP_TABLE);
}

/**
 * \brief Gets the result table pointer for App checksums
 * \returns Pointer to App checksum result data
 * \retval  NULL if no App checksum result registered
 */
static inline CS_Res_App_Table_Entry_t *CS_GetAppResTable(void)
{
    return (CS_Res_App_Table_Entry_t *)CS_GetResAddr(CS_ChecksumType_APP_TABLE);
}

/**
 * \brief CFS Checksum (CS) application entry point
 *
 *  \par Description
 *       Checksum application entry point and main process loop.
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 */
void CS_AppMain(void);

/**
 * \brief CFS Checksum (CS) Critical Data Store Update
 *
 *  \par Description
 *       Checksum application entry point and main process loop.
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 */
void CS_UpdateCDS(void);

/**
 * \brief Initialize the Checksum CFS application
 *
 *  \par Description
 *       Checksum application initialization routine. This
 *       function performs all the required startup steps to
 *       get the application registered with the cFE services so
 *       it can begin to receive command messages and begin
 *       background checksumming.
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 * \return Execution status, see \ref CFEReturnCodes
 * \retval #CFE_SUCCESS \copybrief CFE_SUCCESS
 */
CFE_Status_t CS_AppInit(void);

/**
 * \brief Restore tables states from CDS if enabled
 *
 *  \par Description
 *       Restore CS state of tables from CDS
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 * \return Execution status, see \ref CFEReturnCodes
 * \retval #CFE_SUCCESS \copybrief CFE_SUCCESS
 */
CFE_Status_t CS_CreateRestoreStatesFromCDS(void);

#endif
