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

/*
 * Includes
 */

#include "cs_app.h"

/* UT includes */
#include "uttest.h"
#include "utassert.h"
#include "utstubs.h"

CFE_Status_t CS_SbInit(void)
{
    return UT_DEFAULT_IMPL(CS_SbInit);
}

CFE_Status_t CS_InitAllTables(void)
{
    return UT_DEFAULT_IMPL(CS_InitAllTables);
}

CFE_Status_t CS_InitSegments(void)
{
    return UT_DEFAULT_IMPL(CS_InitAllTables);
}
