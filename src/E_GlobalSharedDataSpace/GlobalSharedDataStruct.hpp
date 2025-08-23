/*
* @file GlobalSharedDataSpace.hpp
* @brief 全局共享数据空间定义
* @author VFT_SMF Development Team
* @date 2024
* 
* 全局共享数据空间负责：
* 1. 定义全局共享数据空间的数据结构，包括：
* 
* 1）飞行计划数据结构体
* 2）标准事件结构体
* 3）计划事件库数据结构体
* 4）已触发事件库数据结构体
* 5）飞行动力学状态数据结构体
* 6）飞机系统状态数据结构体
* 7）环境状态数据结构体
* 8）ATC状态数据结构体
* 9）飞行员状态数据结构体
* 10）飞行员逻辑数据结构体
* 11）飞机系统逻辑数据结构体
* 12）环境逻辑数据结构体
* 13）ATC逻辑数据结构体
*/
#pragma once

#include <string>
#include <map>
#include <vector>
#include <queue>
#include <mutex>
#include <tuple>
#include <chrono>
#include <algorithm>
#include "../F_ScenarioModelling/B_ScenarioModel/VFT_SMF_Base.hpp"
#include "../G_SimulationManager/B_SimManage/SimulationNameSpace.hpp"
#include "../G_SimulationManager/LogAndData/Logger.hpp"
#include "../G_SimulationManager/B_SimManage/SimulationNameSpace.hpp"
#include <atomic>
#include <condition_variable>


namespace VFT_SMF {
    namespace GlobalSharedDataStruct  {
        
        // ==================== 时间同步结构体 ====================
        
        /**
         * @brief 线程同步状态枚举
         */
        enum class ThreadSyncState {
            WAITING_FOR_CLOCK,    ///< 等待时钟信号
            RUNNING,              ///< 正在运行
            COMPLETED,            ///< 已完成当前步骤
            ERROR_STATE           ///< 错误状态
        };
        
        /**
         * @brief 线程注册信息结构体
         */
        struct ThreadRegistrationInfo {
            std::string thread_id;           ///< 线程ID
            std::string thread_name;         ///< 线程名称
            std::string thread_type;         ///< 线程类型（环境、数据空间等）
            bool is_registered;              ///< 是否已注册
            bool is_ready;                   ///< 是否就绪
            ThreadSyncState sync_state;      ///< 同步状态
            double last_completion_time;     ///< 上次完成时间
            double current_step_time;        ///< 当前步骤时间
            
            ThreadRegistrationInfo() : is_registered(false), is_ready(false),
                                     sync_state(ThreadSyncState::WAITING_FOR_CLOCK),
                                     last_completion_time(0.0), current_step_time(0.0) {}
        };
        
        /**
         * @brief 时钟同步信号结构体
         */
        struct ClockSyncSignal {
            double current_simulation_time;  ///< 当前仿真时间
            uint64_t current_step;           ///< 当前仿真步数
            bool step_ready;                 ///< 步骤是否就绪
            bool all_threads_completed;      ///< 所有线程是否完成
            std::vector<std::string> completed_threads; ///< 已完成的线程列表
            std::vector<std::string> waiting_threads;   ///< 等待中的线程列表
            
            ClockSyncSignal() : current_simulation_time(0.0), current_step(0),
                               step_ready(false), all_threads_completed(false) {}
        };
        
        /**
         * @brief 线程同步管理结构体 - 只包含数据，不包含方法
         */
        struct ThreadSyncManager {
            std::map<std::string, ThreadRegistrationInfo> registered_threads; ///< 注册的线程
            ClockSyncSignal current_sync_signal;                              ///< 当前同步信号
            std::atomic<bool> clock_running;                                  ///< 时钟是否运行
            std::atomic<bool> step_in_progress;                               ///< 步骤是否进行中
            std::atomic<bool> is_sim_over;                                    ///< 仿真是否结束标志
            
            ThreadSyncManager() : clock_running(false), step_in_progress(false), is_sim_over(false) {}
        };
         
        // 1）飞行计划数据结构体
        struct FlightPlanData {
            std::string datasource;              ///< 数据来源标识
            
            // 场景配置信息
            struct ScenarioConfig {
                std::string ScenarioName;        ///< 场景名称
                std::string Description;         ///< 场景描述
                std::string Author;              ///< 作者
                std::string CreationDate;        ///< 创建日期
                std::string ScenarioType;        ///< 场景类型
                std::string Pilot_ID;            ///< 飞行员ID
                std::string Aircraft_ID;         ///< 飞机ID
                std::string ATC_ID;              ///< ATC ID
                std::string Environment_Name;    ///< 环境名称
            } scenario_config;
            
            // 全局初始状态
            std::map<std::string, std::string> global_initial_state;  ///< 全局初始状态映射
            
            // 逻辑线数据
            std::map<std::string, std::string> logic_lines;           ///< 逻辑线数据映射
            
            // 场景事件
            struct ScenarioEvent {
                std::string event_id;            ///< 事件ID
                std::string event_type;          ///< 事件类型
                double trigger_delay;            ///< 触发延迟（秒）
                std::string condition_expression; ///< 条件表达式
                std::string condition_description; ///< 条件描述
                bool is_triggered;               ///< 是否已触发
                
                ScenarioEvent(const std::string& id, const std::string& type, double delay = 0.0,
                             const std::string& cond_expr = "", const std::string& cond_desc = "")
                    : event_id(id), event_type(type), trigger_delay(delay), 
                      condition_expression(cond_expr), condition_description(cond_desc), is_triggered(false) {}
            };
            std::vector<ScenarioEvent> scenario_events;              ///< 场景事件列表
            
            // 驱动过程
            struct DrivenProcess {
                std::string line_name;           ///< 逻辑线名称
                std::string event_id;            ///< 事件ID
                std::string event_name;          ///< 事件名称
                std::string controller_type;     ///< 控制器类型
                std::string controller_name;     ///< 控制器名称
                std::string description;         ///< 描述
                std::string termination_condition; ///< 终止条件
                
                DrivenProcess() = default;
            };
            std::map<std::string, DrivenProcess> driven_processes;   ///< 驱动过程映射
            
            // 元数据
            std::map<std::string, std::string> metadata;             ///< 元数据映射
            
            // 解析状态
            bool is_parsed;                      ///< 是否已解析
            std::string file_path;               ///< 文件路径
            SimulationTimePoint parse_time;      ///< 解析时间
            
            FlightPlanData() : datasource("initialspace"), is_parsed(false), 
                              parse_time(SimulationTimePoint{}) {}
        };
        
        // 2）标准事件数据结构体
        
