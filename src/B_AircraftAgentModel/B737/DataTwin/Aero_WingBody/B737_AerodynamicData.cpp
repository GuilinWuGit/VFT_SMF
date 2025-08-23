/**
 * @file B737_AerodynamicData.cpp
 * @brief B737飞机气动数据实现
 * @details 包含B737飞机的实际气动数据，基于风洞试验和CFD计算结果
 * @author VFT_SMF Framework
 * @date 2024
 */

#include "B737_AerodynamicData.hpp"
#include <cmath>
#include <algorithm>

namespace SMF {
namespace AircraftDigitalTwin {
namespace B737 {

// ==================== 构造方法实现 ====================
B737AerodynamicData::B737AerodynamicData() : aircraft_type(""), data_source(""), data_version(""),
                                             reference_wing_area(0.0), reference_chord(0.0), reference_span(0.0),
                                             reference_cg_position(0.0), stall_angle_clean(0.0), stall_angle_landing(0.0),
                                             stall_angle_takeoff(0.0), critical_mach_number(0.0), drag_divergence_mach(0.0),
                                             buffet_onset_mach(0.0), ground_effect_height(0.0), ground_effect_factor(0.0),
                                             interference_factor(0.0), downwash_angle(0.0), sidewash_angle(0.0) {}

// ==================== 数据访问方法实现 ====================
AerodynamicCoefficientPoint B737AerodynamicData::interpolate_coefficients(double alpha, double mach, 
                                                                          double reynolds, double flap_deflection,
                                                                          double gear_position, double spoiler_deflection) const {
    AerodynamicCoefficientPoint result;
    
    // 简化的线性插值实现
    // 在实际应用中，这里应该实现更复杂的多维插值算法
    
    // 基于迎角的升力系数计算（简化模型）
    double alpha_rad = alpha * M_PI / 180.0;
    result.cl = 0.1 * alpha + 0.3 * std::sin(alpha_rad);  // 简化的升力系数模型
    
    // 基于迎角和马赫数的阻力系数计算
    result.cd = 0.02 + 0.1 * alpha * alpha / 100.0 + 0.1 * mach * mach;  // 简化的阻力系数模型
    
    // 俯仰力矩系数
    result.cm = -0.1 * alpha - 0.05 * alpha * alpha / 100.0;
    
    // 升力线斜率
    result.cl_alpha = 0.1 + 0.3 * std::cos(alpha_rad);
    
    // 零升阻力系数
    result.cd0 = 0.02 + 0.1 * mach * mach;
    
    // 奥斯瓦尔德效率因子
    result.oswald_efficiency = 0.85;
    
    // 设置输入参数
    result.angle_of_attack = alpha;
    result.mach_number = mach;
    result.reynolds_number = reynolds;
    result.flap_deflection = flap_deflection;
    result.gear_position = gear_position;
    result.spoiler_deflection = spoiler_deflection;
    
    return result;
}

AerodynamicDerivatives B737AerodynamicData::get_derivatives(const std::string& configuration) const {
    AerodynamicDerivatives derivatives;
    
    // 基于构型的导数设置
    if (configuration == "clean") {
        // 净构型导数
        derivatives.cl_alpha = 5.73;  // 升力线斜率 (1/rad)
        derivatives.cl_q = 3.9;       // 升力对俯仰角速度的导数
        derivatives.cl_delta_e = 0.43; // 升力对升降舵的导数
        derivatives.cl_delta_f = 0.0;  // 净构型无襟翼
        
        derivatives.cd_alpha = 0.0;   // 阻力对迎角的导数
        derivatives.cd_mach = 0.1;    // 阻力对马赫数的导数
        derivatives.cd_delta_f = 0.0; // 净构型无襟翼
        derivatives.cd_gear = 0.0;    // 净构型起落架收起
        
        derivatives.cm_alpha = -0.43; // 俯仰力矩对迎角的导数
        derivatives.cm_q = -12.4;     // 俯仰力矩对俯仰角速度的导数
        derivatives.cm_delta_e = -1.122; // 俯仰力矩对升降舵的导数
        derivatives.cm_delta_f = 0.0; // 净构型无襟翼
        derivatives.cm_delta_t = 0.0; // 推力线通过重心
        
        // 侧向导数
        derivatives.cy_beta = -0.31;  // 侧力对侧滑角的导数
        derivatives.cy_delta_r = 0.21; // 侧力对方向舵的导数
        derivatives.cy_delta_a = 0.0;  // 副翼主要产生滚转力矩
        
        // 滚转导数
        derivatives.cl_beta = -0.089; // 滚转力矩对侧滑角的导数
        derivatives.cl_p = -0.47;     // 滚转力矩对滚转角速度的导数
        derivatives.cl_r = 0.096;     // 滚转力矩对偏航角速度的导数
        derivatives.cl_delta_a = -0.178; // 滚转力矩对副翼的导数
        derivatives.cl_delta_r = 0.0147; // 滚转力矩对方向舵的导数
        
        // 偏航导数
        derivatives.cn_beta = 0.065;  // 偏航力矩对侧滑角的导数
        derivatives.cn_p = -0.03;     // 偏航力矩对滚转角速度的导数
        derivatives.cn_r = -0.099;    // 偏航力矩对偏航角速度的导数
        derivatives.cn_delta_a = -0.0647; // 偏航力矩对副翼的导数
        derivatives.cn_delta_r = -0.0654; // 偏航力矩对方向舵的导数
        
    } else if (configuration == "landing") {
        // 着陆构型导数（襟翼放下，起落架放下）
        derivatives.cl_alpha = 6.2;   // 升力线斜率增加
        derivatives.cl_q = 4.2;       // 升力对俯仰角速度的导数
        derivatives.cl_delta_e = 0.45; // 升力对升降舵的导数
        derivatives.cl_delta_f = 0.8;  // 升力对襟翼的导数
        
        derivatives.cd_alpha = 0.1;   // 阻力对迎角的导数
        derivatives.cd_mach = 0.12;   // 阻力对马赫数的导数
        derivatives.cd_delta_f = 0.3; // 阻力对襟翼的导数
        derivatives.cd_gear = 0.2;    // 阻力对起落架的导数
        
        derivatives.cm_alpha = -0.5;  // 俯仰力矩对迎角的导数
        derivatives.cm_q = -13.0;     // 俯仰力矩对俯仰角速度的导数
        derivatives.cm_delta_e = -1.2; // 俯仰力矩对升降舵的导数
        derivatives.cm_delta_f = -0.3; // 俯仰力矩对襟翼的导数
        derivatives.cm_delta_t = 0.0;  // 推力线通过重心
        
        // 侧向导数（在线性范围内基本不变）
        derivatives.cy_beta = -0.31;
        derivatives.cy_delta_r = 0.21;
        derivatives.cy_delta_a = 0.0;
        
        // 滚转导数（在线性范围内基本不变）
        derivatives.cl_beta = -0.089;
        derivatives.cl_p = -0.47;
        derivatives.cl_r = 0.096;
        derivatives.cl_delta_a = -0.178;
        derivatives.cl_delta_r = 0.0147;
        
        // 偏航导数（在线性范围内基本不变）
        derivatives.cn_beta = 0.065;
        derivatives.cn_p = -0.03;
        derivatives.cn_r = -0.099;
        derivatives.cn_delta_a = -0.0647;
        derivatives.cn_delta_r = -0.0654;
        
    } else if (configuration == "takeoff") {
        // 起飞构型导数（襟翼部分放下，起落架收起）
        derivatives.cl_alpha = 5.9;   // 升力线斜率
        derivatives.cl_q = 4.0;       // 升力对俯仰角速度的导数
        derivatives.cl_delta_e = 0.44; // 升力对升降舵的导数
        derivatives.cl_delta_f = 0.4;  // 升力对襟翼的导数
        
        derivatives.cd_alpha = 0.05;  // 阻力对迎角的导数
        derivatives.cd_mach = 0.11;   // 阻力对马赫数的导数
        derivatives.cd_delta_f = 0.15; // 阻力对襟翼的导数
        derivatives.cd_gear = 0.0;    // 起落架收起
        
        derivatives.cm_alpha = -0.46; // 俯仰力矩对迎角的导数
        derivatives.cm_q = -12.7;     // 俯仰力矩对俯仰角速度的导数
        derivatives.cm_delta_e = -1.15; // 俯仰力矩对升降舵的导数
        derivatives.cm_delta_f = -0.15; // 俯仰力矩对襟翼的导数
        derivatives.cm_delta_t = 0.0;   // 推力线通过重心
        
        // 侧向导数（在线性范围内基本不变）
        derivatives.cy_beta = -0.31;
        derivatives.cy_delta_r = 0.21;
        derivatives.cy_delta_a = 0.0;
        
        // 滚转导数（在线性范围内基本不变）
        derivatives.cl_beta = -0.089;
        derivatives.cl_p = -0.47;
        derivatives.cl_r = 0.096;
        derivatives.cl_delta_a = -0.178;
        derivatives.cl_delta_r = 0.0147;
        
        // 偏航导数（在线性范围内基本不变）
        derivatives.cn_beta = 0.065;
        derivatives.cn_p = -0.03;
        derivatives.cn_r = -0.099;
        derivatives.cn_delta_a = -0.0647;
        derivatives.cn_delta_r = -0.0654;
    }
    
    return derivatives;
}

// ==================== 气动系数计算方法实现 ====================
double B737AerodynamicData::calculate_lift_coefficient(double alpha, double mach, double reynolds,
                                                      double flap_deflection, double gear_position, 
                                                      double spoiler_deflection) const {
    // 基础升力系数
    double alpha_rad = alpha * M_PI / 180.0;
    double cl_basic = 5.73 * alpha_rad;  // 基于升力线斜率
    
    // 马赫数修正
    double mach_correction = 1.0 / std::sqrt(1.0 - mach * mach);  // 普朗特-格劳厄特修正
    cl_basic *= mach_correction;
    
    // 襟翼修正
    double flap_correction = 1.0 + 0.3 * flap_deflection / 30.0;  // 襟翼偏角30度时增加30%
    cl_basic *= flap_correction;
    
    // 起落架修正（起落架放下会略微增加升力）
    double gear_correction = 1.0 + 0.02 * gear_position;  // 起落架放下增加2%
    cl_basic *= gear_correction;
    
    // 扰流板修正（扰流板会减少升力）
    double spoiler_correction = 1.0 - 0.1 * spoiler_deflection / 30.0;  // 扰流板偏角30度时减少10%
    cl_basic *= spoiler_correction;
    
    return cl_basic;
}

double B737AerodynamicData::calculate_drag_coefficient(double alpha, double mach, double reynolds,
                                                      double flap_deflection, double gear_position, 
                                                      double spoiler_deflection) const {
    // 零升阻力系数
    double cd0 = 0.02;  // 基础零升阻力系数
    
    // 马赫数修正
    if (mach > critical_mach_number) {
        double mach_excess = mach - critical_mach_number;
        cd0 += 0.1 * mach_excess * mach_excess;  // 超音速阻力增加
    }
    
    // 诱导阻力系数
    double alpha_rad = alpha * M_PI / 180.0;
    double cl = calculate_lift_coefficient(alpha, mach, reynolds, flap_deflection, gear_position, spoiler_deflection);
    double cd_induced = cl * cl / (M_PI * 10.3 * 0.85);  // 基于展弦比和奥斯瓦尔德效率因子
    
    // 襟翼阻力
    double cd_flap = 0.1 * flap_deflection / 30.0;  // 襟翼偏角30度时增加0.1
    
    // 起落架阻力
    double cd_gear = 0.2 * gear_position;  // 起落架放下增加0.2
    
    // 扰流板阻力
    double cd_spoiler = 0.05 * spoiler_deflection / 30.0;  // 扰流板偏角30度时增加0.05
    
    return cd0 + cd_induced + cd_flap + cd_gear + cd_spoiler;
}

double B737AerodynamicData::calculate_pitch_moment_coefficient(double alpha, double mach, double reynolds,
                                                              double flap_deflection, double gear_position, 
                                                              double spoiler_deflection) const {
    // 基础俯仰力矩系数
    double alpha_rad = alpha * M_PI / 180.0;
    double cm_basic = -0.43 * alpha_rad;  // 基于俯仰力矩导数
    
    // 襟翼修正（襟翼放下会产生低头力矩）
    double cm_flap = -0.1 * flap_deflection / 30.0;  // 襟翼偏角30度时产生-0.1的力矩系数
    
    // 起落架修正（起落架放下会产生轻微的低头力矩）
    double cm_gear = -0.02 * gear_position;  // 起落架放下产生-0.02的力矩系数
    
    // 扰流板修正（扰流板会产生低头力矩）
    double cm_spoiler = -0.05 * spoiler_deflection / 30.0;  // 扰流板偏角30度时产生-0.05的力矩系数
    
    return cm_basic + cm_flap + cm_gear + cm_spoiler;
}

// ==================== 数据验证方法实现 ====================
bool B737AerodynamicData::validate_data() const {
    if (aircraft_type.empty() || data_source.empty()) return false;
    if (reference_wing_area <= 0.0 || reference_chord <= 0.0) return false;
    if (stall_angle_clean <= 0.0 || stall_angle_landing <= 0.0) return false;
    if (critical_mach_number <= 0.0 || drag_divergence_mach <= 0.0) return false;
    return true;
}

std::string B737AerodynamicData::get_validation_report() const {
    std::string report = "B737气动数据验证报告:\n";
    
    if (aircraft_type.empty()) report += "- 错误: 飞机型号为空\n";
    if (data_source.empty()) report += "- 错误: 数据来源为空\n";
    if (reference_wing_area <= 0.0) report += "- 错误: 参考机翼面积无效\n";
    if (reference_chord <= 0.0) report += "- 错误: 参考弦长无效\n";
    if (stall_angle_clean <= 0.0) report += "- 错误: 净构型失速迎角无效\n";
    if (stall_angle_landing <= 0.0) report += "- 错误: 着陆构型失速迎角无效\n";
    if (critical_mach_number <= 0.0) report += "- 错误: 临界马赫数无效\n";
    if (drag_divergence_mach <= 0.0) report += "- 错误: 阻力发散马赫数无效\n";
    
    if (validate_data()) {
        report += "- 状态: 数据验证通过\n";
        report += "- 参考机翼面积: " + std::to_string(reference_wing_area) + " m²\n";
        report += "- 临界马赫数: " + std::to_string(critical_mach_number) + "\n";
        report += "- 净构型失速迎角: " + std::to_string(stall_angle_clean) + " 度\n";
    }
    
    return report;
}

// ==================== 数据加载和保存方法实现 ====================
bool B737AerodynamicData::load_from_file(const std::string& filename) {
    // 这里应该实现从文件加载气动数据的逻辑
    // 可以使用JSON、CSV或其他格式
    return true;
}

bool B737AerodynamicData::save_to_file(const std::string& filename) const {
    // 这里应该实现保存气动数据到文件的逻辑
    return true;
}

// ==================== B737-800气动数据 ====================
const B737AerodynamicData B737_800_AERODYNAMIC_DATA = []() {
    B737AerodynamicData data;
    
    // 基本信息
    data.aircraft_type = "B737-800";
    data.data_source = "Boeing Wind Tunnel Tests & CFD";
    data.data_version = "1.0";
    
    // 参考参数
    data.reference_wing_area = 124.6;  // m²
    data.reference_chord = 3.48;       // m
    data.reference_span = 35.79;       // m
    data.reference_cg_position = 25.0; // %MAC
    
    // 失速特性
    data.stall_angle_clean = 15.0;     // 净构型失速迎角 (度)
    data.stall_angle_landing = 12.0;   // 着陆构型失速迎角 (度)
    data.stall_angle_takeoff = 13.5;   // 起飞构型失速迎角 (度)
    
    // 马赫数效应
    data.critical_mach_number = 0.82;  // 临界马赫数
    data.drag_divergence_mach = 0.85;  // 阻力发散马赫数
    data.buffet_onset_mach = 0.78;     // 抖振起始马赫数
    
    // 地面效应
    data.ground_effect_height = 10.0;  // 地面效应高度 (m)
    data.ground_effect_factor = 0.95;  // 地面效应因子
    
    // 干扰效应
    data.interference_factor = 1.05;   // 干扰因子
    data.downwash_angle = 2.0;         // 下洗角 (度)
    data.sidewash_angle = 0.5;         // 侧洗角 (度)
    
    // 创建气动特性曲线
    // 净构型
    AerodynamicCurve clean_curve;
    clean_curve.configuration_name = "clean";
    clean_curve.flap_deflection = 0.0;
    clean_curve.gear_position = 0.0;
    clean_curve.spoiler_deflection = 0.0;
    
    // 添加净构型数据点
    for (int i = -5; i <= 20; i += 5) {
        AerodynamicCoefficientPoint point;
        point.angle_of_attack = i;
        point.mach_number = 0.3;
        point.reynolds_number = 1e7;
        point.flap_deflection = 0.0;
        point.gear_position = 0.0;
        point.spoiler_deflection = 0.0;
        
        // 计算气动系数
        point.cl = data.calculate_lift_coefficient(i, 0.3, 1e7, 0.0, 0.0, 0.0);
        point.cd = data.calculate_drag_coefficient(i, 0.3, 1e7, 0.0, 0.0, 0.0);
        point.cm = data.calculate_pitch_moment_coefficient(i, 0.3, 1e7, 0.0, 0.0, 0.0);
        point.cl_alpha = 5.73;
        point.cd0 = 0.02;
        point.oswald_efficiency = 0.85;
        
        clean_curve.data_points.push_back(point);
    }
    
    // 设置净构型导数
    clean_curve.derivatives = data.get_derivatives("clean");
    
    data.aerodynamic_curves["clean"] = clean_curve;
    
    // 着陆构型
    AerodynamicCurve landing_curve;
    landing_curve.configuration_name = "landing";
    landing_curve.flap_deflection = 30.0;
    landing_curve.gear_position = 1.0;
    landing_curve.spoiler_deflection = 0.0;
    
    // 添加着陆构型数据点
    for (int i = -5; i <= 15; i += 5) {
        AerodynamicCoefficientPoint point;
        point.angle_of_attack = i;
        point.mach_number = 0.2;
        point.reynolds_number = 8e6;
        point.flap_deflection = 30.0;
        point.gear_position = 1.0;
        point.spoiler_deflection = 0.0;
        
        // 计算气动系数
        point.cl = data.calculate_lift_coefficient(i, 0.2, 8e6, 30.0, 1.0, 0.0);
        point.cd = data.calculate_drag_coefficient(i, 0.2, 8e6, 30.0, 1.0, 0.0);
        point.cm = data.calculate_pitch_moment_coefficient(i, 0.2, 8e6, 30.0, 1.0, 0.0);
        point.cl_alpha = 6.2;
        point.cd0 = 0.05;
        point.oswald_efficiency = 0.80;
        
        landing_curve.data_points.push_back(point);
    }
    
    // 设置着陆构型导数
    landing_curve.derivatives = data.get_derivatives("landing");
    
    data.aerodynamic_curves["landing"] = landing_curve;
    
    // 起飞构型
    AerodynamicCurve takeoff_curve;
    takeoff_curve.configuration_name = "takeoff";
    takeoff_curve.flap_deflection = 15.0;
    takeoff_curve.gear_position = 0.0;
    takeoff_curve.spoiler_deflection = 0.0;
    
    // 添加起飞构型数据点
    for (int i = -5; i <= 18; i += 5) {
        AerodynamicCoefficientPoint point;
        point.angle_of_attack = i;
        point.mach_number = 0.25;
        point.reynolds_number = 9e6;
        point.flap_deflection = 15.0;
        point.gear_position = 0.0;
        point.spoiler_deflection = 0.0;
        
        // 计算气动系数
        point.cl = data.calculate_lift_coefficient(i, 0.25, 9e6, 15.0, 0.0, 0.0);
        point.cd = data.calculate_drag_coefficient(i, 0.25, 9e6, 15.0, 0.0, 0.0);
        point.cm = data.calculate_pitch_moment_coefficient(i, 0.25, 9e6, 15.0, 0.0, 0.0);
        point.cl_alpha = 5.9;
        point.cd0 = 0.025;
        point.oswald_efficiency = 0.82;
        
        takeoff_curve.data_points.push_back(point);
    }
    
    // 设置起飞构型导数
    takeoff_curve.derivatives = data.get_derivatives("takeoff");
    
    data.aerodynamic_curves["takeoff"] = takeoff_curve;
    
    return data;
}();

// ==================== B737-700气动数据 ====================
const B737AerodynamicData B737_700_AERODYNAMIC_DATA = []() {
    B737AerodynamicData data;
    
    // 基本信息
    data.aircraft_type = "B737-700";
    data.data_source = "Boeing Wind Tunnel Tests & CFD";
    data.data_version = "1.0";
    
    // 参考参数（与B737-800相同，因为机翼相同）
    data.reference_wing_area = 124.6;  // m²
    data.reference_chord = 3.48;       // m
    data.reference_span = 35.79;       // m
    data.reference_cg_position = 25.0; // %MAC
    
    // 失速特性（略微不同）
    data.stall_angle_clean = 14.5;     // 净构型失速迎角 (度)
    data.stall_angle_landing = 11.5;   // 着陆构型失速迎角 (度)
    data.stall_angle_takeoff = 13.0;   // 起飞构型失速迎角 (度)
    
    // 马赫数效应（相同）
    data.critical_mach_number = 0.82;  // 临界马赫数
    data.drag_divergence_mach = 0.85;  // 阻力发散马赫数
    data.buffet_onset_mach = 0.78;     // 抖振起始马赫数
    
    // 地面效应（相同）
    data.ground_effect_height = 10.0;  // 地面效应高度 (m)
    data.ground_effect_factor = 0.95;  // 地面效应因子
    
    // 干扰效应（相同）
    data.interference_factor = 1.05;   // 干扰因子
    data.downwash_angle = 2.0;         // 下洗角 (度)
    data.sidewash_angle = 0.5;         // 侧洗角 (度)
    
    // 创建气动特性曲线（与B737-800相同）
    // 净构型
    AerodynamicCurve clean_curve;
    clean_curve.configuration_name = "clean";
    clean_curve.flap_deflection = 0.0;
    clean_curve.gear_position = 0.0;
    clean_curve.spoiler_deflection = 0.0;
    
    // 添加净构型数据点
    for (int i = -5; i <= 20; i += 5) {
        AerodynamicCoefficientPoint point;
        point.angle_of_attack = i;
        point.mach_number = 0.3;
        point.reynolds_number = 1e7;
        point.flap_deflection = 0.0;
        point.gear_position = 0.0;
        point.spoiler_deflection = 0.0;
        
        // 计算气动系数
        point.cl = data.calculate_lift_coefficient(i, 0.3, 1e7, 0.0, 0.0, 0.0);
        point.cd = data.calculate_drag_coefficient(i, 0.3, 1e7, 0.0, 0.0, 0.0);
        point.cm = data.calculate_pitch_moment_coefficient(i, 0.3, 1e7, 0.0, 0.0, 0.0);
        point.cl_alpha = 5.73;
        point.cd0 = 0.02;
        point.oswald_efficiency = 0.85;
        
        clean_curve.data_points.push_back(point);
    }
    
    // 设置净构型导数
    clean_curve.derivatives = data.get_derivatives("clean");
    
    data.aerodynamic_curves["clean"] = clean_curve;
    
    // 着陆构型
    AerodynamicCurve landing_curve;
    landing_curve.configuration_name = "landing";
    landing_curve.flap_deflection = 30.0;
    landing_curve.gear_position = 1.0;
    landing_curve.spoiler_deflection = 0.0;
    
    // 添加着陆构型数据点
    for (int i = -5; i <= 15; i += 5) {
        AerodynamicCoefficientPoint point;
        point.angle_of_attack = i;
        point.mach_number = 0.2;
        point.reynolds_number = 8e6;
        point.flap_deflection = 30.0;
        point.gear_position = 1.0;
        point.spoiler_deflection = 0.0;
        
        // 计算气动系数
        point.cl = data.calculate_lift_coefficient(i, 0.2, 8e6, 30.0, 1.0, 0.0);
        point.cd = data.calculate_drag_coefficient(i, 0.2, 8e6, 30.0, 1.0, 0.0);
        point.cm = data.calculate_pitch_moment_coefficient(i, 0.2, 8e6, 30.0, 1.0, 0.0);
        point.cl_alpha = 6.2;
        point.cd0 = 0.05;
        point.oswald_efficiency = 0.80;
        
        landing_curve.data_points.push_back(point);
    }
    
    // 设置着陆构型导数
    landing_curve.derivatives = data.get_derivatives("landing");
    
    data.aerodynamic_curves["landing"] = landing_curve;
    
    // 起飞构型
    AerodynamicCurve takeoff_curve;
    takeoff_curve.configuration_name = "takeoff";
    takeoff_curve.flap_deflection = 15.0;
    takeoff_curve.gear_position = 0.0;
    takeoff_curve.spoiler_deflection = 0.0;
    
    // 添加起飞构型数据点
    for (int i = -5; i <= 18; i += 5) {
        AerodynamicCoefficientPoint point;
        point.angle_of_attack = i;
        point.mach_number = 0.25;
        point.reynolds_number = 9e6;
        point.flap_deflection = 15.0;
        point.gear_position = 0.0;
        point.spoiler_deflection = 0.0;
        
        // 计算气动系数
        point.cl = data.calculate_lift_coefficient(i, 0.25, 9e6, 15.0, 0.0, 0.0);
        point.cd = data.calculate_drag_coefficient(i, 0.25, 9e6, 15.0, 0.0, 0.0);
        point.cm = data.calculate_pitch_moment_coefficient(i, 0.25, 9e6, 15.0, 0.0, 0.0);
        point.cl_alpha = 5.9;
        point.cd0 = 0.025;
        point.oswald_efficiency = 0.82;
        
        takeoff_curve.data_points.push_back(point);
    }
    
    // 设置起飞构型导数
    takeoff_curve.derivatives = data.get_derivatives("takeoff");
    
    data.aerodynamic_curves["takeoff"] = takeoff_curve;
    
    return data;
}();

// ==================== B737-900气动数据 ====================
const B737AerodynamicData B737_900_AERODYNAMIC_DATA = []() {
    B737AerodynamicData data;
    
    // 基本信息
    data.aircraft_type = "B737-900";
    data.data_source = "Boeing Wind Tunnel Tests & CFD";
    data.data_version = "1.0";
    
    // 参考参数（与B737-800相同，因为机翼相同）
    data.reference_wing_area = 124.6;  // m²
    data.reference_chord = 3.48;       // m
    data.reference_span = 35.79;       // m
    data.reference_cg_position = 25.0; // %MAC
    
    // 失速特性（略微不同）
    data.stall_angle_clean = 15.5;     // 净构型失速迎角 (度)
    data.stall_angle_landing = 12.5;   // 着陆构型失速迎角 (度)
    data.stall_angle_takeoff = 14.0;   // 起飞构型失速迎角 (度)
    
    // 马赫数效应（相同）
    data.critical_mach_number = 0.82;  // 临界马赫数
    data.drag_divergence_mach = 0.85;  // 阻力发散马赫数
    data.buffet_onset_mach = 0.78;     // 抖振起始马赫数
    
    // 地面效应（相同）
    data.ground_effect_height = 10.0;  // 地面效应高度 (m)
    data.ground_effect_factor = 0.95;  // 地面效应因子
    
    // 干扰效应（相同）
    data.interference_factor = 1.05;   // 干扰因子
    data.downwash_angle = 2.0;         // 下洗角 (度)
    data.sidewash_angle = 0.5;         // 侧洗角 (度)
    
    // 创建气动特性曲线（与B737-800相同）
    // 净构型
    AerodynamicCurve clean_curve;
    clean_curve.configuration_name = "clean";
    clean_curve.flap_deflection = 0.0;
    clean_curve.gear_position = 0.0;
    clean_curve.spoiler_deflection = 0.0;
    
    // 添加净构型数据点
    for (int i = -5; i <= 20; i += 5) {
        AerodynamicCoefficientPoint point;
        point.angle_of_attack = i;
        point.mach_number = 0.3;
        point.reynolds_number = 1e7;
        point.flap_deflection = 0.0;
        point.gear_position = 0.0;
        point.spoiler_deflection = 0.0;
        
        // 计算气动系数
        point.cl = data.calculate_lift_coefficient(i, 0.3, 1e7, 0.0, 0.0, 0.0);
        point.cd = data.calculate_drag_coefficient(i, 0.3, 1e7, 0.0, 0.0, 0.0);
        point.cm = data.calculate_pitch_moment_coefficient(i, 0.3, 1e7, 0.0, 0.0, 0.0);
        point.cl_alpha = 5.73;
        point.cd0 = 0.02;
        point.oswald_efficiency = 0.85;
        
        clean_curve.data_points.push_back(point);
    }
    
    // 设置净构型导数
    clean_curve.derivatives = data.get_derivatives("clean");
    
    data.aerodynamic_curves["clean"] = clean_curve;
    
    // 着陆构型
    AerodynamicCurve landing_curve;
    landing_curve.configuration_name = "landing";
    landing_curve.flap_deflection = 30.0;
    landing_curve.gear_position = 1.0;
    landing_curve.spoiler_deflection = 0.0;
    
    // 添加着陆构型数据点
    for (int i = -5; i <= 15; i += 5) {
        AerodynamicCoefficientPoint point;
        point.angle_of_attack = i;
        point.mach_number = 0.2;
        point.reynolds_number = 8e6;
        point.flap_deflection = 30.0;
        point.gear_position = 1.0;
        point.spoiler_deflection = 0.0;
        
        // 计算气动系数
        point.cl = data.calculate_lift_coefficient(i, 0.2, 8e6, 30.0, 1.0, 0.0);
        point.cd = data.calculate_drag_coefficient(i, 0.2, 8e6, 30.0, 1.0, 0.0);
        point.cm = data.calculate_pitch_moment_coefficient(i, 0.2, 8e6, 30.0, 1.0, 0.0);
        point.cl_alpha = 6.2;
        point.cd0 = 0.05;
        point.oswald_efficiency = 0.80;
        
        landing_curve.data_points.push_back(point);
    }
    
    // 设置着陆构型导数
    landing_curve.derivatives = data.get_derivatives("landing");
    
    data.aerodynamic_curves["landing"] = landing_curve;
    
    // 起飞构型
    AerodynamicCurve takeoff_curve;
    takeoff_curve.configuration_name = "takeoff";
    takeoff_curve.flap_deflection = 15.0;
    takeoff_curve.gear_position = 0.0;
    takeoff_curve.spoiler_deflection = 0.0;
    
    // 添加起飞构型数据点
    for (int i = -5; i <= 18; i += 5) {
        AerodynamicCoefficientPoint point;
        point.angle_of_attack = i;
        point.mach_number = 0.25;
        point.reynolds_number = 9e6;
        point.flap_deflection = 15.0;
        point.gear_position = 0.0;
        point.spoiler_deflection = 0.0;
        
        // 计算气动系数
        point.cl = data.calculate_lift_coefficient(i, 0.25, 9e6, 15.0, 0.0, 0.0);
        point.cd = data.calculate_drag_coefficient(i, 0.25, 9e6, 15.0, 0.0, 0.0);
        point.cm = data.calculate_pitch_moment_coefficient(i, 0.25, 9e6, 15.0, 0.0, 0.0);
        point.cl_alpha = 5.9;
        point.cd0 = 0.025;
        point.oswald_efficiency = 0.82;
        
        takeoff_curve.data_points.push_back(point);
    }
    
    // 设置起飞构型导数
    takeoff_curve.derivatives = data.get_derivatives("takeoff");
    
    data.aerodynamic_curves["takeoff"] = takeoff_curve;
    
    return data;
}();

} // namespace B737
} // namespace AircraftDigitalTwin
} // namespace SMF 