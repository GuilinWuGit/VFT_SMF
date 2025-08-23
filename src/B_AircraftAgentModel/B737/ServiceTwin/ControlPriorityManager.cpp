/**
 * @file ControlPriorityManager.cpp
 * @brief 控制优先级管理器实现
 * @author VFT_SMF Development Team
 * @date 2025-08-23
 */

#include "ControlPriorityManager.hpp"
#include <algorithm>
#include <cmath>

namespace VFT_SMF {

    // ==================== 私有方法实现 ====================

    void ControlPriorityManager::initializePriorityMap() {
        // 初始化控制源到优先级的映射
        source_priority_map["emergency"] = GlobalSharedDataStruct::ControlPriority::EMERGENCY;
        source_priority_map["manual_override"] = GlobalSharedDataStruct::ControlPriority::MANUAL_OVERRIDE;
        source_priority_map["autopilot"] = GlobalSharedDataStruct::ControlPriority::AUTOPILOT;
        source_priority_map["autothrottle"] = GlobalSharedDataStruct::ControlPriority::AUTOTHROTTLE;
        source_priority_map["flight_director"] = GlobalSharedDataStruct::ControlPriority::FLIGHT_DIRECTOR;
        source_priority_map["stability_aug"] = GlobalSharedDataStruct::ControlPriority::STABILITY_AUG;
        source_priority_map["manual"] = GlobalSharedDataStruct::ControlPriority::MANUAL;
        source_priority_map["system_default"] = GlobalSharedDataStruct::ControlPriority::SYSTEM_DEFAULT;
        
        // 初始化控制源状态
        control_source_status["emergency"] = false;
        control_source_status["manual_override"] = false;
        control_source_status["autopilot"] = false;
        control_source_status["autothrottle"] = false;
        control_source_status["flight_director"] = false;
        control_source_status["stability_aug"] = false;
        control_source_status["manual"] = false;
        control_source_status["system_default"] = true; // 系统默认始终激活
    }

    void ControlPriorityManager::logControlCommand(const GlobalSharedDataStruct::ControlCommand& command, 
                                                  const std::string& action) const {
        logBrief(LogLevel::Brief, "控制优先级管理器: " + action + 
                " - 源: " + command.source + 
                ", 优先级: " + std::to_string(static_cast<int>(command.priority)) +
                ", 油门: " + std::to_string(command.throttle_command) +
                ", 升降舵: " + std::to_string(command.elevator_command) +
                ", 副翼: " + std::to_string(command.aileron_command) +
                ", 方向舵: " + std::to_string(command.rudder_command) +
                ", 刹车: " + std::to_string(command.brake_command));
    }

    void ControlPriorityManager::limitControlCommand(GlobalSharedDataStruct::ControlCommand& command) const {
        // 限制控制指令范围
        command.throttle_command = std::clamp(command.throttle_command, 0.0, 1.0);
        command.elevator_command = std::clamp(command.elevator_command, -1.0, 1.0);
        command.aileron_command = std::clamp(command.aileron_command, -1.0, 1.0);
        command.rudder_command = std::clamp(command.rudder_command, -1.0, 1.0);
        command.brake_command = std::clamp(command.brake_command, 0.0, 1.0);
    }

    // ==================== 控制指令管理 ====================

    void ControlPriorityManager::setManualControlCommand(double throttle, double elevator, double aileron, 
                                                        double rudder, double brake, double current_time) {
        GlobalSharedDataStruct::ControlCommand command;
        command.source = "pilot_manual";
        command.priority = GlobalSharedDataStruct::ControlPriority::MANUAL;
        command.throttle_command = throttle;
        command.elevator_command = elevator;
        command.aileron_command = aileron;
        command.rudder_command = rudder;
        command.brake_command = brake;
        command.timestamp = SimulationTimePoint{};
        command.active = true;

        limitControlCommand(command);
        
        if (validateControlCommand(command)) {
            shared_data_space->setControlCommand(command);
            logControlCommand(command, "设置飞行员手动控制指令");
        } else {
            logBrief(LogLevel::Brief, "控制优先级管理器: 飞行员手动控制指令未通过安全检查");
        }
    }

