PROC scx_cpu1_cs_reset2
;*******************************************************************************
;  Test Name:  cs_reset
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	This test verifies that the CFS Checksum (CS) application initializes
;	the appropriate data items when any initialization occurs (Application
;	Reset, Power-On Reset, or Processor Reset).
;
;	This test will only be executed if the <PLATFORM_DEFINED>
;	PRESERVE_STATES_ON_PROCESSOR_RESET Flag is set to False. 
;
;  Requirements Tested
;    CS2010    CS shall provide the ability to dump the baseline CRCs and
;		status for the non-volatile memory segments via a dump-only
;		table.
;    CS4008	CS shall provide the ability to dump the baseline CRCs and
;		status for the Application code segment memory segments via a
;		dump-only table.
;    CS5008	CS shall provide the ability to dump the baseline CRCs and
;		status for the tables via a dump-only table.
;    CS6008	CS shall provide the ability to dump the baseline CRCs and
;		status for all User-Defined Memory via a dump-only table.
;    CS9000	CS shall generate a housekeeping message containing the
;		following:
;			a) Valid Ground Command Counter
;			b) Ground Command Rejected Counter
;			c) Overall CRC enable/disable status
;			d) Total Non-Volatile Baseline CRC
;			e) OS code segment Baseline CRC
;			f) cFE code segment Baseline CRC
;			g) Non-Volatile CRC Miscompare Counter
;			h) OS Code Segment CRC Miscompare Counter
;			i) cFE Code Segment CRC Miscompare Counter
;			j) Application CRC Miscompare Counter
;			k) Table CRC Miscompare Counter
;			l) User-Defined Memory CRC Miscompare Counter
;			m) Last One Shot Address
;			n) Last One Shot Size
;			o) Last One Shot Checksum
;			p) Checksum Pass Counter (number of passes thru all of
;			   the checksum areas)
;			q) Current Checksum Region (Non-Volatile, OS code
;			   segment, cFE Code Segment etc)
;			r) Non-Volatile CRC enable/disable status
;			s) OS Code Segment CRC enable/disable status
;			t) cFE Code Segment CRC enable/disable status
;			u) Application CRC enable/disable status
;			v) Table CRC enable/disable status
;			w) User-Defined Memory CRC enable/disable status
;			x) Last One Shot Rate
;			y) Recompute In Progress Flag
;			z) One Shot In Progress Flag
;    CS9001	Upon initialization of the CS Application (cFE Power On, cFE
;		Processor Reset or CS Application Reset), CS shall initialize
;		the following data to Zero:
;			a) Valid Ground Command Counter
;			b) Ground Command Rejected Counter
;			c) Non-Volatile CRC Miscompare Counter
;			d) OS Code Segment CRC Miscompare Counter
;			e) cFE Code Segment CRC Miscompare Counter
;			f) Application CRC Miscompare Counter
;			g) Table CRC Miscompare Counter
;			h) User-Defined Memory CRC Miscompare Counter
;			i) Recompute In Progress Flag
;			j) One Shot In Progress Flag
;    CS9002	Upon a cFE Power On Reset, if the segment's <PLATFORM_DEFINED>
;		Power-On initialization state is set to Enabled, CS shall
;		compute the baseline CRCs for the following regions:
;			a) OS code segment
;			b) cFE code segment
;    CS9003	Upon a cFE Power On Reset, if the Non-Volatile
;		<PLATFORM_DEFINED> Power-On initialization state is set to
;		Enabled, CS shall compute the baseline CRCs for Non-volatile
;		segments based on the corresponding table definition for up to
;		<PLATFORM_DEFINED> segments.
;    CS9004	Upon a cFE Power On Reset, if the Non-Volatile
;		<PLATFORM_DEFINED> Power-On initialization state is set to
;		Enabled, CS shall compute the baseline CRC for the total of all
;		of non-volatile segments.
;    CS9005	Upon a cFE Power On Reset, if the Application <PLATFORM_DEFINED>
;		Power-On initialization state is set to Enabled, CS shall
;		compute baseline CRCs for the Application code segments region
;		based on the corresponding table definition for up to
;		<PLATFORM_DEFINED> Applications.
;    CS9006	Upon a cFE Power On Reset, if the Tables <PLATFORM_DEFINED>
;		Power-On initialization state is set to Enabled, CS shall
;		compute baseline CRCs for the tables specified in the
;		corresponding table definition for up to <PLATFORM_DEFINED>
;		tables.
;    CS9007	Upon a cFE Power On Reset, if the User-Defined Memory
;		<PLATFORM_DEFINED> Power-on initialization state is set to
;		Enabled, CS shall compute baseline CRCs for the User-defined
;		memory region based on the corresponding table definition for up
;		to <PLATFORM_DEFINED> memory segments.
;    CS9009     Upon a cFE Processor Reset or CS Application Reset, if the
;               <PLATFORM_DEFINED> PRESERVE_STATES_ON_PROCESSOR_RESET Flag is
;               set to False, CS shall perform initialization in accordance with
;               a Power On reset.
;
;  Prerequisite Conditions
;	The CFS is up and running and ready to accept commands.
;	The CS commands and telemetry items exist in the GSE database.
;	The display pages exist for the CS Housekeeping and the dump-only
;		Application Code Segment Result Table.
;	The Application Code Segment definition table exists defining the
;		segments to checksum.
;	A CS Test application (TST_CS) exists in order to fully test the CS
;		Application.
;
;  Assumptions and Constraints
;	None.
;
;  Change History
;	Date		   Name		Description
;	03/23/17	Walt Moleski	Initial procedure copied from ce_reset
;                                       snd modified to test the FALSE condition
;                                       of the configuration parameters.
;
;  Arguments
;	None.
;
;  Procedures Called
;	Name			Description
;       ut_tlmwait	Wait for a specified telemetry point to update to a
;			specified value. 
;       ut_pfindicate	Print the pass fail status of a particular requirement
;			number.
;       ut_setupevents	Performs setup to verify that a particular event
;			message was received by ASIST.
;	ut_setrequirements	A directive to set the status of the cFE
;			requirements array.
;
;  Expected Test Results and Analysis
;
;**********************************************************************

local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

#include "ut_statusdefs.h"
#include "ut_cfe_info.h"
#include "cfe_platform_cfg.h"
#include "osconfig.h"
#include "cfe_evs_events.h"
#include "cfe_es_events.h"
#include "cfe_tbl_events.h"
#include "to_lab_events.h"
#include "cs_msgdefs.h"
#include "cs_platform_cfg.h"
#include "cs_events.h"
#include "cs_tbldefs.h"
#include "tst_cs_events.h"
#include "tst_cs_msgdefs.h"
#include "tst_tbl_events.h"

%liv (log_procedure) = logging

#define CS_2010		0
#define CS_4008		1
#define CS_5008		2
#define CS_6008		3
#define CS_9000		4
#define CS_9001		5
#define CS_9002		6
#define CS_9003		7
#define CS_9004		8
#define CS_9005		9
#define CS_9006		10
#define CS_9007		11
#define CS_9009		12


global ut_req_array_size = 12
global ut_requirement[0 .. ut_req_array_size]

for i = 0 to ut_req_array_size DO
  ut_requirement[i] = "U"
enddo

;**********************************************************************
; Set the local values
;**********************************************************************
local cfe_requirements[0 .. ut_req_array_size] = ["CS_2010","CS_4008","CS_5008","CS_6008","CS_9000","CS_9001","CS_9002","CS_9003","CS_9004","CS_9005","CS_9006","CS_9007","CS_9009" ]

;**********************************************************************
; Define local variables
;**********************************************************************
LOCAL appDefTblId, appDefPktId, appResTblId, appResPktId
LOCAL tblDefTblId, tblDefPktId, tblResTblId, tblResPktId
LOCAL eeDefTblId, eeDefPktId, eeResTblId, eeResPktId
LOCAL usrDefTblId, usrDefPktId, usrResTblId, usrResPktId
local i, cmdCtr, stream
local osCRC, cFECRC, eepromCRC
local appCRCs[0 .. CS_MAX_NUM_APP_TABLE_ENTRIES-1]
local tblCRCs[0 .. CS_MAX_NUM_TABLES_TABLE_ENTRIES-1]
local eeCRCs[0 .. CS_MAX_NUM_EEPROM_TABLE_ENTRIES-1]
local usrCRCs[0 .. CS_MAX_NUM_MEMORY_TABLE_ENTRIES-1]
local CSAppName = "CS" 
local ramDir = "RAM:0"
local hostCPU = "CPU3"
local appDefTblName = CSAppName & "." & CS_DEF_APP_TABLE_NAME
local appResTblName = CSAppName & "." & CS_RESULTS_APP_TABLE_NAME
local eeDefTblName = CSAppName & "." & CS_DEF_EEPROM_TABLE_NAME
local eeResTblName = CSAppName & "." & CS_RESULTS_EEPROM_TABLE_NAME
local tblDefTblName = CSAppName & "." & CS_DEF_TABLES_TABLE_NAME
local tblResTblName = CSAppName & "." & CS_RESULTS_TABLES_TABLE_NAME
local memDefTblName = CSAppName & "." & CS_DEF_MEMORY_TABLE_NAME
local memResTblName = CSAppName & "." & CS_RESULTS_MEMORY_TABLE_NAME

;; Set the pkt and app IDs for the tables based upon the cpu being used
;; There are 4 sets of tables for Applications, Tables, EEPROM and User-defined
;; memory
;; CPU1 is the default
appDefTblId = "0FAF"
appResTblId = "0FB3"
appDefPktId = 4015
appResPktId = 4019
tblDefTblId = "0FAE"
tblResTblId = "0FB2"
tblDefPktId = 4014
tblResPktId = 4018
eeDefTblId = "0FAC"
eeResTblId = "0FB0"
eeDefPktId = 4012
eeResPktId = 4016
usrDefTblId = "0FAD"
usrResTblId = "0FB1"
usrDefPktId = 4013
usrResPktId = 4017

write ";*********************************************************************"
write ";  Step 1.0: Checksum Reset Test Setup."
write ";*********************************************************************"
;; Check to see if the PRESERVE State is true. If not, end the test.
if (CS_PRESERVE_STATES_ON_PROCESSOR_RESET = 1) then
  write "** Preserve State is TRUE **"
  write "** State must be FALSE to execute this test. Ending test. **"
  goto procterm
endif

write ";*********************************************************************"
write ";  Step 1.1: Command a Power-on Reset on CPU1."
write ";*********************************************************************"
/SCX_CPU1_ES_POWERONRESET
wait 10
       
close_data_center
wait 60

cfe_startup {hostCPU}
wait 5

write ";*********************************************************************"
write ";  Step 1.2: Download the default Application Code Segment Definition "
write ";  Table file in order to use it during cleanup."
write ";********************************************************************"
;; use ftp utilities to get the file
;; Parse the filename configuration parameter for the default table
local appTableFileName = CS_DEF_APP_TABLE_FILENAME
local slashLoc = %locate(appTableFileName,"/")
write "==> Default Application Code Segment Table filename config param = '",appTableFileName,"'"

;; loop until all slashes are found
while (slashLoc <> 0) do
  appTableFileName = %substring(appTableFileName,slashLoc+1,%length(appTableFileName))
  slashLoc = %locate(appTableFileName,"/")
enddo
write "==> Table filename ONLY = '",appTableFileName,"'"

;; Get the file in order to restore it in the cleanup steps
s ftp_file ("CF:0/apps",appTableFileName,"cs_app_orig_tbl.tbl",hostCPU,"G")

write ";*********************************************************************"
write ";  Step 1.3: Download the default Table Definition Table file in order"
write ";  to use it during cleanup."
write ";********************************************************************"
;; use ftp utilities to get the file
;; CS_DEF_TABLES_TABLE_FILENAME -> full path file spec.
;; Parse the filename configuration parameter for the default table
local tblTableFileName = CS_DEF_TABLES_TABLE_FILENAME
slashLoc = %locate(tblTableFileName,"/")
write "==> Default Table Definition Table filename config param = '",tblTableFileName,"'"

;; loop until all slashes are found
while (slashLoc <> 0) do
  tblTableFileName = %substring(tblTableFileName,slashLoc+1,%length(tblTableFileName))
  slashLoc = %locate(tblTableFileName,"/")
enddo
write "==> Table filename ONLY = '",tblTableFileName,"'"

;; Get the file in order to restore it in the cleanup steps
s ftp_file ("CF:0/apps",tblTableFileName,"cs_tbl_orig_tbl.tbl",hostCPU,"G")

write ";*********************************************************************"
write ";  Step 1.4: Download the default EEPROM Definition Table file in order"
write ";  to use it during cleanup."
write ";********************************************************************"
;; use ftp utilities to get the file
;; CS_DEF_EEPROM_TABLE_FILENAME -> full path file spec.
;; Parse the filename configuration parameter for the default table
local eeTableFileName = CS_DEF_EEPROM_TABLE_FILENAME
slashLoc = %locate(eeTableFileName,"/")
write "==> Default EEPROM Definition Table filename config param = '",eeTableFileName,"'"

;; loop until all slashes are found
while (slashLoc <> 0) do
  eeTableFileName = %substring(eeTableFileName,slashLoc+1,%length(eeTableFileName))
  slashLoc = %locate(eeTableFileName,"/")
enddo
write "==> Table filename ONLY = '",eeTableFileName,"'"

;; Get the file in order to restore it in the cleanup steps
s ftp_file ("CF:0/apps",eeTableFileName,"cs_eeprom_orig_tbl.tbl",hostCPU,"G")

