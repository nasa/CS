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

#include "cs_compute.h"
#include "cs_msg.h"
#include "cs_msgdefs.h"
#include "cs_events.h"
#include "cs_version.h"
#include "cs_utils.h"
#include "cs_test_utils.h"
#include <unistd.h>
#include <stdlib.h>

/* UT includes */
#include "uttest.h"
#include "utassert.h"
#include "utstubs.h"

/* cs_compute_tests globals */
uint8 call_count_CFE_EVS_SendEvent;

/*
 * Function Definitions
 */

int32 CS_COMPUTE_TEST_CFE_TBL_GetAddressHook(void *UserObj, int32 StubRetcode, uint32 CallCount,
                                             const UT_StubContext_t *Context)
{
    /* This function exists so that one return code can be set for the 1st run and a different for the 2nd run */

    return CFE_TBL_ERR_UNREGISTERED;
}

int32 CS_COMPUTE_TEST_CFE_TBL_GetInfoHook1(void *UserObj, int32 StubRetcode, uint32 CallCount,
                                           const UT_StubContext_t *Context)
{
    CFE_TBL_Info_t *TblInfoPtr = (CFE_TBL_Info_t *)Context->ArgPtr[0];

    TblInfoPtr->Size = 5;

    return CFE_TBL_INFO_UPDATED;
}

void CS_COMPUTE_TEST_CFE_TBL_ShareHandler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CFE_TBL_Handle_t *TblHandlePtr =
        (CFE_TBL_Handle_t *)UT_Hook_GetArgValueByName(Context, "TblHandlePtr", CFE_TBL_Handle_t *);

    *TblHandlePtr = 99;
}

void CS_COMPUTE_TEST_CFE_ES_GetModuleInfoHandler1(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CFE_ES_AppInfo_t *AppInfo =
        (CFE_ES_AppInfo_t *)UT_Hook_GetArgValueByName(Context, "ModuleInfo", CFE_ES_AppInfo_t *);

    AppInfo->CodeSize          = 5;
    AppInfo->CodeAddress       = 1;
    AppInfo->AddressesAreValid = true;
}

void CS_COMPUTE_TEST_CFE_ES_GetModuleInfoHandler2(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CFE_ES_AppInfo_t *AppInfo =
        (CFE_ES_AppInfo_t *)UT_Hook_GetArgValueByName(Context, "ModuleInfo", CFE_ES_AppInfo_t *);

    AppInfo->AddressesAreValid = false;
}

void CS_ComputeEepromMemory_Test_Nominal(void)
{
    CS_Res_EepromMemory_Table_Entry_t ResultsEntry;
    uint32                            ComputedCSValue = 0;
    bool                              DoneWithEntry   = false;

    memset(&ResultsEntry, 0, sizeof(ResultsEntry));

    ResultsEntry.NumBytesToChecksum = 1;
    CS_AppData.MaxBytesPerCycle     = 2;

    ResultsEntry.ComputedYet = true;

    ResultsEntry.ComparisonValue = 1;

    /* Set to fail condition "NewChecksumValue != ResultsEntry -> ComparisonValue" */
    UT_SetDefaultReturnValue(UT_KEY(CFE_ES_CalculateCRC), 1);

    /* Execute the function being tested */
    UtAssert_UINT32_EQ(CS_ComputeEepromMemory(&ResultsEntry, &ComputedCSValue, &DoneWithEntry), CFE_SUCCESS);

    /* Verify results */
    UtAssert_BOOL_TRUE(DoneWithEntry);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
}

void CS_ComputeEepromMemory_Test_Error(void)
{
    CS_Res_EepromMemory_Table_Entry_t ResultsEntry;
    uint32                            ComputedCSValue = 0;
    bool                              DoneWithEntry   = false;

    memset(&ResultsEntry, 0, sizeof(ResultsEntry));

    ResultsEntry.NumBytesToChecksum = 1;
    CS_AppData.MaxBytesPerCycle     = 2;

    ResultsEntry.ComputedYet = true;

    ResultsEntry.ComparisonValue = 5;

    /* Set to satisfy condition "NewChecksumValue != ResultsEntry -> ComparisonValue" */
    UT_SetDefaultReturnValue(UT_KEY(CFE_ES_CalculateCRC), 1);

    /* Execute the function being tested */
    UtAssert_UINT32_EQ(CS_ComputeEepromMemory(&ResultsEntry, &ComputedCSValue, &DoneWithEntry), CS_ERROR);

    /* Verify results */
    UtAssert_BOOL_TRUE(DoneWithEntry);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
}

void CS_ComputeEepromMemory_Test_FirstTimeThrough(void)
{
    CS_Res_EepromMemory_Table_Entry_t ResultsEntry;
    uint32                            ComputedCSValue = 0;
    bool                              DoneWithEntry   = false;

    memset(&ResultsEntry, 0, sizeof(ResultsEntry));

    ResultsEntry.NumBytesToChecksum = 1;
    CS_AppData.MaxBytesPerCycle     = 2;

    ResultsEntry.ComputedYet = false;

    /* ComputedCSValue and ResultsEntry.ComparisonValue will be set to value returned by this function */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_CalculateCRC), 1, 1);

    /* Execute the function being tested */
    UtAssert_UINT32_EQ(CS_ComputeEepromMemory(&ResultsEntry, &ComputedCSValue, &DoneWithEntry), CFE_SUCCESS);

    /* Verify results */
    UtAssert_BOOL_TRUE(DoneWithEntry);

    UtAssert_BOOL_TRUE(ResultsEntry.ComputedYet);
    UtAssert_UINT32_EQ(ResultsEntry.ComparisonValue, 1);
    UtAssert_UINT32_EQ(ComputedCSValue, 1);
    UtAssert_UINT32_EQ(ResultsEntry.ByteOffset, 0);
    UtAssert_UINT32_EQ(ResultsEntry.TempChecksumValue, 0);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
}

void CS_ComputeEepromMemory_Test_NotFinished(void)
{
    CS_Res_EepromMemory_Table_Entry_t ResultsEntry;
    uint32                            ComputedCSValue = 0;
    bool                              DoneWithEntry   = false;

    memset(&ResultsEntry, 0, sizeof(ResultsEntry));

    ResultsEntry.NumBytesToChecksum = 2;
    CS_AppData.MaxBytesPerCycle     = 1;

    ResultsEntry.ComputedYet = false;

    /* ComputedCSValue and ResultsEntry.TempChecksumValue will be set to value returned by this function */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_CalculateCRC), 1, 1);

    /* Execute the function being tested */
    UtAssert_UINT32_EQ(CS_ComputeEepromMemory(&ResultsEntry, &ComputedCSValue, &DoneWithEntry), CFE_SUCCESS);

    /* Verify results */
    UtAssert_UINT32_EQ(ResultsEntry.ByteOffset, 1);
    UtAssert_UINT32_EQ(ResultsEntry.TempChecksumValue, 1);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
}

void CS_ComputeTables_Test_TableNeverLoaded(void)
{
    CS_Res_Tables_Table_Entry_t ResultsEntry;
    uint32                      ComputedCSValue = 0;
    bool                        DoneWithEntry   = true;
    char                        ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    memset(&ResultsEntry, 0, sizeof(ResultsEntry));

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS Tables: Problem Getting table %%s info Share: 0x%%08X, GetInfo: 0x%%08X, GetAddress: 0x%%08X");

    ResultsEntry.TblHandle = 99;

    strncpy(ResultsEntry.Name, "name", 10);

    /* Set to satisfy first instance of condition "ResultGetAddress == CFE_TBL_ERR_NEVER_LOADED" */
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_GetAddress), 1, CFE_TBL_ERR_NEVER_LOADED);

    /* Execute the function being tested */
    UtAssert_UINT32_EQ(CS_ComputeTables(&ResultsEntry, &ComputedCSValue, &DoneWithEntry), CS_ERR_NOT_FOUND);

    /* Verify results */
    UtAssert_BOOL_FALSE(DoneWithEntry);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_COMPUTE_TABLES_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    UtAssert_STRINGBUF_EQ(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, context_CFE_EVS_SendEvent[0].Spec,
                          CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
}

void CS_ComputeTables_Test_TableUnregisteredAndNeverLoaded(void)
{
    CS_Res_Tables_Table_Entry_t ResultsEntry;
    uint32                      ComputedCSValue = 0;
    bool                        DoneWithEntry   = true;
    char                        ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    memset(&ResultsEntry, 0, sizeof(ResultsEntry));

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS Tables: Problem Getting table %%s info Share: 0x%%08X, GetInfo: 0x%%08X, GetAddress: 0x%%08X");

    ResultsEntry.TblHandle = 99;

    strncpy(ResultsEntry.Name, "name", 10);

    /* Set to satisfy condition "Result == CFE_TBL_ERR_UNREGISTERED" */
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_GetInfo), 1, CFE_TBL_ERR_UNREGISTERED);

    /* Satisfies second instance of conditions "Result == CFE_TBL_ERR_UNREGISTERED" and "ResultGetAddress ==
     * CFE_TBL_ERR_NEVER_LOADED" */
    UT_SetHookFunction(UT_KEY(CFE_TBL_GetAddress), &CS_COMPUTE_TEST_CFE_TBL_GetAddressHook, NULL);
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_GetAddress), 2, CFE_TBL_ERR_NEVER_LOADED);
    UT_SetDeferredRetcode(UT_KEY(CS_AttemptTableReshare), 1, -1);

    /* Execute the function being tested */
    UtAssert_UINT32_EQ(CS_ComputeTables(&ResultsEntry, &ComputedCSValue, &DoneWithEntry), CS_ERR_NOT_FOUND);

    /* Verify results */
    UtAssert_BOOL_FALSE(DoneWithEntry);

    UtAssert_UINT32_EQ(ResultsEntry.ComparisonValue, 0);
    UtAssert_UINT32_EQ(ResultsEntry.StartAddress, 0);
    UtAssert_UINT32_EQ(ResultsEntry.NumBytesToChecksum, 0);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_COMPUTE_TABLES_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    UtAssert_STRINGBUF_EQ(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, context_CFE_EVS_SendEvent[0].Spec,
                          CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
}

