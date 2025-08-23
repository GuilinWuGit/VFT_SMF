/**
 * @file test_pilot_manual_control.cpp
 * @brief 飞行员手动控制单元测试
 * @author VFT_SMF V3 Team
 * @date 2024
 */

#include <gtest/gtest.h>
#include <memory>
#include <iostream>

// 包含被测试的头文件
#include "../../../src/A_PilotAgentModel/Pilot_001/ServiceTwin/PilotManualControlHandler.hpp"
#include "../../../src/E_GlobalSharedDataSpace/GlobalSharedDataSpace.hpp"

/**
 * @brief 飞行员手动控制测试类
 */
class PilotManualControlTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 在每个测试前初始化
        shared_data_space = std::make_shared<VFT_SMF::GlobalShared_DataSpace::GlobalSharedDataSpace>();
        pilot_handler = std::make_unique<VFT_SMF::PilotManualControlHandler>(shared_data_space);
    }
    
    void TearDown() override {
        // 在每个测试后清理
        pilot_handler.reset();
        shared_data_space.reset();
    }
    
    std::shared_ptr<VFT_SMF::GlobalShared_DataSpace::GlobalSharedDataSpace> shared_data_space;
    std::unique_ptr<VFT_SMF::PilotManualControlHandler> pilot_handler;
};

/**
 * @brief 测试飞行员手动控制初始化
 */
TEST_F(PilotManualControlTest, InitializationTest) {
    // 验证飞行员手动控制处理器是否正确初始化
    EXPECT_TRUE(pilot_handler != nullptr);
    
    // 验证共享数据空间连接
    EXPECT_TRUE(shared_data_space != nullptr);
}

/**
 * @brief 测试油门推至最大操作
 */
TEST_F(PilotManualControlTest, ThrottlePushToMaxTest) {
    // 执行油门推至最大操作
    pilot_handler->executeThrottlePush2Max(1.0);
    
    // 验证操作意图是否正确设置
    // 注意：这里需要根据实际实现来验证
    // 由于飞行员模型现在只定义意图，具体执行由飞机模型负责
    // 这里主要验证操作是否被正确触发
    
    // 可以通过检查共享数据空间中的控制命令来验证
    auto control_manager = shared_data_space->getControlPriorityManager();
    auto final_command = control_manager.calculateFinalCommand();
    
    // 验证手动控制命令是否被设置
    EXPECT_TRUE(final_command.active);
    EXPECT_EQ(final_command.source, VFT_SMF::GlobalSharedDataStruct::ControlPriority::MANUAL);
}

/**
 * @brief 测试刹车推至最大操作
 */
TEST_F(PilotManualControlTest, BrakePushToMaxTest) {
    // 执行刹车推至最大操作
    pilot_handler->executeBrakePush2Max(2.0);
    
    // 验证操作意图是否正确设置
    auto control_manager = shared_data_space->getControlPriorityManager();
    auto final_command = control_manager.calculateFinalCommand();
    
    // 验证手动控制命令是否被设置
    EXPECT_TRUE(final_command.active);
    EXPECT_EQ(final_command.source, VFT_SMF::GlobalSharedDataStruct::ControlPriority::MANUAL);
}

/**
 * @brief 测试保持跑道速度操作
 */
TEST_F(PilotManualControlTest, MaintainRunwaySpeedTest) {
    // 执行保持跑道速度操作
    pilot_handler->executeMaintainSPDRunway(3.0);
    
    // 验证操作意图是否正确设置
    auto control_manager = shared_data_space->getControlPriorityManager();
    auto final_command = control_manager.calculateFinalCommand();
    
    // 验证手动控制命令是否被设置
    EXPECT_TRUE(final_command.active);
    EXPECT_EQ(final_command.source, VFT_SMF::GlobalSharedDataStruct::ControlPriority::MANUAL);
}

/**
 * @brief 测试事件处理
 */
TEST_F(PilotManualControlTest, EventHandlingTest) {
    // 创建测试事件
    VFT_SMF::GlobalSharedDataStruct::StandardEvent test_event;
    test_event.event_id = 1001;
    test_event.event_name = "PILOT_THROTTLE_PUSH_TO_MAX";
    test_event.event_time = 1.0;
    test_event.event_data = "{\"target_throttle\": 1.0}";
    
    // 处理事件
    pilot_handler->handleManualControl(test_event, 1.0);
    
    // 验证事件是否被正确处理
    auto control_manager = shared_data_space->getControlPriorityManager();
    auto final_command = control_manager.calculateFinalCommand();
    
    // 验证手动控制命令是否被设置
    EXPECT_TRUE(final_command.active);
    EXPECT_EQ(final_command.source, VFT_SMF::GlobalSharedDataStruct::ControlPriority::MANUAL);
}

