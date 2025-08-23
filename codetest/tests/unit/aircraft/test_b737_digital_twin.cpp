/**
 * @file test_b737_digital_twin.cpp
 * @brief B737数字孪生单元测试
 * @author VFT_SMF V3 Team
 * @date 2024
 */

#include <gtest/gtest.h>
#include <memory>
#include <iostream>

// 包含被测试的头文件
#include "../../../src/B_AircraftAgentModel/B737/B737DigitalTwin.hpp"
#include "../../../src/E_GlobalSharedDataSpace/GlobalSharedDataSpace.hpp"

/**
 * @brief B737数字孪生测试类
 */
class B737DigitalTwinTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 在每个测试前初始化
        shared_data_space = std::make_shared<VFT_SMF::GlobalShared_DataSpace::GlobalSharedDataSpace>();
        twin = std::make_unique<VFT_SMF::B737DigitalTwin>(shared_data_space);
    }
    
    void TearDown() override {
        // 在每个测试后清理
        twin.reset();
        shared_data_space.reset();
    }
    
    std::shared_ptr<VFT_SMF::GlobalShared_DataSpace::GlobalSharedDataSpace> shared_data_space;
    std::unique_ptr<VFT_SMF::B737DigitalTwin> twin;
};

/**
 * @brief 测试B737数字孪生初始化
 */
TEST_F(B737DigitalTwinTest, InitializationTest) {
    // 验证数字孪生是否正确初始化
    EXPECT_TRUE(twin != nullptr);
    
    // 验证飞机类型
    EXPECT_EQ(twin->getAircraftType(), VFT_SMF::AircraftType::B737);
    
    // 验证共享数据空间连接
    EXPECT_TRUE(shared_data_space != nullptr);
}

/**
 * @brief 测试系统状态更新
 */
TEST_F(B737DigitalTwinTest, SystemStateUpdateTest) {
    // 创建测试状态
    VFT_SMF::GlobalSharedDataStruct::AircraftSystemState test_state;
    test_state.throttle_command = 0.5;
    test_state.brake_pressure = 0.3;
    test_state.engine_rpm_left = 2000.0;
    test_state.engine_rpm_right = 2000.0;
    
    // 更新系统状态
    twin->updateSystemState(test_state);
    
    // 验证状态是否正确更新
    auto current_state = twin->getSystemState();
    EXPECT_DOUBLE_EQ(current_state.throttle_command, 0.5);
    EXPECT_DOUBLE_EQ(current_state.brake_pressure, 0.3);
    EXPECT_DOUBLE_EQ(current_state.engine_rpm_left, 2000.0);
    EXPECT_DOUBLE_EQ(current_state.engine_rpm_right, 2000.0);
}

/**
 * @brief 测试飞行状态更新
 */
TEST_F(B737DigitalTwinTest, FlightStateUpdateTest) {
    // 创建测试飞行状态
    VFT_SMF::GlobalSharedDataStruct::AircraftFlightState test_state;
    test_state.position_x = 100.0;
    test_state.position_y = 200.0;
    test_state.velocity_x = 50.0;
    test_state.velocity_y = 25.0;
    test_state.altitude = 1000.0;
    
    // 更新飞行状态
    twin->updateFlightState(test_state);
    
    // 验证状态是否正确更新
    auto current_state = twin->getFlightState();
    EXPECT_DOUBLE_EQ(current_state.position_x, 100.0);
    EXPECT_DOUBLE_EQ(current_state.position_y, 200.0);
    EXPECT_DOUBLE_EQ(current_state.velocity_x, 50.0);
    EXPECT_DOUBLE_EQ(current_state.velocity_y, 25.0);
    EXPECT_DOUBLE_EQ(current_state.altitude, 1000.0);
}

/**
 * @brief 测试边界条件
 */
TEST_F(B737DigitalTwinTest, BoundaryConditionsTest) {
    // 测试极端值
    VFT_SMF::GlobalSharedDataStruct::AircraftSystemState extreme_state;
    extreme_state.throttle_command = 1.0;  // 最大油门
    extreme_state.brake_pressure = 1.0;    // 最大刹车
    extreme_state.engine_rpm_left = 3000.0; // 最大转速
    extreme_state.engine_rpm_right = 3000.0;
    
    // 更新状态
    twin->updateSystemState(extreme_state);
    
    // 验证极端值处理
    auto current_state = twin->getSystemState();
    EXPECT_DOUBLE_EQ(current_state.throttle_command, 1.0);
    EXPECT_DOUBLE_EQ(current_state.brake_pressure, 1.0);
    EXPECT_DOUBLE_EQ(current_state.engine_rpm_left, 3000.0);
    EXPECT_DOUBLE_EQ(current_state.engine_rpm_right, 3000.0);
}

/**
 * @brief 测试无效状态处理
 */
TEST_F(B737DigitalTwinTest, InvalidStateTest) {
    // 测试负值处理
    VFT_SMF::GlobalSharedDataStruct::AircraftSystemState invalid_state;
    invalid_state.throttle_command = -0.1;  // 无效负值
    invalid_state.brake_pressure = 1.5;     // 超出范围值
    
    // 更新状态
    twin->updateSystemState(invalid_state);
    
    // 验证无效值是否被正确处理（这里假设系统会限制在有效范围内）
    auto current_state = twin->getSystemState();
    // 注意：具体的行为取决于实现，这里只是示例
    EXPECT_GE(current_state.throttle_command, 0.0);
    EXPECT_LE(current_state.throttle_command, 1.0);
    EXPECT_GE(current_state.brake_pressure, 0.0);
    EXPECT_LE(current_state.brake_pressure, 1.0);
}

/**
 * @brief 主函数（Google Test会自动处理）
 */
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
