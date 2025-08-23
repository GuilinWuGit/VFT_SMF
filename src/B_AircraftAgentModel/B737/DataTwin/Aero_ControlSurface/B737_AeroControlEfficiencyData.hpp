/**
 * @file B737_AeroControlEfficiencyData.hpp
 * @brief B737飞机气动操纵面效率数据定义
 * @details 包含B737飞机的副翼、升降舵、方向舵、扰流板等操纵面的气动效率数据
 * @author VFT_SMF Framework
 * @date 2024
 */

#ifndef B737_AERO_CONTROL_EFFICIENCY_DATA_HPP
#define B737_AERO_CONTROL_EFFICIENCY_DATA_HPP

#include <string>
#include <vector>
#include <map>
#include <array>

namespace SMF {
namespace AircraftDigitalTwin {
namespace B737 {

/**
 * @brief 操纵面几何参数结构体
 * @details 定义操纵面的基本几何参数
 */
struct ControlSurfaceGeometry {
    std::string surface_name;           ///< 操纵面名称
    double span;                        ///< 展长 (m)
    double chord;                       ///< 弦长 (m)
    double area;                        ///< 面积 (m²)
    double hinge_line_position;         ///< 铰链线位置 (%弦长)
    double deflection_angle_max;        ///< 最大偏角 (度)
    double deflection_angle_min;        ///< 最小偏角 (度)
    double deflection_rate_max;         ///< 最大偏转速率 (度/s)
    double deflection_rate_min;         ///< 最小偏转速率 (度/s)
    
    ControlSurfaceGeometry() : surface_name(""), span(0.0), chord(0.0), area(0.0),
                              hinge_line_position(0.0), deflection_angle_max(0.0),
                              deflection_angle_min(0.0), deflection_rate_max(0.0),
                              deflection_rate_min(0.0) {}
};

/**
 * @brief 操纵面效率数据点结构体
 * @details 定义特定条件下的操纵面效率
 */
struct ControlEfficiencyPoint {
    double deflection_angle;            ///< 偏角 (度)
    double mach_number;                 ///< 马赫数
    double reynolds_number;             ///< 雷诺数
    double angle_of_attack;             ///< 迎角 (度)
    double sideslip_angle;              ///< 侧滑角 (度)
    
    double effectiveness_factor;         ///< 效率因子 [0.0, 1.0]
    double control_derivative;          ///< 控制导数
    double hinge_moment_coefficient;    ///< 铰链力矩系数
    double power_required;              ///< 所需功率 (W)
    double response_time;               ///< 响应时间 (s)
    
    ControlEfficiencyPoint() : deflection_angle(0.0), mach_number(0.0), reynolds_number(0.0),
                              angle_of_attack(0.0), sideslip_angle(0.0), effectiveness_factor(0.0),
                              control_derivative(0.0), hinge_moment_coefficient(0.0),
                              power_required(0.0), response_time(0.0) {}
};

/**
 * @brief 操纵面效率曲线结构体
 * @details 定义操纵面的效率特性曲线
 */
struct ControlEfficiencyCurve {
    std::string control_mode;           ///< 控制模式
    std::vector<ControlEfficiencyPoint> data_points; ///< 效率数据点
    
    ControlEfficiencyCurve() : control_mode("") {}
};

/**
 * @brief 操纵面耦合效应结构体
 * @details 定义操纵面之间的耦合效应
 */
struct ControlCouplingEffect {
    std::string primary_control;        ///< 主操纵面
    std::string secondary_control;      ///< 次操纵面
    double coupling_factor;             ///< 耦合因子
    double coupling_phase;              ///< 耦合相位 (度)
    double coupling_frequency;          ///< 耦合频率 (Hz)
    
    ControlCouplingEffect() : primary_control(""), secondary_control(""), coupling_factor(0.0),
                             coupling_phase(0.0), coupling_frequency(0.0) {}
};

/**
 * @brief 操纵面故障模式结构体
 * @details 定义操纵面故障时的性能参数
 */
struct ControlFailureMode {
    std::string failure_type;           ///< 故障类型
    double effectiveness_reduction;      ///< 效率降低比例
    double response_delay;              ///< 响应延迟 (s)
    double jamming_angle;               ///< 卡阻角度 (度)
    double free_play;                   ///< 自由间隙 (度)
    
