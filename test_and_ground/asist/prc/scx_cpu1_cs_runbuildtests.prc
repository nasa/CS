PROC scx_cpu1_cs_runbuildtests (directorySpec)
;*******************************************************************************
;  Proc Name:  CS_RunBuildTests
;  Test Level: none 
;		     
;  Proc Description
;   The purpose of this procedure is execute the CS build test procedures. The
;   procedures are executed one after the other until complete.
;
;  Change History
;	Date	   Name		Description
;	01/28/15   W. Moleski	Original Procedure
;
;  Arguments
;	directorySpec	The name of an existing directory under the test_logs
;			directory to store the generated log files.
;			NOTE: If specified and the directory does not exist, the
;			      log files will be lost.
;
;**********************************************************************

; Determine if proc was called with minimum # of parameters
if (%nargs < 1) then
  directorySpec = ""
endif

write ";*********************************************************************"
write ";  Executing AppCode procedure                                 "
write ";*********************************************************************"
if (directorySpec <> "") then
  s ut_runproc("scx_cpu1_cs_appcode",directorySpec)
else
  s ut_runproc("scx_cpu1_cs_appcode")
endif
wait 2

write ";*********************************************************************"
write ";  Executing CoreCode procedure                                 "
write ";*********************************************************************"
if (directorySpec <> "") then
  s ut_runproc("scx_cpu1_cs_corecode",directorySpec)
else
  s ut_runproc("scx_cpu1_cs_corecode")
endif
wait 2

write ";*********************************************************************"
write ";  Executing GenCmds procedure                                 "
write ";*********************************************************************"
if (directorySpec <> "") then
  s ut_runproc("scx_cpu1_cs_gencmds",directorySpec)
else
  s ut_runproc("scx_cpu1_cs_gencmds")
endif
wait 2

write ";*********************************************************************"
write ";  Executing NVMem procedure                                "
write ";*********************************************************************"
if (directorySpec <> "") then
  s ut_runproc("scx_cpu1_cs_nvmem",directorySpec)
else
  s ut_runproc("scx_cpu1_cs_nvmem")
endif
wait 2

write ";*********************************************************************"
write ";  Executing Reset procedure                               "
write ";*********************************************************************"
if (directorySpec <> "") then
  s ut_runproc("scx_cpu1_cs_reset",directorySpec)
else
  s ut_runproc("scx_cpu1_cs_reset")
endif
wait 2

write ";*********************************************************************"
write ";  Executing Table procedure                                "
write ";*********************************************************************"
if (directorySpec <> "") then
  s ut_runproc("scx_cpu1_cs_table",directorySpec)
else
  s ut_runproc("scx_cpu1_cs_table")
endif
wait 2

write ";*********************************************************************"
write ";  Executing UserMem procedure                                "
write ";*********************************************************************"
if (directorySpec <> "") then
  s ut_runproc("scx_cpu1_cs_usermem",directorySpec)
else
  s ut_runproc("scx_cpu1_cs_usermem")
endif
wait 2

write ";*********************************************************************"
write ";  End procedure scx_cpu1_cs_runbuildtests                           "
write ";*********************************************************************"
ENDPROC
