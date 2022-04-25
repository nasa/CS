core Flight System (cFS) Checksum Application (CS) 
==================================================

Open Source Release Readme
==========================

CS Release 2.5.0

Date: 
9/24/2021

Introduction
-------------
  The Checksum application (CS) is a core Flight System (cFS) application that 
  is a plug in to the Core Flight Executive (cFE) component of the cFS.  
  
  The CS application is used for for ensuring the integrity of onboard memory.  
  CS calculates Cyclic Redundancy Checks (CRCs) on the different memory regions 
  and compares the CRC values with a baseline value calculated at system startup. 
  CS has the ability to ensure the integrity of cFE applications, cFE tables, the 
  cFE core, the onboard operating system (OS), onboard EEPROM, as well as, any 
  memory regions ("Memory") specified by the users.

  The CS application is written in C and depends on the cFS Operating System 
  Abstraction Layer (OSAL) and cFE components. There is additional CS application 
  specific configuration information contained in the application user's guide.

  Developer's guide information can be generated using Doxygen:
  doxygen cs_doxygen_config.txt
 
 
Software Included
------------------
  Checksum application (CS) 2.5.0
  
 
Software Required
------------------
 cFS Caelum

 Note: An integrated bundle including the cFE, OSAL, and PSP can
 be obtained at https://github.com/nasa/cfs
 
About cFS
----------
  The cFS is a platform and project independent reusable software framework and
  set of reusable applications developed by NASA Goddard Space Flight Center.
  This framework is used as the basis for the flight software for satellite data
  systems and instruments, but can be used on other embedded systems.  More
  information on the cFS can be found at http://cfs.gsfc.nasa.gov

EOF
