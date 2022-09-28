/************************************************************************
 * NASA Docket No. GSC-18,915-1, and identified as “cFS Checksum
 * Application version 2.5.1”
 *
 * Copyright (c) 2021 United States Government as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ************************************************************************/

/**
 * @file
 *   Specification for the CFS Checksum event identifers.
 */
#ifndef CS_EVENTS_H
#define CS_EVENTS_H

/**
 * \defgroup cfscsevents CFS Checksum Event IDs
 * \{
 */

/**
 * \brief CS Initialization Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message is issued when the CFS Checksum has completed initialization.
 */
#define CS_INIT_INF_EID 1

/**
 * \brief CS No-op Command Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message is issued when a NOOP command has been received.
 */
#define CS_NOOP_INF_EID 2

/**
 * \brief CS Reset Counters Command Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  This event message is issued when a reset counters command has been received.
 */
#define CS_RESET_DBG_EID 3

/**
 * \brief CS Disable Checksumming Command Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message is issued when disable checksumming
 *  command has been received.
 */
#define CS_DISABLE_ALL_INF_EID 4

/**
 * \brief CS Enable Checksumming Command Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message is issued when enable checksumming
 *  command has been received.
 */
#define CS_ENABLE_ALL_INF_EID 5

/**
 * \brief CS Disable cFE Core Checksumming Command Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message is issued when disable checksumming
 *  for the cFE core command has been received.
 */
#define CS_DISABLE_CFECORE_INF_EID 6

/**
 * \brief CS Enable cFE Core Checksumming Command Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message is issued when enable checksumming
 *  for the cFE core command has been received.
 */
#define CS_ENABLE_CFECORE_INF_EID 7

/**
 * \brief CS Disable OS Code Checksumming Command Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message is issued when disable checksumming
 *  for the OS code segment command has been received.
 */
#define CS_DISABLE_OS_INF_EID 8

/**
 * \brief CS Enable OS Code Checksumming Command Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message is issued when enable checksumming
 *  for the OS code segment command has been received.
 */
#define CS_ENABLE_OS_INF_EID 9

/**
 * \brief CS Baseline cFE Core Checksum Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message is issued when a report baseline
 *  for the cFE core command has been received and there is
 *  a baseline computed to report.
 */
#define CS_BASELINE_CFECORE_INF_EID 10

/**
 * \brief CS Baseline cFE Core Checksum Pending Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message is issued when a report baseline
 *  for the cFE core command has been received but the
 *  baseline has not yet been computed.
 */
#define CS_NO_BASELINE_CFECORE_INF_EID 11

/**
 * \brief CS Baseline OS Code Checksum Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message is issued when a report baseline
 *  for the OS code segment command has been received
 *  and there is a baseline computed to report.
 */
#define CS_BASELINE_OS_INF_EID 12

/**
 * \brief CS Baseline OS Code Checksum Pending Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message is issued when a report baseline
 *  for the cFE core command has been received but the
 *  baseline has not yet been computed.
 */
#define CS_NO_BASELINE_OS_INF_EID 13

/**
 * \brief CS cFE Core Checksum Recompute Started Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  This event message is issued when a recompute baseline
 *  for the cFE core command has been received and the
 *  recompute task has been started.
 */
#define CS_RECOMPUTE_CFECORE_STARTED_DBG_EID 14

/**
 * \brief CS cFE Core Checksum Failed Child Task Create Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when a recompute baseline
 *  for the cFE core command has been received and the
 *  recompute failed because CFE_ES_CreateChildTask returned
 *  an error.
 */
#define CS_RECOMPUTE_CFECORE_CREATE_CHDTASK_ERR_EID 15

/**
 * \brief CS cFE Core Checksum Failed Child Task In Use Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when a recompute baseline
 *  for the cFE core command has been received and the
 *  recompute failed because there is already a CS child
 *  task running.
 */
#define CS_RECOMPUTE_CFECORE_CHDTASK_ERR_EID 16

/**
 * \brief CS OS Code Checksum Recompute Started Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  This event message is issued when a recompute baseline
 *  for the OS code segment command has been received and the
 *  recompute task has been started.
 */
#define CS_RECOMPUTE_OS_STARTED_DBG_EID 17

/**
 * \brief CS OS Code Checksum Recompute Failed Child Task Create Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when a recompute baseline
 *  for the OS code segment command has been received and the
 *  recompute failed because CFE_ES_CreateChildTask returned
 *  an error.
 */
#define CS_RECOMPUTE_OS_CREATE_CHDTASK_ERR_EID 18

/**
 * \brief CS OS Code Checksum Recompute Failed Child Task In Use Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when a recompute baseline
 *  for the OS code segment command has been received and the
 *  recompute failed because there is already a CS child
 *  task running.
 */
#define CS_RECOMPUTE_OS_CHDTASK_ERR_EID 19

/**
 * \brief CS Oneshot Checksum Started Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  This event message is issued when a OneShot calculation
 *  command has been received and the OneShot task has been started.
 */
#define CS_ONESHOT_STARTED_DBG_EID 20

