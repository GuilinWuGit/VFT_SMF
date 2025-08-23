# VFT_SMF 数据流向设计文档

## 0. 重要

**********本文档属于核心设计文档，只能在团队内部使用，严谨上传Github, 严谨私自发送给任何团队以外的人。请尊重大家的科学价值。**********

## 1. 概述

本文档详细描述了VFT_SMF系统中的数据流向设计，包括初始化流程、运行时数据交换、以及各代理之间的数据依赖关系。

## 2. 系统架构概览

### 2.1 核心组件
- **全局共享数据空间** (GlobalSharedDataSpace)：双缓冲数据容器
- **飞行计划解析器** (FlightPlanParser)：解析JSON配置文件
- **5大代理模型**：飞行员、飞机系统、环境、ATC、飞行动力学
- **事件系统**：事件监测和分发
- **数据记录器** (DataRecorder)：记录仿真数据

### 2.2 数据容器结构
```
GlobalSharedDataSpace
├── 状态数据模块（双缓冲）
│   ├── FlightPlanData (飞行计划数据)
│   ├── AircraftFlightState (飞机飞行状态)
│   ├── AircraftSystemState (飞机系统状态)
│   ├── PilotGlobalState (飞行员状态)
│   ├── EnvironmentGlobalState (环境状态)
│   ├── ATCGlobalState (ATC状态)
│   └── AircraftNetForce (飞机净力)
├── 逻辑数据模块（双缓冲）
│   ├── AircraftGlobalLogic
│   ├── PilotGlobalLogic
│   ├── EnvironmentGlobalLogic
│   └── ATCGlobalLogic
└── 事件系统数据
    ├── PlannedEventLibrary (计划事件库)
    ├── TriggeredEventLibrary (已触发事件库)
    └── EventQueue (事件队列)
```

## 3. 初始化阶段数据流向

### 3.1 主函数初始化步骤

```
步骤1: 加载配置文件
步骤2: 初始化日志系统
步骤3: 创建全局共享数据空间
步骤4: 解析飞行计划
步骤5: 创建数据记录器
步骤6: 创建时钟系统
步骤7: 按依赖关系创建代理
步骤8: 所有代理就绪
步骤9: 发布初始化数据
步骤10: 启动仿真时钟
```

### 3.2 飞行计划解析数据流向

**正确的数据流向：**
```
FlightPlan.json → FlightPlanParser → FlightPlanData.global_initial_state
```

**关键设计原则：**
1. **飞行计划解析器只将初始状态存储到 `FlightPlanData.global_initial_state`**
2. **不直接设置到各状态缓冲区**
3. **各代理初始化时从 `FlightPlanData` 读取初始数据**

### 3.3 代理初始化数据流向

**代理依赖顺序：**
1. 环境代理（无依赖）
2. 飞机系统代理（依赖环境）
3. 飞行动力学代理（依赖飞机系统和环境）
4. 飞行员代理（依赖飞行动力学）
5. ATC代理（依赖环境）

**数据流向：**
```
FlightPlanData → 各代理初始化 → 代理计算初始状态 → 存储到对应状态缓冲区
```

## 4. 运行时数据流向

### 4.1 双缓冲机制

- **生产者写入后端缓冲区**
- **swap交换缓冲区**
- **消费者读取前端缓冲区**

### 4.2 代理间数据依赖

```
环境代理 → 飞机系统代理
环境代理 → ATC代理
飞机系统代理 → 飞行动力学代理
飞行动力学代理 → 飞行员代理

数据流向：
环境状态 → 飞机系统状态 → 飞行动力学状态 → 净力计算
```

## 5. 数据初始化详细流程

### 5.1 飞行计划解析阶段

```cpp
// 步骤4: 解析飞行计划
FlightPlanParser::parse_and_store_flight_plan_data()
├── 解析场景配置 → FlightPlanData.scenario_config
├── 解析全局初始状态 → FlightPlanData.global_initial_state
├── 解析逻辑线 → FlightPlanData.logic_lines
├── 解析场景事件 → FlightPlanData.scenario_events
└── 存储到共享数据空间 → setFlightPlanData(flight_plan_data)
```

**重要：** 飞行计划解析器**不直接**调用 `setAircraftSystemState()` 等方法

### 5.2 代理初始化阶段

