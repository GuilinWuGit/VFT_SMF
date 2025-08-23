/**
 * @file ATC_001_Strategy.cpp
 * @brief ATC_001策略实现
 * @author VFT_SMF Development Team
 * @date 2025-08-21
 */

#include "ATC_001_Strategy.hpp"

namespace VFT_SMF {

    void ATC_001_Strategy::initialize(std::shared_ptr<GlobalShared_DataSpace::GlobalSharedDataSpace> data_space,
                                     const std::string& id) {
        shared_data_space = data_space;
        agent_id = id;
        total_clearances_issued = 0;
        emergency_interventions = 0;
        last_update_time = 0.0;
        
        logBrief(LogLevel::Brief, "ATC_001策略初始化完成 - 代理ID: " + agent_id);
    }

    bool ATC_001_Strategy::executeClearanceController(const std::map<std::string, std::string>& params, 
                                                     double current_time) {
        logATCAction("滑行许可控制器", "ATC_001标准模式执行");
        
        // ATC_001 标准滑行许可逻辑
        if (!validateStandardConditions(current_time)) {
            logBrief(LogLevel::Brief, "ATC_001: 标准条件验证失败，拒绝滑行许可");
            return false;
        }

        if (!checkAircraftStatus()) {
            logBrief(LogLevel::Brief, "ATC_001: 飞机状态检查失败，拒绝滑行许可");
            return false;
        }

        // 发布滑行许可
        updateATCCommand("clearance_granted", true);
        total_clearances_issued++;
        
        logBrief(LogLevel::Brief, "ATC_001: 滑行许可已发布 - 总许可数: " + std::to_string(total_clearances_issued));
        return true;
    }

    bool ATC_001_Strategy::executeEmergencyBrakeController(const std::map<std::string, std::string>& params, 
                                                          double current_time) {
        logATCAction("紧急刹车控制器", "ATC_001标准模式执行 - 立即响应");
        
        // ATC_001 紧急刹车逻辑 - 立即执行，无条件检查
        updateATCCommand("emergency_brake", true);
        emergency_interventions++;
        
        logBrief(LogLevel::Brief, "ATC_001: 紧急刹车指令已发布 - 总紧急干预次数: " + std::to_string(emergency_interventions));
        return true;
    }

    bool ATC_001_Strategy::executeTakeoffClearanceController(const std::map<std::string, std::string>& params, 
                                                            double current_time) {
        logATCAction("起飞许可控制器", "ATC_001标准模式执行");
        
        // ATC_001 标准起飞许可逻辑
        if (!validateStandardConditions(current_time)) {
            logBrief(LogLevel::Brief, "ATC_001: 标准条件验证失败，拒绝起飞许可");
            return false;
        }

        // 检查起飞条件
        auto flight_state = shared_data_space->getAircraftFlightState();
        if (flight_state.airspeed > 5.0) {  // 标准阈值
            logBrief(LogLevel::Brief, "ATC_001: 飞机速度过高，拒绝起飞许可");
            return false;
        }

        updateATCCommand("clearance_granted", true);
        total_clearances_issued++;
        
        logBrief(LogLevel::Brief, "ATC_001: 起飞许可已发布");
        return true;
    }

    bool ATC_001_Strategy::executeLandingClearanceController(const std::map<std::string, std::string>& params, 
                                                            double current_time) {
        logATCAction("着陆许可控制器", "ATC_001标准模式执行");
        
        // ATC_001 标准着陆许可逻辑
        if (!validateStandardConditions(current_time)) {
            logBrief(LogLevel::Brief, "ATC_001: 标准条件验证失败，拒绝着陆许可");
            return false;
        }

        // 检查着陆条件
        auto flight_state = shared_data_space->getAircraftFlightState();
        if (flight_state.altitude > 200.0) {  // 标准阈值
            logBrief(LogLevel::Brief, "ATC_001: 飞机高度过高，拒绝着陆许可");
            return false;
        }

        updateATCCommand("clearance_granted", true);
        total_clearances_issued++;
        
        logBrief(LogLevel::Brief, "ATC_001: 着陆许可已发布");
        return true;
    }

    std::map<std::string, std::string> ATC_001_Strategy::getStrategyConfig() const {
        std::map<std::string, std::string> config;
        config["strategy_id"] = "ATC_001";
        config["mode"] = "standard";
        config["safety_level"] = "normal";
        config["efficiency_priority"] = "balanced";
        config["takeoff_speed_threshold"] = "5.0";
        config["landing_altitude_threshold"] = "200.0";
        return config;
    }

    std::string ATC_001_Strategy::getPerformanceStats() const {
        return "ATC_001性能统计: 总许可数=" + std::to_string(total_clearances_issued) + 
               ", 紧急干预次数=" + std::to_string(emergency_interventions) + 
               ", 最后更新时间=" + std::to_string(last_update_time);
    }

    bool ATC_001_Strategy::validateStandardConditions(double current_time) {
        // ATC_001 标准条件验证
        last_update_time = current_time;
        
        // 基础时间检查
        if (current_time < 0.0) {
            return false;
        }

        // 检查共享数据空间状态
        if (!shared_data_space) {
            return false;
        }

        return true;
    }

    void ATC_001_Strategy::logATCAction(const std::string& action, const std::string& details) {
        logBrief(LogLevel::Brief, "ATC_001 " + action + ": " + details);
    }

    void ATC_001_Strategy::updateATCCommand(const std::string& command_type, bool value) {
        auto current_atc_command = shared_data_space->getATCCommand();
        
        if (command_type == "clearance_granted") {
            current_atc_command.clearance_granted = value;
        } else if (command_type == "emergency_brake") {
            current_atc_command.emergency_brake = value;
        }
        
        // 使用代理ID作为数据源
        shared_data_space->setATCCommand(current_atc_command, agent_id + "_standard_strategy");
        
        logBrief(LogLevel::Brief, "ATC_001 指令状态更新: " + command_type + " = " + (value ? "true" : "false"));
    }

    bool ATC_001_Strategy::checkAircraftStatus() {
        auto flight_state = shared_data_space->getAircraftFlightState();
        auto system_state = shared_data_space->getAircraftSystemState();
        
        // ATC_001 标准飞机状态检查
        if (flight_state.groundspeed > 30.0) {  // 标准速度阈值
            logBrief(LogLevel::Brief, "ATC_001: 飞机地面速度过高");
            return false;
        }
        
        if (system_state.current_brake_pressure < 50000.0) {  // 标准刹车压力阈值
            logBrief(LogLevel::Brief, "ATC_001: 刹车压力不足");
            return false;
        }
        
        return true;
    }

} // namespace VFT_SMF

