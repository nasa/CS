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
 *   The CFS Checksum (CS) Application's table updating functions
 */

/*************************************************************************
 **
 ** Include section
 **
 **************************************************************************/
#include "cfe.h"
#include "cs_app.h"
#include "cs_eventids.h"
#include "cs_tbl.h"
#include "cs_utils.h"
#include "cs_table_processing.h"

#include <string.h>

/* Errors that may be detected by table validation */
typedef enum CS_ValidationError
{
    CS_ValidationError_NONE, /* keep first, should be 0 */

    CS_ValidationError_LONG_NAME,
    CS_ValidationError_STATE,
    CS_ValidationError_ZERO_NAME,
    CS_ValidationError_DUPLICATE,
    CS_ValidationError_MEM_RANGE,

    CS_ValidationError_MAX /* keep last, indicates number of entries */

} CS_ValidationError_t;

/*
 * Helper struct to keep state of table validation
 * This allows app, table, memory, and eeprom table validation to
 * share common counting and reporting logic to keep things consistent.
 */
typedef struct CS_ValidationMetrics
{
    const char   *TableName;
    const uint16 *EventMap;

    uint16 Position;
    uint16 StateField;

    CS_ValidationError_t PendingError;
    char                 ErrorMessage[CFE_TBL_MAX_FULL_NAME_LEN]; /* Long enough to hold a table name, if needed */

    int32 GoodCount;
    int32 BadCount;
    int32 EmptyCount;
} CS_ValidationMetrics_t;

/*************************************************************************
 **
 ** Function Definitions
 **
 **************************************************************************/

/*----------------------------------------------------------------
 *
 * Local helper function
 * Checks if the overall validation is OK so far
 *
 *-----------------------------------------------------------------*/
static inline bool CS_ValidationMetrics_IsAllOK(const CS_ValidationMetrics_t *Metrics)
{
    return (Metrics->BadCount == 0);
}

/*----------------------------------------------------------------
 *
 * Local helper function
 * Checks if the current entry is OK
 *
 *-----------------------------------------------------------------*/
static inline bool CS_ValidationMetrics_IsCurrentOK(const CS_ValidationMetrics_t *Metrics)
{
    return (Metrics->PendingError == CS_ValidationError_NONE);
}

/*----------------------------------------------------------------
 *
 * Local helper function
 * Starts checking an entry, checks that the state is valid
 *
 *-----------------------------------------------------------------*/
void CS_ValidationMetrics_StartNext(CS_ValidationMetrics_t *Metrics, uint16 EntryState)
{
    Metrics->StateField      = EntryState;
    Metrics->ErrorMessage[0] = 0;

    if (!CS_StateValid(EntryState) && EntryState != CS_ChecksumState_EMPTY)
    {
        /* The state field is not one of the valid values */
        Metrics->PendingError = CS_ValidationError_STATE;
    }
    else
    {
        Metrics->PendingError = CS_ValidationError_NONE;
    }
}

/*----------------------------------------------------------------
 *
 * Local helper function
 * Reports an error event associated with table validation
 *
 *-----------------------------------------------------------------*/
void CS_ValidationMetrics_ReportEvent(CS_ValidationMetrics_t *Metrics)
{
    uint16 PendingEventId;

    PendingEventId = Metrics->EventMap[Metrics->PendingError];
    switch (Metrics->PendingError)
    {
        case CS_ValidationError_STATE:
        default:
            CFE_EVS_SendEvent(PendingEventId,
                              CFE_EVS_EventType_ERROR,
                              "%s Table Validate: Illegal State Field (0x%04X) found in Entry ID %d",
                              Metrics->TableName,
                              (unsigned short)Metrics->StateField,
                              (int)Metrics->Position);
            break;

        case CS_ValidationError_LONG_NAME:
            CFE_EVS_SendEvent(PendingEventId,
                              CFE_EVS_EventType_ERROR,
                              "%s Table Validate: Unterminated Name found at entry %d",
                              Metrics->TableName,
                              (int)Metrics->Position);
            break;

        case CS_ValidationError_ZERO_NAME:
            CFE_EVS_SendEvent(PendingEventId,
                              CFE_EVS_EventType_ERROR,
                              "%s Table Validate: Illegal State (0x%04X) with empty name at entry %d",
                              Metrics->TableName,
                              (unsigned short)Metrics->StateField,
                              (int)Metrics->Position);
            break;

        case CS_ValidationError_DUPLICATE:
            CFE_EVS_SendEvent(PendingEventId,
                              CFE_EVS_EventType_ERROR,
                              "%s Table Validate: Duplicate Name (%s) found at entry %d",
                              Metrics->TableName,
                              Metrics->ErrorMessage,
                              (int)Metrics->Position);
            break;

        case CS_ValidationError_MEM_RANGE:
            CFE_EVS_SendEvent(PendingEventId,
                              CFE_EVS_EventType_ERROR,
                              "%s Table Validate: Illegal MemRange in Entry ID %d, "
                              "CFE_PSP_MemValidateRange: %s",
                              Metrics->TableName,
                              (int)Metrics->Position,
                              Metrics->ErrorMessage);
            break;
    }
}

