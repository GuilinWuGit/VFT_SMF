/**
 * @file B737_AeroControlIncrementalForces.hpp
 * @brief B737飞机气动操纵面6分量气动力增量数据定义
 * @details 包含B737飞机的副翼、升降舵、方向舵、扰流板等操纵面偏转产生的力和力矩增量
 * @author VFT_SMF Framework
 * @date 2024
 */

#ifndef B737_AERO_CONTROL_INCREMENTAL_FORCES_HPP
#define B737_AERO_CONTROL_INCREMENTAL_FORCES_HPP

#include <string>
#include <vector>
#include <map>
#include <array>

namespace SMF {
namespace AircraftDigitalTwin {
namespace B737 {

/**
 * @brief 操纵面气动力增量数据点结构体
 * @details 定义特定条件下操纵面偏转产生的气动力增量
 */
struct ControlForceIncrementPoint {
    double deflection_angle;            ///< 操纵面偏角 (度)
    double mach_number;                 ///< 马赫数
    double reynolds_number;             ///< 雷诺数
    double angle_of_attack;             ///< 迎角 (度)
    double sideslip_angle;              ///< 侧滑角 (度)
    
    // 6分量气动力增量（机体轴系，以重心为参考点）
    double delta_force_x;               ///< X轴力增量 (N)
    double delta_force_y;               ///< Y轴力增量 (N)
    double delta_force_z;               ///< Z轴力增量 (N)
    double delta_moment_x;              ///< X轴力矩增量 (N·m)
    double delta_moment_y;              ///< Y轴力矩增量 (N·m)
    double delta_moment_z;              ///< Z轴力矩增量 (N·m)
    
    // 无量纲系数增量
    double delta_cx;                    ///< X轴力系数增量
    double delta_cy;                    ///< Y轴力系数增量
    double delta_cz;                    ///< Z轴力系数增量
    double delta_cl;                    ///< 滚转力矩系数增量
    double delta_cm;                    ///< 俯仰力矩系数增量
    double delta_cn;                    ///< 偏航力矩系数增量
    
    ControlForceIncrementPoint() : deflection_angle(0.0), mach_number(0.0), reynolds_number(0.0),
                                  angle_of_attack(0.0), sideslip_angle(0.0),
                                  delta_force_x(0.0), delta_force_y(0.0), delta_force_z(0.0),
                                  delta_moment_x(0.0), delta_moment_y(0.0), delta_moment_z(0.0),
                                  delta_cx(0.0), delta_cy(0.0), delta_cz(0.0),
                                  delta_cl(0.0), delta_cm(0.0), delta_cn(0.0) {}
};

/**
 * @brief 操纵面气动力导数结构体
 * @details 定义操纵面偏转对气动力的导数
 */
struct ControlForceDerivatives {
    // 力导数（相对于操纵面偏角）
    double d_cx_d_delta;                ///< X轴力系数对操纵面偏角的导数
    double d_cy_d_delta;                ///< Y轴力系数对操纵面偏角的导数
    double d_cz_d_delta;                ///< Z轴力系数对操纵面偏角的导数
    
    // 力矩导数（相对于操纵面偏角）
    double d_cl_d_delta;                ///< 滚转力矩系数对操纵面偏角的导数
    double d_cm_d_delta;                ///< 俯仰力矩系数对操纵面偏角的导数
    double d_cn_d_delta;                ///< 偏航力矩系数对操纵面偏角的导数
    
    // 交叉导数（相对于迎角和侧滑角）
    double d_cx_d_alpha;                ///< X轴力系数对迎角的导数
    double d_cy_d_beta;                 ///< Y轴力系数对侧滑角的导数
    double d_cz_d_alpha;                ///< Z轴力系数对迎角的导数
    double d_cl_d_beta;                 ///< 滚转力矩系数对侧滑角的导数
    double d_cm_d_alpha;                ///< 俯仰力矩系数对迎角的导数
    double d_cn_d_beta;                 ///< 偏航力矩系数对侧滑角的导数
    
    // 马赫数修正因子
    double mach_correction_factor;      ///< 马赫数修正因子
    
    ControlForceDerivatives() : d_cx_d_delta(0.0), d_cy_d_delta(0.0), d_cz_d_delta(0.0),
                               d_cl_d_delta(0.0), d_cm_d_delta(0.0), d_cn_d_delta(0.0),
                               d_cx_d_alpha(0.0), d_cy_d_beta(0.0), d_cz_d_alpha(0.0),
                               d_cl_d_beta(0.0), d_cm_d_alpha(0.0), d_cn_d_beta(0.0),
                               mach_correction_factor(1.0) {}
};

/**
 * @brief 操纵面气动力增量曲线结构体
 * @details 定义操纵面的气动力增量特性曲线
 */
struct ControlForceIncrementCurve {
    std::string control_surface;        ///< 操纵面名称
    std::string control_mode;           ///< 控制模式
    std::vector<ControlForceIncrementPoint> data_points; ///< 气动力增量数据点
    ControlForceDerivatives derivatives; ///< 气动力导数
    
