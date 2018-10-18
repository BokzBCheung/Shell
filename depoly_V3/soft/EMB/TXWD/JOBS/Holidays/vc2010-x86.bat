@echo off
call "%VS100COMNTOOLS%..\..\VC\vcvarsall.bat" x86
cl /c /O2 /Oi /Ot /GF /GL /GR- /GS /Gy /MD /Zc:wchar_t- /wd4819 /D "WIN32" /D "_WIN32" /D "UNICODE" /D "_UNICODE" /I "..\..\..\CrHack\inc" /I "..\jni\win32" Holidays.c
link /DLL /OUT:Holidays.dll /DEF:Holidays.def /OPT:REF /OPT:ICF /LTCG /DYNAMICBASE /NXCOMPAT /ERRORREPORT:PROMPT /MACHINE:X86 /LIBPATH:..\..\..\CrHack\bin\vc2010_win32_x86 Holidays.obj User32.lib
del *.obj /Q
del *.exp /Q
pause
