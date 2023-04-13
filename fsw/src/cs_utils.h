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
 *   Specification for the CFS utilty functions
 */
#ifndef CS_UTILS_H
#define CS_UTILS_H

/**************************************************************************
 **
 ** Include section
 **
 **************************************************************************/
#include "cfe.h"
#include "cs_tbldefs.h"

/**
 * \brief Zeros out temporary checksum values of EEPROM table entries
 *
 *  \par Description
 *       Zeros the TempChecksumValue and the byte offset for every entry
 *       in the table. This allows all entries in the table to have their
 *       checksum started 'fresh'.
 *
 *  \par Assumptions, External Events, and Notes:
 *        None
 */
void CS_ZeroEepromTempValues(void);

/**
 * \brief Zeros out temporary checksum values of Memory table entries
 *
 *  \par Description
 *       Zeros the TempChecksumValue and the byte offset for every entry
 *       in the table. This allows all entries in the table to have their
 *       checksum started 'fresh'.
 *
 *  \par Assumptions, External Events, and Notes:
 *        None
 */
void CS_ZeroMemoryTempValues(void);

/**
 * \brief Zeros out temporary checksum values of Tables table entries
 *
 *  \par Description
 *       Zeros the TempChecksumValue and the byte offset for every entry
 *       in the table. This allows all entries in the table to have their
 *       checksum started 'fresh'.
 *
 *  \par Assumptions, External Events, and Notes:
 *        None
 */
void CS_ZeroTablesTempValues(void);

/**
 * \brief Zeros out temporary checksum values of App table entries
 *
 *  \par Description
 *       Zeros the TempChecksumValue and the byte offset for every entry
 *       in the table. This allows all entries in the table to have their
 *       checksum started 'fresh'.
 *
 *  \par Assumptions, External Events, and Notes:
 *        None
 */
void CS_ZeroAppTempValues(void);

/**
 * \brief Zeros out temporary checksum values of the cFE Core
 *
 *  \par Description
 *       Zeros the TempChecksumValue and the byte offset for the cFE core.
 *       This allows the cFE core checksum to be started 'fresh'.
 *
 *  \par Assumptions, External Events, and Notes:
 *        None
 */
void CS_ZeroCfeCoreTempValues(void);

/**
 * \brief Zeros out temporary checksum values of the OS code segment
 *
 *  \par Description
 *       Zeros the TempChecksumValue and the byte offset for the OS.
 *       This allows the OS checksum to be started 'fresh'.
 *
 *  \par Assumptions, External Events, and Notes:
 *        None
 */
void CS_ZeroOSTempValues(void);

/**
 * \brief Initializes the default definition tables
 *
 *  \par Description
 *       Sets all of the entries in the default definitions tables for
 *       EEPROM,Memory, Tables, and Apps to zero and sets theri states
 *       to 'empty'.
 *
 *  \par Assumptions, External Events, and Notes:
 *        None
 */
void CS_InitializeDefaultTables(void);

/**
 * \brief Moves global variables to point to the next table
 *
 *  \par Description
 *       Moves the global variables to point to the next table to checksum
 *
 *  \par Assumptions, External Events, and Notes:
 *        None
 */
void CS_GoToNextTable(void);

/**
 * \brief Gets a pointer to the results entry given a table name
 *
 *  \par Description
 *       This routine will look through the Tables results table
 *       to find an entry that has the given name. It returns
 *       a pointer to the entry through a parameter.
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param [in]   Name          The name associated with the entry we want
 *                              to find.  Calling function ensures this is
 *                              non-null.
 *
 *  \param [out]  EntryPtr      A pointer to a #CS_Res_Tables_Table_Entry_t
 *                              pointer that contains the start address of the
 *                              entry whose name field matches the name passed in
 *                              in the table passed in.
 *
 *  \return Boolean name found response
 *  \retval true  Name was found in the table
 *  \retval false Name was not found in the table
 */
bool CS_GetTableResTblEntryByName(CS_Res_Tables_Table_Entry_t **EntryPtr, const char *Name);

/**
 * \brief Gets a pointer to the definition entry given a table name
 *
 *  \par Description
 *       This routine will look through the Tables definition table
 *       to find an entry that has the given name. It returns
 *       a pointer to the entry through a parameter.
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param [in]   Name          The name associated with the entry we want
 *                              to find.  Calling function ensures this is
 *                              non-null.
 *
 *  \param [out]  EntryPtr      A pointer to a #CS_Def_Tables_Table_Entry_t
 *                              pointer that contains the start address of the
 *                              entry whose name field matches the name passed in
 *                              in the table passed in.
 *
 *  \return Boolean name found response
 *  \retval true  Name was found in the table
 *  \retval false Name was not found in the table
 */
