/**
 * @file ATC_002_Strategy.cpp
 * @brief ATC_002策略实现 - 严格ATC管理逻辑
 * @author VFT_SMF Development Team
 * @date 2025-08-21
 */

#include "ATC_002_Strategy.hpp"

namespace VFT_SMF {

    void ATC_002_Strategy::initialize(std::shared_ptr<GlobalShared_DataSpace::GlobalSharedDataSpace> data_space,
                                     const std::string& id) {
        shared_data_space = data_space;
        agent_id = id;
        strict_mode_enabled = true;
        last_safety_check_time = 0.0;
        total_commands_issued = 0;
        safety_violations_detected = 0;
        clearances_denied = 0;
        
        logBrief(LogLevel::Brief, "ATC_002策略初始化完成 - 严格模式已启用 - 代理ID: " + agent_id);
    }

    bool ATC_002_Strategy::executeClearanceController(const std::map<std::string, std::string>& params, 
                                                     double current_time) {
        logATCAction("滑行许可控制器", "ATC_002严格模式执行");
        
        // 执行严格安全检查
        if (!performStrictSafetyCheck(current_time)) {
            clearances_denied++;
            logBrief(LogLevel::Brief, "ATC_002: 严格安全检查失败，拒绝滑行许可");
            return false;
        }

        // 严格条件验证
        if (!validateStrictConditions("taxi", current_time)) {
            clearances_denied++;
            logBrief(LogLevel::Brief, "ATC_002: 严格条件验证失败，拒绝滑行许可");
            return false;
        }

        // 判断是否应该发布许可
        if (!shouldIssueClearance("taxi", current_time)) {
            clearances_denied++;
            logBrief(LogLevel::Brief, "ATC_002: 安全评估不通过，拒绝滑行许可");
            return false;
        }

        // 发布滑行许可
        updateATCCommand("clearance_granted", true);
        applyStrictModeLogic("taxi_clearance");
        total_commands_issued++;
        
        logBrief(LogLevel::Brief, "ATC_002: 滑行许可已发布（严格模式） - 总指令数: " + std::to_string(total_commands_issued));
        return true;
    }

    bool ATC_002_Strategy::executeEmergencyBrakeController(const std::map<std::string, std::string>& params, 
                                                          double current_time) {
        logATCAction("紧急刹车控制器", "ATC_002严格模式执行 - 立即响应");
        
        // 紧急情况 - 立即执行，但记录详细信息
        updateATCCommand("emergency_brake", true);
        applyStrictModeLogic("emergency_brake");
        total_commands_issued++;
        
        // 记录紧急情况
        safety_violations_detected++;
        
        logBrief(LogLevel::Brief, "ATC_002: 紧急刹车指令已发布（严格模式） - 安全违规检测数: " + std::to_string(safety_violations_detected));
        return true;
    }

    bool ATC_002_Strategy::executeTakeoffClearanceController(const std::map<std::string, std::string>& params, 
                                                            double current_time) {
        logATCAction("起飞许可控制器", "ATC_002严格模式执行");
        
        // 执行严格安全检查
        if (!performStrictSafetyCheck(current_time)) {
            clearances_denied++;
            logBrief(LogLevel::Brief, "ATC_002: 严格安全检查失败，拒绝起飞许可");
            return false;
        }

        // 严格条件验证
        if (!validateStrictConditions("takeoff", current_time)) {
            clearances_denied++;
            logBrief(LogLevel::Brief, "ATC_002: 严格条件验证失败，拒绝起飞许可");
            return false;
        }

        // 起飞需要更严格的条件
        auto flight_state = shared_data_space->getAircraftFlightState();
        if (flight_state.airspeed > 0.5) {  // 更严格的阈值
            clearances_denied++;
            logBrief(LogLevel::Brief, "ATC_002: 飞机速度超过严格阈值，拒绝起飞许可");
            return false;
        }

        // 额外的时间要求
        if (current_time < 15.0) {  // 至少15秒后才允许起飞
            clearances_denied++;
            logBrief(LogLevel::Brief, "ATC_002: 起飞时间过早，拒绝起飞许可");
            return false;
        }

        updateATCCommand("clearance_granted", true);
        applyStrictModeLogic("takeoff_clearance");
        total_commands_issued++;
        
        logBrief(LogLevel::Brief, "ATC_002: 起飞许可已发布（严格验证通过）");
        return true;
    }

