# 快速上手指南 / Quick Start Guide

## 🚀 快速上手指南

在几分钟内启动并运行VFT_SMF！本指南将带您完成第一个飞行仿真的运行。

### 📋 前置要求

在开始之前，请确保您具备：
- ✅ VFT_SMF已安装（参见[安装指南](Installation_Guide.md)）
- ✅ C++17兼容编译器
- ✅ 至少8GB可用内存

### ⚡ 快速开始（5分钟）

#### 步骤1：导航到场景目录
```bash
cd ScenarioExamples/B737_Taxi
```

#### 步骤2：编译仿真程序
```bash
.\build.bat
```

#### 步骤3：运行仿真
```bash
.\run_simulation_and_visualize.bat
```

#### 步骤4：查看结果
仿真将自动：
- 运行B737滑行场景
- 生成可视化数据
- 在控制台显示结果

### 📊 理解输出结果

#### 控制台输出
您应该看到类似以下的输出：
```
VFT_SMF仿真已启动
================================
时间: 0.00s - 飞机: B737-800
位置: 纬度=40.7128, 经度=-74.0060, 高度=0m
速度: 0.0 m/s, 航向: 0.0°
状态: 地面, 发动机: 关闭

时间: 0.01s - 事件: 发动机启动开始
...

仿真成功完成
总时间: 120.0s
数据保存至: output/simulation_data.csv
```

#### 生成的文件
- `output/simulation_data.csv` - 飞行数据
- `output/events_log.txt` - 事件时间线
- `output/performance_metrics.json` - 性能统计

### 🎮 交互功能

#### 实时监控
仿真期间，您可以：
- **暂停**: 按`P`键暂停/恢复
- **步进**: 按`S`键逐步执行
- **停止**: 按`Q`键退出

#### 数据可视化
仿真后，可视化结果：
```bash
# 查看飞行路径
.\tools\visualize_flight_state.exe

# 分析力
.\tools\visualize_aircraft_net_force.exe

# 监控事件
.\tools\visualize_triggered_events.exe
```

### 🔧 自定义您的第一个仿真

#### 修改配置
编辑`config/SimulationConfig.json`：
```json
{
  "simulation_time_step": 0.01,
  "max_simulation_time": 300.0,
  "output_directory": "output",
  "log_level": "INFO"
}
```

#### 更改飞行计划
编辑`input/FlightPlan.json`：
```json
{
  "aircraft_type": "B737-800",
  "departure": {
    "airport": "KJFK",
    "runway": "04L"
  },
  "arrival": {
    "airport": "KJFK", 
    "runway": "22R"
  },
  "waypoints": [
    {"lat": 40.7128, "lon": -74.0060, "alt": 0},
    {"lat": 40.7130, "lon": -74.0058, "alt": 0}
  ]
}
```

### 📈 下一步

#### 探索不同场景
```bash
# 平飞仿真
cd ../B737_LevelFlight
.\run_simulation_and_visualize.bat

# 创建自定义场景
cd ..
.\create_new_scenario.bat
```

#### 学习高级功能
- **[场景管理](Scenario_Management.md)** - 创建自定义场景
- **[可视化工具](Visualization_Tools.md)** - 高级数据分析
- **[配置参考](Configuration_Reference.md)** - 所有选项说明

### 🆘 故障排除

#### 常见问题

**"编译失败"**
```bash
# 检查编译器
g++ --version
# 如需要，重新安装Visual Studio C++工作负载
```

**"配置文件未找到"**
```bash
# 确保在正确的目录中
pwd
# 检查文件是否存在
dir config\SimulationConfig.json
```

**"内存分配失败"**
- 关闭其他应用程序
- 增加虚拟内存
- 使用较小的仿真时间

