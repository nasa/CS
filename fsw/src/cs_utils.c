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
 *   The CFS Checksum (CS) Application's utility functions
 */

/**************************************************************************
 **
 ** Include section
 **
 **************************************************************************/
#include "cfe.h"
#include "cs_app.h"
#include "cs_events.h"
#include "cs_compute.h"
#include "cs_utils.h"
#include <string.h>

/**************************************************************************
 **
 ** Functions
 **
 **************************************************************************/
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Zero out the temp chcksum values of Eeprom                   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_ZeroEepromTempValues(void)
{
    uint16 Loop;

    for (Loop = 0; Loop < CS_MAX_NUM_EEPROM_TABLE_ENTRIES; Loop++)
    {
        CS_AppData.ResEepromTblPtr[Loop].TempChecksumValue = 0;
        CS_AppData.ResEepromTblPtr[Loop].ByteOffset        = 0;
    }
    return;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Zero out the temp chcksum values of Memory                   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_ZeroMemoryTempValues(void)
{
    uint16 Loop;

    for (Loop = 0; Loop < CS_MAX_NUM_MEMORY_TABLE_ENTRIES; Loop++)
    {
        CS_AppData.ResMemoryTblPtr[Loop].TempChecksumValue = 0;
        CS_AppData.ResMemoryTblPtr[Loop].ByteOffset        = 0;
    }

    return;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Zero out the temp chcksum values of Tables                   */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_ZeroTablesTempValues(void)
{
    uint16 Loop;

    for (Loop = 0; Loop < CS_MAX_NUM_TABLES_TABLE_ENTRIES; Loop++)
    {
        CS_AppData.ResTablesTblPtr[Loop].TempChecksumValue = 0;
        CS_AppData.ResTablesTblPtr[Loop].ByteOffset        = 0;
    }

    return;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Zero out the temp chcksum values of Applications             */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_ZeroAppTempValues(void)
{
    uint16 Loop;

    for (Loop = 0; Loop < CS_MAX_NUM_APP_TABLE_ENTRIES; Loop++)
    {
        CS_AppData.ResAppTblPtr[Loop].TempChecksumValue = 0;
        CS_AppData.ResAppTblPtr[Loop].ByteOffset        = 0;
    }

    return;
}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Zero out the temp chcksum values ofthe cFE core              */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_ZeroCfeCoreTempValues(void)
{
    CS_AppData.CfeCoreCodeSeg.TempChecksumValue = 0;
    CS_AppData.CfeCoreCodeSeg.ByteOffset        = 0;

    return;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Zero out the temp chcksum values of the OS code segment      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_ZeroOSTempValues(void)
{
    CS_AppData.OSCodeSeg.TempChecksumValue = 0;
    CS_AppData.OSCodeSeg.ByteOffset        = 0;

    return;
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
        CS_AppData.DefaultEepromDefTable[Loop].State              = CS_STATE_EMPTY;
        CS_AppData.DefaultEepromDefTable[Loop].NumBytesToChecksum = 0;
        CS_AppData.DefaultEepromDefTable[Loop].StartAddress       = 0;
    }

    for (Loop = 0; Loop < CS_MAX_NUM_MEMORY_TABLE_ENTRIES; Loop++)
    {
        CS_AppData.DefaultMemoryDefTable[Loop].State              = CS_STATE_EMPTY;
        CS_AppData.DefaultMemoryDefTable[Loop].NumBytesToChecksum = 0;
        CS_AppData.DefaultMemoryDefTable[Loop].StartAddress       = 0;
    }

    for (Loop = 0; Loop < CS_MAX_NUM_APP_TABLE_ENTRIES; Loop++)
    {
        CS_AppData.DefaultAppDefTable[Loop].State   = CS_STATE_EMPTY;
        CS_AppData.DefaultAppDefTable[Loop].Name[0] = '\0';
    }

    for (Loop = 0; Loop < CS_MAX_NUM_TABLES_TABLE_ENTRIES; Loop++)
    {
        CS_AppData.DefaultTablesDefTable[Loop].State   = CS_STATE_EMPTY;
        CS_AppData.DefaultTablesDefTable[Loop].Name[0] = '\0';
    }

    return;
} /* end CS_InitializeDefaultTables */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Set the global variables to the start of the next table      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_GoToNextTable(void)
{
    if (CS_AppData.HkPacket.CurrentCSTable < (CS_NUM_TABLES - 1))
    {
        CS_AppData.HkPacket.CurrentCSTable++;
    }
    else
    {
        CS_AppData.HkPacket.CurrentCSTable = 0;
        /* we are back to the beginning of the tables to checksum
         we need to update the pass counter */
        CS_AppData.HkPacket.PassCounter++;
    }

    CS_AppData.HkPacket.CurrentEntryInTable = 0;
    return;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Get the Results Table Entry info of a table by its name      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
bool CS_GetTableResTblEntryByName(CS_Res_Tables_Table_Entry_t **EntryPtr, const char *Name)
{
    CS_Res_Tables_Table_Entry_t *StartOfResultsTable = NULL;
    CS_Res_Tables_Table_Entry_t *ResultsEntry        = NULL;
    int32                        Loop;
    uint16                       MaxEntries;
    bool                         Status = false;

    StartOfResultsTable = CS_AppData.ResTablesTblPtr;
    MaxEntries          = CS_MAX_NUM_TABLES_TABLE_ENTRIES;

    for (Loop = 0; Loop < MaxEntries; Loop++)
    {
        ResultsEntry = &StartOfResultsTable[Loop];

        if (strncmp(ResultsEntry->Name, Name, CFE_TBL_MAX_FULL_NAME_LEN) == 0 && ResultsEntry->State != CS_STATE_EMPTY)
        {
            Status    = true;
            *EntryPtr = ResultsEntry;
            break;
        }
    }
    return Status;
} /* end CS_GetTableResTblEntryByName */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Get the Defintion Table Entry info of a table by its name      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
bool CS_GetTableDefTblEntryByName(CS_Def_Tables_Table_Entry_t **EntryPtr, const char *Name)
{
    CS_Def_Tables_Table_Entry_t *StartOfDefinitionTable = NULL;
    CS_Def_Tables_Table_Entry_t *DefinitionEntry        = NULL;
    int32                        Loop;
    uint16                       MaxEntries;
    bool                         Status = false;

    StartOfDefinitionTable = CS_AppData.DefTablesTblPtr;
    MaxEntries             = CS_MAX_NUM_TABLES_TABLE_ENTRIES;

    for (Loop = 0; Loop < MaxEntries; Loop++)
    {
        DefinitionEntry = &StartOfDefinitionTable[Loop];

        if (strncmp(DefinitionEntry->Name, Name, CFE_TBL_MAX_FULL_NAME_LEN) == 0 &&
            DefinitionEntry->State != CS_STATE_EMPTY)
        {
            Status    = true;
            *EntryPtr = DefinitionEntry;
            break;
        }
    }
    return Status;
} /* end CS_GetTableDefTblEntryByName */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Get the Results Entry info of an app by its name             */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
bool CS_GetAppResTblEntryByName(CS_Res_App_Table_Entry_t **EntryPtr, const char *Name)
{
    CS_Res_App_Table_Entry_t *StartOfResultsTable = NULL;
    CS_Res_App_Table_Entry_t *ResultsEntry        = NULL;
    int32                     Loop;
    uint16                    MaxEntries;
    bool                      Status = false;

    StartOfResultsTable = CS_AppData.ResAppTblPtr;
    MaxEntries          = CS_MAX_NUM_APP_TABLE_ENTRIES;

    for (Loop = 0; Loop < MaxEntries; Loop++)
    {
        ResultsEntry = &StartOfResultsTable[Loop];

        if (strncmp(ResultsEntry->Name, Name, OS_MAX_API_NAME) == 0 && ResultsEntry->State != CS_STATE_EMPTY)
        {
            Status    = true;
            *EntryPtr = ResultsEntry;
            break;
        }
    }
    return Status;
} /* end CS_GetAppResTblEntryByName */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Get the Definition Entry info of an app by its name          */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
bool CS_GetAppDefTblEntryByName(CS_Def_App_Table_Entry_t **EntryPtr, const char *Name)
{
    CS_Def_App_Table_Entry_t *StartOfDefinitionTable = NULL;
    CS_Def_App_Table_Entry_t *DefinitionEntry        = NULL;
    int32                     Loop;
    uint16                    MaxEntries;
    bool                      Status = false;

    StartOfDefinitionTable = CS_AppData.DefAppTblPtr;
    MaxEntries             = CS_MAX_NUM_APP_TABLE_ENTRIES;

    for (Loop = 0; Loop < MaxEntries; Loop++)
    {
        DefinitionEntry = &StartOfDefinitionTable[Loop];

        if (strncmp(DefinitionEntry->Name, Name, OS_MAX_API_NAME) == 0 && DefinitionEntry->State != CS_STATE_EMPTY)
        {
            Status    = true;
            *EntryPtr = DefinitionEntry;
            break;
        }
    }
    return Status;
} /* end CS_GetAppDefTblEntryByName */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Get the next CS-enabled entry of this table                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
bool CS_FindEnabledEepromEntry(uint16 *EnabledEntry)
{
    CS_Res_EepromMemory_Table_Entry_t *StartOfResultsTable = NULL;
    CS_Res_EepromMemory_Table_Entry_t *ResultsEntry        = NULL;
    bool                               EnabledEntries      = true;

    StartOfResultsTable = CS_AppData.ResEepromTblPtr;

    ResultsEntry = &StartOfResultsTable[CS_AppData.HkPacket.CurrentEntryInTable];

    while (ResultsEntry->State != CS_STATE_ENABLED)
    {
        CS_AppData.HkPacket.CurrentEntryInTable++;

        if (CS_AppData.HkPacket.CurrentEntryInTable >= CS_MAX_NUM_EEPROM_TABLE_ENTRIES)
        {
            /* we reached the end no more enabled entries */
            EnabledEntries = false;
            break;
        }

        ResultsEntry = &StartOfResultsTable[CS_AppData.HkPacket.CurrentEntryInTable];
    } /* end while */

    *EnabledEntry = CS_AppData.HkPacket.CurrentEntryInTable;

    return EnabledEntries;
} /* end CS FindEnabledEepromEntry */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Get the next CS-enabled entry of this table                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
bool CS_FindEnabledMemoryEntry(uint16 *EnabledEntry)
{
    CS_Res_EepromMemory_Table_Entry_t *StartOfResultsTable = NULL;
    CS_Res_EepromMemory_Table_Entry_t *ResultsEntry        = NULL;
    bool                               EnabledEntries      = true;

    StartOfResultsTable = CS_AppData.ResMemoryTblPtr;
    ResultsEntry        = &StartOfResultsTable[CS_AppData.HkPacket.CurrentEntryInTable];

    while (ResultsEntry->State != CS_STATE_ENABLED)
    {
        CS_AppData.HkPacket.CurrentEntryInTable++;

        if (CS_AppData.HkPacket.CurrentEntryInTable >= CS_MAX_NUM_MEMORY_TABLE_ENTRIES)
        {
            /* we reached the end no more enabled entries */
            EnabledEntries = false;
            break;
        }

        ResultsEntry = &StartOfResultsTable[CS_AppData.HkPacket.CurrentEntryInTable];
    } /* end while */

    *EnabledEntry = CS_AppData.HkPacket.CurrentEntryInTable;

    return EnabledEntries;
} /* end CS FindEnabledMemoryEntry */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Get the next CS-enabled entry of this table                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
bool CS_FindEnabledTablesEntry(uint16 *EnabledEntry)
{
    CS_Res_Tables_Table_Entry_t *StartOfResultsTable = NULL;
    CS_Res_Tables_Table_Entry_t *ResultsEntry        = NULL;
    bool                         EnabledEntries      = true;

    StartOfResultsTable = CS_AppData.ResTablesTblPtr;
    ResultsEntry        = &StartOfResultsTable[CS_AppData.HkPacket.CurrentEntryInTable];

    while (ResultsEntry->State != CS_STATE_ENABLED)
    {
        CS_AppData.HkPacket.CurrentEntryInTable++;

        if (CS_AppData.HkPacket.CurrentEntryInTable >= CS_MAX_NUM_TABLES_TABLE_ENTRIES)
        {
            /* we reached the end no more enabled entries */
            EnabledEntries = false;

            break;
        }

        ResultsEntry = &StartOfResultsTable[CS_AppData.HkPacket.CurrentEntryInTable];

    } /* end while */

    *EnabledEntry = CS_AppData.HkPacket.CurrentEntryInTable;

    return EnabledEntries;
} /* end CS FindEnabledTablesEntry */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Get the next CS-enabled entry of this table                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
bool CS_FindEnabledAppEntry(uint16 *EnabledEntry)
{
    CS_Res_App_Table_Entry_t *StartOfResultsTable = NULL;
    CS_Res_App_Table_Entry_t *ResultsEntry        = NULL;
    bool                      EnabledEntries      = true;

    StartOfResultsTable = CS_AppData.ResAppTblPtr;
    ResultsEntry        = &StartOfResultsTable[CS_AppData.HkPacket.CurrentEntryInTable];

    while (ResultsEntry->State != CS_STATE_ENABLED)
    {
        CS_AppData.HkPacket.CurrentEntryInTable++;

        if (CS_AppData.HkPacket.CurrentEntryInTable >= CS_MAX_NUM_APP_TABLE_ENTRIES)
        {
            /* we reached the end no more enabled entries */
            EnabledEntries = false;
            break;
        }

        ResultsEntry = &StartOfResultsTable[CS_AppData.HkPacket.CurrentEntryInTable];

    } /* end while */

    *EnabledEntry = CS_AppData.HkPacket.CurrentEntryInTable;

    return EnabledEntries;
} /* end CS FindEnabledAppEntry */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Verify the length of the command                             */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
bool CS_VerifyCmdLength(const CFE_MSG_Message_t *msg, size_t ExpectedLength)
{
    CFE_SB_MsgId_t    MessageID    = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_FcnCode_t CommandCode  = 0;
    bool              Result       = true;
    size_t            ActualLength = 0;

    CFE_MSG_GetSize(msg, &ActualLength);

    /* Verify the command packet length */
    if (ExpectedLength != ActualLength)
    {

        CFE_MSG_GetMsgId(msg, &MessageID);
        CFE_MSG_GetFcnCode(msg, &CommandCode);

        CFE_EVS_SendEvent(CS_LEN_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Invalid msg length: ID = 0x%08lX, CC = %d, Len = %lu, Expected = %lu",
                          (unsigned long)CFE_SB_MsgIdToValue(MessageID), CommandCode, (unsigned long)ActualLength,
                          (unsigned long)ExpectedLength);
        Result = false;
        CS_AppData.HkPacket.CmdErrCounter++;
    }
    return Result;
} /* End of CS_VerifyCmdLength */

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
    int32                              Status;

    if (CS_AppData.HkPacket.CfeCoreCSState == CS_STATE_ENABLED)
    {
        ResultsEntry = &CS_AppData.CfeCoreCodeSeg;

        /* Check if the code segment state is enabled (could be disabled for
           a ground-commanded recompute) */
        if (ResultsEntry->State == CS_STATE_ENABLED)
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

                CS_AppData.HkPacket.CfeCoreCSErrCounter++;

                CFE_EVS_SendEvent(CS_CFECORE_MISCOMPARE_ERR_EID, CFE_EVS_EventType_ERROR,
                                  "Checksum Failure: cFE Core, Expected: 0x%08X, Calculated: 0x%08X",
                                  (unsigned int)(ResultsEntry->ComparisonValue), (unsigned int)ComputedCSValue);
            }

            if (DoneWithEntry == true)
            {
                CS_AppData.HkPacket.CurrentEntryInTable++;
            }

            /* only one entry to do */
            if (CS_AppData.HkPacket.CurrentEntryInTable > 0)
            {
                /* We are done with this table */
                CS_AppData.HkPacket.CfeCoreBaseline = ResultsEntry->ComparisonValue;
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
} /* end CS_BackgroundCfeCore */

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
    int32                              Status;

    if (CS_AppData.HkPacket.OSCSState == CS_STATE_ENABLED)
    {
        ResultsEntry = &CS_AppData.OSCodeSeg;

        /* Check if the code segment state is enabled (could be disabled for
         a ground-commanded recompute) */
        if (ResultsEntry->State == CS_STATE_ENABLED)
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
                CS_AppData.HkPacket.OSCSErrCounter++;

                CFE_EVS_SendEvent(CS_OS_MISCOMPARE_ERR_EID, CFE_EVS_EventType_ERROR,
                                  "Checksum Failure: OS code segment, Expected: 0x%08X, Calculated: 0x%08X",
                                  (unsigned int)(ResultsEntry->ComparisonValue), (unsigned int)ComputedCSValue);
            }

            if (DoneWithEntry == true)
            {
                CS_AppData.HkPacket.CurrentEntryInTable++;
            }

            /* only one entry to do */
            if (CS_AppData.HkPacket.CurrentEntryInTable > 0)
            {
                /* We are done with this table */
                CS_AppData.HkPacket.OSBaseline = ResultsEntry->ComparisonValue;
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

} /* end CS_BackgroundOS */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Background check Eeprom                                         */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
bool CS_BackgroundEeprom(void)
{
    CS_Res_EepromMemory_Table_Entry_t *StartOfResultsTable;
    CS_Res_EepromMemory_Table_Entry_t *ResultsEntry;
    bool                               DoneWithCycle   = false;
    bool                               DoneWithEntry   = false;
    uint32                             ComputedCSValue = 0;
    int32                              Loop;
    uint32                             EntireEepromCS;
    uint16                             CurrEntry;
    int32                              Status;

    if (CS_AppData.HkPacket.EepromCSState == CS_STATE_ENABLED)
    {
        if (CS_FindEnabledEepromEntry(&CurrEntry) == true)
        {
            /* we found an enabled entry to checksum */
            StartOfResultsTable = CS_AppData.ResEepromTblPtr;
            ResultsEntry        = &StartOfResultsTable[CurrEntry];

            /* If we complete an entry's checksum, this function will update it for us */
            Status = CS_ComputeEepromMemory(ResultsEntry, &ComputedCSValue, &DoneWithEntry);

            /* We need to avoid the case of finishing a table, moving on to the next one
             and computing an entry in that table, since it could put us above the
             maximum bytes per cycle */
            DoneWithCycle = true;

            if (Status == CS_ERROR)
            {
                /* we had a miscompare */

                CS_AppData.HkPacket.EepromCSErrCounter++;

                CFE_EVS_SendEvent(CS_EEPROM_MISCOMPARE_ERR_EID, CFE_EVS_EventType_ERROR,
                                  "Checksum Failure: Entry %d in Eeprom Table, Expected: 0x%08X, Calculated: 0x%08X",
                                  CurrEntry, (unsigned int)(ResultsEntry->ComparisonValue),
                                  (unsigned int)ComputedCSValue);
            }

            if (DoneWithEntry == true)
            {
                CS_AppData.HkPacket.CurrentEntryInTable++;
            }
        }

        if (CS_AppData.HkPacket.CurrentEntryInTable >= CS_MAX_NUM_EEPROM_TABLE_ENTRIES)
        {
            /* Since we are done CS'ing the entire Eeprom table, update the baseline
             number for telemetry */
            EntireEepromCS = 0;
            for (Loop = 0; Loop < CS_MAX_NUM_EEPROM_TABLE_ENTRIES; Loop++)
            {
                EntireEepromCS += CS_AppData.ResEepromTblPtr[Loop].ComparisonValue;
            }

            CS_AppData.HkPacket.EepromBaseline = EntireEepromCS;

            /* We are done with this table */
            CS_GoToNextTable();
        }
    }
    else /* Table is disabled, skip it */
    {
        CS_GoToNextTable();
    }

    return DoneWithCycle;

} /* end CS_BackgroundEeprom */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Background check Memory                                         */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
bool CS_BackgroundMemory(void)
{
    CS_Res_EepromMemory_Table_Entry_t *StartOfResultsTable;
    CS_Res_EepromMemory_Table_Entry_t *ResultsEntry;
    bool                               DoneWithCycle   = false;
    bool                               DoneWithEntry   = false;
    uint32                             ComputedCSValue = 0;
    uint16                             CurrEntry;
    int32                              Status;

    if (CS_AppData.HkPacket.MemoryCSState == CS_STATE_ENABLED)
    {
        /* If we complete an entry's checksum, this function will update it for us */

        if (CS_FindEnabledMemoryEntry(&CurrEntry) == true)
        {
            /* we found an enabled entry to checksum */
            StartOfResultsTable = CS_AppData.ResMemoryTblPtr;
            ResultsEntry        = &StartOfResultsTable[CurrEntry];

            /* If we complete an entry's checksum, this function will update it for us */
            Status = CS_ComputeEepromMemory(ResultsEntry, &ComputedCSValue, &DoneWithEntry);

            /* We need to avoid the case of finishing a table, moving on to the next one
             and computing an entry in that table, since it could put us above the
             maximum bytes per cycle */
            DoneWithCycle = true;

            if (Status == CS_ERROR)
            {
                /* we had a miscompare */

                CS_AppData.HkPacket.MemoryCSErrCounter++;

                CFE_EVS_SendEvent(CS_MEMORY_MISCOMPARE_ERR_EID, CFE_EVS_EventType_ERROR,
                                  "Checksum Failure: Entry %d in Memory Table, Expected: 0x%08X, Calculated: 0x%08X",
                                  CurrEntry, (unsigned int)(ResultsEntry->ComparisonValue),
                                  (unsigned int)ComputedCSValue);
            }

            if (DoneWithEntry == true)
            {
                CS_AppData.HkPacket.CurrentEntryInTable++;
            }

            if (CS_AppData.HkPacket.CurrentEntryInTable >= CS_MAX_NUM_MEMORY_TABLE_ENTRIES)
            {
                /* We are done with this table */
                CS_GoToNextTable();
            }
        }
        else /* since there aren't any enabled entries in this table, go to the next one */
        {
            CS_GoToNextTable();
        }
    }
    else /* Table is disabled, skip it */
    {
        CS_GoToNextTable();
    }

    return DoneWithCycle;

} /* end CS_BackgroundMemory */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Background check Tables                                         */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
bool CS_BackgroundTables(void)
{
    CS_Res_Tables_Table_Entry_t *StartOfTablesResultsTable;
    CS_Res_Tables_Table_Entry_t *TablesResultsEntry;
    bool                         DoneWithCycle   = false;
    bool                         DoneWithEntry   = false;
    uint32                       ComputedCSValue = 0;
    uint16                       CurrEntry;
    int32                        Status;

    if (CS_AppData.HkPacket.TablesCSState == CS_STATE_ENABLED)
    {
        /* If we complete an entry's checksum, this function will update it for us */

        if (CS_FindEnabledTablesEntry(&CurrEntry) == true)
        {
            /* we found an enabled entry to checksum */

            StartOfTablesResultsTable = CS_AppData.ResTablesTblPtr;
            TablesResultsEntry        = &StartOfTablesResultsTable[CurrEntry];

            Status = CS_ComputeTables(TablesResultsEntry, &ComputedCSValue, &DoneWithEntry);

            /* We need to avoid the case of finishing a table, moving on to the next one
             and computing an entry in that table, since  it could put us above the
             maximum bytes per cycle */
            DoneWithCycle = true;

            if (Status == CS_ERROR)
            {
                /* we had a miscompare */
                CS_AppData.HkPacket.TablesCSErrCounter++;

                CFE_EVS_SendEvent(CS_TABLES_MISCOMPARE_ERR_EID, CFE_EVS_EventType_ERROR,
                                  "Checksum Failure: Table %s, Expected: 0x%08X, Calculated: 0x%08X",
                                  TablesResultsEntry->Name, (unsigned int)(TablesResultsEntry->ComparisonValue),
                                  (unsigned int)ComputedCSValue);
            }

            if (Status == CS_ERR_NOT_FOUND)
            {
                CFE_EVS_SendEvent(CS_COMPUTE_TABLES_NOT_FOUND_ERR_EID, CFE_EVS_EventType_ERROR,
                                  "Tables table computing: Table %s could not be found, skipping",
                                  TablesResultsEntry->Name);

                CS_AppData.HkPacket.CurrentEntryInTable++;
            }

            if (DoneWithEntry == true)
            {
                CS_AppData.HkPacket.CurrentEntryInTable++;
            }

            if (CS_AppData.HkPacket.CurrentEntryInTable >= CS_MAX_NUM_TABLES_TABLE_ENTRIES)
            {
                /* We are done with this table */
                CS_GoToNextTable();
            }
        }
        else /* since there aren't any enabled entries in this table, go to the next one */
        {
            CS_GoToNextTable();
        }
    }
    else /* Table is disabled, skip it */
    {
        CS_GoToNextTable();
    }

    return DoneWithCycle;

} /* end CS_BackgroundTables */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Background check App                                            */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
bool CS_BackgroundApp(void)
{
    CS_Res_App_Table_Entry_t *StartOfAppResultsTable;
    CS_Res_App_Table_Entry_t *AppResultsEntry;
    bool                      DoneWithCycle   = false;
    bool                      DoneWithEntry   = false;
    uint32                    ComputedCSValue = 0;
    uint16                    CurrEntry;
    int32                     Status;

    if (CS_AppData.HkPacket.AppCSState == CS_STATE_ENABLED)
    {
        if (CS_FindEnabledAppEntry(&CurrEntry) == true)
        {
            /* we found an enabled entry to checksum */

            StartOfAppResultsTable = CS_AppData.ResAppTblPtr;
            AppResultsEntry        = &StartOfAppResultsTable[CurrEntry];

            /* If we complete an entry's checksum, this function will update it for us */
            Status = CS_ComputeApp(AppResultsEntry, &ComputedCSValue, &DoneWithEntry);

            /* We need to avoid the case of finishing a table, moving on to the next one
             and computing an entry in that table, since it could put us above the
             maximum bytes per cycle */
            DoneWithCycle = true;

            if (Status == CS_ERROR)
            {
                /* we had a miscompare */
                CS_AppData.HkPacket.AppCSErrCounter++;

                CFE_EVS_SendEvent(CS_APP_MISCOMPARE_ERR_EID, CFE_EVS_EventType_ERROR,
                                  "Checksum Failure: Application %s, Expected: 0x%08X, Calculated: 0x%08X",
                                  AppResultsEntry->Name, (unsigned int)(AppResultsEntry->ComparisonValue),
                                  (unsigned int)ComputedCSValue);
            }

            if (Status == CS_ERR_NOT_FOUND)
            {
                CFE_EVS_SendEvent(CS_COMPUTE_APP_NOT_FOUND_ERR_EID, CFE_EVS_EventType_ERROR,
                                  "App table computing: App %s could not be found, skipping", AppResultsEntry->Name);

                CS_AppData.HkPacket.CurrentEntryInTable++;
            }

            if (DoneWithEntry == true)
            {
                CS_AppData.HkPacket.CurrentEntryInTable++;
            }

            if (CS_AppData.HkPacket.CurrentEntryInTable >= CS_MAX_NUM_APP_TABLE_ENTRIES)
            {
                /* We are done with this table */
                CS_GoToNextTable();
            }
        }
        else /* since there aren't any enabled entries in this table, go to the next one */
        {
            CS_GoToNextTable();
        }
    }
    else /* Table is disabled, skip it */
    {
        CS_GoToNextTable();
    }
    return DoneWithCycle;

} /* end CS_BackgroundApp */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Reset Tables result table entry after a CS table gets modified  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_ResetTablesTblResultEntry(CS_Res_Tables_Table_Entry_t *TablesTblResultEntry)
{
    /* Ptr will be NULL if this CS table is not listed in the CS Tables table */
    if (TablesTblResultEntry != (CS_Res_Tables_Table_Entry_t *)NULL)
    {
        TablesTblResultEntry->ByteOffset        = 0;
        TablesTblResultEntry->TempChecksumValue = 0;
        TablesTblResultEntry->ComputedYet       = false;
    }

    return;

} /* end CS_ResetTablesTblResultEntry */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Update all tables                                               */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 CS_HandleRoutineTableUpdates(void)
{
    int32 Result    = CFE_SUCCESS;
    int32 ErrorCode = CFE_SUCCESS;

    if (!((CS_AppData.HkPacket.RecomputeInProgress == true) && (CS_AppData.HkPacket.OneShotInProgress == false) &&
          (CS_AppData.ChildTaskTable == CS_EEPROM_TABLE)))
    {
        Result = CS_HandleTableUpdate((void *)&CS_AppData.DefEepromTblPtr, (void *)&CS_AppData.ResEepromTblPtr,
                                      CS_AppData.DefEepromTableHandle, CS_AppData.ResEepromTableHandle, CS_EEPROM_TABLE,
                                      CS_MAX_NUM_EEPROM_TABLE_ENTRIES);

        if (Result != CFE_SUCCESS)
        {
            CS_AppData.HkPacket.EepromCSState = CS_STATE_DISABLED;
            Result                            = CFE_EVS_SendEvent(CS_UPDATE_EEPROM_ERR_EID, CFE_EVS_EventType_ERROR,
                                       "Table update failed for Eeprom: 0x%08X, checksumming Eeprom is disabled",
                                       (unsigned int)Result);
            ErrorCode                         = Result;
        }
    }

    if (!((CS_AppData.HkPacket.RecomputeInProgress == true) && (CS_AppData.HkPacket.OneShotInProgress == false) &&
          (CS_AppData.ChildTaskTable == CS_MEMORY_TABLE)))
    {
        Result = CS_HandleTableUpdate((void *)&CS_AppData.DefMemoryTblPtr, (void *)&CS_AppData.ResMemoryTblPtr,
                                      CS_AppData.DefMemoryTableHandle, CS_AppData.ResMemoryTableHandle, CS_MEMORY_TABLE,
                                      CS_MAX_NUM_MEMORY_TABLE_ENTRIES);
        if (Result != CFE_SUCCESS)
        {
            CS_AppData.HkPacket.MemoryCSState = CS_STATE_DISABLED;
            Result                            = CFE_EVS_SendEvent(CS_UPDATE_MEMORY_ERR_EID, CFE_EVS_EventType_ERROR,
                                       "Table update failed for Memory: 0x%08X, checksumming Memory is disabled",
                                       (unsigned int)Result);

            if (ErrorCode == CFE_SUCCESS)
            {
                ErrorCode = Result;
            }
        }
    }

    if (!((CS_AppData.HkPacket.RecomputeInProgress == true) && (CS_AppData.HkPacket.OneShotInProgress == false) &&
          (CS_AppData.ChildTaskTable == CS_APP_TABLE)))
    {
        Result = CS_HandleTableUpdate((void *)&CS_AppData.DefAppTblPtr, (void *)&CS_AppData.ResAppTblPtr,
                                      CS_AppData.DefAppTableHandle, CS_AppData.ResAppTableHandle, CS_APP_TABLE,
                                      CS_MAX_NUM_APP_TABLE_ENTRIES);
        if (Result != CFE_SUCCESS)
        {
            CS_AppData.HkPacket.AppCSState = CS_STATE_DISABLED;
            Result                         = CFE_EVS_SendEvent(CS_UPDATE_APP_ERR_EID, CFE_EVS_EventType_ERROR,
                                       "Table update failed for Apps: 0x%08X, checksumming Apps is disabled",
                                       (unsigned int)Result);
            if (ErrorCode == CFE_SUCCESS)
            {
                ErrorCode = Result;
            }
        }
    }

    if (!((CS_AppData.HkPacket.RecomputeInProgress == true) && (CS_AppData.HkPacket.OneShotInProgress == false) &&
          (CS_AppData.ChildTaskTable == CS_TABLES_TABLE)))
    {
        Result = CS_HandleTableUpdate((void *)&CS_AppData.DefTablesTblPtr, (void *)&CS_AppData.ResTablesTblPtr,
                                      CS_AppData.DefTablesTableHandle, CS_AppData.ResTablesTableHandle, CS_TABLES_TABLE,
                                      CS_MAX_NUM_TABLES_TABLE_ENTRIES);

        if (Result != CFE_SUCCESS)
        {
            CS_AppData.HkPacket.TablesCSState = CS_STATE_DISABLED;
            Result                            = CFE_EVS_SendEvent(CS_UPDATE_TABLES_ERR_EID, CFE_EVS_EventType_ERROR,
                                       "Table update failed for Tables: 0x%08X, checksumming Tables is disabled",
                                       (unsigned int)Result);
            if (ErrorCode == CFE_SUCCESS)
            {
                ErrorCode = Result;
            }
        }
    }

    return ErrorCode;
} /* end CS_HandleRoutineTableUpdates */

int32 CS_AttemptTableReshare(CS_Res_Tables_Table_Entry_t *ResultsEntry, CFE_TBL_Handle_t *LocalTblHandle,
                             CFE_TBL_Info_t *TblInfo, cpuaddr *LocalAddress, int32 *ResultGetInfo)
{
    int32 Result           = CS_SUCCESS;
    int32 ResultShare      = 0;
    int32 ResultGetAddress = 0;

    /* Maybe the table came back, try and reshare it */
    ResultShare = CFE_TBL_Share(LocalTblHandle, ResultsEntry->Name);

    if (ResultShare == CFE_SUCCESS)
    {
        ResultsEntry->TblHandle = *LocalTblHandle;

        *ResultGetInfo = CFE_TBL_GetInfo(TblInfo, ResultsEntry->Name);

        /* need to try to get the address again */
        ResultGetAddress = CFE_TBL_GetAddress((void *)LocalAddress, *LocalTblHandle);
        Result           = ResultGetAddress;

        /* if the table was never loaded, release the address to prevent the table from being
        locked by CS, which would prevent the owner app from updating it*/
        if (ResultGetAddress == CFE_TBL_ERR_NEVER_LOADED)
        {
            CFE_TBL_ReleaseAddress(*LocalTblHandle);
        }
    }

    else /* table was not there on the new share */
    {
        Result = ResultShare;
    }

    return Result;
}

bool CS_CheckRecomputeOneshot(void)
{
    bool Result = false;

    if (CS_AppData.HkPacket.RecomputeInProgress == true || CS_AppData.HkPacket.OneShotInProgress == true)
    {
        CFE_EVS_SendEvent(CS_CMD_COMPUTE_PROG_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Cannot perform command. Recompute or oneshot in progress.");

        CS_AppData.HkPacket.CmdErrCounter++;

        Result = true;
    }
    return Result;
}

/************************/
/*  End of File Comment */
/************************/
