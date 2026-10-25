@echo off
setlocal EnableExtensions EnableDelayedExpansion

set "BUILD_TYPE=Debug"

:parse_args
if "%~1"=="" goto :configure
set "ARG=%~1"
if /I "%ARG%"=="-Debug" (
	set "BUILD_TYPE=Debug"
) else if /I "%ARG%"=="-Release" (
	set "BUILD_TYPE=Release"
) else if /I "%ARG%"=="-ReleaseDebInfo" (
	set "BUILD_TYPE=ReleaseDebInfo"
) else if /I "%ARG%"=="-RelWithDebInfo" (
	set "BUILD_TYPE=RelWithDebInfo"
) else if /I "%ARG%"=="-MinSizeRel" (
	set "BUILD_TYPE=MinSizeRel"
) else if /I "%ARG%"=="-h" (
	goto :usage
) else if /I "%ARG%"=="--help" (
	goto :usage
) else (
	echo error: unknown argument: %ARG%
	goto :usage_error
)
shift
goto :parse_args

:usage
echo usage: %~nx0 [-Debug^|-Release^|-ReleaseDebInfo^|-RelWithDebInfo^|-MinSizeRel]
echo        no argument builds Debug
exit /b 0

:usage_error
call :usage
exit /b 1

:configure
set "ROOT_DIR=%~dp0"
if "%ROOT_DIR:~-1%"=="\" set "ROOT_DIR=%ROOT_DIR:~0,-1%"
set "BUILD_DIR=%ROOT_DIR%\build-win"
set "BIN_DIR=%ROOT_DIR%\bin-win"
set "BIN_DIR_CMAKE=%BIN_DIR:\=/%"

where gcc >nul 2>nul
if errorlevel 1 (
	echo error: gcc not found in PATH.
	echo        Run this script from an MSYS2 UCRT64 shell, or add its bin directory to PATH.
	exit /b 1
)
where ninja >nul 2>nul
if errorlevel 1 (
	echo error: ninja not found in PATH.
	exit /b 1
)

if not exist "%BIN_DIR%" mkdir "%BIN_DIR%"

echo === configure: %BUILD_TYPE% ===
cmake -S "%ROOT_DIR%" -B "%BUILD_DIR%" -G Ninja -DCMAKE_BUILD_TYPE=%BUILD_TYPE% -DLMNS_BIN_DIR="%BIN_DIR_CMAKE%" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
if errorlevel 1 (
	echo error: cmake configure failed.
	exit /b 1
)

echo === build ===
cmake --build "%BUILD_DIR%"
if errorlevel 1 (
	echo error: build failed.
	exit /b 1
)

echo.
echo build type : %BUILD_TYPE%
echo build dir  : %BUILD_DIR%
echo test bins  : %BIN_DIR%\tests\%BUILD_TYPE%

endlocal