write ";*********************************************************************"
write ";  Step 1.5: Download the default Memory Definition Table file in order"
write ";  to use it during cleanup."
write ";********************************************************************"
;; use ftp utilities to get the file
;; CS_DEF_MEMORY_TABLE_FILENAME -> full path file spec.
;; Parse the filename configuration parameter for the default table
local memTableFileName = CS_DEF_MEMORY_TABLE_FILENAME
slashLoc = %locate(memTableFileName,"/")
write "==> Default Memory Definition Table filename config param = '",memTableFileName,"'"

;; loop until all slashes are found
while (slashLoc <> 0) do
  memTableFileName = %substring(memTableFileName,slashLoc+1,%length(memTableFileName))
  slashLoc = %locate(memTableFileName,"/")
enddo
write "==> Table filename ONLY = '",memTableFileName,"'"

;; Get the file in order to restore it in the cleanup steps
s ftp_file ("CF:0/apps",memTableFileName,"cs_mem_orig_tbl.tbl",hostCPU,"G")

write ";**********************************************************************"
write ";  Step 1.6: Display the Housekeeping and Table Telemetry pages.       "
write ";**********************************************************************"
;; The Definition table pages are not really needed and are commented out
;; below. If you wish to view these pages, just uncomment them.
page SCX_CPU1_CS_HK
page SCX_CPU1_TST_CS_HK
;;page SCX_CPU1_CS_APP_DEF_TABLE
page SCX_CPU1_CS_APP_RESULTS_TBL
;;page SCX_CPU1_CS_TBL_DEF_TABLE
page SCX_CPU1_CS_TBL_RESULTS_TBL
;;page SCX_CPU1_CS_EEPROM_DEF_TABLE
page SCX_CPU1_CS_EEPROM_RESULTS_TBL
;;page SCX_CPU1_CS_MEM_DEF_TABLE
page SCX_CPU1_CS_MEM_RESULTS_TBL

write ";*********************************************************************"
write ";  Step 1.7: Start the TST_CS_MemTbl application in order to setup   "
write ";  the OS_Memory_Table for the Checksum (CS) application. "
write ";********************************************************************"
ut_setupevents "SCX", "CPU1", "CFE_ES", CFE_ES_START_INF_EID, "INFO", 1
ut_setupevents "SCX", "CPU1", "TST_CS_MEMTBL", 1, "INFO", 2

s load_start_app ("TST_CS_MEMTBL",hostCPU,"TST_CS_MemTblMain")

;;  Wait for app startup events
ut_tlmwait SCX_CPU1_find_event[2].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
  if (SCX_CPU1_find_event[1].num_found_messages = 1) then
    write "<*> Passed - TST_CS_MEMTBL Application Started"
  else
    write "<!> Failed - CFE_ES start Event Message for TST_CS_MEMTBL not received."
    write "Event Message count = ",SCX_CPU1_find_event[1].num_found_messages
  endif
else
  write "<!> Failed - TST_CS_MEMTBL Application start Event Message not received."
endif

;; These are the TST_CS HK Packet IDs since this app sends this packet
;; CPU1 is the default
stream = x'0930'

/SCX_CPU1_TO_ADDPACKET STREAM=stream PKT_SIZE=X'0' PRIORITY=X'0' RELIABILITY=X'0' BUFLIMIT=x'4'
wait 10

write ";*********************************************************************"
write ";  Step 1.8: Create & upload the EEPROM Definition Table file to be   "
write ";  used during this test."
write ";********************************************************************"
s scx_cpu1_cs_edt1
wait 5

;; Upload the file created above as the default
;; Non-volatile (EEPROM) Definition Table load file
;;s ftp_file ("CF:0/apps","eeprom_def_ld_1","cs_eepromtbl.tbl",hostCPU,"P")
s ftp_file ("CF:0/apps","eeprom_def_ld_1",eeTableFileName,hostCPU,"P")
wait 10

write ";*********************************************************************"
write ";  Step 1.9: Create & upload the Application Code Segment Definition "
write ";  Table file to be used during this test."
write ";********************************************************************"
s scx_cpu1_cs_adt1
wait 5

;; Upload the file created above as the default
;; Application Definition Table load file
;;s ftp_file ("CF:0/apps","app_def_tbl_ld_1","cs_apptbl.tbl",hostCPU,"P")
s ftp_file ("CF:0/apps","app_def_tbl_ld_1",appTableFileName,hostCPU,"P")
wait 10

write ";*********************************************************************"
write ";  Step 1.10: Create & upload the Tables Definition Table file to be   "
write ";  used during this test."
write ";********************************************************************"
s scx_cpu1_cs_tdt5
wait 5

;; Tables Definition Table load file
;;s ftp_file ("CF:0/apps","tbl_def_tbl_ld_3","cs_tablestbl.tbl",hostCPU,"P")
s ftp_file ("CF:0/apps","tbl_def_tbl_ld_3",tblTableFileName,hostCPU,"P")
wait 10

write ";*********************************************************************"
write ";  Step 1.11: Create & upload the Memory Definition Table file to be  "
write ";  used during this test."
write ";********************************************************************"
s scx_cpu1_cs_mdt5
wait 5

;; Upload the file created above as the default 
;;s ftp_file ("CF:0/apps","usrmem_def_ld_3","cs_memorytbl.tbl",hostCPU,"P")
s ftp_file ("CF:0/apps","usrmem_def_ld_3",memTableFileName,hostCPU,"P")
wait 10

write ";*********************************************************************"
write ";  Step 1.12: Start the applications in order for the load files created"
write ";  above to successfully pass validation and load. "
write ";********************************************************************"
s scx_cpu1_cs_start_apps("1.12")
wait 5

;; Verify the Housekeeping Packet is being generated
;; Set the DS HK packet ID based upon the cpu being used
local hkPktId = "p0A4"

;; Verify the HK Packet is getting generated by waiting for the
;; sequencecount to increment twice
local seqTlmItem = hkPktId & "scnt"
local currSCnt = {seqTlmItem}
local expectedSCnt = currSCnt + 2

ut_tlmwait {seqTlmItem}, {expectedSCnt}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (9000) - Housekeeping packet is being generated."
  ut_setrequirements CS_9000, "P"
else
  write "<!> Failed (9000) - Housekeeping packet sequence count did not increment. Housekeeping packet is not being recieved."
  ut_setrequirements CS_9000, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 1.13: Start the other applications required for this test. "
write ";********************************************************************"
ut_setupevents "SCX", "CPU1", "CFE_ES", CFE_ES_START_INF_EID, "INFO", 1
ut_setupevents "SCX", "CPU1", "TST_TBL", TST_TBL_INIT_INF_EID, "INFO", 2

s load_start_app ("TST_TBL",hostCPU)

; Wait for app startup events
ut_tlmwait SCX_CPU1_find_event[2].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
  if (SCX_CPU1_find_event[1].num_found_messages = 1) then
    write "<*> Passed - TST_TBL Application Started"
  else
    write "<!> Failed - CFE_ES start Event Message for TST_TBL not received."
    write "Event Message count = ",SCX_CPU1_find_event[1].num_found_messages
  endif
else
  write "<!> Failed - TST_TBL Application start Event Message not received."
endif

write ";*********************************************************************"
write ";  Step 1.14: Verify that the CS Housekeeping telemetry items are "
write ";  initialized to zero (0). "
write ";*********************************************************************"
;; Check the HK tlm items to see if they are 0 or NULL
;; the TST_CS application sends its HK packet
if (SCX_CPU1_CS_CMDPC = 0) AND (SCX_CPU1_CS_CMDEC = 0) AND ;;
   (SCX_CPU1_CS_EepromEC = 0) AND (SCX_CPU1_CS_MemoryEC = 0) AND ;;
   (SCX_CPU1_CS_TableEC = 0) AND (SCX_CPU1_CS_AppEC = 0) AND ;;
   (SCX_CPU1_CS_CFECoreEC = 0) AND (SCX_CPU1_CS_OSEC = 0) THEN
  write "<*> Passed (9001) - Housekeeping telemetry initialized properly."
  ut_setrequirements CS_9001, "P"
else
  write "<!> Failed (9001) - Housekeeping telemetry NOT initialized at startup."
  write "  CMDPC       = ",SCX_CPU1_CS_CMDPC
  write "  CMDEC       = ",SCX_CPU1_CS_CMDEC
  write "  EEPROM MC   = ",SCX_CPU1_CS_EEPROMEC
  write "  Memory MC   = ",SCX_CPU1_CS_MemoryEC
  write "  Table MC    = ",SCX_CPU1_CS_TABLEEC
  write "  App MC      = ",SCX_CPU1_CS_AppEC
  write "  cFE Core MC = ",SCX_CPU1_CS_CFECOREEC
  write "  OS MC       = ",SCX_CPU1_CS_OSEC
  ut_setrequirements CS_9001, "F"
endif

wait 5

write ";*********************************************************************"
write ";  Step 1.15: Check the Checksum States to verify they are correct. "
write ";*********************************************************************"
;; Check the POWERON States are set 
;; OS State
if ((p@SCX_CPU1_CS_OSState = "Enabled") AND (CS_OSCS_CHECKSUM_STATE = 1)) then
  write "<*> Passed - OS State as expected after Power-ON reset."
elseif ((p@SCX_CPU1_CS_OSState = "Disabled") AND ;;
	(CS_OSCS_CHECKSUM_STATE = 2)) then
  write "<*> Passed - OS State as expected after Power-ON reset."
else
  write "<!> Failed - OS State not set as expected after Power-ON reset."
endif

;; cFE Core State
if ((p@SCX_CPU1_CS_CFECoreState = "Enabled") AND ;;
    (CS_CFECORE_CHECKSUM_STATE = 1)) then
  write "<*> Passed - cFE Core State as expected after Power-On reset."
elseif ((p@SCX_CPU1_CS_CFECoreState = "Disabled") AND ;;
    (CS_CFECORE_CHECKSUM_STATE = 2)) then
  write "<*> Passed - cFE Core State as expected after Power-On reset."
else
  write "<!> Failed - cFE Core State not set as expected after Power-On reset."
endif

;; EEPROM State
if ((p@SCX_CPU1_CS_EepromState = "Enabled") AND ;;
    (CS_EEPROM_TBL_POWERON_STATE = 1)) then
  write "<*> Passed - EEPROM State as expected after Power-On reset."
elseif ((p@SCX_CPU1_CS_EepromState = "Disabled") AND ;;
    (CS_EEPROM_TBL_POWERON_STATE = 2)) then
  write "<*> Passed - EEPROM State as expected after Power-On reset."
else
  write "<!> Failed - EEPROM State not set as expected after Power-On reset."
endif

;; User-Defined Memory State
if ((p@SCX_CPU1_CS_MemoryState = "Enabled") AND ;;
    (CS_MEMORY_TBL_POWERON_STATE = 1)) then
  write "<*> Passed - User-Defined Memory State as expected after Power-On reset."
elseif ((p@SCX_CPU1_CS_MemoryState = "Disabled") AND ;;
    (CS_MEMORY_TBL_POWERON_STATE = 2)) then
  write "<*> Passed - User-Defined Memory State as expected after Power-On reset."
else
  write "<!> Failed - User-Defined Memory State not set as expected after Power-On reset."
endif

;; Applications State
if ((p@SCX_CPU1_CS_AppState = "Enabled") AND ;;
    (CS_APPS_TBL_POWERON_STATE = 1)) then
  write "<*> Passed - Application State as expected after Power-On reset."
elseif ((p@SCX_CPU1_CS_AppState = "Disabled") AND ;;
    (CS_APPS_TBL_POWERON_STATE = 2)) then
  write "<*> Passed - Application State as expected after Power-On reset."
else
  write "<!> Failed - Application State not set as expected after Power-On reset."
endif

;; Tables State
if ((p@SCX_CPU1_CS_TableState = "Enabled") AND ;;
    (CS_TABLES_TBL_POWERON_STATE = 1)) then
  write "<*> Passed - Tables State as expected after Power-On reset."
elseif ((p@SCX_CPU1_CS_TableState = "Disabled") AND ;;
    (CS_TABLES_TBL_POWERON_STATE = 2)) then
  write "<*> Passed - Tables State as expected after reset."
else
  write "<!> Failed - Tables State not set as expected after Power-On reset."
endif

write ";*********************************************************************"
write ";  Step 1.16: Wait until the Pass Counter indicates that it has made a"
write ";  complete pass through the checksum tables. If this takes longer than"
write ";  300 seconds, then time-out. "
write ";*********************************************************************"
if (SCX_CPU1_CS_PASSCTR = 0) then
  ut_tlmwait SCX_CPU1_CS_PASSCTR, 1, 300
else
  write ";** CS has already performed at least 1 complete pass."
endif 

write ";*********************************************************************"
write ";  Step 2.0: Power-On Reset Test."
write ";*********************************************************************"
write ";  Step 2.1: Modify the OS and cFE code segment baseline CRCs."
write ";*********************************************************************"
;; OS State
if (p@SCX_CPU1_CS_OSState = "Disabled") then
  write ";** Skipping because OS State is Disabled."
  goto check_cFE_State
endif

;; Use the TST_CS app to corrupt the OS CRC
ut_setupevents "SCX","CPU1","TST_CS",TST_CS_CORRUPT_OS_CRC_INF_EID, "INFO", 1
ut_setupevents "SCX","CPU1",{CSAppName},CS_OS_MISCOMPARE_ERR_EID, "ERROR", 2

/SCX_CPU1_TST_CS_CorruptOSCRC
wait 5

