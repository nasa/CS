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
 *   Specification for the CFS Checksum macro constants that can
 *   be configured from one mission to another
 */
#ifndef CS_MISSION_CFG_H
#define CS_MISSION_CFG_H

#include "cfe_mission_cfg.h"

/**
 * \defgroup cfscsmissioncfg CFS Checksum Mission Configuration
 * \{
 */

/**
 * \brief default CRC algorithm
 *
 *  \par  Description:
 *        This parameter is the algorithm used by CS to checksum
 *        the requested data.
 *
 *  \par Limits:
 *         This parameter is limited to either #CFE_MISSION_ES_DEFAULT_CRC,
 *         or  #CFE_MISSION_ES_CRC_16
 */
#define CS_DEFAULT_ALGORITHM CFE_MISSION_ES_DEFAULT_CRC

/**\}*/

#endif