/**
 * \brief CS Oneshot Checksum Failed Child Task Create Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when a OneShot calculation
 *  command has been received and the OneShot failed because
 *  CFE_ES_CreateChildTask returned an error.
 */
#define CS_ONESHOT_CREATE_CHDTASK_ERR_EID 21

/**
 * \brief CS Oneshot Checksum Failed Child Task In Use Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when a OneShot command
 *  has been received and the OneShot failed because
 *  there is already a CS child task running.
 */
#define CS_ONESHOT_CHDTASK_ERR_EID 22

/**
 * \brief CS Oneshot Checksum Failed Invalid Memory Range Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when a OneShot command
 *  has been received and the OneShot failed because
 *  CFE_PSP_MemValidateRange returned an error.
 */
#define CS_ONESHOT_MEMVALIDATE_ERR_EID 23

/**
 * \brief CS Oneshot Checksum Cancelled Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message is issued when a  cancel OneShot calculation
 *  command has been received and the OneShot task has been cancelled.
 */
#define CS_ONESHOT_CANCELLED_INF_EID 24

/**
 * \brief CS Oneshot Checksum Failed Child Task Delete Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when a cancel OneShot calculation
 *  command has been received and the cancel OneShot failed because
 *  CFE_ES_DeleteChildTask returned an error.
 */
#define CS_ONESHOT_CANCEL_DELETE_CHDTASK_ERR_EID 25

/**
 * \brief CS Oneshot Checksum Cancel Failed No Oneshot Active Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when a cancel OneShot command
 *  has been received and the cancel OneShot failed because
 *  there was no OneShot child task running.
 */
#define CS_ONESHOT_CANCEL_NO_CHDTASK_ERR_EID 26

/**
 * \brief CS EEPROM Checksum Miscompare Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when a checksum miscompare occurs
 *  when checksumming entries in the EEPROM table.
 */
#define CS_EEPROM_MISCOMPARE_ERR_EID 27

/**
 * \brief CS Memory Checksum Miscompare Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when a checksum miscompare occurs
 *  when checksumming entries in the user-define memory table
 */
#define CS_MEMORY_MISCOMPARE_ERR_EID 28

/**
 * \brief CS Table Checksum Miscompare Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when a checksum miscompare occurs
 *  when checksumming entries in the table of tables to checksum.
 */
#define CS_TABLES_MISCOMPARE_ERR_EID 29

/**
 * \brief CS App Checksum Miscompare Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when a checksum miscompare occurs
 *  when checksumming entries in the table of applications to checksum.
 */
#define CS_APP_MISCOMPARE_ERR_EID 30

/**
 * \brief CS cFE Core Checksum Miscompare Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when a checksum miscompare occurs
 *  when checksumming the cFE Core.
 */
#define CS_CFECORE_MISCOMPARE_ERR_EID 31

/**
 * \brief CS OS Code Checksum Miscompare Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when a checksum miscompare occurs
 *  when checksumming the OS code segment.
 */
#define CS_OS_MISCOMPARE_ERR_EID 32

/**
 * \brief CS Invalid Command Pipe Message ID Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when a software bus message is received
 *  with an invalid message ID.
 */
#define CS_MID_ERR_EID 33

/**
 * \brief CS Invalid Ground Command Code Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when a software bus message is received
 *  with an invalid command code.
 */
#define CS_CC1_ERR_EID 34

/**
 * \brief CS App Termination Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when CS has a critical error in its
 *  main loop
 */
#define CS_EXIT_ERR_EID 35

/**
 * \brief CS Invalid Message Length Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when command message is received with a message
 *  length that doesn't match the expected value.
 */
#define CS_LEN_ERR_EID 36

/**********************************************************************/
/*EEPROM Commands                                                     */
/**********************************************************************/

/**
 * \brief CS Disable EEPROM Checksumming Command Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message is issued when disable checksumming
 *  for the Eeprom table command has been received.
 */
#define CS_DISABLE_EEPROM_INF_EID 37

/**
 * \brief CS Enable EEPROM Checksumming Command Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message is issued when enable checksumming
 *  for the Eeprom table command has been received.
 */
#define CS_ENABLE_EEPROM_INF_EID 38

/**
 * \brief CS Baseline EEPROM Checksum Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message is issued when a report baseline
 *  for the Eeprom entry specifiedcommand has been received
 *  and there is a baseline computed to report.
 */
#define CS_BASELINE_EEPROM_INF_EID 39

/**
 * \brief CS Baseline EEPROM Checksum Pending Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message is issued when a report baseline
 *  for the command specified entry command has been
 *  received but the baseline has not yet been computed.
 */
#define CS_NO_BASELINE_EEPROM_INF_EID 40

/**
 * \brief CS Baseline EEPROM Checksum Failed Invalid Entry ID Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when a report baseline
 *  for the command specified entry command has been
 *  received but specified Entry ID is invalid.
 */
#define CS_BASELINE_INVALID_ENTRY_EEPROM_ERR_EID 41

/**
 * \brief CS EEPROM Checksum Recompute Started Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  This event message is issued when a recompute baseline
 *  for the specified Eeprom Entry ID command has been received and the
 *  recompute task has been started.
 */
