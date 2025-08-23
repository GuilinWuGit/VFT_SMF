/**
 * @file Simulation_Clock.hpp
 * @brief 仿真时钟管理系统
 * @author VFT_SMF Development Team
 * @date 2024
 * 
 * 仿真时钟管理系统负责：
 * 1. 管理仿真时间的推进和同步
 * 2. 支持多种时间模式（实时、快速、慢速、批处理等）
 * 3. 提供时间同步和校正机制
 * 4. 监控仿真性能和帧率
 * 5. 支持基于仿真步的时间同步
 */

#pragma once

#include "../B_SimManage/SimulationNameSpace.hpp"
#include "../../E_GlobalSharedDataSpace/GlobalSharedDataSpace.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <vector>
#include <functional>
#include <chrono>

namespace VFT_SMF {

    /**
     * @brief 仿真时钟类
     * 
     * 提供完整的仿真时间管理功能，包括：
     * - 多种时间模式支持
     * - 时间同步和校正
     * - 性能监控
     * - 基于仿真步的时间管理
     */
    class SimulationClock {
    private:
        SimulationConfig config;              ///< 仿真配置
        std::atomic<double> current_simulation_time;  ///< 当前仿真时间
        std::atomic<uint64_t> current_step;           ///< 当前仿真步数
        std::chrono::high_resolution_clock::time_point start_time;  ///< 开始时间
        std::chrono::high_resolution_clock::time_point last_update_time;  ///< 上次更新时间
        
        // 运行状态
        std::atomic<bool> is_running;         ///< 是否运行
        std::atomic<bool> is_paused;          ///< 是否暂停
        SimulationMode current_mode;          ///< 当前模式
        
        // 时间管理
        std::chrono::system_clock::time_point start_real_time; ///< 开始真实时间
        std::atomic<uint64_t> current_frame;  ///< 当前帧数
        
        // 统计信息结构已移除
        
        TimeUpdateCallback time_update_callback;      ///< 时间更新回调
        
        mutable std::mutex clock_mutex;       ///< 时钟互斥锁
        
     
        
    public:
        /**
         * @brief 构造函数
         * @param config 仿真配置
         */
        explicit SimulationClock(const SimulationConfig& config);
        
        /**
         * @brief 析构函数
         */
        ~SimulationClock() = default;
        
        // ==================== 配置管理 ====================
        
        /**
         * @brief 设置仿真模式
         * @param mode 仿真模式
         */
        void set_time_mode(SimulationMode mode);
        
        /**
         * @brief 获取仿真模式
         * @return 仿真模式
         */
        SimulationMode get_time_mode() const;
        
        /**
         * @brief 设置时间缩放因子
         * @param scale 时间缩放因子
         */
        void set_time_scale(double scale);
        
        /**
         * @brief 获取时间缩放因子
         * @return 时间缩放因子
         */
        double get_time_scale() const;
        
        /**
         * @brief 设置时间步长
         * @param step 时间步长（秒）
         */
        void set_time_step(double step);
        
        /**
         * @brief 获取时间步长
         * @return 时间步长（秒）
         */
        double get_time_step() const;
        
        /**
         * @brief 设置步时间增量
         * @param increment 步时间增量（秒）
         */
        void set_step_time_increment(double increment);
        
        /**
         * @brief 获取步时间增量
         * @return 步时间增量（秒）
         */
        double get_step_time_increment() const;
        
        /**
         * @brief 设置同步策略
         * @param strategy 同步策略
         */
        void set_sync_strategy(TimeSyncStrategy strategy);
        
        /**
         * @brief 获取同步策略
         * @return 同步策略
         */
        TimeSyncStrategy get_sync_strategy() const;
        
        // ==================== 基本时间管理 ====================
        
        /**
         * @brief 启动仿真时钟（并同步共享数据空间的运行状态）
         * @param shared_data_space 共享数据空间指针
         */
        void start(std::shared_ptr<GlobalShared_DataSpace::GlobalSharedDataSpace> shared_data_space);
        
        /**
         * @brief 停止仿真时钟（并同步共享数据空间的运行状态）
         * @param shared_data_space 共享数据空间指针
         */
        void stop(std::shared_ptr<GlobalShared_DataSpace::GlobalSharedDataSpace> shared_data_space);
        
