/**
 * @file test_control_priority_manager.cpp
 * @brief 控制优先级管理器单元测试
 * @author VFT_SMF V3 Team
 * @date 2024
 */

#include <gtest/gtest.h>
#include <memory>
#include <iostream>

// 包含被测试的头文件
#include "../../../src/B_AircraftAgentModel/B737/ServiceTwin/ControlPriorityManager.hpp"
#include "../../../src/E_GlobalSharedDataSpace/GlobalSharedDataSpace.hpp"

/**
 * @brief 控制优先级管理器测试类
 */
class ControlPriorityManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 在每个测试前初始化
        shared_data_space = std::make_shared<VFT_SMF::GlobalShared_DataSpace::GlobalSharedDataSpace>();
        priority_manager = std::make_unique<VFT_SMF::ControlPriorityManager>(shared_data_space);
    }
    
    void TearDown() override {
        // 在每个测试后清理
        priority_manager.reset();
        shared_data_space.reset();
    }
    
    std::shared_ptr<VFT_SMF::GlobalShared_DataSpace::GlobalSharedDataSpace> shared_data_space;
    std::unique_ptr<VFT_SMF::ControlPriorityManager> priority_manager;
};

/**
 * @brief 测试控制优先级管理器初始化
 */
TEST_F(ControlPriorityManagerTest, InitializationTest) {
    // 验证优先级管理器是否正确初始化
    EXPECT_TRUE(priority_manager != nullptr);
    
    // 验证共享数据空间连接
    EXPECT_TRUE(shared_data_space != nullptr);
    
    // 验证初始状态：没有激活的控制命令
    auto final_command = priority_manager->getFinalControlCommand();
    EXPECT_FALSE(final_command.active);
}

/**
 * @brief 测试手动控制命令设置
 */
TEST_F(ControlPriorityManagerTest, ManualControlCommandTest) {
    // 设置手动控制命令
    priority_manager->setManualControlCommand(0.8, 0.2, 0.1, 0.05, 0.0, 1.0);
    
    // 验证命令是否正确设置
    auto final_command = priority_manager->getFinalControlCommand();
    EXPECT_TRUE(final_command.active);
    EXPECT_EQ(final_command.source, VFT_SMF::GlobalSharedDataStruct::ControlPriority::MANUAL);
    EXPECT_DOUBLE_EQ(final_command.throttle, 0.8);
    EXPECT_DOUBLE_EQ(final_command.brake, 0.2);
    EXPECT_DOUBLE_EQ(final_command.elevator, 0.1);
    EXPECT_DOUBLE_EQ(final_command.aileron, 0.05);
    EXPECT_DOUBLE_EQ(final_command.rudder, 0.0);
}

/**
 * @brief 测试自动飞行控制命令设置
 */
TEST_F(ControlPriorityManagerTest, AutopilotControlCommandTest) {
    // 设置自动飞行控制命令
    priority_manager->setAutopilotControlCommand(0.6, 0.0, 0.15, 0.1, 0.02, 2.0);
    
    // 验证命令是否正确设置
    auto final_command = priority_manager->getFinalControlCommand();
    EXPECT_TRUE(final_command.active);
    EXPECT_EQ(final_command.source, VFT_SMF::GlobalSharedDataStruct::ControlPriority::AUTOPILOT);
    EXPECT_DOUBLE_EQ(final_command.throttle, 0.6);
    EXPECT_DOUBLE_EQ(final_command.elevator, 0.15);
    EXPECT_DOUBLE_EQ(final_command.aileron, 0.1);
    EXPECT_DOUBLE_EQ(final_command.rudder, 0.02);
}

/**
 * @brief 测试自动油门控制命令设置
 */
TEST_F(ControlPriorityManagerTest, AutothrottleControlCommandTest) {
    // 设置自动油门控制命令
    priority_manager->setAutothrottleControlCommand(0.7, 3.0);
    
    // 验证命令是否正确设置
    auto final_command = priority_manager->getFinalControlCommand();
    EXPECT_TRUE(final_command.active);
    EXPECT_EQ(final_command.source, VFT_SMF::GlobalSharedDataStruct::ControlPriority::AUTOTHROTTLE);
    EXPECT_DOUBLE_EQ(final_command.throttle, 0.7);
}

/**
 * @brief 测试紧急控制命令设置
 */
TEST_F(ControlPriorityManagerTest, EmergencyControlCommandTest) {
    // 设置紧急控制命令
    priority_manager->setEmergencyControlCommand(0.0, 1.0, 0.0, 0.0, 0.0, 4.0);
    
    // 验证命令是否正确设置
    auto final_command = priority_manager->getFinalControlCommand();
    EXPECT_TRUE(final_command.active);
    EXPECT_EQ(final_command.source, VFT_SMF::GlobalSharedDataStruct::ControlPriority::EMERGENCY);
    EXPECT_DOUBLE_EQ(final_command.throttle, 0.0);
    EXPECT_DOUBLE_EQ(final_command.brake, 1.0);
}

