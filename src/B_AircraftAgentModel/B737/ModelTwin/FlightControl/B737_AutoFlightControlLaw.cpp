/**
 * @file B737_AutoFlightControlLaw.cpp
 * @brief B737飞机自动飞行控制律模型实现
 * @details 包含B737飞机的自动飞行控制律实现，包括自动驾驶仪、自动油门、飞行指引等
 * @author VFT_SMF Framework
 * @date 2024
 */

#include "B737_AutoFlightControlLaw.hpp"
#include <cmath>
#include <algorithm>
#include <iostream>
#include <fstream>

namespace SMF {
namespace AircraftDigitalTwin {
namespace B737 {

// ==================== PID控制器实现 ====================
double PIDController::calculate(double error, double dt) {
    if (dt <= 0.0) return 0.0;
    
    // 积分项
    integral += error * dt;
    
    // 积分限制
    if (integral > integral_limit) integral = integral_limit;
    if (integral < -integral_limit) integral = -integral_limit;
    
    // 微分项
    double derivative = (error - previous_error) / dt;
    previous_error = error;
    
    // PID输出
    double output = kp * error + ki * integral + kd * derivative;
    
    // 输出限制
    if (output > output_limit) output = output_limit;
    if (output < -output_limit) output = -output_limit;
    
    return output;
}

void PIDController::reset() {
    integral = 0.0;
    previous_error = 0.0;
}

// ==================== 自动驾驶仪控制律实现 ====================
AutopilotControlLaw::AutopilotControlLaw() : mode(FlightMode::MANUAL), engaged(false) {
    // 设置默认PID参数
    set_roll_gains(2.0, 0.1, 0.5);
    set_pitch_gains(1.5, 0.05, 0.3);
    set_yaw_gains(1.0, 0.02, 0.2);
}

void AutopilotControlLaw::engage(FlightMode new_mode) {
    mode = new_mode;
    engaged = true;
    
    // 重置PID控制器
    roll_controller.reset();
    pitch_controller.reset();
    yaw_controller.reset();
}

void AutopilotControlLaw::disengage() {
    engaged = false;
    mode = FlightMode::MANUAL;
}

double AutopilotControlLaw::calculate_roll_command(const FlightParameters& current, const TargetParameters& target, double dt) {
    if (!engaged) return 0.0;
    
    double error = 0.0;
    
    switch (mode) {
        case FlightMode::AUTOPILOT_HDG:
            // 航向保持模式
            error = target.target_heading - current.heading;
            // 处理航向角跨越360度的情况
            if (error > 180.0) error -= 360.0;
            if (error < -180.0) error += 360.0;
            break;
            
        case FlightMode::AUTOPILOT_NAV:
            // 导航模式（简化实现）
            error = target.target_heading - current.heading;
            if (error > 180.0) error -= 360.0;
            if (error < -180.0) error += 360.0;
            break;
            
        case FlightMode::AUTOPILOT_LOC:
            // 航向道模式（简化实现）
            error = target.target_heading - current.heading;
            if (error > 180.0) error -= 360.0;
            if (error < -180.0) error += 360.0;
            break;
            
        default:
            error = 0.0;
            break;
    }
    
    return roll_controller.calculate(error, dt);
}

double AutopilotControlLaw::calculate_pitch_command(const FlightParameters& current, const TargetParameters& target, double dt) {
    if (!engaged) return 0.0;
    
    double error = 0.0;
    
    switch (mode) {
        case FlightMode::AUTOPILOT_ALT_HOLD:
            // 高度保持模式
            error = target.target_altitude - current.altitude;
            break;
            
        case FlightMode::AUTOPILOT_VS:
            // 垂直速度模式
            error = target.target_vertical_speed - current.vertical_speed;
            break;
            
        case FlightMode::AUTOPILOT_GS:
            // 下滑道模式（简化实现）
            error = target.target_altitude - current.altitude;
            break;
            
        case FlightMode::AUTOPILOT_APP:
            // 进近模式（简化实现）
            error = target.target_altitude - current.altitude;
            break;
            
        default:
            error = 0.0;
            break;
    }
    
    return pitch_controller.calculate(error, dt);
}

double AutopilotControlLaw::calculate_yaw_command(const FlightParameters& current, const TargetParameters& target, double dt) {
    if (!engaged) return 0.0;
    
    // 偏航控制主要用于协调转弯
    double error = -current.yaw_rate;  // 阻尼偏航角速度
    
    return yaw_controller.calculate(error, dt);
}

void AutopilotControlLaw::set_roll_gains(double kp, double ki, double kd) {
    roll_controller.kp = kp;
    roll_controller.ki = ki;
    roll_controller.kd = kd;
}

void AutopilotControlLaw::set_pitch_gains(double kp, double ki, double kd) {
    pitch_controller.kp = kp;
    pitch_controller.ki = ki;
    pitch_controller.kd = kd;
}

void AutopilotControlLaw::set_yaw_gains(double kp, double ki, double kd) {
    yaw_controller.kp = kp;
    yaw_controller.ki = ki;
    yaw_controller.kd = kd;
}

// ==================== 自动油门控制律实现 ====================
AutothrottleControlLaw::AutothrottleControlLaw() : mode(FlightMode::MANUAL), engaged(false) {
    // 设置默认PID参数
    set_speed_gains(0.5, 0.02, 0.1);
    set_n1_gains(1.0, 0.05, 0.2);
}

void AutothrottleControlLaw::engage(FlightMode new_mode) {
    mode = new_mode;
    engaged = true;
    
    // 重置PID控制器
    speed_controller.reset();
    n1_controller.reset();
}

void AutothrottleControlLaw::disengage() {
    engaged = false;
    mode = FlightMode::MANUAL;
}

double AutothrottleControlLaw::calculate_throttle_command(const FlightParameters& current, const TargetParameters& target, double dt) {
    if (!engaged) return 0.0;
    
    double error = 0.0;
    
    switch (mode) {
        case FlightMode::AUTOTHROTTLE_SPEED:
            // 速度保持模式
            error = target.target_airspeed - current.airspeed;
            return speed_controller.calculate(error, dt);
            
        case FlightMode::AUTOTHROTTLE_N1: {
            // N1保持模式
            double current_n1 = (current.n1_left + current.n1_right) / 2.0;
            error = target.target_n1 - current_n1;
            return n1_controller.calculate(error, dt);
        }
            
        case FlightMode::AUTOTHROTTLE_THR_REF: {
            // 推力基准模式（简化实现）
            error = target.target_airspeed - current.airspeed;
            return speed_controller.calculate(error, dt);
        }
            
        default:
            return 0.0;
    }
}

void AutothrottleControlLaw::set_speed_gains(double kp, double ki, double kd) {
    speed_controller.kp = kp;
    speed_controller.ki = ki;
    speed_controller.kd = kd;
}

void AutothrottleControlLaw::set_n1_gains(double kp, double ki, double kd) {
    n1_controller.kp = kp;
    n1_controller.ki = ki;
    n1_controller.kd = kd;
}

// ==================== 飞行指引控制律实现 ====================
FlightDirectorControlLaw::FlightDirectorControlLaw() : active(false) {
    // 设置默认PID参数
    set_roll_gains(1.5, 0.05, 0.3);
    set_pitch_gains(1.2, 0.03, 0.2);
}

void FlightDirectorControlLaw::activate() {
    active = true;
    
    // 重置PID控制器
    roll_controller.reset();
    pitch_controller.reset();
}

void FlightDirectorControlLaw::deactivate() {
    active = false;
}

double FlightDirectorControlLaw::calculate_roll_guidance(const FlightParameters& current, const TargetParameters& target, double dt) {
    if (!active) return 0.0;
    
    // 计算航向误差
    double error = target.target_heading - current.heading;
    if (error > 180.0) error -= 360.0;
    if (error < -180.0) error += 360.0;
    
    return roll_controller.calculate(error, dt);
}

double FlightDirectorControlLaw::calculate_pitch_guidance(const FlightParameters& current, const TargetParameters& target, double dt) {
    if (!active) return 0.0;
    
    // 计算高度误差
    double error = target.target_altitude - current.altitude;
    
    return pitch_controller.calculate(error, dt);
}

void FlightDirectorControlLaw::set_roll_gains(double kp, double ki, double kd) {
    roll_controller.kp = kp;
    roll_controller.ki = ki;
    roll_controller.kd = kd;
}

void FlightDirectorControlLaw::set_pitch_gains(double kp, double ki, double kd) {
    pitch_controller.kp = kp;
    pitch_controller.ki = ki;
    pitch_controller.kd = kd;
}

// ==================== 偏航阻尼器控制律实现 ====================
YawDamperControlLaw::YawDamperControlLaw() : active(false) {
    // 设置默认PID参数
    set_gains(0.8, 0.0, 0.3);
}

void YawDamperControlLaw::activate() {
    active = true;
    yaw_rate_controller.reset();
}

void YawDamperControlLaw::deactivate() {
    active = false;
}

double YawDamperControlLaw::calculate_yaw_damper_command(const FlightParameters& current, double dt) {
    if (!active) return 0.0;
    
    // 阻尼偏航角速度
    double error = -current.yaw_rate;
    
    return yaw_rate_controller.calculate(error, dt);
}

void YawDamperControlLaw::set_gains(double kp, double ki, double kd) {
    yaw_rate_controller.kp = kp;
    yaw_rate_controller.ki = ki;
    yaw_rate_controller.kd = kd;
}

// ==================== B737自动飞行控制律主类实现 ====================
B737AutoFlightControlLaw::B737AutoFlightControlLaw() : update_time(0.01) {
    // 创建各个控制律组件
    autopilot = std::make_unique<AutopilotControlLaw>();
    autothrottle = std::make_unique<AutothrottleControlLaw>();
    flight_director = std::make_unique<FlightDirectorControlLaw>();
    yaw_damper = std::make_unique<YawDamperControlLaw>();
}

void B737AutoFlightControlLaw::initialize() {
    reset();
    
    // 初始化默认配置
    state.current_mode = FlightMode::MANUAL;
    state.autopilot_engaged = false;
    state.autothrottle_engaged = false;
    state.flight_director_active = false;
    state.yaw_damper_active = false;
    
    // 设置默认增益
    state.roll_gain = 1.0;
    state.pitch_gain = 1.0;
    state.yaw_gain = 1.0;
    state.throttle_gain = 1.0;
}

void B737AutoFlightControlLaw::reset() {
    // 重置所有控制律
    autopilot->disengage();
    autothrottle->disengage();
    flight_director->deactivate();
    yaw_damper->deactivate();
    
    // 重置状态
    state.roll_command = 0.0;
    state.pitch_command = 0.0;
    state.yaw_command = 0.0;
    state.throttle_command = 0.0;
    
    // 重置飞行员操作意图执行状态
    is_throttle_ramping = false;
    is_speed_hold_active = false;
    pid_integral = 0.0;
    pid_prev_error = 0.0;
}

void B737AutoFlightControlLaw::engage_autopilot(FlightMode mode) {
    autopilot->engage(mode);
    state.autopilot_engaged = true;
    state.current_mode = mode;
}

void B737AutoFlightControlLaw::disengage_autopilot() {
    autopilot->disengage();
    state.autopilot_engaged = false;
    state.current_mode = FlightMode::MANUAL;
}

void B737AutoFlightControlLaw::engage_autothrottle(FlightMode mode) {
    autothrottle->engage(mode);
    state.autothrottle_engaged = true;
}

void B737AutoFlightControlLaw::disengage_autothrottle() {
    autothrottle->disengage();
    state.autothrottle_engaged = false;
}

void B737AutoFlightControlLaw::activate_flight_director() {
    flight_director->activate();
    state.flight_director_active = true;
}

void B737AutoFlightControlLaw::deactivate_flight_director() {
    flight_director->deactivate();
    state.flight_director_active = false;
}

void B737AutoFlightControlLaw::activate_yaw_damper() {
    yaw_damper->activate();
    state.yaw_damper_active = true;
}

void B737AutoFlightControlLaw::deactivate_yaw_damper() {
    yaw_damper->deactivate();
    state.yaw_damper_active = false;
}

void B737AutoFlightControlLaw::set_flight_parameters(const FlightParameters& params) {
    current_params = params;
}

void B737AutoFlightControlLaw::set_target_parameters(const TargetParameters& params) {
    target_params = params;
}

ControlLawState B737AutoFlightControlLaw::calculate_control_commands() {
    // 计算自动驾驶仪指令
    if (state.autopilot_engaged) {
        state.roll_command = autopilot->calculate_roll_command(current_params, target_params, update_time);
        state.pitch_command = autopilot->calculate_pitch_command(current_params, target_params, update_time);
        state.yaw_command = autopilot->calculate_yaw_command(current_params, target_params, update_time);
    } else {
        state.roll_command = 0.0;
        state.pitch_command = 0.0;
        state.yaw_command = 0.0;
    }
    
    // 计算自动油门指令
    if (state.autothrottle_engaged) {
        state.throttle_command = autothrottle->calculate_throttle_command(current_params, target_params, update_time);
    } else {
        state.throttle_command = 0.0;
    }
    
    // 应用增益
    state.roll_command *= state.roll_gain;
    state.pitch_command *= state.pitch_gain;
    state.yaw_command *= state.yaw_gain;
    state.throttle_command *= state.throttle_gain;
    
    // 限制输出范围
    state.roll_command = std::clamp(state.roll_command, -30.0, 30.0);
    state.pitch_command = std::clamp(state.pitch_command, -20.0, 20.0);
    state.yaw_command = std::clamp(state.yaw_command, -15.0, 15.0);
    state.throttle_command = std::clamp(state.throttle_command, 0.0, 1.0);
    
    return state;
}

void B737AutoFlightControlLaw::load_configuration(const std::string& config_file) {
    // 这里应该实现从配置文件加载控制律参数的逻辑
    std::cout << "加载控制律配置: " << config_file << std::endl;
}

void B737AutoFlightControlLaw::save_configuration(const std::string& config_file) const {
    // 这里应该实现保存控制律参数到配置文件的逻辑
    std::cout << "保存控制律配置: " << config_file << std::endl;
}

bool B737AutoFlightControlLaw::validate_control_commands() const {
    // 验证控制指令的合理性
    if (std::abs(state.roll_command) > 30.0) return false;
    if (std::abs(state.pitch_command) > 20.0) return false;
    if (std::abs(state.yaw_command) > 15.0) return false;
    if (state.throttle_command < 0.0 || state.throttle_command > 1.0) return false;
    
    return true;
}

std::string B737AutoFlightControlLaw::get_validation_report() const {
    std::string report = "B737自动飞行控制律验证报告:\n";
    
    if (!validate_control_commands()) {
        report += "- 错误: 控制指令超出合理范围\n";
        report += "- 滚转指令: " + std::to_string(state.roll_command) + " 度\n";
        report += "- 俯仰指令: " + std::to_string(state.pitch_command) + " 度\n";
        report += "- 偏航指令: " + std::to_string(state.yaw_command) + " 度\n";
        report += "- 油门指令: " + std::to_string(state.throttle_command) + "\n";
    } else {
        report += "- 状态: 控制指令验证通过\n";
    }
    
    report += "- 自动驾驶仪: " + std::string(state.autopilot_engaged ? "接通" : "断开") + "\n";
    report += "- 自动油门: " + std::string(state.autothrottle_engaged ? "接通" : "断开") + "\n";
    report += "- 飞行指引: " + std::string(state.flight_director_active ? "激活" : "关闭") + "\n";
    report += "- 偏航阻尼器: " + std::string(state.yaw_damper_active ? "激活" : "关闭") + "\n";
    
    return report;
}

// ==================== 飞行员操作意图执行接口实现 ====================
// 飞机模型负责具体的控制算法执行

void B737AutoFlightControlLaw::executeThrottlePushToMax(double current_time) {
    // 飞机模型：实现S曲线平滑控制算法
    if (!is_throttle_ramping) {
        // 启动S曲线控制
        throttle_ramp_start_value = state.throttle_command;
        throttle_ramp_target_value = 1.0;
        throttle_ramp_start_time = current_time;
        double delta = std::max(0.0, std::min(1.0, throttle_ramp_target_value) - std::max(0.0, std::min(1.0, throttle_ramp_start_value)));
        throttle_ramp_duration = std::max(0.5, 2.0 * delta); // 0.5s~2.0s之间平滑
        is_throttle_ramping = true;
        
        std::cout << "飞机模型: 启动油门S曲线控制 - 起点=" << throttle_ramp_start_value 
                  << ", 目标=1.0, 时长=" << throttle_ramp_duration << "s" << std::endl;
    }
}

void B737AutoFlightControlLaw::executeBrakePushToMax(double current_time) {
    // 飞机模型：实现刹车控制算法
    state.throttle_command = 0.0; // 油门归零
    // 刹车控制逻辑可以在这里实现
    std::cout << "飞机模型: 执行刹车推至最大操作" << std::endl;
}

void B737AutoFlightControlLaw::executeSpeedHold(double target_speed, double current_time) {
    // 飞机模型：实现速度保持PID控制算法
    speed_hold_setpoint = target_speed;
    is_speed_hold_active = true;
    pid_integral = 0.0;
    pid_prev_error = 0.0;
    last_tick_time = current_time;
    
    // 停止油门S曲线，避免冲突
    is_throttle_ramping = false;
    
    std::cout << "飞机模型: 启动速度保持PID控制 - 目标速度=" << speed_hold_setpoint << " m/s" << std::endl;
}

void B737AutoFlightControlLaw::stopSpeedHold() {
    // 飞机模型：停止速度保持控制
    is_speed_hold_active = false;
    pid_integral = 0.0;
    pid_prev_error = 0.0;
    
    std::cout << "飞机模型: 停止速度保持控制" << std::endl;
}

void B737AutoFlightControlLaw::tick(double current_time) {
    // 飞机模型：每步推进控制算法
    
    // 1) 油门S曲线控制算法
    if (is_throttle_ramping) {
        const double t = std::max(0.0, current_time - throttle_ramp_start_time);
        double u = throttle_ramp_duration <= 0.0 ? 1.0 : std::min(1.0, t / throttle_ramp_duration);
        double s = (3.0 * u * u) - (2.0 * u * u * u); // S曲线
        double value = throttle_ramp_start_value + (throttle_ramp_target_value - throttle_ramp_start_value) * s;
        value = std::max(0.0, std::min(1.0, value));
        
        state.throttle_command = value;
        
        if (u >= 1.0 - 1e-6) {
            is_throttle_ramping = false;
            std::cout << "飞机模型: 油门S曲线控制完成 - 最终油门=" << value << std::endl;
        }
    }
    
    // 2) 速度保持PID控制算法
    if (is_speed_hold_active) {
        const double dt = std::max(1e-3, current_time - last_tick_time);
        last_tick_time = current_time;
        
        // 获取当前速度（这里需要从飞行参数中获取）
        const double measured = std::max(0.0, current_params.groundspeed);
        const double error = speed_hold_setpoint - measured; // 正误差=欠速，负误差=超速
        const double deadband = 0.2; // m/s
        
        // 计算导数项
        const double derivative = (error - pid_prev_error) / dt;
        pid_prev_error = error;
        
        // 处理死区
        if (std::abs(error) <= deadband) {
            // 轻微释放刹车，避免拖拽
            std::cout << "飞机模型: 速度保持 - 在死区内，轻微调整" << std::endl;
            return;
        }
        
        // 抗饱和逻辑
        bool allow_integrate = true;
        if (state.throttle_command >= 1.0 - 1e-6 && error > 0.0) {
            allow_integrate = false; // 欠速且油门已顶，不再积分
        }
        if (state.throttle_command <= 1e-6 && error < 0.0) {
            allow_integrate = false; // 超速且油门已0，不再积分
        }
        
        if (allow_integrate) {
            pid_integral += error * dt;
            const double I_MAX = 50.0; // 积分限幅
            if (pid_integral > I_MAX) pid_integral = I_MAX;
            if (pid_integral < -I_MAX) pid_integral = -I_MAX;
        }
        
        // 计算PID控制量
        const double control = pid_kp * error + pid_ki * pid_integral + pid_kd * derivative;
        
        // 双作动策略
        if (error > deadband) {
            // 欠速：提升油门
            double throttle = state.throttle_command + control;
            throttle = std::max(0.0, std::min(1.0, throttle));
            state.throttle_command = throttle;
            
            std::cout << "飞机模型: 速度保持 - 欠速，提升油门至 " << throttle << std::endl;
        } else if (error < -deadband) {
            // 超速：降低油门
            const double throttle_slew = 0.8 * dt; // 每秒最大降0.8
            double throttle = state.throttle_command;
            throttle = std::max(0.0, throttle - throttle_slew);
            throttle = std::max(0.0, std::min(1.0, throttle + std::min(0.0, control)));
            state.throttle_command = throttle;
            
            std::cout << "飞机模型: 速度保持 - 超速，降低油门至 " << throttle << std::endl;
        }
    }
    
    // 3) 计算自动飞行控制律（原有的自动控制逻辑）
    if (state.autopilot_engaged) {
        state.roll_command = autopilot->calculate_roll_command(current_params, target_params, update_time);
        state.pitch_command = autopilot->calculate_pitch_command(current_params, target_params, update_time);
        state.yaw_command = autopilot->calculate_yaw_command(current_params, target_params, update_time);
    }
    
    if (state.autothrottle_engaged) {
        // 自动油门与手动油门控制冲突时，手动控制优先级更高
        if (!is_throttle_ramping && !is_speed_hold_active) {
            state.throttle_command = autothrottle->calculate_throttle_command(current_params, target_params, update_time);
        }
    }
    
    // 应用增益
    state.roll_command *= state.roll_gain;
    state.pitch_command *= state.pitch_gain;
    state.yaw_command *= state.yaw_gain;
    state.throttle_command *= state.throttle_gain;
    
    // 限制输出范围
    state.roll_command = std::clamp(state.roll_command, -30.0, 30.0);
    state.pitch_command = std::clamp(state.pitch_command, -20.0, 20.0);
    state.yaw_command = std::clamp(state.yaw_command, -15.0, 15.0);
    state.throttle_command = std::clamp(state.throttle_command, 0.0, 1.0);
}

} // namespace B737
} // namespace AircraftDigitalTwin
} // namespace SMF 