@echo off
chcp 65001 >nul
echo ========================================
echo VFT_SMF V3 - 简化测试编译脚本
echo ========================================
echo.

echo 注意: 此脚本需要手动安装Google Test
echo 安装步骤:
echo 1. 下载: https://github.com/google/googletest
echo 2. 编译: cmake .. && cmake --build . --config Release
echo 3. 将编译后的库文件复制到 lib/ 目录
echo.

:: 创建测试输出目录
echo [1/3] 创建测试输出目录...
if not exist "test_output" mkdir test_output
echo 测试输出目录创建完成！
echo.

:: 检查Google Test库文件
echo [2/3] 检查Google Test库文件...
if not exist "lib\gtest.lib" (
    echo 错误: 未找到Google Test库文件
    echo 请先安装Google Test并将库文件放在 lib/ 目录下
    echo.
    echo 需要的文件:
    echo - lib\gtest.lib
    echo - lib\gtest_main.lib
    echo - include\gtest\gtest.h
    echo - include\gtest\gtest_main.h
    pause
    exit /b 1
) else (
    echo Google Test库文件检查通过！
)
echo.

:: 编译项目测试
echo [3/3] 编译项目测试...
g++ -std=c++17 ^
    -Isrc -Isrc/I_ThirdPartyTools ^
    -Iinclude ^
    -Llib -lgtest -lgtest_main ^
    -lpthread ^
    -o test_output/run_tests.exe ^
    tests/unit/aircraft/test_b737_digital_twin.cpp ^
    tests/unit/aircraft/test_control_priority_manager.cpp ^
    tests/unit/pilot/test_pilot_manual_control.cpp ^
    tests/unit/simulation/test_simulation_clock.cpp ^
    tests/integration/test_simulation_workflow.cpp ^
    tests/performance/test_simulation_performance.cpp ^
    src/B_AircraftAgentModel/B737/B737DigitalTwin.cpp ^
    src/B_AircraftAgentModel/B737/ServiceTwin/ControlPriorityManager.cpp ^
    src/A_PilotAgentModel/Pilot_001/ServiceTwin/PilotManualControlHandler.cpp ^
    src/G_SimulationManager/A_TimeSYNC/Simulation_Clock.cpp ^
    src/E_GlobalSharedDataSpace/GlobalSharedDataSpace.cpp ^
    src/E_GlobalSharedDataSpace/GlobalSharedDataStruct.cpp ^
    src/G_SimulationManager/LogAndData/DataRecorder.cpp ^
    src/G_SimulationManager/C_ConfigManager/ConfigManager.cpp

if %ERRORLEVEL% EQU 0 (
    echo 项目测试编译成功！
    echo.
    echo 测试可执行文件位置: test_output/run_tests.exe
) else (
    echo 错误: 项目测试编译失败！
    echo 请检查错误信息并修复问题。
    pause
    exit /b 1
)

echo.
echo ========================================
echo 测试编译完成！
echo ========================================
echo.
echo 下一步操作:
echo 1. 进入测试输出目录: cd test_output
echo 2. 运行测试: .\run_tests.exe
echo.
echo 测试文件位置:
echo - 可执行文件: test_output/run_tests.exe
echo ========================================
echo.
pause
