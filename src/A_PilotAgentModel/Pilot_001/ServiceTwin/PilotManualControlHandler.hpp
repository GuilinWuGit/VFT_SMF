/**
 * @file PilotManualControlHandler.hpp
 * @brief 飞行员手动控制处理器 - 专注于飞行员操作意图定义
 * @details 飞行员模型负责定义"做什么"，飞机模型负责"怎么做"
 */

#pragma once

#include "E_GlobalSharedDataSpace/GlobalSharedDataSpace.hpp"
#include "G_SimulationManager/LogAndData/Logger.hpp"
#include "../../../B_AircraftAgentModel/B737/ServiceTwin/ControlPriorityManager.hpp"
#include <memory>
#include <string>

namespace VFT_SMF {

    /**
     * @brief 飞行员操作意图结构体
     * @details 定义飞行员的具体操作意图，由飞机模型执行
     */
    struct PilotOperationIntent {
        enum class OperationType {
            THROTTLE_PUSH_TO_MAX,    ///< 推油门到最大
            THROTTLE_SET_TO_VALUE,   ///< 设置油门到指定值
            BRAKE_PUSH_TO_MAX,       ///< 推刹车到最大
            BRAKE_SET_TO_VALUE,      ///< 设置刹车到指定值
            SPEED_HOLD,              ///< 保持指定速度
            SPEED_HOLD_STOP,         ///< 停止速度保持
            MANUAL_OVERRIDE,         ///< 手动超控
            CLEAR_MANUAL_CONTROL     ///< 清除手动控制
        };

        OperationType type;          ///< 操作类型
        double target_value;         ///< 目标值
        double current_time;         ///< 当前时间
        std::string description;     ///< 操作描述

        PilotOperationIntent(OperationType t, double value, double time, const std::string& desc)
            : type(t), target_value(value), current_time(time), description(desc) {}
    };

    class PilotManualControlHandler {
    private:
        std::shared_ptr<GlobalShared_DataSpace::GlobalSharedDataSpace> shared_data_space;
        std::unique_ptr<ControlPriorityManager> control_priority_manager;

        // 飞行员操作状态
        bool is_throttle_operation_active {false};
        bool is_speed_hold_requested {false};
        double speed_hold_target {5.0}; // m/s

    public:
        explicit PilotManualControlHandler(std::shared_ptr<GlobalShared_DataSpace::GlobalSharedDataSpace> data_space)
            : shared_data_space(std::move(data_space)) {
            control_priority_manager = std::make_unique<ControlPriorityManager>(shared_data_space);
        }

        /**
         * @brief 处理飞行员手动控制事件
         * @details 将事件转换为操作意图，由飞机模型执行
         */
        void handleManualControl(const GlobalSharedDataStruct::StandardEvent& event,
                                 double current_time);

        /**
         * @brief 每步调用，推进飞行员操作意图
         * @details 飞行员模型只负责意图定义，具体执行由飞机模型负责
         */
        void tick(double current_time);

    private:
        // 飞行员操作意图定义方法
        void executeThrottlePush2Max(double current_time);    ///< 飞行员意图：推油门到最大
        void executeBrakePush2Max(double current_time);       ///< 飞行员意图：推刹车到最大
        void executeMaintainSPDRunway(double current_time);   ///< 飞行员意图：保持跑道速度

        // 辅助方法
        void sendOperationIntent(const PilotOperationIntent& intent); ///< 发送操作意图到飞机模型
    };

} // namespace VFT_SMF