    bool ATC_002_Strategy::executeLandingClearanceController(const std::map<std::string, std::string>& params, 
                                                            double current_time) {
        logATCAction("着陆许可控制器", "ATC_002严格模式执行");
        
        // 执行严格安全检查
        if (!performStrictSafetyCheck(current_time)) {
            clearances_denied++;
            logBrief(LogLevel::Brief, "ATC_002: 严格安全检查失败，拒绝着陆许可");
            return false;
        }

        // 严格条件验证
        if (!validateStrictConditions("landing", current_time)) {
            clearances_denied++;
            logBrief(LogLevel::Brief, "ATC_002: 严格条件验证失败，拒绝着陆许可");
            return false;
        }

        // 着陆需要更严格的条件
        auto flight_state = shared_data_space->getAircraftFlightState();
        if (flight_state.altitude > 100.0) {  // 更严格的高度阈值
            clearances_denied++;
            logBrief(LogLevel::Brief, "ATC_002: 飞机高度超过严格阈值，拒绝着陆许可");
            return false;
        }

        if (flight_state.airspeed > 25.0) {  // 更严格的速度阈值
            clearances_denied++;
            logBrief(LogLevel::Brief, "ATC_002: 飞机速度超过严格阈值，拒绝着陆许可");
            return false;
        }

        updateATCCommand("clearance_granted", true);
        applyStrictModeLogic("landing_clearance");
        total_commands_issued++;
        
        logBrief(LogLevel::Brief, "ATC_002: 着陆许可已发布（严格验证通过）");
        return true;
    }

    std::map<std::string, std::string> ATC_002_Strategy::getStrategyConfig() const {
        std::map<std::string, std::string> config;
        config["strategy_id"] = "ATC_002";
        config["mode"] = "strict";
        config["safety_level"] = "high";
        config["efficiency_priority"] = "safety_first";
        config["takeoff_speed_threshold"] = "0.5";
        config["landing_altitude_threshold"] = "100.0";
        config["landing_speed_threshold"] = "25.0";
        config["safety_check_interval"] = std::to_string(safety_check_interval);
        config["minimum_takeoff_time"] = "15.0";
        return config;
    }

    std::string ATC_002_Strategy::getPerformanceStats() const {
        return "ATC_002性能统计: 总指令数=" + std::to_string(total_commands_issued) + 
               ", 安全违规检测数=" + std::to_string(safety_violations_detected) + 
               ", 拒绝许可次数=" + std::to_string(clearances_denied) + 
               ", 严格模式=" + (strict_mode_enabled ? "启用" : "禁用");
    }

    bool ATC_002_Strategy::performStrictSafetyCheck(double current_time) {
        // 频繁的安全检查
        if (current_time - last_safety_check_time > safety_check_interval) {
            last_safety_check_time = current_time;
            
            // 检查飞机状态
            auto flight_state = shared_data_space->getAircraftFlightState();
            auto system_state = shared_data_space->getAircraftSystemState();
            
            // 严格的安全检查条件
            if (flight_state.airspeed > 40.0) {  // 更严格的速度限制
                safety_violations_detected++;
                logBrief(LogLevel::Brief, "ATC_002 安全检查: 空速过高警告 - " + std::to_string(flight_state.airspeed));
                return false;
            }
            
            if (system_state.current_brake_pressure < 80000.0) {  // 更严格的刹车压力要求
                safety_violations_detected++;
                logBrief(LogLevel::Brief, "ATC_002 安全检查: 刹车压力不足警告 - " + std::to_string(system_state.current_brake_pressure));
                return false;
            }
            
            // 检查发动机状态
            if (system_state.left_engine_failed || system_state.right_engine_failed) {
                safety_violations_detected++;
                logBrief(LogLevel::Brief, "ATC_002 安全检查: 发动机故障检测");
                return false;
            }
            
            logBrief(LogLevel::Brief, "ATC_002 严格安全检查通过 - 时间: " + std::to_string(current_time) + "s");
        }
        
        return true;
    }

