/**
 * @file ATCCommandHandler.hpp
 * @brief ATC指令处理器 - 负责处理飞行员和ATC之间的指令交互
 * @author VFT_SMF Development Team
 * @date 2025-08-21
 * 
 * ATC指令处理器负责：
 * 1. 处理飞行员ATC指令事件
 * 2. 处理ATC事件
 * 3. 管理ATC指令的状态更新
 */

#pragma once

#include "../A_StandardBase/IATCCommandHandler.hpp"
#include "../../G_SimulationManager/LogAndData/Logger.hpp"
#include <memory>
#include <string>

namespace VFT_SMF {

    /**
     * @brief ATC_001 指令处理器类
     */
    class ATCCommandHandler : public IATCCommandHandler {
    private:
        std::shared_ptr<GlobalShared_DataSpace::GlobalSharedDataSpace> shared_data_space;

    public:
        ATCCommandHandler(std::shared_ptr<GlobalShared_DataSpace::GlobalSharedDataSpace> data_space);
        ~ATCCommandHandler() = default;

        // 飞行员ATC指令处理函数
        void handlePilotATCCommand(const GlobalSharedDataStruct::StandardEvent& event,
                                  double current_time) override;

        // ATC事件处理函数
        void handleATCEvent(const GlobalSharedDataStruct::StandardEvent& event,
                           double current_time) override;

    private:
        // 私有辅助方法
        void updateATCCommandState(const std::string& command_type, bool value);
        void logATCCommand(const std::string& command_type, const std::string& action);
    };

} // namespace VFT_SMF
