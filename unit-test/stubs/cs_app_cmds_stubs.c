
/*
 * Includes
 */

#include "cs_app_cmds.h"

/* UT includes */
#include "uttest.h"
#include "utassert.h"
#include "utstubs.h"

void CS_DisableAppCmd(const CFE_SB_Buffer_t *BufPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_DisableAppCmd), BufPtr);
    UT_DEFAULT_IMPL(CS_DisableAppCmd);
}

void CS_EnableAppCmd(const CFE_SB_Buffer_t *BufPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_EnableAppCmd), BufPtr);
    UT_DEFAULT_IMPL(CS_EnableAppCmd);
}

void CS_ReportBaselineAppCmd(const CFE_SB_Buffer_t *BufPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_ReportBaselineAppCmd), BufPtr);
    UT_DEFAULT_IMPL(CS_ReportBaselineAppCmd);
}

void CS_RecomputeBaselineAppCmd(const CFE_SB_Buffer_t *BufPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_RecomputeBaselineAppCmd), BufPtr);
    UT_DEFAULT_IMPL(CS_RecomputeBaselineAppCmd);
}

void CS_DisableNameAppCmd(const CFE_SB_Buffer_t *BufPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_DisableNameAppCmd), BufPtr);
    UT_DEFAULT_IMPL(CS_DisableNameAppCmd);
}

void CS_EnableNameAppCmd(const CFE_SB_Buffer_t *BufPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_EnableNameAppCmd), BufPtr);
    UT_DEFAULT_IMPL(CS_EnableNameAppCmd);
}
