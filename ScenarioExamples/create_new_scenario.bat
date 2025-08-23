@echo off
setlocal enabledelayedexpansion
chcp 65001 >nul
echo ========================================
echo VFT_SMF V3 - 快速创建新场景
echo ========================================
echo.

if "%1"=="" (
    echo 使用方法: create_new_scenario.bat [场景名称]
    echo 示例: create_new_scenario.bat MyNewScenario
    echo.
    echo 或者直接运行脚本，然后输入场景名称:
    set /p SCENARIO_NAME="请输入场景名称: "
    if "!SCENARIO_NAME!"=="" (
        echo 错误: 场景名称不能为空！
        pause
        exit /b 1
    )
) else (
    set SCENARIO_NAME=%1
)
echo 正在创建场景: %SCENARIO_NAME%
echo.

:: 检查场景名称是否已存在
if exist "%SCENARIO_NAME%" (
    echo 错误: 场景目录 "%SCENARIO_NAME%" 已存在！
    echo 请使用不同的场景名称。
    pause
    exit /b 1
)

:: 创建场景目录结构
echo [1/6] 创建目录结构...
mkdir "%SCENARIO_NAME%"
mkdir "%SCENARIO_NAME%\input"
mkdir "%SCENARIO_NAME%\config"
mkdir "%SCENARIO_NAME%\output"
echo 目录结构创建完成！
echo.

:: 复制运行脚本
echo [2/6] 复制运行脚本...
copy "run_simulation_template.bat" "%SCENARIO_NAME%\run_simulation_and_visualize.bat"
echo 运行脚本复制完成！
echo.

:: 修改脚本标题
echo [3/6] 修改脚本标题...
:: 使用简单的字符串替换方法，避免PowerShell编码问题
:: 先复制模板文件
copy "run_simulation_template.bat" "%SCENARIO_NAME%\temp_template.bat"
:: 创建新的脚本文件，替换标题行
(
echo @echo off
echo chcp 65001 ^>nul
echo echo ========================================
echo echo VFT_SMF V3 - %SCENARIO_NAME% 仿真与可视化
echo echo ========================================
echo echo.
:: 跳过模板文件的前5行（标题部分），复制其余内容
for /f "skip=5 delims=" %%i in ('type "%SCENARIO_NAME%\temp_template.bat"') do echo %%i
) > "%SCENARIO_NAME%\run_simulation_and_visualize.bat"
:: 删除临时文件
del "%SCENARIO_NAME%\temp_template.bat"
echo 脚本标题修改完成！
echo.

:: 复制配置文件模板
echo [4/6] 复制配置文件模板...
echo 请选择配置文件模板来源:
echo 1. B737_Taxi (滑行场景)
echo 2. B737_LevelFlight (平飞场景)
echo.
:choice_loop
set /p choice="请输入选择 (1-2): "

if "%choice%"=="1" (
    echo 复制 B737_Taxi 配置文件...
    copy "B737_Taxi\input\FlightPlan.json" "%SCENARIO_NAME%\input\FlightPlan.json"
    copy "B737_Taxi\config\SimulationConfig.json" "%SCENARIO_NAME%\config\SimulationConfig.json"
    echo B737_Taxi 配置文件复制完成！
) else if "%choice%"=="2" (
    echo 复制 B737_LevelFlight 配置文件...
    copy "B737_LevelFlight\input\FlightPlan.json" "%SCENARIO_NAME%\input\FlightPlan.json"
    copy "B737_LevelFlight\config\SimulationConfig.json" "%SCENARIO_NAME%\config\SimulationConfig.json"
    echo B737_LevelFlight 配置文件复制完成！
) else (
    echo 错误: 请输入有效的选择 (1 或 2)
    goto choice_loop
)
echo.

:: 创建README文件
echo [5/6] 创建场景说明文件...
echo # %SCENARIO_NAME% 场景说明 > "%SCENARIO_NAME%\README.md"
echo. >> "%SCENARIO_NAME%\README.md"
echo ## 场景描述 >> "%SCENARIO_NAME%\README.md"
echo 请在此描述您的仿真场景。 >> "%SCENARIO_NAME%\README.md"
echo. >> "%SCENARIO_NAME%\README.md"
echo ## 配置文件 >> "%SCENARIO_NAME%\README.md"
echo - input/FlightPlan.json: 飞行计划配置 >> "%SCENARIO_NAME%\README.md"
echo - config/SimulationConfig.json: 仿真参数配置 >> "%SCENARIO_NAME%\README.md"
echo. >> "%SCENARIO_NAME%\README.md"
echo ## 运行方法 >> "%SCENARIO_NAME%\README.md"
echo ```bash >> "%SCENARIO_NAME%\README.md"
echo cd %SCENARIO_NAME% >> "%SCENARIO_NAME%\README.md"
echo .\run_simulation_and_visualize.bat >> "%SCENARIO_NAME%\README.md"
echo ``` >> "%SCENARIO_NAME%\README.md"
echo 场景说明文件创建完成！
echo.

:: 完成提示
echo [6/6] 场景创建完成！
echo.
echo ========================================
echo 场景 "%SCENARIO_NAME%" 创建成功！
echo ========================================
echo.
echo 场景目录: %SCENARIO_NAME%\
echo 包含文件:
echo - run_simulation_and_visualize.bat (运行脚本)
echo - input\FlightPlan.json (飞行计划)
echo - config\SimulationConfig.json (仿真配置)
echo - output\ (输出目录)
echo - README.md (场景说明)
echo.
echo 下一步操作:
echo 1. 进入场景目录: cd %SCENARIO_NAME%
echo 2. 修改配置文件 (如需要)
echo 3. 运行场景: .\run_simulation_and_visualize.bat
echo.
echo ========================================
pause
