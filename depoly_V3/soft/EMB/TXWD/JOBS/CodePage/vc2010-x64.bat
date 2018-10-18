@echo off
call "%VS100COMNTOOLS%..\..\VC\vcvarsall.bat" x86_amd64
cl /c /O2 /Oi /Ot /GF /GL /GR- /GS /Gy /MD /Zc:wchar_t- /wd4819 /D "WIN64" /D "_WIN64" /D "UNICODE" /D "_UNICODE" /I "..\..\..\CrHack\inc" CodePage.c
link /DLL /OUT:CodePage.dll /OPT:REF /OPT:ICF /LTCG /DYNAMICBASE /NXCOMPAT /ERRORREPORT:PROMPT /MACHINE:X64 /LIBPATH:..\..\..\CrHack\bin\vc2010_win32_x64 CodePage.obj User32.lib
del *.obj /Q
del *.exp /Q
pause
