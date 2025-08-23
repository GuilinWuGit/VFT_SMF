# 飞机净力数据可视化工具

## 文件说明

`visualize_aircraft_net_force.cpp` 是一个专门用于可视化飞机净力数据的C++程序，通过调用MATLAB生成高质量的图表。

## 功能特点

- **数据解析**：自动解析 `aircraft_net_force.csv` 文件
- **MATLAB集成**：生成MATLAB脚本并调用MATLAB绘制图表
- **专业布局**：3列4行布局，显示11个力和力矩参数
- **高质量输出**：300 DPI分辨率，适合学术和工程应用

## 编译方法

### 使用批处理文件编译
```cmd
cd D:\VFT_SMF_V3\tools
build_net_force_visualizer.bat
```

### 手动编译
```cmd
g++ -std=c++17 -O2 -o visualize_aircraft_net_force.exe visualize_aircraft_net_force.cpp
```

## 使用方法

### 基本用法
```cmd
visualize_aircraft_net_force.exe "CSV文件路径" [输出目录]
```

### 示例
```cmd
# 使用默认输出目录（CSV文件所在目录）
visualize_aircraft_net_force.exe "..\ScenarioExamples\B737_Taxi\output\aircraft_net_force.csv"

# 指定输出目录
visualize_aircraft_net_force.exe "..\ScenarioExamples\B737_Taxi\output\aircraft_net_force.csv" "D:\output"
```

## 输出文件

### 生成的文件
- `aircraft_net_force.png` - 飞机净力可视化图表
- `plot_aircraft_net_force.m` - MATLAB脚本文件

### 图表布局
- **3列4行布局**：共12个子图位置，使用11个显示数据
- **图表尺寸**：1400x900像素
- **分辨率**：300 DPI

## 数据参数

程序会显示以下11个力和力矩参数：

### 三轴力
- `longitudinal_force` - 纵向力
- `lateral_force` - 横向力
- `vertical_force` - 垂直力

### 三轴力矩
- `roll_moment` - 滚转力矩
- `pitch_moment` - 俯仰力矩
- `yaw_moment` - 偏航力矩

### 主要气动力
- `thrust_force` - 推力
- `drag_force` - 阻力
- `lift_force` - 升力

### 其他力
- `weight_force` - 重力
- `side_force` - 侧力

## 图表特性

### 视觉设计
- **简洁布局**：无冗余标题和均值线
- **网格线**：便于读取数值
- **左对齐标签**：专业的视觉效果
- **2位小数格式**：统一的数据精度

### 技术规格
- **时间轴**：仿真时间（秒）
- **数据轴**：各参数的数值
- **颜色方案**：蓝色线条，白色背景
- **字体**：MATLAB默认字体

## 数据格式要求

### CSV文件格式
- **分隔符**：空格分隔
- **编码**：UTF-8
- **表头**：必须包含列名
- **数据类型**：数值型（除datasource列外）

### 必需列
- `SimulationTime` - 仿真时间
- `datasource` - 数据源（字符串）
- 其他11个力和力矩参数列

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
- 验证数值格式

### 调试信息

程序会输出详细的调试信息：
- 表头解析结果
- 数据行统计
- 每列的数据类型和数量
- MATLAB脚本生成状态

## 性能优化

### 编译优化
- 使用 `-O2` 优化级别
- 启用C++17特性

### 内存管理
- 使用 `std::vector` 动态分配
- 避免不必要的内存拷贝

### 文件I/O
- 流式读取大文件
- 错误处理和异常捕获

## 扩展功能

### 可能的改进
- 支持更多数据格式
- 添加数据过滤功能
- 自定义图表样式
- 批量处理多个文件

### 代码结构
- 模块化设计
- 易于扩展和维护
- 清晰的类层次结构
