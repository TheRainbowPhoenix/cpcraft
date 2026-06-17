#include <appdef.h>
#include <sdk/os/debug.h>
#include <sdk/os/lcd.h>

extern "C" void game_main();

APP_NAME("fxcraft")
APP_AUTHOR("010ello010")
APP_DESCRIPTION("A Minecraft clone ported to ClassPad")
APP_VERSION("1.0.0")

int main(int argc, char **argv) {
    game_main();
    return 0;
}