void CS_ComputeTables_Test_ResultShareNotSuccess(void)
{
    CS_Res_Tables_Table_Entry_t ResultsEntry;
    uint32                      ComputedCSValue = 0;
    bool                        DoneWithEntry   = true;
    char                        ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    memset(&ResultsEntry, 0, sizeof(ResultsEntry));

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS Tables: Problem Getting table %%s info Share: 0x%%08X, GetInfo: 0x%%08X, GetAddress: 0x%%08X");

    ResultsEntry.TblHandle = CFE_TBL_BAD_TABLE_HANDLE;

    strncpy(ResultsEntry.Name, "name", 10);

    /* Set to satisfy condition "Result == CFE_TBL_ERR_UNREGISTERED" */
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_GetInfo), 1, CFE_TBL_ERR_UNREGISTERED);

    /* Set to fail subsequent condition "ResultShare == CFE_SUCCESS" */
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_Share), 1, -1);
    UT_SetDeferredRetcode(UT_KEY(CS_AttemptTableReshare), 1, -1);

    /* Set to satisfy second instance of condition "ResultGetAddress == CFE_TBL_ERR_NEVER_LOADED" */
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_GetAddress), 1, CFE_TBL_ERR_UNREGISTERED);

    /* Execute the function being tested */
    UtAssert_UINT32_EQ(CS_ComputeTables(&ResultsEntry, &ComputedCSValue, &DoneWithEntry), CS_ERR_NOT_FOUND);

    /* Verify results */
    UtAssert_BOOL_FALSE(DoneWithEntry);

    UtAssert_INT32_EQ(ResultsEntry.TblHandle, CFE_TBL_BAD_TABLE_HANDLE);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_COMPUTE_TABLES_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    UtAssert_STRINGBUF_EQ(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, context_CFE_EVS_SendEvent[0].Spec,
                          CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
}

void CS_ComputeTables_Test_TblInfoUpdated(void)
{
    CS_Res_Tables_Table_Entry_t ResultsEntry;
    uint32                      ComputedCSValue = 0;
    bool                        DoneWithEntry   = true;
    CFE_TBL_Info_t              TblInfo;

    memset(&ResultsEntry, 0, sizeof(ResultsEntry));
    memset(&TblInfo, 0, sizeof(TblInfo));

    ResultsEntry.TblHandle      = 99;
    CS_AppData.MaxBytesPerCycle = 5;

    /* Sets TblInfo.Size = 5 and returns CFE_TBL_INFO_UPDATED */
    TblInfo.Size = 5;

    UT_SetDataBuffer(UT_KEY(CFE_TBL_GetInfo), &TblInfo, sizeof(TblInfo), false);

    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetInfo), CFE_SUCCESS);

    /* Set to satisfy subsequent condition "Result == CFE_TBL_INFO_UPDATED" */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_TBL_INFO_UPDATED);

    /* Execute the function being tested */
    UtAssert_UINT32_EQ(CS_ComputeTables(&ResultsEntry, &ComputedCSValue, &DoneWithEntry), CFE_SUCCESS);

    /* Verify results */
    UtAssert_BOOL_FALSE(DoneWithEntry);

    UtAssert_UINT32_EQ(ResultsEntry.NumBytesToChecksum, 5);
    UtAssert_UINT32_EQ(ResultsEntry.StartAddress, 0);
    UtAssert_UINT32_EQ(ComputedCSValue, 0);
    UtAssert_BOOL_FALSE(ResultsEntry.ComputedYet);
    UtAssert_UINT32_EQ(ResultsEntry.ComparisonValue, 0);
    UtAssert_UINT32_EQ(ResultsEntry.ByteOffset, 0);
    UtAssert_UINT32_EQ(ResultsEntry.TempChecksumValue, 0);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
}

void CS_ComputeTables_Test_GetInfoResult(void)
{
    CS_Res_Tables_Table_Entry_t ResultsEntry;
    uint32                      ComputedCSValue = 0;
    bool                        DoneWithEntry   = true;
    CFE_TBL_Info_t              TblInfo;

    memset(&ResultsEntry, 0, sizeof(ResultsEntry));
    memset(&TblInfo, 0, sizeof(TblInfo));

    ResultsEntry.TblHandle = 99;

    CS_AppData.MaxBytesPerCycle = 5;

    /* Sets TblInfo.Size = 5 and returns CFE_TBL_INFO_UPDATED */
    TblInfo.Size = 5;

    UT_SetDataBuffer(UT_KEY(CFE_TBL_GetInfo), &TblInfo, sizeof(TblInfo), false);

    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetInfo), -1);

    /* Set to satisfy subsequent condition "Result == CFE_TBL_INFO_UPDATED" */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_TBL_INFO_UPDATED);

    /* Execute the function being tested */
    UtAssert_UINT32_EQ(CS_ComputeTables(&ResultsEntry, &ComputedCSValue, &DoneWithEntry), CFE_SUCCESS);

    /* Verify results */
    UtAssert_BOOL_FALSE(DoneWithEntry);

    UtAssert_UINT32_EQ(ResultsEntry.NumBytesToChecksum, 0);
    UtAssert_UINT32_EQ(ResultsEntry.StartAddress, 0);
    UtAssert_UINT32_EQ(ComputedCSValue, 0);
    UtAssert_BOOL_FALSE(ResultsEntry.ComputedYet);
    UtAssert_UINT32_EQ(ResultsEntry.ComparisonValue, 0);
    UtAssert_UINT32_EQ(ResultsEntry.ByteOffset, 0);
    UtAssert_UINT32_EQ(ResultsEntry.TempChecksumValue, 0);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
}

void CS_ComputeTables_Test_CSError(void)
{
    CS_Res_Tables_Table_Entry_t ResultsEntry;
    uint32                      ComputedCSValue = 0;
    bool                        DoneWithEntry   = false;
    CFE_TBL_Info_t              TblInfo;

    memset(&ResultsEntry, 0, sizeof(ResultsEntry));
    memset(&TblInfo, 0, sizeof(TblInfo));

    ResultsEntry.TblHandle = 99;

    CS_AppData.MaxBytesPerCycle = 5;

    ResultsEntry.ComputedYet = true;

    ResultsEntry.ComparisonValue = 1;

    /* Sets TblInfo.Size = 5 and returns CFE_SUCCESS */
    TblInfo.Size = 5;

    UT_SetDataBuffer(UT_KEY(CFE_TBL_GetInfo), &TblInfo, sizeof(TblInfo), false);

    /* Set to satisfy condition "Result == CFE_SUCCESS" and to fail other conditions that check for other values of
     * Result */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_SUCCESS);

    /* Set to fail condition "NewChecksumValue != ResultsEntry -> ComparisonValue" */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_CalculateCRC), 1, 2);

    /* Execute the function being tested */
    UtAssert_UINT32_EQ(CS_ComputeTables(&ResultsEntry, &ComputedCSValue, &DoneWithEntry), CS_ERROR);

    /* Verify results */
    UtAssert_BOOL_TRUE(DoneWithEntry);

    UtAssert_UINT32_EQ(ResultsEntry.NumBytesToChecksum, 5);
    UtAssert_UINT32_EQ(ResultsEntry.StartAddress, 0);
    UtAssert_UINT32_EQ(ComputedCSValue, 2);
    UtAssert_UINT32_EQ(ResultsEntry.ByteOffset, 0);
    UtAssert_UINT32_EQ(ResultsEntry.TempChecksumValue, 0);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
}

void CS_ComputeTables_Test_NominalBadTableHandle(void)
{
    CS_Res_Tables_Table_Entry_t ResultsEntry;
    uint32                      ComputedCSValue = 0;
    bool                        DoneWithEntry   = false;
    CFE_TBL_Info_t              TblInfo;

    memset(&ResultsEntry, 0, sizeof(ResultsEntry));
    memset(&TblInfo, 0, sizeof(TblInfo));

    ResultsEntry.TblHandle = CFE_TBL_BAD_TABLE_HANDLE;

    CS_AppData.MaxBytesPerCycle = 5;

    ResultsEntry.ComputedYet = true;

    ResultsEntry.ComparisonValue = 2;

    /* Sets ResultsEntry->TblHandle to 99 and returns CFE_SUCCESS */
    UT_SetHandlerFunction(UT_KEY(CFE_TBL_Share), CS_COMPUTE_TEST_CFE_TBL_ShareHandler, NULL);

    /* Sets TblInfo.Size = 5 and returns CFE_SUCCESS */
    TblInfo.Size = 5;

    UT_SetDataBuffer(UT_KEY(CFE_TBL_GetInfo), &TblInfo, sizeof(TblInfo), false);

    /* Set to satisfy condition "Result == CFE_SUCCESS" and to fail other conditions that check for other values of
     * Result */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_SUCCESS);

    /* Set to fail condition "NewChecksumValue != ResultsEntry -> ComparisonValue" */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_CalculateCRC), 1, 2);

    /* Execute the function being tested */
    UtAssert_UINT32_EQ(CS_ComputeTables(&ResultsEntry, &ComputedCSValue, &DoneWithEntry), CFE_SUCCESS);

    /* Verify results */
    UtAssert_BOOL_TRUE(DoneWithEntry);

    UtAssert_INT32_EQ(ResultsEntry.TblHandle, 99);

    UtAssert_UINT32_EQ(ResultsEntry.NumBytesToChecksum, 5);
    UtAssert_UINT32_EQ(ResultsEntry.StartAddress, 0);
    UtAssert_UINT32_EQ(ComputedCSValue, 2);
    UtAssert_UINT32_EQ(ResultsEntry.ByteOffset, 0);
    UtAssert_UINT32_EQ(ResultsEntry.TempChecksumValue, 0);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
}

void CS_ComputeTables_Test_FirstTimeThrough(void)
{
    CS_Res_Tables_Table_Entry_t ResultsEntry;
    uint32                      ComputedCSValue = 0;
    bool                        DoneWithEntry   = false;
    CFE_TBL_Info_t              TblInfo;

    memset(&ResultsEntry, 0, sizeof(ResultsEntry));
    memset(&TblInfo, 0, sizeof(TblInfo));

    ResultsEntry.TblHandle = CFE_TBL_BAD_TABLE_HANDLE;

    CS_AppData.MaxBytesPerCycle = 5;

    ResultsEntry.ComputedYet = false;

    ResultsEntry.ComparisonValue = 2;

    /* Sets ResultsEntry->TblHandle to 99 and returns CFE_SUCCESS */
    UT_SetHandlerFunction(UT_KEY(CFE_TBL_Share), CS_COMPUTE_TEST_CFE_TBL_ShareHandler, NULL);

    /* Sets TblInfo.Size = 5 and returns CFE_SUCCESS */
    TblInfo.Size = 5;

    UT_SetDataBuffer(UT_KEY(CFE_TBL_GetInfo), &TblInfo, sizeof(TblInfo), false);

    /* Set to satisfy condition "Result == CFE_SUCCESS" and to fail other conditions that check for other values of
     * Result */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_SUCCESS);

    /* Set to cause ResultsEntry->ComparisonValue to be set to 3 */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_CalculateCRC), 1, 3);

    /* Execute the function being tested */
    UtAssert_UINT32_EQ(CS_ComputeTables(&ResultsEntry, &ComputedCSValue, &DoneWithEntry), CFE_SUCCESS);

    /* Verify results */
    UtAssert_BOOL_TRUE(DoneWithEntry);

    UtAssert_INT32_EQ(ResultsEntry.TblHandle, 99);

    UtAssert_UINT32_EQ(ResultsEntry.NumBytesToChecksum, 5);
    UtAssert_UINT32_EQ(ResultsEntry.StartAddress, 0);

    UtAssert_BOOL_TRUE(ResultsEntry.ComputedYet);
    UtAssert_UINT32_EQ(ResultsEntry.ComparisonValue, 3);

    UtAssert_UINT32_EQ(ComputedCSValue, 3);
    UtAssert_UINT32_EQ(ResultsEntry.ByteOffset, 0);
    UtAssert_UINT32_EQ(ResultsEntry.TempChecksumValue, 0);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
}

