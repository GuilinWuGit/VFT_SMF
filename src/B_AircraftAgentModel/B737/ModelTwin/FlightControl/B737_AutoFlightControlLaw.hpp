/**
 * @file B737_AutoFlightControlLaw.hpp
 * @brief B737飞机自动飞行控制律模型定义
 * @details 包含B737飞机的自动飞行控制律，包括自动驾驶仪、自动油门、飞行指引等
 * @author VFT_SMF Framework
 * @date 2024
 */

#ifndef B737_AUTO_FLIGHT_CONTROL_LAW_HPP
#define B737_AUTO_FLIGHT_CONTROL_LAW_HPP

#include <string>
#include <vector>
#include <map>
#include <array>
#include <memory>
#include <functional>

namespace SMF {
namespace AircraftDigitalTwin {
namespace B737 {

/**
 * @brief 飞行模式枚举
 * @details 定义自动飞行的各种模式
 */
enum class FlightMode {
    MANUAL,                     ///< 手动飞行
    AUTOPILOT_HDG,             ///< 自动驾驶-航向保持
    AUTOPILOT_NAV,             ///< 自动驾驶-导航
    AUTOPILOT_ALT_HOLD,        ///< 自动驾驶-高度保持
    AUTOPILOT_VS,              ///< 自动驾驶-垂直速度
    AUTOPILOT_GS,              ///< 自动驾驶-下滑道
    AUTOPILOT_LOC,             ///< 自动驾驶-航向道
    AUTOPILOT_APP,             ///< 自动驾驶-进近
    AUTOPILOT_GA,              ///< 自动驾驶-复飞
    AUTOTHROTTLE_SPEED,        ///< 自动油门-速度保持
    AUTOTHROTTLE_N1,           ///< 自动油门-N1保持
    AUTOTHROTTLE_THR_REF,      ///< 自动油门-推力基准
    FLIGHT_DIRECTOR,           ///< 飞行指引
    YAW_DAMPER,                ///< 偏航阻尼器
    STABILITY_AUGMENTATION     ///< 增稳系统
};

/**
 * @brief 控制律状态结构体
 * @details 定义控制律的当前状态
 */
struct ControlLawState {
    FlightMode current_mode;           ///< 当前飞行模式
    bool autopilot_engaged;            ///< 自动驾驶仪接通状态
    bool autothrottle_engaged;         ///< 自动油门接通状态
    bool flight_director_active;       ///< 飞行指引激活状态
    bool yaw_damper_active;            ///< 偏航阻尼器激活状态
    
    // 控制输出
    double roll_command;               ///< 滚转指令 (度)
    double pitch_command;              ///< 俯仰指令 (度)
    double yaw_command;                ///< 偏航指令 (度)
    double throttle_command;           ///< 油门指令 (0.0-1.0)
    
    // 控制律参数
    double roll_gain;                  ///< 滚转增益
    double pitch_gain;                 ///< 俯仰增益
    double yaw_gain;                   ///< 偏航增益
    double throttle_gain;              ///< 油门增益
    
    ControlLawState() : current_mode(FlightMode::MANUAL), autopilot_engaged(false),
                       autothrottle_engaged(false), flight_director_active(false),
                       yaw_damper_active(false), roll_command(0.0), pitch_command(0.0),
                       yaw_command(0.0), throttle_command(0.0), roll_gain(1.0),
                       pitch_gain(1.0), yaw_gain(1.0), throttle_gain(1.0) {}
};

/**
 * @brief 飞行参数结构体
 * @details 定义飞行控制所需的参数
 */
struct FlightParameters {
    // 位置参数
    double latitude;                   ///< 纬度 (度)
    double longitude;                  ///< 经度 (度)
    double altitude;                   ///< 高度 (m)
    double heading;                    ///< 航向 (度)
    
    // 速度参数
    double airspeed;                   ///< 空速 (m/s)
    double groundspeed;                ///< 地速 (m/s)
    double vertical_speed;             ///< 垂直速度 (m/s)
    
    // 姿态参数
    double roll_angle;                 ///< 滚转角 (度)
    double pitch_angle;                ///< 俯仰角 (度)
    double yaw_angle;                  ///< 偏航角 (度)
    
    // 角速度参数
    double roll_rate;                  ///< 滚转角速度 (度/s)
    double pitch_rate;                 ///< 俯仰角速度 (度/s)
    double yaw_rate;                   ///< 偏航角速度 (度/s)
    
