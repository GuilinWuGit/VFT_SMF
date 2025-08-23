/**
 * @file ATC_001_Strategy.hpp
 * @brief ATC_001策略实现 - 标准ATC管理逻辑
 * @author VFT_SMF Development Team
 * @date 2025-08-21
 * 
 * ATC_001特点：
 * 1. 标准的ATC控制逻辑
 * 2. 平衡的安全性和效率
 * 3. 常规的许可发放流程
 */

#pragma once

#include "../A_StandardBase/IATCStrategy.hpp"
#include "../../G_SimulationManager/LogAndData/Logger.hpp"

namespace VFT_SMF {

    /**
     * @brief ATC_001策略类 - 标准ATC管理逻辑
     */
    class ATC_001_Strategy : public IATCStrategy {
    private:
        std::shared_ptr<GlobalShared_DataSpace::GlobalSharedDataSpace> shared_data_space;
        std::string agent_id;
        
        // ATC_001 特有的状态变量
        int total_clearances_issued;
        int emergency_interventions;
        double last_update_time;

    public:
        ATC_001_Strategy() : total_clearances_issued(0), emergency_interventions(0), last_update_time(0.0) {}
        ~ATC_001_Strategy() = default;

        // IATCStrategy接口实现
        std::string getStrategyId() const override { return "ATC_001"; }
        
        std::string getStrategyDescription() const override { 
            return "标准ATC管理策略 - 平衡安全性和效率的常规控制模式"; 
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
        // ATC_001 特有的私有方法
        bool validateStandardConditions(double current_time);
        void logATCAction(const std::string& action, const std::string& details);
        void updateATCCommand(const std::string& command_type, bool value);
        bool checkAircraftStatus();
    };

} // namespace VFT_SMF