void CS_ComputeTables_Test_EntryNotFinished(void)
{
    CS_Res_Tables_Table_Entry_t ResultsEntry;
    uint32                      ComputedCSValue = 0;
    bool                        DoneWithEntry   = true;
    CFE_TBL_Info_t              TblInfo;

    memset(&ResultsEntry, 0, sizeof(ResultsEntry));
    memset(&TblInfo, 0, sizeof(TblInfo));

    ResultsEntry.TblHandle = CFE_TBL_BAD_TABLE_HANDLE;

    CS_AppData.MaxBytesPerCycle = 3;

    ResultsEntry.ComputedYet = false;

    ResultsEntry.ComparisonValue = 2;

    /* Sets ResultsEntry->TblHandle to 99 and returns CFE_SUCCESS */
    UT_SetHandlerFunction(UT_KEY(CFE_TBL_Share), CS_COMPUTE_TEST_CFE_TBL_ShareHandler, NULL);

    /* Sets TblInfo.Size = 5 and returns CFE_SUCCESS */
    TblInfo.Size = 5;

    UT_SetDataBuffer(UT_KEY(CFE_TBL_GetInfo), &TblInfo, sizeof(TblInfo), false);

    /* Set to satisfy condition "Result == CFE_SUCCESS" and to fail other conditions that check for other values of
     * Result */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_SUCCESS);

    /* Set to cause ResultsEntry->ComparisonValue to be set to 3 */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_CalculateCRC), 1, 3);

    /* Execute the function being tested */
    UtAssert_UINT32_EQ(CS_ComputeTables(&ResultsEntry, &ComputedCSValue, &DoneWithEntry), CFE_SUCCESS);

    /* Verify results */
    UtAssert_BOOL_FALSE(DoneWithEntry);

    UtAssert_INT32_EQ(ResultsEntry.TblHandle, 99);

    UtAssert_UINT32_EQ(ResultsEntry.NumBytesToChecksum, 5);
    UtAssert_UINT32_EQ(ResultsEntry.StartAddress, 0);

    UtAssert_UINT32_EQ(ComputedCSValue, 3);
    UtAssert_UINT32_EQ(ResultsEntry.ByteOffset, 3);
    UtAssert_UINT32_EQ(ResultsEntry.TempChecksumValue, 3);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
}

void CS_ComputeTables_Test_ComputeTablesReleaseError(void)
{
    CS_Res_Tables_Table_Entry_t ResultsEntry;
    uint32                      ComputedCSValue = 0;
    bool                        DoneWithEntry   = true;
    CFE_TBL_Info_t              TblInfo;
    char                        ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    memset(&ResultsEntry, 0, sizeof(ResultsEntry));
    memset(&TblInfo, 0, sizeof(TblInfo));

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS Tables: Could not release addresss for table %%s, returned: 0x%%08X");

    ResultsEntry.TblHandle = CFE_TBL_BAD_TABLE_HANDLE;

    CS_AppData.MaxBytesPerCycle = 3;

    ResultsEntry.ComputedYet = false;

    ResultsEntry.ComparisonValue = 2;

    strncpy(ResultsEntry.Name, "name", 10);

    /* Sets ResultsEntry->TblHandle to 99 and returns CFE_SUCCESS */
    UT_SetHandlerFunction(UT_KEY(CFE_TBL_Share), CS_COMPUTE_TEST_CFE_TBL_ShareHandler, NULL);

    /* Sets TblInfo.Size = 5 and returns CFE_SUCCESS */
    TblInfo.Size = 5;

    UT_SetDataBuffer(UT_KEY(CFE_TBL_GetInfo), &TblInfo, sizeof(TblInfo), false);

    /* Set to satisfy condition "Result == CFE_SUCCESS" and to fail other conditions that check for other values of
     * Result */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_SUCCESS);

    /* Set to cause ResultsEntry->ComparisonValue to be set to 3 */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_CalculateCRC), 1, 3);

    /* Set to generate error message CS_COMPUTE_TABLES_RELEASE_ERR_EID */
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_ReleaseAddress), 1, -1);

    /* Execute the function being tested */
    UtAssert_UINT32_EQ(CS_ComputeTables(&ResultsEntry, &ComputedCSValue, &DoneWithEntry), CFE_SUCCESS);

    /* Verify results */
    UtAssert_BOOL_FALSE(DoneWithEntry);

    UtAssert_INT32_EQ(ResultsEntry.TblHandle, 99);

    UtAssert_UINT32_EQ(ResultsEntry.NumBytesToChecksum, 5);
    UtAssert_UINT32_EQ(ResultsEntry.StartAddress, 0);

    UtAssert_UINT32_EQ(ResultsEntry.ByteOffset, 3);
    UtAssert_UINT32_EQ(ResultsEntry.TempChecksumValue, 3);
    UtAssert_UINT32_EQ(ComputedCSValue, 3);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_COMPUTE_TABLES_RELEASE_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    UtAssert_STRINGBUF_EQ(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, context_CFE_EVS_SendEvent[0].Spec,
                          CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
}

void CS_ComputeTables_Test_ComputeTablesError(void)
{
    CS_Res_Tables_Table_Entry_t ResultsEntry;
    uint32                      ComputedCSValue = 0;
    bool                        DoneWithEntry   = true;
    CFE_TBL_Info_t              TblInfo;
    char                        ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    memset(&ResultsEntry, 0, sizeof(ResultsEntry));
    memset(&TblInfo, 0, sizeof(TblInfo));

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS Tables: Problem Getting table %%s info Share: 0x%%08X, GetInfo: 0x%%08X, GetAddress: 0x%%08X");

    ResultsEntry.TblHandle = CFE_TBL_BAD_TABLE_HANDLE;

    strncpy(ResultsEntry.Name, "name", 10);

    /* Sets ResultsEntry->TblHandle to 99 and returns CFE_SUCCESS */
    UT_SetHandlerFunction(UT_KEY(CFE_TBL_Share), CS_COMPUTE_TEST_CFE_TBL_ShareHandler, NULL);

    /* Sets TblInfo.Size = 5 and returns CFE_SUCCESS */
    TblInfo.Size = 5;

    UT_SetDataBuffer(UT_KEY(CFE_TBL_GetInfo), &TblInfo, sizeof(TblInfo), false);

    /* Set to generate error message CS_COMPUTE_TABLES_ERR_EID */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), -1);

    /* Execute the function being tested */
    UtAssert_UINT32_EQ(CS_ComputeTables(&ResultsEntry, &ComputedCSValue, &DoneWithEntry), CS_ERR_NOT_FOUND);

    /* Verify results */
    UtAssert_BOOL_FALSE(DoneWithEntry);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_COMPUTE_TABLES_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    UtAssert_STRINGBUF_EQ(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, context_CFE_EVS_SendEvent[0].Spec,
                          CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
}

void CS_ComputeApp_Test_NominalApp(void)
{
    CS_Res_App_Table_Entry_t ResultsEntry;
    uint32                   ComputedCSValue = 0;
    bool                     DoneWithEntry   = false;

    memset(&ResultsEntry, 0, sizeof(ResultsEntry));

    CS_AppData.MaxBytesPerCycle = 5;

    ResultsEntry.ComputedYet = true;

    ResultsEntry.ComparisonValue = 2;

    /* Sets AppInfo.CodeSize = 5, sets AppInfo.CodeAddress = 1, AppInfo.AddressesAreValid = true, and returns
     * CFE_SUCCESS */
    UT_SetHandlerFunction(UT_KEY(CFE_ES_GetModuleInfo), CS_COMPUTE_TEST_CFE_ES_GetModuleInfoHandler1, NULL);

    /* Set to fail condition "NewChecksumValue != ResultsEntry -> ComparisonValue" */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_CalculateCRC), 1, 2);

    /* Execute the function being tested */
    UtAssert_UINT32_EQ(CS_ComputeApp(&ResultsEntry, &ComputedCSValue, &DoneWithEntry), CFE_SUCCESS);

    /* Verify results */
    UtAssert_BOOL_TRUE(DoneWithEntry);

    UtAssert_UINT32_EQ(ResultsEntry.NumBytesToChecksum, 5);
    UtAssert_UINT32_EQ(ResultsEntry.StartAddress, 1);

    UtAssert_UINT32_EQ(ComputedCSValue, 2);
    UtAssert_UINT32_EQ(ResultsEntry.ByteOffset, 0);
    UtAssert_UINT32_EQ(ResultsEntry.TempChecksumValue, 0);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
}

void CS_ComputeApp_Test_NominalLib(void)
{
    CS_Res_App_Table_Entry_t ResultsEntry;
    uint32                   ComputedCSValue = 0;
    bool                     DoneWithEntry   = false;

    memset(&ResultsEntry, 0, sizeof(ResultsEntry));

    CS_AppData.MaxBytesPerCycle = 5;

    ResultsEntry.ComputedYet = true;

    ResultsEntry.ComparisonValue = 2;

    /* Set to generate error CS_COMPUTE_APP_ERR_EID */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetAppIDByName), 1, CFE_ES_ERR_NAME_NOT_FOUND);

    /* Sets AppInfo.CodeSize = 5, sets AppInfo.CodeAddress = 1, AppInfo.AddressesAreValid = true, and returns
     * CFE_SUCCESS */
    UT_SetHandlerFunction(UT_KEY(CFE_ES_GetModuleInfo), CS_COMPUTE_TEST_CFE_ES_GetModuleInfoHandler1, NULL);

    /* Set to fail condition "NewChecksumValue != ResultsEntry -> ComparisonValue" */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_CalculateCRC), 1, 2);

    /* Execute the function being tested */
    UtAssert_UINT32_EQ(CS_ComputeApp(&ResultsEntry, &ComputedCSValue, &DoneWithEntry), CFE_SUCCESS);

    /* Verify results */
    UtAssert_BOOL_TRUE(DoneWithEntry);

    UtAssert_UINT32_EQ(ResultsEntry.NumBytesToChecksum, 5);
    UtAssert_UINT32_EQ(ResultsEntry.StartAddress, 1);

    UtAssert_UINT32_EQ(ComputedCSValue, 2);
    UtAssert_UINT32_EQ(ResultsEntry.ByteOffset, 0);
    UtAssert_UINT32_EQ(ResultsEntry.TempChecksumValue, 0);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
}

