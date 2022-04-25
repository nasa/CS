PROC scx_cpu1_cs_adt3
;*******************************************************************************
;  Test Name:  cs_adt3
;  Test Level: Build Verification
;  Test Type:  Functional
;
;  Test Description
;	The purpose of this procedure is to generate an Application Definition
;	Table for the Checksum Application containing an invalid state entry.
;
;  Requirements Tested:
;	None
;
;  Prerequisite Conditions
;	None
;
;  Assumptions and Constraints
;	None.
;
;  Change History
;
;	Date		   Name		Description
;	07/18/11	Walt Moleski	Initial release.
;       03/01/17        Walt Moleski    Updated for CS 2.4.0.0 using CPU1 for
;                                       commanding and added a hostCPU variable
;                                       for the utility procs to connect to the
;                                       proper host IP address.
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

#include "cs_msgdefs.h"
#include "cs_platform_cfg.h"
#include "cs_tbldefs.h"

%liv (log_procedure) = logging

;**********************************************************************
; Define local variables
;**********************************************************************
LOCAL defAppId, defPktId
local CSAppName = "CS"
local ramDir = "RAM:0"
local hostCPU = "CPU3"
local appDefTblName = CSAppName & "." & CS_DEF_APP_TABLE_NAME

;;; Set the pkt and app IDs for the tables based upon the cpu being used
;; CPU1 is the default
defAppId = "0FAF"
defPktId = 4015

write ";*********************************************************************"
write ";  Define the Application Definition Table "
write ";********************************************************************"
;; States are 0=CS_STATE_EMPTY; 1=CS_STATE_ENABLED; 2=CS_STATE_DISABLED;
;;            3=CS_STATE_UNDEFINED
SCX_CPU1_CS_APP_DEF_TABLE[0].State = CS_STATE_ENABLED
SCX_CPU1_CS_APP_DEF_TABLE[0].Name = CSAppName
SCX_CPU1_CS_APP_DEF_TABLE[1].State = CS_STATE_EMPTY
SCX_CPU1_CS_APP_DEF_TABLE[1].Name = ""
SCX_CPU1_CS_APP_DEF_TABLE[2].State = CS_STATE_DISABLED
SCX_CPU1_CS_APP_DEF_TABLE[2].Name = "CF_APP"
SCX_CPU1_CS_APP_DEF_TABLE[3].State = CS_STATE_EMPTY
SCX_CPU1_CS_APP_DEF_TABLE[3].Name = ""
SCX_CPU1_CS_APP_DEF_TABLE[4].State = CS_STATE_ENABLED
SCX_CPU1_CS_APP_DEF_TABLE[4].Name = "SCH_LAB_APP"
SCX_CPU1_CS_APP_DEF_TABLE[5].State = 7
SCX_CPU1_CS_APP_DEF_TABLE[5].Name = "IN_VALID_APP"

local maxEntry = CS_MAX_NUM_APP_TABLE_ENTRIES - 1

;; Clear the remainder of the table
for i = 6 to maxEntry do
  SCX_CPU1_CS_APP_DEF_TABLE[i].State = CS_STATE_EMPTY
  SCX_CPU1_CS_APP_DEF_TABLE[i].Name = ""
enddo

local endmnemonic = "SCX_CPU1_CS_APP_DEF_TABLE[" & maxEntry & "].Name"

;; Create the Table Load file
s create_tbl_file_from_cvt (hostCPU,defAppId,"App Definition Table Invalid State","app_def_tbl_invalid",appDefTblName,"SCX_CPU1_CS_APP_DEF_TABLE[0].State",endmnemonic)

write ";*********************************************************************"
write ";  End procedure SCX_CPU1_cs_adt3                              "
write ";*********************************************************************"
ENDPROC
