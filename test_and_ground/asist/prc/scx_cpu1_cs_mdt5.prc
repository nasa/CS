PROC scx_cpu1_cs_mdt5
;*******************************************************************************
;  Test Name:  cs_mdt5
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	The purpose of this procedure is to generate the User Memory Definition
;	Table for the Checksum Application used by the cs_reset procedure.
;
;  Requirements Tested:
;	None
;
;  Prerequisite Conditions
;	The TST_CS_MemTbl application must be executing for this procedure to
;	generate the appropriate EEPROM Definition Table
;
;  Assumptions and Constraints
;	None.
;
;  Change History
;
;	Date		   Name		Description
;	07/19/11	Walt Moleski	Initial release.
;       09/19/12        Walt Moleski    Added write of new HK items and added a
;                                       define of the OS_MEM_TABLE_SIZE that
;                                       was removed from osconfig.h in 3.5.0.0
;       03/01/17        Walt Moleski    Updated for CS 2.4.0.0 using CPU1 for
;                                       commanding and added a hostCPU variable
;                                       for the utility procs to connect to the
;                                       proper host IP address. Changed define
;                                       of OS_MEM_TABLE_SIZE to MEM_TABLE_SIZE.
;
;  Arguments
;	None.
;
;  Procedures Called
;	Name			Description
;      create_tbl_file_from_cvt Procedure that creates a load file from
;                               the specified arguments and cvt
;
;  Expected Test Results and Analysis
;
;**********************************************************************

local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

#include "osconfig.h"
#include "cs_msgdefs.h"
#include "cs_platform_cfg.h"
#include "cs_tbldefs.h"

%liv (log_procedure) = logging

#define MEM_TABLE_SIZE       10

;**********************************************************************
; Define local variables
;**********************************************************************
LOCAL defTblId, defPktId
local CSAppName = "CS"
local ramDir = "RAM:0"
local hostCPU = "CPU3"
local memDefTblName = CSAppName & "." & CS_DEF_MEMORY_TABLE_NAME

;;; Set the pkt and app IDs for the tables based upon the cpu being used
;; CPU1 is the default
defTblId = "0FAD"
defPktId = 4013

write ";*********************************************************************"
write ";  Define the Memory Definition Table "
write ";********************************************************************"
;; States are 0=CS_STATE_EMPTY; 1=CS_STATE_ENABLED; 2=CS_STATE_DISABLED;
;;            3=CS_STATE_UNDEFINED
local eepromEntry = 0
local ramEntry = 0

;; Parse the memory table to find a valid RAM and EEPROM entry
for i=1 to MEM_TABLE_SIZE do
  if (p@SCX_CPU1_TST_CS_MemType[i] = "EEPROM") then
    eepromEntry = i
  elseif ((p@SCX_CPU1_TST_CS_MemType[i] = "RAM") .AND. ;;
          (ramEntry = 0)) then
    ramEntry = i
  endif
enddo

local eeQuarterSize = SCX_CPU1_TST_CS_Size[eepromEntry] / 4
local eeHalfSize = SCX_CPU1_TST_CS_Size[eepromEntry] / 2
local ramQuarterSize = SCX_CPU1_TST_CS_Size[ramEntry] / 4
local ramHalfSize = SCX_CPU1_TST_CS_Size[ramEntry] / 2

write "; RAM Entry found at index = ", ramEntry
write "; RAM Entry found at index = ", eepromEntry

write "; EEPROM Start address = ", SCX_CPU1_TST_CS_StartAddr[eepromEntry]
write "; EEPROM Size = ", SCX_CPU1_TST_CS_Size[eepromEntry]
write "; RAM Start address = ", SCX_CPU1_TST_CS_StartAddr[ramEntry]
write "; RAM Size = ", SCX_CPU1_TST_CS_Size[ramEntry]
write "; EEQuarterSize = ",eeQuarterSize
write "; EEHalfSize = ",eeHalfSize
write "; RAMQuarterSize = ",ramQuarterSize
write "; RAMHalfSize = ",ramHalfSize

