@echo off
echo ========================================
echo 编译飞机状态数据可视化工具
echo ========================================
echo.

echo 编译 visualize_aircraft_state.exe...
g++ -std=c++17 -o visualize_aircraft_state.exe visualize_aircraft_state.cpp

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ========================================
    echo 编译成功!
    echo ========================================
    echo 可执行文件: visualize_aircraft_state.exe
    echo.
    echo 使用方法:
    echo   visualize_aircraft_state.exe ^<CSV文件路径^> [输出目录]
    echo.
    echo 示例:
    echo   visualize_aircraft_state.exe ../ScenarioExamples/B737_Taxi/output/aircraft_flight_state.csv
    echo.
) else (
    echo.
    echo ========================================
    echo 编译失败! 请检查错误信息。
    echo ========================================
)

echo.
pause
