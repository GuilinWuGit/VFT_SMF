/**
 * @file B737_AerodynamicData.hpp
 * @brief B737飞机气动数据定义
 * @details 包含B737飞机的升力系数、阻力系数、力矩系数等气动特性数据
 * @author VFT_SMF Framework
 * @date 2024
 */

#ifndef B737_AERODYNAMIC_DATA_HPP
#define B737_AERODYNAMIC_DATA_HPP

#include <string>
#include <vector>
#include <map>
#include <array>

namespace SMF {
namespace AircraftDigitalTwin {
namespace B737 {

/**
 * @brief 气动系数数据点结构体
 * @details 定义特定条件下的气动系数
 */
struct AerodynamicCoefficientPoint {
    double angle_of_attack;             ///< 迎角 (度)
    double mach_number;                 ///< 马赫数
    double reynolds_number;             ///< 雷诺数
    double flap_deflection;             ///< 襟翼偏角 (度)
    double gear_position;               ///< 起落架位置 (0=收起, 1=放下)
    double spoiler_deflection;          ///< 扰流板偏角 (度)
    
    double cl;                          ///< 升力系数
    double cd;                          ///< 阻力系数
    double cm;                          ///< 俯仰力矩系数
    double cl_alpha;                    ///< 升力线斜率
    double cd0;                         ///< 零升阻力系数
    double oswald_efficiency;           ///< 奥斯瓦尔德效率因子
    
    AerodynamicCoefficientPoint() : angle_of_attack(0.0), mach_number(0.0), 
                                   reynolds_number(0.0), flap_deflection(0.0),
                                   gear_position(0.0), spoiler_deflection(0.0),
                                   cl(0.0), cd(0.0), cm(0.0), cl_alpha(0.0),
                                   cd0(0.0), oswald_efficiency(0.0) {}
};

/**
 * @brief 气动导数结构体
 * @details 定义气动导数，用于稳定性分析
 */
struct AerodynamicDerivatives {
    // 升力导数
    double cl_alpha;                    ///< 升力对迎角的导数
    double cl_q;                        ///< 升力对俯仰角速度的导数
    double cl_delta_e;                  ///< 升力对升降舵的导数
    double cl_delta_f;                  ///< 升力对襟翼的导数
    
    // 阻力导数
    double cd_alpha;                    ///< 阻力对迎角的导数
    double cd_mach;                     ///< 阻力对马赫数的导数
    double cd_delta_f;                  ///< 阻力对襟翼的导数
    double cd_gear;                     ///< 阻力对起落架的导数
    
    // 俯仰力矩导数
    double cm_alpha;                    ///< 俯仰力矩对迎角的导数
    double cm_q;                        ///< 俯仰力矩对俯仰角速度的导数
    double cm_delta_e;                  ///< 俯仰力矩对升降舵的导数
    double cm_delta_f;                  ///< 俯仰力矩对襟翼的导数
    double cm_delta_t;                  ///< 俯仰力矩对推力的导数
    
    // 侧力导数
    double cy_beta;                     ///< 侧力对侧滑角的导数
    double cy_delta_r;                  ///< 侧力对方向舵的导数
    double cy_delta_a;                  ///< 侧力对副翼的导数
    
    // 滚转力矩导数
    double cl_beta;                     ///< 滚转力矩对侧滑角的导数
    double cl_p;                        ///< 滚转力矩对滚转角速度的导数
    double cl_r;                        ///< 滚转力矩对偏航角速度的导数
    double cl_delta_a;                  ///< 滚转力矩对副翼的导数
    double cl_delta_r;                  ///< 滚转力矩对方向舵的导数
    
    // 偏航力矩导数
    double cn_beta;                     ///< 偏航力矩对侧滑角的导数
    double cn_p;                        ///< 偏航力矩对滚转角速度的导数
    double cn_r;                        ///< 偏航力矩对偏航角速度的导数
    double cn_delta_a;                  ///< 偏航力矩对副翼的导数
    double cn_delta_r;                  ///< 偏航力矩对方向舵的导数
    
