/**
 * @file PilotATCCommandHandler.hpp
 * @brief 飞行员ATC指令处理器 - 负责处理飞行员接收和执行ATC指令
 * @author VFT_SMF Development Team
 * @date 2025-08-21
 * 
 * 飞行员ATC指令处理器负责：
 * 1. 处理飞行员接收的ATC指令
 * 2. 执行相应的飞行操作
 * 3. 更新飞行状态和系统状态
 */

#pragma once

#include "E_GlobalSharedDataSpace/GlobalSharedDataSpace.hpp"
#include "G_SimulationManager/LogAndData/Logger.hpp"
#include <memory>
#include <string>

namespace VFT_SMF {

    /**
     * @brief 飞行员ATC指令处理器类
     */
    class PilotATCCommandHandler {
    private:
        std::shared_ptr<GlobalShared_DataSpace::GlobalSharedDataSpace> shared_data_space;

    public:
        PilotATCCommandHandler(std::shared_ptr<GlobalShared_DataSpace::GlobalSharedDataSpace> data_space);
        ~PilotATCCommandHandler() = default;

        // 飞行员ATC指令处理函数
        void handlePilotATCCommand(const GlobalSharedDataStruct::StandardEvent& event,
                                  double current_time);

    private:
        // 私有辅助方法
        void executeTaxiClearance(double current_time);
        void executeEmergencyBrake(double current_time);
        void logPilotAction(const std::string& action, const std::string& details);
    };

} // namespace VFT_SMF
