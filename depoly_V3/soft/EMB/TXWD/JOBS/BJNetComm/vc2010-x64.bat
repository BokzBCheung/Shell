@echo off
call "%VS100COMNTOOLS%..\..\VC\vcvarsall.bat" x86_amd64
cl /c /O2 /Oi /Ot /GF /GL /GR- /GS /Gy /MD /Zc:wchar_t- /wd4819 /D "WIN64" /D "_WIN64" /D "UNICODE" /D "_UNICODE" /I "..\..\..\CrHack\inc" /I "..\jni\win32" BJNetComm.c
link /DLL /OUT:BJNetComm.dll /OPT:REF /OPT:ICF /LTCG /DYNAMICBASE /NXCOMPAT /ERRORREPORT:PROMPT /MACHINE:X64 /LIBPATH:..\..\..\CrHack\bin\vc2010_win32_x64 BJNetComm.obj User32.lib
cl /O2 /Oi /Ot /GF /GL /GR- /GS /Gy /MD test.c
del *.obj /Q
del *.exp /Q
pause