/*----------------------------------------------------------------
 *
 * Local helper function
 * Accumulate table validation counters
 *
 *-----------------------------------------------------------------*/
void CS_ValidationMetrics_Accumulate(CS_ValidationMetrics_t *Metrics)
{
    /* increment the respective counter for final report */
    if (!CS_ValidationMetrics_IsCurrentOK(Metrics))
    {
        /* Only do event report on the first error */
        if (CS_ValidationMetrics_IsAllOK(Metrics))
        {
            CS_ValidationMetrics_ReportEvent(Metrics);
        }

        ++Metrics->BadCount;
    }
    else if (Metrics->StateField == CS_ChecksumState_EMPTY)
    {
        ++Metrics->EmptyCount;
    }
    else
    {
        ++Metrics->GoodCount;
    }

    ++Metrics->Position;
}

/*----------------------------------------------------------------
 *
 * Local helper function
 * Gets the final status of the table validation and report INFO event
 *
 *-----------------------------------------------------------------*/
int32 CS_ValidationMetrics_GetFinalStatus(const CS_ValidationMetrics_t *Metrics)
{
    CFE_EVS_SendEvent(Metrics->EventMap[0],
                      CFE_EVS_EventType_INFORMATION,
                      "CS %s Table verification results: good = %d, bad = %d, unused = %d",
                      Metrics->TableName,
                      (int)Metrics->GoodCount,
                      (int)Metrics->BadCount,
                      (int)Metrics->EmptyCount);

    if (CS_ValidationMetrics_IsAllOK(Metrics))
    {
        return CFE_SUCCESS;
    }
    else
    {
        return CS_TABLE_ERROR;
    }
}

/*----------------------------------------------------------------
 *
 * Local helper function
 * Call respective table update handler
 *
 *-----------------------------------------------------------------*/
void CS_CallTableUpdateHandler(CS_TableWrapper_t *tw)
{
    if (tw->UpdateHandler)
    {
        tw->UpdateHandler(tw);
    }
}

/*----------------------------------------------------------------
 *
 * Local helper function
 * Checks for duplicate name enty in table
 *
 *-----------------------------------------------------------------*/
void CS_CheckForDuplicateEntry(CS_ValidationMetrics_t *Metrics, const char *RefName, size_t EntrySize)
{
    const char *EntryName;
    uint16      Count;

    EntryName = RefName;
    Count     = Metrics->Position;

    while (Count > 0)
    {
        --Count;
        EntryName -= EntrySize;

        if (strcmp(RefName, EntryName) == 0)
        {
            Metrics->PendingError = CS_ValidationError_DUPLICATE;
            snprintf(Metrics->ErrorMessage, sizeof(Metrics->ErrorMessage), "position=%d", (int)Count);
            break;
        }
    }
}

/*----------------------------------------------------------------
 *
 * Local helper function
 * Checks sanity of entry name
 *
 *-----------------------------------------------------------------*/
