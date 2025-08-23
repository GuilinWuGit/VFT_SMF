@echo off
chcp 65001 >nul
echo ========================================
echo 编译事件触发数据可视化工具
echo ========================================
echo.

echo 正在编译 visualize_triggered_events.cpp...
g++ -std=c++17 -O2 -o visualize_triggered_events.exe visualize_triggered_events.cpp

if %errorlevel% equ 0 (
    echo.
    echo 编译成功！
    echo 生成的可执行文件: visualize_triggered_events.exe
    echo.
    echo 使用方法:
    echo visualize_triggered_events.exe "CSV文件路径" [输出目录]
    echo.
    echo 示例:
    echo visualize_triggered_events.exe "..\ScenarioExamples\B737_Taxi\output\triggered_events.csv"
    echo.
) else (
    echo.
    echo 编译失败！
    echo 请检查错误信息并修复代码。
    echo.
)

pause
