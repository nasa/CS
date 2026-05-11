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
 *   CFS Checksum (CS) Application provides the service of background
 *   checksumming user-defined objects in the CFS
 */
#include <string.h>
#include "cfe.h"
#include "cs_app.h"

#include "cs_platform_cfg.h"
#include "cs_eventids.h"
#include "cs_utils.h"
#include "cs_compute.h"
#include "cs_eeprom_cmds.h"
#include "cs_table_cmds.h"
#include "cs_memory_cmds.h"
#include "cs_app_cmds.h"
#include "cs_cmds.h"
#include "cs_init.h"
#include "cs_dispatch.h"

/*************************************************************************
**
** Macro definitions
**
**************************************************************************/
#define CS_NUM_DATA_STORE_STATES 6 /* 4 tables + OS CS + cFE core number of checksum states for CDS */

/*************************************************************************
**
** CS global application data
**
**************************************************************************/
CS_AppData_t CS_AppData;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS application entry point and main process loop                */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_AppMain(void)
{
    CFE_Status_t     Result = 0;
    CFE_SB_Buffer_t *BufPtr = NULL;

    /* Performance Log (start time counter) */
    CFE_ES_PerfLogEntry(CS_APPMAIN_PERF_ID);

    /* Perform application-specific initialization */
    Result = CS_AppInit();

    /* Check for start-up error */
    if (Result != CFE_SUCCESS)
    {
        /* Set request to terminate main loop */
        CS_AppData.RunStatus = CFE_ES_RunStatus_APP_ERROR;
    }

    CFE_ES_WaitForStartupSync(CS_STARTUP_TIMEOUT);

    /* Main process loop */
    while (CFE_ES_RunLoop(&CS_AppData.RunStatus))
    {
        /* Performance Log (stop time counter) */
        CFE_ES_PerfLogExit(CS_APPMAIN_PERF_ID);

        /* Wait for the next Software Bus message */
        Result = CFE_SB_ReceiveBuffer(&BufPtr, CS_AppData.CmdPipe, CS_WAKEUP_TIMEOUT);

        /* Performance Log (start time counter)  */
        CFE_ES_PerfLogEntry(CS_APPMAIN_PERF_ID);

        if ((Result == CFE_SUCCESS) && (BufPtr != NULL))
        {
            /* Process Software Bus message */
            Result = CS_AppPipe(BufPtr);
        }
        else if ((Result == CFE_SB_TIME_OUT) || (Result == CFE_SB_NO_MESSAGE))
        {
            Result = CS_HandleRoutineTableUpdates();
        }
        else
        {
            /* All other cases are caught by the following condition
               Result != CFE_SUCCESS */
        }

        /*
         ** Note: If there were some reason to exit the task
         **       normally (without error) then we would set
         **       RunStatus = CFE_ES_APP_EXIT
         */
        if (Result != CFE_SUCCESS)
        {
            /* Set request to terminate main loop */
            CS_AppData.RunStatus = CFE_ES_RunStatus_APP_ERROR;
        }
    } /* end run loop */

    /* Check for "fatal" process error */
    if (CS_AppData.RunStatus == CFE_ES_RunStatus_APP_ERROR || CS_AppData.RunStatus == CFE_ES_RunStatus_SYS_EXCEPTION)
    {
        /* Send an error event with run status and result */
        CFE_EVS_SendEvent(CS_EXIT_ERR_EID,
                          CFE_EVS_EventType_ERROR,
                          "App terminating, RunStatus:0x%08X, RC:0x%08X",
                          (unsigned int)CS_AppData.RunStatus,
                          (unsigned int)Result);
    }
    else
    {
        /* Send an informational event describing the reason for the termination */
        CFE_EVS_SendEvent(CS_EXIT_INF_EID,
                          CFE_EVS_EventType_INFORMATION,
                          "App terminating, RunStatus:0x%08X",
                          (unsigned int)CS_AppData.RunStatus);
    }

    /* In case cFE Event Services is not working */
    CFE_ES_WriteToSysLog("CS App terminating, RunStatus:0x%08X, RC:0x%08X\n",
                         (unsigned int)CS_AppData.RunStatus,
                         (unsigned int)Result);

    /* Performance Log (stop time counter) */
    CFE_ES_PerfLogExit(CS_APPMAIN_PERF_ID);

    /* Let cFE kill the task (and child task) */
    CFE_ES_ExitApp(CS_AppData.RunStatus);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Application initialization function                          */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CFE_Status_t CS_AppInit(void)
{
    CFE_Status_t Result = CFE_SUCCESS;

    /* Register for event services */
    Result = CFE_EVS_Register(NULL, 0, 0);

    if (Result != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("CS App: Error Registering For Event Services, RC = 0x%08X\n", (unsigned int)Result);
    }
    else
    {
        /* Zero out all data in CS_AppData, including the housekeeping data*/
        memset(&CS_AppData, 0, sizeof(CS_AppData));

        CS_AppData.RunStatus = CFE_ES_RunStatus_APP_RUN;

        Result = CS_SbInit();
    }

    if (Result == CFE_SUCCESS)
    {
        /* Set up default tables in memory */
        CS_InitializeDefaultTables();

        CS_AppData.HkPacket.Payload.EepromCSState = CS_EEPROM_TBL_POWERON_STATE;
        CS_AppData.HkPacket.Payload.MemoryCSState = CS_MEMORY_TBL_POWERON_STATE;
        CS_AppData.HkPacket.Payload.AppCSState    = CS_APPS_TBL_POWERON_STATE;
        CS_AppData.HkPacket.Payload.TablesCSState = CS_TABLES_TBL_POWERON_STATE;

        CS_AppData.HkPacket.Payload.OSCSState      = CS_OSCS_CHECKSUM_STATE;
        CS_AppData.HkPacket.Payload.CfeCoreCSState = CS_CFECORE_CHECKSUM_STATE;

#if (CS_PRESERVE_STATES_ON_PROCESSOR_RESET == true)
        Result = CS_CreateRestoreStatesFromCDS();
#endif
    }

    if (Result == CFE_SUCCESS)
    {
        Result = CS_InitAllTables();
    }

    if (Result == CFE_SUCCESS)
    {
        CS_InitSegments();

        /* initialize the place to ostart background checksumming */
        CS_AppData.HkPacket.Payload.CurrentCSTable      = 0;
        CS_AppData.HkPacket.Payload.CurrentEntryInTable = 0;

        /* Initial settings for the CS Application */
        /* the rest of the tables are initialized in CS_TableInit */
        CS_AppData.HkPacket.Payload.ChecksumState = CS_ChecksumState_ENABLED;

        CS_AppData.HkPacket.Payload.RecomputeInProgress = false;
        CS_AppData.HkPacket.Payload.OneShotInProgress   = false;

        CS_AppData.MaxBytesPerCycle = CS_DEFAULT_BYTES_PER_CYCLE;

        /* Application startup event message */
        Result = CFE_EVS_SendEvent(CS_INIT_INF_EID,
                                   CFE_EVS_EventType_INFORMATION,
                                   "CS Initialized. Version %d.%d.%d.%d",
                                   CS_MAJOR_VERSION,
                                   CS_MINOR_VERSION,
                                   CS_REVISION,
                                   CS_MISSION_REV);
    }
    return Result;
}

#if (CS_PRESERVE_STATES_ON_PROCESSOR_RESET == true)

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS_CreateCDS() - create CS storage area in CDS                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

CFE_Status_t CS_CreateRestoreStatesFromCDS(void)
{
    /* Store task ena/dis state of tables in CDS */
    uint8        DataStoreBuffer[CS_NUM_DATA_STORE_STATES];
    CFE_Status_t Result;
    int32        EventId = 0;

    memset(DataStoreBuffer, 0, sizeof(DataStoreBuffer));

    /*
    ** Request for CDS area from cFE Executive Services...
    */
    Result = CFE_ES_RegisterCDS(&CS_AppData.DataStoreHandle, sizeof(DataStoreBuffer), CS_CDS_NAME);

    if (Result == CFE_SUCCESS)
    {
        /*
        ** New CDS area - write to Critical Data Store...
        */
        DataStoreBuffer[0] = CS_AppData.HkPacket.Payload.EepromCSState;
        DataStoreBuffer[1] = CS_AppData.HkPacket.Payload.MemoryCSState;
        DataStoreBuffer[2] = CS_AppData.HkPacket.Payload.AppCSState;
        DataStoreBuffer[3] = CS_AppData.HkPacket.Payload.TablesCSState;

        DataStoreBuffer[4] = CS_AppData.HkPacket.Payload.OSCSState;
        DataStoreBuffer[5] = CS_AppData.HkPacket.Payload.CfeCoreCSState;

        Result = CFE_ES_CopyToCDS(CS_AppData.DataStoreHandle, DataStoreBuffer);

        if (Result != CFE_SUCCESS)
        {
            EventId = CS_CR_CDS_CPY_ERR_EID;
        }
    }
    else if (Result == CFE_ES_CDS_ALREADY_EXISTS)
    {
        /*
        ** Pre-existing CDS area - read from Critical Data Store...
        */
        Result = CFE_ES_RestoreFromCDS(DataStoreBuffer, CS_AppData.DataStoreHandle);

        if (Result == CFE_SUCCESS)
        {
            CS_AppData.HkPacket.Payload.EepromCSState = DataStoreBuffer[0];
            CS_AppData.HkPacket.Payload.MemoryCSState = DataStoreBuffer[1];
            CS_AppData.HkPacket.Payload.AppCSState    = DataStoreBuffer[2];
            CS_AppData.HkPacket.Payload.TablesCSState = DataStoreBuffer[3];

            CS_AppData.HkPacket.Payload.OSCSState      = DataStoreBuffer[4];
            CS_AppData.HkPacket.Payload.CfeCoreCSState = DataStoreBuffer[5];
        }
        else
        {
            EventId = CS_CR_CDS_RES_ERR_EID;
        }
    }
    else
    {
        EventId = CS_CR_CDS_REG_ERR_EID;
    }

    if (Result != CFE_SUCCESS)
    {
        /*
        ** CDS is broken - prevent further errors...
        */
        CS_AppData.DataStoreHandle = CFE_ES_CDS_BAD_HANDLE;

        /* Use states from platform configuration */
        CS_AppData.HkPacket.Payload.EepromCSState = CS_EEPROM_TBL_POWERON_STATE;
        CS_AppData.HkPacket.Payload.MemoryCSState = CS_MEMORY_TBL_POWERON_STATE;
        CS_AppData.HkPacket.Payload.AppCSState    = CS_APPS_TBL_POWERON_STATE;
        CS_AppData.HkPacket.Payload.TablesCSState = CS_TABLES_TBL_POWERON_STATE;

        CS_AppData.HkPacket.Payload.OSCSState      = CS_OSCS_CHECKSUM_STATE;
        CS_AppData.HkPacket.Payload.CfeCoreCSState = CS_CFECORE_CHECKSUM_STATE;

        CFE_EVS_SendEvent(EventId,
                          CFE_EVS_EventType_ERROR,
                          "Critical Data Store access error = 0x%08X",
                          (unsigned int)Result);
        /*
        ** CDS errors are not fatal - CS can still run...
        */
        Result = CFE_SUCCESS;
    }

    return Result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS_UpdateCDS() - update DS storage area in CDS                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CS_UpdateCDS(void)
{
    /* Store table ena/dis state in CDS */
    uint8        DataStoreBuffer[CS_NUM_DATA_STORE_STATES];
    CFE_Status_t Result;

    /*
    ** Handle is defined when CDS is active...
    */
    if (CFE_RESOURCEID_TEST_DEFINED(CS_AppData.DataStoreHandle))
    {
        /*
        ** Copy ena/dis states of tables to the data array...
        */
        DataStoreBuffer[0] = CS_AppData.HkPacket.Payload.EepromCSState;
        DataStoreBuffer[1] = CS_AppData.HkPacket.Payload.MemoryCSState;
        DataStoreBuffer[2] = CS_AppData.HkPacket.Payload.AppCSState;
        DataStoreBuffer[3] = CS_AppData.HkPacket.Payload.TablesCSState;

        DataStoreBuffer[4] = CS_AppData.HkPacket.Payload.OSCSState;
        DataStoreBuffer[5] = CS_AppData.HkPacket.Payload.CfeCoreCSState;

        /*
        ** Update CS portion of Critical Data Store...
        */
        Result = CFE_ES_CopyToCDS(CS_AppData.DataStoreHandle, DataStoreBuffer);

        if (Result != CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(CS_UPDATE_CDS_ERR_EID,
                              CFE_EVS_EventType_ERROR,
                              "Critical Data Store access error = 0x%08X",
                              (unsigned int)Result);
            /*
            ** CDS is broken - prevent further errors...
            */
            CS_AppData.DataStoreHandle = CFE_ES_CDS_BAD_HANDLE;
        }
    }
}
#endif /* #if (CS_PRESERVE_STATES_ON_PROCESSOR_RESET == true   ) */
