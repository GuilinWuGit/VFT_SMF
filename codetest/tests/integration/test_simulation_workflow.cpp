/**
 * @file test_simulation_workflow.cpp
 * @brief 仿真工作流程集成测试
 * @author VFT_SMF V3 Team
 * @date 2024
 */

#include <gtest/gtest.h>
#include <memory>
#include <iostream>
#include <vector>

// 包含被测试的头文件
#include "../../../src/G_SimulationManager/A_TimeSYNC/Simulation_Clock.hpp"
#include "../../../src/E_GlobalSharedDataSpace/GlobalSharedDataSpace.hpp"
#include "../../../src/B_AircraftAgentModel/B737/ServiceTwin/ControlPriorityManager.hpp"
#include "../../../src/A_PilotAgentModel/Pilot_001/ServiceTwin/PilotManualControlHandler.hpp"
#include "../../../src/G_SimulationManager/LogAndData/DataRecorder.hpp"

/**
 * @brief 仿真工作流程集成测试类
 */
class SimulationWorkflowTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 在每个测试前初始化
        shared_data_space = std::make_shared<VFT_SMF::GlobalShared_DataSpace::GlobalSharedDataSpace>();
        clock = std::make_unique<VFT_SMF::Simulation_Clock>();
        priority_manager = std::make_unique<VFT_SMF::ControlPriorityManager>(shared_data_space);
        pilot_handler = std::make_unique<VFT_SMF::PilotManualControlHandler>(shared_data_space);
        data_recorder = std::make_unique<VFT_SMF::DataRecorder>();
        
        // 设置时间步长
        clock->setTimeStep(0.01);
    }
    
    void TearDown() override {
        // 在每个测试后清理
        data_recorder.reset();
        pilot_handler.reset();
        priority_manager.reset();
        clock.reset();
        shared_data_space.reset();
    }
    
    std::shared_ptr<VFT_SMF::GlobalShared_DataSpace::GlobalSharedDataSpace> shared_data_space;
    std::unique_ptr<VFT_SMF::Simulation_Clock> clock;
    std::unique_ptr<VFT_SMF::ControlPriorityManager> priority_manager;
    std::unique_ptr<VFT_SMF::PilotManualControlHandler> pilot_handler;
    std::unique_ptr<VFT_SMF::DataRecorder> data_recorder;
};

/**
 * @brief 测试完整的仿真工作流程
 */
TEST_F(SimulationWorkflowTest, CompleteWorkflowTest) {
    // 1. 初始化阶段
    EXPECT_TRUE(shared_data_space != nullptr);
    EXPECT_TRUE(clock != nullptr);
    EXPECT_TRUE(priority_manager != nullptr);
    EXPECT_TRUE(pilot_handler != nullptr);
    EXPECT_TRUE(data_recorder != nullptr);
    
    // 2. 设置初始状态
    clock->setCurrentTime(0.0);
    EXPECT_DOUBLE_EQ(clock->getCurrentTime(), 0.0);
    
    // 3. 执行仿真步骤
    for (int step = 0; step < 100; ++step) {
        double current_time = clock->getCurrentTime();
        
        // 飞行员操作（每10步执行一次）
        if (step % 10 == 0) {
            pilot_handler->executeThrottlePush2Max(current_time);
        }
        
        // 记录数据
        data_recorder->recordAllData(current_time, *shared_data_space);
        
        // 推进时间
        clock->advanceTime();
    }
    
    // 4. 验证仿真结果
    EXPECT_DOUBLE_EQ(clock->getCurrentTime(), 1.0);  // 100 * 0.01 = 1.0
    
    // 验证控制命令
    auto final_command = priority_manager->getFinalControlCommand();
    EXPECT_TRUE(final_command.active);
    EXPECT_EQ(final_command.source, VFT_SMF::GlobalSharedDataStruct::ControlPriority::MANUAL);
}

/**
 * @brief 测试多模块交互
 */
