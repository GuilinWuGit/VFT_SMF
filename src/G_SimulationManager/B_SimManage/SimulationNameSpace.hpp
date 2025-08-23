/**
 * @file SimulationNameSpace.hpp
 * @brief 仿真系统命名空间和类型定义
 * @author VFT_SMF Development Team
 * @date 2024
 * 
 * 统一定义VFT_SMF仿真系统所需的核心枚举、结构体、类型别名等
 * 只保留实际被使用的定义，避免冗余
 */

#pragma once

#include <string>
#include <map>
#include <vector>
#include <chrono>
#include <atomic>
#include <memory>
#include <functional>

namespace VFT_SMF {

    // ==================== 仿真时间相关定义 ====================
    
    /**
     * @brief 时间同步策略枚举
     */
    enum class TimeSyncStrategy {
        STRICT_SYNC,       ///< 严格同步 - 确保仿真时间与真实时间严格对应，用于硬件在环的硬实时仿真
        STEP_BASED_SYNC    ///< 基于仿真步的同步 - 每个仿真步对应固定的时间增量，确保仿真时间的一致性
    };

    /**
     * @brief 仿真时间点结构体
     */
    struct SimulationTimePoint {
        uint64_t step_number;          ///< 仿真步数，第0步为初始状态
        double simulation_time;        ///< 仿真时间（秒）
        std::chrono::system_clock::time_point real_time;  ///< 对应的真实时间   
        
        SimulationTimePoint() : step_number(0), simulation_time(0.0), 
                               real_time(std::chrono::system_clock::now()) {}
        
        SimulationTimePoint(double sim_sec, uint64_t step = 0) 
            : step_number(step), simulation_time(sim_sec), 
              real_time(std::chrono::system_clock::now()) {}
    };

    /**
     * @brief 仿真持续时间结构体
     */
    struct SimulationDuration {
        double duration;               ///< 持续时间（秒）
        uint64_t frames;               ///< 帧数
        
        SimulationDuration() : duration(0.0), frames(0) {}
        SimulationDuration(double sec, uint64_t frame_count = 0) 
            : duration(sec), frames(frame_count) {}
    };

    // ==================== 仿真管理器相关定义 ====================
    
    /**
     * @brief 仿真模式枚举（统一的时间模式和仿真模式）
     */
    enum class SimulationMode {
        SCALE_TIME,        ///< 缩放时间模式 - 使用time_scale控制仿真速度
        STEP_BY_STEP,      ///< 逐步模式 - 手动控制时间推进
        PAUSED             ///< 暂停模式 - 时间停止
    };

    /**
     * @brief 仿真配置结构体（统一配置）
     */
    struct SimulationConfig {
        // 时间控制相关
        SimulationMode mode;              ///< 仿真模式
        TimeSyncStrategy sync_strategy;   ///< 时间同步策略
        double time_scale;                ///< 时间缩放因子
        double time_step;                 ///< 仿真时间步长（秒）
        double step_time_increment;       ///< 每个仿真步的时间增量（秒），默认10ms
        
        // 仿真控制相关
        double max_simulation_time;       ///< 最大仿真时间（秒）
        double sync_tolerance;            ///< 同步容差
        
        // 监控和日志相关
        bool enable_logging;              ///< 启用日志
        bool enable_performance_monitoring; ///< 启用性能监控
        bool enable_sync_monitoring;      ///< 启用同步监控
        
        // 输出相关
        std::string output_directory;     ///< 输出目录
        std::map<std::string, std::string> parameters; ///< 其他参数
        
        SimulationConfig() : mode(SimulationMode::SCALE_TIME),
                           sync_strategy(TimeSyncStrategy::STEP_BASED_SYNC),
                           time_scale(1.0), time_step(0.016), 
                           step_time_increment(0.01), max_simulation_time(3600.0),
                           sync_tolerance(0.001),
                           enable_logging(true), enable_performance_monitoring(true),
                           enable_sync_monitoring(true) {}
    };

    // ==================== 类型别名定义 ====================
    
    /**
     * @brief 时间更新回调函数类型
     */
    using TimeUpdateCallback = std::function<void(const SimulationTimePoint&)>;

    // ==================== 常量定义 ====================
    
    namespace SimulationConstants {
        constexpr double DEFAULT_TIME_STEP = 0.016;           ///< 默认时间步长 (60 FPS)
        constexpr double DEFAULT_STEP_INCREMENT = 0.01;       ///< 默认仿真步时间增量 (10ms)
        constexpr double DEFAULT_SYNC_TOLERANCE = 0.001;      ///< 默认同步容差 (1ms)
        constexpr double DEFAULT_TARGET_FPS = 60.0;           ///< 默认目标帧率
        constexpr double DEFAULT_MAX_SIMULATION_TIME = 3600.0; ///< 默认最大仿真时间 (1小时)
        constexpr double DEFAULT_REAL_TIME_FACTOR = 1.0;      ///< 默认实时因子
    }

} // namespace VFT_SMF 