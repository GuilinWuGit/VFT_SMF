/**
 * @file ATCCommandHandler.cpp
 * @brief ATC指令处理器实现
 * @author VFT_SMF Development Team
 * @date 2025-08-21
 */

#include "ATCCommandHandler.hpp"
#include <algorithm>

namespace VFT_SMF {

    ATCCommandHandler::ATCCommandHandler(std::shared_ptr<GlobalShared_DataSpace::GlobalSharedDataSpace> data_space)
        : shared_data_space(data_space) {
        logBrief(LogLevel::Brief, "ATC指令处理器创建完成");
    }

    void ATCCommandHandler::handlePilotATCCommand(const GlobalSharedDataStruct::StandardEvent& event,
                                                 double current_time) {
        
        logBrief(LogLevel::Brief, "飞行员ATC指令处理器: 处理事件 " + event.event_name + 
                " (ID: " + event.getEventIdString() + ") - 时间: " + std::to_string(current_time) + "s");
        
        // 获取事件的驱动过程信息
        const auto& driven_process = event.driven_process;
        const std::string& controller_name = driven_process.controller_name;
        
        // 根据控制器名称执行相应的指令处理
        if (controller_name == "handle_taxi_clearance") {
            logATCCommand("taxi_clearance", "处理滑行许可指令");
            updateATCCommandState("clearance_granted", true);
            
        } else if (controller_name == "handle_emergency_brake") {
            logATCCommand("emergency_brake", "处理紧急制动指令");
            updateATCCommandState("emergency_brake", true);
            
        } else if (controller_name == "handle_landing_clearance") {
            logATCCommand("landing_clearance", "处理着陆许可指令");
            // 着陆许可使用clearance_granted字段
            updateATCCommandState("clearance_granted", true);
            
        } else if (controller_name == "handle_takeoff_clearance") {
            logATCCommand("takeoff_clearance", "处理起飞许可指令");
            // 起飞许可使用clearance_granted字段
            updateATCCommandState("clearance_granted", true);
            
        } else {
            logBrief(LogLevel::Brief, "飞行员ATC指令处理器: 未知的控制器名称: " + controller_name);
        }
    }

    void ATCCommandHandler::handleATCEvent(const GlobalSharedDataStruct::StandardEvent& event,
                                          double current_time) {
        
        logBrief(LogLevel::Brief, "ATC事件处理器: 处理事件 " + event.event_name + 
                " (ID: " + event.getEventIdString() + ") - 时间: " + std::to_string(current_time) + "s");
        
        // 获取事件的驱动过程信息
        const auto& driven_process = event.driven_process;
        const std::string& controller_name = driven_process.controller_name;
        
        // 根据控制器名称执行相应的ATC事件处理
        if (controller_name == "issue_taxi_clearance") {
            logATCCommand("issue_taxi_clearance", "发布滑行许可");
            updateATCCommandState("clearance_granted", true);
            
        } else if (controller_name == "issue_emergency_brake") {
            logATCCommand("issue_emergency_brake", "发布紧急制动指令");
            updateATCCommandState("emergency_brake", true);
            
        } else if (controller_name == "issue_landing_clearance") {
            logATCCommand("issue_landing_clearance", "发布着陆许可");
            // 着陆许可使用clearance_granted字段
            updateATCCommandState("clearance_granted", true);
            
        } else if (controller_name == "issue_takeoff_clearance") {
            logATCCommand("issue_takeoff_clearance", "发布起飞许可");
            // 起飞许可使用clearance_granted字段
            updateATCCommandState("clearance_granted", true);
            
        } else {
            logBrief(LogLevel::Brief, "ATC事件处理器: 未知的控制器名称: " + controller_name);
        }
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
        
        // 更新ATC指令状态
        shared_data_space->setATCCommand(current_atc_command, "ATCCommandHandler");
        
        logBrief(LogLevel::Brief, "ATC指令状态已更新: " + command_type + " = " + (value ? "true" : "false"));
    }

    void ATCCommandHandler::logATCCommand(const std::string& command_type, const std::string& action) {
        logBrief(LogLevel::Brief, "ATC指令处理器: " + action + " - 指令类型: " + command_type);
    }

} // namespace VFT_SMF
