
/*
 * Includes
 */

#include "cs_test_utils.h"
#include "cs_tbldefs.h"

/* UT includes */
#include "uttest.h"
#include "utassert.h"
#include "utstubs.h"

/*
 * Function Definitions
 */

CS_Res_EepromMemory_Table_Entry_t CS_DefaultEepromResTable[CS_MAX_NUM_EEPROM_TABLE_ENTRIES];
CS_Res_EepromMemory_Table_Entry_t CS_DefaultMemoryResTable[CS_MAX_NUM_MEMORY_TABLE_ENTRIES];
CS_Res_Tables_Table_Entry_t       CS_DefaultTablesResTable[CS_MAX_NUM_TABLES_TABLE_ENTRIES];
CS_Res_App_Table_Entry_t          CS_DefaultAppResTable[CS_MAX_NUM_APP_TABLE_ENTRIES];


int32 UT_Utils_stub_reporter_hook(void *UserObj, int32 StubRetcode, uint32 CallCount, const UT_StubContext_t *Context)
{
    uint8 i            = 0;    /* i is index */
    uint8 size_used    = 0;    /* determines size of argument to be saved */
    void *val_location = NULL; /* pointer to arg value to be saved */
    void *obj_ptr;             /* tracker indicates where to push data into UserObj */

    /* Determine where in the UserObj we should be located dependent upon CallCount */
    if (CallCount == 0)
    {
        obj_ptr = UserObj;
    }
    else
    {
        uint8 context_size = 0;

        for (i = 0; i < Context->ArgCount; ++i)
        {
            /* A UT_STUBCONTEXT_ARG_TYPE_DIRECT type indicates the arg itself is the ptr argument, add a (void*) size */
            if (Context->Meta[i].Type == UT_STUBCONTEXT_ARG_TYPE_DIRECT)
            {
                context_size += sizeof(void *);
            }
            else /* UT_STUBCONTEXT_ARG_TYPE_INDIRECT indicates the arg is pointing to the value to be saved, add its
                    size */
            {
                context_size += Context->Meta[i].Size;
            }
        }

        /* obj_ptr moves a full context_size for every call (initial value is 0) -- user object for calls > 1 must be an
         * array of contexts */
        obj_ptr = UserObj + (context_size * CallCount);
    }

    for (i = 0; i < Context->ArgCount; ++i)
    {
        /* UT_STUBCONTEXT_ARG_TYPE_DIRECT indicates the arg is the ptr that is to be saved */
        if (Context->Meta[i].Type == UT_STUBCONTEXT_ARG_TYPE_DIRECT)
        {
            val_location = &Context->ArgPtr[i];
            size_used    = sizeof(void *);
        }
        else /* UT_STUBCONTEXT_ARG_TYPE_INDIRECT indicates the arg is pointing to the value to be saved */
        {
            val_location = Context->ArgPtr[i];
            size_used    = Context->Meta[i].Size;
        }
        /* put the argument value into the user object */
        memcpy(obj_ptr, val_location, size_used);
        /* move to end of this size item in the user object */
        obj_ptr += size_used;
    }

    return StubRetcode;
}



void CS_Test_Setup(void)
{
    /* initialize test environment to default state for every test */
    UT_ResetState(0);

    CFE_PSP_MemSet(&CS_AppData, 0, sizeof(CS_AppData_t));

    CS_AppData.DefEepromTblPtr = &CS_AppData.DefaultEepromDefTable[0];
    CS_AppData.ResEepromTblPtr = &CS_DefaultEepromResTable[0];
    CS_AppData.DefMemoryTblPtr = &CS_AppData.DefaultMemoryDefTable[0];
    CS_AppData.ResMemoryTblPtr = &CS_DefaultMemoryResTable[0];
    CS_AppData.DefTablesTblPtr = &CS_AppData.DefaultTablesDefTable[0];
    CS_AppData.ResTablesTblPtr = &CS_DefaultTablesResTable[0];
    CS_AppData.DefAppTblPtr    = &CS_AppData.DefaultAppDefTable[0];
    CS_AppData.ResAppTblPtr    = &CS_DefaultAppResTable[0];

    memset(CS_DefaultEepromResTable, 0, sizeof(CS_Res_EepromMemory_Table_Entry_t) * CS_MAX_NUM_EEPROM_TABLE_ENTRIES);
    memset(CS_DefaultMemoryResTable, 0, sizeof(CS_Res_EepromMemory_Table_Entry_t) * CS_MAX_NUM_MEMORY_TABLE_ENTRIES);
    memset(CS_DefaultTablesResTable, 0, sizeof(CS_Res_Tables_Table_Entry_t) * CS_MAX_NUM_TABLES_TABLE_ENTRIES);
    memset(CS_DefaultAppResTable, 0, sizeof(CS_Res_App_Table_Entry_t) * CS_MAX_NUM_APP_TABLE_ENTRIES);

} /* end CS_Test_Setup */

void CS_Test_TearDown(void)
{
    /* cleanup test environment */
} /* end CS_Test_TearDown */

/************************/
/*  End of File Comment */
/************************/
