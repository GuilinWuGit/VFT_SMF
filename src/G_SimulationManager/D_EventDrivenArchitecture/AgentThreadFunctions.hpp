/**
 * @file AgentThreadFunctions.hpp
 * @brief 代理线程函数定义 - 使用Simulation_Clock版本
 * @author VFT_SMF Development Team
 * @date 2024
 * 
 * 使用Simulation_Clock类进行时间管理
 * 目标：利用现有的Simulation_Clock类进行时间同步，使用环境线程和数据共享空间线程
 */

#pragma once

#include "../../G_SimulationManager/LogAndData/Logger.hpp"
#include "../../E_GlobalSharedDataSpace/GlobalSharedDataSpace.hpp"
#include "../../C_EnvirnomentAgentModel/EnvironmentAgent.hpp"
#include "../../B_AircraftAgentModel/AircraftAgent.hpp"
#include "../../A_PilotAgentModel/PilotAgent.hpp"
#include "../../E_FlightDynamics/FlightDynamicsAgent.hpp"
#include "../../G_SimulationManager/A_TimeSYNC/Simulation_Clock.hpp"
#include <iostream>
#include <memory>
#include <chrono>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <map>
#include <vector>
#include <queue>

namespace VFT_SMF {

// ==================== 全局变量声明 ====================

// 全局同步变量
extern std::atomic<bool> simulation_running;
extern std::atomic<bool> environment_thread_ready;
extern std::atomic<bool> data_space_thread_ready;
extern std::atomic<bool> flight_dynamics_thread_ready;
extern std::atomic<bool> aircraft_system_thread_ready;
extern std::atomic<bool> event_monitor_thread_ready;
extern std::atomic<bool> event_dispatcher_thread_ready;
extern std::atomic<bool> pilot_thread_ready;
extern std::atomic<bool> atc_thread_ready;

// ==================== 辅助函数声明 ====================

void wait_for_environment_thread_ready();
void wait_for_data_space_thread_ready();
void wait_for_flight_dynamics_thread_ready();
void wait_for_aircraft_system_thread_ready();
void wait_for_event_monitor_thread_ready();
void wait_for_event_dispatcher_thread_ready();
void wait_for_pilot_thread_ready();
void wait_for_atc_thread_ready();

// ==================== 线程函数声明 ====================

// 1. 环境线程函数
void environment_thread_function(std::shared_ptr<GlobalShared_DataSpace::GlobalSharedDataSpace> shared_data_space);
// 2. 数据空间线程函数
void data_space_thread_function(std::shared_ptr<GlobalShared_DataSpace::GlobalSharedDataSpace> shared_data_space);
// 3. 飞行动力学线程函数
void flight_dynamics_thread_function(std::shared_ptr<GlobalShared_DataSpace::GlobalSharedDataSpace> shared_data_space);
// 4. 飞行器系统线程函数
void aircraft_system_thread_function(std::shared_ptr<GlobalShared_DataSpace::GlobalSharedDataSpace> shared_data_space);
// 5. 事件监测线程函数
void event_monitor_thread_function(std::shared_ptr<GlobalShared_DataSpace::GlobalSharedDataSpace> shared_data_space);
// 6. 事件分发线程函数
void event_dispatcher_thread_function(std::shared_ptr<GlobalShared_DataSpace::GlobalSharedDataSpace> shared_data_space);
// 7. 飞行员线程函数
void pilot_thread_function(std::shared_ptr<GlobalShared_DataSpace::GlobalSharedDataSpace> shared_data_space);
// 8. ATC线程函数
void atc_thread_function(std::shared_ptr<GlobalShared_DataSpace::GlobalSharedDataSpace> shared_data_space);

} // namespace VFT_SMF
