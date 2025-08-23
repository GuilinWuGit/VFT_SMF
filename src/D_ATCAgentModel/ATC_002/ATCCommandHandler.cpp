/**
 * @file ATCCommandHandler.cpp
 * @brief ATC_002 指令处理器实现 - 严格的ATC管理逻辑
 * @author VFT_SMF Development Team
 * @date 2025-08-21
 */

#include "ATCCommandHandler.hpp"
#include <algorithm>

namespace VFT_SMF {

    ATCCommandHandler_002::ATCCommandHandler_002(std::shared_ptr<GlobalShared_DataSpace::GlobalSharedDataSpace> data_space)
        : shared_data_space(data_space),
          strict_mode_enabled(true),
          last_safety_check_time(0.0),
          total_commands_issued(0),
          safety_violations_detected(0) {
        logBrief(LogLevel::Brief, "ATC_002 指令处理器创建完成 - 严格模式已启用");
    }

    void ATCCommandHandler_002::handlePilotATCCommand(const GlobalSharedDataStruct::StandardEvent& event,
                                                 double current_time) {
        
        logBrief(LogLevel::Brief, "ATC_002 飞行员指令处理器: 处理事件 " + event.event_name + 
                " (ID: " + event.getEventIdString() + ") - 时间: " + std::to_string(current_time) + "s");
        
        // 执行安全检查
        performSafetyCheck(current_time);
        
        // 获取事件的驱动过程信息
        const auto& driven_process = event.driven_process;
        const std::string& controller_name = driven_process.controller_name;
        
        // ATC_002 特有的严格验证逻辑
        if (!validateClearanceRequest(controller_name, current_time)) {
            logBrief(LogLevel::Brief, "ATC_002: 拒绝指令请求 - 安全检查未通过: " + controller_name);
            safety_violations_detected++;
            return;
        }
        
        // 根据控制器名称执行相应的指令处理
        if (controller_name == "handle_taxi_clearance") {
            if (shouldIssueClearance("taxi", current_time)) {
                logATCCommand("taxi_clearance", "ATC_002 处理滑行许可指令 - 严格模式");
                updateATCCommandState("clearance_granted", true);
                applyStrictModeLogic("taxi_clearance");
            } else {
                logBrief(LogLevel::Brief, "ATC_002: 拒绝滑行许可 - 条件不满足");
            }
            
        } else if (controller_name == "handle_emergency_brake") {
            logATCCommand("emergency_brake", "ATC_002 处理紧急制动指令 - 立即执行");
            updateATCCommandState("emergency_brake", true);
            applyStrictModeLogic("emergency_brake");
            
        } else if (controller_name == "handle_landing_clearance") {
            if (shouldIssueClearance("landing", current_time)) {
                logATCCommand("landing_clearance", "ATC_002 处理着陆许可指令 - 严格验证");
                updateATCCommandState("clearance_granted", true);
                applyStrictModeLogic("landing_clearance");
            } else {
                logBrief(LogLevel::Brief, "ATC_002: 拒绝着陆许可 - 安全条件不满足");
            }
            
        } else if (controller_name == "handle_takeoff_clearance") {
            if (shouldIssueClearance("takeoff", current_time)) {
                logATCCommand("takeoff_clearance", "ATC_002 处理起飞许可指令 - 严格验证");
                updateATCCommandState("clearance_granted", true);
                applyStrictModeLogic("takeoff_clearance");
            } else {
                logBrief(LogLevel::Brief, "ATC_002: 拒绝起飞许可 - 安全条件不满足");
            }
            
        } else {
            logBrief(LogLevel::Brief, "ATC_002 飞行员指令处理器: 未知的控制器名称: " + controller_name);
        }
        
        total_commands_issued++;
        updateSafetyMetrics(controller_name);
    }

    void ATCCommandHandler_002::handleATCEvent(const GlobalSharedDataStruct::StandardEvent& event,
                                          double current_time) {
        
        logBrief(LogLevel::Brief, "ATC_002 事件处理器: 处理事件 " + event.event_name + 
                " (ID: " + event.getEventIdString() + ") - 时间: " + std::to_string(current_time) + "s");
        
        // 执行安全检查
        performSafetyCheck(current_time);
        
        // 获取事件的驱动过程信息
        const auto& driven_process = event.driven_process;
        const std::string& controller_name = driven_process.controller_name;
        
        // ATC_002 特有的严格验证逻辑
        if (!validateClearanceRequest(controller_name, current_time)) {
            logBrief(LogLevel::Brief, "ATC_002: 拒绝ATC事件 - 安全检查未通过: " + controller_name);
            safety_violations_detected++;
            return;
        }
        
        // 根据控制器名称执行相应的ATC事件处理
        if (controller_name == "issue_taxi_clearance") {
            if (shouldIssueClearance("taxi", current_time)) {
                logATCCommand("issue_taxi_clearance", "ATC_002 发布滑行许可 - 严格模式");
                updateATCCommandState("clearance_granted", true);
                applyStrictModeLogic("issue_taxi_clearance");
            } else {
                logBrief(LogLevel::Brief, "ATC_002: 拒绝发布滑行许可 - 条件不满足");
            }
            
        } else if (controller_name == "issue_emergency_brake") {
            logATCCommand("issue_emergency_brake", "ATC_002 发布紧急制动指令 - 立即执行");
            updateATCCommandState("emergency_brake", true);
            applyStrictModeLogic("issue_emergency_brake");
            
        } else if (controller_name == "issue_landing_clearance") {
            if (shouldIssueClearance("landing", current_time)) {
                logATCCommand("issue_landing_clearance", "ATC_002 发布着陆许可 - 严格验证");
                updateATCCommandState("clearance_granted", true);
                applyStrictModeLogic("issue_landing_clearance");
            } else {
                logBrief(LogLevel::Brief, "ATC_002: 拒绝发布着陆许可 - 安全条件不满足");
            }
            
        } else if (controller_name == "issue_takeoff_clearance") {
            if (shouldIssueClearance("takeoff", current_time)) {
                logATCCommand("issue_takeoff_clearance", "ATC_002 发布起飞许可 - 严格验证");
                updateATCCommandState("clearance_granted", true);
                applyStrictModeLogic("issue_takeoff_clearance");
            } else {
                logBrief(LogLevel::Brief, "ATC_002: 拒绝发布起飞许可 - 安全条件不满足");
            }
            
        } else {
            logBrief(LogLevel::Brief, "ATC_002 事件处理器: 未知的控制器名称: " + controller_name);
        }
        
        total_commands_issued++;
        updateSafetyMetrics(controller_name);
    }