void CS_ComputeApp_Test_GetAppAndLibIDByNameError(void)
{
    CS_Res_App_Table_Entry_t ResultsEntry;
    uint32                   ComputedCSValue = 0;
    bool                     DoneWithEntry   = true;
    char                     ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    memset(&ResultsEntry, 0, sizeof(ResultsEntry));

    snprintf(
        ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
        "CS Apps: Problems getting module %%s info, GetResourceID: 0x%%08X, GetModuleInfo: 0x%%08X, AddressValid: %%d");

    strncpy(ResultsEntry.Name, "name", 10);

    /* Set to generate error CS_COMPUTE_APP_ERR_EID */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetAppIDByName), 1, -1);

    /* Set to generate error CS_COMPUTE_APP_ERR_EID */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetLibIDByName), 1, -1);

    /* Execute the function being tested */
    UtAssert_UINT32_EQ(CS_ComputeApp(&ResultsEntry, &ComputedCSValue, &DoneWithEntry), CS_ERR_NOT_FOUND);

    /* Verify results */
    UtAssert_BOOL_FALSE(DoneWithEntry);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_COMPUTE_APP_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    UtAssert_STRINGBUF_EQ(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, context_CFE_EVS_SendEvent[0].Spec,
                          CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
}

void CS_ComputeApp_Test_GetModuleInfoError(void)
{
    CS_Res_App_Table_Entry_t ResultsEntry;
    uint32                   ComputedCSValue = 0;
    bool                     DoneWithEntry   = true;
    char                     ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    memset(&ResultsEntry, 0, sizeof(ResultsEntry));

    snprintf(
        ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
        "CS Apps: Problems getting module %%s info, GetResourceID: 0x%%08X, GetModuleInfo: 0x%%08X, AddressValid: %%d");

    strncpy(ResultsEntry.Name, "name", 10);

    /* Set to generate error CS_COMPUTE_APP_ERR_EID */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetModuleInfo), 1, -1);

    /* Execute the function being tested */
    UtAssert_UINT32_EQ(CS_ComputeApp(&ResultsEntry, &ComputedCSValue, &DoneWithEntry), CS_ERR_NOT_FOUND);

    /* Verify results */
    UtAssert_BOOL_FALSE(DoneWithEntry);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_COMPUTE_APP_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    UtAssert_STRINGBUF_EQ(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, context_CFE_EVS_SendEvent[0].Spec,
                          CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
}

void CS_ComputeApp_Test_ComputeAppPlatformError(void)
{
    CS_Res_App_Table_Entry_t ResultsEntry;
    uint32                   ComputedCSValue = 0;
    bool                     DoneWithEntry   = true;
    char                     ExpectedEventString[2][CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    memset(&ResultsEntry, 0, sizeof(ResultsEntry));

    snprintf(ExpectedEventString[0], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CS cannot get a valid address for %%s, due to the platform");

    snprintf(
        ExpectedEventString[1], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
        "CS Apps: Problems getting module %%s info, GetResourceID: 0x%%08X, GetModuleInfo: 0x%%08X, AddressValid: %%d");

    strncpy(ResultsEntry.Name, "name", 10);

    /* Sets AppInfo.AddressesAreValid = false and returns CFE_SUCCESS */
    UT_SetHandlerFunction(UT_KEY(CFE_ES_GetModuleInfo), CS_COMPUTE_TEST_CFE_ES_GetModuleInfoHandler2, NULL);

    /* Execute the function being tested */
    UtAssert_UINT32_EQ(CS_ComputeApp(&ResultsEntry, &ComputedCSValue, &DoneWithEntry), CS_ERR_NOT_FOUND);

    /* Verify results */
    UtAssert_BOOL_FALSE(DoneWithEntry);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_COMPUTE_APP_PLATFORM_DBG_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_DEBUG);

    UtAssert_STRINGBUF_EQ(ExpectedEventString[0], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, context_CFE_EVS_SendEvent[0].Spec,
                          CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, CS_COMPUTE_APP_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_ERROR);

    UtAssert_STRINGBUF_EQ(ExpectedEventString[1], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, context_CFE_EVS_SendEvent[1].Spec,
                          CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 2);
}

void CS_ComputeApp_Test_DifferFromSavedValue(void)
{
    CS_Res_App_Table_Entry_t ResultsEntry;
    uint32                   ComputedCSValue = 0;
    bool                     DoneWithEntry   = false;

    memset(&ResultsEntry, 0, sizeof(ResultsEntry));

    CS_AppData.MaxBytesPerCycle = 5;

    ResultsEntry.ComputedYet = true;

    ResultsEntry.ComparisonValue = 3;

    /* Sets AppInfo.CodeSize = 5, sets AppInfo.CodeAddress = 1, AppInfo.AddressesAreValid = true, and returns
     * CFE_SUCCESS */
    UT_SetHandlerFunction(UT_KEY(CFE_ES_GetModuleInfo), CS_COMPUTE_TEST_CFE_ES_GetModuleInfoHandler1, NULL);

    /* Set to fail condition "NewChecksumValue != ResultsEntry -> ComparisonValue" */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_CalculateCRC), 1, 2);

    /* Execute the function being tested */
    UtAssert_UINT32_EQ(CS_ComputeApp(&ResultsEntry, &ComputedCSValue, &DoneWithEntry), CS_ERROR);

    /* Verify results */
    UtAssert_BOOL_TRUE(DoneWithEntry);

    UtAssert_UINT32_EQ(ResultsEntry.NumBytesToChecksum, 5);
    UtAssert_UINT32_EQ(ResultsEntry.StartAddress, 1);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
}

void CS_ComputeApp_Test_FirstTimeThrough(void)
{
    CS_Res_App_Table_Entry_t ResultsEntry;
    uint32                   ComputedCSValue = 0;
    bool                     DoneWithEntry   = false;

    memset(&ResultsEntry, 0, sizeof(ResultsEntry));

    CS_AppData.MaxBytesPerCycle = 5;

    ResultsEntry.ComputedYet = false;

    ResultsEntry.ComparisonValue = 3;

    /* Sets AppInfo.CodeSize = 5, sets AppInfo.CodeAddress = 1, AppInfo.AddressesAreValid = true, and returns
     * CFE_SUCCESS */
    UT_SetHandlerFunction(UT_KEY(CFE_ES_GetModuleInfo), CS_COMPUTE_TEST_CFE_ES_GetModuleInfoHandler1, NULL);

    /* Set to fail condition "NewChecksumValue != ResultsEntry -> ComparisonValue" */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_CalculateCRC), 1, 2);

    /* Execute the function being tested */
    UtAssert_UINT32_EQ(CS_ComputeApp(&ResultsEntry, &ComputedCSValue, &DoneWithEntry), CFE_SUCCESS);

    /* Verify results */
    UtAssert_BOOL_TRUE(DoneWithEntry);

    UtAssert_UINT32_EQ(ResultsEntry.NumBytesToChecksum, 5);
    UtAssert_UINT32_EQ(ResultsEntry.StartAddress, 1);

    UtAssert_BOOL_TRUE(ResultsEntry.ComputedYet);
    UtAssert_UINT32_EQ(ResultsEntry.ComparisonValue, 2);

    UtAssert_UINT32_EQ(ComputedCSValue, 2);
    UtAssert_UINT32_EQ(ResultsEntry.ByteOffset, 0);
    UtAssert_UINT32_EQ(ResultsEntry.TempChecksumValue, 0);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
}

void CS_ComputeApp_Test_EntryNotFinished(void)
{
    CS_Res_App_Table_Entry_t ResultsEntry;
    uint32                   ComputedCSValue = 0;
    bool                     DoneWithEntry   = true;
    CFE_TBL_Info_t           TblInfo;

    memset(&ResultsEntry, 0, sizeof(ResultsEntry));
    memset(&TblInfo, 0, sizeof(TblInfo));

    CS_AppData.MaxBytesPerCycle = 3;

    ResultsEntry.ComputedYet = true;

    ResultsEntry.ComparisonValue = 3;

    /* Sets AppInfo.CodeSize = 5, sets AppInfo.CodeAddress = 1, AppInfo.AddressesAreValid = true, and returns
     * CFE_SUCCESS */
    UT_SetHandlerFunction(UT_KEY(CFE_ES_GetModuleInfo), CS_COMPUTE_TEST_CFE_ES_GetModuleInfoHandler1, NULL);

    TblInfo.Size = 5;

    UT_SetDataBuffer(UT_KEY(CFE_TBL_GetInfo), &TblInfo, sizeof(TblInfo), false);

    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetInfo), CFE_TBL_INFO_UPDATED);

    /* Set to fail condition "NewChecksumValue != ResultsEntry -> ComparisonValue" */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_CalculateCRC), 1, 2);

    /* Execute the function being tested */
    UtAssert_UINT32_EQ(CS_ComputeApp(&ResultsEntry, &ComputedCSValue, &DoneWithEntry), CFE_SUCCESS);

    /* Verify results */
    UtAssert_BOOL_FALSE(DoneWithEntry);

    UtAssert_UINT32_EQ(ResultsEntry.StartAddress, 1);

    UtAssert_UINT32_EQ(ResultsEntry.ByteOffset, 3);
    UtAssert_UINT32_EQ(ResultsEntry.TempChecksumValue, 2);
    UtAssert_UINT32_EQ(ComputedCSValue, 2);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
}

