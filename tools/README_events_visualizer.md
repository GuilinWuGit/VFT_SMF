# 事件触发数据可视化工具

## 文件说明

`visualize_triggered_events.cpp` 是一个专门用于可视化事件触发数据的C++程序，通过调用MATLAB生成散点图，显示事件在时间轴上的触发情况。

## 功能特点

- **数据解析**：自动解析 `triggered_events.csv` 文件
- **JSON解析**：智能解析JSON格式的事件列表
- **MATLAB集成**：生成MATLAB脚本并调用MATLAB绘制图表
- **散点图显示**：横坐标为仿真时间，纵坐标为事件ID
- **事件标注**：在散点附近标注事件名称

## 编译方法

### 使用批处理文件编译
```cmd
cd D:\VFT_SMF_V3\tools
build_events_visualizer.bat
```

### 手动编译
```cmd
g++ -std=c++17 -O2 -o visualize_triggered_events.exe visualize_triggered_events.cpp
```

## 使用方法

### 基本用法
```cmd
visualize_triggered_events.exe "CSV文件路径" [输出目录]
```

### 示例
```cmd
# 使用默认输出目录（CSV文件所在目录）
visualize_triggered_events.exe "..\ScenarioExamples\B737_Taxi\output\triggered_events.csv"

# 指定输出目录
visualize_triggered_events.exe "..\ScenarioExamples\B737_Taxi\output\triggered_events.csv" "D:\output"
```

## 输出文件

### 生成的文件
- `triggered_events.png` - 事件触发散点图
- `plot_triggered_events.m` - MATLAB脚本文件

### 图表布局
- **散点图**：红色圆点表示事件触发
- **时间轴**：横坐标显示仿真时间（秒）
- **事件轴**：纵坐标显示事件ID
- **事件标注**：蓝色文字标注事件名称

## 数据解析

### CSV文件格式
- **SimulationTime**：仿真时间
- **StepNumber**：仿真步数
- **EventCount**：事件计数
- **EventList**：事件列表（JSON格式）

### JSON事件格式
程序能够解析以下格式的事件列表：
```json
[{'id':'3', 'name':'issue_taxi_clearance', 'triggered':true}]
```

### 事件类型识别
程序会自动提取事件名称并为每种事件类型分配唯一ID：
- 事件ID 1: issue_taxi_clearance
- 事件ID 2: taxi_clearance_received
- 事件ID 3: Encounter_Left_Engine_Out
- 事件ID 4: start_cruise
- 事件ID 5: Encounter_Runway_Condition_Change

## 图表特性

### 视觉设计
- **散点标记**：红色填充圆点，黑色边框
- **事件标注**：蓝色文字，45度旋转
- **网格线**：便于读取时间和事件ID
- **坐标轴标签**：清晰的轴标题

### 技术规格
- **图表尺寸**：1400x900像素
- **分辨率**：300 DPI
- **字体大小**：标题14pt，轴标签12pt，事件标注10pt

## 数据格式要求

### CSV文件格式
- **分隔符**：空格分隔
- **编码**：UTF-8
- **表头**：必须包含列名
- **数据类型**：数值型和字符串型

### 必需列
- `SimulationTime` - 仿真时间（数值）
- `StepNumber` - 仿真步数（数值）
- `EventCount` - 事件计数（数值）
- `EventList` - 事件列表（字符串，JSON格式）

## 故障排除

### 常见问题

#### 1. 编译错误
- 确保使用C++17标准
- 检查编译器是否正确安装

#### 2. MATLAB调用失败
- 确保MATLAB已安装
- 检查MATLAB是否添加到系统PATH
- 验证MATLAB许可证

#### 3. 文件路径问题
- 使用绝对路径或正确的相对路径
- 确保路径中没有特殊字符

#### 4. 数据解析错误
- 检查CSV文件格式
- 确保数据列数一致
- 验证JSON格式是否正确

### 调试信息

程序会输出详细的调试信息：
- 表头解析结果
- 数据行统计
- 事件触发统计
- 事件类型统计
- MATLAB脚本生成状态

## 性能优化

### 编译优化
- 使用 `-O2` 优化级别
- 启用C++17特性

### 内存管理
- 使用 `std::vector` 动态分配
- 使用 `std::map` 高效查找事件ID

### 文件I/O
- 流式读取大文件
- 错误处理和异常捕获

## 扩展功能

### 可能的改进
- 支持更多JSON格式
- 添加事件持续时间显示
- 自定义图表样式
- 批量处理多个文件
- 事件统计分析

### 代码结构
- 模块化设计
- 易于扩展和维护
- 清晰的类层次结构
- 智能的事件解析算法
