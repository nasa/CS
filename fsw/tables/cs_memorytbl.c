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
 *  The CFS Checksum (CS) Application Default Memory Table Definition
 */

/************************************************************************
** Includes
*************************************************************************/
#include "cfe.h"
#include "cfe_tbl_filedef.h"
#include "cs_msgdefs.h"
#include "cs_platform_cfg.h"
#include "cs_tbldefs.h"

CS_Def_EepromMemory_Table_Entry_t CS_MemoryTable[CS_MAX_NUM_MEMORY_TABLE_ENTRIES] = {
    /*         State           Filler   StartAddress     NumBytes */
    /*  0 */
    {.State = CS_STATE_EMPTY, .Filler16 = 0x1234, .StartAddress = 0x00000000, .NumBytesToChecksum = 0x00000000},
    /*  1 */
    {.State = CS_STATE_EMPTY, .Filler16 = 0x1234, .StartAddress = 0x00000000, .NumBytesToChecksum = 0x00000000},
    /*  2 */
    {.State = CS_STATE_EMPTY, .Filler16 = 0x1234, .StartAddress = 0x00000000, .NumBytesToChecksum = 0x00000000},
    /*  3 */
    {.State = CS_STATE_EMPTY, .Filler16 = 0x1234, .StartAddress = 0x00000000, .NumBytesToChecksum = 0x00000000},
    /*  4 */
    {.State = CS_STATE_EMPTY, .Filler16 = 0x1234, .StartAddress = 0x00000000, .NumBytesToChecksum = 0x00000000},
    /*  5 */
    {.State = CS_STATE_EMPTY, .Filler16 = 0x1234, .StartAddress = 0x00000000, .NumBytesToChecksum = 0x00000000},
    /*  6 */
    {.State = CS_STATE_EMPTY, .Filler16 = 0x1234, .StartAddress = 0x00000000, .NumBytesToChecksum = 0x00000000},
    /*  7 */
    {.State = CS_STATE_EMPTY, .Filler16 = 0x1234, .StartAddress = 0x00000000, .NumBytesToChecksum = 0x00000000},
    /*  8 */
    {.State = CS_STATE_EMPTY, .Filler16 = 0x1234, .StartAddress = 0x00000000, .NumBytesToChecksum = 0x00000000},
    /*  9 */
    {.State = CS_STATE_EMPTY, .Filler16 = 0x1234, .StartAddress = 0x00000000, .NumBytesToChecksum = 0x00000000},
    /* 10 */
    {.State = CS_STATE_EMPTY, .Filler16 = 0x1234, .StartAddress = 0x00000000, .NumBytesToChecksum = 0x00000000},
    /* 11 */
    {.State = CS_STATE_EMPTY, .Filler16 = 0x1234, .StartAddress = 0x00000000, .NumBytesToChecksum = 0x00000000},
    /* 12 */
    {.State = CS_STATE_EMPTY, .Filler16 = 0x1234, .StartAddress = 0x00000000, .NumBytesToChecksum = 0x00000000},
    /* 13 */
    {.State = CS_STATE_EMPTY, .Filler16 = 0x1234, .StartAddress = 0x00000000, .NumBytesToChecksum = 0x00000000},
    /* 14 */
    {.State = CS_STATE_EMPTY, .Filler16 = 0x1234, .StartAddress = 0x00000000, .NumBytesToChecksum = 0x00000000},
    /* 15 */
    {.State = CS_STATE_EMPTY, .Filler16 = 0x1234, .StartAddress = 0x00000000, .NumBytesToChecksum = 0x00000000}};

/*
** Table file header
*/
CFE_TBL_FILEDEF(CS_MemoryTable, CS.DefMemoryTbl, CS Memory Tbl, cs_memorytbl.tbl)

/************************/
/*  End of File Comment */
/************************/
