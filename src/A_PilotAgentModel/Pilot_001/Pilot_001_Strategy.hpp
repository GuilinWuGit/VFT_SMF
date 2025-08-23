/**
 * @file Pilot_001_Strategy.hpp
 * @brief Pilot_001策略实现 - 标准飞行员控制逻辑
 * @author VFT_SMF Development Team
 * @date 2025-08-21
 * 
 * Pilot_001特点：
 * 1. 标准的飞行员控制逻辑
 * 2. 平衡的技能和注意力水平
 * 3. 常规的操作流程
 * 4. 中等经验水平
 */

#pragma once

#include "../A_StandardBase/IPilotStrategy.hpp"
#include "../PilotAgent.hpp"  // 包含PilotExperienceLevel定义
#include "../../G_SimulationManager/LogAndData/Logger.hpp"
#include <string>
#include <map>
#include <memory>
#include <random>

namespace VFT_SMF {

    /**
     * @brief Pilot_001策略类 - 标准飞行员控制逻辑
     */
    class Pilot_001_Strategy : public IPilotStrategy {
    private:
        std::shared_ptr<GlobalShared_DataSpace::GlobalSharedDataSpace> shared_data_space;
        std::string agent_id;
        
        // Pilot_001 特有的状态变量
        double skill_level;
        double attention_level;
        PilotExperienceLevel experience_level;
        
        // 操作统计
        int total_operations_performed;
        int successful_operations;
        double last_operation_time;
        
        // 随机数生成器（用于模拟真实飞行员的不确定性）
        std::random_device rd;
        std::mt19937 gen;
        std::uniform_real_distribution<double> dist;

    public:
        Pilot_001_Strategy();
        ~Pilot_001_Strategy() override = default;

        // IPilotStrategy接口实现
        std::string getStrategyId() const override { return "Pilot_001"; }
        
        std::string getStrategyDescription() const override { 
            return "标准飞行员策略 - 平衡技能和注意力的常规控制模式"; 
        }

        void initialize(std::shared_ptr<GlobalShared_DataSpace::GlobalSharedDataSpace> data_space,
                       const std::string& id) override;

        bool executeTaxiControlController(const std::map<std::string, std::string>& params, 
                                        double current_time) override;

        bool executeTakeoffControlController(const std::map<std::string, std::string>& params, 
                                          double current_time) override;

        bool executeLandingControlController(const std::map<std::string, std::string>& params, 
                                          double current_time) override;

        bool executeEmergencyResponseController(const std::map<std::string, std::string>& params, 
                                              double current_time) override;

        bool executeATCCommandResponseController(const std::map<std::string, std::string>& params, 
                                               double current_time) override;

        void update(double delta_time) override;
        double getSkillLevel() const override { return skill_level; }
        double getAttentionLevel() const override { return attention_level; }
        PilotExperienceLevel getExperienceLevel() const override { return experience_level; }

    private:
        // Pilot_001 特有的私有方法
        bool validateOperationConditions(double current_time);
        void updatePilotState(double delta_time);
        void logPilotAction(const std::string& action_type, const std::string& action);
        void updateOperationMetrics(const std::string& operation_type, bool success);
        bool shouldExecuteOperation(const std::string& operation_type, double current_time);
        void applyStandardPilotLogic(const std::string& operation_type);
    };

} // namespace VFT_SMF
