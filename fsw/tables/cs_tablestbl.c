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
 *  The CFS Checksum (CS) Application Default Tables Table Definition
 */

/************************************************************************
** Includes
*************************************************************************/
#include "cfe.h"
#include "cs_platform_cfg.h"
#include "cs_msgdefs.h"
#include "cs_tbldefs.h"
#include "cfe_tbl_filedef.h"

CS_Def_Tables_Table_Entry_t CS_TablesTable[CS_MAX_NUM_TABLES_TABLE_ENTRIES] = {
    /*         State             Name   */

    /*   0 */ {CS_STATE_EMPTY, ""},
    /*   1 */ {CS_STATE_EMPTY, ""},
    /*   2 */ {CS_STATE_EMPTY, ""},
    /*   3 */ {CS_STATE_EMPTY, ""},
    /*   4 */ {CS_STATE_EMPTY, ""},
    /*   5 */ {CS_STATE_EMPTY, ""},
    /*   6 */ {CS_STATE_EMPTY, ""},
    /*   7 */ {CS_STATE_EMPTY, ""},
    /*   8 */ {CS_STATE_EMPTY, ""},
    /*   9 */ {CS_STATE_EMPTY, ""},
    /*  10 */ {CS_STATE_EMPTY, ""},
    /*  11 */ {CS_STATE_EMPTY, ""},
    /*  12 */ {CS_STATE_EMPTY, ""},
    /*  13 */ {CS_STATE_EMPTY, ""},
    /*  14 */ {CS_STATE_EMPTY, ""},
    /*  15 */ {CS_STATE_EMPTY, ""},
    /*  16 */ {CS_STATE_EMPTY, ""},
    /*  17 */ {CS_STATE_EMPTY, ""},
    /*  18 */ {CS_STATE_EMPTY, ""},
    /*  19 */ {CS_STATE_EMPTY, ""},
    /*  20 */ {CS_STATE_EMPTY, ""},
    /*  21 */ {CS_STATE_EMPTY, ""},
    /*  22 */ {CS_STATE_EMPTY, ""},
    /*  23 */ {CS_STATE_EMPTY, ""}

};

/*
** Table file header
*/
CFE_TBL_FILEDEF(CS_TablesTable, CS.DefTablesTbl, CS Tables Tbl, cs_tablestbl.tbl)

/************************/
/*  End of File Comment */
/************************/