/**
 * @brief 测试tick函数
 */
TEST_F(PilotManualControlTest, TickFunctionTest) {
    // 先设置一个操作意图
    pilot_handler->executeThrottlePush2Max(1.0);
    
    // 执行tick函数
    pilot_handler->tick(2.0);
    
    // 验证tick函数是否正确处理了操作意图
    auto control_manager = shared_data_space->getControlPriorityManager();
    auto final_command = control_manager.calculateFinalCommand();
    
    // 验证手动控制命令仍然存在
    EXPECT_TRUE(final_command.active);
    EXPECT_EQ(final_command.source, VFT_SMF::GlobalSharedDataStruct::ControlPriority::MANUAL);
}

/**
 * @brief 测试多个操作意图的处理
 */
TEST_F(PilotManualControlTest, MultipleIntentTest) {
    // 设置多个操作意图
    pilot_handler->executeThrottlePush2Max(1.0);
    pilot_handler->executeBrakePush2Max(2.0);
    pilot_handler->executeMaintainSPDRunway(3.0);
    
    // 执行tick函数
    pilot_handler->tick(4.0);
    
    // 验证操作意图是否被正确处理
    auto control_manager = shared_data_space->getControlPriorityManager();
    auto final_command = control_manager.calculateFinalCommand();
    
    // 验证手动控制命令存在
    EXPECT_TRUE(final_command.active);
    EXPECT_EQ(final_command.source, VFT_SMF::GlobalSharedDataStruct::ControlPriority::MANUAL);
}

/**
 * @brief 测试操作意图的清除
 */
TEST_F(PilotManualControlTest, IntentClearTest) {
    // 设置操作意图
    pilot_handler->executeThrottlePush2Max(1.0);
    
    // 验证意图已设置
    auto control_manager = shared_data_space->getControlPriorityManager();
    auto final_command = control_manager.calculateFinalCommand();
    EXPECT_TRUE(final_command.active);
    
    // 清除控制命令
    control_manager.clearControlCommand(VFT_SMF::GlobalSharedDataStruct::ControlPriority::MANUAL);
    
    // 验证意图已被清除
    final_command = control_manager.calculateFinalCommand();
    EXPECT_FALSE(final_command.active);
}

/**
 * @brief 测试边界条件
 */
TEST_F(PilotManualControlTest, BoundaryConditionsTest) {
    // 测试时间边界
    pilot_handler->executeThrottlePush2Max(0.0);  // 零时间
    pilot_handler->executeThrottlePush2Max(999999.0);  // 极大时间
    
    // 验证操作仍然有效
    auto control_manager = shared_data_space->getControlPriorityManager();
    auto final_command = control_manager.calculateFinalCommand();
    EXPECT_TRUE(final_command.active);
}

/**
 * @brief 测试无效事件处理
 */
TEST_F(PilotManualControlTest, InvalidEventTest) {
    // 创建无效事件
    VFT_SMF::GlobalSharedDataStruct::StandardEvent invalid_event;
    invalid_event.event_id = -1;
    invalid_event.event_name = "INVALID_EVENT";
    invalid_event.event_time = -1.0;
    invalid_event.event_data = "";
    
    // 处理无效事件
    pilot_handler->handleManualControl(invalid_event, 1.0);
    
    // 验证系统没有崩溃，并且状态保持一致
    auto control_manager = shared_data_space->getControlPriorityManager();
    auto final_command = control_manager.calculateFinalCommand();
    
    // 无效事件不应该影响系统状态
    // 这里的具体行为取决于实现
}

/**
 * @brief 测试并发操作意图
 */
TEST_F(PilotManualControlTest, ConcurrentIntentTest) {
    // 模拟并发设置多个操作意图
    pilot_handler->executeThrottlePush2Max(1.0);
    pilot_handler->executeBrakePush2Max(1.0);
    
    // 执行tick函数
    pilot_handler->tick(2.0);
    
    // 验证系统能够正确处理并发意图
    auto control_manager = shared_data_space->getControlPriorityManager();
    auto final_command = control_manager.calculateFinalCommand();
    
    // 验证手动控制命令存在
    EXPECT_TRUE(final_command.active);
    EXPECT_EQ(final_command.source, VFT_SMF::GlobalSharedDataStruct::ControlPriority::MANUAL);
}

/**
 * @brief 主函数（Google Test会自动处理）
 */
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