TEST_F(SimulationWorkflowTest, MultiModuleInteractionTest) {
    // 设置仿真参数
    clock->setTimeStep(0.1);
    
    // 执行多步仿真
    for (int step = 0; step < 20; ++step) {
        double current_time = clock->getCurrentTime();
        
        // 飞行员操作
        if (step == 5) {
            pilot_handler->executeThrottlePush2Max(current_time);
        } else if (step == 10) {
            pilot_handler->executeBrakePush2Max(current_time);
        } else if (step == 15) {
            pilot_handler->executeMaintainSPDRunway(current_time);
        }
        
        // 自动飞行控制（模拟）
        if (step >= 8) {
            priority_manager->setAutopilotControlCommand(0.6, 0.0, 0.1, 0.05, 0.02, current_time);
        }
        
        // 记录数据
        data_recorder->recordAllData(current_time, *shared_data_space);
        
        // 推进时间
        clock->advanceTime();
    }
    
    // 验证最终状态
    EXPECT_DOUBLE_EQ(clock->getCurrentTime(), 2.0);
    
    // 验证控制优先级（手动控制应该优先于自动飞行）
    auto final_command = priority_manager->getFinalControlCommand();
    EXPECT_TRUE(final_command.active);
    EXPECT_EQ(final_command.source, VFT_SMF::GlobalSharedDataStruct::ControlPriority::MANUAL);
}

/**
 * @brief 测试数据记录和同步
 */
TEST_F(SimulationWorkflowTest, DataRecordingAndSyncTest) {
    // 设置时间步长
    clock->setTimeStep(0.05);
    
    std::vector<double> recorded_times;
    std::vector<double> throttle_values;
    
    // 执行仿真并记录数据
    for (int step = 0; step < 50; ++step) {
        double current_time = clock->getCurrentTime();
        recorded_times.push_back(current_time);
        
        // 飞行员操作
        if (step == 10) {
            pilot_handler->executeThrottlePush2Max(current_time);
        }
        
        // 记录数据
        data_recorder->recordAllData(current_time, *shared_data_space);
        
        // 获取当前油门值
        auto final_command = priority_manager->getFinalControlCommand();
        throttle_values.push_back(final_command.throttle);
        
        // 推进时间
        clock->advanceTime();
    }
    
    // 验证数据记录
    EXPECT_EQ(recorded_times.size(), 50);
    EXPECT_EQ(throttle_values.size(), 50);
    
    // 验证时间序列
    for (size_t i = 1; i < recorded_times.size(); ++i) {
        EXPECT_DOUBLE_EQ(recorded_times[i] - recorded_times[i-1], 0.05);
    }
    
    // 验证油门值变化
    bool throttle_changed = false;
    for (size_t i = 1; i < throttle_values.size(); ++i) {
        if (throttle_values[i] != throttle_values[i-1]) {
            throttle_changed = true;
            break;
        }
    }
    EXPECT_TRUE(throttle_changed);
}

/**
 * @brief 测试事件驱动流程
 */
TEST_F(SimulationWorkflowTest, EventDrivenWorkflowTest) {
    // 创建事件序列
    std::vector<VFT_SMF::GlobalSharedDataStruct::StandardEvent> events = {
        {1001, "PILOT_THROTTLE_PUSH_TO_MAX", 0.5, "{\"target_throttle\": 1.0}"},
        {1002, "PILOT_BRAKE_PUSH_TO_MAX", 1.0, "{\"target_brake\": 1.0}"},
        {1003, "PILOT_SPEED_HOLD", 1.5, "{\"target_speed\": 5.0}"},
        {1004, "AUTOPILOT_ENGAGE", 2.0, "{\"mode\": \"altitude_hold\"}"},
        {1005, "EMERGENCY_BRAKE", 2.5, "{\"reason\": \"obstacle_detected\"}"}
    };
    
    // 执行事件驱动的仿真
    clock->setTimeStep(0.1);
    int event_index = 0;
    
    for (int step = 0; step < 30; ++step) {
        double current_time = clock->getCurrentTime();
        
        // 处理事件
        if (event_index < events.size() && current_time >= events[event_index].event_time) {
            pilot_handler->handleManualControl(events[event_index], current_time);
            event_index++;
        }
        
        // 记录数据
        data_recorder->recordAllData(current_time, *shared_data_space);
        
        // 推进时间
        clock->advanceTime();
    }
    
    // 验证事件处理
    EXPECT_EQ(event_index, events.size());
    EXPECT_DOUBLE_EQ(clock->getCurrentTime(), 3.0);
}

