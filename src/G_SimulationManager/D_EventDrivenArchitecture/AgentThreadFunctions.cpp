/**
 * @file AgentThreadFunctions.cpp
 * @brief 代理线程函数实现 - 使用Simulation_Clock版本
 * @author VFT_SMF Development Team
 * @date 2024
 */

#include "AgentThreadFunctions.hpp"
#include "EventDispatcher.hpp"

#include "../../A_PilotAgentModel/Pilot_001/ServiceTwin/PilotATCCommandHandler.hpp"
#include "../../A_PilotAgentModel/Pilot_001/ServiceTwin/PilotManualControlHandler.hpp"
#include "../../F_ScenarioModelling/A_FlightPlanParser/FlightPlanParser.hpp"
#include "../../G_SimulationManager/B_SimManage/EventMonitor.hpp"
#include "../../D_ATCAgentModel/A_StandardBase/ATCAgent.hpp"
#include <algorithm>
#include <unordered_set>
#include <sstream>
#include <fstream>
#include "../../src/I_ThirdPartyTools/json.hpp"

namespace VFT_SMF {

// ==================== 全局变量定义 ====================

// 全局同步变量
std::atomic<bool> simulation_running{false};
std::atomic<bool> environment_thread_ready{false};
std::atomic<bool> data_space_thread_ready{false};
std::atomic<bool> flight_dynamics_thread_ready{false};
std::atomic<bool> aircraft_system_thread_ready{false};
std::atomic<bool> event_monitor_thread_ready{false};
std::atomic<bool> event_dispatcher_thread_ready{false};
std::atomic<bool> pilot_thread_ready{false};
std::atomic<bool> atc_thread_ready{false};

// ==================== 辅助函数实现 ====================

void wait_for_environment_thread_ready() {
    while (!environment_thread_ready) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    logBrief(LogLevel::Brief, "环境线程已就绪");
}

void wait_for_data_space_thread_ready() {
    while (!data_space_thread_ready) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    logBrief(LogLevel::Brief, "数据共享空间线程已就绪");
}

void wait_for_flight_dynamics_thread_ready() {
    while (!flight_dynamics_thread_ready) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    logBrief(LogLevel::Brief, "飞行动力学线程已就绪");
}

void wait_for_aircraft_system_thread_ready() {
    while (!aircraft_system_thread_ready) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    logBrief(LogLevel::Brief, "飞行器系统线程已就绪");
}

void wait_for_event_monitor_thread_ready() {
    while (!event_monitor_thread_ready) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    logBrief(LogLevel::Brief, "事件监测线程已就绪");
}

void wait_for_event_dispatcher_thread_ready() {
    while (!event_dispatcher_thread_ready) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    logBrief(LogLevel::Brief, "事件分发线程已就绪");
}

void wait_for_pilot_thread_ready() {
    while (!pilot_thread_ready) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    logBrief(LogLevel::Brief, "飞行员线程已就绪");
}

void wait_for_atc_thread_ready() {
    while (!atc_thread_ready) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    logBrief(LogLevel::Brief, "ATC线程已就绪");
}

// ==================== 线程函数实现 ====================
// 1. 环境线程函数
void environment_thread_function(std::shared_ptr<GlobalShared_DataSpace::GlobalSharedDataSpace> shared_data_space) {
    
    logBrief(LogLevel::Brief, "环境线程启动");
    
    // 环境线程自己实现注册到时钟的功能
    const std::string thread_id = "ENV_THREAD_001";
    const std::string thread_name = "Environment_Thread";
    const std::string thread_type = "Environment";
    
    if (!shared_data_space->registerThread(thread_id, thread_name, thread_type)) {
        logBrief(LogLevel::Brief, "环境线程注册失败");
        return;
    }
    
    logBrief(LogLevel::Brief, "环境线程注册成功");
    
    // 从配置文件读取环境模型名称
    std::string environment_name = "PEK_Runway_02"; // 默认值
    
    logBrief(LogLevel::Brief, "环境线程: 开始读取配置文件");
    
    // 从配置文件读取环境模型名称
    try {
        std::ifstream file("input/FlightPlan.json");
        if (file.is_open()) {
            nlohmann::json flight_plan;
            file >> flight_plan;
            file.close();
            
            if (flight_plan.contains("flight_plan") && 
                flight_plan["flight_plan"].contains("scenario_config") &&
                flight_plan["flight_plan"]["scenario_config"].contains("Environment_Name")) {
                environment_name = flight_plan["flight_plan"]["scenario_config"]["Environment_Name"];
                logBrief(LogLevel::Brief, "从配置文件读取环境模型名称: " + environment_name);
            } else {
                logBrief(LogLevel::Brief, "配置文件中未找到Environment_Name字段，使用默认值: " + environment_name);
            }
        } else {
            logBrief(LogLevel::Brief, "无法打开配置文件，使用默认值: " + environment_name);
        }
    } catch (const std::exception& e) {
        logBrief(LogLevel::Brief, "读取环境模型配置失败，使用默认值: " + environment_name + "，错误: " + e.what());
    }
    
    // 创建环境代理
    VFT_SMF::EnvirDataSpace::EnvironmentAgentConfig env_config;
    env_config.environment_model_name = environment_name;
    env_config.airport_code = "PEK";
    env_config.runway_code = "02";
    env_config.weather_code = "CAVOK";
    
    VFT_SMF::EnvironmentAgent environment_agent("ENV_001", "Environment_Agent_001", env_config, VFT_SMF::EnvironmentType::AIRPORT_RUNWAY);
    
    // 设置全局共享数据空间
    environment_agent.set_global_data_space(shared_data_space);
    
    // 初始化环境模型（配置驱动）
    environment_agent.initializeEnvironmentModel(environment_name);
    
    // 启动环境代理
    environment_agent.start();
    
    // 环境代理初始化后立即运行一次更新，计算出基于初始状态的动态数据并覆盖共享数据空间
    environment_agent.update(0.0); // 运行一次初始更新
    
    logBrief(LogLevel::Brief, "环境代理创建完成并已启动，初始状态已计算并更新到共享数据空间");
    
    // 设置线程就绪状态
    environment_thread_ready = true;
    logBrief(LogLevel::Brief, "环境代理已创建并启动");
    
    double last_update_time = 0.0; // 记录上次更新时间
    
    // 环境线程主循环 - 订阅时钟通知
    logBrief(LogLevel::Brief, "环境线程进入主循环");
    static uint64_t env_last_step = std::numeric_limits<uint64_t>::max();
    while (!shared_data_space->isSimulationOver()) {
        // 设置状态为等待时钟信号（降噪：不再逐步输出Brief）
        shared_data_space->updateThreadState(thread_id, VFT_SMF::GlobalSharedDataStruct::ThreadSyncState::WAITING_FOR_CLOCK);
        
        // 轮询等待新步（单一谓词：沿触发）
        auto sync_signal = shared_data_space->getCurrentSyncSignal();
        while (!(sync_signal.step_ready && sync_signal.current_step != env_last_step)) {
            if (shared_data_space->isSimulationOver()) {
                logBrief(LogLevel::Brief, "环境线程检测到仿真结束标志，退出等待");
                goto env_thread_exit;
            }
            std::this_thread::sleep_for(std::chrono::microseconds(150));
            sync_signal = shared_data_space->getCurrentSyncSignal();
        }
        
        // 收到时钟通知，设置状态为运行（降噪：不再逐步输出Brief）
        shared_data_space->updateThreadState(thread_id, VFT_SMF::GlobalSharedDataStruct::ThreadSyncState::RUNNING);
        
        // 获取当前步与时间（基于步号计算时间，避免浮点累计误差）
        const uint64_t step = sync_signal.current_step;
        env_last_step = step;
        const double current_time = static_cast<double>(step) * 0.01;
            
        // 环境线程更新
        environment_agent.update(0.01); // 固定时间步长
        last_update_time = current_time; // 更新last_update_time
        
        // 减少日志输出频率，只在每50步输出一次
        static int env_log_counter = 0;
        env_log_counter++;
        if (env_log_counter % 50 == 0) {
            logBrief(LogLevel::Brief, "环境线程更新 - 仿真时间: " + std::to_string(current_time) + "s, 步骤: " + std::to_string(step));
        }
        
        // 完成当前步骤的工作，设置状态为已完成（降噪：不再逐步输出Brief）
        shared_data_space->updateThreadState(thread_id, VFT_SMF::GlobalSharedDataStruct::ThreadSyncState::COMPLETED);
        // 等待时钟重置，避免同一步再次进入
        auto post_sync = shared_data_space->getCurrentSyncSignal();
        while (post_sync.step_ready) {
            if (shared_data_space->isSimulationOver()) {
                goto env_thread_exit;
            }
            std::this_thread::sleep_for(std::chrono::microseconds(150));
            post_sync = shared_data_space->getCurrentSyncSignal();
        }
    }
    
env_thread_exit:
    // 注销线程
    shared_data_space->unregisterThread(thread_id);
    logBrief(LogLevel::Brief, "环境线程结束");
}

// 2. 数据共享空间线程函数
void data_space_thread_function(std::shared_ptr<GlobalShared_DataSpace::GlobalSharedDataSpace> shared_data_space) {
    
    logBrief(LogLevel::Brief, "数据共享空间线程启动");
    
    // 数据空间线程注册到时钟同步机制
    const std::string thread_id = "DATA_THREAD_001";
    const std::string thread_name = "Data_Space_Thread";
    const std::string thread_type = "DataSpace";
    
    if (!shared_data_space->registerThread(thread_id, thread_name, thread_type)) {
        logBrief(LogLevel::Brief, "数据共享空间线程注册失败");
        return;
    }
    
    logBrief(LogLevel::Brief, "数据共享空间线程注册成功");
    
    // 设置线程就绪状态
    data_space_thread_ready = true;
    logBrief(LogLevel::Brief, "数据共享空间线程已就绪");

    
    // 数据共享空间线程主循环 - 强制每步都工作（沿触发 + reset 等待）
    static uint64_t last_processed_step = std::numeric_limits<uint64_t>::max();
    while (!shared_data_space->isSimulationOver()) {
        // 设置状态为等待时钟信号
        shared_data_space->updateThreadState(thread_id, VFT_SMF::GlobalSharedDataStruct::ThreadSyncState::WAITING_FOR_CLOCK);
        
        // 轮询等待时钟信号（单一谓词：沿触发）
        auto sync_signal = shared_data_space->getCurrentSyncSignal();
        while (!(sync_signal.step_ready && sync_signal.current_step != last_processed_step)) {
            if (shared_data_space->isSimulationOver()) {
                logBrief(LogLevel::Brief, "数据共享空间线程检测到仿真结束标志，退出等待");
                goto data_thread_exit;
            }
            std::this_thread::sleep_for(std::chrono::microseconds(150));
            sync_signal = shared_data_space->getCurrentSyncSignal();
        }
        
        // 收到时钟通知，设置状态为运行
        shared_data_space->updateThreadState(thread_id, VFT_SMF::GlobalSharedDataStruct::ThreadSyncState::RUNNING);
        
        // 使用步号计算时间，避免浮点累计误差
        const uint64_t current_step = sync_signal.current_step;
        last_processed_step = current_step;
        const double record_time = static_cast<double>(current_step) * 0.01; // 与时钟time_step一致
        
        // 记录每个时间步的数据发布
        static int data_log_counter = 0;
        data_log_counter++;
        logBrief(LogLevel::Brief, "数据共享空间线程 - 数据已发布到记录器，仿真时间: " + std::to_string(record_time) + "s, 步号: " + std::to_string(current_step) + ", 总步数: " + std::to_string(data_log_counter));
        
        // 调用数据发布到数据记录器的函数（每步都调用）
        shared_data_space->publishToDataRecorder(record_time);
        
        // 减少状态日志输出频率
        static int state_log_counter = 0;
        state_log_counter++;        
        if (state_log_counter % 200 == 0) {
            auto env_state = shared_data_space->getEnvironmentState();
            logBrief(LogLevel::Brief, 
                "数据共享空间状态 - 仿真时间: " + std::to_string(record_time) + 
                "s, 风速: " + std::to_string(env_state.wind_speed) + 
                " m/s, 空气密度: " + std::to_string(env_state.air_density) + " kg/m³");
        }
        
        // 完成当前步骤的工作，设置状态为已完成
        shared_data_space->updateThreadState(thread_id, VFT_SMF::GlobalSharedDataStruct::ThreadSyncState::COMPLETED);
        
        // 等待时钟重置，避免同一步再次进入
        auto post_sync = shared_data_space->getCurrentSyncSignal();
        while (post_sync.step_ready) {
            if (shared_data_space->isSimulationOver()) {
                goto data_thread_exit;
            }
            std::this_thread::sleep_for(std::chrono::microseconds(150));
            post_sync = shared_data_space->getCurrentSyncSignal();
        }
    }
    
data_thread_exit:
    // 当前实现每步已记录，无需额外补充最后一步
    
    // 注销线程
    shared_data_space->unregisterThread(thread_id);
    logBrief(LogLevel::Brief, "数据共享空间线程结束");
}

// 3. 飞行动力学线程函数
void flight_dynamics_thread_function(std::shared_ptr<GlobalShared_DataSpace::GlobalSharedDataSpace> shared_data_space) {
    logBrief(LogLevel::Brief, "飞行动力学线程启动");
    
    const std::string thread_id = "FD_THREAD_001";
    const std::string thread_name = "Flight_Dynamics_Thread";
    const std::string thread_type = "FlightDynamics";
    
    if (!shared_data_space->registerThread(thread_id, thread_name, thread_type)) {
        logBrief(LogLevel::Brief, "飞行动力学线程注册失败");
        return;
    }
    
    logBrief(LogLevel::Brief, "飞行动力学线程注册成功");
    
    // 创建并初始化飞行动力学代理
    VFT_SMF::FlightDynamics::FlightDynamicsAgent fd_agent("B737");
    auto initial_state = shared_data_space->getAircraftFlightState();
    fd_agent.initialize(initial_state);
    
    // 代理初始化后立即运行一次更新，计算出基于初始状态的动态数据并覆盖共享数据空间
    const auto system_state = shared_data_space->getAircraftSystemState();
    const auto env_state = shared_data_space->getEnvironmentState();
    auto updated_state = fd_agent.updateFromGlobalState(0.0, system_state, env_state);
    shared_data_space->setAircraftFlightState(updated_state, "flight_dynamics_initial");
    
    // 计算并发布初始六分量合外力
    auto forces = fd_agent.getCurrentForces();
    VFT_SMF::GlobalSharedDataStruct::AircraftNetForce net_force;
    net_force.longitudinal_force = forces.force_x;
    net_force.lateral_force = forces.force_y;
    net_force.vertical_force = forces.force_z;
    net_force.roll_moment = forces.moment_x;
    net_force.pitch_moment = forces.moment_y;
    net_force.yaw_moment = forces.moment_z;
    net_force.thrust_force = (forces.force_x > 0.0) ? forces.force_x : 0.0;
    net_force.drag_force = (forces.force_x < 0.0) ? -forces.force_x : 0.0;
    net_force.lift_force = (forces.force_z > 0.0) ? forces.force_z : 0.0;
    // 使用系统状态中的质量推导重量（向下为负号）
    net_force.weight_force = -system_state.current_mass * 9.81;
    net_force.side_force = forces.force_y;
    net_force.timestamp = VFT_SMF::SimulationTimePoint{};
    shared_data_space->setAircraftNetForce(net_force, "flight_dynamics_initial");
    
    logBrief(LogLevel::Brief, "飞行动力学代理初始状态计算完成并已更新到共享数据空间");
    
    // 设置线程就绪状态
    flight_dynamics_thread_ready = true;
    
    // 飞行动力学线程主循环 - 订阅时钟通知
#if VFT_ENABLE_FD_TIMING
    // 记录每步执行时间（秒, 纳秒），结束时统一写文件，避免每步I/O干扰
    std::vector<std::pair<double, long long>> fd_timing_records;
    fd_timing_records.reserve(200000);
    std::unordered_set<uint64_t> fd_recorded_steps;
#endif
    uint64_t last_processed_step = std::numeric_limits<uint64_t>::max();
    while (!shared_data_space->isSimulationOver()) {
        shared_data_space->updateThreadState(thread_id, VFT_SMF::GlobalSharedDataStruct::ThreadSyncState::WAITING_FOR_CLOCK);
        
        // 轮询等待时钟同步信号（单一谓词：沿触发）
        auto sync_signal = shared_data_space->getCurrentSyncSignal();
        while (!(sync_signal.step_ready && sync_signal.current_step != last_processed_step)) {
            if (shared_data_space->isSimulationOver()) {
                goto fd_thread_exit;
            }
            std::this_thread::sleep_for(std::chrono::microseconds(100));
            sync_signal = shared_data_space->getCurrentSyncSignal();
        }
        
        shared_data_space->updateThreadState(thread_id, VFT_SMF::GlobalSharedDataStruct::ThreadSyncState::RUNNING);
        
        // 锁定本步步号
        const uint64_t fd_step = sync_signal.current_step;
        last_processed_step = fd_step;
        
        auto step_start_tp = std::chrono::steady_clock::now();
        
        const double current_time = static_cast<double>(fd_step) * 0.01;
        const double dt = 0.01; // 固定时间步长（恢复）
        
        // 从共享空间获取输入
        const auto system_state = shared_data_space->getAircraftSystemState();
        const auto env_state = shared_data_space->getEnvironmentState();
        
        // 更新飞行动力学
        auto new_state = fd_agent.updateFromGlobalState(dt, system_state, env_state);
        
        // 发布飞行状态
        shared_data_space->setAircraftFlightState(new_state, "flight_dynamics");

        // 计算并发布六分量合外力（含推/阻/升/重等分解），供数据记录器输出
        {
            auto forces = fd_agent.getCurrentForces();
            VFT_SMF::GlobalSharedDataStruct::AircraftNetForce net_force;
            net_force.longitudinal_force = forces.force_x;
            net_force.lateral_force = forces.force_y;
            net_force.vertical_force = forces.force_z;
            net_force.roll_moment = forces.moment_x;
            net_force.pitch_moment = forces.moment_y;
            net_force.yaw_moment = forces.moment_z;
            // 分解：推力/阻力/升力/重力/侧力
            net_force.thrust_force = (forces.force_x > 0.0) ? forces.force_x : 0.0;
            net_force.drag_force = (forces.force_x < 0.0) ? -forces.force_x : 0.0;
            net_force.lift_force = (forces.force_z > 0.0) ? forces.force_z : 0.0;
            // 使用系统状态中的质量推导重量（向下为负号）
            auto system_state_snapshot = shared_data_space->getAircraftSystemState();
            net_force.weight_force = -system_state_snapshot.current_mass * 9.81;
            net_force.side_force = forces.force_y;
            net_force.timestamp = VFT_SMF::SimulationTimePoint{};
            shared_data_space->setAircraftNetForce(net_force, "flight_dynamics");
        }
        
        // 记录本步FD耗时（纳秒），从 step 1 开始记录（跳过 step 0）
#if VFT_ENABLE_FD_TIMING
        auto step_end_tp = std::chrono::steady_clock::now();
        long long duration_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(step_end_tp - step_start_tp).count();
        if (fd_step >= 1) {
            fd_timing_records.emplace_back(current_time, duration_ns);
            fd_recorded_steps.insert(fd_step);
        }
#endif

        // 恢复 brief 输出，仍然保留较低频率
        static int fd_log_counter = 0;
        fd_log_counter++;
        if (fd_log_counter % 100 == 0) {
            logBrief(LogLevel::Brief, "飞行动力学更新 - 仿真时间: " + std::to_string(current_time) + "s");
        }
        
        shared_data_space->updateThreadState(thread_id, VFT_SMF::GlobalSharedDataStruct::ThreadSyncState::COMPLETED);
        // 等待时钟重置，避免同一步再次进入
        auto post_sync = shared_data_space->getCurrentSyncSignal();
        while (post_sync.step_ready) {
            if (shared_data_space->isSimulationOver()) {
                goto fd_thread_exit;
            }
            std::this_thread::sleep_for(std::chrono::microseconds(150));
            post_sync = shared_data_space->getCurrentSyncSignal();
        }
    }
    
fd_thread_exit:
    // 将采样到的计时数据写出到 output/fd_timing.csv（两列：微秒(小数) 与 纳秒(整数)）
#if VFT_ENABLE_FD_TIMING
    try {
        std::ofstream ofs("output/fd_timing.csv", std::ios::out | std::ios::trunc);
        if (ofs.is_open()) {
            ofs << "time_s,duration_us,duration_ns\n";
            ofs << std::fixed;
            for (const auto &rec : fd_timing_records) {
                double duration_us_d = static_cast<double>(rec.second) / 1000.0; // ns -> us
                ofs << std::setprecision(6) << rec.first << ","
                    << std::setprecision(3) << duration_us_d << ","
                    << rec.second << "\n";
            }
        }
        // 完整性校验：应覆盖 [1..last_processed_step]
        if (last_processed_step >= 1) {
            std::vector<uint64_t> missing_steps;
            missing_steps.reserve(16);
            for (uint64_t s = 1; s <= last_processed_step; ++s) {
                if (fd_recorded_steps.find(s) == fd_recorded_steps.end()) {
                    if (missing_steps.size() < 16) missing_steps.push_back(s);
                }
            }
            if (!missing_steps.empty()) {
                std::string msg = "FD计时缺失步号数量: " + std::to_string((last_processed_step) - static_cast<uint64_t>(fd_recorded_steps.size())) + ", 示例缺失: ";
                for (size_t i = 0; i < missing_steps.size(); ++i) {
                    msg += std::to_string(missing_steps[i]);
                    if (i + 1 < missing_steps.size()) msg += ",";
                }
                logBrief(LogLevel::Brief, msg);
            } else {
                logBrief(LogLevel::Brief, "FD计时完整覆盖 [1.." + std::to_string(last_processed_step) + "]");
            }
        }
    } catch (...) {
        // 忽略写出异常
    }
#endif
    shared_data_space->unregisterThread(thread_id);
    logBrief(LogLevel::Brief, "飞行动力学线程结束");
}

// 4. 飞行器系统线程函数
void aircraft_system_thread_function(std::shared_ptr<GlobalShared_DataSpace::GlobalSharedDataSpace> shared_data_space) {

    logBrief(LogLevel::Brief, "飞行器系统线程启动");
    
    const std::string thread_id = "AC_THREAD_001";
    const std::string thread_name = "Aircraft_System_Thread";
    const std::string thread_type = "AircraftSystem";
    
    if (!shared_data_space->registerThread(thread_id, thread_name, thread_type)) {
        logBrief(LogLevel::Brief, "飞行器系统线程注册失败");
        return;
    }
    
    logBrief(LogLevel::Brief, "飞行器系统线程注册成功");

    // 从飞行计划数据中获取配置的Aircraft_ID
    auto flight_plan_data = shared_data_space->getFlightPlanData();
    std::string aircraft_id = flight_plan_data.scenario_config.Aircraft_ID;
    if (aircraft_id.empty()) {
        aircraft_id = "Aircraft_001"; // 默认值
        logBrief(LogLevel::Brief, "警告: 未找到配置的Aircraft_ID，使用默认值: " + aircraft_id);
    } else {
        logBrief(LogLevel::Brief, "使用配置的Aircraft_ID: " + aircraft_id);
    }
    
    // 创建并初始化飞机系统代理
    VFT_SMF::AircraftAgent ACSystem_agent(aircraft_id, "B737_Aircraft_System");
    ACSystem_agent.initialize();
    
    // 设置全局共享数据空间
    ACSystem_agent.set_global_data_space(shared_data_space);
    
    // 启动飞机代理
    ACSystem_agent.start();
    
    // 飞机系统代理初始化后立即运行一次更新，计算出基于初始状态的动态数据并覆盖共享数据空间
    ACSystem_agent.update(0.0); // 运行一次初始更新
    ACSystem_agent.updateAircraftSystemState();
    auto initial_system_state = ACSystem_agent.getAircraftSystemState();
    shared_data_space->setAircraftSystemState(initial_system_state, "aircraft_system_initial");
    
    logBrief(LogLevel::Brief, "飞机系统代理初始状态计算完成并已更新到共享数据空间");
    
    // 设置线程就绪状态
    aircraft_system_thread_ready = true;
    logBrief(LogLevel::Brief, "飞行器系统代理已创建并启动");
    
    // 飞行器系统线程主循环 - 订阅时钟通知
    logBrief(LogLevel::Brief, "飞行器系统线程进入主循环");
    static uint64_t ac_last_step = std::numeric_limits<uint64_t>::max();
    while (!shared_data_space->isSimulationOver()) {
        // 设置状态为等待时钟信号
        shared_data_space->updateThreadState(thread_id, VFT_SMF::GlobalSharedDataStruct::ThreadSyncState::WAITING_FOR_CLOCK);
        
        // 轮询等待新步（单一谓词：沿触发）
        auto sync_signal = shared_data_space->getCurrentSyncSignal();
        while (!(sync_signal.step_ready && sync_signal.current_step != ac_last_step)) {
            if (shared_data_space->isSimulationOver()) {
                logBrief(LogLevel::Brief, "飞行器系统线程检测到仿真结束标志，退出等待");
                goto ac_thread_exit;
            }
            std::this_thread::sleep_for(std::chrono::microseconds(150));
            sync_signal = shared_data_space->getCurrentSyncSignal();
        }
        
        // 收到时钟通知，设置状态为运行
        shared_data_space->updateThreadState(thread_id, VFT_SMF::GlobalSharedDataStruct::ThreadSyncState::RUNNING);
        
        // 获取当前步与时间
        const uint64_t step = sync_signal.current_step;
        ac_last_step = step;
        const double current_time = static_cast<double>(step) * 0.01;
            
        // 飞行器系统线程更新
        ACSystem_agent.update(0.01); // 固定时间步长
        
        // 更新飞行器系统状态到共享数据空间（先更新，再获取）
        ACSystem_agent.updateAircraftSystemState();
        auto updated_system_state = ACSystem_agent.getAircraftSystemState();
        
        // 应用控制优先级管理器的最终控制指令
        auto final_control_command = shared_data_space->getFinalControlCommand();
        if (final_control_command.active) {
            // 应用最终控制指令到系统状态
            updated_system_state.current_throttle_position = final_control_command.throttle_command;
            updated_system_state.current_elevator_deflection = final_control_command.elevator_command * 50.0; // 转换为度数
            updated_system_state.current_aileron_deflection = final_control_command.aileron_command * 50.0;
            updated_system_state.current_rudder_deflection = final_control_command.rudder_command * 50.0;
            updated_system_state.current_brake_pressure = final_control_command.brake_command * 1e6; // 转换为Pa
            updated_system_state.datasource = "aircraft_system_with_priority_control";
            
            logBrief(LogLevel::Brief, "飞机系统线程: 应用优先级控制指令 - 源: " + final_control_command.source +
                    ", 油门: " + std::to_string(final_control_command.throttle_command) +
                    ", 刹车: " + std::to_string(final_control_command.brake_command));
        } else {
            // 如果没有激活的控制指令，保留原有逻辑
            auto existing_system_state = shared_data_space->getAircraftSystemState();
            updated_system_state.current_throttle_position = existing_system_state.current_throttle_position;
            updated_system_state.datasource = "aircraft_system";
        }
        
        shared_data_space->setAircraftSystemState(updated_system_state, updated_system_state.datasource);
        
        // 减少日志输出频率，只在每50步输出一次
        static int ac_log_counter = 0;
        ac_log_counter++;
        if (ac_log_counter % 50 == 0) {
            logBrief(LogLevel::Brief, "飞行器系统线程更新 - 仿真时间: " + std::to_string(current_time) + "s, 步骤: " + std::to_string(step));
        }
        
        // 完成当前步骤的工作，设置状态为已完成
        shared_data_space->updateThreadState(thread_id, VFT_SMF::GlobalSharedDataStruct::ThreadSyncState::COMPLETED);
        // 等待时钟重置，避免同一步再次进入
        auto post_sync = shared_data_space->getCurrentSyncSignal();
        while (post_sync.step_ready) {
            if (shared_data_space->isSimulationOver()) {
                goto ac_thread_exit;
            }
            std::this_thread::sleep_for(std::chrono::microseconds(150));
            post_sync = shared_data_space->getCurrentSyncSignal();
        }
    }
    
ac_thread_exit:
    // 注销线程
    shared_data_space->unregisterThread(thread_id);
    logBrief(LogLevel::Brief, "飞行器系统线程结束");
}

// 5. 事件监测线程函数
void event_monitor_thread_function(std::shared_ptr<GlobalShared_DataSpace::GlobalSharedDataSpace> shared_data_space) {
   
    logBrief(LogLevel::Brief, "事件监测线程启动");
    
    const std::string thread_id = "EM_THREAD_001";
    const std::string thread_name = "Event_Monitor_Thread";
    const std::string thread_type = "EventMonitor";
    
    if (!shared_data_space->registerThread(thread_id, thread_name, thread_type)) {
        logBrief(LogLevel::Brief, "事件监测线程注册失败");
        return;
    }
    
    logBrief(LogLevel::Brief, "事件监测线程注册成功");

    // 创建事件监测器
    std::unique_ptr<VFT_SMF::EventMonitor> event_monitor = 
        std::make_unique<VFT_SMF::EventMonitor>(shared_data_space);
    
    // 初始化事件监测器
    event_monitor->initialize();
    
    // 设置线程就绪状态
    event_monitor_thread_ready = true;
    logBrief(LogLevel::Brief, "事件监测器已创建并初始化");
    
    // 事件监测线程主循环 - 订阅时钟通知
    logBrief(LogLevel::Brief, "事件监测线程进入主循环");
    static uint64_t em_last_step = std::numeric_limits<uint64_t>::max();
    while (!shared_data_space->isSimulationOver()) {
        // 设置状态为等待时钟信号
        shared_data_space->updateThreadState(thread_id, VFT_SMF::GlobalSharedDataStruct::ThreadSyncState::WAITING_FOR_CLOCK);
        
        // 轮询等待新步（单一谓词：沿触发）
        auto sync_signal = shared_data_space->getCurrentSyncSignal();
        while (!(sync_signal.step_ready && sync_signal.current_step != em_last_step)) {
            if (shared_data_space->isSimulationOver()) {
                logBrief(LogLevel::Brief, "事件监测线程检测到仿真结束标志，退出等待");
                goto em_thread_exit;
            }
            std::this_thread::sleep_for(std::chrono::microseconds(150));
            sync_signal = shared_data_space->getCurrentSyncSignal();
        }
        
        // 收到时钟通知，设置状态为运行
        shared_data_space->updateThreadState(thread_id, VFT_SMF::GlobalSharedDataStruct::ThreadSyncState::RUNNING);
        
        // 获取当前步与时间
        const uint64_t step = sync_signal.current_step;
        em_last_step = step;
        const double current_time = static_cast<double>(step) * 0.01;
            
        // 事件监测更新
        auto newly_triggered_events = event_monitor->monitorEvents(current_time);
        
        // 处理新触发的事件：入队并按时间步记录（monitorEvents 内部已标记并统计）
        for (const auto& event : newly_triggered_events) {
            // 入队到共享数据空间的事件队列
            shared_data_space->enqueueEvent(event, current_time, "event_monitor");
            
            // 仍保留按时间步记录，供触发事件CSV输出（库内已去重）
            shared_data_space->addEventToStep(current_time, event);
            
            logBrief(LogLevel::Brief, "事件触发并入队: " + event.event_name + " (ID: " + event.getEventIdString() + ") - 时间: " + std::to_string(current_time) + "s");
        }
        
        // 减少日志输出频率，只在每100步输出一次
        static int em_log_counter = 0;
        em_log_counter++;
        if (em_log_counter % 100 == 0) {
            logBrief(LogLevel::Brief, "事件监测线程更新 - 仿真时间: " + std::to_string(current_time) + "s, 步骤: " + std::to_string(step));
        }
        
        // 如果有事件被触发，输出日志
        if (!newly_triggered_events.empty()) {
            logBrief(LogLevel::Brief, "事件监测线程在时间 " + std::to_string(current_time) + "s 检测到 " + std::to_string(newly_triggered_events.size()) + " 个新事件");
        }
        
        // 完成当前步骤的工作，设置状态为已完成
        shared_data_space->updateThreadState(thread_id, VFT_SMF::GlobalSharedDataStruct::ThreadSyncState::COMPLETED);
        // 等待时钟重置，避免同一步再次进入
        auto post_sync = shared_data_space->getCurrentSyncSignal();
        while (post_sync.step_ready) {
            if (shared_data_space->isSimulationOver()) {
                goto em_thread_exit;
            }
            std::this_thread::sleep_for(std::chrono::microseconds(150));
            post_sync = shared_data_space->getCurrentSyncSignal();
        }
    }
    
em_thread_exit:
    // 生成事件监测报告
    std::string event_report = event_monitor->generateReport();
    logBrief(LogLevel::Brief, "事件监测报告:\n" + event_report);
    
    // 注销线程
    shared_data_space->unregisterThread(thread_id);
    logBrief(LogLevel::Brief, "事件监测线程结束");
}

// 6. 事件分发线程

void event_dispatcher_thread_function(std::shared_ptr<GlobalShared_DataSpace::GlobalSharedDataSpace> shared_data_space) {
    
    logBrief(LogLevel::Brief, "事件分发线程启动");
    
    // 事件分发线程自己实现注册到时钟的功能
    const std::string thread_id = "ED_THREAD_001";
    const std::string thread_name = "Event_Dispatcher_Thread";
    const std::string thread_type = "EventDispatcher";
    
    if (!shared_data_space->registerThread(thread_id, thread_name, thread_type)) {
        logBrief(LogLevel::Brief, "事件分发线程注册失败");
        return;
    }
    
    logBrief(LogLevel::Brief, "事件分发线程注册成功");

    // 创建事件分发器
    std::unique_ptr<EventDispatcher> event_dispatcher = 
        std::make_unique<EventDispatcher>(shared_data_space);
    
    // 设置线程就绪状态
    event_dispatcher_thread_ready = true;
    logBrief(LogLevel::Brief, "EventDispatcher 已创建并初始化");
    
    // 控制器管理线程主循环 - 订阅时钟通知
    logBrief(LogLevel::Brief, "事件分发线程进入主循环");
    static uint64_t cm_last_step = std::numeric_limits<uint64_t>::max();
    while (!shared_data_space->isSimulationOver()) {
        // 设置状态为等待时钟信号
        shared_data_space->updateThreadState(thread_id, VFT_SMF::GlobalSharedDataStruct::ThreadSyncState::WAITING_FOR_CLOCK);
        
        // 轮询等待新步（单一谓词：沿触发）
        auto sync_signal = shared_data_space->getCurrentSyncSignal();
        while (!(sync_signal.step_ready && sync_signal.current_step != cm_last_step)) {
            if (shared_data_space->isSimulationOver()) {
                std::cout << "控制器管理线程检测到仿真结束标志，退出等待(前等待循环)" << std::endl;
                goto cm_thread_exit;
            }
            std::this_thread::sleep_for(std::chrono::microseconds(150));
            sync_signal = shared_data_space->getCurrentSyncSignal();
        }
        
        // 收到时钟通知，设置状态为运行
        shared_data_space->updateThreadState(thread_id, VFT_SMF::GlobalSharedDataStruct::ThreadSyncState::RUNNING);
        
        // 获取当前步与时间
        const uint64_t step = sync_signal.current_step;
        cm_last_step = step;
        const double current_time = static_cast<double>(step) * 0.01;
            
        // 使用新的方法处理已触发事件列表
        event_dispatcher->processTriggeredEvents(current_time);
        
        // 减少日志输出频率，只在每100步输出一次
        static int cm_log_counter = 0;
        cm_log_counter++;
        if (cm_log_counter % 100 == 0) {
            logBrief(LogLevel::Brief, "事件分发线程更新 - 仿真时间: " + std::to_string(current_time) + "s, 步骤: " + std::to_string(step));
        }
        
        // 完成当前步骤的工作，设置状态为已完成
        shared_data_space->updateThreadState(thread_id, VFT_SMF::GlobalSharedDataStruct::ThreadSyncState::COMPLETED);
        // 等待时钟重置，避免同一步再次进入
        auto post_sync = shared_data_space->getCurrentSyncSignal();
        while (post_sync.step_ready) {
            if (shared_data_space->isSimulationOver()) {
                std::cout << "控制器管理线程检测到仿真结束标志，退出等待(后等待循环)" << std::endl;
                goto cm_thread_exit;
            }
            std::this_thread::sleep_for(std::chrono::microseconds(150));
            post_sync = shared_data_space->getCurrentSyncSignal();
        }
    }
    
cm_thread_exit:
    // 注销线程
    std::cout << "事件分发线程退出清理" << std::endl;
    shared_data_space->unregisterThread(thread_id);
    logBrief(LogLevel::Brief, "事件分发线程结束");
}

// 7. 飞行员线程函数
void pilot_thread_function(std::shared_ptr<GlobalShared_DataSpace::GlobalSharedDataSpace> shared_data_space) {
    
    logBrief(LogLevel::Brief, "飞行员线程启动");
    
    // 飞行员线程自己实现注册到时钟的功能
    const std::string thread_id = "PILOT_THREAD_001";
    const std::string thread_name = "Pilot_Thread";
    const std::string thread_type = "Pilot";
    
    if (!shared_data_space->registerThread(thread_id, thread_name, thread_type)) {
        logBrief(LogLevel::Brief, "飞行员线程注册失败");
        return;
    }
    
    logBrief(LogLevel::Brief, "飞行员线程注册成功");

    // 从飞行计划数据中获取配置的Pilot_ID
    auto flight_plan_data = shared_data_space->getFlightPlanData();
    std::string pilot_id = flight_plan_data.scenario_config.Pilot_ID;
    if (pilot_id.empty()) {
        pilot_id = "Pilot_001"; // 默认值
        logBrief(LogLevel::Brief, "警告: 未找到配置的Pilot_ID，使用默认值: " + pilot_id);
    } else {
        logBrief(LogLevel::Brief, "使用配置的Pilot_ID: " + pilot_id);
    }
    
    // 创建并初始化飞行员代理
    VFT_SMF::PilotAgent pilot_agent(pilot_id, "B737_Pilot");
    pilot_agent.initializePilotStrategy(pilot_id);  // 初始化飞行员策略
    pilot_agent.initialize();
    pilot_agent.start();
    
    // 创建飞行员ATC指令处理器
    std::unique_ptr<PilotATCCommandHandler> pilot_atc_command_handler = 
        std::make_unique<PilotATCCommandHandler>(shared_data_space);
    // 创建飞行员手动控制处理器
    std::unique_ptr<PilotManualControlHandler> pilot_manual_control_handler =
        std::make_unique<PilotManualControlHandler>(shared_data_space);
    
    // 飞行员代理初始化后立即运行一次更新，计算出基于初始状态的动态数据并覆盖共享数据空间
    pilot_agent.update(0.0); // 运行一次初始更新
    
    logBrief(LogLevel::Brief, "飞行员代理初始状态计算完成并已更新到共享数据空间");
    
    // 设置线程就绪状态
    pilot_thread_ready = true;
    logBrief(LogLevel::Brief, "飞行员代理已创建并启动");
    
    // 飞行员线程主循环 - 订阅时钟通知
    logBrief(LogLevel::Brief, "飞行员线程进入主循环");
    static uint64_t pilot_last_step = std::numeric_limits<uint64_t>::max();
    while (!shared_data_space->isSimulationOver()) {
        // 设置状态为等待时钟信号
        shared_data_space->updateThreadState(thread_id, VFT_SMF::GlobalSharedDataStruct::ThreadSyncState::WAITING_FOR_CLOCK);
        
        // 轮询等待新步（单一谓词：沿触发）
        auto sync_signal = shared_data_space->getCurrentSyncSignal();
        while (!(sync_signal.step_ready && sync_signal.current_step != pilot_last_step)) {
            if (shared_data_space->isSimulationOver()) {
                logBrief(LogLevel::Brief, "飞行员线程检测到仿真结束标志，退出等待");
                goto pilot_thread_exit;
            }
            std::this_thread::sleep_for(std::chrono::microseconds(150));
            sync_signal = shared_data_space->getCurrentSyncSignal();
        }
        
        // 收到时钟通知，设置状态为运行
        shared_data_space->updateThreadState(thread_id, VFT_SMF::GlobalSharedDataStruct::ThreadSyncState::RUNNING);
        
        // 获取当前步与时间
        const uint64_t step = sync_signal.current_step;
        pilot_last_step = step;
        const double current_time = static_cast<double>(step) * 0.01;
            
        // 飞行员代理更新
        pilot_agent.update(0.01); // 固定时间步长
        
        // 检查是否有需要飞行员处理的事件（检查当前时间步及之前未处理的事件）
        auto triggered_events = shared_data_space->getTriggeredEventLibrary().getEventsAtStep(current_time);
        // 如果当前时间步没有事件，检查是否有在非整数秒触发的事件（时间匹配容差）
        if (triggered_events.empty()) {
            // 检查时间范围内的所有事件，容差为±0.1秒
            for (double check_time = current_time - 0.1; check_time <= current_time + 0.1; check_time += 0.01) {
                auto events_at_time = shared_data_space->getTriggeredEventLibrary().getEventsAtStep(check_time);
                for (const auto& event : events_at_time) {
                    if (event.is_triggered) {
                        triggered_events.push_back(event);
                    }
                }
            }
        }
        for (const auto& event : triggered_events) {
            if (event.is_triggered) {
                // 1) ATC 指令类 -> 交给飞行员ATC处理器
                if (event.driven_process.controller_type == "ATC_command") {
                    logBrief(LogLevel::Brief, "飞行员线程处理ATC指令: " + event.event_name + 
                            " (控制器: " + event.driven_process.controller_name + ") - 时间: " + std::to_string(current_time) + "s");
                    
                    // 使用飞行员ATC指令处理器处理指令
                    pilot_atc_command_handler->handlePilotATCCommand(event, current_time);
                // 2) 飞行员手动控制类 -> 交给飞行员手动控制处理器
                } else if (event.driven_process.controller_type == "Pilot_Manual_Control") {
                    logBrief(LogLevel::Brief, "飞行员线程处理手动控制: " + event.event_name +
                            " (控制器: " + event.driven_process.controller_name + ") - 时间: " + std::to_string(current_time) + "s");
                    pilot_manual_control_handler->handleManualControl(event, current_time);
                // 3) Pilot 飞行任务控制（例如 MaintainSPDRunway），也由飞行员线程处理
                } else if (event.driven_process.controller_type == "Pilot_Flight_Task_Control") {
                    logBrief(LogLevel::Brief, "飞行员线程处理飞行任务控制: " + event.event_name +
                            " (控制器: " + event.driven_process.controller_name + ") - 时间: " + std::to_string(current_time) + "s");
                    pilot_manual_control_handler->handleManualControl(event, current_time);
                // 4) 将 MaintainSPDRunway 视作飞行员的手动控制器，由飞行员线程处理（兼容旧映射: Aircraft_AutoPilot）
                } else if (event.driven_process.controller_type == "Aircraft_AutoPilot"
                           && event.driven_process.controller_name == "MaintainSPDRunway") {
                    logBrief(LogLevel::Brief, "飞行员线程处理速度保持: " + event.event_name +
                            " (控制器: MaintainSPDRunway) - 时间: " + std::to_string(current_time) + "s");
                    pilot_manual_control_handler->handleManualControl(event, current_time);
                }
            }
        }

        // 兼容兜底：如果已收到ATC放行且本步未从事件库拿到手动控制事件，则由飞行员线程触发平滑推油门到最大
        // 避免因事件映射缺失或浮点匹配导致的漏触发
        {
            static bool throttle_applied_after_clearance = false;
            const auto& atc_cmd_snapshot = shared_data_space->getATCCommand();
            if (atc_cmd_snapshot.clearance_granted && !throttle_applied_after_clearance) {
                VFT_SMF::GlobalSharedDataStruct::StandardEvent synth_event;
                synth_event.event_id = 6; // 与飞行计划中 taxi_clearance_received 对应的ID（如有差异不影响执行）
                synth_event.event_name = "taxi_clearance_received";
                synth_event.is_triggered = true;
                synth_event.driven_process.controller_type = "Pilot_Manual_Control";
                synth_event.driven_process.controller_name = "throttle_push2max";
                synth_event.driven_process.description = "推油门控制";
                logBrief(LogLevel::Brief, "飞行员线程兜底触发手动控制: " + synth_event.event_name +
                        " -> " + synth_event.driven_process.controller_name + " - 时间: " + std::to_string(current_time) + "s");
                pilot_manual_control_handler->handleManualControl(synth_event, current_time);
                throttle_applied_after_clearance = true;
            }
        }



        // 每步推进飞行员手动控制器的平滑过程（只改变系统状态，不改变飞行状态）
        pilot_manual_control_handler->tick(current_time);
        
        // 减少日志输出频率，只在每100步输出一次
        static int pilot_log_counter = 0;
        pilot_log_counter++;
        if (pilot_log_counter % 100 == 0) {
            logBrief(LogLevel::Brief, "飞行员线程更新 - 仿真时间: " + std::to_string(current_time) + "s, 步骤: " + std::to_string(step));
        }
        
        // 完成当前步骤的工作，设置状态为已完成
        shared_data_space->updateThreadState(thread_id, VFT_SMF::GlobalSharedDataStruct::ThreadSyncState::COMPLETED);
        // 等待时钟重置，避免同一步再次进入
        auto post_sync = shared_data_space->getCurrentSyncSignal();
        while (post_sync.step_ready) {
            if (shared_data_space->isSimulationOver()) {
                goto pilot_thread_exit;
            }
            std::this_thread::sleep_for(std::chrono::microseconds(150));
            post_sync = shared_data_space->getCurrentSyncSignal();
        }
    }
    
pilot_thread_exit:
    // 停止飞行员代理
    pilot_agent.stop();
    
    // 注销线程
    shared_data_space->unregisterThread(thread_id);
    logBrief(LogLevel::Brief, "飞行员线程结束");
}


// 8. ATC线程

void atc_thread_function(std::shared_ptr<GlobalShared_DataSpace::GlobalSharedDataSpace> shared_data_space) {
    
    logBrief(LogLevel::Brief, "ATC线程启动");
    
    // ATC线程自己实现注册到时钟的功能
    const std::string thread_id = "ATC_THREAD_001";
    const std::string thread_name = "ATC_Thread";
    const std::string thread_type = "ATC";
    
    if (!shared_data_space->registerThread(thread_id, thread_name, thread_type)) {
        logBrief(LogLevel::Brief, "ATC线程注册失败");
        return;
    }
    
    logBrief(LogLevel::Brief, "ATC线程注册成功");

    // 从飞行计划数据中获取配置的ATC_ID
    auto flight_plan_data = shared_data_space->getFlightPlanData();
    std::string atc_id = flight_plan_data.scenario_config.ATC_ID;
    if (atc_id.empty()) {
        atc_id = "ATC_001"; // 默认值
        logBrief(LogLevel::Brief, "警告: 未找到配置的ATC_ID，使用默认值: " + atc_id);
    } else {
        logBrief(LogLevel::Brief, "使用配置的ATC_ID: " + atc_id);
    }
    
    // 创建并初始化ATC代理
    VFT_SMF::ATCAgent atc_agent(atc_id, "PEK_Tower");
    
    // 设置共享数据空间到ATC代理
    atc_agent.set_shared_data_space(shared_data_space);
    
    // 设置飞行计划数据到ATC代理
    atc_agent.set_flight_plan_data(flight_plan_data);
    
    // 根据配置的ATC_ID初始化对应的策略
    atc_agent.initializeATCStrategy(atc_id);
    logBrief(LogLevel::Brief, "ATC代理已初始化策略: " + atc_id);
    
    atc_agent.initialize();
    atc_agent.start();
    
    // ATC代理初始化后立即运行一次更新，计算出基于初始状态的动态数据并覆盖共享数据空间
    atc_agent.update(0.0); // 运行一次初始更新
    
    logBrief(LogLevel::Brief, "ATC代理初始状态计算完成并已更新到共享数据空间");
    
    // 设置线程就绪状态
    atc_thread_ready = true;
    logBrief(LogLevel::Brief, "ATC代理已创建并启动");
    
    // ATC线程主循环 - 订阅时钟通知
    logBrief(LogLevel::Brief, "ATC线程进入主循环");
    static uint64_t atc_last_step = std::numeric_limits<uint64_t>::max(); //确保每个仿真步的事件只被ATC线程处理一次,避免在同一时间步内多次更新ATC指令状态;使用最大值作为初始值，确保第一次调用时能正常处理,因为任何实际的仿真步号都会小于这个最大值
    while (!shared_data_space->isSimulationOver()) {
        // 设置状态为等待时钟信号
        shared_data_space->updateThreadState(thread_id, VFT_SMF::GlobalSharedDataStruct::ThreadSyncState::WAITING_FOR_CLOCK);
        
        // 轮询等待新步（单一谓词：沿触发）
        auto sync_signal = shared_data_space->getCurrentSyncSignal();
        while (!(sync_signal.step_ready && sync_signal.current_step != atc_last_step)) {
            if (shared_data_space->isSimulationOver()) {
                logBrief(LogLevel::Brief, "ATC线程检测到仿真结束标志，退出等待");
                goto atc_thread_exit;
            }
            std::this_thread::sleep_for(std::chrono::microseconds(150));
            sync_signal = shared_data_space->getCurrentSyncSignal();
        }
        
        // 收到时钟通知，设置状态为运行
        shared_data_space->updateThreadState(thread_id, VFT_SMF::GlobalSharedDataStruct::ThreadSyncState::RUNNING);
        
        // 获取当前步与时间
        const uint64_t step = sync_signal.current_step;
        atc_last_step = step;
        const double current_time = static_cast<double>(step) * 0.01;
            
        // 检查是否有需要处理的ATC相关事件
        // 使用仿真时间获取事件（时间为键），避免类型不匹配导致查不到
        auto triggered_events = shared_data_space->getTriggeredEventLibrary().getEventsAtStep(current_time);
        
        // 减少日志输出频率，只在有事件或每100步输出一次
        static int atc_event_log_counter = 0;
        atc_event_log_counter++;
        if (!triggered_events.empty() || atc_event_log_counter % 100 == 0) {
            logBrief(LogLevel::Brief, "ATC线程检查时间 " + std::to_string(current_time) + "s 的事件，找到 " + std::to_string(triggered_events.size()) + " 个事件");
        }
        
        // 处理当前步的事件
        for (const auto& event : triggered_events) {
            if (event.is_triggered) {
                // 检查是否是ATC指令类型的事件
                if (event.driven_process.controller_type == "ATC_command") {
                    logBrief(LogLevel::Brief, "ATC线程处理事件: " + event.event_name + 
                            " (控制器: " + event.driven_process.controller_name + ") - 时间: " + std::to_string(current_time) + "s");
                    
                    // 使用ATC代理的控制器接口处理事件
                    atc_agent.executeController(event.driven_process.controller_name, 
                                              std::map<std::string, std::string>(), current_time);
                }
            }
        }
                
        // ATC代理更新（用于状态记录，不依赖其内部逻辑）
        atc_agent.update(0.01);
        
        // 减少日志输出频率，只在每100步输出一次
        static int atc_log_counter = 0;
        atc_log_counter++;
        if (atc_log_counter % 100 == 0) {
            logBrief(LogLevel::Brief, "ATC线程更新 - 仿真时间: " + std::to_string(current_time) + "s, 步骤: " + std::to_string(step));
        }
        
        // 完成当前步骤的工作，设置状态为已完成
        shared_data_space->updateThreadState(thread_id, VFT_SMF::GlobalSharedDataStruct::ThreadSyncState::COMPLETED);
        // 等待时钟重置，避免同一步再次进入
        auto post_sync = shared_data_space->getCurrentSyncSignal();
        while (post_sync.step_ready) {
            if (shared_data_space->isSimulationOver()) {
                goto atc_thread_exit;
            }
            std::this_thread::sleep_for(std::chrono::microseconds(150));
            post_sync = shared_data_space->getCurrentSyncSignal();
        }
    }
    
atc_thread_exit:
    // 停止ATC代理
    atc_agent.stop();
    
    // 注销线程
    shared_data_space->unregisterThread(thread_id);
    logBrief(LogLevel::Brief, "ATC线程结束");
}

} // namespace VFT_SMF

