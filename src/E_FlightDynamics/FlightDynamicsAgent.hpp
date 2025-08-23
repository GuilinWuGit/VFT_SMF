/**
 * @file FlightDynamicsAgent.hpp
 * @brief 飞行动力学代理标准化接口
 * @details 实现通用的飞行动力学计算（6分量外力→加速度→速度→位置），
 *          支持B737等具体机型模型
 * @author VFT_SMF Framework
 * @date 2024
 */

#ifndef FLIGHT_DYNAMICS_AGENT_HPP
#define FLIGHT_DYNAMICS_AGENT_HPP

#include <string>
#include <memory>
#include <chrono>
#include <cmath>
#include <random>
#include <mutex>
#include <algorithm>
#include "../E_GlobalSharedDataSpace/GlobalSharedDataSpace.hpp"
#include "../G_SimulationManager/B_SimManage/SimulationNameSpace.hpp"
#include "../G_SimulationManager/LogAndData/Logger.hpp"

// 定义M_PI（Windows上可能未定义）
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace VFT_SMF {
namespace FlightDynamics {

    /**
     * @brief 6分量外力结构体
     * @details 定义作用在飞机上的6个自由度的外力
     */
    struct SixAxisForces {
        // 线性力 (N)
        double force_x;        ///< X轴力（前后方向）
        double force_y;        ///< Y轴力（左右方向）
        double force_z;        ///< Z轴力（上下方向）
        
        // 力矩 (N·m)
        double moment_x;       ///< X轴力矩（滚转）
        double moment_y;       ///< Y轴力矩（俯仰）
        double moment_z;       ///< Z轴力矩（偏航）
        
        SixAxisForces() : force_x(0.0), force_y(0.0), force_z(0.0),
                         moment_x(0.0), moment_y(0.0), moment_z(0.0) {}
        
        SixAxisForces(double fx, double fy, double fz, 
                     double mx, double my, double mz)
            : force_x(fx), force_y(fy), force_z(fz),
              moment_x(mx), moment_y(my), moment_z(mz) {}
    };



    /**
     * @brief 飞机物理参数结构体
     * @details 定义飞机的基本物理参数，用于飞行动力学计算
     */
    struct AircraftPhysicsParams {
        double mass;                   ///< 质量 (kg)
        
        // 转动惯量矩阵的6个分量 (kg·m²)
        double inertia_xx;             ///< X轴转动惯量
        double inertia_yy;             ///< Y轴转动惯量  
        double inertia_zz;             ///< Z轴转动惯量
        double inertia_xy;             ///< XY轴惯性积
        double inertia_xz;             ///< XZ轴惯性积
        double inertia_yz;             ///< YZ轴惯性积
        
        AircraftPhysicsParams() : mass(45000.0), 
                                 inertia_xx(100000.0), inertia_yy(200000.0), inertia_zz(300000.0),
                                 inertia_xy(0.0), inertia_xz(0.0), inertia_yz(0.0) {}
    };

    /**
     * @brief 飞行动力学模型接口
     * @details 定义具体机型飞行动力学模型必须实现的接口
     */
    class IFlightDynamicsModel {
    public:
        virtual ~IFlightDynamicsModel() = default;
        
        /**
         * @brief 计算6分量外力
         * @param current_state 当前飞机飞行状态
         * @return 6分量外力
         */
        virtual SixAxisForces calculateForces(const VFT_SMF::GlobalSharedDataStruct::AircraftFlightState& current_state) = 0;
        
        /**
         * @brief 获取飞机物理参数
         * @return 飞机物理参数
         */
        virtual AircraftPhysicsParams getPhysicsParams() const = 0;
        
        /**
         * @brief 获取模型名称
         * @return 模型名称
         */
        virtual std::string getModelName() const = 0;
        
        /**
         * @brief 初始化模型
         * @param initial_state 初始状态
         */
        virtual void initialize(const VFT_SMF::GlobalSharedDataStruct::AircraftFlightState& initial_state) = 0;
        
        /**
         * @brief 从全局共享数据空间状态更新模型输入
         * @param system_state 飞机系统状态
         * @param env_state 环境状态
         */
        virtual void updateInputFromGlobalState(
            const VFT_SMF::GlobalSharedDataStruct::AircraftSystemState& system_state,
            const VFT_SMF::GlobalSharedDataStruct::EnvironmentGlobalState& env_state) = 0;
    };

    /**
     * @brief 飞行动力学代理类
     * @details 实现通用的飞行动力学计算，管理具体机型模型
     */
    class FlightDynamicsAgent {
    private:
        // 当前状态
        VFT_SMF::GlobalSharedDataStruct::AircraftFlightState current_state;
        
        // 物理参数
        AircraftPhysicsParams physics_params;
        // 预计算的惯量逆矩阵（仅用于对角矩阵场景）
        struct InverseInertiaMatrix {
            double inv_xx;
            double inv_yy;
            double inv_zz;
            bool diagonal_only; // 若为 true，表示惯量积为0，可用对角逆
            InverseInertiaMatrix() : inv_xx(0.0), inv_yy(0.0), inv_zz(0.0), diagonal_only(true) {}
        } inverse_inertia;
        
        // 机型模型
        std::unique_ptr<IFlightDynamicsModel> aircraft_model;
        std::string current_aircraft_type;
        
        // 时间管理
        std::chrono::high_resolution_clock::time_point last_update_time;
        
        // 线程安全
        mutable std::mutex agent_mutex;
        
        // 随机数生成器（用于模拟真实飞行中的微小扰动）
        std::random_device rd;
        std::mt19937 gen;
        std::normal_distribution<double> noise_dist;
        // 缓存上一帧计算的外力，避免重复计算
        SixAxisForces last_forces;

    public:
        /**
         * @brief 构造函数
         * @param aircraft_type 飞机类型
         */
        FlightDynamicsAgent(const std::string& aircraft_type = "B737");
        
        /**
         * @brief 析构函数
         */
        ~FlightDynamicsAgent() = default;
        
        /**
         * @brief 初始化代理
         * @param initial_state 初始状态
         */
        void initialize(const VFT_SMF::GlobalSharedDataStruct::AircraftFlightState& initial_state);
        
        /**
         * @brief 更新飞机飞行状态
         * @param delta_time 时间步长 (秒)
         * @return 更新后的状态
         */
        VFT_SMF::GlobalSharedDataStruct::AircraftFlightState update(double delta_time);
        
        /**
         * @brief 从全局共享数据空间更新飞机飞行状态
         * @param delta_time 时间步长 (秒)
         * @param system_state 飞机系统状态
         * @param env_state 环境状态
         * @return 更新后的状态
         */
            VFT_SMF::GlobalSharedDataStruct::AircraftFlightState updateFromGlobalState(double delta_time,
            const VFT_SMF::GlobalSharedDataStruct::AircraftSystemState& system_state,
            const VFT_SMF::GlobalSharedDataStruct::EnvironmentGlobalState& env_state);
        
        /**
         * @brief 获取当前状态
         * @return 当前飞机飞行状态
         */
        VFT_SMF::GlobalSharedDataStruct::AircraftFlightState getCurrentState() const;
        
        /**
         * @brief 获取当前飞机类型
         * @return 飞机类型
         */
        std::string getAircraftType() const;
        
        /**
         * @brief 获取当前六分量合外力
         * @return 六分量合外力
         */
        SixAxisForces getCurrentForces() const;
        


    private:
        /**
         * @brief 计算加速度
         * @param forces 6分量外力
         * @return 6分量加速度（3个线性加速度 + 3个角加速度）
         */
        std::array<double, 6> calculateAccelerations(const SixAxisForces& forces);
        
        /**
         * @brief 更新速度
         * @param delta_time 时间步长
         * @param accelerations 加速度
         */
        void updateVelocities(double delta_time, const std::array<double, 6>& accelerations);
        
        /**
         * @brief 更新位置和姿态
         * @param delta_time 时间步长
         */
        void updatePositionAndAttitude(double delta_time);
        
        /**
         * @brief 添加噪声
         * @param value 原始值
         * @param noise_level 噪声水平
         * @return 添加噪声后的值
         */
        double addNoise(double value, double noise_level);
        
        /**
         * @brief 创建机型模型
         * @param aircraft_type 飞机类型
         * @return 机型模型指针
         */
        std::unique_ptr<IFlightDynamicsModel> createAircraftModel(const std::string& aircraft_type);
    };

} // namespace FlightDynamics
} // namespace VFT_SMF

#endif // FLIGHT_DYNAMICS_AGENT_HPP
