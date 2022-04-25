
/*
 * Includes
 */

#include "cs_table_cmds.h"

/* UT includes */
#include "uttest.h"
#include "utassert.h"
#include "utstubs.h"

void CS_DisableTablesCmd(const CFE_SB_Buffer_t *BufPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_DisableTablesCmd), BufPtr);
    UT_DEFAULT_IMPL(CS_DisableTablesCmd);
}

void CS_EnableTablesCmd(const CFE_SB_Buffer_t *BufPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_EnableTablesCmd), BufPtr);
    UT_DEFAULT_IMPL(CS_EnableTablesCmd);
}

void CS_ReportBaselineTablesCmd(const CFE_SB_Buffer_t *BufPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_ReportBaselineTablesCmd), BufPtr);
    UT_DEFAULT_IMPL(CS_ReportBaselineTablesCmd);
}

void CS_RecomputeBaselineTablesCmd(const CFE_SB_Buffer_t *BufPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_RecomputeBaselineTablesCmd), BufPtr);
    UT_DEFAULT_IMPL(CS_RecomputeBaselineTablesCmd);
}

void CS_DisableNameTablesCmd(const CFE_SB_Buffer_t *BufPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_DisableNameTablesCmd), BufPtr);
    UT_DEFAULT_IMPL(CS_DisableNameTablesCmd);
}

void CS_EnableNameTablesCmd(const CFE_SB_Buffer_t *BufPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_EnableNameTablesCmd), BufPtr);
    UT_DEFAULT_IMPL(CS_EnableNameTablesCmd);
}
