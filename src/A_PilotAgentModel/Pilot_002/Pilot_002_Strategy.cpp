/**
 * @file Pilot_002_Strategy.cpp
 * @brief Pilot_002策略实现
 * @author VFT_SMF Development Team
 * @date 2025-08-21
 */

#include "Pilot_002_Strategy.hpp"
#include <iostream>
#include <sstream>

namespace VFT_SMF {

    Pilot_002_Strategy::Pilot_002_Strategy()
        : skill_level(0.9),  // 高技能水平
          attention_level(0.95),  // 高注意力水平
          experience_level(PilotExperienceLevel::EXPERT),  // 专家级
          total_operations_performed(0),
          successful_operations(0),
          last_operation_time(0.0),
          situation_awareness(0.95),  // 高情境感知
          decision_speed(0.9),        // 快速决策
          stress_tolerance(0.95),     // 高压力承受
          fatigue_resistance(0.9),    // 高疲劳抵抗
          gen(rd()),
          dist(0.0, 1.0) {
        // 构造函数初始化
    }

    void Pilot_002_Strategy::initialize(std::shared_ptr<GlobalShared_DataSpace::GlobalSharedDataSpace> data_space,
                                       const std::string& id) {
        shared_data_space = data_space;
        agent_id = id;
        total_operations_performed = 0;
        successful_operations = 0;
        last_operation_time = 0.0;
        
        logBrief(LogLevel::Brief, "Pilot_002 策略初始化完成，代理ID: " + agent_id + " - 专家模式已启用");
    }

    bool Pilot_002_Strategy::executeTaxiControlController(const std::map<std::string, std::string>& params, 
                                                         double current_time) {
        logPilotAction("taxi_control", "执行专家级滑行控制");
        
        if (!validateExpertOperationConditions(current_time)) {
            logBrief(LogLevel::Brief, "Pilot_002: 专家操作条件验证失败，拒绝滑行控制");
            return false;
        }

        if (!shouldExecuteExpertOperation("taxi_control", current_time)) {
            logBrief(LogLevel::Brief, "Pilot_002: 滑行控制条件不满足");
            return false;
        }

        // 执行专家级滑行控制逻辑
        applyExpertPilotLogic("taxi_control");
        updateOperationMetrics("taxi_control", true);
        
        logBrief(LogLevel::Brief, "Pilot_002: 专家级滑行控制已执行 - 总操作数: " + std::to_string(total_operations_performed));
        return true;
    }

    bool Pilot_002_Strategy::executeTakeoffControlController(const std::map<std::string, std::string>& params, 
                                                           double current_time) {
        logPilotAction("takeoff_control", "执行专家级起飞控制");
        
        if (!validateExpertOperationConditions(current_time)) {
            logBrief(LogLevel::Brief, "Pilot_002: 专家操作条件验证失败，拒绝起飞控制");
            return false;
        }

        if (!shouldExecuteExpertOperation("takeoff_control", current_time)) {
            logBrief(LogLevel::Brief, "Pilot_002: 起飞控制条件不满足");
            return false;
        }

        // 执行专家级起飞控制逻辑
        applyExpertPilotLogic("takeoff_control");
        updateOperationMetrics("takeoff_control", true);
        
        logBrief(LogLevel::Brief, "Pilot_002: 专家级起飞控制已执行 - 总操作数: " + std::to_string(total_operations_performed));
        return true;
    }

    bool Pilot_002_Strategy::executeLandingControlController(const std::map<std::string, std::string>& params, 
                                                           double current_time) {
        logPilotAction("landing_control", "执行专家级着陆控制");
        
        if (!validateExpertOperationConditions(current_time)) {
            logBrief(LogLevel::Brief, "Pilot_002: 专家操作条件验证失败，拒绝着陆控制");
            return false;
        }

        if (!shouldExecuteExpertOperation("landing_control", current_time)) {
            logBrief(LogLevel::Brief, "Pilot_002: 着陆控制条件不满足");
            return false;
        }

        // 执行专家级着陆控制逻辑
        applyExpertPilotLogic("landing_control");
        updateOperationMetrics("landing_control", true);
        
        logBrief(LogLevel::Brief, "Pilot_002: 专家级着陆控制已执行 - 总操作数: " + std::to_string(total_operations_performed));
        return true;
    }

    bool Pilot_002_Strategy::executeEmergencyResponseController(const std::map<std::string, std::string>& params, 
                                                               double current_time) {
        logPilotAction("emergency_response", "执行专家级紧急响应");
        
        // 专家级飞行员在紧急情况下表现更出色
        if (!validateExpertOperationConditions(current_time)) {
            logBrief(LogLevel::Brief, "Pilot_002: 专家条件验证失败，但专家级飞行员仍能处理紧急情况");
        }

        // 执行专家级紧急响应逻辑
        applyExpertPilotLogic("emergency_response");
        updateOperationMetrics("emergency_response", true);
        
        logBrief(LogLevel::Brief, "Pilot_002: 专家级紧急响应已执行 - 总操作数: " + std::to_string(total_operations_performed));
        return true;
    }

    bool Pilot_002_Strategy::executeATCCommandResponseController(const std::map<std::string, std::string>& params, 
                                                                double current_time) {
        logPilotAction("atc_command_response", "执行专家级ATC指令响应");
        
        if (!validateExpertOperationConditions(current_time)) {
            logBrief(LogLevel::Brief, "Pilot_002: 专家操作条件验证失败，拒绝ATC指令响应");
            return false;
        }

        if (!shouldExecuteExpertOperation("atc_command_response", current_time)) {
            logBrief(LogLevel::Brief, "Pilot_002: ATC指令响应条件不满足");
            return false;
        }

        // 执行专家级ATC指令响应逻辑
        applyExpertPilotLogic("atc_command_response");
        updateOperationMetrics("atc_command_response", true);
        
        logBrief(LogLevel::Brief, "Pilot_002: 专家级ATC指令响应已执行 - 总操作数: " + std::to_string(total_operations_performed));
        return true;
    }