       struct TriggerCondition {
        std::string condition_expression;    ///< 条件表达式
        std::string description;             ///< 条件描述
        
        TriggerCondition(const std::string& expr = "", const std::string& desc = "")
            : condition_expression(expr), description(desc) {}
    };
       struct DrivenProcess {
        std::string controller_type;         ///< 控制器类型
        std::string controller_name;         ///< 控制器名称
        std::string description;             ///< 过程描述
        std::string termination_condition;   ///< 终止条件
        
        DrivenProcess(const std::string& ctrl_type = "", const std::string& ctrl_name = "",
                     const std::string& desc = "", const std::string& term_cond = "")
            : controller_type(ctrl_type), controller_name(ctrl_name),
              description(desc), termination_condition(term_cond) {}
    };
       struct StandardEvent {
        std::string datasource;              ///< 数据来源标识
        
        int event_id;                        ///< 事件ID（与模板匹配，使用int类型）
        std::string event_name;              ///< 事件名称
        std::string description;             ///< 事件描述（与模板匹配，统一命名）
        TriggerCondition trigger_condition;  ///< 触发条件（与模板匹配）
        DrivenProcess driven_process;        ///< 驱动过程（与模板匹配）
        std::string source_agent;            ///< 源代理
        bool is_triggered;                   ///< 是否已触发
        
        // ==================== 构造函数 ====================
        
        /**
         * @brief 默认构造函数
         */
        StandardEvent() : datasource("initialspace"), event_id(0), event_name(""), description(""),
                         trigger_condition(), driven_process(), source_agent(""), is_triggered(false) {}
        
        /**
         * @brief 标准构造函数（与飞行计划模板匹配）
         */
        StandardEvent(int id, const std::string& name, const std::string& desc,
                    const TriggerCondition& trigger_cond, const DrivenProcess& driven_proc,
                    const std::string& source = "")
            : datasource("initialspace"), event_id(id), event_name(name), description(desc),
              trigger_condition(trigger_cond), driven_process(driven_proc),
              source_agent(source), is_triggered(false) {}
        
        // ==================== 辅助方法 ====================
        
        /**
         * @brief 获取事件ID的字符串表示
         */
        std::string getEventIdString() const {
            return std::to_string(event_id);
        }
        
        /**
         * @brief 获取控制器信息
         */
        std::string getControllerInfo() const {
            return driven_process.controller_type + "::" + driven_process.controller_name;
        }
    };
        
        // 3）计划事件库数据结构体
        struct PlannedEventLibrary {
            std::string datasource;    ///< 数据来源标识
            std::vector<StandardEvent> planned_events_list;  ///< 预定义事件列表
            mutable std::mutex events_mutex;           ///< 事件库互斥锁
            
            // 默认构造函数
            PlannedEventLibrary() : datasource("initialspace") {}
            
            // 复制构造函数
            PlannedEventLibrary(const PlannedEventLibrary& other) {
                std::lock_guard<std::mutex> lock(other.events_mutex);
                datasource = other.datasource;
                planned_events_list = other.planned_events_list;
            }
            
            // 赋值操作符
            PlannedEventLibrary& operator=(const PlannedEventLibrary& other) {
                if (this != &other) {
                    std::lock_guard<std::mutex> lock1(events_mutex);
                    std::lock_guard<std::mutex> lock2(other.events_mutex);
                    datasource = other.datasource;
                    planned_events_list = other.planned_events_list;
                }
                return *this;
            }
            
            // 添加预定义事件
            void addPlannedEvent(const StandardEvent& event) {
                std::lock_guard<std::mutex> lock(events_mutex);
                planned_events_list.push_back(event);
            }
            
            // 获取预定义事件列表
            std::vector<StandardEvent> getPlannedEvents() const {
                std::lock_guard<std::mutex> lock(events_mutex);
                return planned_events_list;
            }
            
            // 根据ID查找预定义事件
            StandardEvent* findPlannedEvent(const std::string& event_id) {
                std::lock_guard<std::mutex> lock(events_mutex);
                for (auto& event : planned_events_list) {
                    if (event.getEventIdString() == event_id) { // 使用 getEventIdString 进行比较
                        return &event;
                    }
                }
                return nullptr;
            }
            
            // 清空预定义事件列表
            void clearPlannedEvents() {
                std::lock_guard<std::mutex> lock(events_mutex);
                planned_events_list.clear();
            }
            
            // 标记事件为已触发
            bool markEventAsTriggered(const std::string& event_id, double trigger_time) {
                std::lock_guard<std::mutex> lock(events_mutex);
                for (auto& event : planned_events_list  ) {
                    if (event.getEventIdString() == event_id && !event.is_triggered) { // 使用 getEventIdString 进行比较
                        event.is_triggered = true;
                        return true;
                    }
                }
                return false;
            }
            
            /**
             * @brief 示例：创建与飞行计划模板匹配的事件
             * 展示如何使用新的 PlannedEvent 结构体
             */
            void createExampleFlightPlanEvent() {
                // 示例：创建 "taxi_clearance_received" 事件
                // 对应飞行计划模板中的第一个事件
                
                // 1. 创建触发条件
                TriggerCondition trigger_cond(
                    "taxi_clearance_received",  // condition_expression
                    "收到ATC滑行许可"           // description
                );
                
                // 2. 创建驱动过程
                DrivenProcess driven_proc(
                    "Pilot_Manual_Control",     // controller_type
                    "throttle_push2max",        // controller_name
                    "推油门控制",               // description
                    "Throttle = 1"              // termination_condition
                );
                
                // 3. 创建完整的计划事件
                StandardEvent taxi_event(
                    1,                          // event_id (int类型)
                    "taxi_clearance_received",  // event_name
                    "收到滑行许可",             // description
                    trigger_cond,               // trigger_condition
                    driven_proc,                // driven_process
                    "ATC_001"                   // source_agent (扩展字段)
                );
                
                // 4. 添加到事件库
                addPlannedEvent(taxi_event);
                
                VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, 
                    "示例事件已创建: " + taxi_event.event_name + 
                    " (ID: " + taxi_event.getEventIdString() + 
                    ", 控制器: " + taxi_event.getControllerInfo() + ")");
            }
            
