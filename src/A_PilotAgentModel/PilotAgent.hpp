/**
 * @file PilotAgent.hpp
 * @brief 飞行员代理模型定义
 * @author VFT_SMF Development Team
 * @date 2024
 * 
 * 飞行员代理模型负责模拟飞行员的基本状态
 */

#pragma once

#include "../F_ScenarioModelling/B_ScenarioModel/VFT_SMF_Base.hpp"
#include "A_StandardBase/IPilotStrategy.hpp"
#include <vector>
#include <random>
#include <algorithm>
#include <memory>

namespace VFT_SMF {

    /**
     * @brief 飞行员分类枚举：新手、中级、有经验、专家、大师级
     */
    enum class PilotExperienceLevel {
        NOVICE = 1,         ///< 新手
        INTERMEDIATE = 2,   ///< 中级
        EXPERIENCED = 3,    ///< 有经验
        EXPERT = 4,         ///< 专家
        MASTER = 5          ///< 大师级
    };

     /**
     * @brief 飞行员手动操纵影响因素结构体：由经验水平和注意力水平决定，在飞行员的模型层。
     */
    struct PilotManualControlImpact {
        double delay_time; // 延迟时间（秒）
        double target_accuracy; // 操作目标精准度（0-1）
        double impact_probability; // 影响概率（0-1）
        double action_jitter; // 动作抖动幅度（0~0.1）
    };

    /**
     * @brief 飞行员决策影响因素结构体：由经验水平和注意力水平决定，在飞行员的模型层。
     */
    struct PilotDecisionImpact {
        double delay_time; // 延迟时间（秒），影响飞行员的决策速度
    };


    /**
     * @brief 飞行员代理类
     */
    class PilotAgent : public BaseAgent {
    private:
        // 基本状态参数
        double skill_level; // 技能水平（0-1）
        double attention_level; // 注意力水平（0-1）
        
        // 影响因子
        PilotManualControlImpact manual_control_impact;
        PilotDecisionImpact decision_impact;
        
        // 随机数生成器
        std::random_device rd;
        std::mt19937 gen;
        std::uniform_real_distribution<double> dist;
        
        // 飞行员策略（根据配置的Pilot_ID动态选择）
        std::unique_ptr<IPilotStrategy> pilot_strategy;

    public:
        PilotAgent(const std::string& id, const std::string& name);
        ~PilotAgent() override = default;

        // BaseAgent接口实现
        void initialize() override;
        void start() override;
        void pause() override;
        void resume() override;
        void stop() override;
        void update(double delta_time) override;
        void handle_event(const Event& event) override;
        void send_event(const Event& event) override;
        std::string get_status() const override;
        bool is_ready() const override;

        // 简化的飞行员方法
        // 手动操纵影响因子计算，输入参数是skill_level和attention_level，输出参数是PilotManualControlImpact
        PilotManualControlImpact calculate_manual_control_impact(double skill_level, double attention_level) {
            PilotManualControlImpact impact;
            impact.delay_time = 0.0;
            impact.target_accuracy = 0.0;
            impact.impact_probability = 0.0;
            impact.action_jitter = 0.0;
            return impact;
        }

        // 决策影响因子计算，输入参数是skill_level和attention_level，输出参数是PilotDecisionImpact
        PilotDecisionImpact calculate_decision_impact(double skill_level, double attention_level) {
            PilotDecisionImpact impact;
            impact.delay_time = 0.0;
            return impact;
        }
        
        // Getter方法
        double get_skill_level() const { return skill_level; } // 获取实时的技能水平
        double get_attention_level() const { return attention_level; } // 获取实时的注意力水平
        PilotManualControlImpact get_manual_control_impact() const { return manual_control_impact; } // 获取实时的手动操纵影响因子
        PilotDecisionImpact get_decision_impact() const { return decision_impact; } // 获取实时的决策影响因子
        
        // 策略管理方法
        void setPilotStrategy(std::unique_ptr<IPilotStrategy> strategy);
        void initializePilotStrategy(const std::string& pilot_id);
        IPilotStrategy* getPilotStrategy() const { return pilot_strategy.get(); }
        std::string getStrategyConfig() const;
        
        // 控制器执行方法（委托给策略）
        bool executeController(const std::string& controller_name, const std::map<std::string, std::string>& params, double current_time);
        
    private:
        // 从飞行员配置文件加载配置
        void load_pilot_config();
    };

} // namespace VFT_SMF 