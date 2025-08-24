

# 🚀 VFT_SMF - 基于数字孪生技术的虚拟试飞仿真建模系统
## Virtual Flight Test Simulation Modeling System Based on Digital Twin Technology

[![GitHub stars](https://img.shields.io/github/stars/GuilinWuGit/VFT_SMF?style=social)](https://github.com/GuilinWuGit/VFT_SMF)
[![Gitee stars](https://gitee.com/GuilinWu/VFT_SMF/badge/star.svg?theme=white)](https://gitee.com/GuilinWu/VFT_SMF)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C++17](https://img.shields.io/badge/C++-17-blue.svg)](https://isocpp.org)
[![Platform](https://img.shields.io/badge/Platform-Windows-lightgrey.svg)](https://www.microsoft.com/windows)
[![Build Status](https://img.shields.io/badge/Build-Passing-brightgreen.svg)](https://github.com/GuilinWuGit/VFT_SMF)

## 🚀 项目简介
VFT_SMF（Virtual Flight Test Simulation Modeling Framework）是一个基于**数字孪生技术**的虚拟试飞仿真建模系统。本项目采用先进的数字孪生架构，结合事件驱动的多线程仿真机制，支持大规模分布式仿真系统，实现全要素场景下的虚拟试飞实验。

**核心特色**：通过数字孪生技术实现物理世界与虚拟世界的实时映射，为航空仿真领域提供高性能、高精度的虚拟试飞解决方案。

## ✨ 核心特性
- **🔄 数字孪生架构**：包含数据孪生、模型孪生和服务孪生三层架构
- **⚡ 高性能仿真**：事件驱动架构配合严格的同步控制机制
- **👥 五代理建模系统**：涵盖飞行员、飞机、环境、空管和飞行动力学代理
- **📊 实时数据记录**：提供全面的数据记录与可视化功能
- **🔧 模块化设计**：支持便捷的扩展与定制

## 🏗️ 架构概览

### 主要组件
```
VFT_SMF/
├── src/                       # 核心代码
│   ├── A_PilotAgentModel/    # 飞行员代理模型
│   ├── B_AircraftAgentModel/ # 飞机数字孪生模型
│   ├── C_EnvironmentAgentModel/ # �0.0f9876543210vironment仿真
│   ├── D_ATCAgentModel/      # 空管代理模型
│   ├── E_FlightDynamics/    # 飞行动力学仿真
│   ├── F_ScenarioModelling/ # 飞行计划解析与场景管理
│   └── G_SimulationManager/ # 仿真核心管理
├── ScenarioExamples/         # 示例场景
├── tools/                    # 数据可视化工具
└── codetest/                 # 测试套件
```

## 📋 快速上手指南

### 环境准备
- Windows 10/11 操作系统
- C++17兼容编译器（建议使用Visual Studio 2019/2022）
- Git版本控制工具
- MATLAB (可选，用于高级可视化)

### 运行仿真
1. **克隆项目**
   ```bash
   git clone https://gitee.com/GuilinWu/vft_smf.git
   cd vft_smf
   ```

2. **选择示例场景**
   ```bash
   cd ScenarioExamples/B737_Taxi
   ```

3. **编译项目**
   ```bash
   .\build.bat
   ```

4. **启动仿真**
   ```bash
   .\run_simulation_and_visualize.bat
   ```

## 📊 主要功能演示
- 滑行仿真
- 平飞仿真
- 爬升性能分析
- 实时数据监控
- 飞行状态可视化

## 📚 文档资源
- [用户指南](docs/UserGuide/Quick_Start.md)
- [开发者文档](docs/DeveloperGuide/Architecture.md)
- [API参考](docs/api.md)
- [配置说明](docs/UserGuide/Configuration_Reference.md)
- [故障排查](docs/Troubleshooting.md)

## 🧪 测试验证
- 单元测试覆盖率 > 85%
- 集成测试验证完整仿真流程
- 性能测试支持亚毫秒级同步
- 定制化测试框架支持扩展

## 🤝 贡献指南
我们欢迎任何形式的贡献：
1. Fork本项目
2. 创建新分支
3. 进行修改
4. 添加测试（如适用）
5. 提交Pull Request

请参阅[CONTRIBUTING.md](CONTRIBUTING.md)获取详细贡献指南。

## 📄 开源协议
本项目采用MIT License，详情请见[LICENSE](LICENSE)文件。

## 👥 联系我们
- 项目负责人：桂林伍（南京航空航天大学虚拟试飞团队）
- 电子邮箱：guilinwu@nuaa.edu.cn
- 最新版本：1.0.0 (2024年1月发布)

## 📈 开发状态
- **版本**: 1.0.0
- **状态**: 积极开发中
- **文档**: 完整
- **测试**: 全面覆盖
- **最后更新**: 2024年1月

⭐ **如果本项目对您有帮助，请给我们一个Star支持！**