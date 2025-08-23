/**
 * 
 * 2. 定义以上所有数据结构体的双缓冲技术的容器
 * 
 * 3. 全局共享数据空间主类，包括：
 * 1）3个私有数据容器实 2）数据写入接 3）数据读取接 4）发布数据到数据记录器的方法
 * 5)交换缓冲区的方法
 * 6）清理数据的方法
 * 
 * 
 */

#pragma once

#include "GlobalSharedDataStruct.hpp"
#include "../F_ScenarioModelling/B_ScenarioModel/VFT_SMF_Base.hpp"
#include "../G_SimulationManager/B_SimManage/SimulationNameSpace.hpp"
#include "../G_SimulationManager/LogAndData/Logger.hpp"
#include "../G_SimulationManager/LogAndData/DataRecorder.hpp"
#include <unordered_map>
#include <vector>
#include <queue>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <condition_variable>
#include <atomic>
#include <functional>
#include <chrono>
#include <string>
#include <memory>
#include <sstream>
#include <iomanip>

namespace VFT_SMF {

    namespace GlobalShared_DataSpace {

    // ==================== 2. 定义双缓冲技术的数据容器 ====================

     template <typename T>
     class DoubleBuffer {
     public:
        DoubleBuffer() : frontBuffer(&bufferA), backBuffer(&bufferB) {}
        
        // 获取只读的前端缓冲区（消费者使用）
        const T& read() const {
            return *frontBuffer;
        }
        
        // 获取可写的后端缓冲区（生产者使用）
        T& write() {
            return *backBuffer;
        }
        
        // 交换前后端缓冲区
        void swap() {
            std::lock_guard<std::mutex> lock(swapMutex);
            std::swap(frontBuffer, backBuffer);
        }
        
    private:
        T bufferA;
        T bufferB;
        T* frontBuffer;  // 前端缓冲区（只读）
        T* backBuffer;   // 后端缓冲区（可写）
        std::mutex swapMutex;
    };

    // ==================== 3. 定义全局共享数据空间主类 ====================
    class GlobalSharedDataSpace {
    private:
        
        // 3.1 双缓冲数据容器实现- 状态数据模块（7个）
        DoubleBuffer<VFT_SMF::GlobalSharedDataStruct::FlightPlanData> flightPlanBuffer;
        DoubleBuffer<VFT_SMF::GlobalSharedDataStruct::AircraftFlightState> aircraftFlightStateBuffer;
        DoubleBuffer<VFT_SMF::GlobalSharedDataStruct::AircraftSystemState> aircraftSystemStateBuffer;
        DoubleBuffer<VFT_SMF::GlobalSharedDataStruct::PilotGlobalState> pilotStateBuffer;
        DoubleBuffer<VFT_SMF::GlobalSharedDataStruct::EnvironmentGlobalState> environmentStateBuffer;
        DoubleBuffer<VFT_SMF::GlobalSharedDataStruct::ATCGlobalState> atcStateBuffer;
        DoubleBuffer<VFT_SMF::GlobalSharedDataStruct::AircraftNetForce> aircraftNetForceBuffer;
        
        // 3.2 双缓冲数据容器实现- 逻辑数据模块（4个）
        DoubleBuffer<VFT_SMF::GlobalSharedDataStruct::AircraftGlobalLogic> aircraftLogicBuffer;
        DoubleBuffer<VFT_SMF::GlobalSharedDataStruct::PilotGlobalLogic> pilotLogicBuffer;
        DoubleBuffer<VFT_SMF::GlobalSharedDataStruct::EnvironmentGlobalLogic> environmentLogicBuffer;
        DoubleBuffer<VFT_SMF::GlobalSharedDataStruct::ATCGlobalLogic> atcLogicBuffer;
        
        // 3.3 事件系统数据容器（3个）
        VFT_SMF::GlobalSharedDataStruct::PlannedEventLibrary planned_event_library;       ///< 计划事件库 - 存储预定义事件，来自飞行计划
        VFT_SMF::GlobalSharedDataStruct::TriggeredEventLibrary triggered_event_library;   ///< 已触发事件库 - 存储已触发事件记录，来自事件系统
        VFT_SMF::GlobalSharedDataStruct::EventQueue eventQueue;       ///< 事件队列 - 存储待处理事件队列（单缓冲区实现）
        mutable std::mutex eventQueueAccessMutex;  ///< 事件队列访问锁
        
        // 3.4 ATC指令数据容器（1个）
        DoubleBuffer<VFT_SMF::GlobalSharedDataStruct::ATC_Command> atcCommandBuffer;      ///< ATC指令数据 - 存储ATC发出的指令
        
        // 3.5 计划控制器数据容器（1个）
        DoubleBuffer<VFT_SMF::GlobalSharedDataStruct::PlanedControllersLibrary> planedControllersBuffer; ///< 计划控制器数据 - 存储飞行计划中定义的控制器
        
        // 3.6 控制器执行状态数据容器（1个）
        DoubleBuffer<VFT_SMF::GlobalSharedDataStruct::ControllerExecutionStatus> controllerExecutionStatusBuffer; ///< 控制器执行状态跟踪数据 - 存储每个控制器的运行状态
        