    // 发动机参数
    double n1_left;                    ///< 左发N1 (%)
    double n1_right;                   ///< 右发N1 (%)
    double thrust_left;                ///< 左发推力 (N)
    double thrust_right;               ///< 右发推力 (N)
    
    FlightParameters() : latitude(0.0), longitude(0.0), altitude(0.0), heading(0.0),
                        airspeed(0.0), groundspeed(0.0), vertical_speed(0.0),
                        roll_angle(0.0), pitch_angle(0.0), yaw_angle(0.0),
                        roll_rate(0.0), pitch_rate(0.0), yaw_rate(0.0),
                        n1_left(0.0), n1_right(0.0), thrust_left(0.0), thrust_right(0.0) {}
};

/**
 * @brief 目标参数结构体
 * @details 定义控制律的目标参数
 */
struct TargetParameters {
    double target_altitude;            ///< 目标高度 (m)
    double target_airspeed;            ///< 目标空速 (m/s)
    double target_heading;             ///< 目标航向 (度)
    double target_vertical_speed;      ///< 目标垂直速度 (m/s)
    double target_n1;                  ///< 目标N1 (%)
    
    TargetParameters() : target_altitude(0.0), target_airspeed(0.0), target_heading(0.0),
                        target_vertical_speed(0.0), target_n1(0.0) {}
};

/**
 * @brief PID控制器结构体
 * @details 定义PID控制器的参数和状态
 */
struct PIDController {
    double kp;                         ///< 比例增益
    double ki;                         ///< 积分增益
    double kd;                         ///< 微分增益
    double integral;                   ///< 积分项
    double previous_error;             ///< 上一次误差
    double output_limit;               ///< 输出限制
    double integral_limit;             ///< 积分限制
    
    PIDController() : kp(1.0), ki(0.0), kd(0.0), integral(0.0), previous_error(0.0),
                     output_limit(100.0), integral_limit(50.0) {}
    
    double calculate(double error, double dt);
    void reset();
};

/**
 * @brief 自动驾驶仪控制律类
 * @details 实现自动驾驶仪的控制逻辑
 */
class AutopilotControlLaw {
private:
    PIDController roll_controller;     ///< 滚转PID控制器
    PIDController pitch_controller;    ///< 俯仰PID控制器
    PIDController yaw_controller;      ///< 偏航PID控制器
    
    FlightMode mode;                   ///< 当前模式
    bool engaged;                      ///< 接通状态
    
public:
    AutopilotControlLaw();
    
    // 控制方法
    void engage(FlightMode mode);
    void disengage();
    bool is_engaged() const { return engaged; }
    FlightMode get_mode() const { return mode; }
    
    // 控制律计算
    double calculate_roll_command(const FlightParameters& current, const TargetParameters& target, double dt);
    double calculate_pitch_command(const FlightParameters& current, const TargetParameters& target, double dt);
    double calculate_yaw_command(const FlightParameters& current, const TargetParameters& target, double dt);
    
    // 参数设置
    void set_roll_gains(double kp, double ki, double kd);
    void set_pitch_gains(double kp, double ki, double kd);
    void set_yaw_gains(double kp, double ki, double kd);
};

/**
 * @brief 自动油门控制律类
 * @details 实现自动油门的控制逻辑
 */
class AutothrottleControlLaw {
private:
    PIDController speed_controller;    ///< 速度PID控制器
    PIDController n1_controller;       ///< N1 PID控制器
    
    FlightMode mode;                   ///< 当前模式
    bool engaged;                      ///< 接通状态
    
public:
    AutothrottleControlLaw();
    
    // 控制方法
    void engage(FlightMode mode);
    void disengage();
    bool is_engaged() const { return engaged; }
    FlightMode get_mode() const { return mode; }
    
    // 控制律计算
    double calculate_throttle_command(const FlightParameters& current, const TargetParameters& target, double dt);
    
    // 参数设置
    void set_speed_gains(double kp, double ki, double kd);
    void set_n1_gains(double kp, double ki, double kd);
};

/**
 * @brief 飞行指引控制律类
 * @details 实现飞行指引的控制逻辑
 */
class FlightDirectorControlLaw {
private:
    PIDController roll_controller;     ///< 滚转PID控制器
    PIDController pitch_controller;    ///< 俯仰PID控制器
    
    bool active;                       ///< 激活状态
    
public:
    FlightDirectorControlLaw();
    
    // 控制方法
    void activate();
    void deactivate();
    bool is_active() const { return active; }
    
