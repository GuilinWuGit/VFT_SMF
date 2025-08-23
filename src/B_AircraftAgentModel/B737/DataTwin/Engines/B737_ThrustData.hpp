/**
 * @file B737_ThrustData.hpp
 * @brief B737飞机推力数据定义
 * @details 包含B737飞机的发动机推力特性、燃油消耗、发动机性能等数据
 * @author VFT_SMF Framework
 * @date 2024
 */

#ifndef B737_THRUST_DATA_HPP
#define B737_THRUST_DATA_HPP

#include <string>
#include <vector>
#include <map>
#include <array>

namespace SMF {
namespace AircraftDigitalTwin {
namespace B737 {

/**
 * @brief 发动机推力数据点结构体
 * @details 定义特定条件下的发动机推力特性
 */
struct ThrustDataPoint {
    double altitude;                    ///< 高度 (m)
    double mach_number;                 ///< 马赫数
    double temperature;                 ///< 温度 (K)
    double pressure_ratio;              ///< 压力比
    double n1_rpm;                      ///< 低压转子转速 (RPM)
    double n2_rpm;                      ///< 高压转子转速 (RPM)
    double egt;                         ///< 排气温度 (K)
    double fuel_flow;                   ///< 燃油流量 (kg/h)
    double thrust;                      ///< 推力 (N)
    double thrust_specific_fuel_consumption; ///< 单位推力燃油消耗率 (kg/N/h)
    
    ThrustDataPoint() : altitude(0.0), mach_number(0.0), temperature(288.15),
                       pressure_ratio(1.0), n1_rpm(0.0), n2_rpm(0.0), egt(0.0),
                       fuel_flow(0.0), thrust(0.0), thrust_specific_fuel_consumption(0.0) {}
};

/**
 * @brief 发动机性能曲线结构体
 * @details 定义不同工作模式下的发动机性能曲线
 */
struct EnginePerformanceCurve {
    std::string engine_mode;            ///< 发动机工作模式
    double throttle_position;           ///< 油门位置 [0.0, 1.0]
    std::vector<ThrustDataPoint> data_points; ///< 推力数据点
    
    EnginePerformanceCurve() : engine_mode(""), throttle_position(0.0) {}
};

/**
 * @brief 发动机限制结构体
 * @details 定义发动机的各种操作限制
 */
struct EngineLimits {
    double max_n1_rpm;                  ///< 最大低压转子转速 (RPM)
    double max_n2_rpm;                  ///< 最大高压转子转速 (RPM)
    double max_egt;                     ///< 最大排气温度 (K)
    double max_epr;                     ///< 最大发动机压力比
    double max_thrust_takeoff;          ///< 最大起飞推力 (N)
    double max_thrust_climb;            ///< 最大爬升推力 (N)
    double max_thrust_cruise;           ///< 最大巡航推力 (N)
    double max_thrust_continuous;       ///< 最大连续推力 (N)
    double max_thrust_5min;             ///< 5分钟最大推力 (N)
    double max_thrust_2min;             ///< 2分钟最大推力 (N)
    double max_thrust_30sec;            ///< 30秒最大推力 (N)
    
    EngineLimits() : max_n1_rpm(0.0), max_n2_rpm(0.0), max_egt(0.0), max_epr(0.0),
                    max_thrust_takeoff(0.0), max_thrust_climb(0.0), max_thrust_cruise(0.0),
                    max_thrust_continuous(0.0), max_thrust_5min(0.0), max_thrust_2min(0.0),
                    max_thrust_30sec(0.0) {}
};

/**
 * @brief 发动机启动数据结构体
 * @details 定义发动机启动过程的参数
 */
struct EngineStartData {
    double starter_torque;              ///< 启动机扭矩 (N·m)
    double starter_speed;               ///< 启动机转速 (RPM)
    double light_off_n2;                ///< 点火N2转速 (RPM)
    double light_off_egt;               ///< 点火排气温度 (K)
    double idle_n1;                     ///< 慢车N1转速 (RPM)
    double idle_n2;                     ///< 慢车N2转速 (RPM)
    double idle_egt;                    ///< 慢车排气温度 (K)
    double idle_fuel_flow;              ///< 慢车燃油流量 (kg/h)
    double start_time;                  ///< 启动时间 (s)
    
    EngineStartData() : starter_torque(0.0), starter_speed(0.0), light_off_n2(0.0),
                       light_off_egt(0.0), idle_n1(0.0), idle_n2(0.0), idle_egt(0.0),
                       idle_fuel_flow(0.0), start_time(0.0) {}
};

/**
 * @brief 发动机故障模式结构体
 * @details 定义发动机故障时的性能参数
 */
struct EngineFailureData {
    double failure_thrust_ratio;        ///< 故障推力比
    double failure_fuel_flow_ratio;     ///< 故障燃油流量比
    double failure_egt_increase;        ///< 故障排气温度增加 (K)
    double failure_n1_decrease;         ///< 故障N1转速减少 (RPM)
    double failure_n2_decrease;         ///< 故障N2转速减少 (RPM)
    
