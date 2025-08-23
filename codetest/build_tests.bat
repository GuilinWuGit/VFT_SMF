@echo off
chcp 65001 >nul
echo ========================================
echo VFT_SMF V3 - 测试编译脚本
echo ========================================
echo.

:: 检查Google Test是否已安装
echo [1/5] 检查Google Test环境...
where gtest-config >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo 警告: 未检测到Google Test，将使用手动编译方式
    echo 建议安装Google Test: https://github.com/google/googletest
    echo.
)

:: 创建测试输出目录
echo [2/5] 创建测试输出目录...
if not exist "test_output" mkdir test_output
echo 测试输出目录创建完成！
echo.

:: 编译Google Test (如果未安装)
echo [3/5] 编译Google Test...
if not exist "googletest" (
    echo 下载Google Test...
    git clone https://github.com/google/googletest.git
    cd googletest
    mkdir build
    cd build
    cmake ..
    cmake --build . --config Release
    cd ..\..
) else (
    echo Google Test已存在，跳过下载
)
echo Google Test编译完成！
echo.

:: 编译项目测试
echo [4/5] 编译项目测试...
g++ -std=c++17 ^
    -Isrc -Isrc/I_ThirdPartyTools ^
    -Igoogletest/googletest/include ^
    -Igoogletest/googlemock/include ^
    -Lgoogletest/build/lib -lgtest -lgtest_main -lgmock ^
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

:: 创建测试运行脚本
echo [5/5] 创建测试运行脚本...
echo @echo off > test_output/run_tests.bat
echo chcp 65001 ^>nul >> test_output/run_tests.bat
echo echo ======================================== >> test_output/run_tests.bat
echo echo VFT_SMF V3 - 运行测试 >> test_output/run_tests.bat
echo echo ======================================== >> test_output/run_tests.bat
echo echo. >> test_output/run_tests.bat
echo. >> test_output/run_tests.bat
echo :: 运行所有测试 >> test_output/run_tests.bat
echo echo [1/4] 运行单元测试... >> test_output/run_tests.bat
echo run_tests.exe --gtest_filter="UnitTest*" >> test_output/run_tests.bat
echo echo. >> test_output/run_tests.bat
echo. >> test_output/run_tests.bat
echo echo [2/4] 运行集成测试... >> test_output/run_tests.bat
echo run_tests.exe --gtest_filter="IntegrationTest*" >> test_output/run_tests.bat
echo echo. >> test_output/run_tests.bat
echo. >> test_output/run_tests.bat
echo echo [3/4] 运行性能测试... >> test_output/run_tests.bat
echo run_tests.exe --gtest_filter="PerformanceTest*" >> test_output/run_tests.bat
echo echo. >> test_output/run_tests.bat
echo. >> test_output/run_tests.bat
echo echo [4/4] 生成测试报告... >> test_output/run_tests.bat
echo run_tests.exe --gtest_output=xml:test_results.xml >> test_output/run_tests.bat
echo echo. >> test_output/run_tests.bat
echo echo 测试完成！查看 test_results.xml 获取详细结果。 >> test_output/run_tests.bat
echo pause >> test_output/run_tests.bat

echo 测试运行脚本创建完成！
echo.

echo ========================================
echo 测试编译完成！
echo ========================================
echo.
echo 下一步操作:
echo 1. 进入测试输出目录: cd test_output
echo 2. 运行测试: .\run_tests.bat
echo.
echo 测试文件位置:
echo - 可执行文件: test_output/run_tests.exe
echo - 运行脚本: test_output/run_tests.bat
echo - 测试结果: test_output/test_results.xml
echo ========================================
echo.
pause