    bool ATC_002_Strategy::validateStrictConditions(const std::string& operation_type, double current_time) {
        if (!checkAdvancedAircraftStatus()) {
            return false;
        }
        
        // 基础条件检查
        if (current_time < 0.0 || !shared_data_space) {
            return false;
        }
        
        // 针对不同操作类型的特殊验证
        if (operation_type == "takeoff") {
            auto flight_state = shared_data_space->getAircraftFlightState();
            if (flight_state.groundspeed > 0.1) {  // 起飞前必须完全静止
                logBrief(LogLevel::Brief, "ATC_002: 起飞验证失败 - 飞机未完全静止");
                return false;
            }
        }
        
        updateSafetyMetrics();
        return true;
    }

    bool ATC_002_Strategy::shouldIssueClearance(const std::string& clearance_type, double current_time) {
        auto flight_state = shared_data_space->getAircraftFlightState();
        
        if (clearance_type == "taxi") {
            // 滑行许可：需要飞机完全静止
            return flight_state.airspeed < 0.05 && flight_state.groundspeed < 0.05;
        } else if (clearance_type == "takeoff") {
            // 起飞许可：更严格的条件
            return flight_state.airspeed == 0.0 && flight_state.groundspeed == 0.0 && current_time > 15.0;
        } else if (clearance_type == "landing") {
            // 着陆许可：严格的高度和速度条件
            return flight_state.altitude < 100.0 && flight_state.airspeed < 25.0;
        }
        
        return false;
    }

    void ATC_002_Strategy::applyStrictModeLogic(const std::string& command_type) {
        if (strict_mode_enabled) {
            logBrief(LogLevel::Brief, "ATC_002 严格模式: 应用额外安全措施 - " + command_type);
            
            // 额外的安全措施
            if (command_type.find("clearance") != std::string::npos) {
                logBrief(LogLevel::Brief, "ATC_002: 执行许可确认程序");
                // 可以在这里添加额外的确认步骤
            }
        }
    }

    void ATC_002_Strategy::logATCAction(const std::string& action, const std::string& details) {
        logBrief(LogLevel::Brief, "ATC_002 " + action + ": " + details);
    }

    void ATC_002_Strategy::updateATCCommand(const std::string& command_type, bool value) {
        auto current_atc_command = shared_data_space->getATCCommand();
        
        if (command_type == "clearance_granted") {
            current_atc_command.clearance_granted = value;
        } else if (command_type == "emergency_brake") {
            current_atc_command.emergency_brake = value;
        }
        
        // 使用代理ID作为数据源，标识为严格策略
        shared_data_space->setATCCommand(current_atc_command, agent_id + "_strict_strategy");
        
        logBrief(LogLevel::Brief, "ATC_002 指令状态更新: " + command_type + " = " + (value ? "true" : "false"));
    }

    bool ATC_002_Strategy::checkAdvancedAircraftStatus() {
        auto flight_state = shared_data_space->getAircraftFlightState();
        auto system_state = shared_data_space->getAircraftSystemState();
        
        // ATC_002 严格飞机状态检查
        if (flight_state.groundspeed > 15.0) {  // 更严格的速度阈值
            logBrief(LogLevel::Brief, "ATC_002: 飞机地面速度超过严格限制");
            return false;
        }
        
        if (system_state.current_brake_pressure < 80000.0) {  // 更严格的刹车压力阈值
            logBrief(LogLevel::Brief, "ATC_002: 刹车压力不满足严格要求");
            return false;
        }
        
        // 检查刹车效率
        if (system_state.brake_efficiency < 0.8) {  // 要求较高的刹车效率
            logBrief(LogLevel::Brief, "ATC_002: 刹车效率不满足严格要求");
            return false;
        }
        
        return true;
    }

    void ATC_002_Strategy::updateSafetyMetrics() {
        // 更新安全指标
        static int update_counter = 0;
        update_counter++;
        
        // 每10次更新输出一次详细统计
        if (update_counter % 10 == 0) {
            logBrief(LogLevel::Brief, getPerformanceStats());
        }
    }

} // namespace VFT_SMF

