/************************************************************************
 ** File: cs_init.h
 **
 **   Copyright (c) 2007-2014 United States Government as represented by the 
 **   Administrator of the National Aeronautics and Space Administration. 
 **   All Other Rights Reserved.  
 **
 **   This software was created at NASA's Goddard Space Flight Center.
 **   This software is governed by the NASA Open Source Agreement and may be 
 **   used, distributed and modified only pursuant to the terms of that 
 **   agreement.
 **
 ** Purpose: 
 **   Initialization subroutines for CS.
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 **   CFS CS Heritage Analysis Document
 **   CFS CS CDR Package
 ** 
 *************************************************************************/
#ifndef _cs_init_
#define _cs_init_

/**************************************************************************
 **
 ** Include section
 **
 **************************************************************************/
#include "cfe.h"

/************************************************************************/
/** \brief Initializes the Software Bus Pipes for the Checksum Application
 **  
 **  \par Description
 **       Configures cFE Software Bus resources used by the Checksum
 **       application.
 **
 **  \par Assumptions, External Events, and Notes:
 **       None
 **       
 **  \returns
 **  \retcode #CFE_SUCCESS  \retdesc \copydoc CFE_SUCCESS \endcode
 **  \retstmt Return codes from #CFE_SB_CreatePipe        \endcode
 **  \retstmt Return codes from #CFE_SB_Subscribe         \endcode
 **  \endreturns
 **
 *************************************************************************/
int32 CS_SbInit (void);

/************************************************************************/
/** \brief Initializes the tables for the Checksum Application
 **  
 **  \par Description
 **       Initializes all tables used by the Checksum application.
 **
 **  \par Assumptions, External Events, and Notes:
 **       None
 **       
 **  \returns
 **  \retcode #CFE_SUCCESS  \retdesc \copydoc CFE_SUCCESS \endcode
 **  \retstmt Return codes from #CS_TableInit        \endcode
 **  \endreturns
 **
 *************************************************************************/
int32 CS_InitAllTables (void);

/************************************************************************/
/** \brief Initializes the cFE and OS segments for the Checksum Application
 **  
 **  \par Description
 **       Initializes the cFE and OS segments for the Checksum Application.
 **
 **  \par Assumptions, External Events, and Notes:
 **       None
 **       
 *************************************************************************/
void CS_InitSegments(void);

#endif /* _cs_init_h_ */
