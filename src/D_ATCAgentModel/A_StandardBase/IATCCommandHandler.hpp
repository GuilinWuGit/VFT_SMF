/**
 * @file IATCCommandHandler.hpp
 * @brief ATC指令处理器基类接口
 * @author VFT_SMF Development Team
 * @date 2025-08-21
 * 
 * 定义ATC指令处理器的统一接口，支持不同的ATC实现
 */

#pragma once

#include "../../E_GlobalSharedDataSpace/GlobalSharedDataSpace.hpp"
#include <memory>

namespace VFT_SMF {

    /**
     * @brief ATC指令处理器基类接口
     */
    class IATCCommandHandler {
    public:
        virtual ~IATCCommandHandler() = default;

        /**
         * @brief 处理飞行员ATC指令
         * @param event 标准事件
         * @param current_time 当前时间
         */
        virtual void handlePilotATCCommand(const GlobalSharedDataStruct::StandardEvent& event,
                                          double current_time) = 0;

        /**
         * @brief 处理ATC事件
         * @param event 标准事件
         * @param current_time 当前时间
         */
        virtual void handleATCEvent(const GlobalSharedDataStruct::StandardEvent& event,
                                   double current_time) = 0;
    };

} // namespace VFT_SMF
