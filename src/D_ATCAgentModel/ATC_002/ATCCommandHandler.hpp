/**
 * @file ATCCommandHandler.hpp
 * @brief ATC_002 指令处理器 - 实现不同的ATC管理逻辑
 * @author VFT_SMF Development Team
 * @date 2025-08-21
 * 
 * ATC_002 指令处理器特点：
 * 1. 更严格的许可控制逻辑
 * 2. 更频繁的状态检查
 * 3. 不同的决策算法
 * 4. 更保守的安全策略
 */

#pragma once

#include "../A_StandardBase/IATCCommandHandler.hpp"
#include "../../G_SimulationManager/LogAndData/Logger.hpp"
#include <memory>
#include <string>

namespace VFT_SMF {

    /**
     * @brief ATC_002 指令处理器类 - 实现不同的ATC逻辑
     */
    class ATCCommandHandler_002 : public IATCCommandHandler {
    private:
        std::shared_ptr<GlobalShared_DataSpace::GlobalSharedDataSpace> shared_data_space;
        
        // ATC_002 特有的状态变量
        bool strict_mode_enabled;
        double last_safety_check_time;
        int total_commands_issued;
        int safety_violations_detected;

    public:
        ATCCommandHandler_002(std::shared_ptr<GlobalShared_DataSpace::GlobalSharedDataSpace> data_space);
        ~ATCCommandHandler_002() = default;

        // 飞行员ATC指令处理函数 - ATC_002 版本
        void handlePilotATCCommand(const GlobalSharedDataStruct::StandardEvent& event,
                                  double current_time) override;

        // ATC事件处理函数 - ATC_002 版本
        void handleATCEvent(const GlobalSharedDataStruct::StandardEvent& event,
                           double current_time) override;

    private:
        // ATC_002 特有的私有方法
        void performSafetyCheck(double current_time);
        bool validateClearanceRequest(const std::string& clearance_type, double current_time);
        void updateSafetyMetrics(const std::string& command_type);
        void logATCCommand(const std::string& command_type, const std::string& action);
        void updateATCCommandState(const std::string& command_type, bool value);
        
        // ATC_002 特有的决策逻辑
        bool shouldIssueClearance(const std::string& clearance_type, double current_time);
        void applyStrictModeLogic(const std::string& command_type);
    };

} // namespace VFT_SMF