    EngineFailureData() : failure_thrust_ratio(0.0), failure_fuel_flow_ratio(0.0),
                         failure_egt_increase(0.0), failure_n1_decrease(0.0), failure_n2_decrease(0.0) {}
};

/**
 * @brief B737飞机推力数据结构体
 * @details 包含B737飞机的完整推力特性数据
 */
struct B737ThrustData {
    // ==================== 基本信息 ====================
    std::string aircraft_type;          ///< 飞机型号
    std::string engine_type;            ///< 发动机型号
    std::string engine_manufacturer;    ///< 发动机制造商
    int engine_count;                   ///< 发动机数量
    std::string data_source;            ///< 数据来源
    std::string data_version;           ///< 数据版本
    
    // ==================== 发动机几何参数 ====================
    double engine_length;               ///< 发动机长度 (m)
    double engine_diameter;             ///< 发动机直径 (m)
    double engine_weight;               ///< 发动机重量 (kg)
    double engine_mount_position_x;     ///< 发动机安装位置X坐标 (m)
    double engine_mount_position_y;     ///< 发动机安装位置Y坐标 (m)
    double engine_mount_position_z;     ///< 发动机安装位置Z坐标 (m)
    
    // ==================== 发动机性能曲线 ====================
    std::map<std::string, EnginePerformanceCurve> performance_curves; ///< 不同模式的性能曲线
    
    // ==================== 发动机限制 ====================
    EngineLimits engine_limits;         ///< 发动机操作限制
    
    // ==================== 发动机启动数据 ====================
    EngineStartData start_data;         ///< 发动机启动数据
    
    // ==================== 发动机故障数据 ====================
    EngineFailureData failure_data;     ///< 发动机故障数据
    
    // ==================== 燃油系统数据 ====================
    double fuel_density;                ///< 燃油密度 (kg/m³)
    double fuel_lower_heating_value;    ///< 燃油低热值 (J/kg)
    double fuel_tank_capacity;          ///< 燃油箱容量 (kg)
    double fuel_pump_flow_rate;         ///< 燃油泵流量 (kg/h)
    
    // ==================== 推力修正因子 ====================
    double altitude_correction_factor;  ///< 高度修正因子
    double temperature_correction_factor; ///< 温度修正因子
    double humidity_correction_factor;  ///< 湿度修正因子
    double bleed_air_correction_factor; ///< 引气修正因子
    double anti_ice_correction_factor;  ///< 防冰修正因子
    
    // ==================== 构造方法 ====================
    B737ThrustData();
    
    // ==================== 推力计算方法 ====================
    double calculate_thrust(double altitude, double mach, double temperature,
                           double throttle_position, double pressure_ratio) const;
    
    double calculate_fuel_flow(double altitude, double mach, double temperature,
                              double throttle_position, double pressure_ratio) const;
    
    double calculate_tsfc(double altitude, double mach, double temperature,
                         double throttle_position) const;
    
    // ==================== 发动机状态计算方法 ====================
    double calculate_n1_rpm(double throttle_position, double altitude, double mach) const;
    double calculate_n2_rpm(double throttle_position, double altitude, double mach) const;
    double calculate_egt(double throttle_position, double altitude, double mach) const;
    
    // ==================== 数据插值方法 ====================
    ThrustDataPoint interpolate_thrust_data(double altitude, double mach, double temperature,
                                           double throttle_position) const;
    
    // ==================== 发动机启动模拟 ====================
    bool simulate_engine_start(double time_step, double& n1, double& n2, double& egt, double& fuel_flow) const;
    
    // ==================== 发动机故障模拟 ====================
    void simulate_engine_failure(double& thrust, double& fuel_flow, double& egt, 
                                double& n1, double& n2) const;
    
    // ==================== 数据验证方法 ====================
    bool validate_data() const;
    std::string get_validation_report() const;
    
    // ==================== 数据加载方法 ====================
    bool load_from_file(const std::string& filename);
    bool save_to_file(const std::string& filename) const;
};

/**
 * @brief B737-800推力数据 (CFM56-7B26)
 * @details B737-800型号的详细推力参数
 */
extern const B737ThrustData B737_800_THRUST_DATA;

/**
 * @brief B737-700推力数据 (CFM56-7B20)
 * @details B737-700型号的详细推力参数
 */
extern const B737ThrustData B737_700_THRUST_DATA;

/**
 * @brief B737-900推力数据 (CFM56-7B27)
 * @details B737-900型号的详细推力参数
 */
extern const B737ThrustData B737_900_THRUST_DATA;

} // namespace B737
} // namespace AircraftDigitalTwin
} // namespace SMF

#endif // B737_THRUST_DATA_HPP 