#define CS_RECOMPUTE_EEPROM_STARTED_DBG_EID 42

/**
 * \brief CS EEPROM Checksum Recompute Failed Child Task Create Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when a recompute baseline
 *  for the specified Eeprom Entry ID command has been
 *  received and the recompute failed because
 *  CFE_ES_CreateChildTask returned an error.
 */
#define CS_RECOMPUTE_EEPROM_CREATE_CHDTASK_ERR_EID 43

/**
 * \brief CS EEPROM Checksum Recompute Failed Invalid Entry ID Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when a recompute baseline
 *  for the command specified entry command has been
 *  received but specified Entry ID is invalid.
 */
#define CS_RECOMPUTE_INVALID_ENTRY_EEPROM_ERR_EID 44

/**
 * \brief CS EEPROM Checksum Recompute Failed Child Task In Use Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when a recompute baseline
 *  for the command specified Entry ID command has been
 *  received and the recompute failed because there is
 *  already a CS child task running.
 */
#define CS_RECOMPUTE_EEPROM_CHDTASK_ERR_EID 45

/**
 * \brief CS EEPROM Checksum Entry ID Enabled Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:invalid
 *
 *  This event message is issued when an enable Eeprom Entry ID
 *  command is accepted.
 */
#define CS_ENABLE_EEPROM_ENTRY_INF_EID 46

/**
 * \brief CS EEPROM Checksum Entry ID Enable Failed Invalid Entry ID Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when an enable Eeprom Entry ID
 *  command is received, but has an invalid Entry ID.
 */
#define CS_ENABLE_EEPROM_INVALID_ENTRY_ERR_EID 47

/**
 * \brief CS EEPROM Checksum Entry ID Disabled Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message is issued when a disable Eeprom Entry ID
 *  command is accepted.
 */
#define CS_DISABLE_EEPROM_ENTRY_INF_EID 48

/**
 * \brief CS EEPROM Checksum Entry ID Disable Failed Invalid Entry ID Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when a disable Eeprom Entry ID
 *  command is received, but has an invalid Entry ID.
 */
#define CS_DISABLE_EEPROM_INVALID_ENTRY_ERR_EID 49

/**
 * \brief CS EEPROM Get Entry ID Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message is issued when a Get Entry ID Eeprom
 *  command is received and the command specified address
 *  is found in the Eeprom table.
 *
 *  Note that more than one of these event messages can
 *  be sent per command (if the address is in several
 *  entries in the table).
 */
#define CS_GET_ENTRY_ID_EEPROM_INF_EID 50

/**
 * \brief CS EEPROM Get Entry ID Address Not Found Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message is issued when the command specified
 *  address in a Get Entry ID Eeprom command cannot be found
 *  in the Eeprom table.
 */
#define CS_GET_ENTRY_ID_EEPROM_NOT_FOUND_INF_EID 51

/**********************************************************************/
/*MEMORY Commands                                                     */
/**********************************************************************/

/**
 * \brief CS Disable Memory Checksumming Command Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message is issued when disable checksumming
 *  for the Memory table command has been received.
 */
#define CS_DISABLE_MEMORY_INF_EID 52

/**
 * \brief CS Enable Memory Checksumming Command Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message is issued when enable checksumming
 *  for the Memory table command has been received.
 */
#define CS_ENABLE_MEMORY_INF_EID 53

/**
 * \brief CS Baseline Memory Checksum Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message is issued when a report baseline
 *  for the Memory entry specifiedcommand has been received
 *  and there is a baseline computed to report.
 */
#define CS_BASELINE_MEMORY_INF_EID 54

/**
 * \brief CS Baseline Memory Checksum Pending Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message is issued when a report baseline
 *  for the command specified entry command has been
 *  received but the baseline has not yet been computed.
 */
#define CS_NO_BASELINE_MEMORY_INF_EID 55

/**
 * \brief CS Baseline Memory Checksum Failed Invalid Entry ID Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when a report baseline
 *  for the command specified entry command has been
 *  received but specified Entry ID is invalid.
 */
#define CS_BASELINE_INVALID_ENTRY_MEMORY_ERR_EID 56

/**
 * \brief CS Memory Checksum Recompute Started Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  This event message is issued when a recompute baseline
 *  for the specified Memory Entry ID command has been received and the
 *  recompute task has been started.
 */
#define CS_RECOMPUTE_MEMORY_STARTED_DBG_EID 57

/**
 * \brief CS Memory Checksum Recompute Failed Child Task Create Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when a recompute baseline
 *  for the specified Memory Entry ID command has been
 *  received and the recompute failed because
 *  CFE_ES_CreateChildTask returned an error.
 */
#define CS_RECOMPUTE_MEMORY_CREATE_CHDTASK_ERR_EID 58

/**
 * \brief CS Memory Checksum Recompute Failed Invalid Entry ID Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when a recompute baseline
 *  for the command specified entry command has been
 *  received but specified Entry ID is invalid.
 */
#define CS_RECOMPUTE_INVALID_ENTRY_MEMORY_ERR_EID 59

