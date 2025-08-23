/**
 * @file PilotManualControlHandler.cpp
 * @brief 飞行员手动控制处理器实现 - 专注于操作意图定义
 */

#include "PilotManualControlHandler.hpp"
#include <algorithm>

namespace VFT_SMF {

// ================================================ 飞行员操作意图处理器 ================================================
void PilotManualControlHandler::handleManualControl(const GlobalSharedDataStruct::StandardEvent& event,
                                                    double current_time) {
    const auto& controller_name = event.driven_process.controller_name;
    logBrief(LogLevel::Brief, "飞行员手动控制处理器: 定义操作意图 " + controller_name +
            " (事件: " + event.event_name + ", 时间: " + std::to_string(current_time) + "s)");

    if (controller_name == "throttle_push2max") {
        executeThrottlePush2Max(current_time);
    } else if (controller_name == "brake_push2max") {
        executeBrakePush2Max(current_time);
    } else if (controller_name == "MaintainSPDRunway") {
        executeMaintainSPDRunway(current_time);
    } else {
        logBrief(LogLevel::Brief, "飞行员手动控制处理器: 未知的控制器操作: " + controller_name);
    }
}

// ================================ 飞行员操作意图定义 ================================

void PilotManualControlHandler::tick(double current_time) {
    // 飞行员模型只负责意图定义，具体执行由飞机模型负责
    // 这里可以添加飞行员意图的持续更新逻辑
    
    if (is_throttle_operation_active) {
        // 飞行员意图：持续推油门到最大
        PilotOperationIntent intent(PilotOperationIntent::OperationType::THROTTLE_PUSH_TO_MAX, 
                                   1.0, current_time, "飞行员持续推油门到最大");
        sendOperationIntent(intent);
    }
    
    if (is_speed_hold_requested) {
        // 飞行员意图：保持指定速度
        PilotOperationIntent intent(PilotOperationIntent::OperationType::SPEED_HOLD, 
                                   speed_hold_target, current_time, 
                                   "飞行员要求保持速度: " + std::to_string(speed_hold_target) + " m/s");
        sendOperationIntent(intent);
    }
}

// 1. 飞行员意图：推油门到最大
void PilotManualControlHandler::executeThrottlePush2Max(double current_time) {
    // 飞行员意图：启动推油门到最大操作
    is_throttle_operation_active = true;
    
    PilotOperationIntent intent(PilotOperationIntent::OperationType::THROTTLE_PUSH_TO_MAX, 
                               1.0, current_time, "飞行员意图：推油门到最大");
    sendOperationIntent(intent);
    
    logBrief(LogLevel::Brief, "飞行员: 定义推油门到最大意图 - 由飞机模型执行具体控制");
}

// 2. 飞行员意图：推刹车到最大
void PilotManualControlHandler::executeBrakePush2Max(double current_time) {
    // 飞行员意图：启动推刹车到最大操作
    PilotOperationIntent intent(PilotOperationIntent::OperationType::BRAKE_PUSH_TO_MAX, 
                               1.0, current_time, "飞行员意图：推刹车到最大");
    sendOperationIntent(intent);
    
    logBrief(LogLevel::Brief, "飞行员: 定义推刹车到最大意图 - 由飞机模型执行具体控制");
}

// 3. 飞行员意图：保持跑道速度
void PilotManualControlHandler::executeMaintainSPDRunway(double current_time) {
    // 飞行员意图：以当前地速为基准 +1 m/s 作为目标速度
    auto flight_state_now = shared_data_space->getAircraftFlightState();
    const double event_speed = std::max(0.0, flight_state_now.groundspeed);
    speed_hold_target = event_speed + 1.0;
    
    is_speed_hold_requested = true;
    
    PilotOperationIntent intent(PilotOperationIntent::OperationType::SPEED_HOLD, 
                               speed_hold_target, current_time, 
                               "飞行员意图：保持跑道速度 " + std::to_string(speed_hold_target) + " m/s");
    sendOperationIntent(intent);
    
    logBrief(LogLevel::Brief, "飞行员: 定义速度保持意图 - 目标速度=" + std::to_string(speed_hold_target) + 
            " m/s, 由飞机模型执行PID控制");
}

// ================================ 辅助方法 ================================

void PilotManualControlHandler::sendOperationIntent(const PilotOperationIntent& intent) {
    // 将飞行员操作意图发送到飞机模型
    // 方式1：通过控制优先级管理器发送意图（当前方式）
    switch (intent.type) {
        case PilotOperationIntent::OperationType::THROTTLE_PUSH_TO_MAX:
            // 传递的参数：油门指令，刹车指令，升降舵指令，副翼指令，方向舵指令，当前时间
            control_priority_manager->setManualControlCommand(intent.target_value, 0.0, 0.0, 0.0, 0.0, intent.current_time);
            break;
            
        case PilotOperationIntent::OperationType::BRAKE_PUSH_TO_MAX:
            // 传递的参数：油门指令，刹车指令，升降舵指令，副翼指令，方向舵指令，当前时间
            control_priority_manager->setManualControlCommand(0.0, 0.0, 0.0, 0.0, intent.target_value, intent.current_time);
            break;
            
        case PilotOperationIntent::OperationType::SPEED_HOLD:
            // 速度保持意图需要飞机模型实现具体的PID控制
            // 这里先发送一个标记，飞机模型会处理具体的控制逻辑
            // 传递的参数：油门指令，刹车指令，升降舵指令，副翼指令，方向舵指令，当前时间
            control_priority_manager->setManualControlCommand(0.0, 0.0, 0.0, 0.0, 0.0, intent.current_time);
            break;
            
        default:
            logBrief(LogLevel::Brief, "飞行员: 未知的操作意图类型");
            break;
    }
    
    logBrief(LogLevel::Brief, "飞行员: 发送操作意图 - " + intent.description);
    
    // 方式2：直接调用飞机模型接口（未来扩展）
    // 这里可以添加对飞机模型控制律的直接调用
    // 例如：aircraft_control_law->executeThrottlePushToMax(intent.current_time);
}

} // namespace VFT_SMF


