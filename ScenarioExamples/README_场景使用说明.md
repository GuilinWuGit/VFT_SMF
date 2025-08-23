# VFT_SMF V3 场景使用说明

## 概述

本目录包含VFT_SMF V3的各种仿真场景示例。每个场景都是独立的，包含自己的配置文件和输入数据。

## 目录结构

```
ScenarioExamples/
├── B737_Taxi/                    # B737滑行场景
│   ├── input/                    # 输入文件
│   │   └── FlightPlan.json       # 飞行计划
│   ├── config/                   # 配置文件
│   │   └── SimulationConfig.json # 仿真配置
│   ├── output/                   # 输出文件（运行时生成）
│   └── run_simulation_and_visualize.bat  # 运行脚本
├── B737_LevelFlight/             # B737平飞场景
│   ├── input/
│   ├── config/
│   ├── output/
│   └── run_simulation_and_visualize.bat
├── create_new_scenario.bat       # 快速创建新场景脚本
└── README_场景使用说明.md        # 本文件
```

## 使用方法

### 1. 运行现有场景

```bash
# 进入场景目录
cd ScenarioExamples\B737_Taxi

# 运行仿真和可视化
.\run_simulation_and_visualize.bat
```

### 2. 创建新场景

使用快速创建脚本自动创建新场景：

```bash
# 在ScenarioExamples目录下运行
.\create_new_scenario.bat MyNewScenario
```

脚本会自动完成以下操作：
1. 创建目录结构（input、config、output）
2. 复制并修改运行脚本
3. 复制配置文件模板（可选择B737_Taxi或B737_LevelFlight作为模板）
4. 创建场景说明文件

**使用示例：**
```bash
# 进入场景目录
cd D:\VFT_SMF_V3\ScenarioExamples

# 创建新场景
.\create_new_scenario.bat MyNewScenario

# 按提示选择配置文件模板来源
# 1. B737_Taxi (滑行场景)
# 2. B737_LevelFlight (平飞场景)

# 进入新创建的场景目录
cd MyNewScenario

# 修改配置文件（如需要）
# 运行场景
.\run_simulation_and_visualize.bat
```

## 配置文件说明

快速创建脚本会自动复制现有场景的配置文件作为模板。您可以根据需要修改以下配置项：

### FlightPlan.json 主要配置项
- `Aircraft_ID`: 飞机标识
- `Pilot_ID`: 飞行员标识  
- `ATC_ID`: ATC标识
- `Environment_Name`: 环境名称
- `events`: 事件列表（时间、类型、控制器等）

### SimulationConfig.json 主要配置项
- `max_simulation_time`: 最大仿真时间
- `time_step`: 时间步长
- `time_scale`: 时间缩放比例
- `enable_logging`: 是否启用日志

**注意：** 快速创建脚本会自动处理配置文件的复制，无需手动操作。

## 路径说明

### 相对路径设计

所有批处理文件都使用相对路径，确保场景可以复制到任何位置：

```
仿真程序: ..\..\src\G_SimulationManager\D_EventDrivenArchitecture\EventDrivenSimulation_NewArchitecture.exe
可视化工具: ..\..\tools\visualize_*.exe
输出文件: output\*.csv
```

### 路径关系

```
项目根目录/
├── src/                          # 源代码
│   └── G_SimulationManager/
│       └── D_EventDrivenArchitecture/
│           └── EventDrivenSimulation_NewArchitecture.exe
├── tools/                        # 可视化工具
│   ├── visualize_flight_state.exe
│   ├── visualize_aircraft_net_force.exe
│   └── ...
└── ScenarioExamples/             # 场景目录
    ├── B737_Taxi/               # 场景1
    ├── B737_LevelFlight/        # 场景2
    └── MyNewScenario/           # 新场景
```

## 输出文件

每个场景运行后会生成以下文件：

### 仿真数据文件 (CSV)
- `aircraft_flight_state.csv` - 飞机飞行状态
- `aircraft_net_force.csv` - 飞机净力
- `aircraft_system_state.csv` - 飞机系统状态
- `atc_command.csv` - ATC指令
- `environment_state.csv` - 环境状态
- `triggered_events.csv` - 触发事件
- `controller_execution_status.csv` - 控制器执行状态

### 可视化图片 (PNG)
- `aircraft_flight_state.png` - 飞机状态可视化
- `aircraft_net_force.png` - 飞机净力可视化
- `aircraft_system_state.png` - 飞机系统状态可视化
- `atc_command.png` - ATC指令可视化
- `environment_state.png` - 环境状态可视化
- `triggered_events.png` - 事件触发可视化
- `controller_execution_status.png` - 控制器执行状态可视化

### 日志文件
- `Brief.log` - 简要日志
- `Detail.log` - 详细日志

## 注意事项

1. **快速创建**: 推荐使用 `create_new_scenario.bat` 脚本创建新场景，避免手动操作错误
2. **路径要求**: 确保场景目录位于 `ScenarioExamples/` 下，以保持相对路径的正确性
3. **可执行文件**: 首次使用前需要运行项目根目录的 `build_simulation.bat` 编译仿真程序
4. **可视化工具**: 确保 `tools/` 目录下有所有必要的可视化工具
5. **MATLAB**: 可视化工具需要MATLAB支持
6. **权限**: 确保有写入 `output/` 目录的权限

## 故障排除

### 常见问题

1. **"找不到可执行文件"**
   - 检查是否已运行 `build_simulation.bat`
   - 确认可执行文件路径正确

2. **"找不到可视化工具"**
   - 检查 `tools/` 目录是否存在
   - 确认所有可视化工具已编译

3. **"MATLAB错误"**
   - 确认MATLAB已安装并添加到PATH
   - 检查MATLAB许可证是否有效

4. **"权限错误"**
   - 以管理员身份运行
   - 检查目录写入权限

### 调试建议

1. 先单独运行仿真程序，确认基本功能正常
2. 再逐个运行可视化工具，定位问题
3. 检查日志文件获取详细错误信息
4. 确认所有依赖文件路径正确
