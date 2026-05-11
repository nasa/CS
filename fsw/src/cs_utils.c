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
 *   The CFS Checksum (CS) Application's utility functions
 */

/**************************************************************************
 **
 ** Include section
 **
 **************************************************************************/
#include "cfe.h"
#include "cs_app.h"
#include "cs_eventids.h"
#include "cs_compute.h"
#include "cs_utils.h"
#include "cs_table_processing.h"
#include <string.h>

/**************************************************************************
 **
 ** Functions
 **
 **************************************************************************/

/*----------------------------------------------------------------
 *
 * Local helper function
 * Checks if two names match in a NULL-safe manner.
 * If either string is NULL then there is no match.
 *
 *-----------------------------------------------------------------*/
bool CS_CheckGenericNameMatch(const char *Name1, const char *Name2)
{
    if (Name1 == NULL || Name2 == NULL)
    {
        return false;
    }

    return (strcmp(Name1, Name2) == 0);
}

/*----------------------------------------------------------------
 *
 * Application utility function
 * Checks if the definition table name matches (NULL-safe)
 *
 *-----------------------------------------------------------------*/
bool CS_CheckDefTableNameMatch(const char *Name, uint16 TableId)
{
    return CS_CheckGenericNameMatch(CS_GetDefName(TableId), Name);
}

/*----------------------------------------------------------------
 *
 * Application utility function
 * Checks if the result table name matches (NULL-safe)
 *
 *-----------------------------------------------------------------*/
bool CS_CheckResTableNameMatch(const char *Name, uint16 TableId)
{
    return CS_CheckGenericNameMatch(CS_GetResName(TableId), Name);
}

/*----------------------------------------------------------------
 *
 * Application utility function
 * Gets a pointer to the given definition table entry
 *
 *-----------------------------------------------------------------*/
void *CS_GetDefEntryAddr(CS_TableWrapper_t *tw, uint16 EntryId)
{
    uint8 *EntryAddr = tw->DefAddr;

    if (tw->DefAddr != NULL && EntryId < tw->NumEntries)
    {
        EntryAddr  = tw->DefAddr;
        EntryAddr += (tw->DefEntrySize * EntryId);
    }
    else
    {
        EntryAddr = NULL;
    }

    return EntryAddr;
}

/*----------------------------------------------------------------
 *
 * Application utility function
 * Gets a pointer to the given result table entry
 *
 *-----------------------------------------------------------------*/
void *CS_GetResEntryAddr(CS_TableWrapper_t *tw, uint16 EntryId)
{
    uint8 *EntryAddr;

    if (tw->ResAddr != NULL && EntryId < tw->NumEntries)
    {
        EntryAddr  = tw->ResAddr;
        EntryAddr += (tw->ResEntrySize * EntryId);
    }
    else
    {
        EntryAddr = NULL;
    }

    return EntryAddr;
}

/*----------------------------------------------------------------
 *
 * Local helper function
 * Sets the state of a definition table entry
 *
 *-----------------------------------------------------------------*/
CS_ChecksumState_Enum_t CS_SetGenericEntryState(void *EntryPtr, size_t Offset, CS_ChecksumState_Enum_t NewState)
{
    uint8                  *StateAddr;
    CS_ChecksumState_Enum_t PreviousDefState;
    uint16                 *StatePtr;

    PreviousDefState = CS_ChecksumState_UNDEFINED;
    if (CS_StateValid(NewState))
    {
        StateAddr = EntryPtr;
    }
    else
    {
        StateAddr = NULL;
    }

    if (StateAddr != NULL)
    {
        StateAddr += Offset;
        StatePtr   = (uint16 *)(void *)StateAddr;

        PreviousDefState = *StatePtr;
        if (CS_StateValid(PreviousDefState))
        {
            *StatePtr = NewState;
        }
    }

    return PreviousDefState;
}

/*----------------------------------------------------------------
 *
 * Application utility function
 * Sets the state of a definition table entry
 *
 *-----------------------------------------------------------------*/
