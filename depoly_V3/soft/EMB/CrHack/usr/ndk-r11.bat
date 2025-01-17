@echo off
cd ..\mak
call ndk.bat arm-linux-androideabi 4.9 24 arm
move *.a ..\bin\gcc_android_all\armeabi

call ndk.bat arm-linux-androideabi 4.9 24 arm "-march=armv7-a"
move *.a ..\bin\gcc_android_all\armeabi-v7a

call ndk.bat aarch64-linux-android 4.9 24 arm64
move *.a ..\bin\gcc_android_all\arm64-v8a

call ndk.bat mipsel-linux-android 4.9 24 mips
move *.a ..\bin\gcc_android_all\mips

call ndk.bat mips64el-linux-android 4.9 24 mips64
move *.a ..\bin\gcc_android_all\mips64

call ndk.bat x86 4.9 24 x86
move *.a ..\bin\gcc_android_all\x86

call ndk.bat x86_64 4.9 24 x86_64
move *.a ..\bin\gcc_android_all\x86_64
cd ..\usr
