
/*
 * Includes
 */

#include "cs_memory_cmds.h"

/* UT includes */
#include "uttest.h"
#include "utassert.h"
#include "utstubs.h"

void CS_DisableMemoryCmd(const CFE_SB_Buffer_t *BufPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_DisableMemoryCmd), BufPtr);
    UT_DEFAULT_IMPL(CS_DisableMemoryCmd);
}

void CS_EnableMemoryCmd(const CFE_SB_Buffer_t *BufPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_EnableMemoryCmd), BufPtr);
    UT_DEFAULT_IMPL(CS_EnableMemoryCmd);
}

void CS_ReportBaselineEntryIDMemoryCmd(const CFE_SB_Buffer_t *BufPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_ReportBaselineEntryIDMemoryCmd), BufPtr);
    UT_DEFAULT_IMPL(CS_ReportBaselineEntryIDMemoryCmd);
}

void CS_DisableEntryIDMemoryCmd(const CFE_SB_Buffer_t *BufPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_DisableEntryIDMemoryCmd), BufPtr);
    UT_DEFAULT_IMPL(CS_DisableEntryIDMemoryCmd);
}

void CS_RecomputeBaselineMemoryCmd(const CFE_SB_Buffer_t *BufPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_RecomputeBaselineMemoryCmd), BufPtr);
    UT_DEFAULT_IMPL(CS_RecomputeBaselineMemoryCmd);
}

void CS_EnableEntryIDMemoryCmd(const CFE_SB_Buffer_t *BufPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_EnableEntryIDMemoryCmd), BufPtr);
    UT_DEFAULT_IMPL(CS_EnableEntryIDMemoryCmd);
}

void CS_GetEntryIDMemoryCmd(const CFE_SB_Buffer_t *BufPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_GetEntryIDMemoryCmd), BufPtr);
    UT_DEFAULT_IMPL(CS_GetEntryIDMemoryCmd);
}
