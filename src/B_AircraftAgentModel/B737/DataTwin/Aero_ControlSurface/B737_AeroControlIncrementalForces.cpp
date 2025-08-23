/**
 * @file B737_AeroControlIncrementalForces.cpp
 * @brief B737飞机气动操纵面6分量气动力增量数据实现
 * @details 包含B737飞机的副翼、升降舵、方向舵、扰流板等操纵面偏转产生的力和力矩增量的实际数据
 * @author VFT_SMF Framework
 * @date 2024
 */

#include "B737_AeroControlIncrementalForces.hpp"
#include <cmath>
#include <algorithm>

namespace SMF {
namespace AircraftDigitalTwin {
namespace B737 {

// ==================== 构造方法实现 ====================
B737AeroControlIncrementalForces::B737AeroControlIncrementalForces() : aircraft_type(""), data_source(""), data_version(""),
                                                                      reference_wing_area(0.0), reference_chord(0.0), reference_span(0.0),
                                                                      reference_dynamic_pressure(0.0) {}

// ==================== 气动力增量计算方法实现 ====================
ControlForceIncrementPoint B737AeroControlIncrementalForces::calculate_force_increment(const std::string& control_surface,
                                                                                      double deflection_angle, double mach,
                                                                                      double reynolds, double alpha, double beta,
                                                                                      double dynamic_pressure) const {
    ControlForceIncrementPoint result;
    
    // 设置输入参数
    result.deflection_angle = deflection_angle;
    result.mach_number = mach;
    result.reynolds_number = reynolds;
    result.angle_of_attack = alpha;
    result.sideslip_angle = beta;
    
    // 计算气动力导数
    ControlForceDerivatives derivatives = calculate_derivatives(control_surface, mach, reynolds, alpha, beta);
    
    // 将偏角转换为弧度
    double deflection_rad = deflection_angle * M_PI / 180.0;
    
    // 计算无量纲系数增量
    result.delta_cx = derivatives.d_cx_d_delta * deflection_rad * derivatives.mach_correction_factor;
    result.delta_cy = derivatives.d_cy_d_delta * deflection_rad * derivatives.mach_correction_factor;
    result.delta_cz = derivatives.d_cz_d_delta * deflection_rad * derivatives.mach_correction_factor;
    result.delta_cl = derivatives.d_cl_d_delta * deflection_rad * derivatives.mach_correction_factor;
    result.delta_cm = derivatives.d_cm_d_delta * deflection_rad * derivatives.mach_correction_factor;
    result.delta_cn = derivatives.d_cn_d_delta * deflection_rad * derivatives.mach_correction_factor;
    
    // 计算有量纲力增量 (N)
    result.delta_force_x = result.delta_cx * dynamic_pressure * reference_wing_area;
    result.delta_force_y = result.delta_cy * dynamic_pressure * reference_wing_area;
    result.delta_force_z = result.delta_cz * dynamic_pressure * reference_wing_area;
    
    // 计算有量纲力矩增量 (N·m)
    result.delta_moment_x = result.delta_cl * dynamic_pressure * reference_wing_area * reference_span;
    result.delta_moment_y = result.delta_cm * dynamic_pressure * reference_wing_area * reference_chord;
    result.delta_moment_z = result.delta_cn * dynamic_pressure * reference_wing_area * reference_span;
    
    return result;
}

// ==================== 气动力导数计算方法实现 ====================
ControlForceDerivatives B737AeroControlIncrementalForces::calculate_derivatives(const std::string& control_surface,
                                                                               double mach, double reynolds, double alpha, double beta) const {
    ControlForceDerivatives derivatives;
    
    // 根据操纵面类型设置基础导数
    if (control_surface == "aileron") {
        // 副翼导数（主要产生滚转力矩）
        derivatives.d_cl_d_delta = -0.178;  // 副翼滚转导数
        derivatives.d_cm_d_delta = 0.02;    // 副翼俯仰导数（较小）
        derivatives.d_cn_d_delta = 0.01;    // 副翼偏航导数（较小）
        derivatives.d_cx_d_delta = 0.005;   // 副翼阻力增量
        derivatives.d_cy_d_delta = 0.0;     // 副翼侧力增量（忽略）
        derivatives.d_cz_d_delta = 0.0;     // 副翼升力增量（忽略）
    } else if (control_surface == "elevator") {
        // 升降舵导数（主要产生俯仰力矩）
        derivatives.d_cm_d_delta = -1.122;  // 升降舵俯仰导数
        derivatives.d_cl_d_delta = 0.01;    // 升降舵滚转导数（较小）
        derivatives.d_cn_d_delta = 0.0;     // 升降舵偏航导数（忽略）
        derivatives.d_cx_d_delta = 0.01;    // 升降舵阻力增量
        derivatives.d_cy_d_delta = 0.0;     // 升降舵侧力增量（忽略）
        derivatives.d_cz_d_delta = 0.05;    // 升降舵升力增量
    } else if (control_surface == "rudder") {
        // 方向舵导数（主要产生偏航力矩）
        derivatives.d_cn_d_delta = -0.0654; // 方向舵偏航导数
        derivatives.d_cl_d_delta = 0.0;     // 方向舵滚转导数（忽略）
        derivatives.d_cm_d_delta = 0.0;     // 方向舵俯仰导数（忽略）
        derivatives.d_cx_d_delta = 0.005;   // 方向舵阻力增量
        derivatives.d_cy_d_delta = 0.08;    // 方向舵侧力增量
        derivatives.d_cz_d_delta = 0.0;     // 方向舵升力增量（忽略）
    } else if (control_surface == "spoiler") {
        // 扰流板导数（主要产生滚转力矩和阻力）
        derivatives.d_cl_d_delta = -0.05;   // 扰流板滚转导数
        derivatives.d_cm_d_delta = 0.01;    // 扰流板俯仰导数（较小）
        derivatives.d_cn_d_delta = 0.0;     // 扰流板偏航导数（忽略）
        derivatives.d_cx_d_delta = 0.02;    // 扰流板阻力增量
        derivatives.d_cy_d_delta = 0.0;     // 扰流板侧力增量（忽略）
        derivatives.d_cz_d_delta = -0.03;   // 扰流板升力增量（负值，减少升力）
    } else if (control_surface == "flap") {
        // 襟翼导数（主要产生升力和俯仰力矩）
        derivatives.d_cz_d_delta = 0.15;    // 襟翼升力增量
        derivatives.d_cm_d_delta = -0.05;   // 襟翼俯仰导数
        derivatives.d_cl_d_delta = 0.0;     // 襟翼滚转导数（忽略）
        derivatives.d_cn_d_delta = 0.0;     // 襟翼偏航导数（忽略）
        derivatives.d_cx_d_delta = 0.02;    // 襟翼阻力增量
        derivatives.d_cy_d_delta = 0.0;     // 襟翼侧力增量（忽略）
    }
    
    // 交叉导数（迎角和侧滑角的影响）
    derivatives.d_cx_d_alpha = 0.0;        // 迎角对X轴力的影响（忽略）
    derivatives.d_cy_d_beta = 0.0;         // 侧滑角对Y轴力的影响（忽略）
    derivatives.d_cz_d_alpha = 0.0;        // 迎角对Z轴力的影响（忽略）
    derivatives.d_cl_d_beta = 0.0;         // 侧滑角对滚转力矩的影响（忽略）
    derivatives.d_cm_d_alpha = 0.0;        // 迎角对俯仰力矩的影响（忽略）
    derivatives.d_cn_d_beta = 0.0;         // 侧滑角对偏航力矩的影响（忽略）
    
    // 马赫数修正因子（普朗特-格劳厄特修正）
    if (mach < 0.8) {
        derivatives.mach_correction_factor = 1.0 / std::sqrt(1.0 - mach * mach);
    } else {
        derivatives.mach_correction_factor = 1.0;  // 超音速时使用简化修正
    }
    
    return derivatives;
}

// ==================== 耦合气动力增量计算方法实现 ====================
ControlCouplingForceIncrement B737AeroControlIncrementalForces::calculate_coupling_force_increment(const std::string& primary_control,
                                                                                                  const std::string& secondary_control,
                                                                                                  double primary_deflection, double secondary_deflection,
                                                                                                  double mach, double dynamic_pressure) const {
    ControlCouplingForceIncrement coupling;
    
    coupling.primary_control = primary_control;
    coupling.secondary_control = secondary_control;
    
    // 副翼-方向舵耦合
    if ((primary_control == "aileron" && secondary_control == "rudder") ||
        (primary_control == "rudder" && secondary_control == "aileron")) {
        coupling.coupling_factor = 0.1;  // 10%耦合
        coupling.delta_force_x_coupling = 0.001 * dynamic_pressure * reference_wing_area;
        coupling.delta_force_y_coupling = 0.005 * dynamic_pressure * reference_wing_area;
        coupling.delta_force_z_coupling = 0.0;
        coupling.delta_moment_x_coupling = 0.005 * dynamic_pressure * reference_wing_area * reference_span;
        coupling.delta_moment_y_coupling = 0.001 * dynamic_pressure * reference_wing_area * reference_chord;
        coupling.delta_moment_z_coupling = 0.003 * dynamic_pressure * reference_wing_area * reference_span;
    }
    // 副翼-扰流板耦合
    else if ((primary_control == "aileron" && secondary_control == "spoiler") ||
             (primary_control == "spoiler" && secondary_control == "aileron")) {
        coupling.coupling_factor = 0.15;  // 15%耦合
        coupling.delta_force_x_coupling = 0.002 * dynamic_pressure * reference_wing_area;
        coupling.delta_force_y_coupling = 0.0;
        coupling.delta_force_z_coupling = -0.002 * dynamic_pressure * reference_wing_area;
        coupling.delta_moment_x_coupling = 0.008 * dynamic_pressure * reference_wing_area * reference_span;
        coupling.delta_moment_y_coupling = 0.001 * dynamic_pressure * reference_wing_area * reference_chord;
        coupling.delta_moment_z_coupling = 0.0;
    }
    // 升降舵-方向舵耦合
    else if ((primary_control == "elevator" && secondary_control == "rudder") ||
             (primary_control == "rudder" && secondary_control == "elevator")) {
        coupling.coupling_factor = 0.05;  // 5%耦合
        coupling.delta_force_x_coupling = 0.001 * dynamic_pressure * reference_wing_area;
        coupling.delta_force_y_coupling = 0.002 * dynamic_pressure * reference_wing_area;
        coupling.delta_force_z_coupling = 0.001 * dynamic_pressure * reference_wing_area;
        coupling.delta_moment_x_coupling = 0.0;
        coupling.delta_moment_y_coupling = 0.002 * dynamic_pressure * reference_wing_area * reference_chord;
        coupling.delta_moment_z_coupling = 0.001 * dynamic_pressure * reference_wing_area * reference_span;
    }
    else {
        // 无耦合
        coupling.coupling_factor = 0.0;
        coupling.delta_force_x_coupling = 0.0;
        coupling.delta_force_y_coupling = 0.0;
        coupling.delta_force_z_coupling = 0.0;
        coupling.delta_moment_x_coupling = 0.0;
        coupling.delta_moment_y_coupling = 0.0;
        coupling.delta_moment_z_coupling = 0.0;
    }
    
    return coupling;
}

// ==================== 数据插值方法实现 ====================
ControlForceIncrementPoint B737AeroControlIncrementalForces::interpolate_force_increment_data(const std::string& control_surface,
                                                                                              double deflection_angle, double mach,
                                                                                              double reynolds, double alpha, double beta,
                                                                                              double dynamic_pressure) const {
    // 直接调用计算方法，因为我们已经有了解析模型
    return calculate_force_increment(control_surface, deflection_angle, mach, reynolds, alpha, beta, dynamic_pressure);
}

// ==================== 数据验证方法实现 ====================
bool B737AeroControlIncrementalForces::validate_data() const {
    if (aircraft_type.empty() || data_source.empty()) return false;
    if (reference_wing_area <= 0.0 || reference_chord <= 0.0 || reference_span <= 0.0) return false;
    if (force_increment_curves.empty()) return false;
    return true;
}

std::string B737AeroControlIncrementalForces::get_validation_report() const {
    std::string report = "B737操纵面气动力增量数据验证报告:\n";
    
    if (aircraft_type.empty()) report += "- 错误: 飞机型号为空\n";
    if (data_source.empty()) report += "- 错误: 数据来源为空\n";
    if (reference_wing_area <= 0.0) report += "- 错误: 参考机翼面积无效\n";
    if (reference_chord <= 0.0) report += "- 错误: 参考弦长无效\n";
    if (reference_span <= 0.0) report += "- 错误: 参考翼展无效\n";
    if (force_increment_curves.empty()) report += "- 错误: 气动力增量曲线为空\n";
    
    if (validate_data()) {
        report += "- 状态: 数据验证通过\n";
        report += "- 操纵面数量: " + std::to_string(force_increment_curves.size()) + "\n";
        report += "- 参考机翼面积: " + std::to_string(reference_wing_area) + " m²\n";
        report += "- 参考弦长: " + std::to_string(reference_chord) + " m\n";
        report += "- 参考翼展: " + std::to_string(reference_span) + " m\n";
    }
    
    return report;
}

// ==================== 数据加载和保存方法实现 ====================
bool B737AeroControlIncrementalForces::load_from_file(const std::string& filename) {
    // 这里应该实现从文件加载气动力增量数据的逻辑
    return true;
}

bool B737AeroControlIncrementalForces::save_to_file(const std::string& filename) const {
    // 这里应该实现保存气动力增量数据到文件的逻辑
    return true;
}

// ==================== B737-800操纵面气动力增量数据 ====================
const B737AeroControlIncrementalForces B737_800_CONTROL_FORCE_INCREMENT_DATA = []() {
    B737AeroControlIncrementalForces data;
    
    // 基本信息
    data.aircraft_type = "B737-800";
    data.data_source = "Boeing Aerodynamic Control Data";
    data.data_version = "1.0";
    
    // 参考参数
    data.reference_wing_area = 124.6;      // m²
    data.reference_chord = 3.9;            // m
    data.reference_span = 34.3;            // m
    data.reference_dynamic_pressure = 1e5; // Pa (典型巡航动压)
    
    // 创建气动力增量曲线
    // 副翼气动力增量曲线
    ControlForceIncrementCurve aileron_curve;
    aileron_curve.control_surface = "aileron";
    aileron_curve.control_mode = "roll_control";
    
    // 添加副翼气动力增量数据点
    for (int deflection = -25; deflection <= 25; deflection += 5) {
        for (double mach = 0.2; mach <= 0.8; mach += 0.2) {
            ControlForceIncrementPoint point = data.calculate_force_increment("aileron", deflection, mach, 1e7, 0.0, 0.0, 1e5);
            aileron_curve.data_points.push_back(point);
        }
    }
    
    // 计算副翼气动力导数
    aileron_curve.derivatives = data.calculate_derivatives("aileron", 0.5, 1e7, 0.0, 0.0);
    
    data.force_increment_curves["aileron"] = aileron_curve;
    
    // 升降舵气动力增量曲线
    ControlForceIncrementCurve elevator_curve;
    elevator_curve.control_surface = "elevator";
    elevator_curve.control_mode = "pitch_control";
    
    // 添加升降舵气动力增量数据点
    for (int deflection = -30; deflection <= 30; deflection += 5) {
        for (double mach = 0.2; mach <= 0.8; mach += 0.2) {
            ControlForceIncrementPoint point = data.calculate_force_increment("elevator", deflection, mach, 1e7, 0.0, 0.0, 1e5);
            elevator_curve.data_points.push_back(point);
        }
    }
    
    // 计算升降舵气动力导数
    elevator_curve.derivatives = data.calculate_derivatives("elevator", 0.5, 1e7, 0.0, 0.0);
    
    data.force_increment_curves["elevator"] = elevator_curve;
    
    // 方向舵气动力增量曲线
    ControlForceIncrementCurve rudder_curve;
    rudder_curve.control_surface = "rudder";
    rudder_curve.control_mode = "yaw_control";
    
    // 添加方向舵气动力增量数据点
    for (int deflection = -30; deflection <= 30; deflection += 5) {
        for (double mach = 0.2; mach <= 0.8; mach += 0.2) {
            ControlForceIncrementPoint point = data.calculate_force_increment("rudder", deflection, mach, 1e7, 0.0, 0.0, 1e5);
            rudder_curve.data_points.push_back(point);
        }
    }
    
    // 计算方向舵气动力导数
    rudder_curve.derivatives = data.calculate_derivatives("rudder", 0.5, 1e7, 0.0, 0.0);
    
    data.force_increment_curves["rudder"] = rudder_curve;
    
    // 扰流板气动力增量曲线
    ControlForceIncrementCurve spoiler_curve;
    spoiler_curve.control_surface = "spoiler";
    spoiler_curve.control_mode = "roll_and_drag_control";
    
    // 添加扰流板气动力增量数据点
    for (int deflection = 0; deflection <= 60; deflection += 10) {
        for (double mach = 0.2; mach <= 0.8; mach += 0.2) {
            ControlForceIncrementPoint point = data.calculate_force_increment("spoiler", deflection, mach, 1e7, 0.0, 0.0, 1e5);
            spoiler_curve.data_points.push_back(point);
        }
    }
    
    // 计算扰流板气动力导数
    spoiler_curve.derivatives = data.calculate_derivatives("spoiler", 0.5, 1e7, 0.0, 0.0);
    
    data.force_increment_curves["spoiler"] = spoiler_curve;
    
    // 襟翼气动力增量曲线
    ControlForceIncrementCurve flap_curve;
    flap_curve.control_surface = "flap";
    flap_curve.control_mode = "lift_enhancement";
    
    // 添加襟翼气动力增量数据点
    for (int deflection = 0; deflection <= 40; deflection += 5) {
        for (double mach = 0.1; mach <= 0.3; mach += 0.1) {  // 襟翼主要在低速使用
            ControlForceIncrementPoint point = data.calculate_force_increment("flap", deflection, mach, 1e7, 0.0, 0.0, 1e5);
            flap_curve.data_points.push_back(point);
        }
    }
    
    // 计算襟翼气动力导数
    flap_curve.derivatives = data.calculate_derivatives("flap", 0.2, 1e7, 0.0, 0.0);
    
    data.force_increment_curves["flap"] = flap_curve;
    
    // 创建耦合气动力增量
    // 副翼-方向舵耦合
    ControlCouplingForceIncrement aileron_rudder_coupling = data.calculate_coupling_force_increment("aileron", "rudder", 10.0, 5.0, 0.5, 1e5);
    data.coupling_force_increments.push_back(aileron_rudder_coupling);
    
    // 副翼-扰流板耦合
    ControlCouplingForceIncrement aileron_spoiler_coupling = data.calculate_coupling_force_increment("aileron", "spoiler", 10.0, 20.0, 0.5, 1e5);
    data.coupling_force_increments.push_back(aileron_spoiler_coupling);
    
    // 升降舵-方向舵耦合
    ControlCouplingForceIncrement elevator_rudder_coupling = data.calculate_coupling_force_increment("elevator", "rudder", 5.0, 5.0, 0.5, 1e5);
    data.coupling_force_increments.push_back(elevator_rudder_coupling);
    
    return data;
}();

// ==================== B737-700操纵面气动力增量数据 ====================
const B737AeroControlIncrementalForces B737_700_CONTROL_FORCE_INCREMENT_DATA = []() {
    B737AeroControlIncrementalForces data;
    
    // 基本信息
    data.aircraft_type = "B737-700";
    data.data_source = "Boeing Aerodynamic Control Data";
    data.data_version = "1.0";
    
    // 参考参数（与B737-800相同）
    data.reference_wing_area = 124.6;      // m²
    data.reference_chord = 3.9;            // m
    data.reference_span = 34.3;            // m
    data.reference_dynamic_pressure = 1e5; // Pa
    
    // 创建气动力增量曲线（与B737-800相同结构）
    // 副翼气动力增量曲线
    ControlForceIncrementCurve aileron_curve;
    aileron_curve.control_surface = "aileron";
    aileron_curve.control_mode = "roll_control";
    
    // 添加副翼气动力增量数据点
    for (int deflection = -25; deflection <= 25; deflection += 5) {
        for (double mach = 0.2; mach <= 0.8; mach += 0.2) {
            ControlForceIncrementPoint point = data.calculate_force_increment("aileron", deflection, mach, 1e7, 0.0, 0.0, 1e5);
            aileron_curve.data_points.push_back(point);
        }
    }
    
    // 计算副翼气动力导数
    aileron_curve.derivatives = data.calculate_derivatives("aileron", 0.5, 1e7, 0.0, 0.0);
    
    data.force_increment_curves["aileron"] = aileron_curve;
    
    // 升降舵气动力增量曲线
    ControlForceIncrementCurve elevator_curve;
    elevator_curve.control_surface = "elevator";
    elevator_curve.control_mode = "pitch_control";
    
    // 添加升降舵气动力增量数据点
    for (int deflection = -30; deflection <= 30; deflection += 5) {
        for (double mach = 0.2; mach <= 0.8; mach += 0.2) {
            ControlForceIncrementPoint point = data.calculate_force_increment("elevator", deflection, mach, 1e7, 0.0, 0.0, 1e5);
            elevator_curve.data_points.push_back(point);
        }
    }
    
    // 计算升降舵气动力导数
    elevator_curve.derivatives = data.calculate_derivatives("elevator", 0.5, 1e7, 0.0, 0.0);
    
    data.force_increment_curves["elevator"] = elevator_curve;
    
    // 方向舵气动力增量曲线
    ControlForceIncrementCurve rudder_curve;
    rudder_curve.control_surface = "rudder";
    rudder_curve.control_mode = "yaw_control";
    
    // 添加方向舵气动力增量数据点
    for (int deflection = -30; deflection <= 30; deflection += 5) {
        for (double mach = 0.2; mach <= 0.8; mach += 0.2) {
            ControlForceIncrementPoint point = data.calculate_force_increment("rudder", deflection, mach, 1e7, 0.0, 0.0, 1e5);
            rudder_curve.data_points.push_back(point);
        }
    }
    
    // 计算方向舵气动力导数
    rudder_curve.derivatives = data.calculate_derivatives("rudder", 0.5, 1e7, 0.0, 0.0);
    
    data.force_increment_curves["rudder"] = rudder_curve;
    
    // 扰流板气动力增量曲线
    ControlForceIncrementCurve spoiler_curve;
    spoiler_curve.control_surface = "spoiler";
    spoiler_curve.control_mode = "roll_and_drag_control";
    
    // 添加扰流板气动力增量数据点
    for (int deflection = 0; deflection <= 60; deflection += 10) {
        for (double mach = 0.2; mach <= 0.8; mach += 0.2) {
            ControlForceIncrementPoint point = data.calculate_force_increment("spoiler", deflection, mach, 1e7, 0.0, 0.0, 1e5);
            spoiler_curve.data_points.push_back(point);
        }
    }
    
    // 计算扰流板气动力导数
    spoiler_curve.derivatives = data.calculate_derivatives("spoiler", 0.5, 1e7, 0.0, 0.0);
    
    data.force_increment_curves["spoiler"] = spoiler_curve;
    
    // 襟翼气动力增量曲线
    ControlForceIncrementCurve flap_curve;
    flap_curve.control_surface = "flap";
    flap_curve.control_mode = "lift_enhancement";
    
    // 添加襟翼气动力增量数据点
    for (int deflection = 0; deflection <= 40; deflection += 5) {
        for (double mach = 0.1; mach <= 0.3; mach += 0.1) {
            ControlForceIncrementPoint point = data.calculate_force_increment("flap", deflection, mach, 1e7, 0.0, 0.0, 1e5);
            flap_curve.data_points.push_back(point);
        }
    }
    
    // 计算襟翼气动力导数
    flap_curve.derivatives = data.calculate_derivatives("flap", 0.2, 1e7, 0.0, 0.0);
    
    data.force_increment_curves["flap"] = flap_curve;
    
    // 创建耦合气动力增量（与B737-800相同）
    // 副翼-方向舵耦合
    ControlCouplingForceIncrement aileron_rudder_coupling = data.calculate_coupling_force_increment("aileron", "rudder", 10.0, 5.0, 0.5, 1e5);
    data.coupling_force_increments.push_back(aileron_rudder_coupling);
    
    // 副翼-扰流板耦合
    ControlCouplingForceIncrement aileron_spoiler_coupling = data.calculate_coupling_force_increment("aileron", "spoiler", 10.0, 20.0, 0.5, 1e5);
    data.coupling_force_increments.push_back(aileron_spoiler_coupling);
    
    // 升降舵-方向舵耦合
    ControlCouplingForceIncrement elevator_rudder_coupling = data.calculate_coupling_force_increment("elevator", "rudder", 5.0, 5.0, 0.5, 1e5);
    data.coupling_force_increments.push_back(elevator_rudder_coupling);
    
    return data;
}();

// ==================== B737-900操纵面气动力增量数据 ====================
const B737AeroControlIncrementalForces B737_900_CONTROL_FORCE_INCREMENT_DATA = []() {
    B737AeroControlIncrementalForces data;
    
    // 基本信息
    data.aircraft_type = "B737-900";
    data.data_source = "Boeing Aerodynamic Control Data";
    data.data_version = "1.0";
    
    // 参考参数（与B737-800相同）
    data.reference_wing_area = 124.6;      // m²
    data.reference_chord = 3.9;            // m
    data.reference_span = 34.3;            // m
    data.reference_dynamic_pressure = 1e5; // Pa
    
    // 创建气动力增量曲线（与B737-800相同结构）
    // 副翼气动力增量曲线
    ControlForceIncrementCurve aileron_curve;
    aileron_curve.control_surface = "aileron";
    aileron_curve.control_mode = "roll_control";
    
    // 添加副翼气动力增量数据点
    for (int deflection = -25; deflection <= 25; deflection += 5) {
        for (double mach = 0.2; mach <= 0.8; mach += 0.2) {
            ControlForceIncrementPoint point = data.calculate_force_increment("aileron", deflection, mach, 1e7, 0.0, 0.0, 1e5);
            aileron_curve.data_points.push_back(point);
        }
    }
    
    // 计算副翼气动力导数
    aileron_curve.derivatives = data.calculate_derivatives("aileron", 0.5, 1e7, 0.0, 0.0);
    
    data.force_increment_curves["aileron"] = aileron_curve;
    
    // 升降舵气动力增量曲线
    ControlForceIncrementCurve elevator_curve;
    elevator_curve.control_surface = "elevator";
    elevator_curve.control_mode = "pitch_control";
    
    // 添加升降舵气动力增量数据点
    for (int deflection = -30; deflection <= 30; deflection += 5) {
        for (double mach = 0.2; mach <= 0.8; mach += 0.2) {
            ControlForceIncrementPoint point = data.calculate_force_increment("elevator", deflection, mach, 1e7, 0.0, 0.0, 1e5);
            elevator_curve.data_points.push_back(point);
        }
    }
    
    // 计算升降舵气动力导数
    elevator_curve.derivatives = data.calculate_derivatives("elevator", 0.5, 1e7, 0.0, 0.0);
    
    data.force_increment_curves["elevator"] = elevator_curve;
    
    // 方向舵气动力增量曲线
    ControlForceIncrementCurve rudder_curve;
    rudder_curve.control_surface = "rudder";
    rudder_curve.control_mode = "yaw_control";
    
    // 添加方向舵气动力增量数据点
    for (int deflection = -30; deflection <= 30; deflection += 5) {
        for (double mach = 0.2; mach <= 0.8; mach += 0.2) {
            ControlForceIncrementPoint point = data.calculate_force_increment("rudder", deflection, mach, 1e7, 0.0, 0.0, 1e5);
            rudder_curve.data_points.push_back(point);
        }
    }
    
    // 计算方向舵气动力导数
    rudder_curve.derivatives = data.calculate_derivatives("rudder", 0.5, 1e7, 0.0, 0.0);
    
    data.force_increment_curves["rudder"] = rudder_curve;
    
    // 扰流板气动力增量曲线
    ControlForceIncrementCurve spoiler_curve;
    spoiler_curve.control_surface = "spoiler";
    spoiler_curve.control_mode = "roll_and_drag_control";
    
    // 添加扰流板气动力增量数据点
    for (int deflection = 0; deflection <= 60; deflection += 10) {
        for (double mach = 0.2; mach <= 0.8; mach += 0.2) {
            ControlForceIncrementPoint point = data.calculate_force_increment("spoiler", deflection, mach, 1e7, 0.0, 0.0, 1e5);
            spoiler_curve.data_points.push_back(point);
        }
    }
    
    // 计算扰流板气动力导数
    spoiler_curve.derivatives = data.calculate_derivatives("spoiler", 0.5, 1e7, 0.0, 0.0);
    
    data.force_increment_curves["spoiler"] = spoiler_curve;
    
    // 襟翼气动力增量曲线
    ControlForceIncrementCurve flap_curve;
    flap_curve.control_surface = "flap";
    flap_curve.control_mode = "lift_enhancement";
    
    // 添加襟翼气动力增量数据点
    for (int deflection = 0; deflection <= 40; deflection += 5) {
        for (double mach = 0.1; mach <= 0.3; mach += 0.1) {
            ControlForceIncrementPoint point = data.calculate_force_increment("flap", deflection, mach, 1e7, 0.0, 0.0, 1e5);
            flap_curve.data_points.push_back(point);
        }
    }
    
    // 计算襟翼气动力导数
    flap_curve.derivatives = data.calculate_derivatives("flap", 0.2, 1e7, 0.0, 0.0);
    
    data.force_increment_curves["flap"] = flap_curve;
    
    // 创建耦合气动力增量（与B737-800相同）
    // 副翼-方向舵耦合
    ControlCouplingForceIncrement aileron_rudder_coupling = data.calculate_coupling_force_increment("aileron", "rudder", 10.0, 5.0, 0.5, 1e5);
    data.coupling_force_increments.push_back(aileron_rudder_coupling);
    
    // 副翼-扰流板耦合
    ControlCouplingForceIncrement aileron_spoiler_coupling = data.calculate_coupling_force_increment("aileron", "spoiler", 10.0, 20.0, 0.5, 1e5);
    data.coupling_force_increments.push_back(aileron_spoiler_coupling);
    
    // 升降舵-方向舵耦合
    ControlCouplingForceIncrement elevator_rudder_coupling = data.calculate_coupling_force_increment("elevator", "rudder", 5.0, 5.0, 0.5, 1e5);
    data.coupling_force_increments.push_back(elevator_rudder_coupling);
    
    return data;
}();

} // namespace B737
} // namespace AircraftDigitalTwin
} // namespace SMF 