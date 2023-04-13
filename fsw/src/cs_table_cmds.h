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
 *   Specification for the CFS table cmds
 */
#ifndef CS_TABLE_CMDS_H
#define CS_TABLE_CMDS_H

/**************************************************************************
 **
 ** Include section
 **
 **************************************************************************/
#include "cfe.h"
#include "cs_msg.h"

/**
 * \brief Process a disable background checking for the Tables
 *        table command
 *
 *  \par Description
 *       Disables background checking for the Tables table
 *
 *  \par Assumptions, External Events, and Notes:
 *       In order for background checking of individual areas
 *       to checksum (OS code segment, cFE core, EEPROM, Memory,
 *       Apps, and Tables) to occur, the table must be enabled
 *       and overall checksumming must be enabled.
 *
 *  \param[in] CmdPtr Command pointer, verified non-null in CS_AppMain
 *
 *  \sa #CS_DISABLE_TABLES_CC
 */
void CS_DisableTablesCmd(const CS_NoArgsCmd_t *CmdPtr);

/**
 * \brief Process an enable background checking for the Tables
 *        table command
 *
 *  \par Description
 *       Allows the Tables table to be background checksummed.
 *
 *  \par Assumptions, External Events, and Notes:
 *       In order for background checking of individual areas
 *       to checksum (OS code segment, cFE core, EEPROM, Memory,
 *       Apps, and Tables) to occur, the table must be enabled
 *       and overall checksumming must be enabled.
 *
 *  \param[in] CmdPtr Command pointer, verified non-null in CS_AppMain
 *
 *  \sa #CS_ENABLE_TABLES_CC
 */
void CS_EnableTablesCmd(const CS_NoArgsCmd_t *CmdPtr);

/**
 * \brief Proccess a report baseline of a Table command
 *
 *  \par Description
 *        Reports the baseline checksum of the specified table
 *        if it has already been computed
 *
 *  \par Assumptions, External Events, and Notes:
 *        None
 *
 *  \param[in] CmdPtr Command pointer, verified non-null in CS_AppMain
 *
 *  \sa #CS_REPORT_BASELINE_TABLE_CC
 */
void CS_ReportBaselineTablesCmd(const CS_TableNameCmd_t *CmdPtr);

/**
 * \brief Process a recompute baseline of a Table command
 *
 *  \par Description
 *        Recomputes the checksum of a table and use that
 *        value as the new baseline for that entry.
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param[in] CmdPtr Command pointer, verified non-null in CS_AppMain
 *
 *  \sa #CS_RECOMPUTE_BASELINE_TABLE_CC
 */
void CS_RecomputeBaselineTablesCmd(const CS_TableNameCmd_t *CmdPtr);

/**
 * \brief Process a disable background checking for a Table
 *        entry command
 *
 *  \par Description
 *       Disables the specified Tables entry to be background checksummed.
 *
 *  \par Assumptions, External Events, and Notes:
 *       In order for background checking of individual entries
 *       to checksum to occur, the entry must be enabled,
 *       the table must be enabled, and overall checksumming must be enabled.
 *       This command updates both the results table and the definition table.
 *       If the entry exists in the results table but not in the definition
 *       table, the command is still successful, but the definition table
 *       is not updated.  If the entry does not exist in the results table,
 *       neither table is updated.
 *
 *  \param[in] CmdPtr Command pointer, verified non-null in CS_AppMain
 *
 *  \sa #CS_DISABLE_NAME_TABLE_CC
 */
void CS_DisableNameTablesCmd(const CS_TableNameCmd_t *CmdPtr);

/**
 * \brief Process an enable background checking for a Table
 *        entry command
 *
 *  \par Description
 *       Allows the specified table to be background checksummed.
 *
 *  \par Assumptions, External Events, and Notes:
 *       In order for background checking of individual entries
 *       to checksum to occur, the entry must be enabled,
 *       the table must be enabled and, overall checksumming must be enabled.
 *       This command updates both the results table and the definition table.
 *       If the entry exists in the results table but not in the definition
 *       table, the command is still successful, but the definition table
 *       is not updated.  If the entry does not exist in the results table,
 *       neither table is updated.
 *
 *  \param[in] CmdPtr Command pointer, verified non-null in CS_AppMain
 *
 *  \sa #CS_ENABLE_NAME_TABLE_CC
 */
void CS_EnableNameTablesCmd(const CS_TableNameCmd_t *CmdPtr);

#endif
