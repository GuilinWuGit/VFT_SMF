/**
 * @file B737_DigitalTwin.hpp
 * @brief B737飞机数字孪生主索引文件
 * @details 整合B737飞机的总体数据、气动数据和推力数据，提供统一的数字孪生接口
 * @author VFT_SMF Framework
 * @date 2024
 */

#ifndef B737_DIGITAL_TWIN_HPP
#define B737_DIGITAL_TWIN_HPP

#include "B737_GeneralData.hpp"
#include "B737_AerodynamicData.hpp"
#include "B737_ThrustData.hpp"
#include <memory>
#include <string>

namespace SMF {
namespace AircraftDigitalTwin {
namespace B737 {

/**
 * @brief B737飞机数字孪生类
 * @details 整合B737飞机的所有数据，提供统一的数字孪生功能
 */
class B737DigitalTwin {
private:
    // ==================== 数据成员 ====================
    std::shared_ptr<B737GeneralData> general_data;      ///< 总体数据
    std::shared_ptr<B737AerodynamicData> aerodynamic_data; ///< 气动数据
    std::shared_ptr<B737ThrustData> thrust_data;        ///< 推力数据
    
    std::string aircraft_serial_number;                 ///< 飞机序列号
    std::string registration_number;                    ///< 注册号
    std::string current_configuration;                  ///< 当前构型
    
    // ==================== 实时状态 ====================
    double current_altitude;                            ///< 当前高度 (m)
    double current_mach;                                ///< 当前马赫数
    double current_angle_of_attack;                     ///< 当前迎角 (度)
    double current_flap_deflection;                     ///< 当前襟翼偏角 (度)
    double current_gear_position;                       ///< 当前起落架位置
    double current_throttle_position;                   ///< 当前油门位置
    
    // ==================== 性能监控 ====================
    double total_flight_time;                           ///< 总飞行时间 (小时)
    double total_landings;                              ///< 总着陆次数
    double total_cycles;                                ///< 总飞行循环数
    double engine_1_running_time;                       ///< 1号发动机运行时间 (小时)
    double engine_2_running_time;                       ///< 2号发动机运行时间 (小时)
    
public:
    // ==================== 构造方法 ====================
    B737DigitalTwin();
    B737DigitalTwin(const std::string& aircraft_type);
    B737DigitalTwin(const std::string& aircraft_type, 
                   const std::string& serial_number,
                   const std::string& registration);
    
    // ==================== 数据访问方法 ====================
    const B737GeneralData& get_general_data() const;
    const B737AerodynamicData& get_aerodynamic_data() const;
    const B737ThrustData& get_thrust_data() const;
    
    // ==================== 飞机信息方法 ====================
    std::string get_aircraft_type() const;
    std::string get_serial_number() const;
    std::string get_registration() const;
    std::string get_engine_type() const;
    
    // ==================== 实时状态更新方法 ====================
    void update_flight_state(double altitude, double mach, double angle_of_attack,
                           double flap_deflection, double gear_position, double throttle_position);
    
    void update_configuration(const std::string& configuration);
    
    // ==================== 性能计算方法 ====================
    double calculate_current_lift_coefficient() const;
    double calculate_current_drag_coefficient() const;
    double calculate_current_thrust() const;
    double calculate_current_fuel_flow() const;
    
    // ==================== 飞行性能预测方法 ====================
    double predict_stall_speed() const;
    double predict_max_range() const;
    double predict_max_endurance() const;
    double predict_takeoff_distance() const;
    double predict_landing_distance() const;
    
    // ==================== 发动机性能监控方法 ====================
    double get_engine_1_n1() const;
    double get_engine_1_n2() const;
    double get_engine_1_egt() const;
    double get_engine_2_n1() const;
    double get_engine_2_n2() const;
    double get_engine_2_egt() const;
    
    // ==================== 维护监控方法 ====================
    double get_airframe_hours() const;
    double get_engine_1_hours() const;
    double get_engine_2_hours() const;
    int get_total_cycles() const;
    int get_total_landings() const;
    
    // ==================== 数据验证方法 ====================
    bool validate_all_data() const;
    std::string get_comprehensive_validation_report() const;
    
    // ==================== 数据加载和保存方法 ====================
    bool load_aircraft_data(const std::string& aircraft_type);
    bool save_aircraft_data(const std::string& filename) const;
    bool export_performance_report(const std::string& filename) const;
    
    // ==================== 故障模拟方法 ====================
    void simulate_engine_1_failure();
    void simulate_engine_2_failure();
    void simulate_dual_engine_failure();
    void reset_failures();
    
    // ==================== 数据统计方法 ====================
    double get_average_fuel_consumption() const;
    double get_average_thrust_utilization() const;
    double get_operational_efficiency() const;
    
private:
    // ==================== 私有辅助方法 ====================
    void initialize_default_data();
    bool load_b737_800_data();
    bool load_b737_700_data();
    bool load_b737_900_data();
    void update_performance_metrics();
};

/**
 * @brief B737数字孪生工厂类
 * @details 用于创建和管理不同型号的B737数字孪生实例
 */
class B737DigitalTwinFactory {
public:
    // ==================== 工厂方法 ====================
    static std::shared_ptr<B737DigitalTwin> create_b737_800(const std::string& serial_number = "",
                                                           const std::string& registration = "");
    
    static std::shared_ptr<B737DigitalTwin> create_b737_700(const std::string& serial_number = "",
                                                           const std::string& registration = "");
    
    static std::shared_ptr<B737DigitalTwin> create_b737_900(const std::string& serial_number = "",
                                                           const std::string& registration = "");
    
    static std::shared_ptr<B737DigitalTwin> create_by_type(const std::string& aircraft_type,
                                                          const std::string& serial_number = "",
                                                          const std::string& registration = "");
    
    // ==================== 数据管理方法 ====================
    static bool validate_aircraft_type(const std::string& aircraft_type);
    static std::vector<std::string> get_available_aircraft_types();
    static std::string get_aircraft_description(const std::string& aircraft_type);
};

} // namespace B737
} // namespace AircraftDigitalTwin
} // namespace SMF

#endif // B737_DIGITAL_TWIN_HPP 