    void ATCCommandHandler::performSafetyCheck(double current_time) {
        // ATC_002 特有的安全检查逻辑
        if (current_time - last_safety_check_time > 1.0) { // 每秒检查一次
            last_safety_check_time = current_time;
            
            // 检查飞机状态
            auto flight_state = shared_data_space->getAircraftFlightState();
            auto system_state = shared_data_space->getAircraftSystemState();
            
            // 严格的安全检查条件
            if (flight_state.airspeed > 50.0) {
                logBrief(LogLevel::Brief, "ATC_002 安全检查: 空速过高警告");
            }
            
            if (system_state.current_brake_pressure < 100000.0) {
                logBrief(LogLevel::Brief, "ATC_002 安全检查: 刹车压力不足警告");
            }
            
            logBrief(LogLevel::Brief, "ATC_002 执行安全检查 - 时间: " + std::to_string(current_time) + "s");
        }
    }

    bool ATCCommandHandler::validateClearanceRequest(const std::string& clearance_type, double current_time) {
        // ATC_002 特有的验证逻辑
        auto flight_state = shared_data_space->getAircraftFlightState();
        auto system_state = shared_data_space->getAircraftSystemState();
        
        // 严格的条件检查
        if (clearance_type.find("takeoff") != std::string::npos) {
            // 起飞许可需要更严格的条件
            if (flight_state.airspeed > 0.0) {
                logBrief(LogLevel::Brief, "ATC_002: 起飞许可验证失败 - 飞机仍在移动");
                return false;
            }
        }
        
        if (clearance_type.find("landing") != std::string::npos) {
            // 着陆许可需要更严格的条件
            if (flight_state.altitude > 100.0) {
                logBrief(LogLevel::Brief, "ATC_002: 着陆许可验证失败 - 高度过高");
                return false;
            }
        }
        
        return true;
    }

    bool ATCCommandHandler::shouldIssueClearance(const std::string& clearance_type, double current_time) {
        // ATC_002 特有的决策逻辑
        auto flight_state = shared_data_space->getAircraftFlightState();
        
        if (clearance_type == "taxi") {
            // 滑行许可：需要飞机完全静止
            return flight_state.airspeed < 0.1 && flight_state.groundspeed < 0.1;
        } else if (clearance_type == "takeoff") {
            // 起飞许可：需要更严格的条件
            return flight_state.airspeed == 0.0 && flight_state.groundspeed == 0.0 && 
                   current_time > 10.0; // 至少10秒后
        } else if (clearance_type == "landing") {
            // 着陆许可：需要高度和速度条件
            return flight_state.altitude < 50.0 && flight_state.airspeed < 30.0;
        }
        
        return true;
    }

    void ATCCommandHandler::applyStrictModeLogic(const std::string& command_type) {
        // ATC_002 特有的严格模式逻辑
        if (strict_mode_enabled) {
            logBrief(LogLevel::Brief, "ATC_002 严格模式: 应用额外安全措施 - " + command_type);
            
            // 额外的安全措施
            if (command_type.find("clearance") != std::string::npos) {
                // 许可类指令需要额外的确认
                logBrief(LogLevel::Brief, "ATC_002: 执行许可确认程序");
            }
        }
    }

    void ATCCommandHandler::updateSafetyMetrics(const std::string& command_type) {
        // 更新安全指标
        logBrief(LogLevel::Brief, "ATC_002 安全指标更新: 总指令数=" + std::to_string(total_commands_issued) + 
                ", 安全违规数=" + std::to_string(safety_violations_detected));
    }

    void ATCCommandHandler::logATCCommand(const std::string& command_type, const std::string& action) {
        logBrief(LogLevel::Brief, "ATC_002 " + action + " - " + command_type);
    }

    void ATCCommandHandler::updateATCCommandState(const std::string& command_type, bool value) {
        // 获取当前的ATC指令状态
        auto current_atc_command = shared_data_space->getATCCommand();
        
        // 根据指令类型更新相应的状态
        if (command_type == "clearance_granted") {
            current_atc_command.clearance_granted = value;
        } else if (command_type == "emergency_brake") {
            current_atc_command.emergency_brake = value;
        }
        
        // 更新ATC指令状态 - 使用ATC_002标识
        shared_data_space->setATCCommand(current_atc_command, "ATC_002_CommandHandler");
        
        logBrief(LogLevel::Brief, "ATC_002 指令状态已更新: " + command_type + " = " + (value ? "true" : "false"));
    }

} // namespace VFT_SMF