bool CS_GetTableDefTblEntryByName(CS_Def_Tables_Table_Entry_t **EntryPtr, const char *Name);

/**
 * \brief Gets a pointer to the results entry given an app name
 *
 *  \par Description
 *       This routine will look through the App Results table
 *       to find an entry that has the given name. It returns
 *       a pointer to the entry through a parameter.
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param [in]    Name         The name associated with the entry we want
 *                              to find.  Calling function ensures this is
 *                              non-null.
 *
 *  \param [out]   EntryPtr     A pointer to a #CS_Res_App_Table_Entry_t
 *                              pointer that contains the start address of the
 *                              entry whose name field matches the name passed in
 *                              in the table passed in.
 *
 *  \return Boolean name found response
 *  \retval true  Name was found in the table
 *  \retval false Name was not found in the table
 */
bool CS_GetAppResTblEntryByName(CS_Res_App_Table_Entry_t **EntryPtr, const char *Name);

/**
 * \brief Gets a pointer to the definition entry given an app name
 *
 *  \par Description
 *       This routine will look through the App Definition table
 *       to find an entry that has the given name. It returns
 *       a pointer to the entry through a parameter.
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param [in]    Name         The name associated with the entry we want
 *                              to find.  Calling function ensures this is
 *                              non-null.
 *
 *  \param [out]   EntryPtr     A pointer to a #CS_Def_App_Table_Entry_t
 *                              pointer that contains the start address of the
 *                              entry whose name field matches the name passed in
 *                              in the table passed in.
 *
 *  \return Boolean name found response
 *  \retval true  Name was found in the table
 *  \retval false Name was not found in the table
 */
bool CS_GetAppDefTblEntryByName(CS_Def_App_Table_Entry_t **EntryPtr, const char *Name);

/**
 * \brief Find an enabled EEPROM entry
 *
 *  \par Description
 *       This routine will look from the current position to the end of
 *       the table to find an enabled entry.
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param [in]      EnabledEntry   A pointer to a uint16 that will be
 *                                  assigned an enabled entry ID, if
 *                                  one exists.  Calling function ensures
 *                                  this is non-null.
 *
 *  \param [out]   * EnabledEntry   The ID of an enabled entry in the
 *                                  table, if the function resturs true
 *
 *  \return Boolean entry found response
 *  \retval true  Entry was found in the table
 *  \retval false Entry was not found in the table
 */
bool CS_FindEnabledEepromEntry(uint16 *EnabledEntry);

/**
 * \brief Find an enabled Memory entry
 *
 *  \par Description
 *       This routine will look from the current position to the end of
 *       the table to find an enabled entry.
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param [in]      EnabledEntry   A pointer to a uint16 that will be
 *                                  assigned an enabled entry ID, if
 *                                  one exists.  Calling function ensures
 *                                  this is non-null.
 *
 *  \param [out]   * EnabledEntry   The ID of an enabled entry in the
 *                                  table, if the function resturs true
 *
 *  \return Boolean enabled entry found response
 *  \retval true  Enabled entry was found in the table
 *  \retval false Enabled entry was not found in the table
 */
bool CS_FindEnabledMemoryEntry(uint16 *EnabledEntry);

/**
 * \brief Find an enabled Tables entry
 *
 *  \par Description
 *       This routine will look from the current position to the end of
 *       the table to find an enabled entry.
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param [in]      EnabledEntry   A pointer to a uint16 that will be
 *                                  assigned an enabled entry ID, if
 *                                  one exists.  Calling function ensures
 *                                  this is non-null.
 *
 *  \param [out]   * EnabledEntry   The ID of an enabled entry in the
 *                                  table, if the function resturs true
 *
 *  \return Boolean enabled entry found response
 *  \retval true  Enabled entry was found in the table
 *  \retval false Enabled entry was not found in the table
 */
bool CS_FindEnabledTablesEntry(uint16 *EnabledEntry);

/**
 * \brief Find an enabled App entry
 *
 *  \par Description
 *       This routine will look from the current position to the end of
 *       the table to find an enabled entry.
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param [in]      EnabledEntry   A pointer to a uint16 that will be
 *                                  assigned an enabled entry ID, if
 *                                  one exists.  Calling function ensures
 *                                  this is non-null.
 *
 *  \param [out]   * EnabledEntry   The ID of an enabled entry in the
 *                                  table, if the function resturs true
 *
 *  \return Boolean enabled entry found response
 *  \retval true  Enabled entry was found in the table
 *  \retval false Enabled entry was not found in the table
 */
