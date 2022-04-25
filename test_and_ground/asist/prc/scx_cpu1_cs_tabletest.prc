PROC scx_cpu1_cs_tabletest
;**********************************************************************

local logging = %liv (log_procedure)
%liv (log_procedure) = FALSE

#include "ut_statusdefs.h"
#include "ut_cfe_info.h"
#include "cfe_platform_cfg.h"
#include "cfe_evs_events.h"
#include "cfe_es_events.h"
#include "cfe_tbl_events.h"
#include "to_lab_events.h"
#include "cs_platform_cfg.h"
#include "cs_events.h"
#include "cs_tbldefs.h"
#include "tst_cs_events.h"
#include "tst_tbl_events.h"

%liv (log_procedure) = logging

#define CS_1002		0
#define CS_1003		1
#define CS_1004		2
#define CS_2003		3
#define CS_3003		4
#define CS_3008		5
#define CS_4002		6
#define CS_5000 	7
#define CS_50001	8
#define CS_50002	9
#define CS_50003	10
#define CS_5001		11
#define CS_5002		12
#define CS_5003		13
#define CS_5004 	14
#define CS_5005		15
#define CS_50051	16
#define CS_50052	17
#define CS_5006		18
#define CS_5007		19
#define CS_5008		20
#define CS_6002		21
#define CS_7000		22
#define CS_8000		23
#define CS_8001		24
#define CS_9000		25
#define CS_9001		26

global ut_req_array_size = 26
global ut_requirement[0 .. ut_req_array_size]

for i = 0 to ut_req_array_size DO
  ut_requirement[i] = "U"
enddo

;**********************************************************************
; Set the local values
;**********************************************************************
local cfe_requirements[0 .. ut_req_array_size] = ["CS_1002", "CS_1003", "CS_1004", "CS_2003", "CS_3003", "CS_3008", "CS_4002", "CS_5000", "CS_5000.1", "CS_5000.2", "CS_5000.3", "CS_5001", "CS_5002", "CS_5003", "CS_5004", "CS_5005", "CS_5005.1", "CS_5005.2", "CS_5006", "CS_5007", "CS_5008", "CS_6002", "CS_7000", "CS_8000", "CS_8001", "CS_9000", "CS_9001" ]

;**********************************************************************
; Define local variables
;**********************************************************************
LOCAL rawcmd
LOCAL stream
LOCAL defTblId, defPktId, resTblId, resPktId
local i,tblIndex,tblName,foundTbl
local CSAppName = "CS"
local ramDir = "RAM:0"
local hostCPU = "CPU3"
local tblDefTblName = CSAppName & "." & CS_DEF_TABLES_TABLE_NAME
local tblResTblName = CSAppName & "." & CS_RESULTS_TABLES_TABLE_NAME
local appDefTblName = CSAppName & "." & CS_DEF_APP_TABLE_NAME
local appResTblName = CSAppName & "." & CS_RESULTS_APP_TABLE_NAME

;;; Set the pkt and app IDs for the tables based upon the cpu being used
;; CPU1 is the default
defTblId = "0FAE"
resTblId = "0FB2"
defPktId = 4014
resPktId = 4018

write ";*********************************************************************"
write ";  Step 2.9.2: Send the Recompute Table CRC command for the results "
write ";  table in order to verify the flag states. "
write ";*********************************************************************"
ut_setupevents "SCX", "CPU1", {CSAppName}, CS_RECOMPUTE_TABLES_STARTED_DBG_EID, "DEBUG", 1
ut_setupevents "SCX", "CPU1", {CSAppName}, CS_RECOMPUTE_FINISH_TABLES_INF_EID, "INFO", 2

;; Wait for the next HK Pkt
local seqTlmItem = "p0A4scnt"
local currSCnt = {seqTlmItem}
local expectedSCnt = currSCnt + 1

ut_tlmwait {seqTlmItem}, {expectedSCnt}
wait 3
;; Send the Recompute Command
/SCX_CPU1_CS_RecomputeTableName TableName="CS.ResTablesTbl"

;; Wait for the next HK Pkt
currSCnt = {seqTlmItem}
expectedSCnt = currSCnt + 1

ut_tlmwait {seqTlmItem}, {expectedSCnt}
;; Verify the telemetry flag is set to TRUE (5005)
if (p@SCX_CPU1_CS_RecomputeInProgress = "True") then
  write "<*> Passed (5005) - In Progress Flag set to True as expected."
  ut_setrequirements CS_5005, "P"
else
  write "<!> Failed (5005) - In Progress Flag set to False when True was expected."
  ut_setrequirements CS_5005, "F"
endif

;; Check for the Recompute finished event message
ut_tlmwait SCX_CPU1_find_event[2].num_found_messages, 1, 100
if (UT_TW_Status = UT_Success) then
  write "<*> Passed (1003;5005.1) - Expected Event Msg ",CS_RECOMPUTE_FINISH_TABLES_INF_EID," rcv'd."
  ut_setrequirements CS_1003, "P"
  ut_setrequirements CS_50051, "P"
else
  write "<!> Failed (1003;5005.1) - Event message ", SCX_CPU1_evs_eventid," rcv'd. Expected Event Msg ",CS_RECOMPUTE_FINISH_TABLES_INF_EID,"."
  ut_setrequirements CS_1003, "F"
  ut_setrequirements CS_50051, "F"
endif

;; Wait for the next HK Pkt
currSCnt = {seqTlmItem}
expectedSCnt = currSCnt + 1

ut_tlmwait {seqTlmItem}, {expectedSCnt}
;; Verify the telemetry flag is set to FALSE (5005.1)
if (p@SCX_CPU1_CS_RecomputeInProgress = "False") then
  write "<*> Passed (5005.1) - In Progress Flag set to False as expected."
  ut_setrequirements CS_50051, "P"
else
  write "<!> Failed (5005.1) - In Progress Flag set to True when False was expected."
  ut_setrequirements CS_50051, "F"
endif

ENDPROC
