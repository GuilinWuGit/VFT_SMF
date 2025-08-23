@echo off
chcp 65001 >nul
echo ========================================
echo 编译飞机净力数据可视化工具
echo ========================================
echo.

echo 正在编译 visualize_aircraft_net_force.cpp...
g++ -std=c++17 -O2 -o visualize_aircraft_net_force.exe visualize_aircraft_net_force.cpp

if %errorlevel% equ 0 (
    echo.
    echo 编译成功！
    echo 生成的可执行文件: visualize_aircraft_net_force.exe
    echo.
    echo 使用方法:
    echo visualize_aircraft_net_force.exe "CSV文件路径" [输出目录]
    echo.
    echo 示例:
    echo visualize_aircraft_net_force.exe "..\ScenarioExamples\B737_Taxi\output\aircraft_net_force.csv"
    echo.
) else (
    echo.
    echo 编译失败！
    echo 请检查错误信息并修复代码。
    echo.
)

pause
