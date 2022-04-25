
/*
 * Includes
 */

#include "cs_eeprom_cmds.h"

/* UT includes */
#include "uttest.h"
#include "utassert.h"
#include "utstubs.h"

void CS_DisableEepromCmd(const CFE_SB_Buffer_t *BufPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_DisableEepromCmd), BufPtr);
    UT_DEFAULT_IMPL(CS_DisableEepromCmd);
}

void CS_EnableEepromCmd(const CFE_SB_Buffer_t *BufPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_EnableEepromCmd), BufPtr);
    UT_DEFAULT_IMPL(CS_EnableEepromCmd);
}

void CS_ReportBaselineEntryIDEepromCmd(const CFE_SB_Buffer_t *BufPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_ReportBaselineEntryIDEepromCmd), BufPtr);
    UT_DEFAULT_IMPL(CS_ReportBaselineEntryIDEepromCmd);
}

void CS_DisableEntryIDEepromCmd(const CFE_SB_Buffer_t *BufPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_DisableEntryIDEepromCmd), BufPtr);
    UT_DEFAULT_IMPL(CS_DisableEntryIDEepromCmd);
}

void CS_RecomputeBaselineEepromCmd(const CFE_SB_Buffer_t *BufPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_RecomputeBaselineEepromCmd), BufPtr);
    UT_DEFAULT_IMPL(CS_RecomputeBaselineEepromCmd);
}

void CS_EnableEntryIDEepromCmd(const CFE_SB_Buffer_t *BufPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_EnableEntryIDEepromCmd), BufPtr);
    UT_DEFAULT_IMPL(CS_EnableEntryIDEepromCmd);
}

void CS_GetEntryIDEepromCmd(const CFE_SB_Buffer_t *BufPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CS_GetEntryIDEepromCmd), BufPtr);
    UT_DEFAULT_IMPL(CS_GetEntryIDEepromCmd);
}
