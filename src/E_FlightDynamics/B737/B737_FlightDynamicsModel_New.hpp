/**
 * @file B737_FlightDynamicsModel_New.hpp
 * @brief B737飞行动力学模型
 * @details 实现B737机型的飞行动力学计算
 * @author VFT_SMF Framework
 * @date 2024
 */

#ifndef B737_FLIGHT_DYNAMICS_MODEL_NEW_HPP
#define B737_FLIGHT_DYNAMICS_MODEL_NEW_HPP

#include "../FlightDynamicsAgent.hpp"
#include "../../E_GlobalSharedDataSpace/GlobalSharedDataSpace.hpp"
#include "../../G_SimulationManager/LogAndData/Logger.hpp"
#include <string>
#include <cmath>

namespace VFT_SMF {
namespace FlightDynamics {

    /**
     * @brief B737飞行动力学模型
     * @details 实现B737机型的6分量外力计算
     */
    class B737FlightDynamicsModel : public IFlightDynamicsModel {
    private:
        // B737特定的输入状态
        struct B737InputState {
            // 控制输入
            double throttle_position;      ///< 油门位置 [0.0, 1.0]
            double elevator_deflection;    ///< 升降舵偏角 (度)
            double aileron_deflection;     ///< 副翼偏角 (度)
            double rudder_deflection;      ///< 方向舵偏角 (度)
            double flap_position;          ///< 襟翼位置 [0.0, 1.0]
            double landing_gear_position;  ///< 起落架位置 [0.0, 1.0]
            double brake_pressure;         ///< 刹车压力 [0.0, 1.0]
            
            // 环境输入
            double wind_speed;             ///< 风速 (m/s)
            double wind_direction;         ///< 风向 (度)
            double air_density;            ///< 空气密度 (kg/m³)
            double temperature;            ///< 温度 (K)
            
            B737InputState() : throttle_position(0.0), elevator_deflection(0.0),
                              aileron_deflection(0.0), rudder_deflection(0.0),
                              flap_position(0.0), landing_gear_position(1.0),
                              brake_pressure(0.0), wind_speed(0.0),
                              wind_direction(0.0), air_density(1.225),
                              temperature(288.15) {}
        };
        
        B737InputState current_input;
        VFT_SMF::GlobalSharedDataStruct::AircraftFlightState initial_state;
        AircraftPhysicsParams physics_params;
        // 最近一次计算的发动机相关量（用于外部读取/记录）
        double last_thrust {0.0};
        double last_engine_rpm {0.0};
        double last_fuel_flow {0.0};

    public:
        /**
         * @brief 构造函数
         */
        B737FlightDynamicsModel();
        
        /**
         * @brief 析构函数
         */
        ~B737FlightDynamicsModel() override = default;
        
        /**
         * @brief 计算6分量外力
         * @param current_state 当前飞机飞行状态
         * @return 6分量外力
         */
        SixAxisForces calculateForces(const VFT_SMF::GlobalSharedDataStruct::AircraftFlightState& current_state) override;
        
        /**
         * @brief 获取飞机物理参数
         * @return 飞机物理参数
         */
        AircraftPhysicsParams getPhysicsParams() const override;
        
        /**
         * @brief 获取模型名称
         * @return 模型名称
         */
        std::string getModelName() const override;
        
        /**
         * @brief 初始化模型
         * @param initial_state 初始状态
         */
        void initialize(const VFT_SMF::GlobalSharedDataStruct::AircraftFlightState& initial_state) override;
        
        /**
         * @brief 从全局共享数据空间状态更新模型输入
         * @param system_state 飞机系统状态
         * @param env_state 环境状态
         */
        void updateInputFromGlobalState(
            const VFT_SMF::GlobalSharedDataStruct::AircraftSystemState& system_state,
            const VFT_SMF::GlobalSharedDataStruct::EnvironmentGlobalState& env_state) override;

    private:
        /**
         * @brief 计算推力
         * @param current_state 当前状态
         * @return 推力 (N)
         */
        double calculateThrust(const VFT_SMF::GlobalSharedDataStruct::AircraftFlightState& current_state);
        
        /**
         * @brief 计算阻力
         * @param current_state 当前状态
         * @return 阻力 (N)
         */
        double calculateDrag(const VFT_SMF::GlobalSharedDataStruct::AircraftFlightState& current_state);
        
        /**
         * @brief 计算升力
         * @param current_state 当前状态
         * @return 升力 (N)
         */
        double calculateLift(const VFT_SMF::GlobalSharedDataStruct::AircraftFlightState& current_state);
        
        /**
         * @brief 计算侧力
         * @param current_state 当前状态
         * @return 侧力 (N)
         */
        double calculateSideForce(const VFT_SMF::GlobalSharedDataStruct::AircraftFlightState& current_state);
        
        /**
         * @brief 计算滚转力矩
         * @param current_state 当前状态
         * @return 滚转力矩 (N·m)
         */
        double calculateRollMoment(const VFT_SMF::GlobalSharedDataStruct::AircraftFlightState& current_state);
        
        /**
         * @brief 计算俯仰力矩
         * @param current_state 当前状态
         * @return 俯仰力矩 (N·m)
         */
        double calculatePitchMoment(const VFT_SMF::GlobalSharedDataStruct::AircraftFlightState& current_state);
        
        /**
         * @brief 计算偏航力矩
         * @param current_state 当前状态
         * @return 偏航力矩 (N·m)
         */
        double calculateYawMoment(const VFT_SMF::GlobalSharedDataStruct::AircraftFlightState& current_state);

    public:
        // 供外部查询的简化接口
        double getLastThrust() const { return last_thrust; }
        // 基于当前油门的简易估算（不建模N1/N2，单位RPM仅用于展示）
        double estimateEngineRpm() {
            const double idle_rpm = 4000.0;
            const double max_rpm = 10000.0;
            last_engine_rpm = idle_rpm + (max_rpm - idle_rpm) * std::clamp(current_input.throttle_position, 0.0, 1.0);
            return last_engine_rpm;
        }
        // 基于当前油门的简易估算（kg/h）
        double estimateFuelFlow() {
            const double idle_flow = 400.0;
            const double max_flow = 3200.0;
            last_fuel_flow = idle_flow + (max_flow - idle_flow) * std::clamp(current_input.throttle_position, 0.0, 1.0);
            return last_fuel_flow;
        }
    };

} // namespace FlightDynamics
} // namespace VFT_SMF

#endif // B737_FLIGHT_DYNAMICS_MODEL_NEW_HPP
