
/*
 * Includes
 */

#include "cs_app.h"
#include <time.h>

/* UT includes */
#include "uttest.h"
#include "utassert.h"
#include "utstubs.h"

extern CS_AppData_t CS_AppData;

/*
 * Global context structures
 */

typedef struct
{
    uint16      EventID;
    uint16      EventType;
    const char *Spec;
} __attribute__((packed)) CFE_EVS_SendEvent_context_t;

typedef struct
{
    const char *Spec;
} __attribute__((packed)) CFE_ES_WriteToSysLog_context_t;

/*
 * Function Definitions
 */

int32 UT_Utils_stub_reporter_hook(void *UserObj, int32 StubRetcode, uint32
CallCount, const UT_StubContext_t *Context);
void CS_Test_Setup(void);
void CS_Test_TearDown(void);

/************************/
/*  End of File Comment */
/************************/