    void Pilot_002_Strategy::update(double delta_time) {
        updateExpertPilotState(delta_time);
    }

    bool Pilot_002_Strategy::validateExpertOperationConditions(double current_time) {
        // 专家级飞行员的操作条件更严格
        if (skill_level < 0.7) {
            logBrief(LogLevel::Brief, "Pilot_002: 技能水平过低，无法执行专家级操作");
            return false;
        }
        
        if (attention_level < 0.7) {
            logBrief(LogLevel::Brief, "Pilot_002: 注意力水平过低，无法执行专家级操作");
            return false;
        }
        
        if (situation_awareness < 0.6) {
            logBrief(LogLevel::Brief, "Pilot_002: 情境感知能力不足，无法执行专家级操作");
            return false;
        }
        
        return true;
    }

    void Pilot_002_Strategy::updateExpertPilotState(double delta_time) {
        // 专家级飞行员的注意力水平更稳定
        double attention_change = (dist(gen) - 0.5) * 0.005 * delta_time; // 更小的变化
        attention_level = std::clamp(attention_level + attention_change, 0.7, 1.0);
        
        // 技能水平保持在高水平
        double skill_change = (dist(gen) - 0.5) * 0.002 * delta_time; // 更小的波动
        skill_level = std::clamp(skill_level + skill_change, 0.8, 1.0);
        
        // 情境感知能力随时间缓慢提升
        double awareness_change = (dist(gen) - 0.4) * 0.003 * delta_time; // 偏向提升
        situation_awareness = std::clamp(situation_awareness + awareness_change, 0.8, 1.0);
        
        logDetail(LogLevel::Detail, "Pilot_002 专家状态更新 - 注意力: " + std::to_string(attention_level) + 
                                   ", 技能: " + std::to_string(skill_level) + 
                                   ", 情境感知: " + std::to_string(situation_awareness));
    }

    void Pilot_002_Strategy::logPilotAction(const std::string& action_type, const std::string& action) {
        logBrief(LogLevel::Brief, "Pilot_002 专家策略 (" + agent_id + "): " + action + " - " + action_type);
    }

    void Pilot_002_Strategy::updateOperationMetrics(const std::string& operation_type, bool success) {
        total_operations_performed++;
        if (success) {
            successful_operations++;
        }
        last_operation_time = 0.0; // 重置操作时间
        
        logBrief(LogLevel::Brief, "Pilot_002 专家策略: 操作 '" + operation_type + "' 完成. " +
                                 "总操作数: " + std::to_string(total_operations_performed) + 
                                 ", 成功率: " + std::to_string(static_cast<double>(successful_operations) / total_operations_performed * 100) + "%");
    }

    bool Pilot_002_Strategy::shouldExecuteExpertOperation(const std::string& operation_type, double current_time) {
        // 专家级飞行员的操作间隔更短
        if (current_time - last_operation_time < 0.2) { // 最小操作间隔0.2秒
            return false;
        }
        
        // 根据操作类型调整条件
        if (operation_type == "emergency_response") {
            return true; // 紧急响应总是允许
        }
        
        // 专家级操作需要满足更高条件
        return skill_level >= 0.7 && attention_level >= 0.8 && situation_awareness >= 0.7;
    }

    void Pilot_002_Strategy::applyExpertPilotLogic(const std::string& operation_type) {
        // 应用专家级飞行员逻辑
        logDetail(LogLevel::Detail, "Pilot_002 专家策略: 应用专家级逻辑到 " + operation_type);
        
        // 计算专家级决策时间
        double decision_time = calculateExpertDecisionTime(operation_type);
        logDetail(LogLevel::Detail, "Pilot_002 专家策略: 决策时间 " + std::to_string(decision_time) + " 秒");
        
        // 执行情境评估
        if (performExpertSituationAssessment(0.0)) {
            logDetail(LogLevel::Detail, "Pilot_002 专家策略: 情境评估通过");
        }
        
        // 这里可以添加具体的专家级飞行员逻辑实现
        if (shared_data_space) {
            // 更新共享数据空间中的飞行员状态
            logDetail(LogLevel::Detail, "Pilot_002 专家策略: 更新共享数据空间状态");
        }
    }

    double Pilot_002_Strategy::calculateExpertDecisionTime(const std::string& operation_type) {
        // 专家级飞行员的决策时间更短
        double base_time = 0.1; // 基础决策时间0.1秒
        
        // 根据操作类型调整
        if (operation_type == "emergency_response") {
            return base_time * 0.5; // 紧急响应更快
        } else if (operation_type == "taxi_control") {
            return base_time * 0.8; // 滑行控制较快
        } else {
            return base_time; // 其他操作使用基础时间
        }
    }

    bool Pilot_002_Strategy::performExpertSituationAssessment(double current_time) {
        // 专家级飞行员的情境评估更准确
        double assessment_accuracy = situation_awareness * decision_speed;
        
        // 模拟评估结果
        bool assessment_result = (dist(gen) < assessment_accuracy);
        
        logDetail(LogLevel::Detail, "Pilot_002 专家策略: 情境评估准确度 " + std::to_string(assessment_accuracy) + 
                                   ", 结果: " + (assessment_result ? "通过" : "失败"));
        
        return assessment_result;
    }

} // namespace VFT_SMF
