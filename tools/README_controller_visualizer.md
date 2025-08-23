# 控制器执行状态可视化工具

## 功能简介

本工具用于可视化VFT_SMF仿真系统中控制器的执行状态数据，将`controller_execution_status.csv`文件转换为直观的散点图。

## 主要特性

- **时序展示**：横坐标为仿真时间，清晰展示控制器状态的时间变化
- **多控制器对比**：纵坐标为控制器名称，支持同时显示多个控制器状态
 - **直观图形**：
   - 蓝色实心圆点 (●)：控制器运行中 (状态值=1)
   - 状态为0时不显示任何点，保持图形简洁
- **高质量输出**：生成300 DPI的PNG图像文件

## 技术架构

### C++ + MATLAB 集成
- **C++程序**：负责CSV文件解析和数据处理
- **MATLAB脚本**：负责图形绘制和美化
- **自动化流程**：C++程序自动生成并执行MATLAB脚本

### 核心组件
1. **CSVParser**：解析固定宽度格式的CSV文件
2. **MATLABScriptGenerator**：生成专用的MATLAB绘图脚本
3. **MATLABExecutor**：自动执行MATLAB脚本并保存图像

## 编译方法

```bash
cd D:\VFT_SMF_V3\tools
.\build_controller_visualizer.bat
```

## 使用方法

### 基本用法
```bash
visualize_controller_execution.exe <controller_execution_status.csv文件路径>
```

### 示例
```bash
# 在tools目录下运行
visualize_controller_execution.exe "../ScenarioExamples/B737_Taxi/output/controller_execution_status.csv"
```

## 输入数据格式

程序读取`controller_execution_status.csv`文件，期望格式为：

```
SimulationTime  Left_Engine_Out      Break_Half           clearance_controller Emergency_Brake_Command Runway_Condition_Change throttle_push2max    MaintainSPDRunway    brake_push2max      
0.00           0                     0                     0                     0                        0                        0                     0                     0                   
0.10           0                     0                     1                     0                        0                        0                     0                     0                   
```

- **第一列**：SimulationTime（仿真时间）
- **其他列**：各控制器的执行状态（0=未运行，1=运行中）

## 输出文件

工具会在CSV文件同目录下生成：

1. **controller_execution_status.png**：主要可视化图像
2. **plot_controller_execution.m**：生成的MATLAB脚本（调试用）

## 可视化效果

### 图形布局
- **横坐标**：仿真时间（秒）
- **纵坐标**：控制器名称（垂直排列）
 - **数据点**：
   - 蓝色实心圆：控制器运行中 (状态=1)
   - 状态为0时不显示任何点，保持图形简洁清晰

### 图形特性
- 自动缩放坐标轴范围
- 显示网格线便于读取数值
- 包含图例说明
- 中文标签支持

## 系统要求

- **编译器**：g++ 支持C++17标准
- **MATLAB**：需要系统安装MATLAB并配置PATH环境变量
- **操作系统**：Windows（当前版本）

## 故障排除

### 编译问题
- 确保g++编译器已正确安装
- 检查C++17标准支持

### 运行问题
- 确保MATLAB已安装并在PATH中
- 检查CSV文件路径是否正确
- 验证CSV文件格式是否符合要求

### 图像生成问题
- 检查输出目录是否有写入权限
- 确保MATLAB脚本语法正确
- 查看控制台输出的错误信息

## 开发信息

- **开发团队**：VFT_SMF Development Team
- **创建日期**：2024
- **版本**：V3
- **编程语言**：C++17 + MATLAB

## 相关文件

- `visualize_controller_execution.cpp`：主程序源代码
- `build_controller_visualizer.bat`：编译脚本
- `README_controller_visualizer.md`：本说明文档

本工具是VFT_SMF仿真框架可视化工具集的重要组成部分，与其他数据可视化工具协同工作，提供完整的仿真数据分析能力。
