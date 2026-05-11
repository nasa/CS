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
#ifndef CS_TABLE_PROCESSING_H
#define CS_TABLE_PROCESSING_H

/**************************************************************************
 **
 ** Include section
 **
 **************************************************************************/
#include "cs_tbl.h"
#include "cfe_error.h"
#include "cfe_tbl_api_typedefs.h"

/**************************************************************************
 **
 ** Macro definitions
 **
 **************************************************************************/

/*************************************************************************
 **
 ** Type definitions
 **
 **************************************************************************/
typedef struct CS_TableWrapper
{
    const char *DefTableName; /**< \brief Name of definition table */
    const char *ResTableName; /**< \brief Name of result table */

    CFE_TBL_Handle_t DefHandle; /**< \brief Handle to the definition table */
    CFE_TBL_Handle_t ResHandle; /**< \brief Handle to the results table */

    size_t NumEntries; /**< \brief Number of entries in table (applies to both def and res) */

    void  *DefAddr;        /**< \brief Pointer to definition table */
    size_t DefEntrySize;   /**< \brief Size of each entry in definition table */
    size_t DefStateOffset; /**< \brief Offset of "State" member within definition entry */

    void  *ResAddr;      /**< \brief Pointer to results table */
    size_t ResEntrySize; /**< \brief Size of each entry in result table */

    const void *DefaultDefinitionPtr; /**< Pointer to default definition, if applicable */

    void (*UpdateHandler)(struct CS_TableWrapper *); /**< Function to handle table updates */

    uint32 *BaselineValue; /**< \brief pointer to HK TLM entry containing baseline value */
    uint8  *GlobalState;   /**< \brief pointer to HK TLM entry containing global enable/disable */

    CS_Res_Tables_Table_Entry_t *ResTblPtr; /**< \brief CS results entry for this table */

} CS_TableWrapper_t;

/**************************************************************************
 **
 **  Function Prototypes
 **
 **************************************************************************/

/**
 * \brief Calls the table update handler, if configured
 *
 * This is a helper function, tested separately
 *
 *  \param [in] tw        A pointer to the table wrapper struct
 */
void CS_CallTableUpdateHandler(CS_TableWrapper_t *tw);

/**
 * \brief Validate EEPROM definition table
 *
 *  \par Description
 *       This function is a callback to cFE Table Services that gets called
 *       when a validation is requested.
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param [in]  TblPtr        A pointer to the table data to be validated
 *
 *  \return Execution status see \ref CFEReturnCodes
 *  \retval #CFE_SUCCESS    \copydoc CFE_SUCCESS
 *  \retval #CS_TABLE_ERROR \copydoc CS_TABLE_ERROR
 */
CFE_Status_t CS_ValidateEepromChecksumDefinitionTable(void *TblPtr);

/**
 * \brief Validate Memory definition table
 *
 *  \par Description
 *       This function is a callback to cFE Table Services that gets called
 *       when a validation is requested.
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param [in]  TblPtr        A pointer to the table data to be validated
 *
 *  \return Execution status see \ref CFEReturnCodes
 *  \retval #CFE_SUCCESS    \copydoc CFE_SUCCESS
 *  \retval #CS_TABLE_ERROR \copydoc CS_TABLE_ERROR
 */
CFE_Status_t CS_ValidateMemoryChecksumDefinitionTable(void *TblPtr);

/**
 * \brief Validate Tables definition table
 *
 *  \par Description
 *       This function is a callback to cFE Table Services that gets called
 *       when a validation is requested.
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param [in]  TblPtr        A pointer to the table data to be validated
 *
 *  \return Execution status see \ref CFEReturnCodes
 *  \retval #CFE_SUCCESS    \copydoc CFE_SUCCESS
 *  \retval #CS_TABLE_ERROR \copydoc CS_TABLE_ERROR
 */
CFE_Status_t CS_ValidateTablesChecksumDefinitionTable(void *TblPtr);

/**
 * \brief Validate App definition table
 *
 *  \par Description
 *       This function is a callback to cFE Table Services that gets called
 *       when a validation is requested.
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param [in]  TblPtr        A pointer to the table data to be validated
 *
 *  \return Execution status see \ref CFEReturnCodes
 *  \retval #CFE_SUCCESS    \copydoc CFE_SUCCESS
 *  \retval #CS_TABLE_ERROR \copydoc CS_TABLE_ERROR
 */
CFE_Status_t CS_ValidateAppChecksumDefinitionTable(void *TblPtr);

/**
 * \brief Processes a new definition table for EEPROM or Memory tables
 *
 *  \par Description
 *       Copies data from the definition table to the results table
 *       because the results table is where CS keeps all of its
 *       checksum data
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param [in] tw        A pointer to the table wrapper struct
 */
void CS_ProcessNewEepromMemoryDefinitionTable(CS_TableWrapper_t *tw);

/**
 * \brief Processes a new definition table for the Tables table
 *
 *  \par Description
 *       Copies data from the definition table to the results table
 *       because the results table is where CS keeps all of its
 *       checksum data
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param [in] tw        A pointer to the table wrapper struct
 */
void CS_ProcessNewTablesDefinitionTable(CS_TableWrapper_t *tw);

/**
 * \brief Processes a new definition table for the App table
 *
 *  \par Description
 *       Copies data from the definition table to the results table
 *       because the results table is where CS keeps all of its
 *       checksum data
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param [in] tw        A pointer to the table wrapper struct
 */
void CS_ProcessNewAppDefinitionTable(CS_TableWrapper_t *tw);

/**
 * \brief Initializes the results and definition table on startup
 *
 *  \par Description
 *       Completes the Table Services registration and table load for
 *       the definition table and the registration for the results table
 *
 *  \par Assumptions, External Events, and Notes:
 *       This function is used on all four type of tables individally.
 *       Also, if the default table file is not found for the definitionm
 *       table, this function loads a 'blank' table from memory
 *
 *  \param [in] tw        A pointer to the table wrapper struct
 *
 *  \param [in]    DefinitionTableFileName     The name of the file to load the
 *                                             definition table from
 *
 *  \param [in]    CallBackFunction            A pointer to a function used to
 *                                             validate the definition table
 *
 * \return Execution status, see \ref CFEReturnCodes
 * \retval #CFE_SUCCESS \copybrief CFE_SUCCESS
 */
CFE_Status_t
CS_TableInit(CS_TableWrapper_t *tw, const char *DefinitionTableFileName, CFE_TBL_CallbackFuncPtr_t CallBackFunction);

/**
 * \brief Handles table updates for all CS tables
 *
 *  \par Description
 *       Completes the handshake with Table Services that releases
 *       Addresses for the tables and checks for updates
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param [in] tw        A pointer to the table wrapper struct
 *
 * \return Execution status, see \ref CFEReturnCodes
 * \retval #CFE_SUCCESS \copybrief CFE_SUCCESS
 */
CFE_Status_t CS_HandleTableUpdate(CS_TableWrapper_t *tw);

#endif