;; Check for OS the event message
ut_tlmwait SCX_CPU1_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Expected Event Msg ",TST_CS_CORRUPT_OS_CRC_INF_EID," rcv'd."
else
  write "<!> Failed - Event message ", SCX_CPU1_evs_eventid," rcv'd. Expected Event Msg ",TST_CS_CORRUPT_OS_CRC_INF_EID,"."
endif

;; Check for the OS event message
ut_tlmwait SCX_CPU1_find_event[2].num_found_messages, 1, 300
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - OS Miscompare Event ID=",CS_OS_MISCOMPARE_ERR_EID," rcv'd."
else
  write "<!> Failed - OS Miscompare Event was not received. Time-out occurred."
endif

check_cFE_State:
;; cFE Core State
if (p@SCX_CPU1_CS_CFECoreState = "Disabled") then
  write ";** Skipping because cFE Core State is Disabled."
  goto step_2_2
endif

;; Use the TST_CS app to corrupt the cFE CRC
ut_setupevents "SCX","CPU1","TST_CS",TST_CS_CORRUPT_CFE_CRC_INF_EID, "INFO", 3
ut_setupevents "SCX","CPU1",{CSAppName},CS_CFECORE_MISCOMPARE_ERR_EID,"ERROR", 4

/SCX_CPU1_TST_CS_CorruptCFECRC
wait 5

;; Check for the cFE event message
ut_tlmwait SCX_CPU1_find_event[3].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Expected Event Msg ",TST_CS_CORRUPT_CFE_CRC_INF_EID," rcv'd."
else
  write "<!> Failed - Event message ", SCX_CPU1_evs_eventid," rcv'd. Expected Event Msg ",TST_CS_CORRUPT_CFE_CRC_INF_EID,"."
endif

;; Check for the CFE event message
ut_tlmwait SCX_CPU1_find_event[4].num_found_messages, 1, 300
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - cFE Core Miscompare Event ID=",CS_CFECORE_MISCOMPARE_ERR_EID," rcv'd."
else
  write "<!> Failed - cFE Core Miscompare Event was not received. Time-out occurred."
endif

wait 5

step_2_2:
write ";*********************************************************************"
write ";  Step 2.2: Dump the Non-Volatile Code Segment Results Table."
write ";*********************************************************************"
;; Non-Volatile Memory State
if (p@SCX_CPU1_CS_EepromState = "Disabled") then
  write ";** Skipping tests because Non-Volatile Memory State is Disabled."
  goto step_2_5
endif

cmdCtr = SCX_CPU1_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,eeResTblName,"A","cpu1_eerestbl2_2",hostCPU,eeResTblId)
wait 5

ut_tlmwait SCX_CPU1_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2010) - Dump of EEPROM Memory Results Table successful."
  ut_setrequirements CS_2010, "P"
else
  write "<!> Failed (2010) - Dump of EEPROM Memory Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_2010, "F"
endif

write ";*********************************************************************"
write ";  Step 2.3: Corrupt the Non-volatile Baseline CRCs in order to       "
write ";  determine if they are recalculated upon reset. "
write ";*********************************************************************"
;; Using the TST_CS app, corrupt the CRCs that are enabled
;; Loop for each valid entry in the results table
for i = 0 to CS_MAX_NUM_EEPROM_TABLE_ENTRIES-1 DO
  if (p@SCX_CPU1_CS_EEPROM_RESULT_TABLE[i].State = "Enabled") then
    ;; Send the command to corrupt this entry's CRC
    ut_setupevents "SCX", "CPU1", "TST_CS", TST_CS_CORRUPT_MEMORY_CRC_INF_EID, "INFO", 1

    /SCX_CPU1_TST_CS_CorruptMemCRC MemType=TST_CS_EEPROM_MEM EntryID=i
    wait 3

    ;; Check for the event message
    ut_tlmwait SCX_CPU1_find_event[1].num_found_messages, 1
    if (UT_TW_Status = UT_Success) then
      write "<*> Passed - Expected Event Msg ",TST_CS_CORRUPT_MEMORY_CRC_INF_EID," rcv'd."
    else
      write "<!> Failed - Event message ", SCX_CPU1_evs_eventid," rcv'd. Expected Event Msg ",TST_CS_CORRUPT_MEMORY_CRC_INF_EID,"."
    endif
  endif
enddo

write ";*********************************************************************"
write ";  Step 2.4: Dump the results table to ensure that the CRCs have been "
write ";  modified. "
write ";*********************************************************************"
cmdCtr = SCX_CPU1_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,eeResTblName,"A","cpu1_eerestbl2_4",hostCPU,eeResTblId)
wait 5

ut_tlmwait SCX_CPU1_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2010) - Dump of EEPROM Memory Results Table successful."
  ut_setrequirements CS_2010, "P"
else
  write "<!> Failed (2010) - Dump of EEPROM Memory Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_2010, "F"
endif

step_2_5:
write ";*********************************************************************"
write ";  Step 2.5: Dump the Application Code Segment Results Table."
write ";*********************************************************************"
;; Application State
if (p@SCX_CPU1_CS_AppState = "Disabled") then
  write ";** Skipping tests because Application State is Disabled."
  goto step_2_8
endif

cmdCtr = SCX_CPU1_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,appResTblName,"A","cpu1_apprestbl2_5",hostCPU,appResTblId)
wait 5

ut_tlmwait SCX_CPU1_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (4008) - Dump of App Results Table successful."
  ut_setrequirements CS_4008, "P"
else
  write "<!> Failed (4008) - Dump of App Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_4008, "F"
endif

write ";*********************************************************************"
write ";  Step 2.6: Corrupt the Application CRCs in order to determine if they"
write ";  are recalculated upon reset. "
write ";*********************************************************************"
;; Using the TST_CS app, corrupt the CRCs that are enabled
for i = 0 to CS_MAX_NUM_APP_TABLE_ENTRIES-1 DO
  if (p@SCX_CPU1_CS_APP_RESULT_TABLE[i].State = "Enabled") then
    ut_setupevents "SCX","CPU1","TST_CS",TST_CS_CORRUPT_APP_CRC_INF_EID,"INFO",1

    /SCX_CPU1_TST_CS_CorruptAppCRC AppName=SCX_CPU1_CS_APP_RESULT_TABLE[i].Name
    wait 3

    ;; Check for the event message
    ut_tlmwait SCX_CPU1_find_event[1].num_found_messages, 1
    if (UT_TW_Status = UT_Success) then
      write "<*> Passed - Expected Event Msg ",TST_CS_CORRUPT_APP_CRC_INF_EID," rcv'd."
    else
      write "<!> Failed - Event message ", SCX_CPU1_evs_eventid," rcv'd. Expected Event Msg ",TST_CS_CORRUPT_APP_CRC_INF_EID,"."
    endif
  endif
enddo

write ";*********************************************************************"
write ";  Step 2.7: Dump the results table to ensure that the CRCs have been "
write ";  modified. "
write ";*********************************************************************"
cmdCtr = SCX_CPU1_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,appResTblName,"A","cpu1_apprestbl2_7",hostCPU,appResTblId)
wait 5

ut_tlmwait SCX_CPU1_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (4008) - Dump of App Results Table successful."
  ut_setrequirements CS_4008, "P"
else
  write "<!> Failed (4008) - Dump of App Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_4008, "F"
endif

step_2_8:
write ";*********************************************************************"
write ";  Step 2.8: Dump the Table Results Table."
write ";*********************************************************************"
;; Table State
if (p@SCX_CPU1_CS_TableState = "Disabled") then
  write ";** Skipping tests because Table State is Disabled."
  goto step_2_11
endif

cmdCtr = SCX_CPU1_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,tblResTblName,"A","cpu1_tblrestbl2_8",hostCPU,tblResTblId)
wait 5

ut_tlmwait SCX_CPU1_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (5008) - Dump of Table Results Table successful."
  ut_setrequirements CS_5008, "P"
else
  write "<!> Failed (5008) - Dump of Table Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_5008, "F"
endif

write ";*********************************************************************"
write ";  Step 2.9: Corrupt the Table CRCs in order to determine if they"
write ";  are recalculated upon reset. "
write ";*********************************************************************"
;; Using the TST_CS app, corrupt the CRCs that are enabled
for i = 0 to CS_MAX_NUM_TABLES_TABLE_ENTRIES-1 DO
  if (p@SCX_CPU1_CS_TBL_RESULT_TABLE[i].State = "Enabled") then
    ut_setupevents "SCX", "CPU1", "TST_CS", TST_CS_CORRUPT_TABLE_CRC_INF_EID, "INFO", 1

    /SCX_CPU1_TST_CS_CorruptTblCRC TABLEName=SCX_CPU1_CS_TBL_RESULT_TABLE[i].Name
    wait 3

    ;; Check for the event message
    ut_tlmwait SCX_CPU1_find_event[1].num_found_messages, 1
    if (UT_TW_Status = UT_Success) then
      write "<*> Passed - Expected Event Msg ",TST_CS_CORRUPT_TABLE_CRC_INF_EID," rcv'd."
    else
      write "<!> Failed - Event message ", SCX_CPU1_evs_eventid," rcv'd. Expected Event Msg ",TST_CS_CORRUPT_TABLE_CRC_INF_EID,"."
    endif
  endif
enddo

write ";*********************************************************************"
write ";  Step 2.10: Dump the results table to ensure that the CRCs have been"
write ";  modified. "
write ";*********************************************************************"
cmdCtr = SCX_CPU1_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,tblResTblName,"A","cpu1_tblrestbl2_10",hostCPU,tblResTblId)
wait 5

ut_tlmwait SCX_CPU1_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (5008) - Dump of Table Results Table successful."
  ut_setrequirements CS_5008, "P"
else
  write "<!> Failed (5008) - Dump of Table Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_5008, "F"
endif

step_2_11:
write ";*********************************************************************"
write ";  Step 2.11: Dump the User-defined Memory Results Table."
write ";*********************************************************************"
;; User-Defined Memory State
if (p@SCX_CPU1_CS_MemoryState = "Disabled") then
  write ";** Skipping tests because User-Defined Memory State is Disabled."
  goto step_2_14
endif

cmdCtr = SCX_CPU1_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,memResTblName,"A","cpu1_usrrestbl2_11",hostCPU,usrResTblId)
wait 5

ut_tlmwait SCX_CPU1_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6008) - Dump of User-defined Memory Results Table successful."
  ut_setrequirements CS_6008, "P"
else
  write "<!> Failed (6008) - Dump of User-defined Memory Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_6008, "F"
endif

write ";*********************************************************************"
write ";  Step 2.12: Corrupt the User-defined Memory CRCs in order to determine"
write ";  if they are recalculated upon reset. "
write ";*********************************************************************"
;; Using the TST_CS app, corrupt the CRCs that are enabled
for i = 0 to CS_MAX_NUM_MEMORY_TABLE_ENTRIES-1 DO
  if (p@SCX_CPU1_CS_MEM_RESULT_TABLE[i].State = "Enabled") then
    ;; Send the command to corrupt this entry's CRC
    ut_setupevents "SCX", "CPU1", "TST_CS", TST_CS_CORRUPT_MEMORY_CRC_INF_EID, "INFO", 1

    /SCX_CPU1_TST_CS_CorruptMemCRC MemType=TST_CS_USER_MEM EntryID=i
    wait 3

    ;; Check for the event message
    ut_tlmwait SCX_CPU1_find_event[1].num_found_messages, 1
    if (UT_TW_Status = UT_Success) then
      write "<*> Passed - Expected Event Msg ",TST_CS_CORRUPT_MEMORY_CRC_INF_EID," rcv'd."
    else
      write "<!> Failed - Event message ", SCX_CPU1_evs_eventid," rcv'd. Expected Event Msg ",TST_CS_CORRUPT_MEMORY_CRC_INF_EID,"."
    endif
  endif
enddo

write ";*********************************************************************"
write ";  Step 2.13: Dump the results table to ensure that the CRCs have been"
write ";  modified. "
write ";*********************************************************************"
cmdCtr = SCX_CPU1_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,memResTblName,"A","cpu1_usrrestbl2_13",hostCPU,usrResTblId)
wait 5

ut_tlmwait SCX_CPU1_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6008) - Dump of User-defined Memory Results Table successful."
  ut_setrequirements CS_6008, "P"
else
  write "<!> Failed (6008) - Dump of User-defined Memory Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_6008, "F"
endif

step_2_14:
write ";*********************************************************************"
write ";  Step 2.14: Save the CRCs so that they can be checked after the reset."
write ";*********************************************************************"
osCRC = SCX_CPU1_CS_OSBASELINE
cFECRC = SCX_CPU1_CS_CFECOREBASELINE

;; Loop and store the Non-Volatile Memory CRCs
for i = 0 to CS_MAX_NUM_EEPROM_TABLE_ENTRIES-1 DO
  if (p@SCX_CPU1_CS_EEPROM_RESULT_TABLE[i].State = "Enabled") then
    eeCRCs[i] = SCX_CPU1_CS_EEPROM_RESULT_TABLE[i].BASELINECRC
  endif
enddo

;; Loop and store the Application CRCs
for i = 0 to CS_MAX_NUM_APP_TABLE_ENTRIES-1 DO
  if (p@SCX_CPU1_CS_APP_RESULT_TABLE[i].State = "Enabled") then
    appCRCs[i] = SCX_CPU1_CS_APP_RESULT_TABLE[i].BASELINECRC
  endif
enddo

;; Loop and store the Table CRCs
for i = 0 to CS_MAX_NUM_TABLES_TABLE_ENTRIES-1 DO
  if (p@SCX_CPU1_CS_TBL_RESULT_TABLE[i].State = "Enabled") then
    tblCRCs[i] = SCX_CPU1_CS_TBL_RESULT_TABLE[i].BASELINECRC
  endif