```cpp
// 步骤7: 代理初始化（按依赖顺序）
1. 环境代理初始化
   ├── 从 FlightPlanData.global_initial_state["environment"] 读取初始数据
   ├── 计算环境初始状态
   └── 调用 setEnvironmentState(env_state, "environment_initial")

2. 飞机系统代理初始化
   ├── 从 FlightPlanData.global_initial_state["aircraft"] 读取初始数据
   ├── B737DigitalTwin::update_cached_states() 解析飞行计划数据
   ├── 计算飞机系统初始状态
   └── 调用 setAircraftSystemState(system_state, "aircraft_system_initial")

3. 飞行动力学代理初始化
   ├── 从 FlightPlanData.global_initial_state["flight_dynamics"] 读取初始数据
   ├── 读取飞机系统状态和环境状态
   ├── 计算飞行动力学初始状态
   └── 调用 setAircraftFlightState(flight_state, "flight_dynamics_initial")

4. 飞行员代理初始化
   ├── 从 FlightPlanData.global_initial_state["pilot"] 读取初始数据
   ├── 计算飞行员初始状态
   └── 调用 setPilotState(pilot_state, "pilot_initial")

5. ATC代理初始化
   ├── 从 FlightPlanData.global_initial_state["atc"] 读取初始数据
   ├── 计算ATC初始状态
   └── 调用 setATCState(atc_state, "atc_initial")
```

### 5.3 初始数据发布阶段

```cpp
// 步骤9: 发布初始化数据
shared_data_space->publishToDataRecorder(0.0)
├── 记录飞行计划数据
├── 记录各代理的初始状态
└── 记录初始净力数据
```

## 6. 数据读取和写入规范

### 6.1 数据读取规范

**代理初始化时：**
```cpp
// 正确：从飞行计划读取初始数据
auto flight_plan_data = global_data_space->getFlightPlanData();
auto aircraft_json = nlohmann::json::parse(flight_plan_data.global_initial_state["aircraft"]);
double throttle = aircraft_json.value("throttle_position", 0.3);

// 错误：从状态缓冲区读取（可能包含默认值）
auto system_state = global_data_space->getAircraftSystemState();
double throttle = system_state.current_throttle_position; // 可能是0.0
```

**运行时：**
```cpp
// 正确：从状态缓冲区读取当前状态
auto system_state = global_data_space->getAircraftSystemState();
auto env_state = global_data_space->getEnvironmentState();
```

### 6.2 数据写入规范

**初始化时：**
```cpp
// 正确：写入初始状态，标记数据来源
shared_data_space->setAircraftSystemState(system_state, "aircraft_system_initial");

// 错误：不标记数据来源
shared_data_space->setAircraftSystemState(system_state);
```

**运行时：**
```cpp
// 正确：写入运行时状态，标记数据来源
shared_data_space->setAircraftSystemState(system_state, "aircraft_system");
```

## 7. 数据一致性保证

### 7.1 初始化顺序保证

```cpp
// 严格的依赖顺序初始化
std::thread environment_thread(VFT_SMF::environment_thread_function, shared_data_space_ptr);
VFT_SMF::wait_for_environment_thread_ready();

std::thread aircraft_system_thread(VFT_SMF::aircraft_system_thread_function, shared_data_space_ptr);
VFT_SMF::wait_for_aircraft_system_thread_ready();

std::thread flight_dynamics_thread(VFT_SMF::flight_dynamics_thread_function, shared_data_space_ptr);
VFT_SMF::wait_for_flight_dynamics_thread_ready();
```

### 7.2 数据来源追踪

每个数据记录都应包含 `datasource` 字段，用于追踪数据来源：
- `"flight_plan"`：来自飞行计划
- `"environment_initial"`：环境代理初始计算
- `"aircraft_system_initial"`：飞机系统代理初始计算
- `"flight_dynamics_initial"`：飞行动力学代理初始计算
- `"pilot_initial"`：飞行员代理初始计算
- `"atc_initial"`：ATC代理初始计算

### 7.3 错误处理

```cpp
// 数据读取失败时的降级策略
try {
    auto aircraft_json = nlohmann::json::parse(flight_plan_data.global_initial_state["aircraft"]);
    cached_throttle_position = aircraft_json.value("throttle_position", 0.3);
} catch (const std::exception& e) {
    // 解析失败时使用默认值
    cached_throttle_position = 0.0;
    logDetail(LogLevel::Detail, "解析飞行计划数据失败，使用默认值");
}
```

## 8. 性能优化考虑

### 8.1 双缓冲优化

- **写入操作**：写入后端缓冲区，避免阻塞读取
- **交换操作**：原子操作，最小化锁竞争
- **读取操作**：读取前端缓冲区，保证数据一致性

### 8.2 内存布局优化

- **缓存友好**：相关数据放在连续内存中
- **对齐优化**：数据结构按缓存行对齐
- **减少拷贝**：使用引用和移动语义

## 9. 调试和监控

