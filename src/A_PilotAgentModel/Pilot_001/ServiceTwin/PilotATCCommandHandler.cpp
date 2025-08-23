/**
 * @file PilotATCCommandHandler.cpp
 * @brief 飞行员ATC指令处理器实现
 * @author VFT_SMF Development Team
 * @date 2025-08-21
 */

#include "PilotATCCommandHandler.hpp"
#include <algorithm>

namespace VFT_SMF {

    PilotATCCommandHandler::PilotATCCommandHandler(std::shared_ptr<GlobalShared_DataSpace::GlobalSharedDataSpace> data_space)
        : shared_data_space(data_space) {
        logBrief(LogLevel::Brief, "飞行员ATC指令处理器创建完成");
    }

    void PilotATCCommandHandler::handlePilotATCCommand(const GlobalSharedDataStruct::StandardEvent& event,
                                                       double current_time) {
        
        logBrief(LogLevel::Brief, "飞行员ATC指令处理器: 处理事件 " + event.event_name + 
                " (ID: " + event.getEventIdString() + ") - 时间: " + std::to_string(current_time) + "s");
        
        // 获取事件的驱动过程信息
        const auto& driven_process = event.driven_process;
        const std::string& controller_name = driven_process.controller_name;
        
        // 根据控制器名称执行相应的指令处理
        if (controller_name == "clearance_controller") {
            logPilotAction("收到滑行许可", "开始执行滑行程序");
            executeTaxiClearance(current_time);
            
        } else if (controller_name == "Emergency_Brake_Command") {
            logPilotAction("收到紧急刹车指令", "立即执行紧急刹车");
            executeEmergencyBrake(current_time);
            
        } else {
            logBrief(LogLevel::Brief, "飞行员: 收到未知ATC指令: " + controller_name);
        }
    }

    void PilotATCCommandHandler::executeTaxiClearance(double current_time) {
        // 飞行员接收并确认滑行许可
        logBrief(LogLevel::Brief, "飞行员: 收到滑行许可，开始执行滑行程序");
        
        // 飞行员确认ATC指令的逻辑
        // 这里可以更新飞行员状态，例如设置"已收到滑行许可"标志
        // 同步到共享ATC指令：设置 clearance_granted = true
        auto atc_cmd = shared_data_space->getATCCommand();
        atc_cmd.clearance_granted = true;
        atc_cmd.datasource = "pilot_atc_handler";
        shared_data_space->setATCCommand(atc_cmd);
        
        // 飞行员开始执行滑行操作
        auto flight_state = shared_data_space->getAircraftFlightState();
        if (flight_state.groundspeed < 5.0) {  // 如果速度较低，开始滑行
            flight_state.groundspeed += 0.5;  // 缓慢开始滑行
            flight_state.airspeed += 0.3;
            shared_data_space->setAircraftFlightState(flight_state);
            
            logBrief(LogLevel::Brief, "飞行员: 开始滑行，当前地速: " + std::to_string(flight_state.groundspeed) + " m/s");
        }
    }

    void PilotATCCommandHandler::executeEmergencyBrake(double current_time) {
        // 飞行员接收紧急刹车指令
        logBrief(LogLevel::Brief, "飞行员: 收到紧急刹车指令，立即执行紧急刹车");
        
        // 飞行员执行紧急刹车
        auto flight_state = shared_data_space->getAircraftFlightState();
        flight_state.airspeed = std::max(0.0, flight_state.airspeed - 8.0);  // 紧急减速
        flight_state.groundspeed = std::max(0.0, flight_state.groundspeed - 6.0);
        shared_data_space->setAircraftFlightState(flight_state);
        
        auto system_state = shared_data_space->getAircraftSystemState();
        system_state.current_brake_pressure = 2500000.0;  // 最大刹车压力
        shared_data_space->setAircraftSystemState(system_state);

        // 同步到共享ATC指令：设置 emergency_brake = true
        auto atc_cmd = shared_data_space->getATCCommand();
        atc_cmd.emergency_brake = true;
        atc_cmd.datasource = "pilot_atc_handler";
        shared_data_space->setATCCommand(atc_cmd);
        
        logBrief(LogLevel::Brief, "飞行员: 紧急刹车执行完成，当前地速: " + std::to_string(flight_state.groundspeed) + " m/s");
    }

    void PilotATCCommandHandler::logPilotAction(const std::string& action, const std::string& details) {
        logBrief(LogLevel::Brief, "飞行员: " + action + " - " + details);
    }

} // namespace VFT_SMF
