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
 *   Specification for the CFS Checksum command and telemetry
 *   messages.
 */
#ifndef DEFAULT_CS_FCNCODE_VALUES_H
#define DEFAULT_CS_FCNCODE_VALUES_H

#define CS_CCVAL(x) CS_FunctionCode_##x

enum CS_FunctionCode
{
    CS_FunctionCode_NOOP                            = 0,
    CS_FunctionCode_RESET_COUNTERS                  = 1,
    CS_FunctionCode_ONE_SHOT                        = 2,
    CS_FunctionCode_CANCEL_ONE_SHOT                 = 3,
    CS_FunctionCode_ENABLE_ALL_CS                   = 4,
    CS_FunctionCode_DISABLE_ALL_CS                  = 5,
    CS_FunctionCode_ENABLE_CFE_CORE                 = 6,
    CS_FunctionCode_DISABLE_CFE_CORE                = 7,
    CS_FunctionCode_REPORT_BASELINE_CFE_CORE        = 8,
    CS_FunctionCode_RECOMPUTE_BASELINE_CFE_CORE     = 9,
    CS_FunctionCode_ENABLE_OS                       = 10,
    CS_FunctionCode_DISABLE_OS                      = 11,
    CS_FunctionCode_REPORT_BASELINE_OS              = 12,
    CS_FunctionCode_RECOMPUTE_BASELINE_OS           = 13,
    CS_FunctionCode_ENABLE_EEPROM                   = 14,
    CS_FunctionCode_DISABLE_EEPROM                  = 15,
    CS_FunctionCode_REPORT_BASELINE_ENTRY_ID_EEPROM = 16,
    CS_FunctionCode_RECOMPUTE_BASELINE_EEPROM       = 17,
    CS_FunctionCode_ENABLE_ENTRY_ID_EEPROM          = 18,
    CS_FunctionCode_DISABLE_ENTRY_ID_EEPROM         = 19,
    CS_FunctionCode_GET_ENTRY_ID_EEPROM             = 20,
    CS_FunctionCode_ENABLE_MEMORY                   = 21,
    CS_FunctionCode_DISABLE_MEMORY                  = 22,
    CS_FunctionCode_REPORT_BASELINE_ENTRY_ID_MEMORY = 23,
    CS_FunctionCode_RECOMPUTE_BASELINE_MEMORY       = 24,
    CS_FunctionCode_ENABLE_ENTRY_ID_MEMORY          = 25,
    CS_FunctionCode_DISABLE_ENTRY_ID_MEMORY         = 26,
    CS_FunctionCode_GET_ENTRY_ID_MEMORY             = 27,
    CS_FunctionCode_ENABLE_TABLES                   = 28,
    CS_FunctionCode_DISABLE_TABLES                  = 29,
    CS_FunctionCode_REPORT_BASELINE_TABLE           = 30,
    CS_FunctionCode_RECOMPUTE_BASELINE_TABLE        = 31,
    CS_FunctionCode_ENABLE_NAME_TABLE               = 32,
    CS_FunctionCode_DISABLE_NAME_TABLE              = 33,
    CS_FunctionCode_ENABLE_APPS                     = 34,
    CS_FunctionCode_DISABLE_APPS                    = 35,
    CS_FunctionCode_REPORT_BASELINE_APP             = 36,
    CS_FunctionCode_RECOMPUTE_BASELINE_APP          = 37,
    CS_FunctionCode_ENABLE_NAME_APP                 = 38,
    CS_FunctionCode_DISABLE_NAME_APP                = 39,
};

#endif
