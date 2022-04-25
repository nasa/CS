/************************************************************************
 ** File:
 **   $Id: cs_app.h 1.7 2017/03/29 16:10:34EDT mdeschu Exp  $
 **
 **   Copyright (c) 2007-2020 United States Government as represented by the 
 **   Administrator of the National Aeronautics and Space Administration. 
 **   All Other Rights Reserved.  
 **
 **   This software was created at NASA's Goddard Space Flight Center.
 **   This software is governed by the NASA Open Source Agreement and may be 
 **   used, distributed and modified only pursuant to the terms of that 
 **   agreement.
 **
 ** Purpose: 
 **   Unit specification for the Core Flight System (CFS) 
 **   Checksum (CS) Application.  
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS CS Heritage Analysis Document
 **   CFS CS CDR Package
 **
 *************************************************************************/

#ifndef _cs_app_
#define _cs_app_

/**************************************************************************
 **
 ** Include section
 **
 **************************************************************************/

#include "cfe.h"
#include "cs_tbldefs.h"
#include "cs_msg.h"
#include "cs_msgdefs.h"
#include "cs_platform_cfg.h"
#include "cs_mission_cfg.h"
#include "cs_msgids.h"
#include "cs_perfids.h"
#include "cs_verify.h"
#include "cs_version.h"

/**
 ** \name CS Error Codes */
/**\{ */
#define CS_SUCCESS                0   /**< \brief Success return code
                                            when a checksum compare did 
                                            not fail*/
#define CS_ERROR                (-1)  /**< \brief Error code returned
                                            when a checksum compare
                                            failed*/
#define CS_ERR_NOT_FOUND        (-2)  /**< \brief Error code returned
                                            on a Table or App operation
                                            where the app or table requested
                                            could not be found */
#define CS_TABLE_ERROR          (-3)  /**< \brief Error code returned
                                            on table validation error */
/**\} */

/**************************************************************************
 **
 ** Macro definitions
 **
 **************************************************************************/

/**
** \name CS Command Pipe Parameters */
/**\{ */
#define CS_CMD_PIPE_NAME               "CS_CMD_PIPE"
#define CS_CMD_PIPE_NAME_LEN            16
/**\} */


/**
 ** \name CS Name of Table Size */
/**\{ */
#define CS_TABLETYPE_NAME_SIZE          10
/**\} */

/**
 ** \name CS Child Task Names */
/**\{ */
#define CS_RECOMP_OS_TASK_NAME          "CS_RecmpOSTsk"
#define CS_RECOMP_CFECORE_TASK_NAME     "CS_RecmpCfeCoreTsk"
#define CS_RECOMP_MEMORY_TASK_NAME      "CS_RecmpMemoryTsk"
#define CS_RECOMP_EEPROM_TASK_NAME      "CS_RecmpEepromTsk"
#define CS_RECOMP_APP_TASK_NAME         "CS_RecmpAppTsk"
#define CS_RECOMP_TABLES_TASK_NAME      "CS_RecmpTableTsk"
#define CS_ONESHOT_TASK_NAME            "CS_OneShotTask"
/**\} */


/**
** \brief Wakeup for CS
**
** \par Description
**      Wakes up CS every 1 second for routine maintenance whether a
**      message was received or not.
*/
#define CS_WAKEUP_TIMEOUT          1000


/*************************************************************************
 **
 ** Type definitions
 **
 **************************************************************************/

/** 
 **  \brief CS global data structure
 */