/**
 * @brief 测试性能基准
 */
TEST_F(SimulationWorkflowTest, PerformanceBenchmarkTest) {
    // 设置时间步长
    clock->setTimeStep(0.01);
    
    // 测量性能
    auto start = std::chrono::high_resolution_clock::now();
    
    // 执行大量仿真步骤
    for (int step = 0; step < 10000; ++step) {
        double current_time = clock->getCurrentTime();
        
        // 飞行员操作（每100步执行一次）
        if (step % 100 == 0) {
            pilot_handler->executeThrottlePush2Max(current_time);
        }
        
        // 记录数据
        data_recorder->recordAllData(current_time, *shared_data_space);
        
        // 推进时间
        clock->advanceTime();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // 验证性能（10000步应该在合理时间内完成）
    EXPECT_LT(duration.count(), 5000);  // 小于5秒
    
    // 验证最终时间
    EXPECT_DOUBLE_EQ(clock->getCurrentTime(), 100.0);
}

/**
 * @brief 测试错误恢复
 */
TEST_F(SimulationWorkflowTest, ErrorRecoveryTest) {
    // 设置时间步长
    clock->setTimeStep(0.1);
    
    // 执行正常仿真
    for (int step = 0; step < 10; ++step) {
        double current_time = clock->getCurrentTime();
        
        // 飞行员操作
        pilot_handler->executeThrottlePush2Max(current_time);
        
        // 记录数据
        data_recorder->recordAllData(current_time, *shared_data_space);
        
        // 推进时间
        clock->advanceTime();
    }
    
    // 模拟错误情况（重置时间）
    clock->resetTime();
    EXPECT_DOUBLE_EQ(clock->getCurrentTime(), 0.0);
    
    // 继续仿真
    for (int step = 0; step < 5; ++step) {
        double current_time = clock->getCurrentTime();
        
        // 飞行员操作
        pilot_handler->executeBrakePush2Max(current_time);
        
        // 记录数据
        data_recorder->recordAllData(current_time, *shared_data_space);
        
        // 推进时间
        clock->advanceTime();
    }
    
    // 验证恢复后的状态
    EXPECT_DOUBLE_EQ(clock->getCurrentTime(), 0.5);
    
    auto final_command = priority_manager->getFinalControlCommand();
    EXPECT_TRUE(final_command.active);
}

/**
 * @brief 测试数据一致性
 */
TEST_F(SimulationWorkflowTest, DataConsistencyTest) {
    // 设置时间步长
    clock->setTimeStep(0.05);
    
    // 执行仿真
    for (int step = 0; step < 20; ++step) {
        double current_time = clock->getCurrentTime();
        
        // 飞行员操作
        if (step == 5) {
            pilot_handler->executeThrottlePush2Max(current_time);
        }
        
        // 记录数据
        data_recorder->recordAllData(current_time, *shared_data_space);
        
        // 验证数据一致性
        auto final_command = priority_manager->getFinalControlCommand();
        EXPECT_GE(final_command.throttle, 0.0);
        EXPECT_LE(final_command.throttle, 1.0);
        EXPECT_GE(final_command.brake, 0.0);
        EXPECT_LE(final_command.brake, 1.0);
        
        // 推进时间
        clock->advanceTime();
    }
    
    // 验证最终状态
    EXPECT_DOUBLE_EQ(clock->getCurrentTime(), 1.0);
}

/**
 * @brief 主函数（Google Test会自动处理）
 */
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