    ControlForceIncrementCurve() : control_surface(""), control_mode("") {}
};

/**
 * @brief 操纵面耦合气动力增量结构体
 * @details 定义操纵面之间的耦合气动力增量效应
 */
struct ControlCouplingForceIncrement {
    std::string primary_control;        ///< 主操纵面
    std::string secondary_control;      ///< 次操纵面
    double coupling_factor;             ///< 耦合因子
    
    // 耦合产生的气动力增量
    double delta_force_x_coupling;      ///< 耦合X轴力增量 (N)
    double delta_force_y_coupling;      ///< 耦合Y轴力增量 (N)
    double delta_force_z_coupling;      ///< 耦合Z轴力增量 (N)
    double delta_moment_x_coupling;     ///< 耦合X轴力矩增量 (N·m)
    double delta_moment_y_coupling;     ///< 耦合Y轴力矩增量 (N·m)
    double delta_moment_z_coupling;     ///< 耦合Z轴力矩增量 (N·m)
    
    ControlCouplingForceIncrement() : primary_control(""), secondary_control(""), coupling_factor(0.0),
                                     delta_force_x_coupling(0.0), delta_force_y_coupling(0.0), delta_force_z_coupling(0.0),
                                     delta_moment_x_coupling(0.0), delta_moment_y_coupling(0.0), delta_moment_z_coupling(0.0) {}
};

/**
 * @brief B737飞机气动操纵面6分量气动力增量数据结构体
 * @details 包含B737飞机的完整操纵面气动力增量数据
 */
struct B737AeroControlIncrementalForces {
    // ==================== 基本信息 ====================
    std::string aircraft_type;          ///< 飞机型号
    std::string data_source;            ///< 数据来源
    std::string data_version;           ///< 数据版本
    
    // ==================== 参考参数 ====================
    double reference_wing_area;         ///< 参考机翼面积 (m²)
    double reference_chord;             ///< 参考弦长 (m)
    double reference_span;              ///< 参考翼展 (m)
    double reference_dynamic_pressure;  ///< 参考动压 (Pa)
    
    // ==================== 操纵面气动力增量曲线 ====================
    std::map<std::string, ControlForceIncrementCurve> force_increment_curves; ///< 各操纵面气动力增量曲线
    
    // ==================== 操纵面耦合气动力增量 ====================
    std::vector<ControlCouplingForceIncrement> coupling_force_increments; ///< 操纵面耦合气动力增量
    
    // ==================== 构造方法 ====================
    B737AeroControlIncrementalForces();
    
    // ==================== 气动力增量计算方法 ====================
    ControlForceIncrementPoint calculate_force_increment(const std::string& control_surface,
                                                        double deflection_angle, double mach,
                                                        double reynolds, double alpha, double beta,
                                                        double dynamic_pressure) const;
    
    // ==================== 气动力导数计算方法 ====================
    ControlForceDerivatives calculate_derivatives(const std::string& control_surface,
                                                 double mach, double reynolds, double alpha, double beta) const;
    
    // ==================== 耦合气动力增量计算方法 ====================
    ControlCouplingForceIncrement calculate_coupling_force_increment(const std::string& primary_control,
                                                                    const std::string& secondary_control,
                                                                    double primary_deflection, double secondary_deflection,
                                                                    double mach, double dynamic_pressure) const;
    
    // ==================== 数据插值方法 ====================
    ControlForceIncrementPoint interpolate_force_increment_data(const std::string& control_surface,
                                                               double deflection_angle, double mach,
                                                               double reynolds, double alpha, double beta,
                                                               double dynamic_pressure) const;
    
    // ==================== 数据验证方法 ====================
    bool validate_data() const;
    std::string get_validation_report() const;
    
    // ==================== 数据加载方法 ====================
    bool load_from_file(const std::string& filename);
    bool save_to_file(const std::string& filename) const;
};

/**
 * @brief B737-800气动操纵面6分量气动力增量数据
 * @details B737-800型号的详细操纵面气动力增量参数
 */
extern const B737AeroControlIncrementalForces B737_800_CONTROL_FORCE_INCREMENT_DATA;

/**
 * @brief B737-700气动操纵面6分量气动力增量数据
 * @details B737-700型号的详细操纵面气动力增量参数
 */
extern const B737AeroControlIncrementalForces B737_700_CONTROL_FORCE_INCREMENT_DATA;

/**
 * @brief B737-900气动操纵面6分量气动力增量数据
 * @details B737-900型号的详细操纵面气动力增量参数
 */
extern const B737AeroControlIncrementalForces B737_900_CONTROL_FORCE_INCREMENT_DATA;

} // namespace B737
} // namespace AircraftDigitalTwin
} // namespace SMF

#endif // B737_AERO_CONTROL_INCREMENTAL_FORCES_HPP 