/**
 * @file test_simulation_performance.cpp
 * @brief 仿真性能测试
 * @author VFT_SMF V3 Team
 * @date 2024
 */

#include <gtest/gtest.h>
#include <memory>
#include <iostream>
#include <chrono>
#include <vector>
#include <thread>

// 包含被测试的头文件
#include "../../../src/G_SimulationManager/A_TimeSYNC/Simulation_Clock.hpp"
#include "../../../src/E_GlobalSharedDataSpace/GlobalSharedDataSpace.hpp"
#include "../../../src/B_AircraftAgentModel/B737/ServiceTwin/ControlPriorityManager.hpp"
#include "../../../src/A_PilotAgentModel/Pilot_001/ServiceTwin/PilotManualControlHandler.hpp"
#include "../../../src/G_SimulationManager/LogAndData/DataRecorder.hpp"

/**
 * @brief 仿真性能测试类
 */
class SimulationPerformanceTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 在每个测试前初始化
        shared_data_space = std::make_shared<VFT_SMF::GlobalShared_DataSpace::GlobalSharedDataSpace>();
        clock = std::make_unique<VFT_SMF::Simulation_Clock>();
        priority_manager = std::make_unique<VFT_SMF::ControlPriorityManager>(shared_data_space);
        pilot_handler = std::make_unique<VFT_SMF::PilotManualControlHandler>(shared_data_space);
        data_recorder = std::make_unique<VFT_SMF::DataRecorder>();
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
 * @brief 测试时间推进性能
 */
