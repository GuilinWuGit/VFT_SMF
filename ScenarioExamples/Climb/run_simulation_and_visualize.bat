@echo off
chcp 65001 >nul
echo ========================================
echo VFT_SMF V3 - Climb 仿真与可视化
echo ========================================
echo.
echo.
echo [1/8] 开始执行仿真程序...
echo 正在运行 EventDrivenSimulation_NewArchitecture.exe...
..\..\src\G_SimulationManager\D_EventDrivenArchitecture\EventDrivenSimulation_NewArchitecture.exe
if %errorlevel% neq 0 (
    echo 错误: 仿真程序执行失败！
    pause
    exit /b 1
)
echo 仿真程序执行完成！
echo.
echo [2/8] 开始执行飞机状态可视化程序...
echo 正在运行飞机状态可视化工具...
..\..\tools\visualize_flight_state.exe "output\aircraft_flight_state.csv"
if %errorlevel% neq 0 (
    echo 错误: 飞机状态可视化程序执行失败！
    pause
    exit /b 1
)
echo 飞机状态可视化程序执行完成！
echo.
echo [3/8] 开始执行飞机净力可视化程序...
echo 正在运行飞机净力可视化工具...
..\..\tools\visualize_aircraft_net_force.exe "output\aircraft_net_force.csv"
if %errorlevel% neq 0 (
    echo 错误: 飞机净力可视化程序执行失败！
    pause
    exit /b 1
)
echo 飞机净力可视化程序执行完成！
echo.
echo [4/8] 开始执行事件触发可视化程序...
echo 正在运行事件触发可视化工具...
..\..\tools\visualize_triggered_events.exe "output\triggered_events.csv"
if %errorlevel% neq 0 (
    echo 错误: 事件触发可视化程序执行失败！
    pause
    exit /b 1
)
echo 事件触发可视化程序执行完成！
echo.
echo [5/8] 开始执行控制器执行状态可视化程序...
echo 正在运行控制器执行状态可视化工具...
..\..\tools\visualize_controller_execution.exe "output\controller_execution_status.csv"
if %errorlevel% neq 0 (
    echo 错误: 控制器执行状态可视化程序执行失败！
    pause
    exit /b 1
)
echo 控制器执行状态可视化程序执行完成！
echo.
echo [6/8] 开始执行飞机系统状态可视化程序...
echo 正在运行飞机系统状态可视化工具...
..\..\tools\visualize_aircraft_system.exe "output\aircraft_system_state.csv"
if %errorlevel% neq 0 (
    echo 错误: 飞机系统状态可视化程序执行失败！
    pause
    exit /b 1
)
echo 飞机系统状态可视化程序执行完成！
echo.
echo [7/8] 开始执行ATC指令可视化程序...
echo 正在运行ATC指令可视化工具...
..\..\tools\visualize_atc_command.exe "output\atc_command.csv"
if %errorlevel% neq 0 (
    echo 错误: ATC指令可视化程序执行失败！
    pause
    exit /b 1
)
echo ATC指令可视化程序执行完成！
echo.
echo [8/8] 开始执行环境状态可视化程序...
echo 正在运行环境状态可视化工具...
..\..\tools\visualize_environment_state.exe "output\environment_state.csv"
if %errorlevel% neq 0 (
    echo 错误: 环境状态可视化程序执行失败！
    pause
    exit /b 1
)
echo 环境状态可视化程序执行完成！
echo.
echo ========================================
echo 所有程序执行完成！
echo.
echo 输出文件位置:
echo - 仿真数据: output\*.csv
echo - 飞机状态可视化: output\aircraft_flight_state.png
echo - 飞机净力可视化: output\aircraft_net_force.png
echo - 事件触发可视化: output\triggered_events.png
echo - 控制器执行状态可视化: output\controller_execution_status.png
echo - 飞机系统状态可视化: output\aircraft_system_state.png
echo - ATC指令可视化: output\atc_command.png
echo - 环境状态可视化: output\environment_state.png
echo - 日志文件: output\Brief.log, output\Detail.log
echo ========================================
echo.
