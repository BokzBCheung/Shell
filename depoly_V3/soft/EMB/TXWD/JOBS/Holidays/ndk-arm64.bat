@echo off
set NDK_ROOT=%DEV_ROOT%\android\ndk
set PATH=%NDK_ROOT%\prebuilt\windows\bin;%NDK_ROOT%\toolchains\aarch64-linux-android-4.9\prebuilt\windows\bin;%PATH%
set NDK_INC=%NDK_ROOT%\platforms\android-24\arch-arm64\usr\include
set NDK_LIB=%NDK_ROOT%\platforms\android-24\arch-arm64\usr\lib
copy %NDK_LIB%\*_so.o
aarch64-linux-android-gcc -c -O2 -fPIC -Wall -fshort-wchar -ffunction-sections -fdata-sections -I "%NDK_INC%" -I../../../CrHack/inc Holidays.c
aarch64-linux-android-gcc -o libHolidays.so -shared -fPIC -Wl,--gc-sections -Wl,--version-script=Holidays.map -L "%NDK_LIB%" -L../../../CrHack/bin/gcc_android_all/arm64-v8a Holidays.o -lUTILs -lCOREs -lc
aarch64-linux-android-strip --strip-unneeded libHolidays.so
del *.o /Q
pause