        // 3.7 控制优先级管理器数据容器（1个）
        DoubleBuffer<VFT_SMF::GlobalSharedDataStruct::ControlPriorityManager> controlPriorityManagerBuffer; ///< 控制优先级管理器 - 管理不同控制源的优先级
        
        // 3.8 线程同步管理器
        VFT_SMF::GlobalSharedDataStruct::ThreadSyncManager thread_sync_manager;           ///< 线程同步管理器
        
        // 3.8 代理事件队列管理器
        VFT_SMF::GlobalSharedDataStruct::AgentEventQueueManager agent_event_queue_manager; ///< 代理事件队列管理器
        
        // （回退）移除栅栏相关成员，恢复轮询方案
        
    public:
        GlobalSharedDataSpace() = default;
        ~GlobalSharedDataSpace() = default;

        // ==================== 3.4 定义数据写入接口 ====================
      
        // 3.3.1 设置飞行计划数据
        void setFlightPlanData(const VFT_SMF::GlobalSharedDataStruct::FlightPlanData& data) {
            flightPlanBuffer.write() = data;
            flightPlanBuffer.swap(); // 立即交换，使读端能读到最新数据
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "飞行计划数据已存储到共享数据空间");
        }
        
        // 3.3.1.1 设置飞行计划数据（带数据来源）
        void setFlightPlanData(const VFT_SMF::GlobalSharedDataStruct::FlightPlanData& data, const std::string& datasource) {
            VFT_SMF::GlobalSharedDataStruct::FlightPlanData data_with_source = data;
            data_with_source.datasource = datasource;
            flightPlanBuffer.write() = data_with_source;
            flightPlanBuffer.swap(); // 立即交换，使读端能读到最新数据
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "飞行计划数据已存储到共享数据空间，数据来源: " + datasource);
        }
        