    void ControlPriorityManager::setAutopilotControlCommand(double throttle, double elevator, double aileron, 
                                                           double rudder, double current_time) {
        GlobalSharedDataStruct::ControlCommand command;
        command.source = "autopilot";
        command.priority = GlobalSharedDataStruct::ControlPriority::AUTOPILOT;
        command.throttle_command = throttle;
        command.elevator_command = elevator;
        command.aileron_command = aileron;
        command.rudder_command = rudder;
        command.brake_command = 0.0; // 自动驾驶仪通常不控制刹车
        command.timestamp = SimulationTimePoint{};
        command.active = true;

        limitControlCommand(command);
        
        if (validateControlCommand(command)) {
            shared_data_space->setControlCommand(command);
            logControlCommand(command, "设置自动驾驶仪控制指令");
        } else {
            logBrief(LogLevel::Brief, "控制优先级管理器: 自动驾驶仪控制指令未通过安全检查");
        }
    }

    void ControlPriorityManager::setAutothrottleControlCommand(double throttle, double current_time) {
        GlobalSharedDataStruct::ControlCommand command;
        command.source = "autothrottle";
        command.priority = GlobalSharedDataStruct::ControlPriority::AUTOTHROTTLE;
        command.throttle_command = throttle;
        command.elevator_command = 0.0; // 自动油门只控制油门
        command.aileron_command = 0.0;
        command.rudder_command = 0.0;
        command.brake_command = 0.0;
        command.timestamp = SimulationTimePoint{};
        command.active = true;

        limitControlCommand(command);
        
        if (validateControlCommand(command)) {
            shared_data_space->setControlCommand(command);
            logControlCommand(command, "设置自动油门控制指令");
        } else {
            logBrief(LogLevel::Brief, "控制优先级管理器: 自动油门控制指令未通过安全检查");
        }
    }

    void ControlPriorityManager::setEmergencyControlCommand(double throttle, double elevator, double aileron, 
                                                           double rudder, double brake, double current_time) {
        GlobalSharedDataStruct::ControlCommand command;
        command.source = "emergency";
        command.priority = GlobalSharedDataStruct::ControlPriority::EMERGENCY;
        command.throttle_command = throttle;
        command.elevator_command = elevator;
        command.aileron_command = aileron;
        command.rudder_command = rudder;
        command.brake_command = brake;
        command.timestamp = SimulationTimePoint{};
        command.active = true;

        limitControlCommand(command);
        
        // 紧急控制指令跳过安全检查
        shared_data_space->setControlCommand(command);
        logControlCommand(command, "设置紧急控制指令");
        logBrief(LogLevel::Brief, "控制优先级管理器: 紧急控制指令已激活，覆盖所有其他控制源");
    }

    void ControlPriorityManager::clearControlCommand(GlobalSharedDataStruct::ControlPriority priority) {
        shared_data_space->clearControlCommand(priority);
        logBrief(LogLevel::Brief, "控制优先级管理器: 清除优先级 " + std::to_string(static_cast<int>(priority)) + " 的控制指令");
    }

    void ControlPriorityManager::clearAllControlCommands() {
        auto manager = shared_data_space->getControlPriorityManager();
        manager.clearAllCommands();
        shared_data_space->setControlPriorityManager(manager);
        logBrief(LogLevel::Brief, "控制优先级管理器: 清除所有控制指令");
    }

    // ==================== 控制源状态管理 ====================

    void ControlPriorityManager::activateControlSource(const std::string& source_name) {
        control_source_status[source_name] = true;
        logBrief(LogLevel::Brief, "控制优先级管理器: 激活控制源 " + source_name);
    }

    void ControlPriorityManager::deactivateControlSource(const std::string& source_name) {
        control_source_status[source_name] = false;
        logBrief(LogLevel::Brief, "控制优先级管理器: 停用控制源 " + source_name);
    }

