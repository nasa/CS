
/*
 * Includes
 */

#include "cs_compute.h"

/* UT includes */
#include "uttest.h"
#include "utassert.h"
#include "utstubs.h"

int32 CS_ComputeEepromMemory(CS_Res_EepromMemory_Table_Entry_t *ResultsEntry, uint32 *ComputedCSValue,
                             bool *DoneWithEntry)
{
    UT_Stub_RegisterContext(UT_KEY(CS_ComputeEepromMemory), ResultsEntry);
    UT_Stub_RegisterContext(UT_KEY(CS_ComputeEepromMemory), ComputedCSValue);
    UT_Stub_RegisterContext(UT_KEY(CS_ComputeEepromMemory), DoneWithEntry);

    return UT_DEFAULT_IMPL(CS_ComputeEepromMemory);
}

int32 CS_ComputeTables(CS_Res_Tables_Table_Entry_t *ResultsEntry, uint32 *ComputedCSValue, bool *DoneWithEntry)
{
    UT_Stub_RegisterContext(UT_KEY(CS_ComputeTables), ResultsEntry);
    UT_Stub_RegisterContext(UT_KEY(CS_ComputeTables), ComputedCSValue);
    UT_Stub_RegisterContext(UT_KEY(CS_ComputeTables), DoneWithEntry);

    return UT_DEFAULT_IMPL(CS_ComputeTables);
}

int32 CS_ComputeApp(CS_Res_App_Table_Entry_t *ResultsEntry, uint32 *ComputedCSValue, bool *DoneWithEntry)
{
    UT_Stub_RegisterContext(UT_KEY(CS_ComputeApp), ResultsEntry);
    UT_Stub_RegisterContext(UT_KEY(CS_ComputeApp), ComputedCSValue);
    UT_Stub_RegisterContext(UT_KEY(CS_ComputeApp), DoneWithEntry);

    return UT_DEFAULT_IMPL(CS_ComputeApp);
}

void CS_RecomputeEepromMemoryChildTask(void)
{
    UT_DEFAULT_IMPL(CS_RecomputeEepromMemoryChildTask);
}

void CS_RecomputeTablesChildTask(void)
{
    UT_DEFAULT_IMPL(CS_RecomputeTablesChildTask);
}

void CS_RecomputeAppChildTask(void)
{
    UT_DEFAULT_IMPL(CS_RecomputeAppChildTask);
}

void CS_OneShotChildTask(void)
{
    UT_DEFAULT_IMPL(CS_OneShotChildTask);
}
