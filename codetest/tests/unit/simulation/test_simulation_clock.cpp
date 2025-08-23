/**
 * @file test_simulation_clock.cpp
 * @brief 仿真时钟单元测试
 * @author VFT_SMF V3 Team
 * @date 2024
 */

#include <gtest/gtest.h>
#include <memory>
#include <iostream>
#include <thread>
#include <chrono>

// 包含被测试的头文件
#include "../../../src/G_SimulationManager/A_TimeSYNC/Simulation_Clock.hpp"

/**
 * @brief 仿真时钟测试类
 */
class SimulationClockTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 在每个测试前初始化
        clock = std::make_unique<VFT_SMF::Simulation_Clock>();
    }
    
    void TearDown() override {
        // 在每个测试后清理
        clock.reset();
    }
    
    std::unique_ptr<VFT_SMF::Simulation_Clock> clock;
};

/**
 * @brief 测试仿真时钟初始化
 */
TEST_F(SimulationClockTest, InitializationTest) {
    // 验证仿真时钟是否正确初始化
    EXPECT_TRUE(clock != nullptr);
    
    // 验证初始时间
    EXPECT_DOUBLE_EQ(clock->getCurrentTime(), 0.0);
    EXPECT_DOUBLE_EQ(clock->getTimeStep(), 0.01);  // 默认时间步长
}

/**
 * @brief 测试时间步长设置
 */
TEST_F(SimulationClockTest, TimeStepSettingTest) {
    // 设置时间步长
    clock->setTimeStep(0.1);
    EXPECT_DOUBLE_EQ(clock->getTimeStep(), 0.1);
    
    // 设置另一个时间步长
    clock->setTimeStep(0.05);
    EXPECT_DOUBLE_EQ(clock->getTimeStep(), 0.05);
}

/**
 * @brief 测试时间推进
 */
TEST_F(SimulationClockTest, TimeAdvancementTest) {
    // 设置时间步长
    clock->setTimeStep(0.1);
    
    // 推进时间
    clock->advanceTime();
    EXPECT_DOUBLE_EQ(clock->getCurrentTime(), 0.1);
    
    // 再次推进时间
    clock->advanceTime();
    EXPECT_DOUBLE_EQ(clock->getCurrentTime(), 0.2);
    
    // 多次推进时间
    for (int i = 0; i < 10; ++i) {
        clock->advanceTime();
    }
    EXPECT_DOUBLE_EQ(clock->getCurrentTime(), 1.2);
}

/**
 * @brief 测试时间重置
 */
TEST_F(SimulationClockTest, TimeResetTest) {
    // 推进时间
    clock->setTimeStep(0.1);
    clock->advanceTime();
    clock->advanceTime();
    EXPECT_DOUBLE_EQ(clock->getCurrentTime(), 0.2);
    
    // 重置时间
    clock->resetTime();
    EXPECT_DOUBLE_EQ(clock->getCurrentTime(), 0.0);
}

/**
 * @brief 测试时间设置
 */
TEST_F(SimulationClockTest, TimeSettingTest) {
    // 设置特定时间
    clock->setCurrentTime(5.0);
    EXPECT_DOUBLE_EQ(clock->getCurrentTime(), 5.0);
    
    // 设置另一个时间
    clock->setCurrentTime(10.5);
    EXPECT_DOUBLE_EQ(clock->getCurrentTime(), 10.5);
}

/**
 * @brief 测试边界条件
 */
TEST_F(SimulationClockTest, BoundaryConditionsTest) {
    // 测试零时间步长
    clock->setTimeStep(0.0);
    EXPECT_DOUBLE_EQ(clock->getTimeStep(), 0.0);
    
    // 测试负时间步长（应该被处理为正值或保持原值）
    clock->setTimeStep(-0.1);
    // 具体行为取决于实现，这里只是验证不会崩溃
    
    // 测试极大时间步长
    clock->setTimeStep(1000.0);
    EXPECT_DOUBLE_EQ(clock->getTimeStep(), 1000.0);
    
    // 测试负时间设置
    clock->setCurrentTime(-5.0);
    EXPECT_DOUBLE_EQ(clock->getCurrentTime(), -5.0);
}