/**
 * \brief CS Memory Checksum Recompute Failed Child Task In Use Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when a recompute baseline
 *  for the command specified Entry ID command has been
 *  received and the recompute failed because there is
 *  already a CS child task running.
 */
#define CS_RECOMPUTE_MEMORY_CHDTASK_ERR_EID 60

/**
 * \brief CS Memory Checksum Entry ID Enabled Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message is issued when an enable Memory Entry ID
 *  command is accepted.
 */
#define CS_ENABLE_MEMORY_ENTRY_INF_EID 61

/**
 * \brief CS Memory Checksum Entry ID Enable Failed Invalid Entry ID Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when an enable Memory Entry ID
 *  command is received, but has an invalid Entry ID.
 */

#define CS_ENABLE_MEMORY_INVALID_ENTRY_ERR_EID 62

/**
 * \brief CS Memory Checksum Entry ID Disabled Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message is issued when a disable Memory Entry ID
 *  command is accepted.
 */
#define CS_DISABLE_MEMORY_ENTRY_INF_EID 63

/**
 * \brief CS Memory Checksum Entry ID Disable Failed Invalid Entry ID Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when a disable Memory Entry ID
 *  command is received, but has an invalid Entry ID.
 */
#define CS_DISABLE_MEMORY_INVALID_ENTRY_ERR_EID 64

/**
 * \brief CS Memory Get Entry ID Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message is issued when a Get Entry ID Memory
 *  command is received and the command specified address
 *  is found in the Memory table.
 *
 *  Note that more than one of these event messages can
 *  be sent per command (if the address is in several
 *  entries in the table).
 */
#define CS_GET_ENTRY_ID_MEMORY_INF_EID 65

/**
 * \brief CS Memory Get Entry ID Address Not Found Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message is issued when the command specified
 *  address in a Get Entry ID Memory command cannot be found
 *  in the Memory table.
 */
#define CS_GET_ENTRY_ID_MEMORY_NOT_FOUND_INF_EID 66

/**********************************************************************/
/*TABLES Commands                                                     */
/**********************************************************************/

/**
 * \brief CS Disable Tables Checksumming Command Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message is issued when disable checksumming
 *  for the Tables table command has been received.
 */
#define CS_DISABLE_TABLES_INF_EID 67

/**
 * \brief CS Enable Tables Checksumming Command Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message is issued when enable checksumming
 *  for the Tables table command has been received.
 */
#define CS_ENABLE_TABLES_INF_EID 68

/**
 * \brief CS Baseline Table Checksum Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message is issued when a report baseline
 *  for the Tables entry specifiedcommand has been received
 *  and there is a baseline computed to report.
 */
#define CS_BASELINE_TABLES_INF_EID 69

/**
 * \brief CS Baseline Table Checksum Pending Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message is issued when a report baseline
 *  for the command specified table command has been
 *  received but the baseline has not yet been computed.
 */
#define CS_NO_BASELINE_TABLES_INF_EID 70

/**
 * \brief CS Baseline Table Checksum Failed Table Not Found Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when a report baseline
 *  for the command specified table command has been
 *  received but specified table name cannot be found
 *  or is marked as #CS_STATE_EMPTY.
 */
#define CS_BASELINE_INVALID_NAME_TABLES_ERR_EID 71

/**
 * \brief CS Table Checksum Recompute Started Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  This event message is issued when a recompute baseline
 *  for the specified table command has been received and the
 *  recompute task has been started.
 */
#define CS_RECOMPUTE_TABLES_STARTED_DBG_EID 72

/**
 * \brief CS Table Checksum Recompute Failed Child Task Create Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when a recompute baseline
 *  for the specified Tables Entry ID command has been
 *  received and the recompute failed because
 *  CFE_ES_CreateChildTask returned an error.
 */
#define CS_RECOMPUTE_TABLES_CREATE_CHDTASK_ERR_EID 73

/**
 * \brief CS Table Checksum Recompute Failed Table Not Found Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when a recompute baseline
 *  for the command specified table command has been
 *  received but specified table cannot be found in CS
 *  or is marked as #CS_STATE_EMPTY.
 */
#define CS_RECOMPUTE_UNKNOWN_NAME_TABLES_ERR_EID 74

/**
 * \brief CS Table Checksum Recompute Failed Child Task In Use Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when a recompute baseline
 *  for the command specified table command has been
 *  received and the recompute failed because there is
 *  already a CS child task running.
 */
#define CS_RECOMPUTE_TABLES_CHDTASK_ERR_EID 75

/**
 * \brief CS Table Checksum Enabled Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message is issued when an enable Table name
 *  command is accepted.
 */
#define CS_ENABLE_TABLES_NAME_INF_EID 76

/**
 * \brief CS Table Checksum Failed Table Not Found Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when an enable Table name
 *  command is received, but has an unknown name
 *  or is marked as #CS_STATE_EMPTY.
 */
#define CS_ENABLE_TABLES_UNKNOWN_NAME_ERR_EID 77

