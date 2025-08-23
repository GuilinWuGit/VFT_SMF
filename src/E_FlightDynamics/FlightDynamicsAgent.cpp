/**
 * @file FlightDynamicsAgent.cpp
 * @brief 飞行动力学代理实现
 * @details 实现通用的飞行动力学计算（6分量外力→加速度→速度→位置）
 * @author VFT_SMF Framework
 * @date 2024
 */

#include "FlightDynamicsAgent.hpp"
#include "B737/B737_FlightDynamicsModel_New.hpp"
#include "../G_SimulationManager/LogAndData/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace VFT_SMF {
namespace FlightDynamics {

    // ==================== FlightDynamicsAgent 实现 ====================

    FlightDynamicsAgent::FlightDynamicsAgent(const std::string& aircraft_type)
        : current_aircraft_type(aircraft_type), gen(rd()), noise_dist(0.0, 0.1) {
        last_update_time = std::chrono::high_resolution_clock::now();
        
        // 创建机型模型
        aircraft_model = createAircraftModel(aircraft_type);
        if (aircraft_model) {
            physics_params = aircraft_model->getPhysicsParams();
            // 保持原始行为（不预计算矩阵），避免引入潜在差异
            VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, 
                               "飞行动力学代理已创建，机型: " + aircraft_type + 
                               ", 模型: " + aircraft_model->getModelName());
        } else {
            VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, 
                               "警告: 无法创建机型模型 " + aircraft_type + "，使用默认参数");
        }
    }

    void FlightDynamicsAgent::initialize(const VFT_SMF::GlobalSharedDataStruct::AircraftFlightState& initial_state) {
        std::lock_guard<std::mutex> lock(agent_mutex);
        
        current_state = initial_state;
        last_update_time = std::chrono::high_resolution_clock::now();
        
        if (aircraft_model) {
            aircraft_model->initialize(initial_state);
        }
        
        VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, 
                           "飞行动力学代理已初始化: 位置=(" + 
                           std::to_string(current_state.latitude) + ", " + 
                           std::to_string(current_state.longitude) + "), 高度=" + 
                           std::to_string(current_state.altitude) + "m, 航向=" + 
                           std::to_string(current_state.heading) + "°");
    }

    VFT_SMF::GlobalSharedDataStruct::AircraftFlightState FlightDynamicsAgent::update(double delta_time) {
        std::lock_guard<std::mutex> lock(agent_mutex);
        
        if (!aircraft_model) {
            VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "警告: 没有可用的机型模型");
            return current_state;
        }
        
        // 1. 计算6分量外力（缓存）
        SixAxisForces forces = aircraft_model->calculateForces(current_state);
        last_forces = forces;
        
        // 2. 计算加速度
        std::array<double, 6> accelerations = calculateAccelerations(forces);
        
        // 3. 更新速度
        updateVelocities(delta_time, accelerations);
        
        // 4. 更新位置和姿态
        updatePositionAndAttitude(delta_time);
        
        // 5. 更新时间戳
        last_update_time = std::chrono::high_resolution_clock::now();
        
        return current_state;
    }

        VFT_SMF::GlobalSharedDataStruct::AircraftFlightState FlightDynamicsAgent::updateFromGlobalState(double delta_time,
        const VFT_SMF::GlobalSharedDataStruct::AircraftSystemState& system_state,
        const VFT_SMF::GlobalSharedDataStruct::EnvironmentGlobalState& env_state) {
        if (!aircraft_model) {
            VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "警告: 没有可用的机型模型");
            return current_state;
        }
        
        // 更新机型模型的输入
        aircraft_model->updateInputFromGlobalState(system_state, env_state);
        
        // 计算六分量合外力（缓存）
        SixAxisForces forces = aircraft_model->calculateForces(current_state);
        last_forces = forces;
        
        // 创建六分量合外力数据结构
        VFT_SMF::GlobalSharedDataStruct::AircraftNetForce net_force;
        net_force.longitudinal_force = forces.force_x;
        net_force.lateral_force = forces.force_y;
        net_force.vertical_force = forces.force_z;
        net_force.roll_moment = forces.moment_x;
        net_force.pitch_moment = forces.moment_y;
        net_force.yaw_moment = forces.moment_z;
        
        // 从B737模型获取详细的力分解和发动机量（如果可用）
        if (auto b737_model = dynamic_cast<B737FlightDynamicsModel*>(aircraft_model.get())) {
            // 这里需要添加获取详细力分解的方法
            // 暂时使用简化计算
            net_force.thrust_force = forces.force_x > 0 ? forces.force_x : 0.0;  // 假设X轴正方向为推力
            net_force.drag_force = forces.force_x < 0 ? -forces.force_x : 0.0;   // 假设X轴负方向为阻力
            net_force.lift_force = forces.force_z > 0 ? forces.force_z : 0.0;    // 假设Z轴正方向为升力
            net_force.weight_force = -physics_params.mass * 9.81;                // 重力
            net_force.side_force = forces.force_y;                               // 侧力
            // 展示量写回调整：发动机量改写入系统状态，由系统记录器输出
            // 推力继续经由 aircraft_net_force.csv 体现
        } else {
            // 简化计算
            net_force.thrust_force = forces.force_x > 0 ? forces.force_x : 0.0;
            net_force.drag_force = forces.force_x < 0 ? -forces.force_x : 0.0;
            net_force.lift_force = forces.force_z > 0 ? forces.force_z : 0.0;
            net_force.weight_force = -physics_params.mass * 9.81;
            net_force.side_force = forces.force_y;
        }
        
        // 设置时间戳
        net_force.timestamp = VFT_SMF::SimulationTimePoint{};
        
        // 发布六分量合外力数据到共享数据空间
        // 注意：这里需要访问全局共享数据空间，暂时注释掉
        // VFT_SMF::globalSharedDataSpace.setAircraftNetForce(net_force);
        
        // 调用标准的update方法
        return update(delta_time);
    }

    VFT_SMF::GlobalSharedDataStruct::AircraftFlightState FlightDynamicsAgent::getCurrentState() const {
        std::lock_guard<std::mutex> lock(agent_mutex);
        return current_state;
    }

    std::string FlightDynamicsAgent::getAircraftType() const {
        std::lock_guard<std::mutex> lock(agent_mutex);
        return current_aircraft_type;
    }

    SixAxisForces FlightDynamicsAgent::getCurrentForces() const {
        std::lock_guard<std::mutex> lock(agent_mutex);
        return last_forces;
    }

    // ==================== 私有方法实现 ====================

    std::array<double, 6> FlightDynamicsAgent::calculateAccelerations(const SixAxisForces& forces) {
        std::array<double, 6> accelerations;
        
        // 线性加速度 (F = ma)
        accelerations[0] = forces.force_x / physics_params.mass; // X轴加速度
        accelerations[1] = forces.force_y / physics_params.mass; // Y轴加速度
        accelerations[2] = forces.force_z / physics_params.mass; // Z轴加速度
        
        // 角加速度 (M = Iα) - 恢复原先每步求逆的实现
        double det_I = physics_params.inertia_xx * physics_params.inertia_yy * physics_params.inertia_zz +
                       physics_params.inertia_xy * physics_params.inertia_yz * physics_params.inertia_xz +
                       physics_params.inertia_xz * physics_params.inertia_xy * physics_params.inertia_yz -
                       physics_params.inertia_xz * physics_params.inertia_yy * physics_params.inertia_xz -
                       physics_params.inertia_xy * physics_params.inertia_xy * physics_params.inertia_zz -
                       physics_params.inertia_xx * physics_params.inertia_yz * physics_params.inertia_yz;

        if (std::abs(det_I) < 1e-6) {
            // 简化处理：假设惯性积为0，使用对角惯性矩阵
            accelerations[3] = forces.moment_x / physics_params.inertia_xx; // 滚转角加速度
            accelerations[4] = forces.moment_y / physics_params.inertia_yy; // 俯仰角加速度
            accelerations[5] = forces.moment_z / physics_params.inertia_zz; // 偏航角加速度
        } else {
            // 使用完整的转动惯量矩阵计算角加速度
            // 计算转动惯量矩阵的逆矩阵
            double inv_Ixx = (physics_params.inertia_yy * physics_params.inertia_zz - 
                             physics_params.inertia_yz * physics_params.inertia_yz) / det_I;
            double inv_Iyy = (physics_params.inertia_xx * physics_params.inertia_zz - 
                             physics_params.inertia_xz * physics_params.inertia_xz) / det_I;
            double inv_Izz = (physics_params.inertia_xx * physics_params.inertia_yy - 
                             physics_params.inertia_xy * physics_params.inertia_xy) / det_I;
            double inv_Ixy = -(physics_params.inertia_xy * physics_params.inertia_zz - 
                              physics_params.inertia_xz * physics_params.inertia_yz) / det_I;
            double inv_Ixz = (physics_params.inertia_xy * physics_params.inertia_yz - 
                             physics_params.inertia_xz * physics_params.inertia_yy) / det_I;
            double inv_Iyz = -(physics_params.inertia_xx * physics_params.inertia_yz - 
                              physics_params.inertia_xz * physics_params.inertia_xy) / det_I;
            
            // 计算角加速度 α = [I]^(-1) * M
            accelerations[3] = inv_Ixx * forces.moment_x + inv_Ixy * forces.moment_y + inv_Ixz * forces.moment_z; // 滚转角加速度
            accelerations[4] = inv_Ixy * forces.moment_x + inv_Iyy * forces.moment_y + inv_Iyz * forces.moment_z; // 俯仰角加速度
            accelerations[5] = inv_Ixz * forces.moment_x + inv_Iyz * forces.moment_y + inv_Izz * forces.moment_z; // 偏航角加速度
            
            // 添加角加速度限制，防止异常值导致数值不稳定
            const double MAX_ANGULAR_ACCEL = 1000.0; // 最大角加速度限制 (rad/s²)
            for (int i = 3; i < 6; ++i) {
                if (std::abs(accelerations[i]) > MAX_ANGULAR_ACCEL) {
                    VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "角加速度数值异常: 轴" + std::to_string(i-2) + " 角加速度 " + std::to_string(accelerations[i]) + " 超过限制，已限制为 " + std::to_string(MAX_ANGULAR_ACCEL));
                    accelerations[i] = (accelerations[i] > 0) ? MAX_ANGULAR_ACCEL : -MAX_ANGULAR_ACCEL;
                }
            }
        }
        
        // 添加噪声
        for (int i = 0; i < 6; ++i) {
            accelerations[i] = addNoise(accelerations[i], 0.01);
        }
        
        return accelerations;
    }

    void FlightDynamicsAgent::updateVelocities(double delta_time, const std::array<double, 6>& accelerations) {
        // 更新线性速度
        current_state.longitudinal_accel = accelerations[0];
        current_state.lateral_accel = accelerations[1];
        current_state.vertical_accel = accelerations[2];
        
        // 更新空速（简化：假设空速主要受纵向加速度影响）
        current_state.airspeed += accelerations[0] * delta_time;
        current_state.airspeed = std::max(0.0, current_state.airspeed);
        
        // 更新垂直速度
        current_state.vertical_speed += accelerations[2] * delta_time;
        current_state.vertical_speed = std::max(-50.0, std::min(50.0, current_state.vertical_speed));
        
        // 计算地速（简化：假设地速等于空速）
        current_state.groundspeed = current_state.airspeed;
        current_state.groundspeed = std::max(0.0, current_state.groundspeed);
        
        // 更新角速度
        current_state.roll_rate = accelerations[3] * 180.0 / M_PI; // 转换为度/秒
        current_state.pitch_rate = accelerations[4] * 180.0 / M_PI;
        current_state.yaw_rate = accelerations[5] * 180.0 / M_PI;
        
        // 添加角速度限制，防止异常值导致数值不稳定
        const double MAX_ANGULAR_RATE = 360.0; // 最大角速度限制 (度/秒)
        if (std::abs(current_state.roll_rate) > MAX_ANGULAR_RATE) {
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "角速度数值异常: 滚转角速度 " + std::to_string(current_state.roll_rate) + " 超过限制，已限制为 " + std::to_string(MAX_ANGULAR_RATE));
            current_state.roll_rate = (current_state.roll_rate > 0) ? MAX_ANGULAR_RATE : -MAX_ANGULAR_RATE;
        }
        if (std::abs(current_state.pitch_rate) > MAX_ANGULAR_RATE) {
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "角速度数值异常: 俯仰角速度 " + std::to_string(current_state.pitch_rate) + " 超过限制，已限制为 " + std::to_string(MAX_ANGULAR_RATE));
            current_state.pitch_rate = (current_state.pitch_rate > 0) ? MAX_ANGULAR_RATE : -MAX_ANGULAR_RATE;
        }
        if (std::abs(current_state.yaw_rate) > MAX_ANGULAR_RATE) {
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "角速度数值异常: 偏航角速度 " + std::to_string(current_state.yaw_rate) + " 超过限制，已限制为 " + std::to_string(MAX_ANGULAR_RATE));
            current_state.yaw_rate = (current_state.yaw_rate > 0) ? MAX_ANGULAR_RATE : -MAX_ANGULAR_RATE;
        }
    }

    void FlightDynamicsAgent::updatePositionAndAttitude(double delta_time) {
        // 更新位置（平面地球模型）
        double earth_radius = 6371000.0; // 地球半径 (m)
        double lat_rad = current_state.latitude * M_PI / 180.0;
        
        // 更新纬度
        double lat_change = (current_state.groundspeed * cos(current_state.heading * M_PI / 180.0) * delta_time) / earth_radius;
        current_state.latitude += lat_change * 180.0 / M_PI;
        
        // 更新经度
        double lon_change = (current_state.groundspeed * sin(current_state.heading * M_PI / 180.0) * delta_time) / 
                           (earth_radius * cos(lat_rad));
        current_state.longitude += lon_change * 180.0 / M_PI;
        
        // 更新高度并进行地面钳制
        current_state.altitude += current_state.vertical_speed * delta_time;
        if (current_state.altitude <= 0.0) {
            current_state.altitude = 0.0;
            // 接地时避免持续向下速度导致数值渗透
            if (current_state.vertical_speed < 0.0) {
                current_state.vertical_speed = 0.0;
            }
        }
        
        // 更新姿态
        current_state.pitch += current_state.pitch_rate * delta_time;
        current_state.pitch = std::max(-30.0, std::min(30.0, current_state.pitch)); // 限制俯仰角
        
        current_state.roll += current_state.roll_rate * delta_time;
        current_state.roll = std::max(-60.0, std::min(60.0, current_state.roll)); // 限制滚转角
        
        current_state.heading += current_state.yaw_rate * delta_time;
        // 保持航向在0-360度范围内
        while (current_state.heading >= 360.0) current_state.heading -= 360.0;
        while (current_state.heading < 0.0) current_state.heading += 360.0;
    }

    double FlightDynamicsAgent::addNoise(double value, double noise_level) {
        return value + noise_dist(gen) * noise_level;
    }

    std::unique_ptr<IFlightDynamicsModel> FlightDynamicsAgent::createAircraftModel(const std::string& aircraft_type) {
        // 根据机型类型创建对应的模型
        if (aircraft_type == "B737") {
            return std::make_unique<B737FlightDynamicsModel>();
        }
        // 可以在这里添加更多机型的支持
        // else if (aircraft_type == "F16") {
        //     return std::make_unique<F16FlightDynamicsModel>();
        // }
        
        VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, 
                           "错误: 未找到机型模型 " + aircraft_type + "，使用默认B737模型");
        return std::make_unique<B737FlightDynamicsModel>();
    }

} // namespace FlightDynamics
} // namespace VFT_SMF
