
/*
 * Includes
 */

#include "cs_app.h"

/* UT includes */
#include "uttest.h"
#include "utassert.h"
#include "utstubs.h"

CS_AppData_t CS_AppData;

#if (CS_PRESERVE_STATES_ON_PROCESSOR_RESET == true)
void CS_UpdateCDS(void)
{
    UT_DEFAULT_IMPL(CS_UpdateCDS);
}
#endif /* #if (CS_PRESERVE_STATES_ON_PROCESSOR_RESET == true) */
