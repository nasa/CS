##############################################################################
## File: README.txt
##
## Purpose: CFS CS application unit test instructions, results, and coverage
##############################################################################

-------------------------
CS Unit Test Instructions
-------------------------
This unit test was run in a virtual machine running Ubuntu 18.04 and uses the
ut-assert stubs and default hooks for the cFE, OSAL and PSP.

To run the unit test enter the following commands at the command line prompt in
the top-level cFS directory (after ensuring that CS is listed as a target). Note
that in order to successfully compile the unit tests the "-Werror" compilation
flag must be disabled.

make distclean
make SIMULATION=native ENABLE_UNIT_TESTS=true prep
make
make test
make lcov

CS 2.5.0 Unit Test Results:

Tests Executed:    432
Assert Pass Count: 2348
Assert Fail Count: 0

==========================================================================
cs_app.c - Line Coverage:      99.3%
           Function Coverage: 100.0%
           Branch Coverage:    98.9%

==========================================================================
cs_app_cmds.c - Line Coverage:     100.0%
                Function Coverage: 100.0%
                Branch Coverage:   100.0%

==========================================================================
cs_cmds.c - Line Coverage:     100.0%
            Function Coverage: 100.0%
            Branch Coverage:   100.0%

==========================================================================
cs_compute.c - Line Coverage:     100.0%
               Function Coverage: 100.0%
               Branch Coverage:   100.0%

==========================================================================
cs_eeprom_cmds.c - Line Coverage:     100.0%
                   Function Coverage: 100.0%
                   Branch Coverage:   100.0%

==========================================================================
cs_init.c - Line Coverage:     100.0%
            Function Coverage: 100.0%
            Branch Coverage:   100.0%

==========================================================================
cs_memory_cmds.c - Line Coverage:     100.0%
                   Function Coverage: 100.0%
                   Branch Coverage:   100.0%

==========================================================================
cs_table_cmds.c - Line Coverage:     100.0%
                  Function Coverage: 100.0%
                  Branch Coverage:   100.0%

==========================================================================
cs_table_processing.c - Line Coverage:     100.0%
                        Function Coverage: 100.0%
                        Branch Coverage:   100.0%

==========================================================================
cs_utils.c - Line Coverage:     100.0%
             Function Coverage: 100.0%
             Branch Coverage:   97.8%

This file has branches that cannot be covered due to the way a function 
return value is initialized,  This is captured in ticket GSFCCFS-1730.
==========================================================================
