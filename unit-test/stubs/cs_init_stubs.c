
/*
 * Includes
 */

#include "cs_app.h"

/* UT includes */
#include "uttest.h"
#include "utassert.h"
#include "utstubs.h"

int32 CS_SbInit(void)
{
    return UT_DEFAULT_IMPL(CS_SbInit);
}

int32 CS_InitAllTables(void)
{
    return UT_DEFAULT_IMPL(CS_InitAllTables);
}

int32 CS_InitSegments(void)
{
    return UT_DEFAULT_IMPL(CS_InitAllTables);
}
