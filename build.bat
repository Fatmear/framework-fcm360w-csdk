:: Copyright (C) 2023 Quectel Wireless Solution, Co., Ltd.  All Rights Reserved.
:: Quectel Wireless Solution Proprietary and Confidential.
::

@echo off

set Start_at=%time%
set curtitle=cmd: %~n0 %*
title %curtitle% @ %time%
set ret=0
set def_proj=FCM360W

set build_oper=%1
if "%build_oper%"=="" (
	set oper=new
) else if /i "%build_oper%"=="r" (
	set oper=r
) else if /i "%build_oper%"=="new" (
	set oper=new
) else if /i "%build_oper%"=="n" (
	set oper=new
) else if /i "%build_oper%"=="clean" (
	set oper=clean
) else if /i "%build_oper%"=="c" (
	set oper=clean
) else if /i "%build_oper%"=="h" (
	set oper=h
) else if /i "%build_oper%"=="-h" (
	set oper=h
) else if /i "%build_oper%"=="help" (
	set oper=h
) else if /i "%build_oper%"=="/h" (
	set oper=h
) else if /i "%build_oper%"=="/?" (
	set oper=h
) else if /i "%build_oper%"=="?" (
	set oper=h
) else (
	echo=
	echo ERR: unknown build operation: %build_oper%, should be r/n/new/clean/h/-h
	echo=
	set ret=1
	goto helpinfo
)
echo=

if /i "%oper%"=="h" (
	goto helpinfo
)

set ql_root_dir=%cd%
if /i "%oper%"=="clean" (
	call :cleanall %ql_root_dir%\ql_build %ql_root_dir%\ql_out
	goto doneseg
)

if /i "%2" == "" (
	echo=
	echo We need the Project to build the firmware...
	echo=
	set ret=1
	goto helpinfo
)
set buildproj=%2
::if "%buildproj%"=="" (
::	set buildproj=%def_proj%
::)
for %%i in (A B C D E F G H I J K L M N O P Q R S T U V W X Y Z) do call set buildproj=%%buildproj:%%i=%%i%%


if /i "%3" == "" (
	echo=
	echo We need the Version to build the firmware...
	echo=
	set ret=1
	goto helpinfo
)
set buildver=%3
for %%i in (A B C D E F G H I J K L M N O P Q R S T U V W X Y Z) do call set buildver=%%buildver:%%i=%%i%%
set ql_app_ver=%buildver%_APP

set ql_tool_dir=%ql_root_dir%\ql_tools
set ql_gcc_name=nds32le-elf-mculib-v3s-win
set cmd_7z=%ql_tool_dir%\7z\7z.exe

if not exist %ql_tool_dir%/%ql_gcc_name% ( 
	%cmd_7z% x -y %ql_tool_dir%/%ql_gcc_name%.7z -o%ql_tool_dir% || ( echo ------------unzip gcc failed------------- & set ret=1 & goto exit)
	echo unzip gcc done.
)

set ql_cygwin_name=cygwin
if not exist %ql_tool_dir%/%ql_cygwin_name% ( 
	%cmd_7z% x -y %ql_tool_dir%/%ql_cygwin_name%.7z -o%ql_tool_dir% || ( echo ------------unzip cygwin failed------------- & set ret=1 & goto exit)
	echo unzip cygwin done.
)

echo=
set CYGWIN_BIN_PATH=%ql_tool_dir%\%ql_cygwin_name%\bin
set PATH=%CYGWIN_BIN_PATH%;%PATH%;%ql_tool_dir%\%ql_gcc_name%\bin\
echo %PATH%

if /i "%oper%"=="new" (
	call :cleanall %ql_root_dir%\ql_build %ql_root_dir%\ql_out
)

set GCC_PATH=%ql_tool_dir%/nds32le-elf-mculib-v3s-win/bin/
cd %ql_root_dir%/ql_kernel/Boards/ecr6600/standalone/

make all
:: according to the build return to see success or not
if not %errorlevel% == 0 (
	echo.
	echo xxxxxxxxxxxxxxxxxx   ERROR   xxxxxxxxxxxxxxxxxxxxxxx
	echo             Firmware building is ERROR!
	echo xxxxxxxxxxxxxxxxxx   ERROR   xxxxxxxxxxxxxxxxxxxxxxx
	echo.
	set ret=1
	cd %ql_root_dir%
	goto doneseg
)

cd %ql_root_dir%

if not exist "ql_out" (
    mkdir ql_out
)

if not exist "%ql_root_dir%\ql_build\standalone\ECR6600F_standalone_allinone.bin" (
	echo.
	echo xxxxxxxxxxxxxxxxxx   ERROR   xxxxxxxxxxxxxxxxxxxxxxx
	echo             Firmware generation is ERROR!
	echo xxxxxxxxxxxxxxxxxx   ERROR   xxxxxxxxxxxxxxxxxxxxxxx
	echo.
	set ret=1
	cd %ql_root_dir%
	goto doneseg
)

xcopy /y "%ql_root_dir%\ql_build\standalone\standalone.map" "%ql_root_dir%\ql_out\debug\"
xcopy /y "%ql_root_dir%\ql_build\standalone\standalone.lst" "%ql_root_dir%\ql_out\debug\"
xcopy /y "%ql_root_dir%\ql_build\standalone\standalone.elf" "%ql_root_dir%\ql_out\debug\"
xcopy /y "%ql_root_dir%\ql_build\standalone\standalone.nm" "%ql_root_dir%\ql_out\debug\"
xcopy /y "%ql_root_dir%\ql_build\standalone\ECR6600F_standalone_allinone.bin" "%ql_root_dir%\ql_out\"
xcopy /y "%ql_root_dir%\ql_build\standalone\ECR6600F_standalone_cpu_0x7000.bin" "%ql_root_dir%\ql_out\"

call pythonw.exe %ql_tool_dir%\codesize.py --map %ql_root_dir%\ql_out\debug\standalone.map --outobj ql_out\debug\outobj.csv --outlib ql_out\debug\outlib.csv  --outsect ql_out\debug\outsect.csv

echo.
echo ********************        PASS         ***********************
echo               Firmware package is ready for you.
echo ********************        PASS         ***********************
echo.


:doneseg
echo=
echo %date%
echo START TIME:  %Start_at%
echo END TIME:    %time%
echo=
goto exit

:helpinfo
echo=
echo.Usage: %~n0 r/new/n Project Version [debug/release]
echo.       %~n0 clean
echo.       %~n0 h/-h
echo.Example:
echo.       %~n0 new %def_proj% your_firmware_version
echo=
echo.Supported projects include but are not limited to:
echo        FCM360W
echo=
echo.If you have any question, please contact Quectel.
echo=
echo=

:exit
exit /B %ret%

:add_path
(echo ";%PATH%;" | find /C /I ";%1;" > nul) || set "PATH=%1;%PATH%"
goto :eof

:cleanall
if exist "%1" (
	del /s/q/f %1 > nul
	rd /s/q %1 > nul
	echo clean "%1" done
)
if exist "%2" (
	del /s/q/f %2 > nul
	rd /s/q %2 > nul
	echo clean "%2" done
)
if exist "%3" (
	del /s/q/f %3 > nul
	rd /s/q %3 > nul
	echo clean "%3" done
)

echo=
goto :eof