#### 获取帮助
- 查看[故障排除指南](../Troubleshooting.md)
- 搜索[GitHub Issues](https://github.com/GuilinWuGit/VFT_SMF/issues)
- 在[讨论区](https://github.com/GuilinWuGit/VFT_SMF/discussions)提问

### 🎯 成功检查清单

- ✅ 成功编译项目
- ✅ 运行第一个仿真
- ✅ 查看输出数据
- ✅ 理解基本配置
- ✅ 探索可视化工具

**恭喜！** 您现在已准备好探索VFT_SMF的全部功能。

---

*需要更多帮助？查看[示例集合](Examples_Collection.md)获取即用场景。*

---

## 🚀 Quick Start Guide (English)

Get up and running with VFT_SMF in minutes! This guide will walk you through running your first flight simulation.

## 🚀 Prerequisites

Before starting, ensure you have:
- ✅ VFT_SMF installed (see [Installation Guide](Installation_Guide.md))
- ✅ A C++17 compatible compiler
- ✅ At least 8 GB RAM available

## ⚡ Quick Start (5 Minutes)

### Step 1: Navigate to a Scenario
```bash
cd ScenarioExamples/B737_Taxi
```

### Step 2: Build the Simulation
```bash
.\build.bat
```

### Step 3: Run the Simulation
```bash
.\run_simulation_and_visualize.bat
```

### Step 4: View Results
The simulation will automatically:
- Run the B737 taxi scenario
- Generate visualization data
- Display results in the console

## 📊 Understanding the Output

### Console Output
You should see output similar to:
```
VFT_SMF Simulation Started
================================
Time: 0.00s - Aircraft: B737-800
Position: Lat=40.7128, Lon=-74.0060, Alt=0m
Speed: 0.0 m/s, Heading: 0.0°
Status: On Ground, Engine: Off

Time: 0.01s - Event: Engine Start Initiated
...

Simulation Completed Successfully
Total Time: 120.0s
Data saved to: output/simulation_data.csv
```

### Generated Files
- `output/simulation_data.csv` - Flight data
- `output/events_log.txt` - Event timeline
- `output/performance_metrics.json` - Performance statistics

## 🎮 Interactive Features

### Real-time Monitoring
During simulation, you can:
- **Pause**: Press `P` to pause/resume
- **Step**: Press `S` for step-by-step execution
- **Stop**: Press `Q` to quit

### Data Visualization
After simulation, visualize results:
```bash
# View flight path
.\tools\visualize_flight_state.exe

# Analyze forces
.\tools\visualize_aircraft_net_force.exe

# Monitor events
.\tools\visualize_triggered_events.exe
```

## 🔧 Customizing Your First Simulation

### Modify Configuration
Edit `config/SimulationConfig.json`:
```json
{
  "simulation_time_step": 0.01,
  "max_simulation_time": 300.0,
  "output_directory": "output",
  "log_level": "INFO"
}
```

### Change Flight Plan
Edit `input/FlightPlan.json`:
```json
{
  "aircraft_type": "B737-800",
  "departure": {
    "airport": "KJFK",
    "runway": "04L"
  },
  "arrival": {
    "airport": "KJFK", 
    "runway": "22R"
  },
  "waypoints": [
    {"lat": 40.7128, "lon": -74.0060, "alt": 0},
    {"lat": 40.7130, "lon": -74.0058, "alt": 0}
  ]
}
```

## 📈 Next Steps

### Explore Different Scenarios
```bash
# Level flight simulation
cd ../B737_LevelFlight
.\run_simulation_and_visualize.bat

# Create custom scenario
cd ..
.\create_new_scenario.bat
```

### Learn Advanced Features
- **[Scenario Management](Scenario_Management.md)** - Create custom scenarios
- **[Visualization Tools](Visualization_Tools.md)** - Advanced data analysis
- **[Configuration Reference](Configuration_Reference.md)** - All options explained

## 🆘 Troubleshooting

### Common Issues

#### "Build failed"
```bash
# Check compiler
g++ --version
# Reinstall Visual Studio C++ workload if needed
```

#### "Configuration file not found"
```bash
# Ensure you're in the correct directory
pwd
# Check file exists
dir config\SimulationConfig.json
```

#### "Memory allocation failed"
- Close other applications
- Increase virtual memory
- Use smaller simulation time

### Getting Help
- Check [Troubleshooting Guide](../Troubleshooting.md)
- Search [GitHub Issues](https://github.com/GuilinWuGit/VFT_SMF/issues)
- Ask in [Discussions](https://github.com/GuilinWuGit/VFT_SMF/discussions)

## 🎯 Success Checklist

- ✅ Successfully built the project
- ✅ Ran first simulation
- ✅ Viewed output data
- ✅ Understood basic configuration
- ✅ Explored visualization tools

**Congratulations!** You're now ready to explore the full capabilities of VFT_SMF.

---

*Need more help? Check out the [Examples Collection](Examples_Collection.md) for ready-to-use scenarios.*