        /**
         * @brief 暂停仿真时钟
         */
        void pause();
        
        /**
         * @brief 恢复仿真时钟
         */
        void resume();
        
        /**
         * @brief 重置仿真时钟
         */
        void reset();
        
        /**
         * @brief 更新仿真时钟
         * @param delta_sim_time 仿真时间步增量（固定步长模式下将忽略此值，使用config.time_step）
         */
        void update(double delta_sim_time);
        
        /**
         * @brief 更新仿真时钟（带线程同步）
         * @param delta_sim_time 仿真时间步增量（固定步长模式下将忽略此值，使用config.time_step）
         * @param shared_data_space 共享数据空间指针
         */
        void update(double delta_sim_time, std::shared_ptr<GlobalShared_DataSpace::GlobalSharedDataSpace> shared_data_space);
        
        /**
         * @brief 步进仿真时钟
         * @param steps 步进数量
         */
        void step(uint64_t steps = 1);
        
        // ==================== 时间查询 ====================
        
        /**
         * @brief 获取当前仿真时间
         * @return 当前仿真时间（秒）
         */
        double get_current_simulation_time() const;
        
        /**
         * @brief 获取当前仿真步数
         * @return 当前仿真步数
         */
        uint64_t get_current_step() const;
        
        /**
         * @brief 获取当前仿真时间点
         * @return 仿真时间点
         */
        SimulationTimePoint get_current_simulation_time_point() const;
        
        /**
         * @brief 获取仿真持续时间
         * @return 仿真持续时间
         */
        SimulationDuration get_simulation_duration() const;
        
        /**
         * @brief 获取真实时间持续时间
         * @return 真实时间持续时间（秒）
         */
        double get_real_time_duration() const;
        
        /**
         * @brief 获取仿真时间缩放因子
         * @return 时间缩放因子
         */
        double get_simulation_time_scale() const;
        
        /**
         * @brief 获取仿真时间步长
         * @return 时间步长（秒）
         */
        double get_simulation_time_step() const;
        
        // ==================== 同步管理 ====================
        
        
        
        // ==================== 性能监控 ====================
        
        // FPS/统计接口已移除
        
        // ==================== 回调函数设置 ====================
        
        /**
         * @brief 设置时间更新回调函数
         * @param callback 回调函数
         */
        void set_time_update_callback(TimeUpdateCallback callback);
        
        
        
        // ==================== 状态查询 ====================
        
        /**
         * @brief 检查时钟是否运行
         * @return 是否运行
         */
        bool is_clock_running() const;
        
        /**
         * @brief 检查时钟是否暂停
         * @return 是否暂停
         */
        bool is_clock_paused() const;
        
        /**
         * @brief 获取时钟配置
         * @return 时钟配置
         */
        SimulationConfig get_config() const;
        
        /**
         * @brief 设置时钟配置
         * @param new_config 新配置
         */
        void set_config(const SimulationConfig& new_config);
        
        // ==================== 工具方法 ====================
        
        /**
         * @brief 将时间点转换为字符串
         * @param time_point 时间点
         * @return 字符串表示
         */
        static std::string time_point_to_string(const SimulationTimePoint& time_point);
        
        /**
         * @brief 将持续时间转换为字符串
         * @param duration 持续时间
         * @return 字符串表示
         */
        static std::string duration_to_string(const SimulationDuration& duration);
        
        /**
         * @brief 仿真时间转换为真实时间
         * @param sim_time 仿真时间
         * @return 真实时间
         */
        double simulation_time_to_real_time(double sim_time) const;
        
        /**
         * @brief 真实时间转换为仿真时间
         * @param real_time 真实时间
         * @return 仿真时间
         */
        double real_time_to_simulation_time(double real_time) const;
        
        /**
         * @brief 生成状态报告
         * @return 状态报告字符串
         */
        std::string generate_status_report() const;
        
        // 生成性能报告接口已移除
        
    private:
        // ==================== 私有辅助方法 ====================
       
        /**
         * @brief 限制时间步长
         * @param step 时间步长
         * @return 限制后的时间步长
         */
        double clamp_time_step(double step) const;
    };

} // namespace VFT_SMF 