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
 *   Specification for the CFS computation functions.
 */
#ifndef CS_COMPUTE_H
#define CS_COMPUTE_H

/**************************************************************************
 **
 ** Include section
 **
 **************************************************************************/
#include "cfe.h"
#include "cs_tbldefs.h"

/**
 * \brief Computes checksums on Eeprom or Memory types
 *
 *  \par Description
 *       Computes checksums up to MaxBytesPerCycle bytes every call. This
 *       function is used to compute checksums for Eeprom, Memory, the
 *       OS code segment and the cFE core code segment
 *
 *
 *  \par Assumptions, External Events, and Notes:
 *        None
 *
 *  \param [in]   *ResultsEntry        A pointer to the entry in a table
 *                                     that we want to compute the
 *                                     checksum on.  Verified non-null by
 *                                     calling function.
 *
 *  \param [in]   ComputedCSValue      A pointer to the computed checksum
 *                                     that will be assigned the checksum of
 *                                     the entry.  Verified non-null by
 *                                     calling function.
 *
 *  \param [in]   DoneWithEntry        A pointer to a bool that will be
 *                                     assigned a value based on whether or
 *                                     not the specified entry's checksum
 *                                     was completed.  Verified non-null by
 *                                     calling function.
 *
 *  \param [out]  *ComputedCSValue     Value used to determine the computed
 *                                     checksum, if completed
 *
 *  \param [out]  *DoneWithEntry       Value that specifies whether or not
 *                                     the specified entry's checksum was
 *                                     completed during this call.
 *
 * \return Execution status
 * \retval #CS_SUCCESS \copybrief CS_SUCCESS
 * \retval #CS_ERROR   \copybrief CS_ERROR
 */
int32 CS_ComputeEepromMemory(CS_Res_EepromMemory_Table_Entry_t *ResultsEntry, uint32 *ComputedCSValue,
                             bool *DoneWithEntry);

/**
 * \brief Computes checksums on tables
 *
 *  \par Description
 *       Computes checksums up to MaxBytesPerCycle bytes every call. This
 *       function is used to compute checksums for tables.
 *
 *  \par Assumptions, External Events, and Notes:
 *        None
 *
 *  \param [in]   *ResultsEntry        A pointer to the entry in a table
 *                                     that we want to compute the
 *                                     checksum on.  Verified non-null in
 *                                     calling function.
 *
 *  \param [in]   ComputedCSValue      A pointer to the computed checksum
 *                                     that will be assigned the checksum of
 *                                     the entry.  Verified non-null in
 *                                     calling function.
 *
 *  \param [in]   DoneWithEntry        A pointer to a bool    that will be
 *                                     assigned a value based on whether or
 *                                     not the specified entry's checksum
 *                                     was completed.  Verified non-null in
 *                                     calling function.
 *
 *  \param [out]  *ComputedCSValue     Value used to determine the computed
 *                                     checksum, if completed
 *
 *  \param [out]  *DoneWithEntry       Value that specifies whether or not
 *                                     the specified entry's checksum was
 *                                     completed during this call.
 *
 * \return Execution status
 * \retval #CS_SUCCESS       \copybrief CS_SUCCESS
 * \retval #CS_ERROR         \copybrief CS_ERROR
 * \retval #CS_ERR_NOT_FOUND \copydoc CS_ERR_NOT_FOUND
 */
int32 CS_ComputeTables(CS_Res_Tables_Table_Entry_t *ResultsEntry, uint32 *ComputedCSValue, bool *DoneWithEntry);

/**
 * \brief Computes checksums on applications
 *
 *  \par Description
 *       Computes checksums up to MaxBytesPerCycle bytes every call. This
 *       function is used to compute checksums for applications.
 *
 *  \par Assumptions, External Events, and Notes:
 *        None
 *
 *  \param [in]   *ResultsEntry        A pointer to the entry in a table
 *                                     that we want to compute the
 *                                     checksum on.  Verified non-null in
 *                                     calling function.
 *
 *  \param [in]   ComputedCSValue      A pointer to the computed checksum
 *                                     that will be assigned the checksum of
 *                                     the entry.  Verified non-null in
 *                                     calling function.
 *
 *  \param [in]   DoneWithEntry        A pointer to a bool that will be
 *                                     assigned a value based on whether or
 *                                     not the specified entry's checksum
 *                                     was completed.  Verified non-null in
 *                                     calling function.
 *
 *  \param [out]  *ComputedCSValue     Value used to determine the computed
 *                                     checksum, if completed
 *
 *  \param [out]  *DoneWithEntry       Value that specifies whether or not
 *                                     the specified entry's checksum was
 *                                     completed during this call.
 *
 * \return Execution status
 * \retval #CS_SUCCESS       \copybrief CS_SUCCESS
 * \retval #CS_ERROR         \copybrief CS_ERROR
 * \retval #CS_ERR_NOT_FOUND \copydoc CS_ERR_NOT_FOUND
 */
int32 CS_ComputeApp(CS_Res_App_Table_Entry_t *ResultsEntry, uint32 *ComputedCSValue, bool *DoneWithEntry);

/**
 * \brief Child task main function for recomputing  baselines for
 *        Eeprom and Memory types
 *
 *  \par Description
 *       Child task main function that is spawned when a recompute
 *       baseline command is received for Eeprom, Memory, OS code segment
 *       or cFE core code segment.
 *
 *  \par Assumptions, External Events, and Notes:
 *        Only one child task for CS can be running at any one time.
 */
void CS_RecomputeEepromMemoryChildTask(void);

/**
 * \brief Child task main function for recomputing baselines for
 *        Tables
 *
 *  \par Description
 *       Child task main function that is spawned when a recompute
        baseline command is received for a table.
 *
 *  \par Assumptions, External Events, and Notes:
 *        Only one child task for CS can be running at any one time.
 */
void CS_RecomputeTablesChildTask(void);

/**
 * \brief Child task main function for recomputing  baselines for
 *        Applications
 *
 *  \par Description
        Child task main function that is spawned when a recompute
 *       baseline command is received for Applications.
 *
 *  \par Assumptions, External Events, and Notes:
 *        Only one child task for CS can be running at any one time.
 */
void CS_RecomputeAppChildTask(void);

/**
 * \brief Child task main function for computing a one shot calculatipn
 *
 *  \par Description
 *       Child task main function that is spawned when a one shot
 *       command is received.
 *
 *  \par Assumptions, External Events, and Notes:
 *        Only one child task for CS can be running at any one time.
 */
void CS_OneShotChildTask(void);

#endif
