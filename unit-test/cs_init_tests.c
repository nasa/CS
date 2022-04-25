
/*
 * Includes
 */

#include "cs_init.h"
#include "cs_msg.h"
#include "cs_msgdefs.h"
#include "cs_events.h"
#include "cs_version.h"
#include "cs_utils.h"
#include "cs_test_utils.h"
#include <sys/fcntl.h>
#include <unistd.h>
#include <stdlib.h>

/* UT includes */
#include "uttest.h"
#include "utassert.h"
#include "utstubs.h"

/* cs_init_tests globals */
uint8 call_count_CFE_EVS_SendEvent;

void CS_Init_Test_SBCreatePipeError(void)
{
    int32 Result;
    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Software Bus Create Pipe for command returned: 0x%%08X");

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), UT_Utils_stub_reporter_hook, &context_CFE_EVS_SendEvent);

    /* Set to generate error message CS_INIT_SB_CREATE_ERR_EID */
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_CreatePipe), 1, -1);

    /* Execute the function being tested */
    Result = CS_SbInit();

    /* Verify results */

    UtAssert_True(Result == -1, "Result == -1");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventID, CS_INIT_SB_CREATE_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent.Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent.Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_Init_Test_SBCreatePipeError */

void CS_Init_Test_SBSubscribeHKNominal(void)
{
    int32 Result;

    /* Execute the function being tested */
    Result = CS_SbInit();

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_Init_Test_SBSubscribeHKNominal */

void CS_Init_Test_SBSubscribeHKError(void)
{
    int32 Result;
    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Software Bus subscribe to housekeeping returned: 0x%%08X");

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), UT_Utils_stub_reporter_hook, &context_CFE_EVS_SendEvent);

    /* Set to generate error message CS_INIT_SB_SUBSCRIBE_HK_ERR_EID */
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_Subscribe), 1, -1);

    /* Execute the function being tested */
    Result = CS_SbInit();

    /* Verify results */

    UtAssert_True(Result == -1, "Result == -1");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventID, CS_INIT_SB_SUBSCRIBE_HK_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent.Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent.Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_Init_Test_SBSubscribeHKError */

void CS_Init_Test_SBSubscribeBackgroundCycleError(void)
{
    int32 Result;
    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Software Bus subscribe to background cycle returned: 0x%%08X");

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), UT_Utils_stub_reporter_hook, &context_CFE_EVS_SendEvent);

    /* Set to generate error message CS_INIT_SB_SUBSCRIBE_BACK_ERR_EID */
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_Subscribe), 2, -1);

    /* Execute the function being tested */
    Result = CS_SbInit();

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventID, CS_INIT_SB_SUBSCRIBE_BACK_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent.Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent.Spec);

    UtAssert_True(Result == -1, "Result == -1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_Init_Test_SBSubscribeBackgroundCycleError */

void CS_Init_Test_SBSubscribeCmdError(void)
{
    int32 Result;
    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Software Bus subscribe to command returned: 0x%%08X");

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), UT_Utils_stub_reporter_hook, &context_CFE_EVS_SendEvent);

    /* Set to generate error message CS_INIT_SB_SUBSCRIBE_CMD_ERR_EID */
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_Subscribe), 3, -1);

    /* Execute the function being tested */
    Result = CS_SbInit();

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventID, CS_INIT_SB_SUBSCRIBE_CMD_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent.Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent.Spec);

    UtAssert_True(Result == -1, "Result == -1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_Init_Test_SBSubscribeCmdError */

void CS_Init_Test_TableInitNominal(void)
{
    int32 Result;

    /* Execute the function being tested */
    Result = CS_InitAllTables();

    /* Verify results */

    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_Init_Test_TableInitNominal */

void CS_Init_Test_TableInitErrorEEPROM(void)
{
    int32 Result;
    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Table initialization failed for Eeprom: 0x%%08X");

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), UT_Utils_stub_reporter_hook, &context_CFE_EVS_SendEvent);

    /* Set to generate error message CS_INIT_EEPROM_ERR_EID */

    UT_SetDeferredRetcode(UT_KEY(CS_TableInit), 1, -1);

    /* Execute the function being tested */
    Result = CS_InitAllTables();

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventID, CS_INIT_EEPROM_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent.Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent.Spec);

    UtAssert_True(Result == -1, "Result == -1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
    /* Generates 1 event message we don't care about in this test */

} /* end CS_Init_Test_TableInitErrorEEPROM */

void CS_Init_Test_TableInitErrorMemory(void)
{
    int32 Result;
    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Table initialization failed for Memory: 0x%%08X");

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), UT_Utils_stub_reporter_hook, &context_CFE_EVS_SendEvent);

    /* Set to prevent unintended error messages */

    /* Set to generate error message CS_INIT_SB_CREATE_ERR_EID.
     * Combining a SetReturnCode and a SetFunctionHook in order to return CFE_SUCCESS all runs except the one specified
     */

    UT_SetDeferredRetcode(UT_KEY(CS_TableInit), 1, CFE_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(CS_TableInit), 1, -1);

    /* Execute the function being tested */
    Result = CS_InitAllTables();

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventID, CS_INIT_MEMORY_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent.Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent.Spec);

    UtAssert_True(Result == -1, "Result == -1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
    /* Generates 1 event message we don't care about in this test */

} /* end CS_Init_Test_TableInitErrorMemory */