enddo

;; Loop and store the User-defined Memory CRCs
for i = 0 to CS_MAX_NUM_MEMORY_TABLE_ENTRIES-1 DO
  if (p@SCX_CPU1_CS_MEM_RESULT_TABLE[i].State = "Enabled") then
    usrCRCs[i] = SCX_CPU1_CS_MEM_RESULT_TABLE[i].BASELINECRC
  endif
enddo

write ";*********************************************************************"
write ";  Step 2.15: Send the Power-On reset command."
write ";*********************************************************************"
/SCX_CPU1_ES_POWERONRESET
wait 10

close_data_center
wait 60

cfe_startup {hostCPU}
wait 5

write ";*********************************************************************"
write ";  Step 2.16: Start the TST_CS_MemTbl application in order to setup   "
write ";  the OS_Memory_Table for the Checksum (CS) application. "
write ";********************************************************************"
ut_setupevents "SCX", "CPU1", "CFE_ES", CFE_ES_START_INF_EID, "INFO", 1
ut_setupevents "SCX", "CPU1", "TST_CS_MEMTBL", 1, "INFO", 2

s load_start_app ("TST_CS_MEMTBL",hostCPU,"TST_CS_MemTblMain")

;;  Wait for app startup events
ut_tlmwait SCX_CPU1_find_event[2].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
  if (SCX_CPU1_find_event[1].num_found_messages = 1) then
    write "<*> Passed - TST_CS_MEMTBL Application Started"
  else
    write "<!> Failed - CFE_ES start Event Message for TST_CS_MEMTBL not received."
    write "Event Message count = ",SCX_CPU1_find_event[1].num_found_messages
  endif
else
  write "<!> Failed - TST_CS_MEMTBL Application start Event Message not received."
endif

;; These are the TST_CS HK Packet IDs since this app sends this packet
;; CPU1 is the default
stream = x'0930'

/SCX_CPU1_TO_ADDPACKET STREAM=stream PKT_SIZE=X'0' PRIORITY=X'0' RELIABILITY=X'0' BUFLIMIT=x'4'
wait 10

write ";*********************************************************************"
write ";  Step 2.17: Create & upload the EEPROM Definition Table file to be  "
write ";  used during this test. This needs to be done since the OS Memory   "
write ";  Table gets setup by the application above using global memory which"
write ";  may not be at the same address after a Power-On Reset. "
write ";********************************************************************"
s scx_cpu1_cs_edt1
wait 5

;; Upload the file created above as the default
;; Non-volatile (EEPROM) Definition Table load file
;;s ftp_file ("CF:0/apps","eeprom_def_ld_1","cs_eepromtbl.tbl",hostCPU,"P")
s ftp_file ("CF:0/apps","eeprom_def_ld_1",eeTableFileName,hostCPU,"P")
wait 5

write ";*********************************************************************"
write ";  Step 2.18: Create & upload the Memory Definition Table file to be  "
write ";  used during this test. This needs to be done since the OS Memory   "
write ";  Table gets setup by the application above using global memory which"
write ";  may not be at the same address after a Power-On Reset. "
write ";********************************************************************"
s scx_cpu1_cs_mdt5
wait 5

;; Upload the file created above as the default 
;;s ftp_file ("CF:0/apps","usrmem_def_ld_3","cs_memorytbl.tbl",hostCPU,"P")
s ftp_file ("CF:0/apps","usrmem_def_ld_3",memTableFileName,hostCPU,"P")
wait 5

write ";*********************************************************************"
write ";  Step 2.19:  Start the Checksum (CS) and TST_CS applications.  "
write ";********************************************************************"
s scx_cpu1_cs_start_apps("2.19")

write ";*********************************************************************"
write ";  Step 2.20:  Start the other applications required for this test. "
write ";********************************************************************"
ut_setupevents "SCX", "CPU1", "CFE_ES", CFE_ES_START_INF_EID, "INFO", 1
ut_setupevents "SCX", "CPU1", "TST_TBL", TST_TBL_INIT_INF_EID, "INFO", 2

s load_start_app ("TST_TBL",hostCPU)

; Wait for app startup events
ut_tlmwait SCX_CPU1_find_event[2].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
  if (SCX_CPU1_find_event[1].num_found_messages = 1) then
    write "<*> Passed - TST_TBL Application Started"
  else
    write "<!> Failed - CFE_ES start Event Message for TST_TBL not received."
    write "Event Message count = ",SCX_CPU1_find_event[1].num_found_messages
  endif
else
  write "<!> Failed - TST_TBL Application start Event Message not received."
endif

;; Set the eepromCRC variable to the initial value after app startup
eepromCRC = SCX_CPU1_CS_EEPROMBASELINE

write ";*********************************************************************"
write ";  Step 2.21: Wait until the CRCs have been recalculated.    "
write ";*********************************************************************"
if (SCX_CPU1_CS_PASSCTR = 0) then
  ut_tlmwait SCX_CPU1_CS_PASSCTR, 1, 300
else
  write ";** CS has already performed at least 1 complete pass."
endif 

write ";*********************************************************************"
write ";  Step 2.22: Verify that the CRCs contained in the Housekeeping packet"
write ";  have been recalculated. "
write ";*********************************************************************"
;; OS State
if (p@SCX_CPU1_CS_OSState = "Disabled") then
  write ";** Skipping OS CRC recalculation check because OS State is Disabled."
  goto check_cFE_CRC
endif

;; Check the OS CRC
if (osCRC <> SCX_CPU1_CS_OSBASELINE) then
  write "<*> Passed (9002) - OS CRC has been recalculated on a Power-On Reset."
  ut_setrequirements CS_9002, "P"
else
  write "<!> Failed (9002) - OS CRC was not recalculated on a Power-On Reset."
  ut_setrequirements CS_9002, "F"
endif

check_cFE_CRC:
;; cFE Core State
if (p@SCX_CPU1_CS_CFECoreState = "Disabled") then
  write ";** Skipping CRC recalculation check because cFE Core State is Disabled."
  goto check_EEPROM_CRC
endif

;; Check the cFE CRC
if (cFECRC <> SCX_CPU1_CS_CFECOREBASELINE) then
  write "<*> Passed (9002) - cFE CRC has been recalculated on a Power-On Reset."
  ut_setrequirements CS_9002, "P"
else
  write "<!> Failed (9002) - cFE CRC was not recalculated on a Power-On Reset."
  ut_setrequirements CS_9002, "F"
endif

check_EEPROM_CRC:
;; Non-Volatile Memory State
if (p@SCX_CPU1_CS_EepromState = "Disabled") then
  write ";** Skipping CRC recalculation check because Non-Volatile Memory State is Disabled."
  goto step_2_24
endif

;; Check the overall EEPROM CRC
if (eepromCRC <> SCX_CPU1_CS_EEPROMBASELINE) then
  write "<*> Passed (9004) - Overall EEPROM CRC has been recalculated on a Power-On Reset."
  ut_setrequirements CS_9004, "P"
else
  write "<!> Failed (9004) - Overall EEPROM CRC was not recalculated on a Power-On Reset."
  ut_setrequirements CS_9004, "F"
endif

write ";*********************************************************************"
write ";  Step 2.23: Dump the Non-Volatile Code Segment Results Table and "
write ";  verify the CRCs have been recalculated. "
write ";*********************************************************************"
cmdCtr = SCX_CPU1_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,eeResTblName,"A","cpu1_eerestbl2_23",hostCPU,eeResTblId)
wait 5

ut_tlmwait SCX_CPU1_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2010) - Dump of EEPROM Memory Results Table successful."
  ut_setrequirements CS_2010, "P"
else
  write "<!> Failed (2010) - Dump of EEPROM Memory Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_2010, "F"
endif

;; Check the enabled EEPROM Table entries
for i = 0 to CS_MAX_NUM_EEPROM_TABLE_ENTRIES-1 DO
  if (p@SCX_CPU1_CS_EEPROM_RESULT_TABLE[i].State = "Enabled") then
    if (eeCRCs[i] <> SCX_CPU1_CS_EEPROM_RESULT_TABLE[i].BASELINECRC) then
      write "<*> Passed (9003) - EEPROM entry #",i," CRC has been recalculated on a Power-On Reset."
      ut_setrequirements CS_9003, "P"
    else
      write "<!> Failed (9003) - EEPROM entry #", i, " CRC was not recalculated on a Power-On Reset."
      ut_setrequirements CS_9003, "F"
    endif
  endif
enddo

step_2_24:
write ";*********************************************************************"
write ";  Step 2.24: Dump the Application Code Segment Results Table and "
write ";  verify the CRCs have been recalculated. "
write ";*********************************************************************"
;; Application State
if (p@SCX_CPU1_CS_AppState = "Disabled") then
  write ";** Skipping CRC recalculation check because Application State is Disabled."
  goto step_2_25
endif

cmdCtr = SCX_CPU1_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,appResTblName,"A","cpu1_apprestbl2_24",hostCPU,appResTblId)
wait 5

ut_tlmwait SCX_CPU1_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (4008) - Dump of App Results Table successful."
  ut_setrequirements CS_4008, "P"
else
  write "<!> Failed (4008) - Dump of App Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_4008, "F"
endif

;; Loop and store the Application CRCs
for i = 0 to CS_MAX_NUM_APP_TABLE_ENTRIES-1 DO
  if (p@SCX_CPU1_CS_APP_RESULT_TABLE[i].State = "Enabled") then
    if (appCRCs[i] <> SCX_CPU1_CS_APP_RESULT_TABLE[i].BASELINECRC) then
      write "<*> Passed (9005) - App entry #",i," CRC has been recalculated on a Power-On Reset."
      ut_setrequirements CS_9005, "P"
    else
      write "<!> Failed (9005) - App entry #", i, " CRC was not recalculated on a Power-On Reset."
      ut_setrequirements CS_9005, "F"
    endif
  endif
enddo

step_2_25:
write ";*********************************************************************"
write ";  Step 2.25: Dump the Table Results Table and verify the CRCs have "
write ";  been recalculated. "
write ";*********************************************************************"
;; Table State
if (p@SCX_CPU1_CS_TableState = "Disabled") then
  write ";** Skipping CRC recalculation check because Table State is Disabled."
  goto step_2_26
endif

cmdCtr = SCX_CPU1_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,tblResTblName,"A","cpu1_tblrestbl2_25",hostCPU,tblResTblId)
wait 5

ut_tlmwait SCX_CPU1_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (5008) - Dump of Table Results Table successful."
  ut_setrequirements CS_5008, "P"
else
  write "<!> Failed (5008) - Dump of Table Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_5008, "F"
endif
;; Loop and store the Table CRCs
for i = 0 to CS_MAX_NUM_TABLES_TABLE_ENTRIES-1 DO
  if (p@SCX_CPU1_CS_TBL_RESULT_TABLE[i].State = "Enabled") then
    if (tblCRCs[i] <> SCX_CPU1_CS_TBL_RESULT_TABLE[i].BASELINECRC) then
      write "<*> Passed (9006) - Table entry #",i," CRC has been recalculated on a Power-On Reset."
      ut_setrequirements CS_9006, "P"
    else
      write "<!> Failed (9006) - Table entry #", i, " CRC was not recalculated on a Power-On Reset."
      ut_setrequirements CS_9006, "F"
    endif
  endif
enddo

step_2_26:
write ";*********************************************************************"
write ";  Step 2.26: Dump the User-defined Memory Results Table and verify the"
write ";  CRCs have been recalculated. "
write ";*********************************************************************"
;; User-Defined Memory State
if (p@SCX_CPU1_CS_MemoryState = "Disabled") then
  write ";** Skipping CRC recalculation check because User-Defined Memory State is Disabled."
  goto step_3_0
endif

;; User-Defined Memory State
if (p@SCX_CPU1_CS_MemoryState = "Disabled") then
  write ";** Skipping CRC recalculation check because User-Defined Memory State is Disabled."
  goto step_3_0
endif

cmdCtr = SCX_CPU1_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,memResTblName,"A","cpu1_usrrestbl2_26",hostCPU,usrResTblId)
wait 5

ut_tlmwait SCX_CPU1_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6008) - Dump of User-defined Memory Results Table successful."
  ut_setrequirements CS_6008, "P"
else
  write "<!> Failed (6008) - Dump of User-defined Memory Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_6008, "F"
endif
;; Loop and store the User-defined Memory CRCs
for i = 0 to CS_MAX_NUM_MEMORY_TABLE_ENTRIES-1 DO
  if (p@SCX_CPU1_CS_MEM_RESULT_TABLE[i].State = "Enabled") then
    if (usrCRCs[i] <> SCX_CPU1_CS_MEM_RESULT_TABLE[i].BASELINECRC) then
      write "<*> Passed (9007) - User-defined Memory entry #",i," CRC has been recalculated on a Power-On Reset."
      ut_setrequirements CS_9007, "P"
    else
      write "<!> Failed (9007) - User-defined Memory entry #", i, " CRC was not recalculated on a Power-On Reset."
      ut_setrequirements CS_9007, "F"
    endif
  endif
enddo

step_3_0:
write ";*********************************************************************"
write ";  Step 3.0: Processor Reset Test."
write ";*********************************************************************"
write ";  Step 3.1: Modify the OS and cFE code segment baseline CRCs."
write ";*********************************************************************"
;; OS State
if (p@SCX_CPU1_CS_OSState = "Disabled") then
  write ";** Skipping because OS State is Disabled."
  goto cFE_ProcReset_Check