            /**
             * @brief 从飞行计划JSON数据创建事件
             * @param event_data JSON格式的事件数据
             */
            void createEventFromFlightPlanData(const std::map<std::string, std::string>& event_data) {
                try {
                    // 解析事件数据
                    int event_id = std::stoi(event_data.at("event_id"));
                    std::string event_name = event_data.at("event_name");
                    std::string description = event_data.at("description");
                    
                    // 解析触发条件
                    TriggerCondition trigger_cond(
                        event_data.at("trigger_condition.condition_expression"),
                        event_data.at("trigger_condition.description")
                    );
                    
                    // 解析驱动过程
                    DrivenProcess driven_proc(
                        event_data.at("driven_process.controller_type"),
                        event_data.at("driven_process.controller_name"),
                        event_data.at("driven_process.description"),
                        event_data.at("driven_process.termination_condition")
                    );
                    
                    // 创建计划事件
                    StandardEvent new_event(
                        event_id, event_name, description, trigger_cond, driven_proc
                    );
                    
                    // 添加到事件库
                    addPlannedEvent(new_event);
                    
                    VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, 
                        "从飞行计划创建事件: " + new_event.event_name + 
                        " (ID: " + new_event.getEventIdString() + ")");
                        
                } catch (const std::exception& e) {
                    VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, 
                        "创建事件失败: " + std::string(e.what()));
                }
            }
        };
       
        // 4）已触发事件库数据结构体
        struct TriggeredEventLibrary {
            std::string datasource;    ///< 数据来源标识
            std::vector<StandardEvent> triggered_events_list;  ///< 已触发事件列表
            std::map<double, std::vector<StandardEvent>> step_events_map;  ///< 按时间步记录的事件映射
            mutable std::mutex events_mutex;           ///< 事件库互斥锁
            
            // 默认构造函数
            TriggeredEventLibrary() : datasource("initialspace") {}
            
            // 复制构造函数
            TriggeredEventLibrary(const TriggeredEventLibrary& other) {
                std::lock_guard<std::mutex> lock(other.events_mutex);
                datasource = other.datasource;
                triggered_events_list = other.triggered_events_list;
                step_events_map = other.step_events_map;  // 拷贝step_events_map
            }
            
            // 赋值操作符
            TriggeredEventLibrary& operator=(const TriggeredEventLibrary& other) {
                if (this != &other) {
                    std::lock_guard<std::mutex> lock1(events_mutex);
                    std::lock_guard<std::mutex> lock2(other.events_mutex);
                    datasource = other.datasource;
                    triggered_events_list = other.triggered_events_list;
                    step_events_map = other.step_events_map;  // 拷贝step_events_map
                }
                return *this;
            }
            
            // 添加已触发事件
            void addTriggeredEvent(const StandardEvent& event) {
                std::lock_guard<std::mutex> lock(events_mutex);
                triggered_events_list.push_back(event);
            }
            
            // 获取已触发事件列表
            std::vector<StandardEvent> getTriggeredEvents() const {
                std::lock_guard<std::mutex> lock(events_mutex);
                return triggered_events_list;
            }
            
            // 根据ID查找已触发事件
            StandardEvent* findTriggeredEvent(const std::string& event_id) {
                std::lock_guard<std::mutex> lock(events_mutex);
                for (auto& event : triggered_events_list) {
                    if (event.getEventIdString() == event_id) {
                        return &event;
                    }
                }
                return nullptr;
            }
            
            // 清空已触发事件列表
            void clearTriggeredEvents() {
                std::lock_guard<std::mutex> lock(events_mutex);
                triggered_events_list.clear();
            }
            
            // 获取已触发事件数量
            size_t getTriggeredEventCount() const {
                std::lock_guard<std::mutex> lock(events_mutex);
                return triggered_events_list.size();
            }
            
            // 按时间步添加事件
            void addEventToStep(double step_time, const StandardEvent& event) {
                std::lock_guard<std::mutex> lock(events_mutex);
                auto& vec = step_events_map[step_time];
                // 去重：同一时间步内按 event_id 去重
                auto same_id = std::find_if(vec.begin(), vec.end(), [&](const StandardEvent& e){
                    return e.event_id == event.event_id;
                });
                if (same_id == vec.end()) {
                    vec.push_back(event);
                }
                // 同一事件只加入一次到全局触发列表
                auto exists_global = std::find_if(triggered_events_list.begin(), triggered_events_list.end(), [&](const StandardEvent& e){
                    return e.event_id == event.event_id;
                });
                if (exists_global == triggered_events_list.end()) {
                    triggered_events_list.push_back(event);
                }
            }
            
            // 获取指定时间步的事件列表
            std::vector<StandardEvent> getEventsAtStep(double step_time) const {
                std::lock_guard<std::mutex> lock(events_mutex);
                auto it = step_events_map.find(step_time);
                if (it != step_events_map.end()) {
                    return it->second;
                }
                return std::vector<StandardEvent>();
            }
            
            // 获取所有时间步的事件映射
            const std::map<double, std::vector<StandardEvent>>& getStepEventsMap() const {
                std::lock_guard<std::mutex> lock(events_mutex);
                return step_events_map;
            }
            
            // 生成事件列表的JSON字符串（用于CSV记录）
            std::string generateEventListString(double step_time) const {
                std::lock_guard<std::mutex> lock(events_mutex);
                auto it = step_events_map.find(step_time);
                if (it == step_events_map.end() || it->second.empty()) {
                    return "[]";
                }
                
                std::stringstream ss;
                ss << "[";
                for (size_t i = 0; i < it->second.size(); ++i) {
                    if (i > 0) ss << ",";
                    const auto& event = it->second[i];
                    ss << "{'id':'" << event.getEventIdString() 
                       << "','name':'" << event.event_name 
                       << "','triggered':" << (event.is_triggered ? "true" : "false") << "}";
                }
                ss << "]";
                return ss.str();
            }
        };

    
        // 5 ）飞行动力学状态数据结构体
        struct AircraftFlightState {
            std::string datasource;    ///< 数据来源标识
            
            // 位置和姿态
            double latitude;           ///< 纬度 (度)
            double longitude;          ///< 经度 (度)
            double altitude;           ///< 高度 (米)
            double heading;            ///< 航向 (度)
            double pitch;              ///< 俯仰角 (度)
            double roll;               ///< 滚转角 (度)
            
            // 速度
            double airspeed;           ///< 空速 (m/s)
            double groundspeed;        ///< 地速 (m/s)
            double vertical_speed;     ///< 垂直速度 (m/s)
            
            // 角速度 (deg/s) - 物理计算所需
            double pitch_rate;         ///< 俯仰角速度
            double roll_rate;          ///< 滚转角速度
            double yaw_rate;           ///< 偏航角速度
            
            // 加速度 (m/s²) - 物理计算所需
            double longitudinal_accel; ///< 纵向加速度
            double lateral_accel;      ///< 横向加速度
            double vertical_accel;     ///< 垂直加速度
            
            // 系统状态
            bool landing_gear_deployed; ///< 起落架是否放下
            bool flaps_deployed;        ///< 襟翼是否放下
            bool spoilers_deployed;     ///< 扰流板是否放下
            double brake_pressure;      ///< 刹车压力 (Pa)
            
            // 性能参数
            double center_of_gravity;   ///< 重心位置 (m)
            double wing_loading;        ///< 翼载 (N/m²)
            
            // 时间戳
            SimulationTimePoint timestamp;
            
            AircraftFlightState() : datasource("initialspace"), latitude(0.0), longitude(0.0), altitude(0.0), heading(0.0),
                                   pitch(0.0), roll(0.0), airspeed(0.0), groundspeed(0.0),
                                   vertical_speed(0.0), pitch_rate(0.0), roll_rate(0.0), yaw_rate(0.0),
                                   longitudinal_accel(0.0), lateral_accel(0.0), vertical_accel(0.0),
                                   landing_gear_deployed(false),
                                   flaps_deployed(false), spoilers_deployed(false), brake_pressure(0.0),
                                   center_of_gravity(0.0), wing_loading(0.0),
                                   timestamp(SimulationTimePoint{}) {}
        };
             
        // 6）飞机系统状态数据结构体
        struct AircraftSystemState {
            std::string datasource;    ///< 数据来源标识
            
            // 系统状态
            double current_mass;        ///< 当前飞机总质量(kg)
            double current_fuel;        ///< 当前燃油量(kg)
            double current_center_of_gravity; ///< 当前重心位置(m)
            double current_brake_pressure;    ///< 当前刹车压力(Pa)
            double current_landing_gear_deployed; ///< 当前起落架偏度(0-1)
            double current_flaps_deployed;        ///< 当前襟翼偏度(0-50)
            double current_spoilers_deployed;     ///< 当前扰流板偏度(0-50)
            double current_aileron_deflection;     ///< 当前副翼偏度(0-50)
            double current_elevator_deflection;    ///< 当前升降舵偏度(0-50)
            double current_rudder_deflection;      ///< 当前方向舵偏度(0-50)
            double current_throttle_position;      ///< 当前油门位置(0-1)
            double current_engine_rpm;             ///< 当前发动机转速(RPM)
            
            // 发动机状态
            bool left_engine_failed;               ///< 左发动机是否失效
            double left_engine_rpm;                ///< 左发动机转速(RPM)
            bool right_engine_failed;              ///< 右发动机是否失效
            double right_engine_rpm;               ///< 右发动机转速(RPM)
            
            // 刹车系统状态
            double brake_efficiency;               ///< 刹车效率 [0.0, 1.0]
    
    
            // 时间戳
            SimulationTimePoint timestamp;
            
            AircraftSystemState() : datasource("initialspace"), current_mass(0.0), current_fuel(0.0),
                                   current_center_of_gravity(0.0), current_brake_pressure(0.0),
                                   current_landing_gear_deployed(0.0), current_flaps_deployed(0.0),
                                   current_spoilers_deployed(0.0), current_engine_rpm(0.0),
                                   left_engine_failed(false), left_engine_rpm(0.0),
                                   right_engine_failed(false), right_engine_rpm(0.0),
                                   brake_efficiency(1.0), timestamp(SimulationTimePoint{}) {}
        };
        
        // 7）飞行员全局状态数据
       struct PilotGlobalState {
        std::string datasource;    ///< 数据来源标识
        double attention_level;     ///< 注意力水平 [0.0, 1.0]     
        // 技能状态
        double skill_level;         ///< 技能水平 [0.0, 1.0]                
        // 时间戳
        SimulationTimePoint timestamp;        
        PilotGlobalState() : datasource("initialspace"), attention_level(1.0), skill_level(0.88),
                            timestamp(SimulationTimePoint{}) {}
    };

        // 8）六分量合外力数据结构体
        struct AircraftNetForce {
            std::string datasource;    ///< 数据来源标识
            
            // 线性力分量 (N)
            double longitudinal_force;    ///< 纵向力 (推力-阻力)
            double lateral_force;         ///< 横向力 (侧力)
            double vertical_force;        ///< 垂直力 (升力-重力)
            
            // 力矩分量 (N·m)
            double roll_moment;           ///< 滚转力矩
            double pitch_moment;          ///< 俯仰力矩
            double yaw_moment;            ///< 偏航力矩
            
            // 力的分解
            double thrust_force;          ///< 推力 (N)
            double drag_force;            ///< 阻力 (N)
            double lift_force;            ///< 升力 (N)
            double weight_force;          ///< 重力 (N)
            double side_force;            ///< 侧力 (N)
            
            // 时间戳
            SimulationTimePoint timestamp;
            
            AircraftNetForce() : datasource("initialspace"), longitudinal_force(0.0), lateral_force(0.0), vertical_force(0.0),
                                roll_moment(0.0), pitch_moment(0.0), yaw_moment(0.0),
                                thrust_force(0.0), drag_force(0.0), lift_force(0.0),
                                weight_force(0.0), side_force(0.0),
                                timestamp(SimulationTimePoint{}) {}
        };

        // 9）环境状态数据结构体
        struct EnvironmentGlobalState {
            std::string datasource;    ///< 数据来源标识
            
            double runway_length;                ///< 跑道长度 (米)
            double runway_width;                 ///< 跑道宽度 (米)
            double friction_coefficient;         ///< 摩擦系数 [0.0, 1.0]
            double air_density;                  ///< 空气密度 (kg/m³)
        
            double wind_speed;                   ///< 风速 (m/s)
            double wind_direction;               ///< 风向 (度，0-360)
        
            SimulationTimePoint timestamp;       ///< 数据时间戳
        
        EnvironmentGlobalState() : datasource("initialspace"), runway_length(0.0), runway_width(0.0), 
                                  friction_coefficient(0.0), air_density(1.225),
                                  wind_speed(0.0), wind_direction(0.0), 
                                  timestamp(SimulationTimePoint{}) {}
    };

        // 9）ATC全局状态数据
        struct ATCGlobalState {
            std::string datasource;    ///< 数据来源标识
            
            double controller_workload; ///< 管制员工作负荷 [0.0, 1.0]
            double controller_attention; ///< 管制员注意力 [0.0, 1.0]
            int active_aircraft_count;  ///< 活动飞机数量
            int pending_commands;       ///< 待处理指令数量
        
            double airspace_congestion; ///< 空域拥挤度 [0.0, 1.0]
            int conflict_count;         ///< 当前冲突数量
            double separation_violations; ///< 间隔违规次数
        
            double communication_load;  ///< 通信负荷 [0.0, 1.0]
            int active_frequencies;     ///< 活动频率数量
            double response_time;       ///< 平均响应时间 (s)
        
            bool radar_operational;     ///< 雷达是否工作
            bool communication_system_operational; ///< 通信系统是否工作
            std::string current_phase;  ///< 当前管制阶段
        
            SimulationTimePoint timestamp;
        
        ATCGlobalState() : datasource("initialspace"), controller_workload(0.3), controller_attention(1.0),
                          active_aircraft_count(0), pending_commands(0),
                          airspace_congestion(0.2), conflict_count(0),
                          separation_violations(0.0), communication_load(0.2),
                          active_frequencies(1), response_time(2.0),
                          radar_operational(true), communication_system_operational(true),
                          current_phase("正常"), timestamp(SimulationTimePoint{}) {}
    };

        // 10）飞机全局逻辑数据
        struct AircraftGlobalLogic {
        std::string datasource;    ///< 数据来源标识
        
        // 飞行计划
        std::string flight_plan_id;     ///< 飞行计划ID
        std::string departure_airport;  ///< 出发机场
        std::string arrival_airport;    ///< 到达机场
        std::vector<std::pair<double, double>> waypoints; ///< 航路点
        double planned_altitude;        ///< 计划高度 (m)
        double planned_speed;           ///< 计划速度 (m/s)
        
        // 飞行阶段
        std::string current_phase;      ///< 当前飞行阶段
        std::string next_phase;         ///< 下一飞行阶段
        double phase_progress;          ///< 阶段完成进度 [0.0, 1.0]
        
        // 系统逻辑
        bool autopilot_engaged;         ///< 自动驾驶是否接通
        std::string autopilot_mode;     ///< 自动驾驶模式
        bool auto_throttle_engaged;     ///< 自动油门是否接通
        std::string navigation_mode;    ///< 导航模式
        
        // 性能逻辑
        double performance_index;       ///< 性能指数
        double fuel_efficiency;         ///< 燃油效率
        double optimal_speed;           ///< 最优速度 (m/s)
        double optimal_altitude;        ///< 最优高度 (m)
        
        // 时间戳
        SimulationTimePoint timestamp;
        
        AircraftGlobalLogic() : datasource("initialspace"), flight_plan_id(""), departure_airport(""), arrival_airport(""),
                               planned_altitude(0.0), planned_speed(0.0), current_phase("地面"),
                               next_phase(""), phase_progress(0.0), autopilot_engaged(false),
                               autopilot_mode(""), auto_throttle_engaged(false), navigation_mode(""),
                               performance_index(1.0), fuel_efficiency(1.0), optimal_speed(0.0),
                               optimal_altitude(0.0), timestamp(SimulationTimePoint{}) {}
    };

        // 11）飞行员全局逻辑数据
        struct PilotGlobalLogic {
        std::string datasource;    ///< 数据来源标识
        
        // 决策逻辑
        std::string decision_strategy;  ///< 决策策略
        double risk_tolerance;          ///< 风险容忍度 [0.0, 1.0]
        std::string priority_task;      ///< 优先任务
        std::vector<std::string> task_queue; ///< 任务队列
        
        // 认知逻辑
        std::string attention_focus;    ///< 注意力焦点
        std::string mental_model;       ///< 心智模型
        double situation_awareness;     ///< 情境感知 [0.0, 1.0]
        
        // 行为逻辑
        std::string behavior_pattern;   ///< 行为模式
        double adaptability;            ///< 适应性 [0.0, 1.0]
        std::string communication_style; ///< 通信风格
        
        // 学习逻辑
        double learning_rate;           ///< 学习速率 [0.0, 1.0]
        std::vector<std::string> learned_procedures; ///< 已学习程序
        double performance_improvement; ///< 性能改进 [0.0, 1.0]
        
        // 时间戳
        SimulationTimePoint timestamp;
        
        PilotGlobalLogic() : datasource("initialspace"), decision_strategy("保守"), risk_tolerance(0.3), priority_task(""),
                            attention_focus("主要飞行仪表"), mental_model("标准程序"),
                            situation_awareness(0.83), behavior_pattern("程序化"),
                            adaptability(0.7), communication_style("标准"),
                            learning_rate(0.5), performance_improvement(0.0),
                            timestamp(SimulationTimePoint{}) {}
    };

        // 12）环境全局逻辑数据
        struct EnvironmentGlobalLogic {
        std::string datasource;    ///< 数据来源标识
        
        // 天气逻辑
        std::string weather_pattern;    ///< 天气模式
        double weather_severity;        ///< 天气严重程度 [0.0, 1.0]
        std::string weather_trend;      ///< 天气趋势
        std::vector<std::string> weather_warnings; ///< 天气警告
        
        // 地形逻辑
        std::string terrain_complexity; ///< 地形复杂度
        double terrain_risk_level;      ///< 地形风险等级 [0.0, 1.0]
        std::vector<std::string> terrain_hazards; ///< 地形危险
        
        // 空域逻辑
        std::string airspace_class;     ///< 空域等级
        double airspace_restrictions;   ///< 空域限制 [0.0, 1.0]
        std::vector<std::string> restricted_areas; ///< 限制区域
        
        // 时间逻辑
        std::string time_of_day;        ///< 一天中的时间
        std::string season;             ///< 季节
        double daylight_availability;   ///< 日光可用性 [0.0, 1.0]
        
        // 时间戳
        SimulationTimePoint timestamp;
        
        EnvironmentGlobalLogic() : datasource("initialspace"), weather_pattern("晴朗"), weather_severity(0.1),
                                  weather_trend("稳定"), terrain_complexity("简单"),
                                  terrain_risk_level(0.1), airspace_class("G"),
                                  airspace_restrictions(0.0), time_of_day("白天"),
                                  season("春季"), daylight_availability(1.0),
                                  timestamp(SimulationTimePoint{}) {}
    };

        // 13）ATC全局逻辑数据
        struct ATCGlobalLogic {
        std::string datasource;    ///< 数据来源标识
        
        // 管制策略
        std::string control_strategy;   ///< 管制策略
        double separation_standards;    ///< 间隔标准 (m)
        std::string traffic_flow_management; ///< 交通流管理
        std::vector<std::string> control_procedures; ///< 管制程序
        
        // 冲突管理
        std::string conflict_resolution_strategy; ///< 冲突解决策略
        double conflict_detection_threshold; ///< 冲突检测阈值
        std::vector<std::string> resolution_procedures; ///< 解决程序
        
        // 通信逻辑
        std::string communication_protocol; ///< 通信协议
        double communication_priority; ///< 通信优先级 [0.0, 1.0]
        std::vector<std::string> communication_channels; ///< 通信频道
        
        // 系统逻辑
        std::string system_mode;        ///< 系统模式
        double automation_level;        ///< 自动化水平 [0.0, 1.0]
        std::vector<std::string> system_procedures; ///< 系统程序
        
        // 时间戳
        SimulationTimePoint timestamp;
        
        ATCGlobalLogic() : datasource("initialspace"), control_strategy("标准"), separation_standards(1000.0),
                          traffic_flow_management("正常"), conflict_resolution_strategy("标准"),
                          conflict_detection_threshold(500.0), communication_protocol("标准"),
                          communication_priority(0.5), system_mode("正常"),
                          automation_level(0.7), timestamp(SimulationTimePoint{}) {}
    };

        // 14）ATC指令数据结构体
        struct ATC_Command {
            std::string datasource;    ///< 数据来源标识
            bool clearance_granted;    ///< 是否放行
            bool emergency_brake;      ///< 是否紧急刹车
            
            // 时间戳
            SimulationTimePoint timestamp;
            
            ATC_Command() : datasource("initialspace"), clearance_granted(false), emergency_brake(false), 
                          timestamp(SimulationTimePoint{}) {}
        };

        // 15）计划控制器数据结构体
        struct PlanedController {
            std::string datasource;    ///< 数据来源标识
            std::string event_id;      ///< 关联的事件ID
            std::string event_name;    ///< 事件名称
            std::string controller_type; ///< 控制器类型
            std::string controller_name; ///< 控制器名称
            std::string description;   ///< 控制器描述
            std::string termination_condition; ///< 终止条件
            std::map<std::string, std::string> controller_parameters; ///< 控制器参数
            
            // 时间戳
            SimulationTimePoint timestamp;
            
            PlanedController() : datasource("initialspace"), event_id(""), event_name(""), 
                               controller_type(""), controller_name(""), description(""),
                               termination_condition(""), timestamp(SimulationTimePoint{}) {}
        };

        // 16）计划控制器库数据结构体
        struct PlanedControllersLibrary {
            std::string datasource;    ///< 数据来源标识
            std::vector<PlanedController> controllers; ///< 控制器列表
            std::map<std::string, PlanedController> controller_map; ///< 控制器映射表（按事件ID）
            
            // 时间戳
            SimulationTimePoint timestamp;
            
            PlanedControllersLibrary() : datasource("initialspace"), timestamp(SimulationTimePoint{}) {}
            
            // 添加控制器
            void addController(const PlanedController& controller) {
                controllers.push_back(controller);
                controller_map[controller.event_id] = controller;
            }
            
            // 根据事件ID获取控制器
            PlanedController* getControllerByEventId(const std::string& event_id) {
                auto it = controller_map.find(event_id);
                if (it != controller_map.end()) {
                    return &(it->second);
                }
                return nullptr;
            }
            
            // 获取所有控制器
            const std::vector<PlanedController>& getAllControllers() const {
                return controllers;
            }
            
            // 清空控制器库
            void clear() {
                controllers.clear();
                controller_map.clear();
            }
        };

        // 17）事件队列项结构体
        struct EventQueueItem {
            StandardEvent event;              ///< 事件对象
            double trigger_time;              ///< 触发时间
            bool is_processed;                ///< 是否已处理
            std::string datasource;           ///< 数据来源
            SimulationTimePoint timestamp;    ///< 时间戳

            EventQueueItem() : trigger_time(0.0), is_processed(false), 
                              datasource("initialspace"), timestamp(SimulationTimePoint{}) {}

            EventQueueItem(const StandardEvent& evt, double time, const std::string& source = "event_monitor")
                : event(evt), trigger_time(time), is_processed(false), 
                  datasource(source), timestamp(SimulationTimePoint{}) {}
        };

        // 18）事件队列数据结构体（单缓冲区实现）
        struct EventQueue {
            static const size_t MAX_QUEUE_SIZE = 1000;        ///< 最大队列容量
            
            std::string datasource;                           ///< 数据来源标识
            std::vector<EventQueueItem> event_buffer;         ///< 事件缓冲区数组
            size_t head_index;                                ///< 队列头索引（下一个要处理的事件）
            size_t tail_index;                                ///< 队列尾索引（下一个要插入的位置）
            size_t current_size;                              ///< 当前队列大小
            std::vector<EventQueueItem> processed_events;     ///< 已处理事件列表
            mutable std::mutex queue_mutex;                   ///< 队列互斥锁
            SimulationTimePoint timestamp;                    ///< 时间戳

            EventQueue() : datasource("initialspace"), head_index(0), tail_index(0), current_size(0), timestamp(SimulationTimePoint{}) {
                event_buffer.resize(MAX_QUEUE_SIZE);
            }

            // 复制构造函数
            EventQueue(const EventQueue& other) {
                std::lock_guard<std::mutex> lock(other.queue_mutex);
                datasource = other.datasource;
                event_buffer = other.event_buffer;
                head_index = other.head_index;
                tail_index = other.tail_index;
                current_size = other.current_size;
                processed_events = other.processed_events;
                timestamp = other.timestamp;
            }

            // 赋值操作符
            EventQueue& operator=(const EventQueue& other) {
                if (this != &other) {
                    std::lock_guard<std::mutex> lock1(queue_mutex);
                    std::lock_guard<std::mutex> lock2(other.queue_mutex);
                    datasource = other.datasource;
                    event_buffer = other.event_buffer;
                    head_index = other.head_index;
                    tail_index = other.tail_index;
                    current_size = other.current_size;
                    processed_events = other.processed_events;
                    timestamp = other.timestamp;
                }
                return *this;
            }

            // 添加事件到队列
            void enqueueEvent(const StandardEvent& event, double trigger_time, const std::string& source = "event_monitor") {
                std::lock_guard<std::mutex> lock(queue_mutex);
                if (current_size >= MAX_QUEUE_SIZE) {
                    // 队列满了，覆盖最旧的事件（环形缓冲区行为）
                    head_index = (head_index + 1) % MAX_QUEUE_SIZE;
                    current_size--;
                }
                
                EventQueueItem item(event, trigger_time, source);
                event_buffer[tail_index] = item;
                tail_index = (tail_index + 1) % MAX_QUEUE_SIZE;
                current_size++;
            }

            // 从队列中取出下一个待处理事件
            bool dequeueEvent(EventQueueItem& item) {
                std::lock_guard<std::mutex> lock(queue_mutex);
                if (current_size == 0) {
                    return false;
                }
                
                item = event_buffer[head_index];
                head_index = (head_index + 1) % MAX_QUEUE_SIZE;
                current_size--;
                return true;
            }

            // 标记事件为已处理
            void markEventAsProcessed(const EventQueueItem& item) {
                std::lock_guard<std::mutex> lock(queue_mutex);
                EventQueueItem processed_item = item;
                processed_item.is_processed = true;
                processed_events.push_back(processed_item);
            }

            // 获取队列大小
            size_t getQueueSize() const {
                std::lock_guard<std::mutex> lock(queue_mutex);
                return current_size;
            }

            // 获取已处理事件数量
            size_t getProcessedCount() const {
                std::lock_guard<std::mutex> lock(queue_mutex);
                return processed_events.size();
            }

            // 检查队列是否为空
            bool isEmpty() const {
                std::lock_guard<std::mutex> lock(queue_mutex);
                return current_size == 0;
            }

            // 清空队列
            void clear() {
                std::lock_guard<std::mutex> lock(queue_mutex);
                head_index = 0;
                tail_index = 0;
                current_size = 0;
                processed_events.clear();
            }

            // 获取所有待处理事件（用于调试）
            std::vector<EventQueueItem> getPendingEvents() const {
                std::lock_guard<std::mutex> lock(queue_mutex);
                std::vector<EventQueueItem> events;
                for (size_t i = 0; i < current_size; ++i) {
                    size_t index = (head_index + i) % MAX_QUEUE_SIZE;
                    events.push_back(event_buffer[index]);
                }
                return events;
            }

            // 获取所有已处理事件
            const std::vector<EventQueueItem>& getProcessedEvents() const {
                std::lock_guard<std::mutex> lock(queue_mutex);
                return processed_events;
            }
        };

        // 19）代理事件队列项结构体
        struct AgentEventQueueItem {
            StandardEvent event;              ///< 事件对象
            double trigger_time;              ///< 触发时间
            std::string controller_type;      ///< 控制器类型
            std::string controller_name;      ///< 控制器名称
            std::map<std::string, std::string> parameters; ///< 控制器参数
            bool is_processed;                ///< 是否已处理
            std::string datasource;           ///< 数据来源
            SimulationTimePoint timestamp;    ///< 时间戳

            AgentEventQueueItem() : trigger_time(0.0), is_processed(false), 
                                   datasource("controller_manager"), timestamp(SimulationTimePoint{}) {}

            AgentEventQueueItem(const StandardEvent& evt, double time, 
                               const std::string& ctrl_type, const std::string& ctrl_name,
                               const std::map<std::string, std::string>& params = {},
                               const std::string& source = "controller_manager")
                : event(evt), trigger_time(time), controller_type(ctrl_type), controller_name(ctrl_name),
                  parameters(params), is_processed(false), datasource(source), timestamp(SimulationTimePoint{}) {}
        };

        // 20）代理事件队列数据结构体
        struct AgentEventQueue {
            static const size_t MAX_AGENT_QUEUE_SIZE = 500;   ///< 最大代理队列容量
            
            std::string agent_id;                             ///< 代理ID
            std::string datasource;                           ///< 数据来源标识
            std::vector<AgentEventQueueItem> event_buffer;    ///< 事件缓冲区数组
            size_t head_index;                                ///< 队列头索引
            size_t tail_index;                                ///< 队列尾索引
            size_t current_size;                              ///< 当前队列大小
            std::vector<AgentEventQueueItem> processed_events; ///< 已处理事件列表
            mutable std::mutex queue_mutex;                   ///< 队列互斥锁
            SimulationTimePoint timestamp;                    ///< 时间戳

            AgentEventQueue() : agent_id(""), datasource("controller_manager"), 
                               head_index(0), tail_index(0), current_size(0), timestamp(SimulationTimePoint{}) {
                event_buffer.resize(MAX_AGENT_QUEUE_SIZE);
            }

            AgentEventQueue(const std::string& agent) : agent_id(agent), datasource("controller_manager"),
                                                       head_index(0), tail_index(0), current_size(0), timestamp(SimulationTimePoint{}) {
                event_buffer.resize(MAX_AGENT_QUEUE_SIZE);
            }

            // 添加事件到代理队列
            void enqueueEvent(const StandardEvent& event, double trigger_time,
                             const std::string& ctrl_type, const std::string& ctrl_name,
                             const std::map<std::string, std::string>& params = {},
                             const std::string& source = "controller_manager") {
                std::lock_guard<std::mutex> lock(queue_mutex);
                if (current_size >= MAX_AGENT_QUEUE_SIZE) {
                    // 队列满了，覆盖最旧的事件
                    head_index = (head_index + 1) % MAX_AGENT_QUEUE_SIZE;
                    current_size--;
                }
                
                AgentEventQueueItem item(event, trigger_time, ctrl_type, ctrl_name, params, source);
                event_buffer[tail_index] = item;
                tail_index = (tail_index + 1) % MAX_AGENT_QUEUE_SIZE;
                current_size++;
            }

            // 从代理队列中取出下一个待处理事件
            bool dequeueEvent(AgentEventQueueItem& item) {
                std::lock_guard<std::mutex> lock(queue_mutex);
                if (current_size == 0) {
                    return false;
                }
                
                item = event_buffer[head_index];
                head_index = (head_index + 1) % MAX_AGENT_QUEUE_SIZE;
                current_size--;
                return true;
            }

            // 标记事件为已处理
            void markEventAsProcessed(const AgentEventQueueItem& item) {
                std::lock_guard<std::mutex> lock(queue_mutex);
                AgentEventQueueItem processed_item = item;
                processed_item.is_processed = true;
                processed_events.push_back(processed_item);
            }

            // 获取队列大小
            size_t getQueueSize() const {
                std::lock_guard<std::mutex> lock(queue_mutex);
                return current_size;
            }

            // 检查队列是否为空
            bool isEmpty() const {
                std::lock_guard<std::mutex> lock(queue_mutex);
                return current_size == 0;
            }

            // 清空队列
            void clear() {
                std::lock_guard<std::mutex> lock(queue_mutex);
                head_index = 0;
                tail_index = 0;
                current_size = 0;
                processed_events.clear();
            }
        };

        // 21）代理事件队列管理器结构体
        struct AgentEventQueueManager {
            std::map<std::string, AgentEventQueue> agent_queues; ///< 各代理的事件队列
            mutable std::mutex manager_mutex;                    ///< 管理器互斥锁
            SimulationTimePoint timestamp;                       ///< 时间戳

            AgentEventQueueManager() : timestamp(SimulationTimePoint{}) {}

            // 为代理创建事件队列
            void createAgentQueue(const std::string& agent_id) {
                std::lock_guard<std::mutex> lock(manager_mutex);
                if (agent_queues.find(agent_id) == agent_queues.end()) {
                    agent_queues.emplace(
                        std::piecewise_construct,
                        std::forward_as_tuple(agent_id),
                        std::forward_as_tuple(agent_id)
                    );
                }
            }

            // 向指定代理队列添加事件
            void enqueueAgentEvent(const std::string& agent_id, const StandardEvent& event, 
                                  double trigger_time, const std::string& ctrl_type, 
                                  const std::string& ctrl_name,
                                  const std::map<std::string, std::string>& params = {}) {
                std::lock_guard<std::mutex> lock(manager_mutex);
                auto it = agent_queues.find(agent_id);
                if (it != agent_queues.end()) {
                    it->second.enqueueEvent(event, trigger_time, ctrl_type, ctrl_name, params);
                }
            }

            // 从指定代理队列取出事件
            bool dequeueAgentEvent(const std::string& agent_id, AgentEventQueueItem& item) {
                std::lock_guard<std::mutex> lock(manager_mutex);
                auto it = agent_queues.find(agent_id);
                if (it != agent_queues.end()) {
                    return it->second.dequeueEvent(item);
                }
                return false;
            }

            // 获取指定代理队列大小
            size_t getAgentQueueSize(const std::string& agent_id) const {
                std::lock_guard<std::mutex> lock(manager_mutex);
                auto it = agent_queues.find(agent_id);
                if (it != agent_queues.end()) {
                    return it->second.getQueueSize();
                }
                return 0;
            }

            // 检查指定代理队列是否为空
            bool isAgentQueueEmpty(const std::string& agent_id) const {
                std::lock_guard<std::mutex> lock(manager_mutex);
                auto it = agent_queues.find(agent_id);
                if (it != agent_queues.end()) {
                    return it->second.isEmpty();
                }
                return true;
            }

            // 获取所有代理ID列表
            std::vector<std::string> getAgentIds() const {
                std::lock_guard<std::mutex> lock(manager_mutex);
                std::vector<std::string> agent_ids;
                for (const auto& pair : agent_queues) {
                    agent_ids.push_back(pair.first);
                }
                return agent_ids;
            }
        };

        // 22）控制器执行状态跟踪数据结构体
        struct ControllerExecutionStatus {
            std::string datasource;    ///< 数据来源标识
            std::map<std::string, bool> controller_status; ///< 控制器名称到运行状态的映射
            SimulationTimePoint timestamp;                 ///< 时间戳
            
            ControllerExecutionStatus() : datasource("initialspace"), timestamp(SimulationTimePoint{}) {}
            
            // 设置控制器状态
            void setControllerStatus(const std::string& controller_name, bool is_running) {
                controller_status[controller_name] = is_running;
            }
            
            // 获取控制器状态
            bool getControllerStatus(const std::string& controller_name) const {
                auto it = controller_status.find(controller_name);
                return (it != controller_status.end()) ? it->second : false;
            }
            
            // 获取所有控制器名称
            std::vector<std::string> getAllControllerNames() const {
                std::vector<std::string> names;
                for (const auto& pair : controller_status) {
                    names.push_back(pair.first);
                }
                return names;
            }
            
            // 获取运行中的控制器数量
            size_t getRunningControllerCount() const {
                size_t count = 0;
                for (const auto& pair : controller_status) {
                    if (pair.second) {
                        count++;
                    }
                }
                return count;
            }
            
            // 清空状态
            void clear() {
                controller_status.clear();
            }
        };

        // 23）控制优先级管理数据结构体
        enum class ControlPriority {
            EMERGENCY = 0,        ///< 紧急控制（最高优先级）
            MANUAL_OVERRIDE = 1,  ///< 手动超控
            AUTOPILOT = 2,        ///< 自动驾驶仪
            AUTOTHROTTLE = 3,     ///< 自动油门
            FLIGHT_DIRECTOR = 4,  ///< 飞行指引
            STABILITY_AUG = 5,    ///< 增稳系统
            MANUAL = 6,           ///< 手动控制
            SYSTEM_DEFAULT = 7    ///< 系统默认（最低优先级）
        };

        struct ControlCommand {
            std::string source;           ///< 控制源标识
            ControlPriority priority;     ///< 控制优先级
            double throttle_command;      ///< 油门指令 [0.0, 1.0]
            double elevator_command;      ///< 升降舵指令 [-1.0, 1.0]
            double aileron_command;       ///< 副翼指令 [-1.0, 1.0]
            double rudder_command;        ///< 方向舵指令 [-1.0, 1.0]
            double brake_command;          ///< 刹车指令 [0.0, 1.0]
            SimulationTimePoint timestamp; ///< 时间戳
            bool active;                   ///< 是否激活
            
            ControlCommand() : source(""), priority(ControlPriority::SYSTEM_DEFAULT), 
                              throttle_command(0.0), elevator_command(0.0), 
                              aileron_command(0.0), rudder_command(0.0), 
                              brake_command(0.0), timestamp(SimulationTimePoint{}), active(false) {}
        };

        struct ControlPriorityManager {
            std::map<ControlPriority, ControlCommand> active_commands; ///< 各优先级的激活指令
            ControlCommand final_command;                              ///< 最终执行指令
            SimulationTimePoint last_update;                          ///< 最后更新时间
            
            ControlPriorityManager() : last_update(SimulationTimePoint{}) {}
            
            // 设置控制指令
            void setControlCommand(const ControlCommand& command) {
                active_commands[command.priority] = command;
                last_update = command.timestamp;
            }
            
            // 清除指定优先级的指令
            void clearControlCommand(ControlPriority priority) {
                active_commands.erase(priority);
            }
            
            // 计算最终控制指令（基于优先级）
            ControlCommand calculateFinalCommand() const {
                ControlCommand result = ControlCommand(); // 重置
                
                // 按优先级从高到低选择激活的指令
                for (int priority = static_cast<int>(ControlPriority::EMERGENCY); 
                     priority <= static_cast<int>(ControlPriority::SYSTEM_DEFAULT); ++priority) {
                    auto it = active_commands.find(static_cast<ControlPriority>(priority));
                    if (it != active_commands.end() && it->second.active) {
                        result = it->second;
                        break; // 找到最高优先级的激活指令
                    }
                }
                
                return result;
            }
            
            // 获取当前激活的控制源
            std::string getActiveControlSource() const {
                return final_command.source;
            }
            
            // 检查是否有手动超控
            bool hasManualOverride() const {
                auto it = active_commands.find(ControlPriority::MANUAL_OVERRIDE);
                return (it != active_commands.end() && it->second.active);
            }
            
            // 清空所有指令
            void clearAllCommands() {
                active_commands.clear();
                final_command = ControlCommand();
            }
        };
    }
}
