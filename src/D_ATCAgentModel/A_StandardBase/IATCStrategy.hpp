/**
 * @file IATCStrategy.hpp
 * @brief ATC策略接口 - 支持不同的ATC管理逻辑
 * @author VFT_SMF Development Team
 * @date 2025-08-21
 * 
 * 策略模式实现，支持：
 * 1. 不同的ATC管理人员的控制逻辑
 * 2. 配置驱动的策略选择
 * 3. 易于扩展新的ATC类型
 */

#pragma once

#include "../../E_GlobalSharedDataSpace/GlobalSharedDataSpace.hpp"
#include <memory>
#include <map>
#include <string>

namespace VFT_SMF {

    /**
     * @brief ATC策略接口
     * 定义所有ATC控制器的统一接口
     */
    class IATCStrategy {
    public:
        virtual ~IATCStrategy() = default;

        /**
         * @brief 获取策略标识符
         * @return 策略ID（如 "ATC_001", "ATC_002"）
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
         * @brief 执行滑行许可控制器
         * @param params 控制器参数
         * @param current_time 当前时间
         * @return 是否执行成功
         */
        virtual bool executeClearanceController(const std::map<std::string, std::string>& params, 
                                               double current_time) = 0;

        /**
         * @brief 执行紧急刹车控制器
         * @param params 控制器参数
         * @param current_time 当前时间
         * @return 是否执行成功
         */
        virtual bool executeEmergencyBrakeController(const std::map<std::string, std::string>& params, 
                                                    double current_time) = 0;

        /**
         * @brief 执行起飞许可控制器
         * @param params 控制器参数
         * @param current_time 当前时间
         * @return 是否执行成功
         */
        virtual bool executeTakeoffClearanceController(const std::map<std::string, std::string>& params, 
                                                      double current_time) = 0;

        /**
         * @brief 执行着陆许可控制器
         * @param params 控制器参数
         * @param current_time 当前时间
         * @return 是否执行成功
         */
        virtual bool executeLandingClearanceController(const std::map<std::string, std::string>& params, 
                                                      double current_time) = 0;

        /**
         * @brief 获取策略配置信息
         * @return 配置信息映射
         */
        virtual std::map<std::string, std::string> getStrategyConfig() const = 0;

        /**
         * @brief 获取策略性能统计
         * @return 性能统计信息
         */
        virtual std::string getPerformanceStats() const = 0;
    };

} // namespace VFT_SMF