endif

;; Use the TST_CS app to corrupt the OS and cFE CRCs
ut_setupevents "SCX","CPU1","TST_CS",TST_CS_CORRUPT_OS_CRC_INF_EID,"INFO", 1
ut_setupevents "SCX","CPU1",{CSAppName},CS_OS_MISCOMPARE_ERR_EID,"ERROR", 2

/SCX_CPU1_TST_CS_CorruptOSCRC
wait 5

;; Check for OS the event message
ut_tlmwait SCX_CPU1_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Expected Event Msg ",TST_CS_CORRUPT_OS_CRC_INF_EID," rcv'd."
else
  write "<!> Failed - Event message ", SCX_CPU1_evs_eventid," rcv'd. Expected Event Msg ",TST_CS_CORRUPT_OS_CRC_INF_EID,"."
endif

;; Check for the Miscompare event message
ut_tlmwait SCX_CPU1_find_event[2].num_found_messages, 1, 300
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - OS Miscompare Event ID=",CS_OS_MISCOMPARE_ERR_EID," rcv'd."
else
  write "<!> Failed - OS Miscompare Event was not received. Time-out occurred."
endif

cFE_ProcReset_Check:
;; cFE Core State
if (p@SCX_CPU1_CS_CFECoreState = "Disabled") then
  write ";** Skipping because cFE Core State is Disabled."
  goto step_3_2
endif

;; Use the TST_CS app to corrupt the cFE CRC
ut_setupevents "SCX","CPU1","TST_CS",TST_CS_CORRUPT_CFE_CRC_INF_EID, "INFO", 3
ut_setupevents "SCX","CPU1",{CSAppName},CS_CFECORE_MISCOMPARE_ERR_EID,"ERROR", 4

/SCX_CPU1_TST_CS_CorruptCFECRC
wait 5

;; Check for the cFE event message
ut_tlmwait SCX_CPU1_find_event[3].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Expected Event Msg ",TST_CS_CORRUPT_CFE_CRC_INF_EID," rcv'd."
else
  write "<!> Failed - Event message ", SCX_CPU1_evs_eventid," rcv'd. Expected Event Msg ",TST_CS_CORRUPT_CFE_CRC_INF_EID,"."
endif

;; Check for the event message
ut_tlmwait SCX_CPU1_find_event[4].num_found_messages, 1, 300
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - cFE Core Miscompare Event ID=",CS_CFECORE_MISCOMPARE_ERR_EID," rcv'd."
else
  write "<!> Failed - cFE Core Miscompare Event was not received. Time-out occurred."
endif

wait 5

step_3_2:
write ";*********************************************************************"
write ";  Step 3.2: Dump the Non-Volatile Code Segment Results Table."
write ";*********************************************************************"
;; Non-Volatile Memory State
if (p@SCX_CPU1_CS_EepromState = "Disabled") then
  write ";** Skipping tests because Non-Volatile Memory State is Disabled."
  goto step_3_5
endif

cmdCtr = SCX_CPU1_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,eeResTblName,"A","cpu1_eerestbl3_2",hostCPU,eeResTblId)
wait 5

ut_tlmwait SCX_CPU1_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2010) - Dump of EEPROM Memory Results Table successful."
  ut_setrequirements CS_2010, "P"
else
  write "<!> Failed (2010) - Dump of EEPROM Memory Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_2010, "F"
endif

write ";*********************************************************************"
write ";  Step 3.3: Corrupt the Non-volatile Baseline CRCs in order to       "
write ";  determine if they are recalculated upon reset. "
write ";*********************************************************************"
;; Using the TST_CS app, corrupt the CRCs that are enabled
;; Loop for each valid entry in the results table
for i = 0 to CS_MAX_NUM_EEPROM_TABLE_ENTRIES-1 DO
  if (p@SCX_CPU1_CS_EEPROM_RESULT_TABLE[i].State = "Enabled") then
    ;; Send the command to corrupt this entry's CRC
    ut_setupevents "SCX", "CPU1", "TST_CS", TST_CS_CORRUPT_MEMORY_CRC_INF_EID, "INFO", 1

    /SCX_CPU1_TST_CS_CorruptMemCRC MemType=TST_CS_EEPROM_MEM EntryID=i
    wait 3

    ;; Check for the event message
    ut_tlmwait SCX_CPU1_find_event[1].num_found_messages, 1
    if (UT_TW_Status = UT_Success) then
      write "<*> Passed - Expected Event Msg ",TST_CS_CORRUPT_MEMORY_CRC_INF_EID," rcv'd."
    else
      write "<!> Failed - Event message ", SCX_CPU1_evs_eventid," rcv'd. Expected Event Msg ",TST_CS_CORRUPT_MEMORY_CRC_INF_EID,"."
    endif
  endif
enddo

write ";*********************************************************************"
write ";  Step 3.4: Dump the results table to ensure that the CRCs have been "
write ";  modified. "
write ";*********************************************************************"
cmdCtr = SCX_CPU1_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,eeResTblName,"A","cpu1_eerestbl3_4",hostCPU,eeResTblId)
wait 5

ut_tlmwait SCX_CPU1_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2010) - Dump of EEPROM Memory Results Table successful."
  ut_setrequirements CS_2010, "P"
else
  write "<!> Failed (2010) - Dump of EEPROM Memory Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_2010, "F"
endif

step_3_5:
write ";*********************************************************************"
write ";  Step 3.5: Dump the Application Code Segment Results Table."
write ";*********************************************************************"
;; Application State
if (p@SCX_CPU1_CS_AppState = "Disabled") then
  write ";** Skipping tests because Application State is Disabled."
  goto step_3_8
endif

cmdCtr = SCX_CPU1_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,appResTblName,"A","cpu1_apprestbl3_5",hostCPU,appResTblId)
wait 5

ut_tlmwait SCX_CPU1_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (4008) - Dump of App Results Table successful."
  ut_setrequirements CS_4008, "P"
else
  write "<!> Failed (4008) - Dump of App Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_4008, "F"
endif

write ";*********************************************************************"
write ";  Step 3.6: Corrupt the Application CRCs in order to determine if they"
write ";  are recalculated upon reset. "
write ";*********************************************************************"
;; Using the TST_CS app, corrupt the CRCs that are enabled
for i = 0 to CS_MAX_NUM_APP_TABLE_ENTRIES-1 DO
  if (p@SCX_CPU1_CS_APP_RESULT_TABLE[i].State = "Enabled") then
    ut_setupevents "SCX", "CPU1", "TST_CS", TST_CS_CORRUPT_APP_CRC_INF_EID, "INFO", 1

    /SCX_CPU1_TST_CS_CorruptAppCRC AppName=SCX_CPU1_CS_APP_RESULT_TABLE[i].Name
    wait 3

    ;; Check for the event message
    ut_tlmwait SCX_CPU1_find_event[1].num_found_messages, 1
    if (UT_TW_Status = UT_Success) then
      write "<*> Passed - Expected Event Msg ",TST_CS_CORRUPT_APP_CRC_INF_EID," rcv'd."
    else
      write "<!> Failed - Event message ", SCX_CPU1_evs_eventid," rcv'd. Expected Event Msg ",TST_CS_CORRUPT_APP_CRC_INF_EID,"."
    endif
  endif
enddo

write ";*********************************************************************"
write ";  Step 3.7: Dump the results table to ensure that the CRCs have been "
write ";  modified. "
write ";*********************************************************************"
cmdCtr = SCX_CPU1_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,appResTblName,"A","cpu1_apprestbl3_7",hostCPU,appResTblId)
wait 5

ut_tlmwait SCX_CPU1_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (4008) - Dump of App Results Table successful."
  ut_setrequirements CS_4008, "P"
else
  write "<!> Failed (4008) - Dump of App Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_4008, "F"
endif

step_3_8:
write ";*********************************************************************"
write ";  Step 3.8: Dump the Table Results Table."
write ";*********************************************************************"
;; Table State
if (p@SCX_CPU1_CS_TableState = "Disabled") then
  write ";** Skipping tests because Table State is Disabled."
  goto step_3_11
endif

cmdCtr = SCX_CPU1_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,tblResTblName,"A","cpu1_tblrestbl3_8",hostCPU,tblResTblId)
wait 5

ut_tlmwait SCX_CPU1_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (5008) - Dump of Table Results Table successful."
  ut_setrequirements CS_5008, "P"
else
  write "<!> Failed (5008) - Dump of Table Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_5008, "F"
endif

write ";*********************************************************************"
write ";  Step 3.9: Corrupt the Table CRCs in order to determine if they"
write ";  are recalculated upon reset. "
write ";*********************************************************************"
;; Using the TST_CS app, corrupt the CRCs that are enabled
for i = 0 to CS_MAX_NUM_TABLES_TABLE_ENTRIES-1 DO
  if (p@SCX_CPU1_CS_TBL_RESULT_TABLE[i].State = "Enabled") then
    ut_setupevents "SCX", "CPU1", "TST_CS", TST_CS_CORRUPT_TABLE_CRC_INF_EID, "INFO", 1

    /SCX_CPU1_TST_CS_CorruptTblCRC TABLEName=SCX_CPU1_CS_TBL_RESULT_TABLE[i].Name
    wait 3

    ;; Check for the event message
    ut_tlmwait SCX_CPU1_find_event[1].num_found_messages, 1
    if (UT_TW_Status = UT_Success) then
      write "<*> Passed - Expected Event Msg ",TST_CS_CORRUPT_TABLE_CRC_INF_EID," rcv'd."
    else
      write "<!> Failed - Event message ", SCX_CPU1_evs_eventid," rcv'd. Expected Event Msg ",TST_CS_CORRUPT_TABLE_CRC_INF_EID,"."
    endif
  endif
enddo

write ";*********************************************************************"
write ";  Step 3.10: Dump the results table to ensure that the CRCs have been"
write ";  modified. "
write ";*********************************************************************"
cmdCtr = SCX_CPU1_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,tblResTblName,"A","cpu1_tblrestbl3_10",hostCPU,tblResTblId)
wait 5

ut_tlmwait SCX_CPU1_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (5008) - Dump of Table Results Table successful."
  ut_setrequirements CS_5008, "P"
else
  write "<!> Failed (5008) - Dump of Table Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_5008, "F"
endif

step_3_11:
write ";*********************************************************************"
write ";  Step 3.11: Dump the User-defined Memory Results Table."
write ";*********************************************************************"
;; User-Defined Memory State
if (p@SCX_CPU1_CS_MemoryState = "Disabled") then
  write ";** Skipping tests because User-Defined Memory State is Disabled."
  goto step_3_14
endif

cmdCtr = SCX_CPU1_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,memResTblName,"A","cpu1_usrrestbl3_11",hostCPU,usrResTblId)
wait 5

ut_tlmwait SCX_CPU1_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6008) - Dump of User-defined Memory Results Table successful."
  ut_setrequirements CS_6008, "P"
else
  write "<!> Failed (6008) - Dump of User-defined Memory Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_6008, "F"
endif

write ";*********************************************************************"
write ";  Step 3.12: Corrupt the User-defined Memory CRCs in order to determine"
write ";  if they are recalculated upon reset. "
write ";*********************************************************************"
;; Using the TST_CS app, corrupt the CRCs that are enabled
for i = 0 to CS_MAX_NUM_MEMORY_TABLE_ENTRIES-1 DO
  if (p@SCX_CPU1_CS_MEM_RESULT_TABLE[i].State = "Enabled") then
    ;; Send the command to corrupt this entry's CRC
    ut_setupevents "SCX", "CPU1", "TST_CS", TST_CS_CORRUPT_MEMORY_CRC_INF_EID, "INFO", 1

    /SCX_CPU1_TST_CS_CorruptMemCRC MemType=TST_CS_USER_MEM EntryID=i
    wait 3

    ;; Check for the event message
    ut_tlmwait SCX_CPU1_find_event[1].num_found_messages, 1
    if (UT_TW_Status = UT_Success) then
      write "<*> Passed - Expected Event Msg ",TST_CS_CORRUPT_MEMORY_CRC_INF_EID," rcv'd."
    else
      write "<!> Failed - Event message ", SCX_CPU1_evs_eventid," rcv'd. Expected Event Msg ",TST_CS_CORRUPT_MEMORY_CRC_INF_EID,"."
    endif
  endif
enddo

write ";*********************************************************************"
write ";  Step 3.13: Dump the results table to ensure that the CRCs have been"
write ";  modified. "
write ";*********************************************************************"
cmdCtr = SCX_CPU1_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,memResTblName,"A","cpu1_usrrestbl3_13",hostCPU,usrResTblId)
wait 5

ut_tlmwait SCX_CPU1_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6008) - Dump of User-defined Memory Results Table successful."
  ut_setrequirements CS_6008, "P"
else
  write "<!> Failed (6008) - Dump of User-defined Memory Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_6008, "F"
endif

step_3_14:
write ";*********************************************************************"
write ";  Step 3.14: Save the CRCs so that they can be checked after the reset."
write ";*********************************************************************"
osCRC = SCX_CPU1_CS_OSBASELINE
cFECRC = SCX_CPU1_CS_CFECOREBASELINE

;; Loop and store the Non-Volatile Memory CRCs
for i = 0 to CS_MAX_NUM_EEPROM_TABLE_ENTRIES-1 DO
  if (p@SCX_CPU1_CS_EEPROM_RESULT_TABLE[i].State = "Enabled") then
    eeCRCs[i] = SCX_CPU1_CS_EEPROM_RESULT_TABLE[i].BASELINECRC
  endif
enddo

