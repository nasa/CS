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
#include "cs_tbl.h"
#include "cs_table_processing.h"
#include "cs_app.h"

/**************************************************************************
 **
 ** Utility function declarations
 **
 **************************************************************************/

/**
 * \brief Converts the table reference to a string for logging
 *
 * \param [in] Tbl          Pointer to global table wrapper struct
 *
 * \return Pointer to printable string (never NULL)
 */
const char *CS_GetTableTypeAsString(const CS_TableWrapper_t *Tbl);

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
 *  \return Pointer to next entry
 *  \retval NULL if no enabled entry was found
 */
CS_Res_EepromMemory_Table_Entry_t *CS_FindEnabledEepromEntry(void);

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
 *  \return Pointer to next entry
 *  \retval NULL if no enabled entry was found
 */
CS_Res_EepromMemory_Table_Entry_t *CS_FindEnabledMemoryEntry(void);

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
 *  \return Pointer to next entry
 *  \retval NULL if no enabled entry was found
 */
CS_Res_Tables_Table_Entry_t *CS_FindEnabledTablesEntry(void);

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
 *  \return Pointer to next entry
 *  \retval NULL if no enabled entry was found
 */
CS_Res_App_Table_Entry_t *CS_FindEnabledAppEntry(void);

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
 * \brief Check if one shot recompute is in progress
 *
 *  \par Description
 *       Checks that the one shot is NOT currently running
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 * \retval true if one shot recompute is in progress
 * \retval false if one shot recompute is not in progress
 */
bool CS_CheckRecomputeOneshot(void);

/**
 * \brief Switches definition state between ENABLED and DISABLED
 *
 *  \par Description
 *       Changes the state of the definition table entry to the indicated value
 *       If the state within the definition table is not valid then the state is
 *       not changed, it will remain at the previous value.
 *
 *       If this resulted in an actual change to the definition table, then the
 *       table is marked as modified within table services.
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 * \param tw Pointer to table wrapper struct
 * \param EntryPtr Pointer to entry within definition table
 * \param NewState value to set the "State" field to (Must be ENABLED or DISABLED)
 *
 * \returns Previous state of table entry
 */
CS_ChecksumState_Enum_t CS_SetDefEntryState(CS_TableWrapper_t *tw, void *EntryPtr, CS_ChecksumState_Enum_t NewState);

/**
 * \brief Verifies that an address range is safe to access
 */
CFE_Status_t CS_VerifyAddressRange(cpuaddr StartAddress, size_t Size);

/**
 * \brief Checks if the definition table matches the given name
 *
 *  \par Description
 *       Checks if the passed-in name matches the name of the definition table
 *       for the given table ID
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 * \param Name Name of table
 * \param TableIdx Identifier of global table
 *
 * \retval true if name matches
 * \retval false if name does not match
 */
bool CS_CheckDefTableNameMatch(const char *Name, uint16 TableIdx);

/**
 * \brief Checks if the result table matches the given name
 *
 *  \par Description
 *       Checks if the passed-in name matches the name of the result table
 *       for the given table ID
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 * \param Name Name of table
 * \param TableIdx Identifier of global table
 *
 * \retval true if name matches
 * \retval false if name does not match
 */
bool CS_CheckResTableNameMatch(const char *Name, uint16 TableIdx);

/**
 * \brief Gets the address of the specified definition table entry
 *
 *  \par Description
 *       Computes and returns a pointer to the specified definition table entry
 *
 *  \par Assumptions, External Events, and Notes:
 *       The returned pointer is generic and must be cast to the correct
 *       structure type depending on which table is being accessed.
 *
 * \param tw Pointer to global table wrapper
 * \param EntryIdx Index of entry to locate
 *
 * \returns Pointer to definition table entry
 * \retval  NULL if the entry index is out of range
 */
void *CS_GetDefEntryAddr(CS_TableWrapper_t *tw, uint16 EntryIdx);

/**
 * \brief Gets the address of the specified result table entry
 *
 *  \par Description
 *       Computes and returns a pointer to the specified result table entry
 *
 *  \par Assumptions, External Events, and Notes:
 *       The returned pointer is generic and must be cast to the correct
 *       structure type depending on which table is being accessed.
 *
 * \param tw Pointer to global table wrapper
 * \param EntryIdx Index of entry to locate
 *
 * \returns Pointer to result table entry
 * \retval  NULL if the entry index is out of range
 */
void *CS_GetResEntryAddr(CS_TableWrapper_t *tw, uint16 EntryIdx);

/**************************************************************************
 **
 ** Helper functions (inline)
 ** These exist to consolidate repetitive logic and make the code more readable
 **
 **************************************************************************/

/**
 * \brief Checks that a state value is ENABLED and DISABLED
 *
 *  \par Description
 *       A configured table entry must be set to ENABLED or DISABLED only.
 *       This confirms that the state is one of those two values.
 *
 *  \par Assumptions, External Events, and Notes:
 *       EMPTY is allowed for unused entries.  A check for empty entries
 *       must be done separately.
 *
 * \param State State value to test
 *
 * \retval true if state is ENABLED or DISABLED
 * \retval false if state is anything else
 */
static inline bool CS_StateValid(CS_ChecksumState_Enum_t State)
{
    return (State == CS_ChecksumState_ENABLED || State == CS_ChecksumState_DISABLED);
}