void CS_RecomputeEepromMemoryChildTask_Test_EEPROMTable(void)
{
    CS_Res_EepromMemory_Table_Entry_t RecomputeEepromMemoryEntry;
    CS_Def_EepromMemory_Table_Entry_t DefEepromTbl[10];
    char                              ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    memset(&RecomputeEepromMemoryEntry, 0, sizeof(RecomputeEepromMemoryEntry));
    memset(DefEepromTbl, 0, sizeof(DefEepromTbl));

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "%%s entry %%d recompute finished. New baseline is 0X%%08X");

    CS_AppData.RecomputeEepromMemoryEntryPtr = &RecomputeEepromMemoryEntry;
    CS_AppData.DefEepromTblPtr               = DefEepromTbl;

    CS_AppData.ChildTaskTable = CS_EEPROM_TABLE;

    CS_AppData.ChildTaskEntryID = 1;

    CS_AppData.DefEepromTblPtr[1].StartAddress = 1;

    RecomputeEepromMemoryEntry.StartAddress = CS_AppData.DefEepromTblPtr[1].StartAddress;

    DefEepromTbl[1].State = 1;

    CS_AppData.RecomputeEepromMemoryEntryPtr->NumBytesToChecksum = 1;
    CS_AppData.MaxBytesPerCycle                                  = 2;

    /* Set to a value, which will be printed in message CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_CalculateCRC), 1, 1);

    CS_AppData.RecomputeEepromMemoryEntryPtr->State = 99;

    /* Execute the function being tested */
    CS_RecomputeEepromMemoryChildTask();

    /* Verify results */
    UtAssert_BOOL_FALSE(CS_AppData.HkPacket.RecomputeInProgress);

    UtAssert_UINT32_EQ(CS_AppData.RecomputeEepromMemoryEntryPtr->TempChecksumValue, 0);
    UtAssert_UINT32_EQ(CS_AppData.RecomputeEepromMemoryEntryPtr->ByteOffset, 0);
    UtAssert_BOOL_TRUE(CS_AppData.RecomputeEepromMemoryEntryPtr->ComputedYet);
    UtAssert_UINT16_EQ(CS_AppData.RecomputeEepromMemoryEntryPtr->State, 99);
    UtAssert_UINT16_EQ(CS_AppData.DefEepromTblPtr[CS_AppData.ChildTaskEntryID].State, 1);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    UtAssert_STRINGBUF_EQ(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, context_CFE_EVS_SendEvent[0].Spec,
                          CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
}

void CS_RecomputeEepromMemoryChildTask_Test_MemoryTable(void)
{
    CS_Res_EepromMemory_Table_Entry_t RecomputeEepromMemoryEntry;
    CS_Def_EepromMemory_Table_Entry_t DefMemoryTbl[10];
    char                              ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    memset(&RecomputeEepromMemoryEntry, 0, sizeof(RecomputeEepromMemoryEntry));
    memset(DefMemoryTbl, 0, sizeof(DefMemoryTbl));

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "%%s entry %%d recompute finished. New baseline is 0X%%08X");

    CS_AppData.RecomputeEepromMemoryEntryPtr = &RecomputeEepromMemoryEntry;
    CS_AppData.DefMemoryTblPtr               = DefMemoryTbl;

    CS_AppData.ChildTaskTable = CS_MEMORY_TABLE;

    CS_AppData.ChildTaskEntryID = 1;

    CS_AppData.DefMemoryTblPtr[1].StartAddress = 1;

    RecomputeEepromMemoryEntry.StartAddress = CS_AppData.DefMemoryTblPtr[1].StartAddress;

    DefMemoryTbl[1].State = 1;

    CS_AppData.RecomputeEepromMemoryEntryPtr->NumBytesToChecksum = 1;
    CS_AppData.MaxBytesPerCycle                                  = 2;

    /* Set to a value, which will be printed in message CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_CalculateCRC), 1, 1);

    CS_AppData.RecomputeEepromMemoryEntryPtr->State = 99;

    /* Execute the function being tested */
    CS_RecomputeEepromMemoryChildTask();

    /* Verify results */
    UtAssert_BOOL_FALSE(CS_AppData.HkPacket.RecomputeInProgress);

    UtAssert_UINT32_EQ(CS_AppData.RecomputeEepromMemoryEntryPtr->TempChecksumValue, 0);
    UtAssert_UINT32_EQ(CS_AppData.RecomputeEepromMemoryEntryPtr->ByteOffset, 0);
    UtAssert_BOOL_TRUE(CS_AppData.RecomputeEepromMemoryEntryPtr->ComputedYet);
    UtAssert_UINT16_EQ(CS_AppData.RecomputeEepromMemoryEntryPtr->State, 99);
    UtAssert_UINT16_EQ(CS_AppData.DefMemoryTblPtr[CS_AppData.ChildTaskEntryID].State, 1);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    UtAssert_STRINGBUF_EQ(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, context_CFE_EVS_SendEvent[0].Spec,
                          CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
}

void CS_RecomputeEepromMemoryChildTask_Test_CFECore(void)
{
    CS_Res_EepromMemory_Table_Entry_t RecomputeEepromMemoryEntry;
    CS_Def_EepromMemory_Table_Entry_t DefMemoryTbl[10];
    char                              ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    memset(&RecomputeEepromMemoryEntry, 0, sizeof(RecomputeEepromMemoryEntry));
    memset(DefMemoryTbl, 0, sizeof(DefMemoryTbl));

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "%%s entry %%d recompute finished. New baseline is 0X%%08X");

    CS_AppData.RecomputeEepromMemoryEntryPtr = &RecomputeEepromMemoryEntry;
    CS_AppData.DefMemoryTblPtr               = DefMemoryTbl;

    CS_AppData.ChildTaskTable = CS_CFECORE;

    CS_AppData.ChildTaskEntryID = 1;

    CS_AppData.DefMemoryTblPtr[1].StartAddress = 1;

    RecomputeEepromMemoryEntry.StartAddress = CS_AppData.DefMemoryTblPtr[1].StartAddress;

    DefMemoryTbl[1].State = 1;

    CS_AppData.RecomputeEepromMemoryEntryPtr->NumBytesToChecksum = 1;
    CS_AppData.MaxBytesPerCycle                                  = 2;

    /* Set to a value, which will be printed in message CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_CalculateCRC), 1, 1);

    CS_AppData.RecomputeEepromMemoryEntryPtr->State = 99;

    /* Execute the function being tested */
    CS_RecomputeEepromMemoryChildTask();

    /* Verify results */
    UtAssert_BOOL_FALSE(CS_AppData.HkPacket.RecomputeInProgress);

    UtAssert_UINT32_EQ(CS_AppData.RecomputeEepromMemoryEntryPtr->TempChecksumValue, 0);
    UtAssert_UINT32_EQ(CS_AppData.RecomputeEepromMemoryEntryPtr->ByteOffset, 0);
    UtAssert_BOOL_TRUE(CS_AppData.RecomputeEepromMemoryEntryPtr->ComputedYet);
    UtAssert_UINT16_EQ(CS_AppData.RecomputeEepromMemoryEntryPtr->State, 99);
    UtAssert_UINT16_EQ(CS_AppData.DefMemoryTblPtr[CS_AppData.ChildTaskEntryID].State, 1);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    UtAssert_STRINGBUF_EQ(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, context_CFE_EVS_SendEvent[0].Spec,
                          CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_UINT32_EQ(CS_AppData.HkPacket.CfeCoreBaseline, 1);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
}

void CS_RecomputeEepromMemoryChildTask_Test_OSCore(void)
{
    CS_Res_EepromMemory_Table_Entry_t RecomputeEepromMemoryEntry;
    CS_Def_EepromMemory_Table_Entry_t DefMemoryTbl[10];
    char                              ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    memset(&RecomputeEepromMemoryEntry, 0, sizeof(RecomputeEepromMemoryEntry));
    memset(DefMemoryTbl, 0, sizeof(DefMemoryTbl));

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "%%s entry %%d recompute finished. New baseline is 0X%%08X");

    CS_AppData.RecomputeEepromMemoryEntryPtr = &RecomputeEepromMemoryEntry;
    CS_AppData.DefMemoryTblPtr               = DefMemoryTbl;

    CS_AppData.ChildTaskTable = CS_OSCORE;

    CS_AppData.ChildTaskEntryID = 1;

    CS_AppData.DefMemoryTblPtr[1].StartAddress = 1;

    RecomputeEepromMemoryEntry.StartAddress = CS_AppData.DefMemoryTblPtr[1].StartAddress;

    DefMemoryTbl[1].State = 1;

    CS_AppData.RecomputeEepromMemoryEntryPtr->NumBytesToChecksum = 1;
    CS_AppData.MaxBytesPerCycle                                  = 2;

    /* Set to a value, which will be printed in message CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_CalculateCRC), 1, 1);

    CS_AppData.RecomputeEepromMemoryEntryPtr->State = 99;

    /* Execute the function being tested */
    CS_RecomputeEepromMemoryChildTask();

    /* Verify results */
    UtAssert_BOOL_FALSE(CS_AppData.HkPacket.RecomputeInProgress);

    UtAssert_UINT32_EQ(CS_AppData.RecomputeEepromMemoryEntryPtr->TempChecksumValue, 0);
    UtAssert_UINT32_EQ(CS_AppData.RecomputeEepromMemoryEntryPtr->ByteOffset, 0);
    UtAssert_BOOL_TRUE(CS_AppData.RecomputeEepromMemoryEntryPtr->ComputedYet);
    UtAssert_UINT16_EQ(CS_AppData.RecomputeEepromMemoryEntryPtr->State, 99);
    UtAssert_UINT16_EQ(CS_AppData.DefMemoryTblPtr[CS_AppData.ChildTaskEntryID].State, 1);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    UtAssert_STRINGBUF_EQ(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, context_CFE_EVS_SendEvent[0].Spec,
                          CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_UINT32_EQ(CS_AppData.HkPacket.OSBaseline, 1);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
}

void CS_RecomputeEepromMemoryChildTask_Test_EEPROMTableEntryId(void)
{
    CS_Res_EepromMemory_Table_Entry_t RecomputeEepromMemoryEntry;
    CS_Def_EepromMemory_Table_Entry_t DefEepromTbl[10];
    char                              ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    memset(&RecomputeEepromMemoryEntry, 0, sizeof(RecomputeEepromMemoryEntry));
    memset(DefEepromTbl, 0, sizeof(DefEepromTbl));

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "%%s entry %%d recompute finished. New baseline is 0X%%08X");

    CS_AppData.RecomputeEepromMemoryEntryPtr = &RecomputeEepromMemoryEntry;
    CS_AppData.DefEepromTblPtr               = DefEepromTbl;

    CS_AppData.ChildTaskTable = CS_EEPROM_TABLE;

    CS_AppData.ChildTaskEntryID = CS_MAX_NUM_EEPROM_TABLE_ENTRIES;

    CS_AppData.DefEepromTblPtr[1].StartAddress = 1;

    RecomputeEepromMemoryEntry.StartAddress = CS_AppData.DefEepromTblPtr[1].StartAddress;

    DefEepromTbl[1].State = 1;

    CS_AppData.RecomputeEepromMemoryEntryPtr->NumBytesToChecksum = 1;
    CS_AppData.MaxBytesPerCycle                                  = 2;

    /* Set to a value, which will be printed in message CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_CalculateCRC), 1, 1);

    CS_AppData.RecomputeEepromMemoryEntryPtr->State = 99;

    /* Execute the function being tested */
    CS_RecomputeEepromMemoryChildTask();

    /* Verify results */
    UtAssert_BOOL_FALSE(CS_AppData.HkPacket.RecomputeInProgress);

    UtAssert_UINT32_EQ(CS_AppData.RecomputeEepromMemoryEntryPtr->TempChecksumValue, 0);
    UtAssert_UINT32_EQ(CS_AppData.RecomputeEepromMemoryEntryPtr->ByteOffset, 0);
    UtAssert_BOOL_TRUE(CS_AppData.RecomputeEepromMemoryEntryPtr->ComputedYet);
    UtAssert_UINT16_EQ(CS_AppData.RecomputeEepromMemoryEntryPtr->State, 99);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    UtAssert_STRINGBUF_EQ(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, context_CFE_EVS_SendEvent[0].Spec,
                          CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
}

