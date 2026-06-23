#include <appdef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "helpers/macros.h"

extern "C" {
    #include "doom/d_main.h"
    #include "doom/d_vars.h"
}

APP_NAME(APPNAME_STRING)
APP_DESCRIPTION("Doom port for the ClassPad II")
APP_AUTHOR("diddyholz")
APP_VERSION("1.0.0")

void Main_SetupDoomEnvVars(void);

int main(int argc, char ** argv)
{
    if (D_VarsInit() == -1)
    {
        fprintf(stderr, "Error initializing d_vars struct\n");
        exit(EXIT_FAILURE);
    }

    atexit(D_VarsCleanup);

    myargc = argc;
    myargv = argv;
    Main_SetupDoomEnvVars();

    D_DoomMain();
    
    exit(EXIT_SUCCESS);
}

void Main_SetupDoomEnvVars(void)
{
    setenv("HOME", DIRECTORY_DOOM, true);
    setenv("DOOMWADDIR", DIRECTORY_WAD, true);
}