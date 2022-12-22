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
 *   Specification for the CFS Checksum constants for message IDs
 */
#ifndef CS_MSGIDS_H
#define CS_MSGIDS_H

/**
 * \defgroup cfscscmdmid CFS Checksum Command Message IDs
 * \{
 */

#define CS_CMD_MID              (0x189F) /**< \brief CS Command Message ID */
#define CS_SEND_HK_MID          (0x18A0) /**< \brief CS Housekeeping Request Message ID */
#define CS_BACKGROUND_CYCLE_MID (0x18A1) /**< \brief CS Background Cycle Message ID */

/**\}*/

/**
 * \defgroup cfscstlmmid CFS Checksum Telemetry Message IDs
 * \{
 */

#define CS_HK_TLM_MID (0x08A4) /**< \brief CS Housekeeping Telemetry Message ID */

/**\}*/

#endif