        // 3.3.2 设置飞机飞行状态数据
        void setAircraftFlightState(const VFT_SMF::GlobalSharedDataStruct::AircraftFlightState& state) {
            aircraftFlightStateBuffer.write() = state;
            // 写入后立即交换，使读端能在本步读到最新飞行状态数据
            aircraftFlightStateBuffer.swap();
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "飞行器飞行状态已存储到共享数据空间");
        }
        
        // 3.3.2.1 设置飞机飞行状态数据（带数据来源）
        void setAircraftFlightState(const VFT_SMF::GlobalSharedDataStruct::AircraftFlightState& state, const std::string& datasource) {
            VFT_SMF::GlobalSharedDataStruct::AircraftFlightState state_with_source = state;
            state_with_source.datasource = datasource;
            aircraftFlightStateBuffer.write() = state_with_source;
            // 写入后立即交换，使读端能在本步读到最新飞行状态数据
            aircraftFlightStateBuffer.swap();
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "飞行器飞行状态已存储到共享数据空间，数据来源: " + datasource);
        }
        
        // 3.3.3 设置飞机系统状态数据
        void setAircraftSystemState(const VFT_SMF::GlobalSharedDataStruct::AircraftSystemState& state) {
            aircraftSystemStateBuffer.write() = state;
            aircraftSystemStateBuffer.swap(); // 立即交换，使读端能读到最新数据
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "飞行器系统状态已存储到共享数据空间");
        }
        
        // 3.3.3.1 设置飞机系统状态数据（带数据来源）
        void setAircraftSystemState(const VFT_SMF::GlobalSharedDataStruct::AircraftSystemState& state, const std::string& datasource) {
            VFT_SMF::GlobalSharedDataStruct::AircraftSystemState state_with_source = state;
            state_with_source.datasource = datasource;
            aircraftSystemStateBuffer.write() = state_with_source;
            aircraftSystemStateBuffer.swap(); // 立即交换，使读端能读到最新数据
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "飞行器系统状态已存储到共享数据空间，数据来源: " + datasource);
        }
        
        // 3.3.4 设置飞行员状态数据
        void setPilotState(const VFT_SMF::GlobalSharedDataStruct::PilotGlobalState& state) {
            pilotStateBuffer.write() = state;
            // 写入后立即交换，使读端能在本步读到最新飞行员数据
            pilotStateBuffer.swap(); 
            VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "飞行员状态已存储到共享数据空间");
        }
        
        // 3.3.4.1 设置飞行员状态数据（带数据来源）
        void setPilotState(const VFT_SMF::GlobalSharedDataStruct::PilotGlobalState& state, const std::string& datasource) {
            VFT_SMF::GlobalSharedDataStruct::PilotGlobalState state_with_source = state;
            state_with_source.datasource = datasource;
            pilotStateBuffer.write() = state_with_source;
            // 写入后立即交换，使读端能在本步读到最新飞行员数据
            pilotStateBuffer.swap();
            VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "飞行员状态已存储到共享数据空间，数据来源: " + datasource);
        }
        
        // 3.3.5 设置环境状态数据
        void setEnvironmentState(const VFT_SMF::GlobalSharedDataStruct::EnvironmentGlobalState& state) {
            environmentStateBuffer.write() = state;
            // 写入后立即交换，使读端能在本步读到最新环境数据
            environmentStateBuffer.swap();
            VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "环境状态已存储到共享数据空间");
        }
        
        // 3.3.5.1 设置环境状态数据（带数据来源）
        void setEnvironmentState(const VFT_SMF::GlobalSharedDataStruct::EnvironmentGlobalState& state, const std::string& datasource) {
            VFT_SMF::GlobalSharedDataStruct::EnvironmentGlobalState state_with_source = state;
            state_with_source.datasource = datasource;
            environmentStateBuffer.write() = state_with_source;
            // 写入后立即交换，使读端能在本步读到最新环境数据
            environmentStateBuffer.swap();
            VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "环境状态已存储到共享数据空间，数据来源: " + datasource);
        }
        
        // 3.3.6 设置ATC状态数据
        void setATCState(const VFT_SMF::GlobalSharedDataStruct::ATCGlobalState& state) {
            atcStateBuffer.write() = state;
            atcStateBuffer.swap(); // 立即交换，使读端能读到最新数据
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "ATC状态已存储到共享数据空间");
        }
        
        // 3.3.6.1 设置ATC状态数据（带数据来源）
        void setATCState(const VFT_SMF::GlobalSharedDataStruct::ATCGlobalState& state, const std::string& datasource) {
            VFT_SMF::GlobalSharedDataStruct::ATCGlobalState state_with_source = state;
            state_with_source.datasource = datasource;
            atcStateBuffer.write() = state_with_source;
            atcStateBuffer.swap(); // 立即交换，使读端能读到最新数据
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "ATC状态已存储到共享数据空间，数据来源: " + datasource);
        }
        
        // 3.3.7 设置飞机逻辑数据
        void setAircraftLogic(const VFT_SMF::GlobalSharedDataStruct::AircraftGlobalLogic& logic) {
            aircraftLogicBuffer.write() = logic;
            aircraftLogicBuffer.swap(); // 立即交换，使读端能读到最新数据
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "飞行器逻辑数据已存储到共享数据空间");
        }
        
        // 3.3.7.1 设置飞机逻辑数据（带数据来源）
        void setAircraftLogic(const VFT_SMF::GlobalSharedDataStruct::AircraftGlobalLogic& logic, const std::string& datasource) {
            VFT_SMF::GlobalSharedDataStruct::AircraftGlobalLogic logic_with_source = logic;
            logic_with_source.datasource = datasource;
            aircraftLogicBuffer.write() = logic_with_source;
            aircraftLogicBuffer.swap(); // 立即交换，使读端能读到最新数据
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "飞行器逻辑数据已存储到共享数据空间，数据来源: " + datasource);
        }
        
        // 3.3.8 设置飞行员逻辑数据
        void setPilotLogic(const VFT_SMF::GlobalSharedDataStruct::PilotGlobalLogic& logic) {
            pilotLogicBuffer.write() = logic;
            pilotLogicBuffer.swap(); // 立即交换，使读端能读到最新数据
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "飞行员逻辑数据已存储到共享数据空间");
        }
        
        // 3.3.8.1 设置飞行员逻辑数据（带数据来源）
        void setPilotLogic(const VFT_SMF::GlobalSharedDataStruct::PilotGlobalLogic& logic, const std::string& datasource) {
            VFT_SMF::GlobalSharedDataStruct::PilotGlobalLogic logic_with_source = logic;
            logic_with_source.datasource = datasource;
            pilotLogicBuffer.write() = logic_with_source;
            pilotLogicBuffer.swap(); // 立即交换，使读端能读到最新数据
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "飞行员逻辑数据已存储到共享数据空间，数据来源: " + datasource);
        }
        
        // 3.3.9 设置环境逻辑数据
        void setEnvironmentLogic(const VFT_SMF::GlobalSharedDataStruct::EnvironmentGlobalLogic& logic) {
            environmentLogicBuffer.write() = logic;
            environmentLogicBuffer.swap(); // 立即交换，使读端能读到最新数据
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "环境逻辑数据已存储到共享数据空间");
        }
        
        // 3.3.9.1 设置环境逻辑数据（带数据来源）
        void setEnvironmentLogic(const VFT_SMF::GlobalSharedDataStruct::EnvironmentGlobalLogic& logic, const std::string& datasource) {
            VFT_SMF::GlobalSharedDataStruct::EnvironmentGlobalLogic logic_with_source = logic;
            logic_with_source.datasource = datasource;
            environmentLogicBuffer.write() = logic_with_source;
            environmentLogicBuffer.swap(); // 立即交换，使读端能读到最新数据
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "环境逻辑数据已存储到共享数据空间，数据来源: " + datasource);
        }
        
        // 3.3.10 设置ATC逻辑数据
        void setATCLogic(const VFT_SMF::GlobalSharedDataStruct::ATCGlobalLogic& logic) {
            atcLogicBuffer.write() = logic;
            atcLogicBuffer.swap(); // 立即交换，使读端能读到最新数据
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "ATC逻辑数据已存储到共享数据空间");
        }
        
        // 3.3.10.1 设置ATC逻辑数据（带数据来源）
        void setATCLogic(const VFT_SMF::GlobalSharedDataStruct::ATCGlobalLogic& logic, const std::string& datasource) {
            VFT_SMF::GlobalSharedDataStruct::ATCGlobalLogic logic_with_source = logic;
            logic_with_source.datasource = datasource;
            atcLogicBuffer.write() = logic_with_source;
            atcLogicBuffer.swap(); // 立即交换，使读端能读到最新数据
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "ATC逻辑数据已存储到共享数据空间，数据来源: " + datasource);
        }
        
        // 3.3.11 设置六分量合外力数据
        void setAircraftNetForce(const VFT_SMF::GlobalSharedDataStruct::AircraftNetForce& net_force) {
            aircraftNetForceBuffer.write() = net_force;
            aircraftNetForceBuffer.swap(); // 立即交换，使读端能读到最新数据
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "六分量合外力数据已存储到共享数据空间");
        }
        
        // 3.3.11.1 设置六分量合外力数据（带数据来源）
        void setAircraftNetForce(const VFT_SMF::GlobalSharedDataStruct::AircraftNetForce& net_force, const std::string& datasource) {
            VFT_SMF::GlobalSharedDataStruct::AircraftNetForce net_force_with_source = net_force;
            net_force_with_source.datasource = datasource;
            aircraftNetForceBuffer.write() = net_force_with_source;
            aircraftNetForceBuffer.swap(); // 立即交换，使读端能读到最新数据
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "六分量合外力数据已存储到共享数据空间，数据来源: " + datasource);
        }

        // 3.3.11 设置计划事件库数据
        void setPlannedEventLibrary(const VFT_SMF::GlobalSharedDataStruct::PlannedEventLibrary& library) {
            planned_event_library = library;
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "计划事件库数据已存储到共享数据空间");
        }
        
        // 3.3.11.1 设置计划事件库数据（带数据来源）
        void setPlannedEventLibrary(const VFT_SMF::GlobalSharedDataStruct::PlannedEventLibrary& library, const std::string& datasource) {
            VFT_SMF::GlobalSharedDataStruct::PlannedEventLibrary library_with_source = library;
            library_with_source.datasource = datasource;
            planned_event_library = library_with_source;
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "计划事件库数据已存储到共享数据空间，数据来源: " + datasource);
        }
        
        // 3.3.12 设置已触发事件库数据
        void setTriggeredEventLibrary(const VFT_SMF::GlobalSharedDataStruct::TriggeredEventLibrary& library) {
            triggered_event_library = library;
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "已触发事件库数据已存储到共享数据空间");
        }
        
        // 3.3.12.1 设置已触发事件库数据（带数据来源）
        void setTriggeredEventLibrary(const VFT_SMF::GlobalSharedDataStruct::TriggeredEventLibrary& library, const std::string& datasource) {
            VFT_SMF::GlobalSharedDataStruct::TriggeredEventLibrary library_with_source = library;
            library_with_source.datasource = datasource;
            triggered_event_library = library_with_source;
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "已触发事件库数据已存储到共享数据空间，数据来�? " + datasource);
        }
        
        // 3.3.13 清除事件库中的所有事件（仿真开始时调用）
        void clearEventLibrary() {
            planned_event_library.clearPlannedEvents();
            triggered_event_library.clearTriggeredEvents();
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "事件库已清除");
        }
        
        // 3.3.14 标记事件为已触发
        bool markEventAsTriggered(const std::string& event_id, double trigger_time) {
            VFT_SMF::GlobalSharedDataStruct::StandardEvent* event = planned_event_library.findPlannedEvent(event_id);
            if (event && !event->is_triggered) {
                // 标记为已触发
                event->is_triggered = true;
                
                // 添加到已触发事件库
                triggered_event_library.addTriggeredEvent(*event);
                
                VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "事件已触发: " + event_id + " at " + std::to_string(trigger_time) + "s");
                return true;
            }
            return false;
        }
        
        // 3.3.15 设置ATC指令数据
        void setATCCommand(const VFT_SMF::GlobalSharedDataStruct::ATC_Command& command) {
            atcCommandBuffer.write() = command;
            atcCommandBuffer.swap(); // 立即交换，使读端能读到最新指令
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "ATC指令已存储到共享数据空间: clearance=" + std::to_string(command.clearance_granted) + ", emergency_brake=" + std::to_string(command.emergency_brake));
        }
        
        // 3.3.15.1 设置ATC指令数据（带数据来源）
        void setATCCommand(const VFT_SMF::GlobalSharedDataStruct::ATC_Command& command, const std::string& datasource) {
            VFT_SMF::GlobalSharedDataStruct::ATC_Command command_with_source = command;
            command_with_source.datasource = datasource;
            atcCommandBuffer.write() = command_with_source;
            atcCommandBuffer.swap(); // 立即交换，使读端能读到最新指令
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "ATC指令已存储到共享数据空间，数据来源: " + datasource + ", clearance=" + std::to_string(command.clearance_granted) + ", emergency_brake=" + std::to_string(command.emergency_brake));
        }

        // ==================== 5. 定义数据读取接口 ====================
        // 5.1 获取飞行计划数据
        const VFT_SMF::GlobalSharedDataStruct::FlightPlanData& getFlightPlanData() const {
            return flightPlanBuffer.read();
        }
        
        // 5.2 获取飞机飞行状态数据
        const VFT_SMF::GlobalSharedDataStruct::AircraftFlightState& getAircraftFlightState() const {
            return aircraftFlightStateBuffer.read();
        }
        
        // 5.3 获取飞机系统状态数据
        const VFT_SMF::GlobalSharedDataStruct::AircraftSystemState& getAircraftSystemState() const {
            return aircraftSystemStateBuffer.read();
        }
        
        // 5.4 获取飞行员状态数据
        const VFT_SMF::GlobalSharedDataStruct::PilotGlobalState& getPilotState() const {
            return pilotStateBuffer.read();
        }
        
        // 5.5 获取环境状态数据
        const VFT_SMF::GlobalSharedDataStruct::EnvironmentGlobalState& getEnvironmentState() const {
            return environmentStateBuffer.read();
        }
        
        // 5.6 获取ATC状态数据
        const VFT_SMF::GlobalSharedDataStruct::ATCGlobalState& getATCState() const {
            return atcStateBuffer.read();
        }
        
        // 5.7 获取飞机逻辑数据
        const VFT_SMF::GlobalSharedDataStruct::AircraftGlobalLogic& getAircraftLogic() const {
            return aircraftLogicBuffer.read();
        }
        
        // 5.8 获取飞行员逻辑数据
        const VFT_SMF::GlobalSharedDataStruct::PilotGlobalLogic& getPilotLogic() const {
            return pilotLogicBuffer.read();
        }
        
        // 5.9 获取环境逻辑数据
        const VFT_SMF::GlobalSharedDataStruct::EnvironmentGlobalLogic& getEnvironmentLogic() const {
            return environmentLogicBuffer.read();
        }
        
        // 5.10 获取ATC逻辑数据
        const VFT_SMF::GlobalSharedDataStruct::ATCGlobalLogic& getATCLogic() const {
            return atcLogicBuffer.read();
        }
        
        // 5.11 获取六分量合外力数据
        const VFT_SMF::GlobalSharedDataStruct::AircraftNetForce& getAircraftNetForce() const {
            return aircraftNetForceBuffer.read();
        }

        // 5.12 获取计划事件库数据
        const VFT_SMF::GlobalSharedDataStruct::PlannedEventLibrary& getPlannedEventLibrary() const {    
            return planned_event_library;
        }
        
        // 5.11.1 添加计划事件到事件库
        void addPlannedEventToLibrary(const VFT_SMF::GlobalSharedDataStruct::StandardEvent& event) {
            planned_event_library.addPlannedEvent(event);
        }
        
        // 5.11.2 获取事件库中的所有预定义事件
        std::vector<VFT_SMF::GlobalSharedDataStruct::StandardEvent> getPlannedEvents() const {
            return planned_event_library.getPlannedEvents();
        }
        
        // 5.11.3 根据ID查找预定义事件
        VFT_SMF::GlobalSharedDataStruct::StandardEvent* findPlannedEvent(const std::string& event_id) {
            return planned_event_library.findPlannedEvent(event_id);
        }
        
        // 5.11.4 获取已触发事件列表
        std::vector<VFT_SMF::GlobalSharedDataStruct::StandardEvent> getTriggeredEvents() const {
            return triggered_event_library.getTriggeredEvents();
        }
        
        // 5.11.5 获取下一个活跃事件
        bool getNextActiveEvent(VFT_SMF::GlobalSharedDataStruct::StandardEvent& event) {
            auto planned_events = planned_event_library.getPlannedEvents();
            for (auto& planned_event : planned_events) {
                if (!planned_event.is_triggered) {
                    event = planned_event;
                    return true;
                }
            }
            return false;
        }
        
        // 5.11.6 检查是否有活跃事件
        bool hasActiveEvents() const {
            auto planned_events = planned_event_library.getPlannedEvents();
            for (const auto& planned_event : planned_events) {
                if (!planned_event.is_triggered) {
                    return true;
                }
            }
            return false;
        }
        
        // 5.11.7 获取活跃事件数量
        size_t getActiveEventCount() const {
            size_t count = 0;
            auto planned_events = planned_event_library.getPlannedEvents();
            for (const auto& planned_event : planned_events) {
                if (!planned_event.is_triggered) {
                    count++;
                }
            }
            return count;
        }
        
        // 5.12 获取已触发事件库数据
        const VFT_SMF::GlobalSharedDataStruct::TriggeredEventLibrary& getTriggeredEventLibrary() const {
            return triggered_event_library;
        }
        
        // 5.13 获取已触发事件库数据（非const版本）
        VFT_SMF::GlobalSharedDataStruct::TriggeredEventLibrary& getTriggeredEventLibrary() {
            return triggered_event_library;
        }
        
        // 5.14 添加事件到指定时间步
        void addEventToStep(double step_time, const VFT_SMF::GlobalSharedDataStruct::StandardEvent& event) {
            triggered_event_library.addEventToStep(step_time, event);
            // 添加调试日志
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, 
                "事件已添加到时间�? " + std::to_string(step_time) + 
                "s, 事件名称: " + event.event_name + 
                ", 事件ID: " + event.getEventIdString() + 
                ", 当前step_events_map大小: " + std::to_string(triggered_event_library.getStepEventsMap().size()));
        }

                // 5.15 获取事件队列数据
        VFT_SMF::GlobalSharedDataStruct::EventQueue getEventQueue() const {
            std::lock_guard<std::mutex> lock(eventQueueAccessMutex);
            return eventQueue;  // 返回副本而不是引用，避免锁释放后的数据竞争
        }

        // 5.16 设置事件队列数据
        void setEventQueue(const VFT_SMF::GlobalSharedDataStruct::EventQueue& event_queue,
                          const std::string& datasource = "unknown") {
            std::lock_guard<std::mutex> lock(eventQueueAccessMutex);
            eventQueue = event_queue;
            eventQueue.datasource = datasource;
            eventQueue.timestamp = VFT_SMF::SimulationTimePoint{};

            if (VFT_SMF::globalLogger) {
                VFT_SMF::globalLogger->info("事件队列数据已存储到共享数据空间，数据来�? " + datasource);
            }
        }

        // 5.17 添加事件到队列
        void enqueueEvent(const VFT_SMF::GlobalSharedDataStruct::StandardEvent& event,
                         double trigger_time,
                         const std::string& source = "event_monitor") {
            std::lock_guard<std::mutex> lock(eventQueueAccessMutex);
            eventQueue.enqueueEvent(event, trigger_time, source);

            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief,
                "事件已添加到队列: " + event.event_name +
                ", 触发时间: " + std::to_string(trigger_time) +
                "s, 来源: " + source +
                ", 队列大小: " + std::to_string(eventQueue.getQueueSize()));
        }

        // 5.18 从队列中取出事件
        bool dequeueEvent(VFT_SMF::GlobalSharedDataStruct::EventQueueItem& item) {
            std::lock_guard<std::mutex> lock(eventQueueAccessMutex);
            bool success = eventQueue.dequeueEvent(item);
            if (success) {
                VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, 
                    "事件已从队列取出: " + item.event.event_name + 
                    ", 触发时间: " + std::to_string(item.trigger_time) + "s");
            }
            return success;
        }
        
        // 5.14 获取ATC指令数据
        const VFT_SMF::GlobalSharedDataStruct::ATC_Command& getATCCommand() const {
            return atcCommandBuffer.read();
        }

        // 5.15 获取计划控制器库数据
        const VFT_SMF::GlobalSharedDataStruct::PlanedControllersLibrary& getPlanedControllersLibrary() const {
            return planedControllersBuffer.read();
        }

        // 5.16 设置计划控制器库数据
        void setPlanedControllersLibrary(const VFT_SMF::GlobalSharedDataStruct::PlanedControllersLibrary& planed_controllers, 
                                        const std::string& datasource = "unknown") {
            auto& write_buffer = planedControllersBuffer.write();
            write_buffer = planed_controllers;
            write_buffer.datasource = datasource;
            write_buffer.timestamp = VFT_SMF::SimulationTimePoint{};
            planedControllersBuffer.swap();
            
            if (VFT_SMF::globalLogger) {
                VFT_SMF::globalLogger->info("计划控制器库数据已存储到共享数据空间，数据来�? " + datasource);
            }
        }

        // 5.17 获取控制器执行状态数据
        const VFT_SMF::GlobalSharedDataStruct::ControllerExecutionStatus& getControllerExecutionStatus() const {
            return controllerExecutionStatusBuffer.read();
        }

        // 5.18 设置控制器执行状态数据
        void setControllerExecutionStatus(const VFT_SMF::GlobalSharedDataStruct::ControllerExecutionStatus& status, 
                                         const std::string& datasource = "unknown") {
            auto& write_buffer = controllerExecutionStatusBuffer.write();
            write_buffer = status;
            write_buffer.datasource = datasource;
            write_buffer.timestamp = VFT_SMF::SimulationTimePoint{};
            controllerExecutionStatusBuffer.swap();
            
            if (VFT_SMF::globalLogger) {
                VFT_SMF::globalLogger->info("控制器执行状态数据已存储到共享数据空间，数据来源: " + datasource);
            }
        }

        // 5.19 更新单个控制器状态
        void updateControllerStatus(const std::string& controller_name, bool is_running, 
                                   const std::string& datasource = "unknown") {
            auto& write_buffer = controllerExecutionStatusBuffer.write();
            write_buffer.setControllerStatus(controller_name, is_running);
            write_buffer.datasource = datasource;
            write_buffer.timestamp = VFT_SMF::SimulationTimePoint{};
            controllerExecutionStatusBuffer.swap();
        }

        // 5.20 设置控制优先级管理器数据
        void setControlPriorityManager(const VFT_SMF::GlobalSharedDataStruct::ControlPriorityManager& manager) {
            controlPriorityManagerBuffer.write() = manager;
            controlPriorityManagerBuffer.swap(); // 立即交换，使读端能读到最新数据
            if (VFT_SMF::globalLogger) {
                VFT_SMF::globalLogger->info("控制优先级管理器已存储到共享数据空间");
            }
        }

        // 5.21 设置控制指令（便捷方法）
        void setControlCommand(const VFT_SMF::GlobalSharedDataStruct::ControlCommand& command) {
            auto manager = controlPriorityManagerBuffer.read();
            manager.setControlCommand(command);
            controlPriorityManagerBuffer.write() = manager;
            controlPriorityManagerBuffer.swap();
            if (VFT_SMF::globalLogger) {
                VFT_SMF::globalLogger->info("控制指令已设置，优先级: " + std::to_string(static_cast<int>(command.priority)) + ", 源: " + command.source);
            }
        }

        // 5.22 清除控制指令（便捷方法）
        void clearControlCommand(VFT_SMF::GlobalSharedDataStruct::ControlPriority priority) {
            auto manager = controlPriorityManagerBuffer.read();
            manager.clearControlCommand(priority);
            controlPriorityManagerBuffer.write() = manager;
            controlPriorityManagerBuffer.swap();
            if (VFT_SMF::globalLogger) {
                VFT_SMF::globalLogger->info("控制指令已清除，优先级: " + std::to_string(static_cast<int>(priority)));
            }
        }

        // 5.23 获取控制优先级管理器
        const VFT_SMF::GlobalSharedDataStruct::ControlPriorityManager& getControlPriorityManager() const {
            return controlPriorityManagerBuffer.read();
        }

        // 5.24 获取最终控制指令
        VFT_SMF::GlobalSharedDataStruct::ControlCommand getFinalControlCommand() {
            auto manager = controlPriorityManagerBuffer.read();
            return manager.calculateFinalCommand();
        }


        // ==================== 6. 发布数据到数据记录器 ====================
        /**
         * @brief 发布所有核心数据模块到数据记录器
         * 将当前缓冲区中的所有数据写入到数据记录器的相应缓冲区中
         * @param simulation_time 仿真时间
         */
         void publishToDataRecorder(double simulation_time = 0.0) {
            // 检查全局数据记录器是否可用
            if (VFT_SMF::globalDataRecorder && VFT_SMF::globalDataRecorder->isInitialized()) {
                // 发布所有核心数据模块到数据记录器
                VFT_SMF::globalDataRecorder->recordAllData(simulation_time, this);
                
                if (VFT_SMF::globalLogger) {
                    VFT_SMF::globalLogger->info("数据已发布到数据记录器，仿真时间: " + std::to_string(simulation_time));
                }
            } else {
                if (VFT_SMF::globalLogger) {
                    VFT_SMF::globalLogger->info("数据记录器不可用，跳过数据发布，仿真时间: " + std::to_string(simulation_time));
                }
            }
        }
        
        // 5.7 交换所有缓冲区
        void swapAllBuffers() {
            flightPlanBuffer.swap();
            aircraftFlightStateBuffer.swap();
            aircraftSystemStateBuffer.swap();
            pilotStateBuffer.swap();
            environmentStateBuffer.swap();
            atcStateBuffer.swap();
            aircraftNetForceBuffer.swap();
            aircraftLogicBuffer.swap();
            pilotLogicBuffer.swap();
            environmentLogicBuffer.swap();
            atcLogicBuffer.swap();
            atcCommandBuffer.swap();
            planedControllersBuffer.swap();
            // 注意：eventQueue 不需要swap，因为它使用单缓冲区实现
        }


        
        /**
         * @brief 发布事件数据到数据记录器
         * 只在仿真结束时发布所有事件数据到数据记录器
         * @param simulation_time 仿真时间
         */
        void publishEventDataToRecorder(double simulation_time = 0.0) {
            // 数据记录器只被动接收数据，不主动获取
            // 事件数据发布由外部调用者负责
            if (VFT_SMF::globalLogger) {
                VFT_SMF::globalLogger->info("Event data publishing ready at time: " + std::to_string(simulation_time));
            }
        }

        // ==================== 7. 数据清理 ====================
        /**
         * @brief 清理所有缓冲区
         */
        void clearAllBuffers() {
            flightPlanBuffer.write() = VFT_SMF::GlobalSharedDataStruct::FlightPlanData();
            aircraftFlightStateBuffer.write() = VFT_SMF::GlobalSharedDataStruct::AircraftFlightState();
            aircraftSystemStateBuffer.write() = VFT_SMF::GlobalSharedDataStruct::AircraftSystemState();
            pilotStateBuffer.write() = VFT_SMF::GlobalSharedDataStruct::PilotGlobalState();
            environmentStateBuffer.write() = VFT_SMF::GlobalSharedDataStruct::EnvironmentGlobalState();
            atcStateBuffer.write() = VFT_SMF::GlobalSharedDataStruct::ATCGlobalState();
            aircraftNetForceBuffer.write() = VFT_SMF::GlobalSharedDataStruct::AircraftNetForce();
            aircraftLogicBuffer.write() = VFT_SMF::GlobalSharedDataStruct::AircraftGlobalLogic();
            pilotLogicBuffer.write() = VFT_SMF::GlobalSharedDataStruct::PilotGlobalLogic();
            environmentLogicBuffer.write() = VFT_SMF::GlobalSharedDataStruct::EnvironmentGlobalLogic();
            atcLogicBuffer.write() = VFT_SMF::GlobalSharedDataStruct::ATCGlobalLogic();
            swapAllBuffers();
        }
        
        // ==================== 8. 线程同步管理 ====================
        // 简单的线程注册和状态管理接口
        /**
         * @brief 注册线程到同步管理器
         * @param thread_id 线程ID
         * @param thread_name 线程名称
         * @param thread_type 线程类型
         * @return 是否注册成功
         */
        bool registerThread(const std::string& thread_id, const std::string& thread_name, const std::string& thread_type);
        
        /**
         * @brief 注销线程
         * @param thread_id 线程ID
         * @return 是否注销成功
         */
        bool unregisterThread(const std::string& thread_id);
        
        /**
         * @brief 更新线程状态
         * @param thread_id 线程ID
         * @param state 新状态
         */
        void updateThreadState(const std::string& thread_id, VFT_SMF::GlobalSharedDataStruct::ThreadSyncState state);
        
        /**
         * @brief 获取线程状态
         * @param thread_id 线程ID
         * @return 线程状态
         */
        VFT_SMF::GlobalSharedDataStruct::ThreadSyncState getThreadState(const std::string& thread_id);
        
        /**
         * @brief 获取所有注册的线程
         * @return 注册的线程映射
         */
        std::map<std::string, VFT_SMF::GlobalSharedDataStruct::ThreadRegistrationInfo> getRegisteredThreads();
        
        /**
         * @brief 设置时钟运行状态
         * @param running 是否运行
         */
        void setClockRunning(bool running);
        
        /**
         * @brief 设置仿真结束标志
         * @param is_over 是否结束
         */
        void setSimulationOver(bool is_over);
        
        /**
         * @brief 获取仿真结束标志
         * @return 是否结束
         */
        bool isSimulationOver();
        
        /**
         * @brief 更新同步信号
         * @param simulation_time 仿真时间
         * @param step 仿真步数
         */
        void updateSyncSignal(double simulation_time, uint64_t step);
        
        /**
         * @brief 重置同步信号，表示当前步骤已完成
         */
        void resetSyncSignal();
        
        /**
         * @brief 获取当前同步信号
         * @return 当前同步信号
         */
        VFT_SMF::GlobalSharedDataStruct::ClockSyncSignal getCurrentSyncSignal();

        // ==================== 9. 代理事件队列管理 ====================
        
        /**
         * @brief 为代理创建事件队列
         * @param agent_id 代理ID
         */
        void createAgentEventQueue(const std::string& agent_id);
        
        /**
         * @brief 向指定代理队列添加事件
         * @param agent_id 代理ID
         * @param event 事件对象
         * @param trigger_time 触发时间
         * @param ctrl_type 控制器类型
         * @param ctrl_name 控制器名称
         * @param params 控制器参数
         */
        void enqueueAgentEvent(const std::string& agent_id, const VFT_SMF::GlobalSharedDataStruct::StandardEvent& event,
                              double trigger_time, const std::string& ctrl_type, const std::string& ctrl_name,
                              const std::map<std::string, std::string>& params = {});
        
        /**
         * @brief 从指定代理队列取出事件
         * @param agent_id 代理ID
         * @param item 输出的事件项
         * @return 是否成功取出事件
         */
        bool dequeueAgentEvent(const std::string& agent_id, VFT_SMF::GlobalSharedDataStruct::AgentEventQueueItem& item);
        
        /**
         * @brief 获取指定代理队列大小
         * @param agent_id 代理ID
         * @return 队列大小
         */
        size_t getAgentEventQueueSize(const std::string& agent_id) const;
        
        /**
        * @brief 检查指定代理队列是否为空
         * @param agent_id 代理ID
         * @return 是否为空
         */
        bool isAgentEventQueueEmpty(const std::string& agent_id) const;
        
        /**
         * @brief 获取所有代理ID列表
         * @return 代理ID列表
         */
        std::vector<std::string> getAgentEventQueueIds() const;

        // 回退：不提供栅栏等待接口
    };

    // ==================== 4. 定义全局实例 ====================
    extern GlobalSharedDataSpace globalSharedDataSpace;
} // namespace GlobalShared_DataSpace

} // namespace VFT_SMF 