;; Loop and store the Application CRCs
for i = 0 to CS_MAX_NUM_APP_TABLE_ENTRIES-1 DO
  if (p@SCX_CPU1_CS_APP_RESULT_TABLE[i].State = "Enabled") then
    appCRCs[i] = SCX_CPU1_CS_APP_RESULT_TABLE[i].BASELINECRC
  endif
enddo

;; Loop and store the Table CRCs
for i = 0 to CS_MAX_NUM_TABLES_TABLE_ENTRIES-1 DO
  if (p@SCX_CPU1_CS_TBL_RESULT_TABLE[i].State = "Enabled") then
    tblCRCs[i] = SCX_CPU1_CS_TBL_RESULT_TABLE[i].BASELINECRC
  endif
enddo

;; Loop and store the User-defined Memory CRCs
for i = 0 to CS_MAX_NUM_MEMORY_TABLE_ENTRIES-1 DO
  if (p@SCX_CPU1_CS_MEM_RESULT_TABLE[i].State = "Enabled") then
    usrCRCs[i] = SCX_CPU1_CS_MEM_RESULT_TABLE[i].BASELINECRC
  endif
enddo

write ";*********************************************************************"
write ";  Step 3.15: Send the Processor reset command."
write ";*********************************************************************"
/SCX_CPU1_ES_PROCESSORRESET
wait 10

close_data_center
wait 60

cfe_startup {hostCPU}
wait 5

write ";*********************************************************************"
write ";  Step 3.16: Start the TST_CS_MemTbl application in order to setup   "
write ";  the OS_Memory_Table for the Checksum (CS) application. "
write ";********************************************************************"
ut_setupevents "SCX", "CPU1", "CFE_ES", CFE_ES_START_INF_EID, "INFO", 1
ut_setupevents "SCX", "CPU1", "TST_CS_MEMTBL", 1, "INFO", 2

s load_start_app ("TST_CS_MEMTBL",hostCPU,"TST_CS_MemTblMain")

;;  Wait for app startup events
ut_tlmwait SCX_CPU1_find_event[2].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
  if (SCX_CPU1_find_event[1].num_found_messages = 1) then
    write "<*> Passed - TST_CS_MEMTBL Application Started"
  else
    write "<!> Failed - CFE_ES start Event Message for TST_CS_MEMTBL not received."
    write "Event Message count = ",SCX_CPU1_find_event[1].num_found_messages
  endif
else
  write "<!> Failed - TST_CS_MEMTBL Application start Event Message not received."
endif

;; These are the TST_CS HK Packet IDs since this app sends this packet
;; CPU1 is the default
stream = x'0930'

/SCX_CPU1_TO_ADDPACKET STREAM=stream PKT_SIZE=X'0' PRIORITY=X'0' RELIABILITY=X'0' BUFLIMIT=x'4'
wait 10

write ";*********************************************************************"
write ";  Step 3.17: Create & upload the EEPROM Definition Table file to be  "
write ";  used during this test. This needs to be done since the OS Memory   "
write ";  Table gets setup by the application above using global memory which"
write ";  may not be at the same address after a Processor Reset. "
write ";********************************************************************"
s scx_cpu1_cs_edt1
wait 5

;; Upload the file created above as the default
;; Non-volatile (EEPROM) Definition Table load file
;;s ftp_file ("CF:0/apps","eeprom_def_ld_1","cs_eepromtbl.tbl",hostCPU,"P")
s ftp_file ("CF:0/apps","eeprom_def_ld_1",eeTableFileName,hostCPU,"P")
wait 5

write ";*********************************************************************"
write ";  Step 3.18: Create & upload the Memory Definition Table file to be  "
write ";  used during this test. This needs to be done since the OS Memory   "
write ";  Table gets setup by the application above using global memory which"
write ";  may not be at the same address after a Processor Reset. "
write ";********************************************************************"
s scx_cpu1_cs_mdt5
wait 5

;; Upload the file created above as the default 
;;s ftp_file ("CF:0/apps","usrmem_def_ld_3","cs_memorytbl.tbl",hostCPU,"P")
s ftp_file ("CF:0/apps","usrmem_def_ld_3",memTableFileName,hostCPU,"P")
wait 5

write ";*********************************************************************"
write ";  Step 3.19:  Start the Checksum (CS) and TST_CS applications.  "
write ";********************************************************************"
s scx_cpu1_cs_start_apps("3.19")

write ";*********************************************************************"
write ";  Step 3.20: Start the other applications required for this test. "
write ";********************************************************************"
ut_setupevents "SCX", "CPU1", "CFE_ES", CFE_ES_START_INF_EID, "INFO", 1
ut_setupevents "SCX", "CPU1", "TST_TBL", TST_TBL_INIT_INF_EID, "INFO", 2

s load_start_app ("TST_TBL",hostCPU)

; Wait for app startup events
ut_tlmwait SCX_CPU1_find_event[2].num_found_messages, 1
IF (UT_TW_Status = UT_Success) THEN
  if (SCX_CPU1_find_event[1].num_found_messages = 1) then
    write "<*> Passed - TST_TBL Application Started"
  else
    write "<!> Failed - CFE_ES start Event Message for TST_TBL not received."
    write "Event Message count = ",SCX_CPU1_find_event[1].num_found_messages
  endif
else
  write "<!> Failed - TST_TBL Application start Event Message not received."
endif

;; Set the eepromCRC variable to the initial value after app startup
eepromCRC = SCX_CPU1_CS_EEPROMBASELINE

write ";*********************************************************************"
write ";  Step 3.21: Wait until the CRCs have been recalculated.    "
write ";*********************************************************************"
if (SCX_CPU1_CS_PASSCTR = 0) then
  ut_tlmwait SCX_CPU1_CS_PASSCTR, 1, 300
else
  write ";** CS has already performed at least 1 complete pass."
endif 

write ";*********************************************************************"
write ";  Step 3.22: Verify that the CRCs contained in the Housekeeping packet"
write ";  have been recalculated. "
write ";*********************************************************************"
;; OS State
if (p@SCX_CPU1_CS_OSState = "Disabled") then
  write ";** Skipping OS CRC recalculation check because OS State is Disabled."
  goto check_cFE_CRC2
endif

;; Check the OS CRC
if (osCRC <> SCX_CPU1_CS_OSBASELINE) then
  write "<*> Passed (9002;9009) - OS CRC has been recalculated on a Processor Reset."
  ut_setrequirements CS_9002, "P"
  ut_setrequirements CS_9009, "P"
else
  write "<!> Failed (9002;9009) - OS CRC was not recalculated on a Processor Reset."
  ut_setrequirements CS_9002, "F"
  ut_setrequirements CS_9009, "F"
endif

check_cFE_CRC2:
;; Check the cFE CRC
;; cFE Core State
if (p@SCX_CPU1_CS_CFECoreState = "Disabled") then
  write ";** Skipping CRC recalculation check because cFE Core State is Disabled."
  goto check_EEPROM_CRC2
endif

if (cFECRC <> SCX_CPU1_CS_CFECOREBASELINE) then
  write "<*> Passed (9002;9009) - cFE CRC has been recalculated on a Processor Reset."
  ut_setrequirements CS_9002, "P"
  ut_setrequirements CS_9009, "P"
else
  write "<!> Failed (9002;9009) - cFE CRC was not recalculated on a Processor Reset."
  ut_setrequirements CS_9002, "F"
  ut_setrequirements CS_9009, "F"
endif

check_EEPROM_CRC2:
;; Non-Volatile Memory State
if (p@SCX_CPU1_CS_EepromState = "Disabled") then
  write ";** Skipping CRC recalculation check because Non-Volatile Memory State is Disabled."
  goto step_3_24
endif

;; Check the overall EEPROM CRC
if (eepromCRC <> SCX_CPU1_CS_EEPROMBASELINE) then
  write "<*> Passed (9004;9009) - Overall EEPROM CRC has been recalculated on a Processor Reset."
  ut_setrequirements CS_9004, "P"
  ut_setrequirements CS_9009, "P"
else
  write "<!> Failed (9004;9009) - Overall EEPROM CRC was not recalculated on a Processor Reset."
  ut_setrequirements CS_9004, "F"
  ut_setrequirements CS_9009, "F"
endif

write ";*********************************************************************"
write ";  Step 3.23: Dump the Non-Volatile Code Segment Results Table and "
write ";  verify the CRCs have been recalculated. "
write ";*********************************************************************"
cmdCtr = SCX_CPU1_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,eeResTblName,"A","cpu1_eerestbl3_23",hostCPU,eeResTblId)
wait 5

ut_tlmwait SCX_CPU1_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2010) - Dump of EEPROM Memory Results Table successful."
  ut_setrequirements CS_2010, "P"
else
  write "<!> Failed (2010) - Dump of EEPROM Memory Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_2010, "F"
endif

;; Check the enabled EEPROM Table entries
for i = 0 to CS_MAX_NUM_EEPROM_TABLE_ENTRIES-1 DO
  if (p@SCX_CPU1_CS_EEPROM_RESULT_TABLE[i].State = "Enabled") then
    if (eeCRCs[i] <> SCX_CPU1_CS_EEPROM_RESULT_TABLE[i].BASELINECRC) then
      write "<*> Passed (9003;9009) - EEPROM entry #",i," CRC has been recalculated on a Processor Reset."
      ut_setrequirements CS_9003, "P"
      ut_setrequirements CS_9009, "P"
    else
      write "<!> Failed (9003;9009) - EEPROM entry #", i, " CRC was not recalculated on a Processor Reset."
      ut_setrequirements CS_9003, "F"
      ut_setrequirements CS_9009, "F"
    endif
  endif
enddo

step_3_24:
write ";*********************************************************************"
write ";  Step 3.24: Dump the Application Code Segment Results Table and "
write ";  verify the CRCs have been recalculated. "
write ";*********************************************************************"
;; Application State
if (p@SCX_CPU1_CS_AppState = "Disabled") then
  write ";** Skipping CRC recalculation check because Application State is Disabled."
  goto step_3_25
endif

cmdCtr = SCX_CPU1_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,appResTblName,"A","cpu1_apprestbl3_24",hostCPU,appResTblId)
wait 5

ut_tlmwait SCX_CPU1_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (4008) - Dump of App Results Table successful."
  ut_setrequirements CS_4008, "P"
else
  write "<!> Failed (4008) - Dump of App Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_4008, "F"
endif

;; Loop and store the Application CRCs
for i = 0 to CS_MAX_NUM_APP_TABLE_ENTRIES-1 DO
  if (p@SCX_CPU1_CS_APP_RESULT_TABLE[i].State = "Enabled") then
    if (appCRCs[i] <> SCX_CPU1_CS_APP_RESULT_TABLE[i].BASELINECRC) then
      write "<*> Passed (9005;9009) - App entry #",i," CRC has been recalculated on a Processor Reset."
      ut_setrequirements CS_9005, "P"
      ut_setrequirements CS_9009, "P"
    else
      write "<!> Failed (9005;9009) - App entry #", i, " CRC was not recalculated on a Processor Reset."
      ut_setrequirements CS_9005, "F"
      ut_setrequirements CS_9009, "F"
    endif
  endif
enddo

step_3_25:
write ";*********************************************************************"
write ";  Step 3.25: Dump the Table Results Table and verify the CRCs have "
write ";  been recalculated. "
write ";*********************************************************************"
;; Table State
if (p@SCX_CPU1_CS_TableState = "Disabled") then
  write ";** Skipping CRC recalculation check because Table State is Disabled."
  goto step_3_26
endif

cmdCtr = SCX_CPU1_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,tblResTblName,"A","cpu1_tblrestbl3_25",hostCPU,tblResTblId)
wait 5

ut_tlmwait SCX_CPU1_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (5008) - Dump of Table Results Table successful."
  ut_setrequirements CS_5008, "P"
else
  write "<!> Failed (5008) - Dump of Table Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_5008, "F"
endif
;; Loop and store the Table CRCs
for i = 0 to CS_MAX_NUM_TABLES_TABLE_ENTRIES-1 DO
  if (p@SCX_CPU1_CS_TBL_RESULT_TABLE[i].State = "Enabled") then
    if (tblCRCs[i] <> SCX_CPU1_CS_TBL_RESULT_TABLE[i].BASELINECRC) then
      write "<*> Passed (9006;9009) - Table entry #",i," CRC has been recalculated on a Processor Reset."
      ut_setrequirements CS_9006, "P"
      ut_setrequirements CS_9009, "P"
    else
      write "<!> Failed (9006;9009) - Table entry #", i, " CRC was not recalculated on a Processor Reset."
      ut_setrequirements CS_9006, "F"
      ut_setrequirements CS_9009, "F"
    endif
  endif
enddo

step_3_26:
write ";*********************************************************************"
write ";  Step 3.26: Dump the User-defined Memory Results Table and verify the"
write ";  CRCs have been recalculated. "
write ";*********************************************************************"
;; User-Defined Memory State
if (p@SCX_CPU1_CS_MemoryState = "Disabled") then
  write ";** Skipping CRC recalculation check because User-Defined Memory State is Disabled."
  goto step_4_0
endif

cmdCtr = SCX_CPU1_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,memResTblName,"A","cpu1_usrrestbl3_26",hostCPU,usrResTblId)
wait 5

ut_tlmwait SCX_CPU1_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6008) - Dump of User-defined Memory Results Table successful."
  ut_setrequirements CS_6008, "P"
else
  write "<!> Failed (6008) - Dump of User-defined Memory Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_6008, "F"
