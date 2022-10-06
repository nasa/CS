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

#include "cs_test_utils.h"
#include "cs_tbldefs.h"

/* UT includes */
#include "uttest.h"
#include "utassert.h"
#include "utstubs.h"

/*
 * Function Definitions
 */

CS_Res_EepromMemory_Table_Entry_t CS_DefaultEepromResTable[CS_MAX_NUM_EEPROM_TABLE_ENTRIES];
CS_Res_EepromMemory_Table_Entry_t CS_DefaultMemoryResTable[CS_MAX_NUM_MEMORY_TABLE_ENTRIES];
CS_Res_Tables_Table_Entry_t       CS_DefaultTablesResTable[CS_MAX_NUM_TABLES_TABLE_ENTRIES];
CS_Res_App_Table_Entry_t          CS_DefaultAppResTable[CS_MAX_NUM_APP_TABLE_ENTRIES];

#define UT_MAX_SENDEVENT_DEPTH 4
CFE_EVS_SendEvent_context_t    context_CFE_EVS_SendEvent[UT_MAX_SENDEVENT_DEPTH];
CFE_ES_WriteToSysLog_context_t context_CFE_ES_WriteToSysLog;

void UT_Handler_CFE_EVS_SendEvent(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context, va_list va)
{
    uint16 CallCount;
    uint16 idx;

    CallCount = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    if (CallCount > (sizeof(context_CFE_EVS_SendEvent) / sizeof(context_CFE_EVS_SendEvent[0])))
    {
        UtAssert_Failed("CFE_EVS_SendEvent UT depth %u exceeded: %u, increase UT_MAX_SENDEVENT_DEPTH",
                        UT_MAX_SENDEVENT_DEPTH, CallCount);
    }
    else
    {
        idx                                      = CallCount - 1;
        context_CFE_EVS_SendEvent[idx].EventID   = UT_Hook_GetArgValueByName(Context, "EventID", uint16);
        context_CFE_EVS_SendEvent[idx].EventType = UT_Hook_GetArgValueByName(Context, "EventType", uint16);

        strncpy(context_CFE_EVS_SendEvent[idx].Spec, UT_Hook_GetArgValueByName(Context, "Spec", const char *),
                CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);
        context_CFE_EVS_SendEvent[idx].Spec[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH - 1] = '\0';
    }
}

void UT_Handler_CFE_ES_WriteToSysLog(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context, va_list va)
{
    strncpy(context_CFE_ES_WriteToSysLog.Spec, UT_Hook_GetArgValueByName(Context, "SpecStringPtr", const char *),
            CFE_MISSION_EVS_MAX_MESSAGE_LENGTH - 1);
    context_CFE_ES_WriteToSysLog.Spec[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH - 1] = '\0';
}

void CS_Test_Setup(void)
{
    /* initialize test environment to default state for every test */
    UT_ResetState(0);

    memset(&CS_AppData, 0, sizeof(CS_AppData));
    memset(context_CFE_EVS_SendEvent, 0, sizeof(context_CFE_EVS_SendEvent));
    memset(&context_CFE_ES_WriteToSysLog, 0, sizeof(context_CFE_ES_WriteToSysLog));

    /* Register custom handlers */
    UT_SetVaHandlerFunction(UT_KEY(CFE_EVS_SendEvent), UT_Handler_CFE_EVS_SendEvent, NULL);
    UT_SetVaHandlerFunction(UT_KEY(CFE_ES_WriteToSysLog), UT_Handler_CFE_ES_WriteToSysLog, NULL);

    CS_AppData.DefEepromTblPtr = &CS_AppData.DefaultEepromDefTable[0];
    CS_AppData.ResEepromTblPtr = &CS_DefaultEepromResTable[0];
    CS_AppData.DefMemoryTblPtr = &CS_AppData.DefaultMemoryDefTable[0];
    CS_AppData.ResMemoryTblPtr = &CS_DefaultMemoryResTable[0];
    CS_AppData.DefTablesTblPtr = &CS_AppData.DefaultTablesDefTable[0];
    CS_AppData.ResTablesTblPtr = &CS_DefaultTablesResTable[0];
    CS_AppData.DefAppTblPtr    = &CS_AppData.DefaultAppDefTable[0];
    CS_AppData.ResAppTblPtr    = &CS_DefaultAppResTable[0];

    memset(CS_DefaultEepromResTable, 0, sizeof(CS_DefaultEepromResTable));
    memset(CS_DefaultMemoryResTable, 0, sizeof(CS_DefaultMemoryResTable));
    memset(CS_DefaultTablesResTable, 0, sizeof(CS_DefaultTablesResTable));
    memset(CS_DefaultAppResTable, 0, sizeof(CS_DefaultAppResTable));
} /* end CS_Test_Setup */

void CS_Test_TearDown(void)
{
    /* cleanup test environment */
} /* end CS_Test_TearDown */

/************************/
/*  End of File Comment */
/************************/
