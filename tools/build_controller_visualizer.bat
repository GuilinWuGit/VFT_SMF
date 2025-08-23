@echo off
chcp 65001 >nul
echo ========================================
echo 控制器执行状态可视化工具编译脚本
echo ========================================

echo 正在编译 visualize_controller_execution.cpp...

g++ -std=c++17 -O2 -Wall -Wextra ^
    -I. ^
    visualize_controller_execution.cpp ^
    -o visualize_controller_execution.exe

if %errorlevel% equ 0 (
    echo.
    echo ========================================
    echo 编译成功！
    echo ========================================
    echo 可执行文件: visualize_controller_execution.exe
    echo.
    echo 使用方法:
    echo   visualize_controller_execution.exe ^<controller_execution_status.csv文件路径^>
    echo.
    echo 示例:
    echo   visualize_controller_execution.exe "../ScenarioExamples/B737_Taxi/output/controller_execution_status.csv"
    echo.
    echo ========================================
) else (
    echo.
    echo ========================================
    echo 编译失败！
    echo ========================================
    echo 请检查错误信息并修复代码问题
    echo.
)

pause
