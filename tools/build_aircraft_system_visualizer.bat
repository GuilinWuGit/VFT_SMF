@echo off
chcp 65001 >nul

echo ========================================
echo VFT_SMF V3 - Aircraft System State Visualizer Build Script
echo ========================================

set SCRIPT_DIR=%~dp0
set PROJECT_ROOT=%SCRIPT_DIR%..

set COMPILER=g++
set CXX_STANDARD=-std=c++17
set WARNINGS=-Wall -Wextra
set INCLUDES=-I"%PROJECT_ROOT%\src" -I"%PROJECT_ROOT%\src\E_GlobalSharedDataSpace" -I"%PROJECT_ROOT%\src\G_SimulationManager\LogAndData"
set LIBS=

set SOURCE_FILE=visualize_aircraft_system.cpp
set OUTPUT_EXE=visualize_aircraft_system.exe

echo Compiling %SOURCE_FILE%...
%COMPILER% %CXX_STANDARD% %WARNINGS% %INCLUDES% %LIBS% %SOURCE_FILE% -o %OUTPUT_EXE%

if %errorlevel% neq 0 (
    echo 错误: 编译失败！
    exit /b 1
)

echo ========================================
echo Build Successful!
echo ========================================
echo Executable: %OUTPUT_EXE%
echo.
pause