CS_ChecksumState_Enum_t CS_SetDefEntryState(CS_TableWrapper_t *tw, void *EntryPtr, CS_ChecksumState_Enum_t NewState)
{
    CS_ChecksumState_Enum_t PreviousDefState;

    PreviousDefState = CS_SetGenericEntryState(EntryPtr, tw->DefStateOffset, NewState);
    if (CS_StateValid(PreviousDefState) && PreviousDefState != NewState)
    {
        CS_ResetTablesTblResultEntry(tw->ResTblPtr);
        CFE_TBL_Modified(tw->DefHandle);
    }

    return PreviousDefState;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Gets a table type ID value as a printable string                */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
const char *CS_GetTableTypeAsString(const CS_TableWrapper_t *tw)
{
    const char *TableTypeStr;

    switch (CS_GetTableId(tw))
    {
        case CS_ChecksumType_APP_TABLE:
            TableTypeStr = "Apps";
            break;
        case CS_ChecksumType_TABLES_TABLE:
            TableTypeStr = "Tables";
            break;
        case CS_ChecksumType_EEPROM_TABLE:
            TableTypeStr = "EEPROM";
            break;
        case CS_ChecksumType_MEMORY_TABLE:
            TableTypeStr = "Memory";
            break;
        case CS_ChecksumType_CFECORE:
            TableTypeStr = "cFE Core";
            break;
        case CS_ChecksumType_OSCORE:
            TableTypeStr = "OS";
            break;
        default:
            TableTypeStr = "Undef";
            break;
    }

    return TableTypeStr;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Zero out the temp chcksum values of EEPROM                   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_ZeroEepromTempValues(void)
{
    CS_Res_EepromMemory_Table_Entry_t *ResEepromTblPtr;
    uint16                             Loop;

    Loop = 0;
    while (true)
    {
        ResEepromTblPtr = CS_GetEepromResEntry(Loop);
        if (ResEepromTblPtr == NULL)
        {
            break;
        }

        ResEepromTblPtr->TempChecksumValue = 0;
        ResEepromTblPtr->ByteOffset        = 0;

        ++Loop;
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Zero out the temp chcksum values of Memory                   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_ZeroMemoryTempValues(void)
{
    CS_Res_EepromMemory_Table_Entry_t *ResMemoryTblPtr;
    uint16                             Loop;

    Loop = 0;
    while (true)
    {
        ResMemoryTblPtr = CS_GetMemoryResEntry(Loop);
        if (ResMemoryTblPtr == NULL)
        {
            break;
        }

        ResMemoryTblPtr->TempChecksumValue = 0;
        ResMemoryTblPtr->ByteOffset        = 0;

        ++Loop;
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Zero out the temp chcksum values of Tables                   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_ZeroTablesTempValues(void)
{
    CS_Res_Tables_Table_Entry_t *ResTablesTblPtr;
    uint16                       Loop;

    Loop = 0;
    while (true)
    {
        ResTablesTblPtr = CS_GetTablesResEntry(Loop);
        if (ResTablesTblPtr == NULL)
        {
            break;
        }

        ResTablesTblPtr->TempChecksumValue = 0;
        ResTablesTblPtr->ByteOffset        = 0;

        ++Loop;
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Zero out the temp chcksum values of Applications             */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_ZeroAppTempValues(void)
{
    CS_Res_App_Table_Entry_t *ResAppTblPtr;
    uint16                    Loop;

    Loop = 0;
    while (true)
    {
        ResAppTblPtr = CS_GetAppResEntry(Loop);
        if (ResAppTblPtr == NULL)
        {
            break;
        }

        ResAppTblPtr->TempChecksumValue = 0;
        ResAppTblPtr->ByteOffset        = 0;

        ++Loop;
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Zero out the temp chcksum values ofthe cFE core              */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_ZeroCfeCoreTempValues(void)
{
    CS_Res_EepromMemory_Table_Entry_t *CfeCoreCodeSeg = CS_GetCfeCoreCodeSegResTable();

    if (CfeCoreCodeSeg != NULL)
    {
        CfeCoreCodeSeg->TempChecksumValue = 0;
        CfeCoreCodeSeg->ByteOffset        = 0;
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Zero out the temp chcksum values of the OS code segment      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_ZeroOSTempValues(void)
{
    CS_Res_EepromMemory_Table_Entry_t *OSCodeSeg = CS_GetOSCodeSegResTable();

    if (OSCodeSeg != NULL)
    {
        OSCodeSeg->TempChecksumValue = 0;
        OSCodeSeg->ByteOffset        = 0;
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Nullifies the entries in the Results table for All           */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_InitializeDefaultTables(void)
{
    uint16 Loop;

    for (Loop = 0; Loop < CS_MAX_NUM_EEPROM_TABLE_ENTRIES; Loop++)
    {
        CS_AppData.DefaultEepromDefTable[Loop].State              = CS_ChecksumState_EMPTY;
        CS_AppData.DefaultEepromDefTable[Loop].NumBytesToChecksum = 0;
        CS_AppData.DefaultEepromDefTable[Loop].StartAddress       = 0;
    }

    for (Loop = 0; Loop < CS_MAX_NUM_MEMORY_TABLE_ENTRIES; Loop++)
    {
        CS_AppData.DefaultMemoryDefTable[Loop].State              = CS_ChecksumState_EMPTY;
        CS_AppData.DefaultMemoryDefTable[Loop].NumBytesToChecksum = 0;
        CS_AppData.DefaultMemoryDefTable[Loop].StartAddress       = 0;
    }

    for (Loop = 0; Loop < CS_MAX_NUM_APP_TABLE_ENTRIES; Loop++)
    {
        CS_AppData.DefaultAppDefTable[Loop].State   = CS_ChecksumState_EMPTY;
        CS_AppData.DefaultAppDefTable[Loop].Name[0] = '\0';
    }

    for (Loop = 0; Loop < CS_MAX_NUM_TABLES_TABLE_ENTRIES; Loop++)
    {
        CS_AppData.DefaultTablesDefTable[Loop].State   = CS_ChecksumState_EMPTY;
        CS_AppData.DefaultTablesDefTable[Loop].Name[0] = '\0';
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Set the global variables to the start of the next table      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_GoToNextTable(void)
{
    if (CS_AppData.HkPacket.Payload.CurrentCSTable < (CS_NUM_TABLES - 1))
    {
        CS_AppData.HkPacket.Payload.CurrentCSTable++;
    }
    else
    {
        CS_AppData.HkPacket.Payload.CurrentCSTable = 0;
        /* we are back to the beginning of the tables to checksum
         we need to update the pass counter */
        CS_AppData.HkPacket.Payload.PassCounter++;
    }

    CS_AppData.HkPacket.Payload.CurrentEntryInTable = 0;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Get the Results Table Entry info of a table by its name      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
bool CS_GetTableResTblEntryByName(CS_Res_Tables_Table_Entry_t **EntryPtr, const char *Name)
{
    CS_Res_Tables_Table_Entry_t *ResultsEntry = NULL;
    int32                        Loop;
    bool                         Status = false;

    Loop = 0;
    while (true)
    {
        ResultsEntry = CS_GetTablesResEntry(Loop);
        if (ResultsEntry == NULL)
        {
            break;
        }

        if (ResultsEntry->State != CS_ChecksumState_EMPTY
            && strncmp(ResultsEntry->Name, Name, CFE_TBL_MAX_FULL_NAME_LEN) == 0)
        {
            Status    = true;
            *EntryPtr = ResultsEntry;
            break;
        }

        ++Loop;
    }
    return Status;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Get the Definition Table Entry info of a table by its name      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
bool CS_GetTableDefTblEntryByName(CS_Def_Tables_Table_Entry_t **EntryPtr, const char *Name)
{
    CS_Def_Tables_Table_Entry_t *DefinitionEntry = NULL;
    int32                        Loop;
    bool                         Status = false;

    Loop = 0;
    while (true)
    {
        DefinitionEntry = CS_GetTablesDefEntry(Loop);
        if (DefinitionEntry == NULL)
        {
            break;
        }

        if (CS_StateValid(DefinitionEntry->State)
            && strncmp(DefinitionEntry->Name, Name, CFE_TBL_MAX_FULL_NAME_LEN) == 0)
        {
            Status    = true;
            *EntryPtr = DefinitionEntry;
            break;
        }

        ++Loop;
    }
    return Status;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Get the Results Entry info of an app by its name             */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
bool CS_GetAppResTblEntryByName(CS_Res_App_Table_Entry_t **EntryPtr, const char *Name)
{
    CS_Res_App_Table_Entry_t *ResultsEntry = NULL;
    int32                     Loop;
    bool                      Status = false;

    Loop = 0;
    while (true)
    {
        ResultsEntry = CS_GetAppResEntry(Loop);
        if (ResultsEntry == NULL)
        {
            break;
        }

        if (ResultsEntry->State != CS_ChecksumState_EMPTY && strncmp(ResultsEntry->Name, Name, OS_MAX_API_NAME) == 0)
        {
            Status    = true;
            *EntryPtr = ResultsEntry;
            break;
        }

        ++Loop;
    }
    return Status;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Get the Definition Entry info of an app by its name          */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
bool CS_GetAppDefTblEntryByName(CS_Def_App_Table_Entry_t **EntryPtr, const char *Name)
{
    CS_Def_App_Table_Entry_t *DefinitionEntry = NULL;
    int32                     Loop;
    bool                      Status = false;

    Loop = 0;
    while (true)
    {
        DefinitionEntry = CS_GetAppDefEntry(Loop);
        if (DefinitionEntry == NULL)
        {
            break;
        }

        if (CS_StateValid(DefinitionEntry->State) && strncmp(DefinitionEntry->Name, Name, OS_MAX_API_NAME) == 0)
        {
            Status    = true;
            *EntryPtr = DefinitionEntry;
            break;
        }

        ++Loop;
    }
    return Status;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Get the next CS-enabled entry of this table                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CS_Res_EepromMemory_Table_Entry_t *CS_FindEnabledEepromEntry(void)
{
    CS_Res_EepromMemory_Table_Entry_t *ResultsEntry = NULL;

    while (true)
    {
        ResultsEntry = CS_GetEepromResEntry(CS_AppData.HkPacket.Payload.CurrentEntryInTable);
        if (ResultsEntry == NULL || ResultsEntry->State == CS_ChecksumState_ENABLED)
        {
            break;
        }

        CS_AppData.HkPacket.Payload.CurrentEntryInTable++;
    } /* end while */

    return ResultsEntry;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Get the next CS-enabled entry of this table                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CS_Res_EepromMemory_Table_Entry_t *CS_FindEnabledMemoryEntry(void)
{
    CS_Res_EepromMemory_Table_Entry_t *ResultsEntry = NULL;

    while (true)
    {
        ResultsEntry = CS_GetMemoryResEntry(CS_AppData.HkPacket.Payload.CurrentEntryInTable);
        if (ResultsEntry == NULL || ResultsEntry->State == CS_ChecksumState_ENABLED)
        {
            break;
        }

        CS_AppData.HkPacket.Payload.CurrentEntryInTable++;
    } /* end while */

    return ResultsEntry;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Get the next CS-enabled entry of this table                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CS_Res_Tables_Table_Entry_t *CS_FindEnabledTablesEntry(void)
{
    CS_Res_Tables_Table_Entry_t *ResultsEntry = NULL;

    while (true)
    {
        ResultsEntry = CS_GetTablesResEntry(CS_AppData.HkPacket.Payload.CurrentEntryInTable);
        if (ResultsEntry == NULL || ResultsEntry->State == CS_ChecksumState_ENABLED)
        {
            break;
        }

        CS_AppData.HkPacket.Payload.CurrentEntryInTable++;
    } /* end while */

    return ResultsEntry;
}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Get the next CS-enabled entry of this table                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CS_Res_App_Table_Entry_t *CS_FindEnabledAppEntry(void)
{
    CS_Res_App_Table_Entry_t *ResultsEntry = NULL;

    while (true)
    {
        ResultsEntry = CS_GetAppResEntry(CS_AppData.HkPacket.Payload.CurrentEntryInTable);
        if (ResultsEntry == NULL || ResultsEntry->State == CS_ChecksumState_ENABLED)
        {
            break;
        }

        CS_AppData.HkPacket.Payload.CurrentEntryInTable++;
    } /* end while */

    return ResultsEntry;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Background check cFE core                                       */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
bool CS_BackgroundCfeCore(void)
{
    CS_Res_EepromMemory_Table_Entry_t *ResultsEntry;
    bool                               DoneWithCycle   = false;
    bool                               DoneWithEntry   = false;
    uint32                             ComputedCSValue = 0;
    CFE_Status_t                       Status;

    if (CS_AppData.HkPacket.Payload.CfeCoreCSState == CS_ChecksumState_ENABLED)
    {
        ResultsEntry = CS_GetCfeCoreCodeSegResTable();

        /* Check if the code segment state is enabled (could be disabled for
           a ground-commanded recompute) */
        if (ResultsEntry->State == CS_ChecksumState_ENABLED)
        {
            /* If we complete an entry's checksum, this function will update it for us */
            Status = CS_ComputeEepromMemory(ResultsEntry, &ComputedCSValue, &DoneWithEntry);

            /* We need to avoid the case of finishing a table, moving on to the next one
             and computing an entry in that table, since it could put us above the
             maximum bytes per cycle */
            DoneWithCycle = true;

            if (Status == CS_ERROR)
            {
                /* we had a miscompare */

                CS_AppData.HkPacket.Payload.CfeCoreCSErrCounter++;

                CFE_EVS_SendEvent(CS_CFECORE_MISCOMPARE_ERR_EID,
                                  CFE_EVS_EventType_ERROR,
                                  "Checksum Failure: cFE Core, Expected: 0x%08X, Calculated: 0x%08X",
                                  (unsigned int)(ResultsEntry->ComparisonValue),
                                  (unsigned int)ComputedCSValue);
            }

            if (DoneWithEntry == true)
            {
                CS_AppData.HkPacket.Payload.CurrentEntryInTable++;
            }

            /* only one entry to do */
            if (CS_AppData.HkPacket.Payload.CurrentEntryInTable > 0)
            {
                /* We are done with this table */
                CS_AppData.HkPacket.Payload.CfeCoreBaseline = ResultsEntry->ComparisonValue;
                CS_GoToNextTable();
            }
        }
        else
        {
            /* Currently disabled, so skip it */
            CS_GoToNextTable();
        }
    }
    else /* Table is disabled, skip it */
    {
        CS_GoToNextTable();
    }

    return DoneWithCycle;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Background check OS                                             */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
bool CS_BackgroundOS(void)
{
    CS_Res_EepromMemory_Table_Entry_t *ResultsEntry;
    bool                               DoneWithCycle   = false;
    bool                               DoneWithEntry   = false;
    uint32                             ComputedCSValue = 0;
    CFE_Status_t                       Status;

    if (CS_AppData.HkPacket.Payload.OSCSState == CS_ChecksumState_ENABLED)
    {
        ResultsEntry = CS_GetOSCodeSegResTable();

        /* Check if the code segment state is enabled (could be disabled for
         a ground-commanded recompute) */
        if (ResultsEntry->State == CS_ChecksumState_ENABLED)
        {
            /* If we complete an entry's checksum, this function will update it for us */
            Status = CS_ComputeEepromMemory(ResultsEntry, &ComputedCSValue, &DoneWithEntry);

            /* We need to avoid the case of finishing a table, moving on to the next one
             and computing an entry in that table, since it could put us above the
             maximum bytes per cycle */
            DoneWithCycle = true;

            if (Status == CS_ERROR)
            {
                /* we had a miscompare */
                CS_AppData.HkPacket.Payload.OSCSErrCounter++;

                CFE_EVS_SendEvent(CS_OS_MISCOMPARE_ERR_EID,
                                  CFE_EVS_EventType_ERROR,
                                  "Checksum Failure: OS code segment, Expected: 0x%08X, Calculated: 0x%08X",
                                  (unsigned int)(ResultsEntry->ComparisonValue),
                                  (unsigned int)ComputedCSValue);
            }

            if (DoneWithEntry == true)
            {
                CS_AppData.HkPacket.Payload.CurrentEntryInTable++;
            }

            /* only one entry to do */
            if (CS_AppData.HkPacket.Payload.CurrentEntryInTable > 0)
            {
                /* We are done with this table */
                CS_AppData.HkPacket.Payload.OSBaseline = ResultsEntry->ComparisonValue;
                CS_GoToNextTable();
            }
        }
        else
        {
            /* Currently disabled, so skip it */
            CS_GoToNextTable();
        }
    }
    else /* Table is disabled, skip it */
    {
        CS_GoToNextTable();
    }
    return DoneWithCycle;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Background check EEPROM                                         */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
bool CS_BackgroundEeprom(void)
{
    CS_Res_EepromMemory_Table_Entry_t *ResultsEntry;
    CS_Res_EepromMemory_Table_Entry_t *LoopEntry;
    bool                               DoneWithCycle   = false;
    bool                               DoneWithEntry   = false;
    uint32                             ComputedCSValue = 0;
    int32                              Loop;
    uint32                             EntireEepromCS;
    CFE_Status_t                       Status;

    if (CS_AppData.HkPacket.Payload.EepromCSState == CS_ChecksumState_ENABLED)
    {
        ResultsEntry = CS_FindEnabledEepromEntry();
    }
    else
    {
        ResultsEntry = NULL;
    }

    if (ResultsEntry != NULL)
    {
        /* we found an enabled entry to checksum */
        /* If we complete an entry's checksum, this function will update it for us */
        Status = CS_ComputeEepromMemory(ResultsEntry, &ComputedCSValue, &DoneWithEntry);

        /* We need to avoid the case of finishing a table, moving on to the next one
            and computing an entry in that table, since it could put us above the
            maximum bytes per cycle */
        DoneWithCycle = true;

        if (Status == CS_ERROR)
        {
            /* we had a miscompare */

            CS_AppData.HkPacket.Payload.EepromCSErrCounter++;

            CFE_EVS_SendEvent(CS_EEPROM_MISCOMPARE_ERR_EID,
                              CFE_EVS_EventType_ERROR,
                              "Checksum Failure: Address 0x%lx in EEPROM Table, Expected: 0x%08X, Calculated: 0x%08X",
                              (unsigned long)ResultsEntry->StartAddress,
                              (unsigned int)(ResultsEntry->ComparisonValue),
                              (unsigned int)ComputedCSValue);
        }

        if (DoneWithEntry)
        {
            CS_AppData.HkPacket.Payload.CurrentEntryInTable++;
        }
    }

    if (CS_AppData.HkPacket.Payload.CurrentEntryInTable >= CS_MAX_NUM_EEPROM_TABLE_ENTRIES)
    {
        /* Since we are done CS'ing the entire EEPROM table, update the baseline
            number for telemetry */
        EntireEepromCS = 0;
        Loop           = 0;
        while (true)
        {
            LoopEntry = CS_GetEepromResEntry(Loop);
            if (LoopEntry == NULL)
            {
                break;
            }

            EntireEepromCS += LoopEntry->ComparisonValue;
            ++Loop;
        }

        CS_AppData.HkPacket.Payload.EepromBaseline = EntireEepromCS;
    }

    if (ResultsEntry == NULL || CS_AppData.HkPacket.Payload.CurrentEntryInTable >= CS_MAX_NUM_EEPROM_TABLE_ENTRIES)
    {
        /* We are done with this table */
        CS_GoToNextTable();
    }

    return DoneWithCycle;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Background check Memory                                         */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
bool CS_BackgroundMemory(void)
{
    CS_Res_EepromMemory_Table_Entry_t *ResultsEntry;
    bool                               DoneWithCycle   = false;
    bool                               DoneWithEntry   = false;
    uint32                             ComputedCSValue = 0;
    CFE_Status_t                       Status;

    if (CS_AppData.HkPacket.Payload.MemoryCSState == CS_ChecksumState_ENABLED)
    {
        /* If we complete an entry's checksum, this function will update it for us */
        ResultsEntry = CS_FindEnabledMemoryEntry();
    }
    else
    {
        ResultsEntry = NULL;
    }

    if (ResultsEntry != NULL)
    {
        /* we found an enabled entry to checksum */
        /* If we complete an entry's checksum, this function will update it for us */
        Status = CS_ComputeEepromMemory(ResultsEntry, &ComputedCSValue, &DoneWithEntry);

        /* We need to avoid the case of finishing a table, moving on to the next one
            and computing an entry in that table, since it could put us above the
            maximum bytes per cycle */
        DoneWithCycle = true;

        if (Status == CS_ERROR)
        {
            /* we had a miscompare */

            CS_AppData.HkPacket.Payload.MemoryCSErrCounter++;

            CFE_EVS_SendEvent(CS_MEMORY_MISCOMPARE_ERR_EID,
                              CFE_EVS_EventType_ERROR,
                              "Checksum Failure: Address 0x%lx in Memory Table, Expected: 0x%08X, Calculated: 0x%08X",
                              (unsigned long)ResultsEntry->StartAddress,
                              (unsigned int)(ResultsEntry->ComparisonValue),
                              (unsigned int)ComputedCSValue);
        }

        if (DoneWithEntry)
        {
            CS_AppData.HkPacket.Payload.CurrentEntryInTable++;
        }
    }

    if (ResultsEntry == NULL || CS_AppData.HkPacket.Payload.CurrentEntryInTable >= CS_MAX_NUM_MEMORY_TABLE_ENTRIES)
    {
        /* We are done with this table */
        CS_GoToNextTable();
    }

    return DoneWithCycle;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Background check Tables                                         */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
bool CS_BackgroundTables(void)
{
    CS_Res_Tables_Table_Entry_t *TablesResultsEntry;
    bool                         DoneWithCycle   = false;
    bool                         DoneWithEntry   = false;
    uint32                       ComputedCSValue = 0;
    CFE_Status_t                 Status;

    if (CS_AppData.HkPacket.Payload.TablesCSState == CS_ChecksumState_ENABLED)
    {
        /* If we complete an entry's checksum, this function will update it for us */
        TablesResultsEntry = CS_FindEnabledTablesEntry();
    }
    else
    {
        TablesResultsEntry = NULL;
    }

    if (TablesResultsEntry != NULL)
    {
        /* we found an enabled entry to checksum */
        Status = CS_ComputeTables(TablesResultsEntry, &ComputedCSValue, &DoneWithEntry);

        /* We need to avoid the case of finishing a table, moving on to the next one
            and computing an entry in that table, since  it could put us above the
            maximum bytes per cycle */
        DoneWithCycle = true;

        if (Status == CS_ERROR)
        {
            /* we had a miscompare */
            CS_AppData.HkPacket.Payload.TablesCSErrCounter++;

            CFE_EVS_SendEvent(CS_TABLES_MISCOMPARE_ERR_EID,
                              CFE_EVS_EventType_ERROR,
                              "Checksum Failure: Table %s, Expected: 0x%08X, Calculated: 0x%08X",
                              TablesResultsEntry->Name,
                              (unsigned int)(TablesResultsEntry->ComparisonValue),
                              (unsigned int)ComputedCSValue);
        }

        if (Status == CS_ERR_NOT_FOUND)
        {
            CFE_EVS_SendEvent(CS_COMPUTE_TABLES_NOT_FOUND_ERR_EID,
                              CFE_EVS_EventType_ERROR,
                              "Tables table computing: Table %s could not be found, skipping",
                              TablesResultsEntry->Name);

            DoneWithEntry = true;
        }

        if (DoneWithEntry)
        {
            CS_AppData.HkPacket.Payload.CurrentEntryInTable++;
        }
    }

    if (TablesResultsEntry == NULL
        || CS_AppData.HkPacket.Payload.CurrentEntryInTable >= CS_MAX_NUM_TABLES_TABLE_ENTRIES)
    {
        /* We are done with this table */
        CS_GoToNextTable();
    }

    return DoneWithCycle;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Background check App                                            */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
bool CS_BackgroundApp(void)
{
    CS_Res_App_Table_Entry_t *AppResultsEntry;
    bool                      DoneWithCycle   = false;
    bool                      DoneWithEntry   = false;
    uint32                    ComputedCSValue = 0;
    CFE_Status_t              Status;

    if (CS_AppData.HkPacket.Payload.AppCSState == CS_ChecksumState_ENABLED)
    {
        AppResultsEntry = CS_FindEnabledAppEntry();
    }
    else
    {
        AppResultsEntry = NULL;
    }

    if (AppResultsEntry != NULL)
    {
        /* we found an enabled entry to checksum */
        /* If we complete an entry's checksum, this function will update it for us */
        Status = CS_ComputeApp(AppResultsEntry, &ComputedCSValue, &DoneWithEntry);

        /* We need to avoid the case of finishing a table, moving on to the next one
            and computing an entry in that table, since it could put us above the
            maximum bytes per cycle */
        DoneWithCycle = true;

        if (Status == CS_ERROR)
        {
            /* we had a miscompare */
            CS_AppData.HkPacket.Payload.AppCSErrCounter++;

            CFE_EVS_SendEvent(CS_APP_MISCOMPARE_ERR_EID,
                              CFE_EVS_EventType_ERROR,
                              "Checksum Failure: Application %s, Expected: 0x%08X, Calculated: 0x%08X",
                              AppResultsEntry->Name,
                              (unsigned int)(AppResultsEntry->ComparisonValue),
                              (unsigned int)ComputedCSValue);
        }

        if (Status == CS_ERR_NOT_FOUND)
        {
            CFE_EVS_SendEvent(CS_COMPUTE_APP_NOT_FOUND_ERR_EID,
                              CFE_EVS_EventType_ERROR,
                              "App table computing: App %s could not be found, skipping",
                              AppResultsEntry->Name);

            DoneWithEntry = true;
        }

        if (DoneWithEntry)
        {
            CS_AppData.HkPacket.Payload.CurrentEntryInTable++;
        }
    }

    if (AppResultsEntry == NULL || CS_AppData.HkPacket.Payload.CurrentEntryInTable >= CS_MAX_NUM_APP_TABLE_ENTRIES)
    {
        /* We are done with this table */
        CS_GoToNextTable();
    }

    return DoneWithCycle;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Reset Tables result table entry after a CS table gets modified  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_ResetTablesTblResultEntry(CS_Res_Tables_Table_Entry_t *TablesTblResultEntry)
{
    /* Ptr will be NULL if this CS table is not listed in the CS Tables table */
    if (TablesTblResultEntry != NULL)
    {
        TablesTblResultEntry->ByteOffset        = 0;
        TablesTblResultEntry->TempChecksumValue = 0;
        TablesTblResultEntry->ComputedYet       = false;
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Update all tables                                               */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CFE_Status_t CS_HandleRoutineTableUpdates(void)
{
    CFE_Status_t       Result = CFE_SUCCESS;
    uint16             TableId;
    CS_TableWrapper_t *tw;
    bool               ShouldProcess;

    static const uint16 EventIdMap[CS_NUM_TABLES] = {
        [CS_ChecksumType_EEPROM_TABLE] = CS_UPDATE_EEPROM_ERR_EID,
        [CS_ChecksumType_MEMORY_TABLE] = CS_UPDATE_MEMORY_ERR_EID,
        [CS_ChecksumType_TABLES_TABLE] = CS_UPDATE_TABLES_ERR_EID,
        [CS_ChecksumType_APP_TABLE]    = CS_UPDATE_APP_ERR_EID,
    };

    for (TableId = 0; TableId < CS_NUM_TABLES; ++TableId)
    {
        tw = &CS_AppData.Tbl[TableId];

        if (tw->DefEntrySize == 0)
        {
            /* There is no table to update */
            ShouldProcess = false;
        }
        else if (!CS_AppData.HkPacket.Payload.RecomputeInProgress || CS_AppData.HkPacket.Payload.OneShotInProgress)
        {
            /* Always update */
            ShouldProcess = true;
        }
        else
        {
            /* Do not process if the child task owns this table */
            ShouldProcess = (CS_AppData.ChildTaskTable != TableId);
        }

        if (ShouldProcess)
        {
            Result = CS_HandleTableUpdate(tw);
        }
        else
        {
            Result = CFE_SUCCESS;
        }

        if (Result != CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(EventIdMap[TableId],
                              CFE_EVS_EventType_ERROR,
                              "Table update failed for %s: 0x%08X, checksumming is disabled",
                              CS_GetTableTypeAsString(tw),
                              (unsigned int)Result);

            if (tw->GlobalState)
            {
                *tw->GlobalState = CS_ChecksumState_DISABLED;
            }
        }
    }

    return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Check if one shot recompute is already in progress              */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
bool CS_CheckRecomputeOneshot(void)
{
    bool Result = false;

    if (CS_AppData.HkPacket.Payload.RecomputeInProgress == true
        || CS_AppData.HkPacket.Payload.OneShotInProgress == true)
    {
        CFE_EVS_SendEvent(CS_CMD_COMPUTE_PROG_ERR_EID,
                          CFE_EVS_EventType_ERROR,
                          "Cannot perform command. Recompute or oneshot in progress.");

        CS_AppData.HkPacket.Payload.CommandErrorCounter++;

        Result = true;
    }
    return Result;
}
