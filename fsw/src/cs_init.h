/************************************************************************
 * NASA Docket No. GSC-18,915-1, and identified as “cFS Checksum
 * Application version 2.5.0”
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
 *   Initialization subroutines for CS.
 */
#ifndef CS_INIT_H
#define CS_INIT_H

/**************************************************************************
 **
 ** Include section
 **
 **************************************************************************/
#include "cfe.h"

/**
 * \brief Initializes the Software Bus Pipes for the Checksum Application
 *
 *  \par Description
 *       Configures cFE Software Bus resources used by the Checksum
 *       application.
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 * \return Execution status, see \ref CFEReturnCodes
 * \retval #CFE_SUCCESS \copybrief CFE_SUCCESS
 */
int32 CS_SbInit(void);

/**
 * \brief Initializes the tables for the Checksum Application
 *
 *  \par Description
 *       Initializes all tables used by the Checksum application.
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 * \return Execution status, see \ref CFEReturnCodes
 * \retval #CFE_SUCCESS \copybrief CFE_SUCCESS
 */
int32 CS_InitAllTables(void);

/**
 * \brief Initializes the cFE and OS segments for the Checksum Application
 *
 *  \par Description
 *       Initializes the cFE and OS segments for the Checksum Application.
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 */
void CS_InitSegments(void);

#endif
