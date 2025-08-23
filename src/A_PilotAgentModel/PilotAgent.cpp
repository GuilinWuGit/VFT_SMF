/**
 * @file PilotAgent.cpp
 * @brief 飞行员代理实现
 * @author VFT_SMF Development Team
 * @date 2024
 */

#include "PilotAgent.hpp"
#include "Pilot_001/Pilot_001_Strategy.hpp"
#include "Pilot_002/Pilot_002_Strategy.hpp"
#include "../G_SimulationManager/LogAndData/Logger.hpp"
#include <iostream>
#include <sstream>

namespace VFT_SMF {

    PilotAgent::PilotAgent(const std::string& id, const std::string& name)
        : skill_level(0.6), // 默认值，将从飞行员配置文件中读取
          attention_level(1.0),
          gen(rd()),
          dist(0.0, 1.0) {
        agent_id = id;
        agent_name = name;
        is_running = false;
        current_state = AgentState::UNINITIALIZED;
        
        // 从飞行员配置文件读取经验水平
        load_pilot_config();
        
        // 初始化影响因子
        manual_control_impact = calculate_manual_control_impact(skill_level, attention_level);
        decision_impact = calculate_decision_impact(skill_level, attention_level);
        
        VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "飞行员代理创建完成: " + name);
    }

    void PilotAgent::initialize() {
        VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "飞行员代理初始化: " + get_agent_name());
        set_current_state(AgentState::READY);
    }

    void PilotAgent::start() {
        VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "飞行员代理启动: " + get_agent_name());
        set_current_state(AgentState::RUNNING);
    }

    void PilotAgent::pause() {
        VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "飞行员代理暂停: " + get_agent_name());
        set_current_state(AgentState::PAUSED);
    }

    void PilotAgent::resume() {
        VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "飞行员代理恢复: " + get_agent_name());
        set_current_state(AgentState::RUNNING);
    }

    void PilotAgent::stop() {
        VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "飞行员代理停止: " + get_agent_name());
        set_current_state(AgentState::STOPPED);
    }

    void PilotAgent::update(double delta_time) {
        if (get_current_state() != AgentState::RUNNING) {
            return;
        }
        
        // 简化的更新逻辑：注意力水平随时间缓慢变化
        double attention_change = (dist(gen) - 0.5) * 0.01; // 小的随机变化
        attention_level = std::clamp(attention_level + attention_change, 0.1, 1.0);
        
        // 更新影响因子
        manual_control_impact = calculate_manual_control_impact(skill_level, attention_level);
        decision_impact = calculate_decision_impact(skill_level, attention_level);
        
        VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "飞行员代理 [" + get_agent_id() + "] 更新 - 注意力: " + 
                           std::to_string(attention_level) + ", 技能: " + std::to_string(skill_level));
    }

    void PilotAgent::handle_event(const Event& event) {
        VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "飞行员代理处理事件: " + event.id);
        // 简化的事件处理：暂时不实现复杂逻辑
    }

    void PilotAgent::send_event(const Event& event) {
        VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "飞行员代理发送事件: " + event.id);
        // 简化的事件发送：暂时不实现复杂逻辑
    }

    std::string PilotAgent::get_status() const {
        std::ostringstream oss;
        oss << "飞行员代理 [" << get_agent_id() << "] - " << get_agent_name() << "\n";
        oss << "状态: " << (get_current_state() == AgentState::RUNNING ? "运行中" : "已停止") << "\n";
        oss << "注意力水平: " << attention_level << "\n";
        oss << "技能水平: " << skill_level << "\n";
        return oss.str();
    }

    bool PilotAgent::is_ready() const {
        return get_current_state() == AgentState::READY || get_current_state() == AgentState::RUNNING;
    }

    void PilotAgent::load_pilot_config() {
        try {
            // 构建配置文件路径
            std::string config_path = "../../src/A_PilotAgentModel/" + agent_id + "/config.json";
            
            // 这里应该读取JSON配置文件
            // 暂时使用硬编码的映射，后续可以改为读取JSON文件
            if (agent_id == "Pilot_001") {
                skill_level = 0.9; // 专家水平
                VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "飞行员 " + agent_id + " 配置加载完成: 专家水平");
            } else if (agent_id == "Pilot_002") {
                skill_level = 0.6; // 有经验水平
                VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "飞行员 " + agent_id + " 配置加载完成: 有经验水平");
            } else {
                skill_level = 0.6; // 默认有经验水平
                VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "飞行员 " + agent_id + " 使用默认配置: 有经验水平");
            }
        }
        catch (const std::exception& e) {
            VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "飞行员配置加载失败: " + std::string(e.what()) + "，使用默认配置");
            skill_level = 0.6; // 默认有经验水平
        }
    }

    // ==================== 策略管理方法实现 ====================

    void PilotAgent::setPilotStrategy(std::unique_ptr<IPilotStrategy> strategy) {
        pilot_strategy = std::move(strategy);
        if (pilot_strategy) {
            // 这里可以设置共享数据空间
            // pilot_strategy->initialize(shared_data_space, agent_id);
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "飞行员策略已设置: " + pilot_strategy->getStrategyId());
        }
    }

    void PilotAgent::initializePilotStrategy(const std::string& pilot_id) {
        VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "初始化飞行员策略: " + pilot_id);
        
        if (pilot_id == "Pilot_001") {
            auto strategy = std::make_unique<Pilot_001_Strategy>();
            setPilotStrategy(std::move(strategy));
        } else if (pilot_id == "Pilot_002") {
            auto strategy = std::make_unique<Pilot_002_Strategy>();
            setPilotStrategy(std::move(strategy));
        } else {
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "未知的飞行员ID: " + pilot_id + "，使用默认策略");
            auto strategy = std::make_unique<Pilot_001_Strategy>();
            setPilotStrategy(std::move(strategy));
        }
    }

    std::string PilotAgent::getStrategyConfig() const {
        if (pilot_strategy) {
            return pilot_strategy->getStrategyId() + "_" + pilot_strategy->getStrategyDescription();
        }
        return "No Strategy";
    }

    bool PilotAgent::executeController(const std::string& controller_name, const std::map<std::string, std::string>& params, double current_time) {
        if (!pilot_strategy) {
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "飞行员策略未设置，无法执行控制器: " + controller_name);
            return false;
        }

        // 委托给策略执行具体的控制器
        if (controller_name == "taxi_control") {
            return pilot_strategy->executeTaxiControlController(params, current_time);
        } else if (controller_name == "takeoff_control") {
            return pilot_strategy->executeTakeoffControlController(params, current_time);
        } else if (controller_name == "landing_control") {
            return pilot_strategy->executeLandingControlController(params, current_time);
        } else if (controller_name == "emergency_response") {
            return pilot_strategy->executeEmergencyResponseController(params, current_time);
        } else if (controller_name == "atc_command_response") {
            return pilot_strategy->executeATCCommandResponseController(params, current_time);
        } else {
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "未知的飞行员控制器: " + controller_name);
            return false;
        }
    }

} // namespace VFT_SMF 