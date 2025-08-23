/**
 * @file IPilotStrategy.hpp
 * @brief 飞行员策略接口 - 支持不同的飞行员控制逻辑
 * @author VFT_SMF Development Team
 * @date 2025-08-21
 * 
 * 策略模式实现，支持：
 * 1. 不同的飞行员性格和经验水平的控制逻辑
 * 2. 配置驱动的策略选择
 * 3. 易于扩展新的飞行员类型
 */

#pragma once

#include "../../E_GlobalSharedDataSpace/GlobalSharedDataSpace.hpp"
#include <memory>
#include <map>
#include <string>

// 前向声明
namespace VFT_SMF {
    enum class PilotExperienceLevel;
}

namespace VFT_SMF {

    /**
     * @brief 飞行员策略接口
     * 定义所有飞行员控制器的统一接口
     */
    class IPilotStrategy {
    public:
        virtual ~IPilotStrategy() = default;

        /**
         * @brief 获取策略标识符
         * @return 策略ID（如 "Pilot_001", "Pilot_002"）
         */
        virtual std::string getStrategyId() const = 0;

        /**
         * @brief 获取策略描述
         * @return 策略描述
         */
        virtual std::string getStrategyDescription() const = 0;

        /**
         * @brief 初始化策略
         * @param shared_data_space 全局共享数据空间
         * @param agent_id 代理ID
         */
        virtual void initialize(std::shared_ptr<GlobalShared_DataSpace::GlobalSharedDataSpace> shared_data_space,
                               const std::string& agent_id) = 0;

        /**
         * @brief 执行滑行控制控制器
         * @param params 控制器参数
         * @param current_time 当前时间
         * @return 是否执行成功
         */
        virtual bool executeTaxiControlController(const std::map<std::string, std::string>& params, 
                                                 double current_time) = 0;

        /**
         * @brief 执行起飞控制控制器
         * @param params 控制器参数
         * @param current_time 当前时间
         * @return 是否执行成功
         */
        virtual bool executeTakeoffControlController(const std::map<std::string, std::string>& params, 
                                                   double current_time) = 0;

        /**
         * @brief 执行着陆控制控制器
         * @param params 控制器参数
         * @param current_time 当前时间
         * @return 是否执行成功
         */
        virtual bool executeLandingControlController(const std::map<std::string, std::string>& params, 
                                                   double current_time) = 0;

        /**
         * @brief 执行紧急响应控制器
         * @param params 控制器参数
         * @param current_time 当前时间
         * @return 是否执行成功
         */
        virtual bool executeEmergencyResponseController(const std::map<std::string, std::string>& params, 
                                                       double current_time) = 0;

        /**
         * @brief 执行ATC指令响应控制器
         * @param params 控制器参数
         * @param current_time 当前时间
         * @return 是否执行成功
         */
        virtual bool executeATCCommandResponseController(const std::map<std::string, std::string>& params, 
                                                        double current_time) = 0;

        /**
         * @brief 更新飞行员状态
         * @param delta_time 时间步长
         */
        virtual void update(double delta_time) = 0;

        /**
         * @brief 获取飞行员技能水平
         * @return 技能水平 [0.0, 1.0]
         */
        virtual double getSkillLevel() const = 0;

        /**
         * @brief 获取飞行员注意力水平
         * @return 注意力水平 [0.0, 1.0]
         */
        virtual double getAttentionLevel() const = 0;

        /**
         * @brief 获取飞行员经验水平
         * @return 经验水平枚举
         */
        virtual PilotExperienceLevel getExperienceLevel() const = 0;
    };

} // namespace VFT_SMF
