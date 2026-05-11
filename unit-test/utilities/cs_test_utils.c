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

/*
 * Includes
 */

#include "cs_test_utils.h"
#include "cs_utils.h"
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

struct CS_SetDefEntryState_Context
{
    void                   *EntryPtr;
    CS_ChecksumState_Enum_t NewState;
    CS_ChecksumState_Enum_t OldState;
};

static struct CS_SetDefEntryState_Context CS_SetDefEntryState_Context;

void UT_Handler_CFE_EVS_SendEvent(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context, va_list va)
{
    uint16 CallCount;
    uint16 idx;

    CallCount = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    if (CallCount > (sizeof(context_CFE_EVS_SendEvent) / sizeof(context_CFE_EVS_SendEvent[0])))
    {
        UtAssert_Failed("CFE_EVS_SendEvent UT depth %u exceeded: %u, increase UT_MAX_SENDEVENT_DEPTH",
                        UT_MAX_SENDEVENT_DEPTH,
                        CallCount);
    }
    else
    {
        idx                                      = CallCount - 1;
        context_CFE_EVS_SendEvent[idx].EventID   = UT_Hook_GetArgValueByName(Context, "EventID", uint16);
        context_CFE_EVS_SendEvent[idx].EventType = UT_Hook_GetArgValueByName(Context, "EventType", uint16);

        strncpy(context_CFE_EVS_SendEvent[idx].Spec,
                UT_Hook_GetArgValueByName(Context, "Spec", const char *),
                CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);
        context_CFE_EVS_SendEvent[idx].Spec[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH - 1] = '\0';
    }
}

void UT_Handler_CFE_ES_WriteToSysLog(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context, va_list va)
{
    strncpy(context_CFE_ES_WriteToSysLog.Spec,
            UT_Hook_GetArgValueByName(Context, "SpecStringPtr", const char *),
            CFE_MISSION_EVS_MAX_MESSAGE_LENGTH - 1);
    context_CFE_ES_WriteToSysLog.Spec[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH - 1] = '\0';
}

void UT_Handler_CS_TableEntry(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CS_TableWrapper_t *tw       = UT_Hook_GetArgValueByName(Context, "tw", CS_TableWrapper_t *);
    uint16             EntryIdx = UT_Hook_GetArgValueByName(Context, "EntryIdx", uint16);
    uint8             *Base;
    int32              Status;

    Base   = NULL;
    Status = 0;

    if ((!UT_Stub_GetInt32StatusCode(Context, &Status) && EntryIdx < tw->NumEntries) || Status != 0)
    {
        if (FuncKey == UT_KEY(CS_GetDefEntryAddr))
        {
            Base  = tw->DefAddr;
            Base += (tw->DefEntrySize * EntryIdx);
        }
        else if (FuncKey == UT_KEY(CS_GetResEntryAddr))
        {
            Base  = tw->ResAddr;
            Base += (tw->ResEntrySize * EntryIdx);
        }
    }

    UT_Stub_SetReturnValue(FuncKey, Base);
}

void UT_Handler_CS_SetDefEntryState(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    struct CS_SetDefEntryState_Context *Ctxt = UserObj;

    Ctxt->EntryPtr = UT_Hook_GetArgValueByName(Context, "EntryPtr", void *);
    Ctxt->NewState = UT_Hook_GetArgValueByName(Context, "NewState", CS_ChecksumState_Enum_t);

    UT_Stub_SetReturnValue(FuncKey, Ctxt->OldState);
}

void CS_Test_Setup_SetDefEntryState(CS_ChecksumState_Enum_t OldState)
{
    memset(&CS_SetDefEntryState_Context, 0, sizeof(CS_SetDefEntryState_Context));
    CS_SetDefEntryState_Context.OldState = OldState;
    UT_SetHandlerFunction(UT_KEY(CS_SetDefEntryState), UT_Handler_CS_SetDefEntryState, &CS_SetDefEntryState_Context);
}

void CS_Test_Check_SetDefEntryState(void *EntryPtr, CS_ChecksumState_Enum_t NewState)
{
    UtAssert_ADDRESS_EQ(CS_SetDefEntryState_Context.EntryPtr, EntryPtr);
    UtAssert_UINT32_EQ(CS_SetDefEntryState_Context.NewState, NewState);
}

void CS_Test_ResetEventCheck(void)
{
    memset(context_CFE_EVS_SendEvent, 0, sizeof(context_CFE_EVS_SendEvent));
    UT_ResetState(UT_KEY(CFE_EVS_SendEvent));
    UT_SetVaHandlerFunction(UT_KEY(CFE_EVS_SendEvent), UT_Handler_CFE_EVS_SendEvent, NULL);
}