    ControlFailureMode() : failure_type(""), effectiveness_reduction(0.0), response_delay(0.0),
                          jamming_angle(0.0), free_play(0.0) {}
};

/**
 * @brief B737飞机气动操纵面效率数据结构体
 * @details 包含B737飞机的完整操纵面效率数据
 */
struct B737AeroControlEfficiencyData {
    // ==================== 基本信息 ====================
    std::string aircraft_type;          ///< 飞机型号
    std::string data_source;            ///< 数据来源
    std::string data_version;           ///< 数据版本
    
    // ==================== 操纵面几何参数 ====================
    std::map<std::string, ControlSurfaceGeometry> control_surfaces; ///< 各操纵面几何参数
    
    // ==================== 操纵面效率曲线 ====================
    std::map<std::string, ControlEfficiencyCurve> efficiency_curves; ///< 各操纵面效率曲线
    
    // ==================== 操纵面耦合效应 ====================
    std::vector<ControlCouplingEffect> coupling_effects; ///< 操纵面耦合效应
    
    // ==================== 操纵面故障模式 ====================
    std::map<std::string, ControlFailureMode> failure_modes; ///< 各操纵面故障模式
    
    // ==================== 操纵面性能参数 ====================
    double max_roll_rate;               ///< 最大滚转角速度 (度/s)
    double max_pitch_rate;              ///< 最大俯仰角速度 (度/s)
    double max_yaw_rate;                ///< 最大偏航角速度 (度/s)
    double roll_response_time;          ///< 滚转响应时间 (s)
    double pitch_response_time;         ///< 俯仰响应时间 (s)
    double yaw_response_time;           ///< 偏航响应时间 (s)
    
    // ==================== 操纵面限制 ====================
    double max_control_deflection_speed; ///< 最大操纵偏转速度 (度/s)
    double min_control_deflection_speed; ///< 最小操纵偏转速度 (度/s)
    double control_authority_limit;     ///< 操纵权限限制
    double control_redundancy_factor;   ///< 操纵冗余因子
    
    // ==================== 构造方法 ====================
    B737AeroControlEfficiencyData();
    
    // ==================== 操纵面效率计算方法 ====================
    double calculate_control_effectiveness(const std::string& control_surface, 
                                         double deflection_angle, double mach, 
                                         double reynolds, double alpha, double beta) const;
    
    double calculate_control_derivative(const std::string& control_surface,
                                      double deflection_angle, double mach,
                                      double reynolds, double alpha, double beta) const;
    
    double calculate_hinge_moment(const std::string& control_surface,
                                double deflection_angle, double mach,
                                double reynolds, double alpha, double beta) const;
    
    // ==================== 操纵面响应计算方法 ====================
    double calculate_response_time(const std::string& control_surface,
                                 double deflection_angle, double mach) const;
    
    double calculate_power_required(const std::string& control_surface,
                                  double deflection_angle, double deflection_rate) const;
    
    // ==================== 操纵面耦合计算方法 ====================
    double calculate_coupling_effect(const std::string& primary_control,
                                   const std::string& secondary_control,
                                   double primary_deflection, double secondary_deflection) const;
    
    // ==================== 操纵面故障模拟 ====================
    void simulate_control_failure(const std::string& control_surface,
                                const std::string& failure_type,
                                double& effectiveness, double& response_time) const;
    
    // ==================== 数据插值方法 ====================
    ControlEfficiencyPoint interpolate_efficiency_data(const std::string& control_surface,
                                                      double deflection_angle, double mach,
                                                      double reynolds, double alpha, double beta) const;
    
    // ==================== 数据验证方法 ====================
    bool validate_data() const;
    std::string get_validation_report() const;
    
    // ==================== 数据加载方法 ====================
    bool load_from_file(const std::string& filename);
    bool save_to_file(const std::string& filename) const;
};

/**
 * @brief B737-800气动操纵面效率数据
 * @details B737-800型号的详细操纵面效率参数
 */
extern const B737AeroControlEfficiencyData B737_800_CONTROL_EFFICIENCY_DATA;

/**
 * @brief B737-700气动操纵面效率数据
 * @details B737-700型号的详细操纵面效率参数
 */
extern const B737AeroControlEfficiencyData B737_700_CONTROL_EFFICIENCY_DATA;

/**
 * @brief B737-900气动操纵面效率数据
 * @details B737-900型号的详细操纵面效率参数
 */
extern const B737AeroControlEfficiencyData B737_900_CONTROL_EFFICIENCY_DATA;

} // namespace B737
} // namespace AircraftDigitalTwin
} // namespace SMF

#endif // B737_AERO_CONTROL_EFFICIENCY_DATA_HPP 