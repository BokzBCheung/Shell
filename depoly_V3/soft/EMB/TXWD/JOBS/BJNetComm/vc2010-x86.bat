@echo off
call "%VS100COMNTOOLS%..\..\VC\vcvarsall.bat" x86
cl /c /O2 /Oi /Ot /GF /GL /GR- /GS /Gy /MD /Zc:wchar_t- /wd4819 /D "WIN32" /D "_WIN32" /D "UNICODE" /D "_UNICODE" /I "..\..\..\CrHack\inc" /I "..\jni\win32" BJNetComm.c
link /DLL /OUT:BJNetComm.dll /DEF:BJNetComm.def /OPT:REF /OPT:ICF /LTCG /DYNAMICBASE /NXCOMPAT /ERRORREPORT:PROMPT /MACHINE:X86 /LIBPATH:..\..\..\CrHack\bin\vc2010_win32_x86 BJNetComm.obj User32.lib
cl /O2 /Oi /Ot /GF /GL /GR- /GS /Gy /MD test.c
del *.obj /Q
del *.exp /Q
pause
