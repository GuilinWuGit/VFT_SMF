/**
 * @file ATCFactory.cpp
 * @brief ATC工厂类实现
 * @author VFT_SMF Development Team
 * @date 2025-08-21
 */

#include "ATCFactory.hpp"
#include "A_StandardBase/IATCCommandHandler.hpp"
#include "ATC_001/ATCCommandHandler.hpp"
#include "ATC_002/ATCCommandHandler.hpp"
#include <algorithm>

namespace VFT_SMF {

    // 静态成员初始化
    std::unordered_map<std::string, ATCFactory::ATCCommandHandlerCreator> ATCFactory::atc_handlers;
    bool ATCFactory::initialized = false;

    void ATCFactory::initialize() {
        if (initialized) {
            return;
        }

        logBrief(LogLevel::Brief, "ATC工厂初始化开始...");

        // 注册 ATC_001 处理器
        registerATCHandler("ATC_001", [](std::shared_ptr<GlobalShared_DataSpace::GlobalSharedDataSpace> data_space) {
            return std::make_unique<ATCCommandHandler>(data_space);
        });

        // 注册 ATC_002 处理器
        registerATCHandler("ATC_002", [](std::shared_ptr<GlobalShared_DataSpace::GlobalSharedDataSpace> data_space) {
            return std::make_unique<ATCCommandHandler_002>(data_space);
        });

        initialized = true;
        logBrief(LogLevel::Brief, "ATC工厂初始化完成，已注册 " + std::to_string(atc_handlers.size()) + " 个ATC处理器");
    }

    std::unique_ptr<IATCCommandHandler> ATCFactory::createATCCommandHandler(
        const std::string& atc_id,
        std::shared_ptr<GlobalShared_DataSpace::GlobalSharedDataSpace> shared_data_space) {
        
        // 确保工厂已初始化
        if (!initialized) {
            initialize();
        }

        // 查找对应的创建器
        auto it = atc_handlers.find(atc_id);
        if (it != atc_handlers.end()) {
            logBrief(LogLevel::Brief, "ATC工厂: 创建 " + atc_id + " 处理器");
            return it->second(shared_data_space);
        } else {
            logBrief(LogLevel::Brief, "ATC工厂: 不支持的ATC_ID: " + atc_id + "，使用默认处理器");
            
            // 如果不支持，使用默认的ATC_001处理器
            auto default_it = atc_handlers.find("ATC_001");
            if (default_it != atc_handlers.end()) {
                return default_it->second(shared_data_space);
            }
            
            return nullptr;
        }
    }

    void ATCFactory::registerATCHandler(const std::string& atc_id, ATCCommandHandlerCreator creator) {
        atc_handlers[atc_id] = creator;
        logBrief(LogLevel::Brief, "ATC工厂: 注册 " + atc_id + " 处理器");
    }

    bool ATCFactory::isSupported(const std::string& atc_id) {
        if (!initialized) {
            initialize();
        }
        return atc_handlers.find(atc_id) != atc_handlers.end();
    }

    std::vector<std::string> ATCFactory::getSupportedATCIDs() {
        if (!initialized) {
            initialize();
        }
        
        std::vector<std::string> supported_ids;
        for (const auto& pair : atc_handlers) {
            supported_ids.push_back(pair.first);
        }
        
        return supported_ids;
    }

} // namespace VFT_SMF