void CS_RecomputeEepromMemoryChildTask_Test_EEPROMTableStartAddress(void)
{
    CS_Res_EepromMemory_Table_Entry_t RecomputeEepromMemoryEntry;
    CS_Def_EepromMemory_Table_Entry_t DefEepromTbl[10];
    char                              ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    memset(&RecomputeEepromMemoryEntry, 0, sizeof(RecomputeEepromMemoryEntry));
    memset(DefEepromTbl, 0, sizeof(DefEepromTbl));

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "%%s entry %%d recompute finished. New baseline is 0X%%08X");

    CS_AppData.RecomputeEepromMemoryEntryPtr = &RecomputeEepromMemoryEntry;
    CS_AppData.DefEepromTblPtr               = DefEepromTbl;

    CS_AppData.ChildTaskTable = CS_EEPROM_TABLE;

    CS_AppData.ChildTaskEntryID = 1;

    CS_AppData.DefEepromTblPtr[1].StartAddress = 1;

    RecomputeEepromMemoryEntry.StartAddress = 0;

    DefEepromTbl[1].State = 1;

    CS_AppData.RecomputeEepromMemoryEntryPtr->NumBytesToChecksum = 1;
    CS_AppData.MaxBytesPerCycle                                  = 2;

    /* Set to a value, which will be printed in message CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_CalculateCRC), 1, 1);

    CS_AppData.RecomputeEepromMemoryEntryPtr->State = 99;

    /* Execute the function being tested */
    CS_RecomputeEepromMemoryChildTask();

    /* Verify results */
    UtAssert_BOOL_FALSE(CS_AppData.HkPacket.RecomputeInProgress);

    UtAssert_UINT32_EQ(CS_AppData.RecomputeEepromMemoryEntryPtr->TempChecksumValue, 0);
    UtAssert_UINT32_EQ(CS_AppData.RecomputeEepromMemoryEntryPtr->ByteOffset, 0);
    UtAssert_BOOL_TRUE(CS_AppData.RecomputeEepromMemoryEntryPtr->ComputedYet);
    UtAssert_UINT16_EQ(CS_AppData.RecomputeEepromMemoryEntryPtr->State, 99);
    UtAssert_UINT16_EQ(CS_AppData.DefEepromTblPtr[CS_AppData.ChildTaskEntryID].State, 1);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    UtAssert_STRINGBUF_EQ(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, context_CFE_EVS_SendEvent[0].Spec,
                          CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
}

void CS_RecomputeEepromMemoryChildTask_Test_EEPROMTableState(void)
{
    CS_Res_EepromMemory_Table_Entry_t RecomputeEepromMemoryEntry;
    CS_Def_EepromMemory_Table_Entry_t DefEepromTbl[10];
    char                              ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    memset(&RecomputeEepromMemoryEntry, 0, sizeof(RecomputeEepromMemoryEntry));
    memset(DefEepromTbl, 0, sizeof(DefEepromTbl));

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "%%s entry %%d recompute finished. New baseline is 0X%%08X");

    CS_AppData.RecomputeEepromMemoryEntryPtr = &RecomputeEepromMemoryEntry;
    CS_AppData.DefEepromTblPtr               = DefEepromTbl;

    CS_AppData.ChildTaskTable = CS_EEPROM_TABLE;

    CS_AppData.ChildTaskEntryID = 1;

    CS_AppData.DefEepromTblPtr[1].StartAddress = 1;

    RecomputeEepromMemoryEntry.StartAddress = 1;

    DefEepromTbl[1].State = CS_STATE_EMPTY;

    CS_AppData.RecomputeEepromMemoryEntryPtr->NumBytesToChecksum = 1;
    CS_AppData.MaxBytesPerCycle                                  = 2;

    /* Set to a value, which will be printed in message CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_CalculateCRC), 1, 1);

    CS_AppData.RecomputeEepromMemoryEntryPtr->State = 99;

    /* Execute the function being tested */
    CS_RecomputeEepromMemoryChildTask();

    /* Verify results */
    UtAssert_BOOL_FALSE(CS_AppData.HkPacket.RecomputeInProgress);

    UtAssert_UINT32_EQ(CS_AppData.RecomputeEepromMemoryEntryPtr->TempChecksumValue, 0);
    UtAssert_UINT32_EQ(CS_AppData.RecomputeEepromMemoryEntryPtr->ByteOffset, 0);
    UtAssert_BOOL_TRUE(CS_AppData.RecomputeEepromMemoryEntryPtr->ComputedYet);
    UtAssert_UINT16_EQ(CS_AppData.RecomputeEepromMemoryEntryPtr->State, 99);
    UtAssert_UINT16_EQ(CS_AppData.DefEepromTblPtr[CS_AppData.ChildTaskEntryID].State, CS_STATE_EMPTY);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    UtAssert_STRINGBUF_EQ(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, context_CFE_EVS_SendEvent[0].Spec,
                          CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
}

void CS_RecomputeAppChildTask_Test_Nominal(void)
{
    CS_Res_App_Table_Entry_t RecomputeAppEntry;
    CS_Def_App_Table_Entry_t DefAppTbl[10];
    char                     ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    memset(&RecomputeAppEntry, 0, sizeof(RecomputeAppEntry));
    memset(&DefAppTbl, 0, sizeof(DefAppTbl));

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "App %%s recompute finished. New baseline is 0x%%08X");

    CS_AppData.RecomputeAppEntryPtr = &RecomputeAppEntry;
    CS_AppData.DefAppTblPtr         = DefAppTbl;

    CS_AppData.ChildTaskTable = CS_OSCORE;

    CS_AppData.ChildTaskEntryID = 1;

    DefAppTbl[1].State = 1;

    CS_AppData.RecomputeAppEntryPtr->NumBytesToChecksum = 1;
    CS_AppData.MaxBytesPerCycle                         = 2;

    /* Set to a value, which will be printed in message CS_RECOMPUTE_FINISH_APP_INF_EID */
    UT_SetDefaultReturnValue(UT_KEY(CFE_ES_CalculateCRC), 1);

    CS_AppData.RecomputeAppEntryPtr->State = 99;

    strncpy(CS_AppData.RecomputeAppEntryPtr->Name, "name", 10);
    strncpy(DefAppTbl[1].Name, "name", 10);

    /* Sets AppInfo.CodeSize = 5, sets AppInfo.CodeAddress = 1, AppInfo.AddressesAreValid = true, and returns
     * CFE_SUCCESS */
    UT_SetHandlerFunction(UT_KEY(CFE_ES_GetModuleInfo), CS_COMPUTE_TEST_CFE_ES_GetModuleInfoHandler1, NULL);

    /* Execute the function being tested */
    CS_RecomputeAppChildTask();

    /* Verify results */
    UtAssert_UINT16_EQ(CS_AppData.RecomputeAppEntryPtr->State, 99);
    UtAssert_UINT16_EQ(CS_AppData.DefAppTblPtr[CS_AppData.ChildTaskEntryID].State, 1);
    UtAssert_UINT32_EQ(CS_AppData.RecomputeAppEntryPtr->TempChecksumValue, 0);
    UtAssert_UINT32_EQ(CS_AppData.RecomputeAppEntryPtr->ByteOffset, 0);
    UtAssert_BOOL_TRUE(CS_AppData.RecomputeAppEntryPtr->ComputedYet);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_RECOMPUTE_FINISH_APP_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    UtAssert_STRINGBUF_EQ(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, context_CFE_EVS_SendEvent[0].Spec,
                          CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_BOOL_FALSE(CS_AppData.HkPacket.RecomputeInProgress);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
}

void CS_RecomputeAppChildTask_Test_CouldNotGetAddress(void)
{
    CS_Res_App_Table_Entry_t RecomputeAppEntry;
    CS_Def_App_Table_Entry_t DefAppTbl[10];

    memset(&RecomputeAppEntry, 0, sizeof(RecomputeAppEntry));
    memset(&DefAppTbl, 0, sizeof(DefAppTbl));

    CS_AppData.RecomputeAppEntryPtr = &RecomputeAppEntry;
    CS_AppData.DefAppTblPtr         = DefAppTbl;

    CS_AppData.ChildTaskTable = CS_OSCORE;

    CS_AppData.ChildTaskEntryID = 1;

    /* No matching non-empty state entry */
    DefAppTbl[1].State = CS_STATE_EMPTY;

    CS_AppData.RecomputeAppEntryPtr->State = 99;

    strncpy(CS_AppData.RecomputeAppEntryPtr->Name, "name", 10);
    strncpy(DefAppTbl[1].Name, "name", 10);

    /* Set to cause CS_ComputeApp to return CS_ERR_NOT_FOUND */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetAppIDByName), 1, -1);
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetLibIDByName), 1, -1);

    /* Sets AppInfo.CodeSize = 5, sets AppInfo.CodeAddress = 1, AppInfo.AddressesAreValid = true, and returns
     * CFE_SUCCESS */
    UT_SetHandlerFunction(UT_KEY(CFE_ES_GetModuleInfo), CS_COMPUTE_TEST_CFE_ES_GetModuleInfoHandler1, NULL);

    /* Execute the function being tested */
    CS_RecomputeAppChildTask();

    /* Verify results */
    UtAssert_UINT32_EQ(CS_AppData.RecomputeAppEntryPtr->State, 99);
    UtAssert_UINT32_EQ(CS_AppData.DefAppTblPtr[CS_AppData.ChildTaskEntryID].State, CS_STATE_EMPTY);
    UtAssert_UINT32_EQ(CS_AppData.RecomputeAppEntryPtr->TempChecksumValue, 0);
    UtAssert_UINT32_EQ(CS_AppData.RecomputeAppEntryPtr->ByteOffset, 0);

    UtAssert_BOOL_FALSE(CS_AppData.RecomputeAppEntryPtr->ComputedYet);
    UtAssert_BOOL_FALSE(CS_AppData.HkPacket.RecomputeInProgress);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 2);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_COMPUTE_APP_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, CS_RECOMPUTE_ERROR_APP_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_ERROR);
}