typedef struct
{
    CS_HkPacket_t                               HkPacket;                           /**< \brief Housekeeping telemetry packet */

    char                                        PipeName[CS_CMD_PIPE_NAME_LEN];     /**< \brief Command pipe name*/
    uint16                                      PipeDepth;                          /**< \brief Command pipe depth*/
    
    uint16                                      ChildTaskTable;                     /**< \brief Table for the child task to process*/
    uint16                                      ChildTaskEntryID;                   /**< \brief Entry in table for child task to process */
    uint32                                      ChildTaskID;                        /**< \brief Task ID for the child task*/

    uint32                                      MaxBytesPerCycle;                   /**< \brief Max number of bytes to process in a cycle*/
    
    uint32                                      RunStatus;                          /**< \brief Application run status */

    CS_Res_EepromMemory_Table_Entry_t         * RecomputeEepromMemoryEntryPtr;      /**< \brief Pointer to an entry to recompute in the 
                                                                                                Eeprom or Memory table*/
    
    CS_Res_App_Table_Entry_t                  * RecomputeAppEntryPtr;               /**< \brief Pointer to an entry to recompute in the 
                                                                                                Application table*/
    CS_Res_Tables_Table_Entry_t               * RecomputeTablesEntryPtr;            /**< \brief Pointer to an entry to recompute in the 
                                                                                                Tables table*/
    
    CFE_SB_PipeId_t                             CmdPipe;                            /**< \brief Command pipe ID               */
    
    CFE_TBL_Handle_t                            DefEepromTableHandle;               /**< \brief Handle to the Eeprom definition table*/
    CFE_TBL_Handle_t                            ResEepromTableHandle;               /**< \brief Handle to the Eeprom results table*/
            
    CFE_TBL_Handle_t                            DefMemoryTableHandle;               /**< \brief Handle to the Memory definition table*/
    CFE_TBL_Handle_t                            ResMemoryTableHandle;               /**< \brief Handle to the Memory results table*/

    CFE_TBL_Handle_t                            DefTablesTableHandle;               /**< \brief Handle to the Tables definition table*/
    CFE_TBL_Handle_t                            ResTablesTableHandle;               /**< \brief Handle to the Tables results table*/
    
    CFE_TBL_Handle_t                            DefAppTableHandle;                  /**< \brief Handle to the Apps definition table*/
    CFE_TBL_Handle_t                            ResAppTableHandle;                  /**< \brief Hanlde to the Apps results table */
    
    CS_Def_EepromMemory_Table_Entry_t         * DefEepromTblPtr;                    /**< \brief Pointer to the Eeprom definition table */
    CS_Res_EepromMemory_Table_Entry_t         * ResEepromTblPtr;                    /**< \brief Pointer to the Eeprom results table */
    
    CS_Def_EepromMemory_Table_Entry_t         * DefMemoryTblPtr;                    /**< \brief Pointer to the Memory definition table */
    CS_Res_EepromMemory_Table_Entry_t         * ResMemoryTblPtr;                    /**< \brief Pointer to the Memory results table */
    
    CS_Def_Tables_Table_Entry_t               * DefTablesTblPtr;                    /**< \brief Pointer to the Tables definition table */
    CS_Res_Tables_Table_Entry_t               * ResTablesTblPtr;                    /**< \brief Pointer to the Tables results table */
    
    CS_Def_App_Table_Entry_t                  * DefAppTblPtr;                       /**< \brief Pointer to the Apps definition table */
    CS_Res_App_Table_Entry_t                  * ResAppTblPtr;                       /**< \brief Pointer to the Apps results table */
    
    CS_Res_EepromMemory_Table_Entry_t           OSCodeSeg;                          /**< \brief OS code segment 'table' */
    CS_Res_EepromMemory_Table_Entry_t           CfeCoreCodeSeg;                     /**< \brief cFE core code segment 'table' */
    
    CS_Def_EepromMemory_Table_Entry_t           DefaultEepromDefTable[CS_MAX_NUM_EEPROM_TABLE_ENTRIES];     /**< \brief Default Eeprom definition table */
    CS_Def_EepromMemory_Table_Entry_t           DefaultMemoryDefTable[CS_MAX_NUM_MEMORY_TABLE_ENTRIES];     /**< \brief Default Memory definition table */
    CS_Def_Tables_Table_Entry_t                 DefaultTablesDefTable[CS_MAX_NUM_TABLES_TABLE_ENTRIES];     /**< \brief Default Tables definition table */
    CS_Def_App_Table_Entry_t                    DefaultAppDefTable[CS_MAX_NUM_APP_TABLE_ENTRIES];           /**< \brief Default Apps definition table */
    
    /* The following pointers locate the results for CS tables that get checksummed because they are listed in the CS Tables table */
	
    CS_Res_Tables_Table_Entry_t               * EepResTablesTblPtr;        /**< \brief Pointer to CS Tables table results entry for the CS eeprom table */
    CS_Res_Tables_Table_Entry_t               * MemResTablesTblPtr;        /**< \brief Pointer to CS Tables table results entry for the CS memory table */
    CS_Res_Tables_Table_Entry_t               * AppResTablesTblPtr;        /**< \brief Pointer to CS Tables table results entry for the CS apps table */
    CS_Res_Tables_Table_Entry_t               * TblResTablesTblPtr;        /**< \brief Pointer to CS Tables table results entry for the CS Tables table */
    
    
#if (CS_PRESERVE_STATES_ON_PROCESSOR_RESET == true)
    CFE_ES_CDSHandle_t                        DataStoreHandle;            /**< \brief Handle to critical data store created by CS */
#endif
  
} CS_AppData_t;

/**************************************************************************
 **
 ** Exported data
 **
 **************************************************************************/
/**
** \brief Extern the CS_AppData so all CS files can use it 
*/
extern CS_AppData_t             CS_AppData;

/**************************************************************************
 **
 **  Function Prototypes
 **
 **************************************************************************/
/************************************************************************/
/** \brief CFS Checksum (CS) application entry point
 **  
 **  \par Description
 **       Checksum application entry point and main process loop.
 **
 **  \par Assumptions, External Events, and Notes:
 **       None
 **       
 *************************************************************************/
void CS_AppMain(void);


#if (CS_PRESERVE_STATES_ON_PROCESSOR_RESET == true)
/************************************************************************/
/** \brief CFS Checksum (CS) Critical Data Store Update
 **  
 **  \par Description
 **       Checksum application entry point and main process loop.
 **
 **  \par Assumptions, External Events, and Notes:
 **       None
 **       
 *************************************************************************/
void CS_UpdateCDS(void);
#endif /* #if (CS_PRESERVE_STATES_ON_PROCESSOR_RESET == true) */

#endif /* _cs_app_ */

/************************/
/*  End of File Comment */
/************************/
