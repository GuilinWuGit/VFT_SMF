/**
 * @file B737_ThrustData.cpp
 * @brief B737飞机推力数据实现
 * @details 包含B737飞机的实际发动机推力数据，基于CFM56发动机性能数据
 * @author VFT_SMF Framework
 * @date 2024
 */

#include "B737_ThrustData.hpp"
#include <cmath>
#include <algorithm>

namespace SMF {
namespace AircraftDigitalTwin {
namespace B737 {

// ==================== 构造方法实现 ====================
B737ThrustData::B737ThrustData() : aircraft_type(""), engine_type(""), engine_manufacturer(""),
                                  engine_count(0), data_source(""), data_version(""),
                                  engine_length(0.0), engine_diameter(0.0), engine_weight(0.0),
                                  engine_mount_position_x(0.0), engine_mount_position_y(0.0), engine_mount_position_z(0.0),
                                  fuel_density(0.0), fuel_lower_heating_value(0.0), fuel_tank_capacity(0.0),
                                  fuel_pump_flow_rate(0.0), altitude_correction_factor(0.0), temperature_correction_factor(0.0),
                                  humidity_correction_factor(0.0), bleed_air_correction_factor(0.0), anti_ice_correction_factor(0.0) {}

// ==================== 推力计算方法实现 ====================
double B737ThrustData::calculate_thrust(double altitude, double mach, double temperature,
                                       double throttle_position, double pressure_ratio) const {
    // 基础推力（海平面标准大气条件下）
    double base_thrust = engine_limits.max_thrust_takeoff * throttle_position;
    
    // 高度修正（密度修正）
    double density_ratio = std::pow(1.0 - 0.0065 * altitude / 288.15, 4.256);
    double altitude_correction = std::pow(density_ratio, 0.7);  // 推力随密度变化
    
    // 马赫数修正
    double mach_correction = 1.0 - 0.1 * mach * mach;  // 超音速推力损失
    
    // 温度修正
    double temperature_ratio = temperature / 288.15;  // 标准大气温度
    double temperature_correction = std::pow(temperature_ratio, -0.5);  // 温度升高推力降低
    
    // 压力比修正
    double pressure_correction = std::min(pressure_ratio / 1.0, 1.2);  // 压力比影响
    
    // 综合修正
    double total_correction = altitude_correction * mach_correction * temperature_correction * pressure_correction;
    
    return base_thrust * total_correction;
}

double B737ThrustData::calculate_fuel_flow(double altitude, double mach, double temperature,
                                          double throttle_position, double pressure_ratio) const {
    // 基础燃油流量（海平面标准大气条件下）
    double base_fuel_flow = 2400.0 * throttle_position;  // kg/h，基于巡航燃油流量
    
    // 高度修正
    double density_ratio = std::pow(1.0 - 0.0065 * altitude / 288.15, 4.256);
    double altitude_correction = std::pow(density_ratio, 0.8);  // 燃油流量随密度变化
    
    // 马赫数修正
    double mach_correction = 1.0 + 0.2 * mach;  // 马赫数增加燃油流量增加
    
    // 温度修正
    double temperature_ratio = temperature / 288.15;
    double temperature_correction = std::pow(temperature_ratio, 0.5);  // 温度升高燃油流量增加
    
    // 压力比修正
    double pressure_correction = std::min(pressure_ratio / 1.0, 1.1);
    
    // 综合修正
    double total_correction = altitude_correction * mach_correction * temperature_correction * pressure_correction;
    
    return base_fuel_flow * total_correction;
}

double B737ThrustData::calculate_tsfc(double altitude, double mach, double temperature,
                                     double throttle_position) const {
    // 计算推力和燃油流量
    double thrust = calculate_thrust(altitude, mach, temperature, throttle_position, 1.0);
    double fuel_flow = calculate_fuel_flow(altitude, mach, temperature, throttle_position, 1.0);
    
    // 计算单位推力燃油消耗率
    if (thrust > 0.0) {
        return fuel_flow / thrust;  // kg/N/h
    }
    return 0.0;
}

// ==================== 发动机状态计算方法实现 ====================
double B737ThrustData::calculate_n1_rpm(double throttle_position, double altitude, double mach) const {
    // 基础N1转速
    double base_n1 = 2500.0 * throttle_position;  // RPM
    
    // 高度修正
    double density_ratio = std::pow(1.0 - 0.0065 * altitude / 288.15, 4.256);
    double altitude_correction = std::pow(density_ratio, 0.3);
    
    // 马赫数修正
    double mach_correction = 1.0 + 0.05 * mach;
    
    return base_n1 * altitude_correction * mach_correction;
}

double B737ThrustData::calculate_n2_rpm(double throttle_position, double altitude, double mach) const {
    // N2转速通常与N1转速相关
    double n1 = calculate_n1_rpm(throttle_position, altitude, mach);
    return n1 * 3.5;  // N2转速约为N1的3.5倍
}

double B737ThrustData::calculate_egt(double throttle_position, double altitude, double mach) const {
    // 基础排气温度
    double base_egt = 800.0 + 400.0 * throttle_position;  // K
    
    // 高度修正
    double density_ratio = std::pow(1.0 - 0.0065 * altitude / 288.15, 4.256);
    double altitude_correction = 1.0 + 0.1 * (1.0 - density_ratio);
    
    // 马赫数修正
    double mach_correction = 1.0 + 0.05 * mach;
    
    return base_egt * altitude_correction * mach_correction;
}

// ==================== 数据插值方法实现 ====================
ThrustDataPoint B737ThrustData::interpolate_thrust_data(double altitude, double mach, double temperature,
                                                       double throttle_position) const {
    ThrustDataPoint result;
    
    // 计算推力参数
    result.thrust = calculate_thrust(altitude, mach, temperature, throttle_position, 1.0);
    result.fuel_flow = calculate_fuel_flow(altitude, mach, temperature, throttle_position, 1.0);
    result.thrust_specific_fuel_consumption = calculate_tsfc(altitude, mach, temperature, throttle_position);
    
    // 计算发动机状态参数
    result.n1_rpm = calculate_n1_rpm(throttle_position, altitude, mach);
    result.n2_rpm = calculate_n2_rpm(throttle_position, altitude, mach);
    result.egt = calculate_egt(throttle_position, altitude, mach);
    
    // 设置输入参数
    result.altitude = altitude;
    result.mach_number = mach;
    result.temperature = temperature;
    result.pressure_ratio = 1.0;
    
    return result;
}

// ==================== 发动机启动模拟实现 ====================
bool B737ThrustData::simulate_engine_start(double time_step, double& n1, double& n2, double& egt, double& fuel_flow) const {
    static double start_time = 0.0;
    static bool ignition_occurred = false;
    
    start_time += time_step;
    
    // 启动过程模拟
    if (start_time < 5.0) {
        // 启动机带动阶段
        n1 = start_data.starter_speed * (start_time / 5.0);
        n2 = n1 * 0.1;  // N2初始很低
        egt = 288.15;   // 环境温度
        fuel_flow = 0.0;
    } else if (start_time < 15.0 && !ignition_occurred) {
        // 点火阶段
        if (n1 >= start_data.light_off_n2) {
            ignition_occurred = true;
            egt = start_data.light_off_egt;
            fuel_flow = start_data.idle_fuel_flow * 0.5;
        }
        n1 = start_data.starter_speed;
        n2 = n1 * 0.2;
    } else if (start_time < 30.0) {
        // 加速到慢车
        double progress = (start_time - 15.0) / 15.0;
        n1 = start_data.starter_speed + (start_data.idle_n1 - start_data.starter_speed) * progress;
        n2 = n1 * 3.5;
        egt = start_data.light_off_egt + (start_data.idle_egt - start_data.light_off_egt) * progress;
        fuel_flow = start_data.idle_fuel_flow * (0.5 + 0.5 * progress);
    } else {
        // 慢车状态
        n1 = start_data.idle_n1;
        n2 = start_data.idle_n2;
        egt = start_data.idle_egt;
        fuel_flow = start_data.idle_fuel_flow;
        
        // 重置启动状态
        start_time = 0.0;
        ignition_occurred = false;
        return true;  // 启动完成
    }
    
    return false;  // 启动未完成
}

// ==================== 发动机故障模拟实现 ====================
void B737ThrustData::simulate_engine_failure(double& thrust, double& fuel_flow, double& egt, 
                                            double& n1, double& n2) const {
    // 应用故障修正
    thrust *= failure_data.failure_thrust_ratio;
    fuel_flow *= failure_data.failure_fuel_flow_ratio;
    egt += failure_data.failure_egt_increase;
    n1 -= failure_data.failure_n1_decrease;
    n2 -= failure_data.failure_n2_decrease;
}

// ==================== 数据验证方法实现 ====================
bool B737ThrustData::validate_data() const {
    if (aircraft_type.empty() || engine_type.empty()) return false;
    if (engine_count <= 0) return false;
    if (engine_limits.max_thrust_takeoff <= 0.0) return false;
    if (engine_length <= 0.0 || engine_diameter <= 0.0) return false;
    return true;
}

std::string B737ThrustData::get_validation_report() const {
    std::string report = "B737推力数据验证报告:\n";
    
    if (aircraft_type.empty()) report += "- 错误: 飞机型号为空\n";
    if (engine_type.empty()) report += "- 错误: 发动机型号为空\n";
    if (engine_count <= 0) report += "- 错误: 发动机数量无效\n";
    if (engine_limits.max_thrust_takeoff <= 0.0) report += "- 错误: 最大起飞推力无效\n";
    if (engine_length <= 0.0) report += "- 错误: 发动机长度无效\n";
    if (engine_diameter <= 0.0) report += "- 错误: 发动机直径无效\n";
    
    if (validate_data()) {
        report += "- 状态: 数据验证通过\n";
        report += "- 发动机型号: " + engine_type + "\n";
        report += "- 发动机数量: " + std::to_string(engine_count) + "\n";
        report += "- 最大起飞推力: " + std::to_string(engine_limits.max_thrust_takeoff) + " N\n";
    }
    
    return report;
}

// ==================== 数据加载和保存方法实现 ====================
bool B737ThrustData::load_from_file(const std::string& filename) {
    // 这里应该实现从文件加载推力数据的逻辑
    return true;
}

bool B737ThrustData::save_to_file(const std::string& filename) const {
    // 这里应该实现保存推力数据到文件的逻辑
    return true;
}

// ==================== B737-800推力数据 (CFM56-7B26) ====================
const B737ThrustData B737_800_THRUST_DATA = []() {
    B737ThrustData data;
    
    // 基本信息
    data.aircraft_type = "B737-800";
    data.engine_type = "CFM56-7B26";
    data.engine_manufacturer = "CFM International";
    data.engine_count = 2;
    data.data_source = "CFM International Engine Data";
    data.data_version = "1.0";
    
    // 发动机几何参数
    data.engine_length = 2.36;          // m
    data.engine_diameter = 1.55;        // m
    data.engine_weight = 2381.0;        // kg
    data.engine_mount_position_x = 12.6; // m (从机头)
    data.engine_mount_position_y = 2.86; // m (从机身中心线)
    data.engine_mount_position_z = 0.0;  // m (相对于机翼)
    
    // 发动机限制
    data.engine_limits.max_n1_rpm = 5500.0;      // RPM
    data.engine_limits.max_n2_rpm = 15000.0;     // RPM
    data.engine_limits.max_egt = 950.0;          // K
    data.engine_limits.max_epr = 1.74;           // 发动机压力比
    data.engine_limits.max_thrust_takeoff = 117000.0;  // N
    data.engine_limits.max_thrust_climb = 105300.0;    // N
    data.engine_limits.max_thrust_cruise = 82000.0;    // N
    data.engine_limits.max_thrust_continuous = 82000.0; // N
    data.engine_limits.max_thrust_5min = 117000.0;     // N
    data.engine_limits.max_thrust_2min = 117000.0;     // N
    data.engine_limits.max_thrust_30sec = 117000.0;    // N
    
    // 发动机启动数据
    data.start_data.starter_torque = 5000.0;     // N·m
    data.start_data.starter_speed = 800.0;       // RPM
    data.start_data.light_off_n2 = 1200.0;       // RPM
    data.start_data.light_off_egt = 600.0;       // K
    data.start_data.idle_n1 = 1200.0;            // RPM
    data.start_data.idle_n2 = 4200.0;            // RPM
    data.start_data.idle_egt = 750.0;            // K
    data.start_data.idle_fuel_flow = 400.0;      // kg/h
    data.start_data.start_time = 30.0;           // s
    
    // 发动机故障数据
    data.failure_data.failure_thrust_ratio = 0.0;        // 完全失效
    data.failure_data.failure_fuel_flow_ratio = 0.0;     // 燃油流量为0
    data.failure_data.failure_egt_increase = 100.0;      // K
    data.failure_data.failure_n1_decrease = 1000.0;      // RPM
    data.failure_data.failure_n2_decrease = 3000.0;      // RPM
    
    // 燃油系统数据
    data.fuel_density = 800.0;                   // kg/m³
    data.fuel_lower_heating_value = 43.0e6;      // J/kg
    data.fuel_tank_capacity = 20820.0;           // kg
    data.fuel_pump_flow_rate = 5000.0;           // kg/h
    
    // 推力修正因子
    data.altitude_correction_factor = 1.0;
    data.temperature_correction_factor = 1.0;
    data.humidity_correction_factor = 1.0;
    data.bleed_air_correction_factor = 0.98;     // 引气损失2%
    data.anti_ice_correction_factor = 0.95;      // 防冰损失5%
    
    // 创建性能曲线
    // 起飞模式
    EnginePerformanceCurve takeoff_curve;
    takeoff_curve.engine_mode = "takeoff";
    takeoff_curve.throttle_position = 1.0;
    
    // 添加起飞模式数据点
    for (int alt = 0; alt <= 10000; alt += 1000) {
        for (double mach = 0.0; mach <= 0.3; mach += 0.1) {
            ThrustDataPoint point;
            point.altitude = alt;
            point.mach_number = mach;
            point.temperature = 288.15 - 0.0065 * alt;
            point.pressure_ratio = 1.0;
            point.thrust = data.calculate_thrust(alt, mach, point.temperature, 1.0, 1.0);
            point.fuel_flow = data.calculate_fuel_flow(alt, mach, point.temperature, 1.0, 1.0);
            point.n1_rpm = data.calculate_n1_rpm(1.0, alt, mach);
            point.n2_rpm = data.calculate_n2_rpm(1.0, alt, mach);
            point.egt = data.calculate_egt(1.0, alt, mach);
            point.thrust_specific_fuel_consumption = point.fuel_flow / point.thrust;
            
            takeoff_curve.data_points.push_back(point);
        }
    }
    
    data.performance_curves["takeoff"] = takeoff_curve;
    
    // 巡航模式
    EnginePerformanceCurve cruise_curve;
    cruise_curve.engine_mode = "cruise";
    cruise_curve.throttle_position = 0.7;
    
    // 添加巡航模式数据点
    for (int alt = 8000; alt <= 12000; alt += 1000) {
        for (double mach = 0.7; mach <= 0.8; mach += 0.05) {
            ThrustDataPoint point;
            point.altitude = alt;
            point.mach_number = mach;
            point.temperature = 288.15 - 0.0065 * alt;
            point.pressure_ratio = 1.0;
            point.thrust = data.calculate_thrust(alt, mach, point.temperature, 0.7, 1.0);
            point.fuel_flow = data.calculate_fuel_flow(alt, mach, point.temperature, 0.7, 1.0);
            point.n1_rpm = data.calculate_n1_rpm(0.7, alt, mach);
            point.n2_rpm = data.calculate_n2_rpm(0.7, alt, mach);
            point.egt = data.calculate_egt(0.7, alt, mach);
            point.thrust_specific_fuel_consumption = point.fuel_flow / point.thrust;
            
            cruise_curve.data_points.push_back(point);
        }
    }
    
    data.performance_curves["cruise"] = cruise_curve;
    
    return data;
}();

// ==================== B737-700推力数据 (CFM56-7B20) ====================
const B737ThrustData B737_700_THRUST_DATA = []() {
    B737ThrustData data;
    
    // 基本信息
    data.aircraft_type = "B737-700";
    data.engine_type = "CFM56-7B20";
    data.engine_manufacturer = "CFM International";
    data.engine_count = 2;
    data.data_source = "CFM International Engine Data";
    data.data_version = "1.0";
    
    // 发动机几何参数（相同）
    data.engine_length = 2.36;          // m
    data.engine_diameter = 1.55;        // m
    data.engine_weight = 2381.0;        // kg
    data.engine_mount_position_x = 10.8; // m (从机头)
    data.engine_mount_position_y = 2.86; // m (从机身中心线)
    data.engine_mount_position_z = 0.0;  // m (相对于机翼)
    
    // 发动机限制（较低推力）
    data.engine_limits.max_n1_rpm = 5500.0;      // RPM
    data.engine_limits.max_n2_rpm = 15000.0;     // RPM
    data.engine_limits.max_egt = 950.0;          // K
    data.engine_limits.max_epr = 1.74;           // 发动机压力比
    data.engine_limits.max_thrust_takeoff = 89000.0;   // N
    data.engine_limits.max_thrust_climb = 80100.0;     // N
    data.engine_limits.max_thrust_cruise = 62300.0;    // N
    data.engine_limits.max_thrust_continuous = 62300.0; // N
    data.engine_limits.max_thrust_5min = 89000.0;      // N
    data.engine_limits.max_thrust_2min = 89000.0;      // N
    data.engine_limits.max_thrust_30sec = 89000.0;     // N
    
    // 发动机启动数据（相同）
    data.start_data.starter_torque = 5000.0;     // N·m
    data.start_data.starter_speed = 800.0;       // RPM
    data.start_data.light_off_n2 = 1200.0;       // RPM
    data.start_data.light_off_egt = 600.0;       // K
    data.start_data.idle_n1 = 1200.0;            // RPM
    data.start_data.idle_n2 = 4200.0;            // RPM
    data.start_data.idle_egt = 750.0;            // K
    data.start_data.idle_fuel_flow = 350.0;      // kg/h
    data.start_data.start_time = 30.0;           // s
    
    // 发动机故障数据（相同）
    data.failure_data.failure_thrust_ratio = 0.0;        // 完全失效
    data.failure_data.failure_fuel_flow_ratio = 0.0;     // 燃油流量为0
    data.failure_data.failure_egt_increase = 100.0;      // K
    data.failure_data.failure_n1_decrease = 1000.0;      // RPM
    data.failure_data.failure_n2_decrease = 3000.0;      // RPM
    
    // 燃油系统数据（相同）
    data.fuel_density = 800.0;                   // kg/m³
    data.fuel_lower_heating_value = 43.0e6;      // J/kg
    data.fuel_tank_capacity = 20820.0;           // kg
    data.fuel_pump_flow_rate = 5000.0;           // kg/h
    
    // 推力修正因子（相同）
    data.altitude_correction_factor = 1.0;
    data.temperature_correction_factor = 1.0;
    data.humidity_correction_factor = 1.0;
    data.bleed_air_correction_factor = 0.98;     // 引气损失2%
    data.anti_ice_correction_factor = 0.95;      // 防冰损失5%
    
    // 创建性能曲线（与B737-800相同结构，但推力较低）
    // 起飞模式
    EnginePerformanceCurve takeoff_curve;
    takeoff_curve.engine_mode = "takeoff";
    takeoff_curve.throttle_position = 1.0;
    
    // 添加起飞模式数据点
    for (int alt = 0; alt <= 10000; alt += 1000) {
        for (double mach = 0.0; mach <= 0.3; mach += 0.1) {
            ThrustDataPoint point;
            point.altitude = alt;
            point.mach_number = mach;
            point.temperature = 288.15 - 0.0065 * alt;
            point.pressure_ratio = 1.0;
            point.thrust = data.calculate_thrust(alt, mach, point.temperature, 1.0, 1.0);
            point.fuel_flow = data.calculate_fuel_flow(alt, mach, point.temperature, 1.0, 1.0);
            point.n1_rpm = data.calculate_n1_rpm(1.0, alt, mach);
            point.n2_rpm = data.calculate_n2_rpm(1.0, alt, mach);
            point.egt = data.calculate_egt(1.0, alt, mach);
            point.thrust_specific_fuel_consumption = point.fuel_flow / point.thrust;
            
            takeoff_curve.data_points.push_back(point);
        }
    }
    
    data.performance_curves["takeoff"] = takeoff_curve;
    
    // 巡航模式
    EnginePerformanceCurve cruise_curve;
    cruise_curve.engine_mode = "cruise";
    cruise_curve.throttle_position = 0.7;
    
    // 添加巡航模式数据点
    for (int alt = 8000; alt <= 12000; alt += 1000) {
        for (double mach = 0.7; mach <= 0.8; mach += 0.05) {
            ThrustDataPoint point;
            point.altitude = alt;
            point.mach_number = mach;
            point.temperature = 288.15 - 0.0065 * alt;
            point.pressure_ratio = 1.0;
            point.thrust = data.calculate_thrust(alt, mach, point.temperature, 0.7, 1.0);
            point.fuel_flow = data.calculate_fuel_flow(alt, mach, point.temperature, 0.7, 1.0);
            point.n1_rpm = data.calculate_n1_rpm(0.7, alt, mach);
            point.n2_rpm = data.calculate_n2_rpm(0.7, alt, mach);
            point.egt = data.calculate_egt(0.7, alt, mach);
            point.thrust_specific_fuel_consumption = point.fuel_flow / point.thrust;
            
            cruise_curve.data_points.push_back(point);
        }
    }
    
    data.performance_curves["cruise"] = cruise_curve;
    
    return data;
}();

// ==================== B737-900推力数据 (CFM56-7B27) ====================
const B737ThrustData B737_900_THRUST_DATA = []() {
    B737ThrustData data;
    
    // 基本信息
    data.aircraft_type = "B737-900";
    data.engine_type = "CFM56-7B27";
    data.engine_manufacturer = "CFM International";
    data.engine_count = 2;
    data.data_source = "CFM International Engine Data";
    data.data_version = "1.0";
    
    // 发动机几何参数（相同）
    data.engine_length = 2.36;          // m
    data.engine_diameter = 1.55;        // m
    data.engine_weight = 2381.0;        // kg
    data.engine_mount_position_x = 14.4; // m (从机头)
    data.engine_mount_position_y = 2.86; // m (从机身中心线)
    data.engine_mount_position_z = 0.0;  // m (相对于机翼)
    
    // 发动机限制（较高推力）
    data.engine_limits.max_n1_rpm = 5500.0;      // RPM
    data.engine_limits.max_n2_rpm = 15000.0;     // RPM
    data.engine_limits.max_egt = 950.0;          // K
    data.engine_limits.max_epr = 1.74;           // 发动机压力比
    data.engine_limits.max_thrust_takeoff = 121400.0; // N
    data.engine_limits.max_thrust_climb = 109260.0;   // N
    data.engine_limits.max_thrust_cruise = 85000.0;   // N
    data.engine_limits.max_thrust_continuous = 85000.0; // N
    data.engine_limits.max_thrust_5min = 121400.0;    // N
    data.engine_limits.max_thrust_2min = 121400.0;    // N
    data.engine_limits.max_thrust_30sec = 121400.0;   // N
    
    // 发动机启动数据（相同）
    data.start_data.starter_torque = 5000.0;     // N·m
    data.start_data.starter_speed = 800.0;       // RPM
    data.start_data.light_off_n2 = 1200.0;       // RPM
    data.start_data.light_off_egt = 600.0;       // K
    data.start_data.idle_n1 = 1200.0;            // RPM
    data.start_data.idle_n2 = 4200.0;            // RPM
    data.start_data.idle_egt = 750.0;            // K
    data.start_data.idle_fuel_flow = 420.0;      // kg/h
    data.start_data.start_time = 30.0;           // s
    
    // 发动机故障数据（相同）
    data.failure_data.failure_thrust_ratio = 0.0;        // 完全失效
    data.failure_data.failure_fuel_flow_ratio = 0.0;     // 燃油流量为0
    data.failure_data.failure_egt_increase = 100.0;      // K
    data.failure_data.failure_n1_decrease = 1000.0;      // RPM
    data.failure_data.failure_n2_decrease = 3000.0;      // RPM
    
    // 燃油系统数据（相同）
    data.fuel_density = 800.0;                   // kg/m³
    data.fuel_lower_heating_value = 43.0e6;      // J/kg
    data.fuel_tank_capacity = 20820.0;           // kg
    data.fuel_pump_flow_rate = 5000.0;           // kg/h
    
    // 推力修正因子（相同）
    data.altitude_correction_factor = 1.0;
    data.temperature_correction_factor = 1.0;
    data.humidity_correction_factor = 1.0;
    data.bleed_air_correction_factor = 0.98;     // 引气损失2%
    data.anti_ice_correction_factor = 0.95;      // 防冰损失5%
    
    // 创建性能曲线（与B737-800相同结构，但推力较高）
    // 起飞模式
    EnginePerformanceCurve takeoff_curve;
    takeoff_curve.engine_mode = "takeoff";
    takeoff_curve.throttle_position = 1.0;
    
    // 添加起飞模式数据点
    for (int alt = 0; alt <= 10000; alt += 1000) {
        for (double mach = 0.0; mach <= 0.3; mach += 0.1) {
            ThrustDataPoint point;
            point.altitude = alt;
            point.mach_number = mach;
            point.temperature = 288.15 - 0.0065 * alt;
            point.pressure_ratio = 1.0;
            point.thrust = data.calculate_thrust(alt, mach, point.temperature, 1.0, 1.0);
            point.fuel_flow = data.calculate_fuel_flow(alt, mach, point.temperature, 1.0, 1.0);
            point.n1_rpm = data.calculate_n1_rpm(1.0, alt, mach);
            point.n2_rpm = data.calculate_n2_rpm(1.0, alt, mach);
            point.egt = data.calculate_egt(1.0, alt, mach);
            point.thrust_specific_fuel_consumption = point.fuel_flow / point.thrust;
            
            takeoff_curve.data_points.push_back(point);
        }
    }
    
    data.performance_curves["takeoff"] = takeoff_curve;
    
    // 巡航模式
    EnginePerformanceCurve cruise_curve;
    cruise_curve.engine_mode = "cruise";
    cruise_curve.throttle_position = 0.7;
    
    // 添加巡航模式数据点
    for (int alt = 8000; alt <= 12000; alt += 1000) {
        for (double mach = 0.7; mach <= 0.8; mach += 0.05) {
            ThrustDataPoint point;
            point.altitude = alt;
            point.mach_number = mach;
            point.temperature = 288.15 - 0.0065 * alt;
            point.pressure_ratio = 1.0;
            point.thrust = data.calculate_thrust(alt, mach, point.temperature, 0.7, 1.0);
            point.fuel_flow = data.calculate_fuel_flow(alt, mach, point.temperature, 0.7, 1.0);
            point.n1_rpm = data.calculate_n1_rpm(0.7, alt, mach);
            point.n2_rpm = data.calculate_n2_rpm(0.7, alt, mach);
            point.egt = data.calculate_egt(0.7, alt, mach);
            point.thrust_specific_fuel_consumption = point.fuel_flow / point.thrust;
            
            cruise_curve.data_points.push_back(point);
        }
    }
    
    data.performance_curves["cruise"] = cruise_curve;
    
    return data;
}();

} // namespace B737
} // namespace AircraftDigitalTwin
} // namespace SMF 