endif
;; Loop and store the User-defined Memory CRCs
for i = 0 to CS_MAX_NUM_MEMORY_TABLE_ENTRIES-1 DO
  if (p@SCX_CPU1_CS_MEM_RESULT_TABLE[i].State = "Enabled") then
    if (usrCRCs[i] <> SCX_CPU1_CS_MEM_RESULT_TABLE[i].BASELINECRC) then
      write "<*> Passed (9007;9009) - User-defined Memory entry #",i," CRC has been recalculated on a Processor Reset."
      ut_setrequirements CS_9007, "P"
      ut_setrequirements CS_9009, "P"
    else
      write "<!> Failed (9007;9009) - User-defined Memory entry #", i, " CRC was not recalculated on a Processor Reset."
      ut_setrequirements CS_9007, "F"
      ut_setrequirements CS_9009, "F"
    endif
  endif
enddo

wait 5

step_4_0:
write ";*********************************************************************"
write ";  Step 4.0: Application Reset Test."
write ";*********************************************************************"
write ";  Step 4.1: Modify the OS and cFE code segment baseline CRCs."
write ";*********************************************************************"
;; OS State
if (p@SCX_CPU1_CS_OSState = "Disabled") then
  write ";** Skipping because OS State is Disabled."
  goto cFE_AppReset_Check
endif

;; Use the TST_CS app to corrupt the OS and cFE CRCs
ut_setupevents "SCX","CPU1","TST_CS",TST_CS_CORRUPT_OS_CRC_INF_EID, "INFO", 1
ut_setupevents "SCX", "CPU1", {CSAppName}, CS_OS_MISCOMPARE_ERR_EID, "ERROR", 2

/SCX_CPU1_TST_CS_CorruptOSCRC
wait 5

;; Check for OS the event message
ut_tlmwait SCX_CPU1_find_event[1].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Expected Event Msg ",TST_CS_CORRUPT_OS_CRC_INF_EID," rcv'd."
else
  write "<!> Failed - Event message ", SCX_CPU1_evs_eventid," rcv'd. Expected Event Msg ",TST_CS_CORRUPT_OS_CRC_INF_EID,"."
endif

;; Check for the Miscompare event message
ut_tlmwait SCX_CPU1_find_event[2].num_found_messages, 1, 300
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - OS Miscompare Event ID=",CS_OS_MISCOMPARE_ERR_EID," rcv'd."
else
  write "<!> Failed - OS Miscompare Event was not received. Time-out occurred."
endif

cFE_AppReset_Check:
;; cFE Core State
if (p@SCX_CPU1_CS_CFECoreState = "Disabled") then
  write ";** Skipping because cFE Core State is Disabled."
  goto step_4_2
endif

;; Use the TST_CS app to corrupt the cFE CRC
ut_setupevents "SCX","CPU1","TST_CS",TST_CS_CORRUPT_CFE_CRC_INF_EID, "INFO", 3
ut_setupevents "SCX","CPU1",{CSAppName},CS_CFECORE_MISCOMPARE_ERR_EID,"ERROR", 4

/SCX_CPU1_TST_CS_CorruptCFECRC
wait 5

;; Check for the cFE event message
ut_tlmwait SCX_CPU1_find_event[3].num_found_messages, 1
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - Expected Event Msg ",TST_CS_CORRUPT_CFE_CRC_INF_EID," rcv'd."
else
  write "<!> Failed - Event message ", SCX_CPU1_evs_eventid," rcv'd. Expected Event Msg ",TST_CS_CORRUPT_CFE_CRC_INF_EID,"."
endif

;; Check for the event message
ut_tlmwait SCX_CPU1_find_event[4].num_found_messages, 1, 300
if (UT_TW_Status = UT_Success) then
  write "<*> Passed - cFE Core Miscompare Event ID=",CS_CFECORE_MISCOMPARE_ERR_EID," rcv'd."
else
  write "<!> Failed - cFE Core Miscompare Event was not received. Time-out occurred."
endif

wait 5

step_4_2:
write ";*********************************************************************"
write ";  Step 4.2: Dump the Non-Volatile Code Segment Results Table."
write ";*********************************************************************"
;; Non-Volatile Memory State
if (p@SCX_CPU1_CS_EepromState = "Disabled") then
  write ";** Skipping tests because Non-Volatile Memory State is Disabled."
  goto step_4_5
endif

cmdCtr = SCX_CPU1_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,eeResTblName,"A","cpu1_eerestbl4_2",hostCPU,eeResTblId)
wait 5

ut_tlmwait SCX_CPU1_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2010) - Dump of EEPROM Memory Results Table successful."
  ut_setrequirements CS_2010, "P"
else
  write "<!> Failed (2010) - Dump of EEPROM Memory Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_2010, "F"
endif

write ";*********************************************************************"
write ";  Step 4.3: Corrupt the Non-volatile Baseline CRCs in order to       "
write ";  determine if they are recalculated upon reset. "
write ";*********************************************************************"
;; Using the TST_CS app, corrupt the CRCs that are enabled
;; Loop for each valid entry in the results table
for i = 0 to CS_MAX_NUM_EEPROM_TABLE_ENTRIES-1 DO
  if (p@SCX_CPU1_CS_EEPROM_RESULT_TABLE[i].State = "Enabled") then
    ;; Send the command to corrupt this entry's CRC
    ut_setupevents "SCX", "CPU1", "TST_CS", TST_CS_CORRUPT_MEMORY_CRC_INF_EID, "INFO", 1

    /SCX_CPU1_TST_CS_CorruptMemCRC MemType=TST_CS_EEPROM_MEM EntryID=i
    wait 3

    ;; Check for the event message
    ut_tlmwait SCX_CPU1_find_event[1].num_found_messages, 1
    if (UT_TW_Status = UT_Success) then
      write "<*> Passed - Expected Event Msg ",TST_CS_CORRUPT_MEMORY_CRC_INF_EID," rcv'd."
    else
      write "<!> Failed - Event message ", SCX_CPU1_evs_eventid," rcv'd. Expected Event Msg ",TST_CS_CORRUPT_MEMORY_CRC_INF_EID,"."
    endif
  endif
enddo

write ";*********************************************************************"
write ";  Step 4.4: Dump the results table to ensure that the CRCs have been "
write ";  modified. "
write ";*********************************************************************"
cmdCtr = SCX_CPU1_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,eeResTblName,"A","cpu1_eerestbl4_4",hostCPU,eeResTblId)
wait 5

ut_tlmwait SCX_CPU1_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2010) - Dump of EEPROM Memory Results Table successful."
  ut_setrequirements CS_2010, "P"
else
  write "<!> Failed (2010) - Dump of EEPROM Memory Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_2010, "F"
endif

step_4_5:
write ";*********************************************************************"
write ";  Step 4.5: Dump the Application Code Segment Results Table."
write ";*********************************************************************"
;; Application State
if (p@SCX_CPU1_CS_AppState = "Disabled") then
  write ";** Skipping tests because Application State is Disabled."
  goto step_4_8
endif

cmdCtr = SCX_CPU1_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,appResTblName,"A","cpu1_apprestbl4_5",hostCPU,appResTblId)
wait 5

ut_tlmwait SCX_CPU1_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (4008) - Dump of App Results Table successful."
  ut_setrequirements CS_4008, "P"
else
  write "<!> Failed (4008) - Dump of App Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_4008, "F"
endif

write ";*********************************************************************"
write ";  Step 4.6: Corrupt the Application CRCs in order to determine if they"
write ";  are recalculated upon reset. "
write ";*********************************************************************"
;; Using the TST_CS app, corrupt the CRCs that are enabled
for i = 0 to CS_MAX_NUM_APP_TABLE_ENTRIES-1 DO
  if (p@SCX_CPU1_CS_APP_RESULT_TABLE[i].State = "Enabled") then
    ut_setupevents "SCX", "CPU1", "TST_CS", TST_CS_CORRUPT_APP_CRC_INF_EID, "INFO", 1

    /SCX_CPU1_TST_CS_CorruptAppCRC AppName=SCX_CPU1_CS_APP_RESULT_TABLE[i].Name
    wait 3

    ;; Check for the event message
    ut_tlmwait SCX_CPU1_find_event[1].num_found_messages, 1
    if (UT_TW_Status = UT_Success) then
      write "<*> Passed - Expected Event Msg ",TST_CS_CORRUPT_APP_CRC_INF_EID," rcv'd."
    else
      write "<!> Failed - Event message ", SCX_CPU1_evs_eventid," rcv'd. Expected Event Msg ",TST_CS_CORRUPT_APP_CRC_INF_EID,"."
    endif
  endif
enddo

write ";*********************************************************************"
write ";  Step 4.7: Dump the results table to ensure that the CRCs have been "
write ";  modified. "
write ";*********************************************************************"
cmdCtr = SCX_CPU1_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,appResTblName,"A","cpu1_apprestbl4_7",hostCPU,appResTblId)
wait 5

ut_tlmwait SCX_CPU1_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (4008) - Dump of App Results Table successful."
  ut_setrequirements CS_4008, "P"
else
  write "<!> Failed (4008) - Dump of App Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_4008, "F"
endif

step_4_8:
write ";*********************************************************************"
write ";  Step 4.8: Dump the Table Results Table."
write ";*********************************************************************"
;; Table State
if (p@SCX_CPU1_CS_TableState = "Disabled") then
  write ";** Skipping tests because Table State is Disabled."
  goto step_4_11
endif

cmdCtr = SCX_CPU1_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,tblResTblName,"A","cpu1_tblrestbl4_8",hostCPU,tblResTblId)
wait 5

ut_tlmwait SCX_CPU1_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (5008) - Dump of Table Results Table successful."
  ut_setrequirements CS_5008, "P"
else
  write "<!> Failed (5008) - Dump of Table Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_5008, "F"
endif

write ";*********************************************************************"
write ";  Step 4.9: Corrupt the Table CRCs in order to determine if they"
write ";  are recalculated upon reset. "
write ";*********************************************************************"
;; Using the TST_CS app, corrupt the CRCs that are enabled
for i = 0 to CS_MAX_NUM_TABLES_TABLE_ENTRIES-1 DO
  if (p@SCX_CPU1_CS_TBL_RESULT_TABLE[i].State = "Enabled") then
    ut_setupevents "SCX", "CPU1", "TST_CS", TST_CS_CORRUPT_TABLE_CRC_INF_EID, "INFO", 1

    /SCX_CPU1_TST_CS_CorruptTblCRC TABLEName=SCX_CPU1_CS_TBL_RESULT_TABLE[i].Name
    wait 3

    ;; Check for the event message
    ut_tlmwait SCX_CPU1_find_event[1].num_found_messages, 1
    if (UT_TW_Status = UT_Success) then
      write "<*> Passed - Expected Event Msg ",TST_CS_CORRUPT_TABLE_CRC_INF_EID," rcv'd."
    else
      write "<!> Failed - Event message ", SCX_CPU1_evs_eventid," rcv'd. Expected Event Msg ",TST_CS_CORRUPT_TABLE_CRC_INF_EID,"."
    endif
  endif
enddo

write ";*********************************************************************"
write ";  Step 4.10: Dump the results table to ensure that the CRCs have been"
write ";  modified. "
write ";*********************************************************************"
cmdCtr = SCX_CPU1_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,tblResTblName,"A","cpu1_tblrestbl4_10",hostCPU,tblResTblId)
wait 5

ut_tlmwait SCX_CPU1_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (5008) - Dump of Table Results Table successful."
  ut_setrequirements CS_5008, "P"
else
  write "<!> Failed (5008) - Dump of Table Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_5008, "F"
endif

step_4_11:
write ";*********************************************************************"
write ";  Step 4.11: Dump the User-defined Memory Results Table."
write ";*********************************************************************"
;; User-Defined Memory State
if (p@SCX_CPU1_CS_MemoryState = "Disabled") then
  write ";** Skipping tests because User-Defined Memory State is Disabled."
  goto step_4_14
endif

cmdCtr = SCX_CPU1_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,memResTblName,"A","cpu1_usrrestbl4_11",hostCPU,usrResTblId)
wait 5

ut_tlmwait SCX_CPU1_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6008) - Dump of User-defined Memory Results Table successful."
  ut_setrequirements CS_6008, "P"
else
  write "<!> Failed (6008) - Dump of User-defined Memory Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_6008, "F"
endif

write ";*********************************************************************"
write ";  Step 4.12: Corrupt the User-defined Memory CRCs in order to determine"
write ";  if they are recalculated upon reset. "
write ";*********************************************************************"
;; Using the TST_CS app, corrupt the CRCs that are enabled
for i = 0 to CS_MAX_NUM_MEMORY_TABLE_ENTRIES-1 DO
  if (p@SCX_CPU1_CS_MEM_RESULT_TABLE[i].State = "Enabled") then
    ;; Send the command to corrupt this entry's CRC
    ut_setupevents "SCX", "CPU1", "TST_CS", TST_CS_CORRUPT_MEMORY_CRC_INF_EID, "INFO", 1

    /SCX_CPU1_TST_CS_CorruptMemCRC MemType=TST_CS_USER_MEM EntryID=i
    wait 3

    ;; Check for the event message
    ut_tlmwait SCX_CPU1_find_event[1].num_found_messages, 1
    if (UT_TW_Status = UT_Success) then
      write "<*> Passed - Expected Event Msg ",TST_CS_CORRUPT_MEMORY_CRC_INF_EID," rcv'd."
    else
      write "<!> Failed - Event message ", SCX_CPU1_evs_eventid," rcv'd. Expected Event Msg ",TST_CS_CORRUPT_MEMORY_CRC_INF_EID,"."
    endif
  endif
enddo