### 9.1 数据流向监控

```cpp
// 关键数据点的日志记录
logDetail(LogLevel::Detail, 
    "B737数字孪生从飞行计划更新缓存状态: 油门=" + std::to_string(cached_throttle_position));

logBrief(LogLevel::Brief, 
    "飞行器系统状态已存储到共享数据空间，数据来源: " + datasource);
```

### 9.2 数据验证

```cpp
// 数据合理性检查
if (cached_throttle_position < 0.0 || cached_throttle_position > 1.0) {
    logBrief(LogLevel::Brief, "警告：油门位置超出合理范围: " + std::to_string(cached_throttle_position));
}
```

## 10. 扩展性设计

### 10.1 新代理集成

添加新代理时，需要：
1. 在 `FlightPlanData.global_initial_state` 中添加初始状态定义
2. 实现代理的初始化逻辑，从飞行计划读取数据
3. 在依赖图中确定正确的位置
4. 实现运行时数据交换逻辑

### 10.2 新数据类型支持

添加新数据类型时，需要：
1. 在 `GlobalSharedDataStruct` 中定义数据结构
2. 在 `GlobalSharedDataSpace` 中添加双缓冲容器
3. 实现读写接口
4. 更新数据记录器

## 11. 总结

VFT_SMF的数据流向设计遵循以下核心原则：

1. **单一数据源**：飞行计划是唯一的初始数据源
2. **明确的数据流向**：FlightPlanData → 代理初始化 → 状态缓冲区
3. **严格的时间顺序**：按依赖关系顺序初始化
4. **数据来源追踪**：每个数据记录都标记来源
5. **错误处理**：提供降级策略和错误恢复
6. **性能优化**：使用双缓冲和缓存友好的设计

这种设计确保了系统的数据一致性、可维护性和可扩展性。

## 5. 数据初始化详细流程

### 5.1 飞行计划解析阶段

```cpp
// 步骤4: 解析飞行计划
FlightPlanParser::parse_and_store_flight_plan_data()
├── 解析场景配置 → FlightPlanData.scenario_config
├── 解析全局初始状态 → FlightPlanData.global_initial_state
├── 解析逻辑线 → FlightPlanData.logic_lines
├── 解析场景事件 → FlightPlanData.scenario_events
└── 存储到共享数据空间 → setFlightPlanData(flight_plan_data)
```

**重要：** 飞行计划解析器**不直接**调用 `setAircraftSystemState()` 等方法

### 5.2 代理初始化阶段

```cpp
// 步骤7: 代理初始化（按依赖顺序）
1. 环境代理初始化
   ├── 从 FlightPlanData.global_initial_state["environment"] 读取初始数据
   ├── 计算环境初始状态
   └── 调用 setEnvironmentState(env_state, "environment_initial")

2. 飞机系统代理初始化
   ├── 从 FlightPlanData.global_initial_state["aircraft"] 读取初始数据
   ├── B737DigitalTwin::update_cached_states() 解析飞行计划数据
   ├── 计算飞机系统初始状态
   └── 调用 setAircraftSystemState(system_state, "aircraft_system_initial")

3. 飞行动力学代理初始化
   ├── 从 FlightPlanData.global_initial_state["flight_dynamics"] 读取初始数据
   ├── 读取飞机系统状态和环境状态
   ├── 计算飞行动力学初始状态
   └── 调用 setAircraftFlightState(flight_state, "flight_dynamics_initial")

4. 飞行员代理初始化
   ├── 从 FlightPlanData.global_initial_state["pilot"] 读取初始数据
   ├── 计算飞行员初始状态
   └── 调用 setPilotState(pilot_state, "pilot_initial")

5. ATC代理初始化
   ├── 从 FlightPlanData.global_initial_state["atc"] 读取初始数据
   ├── 计算ATC初始状态
   └── 调用 setATCState(atc_state, "atc_initial")
```

### 5.3 初始数据发布阶段

```cpp
// 步骤9: 发布初始化数据
shared_data_space->publishToDataRecorder(0.0)
├── 记录飞行计划数据
├── 记录各代理的初始状态
└── 记录初始净力数据
```

## 6. 数据读取和写入规范

### 6.1 数据读取规范

**代理初始化时：**
```cpp
// 正确：从飞行计划读取初始数据
auto flight_plan_data = global_data_space->getFlightPlanData();
auto aircraft_json = nlohmann::json::parse(flight_plan_data.global_initial_state["aircraft"]);
double throttle = aircraft_json.value("throttle_position", 0.3);

// 错误：从状态缓冲区读取（可能包含默认值）
auto system_state = global_data_space->getAircraftSystemState();
double throttle = system_state.current_throttle_position; // 可能是0.0
```