/**
 * \brief CS Table Checksum Disabled Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message is issued when a disable table name
 *  command is accepted.
 */
#define CS_DISABLE_TABLES_NAME_INF_EID 78

/**
 * \brief CS Table Checksum Disable Failed Table Not Found Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when a disable able name
 *  command is received, but has an unknown name
 *  or is marked as #CS_STATE_EMPTY.
 */
#define CS_DISABLE_TABLES_UNKNOWN_NAME_ERR_EID 79

/**********************************************************************/
/*APP Commands                                                        */
/**********************************************************************/

/**
 * \brief CS Disable Apps Checksumming Command Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message is issued when disable checksumming
 *  for the App table command has been received.
 */
#define CS_DISABLE_APP_INF_EID 80

/**
 * \brief CS Enable Apps Checksumming Command Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message is issued when enable checksumming
 *  for the App table command has been received.
 */
#define CS_ENABLE_APP_INF_EID 81

/**
 * \brief CS Baseline App Checksum Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message is issued when a report baseline
 *  for the App entry specified command has been received
 *  and there is a baseline computed to report.
 */
#define CS_BASELINE_APP_INF_EID 82

/**
 * \brief CS Baseline App Checksum Pending Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message is issued when a report baseline
 *  for the command specified app command has been
 *  received but the baseline has not yet been computed.
 */
#define CS_NO_BASELINE_APP_INF_EID 83

/**
 * \brief CS Baseline App Checksum Failed App Not Found Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when a report baseline
 *  for the command specified app command has been
 *  received but specified app name cannot be found
 *  or is marked as #CS_STATE_EMPTY.
 */
#define CS_BASELINE_INVALID_NAME_APP_ERR_EID 84

/**
 * \brief CS App Checksum Recompute Started Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  This event message is issued when a recompute baseline
 *  for the specified app command has been received and the
 *  recompute task has been started.
 */
#define CS_RECOMPUTE_APP_STARTED_DBG_EID 85

/**
 * \brief CS App Checksum Recompute Failed Create Child Task Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when a recompute baseline
 *  for the specified app command has been
 *  received and the recompute failed because
 *  CFE_ES_CreateChildTask returned an error.
 */
#define CS_RECOMPUTE_APP_CREATE_CHDTASK_ERR_EID 86

/**
 * \brief CS App Checksum Recompute Failed App Not Found Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when a recompute baseline
 *  for the command specified app command has been
 *  received but specified app cannot be found in CS
 *  or is marked as #CS_STATE_EMPTY.
 */
#define CS_RECOMPUTE_UNKNOWN_NAME_APP_ERR_EID 87

/**
 * \brief CS App Checksum Recompute Failed Child Task In Use Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when a recompute baseline
 *  for the command specified app command has been
 *  received and the recompute failed because there is
 *  already a CS child task running.
 */
#define CS_RECOMPUTE_APP_CHDTASK_ERR_EID 88

/**
 * \brief CS App Checksum Enabled Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message is issued when an enable app name
 *  command is accepted.
 */
#define CS_ENABLE_APP_NAME_INF_EID 89

/**
 * \brief CS App Checksum Enable Failed App Not Found Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when an enable app name
 *  command is received, but has an unknown name
 *  or is marked as #CS_STATE_EMPTY.
 */
#define CS_ENABLE_APP_UNKNOWN_NAME_ERR_EID 90

/**
 * \brief CS App Checksum Disabled Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message is issued when a disable app name
 *  command is accepted.
 */
#define CS_DISABLE_APP_NAME_INF_EID 91

/**
 * \brief CS App Checksum Disable Failed App Not Found Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when a disable app name
 *  command is received, but has an unknown name
 *  or is marked as #CS_STATE_EMPTY.
 */
#define CS_DISABLE_APP_UNKNOWN_NAME_ERR_EID 92

/**********************************************************************/
/* Compute Events                                                     */
/**********************************************************************/

/**
 * \brief CS Apps Table Checksumming Failed App Not Found Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when an app cannot be found when checksumming.
 */
#define CS_COMPUTE_APP_NOT_FOUND_ERR_EID 93

/**
 * \brief CS Tables Table Checksumming Failed Table Not Found Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when an table cannot be found when checksumming.
 */

#define CS_COMPUTE_TABLES_NOT_FOUND_ERR_EID 94

/**
 * \brief CS EEMROM or Memory Checksum Recompute Finished Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message is issued when a recompute entry for Eeprom or Memory
 *  has finished succesessfully.
 */
#define CS_RECOMPUTE_FINISH_EEPROM_MEMORY_INF_EID 95

/**
 * \brief CS Table Checksum Recompute Failed Could Not Get Address Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when a recompute entry for Tables
 *  fails because CS cannot get the address of the table.
 */
#define CS_RECOMPUTE_ERROR_TABLES_ERR_EID 96

/**
 * \brief CS App Checksum Recompute Failed Could Not Get Address Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when a recompute entry for Apps
 *  fails because CS cannot get the address of the application.
 */
#define CS_RECOMPUTE_ERROR_APP_ERR_EID 97

