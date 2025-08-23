/**
 * @file Pilot_001_Strategy.cpp
 * @brief Pilot_001策略实现
 * @author VFT_SMF Development Team
 * @date 2025-08-21
 */

#include "Pilot_001_Strategy.hpp"
#include <iostream>
#include <sstream>

namespace VFT_SMF {

    Pilot_001_Strategy::Pilot_001_Strategy()
        : skill_level(0.7),  // 标准技能水平
          attention_level(0.8),  // 标准注意力水平
          experience_level(PilotExperienceLevel::EXPERIENCED),  // 有经验
          total_operations_performed(0),
          successful_operations(0),
          last_operation_time(0.0),
          gen(rd()),
          dist(0.0, 1.0) {
        // 构造函数初始化
    }

    void Pilot_001_Strategy::initialize(std::shared_ptr<GlobalShared_DataSpace::GlobalSharedDataSpace> data_space,
                                       const std::string& id) {
        shared_data_space = data_space;
        agent_id = id;
        total_operations_performed = 0;
        successful_operations = 0;
        last_operation_time = 0.0;
        
        logBrief(LogLevel::Brief, "Pilot_001 策略初始化完成，代理ID: " + agent_id);
    }

    bool Pilot_001_Strategy::executeTaxiControlController(const std::map<std::string, std::string>& params, 
                                                         double current_time) {
        logPilotAction("taxi_control", "执行标准滑行控制");
        
        if (!validateOperationConditions(current_time)) {
            logBrief(LogLevel::Brief, "Pilot_001: 操作条件验证失败，拒绝滑行控制");
            return false;
        }

        if (!shouldExecuteOperation("taxi_control", current_time)) {
            logBrief(LogLevel::Brief, "Pilot_001: 滑行控制条件不满足");
            return false;
        }

        // 执行滑行控制逻辑
        applyStandardPilotLogic("taxi_control");
        updateOperationMetrics("taxi_control", true);
        
        logBrief(LogLevel::Brief, "Pilot_001: 滑行控制已执行 - 总操作数: " + std::to_string(total_operations_performed));
        return true;
    }

    bool Pilot_001_Strategy::executeTakeoffControlController(const std::map<std::string, std::string>& params, 
                                                           double current_time) {
        logPilotAction("takeoff_control", "执行标准起飞控制");
        
        if (!validateOperationConditions(current_time)) {
            logBrief(LogLevel::Brief, "Pilot_001: 操作条件验证失败，拒绝起飞控制");
            return false;
        }

        if (!shouldExecuteOperation("takeoff_control", current_time)) {
            logBrief(LogLevel::Brief, "Pilot_001: 起飞控制条件不满足");
            return false;
        }

        // 执行起飞控制逻辑
        applyStandardPilotLogic("takeoff_control");
        updateOperationMetrics("takeoff_control", true);
        
        logBrief(LogLevel::Brief, "Pilot_001: 起飞控制已执行 - 总操作数: " + std::to_string(total_operations_performed));
        return true;
    }

    bool Pilot_001_Strategy::executeLandingControlController(const std::map<std::string, std::string>& params, 
                                                           double current_time) {
        logPilotAction("landing_control", "执行标准着陆控制");
        
        if (!validateOperationConditions(current_time)) {
            logBrief(LogLevel::Brief, "Pilot_001: 操作条件验证失败，拒绝着陆控制");
            return false;
        }

        if (!shouldExecuteOperation("landing_control", current_time)) {
            logBrief(LogLevel::Brief, "Pilot_001: 着陆控制条件不满足");
            return false;
        }

        // 执行着陆控制逻辑
        applyStandardPilotLogic("landing_control");
        updateOperationMetrics("landing_control", true);
        
        logBrief(LogLevel::Brief, "Pilot_001: 着陆控制已执行 - 总操作数: " + std::to_string(total_operations_performed));
        return true;
    }

    bool Pilot_001_Strategy::executeEmergencyResponseController(const std::map<std::string, std::string>& params, 
                                                               double current_time) {
        logPilotAction("emergency_response", "执行标准紧急响应");
        
        // 紧急情况下，降低验证标准
        if (!shouldExecuteOperation("emergency_response", current_time)) {
            logBrief(LogLevel::Brief, "Pilot_001: 紧急响应条件不满足，但继续执行");
        }

        // 执行紧急响应逻辑
        applyStandardPilotLogic("emergency_response");
        updateOperationMetrics("emergency_response", true);
        
        logBrief(LogLevel::Brief, "Pilot_001: 紧急响应已执行 - 总操作数: " + std::to_string(total_operations_performed));
        return true;
    }