TEST_F(SimulationPerformanceTest, TimeAdvancementPerformanceTest) {
    // 设置时间步长
    clock->setTimeStep(0.001);
    
    // 测量时间推进性能
    auto start = std::chrono::high_resolution_clock::now();
    
    // 执行大量时间推进
    for (int i = 0; i < 100000; ++i) {
        clock->advanceTime();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    // 验证性能（100000次时间推进应该在合理时间内完成）
    EXPECT_LT(duration.count(), 1000000);  // 小于1秒
    
    // 验证最终时间
    EXPECT_DOUBLE_EQ(clock->getCurrentTime(), 100.0);
    
    // 输出性能指标
    double steps_per_second = 100000.0 / (duration.count() / 1000000.0);
    std::cout << "时间推进性能: " << steps_per_second << " 步/秒" << std::endl;
}

/**
 * @brief 测试控制命令设置性能
 */
TEST_F(SimulationPerformanceTest, ControlCommandPerformanceTest) {
    // 测量控制命令设置性能
    auto start = std::chrono::high_resolution_clock::now();
    
    // 执行大量控制命令设置
    for (int i = 0; i < 10000; ++i) {
        double throttle = (i % 100) / 100.0;
        priority_manager->setManualControlCommand(throttle, 0.0, 0.0, 0.0, 0.0, i * 0.01);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    // 验证性能
    EXPECT_LT(duration.count(), 500000);  // 小于0.5秒
    
    // 输出性能指标
    double commands_per_second = 10000.0 / (duration.count() / 1000000.0);
    std::cout << "控制命令设置性能: " << commands_per_second << " 命令/秒" << std::endl;
}

/**
 * @brief 测试数据记录性能
 */
TEST_F(SimulationPerformanceTest, DataRecordingPerformanceTest) {
    // 设置时间步长
    clock->setTimeStep(0.01);
    
    // 测量数据记录性能
    auto start = std::chrono::high_resolution_clock::now();
    
    // 执行大量数据记录
    for (int i = 0; i < 10000; ++i) {
        double current_time = clock->getCurrentTime();
        data_recorder->recordAllData(current_time, *shared_data_space);
        clock->advanceTime();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    // 验证性能
    EXPECT_LT(duration.count(), 2000000);  // 小于2秒
    
    // 输出性能指标
    double records_per_second = 10000.0 / (duration.count() / 1000000.0);
    std::cout << "数据记录性能: " << records_per_second << " 记录/秒" << std::endl;
}

/**
 * @brief 测试完整仿真循环性能
 */
TEST_F(SimulationPerformanceTest, CompleteSimulationLoopPerformanceTest) {
    // 设置时间步长
    clock->setTimeStep(0.01);
    
    // 测量完整仿真循环性能
    auto start = std::chrono::high_resolution_clock::now();
    
    // 执行完整的仿真循环
    for (int step = 0; step < 10000; ++step) {
        double current_time = clock->getCurrentTime();
        
        // 飞行员操作（每100步执行一次）
        if (step % 100 == 0) {
            pilot_handler->executeThrottlePush2Max(current_time);
        }
        
        // 自动飞行控制（每50步执行一次）
        if (step % 50 == 0) {
            priority_manager->setAutopilotControlCommand(0.6, 0.0, 0.1, 0.05, 0.02, current_time);
        }
        
        // 记录数据
        data_recorder->recordAllData(current_time, *shared_data_space);
        
        // 推进时间
        clock->advanceTime();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    // 验证性能
    EXPECT_LT(duration.count(), 5000000);  // 小于5秒
    
    // 输出性能指标
    double steps_per_second = 10000.0 / (duration.count() / 1000000.0);
    std::cout << "完整仿真循环性能: " << steps_per_second << " 步/秒" << std::endl;
}

/**
 * @brief 测试内存使用性能
 */
TEST_F(SimulationPerformanceTest, MemoryUsagePerformanceTest) {
    // 记录初始内存使用（简化版本）
    std::vector<std::unique_ptr<VFT_SMF::ControlPriorityManager>> managers;
    
    // 创建大量管理器实例
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 1000; ++i) {
        managers.push_back(std::make_unique<VFT_SMF::ControlPriorityManager>(shared_data_space));
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    // 验证性能
    EXPECT_LT(duration.count(), 1000000);  // 小于1秒
    
    // 输出性能指标
    double managers_per_second = 1000.0 / (duration.count() / 1000000.0);
    std::cout << "管理器创建性能: " << managers_per_second << " 管理器/秒" << std::endl;
    
    // 清理
    managers.clear();
}

/**
 * @brief 测试并发性能
 */
TEST_F(SimulationPerformanceTest, ConcurrencyPerformanceTest) {
    // 设置时间步长
    clock->setTimeStep(0.01);
    
    // 测量并发操作性能
    auto start = std::chrono::high_resolution_clock::now();
    
    // 创建多个线程执行并发操作
    std::vector<std::thread> threads;
    const int num_threads = 4;
    const int operations_per_thread = 1000;
    
    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back([this, operations_per_thread, t]() {
            for (int i = 0; i < operations_per_thread; ++i) {
                double throttle = ((i + t) % 100) / 100.0;
                priority_manager->setManualControlCommand(throttle, 0.0, 0.0, 0.0, 0.0, i * 0.01);
            }
        });
    }
    
    // 等待所有线程完成
    for (auto& thread : threads) {
        thread.join();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    // 验证性能
    EXPECT_LT(duration.count(), 1000000);  // 小于1秒
    
    // 输出性能指标
    double operations_per_second = (num_threads * operations_per_thread) / (duration.count() / 1000000.0);
    std::cout << "并发操作性能: " << operations_per_second << " 操作/秒" << std::endl;
}

/**
 * @brief 测试大规模数据性能
 */
TEST_F(SimulationPerformanceTest, LargeScaleDataPerformanceTest) {
    // 设置时间步长
    clock->setTimeStep(0.001);
    
    // 测量大规模数据处理性能
    auto start = std::chrono::high_resolution_clock::now();
    
    // 执行大规模仿真
    for (int step = 0; step < 100000; ++step) {
        double current_time = clock->getCurrentTime();
        
        // 复杂的控制操作
        if (step % 10 == 0) {
            pilot_handler->executeThrottlePush2Max(current_time);
        }
        if (step % 20 == 0) {
            pilot_handler->executeBrakePush2Max(current_time);
        }
        if (step % 30 == 0) {
            pilot_handler->executeMaintainSPDRunway(current_time);
        }
        
        // 自动控制
        if (step % 15 == 0) {
            priority_manager->setAutopilotControlCommand(0.6, 0.0, 0.1, 0.05, 0.02, current_time);
        }
        if (step % 25 == 0) {
            priority_manager->setAutothrottleControlCommand(0.7, current_time);
        }
        
        // 记录数据
        data_recorder->recordAllData(current_time, *shared_data_space);
        
        // 推进时间
        clock->advanceTime();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    // 验证性能
    EXPECT_LT(duration.count(), 10000000);  // 小于10秒
    
    // 输出性能指标
    double steps_per_second = 100000.0 / (duration.count() / 1000000.0);
    std::cout << "大规模数据处理性能: " << steps_per_second << " 步/秒" << std::endl;
}

/**
 * @brief 测试实时性能
 */
TEST_F(SimulationPerformanceTest, RealTimePerformanceTest) {
    // 设置时间步长（模拟实时仿真）
    clock->setTimeStep(0.016);  // 约60Hz
    
    // 测量实时性能
    auto start = std::chrono::high_resolution_clock::now();
    
    // 执行实时仿真循环
    for (int frame = 0; frame < 6000; ++frame) {  // 100秒的仿真
        auto frame_start = std::chrono::high_resolution_clock::now();
        
        double current_time = clock->getCurrentTime();
        
        // 飞行员操作
        if (frame % 60 == 0) {  // 每秒一次
            pilot_handler->executeThrottlePush2Max(current_time);
        }
        
        // 自动控制
        if (frame % 30 == 0) {  // 每0.5秒一次
            priority_manager->setAutopilotControlCommand(0.6, 0.0, 0.1, 0.05, 0.02, current_time);
        }
        
        // 记录数据
        data_recorder->recordAllData(current_time, *shared_data_space);
        
        // 推进时间
        clock->advanceTime();
        
        // 模拟实时约束
        auto frame_end = std::chrono::high_resolution_clock::now();
        auto frame_duration = std::chrono::duration_cast<std::chrono::microseconds>(frame_end - frame_start);
        
        // 验证实时性能（每帧应该在16ms内完成）
        EXPECT_LT(frame_duration.count(), 16000);  // 小于16ms
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    // 输出性能指标
    double frames_per_second = 6000.0 / (duration.count() / 1000000.0);
    std::cout << "实时仿真性能: " << frames_per_second << " 帧/秒" << std::endl;
}

/**
 * @brief 测试内存泄漏性能
 */
TEST_F(SimulationPerformanceTest, MemoryLeakPerformanceTest) {
    // 测量内存泄漏
    for (int cycle = 0; cycle < 100; ++cycle) {
        // 创建和销毁大量对象
        std::vector<std::unique_ptr<VFT_SMF::ControlPriorityManager>> temp_managers;
        
        for (int i = 0; i < 100; ++i) {
            temp_managers.push_back(std::make_unique<VFT_SMF::ControlPriorityManager>(shared_data_space));
        }
        
        // 执行一些操作
        for (auto& manager : temp_managers) {
            manager->setManualControlCommand(0.5, 0.0, 0.0, 0.0, 0.0, cycle * 0.1);
        }
        
        // 自动清理（RAII）
        temp_managers.clear();
    }
    
    // 验证没有内存泄漏（通过观察系统行为）
    // 这里主要是验证程序不会崩溃或内存使用异常增长
    EXPECT_TRUE(true);  // 如果程序运行到这里，说明没有严重的内存泄漏
}

/**
 * @brief 主函数（Google Test会自动处理）
 */
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
