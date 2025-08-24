# VFT_SMF - 虚拟试飞仿真与建模框架

[![Gitee stars](https://gitee.com/你的用户名/VFT_SMF/badge/star.svg?theme=white)](https://gitee.com/你的用户名/VFT_SMF)
[![Gitee forks](https://gitee.com/你的用户名/VFT_SMF/badge/fork.svg?theme=white)](https://gitee.com/你的用户名/VFT_SMF)
[![License](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![C++](https://img.shields.io/badge/C++-17-blue.svg)](https://isocpp.org/)
[![Platform](https://img.shields.io/badge/Platform-Windows-lightgrey.svg)](https://www.microsoft.com/windows)

## 🎯 项目简介

VFT_SMF（Virtual Flight Test Simulation and Modeling Framework）是一个基于**数字孪生**和**事件驱动架构**的航空仿真平台，专为虚拟试飞和飞行仿真研究设计。

本项目采用现代化的C++17标准，结合多线程技术和实时数据处理，为航空仿真领域提供高性能、可扩展的解决方案。

## ✨ 核心特性

### 🏗️ 数字孪生架构
- **三层模型设计**: 数据层、模型层、服务层
- **实时数据同步**: 双缓冲机制确保数据一致性
- **状态管理**: 完整的生命周期管理

### ⚡ 事件驱动设计
- **异步事件处理**: 高性能事件分发系统
- **多线程架构**: 充分利用多核处理器性能
- **实时响应**: 毫秒级事件处理能力

### 🤖 五代理建模系统
- **飞行员代理**: 智能决策和行为模拟
- **飞机系统代理**: 飞行器物理模型
- **环境代理**: 大气、天气、地形建模
- **ATC代理**: 空中交通管制模拟
- **飞行动力学代理**: 精确的飞行物理计算

### 📊 数据管理
- **实时数据记录**: 高性能数据采集
- **可视化分析**: 丰富的图表和监控工具
- **配置驱动**: JSON配置文件管理

## 🚀 快速开始

### 环境要求
- Windows 10/11
- Visual Studio 2019+ 或 MinGW-w64
- Git

### 安装步骤

```bash
# 1. 克隆仓库
git clone https://gitee.com/你的用户名/VFT_SMF.git

# 2. 进入项目目录
cd VFT_SMF

# 3. 编译项目
cd ScenarioExamples/B737_Taxi
./build.bat

# 4. 运行仿真
./EventDrivenSimulation_NewArchitecture.exe
```

### 首次运行
1. 程序会自动生成配置文件
2. 查看 `output/` 目录下的结果文件
3. 使用可视化工具分析数据

## 📚 文档

### 用户指南
- [📖 安装指南](docs/UserGuide/Installation_Guide.md) - 详细的安装和配置说明
- [⚡ 快速开始](docs/UserGuide/Quick_Start.md) - 5分钟快速上手
- [⚙️ 配置参考](docs/UserGuide/Configuration_Reference.md) - 完整的配置选项
- [🔧 故障排除](docs/Troubleshooting.md) - 常见问题解决方案

### 开发者指南
- [🏗️ 系统架构](docs/DeveloperGuide/Architecture.md) - 详细的架构设计
- [📊 数据流向设计](docs/DeveloperGuide/DataFlow.md) - 核心数据流向分析
- [📋 文档规划](docs/DeveloperGuide/文档规划.md) - 完整的文档体系
- [📈 代码质量评估](docs/DeveloperGuide/代码质量评估.md) - 代码质量分析

### 项目文档
- [📝 更新日志](docs/CHANGELOG.md) - 版本更新历史
- [❓ 常见问题](docs/FAQ.md) - 用户常见问题
- [🤝 贡献指南](docs/DeveloperGuide/Contributing_Guidelines.md) - 如何参与项目

## 🎮 使用示例

### 基础仿真场景
```json
{
  "scenario_name": "B737_Taxi",
  "aircraft_type": "Boeing_737",
  "environment_model": "PVG_Runway_05",
  "simulation_duration": 60.0
}
```

### 自定义配置
- 修改 `input/FlightPlan.json` 配置飞行计划
- 调整 `config/SimulationConfig.json` 设置仿真参数
- 在 `src/C_EnvirnomentAgentModel/` 添加新的环境模型

## 🤝 参与贡献

我们欢迎所有形式的贡献！

### 贡献方式
1. **报告问题**: 在 [Issues](https://gitee.com/你的用户名/VFT_SMF/issues) 中报告Bug或提出建议
2. **提交代码**: Fork项目，创建分支，提交Pull Request
3. **改进文档**: 帮助完善项目文档
4. **分享经验**: 在讨论区分享使用经验

### 开发环境设置
```bash
# 克隆开发分支
git clone -b develop https://gitee.com/你的用户名/VFT_SMF.git

# 安装依赖
# 参考 docs/DeveloperGuide/Development_Setup.md
```

## 📊 项目统计

![项目统计](https://gitee.com/你的用户名/VFT_SMF/widgets/widget_card.svg?colors=4183c4,ffffff,ffffff,e3e9ed,666666,9b9b9b)

## 🏆 项目特色

### 技术创新
- **数字孪生技术**: 业界领先的数字孪生应用
- **事件驱动架构**: 高性能异步处理
- **多代理建模**: 复杂的航空系统模拟

### 开源优势
- **完全开源**: MIT协议，可自由使用
- **活跃社区**: 持续更新和改进
- **丰富文档**: 详细的中文文档

### 应用前景
- **航空教育**: 飞行训练和教学
- **科研仿真**: 航空技术研究
- **工业应用**: 飞行器设计和测试

## 📞 联系我们

- **项目主页**: [https://gitee.com/你的用户名/VFT_SMF](https://gitee.com/你的用户名/VFT_SMF)
- **问题反馈**: [Issues](https://gitee.com/你的用户名/VFT_SMF/issues)
- **讨论交流**: [Discussions](https://gitee.com/你的用户名/VFT_SMF/issues)
- **邮箱联系**: your-email@example.com

## 📄 开源协议

本项目采用 [MIT License](LICENSE) 开源协议。

```
MIT License

Copyright (c) 2024 VFT_SMF Development Team

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

## 🙏 致谢

感谢所有为VFT_SMF项目做出贡献的开发者！

### 核心贡献者
- [@GuilinWu](https://gitee.com/GuilinWu) - 项目创始人
- [@YourName](https://gitee.com/YourName) - 核心开发者

### 开源项目
- [nlohmann/json](https://github.com/nlohmann/json) - JSON处理库
- [其他依赖项目]

---

**如果这个项目对您有帮助，请给我们一个 ⭐ Star！**

[![Star History Chart](https://api.star-history.com/svg?repos=你的用户名/VFT_SMF&type=Date)](https://star-history.com/#你的用户名/VFT_SMF&Date)