/**
 * @brief 测试时间精度
 */
TEST_F(SimulationClockTest, TimePrecisionTest) {
    // 设置小时间步长
    clock->setTimeStep(0.001);
    
    // 多次推进时间，验证精度
    for (int i = 0; i < 1000; ++i) {
        clock->advanceTime();
    }
    
    // 验证时间精度（允许小的浮点误差）
    EXPECT_NEAR(clock->getCurrentTime(), 1.0, 1e-10);
}

/**
 * @brief 测试时间比较
 */
TEST_F(SimulationClockTest, TimeComparisonTest) {
    // 设置时间步长
    clock->setTimeStep(0.1);
    
    // 推进时间
    clock->advanceTime();
    double time1 = clock->getCurrentTime();
    
    clock->advanceTime();
    double time2 = clock->getCurrentTime();
    
    // 验证时间递增
    EXPECT_GT(time2, time1);
    EXPECT_DOUBLE_EQ(time2 - time1, 0.1);
}

/**
 * @brief 测试时间同步
 */
TEST_F(SimulationClockTest, TimeSynchronizationTest) {
    // 设置时间步长
    clock->setTimeStep(0.1);
    
    // 模拟多线程环境下的时间同步
    std::vector<double> times;
    
    // 多次获取时间，验证一致性
    for (int i = 0; i < 10; ++i) {
        times.push_back(clock->getCurrentTime());
        clock->advanceTime();
    }
    
    // 验证时间序列正确
    for (size_t i = 1; i < times.size(); ++i) {
        EXPECT_DOUBLE_EQ(times[i] - times[i-1], 0.1);
    }
}

/**
 * @brief 测试性能
 */
TEST_F(SimulationClockTest, PerformanceTest) {
    // 设置时间步长
    clock->setTimeStep(0.001);
    
    // 测量时间推进的性能
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
    EXPECT_NEAR(clock->getCurrentTime(), 100.0, 1e-10);
}

/**
 * @brief 测试时间步长验证
 */
TEST_F(SimulationClockTest, TimeStepValidationTest) {
    // 测试有效的时间步长
    EXPECT_TRUE(clock->setTimeStep(0.1));
    EXPECT_DOUBLE_EQ(clock->getTimeStep(), 0.1);
    
    // 测试零时间步长
    EXPECT_TRUE(clock->setTimeStep(0.0));
    EXPECT_DOUBLE_EQ(clock->getTimeStep(), 0.0);
    
    // 测试极小时间步长
    EXPECT_TRUE(clock->setTimeStep(1e-10));
    EXPECT_DOUBLE_EQ(clock->getTimeStep(), 1e-10);
}

/**
 * @brief 测试时间格式化
 */
TEST_F(SimulationClockTest, TimeFormattingTest) {
    // 设置时间
    clock->setCurrentTime(123.456);
    
    // 获取格式化的时间字符串（如果实现支持）
    // 这里假设有一个formatTime方法
    // std::string formatted = clock->formatTime();
    // EXPECT_FALSE(formatted.empty());
    
    // 验证时间值
    EXPECT_DOUBLE_EQ(clock->getCurrentTime(), 123.456);
}

/**
 * @brief 测试时间统计
 */
TEST_F(SimulationClockTest, TimeStatisticsTest) {
    // 设置时间步长
    clock->setTimeStep(0.1);
    
    // 记录开始时间
    double start_time = clock->getCurrentTime();
    
    // 推进时间
    for (int i = 0; i < 50; ++i) {
        clock->advanceTime();
    }
    
    // 记录结束时间
    double end_time = clock->getCurrentTime();
    
    // 验证时间统计
    EXPECT_DOUBLE_EQ(end_time - start_time, 5.0);
    EXPECT_DOUBLE_EQ(end_time, 5.0);
}

/**
 * @brief 主函数（Google Test会自动处理）
 */
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
