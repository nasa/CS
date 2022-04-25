#include <string.h>
#include "cfe.h"
#include "cs_app.h"

#include "cs_platform_cfg.h"
#include "cs_events.h"
#include "cs_utils.h"
#include "cs_compute.h"
#include "cs_eeprom_cmds.h"
#include "cs_table_cmds.h"
#include "cs_memory_cmds.h"
#include "cs_app_cmds.h"
#include "cs_cmds.h"
#include "cs_init.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Software Bus Setup                                           */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 CS_SbInit(void)
{
    int32     Result = CFE_SUCCESS;

    /* Initialize app configuration data */
    strncpy(CS_AppData.PipeName, CS_CMD_PIPE_NAME, CS_CMD_PIPE_NAME_LEN);
    
    CS_AppData.PipeDepth = CS_PIPE_DEPTH;
    
    /* Initialize housekeeping packet */
    CFE_MSG_Init (& CS_AppData.HkPacket.TlmHeader.Msg,
                    CS_HK_TLM_MID, 
                    sizeof (CS_HkPacket_t));

    /* Create Software Bus message pipe */
    Result = CFE_SB_CreatePipe (& CS_AppData.CmdPipe,
                                CS_AppData.PipeDepth,
                                CS_AppData.PipeName);
    if (Result != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent (CS_INIT_SB_CREATE_ERR_EID,
                           CFE_EVS_EventType_ERROR,
                           "Software Bus Create Pipe for command returned: 0x%08X",
                           (unsigned int)Result);
    }
    else
    {
        /* Subscribe to Housekeeping request commands */
        Result = CFE_SB_Subscribe (CS_SEND_HK_MID,
                                   CS_AppData.CmdPipe);
    
        if (Result != CFE_SUCCESS)
        {
            CFE_EVS_SendEvent (CS_INIT_SB_SUBSCRIBE_HK_ERR_EID,
                               CFE_EVS_EventType_ERROR,
                               "Software Bus subscribe to housekeeping returned: 0x%08X",(unsigned int)Result);
        }
    
        if(Result == CFE_SUCCESS)
        {
            /* Subscribe to background checking schedule */
            Result = CFE_SB_Subscribe( CS_BACKGROUND_CYCLE_MID,
                                      CS_AppData.CmdPipe);
    
            if (Result != CFE_SUCCESS)
            {
                CFE_EVS_SendEvent (CS_INIT_SB_SUBSCRIBE_BACK_ERR_EID,
                                   CFE_EVS_EventType_ERROR,
                                   "Software Bus subscribe to background cycle returned: 0x%08X",(unsigned int)Result);
            }
        }

        if(Result == CFE_SUCCESS)
        {
            /* Subscribe to CS Internal command packets */
            Result = CFE_SB_Subscribe (CS_CMD_MID,
                                   CS_AppData.CmdPipe);
            if (Result != CFE_SUCCESS)
            {
                CFE_EVS_SendEvent (CS_INIT_SB_SUBSCRIBE_CMD_ERR_EID,
                                   CFE_EVS_EventType_ERROR,
                                   "Software Bus subscribe to command returned: 0x%08X",(unsigned int)Result);
            }
        }
    }
    return Result;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Table Initialization                                         */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 CS_InitAllTables(void)
{
    int32 ResultInit = CFE_SUCCESS;

    ResultInit = CS_TableInit(& CS_AppData.DefEepromTableHandle,
                              & CS_AppData.ResEepromTableHandle,
                              (void*) & CS_AppData.DefEepromTblPtr,
                              (void*) &CS_AppData.ResEepromTblPtr,
                              CS_EEPROM_TABLE, 
                              CS_DEF_EEPROM_TABLE_NAME,
                              CS_RESULTS_EEPROM_TABLE_NAME,
                              CS_MAX_NUM_EEPROM_TABLE_ENTRIES,
                              CS_DEF_EEPROM_TABLE_FILENAME,
                              &CS_AppData.DefaultEepromDefTable,
                              sizeof(CS_Def_EepromMemory_Table_Entry_t),
                              sizeof(CS_Res_EepromMemory_Table_Entry_t),
                              CS_ValidateEepromChecksumDefinitionTable);
    
    if(ResultInit != CFE_SUCCESS)
    {
        CS_AppData.HkPacket.EepromCSState = CS_STATE_DISABLED;
        CFE_EVS_SendEvent (CS_INIT_EEPROM_ERR_EID,
                           CFE_EVS_EventType_ERROR,
                           "Table initialization failed for Eeprom: 0x%08X",
                           (unsigned int)ResultInit);
    }
    
    if(ResultInit == CFE_SUCCESS)
    {
        ResultInit = CS_TableInit(& CS_AppData.DefMemoryTableHandle,
                                  & CS_AppData.ResMemoryTableHandle,
                                  (void*) & CS_AppData.DefMemoryTblPtr,
                                  (void*) & CS_AppData.ResMemoryTblPtr,
                                  CS_MEMORY_TABLE, 
                                  CS_DEF_MEMORY_TABLE_NAME,
                                  CS_RESULTS_MEMORY_TABLE_NAME,
                                   CS_MAX_NUM_MEMORY_TABLE_ENTRIES,
                                  CS_DEF_MEMORY_TABLE_FILENAME,
                                  &CS_AppData.DefaultMemoryDefTable,
                                  sizeof(CS_Def_EepromMemory_Table_Entry_t),
                                  sizeof(CS_Res_EepromMemory_Table_Entry_t),
                                  CS_ValidateMemoryChecksumDefinitionTable);
    
    
        if(ResultInit != CFE_SUCCESS)
        {
            CS_AppData.HkPacket.MemoryCSState = CS_STATE_DISABLED;
            CFE_EVS_SendEvent (CS_INIT_MEMORY_ERR_EID,
                               CFE_EVS_EventType_ERROR,
                               "Table initialization failed for Memory: 0x%08X",
                               (unsigned int)ResultInit);
        }
    }

    if(ResultInit == CFE_SUCCESS)
    {
        ResultInit = CS_TableInit(& CS_AppData.DefAppTableHandle,
                                  & CS_AppData.ResAppTableHandle,
                                  (void*) & CS_AppData.DefAppTblPtr,
                                  (void*) & CS_AppData.ResAppTblPtr,
                                  CS_APP_TABLE, 
                                  CS_DEF_APP_TABLE_NAME,
                                  CS_RESULTS_APP_TABLE_NAME,
                                  CS_MAX_NUM_APP_TABLE_ENTRIES,
                                  CS_DEF_APP_TABLE_FILENAME,
                                  &CS_AppData.DefaultAppDefTable,
                                  sizeof(CS_Def_App_Table_Entry_t),
                                  sizeof(CS_Res_App_Table_Entry_t),
                                  CS_ValidateAppChecksumDefinitionTable);
    
        if(ResultInit != CFE_SUCCESS)
        {
            CS_AppData.HkPacket.AppCSState = CS_STATE_DISABLED;
            CFE_EVS_SendEvent (CS_INIT_APP_ERR_EID,
                               CFE_EVS_EventType_ERROR,
                               "Table initialization failed for Apps: 0x%08X",
                               (unsigned int)ResultInit);
        }
    }

    if(ResultInit == CFE_SUCCESS)
    {
        ResultInit = CS_TableInit(& CS_AppData.DefTablesTableHandle,
                                  & CS_AppData.ResTablesTableHandle,
                                  (void*) & CS_AppData.DefTablesTblPtr,
                                  (void*) & CS_AppData.ResTablesTblPtr,
                                  CS_TABLES_TABLE, 
                                  CS_DEF_TABLES_TABLE_NAME,
                                  CS_RESULTS_TABLES_TABLE_NAME,
                                  CS_MAX_NUM_TABLES_TABLE_ENTRIES,
                                  CS_DEF_TABLES_TABLE_FILENAME,
                                  &CS_AppData.DefaultTablesDefTable,
                                  sizeof(CS_Def_Tables_Table_Entry_t),
                                  sizeof(CS_Res_Tables_Table_Entry_t),
                                  CS_ValidateTablesChecksumDefinitionTable);
    
        if(ResultInit != CFE_SUCCESS)
        {
            CS_AppData.HkPacket.TablesCSState = CS_STATE_DISABLED;
            CFE_EVS_SendEvent (CS_INIT_TABLES_ERR_EID,
                               CFE_EVS_EventType_ERROR,
                               "Table initialization failed for Tables: 0x%08X",
                               (unsigned int)ResultInit);
        }
    }

    return ResultInit;

}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CS Segment Initialization                                       */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CS_InitSegments(void)
{
    int32 ResultSegment = OS_SUCCESS;
    uint32    CFESize;
    cpuaddr   CFEAddress;
    uint32    KernelSize;
    cpuaddr   KernelAddress;

    /* Initalize the CFE core segments */
    ResultSegment = CFE_PSP_GetCFETextSegmentInfo(&CFEAddress, &CFESize);
    
    if(ResultSegment != OS_SUCCESS) 
    {
        CS_AppData.CfeCoreCodeSeg.StartAddress           = 0;
        CS_AppData.CfeCoreCodeSeg.NumBytesToChecksum     = 0;
        CS_AppData.CfeCoreCodeSeg.ComputedYet            = false   ;
        CS_AppData.CfeCoreCodeSeg.ComparisonValue        = 0;
        CS_AppData.CfeCoreCodeSeg.ByteOffset             = 0;
        CS_AppData.CfeCoreCodeSeg.TempChecksumValue      = 0;
        CS_AppData.CfeCoreCodeSeg.State                  = CS_STATE_DISABLED;
        
        CFE_EVS_SendEvent (CS_CFE_TEXT_SEG_INF_EID,
                           CFE_EVS_EventType_INFORMATION,
                           "CFE Text Segment disabled");


    }
    else
    {
        CS_AppData.CfeCoreCodeSeg.StartAddress           = CFEAddress;
        CS_AppData.CfeCoreCodeSeg.NumBytesToChecksum     = CFESize;
        CS_AppData.CfeCoreCodeSeg.ComputedYet            = false   ;
        CS_AppData.CfeCoreCodeSeg.ComparisonValue        = 0;
        CS_AppData.CfeCoreCodeSeg.ByteOffset             = 0;
        CS_AppData.CfeCoreCodeSeg.TempChecksumValue      = 0;
        CS_AppData.CfeCoreCodeSeg.State                  = CS_STATE_ENABLED;
    }

    /* Initialize the OS Core code segment*/
    
    ResultSegment  = CFE_PSP_GetKernelTextSegmentInfo( &KernelAddress, &KernelSize);
    
    if (ResultSegment != OS_SUCCESS)
    {
        CS_AppData.OSCodeSeg.StartAddress           = 0;
        CS_AppData.OSCodeSeg.NumBytesToChecksum     = 0;
        CS_AppData.OSCodeSeg.ComputedYet            = false   ;
        CS_AppData.OSCodeSeg.ComparisonValue        = 0;
        CS_AppData.OSCodeSeg.ByteOffset             = 0;
        CS_AppData.OSCodeSeg.TempChecksumValue      = 0;
        CS_AppData.OSCodeSeg.State                  = CS_STATE_DISABLED;
        
        
        CFE_EVS_SendEvent (CS_OS_TEXT_SEG_INF_EID,
                           CFE_EVS_EventType_INFORMATION,
                           "OS Text Segment disabled due to platform");
    }
    else
    {
        CS_AppData.OSCodeSeg.StartAddress           = KernelAddress;
        CS_AppData.OSCodeSeg.NumBytesToChecksum     = KernelSize;
        CS_AppData.OSCodeSeg.ComputedYet            = false   ;
        CS_AppData.OSCodeSeg.ComparisonValue        = 0;
        CS_AppData.OSCodeSeg.ByteOffset             = 0;
        CS_AppData.OSCodeSeg.TempChecksumValue      = 0;
        CS_AppData.OSCodeSeg.State                  = CS_STATE_ENABLED;
        
    }

}