**运行时：**
```cpp
// 正确：从状态缓冲区读取当前状态
auto system_state = global_data_space->getAircraftSystemState();
auto env_state = global_data_space->getEnvironmentState();
```

### 6.2 数据写入规范

**初始化时：**
```cpp
// 正确：写入初始状态，标记数据来源
shared_data_space->setAircraftSystemState(system_state, "aircraft_system_initial");

// 错误：不标记数据来源
shared_data_space->setAircraftSystemState(system_state);
```

**运行时：**
```cpp
// 正确：写入运行时状态，标记数据来源
shared_data_space->setAircraftSystemState(system_state, "aircraft_system");
```

## 7. 数据一致性保证

### 7.1 初始化顺序保证

```cpp
// 严格的依赖顺序初始化
std::thread environment_thread(VFT_SMF::environment_thread_function, shared_data_space_ptr);
VFT_SMF::wait_for_environment_thread_ready();

std::thread aircraft_system_thread(VFT_SMF::aircraft_system_thread_function, shared_data_space_ptr);
VFT_SMF::wait_for_aircraft_system_thread_ready();

std::thread flight_dynamics_thread(VFT_SMF::flight_dynamics_thread_function, shared_data_space_ptr);
VFT_SMF::wait_for_flight_dynamics_thread_ready();
```

### 7.2 数据来源追踪

每个数据记录都应包含 `datasource` 字段，用于追踪数据来源：
- `"flight_plan"`：来自飞行计划
- `"environment_initial"`：环境代理初始计算
- `"aircraft_system_initial"`：飞机系统代理初始计算
- `"flight_dynamics_initial"`：飞行动力学代理初始计算
- `"pilot_initial"`：飞行员代理初始计算
- `"atc_initial"`：ATC代理初始计算

### 7.3 错误处理

```cpp
// 数据读取失败时的降级策略
try {
    auto aircraft_json = nlohmann::json::parse(flight_plan_data.global_initial_state["aircraft"]);
    cached_throttle_position = aircraft_json.value("throttle_position", 0.3);
} catch (const std::exception& e) {
    // 解析失败时使用默认值
    cached_throttle_position = 0.0;
    logDetail(LogLevel::Detail, "解析飞行计划数据失败，使用默认值");
}
```

## 8. 性能优化考虑

### 8.1 双缓冲优化

- **写入操作**：写入后端缓冲区，避免阻塞读取
- **交换操作**：原子操作，最小化锁竞争
- **读取操作**：读取前端缓冲区，保证数据一致性

### 8.2 内存布局优化

- **缓存友好**：相关数据放在连续内存中
- **对齐优化**：数据结构按缓存行对齐
- **减少拷贝**：使用引用和移动语义

## 9. 调试和监控

### 9.1 数据流向监控

```cpp
// 关键数据点的日志记录
logDetail(LogLevel::Detail, 
    "B737数字孪生从飞行计划更新缓存状态: 油门=" + std::to_string(cached_throttle_position));

logBrief(LogLevel::Brief, 
    "飞行器系统状态已存储到共享数据空间，数据来源: " + datasource);
```

### 9.2 数据验证

```cpp
// 数据合理性检查
if (cached_throttle_position < 0.0 || cached_throttle_position > 1.0) {
    logBrief(LogLevel::Brief, "警告：油门位置超出合理范围: " + std::to_string(cached_throttle_position));
}
```

## 10. 扩展性设计

### 10.1 新代理集成

添加新代理时，需要：
1. 在 `FlightPlanData.global_initial_state` 中添加初始状态定义
2. 实现代理的初始化逻辑，从飞行计划读取数据
3. 在依赖图中确定正确的位置
4. 实现运行时数据交换逻辑

### 10.2 新数据类型支持

添加新数据类型时，需要：
1. 在 `GlobalSharedDataStruct` 中定义数据结构
2. 在 `GlobalSharedDataSpace` 中添加双缓冲容器
3. 实现读写接口
4. 更新数据记录器

## 11. 总结

VFT_SMF的数据流向设计遵循以下核心原则：

1. **单一数据源**：飞行计划是唯一的初始数据源
2. **明确的数据流向**：FlightPlanData → 代理初始化 → 状态缓冲区
3. **严格的时间顺序**：按依赖关系顺序初始化
4. **数据来源追踪**：每个数据记录都标记来源
5. **错误处理**：提供降级策略和错误恢复
6. **性能优化**：使用双缓冲和缓存友好的设计

这种设计确保了系统的数据一致性、可维护性和可扩展性。
