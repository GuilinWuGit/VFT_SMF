/**
 * @file B737_FlightDynamicsModel_New.cpp
 * @brief B737飞行动力学模型实现
 * @details 实现B737机型的6分量外力计算
 * @author VFT_SMF Framework
 * @date 2024
 */

#include "B737_FlightDynamicsModel_New.hpp"
#include "../../G_SimulationManager/LogAndData/Logger.hpp"
#include <cmath>
#include <algorithm>

namespace VFT_SMF {
namespace FlightDynamics {

    // B737特定参数
    static constexpr double B737_MAX_AIRSPEED = 250.0;        ///< B737最大空速 (m/s)
    static constexpr double B737_MAX_ALTITUDE = 12500.0;      ///< B737最大高度 (m)
    static constexpr double B737_MAX_THRUST = 120000.0;       ///< B737最大推力 (N)
    static constexpr double B737_EMPTY_WEIGHT = 45000.0;      ///< B737空重 (kg)
    static constexpr double B737_MAX_TAKEOFF_WEIGHT = 78000.0; ///< B737最大起飞重量 (kg)
    static constexpr double B737_WING_AREA = 125.0;           ///< B737机翼面积 (m²)
    static constexpr double B737_WING_SPAN = 35.0;            ///< B737翼展 (m)
    static constexpr double B737_MAC = 3.5;                   ///< B737平均气动弦长 (m)

    // ==================== B737FlightDynamicsModel 实现 ====================

    B737FlightDynamicsModel::B737FlightDynamicsModel() {
        // 初始化B737物理参数
        physics_params.mass = B737_EMPTY_WEIGHT;
        
        // 设置完整的6分量转动惯量矩阵 (kg·m²)
        physics_params.inertia_xx = 100000.0;  // 滚转惯量
        physics_params.inertia_yy = 200000.0;  // 俯仰惯量
        physics_params.inertia_zz = 300000.0;  // 偏航惯量
        physics_params.inertia_xy = 0.0;       // XY惯性积（通常为0）
        physics_params.inertia_xz = 0.0;       // XZ惯性积（通常为0）
        physics_params.inertia_yz = 0.0;       // YZ惯性积（通常为0）
        
        VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "B737飞行动力学模型已创建");
    }

    SixAxisForces B737FlightDynamicsModel::calculateForces(const VFT_SMF::GlobalSharedDataStruct::AircraftFlightState& current_state) {
        SixAxisForces forces;
        
        // 计算各个分量
        forces.force_x = calculateThrust(current_state) - calculateDrag(current_state);
        forces.force_y = calculateSideForce(current_state);
        
        // 升力与重力（向上为正）
        double lift = calculateLift(current_state);
        double gravity = physics_params.mass * 9.81;
        forces.force_z = lift - gravity; // 升力 - 重力
        
        // 地面支反力（法向力）：当接地时抵消向下合力，并加入速度阻尼，避免垂直速度持续向下
        // 跑道高度简化为0
        const double runway_elevation = 0.0;
        if (current_state.altitude <= runway_elevation + 1e-6) {
            // 线性阻尼项：仅对向下速度施加阻尼，使N随下冲速度增大
            const double damping_coefficient = 5e5; // N·s/m（可调）
            const double v_down = std::min(0.0, current_state.vertical_speed); // 向下为负
            const double damping = damping_coefficient * (-v_down);
            // 反力基值：抵消向下合力的负值部分，确保不把机体“吸”到地里
            const double normal_base = std::max(0.0, -forces.force_z);
            const double normal_force = normal_base + damping;
            forces.force_z += normal_force;

            // 跑道滚阻与制动纵向摩擦（基于法向力）
            // 取地面切向速度近似为地速（airspeed）
            const double vx = current_state.airspeed;
            const double sgn_vx = (vx > 1e-3) ? 1.0 : ((vx < -1e-3) ? -1.0 : 0.0);
            // 滚动阻力
            const double c_rr = 0.02; // 经验滚阻系数
            const double F_rr = c_rr * normal_force * sgn_vx;
            // 制动摩擦：环境摩擦系数 * 刹车比例 * N
            // 直接使用环境摩擦与刹车比例：
            const double env_mu = 0.2; // 如需从环境状态读取，可通过 updateInputFromGlobalState 传入
            const double brake_ratio = std::max(0.0, std::min(1.0, current_input.brake_pressure)); // 0..1
            const double F_brk = env_mu * brake_ratio * normal_force * sgn_vx;
            // 将滚阻与刹车力作用到纵向
            forces.force_x -= (F_rr + F_brk);
        }
        
        forces.moment_x = calculateRollMoment(current_state);
        forces.moment_y = calculatePitchMoment(current_state);
        forces.moment_z = calculateYawMoment(current_state);
        
        return forces;
    }

    AircraftPhysicsParams B737FlightDynamicsModel::getPhysicsParams() const {
        return physics_params;
    }

