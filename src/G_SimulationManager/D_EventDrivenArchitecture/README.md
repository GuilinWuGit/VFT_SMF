# 事件驱动架构模块 (D_EventDrivenArchitecture)

## 概述

本模块包含VFT_SMF系统的核心事件驱动架构实现，负责多线程仿真的调度和同步。

## 文件结构

```
D_EventDrivenArchitecture/
├── AgentThreadFunctions.hpp      # 代理线程函数声明
├── AgentThreadFunctions.cpp      # 代理线程函数实现
├── EventDispatcher.hpp           # 事件分发器声明
├── EventDispatcher.cpp           # 事件分发器实现
├── EventDrivenMain_NewArchitecture.cpp  # 主程序入口
└── README.md                     # 本文件
```

## 核心组件

### 1. AgentThreadFunctions
- **功能**: 定义和实现各个代理的线程函数
- **包含线程**:
  - 环境线程 (environment_thread_function)
  - 数据共享空间线程 (data_space_thread_function)
  - 飞行动力学线程 (flight_dynamics_thread_function)
  - 飞行器系统线程 (aircraft_system_thread_function)
  - 事件监测线程 (event_monitor_thread_function)
  - 事件分发线程 (event_dispatcher_thread_function)
  - 飞行员线程 (pilot_thread_function)
  - ATC线程 (atc_thread_function)

### 2. EventDispatcher
- **功能**: 负责将触发的事件分发到对应的代理
- **主要方法**:
  - `processTriggeredEvents()`: 处理已触发事件列表
  - `executeEventController()`: 执行单个事件控制器
  - `routeEventToAgent()`: 将事件路由到指定代理

### 3. EventDrivenMain_NewArchitecture
- **功能**: 主程序入口，协调整个仿真系统
- **主要流程**:
  1. 加载配置文件
  2. 初始化日志系统
  3. 创建全局共享数据空间
  4. 解析飞行计划
  5. 创建数据记录器
  6. 创建时钟系统
  7. 启动各代理线程
  8. 运行仿真主循环
  9. 停止仿真并清理资源

## 架构特点

### 1. 事件驱动
- 基于事件的异步处理机制
- 支持复杂的事件触发条件
- 灵活的事件路由和分发

### 2. 多线程同步
- 使用Simulation_Clock进行严格的时间同步
- 支持实时和加速仿真模式
- 线程间通过共享数据空间通信

### 3. 模块化设计
- 各代理独立运行
- 清晰的接口定义
- 易于扩展和维护

## 使用方式

### 编译
```bash
# 在B737_Taxi项目中编译
./build.bat
```

### 运行
```bash
# 运行仿真
./EventDrivenSimulation_NewArchitecture.exe
```

## 依赖关系

- **G_SimulationManager/A_TimeSYNC**: 时钟同步系统
- **G_SimulationManager/LogAndData**: 日志和数据记录
- **G_SimulationManager/C_ConfigManager**: 配置管理
- **E_GlobalSharedDataSpace**: 全局共享数据空间
- **A_PilotAgentModel**: 飞行员代理模型
- **B_AircraftAgentModel**: 飞机代理模型
- **C_EnvirnomentAgentModel**: 环境代理模型
- **D_ATCAgentModel**: ATC代理模型
- **E_FlightDynamics**: 飞行动力学模型

## 注意事项

1. 所有线程函数必须通过Simulation_Clock进行同步
2. 事件分发需要正确配置控制器到代理的映射关系
3. 共享数据空间的访问需要遵循线程安全原则
4. 配置文件路径需要根据实际部署环境调整