/**
 * \brief CS Table Recompute Finished Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message is issued when a recompute entry for a Table
 *  has finished succesessfully.
 *
 *  For the CS Table Definitions Table only, the checksum
 *  value will be incorrect. This is because all entries in this
 *  table are disabled while being recomputed and disabling the
 *  entry for itself modifies the contents of the table being
 *  recomputed. Thus, recomputing the CS Tables Definition Table
 *  checksum is not recommended.
 */
#define CS_RECOMPUTE_FINISH_TABLES_INF_EID 98

/**
 * \brief CS App Recompute Finished Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message is issued when a recompute entry for an app
 *  has finished succesessfully.
 */
#define CS_RECOMPUTE_FINISH_APP_INF_EID 99

/**
 * \brief CS Oneshot Checksum Finished Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message is issued when a OneShot command finishes
 */
#define CS_ONESHOT_FINISHED_INF_EID 100

/**********************************************************************/
/* Table processing events                                            */
/**********************************************************************/

/**
 * \brief CS EEPROM Table Validate Failed Illegal State Field Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when the Eeprom table validation function detects an illegal state
 */
#define CS_VAL_EEPROM_STATE_ERR_EID 101

/**
 * \brief CS EEPROM Table Validate Failed Illegal Checksum Range Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when the Eeprom table validation function detects an illegal checksum range
 */
#define CS_VAL_EEPROM_RANGE_ERR_EID 102

/**
 * \brief CS Memory Table Valide Failed Illegal State Field Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when the Memory table validation function detects an illegal state
 */
#define CS_VAL_MEMORY_STATE_ERR_EID 103

/**
 * \brief CS Memory Table Validate Failed Illegal Checksum Range Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when the Memory table validation function detects an illegal checksum range
 */
#define CS_VAL_MEMORY_RANGE_ERR_EID 104

/**
 * \brief CS Tables Table Validate Failed Illegal State Field Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when the Tables table validation function detects an illegal state
 */
#define CS_VAL_TABLES_STATE_ERR_EID 105

/**
 * \brief CS Apps Table Validate Failed Illegal State Field Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when the App table validation function detects an illegal state
 */
#define CS_VAL_APP_STATE_ERR_EID 106

/**
 * \brief CS EEPROM or Memory Table No Valid Entries Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message is issued when a new definition table has finished being processed
 *  and there are no valid entries in that table
 */
#define CS_PROCESS_EEPROM_MEMORY_NO_ENTRIES_INF_EID 107

/**
 * \brief CS Apps Table No Valid Entries Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message is issued when a new definition table has finished being processed
 *  and there are no valid entries in that table
 */
#define CS_PROCESS_APP_NO_ENTRIES_INF_EID 108

/**
 * \brief CS Tables Table No Valid Entries Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message is issued when a new definition table has finished being processed
 *  and there are no valid entries in that table
 */
#define CS_PROCESS_TABLES_NO_ENTRIES_INF_EID 109

/**
 * \brief CS Table Initialization Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when a table initialization failed.
 */
#define CS_TBL_INIT_ERR_EID 110

/**
 * \brief CS Table Update Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when a problem occurs during a table update.
 */
#define CS_TBL_UPDATE_ERR_EID 111

/***********************************************************************************************/

/**
 * \brief CS Software Buse Create Pipe Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when CFE_SB_CreatePipe fails for the command pipe
 */
#define CS_INIT_SB_CREATE_ERR_EID 112

/**
 * \brief CS Software Bus Subscribe To Housekeeping Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when CFE_SB_Subscribe failed to subscribe to the
 *  housekeeping MID.
 */
#define CS_INIT_SB_SUBSCRIBE_HK_ERR_EID 113

/**
 * \brief CS Software Bus Subscribe To Backtround Cycle Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when CFE_SB_Subscribe fails to subscribe to the
 *  background cycle MID.
 */
#define CS_INIT_SB_SUBSCRIBE_BACK_ERR_EID 114

/**
 * \brief CS Software Bus Subscribe to Command Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when CFE_SB_Subscribe fails to subscribe to the
 *  command MID.
 */
#define CS_INIT_SB_SUBSCRIBE_CMD_ERR_EID 115

/**
 * \brief CS EEPROM Table Initialization Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when the table could not be initialized at startup.
 */
#define CS_INIT_EEPROM_ERR_EID 116

/**
 * \brief CS Memory Table Initialization Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when the table could not be initialized at startup.
 */
#define CS_INIT_MEMORY_ERR_EID 117

/**
 * \brief CS Tables Table Initialization Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when the table could not be initialized at startup.
 */
#define CS_INIT_TABLES_ERR_EID 118

/**
 * \brief CS Apps Table Initialization Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when the table could not be initialized at startup.
 */
#define CS_INIT_APP_ERR_EID 119

/*************************************************************************************************/

/**
 * \brief CS Table Release Address Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when Table Services can't release the address for the table
 *  being checksummed.
 */
#define CS_COMPUTE_TABLES_RELEASE_ERR_EID 120

/**
 * \brief CS Get Table Info Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when there was a problem getting the table information to checksum.
 */
