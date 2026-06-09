#include <cstdlib>
#include <sdk/os/mcs.h>
#include <sdk/os/debug.h>
#include "../helpers/macros.h"

#include "stdio.h"

#define MCS_DIRECTORY APPNAME_STRING

uint32_t align_val(uint32_t val, uint32_t at)
{
  if ((val % at) == 0) 
  {
    return val;
  }

  return val + (at - (val % at));
}

extern "C"
void
I_McsInit(void)
{
    auto ret = MCS_CreateFolder(MCS_DIRECTORY, nullptr);
    switch (ret)
    {
        case MCS_OK:
        case MCS_FOLDER_EXISTS:
        return;

        default:
        break;
    }
    fprintf(stderr, "Could not create MCS folder \"%s\": %i", MCS_DIRECTORY, ret);
    Debug_WaitKey();
    exit(EXIT_FAILURE);
}

extern "C"
int 
I_McsWrite(const char *name, 
    const void *data, 
    uint32_t size)
{
    size = align_val(size, 4);

    int ret = MCS_SetVariable(MCS_DIRECTORY, name, VARTYPE_STR, size, (void *)data);

    if (ret)
    {
        return -1;
    }

    return 0;
}

extern "C"
int 
I_McsRead(const char *name, 
    void **data, 
    uint32_t *size)
{
    MCS_VariableType vartype;
    char *name2;
    void *tmpdata;
    uint32_t tmpsize;

    if (!data)
    {
        data = &tmpdata;
    }

    if (!size)
    {
        size = &tmpsize;
    }

    if (MCS_GetVariable(MCS_DIRECTORY, name, &vartype, &name2, data, size))
    {
        return -1;
    }

    return 0;
}