void CS_RecomputeAppChildTask_Test_DefEntryId(void)
{
    CS_Res_App_Table_Entry_t RecomputeAppEntry;
    CS_Def_App_Table_Entry_t DefAppTbl[10];
    char                     ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    memset(&RecomputeAppEntry, 0, sizeof(RecomputeAppEntry));
    memset(&DefAppTbl, 0, sizeof(DefAppTbl));

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "App %%s recompute finished. New baseline is 0x%%08X");

    CS_AppData.RecomputeAppEntryPtr = &RecomputeAppEntry;
    CS_AppData.DefAppTblPtr         = DefAppTbl;

    CS_AppData.ChildTaskTable = CS_OSCORE;

    CS_AppData.ChildTaskEntryID = 1;

    CS_AppData.RecomputeAppEntryPtr->NumBytesToChecksum = 1;
    CS_AppData.MaxBytesPerCycle                         = 2;

    /* Set to a value, which will be printed in message CS_RECOMPUTE_FINISH_APP_INF_EID */
    UT_SetDefaultReturnValue(UT_KEY(CFE_ES_CalculateCRC), 1);

    CS_AppData.RecomputeAppEntryPtr->State = 99;

    strncpy(CS_AppData.RecomputeAppEntryPtr->Name, "name", 10);
    strncpy(DefAppTbl[1].Name, "name", 10);
    DefAppTbl[1].State = CS_STATE_ENABLED;

    /* Sets AppInfo.CodeSize = 5, sets AppInfo.CodeAddress = 1, AppInfo.AddressesAreValid = true, and returns
     * CFE_SUCCESS */
    UT_SetHandlerFunction(UT_KEY(CFE_ES_GetModuleInfo), CS_COMPUTE_TEST_CFE_ES_GetModuleInfoHandler1, NULL);

    /* Execute the function being tested */
    CS_RecomputeAppChildTask();

    /* Verify results */
    UtAssert_UINT32_EQ(CS_AppData.RecomputeAppEntryPtr->State, 99);
    UtAssert_UINT32_EQ(CS_AppData.DefAppTblPtr[CS_AppData.ChildTaskEntryID].State, CS_STATE_ENABLED);
    UtAssert_UINT32_EQ(CS_AppData.RecomputeAppEntryPtr->TempChecksumValue, 0);
    UtAssert_UINT32_EQ(CS_AppData.RecomputeAppEntryPtr->ByteOffset, 0);
    UtAssert_BOOL_TRUE(CS_AppData.RecomputeAppEntryPtr->ComputedYet);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_RECOMPUTE_FINISH_APP_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    UtAssert_STRINGBUF_EQ(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, context_CFE_EVS_SendEvent[0].Spec,
                          CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_BOOL_FALSE(CS_AppData.HkPacket.RecomputeInProgress);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
}

void CS_RecomputeTablesChildTask_Test_Nominal(void)
{
    CS_Res_Tables_Table_Entry_t RecomputeTablesEntry;
    CS_Def_Tables_Table_Entry_t DefTablesTbl[10];
    CFE_TBL_Info_t              TblInfo;
    char                        ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    memset(&RecomputeTablesEntry, 0, sizeof(RecomputeTablesEntry));
    memset(&DefTablesTbl, 0, sizeof(DefTablesTbl));

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Table %%s recompute finished. New baseline is 0x%%08X");

    CS_AppData.RecomputeTablesEntryPtr = &RecomputeTablesEntry;
    CS_AppData.DefTablesTblPtr         = DefTablesTbl;

    CS_AppData.ChildTaskTable = CS_OSCORE;

    CS_AppData.ChildTaskEntryID = 1;

    DefTablesTbl[1].State = 1;

    CS_AppData.RecomputeTablesEntryPtr->NumBytesToChecksum = 1;
    CS_AppData.MaxBytesPerCycle                            = 2;

    /* Set to a value, which will be printed in message CS_RECOMPUTE_FINISH_TABLES_INF_EID */
    UT_SetDefaultReturnValue(UT_KEY(CFE_ES_CalculateCRC), 1);

    CS_AppData.RecomputeTablesEntryPtr->State = 99;

    strncpy(CS_AppData.RecomputeTablesEntryPtr->Name, "name", 10);
    strncpy(DefTablesTbl[1].Name, "name", 10);

    RecomputeTablesEntry.TblHandle = CFE_TBL_BAD_TABLE_HANDLE;

    RecomputeTablesEntry.ByteOffset = 0;
    CS_AppData.MaxBytesPerCycle     = 5;

    RecomputeTablesEntry.ComputedYet = true;

    RecomputeTablesEntry.ComparisonValue = 2;

    /* Sets ResultsEntry->TblHandle to 99 and returns CFE_SUCCESS */
    UT_SetHandlerFunction(UT_KEY(CFE_TBL_Share), CS_COMPUTE_TEST_CFE_TBL_ShareHandler, NULL);

    /* Sets TblInfo.Size = 5 and returns CFE_SUCCESS */
    TblInfo.Size = 5;

    UT_SetDataBuffer(UT_KEY(CFE_TBL_GetInfo), &TblInfo, sizeof(TblInfo), false);

    /* Set to satisfy condition "Result == CFE_SUCCESS" and to fail other conditions that check for other values of
     * Result */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_SUCCESS);

    /* Execute the function being tested */
    CS_RecomputeTablesChildTask();

    /* Verify results */
    UtAssert_UINT16_EQ(CS_AppData.RecomputeTablesEntryPtr->State, 99);
    UtAssert_UINT16_EQ(CS_AppData.DefTablesTblPtr[CS_AppData.ChildTaskEntryID].State, 1);
    UtAssert_UINT32_EQ(CS_AppData.RecomputeTablesEntryPtr->TempChecksumValue, 0);
    UtAssert_UINT32_EQ(CS_AppData.RecomputeTablesEntryPtr->ByteOffset, 0);
    UtAssert_BOOL_TRUE(CS_AppData.RecomputeTablesEntryPtr->ComputedYet);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_RECOMPUTE_FINISH_TABLES_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    UtAssert_STRINGBUF_EQ(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, context_CFE_EVS_SendEvent[0].Spec,
                          CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_BOOL_FALSE(CS_AppData.HkPacket.RecomputeInProgress);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
}

void CS_RecomputeTablesChildTask_Test_CouldNotGetAddress(void)
{
    CS_Res_Tables_Table_Entry_t RecomputeTablesEntry;
    CS_Def_Tables_Table_Entry_t DefTablesTbl[10];

    memset(&RecomputeTablesEntry, 0, sizeof(RecomputeTablesEntry));
    memset(&DefTablesTbl, 0, sizeof(DefTablesTbl));

    CS_AppData.RecomputeTablesEntryPtr = &RecomputeTablesEntry;
    CS_AppData.DefTablesTblPtr         = DefTablesTbl;

    CS_AppData.ChildTaskTable = CS_OSCORE;

    CS_AppData.ChildTaskEntryID = 1;

    /* No matching non-empty state entry */
    DefTablesTbl[1].State = CS_STATE_EMPTY;

    CS_AppData.RecomputeTablesEntryPtr->State = 99;

    strncpy(CS_AppData.RecomputeTablesEntryPtr->Name, "name", 10);
    strncpy(DefTablesTbl[1].Name, "name", 10);

    /* Set to make CS_ComputeTables return CS_ERR_NOT_FOUND */
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_Share), 1, -1);
    UT_SetDeferredRetcode(UT_KEY(CS_AttemptTableReshare), 1, -1);
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_GetAddress), 1, -1);

    /* Execute the function being tested */
    CS_RecomputeTablesChildTask();

    /* Verify results */
    UtAssert_UINT32_EQ(CS_AppData.RecomputeTablesEntryPtr->State, 99);
    UtAssert_UINT32_EQ(CS_AppData.DefTablesTblPtr[CS_AppData.ChildTaskEntryID].State, CS_STATE_EMPTY);
    UtAssert_UINT32_EQ(CS_AppData.RecomputeTablesEntryPtr->TempChecksumValue, 0);
    UtAssert_UINT32_EQ(CS_AppData.RecomputeTablesEntryPtr->ByteOffset, 0);

    UtAssert_BOOL_FALSE(CS_AppData.RecomputeTablesEntryPtr->ComputedYet);
    UtAssert_BOOL_FALSE(CS_AppData.HkPacket.RecomputeInProgress);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 2);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, CS_RECOMPUTE_ERROR_TABLES_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_ERROR);

    /* Note: generates 1 event message we don't care about in this test */
}

void CS_RecomputeTablesChildTask_Test_DefEntryId(void)
{
    CS_Res_Tables_Table_Entry_t RecomputeTablesEntry;
    CS_Def_Tables_Table_Entry_t DefTablesTbl[10];
    CFE_TBL_Info_t              TblInfo;
    char                        ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    memset(&RecomputeTablesEntry, 0, sizeof(RecomputeTablesEntry));
    memset(&DefTablesTbl, 0, sizeof(DefTablesTbl));

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Table %%s recompute finished. New baseline is 0x%%08X");

    CS_AppData.RecomputeTablesEntryPtr = &RecomputeTablesEntry;
    CS_AppData.DefTablesTblPtr         = DefTablesTbl;

    CS_AppData.ChildTaskTable = CS_OSCORE;

    CS_AppData.ChildTaskEntryID = 1;

    CS_AppData.RecomputeTablesEntryPtr->NumBytesToChecksum = 1;
    CS_AppData.MaxBytesPerCycle                            = 2;

    /* Set to a value, which will be printed in message CS_RECOMPUTE_FINISH_TABLES_INF_EID */
    UT_SetDefaultReturnValue(UT_KEY(CFE_ES_CalculateCRC), 1);

    CS_AppData.RecomputeTablesEntryPtr->State = 99;

    strncpy(CS_AppData.RecomputeTablesEntryPtr->Name, "name", 10);
    strncpy(DefTablesTbl[1].Name, "name", 10);
    DefTablesTbl[1].State = CS_STATE_ENABLED;

    RecomputeTablesEntry.TblHandle = CFE_TBL_BAD_TABLE_HANDLE;

    RecomputeTablesEntry.ByteOffset = 0;
    CS_AppData.MaxBytesPerCycle     = 5;

    RecomputeTablesEntry.ComputedYet = true;

    RecomputeTablesEntry.ComparisonValue = 2;

    /* Sets ResultsEntry->TblHandle to 99 and returns CFE_SUCCESS */
    UT_SetHandlerFunction(UT_KEY(CFE_TBL_Share), CS_COMPUTE_TEST_CFE_TBL_ShareHandler, NULL);

    /* Sets TblInfo.Size = 5 and returns CFE_SUCCESS */
    TblInfo.Size = 5;

    UT_SetDataBuffer(UT_KEY(CFE_TBL_GetInfo), &TblInfo, sizeof(TblInfo), false);

    /* Set to satisfy condition "Result == CFE_SUCCESS" and to fail other conditions that check for other values of
     * Result */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_SUCCESS);

    /* Execute the function being tested */
    CS_RecomputeTablesChildTask();

    /* Verify results */
    UtAssert_UINT32_EQ(CS_AppData.RecomputeTablesEntryPtr->State, 99);
    UtAssert_UINT32_EQ(CS_AppData.DefTablesTblPtr[CS_AppData.ChildTaskEntryID].State, CS_STATE_ENABLED);
    UtAssert_UINT32_EQ(CS_AppData.RecomputeTablesEntryPtr->TempChecksumValue, 0);
    UtAssert_UINT32_EQ(CS_AppData.RecomputeTablesEntryPtr->ByteOffset, 0);
    UtAssert_BOOL_TRUE(CS_AppData.RecomputeTablesEntryPtr->ComputedYet);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_RECOMPUTE_FINISH_TABLES_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    UtAssert_STRINGBUF_EQ(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, context_CFE_EVS_SendEvent[0].Spec,
                          CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_BOOL_FALSE(CS_AppData.HkPacket.RecomputeInProgress);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
}

