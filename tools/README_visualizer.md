# 飞机状态数据可视化工具 (C++ + MATLAB)

## 功能说明
C++程序解析CSV数据，自动生成MATLAB脚本并调用MATLAB绘制高质量图表。

## 编译方法
```bash
# 在tools目录下运行
.\build_visualizer.bat
```

## 使用方法

### 基本用法
```bash
visualize_aircraft_state.exe "path/to/aircraft_flight_state.csv"
```

### 指定输出目录
```bash
visualize_aircraft_state.exe "path/to/aircraft_flight_state.csv" "output/directory"
```

### 实际示例
```bash
# 在tools目录下运行
visualize_aircraft_state.exe "../ScenarioExamples/B737_Taxi/output/aircraft_flight_state.csv"
```

## 输出文件

### 控制台输出
- 数据统计信息（均值、最大值、最小值、标准差）
- 处理进度提示

### 生成文件
- `plot_aircraft_state.m` - MATLAB脚本文件
- `aircraft_state_visualization.png` - 详细子图（每个状态量一张图）
- `aircraft_state_summary.png` - 汇总图（关键状态量垂直排列）

## 系统要求
- C++17编译器（g++）
- MATLAB（需要添加到系统PATH）
- Windows系统

## 工作原理
1. C++程序解析CSV文件
2. 计算数据统计信息
3. 生成MATLAB脚本
4. 调用MATLAB执行脚本
5. 输出高质量PNG图片

## 故障排除
如果MATLAB调用失败：
1. 确保MATLAB已安装
2. 确保MATLAB已添加到系统PATH
3. 手动运行生成的`plot_aircraft_state.m`脚本

## 特点
- 高性能C++数据解析
- 专业级MATLAB图表
- 自动统计信息计算
- 支持中文显示
- 高分辨率输出（300 DPI）
