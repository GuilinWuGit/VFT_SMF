/**
 * @file ATC_002_Strategy.hpp
 * @brief ATC_002策略实现 - 严格ATC管理逻辑
 * @author VFT_SMF Development Team
 * @date 2025-08-21
 * 
 * ATC_002特点：
 * 1. 更严格的安全检查
 * 2. 更保守的许可发放策略
 * 3. 更频繁的状态监控
 * 4. 更低的风险容忍度
 */

#pragma once

#include "../A_StandardBase/IATCStrategy.hpp"
#include "../../G_SimulationManager/LogAndData/Logger.hpp"

namespace VFT_SMF {

    /**
     * @brief ATC_002策略类 - 严格ATC管理逻辑
     */
    class ATC_002_Strategy : public IATCStrategy {
    private:
        std::shared_ptr<GlobalShared_DataSpace::GlobalSharedDataSpace> shared_data_space;
        std::string agent_id;
        
        // ATC_002 特有的状态变量
        bool strict_mode_enabled;
        double last_safety_check_time;
        int total_commands_issued;
        int safety_violations_detected;
        int clearances_denied;
        double safety_check_interval;  // 安全检查间隔

    public:
        ATC_002_Strategy() : strict_mode_enabled(true), last_safety_check_time(0.0), 
                           total_commands_issued(0), safety_violations_detected(0), 
                           clearances_denied(0), safety_check_interval(0.5) {}  // 0.5秒检查一次
        ~ATC_002_Strategy() = default;

        // IATCStrategy接口实现
        std::string getStrategyId() const override { return "ATC_002"; }
        
        std::string getStrategyDescription() const override { 
            return "严格ATC管理策略 - 高安全标准的保守控制模式"; 
        }

        void initialize(std::shared_ptr<GlobalShared_DataSpace::GlobalSharedDataSpace> data_space,
                       const std::string& id) override;

        bool executeClearanceController(const std::map<std::string, std::string>& params, 
                                      double current_time) override;

        bool executeEmergencyBrakeController(const std::map<std::string, std::string>& params, 
                                           double current_time) override;

        bool executeTakeoffClearanceController(const std::map<std::string, std::string>& params, 
                                             double current_time) override;

        bool executeLandingClearanceController(const std::map<std::string, std::string>& params, 
                                             double current_time) override;

        std::map<std::string, std::string> getStrategyConfig() const override;
        
        std::string getPerformanceStats() const override;

    private:
        // ATC_002 特有的私有方法
        bool performStrictSafetyCheck(double current_time);
        bool validateStrictConditions(const std::string& operation_type, double current_time);
        void logATCAction(const std::string& action, const std::string& details);
        void updateATCCommand(const std::string& command_type, bool value);
        bool checkAdvancedAircraftStatus();
        void updateSafetyMetrics();
        bool shouldIssueClearance(const std::string& clearance_type, double current_time);
        void applyStrictModeLogic(const std::string& command_type);
    };

} // namespace VFT_SMF