void CS_OneShotChildTask_Test_Nominal(void)
{
    char ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "OneShot checksum on Address: 0x%%08X, size %%d completed. Checksum =  0x%%08X");

    /* NewChecksumValue will be set to value returned by this function */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_CalculateCRC), 1, 1);

    CS_AppData.HkPacket.LastOneShotAddress          = 0;
    CS_AppData.HkPacket.LastOneShotSize             = 1;
    CS_AppData.HkPacket.LastOneShotChecksum         = 1;
    CS_AppData.HkPacket.LastOneShotMaxBytesPerCycle = 1;

    /* Execute the function being tested */
    CS_OneShotChildTask();

    /* Verify results */
    UtAssert_UINT32_EQ(CS_AppData.HkPacket.LastOneShotChecksum, 1);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, CS_ONESHOT_FINISHED_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    UtAssert_STRINGBUF_EQ(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, context_CFE_EVS_SendEvent[0].Spec,
                          CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_BOOL_FALSE(CS_AppData.HkPacket.RecomputeInProgress);
    UtAssert_BOOL_FALSE(CS_AppData.HkPacket.OneShotInProgress);
    UtAssert_BOOL_FALSE(CFE_RESOURCEID_TEST_DEFINED(CS_AppData.ChildTaskID));

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
}

void UtTest_Setup(void)
{
    UtTest_Add(CS_ComputeEepromMemory_Test_Nominal, CS_Test_Setup, CS_Test_TearDown,
               "CS_ComputeEepromMemory_Test_Nominal");
    UtTest_Add(CS_ComputeEepromMemory_Test_Error, CS_Test_Setup, CS_Test_TearDown, "CS_ComputeEepromMemory_Test_Error");
    UtTest_Add(CS_ComputeEepromMemory_Test_FirstTimeThrough, CS_Test_Setup, CS_Test_TearDown,
               "CS_ComputeEepromMemory_Test_FirstTimeThrough");
    UtTest_Add(CS_ComputeEepromMemory_Test_NotFinished, CS_Test_Setup, CS_Test_TearDown,
               "CS_ComputeEepromMemory_Test_NotFinished");

    UtTest_Add(CS_ComputeTables_Test_TableNeverLoaded, CS_Test_Setup, CS_Test_TearDown,
               "CS_ComputeTables_Test_TableNeverLoaded");
    UtTest_Add(CS_ComputeTables_Test_TableUnregisteredAndNeverLoaded, CS_Test_Setup, CS_Test_TearDown,
               "CS_ComputeTables_Test_TableUnregisteredAndNeverLoaded");
    UtTest_Add(CS_ComputeTables_Test_ResultShareNotSuccess, CS_Test_Setup, CS_Test_TearDown,
               "CS_ComputeTables_Test_ResultShareNotSuccess");
    UtTest_Add(CS_ComputeTables_Test_TblInfoUpdated, CS_Test_Setup, CS_Test_TearDown,
               "CS_ComputeTables_Test_TblInfoUpdated");
    UtTest_Add(CS_ComputeTables_Test_GetInfoResult, CS_Test_Setup, CS_Test_TearDown,
               "CS_ComputeTables_Test_GetInfoResult");
    UtTest_Add(CS_ComputeTables_Test_CSError, CS_Test_Setup, CS_Test_TearDown, "CS_ComputeTables_Test_CSError");
    UtTest_Add(CS_ComputeTables_Test_NominalBadTableHandle, CS_Test_Setup, CS_Test_TearDown,
               "CS_ComputeTables_Test_NominalBadTableHandle");
    UtTest_Add(CS_ComputeTables_Test_FirstTimeThrough, CS_Test_Setup, CS_Test_TearDown,
               "CS_ComputeTables_Test_FirstTimeThrough");
    UtTest_Add(CS_ComputeTables_Test_EntryNotFinished, CS_Test_Setup, CS_Test_TearDown,
               "CS_ComputeTables_Test_EntryNotFinished");
    UtTest_Add(CS_ComputeTables_Test_ComputeTablesReleaseError, CS_Test_Setup, CS_Test_TearDown,
               "CS_ComputeTables_Test_ComputeTablesReleaseError");
    UtTest_Add(CS_ComputeTables_Test_ComputeTablesError, CS_Test_Setup, CS_Test_TearDown,
               "CS_ComputeTables_Test_ComputeTablesError");

    UtTest_Add(CS_ComputeApp_Test_NominalApp, CS_Test_Setup, CS_Test_TearDown, "CS_ComputeApp_Test_NominalApp");
    UtTest_Add(CS_ComputeApp_Test_NominalLib, CS_Test_Setup, CS_Test_TearDown, "CS_ComputeApp_Test_NominalLib");
    UtTest_Add(CS_ComputeApp_Test_GetAppAndLibIDByNameError, CS_Test_Setup, CS_Test_TearDown,
               "CS_ComputeApp_Test_GetAppAndLibIDByNameError");
    UtTest_Add(CS_ComputeApp_Test_GetModuleInfoError, CS_Test_Setup, CS_Test_TearDown,
               "CS_ComputeApp_Test_GetModuleInfoError");
    UtTest_Add(CS_ComputeApp_Test_ComputeAppPlatformError, CS_Test_Setup, CS_Test_TearDown,
               "CS_ComputeApp_Test_ComputeAppPlatformError");
    UtTest_Add(CS_ComputeApp_Test_DifferFromSavedValue, CS_Test_Setup, CS_Test_TearDown,
               "CS_ComputeApp_Test_DifferFromSavedValue");
    UtTest_Add(CS_ComputeApp_Test_FirstTimeThrough, CS_Test_Setup, CS_Test_TearDown,
               "CS_ComputeApp_Test_FirstTimeThrough");
    UtTest_Add(CS_ComputeApp_Test_EntryNotFinished, CS_Test_Setup, CS_Test_TearDown,
               "CS_ComputeApp_Test_EntryNotFinished");

    UtTest_Add(CS_RecomputeEepromMemoryChildTask_Test_EEPROMTable, CS_Test_Setup, CS_Test_TearDown,
               "CS_RecomputeEepromMemoryChildTask_Test_EEPROMTable");
    UtTest_Add(CS_RecomputeEepromMemoryChildTask_Test_MemoryTable, CS_Test_Setup, CS_Test_TearDown,
               "CS_RecomputeEepromMemoryChildTask_Test_MemoryTable");
    UtTest_Add(CS_RecomputeEepromMemoryChildTask_Test_CFECore, CS_Test_Setup, CS_Test_TearDown,
               "CS_RecomputeEepromMemoryChildTask_Test_CFECore");
    UtTest_Add(CS_RecomputeEepromMemoryChildTask_Test_OSCore, CS_Test_Setup, CS_Test_TearDown,
               "CS_RecomputeEepromMemoryChildTask_Test_OSCore");
    UtTest_Add(CS_RecomputeEepromMemoryChildTask_Test_EEPROMTableEntryId, CS_Test_Setup, CS_Test_TearDown,
               "CS_RecomputeEepromMemoryChildTask_Test_EEPROMTableEntryId");
    UtTest_Add(CS_RecomputeEepromMemoryChildTask_Test_EEPROMTableStartAddress, CS_Test_Setup, CS_Test_TearDown,
               "CS_RecomputeEepromMemoryChildTask_Test_EEPROMTableStartAddress");
    UtTest_Add(CS_RecomputeEepromMemoryChildTask_Test_EEPROMTableState, CS_Test_Setup, CS_Test_TearDown,
               "CS_RecomputeEepromMemoryChildTask_Test_EEPROMTableState");

    UtTest_Add(CS_RecomputeAppChildTask_Test_Nominal, CS_Test_Setup, CS_Test_TearDown,
               "CS_RecomputeAppChildTask_Test_Nominal");
    UtTest_Add(CS_RecomputeAppChildTask_Test_CouldNotGetAddress, CS_Test_Setup, CS_Test_TearDown,
               "CS_RecomputeAppChildTask_Test_CouldNotGetAddress");
    UtTest_Add(CS_RecomputeAppChildTask_Test_DefEntryId, CS_Test_Setup, CS_Test_TearDown,
               "CS_RecomputeAppChildTask_Test_DefEntryId");

    UtTest_Add(CS_RecomputeTablesChildTask_Test_Nominal, CS_Test_Setup, CS_Test_TearDown,
               "CS_RecomputeTablesChildTask_Test_Nominal");
    UtTest_Add(CS_RecomputeTablesChildTask_Test_CouldNotGetAddress, CS_Test_Setup, CS_Test_TearDown,
               "CS_RecomputeTablesChildTask_Test_CouldNotGetAddress");
    UtTest_Add(CS_RecomputeTablesChildTask_Test_DefEntryId, CS_Test_Setup, CS_Test_TearDown,
               "CS_RecomputeTablesChildTask_Test_DefEntryId");

    UtTest_Add(CS_OneShotChildTask_Test_Nominal, CS_Test_Setup, CS_Test_TearDown, "CS_OneShotChildTask_Test_Nominal");
}