void CS_Init_Test_TableInitErrorApps(void)
{
    int32 Result;
    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Table initialization failed for Apps: 0x%%08X");

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), UT_Utils_stub_reporter_hook, &context_CFE_EVS_SendEvent);

    /* Set to prevent unintended error messages */

    /* Set to generate error message CS_INIT_APP_ERR_EID.
     * Combining a SetReturnCode and a SetFunctionHook in order to return CFE_SUCCESS all runs except the one specified
     */

    UT_SetDeferredRetcode(UT_KEY(CS_TableInit), 2, CFE_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(CS_TableInit), 1, -1);

    /* Execute the function being tested */
    Result = CS_InitAllTables();

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventID, CS_INIT_APP_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent.Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent.Spec);

    UtAssert_True(Result == -1, "Result == -1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
    /* Generates 1 event message we don't care about in this test */

} /* end CS_Init_Test_TableInitErrorApps */

void CS_Init_Test_TableInitErrorTables(void)
{
    int32 Result;
    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Table initialization failed for Tables: 0x%%08X");

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), UT_Utils_stub_reporter_hook, &context_CFE_EVS_SendEvent);

    /* Set to prevent unintended error messages */

    /* Set to generate error message CS_INIT_TABLES_ERR_EID.
     * Combining a SetReturnCode and a SetFunctionHook in order to return CFE_SUCCESS all runs except the one specified
     */

    UT_SetDeferredRetcode(UT_KEY(CS_TableInit), 3, CFE_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(CS_TableInit), 1, -1);

    /* Execute the function being tested */
    Result = CS_InitAllTables();

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventID, CS_INIT_TABLES_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent.Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent.Spec);

    UtAssert_True(Result == -1, "Result == -1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
    /* Generates 1 event message we don't care about in this test */

} /* end CS_Init_Test_TableInitErrorTables */

void CS_Init_Test_CFETextSegmentInfoError(void)
{
    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "CFE Text Segment disabled");

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), UT_Utils_stub_reporter_hook, &context_CFE_EVS_SendEvent);

    /* Set to prevent unintended error messages */

    /* Set to prevent unintended error messages */

    /* Set to generate error message CS_CFE_TEXT_SEG_INF_EID */
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_GetCFETextSegmentInfo), 1, -1);

    /* Execute the function being tested */
    CS_InitSegments();

    /* Verify results */

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventID, CS_CFE_TEXT_SEG_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent.Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent.Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_Init_Test_TextSegmentInfoError */

void CS_Init_Test_KernelTextSegmentInfoError(void)
{
    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "OS Text Segment disabled due to platform");

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), UT_Utils_stub_reporter_hook, &context_CFE_EVS_SendEvent);

    /* Set to prevent unintended error messages */

    /* Set to prevent unintended error messages */

    /* Set to generate error message CS_OS_TEXT_SEG_INF_EID */
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_GetKernelTextSegmentInfo), 1, -1);

    /* Execute the function being tested */
    CS_InitSegments();

    /* Verify results */

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventID, CS_OS_TEXT_SEG_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent.Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent.Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end CS_Init_Test_TextSegmentInfoError */

void UtTest_Setup(void)
{
    UtTest_Add(CS_Init_Test_SBCreatePipeError, CS_Test_Setup, CS_Test_TearDown, "CS_Init_Test_SBCreatePipeError");
    UtTest_Add(CS_Init_Test_SBSubscribeHKNominal, CS_Test_Setup, CS_Test_TearDown, "CS_Init_Test_SBSubscribeHKNominal");
    UtTest_Add(CS_Init_Test_SBSubscribeHKError, CS_Test_Setup, CS_Test_TearDown, "CS_Init_Test_SBSubscribeHKError");
    UtTest_Add(CS_Init_Test_SBSubscribeBackgroundCycleError, CS_Test_Setup, CS_Test_TearDown,
               "CS_Init_Test_SBSubscribeBackgroundCycleError");
    UtTest_Add(CS_Init_Test_SBSubscribeCmdError, CS_Test_Setup, CS_Test_TearDown, "CS_Init_Test_SBSubscribeCmdError");
    UtTest_Add(CS_Init_Test_TableInitNominal, CS_Test_Setup, CS_Test_TearDown, "CS_Init_Test_TableInitNominal");
    UtTest_Add(CS_Init_Test_TableInitErrorEEPROM, CS_Test_Setup, CS_Test_TearDown, "CS_Init_Test_TableInitErrorEEPROM");
    UtTest_Add(CS_Init_Test_TableInitErrorMemory, CS_Test_Setup, CS_Test_TearDown, "CS_Init_Test_TableInitErrorMemory");
    UtTest_Add(CS_Init_Test_TableInitErrorApps, CS_Test_Setup, CS_Test_TearDown, "CS_Init_Test_TableInitErrorApps");
    UtTest_Add(CS_Init_Test_TableInitErrorTables, CS_Test_Setup, CS_Test_TearDown, "CS_Init_Test_TableInitErrorTables");
    UtTest_Add(CS_Init_Test_CFETextSegmentInfoError, CS_Test_Setup, CS_Test_TearDown,
               "CS_Init_Test_CFETextSegmentInfoError");
    UtTest_Add(CS_Init_Test_KernelTextSegmentInfoError, CS_Test_Setup, CS_Test_TearDown,
               "CS_Init_Test_KernelTextSegmentInfoError");

} /* end UtTest_Setup */

/************************/
/*  End of File Comment */
/************************/