/**
 * @brief 测试优先级冲突解决
 */
TEST_F(ControlPriorityManagerTest, PriorityConflictResolutionTest) {
    // 设置多个不同优先级的控制命令
    priority_manager->setManualControlCommand(0.5, 0.0, 0.0, 0.0, 0.0, 1.0);
    priority_manager->setAutopilotControlCommand(0.8, 0.0, 0.0, 0.0, 0.0, 2.0);
    priority_manager->setEmergencyControlCommand(0.0, 1.0, 0.0, 0.0, 0.0, 3.0);
    
    // 验证最高优先级的命令（紧急控制）被选中
    auto final_command = priority_manager->getFinalControlCommand();
    EXPECT_TRUE(final_command.active);
    EXPECT_EQ(final_command.source, VFT_SMF::GlobalSharedDataStruct::ControlPriority::EMERGENCY);
    EXPECT_DOUBLE_EQ(final_command.throttle, 0.0);
    EXPECT_DOUBLE_EQ(final_command.brake, 1.0);
}

/**
 * @brief 测试控制源激活和停用
 */
TEST_F(ControlPriorityManagerTest, ControlSourceActivationTest) {
    // 激活手动控制
    priority_manager->activateControlSource(VFT_SMF::GlobalSharedDataStruct::ControlPriority::MANUAL);
    EXPECT_TRUE(priority_manager->isControlSourceActive(VFT_SMF::GlobalSharedDataStruct::ControlPriority::MANUAL));
    
    // 停用手动控制
    priority_manager->deactivateControlSource(VFT_SMF::GlobalSharedDataStruct::ControlPriority::MANUAL);
    EXPECT_FALSE(priority_manager->isControlSourceActive(VFT_SMF::GlobalSharedDataStruct::ControlPriority::MANUAL));
}

/**
 * @brief 测试边界值处理
 */
TEST_F(ControlPriorityManagerTest, BoundaryValueTest) {
    // 测试最大值
    priority_manager->setManualControlCommand(1.0, 1.0, 1.0, 1.0, 1.0, 1.0);
    auto final_command = priority_manager->getFinalControlCommand();
    EXPECT_DOUBLE_EQ(final_command.throttle, 1.0);
    EXPECT_DOUBLE_EQ(final_command.brake, 1.0);
    EXPECT_DOUBLE_EQ(final_command.elevator, 1.0);
    EXPECT_DOUBLE_EQ(final_command.aileron, 1.0);
    EXPECT_DOUBLE_EQ(final_command.rudder, 1.0);
    
    // 测试最小值
    priority_manager->setManualControlCommand(0.0, 0.0, 0.0, 0.0, 0.0, 2.0);
    final_command = priority_manager->getFinalControlCommand();
    EXPECT_DOUBLE_EQ(final_command.throttle, 0.0);
    EXPECT_DOUBLE_EQ(final_command.brake, 0.0);
    EXPECT_DOUBLE_EQ(final_command.elevator, 0.0);
    EXPECT_DOUBLE_EQ(final_command.aileron, 0.0);
    EXPECT_DOUBLE_EQ(final_command.rudder, 0.0);
}

/**
 * @brief 测试无效值处理
 */
TEST_F(ControlPriorityManagerTest, InvalidValueTest) {
    // 测试超出范围的值（应该被限制在有效范围内）
    priority_manager->setManualControlCommand(1.5, -0.1, 2.0, -0.5, 1.2, 1.0);
    auto final_command = priority_manager->getFinalControlCommand();
    
    // 验证值被限制在有效范围内
    EXPECT_GE(final_command.throttle, 0.0);
    EXPECT_LE(final_command.throttle, 1.0);
    EXPECT_GE(final_command.brake, 0.0);
    EXPECT_LE(final_command.brake, 1.0);
    EXPECT_GE(final_command.elevator, 0.0);
    EXPECT_LE(final_command.elevator, 1.0);
    EXPECT_GE(final_command.aileron, 0.0);
    EXPECT_LE(final_command.aileron, 1.0);
    EXPECT_GE(final_command.rudder, 0.0);
    EXPECT_LE(final_command.rudder, 1.0);
}

/**
 * @brief 测试清除控制命令
 */
TEST_F(ControlPriorityManagerTest, ClearControlCommandTest) {
    // 设置控制命令
    priority_manager->setManualControlCommand(0.5, 0.3, 0.1, 0.05, 0.02, 1.0);
    
    // 验证命令已设置
    auto final_command = priority_manager->getFinalControlCommand();
    EXPECT_TRUE(final_command.active);
    
    // 清除控制命令
    priority_manager->clearControlCommand(VFT_SMF::GlobalSharedDataStruct::ControlPriority::MANUAL);
    
    // 验证命令已被清除
    final_command = priority_manager->getFinalControlCommand();
    EXPECT_FALSE(final_command.active);
}

/**
 * @brief 主函数（Google Test会自动处理）
 */
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
