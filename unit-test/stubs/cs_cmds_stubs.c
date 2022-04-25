
/*
 * Includes
 */

#include "cfe.h"
#include "cs_cmds.h"

/* UT includes */
#include "uttest.h"
#include "utassert.h"
#include "utstubs.h"

void CS_NoopCmd(const CFE_SB_Buffer_t *BufPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_NoopCmd), BufPtr);
    UT_DEFAULT_IMPL(CS_NoopCmd);
}

void CS_ResetCmd(const CFE_SB_Buffer_t *BufPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_ResetCmd), BufPtr);
    UT_DEFAULT_IMPL(CS_ResetCmd);
}

void CS_BackgroundCheckCycle(const CFE_SB_Buffer_t *BufPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_BackgroundCheckCycle), BufPtr);
    UT_DEFAULT_IMPL(CS_BackgroundCheckCycle);
}

void CS_DisableAllCSCmd(const CFE_SB_Buffer_t *BufPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_DisableAllCSCmd), BufPtr);
    UT_DEFAULT_IMPL(CS_DisableAllCSCmd);
}

void CS_EnableAllCSCmd(const CFE_SB_Buffer_t *BufPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_EnableAllCSCmd), BufPtr);
    UT_DEFAULT_IMPL(CS_EnableAllCSCmd);
}

void CS_DisableCfeCoreCmd(const CFE_SB_Buffer_t *BufPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_DisableCfeCoreCmd), BufPtr);
    UT_DEFAULT_IMPL(CS_DisableCfeCoreCmd);
}

void CS_EnableCfeCoreCmd(const CFE_SB_Buffer_t *BufPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_EnableCfeCoreCmd), BufPtr);
    UT_DEFAULT_IMPL(CS_EnableCfeCoreCmd);
}

void CS_DisableOSCmd(const CFE_SB_Buffer_t *BufPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_DisableOSCmd), BufPtr);
    UT_DEFAULT_IMPL(CS_DisableOSCmd);
}

void CS_EnableOSCmd(const CFE_SB_Buffer_t *BufPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_EnableOSCmd), BufPtr);
    UT_DEFAULT_IMPL(CS_EnableOSCmd);
}

void CS_ReportBaselineCfeCoreCmd(const CFE_SB_Buffer_t *BufPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_ReportBaselineCfeCoreCmd), BufPtr);
    UT_DEFAULT_IMPL(CS_ReportBaselineCfeCoreCmd);
}

void CS_ReportBaselineOSCmd(const CFE_SB_Buffer_t *BufPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_ReportBaselineOSCmd), BufPtr);
    UT_DEFAULT_IMPL(CS_ReportBaselineOSCmd);
}

void CS_RecomputeBaselineCfeCoreCmd(const CFE_SB_Buffer_t *BufPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_RecomputeBaselineCfeCoreCmd), BufPtr);
    UT_DEFAULT_IMPL(CS_RecomputeBaselineCfeCoreCmd);
}

void CS_RecomputeBaselineOSCmd(const CFE_SB_Buffer_t *BufPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_RecomputeBaselineOSCmd), BufPtr);
    UT_DEFAULT_IMPL(CS_RecomputeBaselineOSCmd);
}

void CS_OneShotCmd(const CFE_SB_Buffer_t *BufPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_OneShotCmd), BufPtr);
    UT_DEFAULT_IMPL(CS_OneShotCmd);
}

void CS_CancelOneShotCmd(const CFE_SB_Buffer_t *BufPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_CancelOneShotCmd), BufPtr);
    UT_DEFAULT_IMPL(CS_CancelOneShotCmd);
}
