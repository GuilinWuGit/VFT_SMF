# VFT_SMF   虚拟试飞仿真与建模框架

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C++](https://img.shields.io/badge/C++-17-blue.svg)](https://isocpp.org/)
[![Platform](https://img.shields.io/badge/Platform-Windows-lightgrey.svg)](https://www.microsoft.com/windows)

## 🚀 概述

VFT_SMF 是一个专为虚拟试飞实验设计的综合性开源飞行仿真平台。它采用先进的数字孪生架构，结合事件驱动多线程仿真，支持大规模分布式仿真系统，实现全要素场景下的虚拟试飞实验。

## ✨ 核心特性

- **🔄 数字孪生架构**: 完整的数字孪生实现，包含数据孪生、模型孪生和服务孪生三层架构
- **⚡ 事件驱动多线程仿真**: 高性能事件驱动架构，具备严格的同步控制
- **👥 五代理建模系统**: 全面的代理模型，包括飞行员、飞机、环境、空管和飞行动力学
- **🎯 集中式事件管理**: 集中式事件监测和控制器管理系统
- **📊 实时数据记录**: 全面的数据记录和可视化能力
- **🔧 模块化设计**: 高度模块化架构，支持轻松扩展和定制

## 🏗️ 架构

### 核心组件

```
VFT_SMF/
├── src/                           # 源代码
│   ├── A_PilotAgentModel/         # 飞行员代理模型
│   ├── B_AircraftAgentModel/      # 飞机数字孪生模型
│   ├── C_EnvironmentAgentModel/   # 环境仿真
│   ├── D_ATCAgentModel/          # 空管代理模型
│   ├── E_FlightDynamics/         # 飞行动力学仿真
│   ├── F_ScenarioModelling/      # 飞行计划解析和场景管理
│   └── G_SimulationManager/      # 核心仿真管理
├── ScenarioExamples/             # 预配置仿真场景
├── tools/                        # 数据可视化工具
└── codetest/                     # 单元和集成测试
```

### 数字孪生架构

- **数据孪生**: 物理和心理数据表示
- **模型孪生**: 决策制定和控制算法
- **服务孪生**: 状态管理和服务接口

## 🚁 支持的飞机

- **波音737**: 完整的B737数字孪生实现
- **可扩展**: 易于添加新的飞机类型

## 📋 系统要求

- **C++17** 兼容编译器（GCC、Clang或MSVC）
- **Windows 10/11**（主要平台）
- **Git** 版本控制
- **MATLAB**（可选，用于高级可视化）

## 🛠️ 安装

### 快速开始

1. **克隆仓库**:
   ```bash
   git clone https://github.com/GuilinWuGit/VFT_SMF.git
   cd VFT_SMF
   ```

2. **进入场景目录**:
   ```bash
   cd ScenarioExamples/B737_Taxi
   ```

3. **编译项目**:
   ```bash
   .\build.bat
   ```

4. **运行仿真**:
   ```bash
   .\run_simulation_and_visualize.bat
   ```

### 详细安装

查看[安装指南](docs/installation.md)了解详细设置说明。

## 🎮 使用

### 运行仿真

1. **选择场景** from `ScenarioExamples/`
2. **配置参数** in `config/SimulationConfig.json`
3. **定义飞行计划** in `input/FlightPlan.json`
4. **运行仿真** using the provided batch scripts

### 示例场景

- **B737滑行**: 带地面控制的滑行仿真
- **B737平飞**: 平飞仿真
- **爬升**: 爬升性能分析

### 数据可视化

平台包含全面的可视化工具：

- **飞行状态可视化**: 实时飞机状态监控
- **净力分析**: 力和力矩分析
- **事件监控**: 事件触发可视化
- **控制器状态**: 控制器执行状态跟踪

## 🧪 测试

运行测试套件：

```bash
cd codetest
.\build_tests.bat
```

## 📊 性能

- **实时仿真**: 亚毫秒级同步
- **多线程架构**: 最优CPU利用率
- **内存高效**: 优化的数据结构和缓冲
- **可扩展**: 支持分布式仿真系统

## 🤝 贡献

我们欢迎贡献！请查看我们的[贡献指南](CONTRIBUTING.md)了解详情。

### 开发设置

1. Fork仓库
2. 创建功能分支
3. 进行修改
4. 如适用则添加测试
5. 提交拉取请求

## 📄 许可证

本项目采用MIT许可证 - 详情请查看[LICENSE](LICENSE)文件。

## 👥 作者

- **VFT_SMF开发团队** - *初始工作*

## 🙏 致谢

- 学术研究社区
- 开源贡献者
- 飞行仿真爱好者

## 📚 文档

- [架构指南](docs/architecture.md)
- [API参考](docs/api.md)
- [故障排除](docs/troubleshooting.md)

## 🔗 链接

- **仓库**: https://github.com/GuilinWuGit/VFT_SMF
- **问题**: https://github.com/GuilinWuGit/VFT_SMF/issues
- **讨论**: https://github.com/GuilinWuGit/VFT_SMF/discussions

## 📈 项目状态

- **版本**: 1.0.0
- **状态**: 积极开发中
- **最后更新**: 2024年1月

---

---

# VFT_SMF  - Virtual Flight Test Simulation and Modeling Framework

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C++](https://img.shields.io/badge/C++-17-blue.svg)](https://isocpp.org/)
[![Platform](https://img.shields.io/badge/Platform-Windows-lightgrey.svg)](https://www.microsoft.com/windows)

## 🚀 Overview

VFT_SMF is a comprehensive open-source flight simulation platform designed for virtual flight testing experiments. It features a sophisticated digital twin architecture with event-driven multi-threaded simulation, supporting large-scale distributed simulation systems for comprehensive virtual flight testing scenarios.

## ✨ Key Features

- **🔄 Digital Twin Architecture**: Complete digital twin implementation with DataTwin, ModelTwin, and ServiceTwin layers
- **⚡ Event-Driven Multi-threaded Simulation**: High-performance event-driven architecture with strict synchronization control
- **👥 5-Agent Modeling System**: Comprehensive agent models including Pilot, Aircraft, Environment, ATC, and Flight Dynamics
- **🎯 Centralized Event Management**: Centralized event monitoring and controller management systems
- **📊 Real-time Data Recording**: Comprehensive data recording and visualization capabilities
- **🔧 Modular Design**: Highly modular architecture supporting easy extension and customization

## 🏗️ Architecture

### Core Components

```
VFT_SMF/
├── src/                           # Source code
│   ├── A_PilotAgentModel/         # Pilot agent models
│   ├── B_AircraftAgentModel/      # Aircraft digital twin models
│   ├── C_EnvironmentAgentModel/   # Environment simulation
│   ├── D_ATCAgentModel/          # Air Traffic Control models
│   ├── E_FlightDynamics/         # Flight dynamics simulation
│   ├── F_ScenarioModelling/      # Flight plan parsing and scenario management
│   └── G_SimulationManager/      # Core simulation management
├── ScenarioExamples/             # Pre-configured simulation scenarios
├── tools/                        # Data visualization tools
└── codetest/                     # Unit and integration tests
```

### Digital Twin Architecture

- **DataTwin**: Physical and mental data representation
- **ModelTwin**: Decision-making and control algorithms
- **ServiceTwin**: State management and service interfaces

## 🚁 Supported Aircraft

- **Boeing 737**: Complete B737 digital twin implementation
- **Extensible**: Easy to add new aircraft types

## 📋 Prerequisites

- **C++17** compatible compiler (GCC, Clang, or MSVC)
- **Windows 10/11** (primary platform)
- **Git** for version control
- **MATLAB** (optional, for advanced visualization)

## 🛠️ Installation

### Quick Start

1. **Clone the repository**:
   ```bash
   git clone https://github.com/GuilinWuGit/VFT_SMF.git
   cd VFT_SMF
   ```

2. **Navigate to a scenario**:
   ```bash
   cd ScenarioExamples/B737_Taxi
   ```

3. **Compile the project**:
   ```bash
   .\build.bat
   ```

4. **Run the simulation**:
   ```bash
   .\run_simulation_and_visualize.bat
   ```

### Detailed Installation

See [Installation Guide](docs/installation.md) for detailed setup instructions.

## 🎮 Usage

### Running Simulations

1. **Choose a scenario** from `ScenarioExamples/`
2. **Configure parameters** in `config/SimulationConfig.json`
3. **Define flight plan** in `input/FlightPlan.json`
4. **Run simulation** using the provided batch scripts

### Example Scenarios

- **B737_Taxi**: Taxi simulation with ground control
- **B737_LevelFlight**: Level flight simulation
- **Climb**: Climb performance analysis

### Data Visualization

The platform includes comprehensive visualization tools:

- **Flight State Visualization**: Real-time aircraft state monitoring
- **Net Force Analysis**: Force and moment analysis
- **Event Monitoring**: Event trigger visualization
- **Controller Status**: Controller execution status tracking

## 🧪 Testing

Run the test suite:

```bash
cd codetest
.\build_tests.bat
```

## 📊 Performance

- **Real-time Simulation**: Sub-millisecond synchronization
- **Multi-threaded Architecture**: Optimal CPU utilization
- **Memory Efficient**: Optimized data structures and buffering
- **Scalable**: Support for distributed simulation systems

## 🤝 Contributing

We welcome contributions! Please see our [Contributing Guide](CONTRIBUTING.md) for details.

### Development Setup

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Add tests if applicable
5. Submit a pull request

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 👥 Authors

- **VFT_SMF Development Team** - *Initial work*

## 🙏 Acknowledgments

- Academic research community
- Open source contributors
- Flight simulation enthusiasts

## 📚 Documentation

- [Architecture Guide](docs/architecture.md)
- [API Reference](docs/api.md)
- [Troubleshooting](docs/troubleshooting.md)

## 🔗 Links

- **Repository**: https://github.com/GuilinWuGit/VFT_SMF
- **Issues**: https://github.com/GuilinWuGit/VFT_SMF/issues
- **Discussions**: https://github.com/GuilinWuGit/VFT_SMF/discussions

## 📈 Project Status

- **Version**: 1.0.0
- **Status**: Active Development
- **Last Updated**: January 2024

---

⭐ **If you find this project useful, please give it a star!**