#define CS_COMPUTE_TABLES_ERR_EID 121

/**
 * \brief CS Get App Info Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when there was a problem getting the app information to checksum.
 */
#define CS_COMPUTE_APP_ERR_EID 122

/*************************************************************************************************/

/**
 * \brief CS EEPROM Table Update Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when the EEPROM table can not be updated.
 */
#define CS_UPDATE_EEPROM_ERR_EID 123

/**
 * \brief CS Memory Table Update Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when the Meomory table can not be updated.
 */
#define CS_UPDATE_MEMORY_ERR_EID 124

/**
 * \brief CS Tables Table Update Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when the Tables table can not be updated.
 */
#define CS_UPDATE_TABLES_ERR_EID 125

/**
 * \brief CS Apps Table Update Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when the Apps table can not be updated.
 */
#define CS_UPDATE_APP_ERR_EID 126

/**
 * \brief CS OS Code Checksum Disabled Platform Not Supported Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message is issued when CS is running on a platform that does not support
 *  getting the information of the OS text segment, such as OS X and Linux.
 */
#define CS_OS_TEXT_SEG_INF_EID 127

/**
 * \brief CS App Checksum Failed Platform Not Supported Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  This event message is issued when CS is running on a platform that does not support
 *  getting the address information of the applications, such as OS X and Linux.
 */
#define CS_COMPUTE_APP_PLATFORM_DBG_EID 128

/**
 * \brief CS Table Enable Checksum Unable To Find Entry In Definition Table Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  This event message is issued when CS successfully enables an entry (specified
 *  by name) in the Tables results table but is unable to find the same entry
 *  in the definition table (or the entry is marked as #CS_STATE_EMPTY).
 */
#define CS_ENABLE_TABLE_DEF_NOT_FOUND_DBG_EID 129

/**
 * \brief CS Table Disable Checksum Unable To Find Entry In Definition Table Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  This event message is issued when CS successfully disables an entry (specified
 *  by name) in the Tables results table but is unable to find the same entry
 *  in the definition table (or the entry is marked as #CS_STATE_EMPTY).
 */
#define CS_DISABLE_TABLE_DEF_NOT_FOUND_DBG_EID 130

/**
 * \brief CS App Enable Checksum Unable To Find Entry In Definition Table Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  This event message is issued when CS successfully enables an entry (specified
 *  by name) in the Apps results table but is unable to find the same entry
 *  in the definition table (or the entry is marked as #CS_STATE_EMPTY).
 */
#define CS_ENABLE_APP_DEF_NOT_FOUND_DBG_EID 131

/**
 * \brief CS App Disable Checksum Unable To Find Entry In Defintion Table Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  This event message is issued when CS successfully disables an entry (specified
 *  by name) in the Apps results table but is unable to find the same entry
 *  in the definition table (or the entry is marked as #CS_STATE_EMPTY).
 */
#define CS_DISABLE_APP_DEF_NOT_FOUND_DBG_EID 132

/**
 * \brief CS Memory Disable Checksum Unable To Find Entry In Definition Table Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  This event message is issued when CS successfully disables an entry (specified
 *  by name) in the Memory results table but identifies the corresponding entry in
 *  the definitions table to be set to #CS_STATE_EMPTY.
 */
#define CS_DISABLE_MEMORY_DEF_EMPTY_DBG_EID 133

/**
 * \brief CS Memory Enable Checksum Unable To Find Entry In Definition Table Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  This event message is issued when CS successfully enables an entry (specified
 *  by name) in the Memory results table but identifies the corresponding entry in
 *  the definitions table to be set to #CS_STATE_EMPTY.
 */
#define CS_ENABLE_MEMORY_DEF_EMPTY_DBG_EID 134

/**
 * \brief CS EEPROM Disable Checksum Unable To Find Entry In Definition Table Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  This event message is issued when CS successfully disables an entry (specified
 *  by name) in the Eeprom results table but identifies the corresponding entry in
 *  the definitions table to be set to #CS_STATE_EMPTY.
 */
#define CS_DISABLE_EEPROM_DEF_EMPTY_DBG_EID 135

/**
 * \brief CS EEPROM Enable Checksum Unable To Find Entry In Definition Table Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  This event message is issued when CS successfully enables an entry (specified
 *  by name) in the Eeprom results table but identifies the corresponding entry in
 *  the definitions table to be set to #CS_STATE_EMPTY.
 */
#define CS_ENABLE_EEPROM_DEF_EMPTY_DBG_EID 136

/**
 * \brief CS Tables Table Validate Failed Duplicate Name Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when CS validation for the Tables definition table finds more
 *  than one entry with the same table name.  Only one entry per table is allowed.  This event
 *  is only issued if it is the first error found during validation.
 */
#define CS_VAL_TABLES_DEF_TBL_DUPL_ERR_EID 137

/**
 * \brief CS Tables Table Validate Failed Illegal State With Empty Name Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when CS validation for the Tables definition table finds an entry
 *  that contains a zero-length name field with a state field that is not marked as #CS_STATE_EMPTY.  This
 *  event is only issued if it is the first error found during validation.
 */