    bool Pilot_001_Strategy::executeATCCommandResponseController(const std::map<std::string, std::string>& params, 
                                                                double current_time) {
        logPilotAction("atc_command_response", "执行标准ATC指令响应");
        
        if (!validateOperationConditions(current_time)) {
            logBrief(LogLevel::Brief, "Pilot_001: 操作条件验证失败，拒绝ATC指令响应");
            return false;
        }

        if (!shouldExecuteOperation("atc_command_response", current_time)) {
            logBrief(LogLevel::Brief, "Pilot_001: ATC指令响应条件不满足");
            return false;
        }

        // 执行ATC指令响应逻辑
        applyStandardPilotLogic("atc_command_response");
        updateOperationMetrics("atc_command_response", true);
        
        logBrief(LogLevel::Brief, "Pilot_001: ATC指令响应已执行 - 总操作数: " + std::to_string(total_operations_performed));
        return true;
    }

    void Pilot_001_Strategy::update(double delta_time) {
        updatePilotState(delta_time);
    }

    bool Pilot_001_Strategy::validateOperationConditions(double current_time) {
        // 检查基本操作条件
        if (skill_level < 0.3) {
            logBrief(LogLevel::Brief, "Pilot_001: 技能水平过低，无法执行操作");
            return false;
        }
        
        if (attention_level < 0.4) {
            logBrief(LogLevel::Brief, "Pilot_001: 注意力水平过低，无法执行操作");
            return false;
        }
        
        return true;
    }

    void Pilot_001_Strategy::updatePilotState(double delta_time) {
        // 注意力水平随时间缓慢变化（模拟疲劳和恢复）
        double attention_change = (dist(gen) - 0.5) * 0.01 * delta_time;
        attention_level = std::clamp(attention_level + attention_change, 0.3, 1.0);
        
        // 技能水平相对稳定，但可能有微小波动
        double skill_change = (dist(gen) - 0.5) * 0.005 * delta_time;
        skill_level = std::clamp(skill_level + skill_change, 0.5, 0.9);
        
        logDetail(LogLevel::Detail, "Pilot_001 状态更新 - 注意力: " + std::to_string(attention_level) + 
                                   ", 技能: " + std::to_string(skill_level));
    }

    void Pilot_001_Strategy::logPilotAction(const std::string& action_type, const std::string& action) {
        logBrief(LogLevel::Brief, "Pilot_001 策略 (" + agent_id + "): " + action + " - " + action_type);
    }

    void Pilot_001_Strategy::updateOperationMetrics(const std::string& operation_type, bool success) {
        total_operations_performed++;
        if (success) {
            successful_operations++;
        }
        last_operation_time = 0.0; // 重置操作时间
        
        logBrief(LogLevel::Brief, "Pilot_001 策略: 操作 '" + operation_type + "' 完成. " +
                                 "总操作数: " + std::to_string(total_operations_performed) + 
                                 ", 成功率: " + std::to_string(static_cast<double>(successful_operations) / total_operations_performed * 100) + "%");
    }

    bool Pilot_001_Strategy::shouldExecuteOperation(const std::string& operation_type, double current_time) {
        // 检查操作间隔
        if (current_time - last_operation_time < 0.5) { // 最小操作间隔0.5秒
            return false;
        }
        
        // 根据操作类型调整条件
        if (operation_type == "emergency_response") {
            return true; // 紧急响应总是允许
        }
        
        // 其他操作需要满足基本条件
        return skill_level >= 0.5 && attention_level >= 0.6;
    }

    void Pilot_001_Strategy::applyStandardPilotLogic(const std::string& operation_type) {
        // 应用标准飞行员逻辑
        logDetail(LogLevel::Detail, "Pilot_001 策略: 应用标准逻辑到 " + operation_type);
        
        // 这里可以添加具体的飞行员逻辑实现
        // 例如：更新共享数据空间中的飞行员状态
        if (shared_data_space) {
            // 更新飞行员状态数据
            logDetail(LogLevel::Detail, "Pilot_001 策略: 更新共享数据空间状态");
        }
    }

} // namespace VFT_SMF