void CS_ValidateEntryName(CS_ValidationMetrics_t *Metrics, const char *NameField, size_t NameSize)
{
    if (CS_StateValid(Metrics->StateField))
    {
        if (memchr(NameField, 0, NameSize) == NULL)
        {
            /* Not null-terminated, name is too long */
            Metrics->PendingError = CS_ValidationError_LONG_NAME;
        }
        else if (NameField[0] == 0)
        {
            /* non-empty entries MUST have a name */
            Metrics->PendingError = CS_ValidationError_ZERO_NAME;
        }
    }
    else if (NameField[0] != 0)
    {
        /* empty entries must NOT have a name */
        Metrics->PendingError = CS_ValidationError_STATE;
    }
}

/*----------------------------------------------------------------
 *
 * Local helper function
 * Checks sanity of entry address
 *
 *-----------------------------------------------------------------*/
void CS_ValidateEntryAddress(CS_ValidationMetrics_t *Metrics, cpuaddr MemoryAddr, size_t MemorySize, uint32 MemoryType)
{
    int32 PspStatus;

    if (CS_StateValid(Metrics->StateField))
    {
        PspStatus = CFE_PSP_MemValidateRange(MemoryAddr, MemorySize, MemoryType);
        if (PspStatus != CFE_PSP_SUCCESS)
        {
            Metrics->PendingError = CS_ValidationError_MEM_RANGE;
            snprintf(Metrics->ErrorMessage, sizeof(Metrics->ErrorMessage), "status=%d", (int)PspStatus);
        }
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Validation Callback function for EEPROM Table                */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CFE_Status_t CS_ValidateEepromChecksumDefinitionTable(void *TblPtr)
{
    static const uint16 EEPROM_EVENTID_MAP[CS_ValidationError_MAX] = {
        [CS_ValidationError_NONE]      = CS_VAL_EEPROM_INF_EID,
        [CS_ValidationError_STATE]     = CS_VAL_EEPROM_STATE_ERR_EID,
        [CS_ValidationError_MEM_RANGE] = CS_VAL_EEPROM_RANGE_ERR_EID,
    };

    CS_Def_EepromMemory_Table_Entry_t *Entry;
    CS_ValidationMetrics_t             Metrics;

    memset(&Metrics, 0, sizeof(Metrics));

    /* These are needed for assembling event data, if necessary */
    Metrics.TableName = "EEPROM";
    Metrics.EventMap  = EEPROM_EVENTID_MAP;

    Entry = (CS_Def_EepromMemory_Table_Entry_t *)TblPtr;
    while (Metrics.Position < CS_MAX_NUM_EEPROM_TABLE_ENTRIES)
    {
        CS_ValidationMetrics_StartNext(&Metrics, Entry->State);

        CS_ValidateEntryAddress(&Metrics, Entry->StartAddress, Entry->NumBytesToChecksum, CFE_PSP_MEM_EEPROM);

        /* increment the respective counter for final report */
        CS_ValidationMetrics_Accumulate(&Metrics);
        ++Entry;
    }

    return CS_ValidationMetrics_GetFinalStatus(&Metrics);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Validation Callback function for Memory Table                */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CFE_Status_t CS_ValidateMemoryChecksumDefinitionTable(void *TblPtr)
{
    static const uint16 MEMORY_EVENTID_MAP[CS_ValidationError_MAX] = {
        [CS_ValidationError_NONE]      = CS_VAL_MEMORY_INF_EID,
        [CS_ValidationError_STATE]     = CS_VAL_MEMORY_STATE_ERR_EID,
        [CS_ValidationError_MEM_RANGE] = CS_VAL_MEMORY_RANGE_ERR_EID,
    };

    CS_Def_EepromMemory_Table_Entry_t *Entry;
    CS_ValidationMetrics_t             Metrics;

    memset(&Metrics, 0, sizeof(Metrics));

    /* These are needed for assembling event data, if necessary */
    Metrics.TableName = "Memory";
    Metrics.EventMap  = MEMORY_EVENTID_MAP;

    Entry = (CS_Def_EepromMemory_Table_Entry_t *)TblPtr;
    while (Metrics.Position < CS_MAX_NUM_MEMORY_TABLE_ENTRIES)
    {
        CS_ValidationMetrics_StartNext(&Metrics, Entry->State);

        CS_ValidateEntryAddress(&Metrics, Entry->StartAddress, Entry->NumBytesToChecksum, CFE_PSP_MEM_ANY);

        /* increment the respective counter for final report */
        CS_ValidationMetrics_Accumulate(&Metrics);
        ++Entry;
    }

    return CS_ValidationMetrics_GetFinalStatus(&Metrics);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Validation Callback function for Tables Table                */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CFE_Status_t CS_ValidateTablesChecksumDefinitionTable(void *TblPtr)
{
    static const uint16 TABLES_EVENTID_MAP[CS_ValidationError_MAX] = {
        [CS_ValidationError_NONE]      = CS_VAL_TABLES_INF_EID,
        [CS_ValidationError_LONG_NAME] = CS_VAL_TABLES_DEF_TBL_LONG_NAME_ERR_EID,
        [CS_ValidationError_STATE]     = CS_VAL_TABLES_STATE_ERR_EID,
        [CS_ValidationError_ZERO_NAME] = CS_VAL_TABLES_DEF_TBL_ZERO_NAME_ERR_EID,
        [CS_ValidationError_DUPLICATE] = CS_VAL_TABLES_DEF_TBL_DUPL_ERR_EID,
    };

    CS_Def_Tables_Table_Entry_t *Entry = NULL;
    CS_ValidationMetrics_t       Metrics;

    memset(&Metrics, 0, sizeof(Metrics));

    /* These are needed for assembling event data, if necessary */
    Metrics.TableName = "Tables";
    Metrics.EventMap  = TABLES_EVENTID_MAP;

    Entry = (CS_Def_Tables_Table_Entry_t *)TblPtr;
    while (Metrics.Position < CS_MAX_NUM_TABLES_TABLE_ENTRIES)
    {
        CS_ValidationMetrics_StartNext(&Metrics, Entry->State);

        CS_ValidateEntryName(&Metrics, Entry->Name, sizeof(Entry->Name));
        if (CS_ValidationMetrics_IsCurrentOK(&Metrics) && CS_StateValid(Entry->State))
        {
            CS_CheckForDuplicateEntry(&Metrics, Entry->Name, sizeof(*Entry));
        }

        /* increment the respective counter for final report */
        CS_ValidationMetrics_Accumulate(&Metrics);
        ++Entry;
    }

    return CS_ValidationMetrics_GetFinalStatus(&Metrics);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Validation Callback function for App Table                   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CFE_Status_t CS_ValidateAppChecksumDefinitionTable(void *TblPtr)
{
    static const uint16 APP_EVENTID_MAP[CS_ValidationError_MAX] = {
        [CS_ValidationError_NONE]      = CS_VAL_APP_INF_EID,
        [CS_ValidationError_LONG_NAME] = CS_VAL_APP_DEF_TBL_LONG_NAME_ERR_EID,
        [CS_ValidationError_STATE]     = CS_VAL_APP_STATE_ERR_EID,
        [CS_ValidationError_ZERO_NAME] = CS_VAL_APP_DEF_TBL_ZERO_NAME_ERR_EID,
        [CS_ValidationError_DUPLICATE] = CS_VAL_APP_DEF_TBL_DUPL_ERR_EID,
    };

    CS_Def_App_Table_Entry_t *Entry;
    CS_ValidationMetrics_t    Metrics;

    memset(&Metrics, 0, sizeof(Metrics));

    /* These are needed for assembling event data, if necessary */
    Metrics.TableName = "Apps";
    Metrics.EventMap  = APP_EVENTID_MAP;

    Entry = (CS_Def_App_Table_Entry_t *)TblPtr;
    while (Metrics.Position < CS_MAX_NUM_APP_TABLE_ENTRIES)
    {
        CS_ValidationMetrics_StartNext(&Metrics, Entry->State);

        CS_ValidateEntryName(&Metrics, Entry->Name, sizeof(Entry->Name));
        if (CS_ValidationMetrics_IsCurrentOK(&Metrics) && CS_StateValid(Entry->State))
        {
            CS_CheckForDuplicateEntry(&Metrics, Entry->Name, sizeof(*Entry));
        }

        /* increment the respective counter for final report */
        CS_ValidationMetrics_Accumulate(&Metrics);
        ++Entry;
    }

    return CS_ValidationMetrics_GetFinalStatus(&Metrics);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS  processing new definition tables for EEPROM or Memory       */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_ProcessNewEepromMemoryDefinitionTable(CS_TableWrapper_t *tw)
{
    const CS_Def_EepromMemory_Table_Entry_t *DefEntry          = NULL;
    CS_Res_EepromMemory_Table_Entry_t       *ResultsEntry      = NULL;
    uint16                                   Loop              = 0;
    uint16                                   NumRegionsInTable = 0;
    CS_ChecksumState_Enum_t                  PreviousState     = CS_ChecksumState_EMPTY;

    /* We don't want to be doing chekcksums while changing the table out */
    if (tw->GlobalState)
    {
        PreviousState    = *tw->GlobalState;
        *tw->GlobalState = CS_ChecksumState_DISABLED;
    }

    Loop = 0;
    while (true)
    {
        ResultsEntry = CS_GetResEntryAddr(tw, Loop);
        DefEntry     = CS_GetDefEntryAddr(tw, Loop);

        if (ResultsEntry == NULL || DefEntry == NULL)
        {
            break;
        }

        /* wipe the entry, sets everything to safe state, state set to CS_ChecksumState_EMPTY (0) */
        memset(ResultsEntry, 0, sizeof(*ResultsEntry));

        if (CS_StateValid(DefEntry->State))
        {
            /* This is just a real simple test, because all of the exception handling
             has already been done by the Validation routine above */
            NumRegionsInTable++;

            ResultsEntry->State              = DefEntry->State;
            ResultsEntry->NumBytesToChecksum = DefEntry->NumBytesToChecksum;
            ResultsEntry->StartAddress       = DefEntry->StartAddress;
        }

        ++Loop;
    }

    /* Reset the table back to the original checksumming state */
    if (tw->GlobalState)
    {
        *tw->GlobalState = PreviousState;
    }

    CS_ResetTablesTblResultEntry(tw->ResTblPtr);

    if (NumRegionsInTable == 0)
    {
        CFE_EVS_SendEvent(CS_PROCESS_EEPROM_MEMORY_NO_ENTRIES_INF_EID,
                          CFE_EVS_EventType_INFORMATION,
                          "CS %s Table: No valid entries in the table",
                          CS_GetTableTypeAsString(tw));
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Helper function to split the App.Table name pattern             */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_ExtractNames(const CS_Def_Tables_Table_Entry_t *DefEntry,
                     char                              *AppBuf,
                     size_t                             AppSz,
                     char                              *TableBuf,
                     size_t                             TableSz)
{
    const char *Sep;
    size_t      TableNameLen;
    size_t      AppNameLen;

    /* Look for the separator char (.) somewhere in the name */
    TableNameLen = OS_strnlen(DefEntry->Name, sizeof(DefEntry->Name));
    Sep          = memchr(DefEntry->Name, '.', TableNameLen);
    if (Sep == NULL)
    {
        /* There is no separator, assume the name is just a table name */
        Sep        = DefEntry->Name;
        AppNameLen = 0;
    }
    else
    {
        /* Move past the separator */
        AppNameLen = Sep - DefEntry->Name;
        ++Sep;
    }

    TableNameLen -= AppNameLen;

    if (AppNameLen >= AppSz)
    {
        AppNameLen = AppSz - 1;
    }
    if (TableNameLen >= TableSz)
    {
        TableNameLen = TableSz - 1;
    }

    memcpy(AppBuf, DefEntry->Name, AppNameLen);
    AppBuf[AppNameLen] = 0;
    memcpy(TableBuf, Sep, TableNameLen);
    TableBuf[TableNameLen] = 0;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS processing new definition tables for Tables                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_ProcessNewTablesDefinitionTable(CS_TableWrapper_t *tw)
{
    const CS_Def_Tables_Table_Entry_t *DefEntry          = NULL;
    CS_Res_Tables_Table_Entry_t       *ResultsEntry      = NULL;
    uint16                             Loop              = 0;
    uint16                             NumRegionsInTable = 0;
    CS_ChecksumState_Enum_t            PreviousState     = CS_ChecksumState_EMPTY;
    CFE_ES_AppId_t                     AppID             = CFE_ES_APPID_UNDEFINED;
    CFE_TBL_Handle_t                   TableHandle       = CFE_TBL_BAD_TABLE_HANDLE;
    bool                               Owned             = false;
    char                               AppName[OS_MAX_API_NAME];
    char                               TableAppName[OS_MAX_API_NAME];
    char                               TableTableName[CFE_MISSION_TBL_MAX_NAME_LENGTH];
    uint16                             TableId;

    CFE_ES_GetAppID(&AppID);
    CFE_ES_GetAppName(AppName, AppID, OS_MAX_API_NAME);

    /* We don't want to be doing chekcksums while changing the table out */
    PreviousState                             = CS_AppData.HkPacket.Payload.TablesCSState;
    CS_AppData.HkPacket.Payload.TablesCSState = CS_ChecksumState_DISABLED;

    /* Assume none of the CS tables are listed in the new Tables table */
    for (TableId = 0; TableId < CS_NUM_TABLES; ++TableId)
    {
        CS_AppData.Tbl[TableId].ResTblPtr = NULL;
    }

    Loop = 0;
    while (true)
    {
        DefEntry     = CS_GetDefEntryAddr(tw, Loop);
        ResultsEntry = CS_GetResEntryAddr(tw, Loop);

        if (DefEntry == NULL || ResultsEntry == NULL)
        {
            break;
        }

        /* wipe the entry, sets everything to safe state, state set to CS_ChecksumState_EMPTY (0) */
        /* This also needs to explicitly set the TBL handle as 0 might be valid */
        memset(ResultsEntry, 0, sizeof(*ResultsEntry));
        ResultsEntry->TblHandleID = CFE_TBL_HANDLEID_UNDEFINED;

        if (CS_StateValid(DefEntry->State))
        {
            CS_ExtractNames(DefEntry, TableAppName, sizeof(TableAppName), TableTableName, sizeof(TableTableName));

            TableHandle = CFE_TBL_BAD_TABLE_HANDLE;
            Owned       = false;

            /* if the table's owner's name is CS */
            if (strcmp(TableAppName, AppName) == 0)
            {
                for (TableId = 0; TableId < CS_NUM_TABLES; ++TableId)
                {
                    if (CS_CheckDefTableNameMatch(TableTableName, TableId))
                    {
                        CS_AppData.Tbl[TableId].ResTblPtr = ResultsEntry;

                        TableHandle = CS_GetDefHandle(TableId);
                        Owned       = true;
                        break;
                    }
                    if (CS_CheckResTableNameMatch(TableTableName, TableId))
                    {
                        TableHandle = CS_GetResHandle(TableId);
                        Owned       = true;
                        break;
                    }
                }
            }

            /* This is just a real simple test, because all of the exception handling
             has already been done by the Validation routine above */
            NumRegionsInTable++;

            ResultsEntry->State       = DefEntry->State;
            ResultsEntry->TblHandleID = CFE_TBL_HandleToID(TableHandle);
            ResultsEntry->IsCSOwner   = Owned;

            CFE_SB_MessageStringGet(ResultsEntry->Name,
                                    DefEntry->Name,
                                    NULL,
                                    sizeof(ResultsEntry->Name),
                                    sizeof(DefEntry->Name));
        }

        ++Loop;
    }

    /* Reset the table back to the original checksumming state */

    CS_AppData.HkPacket.Payload.TablesCSState = PreviousState;

    if (NumRegionsInTable == 0)
    {
        CFE_EVS_SendEvent(CS_PROCESS_TABLES_NO_ENTRIES_INF_EID,
                          CFE_EVS_EventType_INFORMATION,
                          "CS Tables Table: No valid entries in the table");
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS processing new definition tables for Apps                    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_ProcessNewAppDefinitionTable(CS_TableWrapper_t *tw)
{
    const CS_Def_App_Table_Entry_t *DefEntry          = NULL;
    CS_Res_App_Table_Entry_t       *ResultsEntry      = NULL;
    uint16                          Loop              = 0;
    uint16                          NumRegionsInTable = 0;
    CS_ChecksumState_Enum_t         PreviousState     = CS_ChecksumState_EMPTY;

    /* We don't want to be doing chekcksums while changing the table out */

    PreviousState                          = CS_AppData.HkPacket.Payload.AppCSState;
    CS_AppData.HkPacket.Payload.AppCSState = CS_ChecksumState_DISABLED;

    Loop = 0;
    while (true)
    {
        DefEntry     = CS_GetDefEntryAddr(tw, Loop);
        ResultsEntry = CS_GetResEntryAddr(tw, Loop);

        if (DefEntry == NULL || ResultsEntry == NULL)
        {
            break;
        }

        /* wipe the entry, sets everything to safe state, state set to CS_ChecksumState_EMPTY (0) */
        memset(ResultsEntry, 0, sizeof(*ResultsEntry));

        if (CS_StateValid(DefEntry->State))
        {
            /* This is just a real simple test, because all of the exception handling
             has already been done by the Validation routine above */
            NumRegionsInTable++;

            ResultsEntry->State = DefEntry->State;
            CFE_SB_MessageStringGet(ResultsEntry->Name,
                                    DefEntry->Name,
                                    NULL,
                                    sizeof(ResultsEntry->Name),
                                    sizeof(DefEntry->Name));
        }

        ++Loop;
    }

    /* Reset the table back to the original checksumming state */

    CS_AppData.HkPacket.Payload.AppCSState = PreviousState;
    CS_ResetTablesTblResultEntry(tw->ResTblPtr);

    if (NumRegionsInTable == 0)
    {
        CFE_EVS_SendEvent(CS_PROCESS_APP_NO_ENTRIES_INF_EID,
                          CFE_EVS_EventType_INFORMATION,
                          "CS Apps Table: No valid entries in the table");
    }
}

/*----------------------------------------------------------------
 *
 * Local helper function
 * Register the result table with CFE table services and obtain the address
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CS_RegisterResultTable(CS_TableWrapper_t *tw)
{
    CFE_Status_t Result;

    Result = CFE_TBL_Register(&tw->ResHandle,
                              tw->ResTableName,
                              tw->NumEntries * tw->ResEntrySize,
                              CFE_TBL_OPT_SNGL_BUFFER | CFE_TBL_OPT_DUMP_ONLY,
                              NULL);

    if (Result >= CFE_SUCCESS)
    {
        Result = CFE_TBL_GetAddress(&tw->ResAddr, tw->ResHandle);
    }

    if (Result >= CFE_SUCCESS)
    {
        Result = CFE_SUCCESS;
    }

    return Result;
}

/*----------------------------------------------------------------
 *
 * Local helper function
 * Register the definition table with CFE table services, load the
 * table, and obtain the address.
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CS_RegisterDefinitionTable(CS_TableWrapper_t        *tw,
                                        const char               *DefinitionTableFileName,
                                        CFE_TBL_CallbackFuncPtr_t CallBackFunction)
{
    bool         LoadedFromMemory = false;
    CFE_Status_t Result;

    Result = CFE_TBL_Register(&tw->DefHandle,
                              tw->DefTableName,
                              tw->NumEntries * tw->DefEntrySize,
                              CFE_TBL_OPT_SNGL_BUFFER | CFE_TBL_OPT_LOAD_DUMP,
                              CallBackFunction);

    if (Result >= CFE_SUCCESS)
    {
        Result = CFE_TBL_Load(tw->DefHandle, CFE_TBL_SRC_FILE, DefinitionTableFileName);

        /* if the load from the file fails, load from the default tables in CS */
        if (Result < CFE_SUCCESS)
        {
            Result           = CFE_TBL_Load(tw->DefHandle, CFE_TBL_SRC_ADDRESS, tw->DefaultDefinitionPtr);
            LoadedFromMemory = (Result >= CFE_SUCCESS);
        }
    }

    if (Result >= CFE_SUCCESS)
    {
        Result = CFE_TBL_GetAddress(&tw->DefAddr, tw->DefHandle);
    }

    if (Result >= CFE_SUCCESS)
    {
        /* If we loaded from file successfully then the states we wish to use have already been set
         * If we loaded from memory then disable the table  */
        if (LoadedFromMemory && tw->GlobalState != NULL)
        {
            *tw->GlobalState = CS_ChecksumState_DISABLED;
        }

        CS_CallTableUpdateHandler(tw);
        Result = CFE_SUCCESS;
    }

    return Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS  function for initializing new tables                        */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CFE_Status_t
CS_TableInit(CS_TableWrapper_t *tw, const char *DefinitionTableFileName, CFE_TBL_CallbackFuncPtr_t CallBackFunction)
{
    CFE_Status_t Result;

    if (DefinitionTableFileName == NULL)
    {
        Result = CFE_SUCCESS; /* nothing to do */
    }
    else
    {
        Result = CS_RegisterResultTable(tw);

        if (Result == CFE_SUCCESS)
        {
            Result = CS_RegisterDefinitionTable(tw, DefinitionTableFileName, CallBackFunction);
        }

        if (Result != CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(CS_TBL_INIT_ERR_EID,
                              CFE_EVS_EventType_ERROR,
                              "CS received error 0x%08X initializing tables for %s",
                              (unsigned int)Result,
                              CS_GetTableTypeAsString(tw));
        }
    }

    return Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Handles table updates                                        */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CFE_Status_t CS_HandleTableUpdate(CS_TableWrapper_t *tw)
{
    CFE_Status_t ReleaseResult1 = CFE_SUCCESS;
    CFE_Status_t ManageResult1  = CFE_SUCCESS;
    CFE_Status_t GetResult1     = CFE_SUCCESS;
    CFE_Status_t ReleaseResult2 = CFE_SUCCESS;
    CFE_Status_t ManageResult2  = CFE_SUCCESS;
    CFE_Status_t GetResult2     = CFE_SUCCESS;
    CFE_Status_t Result         = CFE_SUCCESS;
    int32        Loop           = 0;

    CFE_TBL_Handle_t             LocalHandle;
    CS_Res_Tables_Table_Entry_t *ResTablesTblPtr;

    /* Below, there are several values that are returned and assigned, but never evaluated. */
    /* This is done so intentionally, as it helps us with Source-Level debugging this functions. */

    /* Release the Table Address.  */
    ReleaseResult1 = CFE_TBL_ReleaseAddress(tw->ResHandle);
    ReleaseResult2 = CFE_TBL_ReleaseAddress(tw->DefHandle);

    ManageResult1 = CFE_TBL_Manage(tw->ResHandle);
    ManageResult2 = CFE_TBL_Manage(tw->DefHandle);

    GetResult1 = CFE_TBL_GetAddress(&tw->ResAddr, tw->ResHandle);
    Result     = GetResult1;

    if (Result >= CFE_SUCCESS)
    {
        GetResult2 = CFE_TBL_GetAddress(&tw->DefAddr, tw->DefHandle);
        Result     = GetResult2;
    }

    if ((Result == CFE_TBL_INFO_UPDATED))
    {
        if (CS_CheckTableId(tw, CS_ChecksumType_TABLES_TABLE))
        {
            /* before we update the results table, we need to release all of the
             table handles that are in the results table */
            Loop = 0;
            while (true)
            {
                ResTablesTblPtr = CS_GetTablesResEntry(Loop);
                if (ResTablesTblPtr == NULL)
                {
                    break;
                }
                if (!ResTablesTblPtr->IsCSOwner)
                {
                    LocalHandle = CFE_TBL_HandleFromID(ResTablesTblPtr->TblHandleID);
                }
                else
                {
                    LocalHandle = CFE_TBL_BAD_TABLE_HANDLE;
                }
                if (CFE_TBL_HANDLE_IS_VALID(LocalHandle))
                {
                    CFE_TBL_Unregister(LocalHandle);
                }
                ++Loop;
            }
        }

        CS_CallTableUpdateHandler(tw);

        Result = CFE_SUCCESS;
    }
    else
    {
        if (Result < CFE_SUCCESS)
        {
            /* There was a problem somewhere, generate an event */
            CFE_EVS_SendEvent(CS_TBL_UPDATE_ERR_EID,
                              CFE_EVS_EventType_ERROR,
                              "CS had problems updating table. Res Release: 0x%08X Def Release:0x%08X Res "
                              "Manage:0x%08X Def Manage: 0x%08X Get:0x%08X for table %s",
                              (unsigned int)ReleaseResult1,
                              (unsigned int)ReleaseResult2,
                              (unsigned int)ManageResult1,
                              (unsigned int)ManageResult2,
                              (unsigned int)GetResult2,
                              CS_GetTableTypeAsString(tw));
        }
    }
    return Result;
}