#define CS_VAL_TABLES_DEF_TBL_ZERO_NAME_ERR_EID 138

/**
 * \brief CS Tables Table Verification Results Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message when CS completes validation of the tables definition table.  This message
 *  reports the number of successful (#CS_STATE_ENABLED or #CS_STATE_DISABLED) entries, the number of
 *  bad entries (due to invalid state definitions or duplicate names), and the number of entries
 *  marked as #CS_STATE_EMPTY.
 */
#define CS_VAL_TABLES_INF_EID 139

/**
 * \brief CS Apps Table Validate Failed Duplicate Name Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when CS validation for the Apps definition table finds more
 *  than one entry with the same table name.  Only one entry per table is allowed.  This event
 *  is only issued if it is the first error found during validation.
 */
#define CS_VAL_APP_DEF_TBL_DUPL_ERR_EID 140

/**
 * \brief CS Apps Table Validate Failed Illegal State With Empty Name Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when CS validation for the Apps definition table finds an entry
 *  that contains a zero-length name field with a state field that is not marked as #CS_STATE_EMPTY.  This
 *  event is only issued if it is the first error found during validation.
 */
#define CS_VAL_APP_DEF_TBL_ZERO_NAME_ERR_EID 141

/**
 * \brief CS Apps Table Verification Results Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message when CS completes validation of the Apps definition table.  This message
 *  reports the number of successful (#CS_STATE_ENABLED or #CS_STATE_DISABLED) entries, the number of
 *  bad entries (due to invalid state definitions or duplicate names), and the number of entries
 *  marked as #CS_STATE_EMPTY.
 */
#define CS_VAL_APP_INF_EID 142

/**
 * \brief CS Memory Table Verification Results Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message when CS completes validation of the Memory definition table.  This message
 *  reports the number of successful (#CS_STATE_ENABLED or #CS_STATE_DISABLED) entries, the number of
 *  bad entries (due to invalid state definitions or bad range), and the number of entries
 *  marked as #CS_STATE_EMPTY.
 */
#define CS_VAL_MEMORY_INF_EID 143

/**
 * \brief CS Eeprom Table Verification Results Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message when CS completes validation of the Eeprom definition table.  This message
 *  reports the number of successful (#CS_STATE_ENABLED or #CS_STATE_DISABLED) entries, the number of
 *  bad entries (due to invalid state definitions or bad range), and the number of entries
 *  marked as #CS_STATE_EMPTY.
 */
#define CS_VAL_EEPROM_INF_EID 144

/**
 *  \brief CS Critical Data Store Registration Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  The CS application optionally stores the table states
 *  in the Critical Data Store (CDS).  This ensures that CS
 *  will not overwrite old data storage files following a processor reset.
 *  This event is issued if CS cannot register the CDS during startup
 *  initialization.  Subsequent CDS errors are ignored by CS.
 */
#define CS_CR_CDS_REG_ERR_EID 145

/**
 *  \brief CS Critical Data Store Copy Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  The CS application optionally stores the table states
 *  in the Critical Data Store (CDS).  This ensures that CS
 *  will not overwrite old data storage files following a processor reset.
 *  This event is issued if CS cannot copy to the CDS during startup
 *  initialization.  Subsequent CDS errors are ignored by CS.
 */
#define CS_CR_CDS_CPY_ERR_EID 146

/**
 *  \brief CS Critical Data Store Restore Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  The CS application optionally stores the table states
 *  in the Critical Data Store (CDS).  This ensures that CS
 *  will not overwrite old data storage files following a processor reset.
 *  This event is issued if CS cannot restore from an existing CDS during
 *  startup initialization.  Subsequent CDS errors are ignored by CS.
 */
#define CS_CR_CDS_RES_ERR_EID 147

/**
 *  \brief CS Critical Data Store Update Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  The CS application optionally stores the table states
 *  in the Critical Data Store (CDS).  This ensures that CS
 *  will not overwrite old data storage files following a processor reset.
 *  This event is issued if CS cannot update the CDS.
 */
#define CS_UPDATE_CDS_ERR_EID 148

/**
 * \brief CS App Termination Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message is issued when CS has exited without error
 *  or exception from its main loop
 */
#define CS_EXIT_INF_EID 149

/**
 * \brief CS cFE Core Checksum Disabled Address Not Available Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message is issued when CS is not able to get the
 *  address and size of the CFE Text Segment.
 */
#define CS_CFE_TEXT_SEG_INF_EID 150

/**
 * \brief CS Command Failed Due To Recompute Or Oneshot In Progress Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when certain commands are received while a
 *  recompute or oneshot is in progress.  Affected commands are those that
 *  affect the checksum processing (such as enable/disable commands).
 */
#define CS_CMD_COMPUTE_PROG_ERR_EID 152

/**
 * \brief CS Skipping Background Cycle Due To Recompute Or Oneshote In Progress Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message is issued when the background cycle is attempted
 *  while a recompute or oneshot is in progress.
 */
#define CS_BKGND_COMPUTE_PROG_INF_EID 153

/**@}*/

#endif
