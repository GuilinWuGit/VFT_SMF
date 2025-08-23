/**
 * @file ATCFactory.hpp
 * @brief ATC工厂类 - 根据配置动态创建不同的ATC处理器
 * @author VFT_SMF Development Team
 * @date 2025-08-21
 * 
 * 工厂模式实现，支持：
 * 1. 根据ATC_ID动态创建对应的ATC处理器
 * 2. 支持扩展新的ATC类型
 * 3. 统一的接口，不同的实现
 */

#pragma once

#include "../E_GlobalSharedDataSpace/GlobalSharedDataSpace.hpp"
#include "../G_SimulationManager/LogAndData/Logger.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <functional>

namespace VFT_SMF {

    // 前向声明
    class IATCCommandHandler;

    /**
     * @brief ATC工厂类
     * 负责根据配置的ATC_ID创建对应的ATC处理器
     */
    class ATCFactory {
    public:
        // ATC处理器创建函数类型
        using ATCCommandHandlerCreator = std::function<std::unique_ptr<IATCCommandHandler>(
            std::shared_ptr<GlobalShared_DataSpace::GlobalSharedDataSpace>)>;

    private:
        // 注册的ATC处理器创建器映射
        static std::unordered_map<std::string, ATCCommandHandlerCreator> atc_handlers;
        
        // 是否已初始化
        static bool initialized;

    public:
        /**
         * @brief 初始化工厂，注册所有可用的ATC处理器
         */
        static void initialize();
        
        /**
         * @brief 创建ATC指令处理器
         * @param atc_id ATC标识符
         * @param shared_data_space 共享数据空间
         * @return 对应的ATC指令处理器，如果不存在则返回nullptr
         */
        static std::unique_ptr<IATCCommandHandler> createATCCommandHandler(
            const std::string& atc_id,
            std::shared_ptr<GlobalShared_DataSpace::GlobalSharedDataSpace> shared_data_space);
        
        /**
         * @brief 注册新的ATC处理器
         * @param atc_id ATC标识符
         * @param creator 创建函数
         */
        static void registerATCHandler(const std::string& atc_id, ATCCommandHandlerCreator creator);
        
        /**
         * @brief 检查是否支持指定的ATC_ID
         * @param atc_id ATC标识符
         * @return 是否支持
         */
        static bool isSupported(const std::string& atc_id);
        
        /**
         * @brief 获取所有支持的ATC_ID列表
         * @return 支持的ATC_ID列表
         */
        static std::vector<std::string> getSupportedATCIDs();

    private:
        // 私有构造函数，防止实例化
        ATCFactory() = delete;
    };

} // namespace VFT_SMF