void CS_Test_Setup(void)
{
    CS_TableWrapper_t *tw;
    /* initialize test environment to default state for every test */
    UT_ResetState(0);

    memset(&CS_AppData, 0, sizeof(CS_AppData));
    memset(&context_CFE_ES_WriteToSysLog, 0, sizeof(context_CFE_ES_WriteToSysLog));

    /* Register custom handlers */
    CS_Test_ResetEventCheck();
    UT_SetVaHandlerFunction(UT_KEY(CFE_ES_WriteToSysLog), UT_Handler_CFE_ES_WriteToSysLog, NULL);

    UT_SetHandlerFunction(UT_KEY(CS_GetDefEntryAddr), UT_Handler_CS_TableEntry, NULL);
    UT_SetHandlerFunction(UT_KEY(CS_GetResEntryAddr), UT_Handler_CS_TableEntry, NULL);

    tw                = &CS_AppData.Tbl[CS_ChecksumType_CFECORE];
    tw->ResAddr       = &CS_AppData.CfeCoreCodeSeg;
    tw->NumEntries    = 1;
    tw->ResEntrySize  = sizeof(CS_Res_EepromMemory_Table_Entry_t);
    tw->BaselineValue = &CS_AppData.HkPacket.Payload.CfeCoreBaseline;
    tw                = &CS_AppData.Tbl[CS_ChecksumType_OSCORE];
    tw->ResAddr       = &CS_AppData.OSCodeSeg;
    tw->NumEntries    = 1;
    tw->ResEntrySize  = sizeof(CS_Res_EepromMemory_Table_Entry_t);
    tw->BaselineValue = &CS_AppData.HkPacket.Payload.OSBaseline;
    tw                = &CS_AppData.Tbl[CS_ChecksumType_EEPROM_TABLE];
    tw->DefAddr       = &CS_AppData.DefaultEepromDefTable[0];
    tw->ResAddr       = &CS_DefaultEepromResTable[0];
    tw->NumEntries    = CS_MAX_NUM_EEPROM_TABLE_ENTRIES;
    tw->DefEntrySize  = sizeof(CS_Def_EepromMemory_Table_Entry_t);
    tw->ResEntrySize  = sizeof(CS_Res_EepromMemory_Table_Entry_t);
    tw->GlobalState   = &CS_AppData.HkPacket.Payload.EepromCSState;
    tw                = &CS_AppData.Tbl[CS_ChecksumType_MEMORY_TABLE];
    tw->DefAddr       = &CS_AppData.DefaultMemoryDefTable[0];
    tw->ResAddr       = &CS_DefaultMemoryResTable[0];
    tw->NumEntries    = CS_MAX_NUM_MEMORY_TABLE_ENTRIES;
    tw->DefEntrySize  = sizeof(CS_Def_EepromMemory_Table_Entry_t);
    tw->ResEntrySize  = sizeof(CS_Res_EepromMemory_Table_Entry_t);
    tw->GlobalState   = &CS_AppData.HkPacket.Payload.MemoryCSState;
    tw                = &CS_AppData.Tbl[CS_ChecksumType_TABLES_TABLE];
    tw->DefAddr       = &CS_AppData.DefaultTablesDefTable[0];
    tw->ResAddr       = &CS_DefaultTablesResTable[0];
    tw->NumEntries    = CS_MAX_NUM_TABLES_TABLE_ENTRIES;
    tw->DefEntrySize  = sizeof(CS_Def_Tables_Table_Entry_t);
    tw->ResEntrySize  = sizeof(CS_Res_Tables_Table_Entry_t);
    tw->GlobalState   = &CS_AppData.HkPacket.Payload.TablesCSState;
    tw                = &CS_AppData.Tbl[CS_ChecksumType_APP_TABLE];
    tw->DefAddr       = &CS_AppData.DefaultAppDefTable[0];
    tw->ResAddr       = &CS_DefaultAppResTable[0];
    tw->NumEntries    = CS_MAX_NUM_APP_TABLE_ENTRIES;
    tw->DefEntrySize  = sizeof(CS_Def_App_Table_Entry_t);
    tw->ResEntrySize  = sizeof(CS_Res_App_Table_Entry_t);
    tw->GlobalState   = &CS_AppData.HkPacket.Payload.AppCSState;

    memset(CS_DefaultEepromResTable, 0, sizeof(CS_DefaultEepromResTable));
    memset(CS_DefaultMemoryResTable, 0, sizeof(CS_DefaultMemoryResTable));
    memset(CS_DefaultTablesResTable, 0, sizeof(CS_DefaultTablesResTable));
    memset(CS_DefaultAppResTable, 0, sizeof(CS_DefaultAppResTable));
}

void CS_Test_TearDown(void)
{
    /* cleanup test environment */
}