    AerodynamicDerivatives() : cl_alpha(0.0), cl_q(0.0), cl_delta_e(0.0), cl_delta_f(0.0),
                              cd_alpha(0.0), cd_mach(0.0), cd_delta_f(0.0), cd_gear(0.0),
                              cm_alpha(0.0), cm_q(0.0), cm_delta_e(0.0), cm_delta_f(0.0), cm_delta_t(0.0),
                              cy_beta(0.0), cy_delta_r(0.0), cy_delta_a(0.0),
                              cl_beta(0.0), cl_p(0.0), cl_r(0.0), cl_delta_a(0.0), cl_delta_r(0.0),
                              cn_beta(0.0), cn_p(0.0), cn_r(0.0), cn_delta_a(0.0), cn_delta_r(0.0) {}
};

/**
 * @brief 气动特性曲线结构体
 * @details 定义不同构型下的气动特性曲线
 */
struct AerodynamicCurve {
    std::string configuration_name;     ///< 构型名称
    double flap_deflection;             ///< 襟翼偏角 (度)
    double gear_position;               ///< 起落架位置
    double spoiler_deflection;          ///< 扰流板偏角 (度)
    
    std::vector<AerodynamicCoefficientPoint> data_points; ///< 气动系数数据点
    AerodynamicDerivatives derivatives; ///< 气动导数
    
    AerodynamicCurve() : configuration_name(""), flap_deflection(0.0), 
                        gear_position(0.0), spoiler_deflection(0.0) {}
};

/**
 * @brief B737飞机气动数据结构体
 * @details 包含B737飞机的完整气动特性数据
 */
struct B737AerodynamicData {
    // ==================== 基本信息 ====================
    std::string aircraft_type;          ///< 飞机型号
    std::string data_source;            ///< 数据来源
    std::string data_version;           ///< 数据版本
    
    // ==================== 参考参数 ====================
    double reference_wing_area;         ///< 参考机翼面积 (m²)
    double reference_chord;             ///< 参考弦长 (m)
    double reference_span;              ///< 参考翼展 (m)
    double reference_cg_position;       ///< 参考重心位置 (%MAC)
    
    // ==================== 气动特性曲线 ====================
    std::map<std::string, AerodynamicCurve> aerodynamic_curves; ///< 不同构型的气动曲线
    
    // ==================== 失速特性 ====================
    double stall_angle_clean;           ///< 净构型失速迎角 (度)
    double stall_angle_landing;         ///< 着陆构型失速迎角 (度)
    double stall_angle_takeoff;         ///< 起飞构型失速迎角 (度)
    
    // ==================== 马赫数效应 ====================
    double critical_mach_number;        ///< 临界马赫数
    double drag_divergence_mach;        ///< 阻力发散马赫数
    double buffet_onset_mach;           ///< 抖振起始马赫数
    
    // ==================== 地面效应 ====================
    double ground_effect_height;        ///< 地面效应高度 (m)
    double ground_effect_factor;        ///< 地面效应因子
    
    // ==================== 干扰效应 ====================
    double interference_factor;         ///< 干扰因子
    double downwash_angle;              ///< 下洗角 (度)
    double sidewash_angle;              ///< 侧洗角 (度)
    
    // ==================== 构造方法 ====================
    B737AerodynamicData();
    
    // ==================== 数据访问方法 ====================
    AerodynamicCoefficientPoint interpolate_coefficients(double alpha, double mach, 
                                                        double reynolds, double flap_deflection,
                                                        double gear_position, double spoiler_deflection) const;
    
    AerodynamicDerivatives get_derivatives(const std::string& configuration) const;
    
    // ==================== 气动系数计算方法 ====================
    double calculate_lift_coefficient(double alpha, double mach, double reynolds,
                                    double flap_deflection, double gear_position, 
                                    double spoiler_deflection) const;
    
    double calculate_drag_coefficient(double alpha, double mach, double reynolds,
                                    double flap_deflection, double gear_position, 
                                    double spoiler_deflection) const;
    
    double calculate_pitch_moment_coefficient(double alpha, double mach, double reynolds,
                                            double flap_deflection, double gear_position, 
                                            double spoiler_deflection) const;
    
    // ==================== 数据验证方法 ====================
    bool validate_data() const;
    std::string get_validation_report() const;
    
    // ==================== 数据加载方法 ====================
    bool load_from_file(const std::string& filename);
    bool save_to_file(const std::string& filename) const;
};

/**
 * @brief B737-800气动数据
 * @details B737-800型号的详细气动参数
 */
extern const B737AerodynamicData B737_800_AERODYNAMIC_DATA;

/**
 * @brief B737-700气动数据
 * @details B737-700型号的详细气动参数
 */
extern const B737AerodynamicData B737_700_AERODYNAMIC_DATA;

/**
 * @brief B737-900气动数据
 * @details B737-900型号的详细气动参数
 */
extern const B737AerodynamicData B737_900_AERODYNAMIC_DATA;

} // namespace B737
} // namespace AircraftDigitalTwin
} // namespace SMF

#endif // B737_AERODYNAMIC_DATA_HPP 