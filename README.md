This is the fxSDK port of [3D Minecraft by 010ello010](https://www.cemetech.net/forum/viewtopic.php?t=19552). Source code is slightly modified to support compiling for Add-in Push and fx-CG100.

Build this game by `fxsdk build-cg -s`. By default it targets fx-CG50. If you want to target fx-CG100 instead, uncomment the following line in CMakeList.txt:

```cmake
#set(MPM 1)
```