SCX_CPU1_CS_MEM_DEF_TABLE[0].State = CS_STATE_ENABLED
SCX_CPU1_CS_MEM_DEF_TABLE[0].StartAddr = SCX_CPU1_TST_CS_StartAddr[ramEntry]
SCX_CPU1_CS_MEM_DEF_TABLE[1].State = CS_STATE_DISABLED
SCX_CPU1_CS_MEM_DEF_TABLE[1].StartAddr = SCX_CPU1_TST_CS_StartAddr[ramEntry] + ramQuarterSize
SCX_CPU1_CS_MEM_DEF_TABLE[1].NumBytes = ramHalfSize
SCX_CPU1_CS_MEM_DEF_TABLE[2].State = CS_STATE_ENABLED
SCX_CPU1_CS_MEM_DEF_TABLE[2].StartAddr = SCX_CPU1_TST_CS_StartAddr[ramEntry] + ramHalfSize
SCX_CPU1_CS_MEM_DEF_TABLE[3].State = CS_STATE_DISABLED
SCX_CPU1_CS_MEM_DEF_TABLE[3].StartAddr = SCX_CPU1_TST_CS_StartAddr[ramEntry]
SCX_CPU1_CS_MEM_DEF_TABLE[3].NumBytes = SCX_CPU1_TST_CS_Size[ramEntry]
SCX_CPU1_CS_MEM_DEF_TABLE[4].State = CS_STATE_ENABLED
SCX_CPU1_CS_MEM_DEF_TABLE[4].StartAddr = SCX_CPU1_TST_CS_StartAddr[eepromEntry]
SCX_CPU1_CS_MEM_DEF_TABLE[5].State = CS_STATE_DISABLED
SCX_CPU1_CS_MEM_DEF_TABLE[5].StartAddr = SCX_CPU1_TST_CS_StartAddr[eepromEntry] + eeQuarterSize
SCX_CPU1_CS_MEM_DEF_TABLE[5].NumBytes = eeHalfSize
SCX_CPU1_CS_MEM_DEF_TABLE[6].State = CS_STATE_ENABLED
SCX_CPU1_CS_MEM_DEF_TABLE[6].StartAddr = SCX_CPU1_TST_CS_StartAddr[eepromEntry] + eeHalfSize
SCX_CPU1_CS_MEM_DEF_TABLE[7].State = CS_STATE_DISABLED
SCX_CPU1_CS_MEM_DEF_TABLE[7].StartAddr = SCX_CPU1_TST_CS_StartAddr[eepromEntry]
SCX_CPU1_CS_MEM_DEF_TABLE[7].NumBytes = SCX_CPU1_TST_CS_Size[eepromEntry]

;; Clear out the remaining entries in the table
for i = 8 to CS_MAX_NUM_MEMORY_TABLE_ENTRIES-1 do
  SCX_CPU1_CS_MEM_DEF_TABLE[i].State = CS_STATE_EMPTY
  SCX_CPU1_CS_MEM_DEF_TABLE[i].StartAddr = 0
  SCX_CPU1_CS_MEM_DEF_TABLE[i].NumBytes = 0
enddo

;; Set the sizes of the enabled entries so that the calculation
;; will not take too long
if (ramQuarterSize > 4096) then
  SCX_CPU1_CS_MEM_DEF_TABLE[0].NumBytes = 2048
  SCX_CPU1_CS_MEM_DEF_TABLE[2].NumBytes = 4096
else
  SCX_CPU1_CS_MEM_DEF_TABLE[0].NumBytes = ramQuarterSize
  SCX_CPU1_CS_MEM_DEF_TABLE[2].NumBytes = ramQuarterSize
endif

if (eeQuarterSize > 4096) then
  SCX_CPU1_CS_MEM_DEF_TABLE[4].NumBytes = 2048
  SCX_CPU1_CS_MEM_DEF_TABLE[6].NumBytes = 4096
else
  SCX_CPU1_CS_MEM_DEF_TABLE[4].NumBytes = eeQuarterSize
  SCX_CPU1_CS_MEM_DEF_TABLE[6].NumBytes = eeQuarterSize
endif

local lastEntry = CS_MAX_NUM_MEMORY_TABLE_ENTRIES - 1
local endmnemonic = "SCX_CPU1_CS_MEM_DEF_TABLE[" & lastEntry & "].NumBytes"

;; Create the Table Load file
s create_tbl_file_from_cvt (hostCPU,defTblId,"User Memory Definition Table Load 3","usrmem_def_ld_3",memDefTblName,"SCX_CPU1_CS_MEM_DEF_TABLE[0].State",endmnemonic)

write ";*********************************************************************"
write ";  End procedure scx_cpu1_cs_mdt5                              "
write ";*********************************************************************"
ENDPROC