write ";*********************************************************************"
write ";  Step 4.13: Dump the results table to ensure that the CRCs have been"
write ";  modified. "
write ";*********************************************************************"
cmdCtr = SCX_CPU1_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,memResTblName,"A","cpu1_usrrestbl4_13",hostCPU,usrResTblId)
wait 5

ut_tlmwait SCX_CPU1_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6008) - Dump of User-defined Memory Results Table successful."
  ut_setrequirements CS_6008, "P"
else
  write "<!> Failed (6008) - Dump of User-defined Memory Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_6008, "F"
endif

step_4_14:
write ";*********************************************************************"
write ";  Step 4.14: Save the CRCs so that they can be checked after the reset."
write ";*********************************************************************"
osCRC = SCX_CPU1_CS_OSBASELINE
cFECRC = SCX_CPU1_CS_CFECOREBASELINE

;; Loop and store the Non-Volatile Memory CRCs
for i = 0 to CS_MAX_NUM_EEPROM_TABLE_ENTRIES-1 DO
  if (p@SCX_CPU1_CS_EEPROM_RESULT_TABLE[i].State = "Enabled") then
    eeCRCs[i] = SCX_CPU1_CS_EEPROM_RESULT_TABLE[i].BASELINECRC
  endif
enddo

;; Loop and store the Application CRCs
for i = 0 to CS_MAX_NUM_APP_TABLE_ENTRIES-1 DO
  if (p@SCX_CPU1_CS_APP_RESULT_TABLE[i].State = "Enabled") then
    appCRCs[i] = SCX_CPU1_CS_APP_RESULT_TABLE[i].BASELINECRC
  endif
enddo

;; Loop and store the Table CRCs
for i = 0 to CS_MAX_NUM_TABLES_TABLE_ENTRIES-1 DO
  if (p@SCX_CPU1_CS_TBL_RESULT_TABLE[i].State = "Enabled") then
    tblCRCs[i] = SCX_CPU1_CS_TBL_RESULT_TABLE[i].BASELINECRC
  endif
enddo

;; Loop and store the User-defined Memory CRCs
for i = 0 to CS_MAX_NUM_MEMORY_TABLE_ENTRIES-1 DO
  if (p@SCX_CPU1_CS_MEM_RESULT_TABLE[i].State = "Enabled") then
    usrCRCs[i] = SCX_CPU1_CS_MEM_RESULT_TABLE[i].BASELINECRC
  endif
enddo

write ";*********************************************************************"
write ";  Step 4.15: Stop the CS and TST_CS applications."
write ";*********************************************************************"
/SCX_CPU1_ES_DELETEAPP APPLICATION="TST_CS"
wait 5
/SCX_CPU1_ES_DELETEAPP APPLICATION=CSAppName
wait 5

write ";*********************************************************************"
write ";  Step 4.16: Re-start the Checksum (CS) and TST_CS applications.  "
write ";********************************************************************"
s scx_cpu1_cs_start_apps("4.16")

;; Set the eepromCRC variable to the initial value after app startup
eepromCRC = SCX_CPU1_CS_EEPROMBASELINE

write ";*********************************************************************"
write ";  Step 4.17: Wait until the CRCs have been recalculated.    "
write ";*********************************************************************"
if (SCX_CPU1_CS_PASSCTR = 0) then
  ut_tlmwait SCX_CPU1_CS_PASSCTR, 1, 300
else
  write ";** CS has already performed at least 1 complete pass."
endif 

write ";*********************************************************************"
write ";  Step 4.18: Verify that the CRCs contained in the Housekeeping packet"
write ";  have been recalculated. "
write ";*********************************************************************"
;; OS State
if (p@SCX_CPU1_CS_OSState = "Disabled") then
  write ";** Skipping OS CRC recalculation check because OS State is Disabled."
  goto check_cFE_CRC3
endif

;; Check the OS CRC
if (osCRC <> SCX_CPU1_CS_OSBASELINE) then
  write "<*> Passed (9002;9009) - OS CRC has been recalculated on an Application Reset."
  ut_setrequirements CS_9002, "P"
  ut_setrequirements CS_9009, "P"
else
  write "<!> Failed (9002;9009) - OS CRC was not recalculated on an Application Reset."
  ut_setrequirements CS_9002, "F"
  ut_setrequirements CS_9009, "F"
endif

check_cFE_CRC3:
;; Check the cFE CRC
;; cFE Core State
if (p@SCX_CPU1_CS_CFECoreState = "Disabled") then
  write ";** Skipping CRC recalculation check because cFE Core State is Disabled."
  goto check_EEPROM_CRC3
endif

if (cFECRC <> SCX_CPU1_CS_CFECOREBASELINE) then
  write "<*> Passed (9002;9009) - cFE CRC has been recalculated on an Application Reset."
  ut_setrequirements CS_9002, "P"
  ut_setrequirements CS_9009, "P"
else
  write "<!> Failed (9002;9009) - cFE CRC was not recalculated on an Application Reset."
  ut_setrequirements CS_9002, "F"
  ut_setrequirements CS_9009, "F"
endif

check_EEPROM_CRC3:
;; Non-Volatile Memory State
if (p@SCX_CPU1_CS_EepromState = "Disabled") then
  write ";** Skipping CRC recalculation check because Non-Volatile Memory State is Disabled."
  goto step_4_20
endif

;; Check the overall EEPROM CRC
if (eepromCRC <> SCX_CPU1_CS_EEPROMBASELINE) then
  write "<*> Passed (9004;9009) - Overall EEPROM CRC has been recalculated on an Application Reset."
  ut_setrequirements CS_9004, "P"
  ut_setrequirements CS_9009, "P"
else
  write "<!> Failed (9004;9009) - Overall EEPROM CRC was not recalculated on an Application Reset."
  ut_setrequirements CS_9004, "F"
  ut_setrequirements CS_9009, "F"
endif

write ";*********************************************************************"
write ";  Step 4.19: Dump the Non-Volatile Code Segment Results Table and "
write ";  verify the CRCs have been recalculated. "
write ";*********************************************************************"
cmdCtr = SCX_CPU1_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,eeResTblName,"A","cpu1_eerestbl4_19",hostCPU,eeResTblId)
wait 5

ut_tlmwait SCX_CPU1_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (2010) - Dump of EEPROM Memory Results Table successful."
  ut_setrequirements CS_2010, "P"
else
  write "<!> Failed (2010) - Dump of EEPROM Memory Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_2010, "F"
endif

;; Check the enabled EEPROM Table entries
for i = 0 to CS_MAX_NUM_EEPROM_TABLE_ENTRIES-1 DO
  if (p@SCX_CPU1_CS_EEPROM_RESULT_TABLE[i].State = "Enabled") then
    if (eeCRCs[i] <> SCX_CPU1_CS_EEPROM_RESULT_TABLE[i].BASELINECRC) then
      write "<*> Passed (9003;9009) - EEPROM entry #",i," CRC has been recalculated on a Application Reset."
      ut_setrequirements CS_9003, "P"
      ut_setrequirements CS_9009, "P"
    else
      write "<!> Failed (9003;9009) - EEPROM entry #", i, " CRC was not recalculated on a Application Reset."
      ut_setrequirements CS_9003, "F"
      ut_setrequirements CS_9009, "F"
    endif
  endif
enddo

step_4_20:
write ";*********************************************************************"
write ";  Step 4.20: Dump the Application Code Segment Results Table and "
write ";  verify the CRCs have been recalculated. "
write ";*********************************************************************"
;; Application State
if (p@SCX_CPU1_CS_AppState = "Disabled") then
  write ";** Skipping CRC recalculation check because Application State is Disabled."
  goto step_4_21
endif

cmdCtr = SCX_CPU1_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,appResTblName,"A","cpu1_apprestbl4_20",hostCPU,appResTblId)
wait 5

ut_tlmwait SCX_CPU1_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (4008) - Dump of App Results Table successful."
  ut_setrequirements CS_4008, "P"
else
  write "<!> Failed (4008) - Dump of App Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_4008, "F"
endif

;; Loop and store the Application CRCs
for i = 0 to CS_MAX_NUM_APP_TABLE_ENTRIES-1 DO
  if (p@SCX_CPU1_CS_APP_RESULT_TABLE[i].State = "Enabled") then
    if (appCRCs[i] <> SCX_CPU1_CS_APP_RESULT_TABLE[i].BASELINECRC) then
      write "<*> Passed (9005;9009) - App entry #",i," CRC has been recalculated on a Application Reset."
      ut_setrequirements CS_9005, "P"
      ut_setrequirements CS_9009, "P"
    else
      write "<!> Failed (9005;9009) - App entry #", i, " CRC was not recalculated on a Application Reset."
      ut_setrequirements CS_9005, "F"
      ut_setrequirements CS_9009, "F"
    endif
  endif
enddo

step_4_21:
write ";*********************************************************************"
write ";  Step 4.21: Dump the Table Results Table and verify the CRCs have "
write ";  been recalculated. "
write ";*********************************************************************"
;; Table State
if (p@SCX_CPU1_CS_TableState = "Disabled") then
  write ";** Skipping CRC recalculation check because Table State is Disabled."
  goto step_4_22
endif

cmdCtr = SCX_CPU1_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,tblResTblName,"A","cpu1_tblrestbl4_21",hostCPU,tblResTblId)
wait 5

ut_tlmwait SCX_CPU1_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (5008) - Dump of Table Results Table successful."
  ut_setrequirements CS_5008, "P"
else
  write "<!> Failed (5008) - Dump of Table Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_5008, "F"
endif
;; Loop and store the Table CRCs
for i = 0 to CS_MAX_NUM_TABLES_TABLE_ENTRIES-1 DO
  if (p@SCX_CPU1_CS_TBL_RESULT_TABLE[i].State = "Enabled") then
    if (tblCRCs[i] <> SCX_CPU1_CS_TBL_RESULT_TABLE[i].BASELINECRC) then
      write "<*> Passed (9006;9009) - Table entry #",i," CRC has been recalculated on a Application Reset."
      ut_setrequirements CS_9006, "P"
      ut_setrequirements CS_9009, "P"
    else
      write "<!> Failed (9006;9009) - Table entry #", i, " CRC was not recalculated on a Application Reset."
      ut_setrequirements CS_9006, "F"
      ut_setrequirements CS_9009, "F"
    endif
  endif
enddo

step_4_22:
write ";*********************************************************************"
write ";  Step 4.22: Dump the User-defined Memory Results Table and verify the"
write ";  CRCs have been recalculated. "
write ";*********************************************************************"
;; User-Defined Memory State
if (p@SCX_CPU1_CS_MemoryState = "Disabled") then
  write ";** Skipping CRC recalculation check because User-Defined Memory State is Disabled."
  goto step_5_0
endif

cmdCtr = SCX_CPU1_TBL_CMDPC + 1

s get_tbl_to_cvt (ramDir,memResTblName,"A","cpu1_usrrestbl4_22",hostCPU,usrResTblId)
wait 5

ut_tlmwait SCX_CPU1_TBL_CMDPC, {cmdCtr}
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (6008) - Dump of User-defined Memory Results Table successful."
  ut_setrequirements CS_6008, "P"
else
  write "<!> Failed (6008) - Dump of User-defined Memory Results Table did not increment TBL_CMDPC."
  ut_setrequirements CS_6008, "F"
endif
;; Loop and store the User-defined Memory CRCs
for i = 0 to CS_MAX_NUM_MEMORY_TABLE_ENTRIES-1 DO
  if (p@SCX_CPU1_CS_MEM_RESULT_TABLE[i].State = "Enabled") then
    if (usrCRCs[i] <> SCX_CPU1_CS_MEM_RESULT_TABLE[i].BASELINECRC) then
      write "<*> Passed (9007;9009) - User-defined Memory entry #",i," CRC has been recalculated on a Application Reset."
      ut_setrequirements CS_9007, "P"
      ut_setrequirements CS_9009, "P"
    else
      write "<!> Failed (9007;9009) - User-defined Memory entry #", i, " CRC was not recalculated on a Application Reset."
      ut_setrequirements CS_9007, "F"
      ut_setrequirements CS_9009, "F"
    endif
  endif
enddo

step_5_0:
write ";*********************************************************************"
write ";  Step 5.0: Clean-up. "
write ";*********************************************************************"
write ";  Step 5.1: Upload the default Definition files downloaded in step 1.1."
write ";*********************************************************************"
s ftp_file ("CF:0/apps","cs_mem_orig_tbl.tbl",memTableFileName,hostCPU,"P")
s ftp_file ("CF:0/apps","cs_app_orig_tbl.tbl",appTableFileName,hostCPU,"P")
s ftp_file ("CF:0/apps","cs_tbl_orig_tbl.tbl",tblTableFileName,hostCPU,"P")
s ftp_file ("CF:0/apps","cs_eeprom_orig_tbl.tbl",eeTableFileName,hostCPU,"P")

write ";*********************************************************************"
write ";  Step 5.2: Send the Power-On Reset command. "
write ";*********************************************************************"
/SCX_CPU1_ES_POWERONRESET
wait 10

close_data_center
wait 60

cfe_startup {hostCPU}
wait 5

write "**** Requirements Status Reporting"

write "--------------------------"
write "   Requirement(s) Report"
write "--------------------------"

FOR i = 0 to ut_req_array_size DO
  ut_pfindicate {cfe_requirements[i]} {ut_requirement[i]}
ENDDO

procterm:

drop ut_requirement ; needed to clear global variables
drop ut_req_array_size ; needed to clear global variables

write ";*********************************************************************"
write ";  End procedure SCX_CPU1_cs_reset2"
write ";*********************************************************************"
ENDPROC
