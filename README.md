# core Flight System (cFS) Checksum Application (CS) 

## Introduction

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

User's guide information can be generated using Doxygen (from top mission directory):
```
  make prep
  make -C build/docs/cs-usersguide cs-usersguide
```
 
## Software Required

cFS Framework (cFE, OSAL, PSP)

A demonstration bundle of the Core Flight System including the cFE, OSAL, and PSP can be obtained at https://github.com/nasa/cfs

For information about a mission ready cFS bundle, see: https://github.com/nasa/cFS#cfs-gov-mission-ready-version
 
## Known issues

See all [open issues](https://github.com/nasa/CF/issues) and closed to milestones later than this version.

## Getting Help

For best results, submit issues:questions or issues:help wanted requests at <https://github.com/nasa/cFS>.

Official cFS page: <http://cfs.gsfc.nasa.gov>
