@echo off
rem Created by xiaoqzye@qq.com
rem Simple Windows CLI environment
rem 2021/10/23

setlocal
title Shell For Win32

rem Get the directory containing the script
set WEDIR=%~dp0
set DFTPATH=C:\Windows\System32;C:\Windows;C:\Windows\System32\Wbem
set PATH=%WEDIR%;%DFTPATH%

rem Invoke abspath.exe application
for /f "delims=" %%p in ('abspath.exe') do set WEDIR=%%p

rem Setup HOME and PATH environment variables
set HOME=%WEDIR%/home
set PATH=%WEDIR%/bin;%DFTPATH%
if not exist "%HOME%" (
	busybox mkdir -p "%HOME%/.viminfo"
	busybox cp -v "%WEDIR%/vimrc" "%HOME%/.vimrc"
	busybox dos2unix "%HOME%/.vimrc"
)

rem Goto Windows Environment directory
cd /d "%WEDIR%"
rem Invoke simple shell script for localization
busybox ash localize.sh

rem Invoke customize script to setup environment
if exist "%WEDIR%/customize.cmd" call "%WEDIR%/customize.cmd"

rem Remove unused variables
set WEDIR=
set DFTPATH=

rem Invoke cmd.exe as the shell
cmd.exe /k cd /d "%HOME%"
endlocal
exit /b 0