    std::string B737FlightDynamicsModel::getModelName() const {
        return "B737 Flight Dynamics Model";
    }

    void B737FlightDynamicsModel::initialize(const VFT_SMF::GlobalSharedDataStruct::AircraftFlightState& initial_state) {
        this->initial_state = initial_state;
        VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, 
                           "B737飞行动力学模型已初始化: 位置=(" + 
                           std::to_string(initial_state.latitude) + ", " + 
                           std::to_string(initial_state.longitude) + ")");
    }

    void B737FlightDynamicsModel::updateInputFromGlobalState(
        const VFT_SMF::GlobalSharedDataStruct::AircraftSystemState& system_state,
        const VFT_SMF::GlobalSharedDataStruct::EnvironmentGlobalState& env_state) {
        
        // 从飞机系统状态更新控制输入
        current_input.throttle_position = system_state.current_throttle_position;
        current_input.elevator_deflection = system_state.current_elevator_deflection;
        current_input.aileron_deflection = system_state.current_aileron_deflection;
        current_input.rudder_deflection = system_state.current_rudder_deflection;
        current_input.flap_position = system_state.current_flaps_deployed / 50.0; // 转换为0-1范围
        current_input.landing_gear_position = system_state.current_landing_gear_deployed;
        current_input.brake_pressure = system_state.current_brake_pressure / 1000000.0; // 转换为0-1范围
        
        // 从环境状态更新环境输入
        current_input.wind_speed = env_state.wind_speed;
        current_input.wind_direction = env_state.wind_direction;
        current_input.air_density = env_state.air_density;
        current_input.temperature = 288.15; // 标准大气温度
    }

    // ==================== 私有方法实现 ====================

    double B737FlightDynamicsModel::calculateThrust(const VFT_SMF::GlobalSharedDataStruct::AircraftFlightState& current_state) {
        // 基于油门位置和高度计算推力
        double max_thrust = B737_MAX_THRUST;
        
        // 高度影响（高度越高，推力越小）
        double altitude_factor = 1.0 - (current_state.altitude / B737_MAX_ALTITUDE) * 0.3;
        altitude_factor = std::max(0.7, altitude_factor); // 最小推力为70%
        
        // 空速影响（空速越高，推力略有下降）
        double speed_factor = 1.0 - (current_state.airspeed / B737_MAX_AIRSPEED) * 0.1;
        speed_factor = std::max(0.9, speed_factor); // 最小推力为90%
        
        double thrust = max_thrust * current_input.throttle_position * altitude_factor * speed_factor;
        thrust = std::max(0.0, thrust);
        last_thrust = thrust;
        // 同步估算转速与油耗，便于记录
        estimateEngineRpm();
        estimateFuelFlow();
        return thrust;
    }

    double B737FlightDynamicsModel::calculateDrag(const VFT_SMF::GlobalSharedDataStruct::AircraftFlightState& current_state) {
        // 计算动态压力
        double dynamic_pressure = 0.5 * current_input.air_density * current_state.airspeed * current_state.airspeed;
        
        // 基础阻力系数
        double cd0 = 0.02; // 零升阻力系数
        
        // 起落架阻力
        double gear_drag = current_input.landing_gear_position * 0.01 * dynamic_pressure * B737_WING_AREA;
        
        // 襟翼阻力
        double flap_drag = current_input.flap_position * 0.02 * dynamic_pressure * B737_WING_AREA;
        
        // 总阻力
        double drag = cd0 * dynamic_pressure * B737_WING_AREA + gear_drag + flap_drag;
        
        return std::max(0.0, drag);
    }

    double B737FlightDynamicsModel::calculateLift(const VFT_SMF::GlobalSharedDataStruct::AircraftFlightState& current_state) {
        // 计算动态压力
        double dynamic_pressure = 0.5 * current_input.air_density * current_state.airspeed * current_state.airspeed;
        
        // 升力系数（简化模型）
        double cl0 = 0.3; // 零攻角升力系数
        double cl_alpha = 5.0; // 升力曲线斜率
        
        // 攻角（简化：基于俯仰角）
        double alpha = current_state.pitch * M_PI / 180.0; // 转换为弧度
        
        // 襟翼影响
        double flap_factor = 1.0 + current_input.flap_position * 0.5; // 襟翼增加升力
        
        // 总升力系数
        double cl = (cl0 + cl_alpha * alpha) * flap_factor;
        
        // 升力
        double lift = cl * dynamic_pressure * B737_WING_AREA;
        
        return std::max(0.0, lift);
    }

    double B737FlightDynamicsModel::calculateSideForce(const VFT_SMF::GlobalSharedDataStruct::AircraftFlightState& current_state) {
        // 计算动态压力
        double dynamic_pressure = 0.5 * current_input.air_density * current_state.airspeed * current_state.airspeed;
        
        // 侧滑角（简化：基于滚转角）
        double beta = current_state.roll * M_PI / 180.0; // 转换为弧度
        
        // 方向舵影响
        double rudder_factor = current_input.rudder_deflection * 0.1; // 方向舵偏角影响
        
        // 侧力系数
        double cy = -0.1 * beta + rudder_factor;
        
        // 侧力
        double side_force = cy * dynamic_pressure * B737_WING_AREA;
        
        return side_force;
    }

    double B737FlightDynamicsModel::calculateRollMoment(const VFT_SMF::GlobalSharedDataStruct::AircraftFlightState& current_state) {
        // 计算动态压力
        double dynamic_pressure = 0.5 * current_input.air_density * current_state.airspeed * current_state.airspeed;
        
        // 滚转角速度影响
        double roll_rate_factor = current_state.roll_rate * M_PI / 180.0; // 转换为弧度/秒
        
        // 副翼影响
        double aileron_factor = current_input.aileron_deflection * 0.05; // 副翼偏角影响
        
        // 滚转力矩系数
        double cl_moment = -0.1 * roll_rate_factor + aileron_factor;
        
        // 滚转力矩
        double roll_moment = cl_moment * dynamic_pressure * B737_WING_AREA * B737_WING_SPAN;
        
        // 添加数值限制，防止异常值导致计算复杂度爆炸
        const double MAX_MOMENT = 1e6; // 最大力矩限制 (N·m)
        if (std::abs(roll_moment) > MAX_MOMENT) {
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "力矩数值异常: 滚转力矩 " + std::to_string(roll_moment) + " 超过限制，已限制为 " + std::to_string(MAX_MOMENT));
            roll_moment = (roll_moment > 0) ? MAX_MOMENT : -MAX_MOMENT;
        }
        
        return roll_moment;
    }

    double B737FlightDynamicsModel::calculatePitchMoment(const VFT_SMF::GlobalSharedDataStruct::AircraftFlightState& current_state) {
        // 计算动态压力
        double dynamic_pressure = 0.5 * current_input.air_density * current_state.airspeed * current_state.airspeed;
        
        // 攻角（基于俯仰角）
        double alpha = current_state.pitch * M_PI / 180.0; // 转换为弧度
        
        // 俯仰角速度影响
        double pitch_rate_factor = current_state.pitch_rate * M_PI / 180.0; // 转换为弧度/秒
        
        // 升降舵影响
        double elevator_factor = current_input.elevator_deflection * 0.1; // 升降舵偏角影响
        
        // 俯仰力矩系数
        double cm = -0.2 * alpha - 0.1 * pitch_rate_factor + elevator_factor;
        
        // 俯仰力矩
        double pitch_moment = cm * dynamic_pressure * B737_WING_AREA * B737_MAC;
        
        // 添加数值限制，防止异常值导致计算复杂度爆炸
        const double MAX_MOMENT = 1e6; // 最大力矩限制 (N·m)
        if (std::abs(pitch_moment) > MAX_MOMENT) {
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "力矩数值异常: 俯仰力矩 " + std::to_string(pitch_moment) + " 超过限制，已限制为 " + std::to_string(MAX_MOMENT));
            pitch_moment = (pitch_moment > 0) ? MAX_MOMENT : -MAX_MOMENT;
        }
        
        return pitch_moment;
    }

        double B737FlightDynamicsModel::calculateYawMoment(const VFT_SMF::GlobalSharedDataStruct::AircraftFlightState& current_state) {
        // 计算动态压力
        double dynamic_pressure = 0.5 * current_input.air_density * current_state.airspeed * current_state.airspeed;
        
        // 侧滑角（基于滚转角）
        double beta = current_state.roll * M_PI / 180.0; // 转换为弧度
        
        // 偏航角速度影响
        double yaw_rate_factor = current_state.yaw_rate * M_PI / 180.0; // 转换为弧度/秒
        
        // 方向舵影响
        double rudder_factor = current_input.rudder_deflection * 0.05; // 方向舵偏角影响
        
        // 偏航力矩系数
        double cn = 0.1 * beta - 0.1 * yaw_rate_factor + rudder_factor;
        
        // 偏航力矩
        double yaw_moment = cn * dynamic_pressure * B737_WING_AREA * B737_WING_SPAN;
        
        // 添加数值限制，防止异常值导致计算复杂度爆炸
        const double MAX_MOMENT = 1e6; // 最大力矩限制 (N·m)
        if (std::abs(yaw_moment) > MAX_MOMENT) {
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "力矩数值异常: 偏航力矩 " + std::to_string(yaw_moment) + " 超过限制，已限制为 " + std::to_string(MAX_MOMENT));
            yaw_moment = (yaw_moment > 0) ? MAX_MOMENT : -MAX_MOMENT;
        }
        
        return yaw_moment;
    }

} // namespace FlightDynamics
} // namespace VFT_SMF