bool CS_FindEnabledAppEntry(uint16 *EnabledEntry);

/**
 * \brief Verify command message length
 *
 *  \par Description
 *       This routine will check if the actual length of a software bus
 *       command message matches the expected length and send an
 *       error event message if a mismatch occurs
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param [in]   msg              A #CFE_MSG_Message_t* pointer that
 *                                 references the software bus message.
 *                                 Calling function ensures this is
 *                                 non-null.
 *
 *  \param [in]   ExpectedLength   The expected length of the message
 *                                 based upon the command code
 *
 *  \return Boolean length valid response
 *  \retval true  Message length matches ExpectedLength
 *  \retval false Message length ExpectedLength mismatch
 *
 *  \sa #CS_LEN_ERR_EID
 */
bool CS_VerifyCmdLength(const CFE_MSG_Message_t *msg, size_t ExpectedLength);

/**
 * \brief Compute a background check cycle on the OS
 *
 *  \par Description
 *       This routine will try and complete a cycle of background checking
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \return Boolean checksum performed response
 *  \retval true  Checksum was done
 *  \retval false Checksum was not done
 */
bool CS_BackgroundOS(void);

/**
 * \brief Compute a background check cycle on the cFE Core
 *
 *  \par Description
 *       This routine will try and complete a cycle of background checking
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \return Boolean checksum performed response
 *  \retval true  Checksum was done
 *  \retval false Checksum was not done
 */
bool CS_BackgroundCfeCore(void);

/**
 * \brief Compute a background check cycle on EEPROM
 *
 *  \par Description
 *       This routine will try and complete a cycle of background checking
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \return Boolean checksum performed response
 *  \retval true  Checksum was done
 *  \retval false Checksum was not done
 */
bool CS_BackgroundEeprom(void);

/**
 * \brief Compute a background check cycle on the Memory
 *
 *  \par Description
 *       This routine will try and complete a cycle of background checking
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \return Boolean checksum performed response
 *  \retval true  Checksum was done
 *  \retval false Checksum was not done
 */
bool CS_BackgroundMemory(void);

/**
 * \brief Compute a background check cycle on Tables
 *
 *  \par Description
 *       This routine will try and complete a cycle of background checking
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \return Boolean checksum performed response
 *  \retval true  Checksum was done
 *  \retval false Checksum was not done
 */
bool CS_BackgroundTables(void);

/**
 * \brief Compute a background check cycle on Apps
 *
 *  \par Description
 *       This routine will try and complete a cycle of background checking
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \return Boolean checksum performed response
 *  \retval true  Checksum was done
 *  \retval false Checksum was not done
 */
bool CS_BackgroundApp(void);

/**
 * \brief Reset the checksum for a CS table entry in the CS tables table
 *
 *  \par Description
 *       If CS tables are listed in the CS tables table, then those tables
 *       must have their checksums recomputed when any of their entries
 *       have their enable/disable state flags modified.
 *
 *       This function will set ByteOffset and TempChecksumValue to zero,
 *       and ComputedYet to false for the specifified CS tables table entry.
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 */
void CS_ResetTablesTblResultEntry(CS_Res_Tables_Table_Entry_t *TablesTblResultEntry);

/**
 * \brief Update All CS Tables
 *
 *  \par Description
 *       Updates all CS tables if no recompute is happening on that table.
 *
 *       This is called as part of the regular housekeeping cycle or
 *       at the wakeup interval if no housekeeping request is received.
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 * \return Execution status, see \ref CFEReturnCodes
 * \retval #CFE_SUCCESS \copybrief CFE_SUCCESS
 */
CFE_Status_t CS_HandleRoutineTableUpdates(void);

/**
 * \brief Attempts to re-share a table
 *
 *  \par Description
 *       This function is called if the first attempt to share the table
 *       is unsuccessful.  This function attempts to share the table again
 *       to see if it reappeared.  Calling function ensures that parameters
 *       are non-null.
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 * \return Execution status, see \ref CFEReturnCodes
 * \retval #CFE_SUCCESS \copybrief CFE_SUCCESS
 */
CFE_Status_t CS_AttemptTableReshare(CS_Res_Tables_Table_Entry_t *ResultsEntry, CFE_TBL_Handle_t *LocalTblHandle,
                                    CFE_TBL_Info_t *TblInfo, cpuaddr *LocalAddress, int32 *ResultGetInfo);

bool CS_CheckRecomputeOneshot(void);

#endif
