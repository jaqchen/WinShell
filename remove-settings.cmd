@echo off

rem Get the directory containing the script
set WEDIR=%~dp0
set DFTPATH=C:\Windows\System32;C:\Windows;C:\Windows\System32\Wbem
set PATH=%WEDIR%\bin;%DFTPATH%

rem Invoke abspath.exe application
for /f "delims=" %%p in ('abspath.exe') do set WEDIR=%%p

cd /d "%WEDIR%"
busybox rm -rf -v "customize.cmd" "bin/pathenv.txt"
pause
exit