    // 控制律计算
    double calculate_roll_guidance(const FlightParameters& current, const TargetParameters& target, double dt);
    double calculate_pitch_guidance(const FlightParameters& current, const TargetParameters& target, double dt);
    
    // 参数设置
    void set_roll_gains(double kp, double ki, double kd);
    void set_pitch_gains(double kp, double ki, double kd);
};

/**
 * @brief 偏航阻尼器控制律类
 * @details 实现偏航阻尼器的控制逻辑
 */
class YawDamperControlLaw {
private:
    PIDController yaw_rate_controller; ///< 偏航角速度PID控制器
    
    bool active;                       ///< 激活状态
    
public:
    YawDamperControlLaw();
    
    // 控制方法
    void activate();
    void deactivate();
    bool is_active() const { return active; }
    
    // 控制律计算
    double calculate_yaw_damper_command(const FlightParameters& current, double dt);
    
    // 参数设置
    void set_gains(double kp, double ki, double kd);
};

/**
 * @brief B737自动飞行控制律主类
 * @details 整合所有自动飞行控制律，负责具体的控制执行
 */
class B737AutoFlightControlLaw {
private:
    std::unique_ptr<AutopilotControlLaw> autopilot;
    std::unique_ptr<AutothrottleControlLaw> autothrottle;
    std::unique_ptr<FlightDirectorControlLaw> flight_director;
    std::unique_ptr<YawDamperControlLaw> yaw_damper;
    
    ControlLawState state;
    FlightParameters current_params;
    TargetParameters target_params;
    
    double update_time;                ///< 更新时间间隔
    
    // 飞行员操作意图执行状态
    bool is_throttle_ramping {false};
    double throttle_ramp_start_time {0.0};
    double throttle_ramp_duration {2.0};
    double throttle_ramp_start_value {0.0};
    double throttle_ramp_target_value {1.0};
    
    bool is_speed_hold_active {false};
    double speed_hold_setpoint {5.0};
    double pid_kp {0.25};
    double pid_ki {0.03};
    double pid_kd {0.02};
    double pid_integral {0.0};
    double pid_prev_error {0.0};
    double last_tick_time {0.0};
    
public:
    B737AutoFlightControlLaw();
    ~B737AutoFlightControlLaw() = default;
    
    // 初始化方法
    void initialize();
    void reset();
    
    // 控制方法
    void engage_autopilot(FlightMode mode);
    void disengage_autopilot();
    void engage_autothrottle(FlightMode mode);
    void disengage_autothrottle();
    void activate_flight_director();
    void deactivate_flight_director();
    void activate_yaw_damper();
    void deactivate_yaw_damper();
    
    // 参数设置
    void set_flight_parameters(const FlightParameters& params);
    void set_target_parameters(const TargetParameters& params);
    void set_update_time(double dt) { update_time = dt; }
    
    // 控制律计算
    ControlLawState calculate_control_commands();
    
    // 状态查询
    const ControlLawState& get_state() const { return state; }
    const FlightParameters& get_current_parameters() const { return current_params; }
    const TargetParameters& get_target_parameters() const { return target_params; }
    
    // 配置方法
    void load_configuration(const std::string& config_file);
    void save_configuration(const std::string& config_file) const;
    
    // 验证方法
    bool validate_control_commands() const;
    std::string get_validation_report() const;
    
    // ==================== 飞行员操作意图执行接口 ====================
    // 飞机模型负责具体的控制执行算法
    
    /**
     * @brief 执行油门推至最大操作
     * @details 实现S曲线平滑控制算法
     */
    void executeThrottlePushToMax(double current_time);
    
    /**
     * @brief 执行刹车推至最大操作
     * @details 实现刹车控制算法
     */
    void executeBrakePushToMax(double current_time);
    
    /**
     * @brief 执行速度保持操作
     * @details 实现PID控制算法
     */
    void executeSpeedHold(double target_speed, double current_time);
    
    /**
     * @brief 停止速度保持操作
     */
    void stopSpeedHold();
    
    /**
     * @brief 每步调用，推进控制算法
     * @details 飞机模型负责具体的控制算法执行
     */
    void tick(double current_time);
    
    /**
     * @brief 获取当前油门S曲线状态
     */
    bool isThrottleRamping() const { return is_throttle_ramping; }
    
    /**
     * @brief 获取当前速度保持状态
     */
    bool isSpeedHoldActive() const { return is_speed_hold_active; }
};

} // namespace B737
} // namespace AircraftDigitalTwin
} // namespace SMF

#endif // B737_AUTO_FLIGHT_CONTROL_LAW_HPP 