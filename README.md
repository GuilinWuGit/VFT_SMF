# VFT_SMF V3 - Virtual Flight Test Simulation and Modeling Framework
# VFT_SMF V3 - 虚拟试飞仿真与建模框架

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C++](https://img.shields.io/badge/C++-17-blue.svg)](https://isocpp.org/)
[![Platform](https://img.shields.io/badge/Platform-Windows-lightgrey.svg)](https://www.microsoft.com/windows)

## 🚀 Overview | 概述

VFT_SMF V3 is a comprehensive open-source flight simulation platform designed for virtual flight testing experiments. It features a sophisticated digital twin architecture with event-driven multi-threaded simulation, supporting large-scale distributed simulation systems for comprehensive virtual flight testing scenarios.

VFT_SMF V3 是一个专为虚拟试飞实验设计的综合性开源飞行仿真平台。它采用先进的数字孪生架构，结合事件驱动多线程仿真，支持大规模分布式仿真系统，实现全要素场景下的虚拟试飞实验。

## ✨ Key Features | 核心特性

- **🔄 Digital Twin Architecture | 数字孪生架构**: Complete digital twin implementation with DataTwin, ModelTwin, and ServiceTwin layers | 完整的数字孪生实现，包含数据孪生、模型孪生和服务孪生三层架构
- **⚡ Event-Driven Multi-threaded Simulation | 事件驱动多线程仿真**: High-performance event-driven architecture with strict synchronization control | 高性能事件驱动架构，具备严格的同步控制
- **👥 5-Agent Modeling System | 五代理建模系统**: Comprehensive agent models including Pilot, Aircraft, Environment, ATC, and Flight Dynamics | 全面的代理模型，包括飞行员、飞机、环境、空管和飞行动力学
- **🎯 Centralized Event Management | 集中式事件管理**: Centralized event monitoring and controller management systems | 集中式事件监测和控制器管理系统
- **📊 Real-time Data Recording | 实时数据记录**: Comprehensive data recording and visualization capabilities | 全面的数据记录和可视化能力
- **🔧 Modular Design | 模块化设计**: Highly modular architecture supporting easy extension and customization | 高度模块化架构，支持轻松扩展和定制

## 🏗️ Architecture | 架构

### Core Components | 核心组件

```
VFT_SMF V3/
├── src/                           # Source code | 源代码
│   ├── A_PilotAgentModel/         # Pilot agent models | 飞行员代理模型
│   ├── B_AircraftAgentModel/      # Aircraft digital twin models | 飞机数字孪生模型
│   ├── C_EnvironmentAgentModel/   # Environment simulation | 环境仿真
│   ├── D_ATCAgentModel/          # Air Traffic Control models | 空管代理模型
│   ├── E_FlightDynamics/         # Flight dynamics simulation | 飞行动力学仿真
│   ├── F_ScenarioModelling/      # Flight plan parsing and scenario management | 飞行计划解析和场景管理
│   └── G_SimulationManager/      # Core simulation management | 核心仿真管理
├── ScenarioExamples/             # Pre-configured simulation scenarios | 预配置仿真场景
├── tools/                        # Data visualization tools | 数据可视化工具
└── codetest/                     # Unit and integration tests | 单元和集成测试
```

### Digital Twin Architecture | 数字孪生架构

- **DataTwin | 数据孪生**: Physical and mental data representation | 物理和心理数据表示
- **ModelTwin | 模型孪生**: Decision-making and control algorithms | 决策制定和控制算法
- **ServiceTwin | 服务孪生**: State management and service interfaces | 状态管理和服务接口

## 🚁 Supported Aircraft | 支持的飞机

- **Boeing 737 | 波音737**: Complete B737 digital twin implementation | 完整的B737数字孪生实现
- **Extensible | 可扩展**: Easy to add new aircraft types | 易于添加新的飞机类型

## 📋 Prerequisites | 系统要求

- **C++17** compatible compiler (GCC, Clang, or MSVC) | C++17兼容编译器（GCC、Clang或MSVC）
- **Windows 10/11** (primary platform) | Windows 10/11（主要平台）
- **Git** for version control | Git版本控制
- **MATLAB** (optional, for advanced visualization) | MATLAB（可选，用于高级可视化）

## 🛠️ Installation | 安装

### Quick Start | 快速开始

1. **Clone the repository | 克隆仓库**:
   ```bash
   git clone https://github.com/GuilinWuGit/VFT_SMF.git
   cd VFT_SMF
   ```

2. **Navigate to a scenario | 进入场景目录**:
   ```bash
   cd ScenarioExamples/B737_Taxi
   ```

3. **Compile the project | 编译项目**:
   ```bash
   .\build.bat
   ```

4. **Run the simulation | 运行仿真**:
   ```bash
   .\run_simulation_and_visualize.bat
   ```

### Detailed Installation | 详细安装

See [Installation Guide](docs/installation.md) for detailed setup instructions. | 查看[安装指南](docs/installation.md)了解详细设置说明。

## 🎮 Usage | 使用

### Running Simulations | 运行仿真

1. **Choose a scenario | 选择场景** from `ScenarioExamples/`
2. **Configure parameters | 配置参数** in `config/SimulationConfig.json`
3. **Define flight plan | 定义飞行计划** in `input/FlightPlan.json`
4. **Run simulation | 运行仿真** using the provided batch scripts

### Example Scenarios | 示例场景

- **B737_Taxi | B737滑行**: Taxi simulation with ground control | 带地面控制的滑行仿真
- **B737_LevelFlight | B737平飞**: Level flight simulation | 平飞仿真
- **Climb | 爬升**: Climb performance analysis | 爬升性能分析

### Data Visualization | 数据可视化

The platform includes comprehensive visualization tools: | 平台包含全面的可视化工具：

- **Flight State Visualization | 飞行状态可视化**: Real-time aircraft state monitoring | 实时飞机状态监控
- **Net Force Analysis | 净力分析**: Force and moment analysis | 力和力矩分析
- **Event Monitoring | 事件监控**: Event trigger visualization | 事件触发可视化
- **Controller Status | 控制器状态**: Controller execution status tracking | 控制器执行状态跟踪

## 🧪 Testing | 测试

Run the test suite: | 运行测试套件：

```bash
cd codetest
.\build_tests.bat
```

## 📊 Performance | 性能

- **Real-time Simulation | 实时仿真**: Sub-millisecond synchronization | 亚毫秒级同步
- **Multi-threaded Architecture | 多线程架构**: Optimal CPU utilization | 最优CPU利用率
- **Memory Efficient | 内存高效**: Optimized data structures and buffering | 优化的数据结构和缓冲
- **Scalable | 可扩展**: Support for distributed simulation systems | 支持分布式仿真系统

## 🤝 Contributing | 贡献

We welcome contributions! Please see our [Contributing Guide](CONTRIBUTING.md) for details. | 我们欢迎贡献！请查看我们的[贡献指南](CONTRIBUTING.md)了解详情。

### Development Setup | 开发设置

1. Fork the repository | Fork仓库
2. Create a feature branch | 创建功能分支
3. Make your changes | 进行修改
4. Add tests if applicable | 如适用则添加测试
5. Submit a pull request | 提交拉取请求

## 📄 License | 许可证

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details. | 本项目采用MIT许可证 - 详情请查看[LICENSE](LICENSE)文件。

## 👥 Authors | 作者

- **VFT_SMF Development Team | VFT_SMF开发团队** - *Initial work | 初始工作*

## 🙏 Acknowledgments | 致谢

- Academic research community | 学术研究社区
- Open source contributors | 开源贡献者
- Flight simulation enthusiasts | 飞行仿真爱好者

## 📚 Documentation | 文档

- [Architecture Guide | 架构指南](docs/architecture.md)
- [API Reference | API参考](docs/api.md)
- [Troubleshooting | 故障排除](docs/troubleshooting.md)

## 🔗 Links | 链接

- **Repository | 仓库**: https://github.com/GuilinWuGit/VFT_SMF
- **Issues | 问题**: https://github.com/GuilinWuGit/VFT_SMF/issues
- **Discussions | 讨论**: https://github.com/GuilinWuGit/VFT_SMF/discussions

## 📈 Project Status | 项目状态

- **Version | 版本**: 3.0.0
- **Status | 状态**: Active Development | 积极开发中
- **Last Updated | 最后更新**: January 2024 | 2024年1月

---

⭐ **If you find this project useful, please give it a star! | 如果您觉得这个项目有用，请给它一个星标！**