/**
 * \brief Gets a pointer to the EEPROM checksum definition for the given index
 *
 *  \par Description
 *       Gets a pointer to the definition table entry
 *
 *  \par Assumptions, External Events, and Notes:
 *       Returns NULL if the entry index is out of range
 *
 * \param EntryIdx Index of entry to locate
 *
 * \returns Pointer to definition table entry, if index is valid
 * \retval  NULL if index is invalid
 */
static inline CS_Def_EepromMemory_Table_Entry_t *CS_GetEepromDefEntry(uint16 EntryIdx)
{
    return CS_GetDefEntryAddr(&CS_AppData.Tbl[CS_ChecksumType_EEPROM_TABLE], EntryIdx);
}

/**
 * \brief Gets a pointer to the EEPROM checksum result for the given index
 *
 *  \par Description
 *       Gets a pointer to the result table entry
 *
 *  \par Assumptions, External Events, and Notes:
 *       Returns NULL if the entry index is out of range
 *
 * \param EntryIdx Index of entry to locate
 *
 * \returns Pointer to result table entry, if index is valid
 * \retval  NULL if index is invalid
 */
static inline CS_Res_EepromMemory_Table_Entry_t *CS_GetEepromResEntry(uint16 EntryIdx)
{
    return CS_GetResEntryAddr(&CS_AppData.Tbl[CS_ChecksumType_EEPROM_TABLE], EntryIdx);
}

/**
 * \brief Gets a pointer to the Memory checksum definition for the given index
 *
 *  \par Description
 *       Gets a pointer to the definition table entry
 *
 *  \par Assumptions, External Events, and Notes:
 *       Returns NULL if the entry index is out of range
 *
 * \param EntryIdx Index of entry to locate
 *
 * \returns Pointer to definition table entry, if index is valid
 * \retval  NULL if index is invalid
 */
static inline CS_Def_EepromMemory_Table_Entry_t *CS_GetMemoryDefEntry(uint16 EntryIdx)
{
    return CS_GetDefEntryAddr(&CS_AppData.Tbl[CS_ChecksumType_MEMORY_TABLE], EntryIdx);
}

/**
 * \brief Gets a pointer to the Memory checksum result for the given index
 *
 *  \par Description
 *       Gets a pointer to the result table entry
 *
 *  \par Assumptions, External Events, and Notes:
 *       Returns NULL if the entry index is out of range
 *
 * \param EntryIdx Index of entry to locate
 *
 * \returns Pointer to result table entry, if index is valid
 * \retval  NULL if index is invalid
 */
static inline CS_Res_EepromMemory_Table_Entry_t *CS_GetMemoryResEntry(uint16 EntryIdx)
{
    return CS_GetResEntryAddr(&CS_AppData.Tbl[CS_ChecksumType_MEMORY_TABLE], EntryIdx);
}

/**
 * \brief Gets a pointer to the Table checksum definition for the given index
 *
 *  \par Description
 *       Gets a pointer to the definition table entry
 *
 *  \par Assumptions, External Events, and Notes:
 *       Returns NULL if the entry index is out of range
 *
 * \param EntryIdx Index of entry to locate
 *
 * \returns Pointer to definition table entry, if index is valid
 * \retval  NULL if index is invalid
 */
static inline CS_Def_Tables_Table_Entry_t *CS_GetTablesDefEntry(uint16 EntryIdx)
{
    return CS_GetDefEntryAddr(&CS_AppData.Tbl[CS_ChecksumType_TABLES_TABLE], EntryIdx);
}

/**
 * \brief Gets a pointer to the Table checksum result for the given index
 *
 *  \par Description
 *       Gets a pointer to the result table entry
 *
 *  \par Assumptions, External Events, and Notes:
 *       Returns NULL if the entry index is out of range
 *
 * \param EntryIdx Index of entry to locate
 *
 * \returns Pointer to result table entry, if index is valid
 * \retval  NULL if index is invalid
 */
static inline CS_Res_Tables_Table_Entry_t *CS_GetTablesResEntry(uint16 EntryIdx)
{
    return CS_GetResEntryAddr(&CS_AppData.Tbl[CS_ChecksumType_TABLES_TABLE], EntryIdx);
}

/**
 * \brief Gets a pointer to the App checksum definition for the given index
 *
 *  \par Description
 *       Gets a pointer to the definition table entry
 *
 *  \par Assumptions, External Events, and Notes:
 *       Returns NULL if the entry index is out of range
 *
 * \param EntryIdx Index of entry to locate
 *
 * \returns Pointer to definition table entry, if index is valid
 * \retval  NULL if index is invalid
 */
static inline CS_Def_App_Table_Entry_t *CS_GetAppDefEntry(uint16 EntryIdx)
{
    return CS_GetDefEntryAddr(&CS_AppData.Tbl[CS_ChecksumType_APP_TABLE], EntryIdx);
}

/**
 * \brief Gets a pointer to the App checksum result for the given index
 *
 *  \par Description
 *       Gets a pointer to the result table entry
 *
 *  \par Assumptions, External Events, and Notes:
 *       Returns NULL if the entry index is out of range
 *
 * \param EntryIdx Index of entry to locate
 *
 * \returns Pointer to result table entry, if index is valid
 * \retval  NULL if index is invalid
 */
static inline CS_Res_App_Table_Entry_t *CS_GetAppResEntry(uint16 EntryIdx)
{
    return CS_GetResEntryAddr(&CS_AppData.Tbl[CS_ChecksumType_APP_TABLE], EntryIdx);
}

#endif
