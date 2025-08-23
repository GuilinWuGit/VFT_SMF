/**
 * @file B737_AeroControlEfficiencyData.cpp
 * @brief B737飞机气动操纵面效率数据实现
 * @details 包含B737飞机的副翼、升降舵、方向舵、扰流板等操纵面的实际效率数据
 * @author VFT_SMF Framework
 * @date 2024
 */

 #include "B737_AeroControlEfficiencyData.hpp"
 #include <cmath>
 #include <algorithm>
 
 namespace SMF {
 namespace AircraftDigitalTwin {
 namespace B737 {
 
 // ==================== 构造方法实现 ====================
 B737AeroControlEfficiencyData::B737AeroControlEfficiencyData() : aircraft_type(""), data_source(""), data_version(""),
                                                                 max_roll_rate(0.0), max_pitch_rate(0.0), max_yaw_rate(0.0),
                                                                 roll_response_time(0.0), pitch_response_time(0.0), yaw_response_time(0.0),
                                                                 max_control_deflection_speed(0.0), min_control_deflection_speed(0.0),
                                                                 control_authority_limit(0.0), control_redundancy_factor(0.0) {}
 
 // ==================== 操纵面效率计算方法实现 ====================
 double B737AeroControlEfficiencyData::calculate_control_effectiveness(const std::string& control_surface, 
                                                                      double deflection_angle, double mach, 
                                                                      double reynolds, double alpha, double beta) const {
     // 基础效率因子
     double base_effectiveness = 1.0;
     
     // 根据操纵面类型设置基础效率
     if (control_surface == "aileron") {
         base_effectiveness = 0.85;  // 副翼基础效率
     } else if (control_surface == "elevator") {
         base_effectiveness = 0.90;  // 升降舵基础效率
     } else if (control_surface == "rudder") {
         base_effectiveness = 0.80;  // 方向舵基础效率
     } else if (control_surface == "spoiler") {
         base_effectiveness = 0.75;  // 扰流板基础效率
     } else if (control_surface == "flap") {
         base_effectiveness = 0.95;  // 襟翼基础效率
     }
     
     // 偏角修正（非线性效应）
     double deflection_rad = deflection_angle * M_PI / 180.0;
     double deflection_correction = 1.0 - 0.1 * std::abs(std::sin(deflection_rad));
     
     // 马赫数修正
     double mach_correction = 1.0 - 0.2 * mach * mach;  // 超音速效率降低
     
     // 迎角修正
     double alpha_correction = 1.0 - 0.05 * std::abs(alpha) / 15.0;  // 大迎角效率降低
     
     // 侧滑角修正
     double beta_correction = 1.0 - 0.03 * std::abs(beta) / 10.0;  // 大侧滑角效率降低
     
     return base_effectiveness * deflection_correction * mach_correction * alpha_correction * beta_correction;
 }
 
 double B737AeroControlEfficiencyData::calculate_control_derivative(const std::string& control_surface,
                                                                   double deflection_angle, double mach,
                                                                   double reynolds, double alpha, double beta) const {
     // 基础控制导数
     double base_derivative = 0.0;
     
     // 根据操纵面类型设置基础导数
     if (control_surface == "aileron") {
         base_derivative = -0.178;  // 副翼滚转导数
     } else if (control_surface == "elevator") {
         base_derivative = -1.122;  // 升降舵俯仰导数
     } else if (control_surface == "rudder") {
         base_derivative = -0.0654; // 方向舵偏航导数
     } else if (control_surface == "spoiler") {
         base_derivative = -0.05;   // 扰流板滚转导数
     }
     
     // 偏角修正
     double deflection_rad = deflection_angle * M_PI / 180.0;
     double deflection_correction = std::sin(deflection_rad);
     
     // 马赫数修正
     double mach_correction = 1.0 / std::sqrt(1.0 - mach * mach);  // 普朗特-格劳厄特修正
     
     // 迎角修正
     double alpha_correction = 1.0 - 0.1 * std::abs(alpha) / 15.0;
     
     return base_derivative * deflection_correction * mach_correction * alpha_correction;
 }
 
 double B737AeroControlEfficiencyData::calculate_hinge_moment(const std::string& control_surface,
                                                             double deflection_angle, double mach,
                                                             double reynolds, double alpha, double beta) const {
     // 基础铰链力矩系数
     double base_hinge_moment = 0.0;
     
     // 根据操纵面类型设置基础铰链力矩
     if (control_surface == "aileron") {
         base_hinge_moment = -0.02;  // 副翼铰链力矩
     } else if (control_surface == "elevator") {
         base_hinge_moment = -0.05;  // 升降舵铰链力矩
     } else if (control_surface == "rudder") {
         base_hinge_moment = -0.03;  // 方向舵铰链力矩
     } else if (control_surface == "spoiler") {
         base_hinge_moment = -0.01;  // 扰流板铰链力矩
     }
     
     // 偏角修正
     double deflection_rad = deflection_angle * M_PI / 180.0;
     double deflection_correction = std::sin(deflection_rad);
     
     // 马赫数修正
     double mach_correction = 1.0 + 0.1 * mach * mach;
     
     // 迎角修正
     double alpha_correction = 1.0 + 0.05 * std::abs(alpha) / 15.0;
     
     return base_hinge_moment * deflection_correction * mach_correction * alpha_correction;
 }
 
 // ==================== 操纵面响应计算方法实现 ====================
 double B737AeroControlEfficiencyData::calculate_response_time(const std::string& control_surface,
                                                              double deflection_angle, double mach) const {
     // 基础响应时间
     double base_response_time = 0.1;  // 秒
     
     // 根据操纵面类型调整响应时间
     if (control_surface == "aileron") {
         base_response_time = 0.08;  // 副翼响应较快
     } else if (control_surface == "elevator") {
         base_response_time = 0.12;  // 升降舵响应中等
     } else if (control_surface == "rudder") {
         base_response_time = 0.15;  // 方向舵响应较慢
     } else if (control_surface == "spoiler") {
         base_response_time = 0.10;  // 扰流板响应中等
     }
     
     // 马赫数修正
     double mach_correction = 1.0 + 0.1 * mach;  // 高速响应稍慢
     
     // 偏角修正
     double deflection_correction = 1.0 + 0.05 * std::abs(deflection_angle) / 30.0;  // 大偏角响应稍慢
     
     return base_response_time * mach_correction * deflection_correction;
 }
 
 double B737AeroControlEfficiencyData::calculate_power_required(const std::string& control_surface,
                                                               double deflection_angle, double deflection_rate) const {
     // 基础功率需求
     double base_power = 1000.0;  // W
     
     // 根据操纵面类型调整功率需求
     if (control_surface == "aileron") {
         base_power = 800.0;   // 副翼功率需求较低
     } else if (control_surface == "elevator") {
         base_power = 1200.0;  // 升降舵功率需求较高
     } else if (control_surface == "rudder") {
         base_power = 1000.0;  // 方向舵功率需求中等
     } else if (control_surface == "spoiler") {
         base_power = 600.0;   // 扰流板功率需求较低
     }
     
     // 偏角修正
     double deflection_correction = 1.0 + 0.2 * std::abs(deflection_angle) / 30.0;
     
     // 偏转速率修正
     double rate_correction = 1.0 + 0.5 * std::abs(deflection_rate) / 60.0;  // 60度/秒为基准
     
     return base_power * deflection_correction * rate_correction;
 }
 
 // ==================== 操纵面耦合计算方法实现 ====================
 double B737AeroControlEfficiencyData::calculate_coupling_effect(const std::string& primary_control,
                                                                const std::string& secondary_control,
                                                                double primary_deflection, double secondary_deflection) const {
     // 基础耦合因子
     double base_coupling = 0.0;
     
     // 副翼-方向舵耦合
     if ((primary_control == "aileron" && secondary_control == "rudder") ||
         (primary_control == "rudder" && secondary_control == "aileron")) {
         base_coupling = 0.1;  // 10%耦合
     }
     // 升降舵-方向舵耦合
     else if ((primary_control == "elevator" && secondary_control == "rudder") ||
              (primary_control == "rudder" && secondary_control == "elevator")) {
         base_coupling = 0.05;  // 5%耦合
     }
     // 副翼-扰流板耦合
     else if ((primary_control == "aileron" && secondary_control == "spoiler") ||
              (primary_control == "spoiler" && secondary_control == "aileron")) {
         base_coupling = 0.15;  // 15%耦合
     }
     
     // 偏角修正
     double primary_correction = std::abs(primary_deflection) / 30.0;
     double secondary_correction = std::abs(secondary_deflection) / 30.0;
     
     return base_coupling * primary_correction * secondary_correction;
 }
 
 // ==================== 操纵面故障模拟实现 ====================
 void B737AeroControlEfficiencyData::simulate_control_failure(const std::string& control_surface,
                                                             const std::string& failure_type,
                                                             double& effectiveness, double& response_time) const {
     // 查找故障模式
     auto it = failure_modes.find(control_surface + "_" + failure_type);
     if (it != failure_modes.end()) {
         const ControlFailureMode& failure = it->second;
         effectiveness *= (1.0 - failure.effectiveness_reduction);
         response_time += failure.response_delay;
     } else {
         // 默认故障模式
         if (failure_type == "jamming") {
             effectiveness *= 0.5;  // 卡阻效率降低50%
             response_time += 0.5;  // 响应延迟0.5秒
         } else if (failure_type == "free_play") {
             effectiveness *= 0.8;  // 自由间隙效率降低20%
             response_time += 0.2;  // 响应延迟0.2秒
         } else if (failure_type == "hydraulic_loss") {
             effectiveness *= 0.3;  // 液压损失效率降低70%
             response_time += 1.0;  // 响应延迟1秒
         }
     }
 }
 
 // ==================== 数据插值方法实现 ====================
 ControlEfficiencyPoint B737AeroControlEfficiencyData::interpolate_efficiency_data(const std::string& control_surface,
                                                                                   double deflection_angle, double mach,
                                                                                   double reynolds, double alpha, double beta) const {
     ControlEfficiencyPoint result;
     
     // 计算效率参数
     result.effectiveness_factor = calculate_control_effectiveness(control_surface, deflection_angle, mach, reynolds, alpha, beta);
     result.control_derivative = calculate_control_derivative(control_surface, deflection_angle, mach, reynolds, alpha, beta);
     result.hinge_moment_coefficient = calculate_hinge_moment(control_surface, deflection_angle, mach, reynolds, alpha, beta);
     result.response_time = calculate_response_time(control_surface, deflection_angle, mach);
     result.power_required = calculate_power_required(control_surface, deflection_angle, 30.0);  // 假设30度/秒偏转速率
     
     // 设置输入参数
     result.deflection_angle = deflection_angle;
     result.mach_number = mach;
     result.reynolds_number = reynolds;
     result.angle_of_attack = alpha;
     result.sideslip_angle = beta;
     
     return result;
 }
 
 // ==================== 数据验证方法实现 ====================
 bool B737AeroControlEfficiencyData::validate_data() const {
     if (aircraft_type.empty() || data_source.empty()) return false;
     if (control_surfaces.empty()) return false;
     if (max_roll_rate <= 0.0 || max_pitch_rate <= 0.0 || max_yaw_rate <= 0.0) return false;
     return true;
 }
 
 std::string B737AeroControlEfficiencyData::get_validation_report() const {
     std::string report = "B737操纵面效率数据验证报告:\n";
     
     if (aircraft_type.empty()) report += "- 错误: 飞机型号为空\n";
     if (data_source.empty()) report += "- 错误: 数据来源为空\n";
     if (control_surfaces.empty()) report += "- 错误: 操纵面数据为空\n";
     if (max_roll_rate <= 0.0) report += "- 错误: 最大滚转角速度无效\n";
     if (max_pitch_rate <= 0.0) report += "- 错误: 最大俯仰角速度无效\n";
     if (max_yaw_rate <= 0.0) report += "- 错误: 最大偏航角速度无效\n";
     
     if (validate_data()) {
         report += "- 状态: 数据验证通过\n";
         report += "- 操纵面数量: " + std::to_string(control_surfaces.size()) + "\n";
         report += "- 最大滚转角速度: " + std::to_string(max_roll_rate) + " 度/s\n";
         report += "- 最大俯仰角速度: " + std::to_string(max_pitch_rate) + " 度/s\n";
         report += "- 最大偏航角速度: " + std::to_string(max_yaw_rate) + " 度/s\n";
     }
     
     return report;
 }
 
 // ==================== 数据加载和保存方法实现 ====================
 bool B737AeroControlEfficiencyData::load_from_file(const std::string& filename) {
     // 这里应该实现从文件加载操纵面效率数据的逻辑
     return true;
 }
 
 bool B737AeroControlEfficiencyData::save_to_file(const std::string& filename) const {
     // 这里应该实现保存操纵面效率数据到文件的逻辑
     return true;
 }
 
 // ==================== B737-800操纵面效率数据 ====================
 const B737AeroControlEfficiencyData B737_800_CONTROL_EFFICIENCY_DATA = []() {
     B737AeroControlEfficiencyData data;
     
     // 基本信息
     data.aircraft_type = "B737-800";
     data.data_source = "Boeing Flight Control System Data";
     data.data_version = "1.0";
     
     // 操纵面性能参数
     data.max_roll_rate = 45.0;          // 度/s
     data.max_pitch_rate = 25.0;         // 度/s
     data.max_yaw_rate = 20.0;           // 度/s
     data.roll_response_time = 0.5;      // s
     data.pitch_response_time = 0.8;     // s
     data.yaw_response_time = 1.0;       // s
     
     // 操纵面限制
     data.max_control_deflection_speed = 60.0;  // 度/s
     data.min_control_deflection_speed = 5.0;   // 度/s
     data.control_authority_limit = 0.9;        // 90%权限
     data.control_redundancy_factor = 0.8;      // 80%冗余
     
     // 创建操纵面几何参数
     // 副翼
     ControlSurfaceGeometry aileron;
     aileron.surface_name = "aileron";
     aileron.span = 3.2;                 // m
     aileron.chord = 0.8;                // m
     aileron.area = 2.56;                // m²
     aileron.hinge_line_position = 70.0; // %弦长
     aileron.deflection_angle_max = 25.0; // 度
     aileron.deflection_angle_min = -25.0; // 度
     aileron.deflection_rate_max = 60.0;  // 度/s
     aileron.deflection_rate_min = -60.0; // 度/s
     data.control_surfaces["aileron"] = aileron;
     
     // 升降舵
     ControlSurfaceGeometry elevator;
     elevator.surface_name = "elevator";
     elevator.span = 4.5;                 // m
     elevator.chord = 1.2;                // m
     elevator.area = 5.4;                 // m²
     elevator.hinge_line_position = 75.0; // %弦长
     elevator.deflection_angle_max = 30.0; // 度
     elevator.deflection_angle_min = -30.0; // 度
     elevator.deflection_rate_max = 50.0;  // 度/s
     elevator.deflection_rate_min = -50.0; // 度/s
     data.control_surfaces["elevator"] = elevator;
     
     // 方向舵
     ControlSurfaceGeometry rudder;
     rudder.surface_name = "rudder";
     rudder.span = 2.8;                   // m
     rudder.chord = 1.0;                  // m
     rudder.area = 2.8;                   // m²
     rudder.hinge_line_position = 80.0;   // %弦长
     rudder.deflection_angle_max = 30.0;   // 度
     rudder.deflection_angle_min = -30.0;  // 度
     rudder.deflection_rate_max = 40.0;    // 度/s
     rudder.deflection_rate_min = -40.0;   // 度/s
     data.control_surfaces["rudder"] = rudder;
     
     // 扰流板
     ControlSurfaceGeometry spoiler;
     spoiler.surface_name = "spoiler";
     spoiler.span = 2.5;                  // m
     spoiler.chord = 0.6;                 // m
     spoiler.area = 1.5;                  // m²
     spoiler.hinge_line_position = 85.0;  // %弦长
     spoiler.deflection_angle_max = 60.0;  // 度
     spoiler.deflection_angle_min = 0.0;   // 度
     spoiler.deflection_rate_max = 45.0;   // 度/s
     spoiler.deflection_rate_min = 0.0;    // 度/s
     data.control_surfaces["spoiler"] = spoiler;
     
     // 襟翼
     ControlSurfaceGeometry flap;
     flap.surface_name = "flap";
     flap.span = 8.0;                     // m
     flap.chord = 1.5;                    // m
     flap.area = 12.0;                    // m²
     flap.hinge_line_position = 90.0;     // %弦长
     flap.deflection_angle_max = 40.0;     // 度
     flap.deflection_angle_min = 0.0;      // 度
     flap.deflection_rate_max = 10.0;      // 度/s
     flap.deflection_rate_min = 0.0;       // 度/s
     data.control_surfaces["flap"] = flap;
     
     // 创建效率曲线
     // 副翼效率曲线
     ControlEfficiencyCurve aileron_curve;
     aileron_curve.control_mode = "roll_control";
     
     // 添加副翼效率数据点
     for (int deflection = -25; deflection <= 25; deflection += 5) {
         for (double mach = 0.2; mach <= 0.8; mach += 0.2) {
             ControlEfficiencyPoint point;
             point.deflection_angle = deflection;
             point.mach_number = mach;
             point.reynolds_number = 1e7;
             point.angle_of_attack = 0.0;
             point.sideslip_angle = 0.0;
             
             // 计算效率参数
             point.effectiveness_factor = data.calculate_control_effectiveness("aileron", deflection, mach, 1e7, 0.0, 0.0);
             point.control_derivative = data.calculate_control_derivative("aileron", deflection, mach, 1e7, 0.0, 0.0);
             point.hinge_moment_coefficient = data.calculate_hinge_moment("aileron", deflection, mach, 1e7, 0.0, 0.0);
             point.response_time = data.calculate_response_time("aileron", deflection, mach);
             point.power_required = data.calculate_power_required("aileron", deflection, 30.0);
             
             aileron_curve.data_points.push_back(point);
         }
     }
     
     data.efficiency_curves["aileron"] = aileron_curve;
     
     // 升降舵效率曲线
     ControlEfficiencyCurve elevator_curve;
     elevator_curve.control_mode = "pitch_control";
     
     // 添加升降舵效率数据点
     for (int deflection = -30; deflection <= 30; deflection += 5) {
         for (double mach = 0.2; mach <= 0.8; mach += 0.2) {
             ControlEfficiencyPoint point;
             point.deflection_angle = deflection;
             point.mach_number = mach;
             point.reynolds_number = 1e7;
             point.angle_of_attack = 0.0;
             point.sideslip_angle = 0.0;
             
             // 计算效率参数
             point.effectiveness_factor = data.calculate_control_effectiveness("elevator", deflection, mach, 1e7, 0.0, 0.0);
             point.control_derivative = data.calculate_control_derivative("elevator", deflection, mach, 1e7, 0.0, 0.0);
             point.hinge_moment_coefficient = data.calculate_hinge_moment("elevator", deflection, mach, 1e7, 0.0, 0.0);
             point.response_time = data.calculate_response_time("elevator", deflection, mach);
             point.power_required = data.calculate_power_required("elevator", deflection, 30.0);
             
             elevator_curve.data_points.push_back(point);
         }
     }
     
     data.efficiency_curves["elevator"] = elevator_curve;
     
     // 方向舵效率曲线
     ControlEfficiencyCurve rudder_curve;
     rudder_curve.control_mode = "yaw_control";
     
     // 添加方向舵效率数据点
     for (int deflection = -30; deflection <= 30; deflection += 5) {
         for (double mach = 0.2; mach <= 0.8; mach += 0.2) {
             ControlEfficiencyPoint point;
             point.deflection_angle = deflection;
             point.mach_number = mach;
             point.reynolds_number = 1e7;
             point.angle_of_attack = 0.0;
             point.sideslip_angle = 0.0;
             
             // 计算效率参数
             point.effectiveness_factor = data.calculate_control_effectiveness("rudder", deflection, mach, 1e7, 0.0, 0.0);
             point.control_derivative = data.calculate_control_derivative("rudder", deflection, mach, 1e7, 0.0, 0.0);
             point.hinge_moment_coefficient = data.calculate_hinge_moment("rudder", deflection, mach, 1e7, 0.0, 0.0);
             point.response_time = data.calculate_response_time("rudder", deflection, mach);
             point.power_required = data.calculate_power_required("rudder", deflection, 30.0);
             
             rudder_curve.data_points.push_back(point);
         }
     }
     
     data.efficiency_curves["rudder"] = rudder_curve;
     
     // 创建耦合效应
     // 副翼-方向舵耦合
     ControlCouplingEffect aileron_rudder_coupling;
     aileron_rudder_coupling.primary_control = "aileron";
     aileron_rudder_coupling.secondary_control = "rudder";
     aileron_rudder_coupling.coupling_factor = 0.1;
     aileron_rudder_coupling.coupling_phase = 0.0;
     aileron_rudder_coupling.coupling_frequency = 1.0;
     data.coupling_effects.push_back(aileron_rudder_coupling);
     
     // 副翼-扰流板耦合
     ControlCouplingEffect aileron_spoiler_coupling;
     aileron_spoiler_coupling.primary_control = "aileron";
     aileron_spoiler_coupling.secondary_control = "spoiler";
     aileron_spoiler_coupling.coupling_factor = 0.15;
     aileron_spoiler_coupling.coupling_phase = 0.0;
     aileron_spoiler_coupling.coupling_frequency = 2.0;
     data.coupling_effects.push_back(aileron_spoiler_coupling);
     
     // 创建故障模式
     // 副翼卡阻故障
     ControlFailureMode aileron_jamming;
     aileron_jamming.failure_type = "jamming";
     aileron_jamming.effectiveness_reduction = 0.5;
     aileron_jamming.response_delay = 0.5;
     aileron_jamming.jamming_angle = 5.0;
     aileron_jamming.free_play = 0.0;
     data.failure_modes["aileron_jamming"] = aileron_jamming;
     
     // 升降舵液压损失故障
     ControlFailureMode elevator_hydraulic_loss;
     elevator_hydraulic_loss.failure_type = "hydraulic_loss";
     elevator_hydraulic_loss.effectiveness_reduction = 0.7;
     elevator_hydraulic_loss.response_delay = 1.0;
     elevator_hydraulic_loss.jamming_angle = 0.0;
     elevator_hydraulic_loss.free_play = 0.0;
     data.failure_modes["elevator_hydraulic_loss"] = elevator_hydraulic_loss;
     
     // 方向舵自由间隙故障
     ControlFailureMode rudder_free_play;
     rudder_free_play.failure_type = "free_play";
     rudder_free_play.effectiveness_reduction = 0.2;
     rudder_free_play.response_delay = 0.2;
     rudder_free_play.jamming_angle = 0.0;
     rudder_free_play.free_play = 2.0;
     data.failure_modes["rudder_free_play"] = rudder_free_play;
     
     return data;
 }();
 
 // ==================== B737-700操纵面效率数据 ====================
 const B737AeroControlEfficiencyData B737_700_CONTROL_EFFICIENCY_DATA = []() {
     B737AeroControlEfficiencyData data;
     
     // 基本信息
     data.aircraft_type = "B737-700";
     data.data_source = "Boeing Flight Control System Data";
     data.data_version = "1.0";
     
     // 操纵面性能参数（与B737-800相同）
     data.max_roll_rate = 45.0;          // 度/s
     data.max_pitch_rate = 25.0;         // 度/s
     data.max_yaw_rate = 20.0;           // 度/s
     data.roll_response_time = 0.5;      // s
     data.pitch_response_time = 0.8;     // s
     data.yaw_response_time = 1.0;       // s
     
     // 操纵面限制（与B737-800相同）
     data.max_control_deflection_speed = 60.0;  // 度/s
     data.min_control_deflection_speed = 5.0;   // 度/s
     data.control_authority_limit = 0.9;        // 90%权限
     data.control_redundancy_factor = 0.8;      // 80%冗余
     
     // 创建操纵面几何参数（与B737-800相同）
     // 副翼
     ControlSurfaceGeometry aileron;
     aileron.surface_name = "aileron";
     aileron.span = 3.2;                 // m
     aileron.chord = 0.8;                // m
     aileron.area = 2.56;                // m²
     aileron.hinge_line_position = 70.0; // %弦长
     aileron.deflection_angle_max = 25.0; // 度
     aileron.deflection_angle_min = -25.0; // 度
     aileron.deflection_rate_max = 60.0;  // 度/s
     aileron.deflection_rate_min = -60.0; // 度/s
     data.control_surfaces["aileron"] = aileron;
     
     // 升降舵
     ControlSurfaceGeometry elevator;
     elevator.surface_name = "elevator";
     elevator.span = 4.5;                 // m
     elevator.chord = 1.2;                // m
     elevator.area = 5.4;                 // m²
     elevator.hinge_line_position = 75.0; // %弦长
     elevator.deflection_angle_max = 30.0; // 度
     elevator.deflection_angle_min = -30.0; // 度
     elevator.deflection_rate_max = 50.0;  // 度/s
     elevator.deflection_rate_min = -50.0; // 度/s
     data.control_surfaces["elevator"] = elevator;
     
     // 方向舵
     ControlSurfaceGeometry rudder;
     rudder.surface_name = "rudder";
     rudder.span = 2.8;                   // m
     rudder.chord = 1.0;                  // m
     rudder.area = 2.8;                   // m²
     rudder.hinge_line_position = 80.0;   // %弦长
     rudder.deflection_angle_max = 30.0;   // 度
     rudder.deflection_angle_min = -30.0;  // 度
     rudder.deflection_rate_max = 40.0;    // 度/s
     rudder.deflection_rate_min = -40.0;   // 度/s
     data.control_surfaces["rudder"] = rudder;
     
     // 扰流板
     ControlSurfaceGeometry spoiler;
     spoiler.surface_name = "spoiler";
     spoiler.span = 2.5;                  // m
     spoiler.chord = 0.6;                 // m
     spoiler.area = 1.5;                  // m²
     spoiler.hinge_line_position = 85.0;  // %弦长
     spoiler.deflection_angle_max = 60.0;  // 度
     spoiler.deflection_angle_min = 0.0;   // 度
     spoiler.deflection_rate_max = 45.0;   // 度/s
     spoiler.deflection_rate_min = 0.0;    // 度/s
     data.control_surfaces["spoiler"] = spoiler;
     
     // 襟翼
     ControlSurfaceGeometry flap;
     flap.surface_name = "flap";
     flap.span = 8.0;                     // m
     flap.chord = 1.5;                    // m
     flap.area = 12.0;                    // m²
     flap.hinge_line_position = 90.0;     // %弦长
     flap.deflection_angle_max = 40.0;     // 度
     flap.deflection_angle_min = 0.0;      // 度
     flap.deflection_rate_max = 10.0;      // 度/s
     flap.deflection_rate_min = 0.0;       // 度/s
     data.control_surfaces["flap"] = flap;
     
     // 创建效率曲线（与B737-800相同结构）
     // 副翼效率曲线
     ControlEfficiencyCurve aileron_curve;
     aileron_curve.control_mode = "roll_control";
     
     // 添加副翼效率数据点
     for (int deflection = -25; deflection <= 25; deflection += 5) {
         for (double mach = 0.2; mach <= 0.8; mach += 0.2) {
             ControlEfficiencyPoint point;
             point.deflection_angle = deflection;
             point.mach_number = mach;
             point.reynolds_number = 1e7;
             point.angle_of_attack = 0.0;
             point.sideslip_angle = 0.0;
             
             // 计算效率参数
             point.effectiveness_factor = data.calculate_control_effectiveness("aileron", deflection, mach, 1e7, 0.0, 0.0);
             point.control_derivative = data.calculate_control_derivative("aileron", deflection, mach, 1e7, 0.0, 0.0);
             point.hinge_moment_coefficient = data.calculate_hinge_moment("aileron", deflection, mach, 1e7, 0.0, 0.0);
             point.response_time = data.calculate_response_time("aileron", deflection, mach);
             point.power_required = data.calculate_power_required("aileron", deflection, 30.0);
             
             aileron_curve.data_points.push_back(point);
         }
     }
     
     data.efficiency_curves["aileron"] = aileron_curve;
     
     // 升降舵效率曲线
     ControlEfficiencyCurve elevator_curve;
     elevator_curve.control_mode = "pitch_control";
     
     // 添加升降舵效率数据点
     for (int deflection = -30; deflection <= 30; deflection += 5) {
         for (double mach = 0.2; mach <= 0.8; mach += 0.2) {
             ControlEfficiencyPoint point;
             point.deflection_angle = deflection;
             point.mach_number = mach;
             point.reynolds_number = 1e7;
             point.angle_of_attack = 0.0;
             point.sideslip_angle = 0.0;
             
             // 计算效率参数
             point.effectiveness_factor = data.calculate_control_effectiveness("elevator", deflection, mach, 1e7, 0.0, 0.0);
             point.control_derivative = data.calculate_control_derivative("elevator", deflection, mach, 1e7, 0.0, 0.0);
             point.hinge_moment_coefficient = data.calculate_hinge_moment("elevator", deflection, mach, 1e7, 0.0, 0.0);
             point.response_time = data.calculate_response_time("elevator", deflection, mach);
             point.power_required = data.calculate_power_required("elevator", deflection, 30.0);
             
             elevator_curve.data_points.push_back(point);
         }
     }
     
     data.efficiency_curves["elevator"] = elevator_curve;
     
     // 方向舵效率曲线
     ControlEfficiencyCurve rudder_curve;
     rudder_curve.control_mode = "yaw_control";
     
     // 添加方向舵效率数据点
     for (int deflection = -30; deflection <= 30; deflection += 5) {
         for (double mach = 0.2; mach <= 0.8; mach += 0.2) {
             ControlEfficiencyPoint point;
             point.deflection_angle = deflection;
             point.mach_number = mach;
             point.reynolds_number = 1e7;
             point.angle_of_attack = 0.0;
             point.sideslip_angle = 0.0;
             
             // 计算效率参数
             point.effectiveness_factor = data.calculate_control_effectiveness("rudder", deflection, mach, 1e7, 0.0, 0.0);
             point.control_derivative = data.calculate_control_derivative("rudder", deflection, mach, 1e7, 0.0, 0.0);
             point.hinge_moment_coefficient = data.calculate_hinge_moment("rudder", deflection, mach, 1e7, 0.0, 0.0);
             point.response_time = data.calculate_response_time("rudder", deflection, mach);
             point.power_required = data.calculate_power_required("rudder", deflection, 30.0);
             
             rudder_curve.data_points.push_back(point);
         }
     }
     
     data.efficiency_curves["rudder"] = rudder_curve;
     
     // 创建耦合效应（与B737-800相同）
     // 副翼-方向舵耦合
     ControlCouplingEffect aileron_rudder_coupling;
     aileron_rudder_coupling.primary_control = "aileron";
     aileron_rudder_coupling.secondary_control = "rudder";
     aileron_rudder_coupling.coupling_factor = 0.1;
     aileron_rudder_coupling.coupling_phase = 0.0;
     aileron_rudder_coupling.coupling_frequency = 1.0;
     data.coupling_effects.push_back(aileron_rudder_coupling);
     
     // 副翼-扰流板耦合
     ControlCouplingEffect aileron_spoiler_coupling;
     aileron_spoiler_coupling.primary_control = "aileron";
     aileron_spoiler_coupling.secondary_control = "spoiler";
     aileron_spoiler_coupling.coupling_factor = 0.15;
     aileron_spoiler_coupling.coupling_phase = 0.0;
     aileron_spoiler_coupling.coupling_frequency = 2.0;
     data.coupling_effects.push_back(aileron_spoiler_coupling);
     
     // 创建故障模式（与B737-800相同）
     // 副翼卡阻故障
     ControlFailureMode aileron_jamming;
     aileron_jamming.failure_type = "jamming";
     aileron_jamming.effectiveness_reduction = 0.5;
     aileron_jamming.response_delay = 0.5;
     aileron_jamming.jamming_angle = 5.0;
     aileron_jamming.free_play = 0.0;
     data.failure_modes["aileron_jamming"] = aileron_jamming;
     
     // 升降舵液压损失故障
     ControlFailureMode elevator_hydraulic_loss;
     elevator_hydraulic_loss.failure_type = "hydraulic_loss";
     elevator_hydraulic_loss.effectiveness_reduction = 0.7;
     elevator_hydraulic_loss.response_delay = 1.0;
     elevator_hydraulic_loss.jamming_angle = 0.0;
     elevator_hydraulic_loss.free_play = 0.0;
     data.failure_modes["elevator_hydraulic_loss"] = elevator_hydraulic_loss;
     
     // 方向舵自由间隙故障
     ControlFailureMode rudder_free_play;
     rudder_free_play.failure_type = "free_play";
     rudder_free_play.effectiveness_reduction = 0.2;
     rudder_free_play.response_delay = 0.2;
     rudder_free_play.jamming_angle = 0.0;
     rudder_free_play.free_play = 2.0;
     data.failure_modes["rudder_free_play"] = rudder_free_play;
     
     return data;
 }();

// ==================== B737-900操纵面效率数据 ====================
const B737AeroControlEfficiencyData B737_900_CONTROL_EFFICIENCY_DATA = []() {
    B737AeroControlEfficiencyData data;
    
    // 基本信息
    data.aircraft_type = "B737-900";
    data.data_source = "Boeing Flight Control System Data";
    data.data_version = "1.0";
    
    // 操纵面性能参数（与B737-800相同）
    data.max_roll_rate = 45.0;          // 度/s
    data.max_pitch_rate = 25.0;         // 度/s
    data.max_yaw_rate = 20.0;           // 度/s
    data.roll_response_time = 0.5;      // s
    data.pitch_response_time = 0.8;     // s
    data.yaw_response_time = 1.0;       // s
    
    // 操纵面限制（与B737-800相同）
    data.max_control_deflection_speed = 60.0;  // 度/s
    data.min_control_deflection_speed = 5.0;   // 度/s
    data.control_authority_limit = 0.9;        // 90%权限
    data.control_redundancy_factor = 0.8;      // 80%冗余
    
    // 创建操纵面几何参数（与B737-800相同）
    // 副翼
    ControlSurfaceGeometry aileron;
    aileron.surface_name = "aileron";
    aileron.span = 3.2;                 // m
    aileron.chord = 0.8;                // m
    aileron.area = 2.56;                // m²
    aileron.hinge_line_position = 70.0; // %弦长
    aileron.deflection_angle_max = 25.0; // 度
    aileron.deflection_angle_min = -25.0; // 度
    aileron.deflection_rate_max = 60.0;  // 度/s
    aileron.deflection_rate_min = -60.0; // 度/s
    data.control_surfaces["aileron"] = aileron;
    
    // 升降舵
    ControlSurfaceGeometry elevator;
    elevator.surface_name = "elevator";
    elevator.span = 4.5;                 // m
    elevator.chord = 1.2;                // m
    elevator.area = 5.4;                 // m²
    elevator.hinge_line_position = 75.0; // %弦长
    elevator.deflection_angle_max = 30.0; // 度
    elevator.deflection_angle_min = -30.0; // 度
    elevator.deflection_rate_max = 50.0;  // 度/s
    elevator.deflection_rate_min = -50.0; // 度/s
    data.control_surfaces["elevator"] = elevator;
    
    // 方向舵
    ControlSurfaceGeometry rudder;
    rudder.surface_name = "rudder";
    rudder.span = 2.8;                   // m
    rudder.chord = 1.0;                  // m
    rudder.area = 2.8;                   // m²
    rudder.hinge_line_position = 80.0;   // %弦长
    rudder.deflection_angle_max = 30.0;   // 度
    rudder.deflection_angle_min = -30.0;  // 度
    rudder.deflection_rate_max = 40.0;    // 度/s
    rudder.deflection_rate_min = -40.0;   // 度/s
    data.control_surfaces["rudder"] = rudder;
    
    // 扰流板
    ControlSurfaceGeometry spoiler;
    spoiler.surface_name = "spoiler";
    spoiler.span = 2.5;                  // m
    spoiler.chord = 0.6;                 // m
    spoiler.area = 1.5;                  // m²
    spoiler.hinge_line_position = 85.0;  // %弦长
    spoiler.deflection_angle_max = 60.0;  // 度
    spoiler.deflection_angle_min = 0.0;   // 度
    spoiler.deflection_rate_max = 45.0;   // 度/s
    spoiler.deflection_rate_min = 0.0;    // 度/s
    data.control_surfaces["spoiler"] = spoiler;
    
    // 襟翼
    ControlSurfaceGeometry flap;
    flap.surface_name = "flap";
    flap.span = 8.0;                     // m
    flap.chord = 1.5;                    // m
    flap.area = 12.0;                    // m²
    flap.hinge_line_position = 90.0;     // %弦长
    flap.deflection_angle_max = 40.0;     // 度
    flap.deflection_angle_min = 0.0;      // 度
    flap.deflection_rate_max = 10.0;      // 度/s
    flap.deflection_rate_min = 0.0;       // 度/s
    data.control_surfaces["flap"] = flap;
    
    // 创建效率曲线（与B737-800相同结构）
    // 副翼效率曲线
    ControlEfficiencyCurve aileron_curve;
    aileron_curve.control_mode = "roll_control";
    
    // 添加副翼效率数据点
    for (int deflection = -25; deflection <= 25; deflection += 5) {
        for (double mach = 0.2; mach <= 0.8; mach += 0.2) {
            ControlEfficiencyPoint point;
            point.deflection_angle = deflection;
            point.mach_number = mach;
            point.reynolds_number = 1e7;
            point.angle_of_attack = 0.0;
            point.sideslip_angle = 0.0;
            
            // 计算效率参数
            point.effectiveness_factor = data.calculate_control_effectiveness("aileron", deflection, mach, 1e7, 0.0, 0.0);
            point.control_derivative = data.calculate_control_derivative("aileron", deflection, mach, 1e7, 0.0, 0.0);
            point.hinge_moment_coefficient = data.calculate_hinge_moment("aileron", deflection, mach, 1e7, 0.0, 0.0);
            point.response_time = data.calculate_response_time("aileron", deflection, mach);
            point.power_required = data.calculate_power_required("aileron", deflection, 30.0);
            
            aileron_curve.data_points.push_back(point);
        }
    }
    
    data.efficiency_curves["aileron"] = aileron_curve;
    
    // 升降舵效率曲线
    ControlEfficiencyCurve elevator_curve;
    elevator_curve.control_mode = "pitch_control";
    
    // 添加升降舵效率数据点
    for (int deflection = -30; deflection <= 30; deflection += 5) {
        for (double mach = 0.2; mach <= 0.8; mach += 0.2) {
            ControlEfficiencyPoint point;
            point.deflection_angle = deflection;
            point.mach_number = mach;
            point.reynolds_number = 1e7;
            point.angle_of_attack = 0.0;
            point.sideslip_angle = 0.0;
            
            // 计算效率参数
            point.effectiveness_factor = data.calculate_control_effectiveness("elevator", deflection, mach, 1e7, 0.0, 0.0);
            point.control_derivative = data.calculate_control_derivative("elevator", deflection, mach, 1e7, 0.0, 0.0);
            point.hinge_moment_coefficient = data.calculate_hinge_moment("elevator", deflection, mach, 1e7, 0.0, 0.0);
            point.response_time = data.calculate_response_time("elevator", deflection, mach);
            point.power_required = data.calculate_power_required("elevator", deflection, 30.0);
            
            elevator_curve.data_points.push_back(point);
        }
    }
    
    data.efficiency_curves["elevator"] = elevator_curve;
    
    // 方向舵效率曲线
    ControlEfficiencyCurve rudder_curve;
    rudder_curve.control_mode = "yaw_control";
    
    // 添加方向舵效率数据点
    for (int deflection = -30; deflection <= 30; deflection += 5) {
        for (double mach = 0.2; mach <= 0.8; mach += 0.2) {
            ControlEfficiencyPoint point;
            point.deflection_angle = deflection;
            point.mach_number = mach;
            point.reynolds_number = 1e7;
            point.angle_of_attack = 0.0;
            point.sideslip_angle = 0.0;
            
            // 计算效率参数
            point.effectiveness_factor = data.calculate_control_effectiveness("rudder", deflection, mach, 1e7, 0.0, 0.0);
            point.control_derivative = data.calculate_control_derivative("rudder", deflection, mach, 1e7, 0.0, 0.0);
            point.hinge_moment_coefficient = data.calculate_hinge_moment("rudder", deflection, mach, 1e7, 0.0, 0.0);
            point.response_time = data.calculate_response_time("rudder", deflection, mach);
            point.power_required = data.calculate_power_required("rudder", deflection, 30.0);
            
            rudder_curve.data_points.push_back(point);
        }
    }
    
    data.efficiency_curves["rudder"] = rudder_curve;
    
    // 创建耦合效应（与B737-800相同）
    // 副翼-方向舵耦合
    ControlCouplingEffect aileron_rudder_coupling;
    aileron_rudder_coupling.primary_control = "aileron";
    aileron_rudder_coupling.secondary_control = "rudder";
    aileron_rudder_coupling.coupling_factor = 0.1;
    aileron_rudder_coupling.coupling_phase = 0.0;
    aileron_rudder_coupling.coupling_frequency = 1.0;
    data.coupling_effects.push_back(aileron_rudder_coupling);
    
    // 副翼-扰流板耦合
    ControlCouplingEffect aileron_spoiler_coupling;
    aileron_spoiler_coupling.primary_control = "aileron";
    aileron_spoiler_coupling.secondary_control = "spoiler";
    aileron_spoiler_coupling.coupling_factor = 0.15;
    aileron_spoiler_coupling.coupling_phase = 0.0;
    aileron_spoiler_coupling.coupling_frequency = 2.0;
    data.coupling_effects.push_back(aileron_spoiler_coupling);
    
    // 创建故障模式（与B737-800相同）
    // 副翼卡阻故障
    ControlFailureMode aileron_jamming;
    aileron_jamming.failure_type = "jamming";
    aileron_jamming.effectiveness_reduction = 0.5;
    aileron_jamming.response_delay = 0.5;
    aileron_jamming.jamming_angle = 5.0;
    aileron_jamming.free_play = 0.0;
    data.failure_modes["aileron_jamming"] = aileron_jamming;
    
    // 升降舵液压损失故障
    ControlFailureMode elevator_hydraulic_loss;
    elevator_hydraulic_loss.failure_type = "hydraulic_loss";
    elevator_hydraulic_loss.effectiveness_reduction = 0.7;
    elevator_hydraulic_loss.response_delay = 1.0;
    elevator_hydraulic_loss.jamming_angle = 0.0;
    elevator_hydraulic_loss.free_play = 0.0;
    data.failure_modes["elevator_hydraulic_loss"] = elevator_hydraulic_loss;
    
    // 方向舵自由间隙故障
    ControlFailureMode rudder_free_play;
    rudder_free_play.failure_type = "free_play";
    rudder_free_play.effectiveness_reduction = 0.2;
    rudder_free_play.response_delay = 0.2;
    rudder_free_play.jamming_angle = 0.0;
    rudder_free_play.free_play = 2.0;
    data.failure_modes["rudder_free_play"] = rudder_free_play;
    
    return data;
}();

} // namespace B737
} // namespace AircraftDigitalTwin
} // namespace SMF