    bool ControlPriorityManager::isControlSourceActive(const std::string& source_name) const {
        auto it = control_source_status.find(source_name);
        return (it != control_source_status.end()) ? it->second : false;
    }

    // ==================== 优先级查询 ====================

    std::string ControlPriorityManager::getActiveControlSource() const {
        return shared_data_space->getControlPriorityManager().getActiveControlSource();
    }

    GlobalSharedDataStruct::ControlCommand ControlPriorityManager::getFinalControlCommand() const {
        return shared_data_space->getControlPriorityManager().calculateFinalCommand();
    }

    bool ControlPriorityManager::hasManualOverride() const {
        return shared_data_space->getControlPriorityManager().hasManualOverride();
    }

    // ==================== 安全检查和冲突解决 ====================

    bool ControlPriorityManager::validateControlCommand(const GlobalSharedDataStruct::ControlCommand& command) const {
        // 基本范围检查
        if (command.throttle_command < 0.0 || command.throttle_command > 1.0) {
            logBrief(LogLevel::Brief, "控制优先级管理器: 油门指令超出范围 [0.0, 1.0]");
            return false;
        }
        
        if (std::abs(command.elevator_command) > 1.0) {
            logBrief(LogLevel::Brief, "控制优先级管理器: 升降舵指令超出范围 [-1.0, 1.0]");
            return false;
        }
        
        if (std::abs(command.aileron_command) > 1.0) {
            logBrief(LogLevel::Brief, "控制优先级管理器: 副翼指令超出范围 [-1.0, 1.0]");
            return false;
        }
        
        if (std::abs(command.rudder_command) > 1.0) {
            logBrief(LogLevel::Brief, "控制优先级管理器: 方向舵指令超出范围 [-1.0, 1.0]");
            return false;
        }
        
        if (command.brake_command < 0.0 || command.brake_command > 1.0) {
            logBrief(LogLevel::Brief, "控制优先级管理器: 刹车指令超出范围 [0.0, 1.0]");
            return false;
        }

        // 可以添加更多安全检查，如：
        // - 检查当前飞行状态是否允许该控制指令
        // - 检查控制指令的变化率是否合理
        // - 检查是否存在危险的控制组合

        return true;
    }

    GlobalSharedDataStruct::ControlCommand ControlPriorityManager::resolveControlConflict(
        const GlobalSharedDataStruct::ControlCommand& command1,
        const GlobalSharedDataStruct::ControlCommand& command2) const {
        
        // 优先级高的指令获胜
        if (static_cast<int>(command1.priority) < static_cast<int>(command2.priority)) {
            logBrief(LogLevel::Brief, "控制优先级管理器: 解决冲突，选择优先级更高的指令: " + command1.source);
            return command1;
        } else {
            logBrief(LogLevel::Brief, "控制优先级管理器: 解决冲突，选择优先级更高的指令: " + command2.source);
            return command2;
        }
    }

    void ControlPriorityManager::applyFinalControlCommand(double current_time) {
        auto final_command = getFinalControlCommand();
        
        if (final_command.active) {
            // 应用最终控制指令到飞机系统状态
            auto system_state = shared_data_space->getAircraftSystemState();
            system_state.current_throttle_position = final_command.throttle_command;
            system_state.current_elevator_deflection = final_command.elevator_command * 50.0; // 转换为度数
            system_state.current_aileron_deflection = final_command.aileron_command * 50.0;
            system_state.current_rudder_deflection = final_command.rudder_command * 50.0;
            system_state.current_brake_pressure = final_command.brake_command * 1e6; // 转换为Pa
            system_state.timestamp = SimulationTimePoint{};
            system_state.datasource = "control_priority_manager";
            
            shared_data_space->setAircraftSystemState(system_state, "control_priority_manager");
            
            logBrief(LogLevel::Brief, "控制优先级管理器: 应用最终控制指令 - 源: " + final_command.source +
                    ", 油门: " + std::to_string(final_command.throttle_command) +
                    ", 刹车: " + std::to_string(final_command.brake_command));
        }
    }

} // namespace VFT_SMF
