/**
 * @file EventDispatcher.hpp
 * @brief EventDispatcher header - formerly ControllerManager
 * @author VFT_SMF Development Team
 * @date 2025-08-21
 * 
 * EventDispatcher 负责：
 * 1. 根据已触发事件列表分发到对应的代理
 * 2. 通过共享数据空间的代理事件队列进行路由
 * 3. 不再负责具体控制器的执行逻辑
 */

#pragma once

#include "../../E_GlobalSharedDataSpace/GlobalSharedDataSpace.hpp"
#include "../../G_SimulationManager/LogAndData/Logger.hpp"
#include <memory>
#include <string>
#include <map>
#include <set>

namespace VFT_SMF {

    /**
     * @brief 事件分发器类
     */
    class EventDispatcher {
    private:
        std::shared_ptr<GlobalShared_DataSpace::GlobalSharedDataSpace> shared_data_space;
        std::set<std::string> processed_events; // 用于事件去重
        
        // 控制器类型到代理ID的映射
        std::map<std::string, std::string> controller_to_agent_mapping;

    public:
        EventDispatcher(std::shared_ptr<GlobalShared_DataSpace::GlobalSharedDataSpace> data_space);
        ~EventDispatcher() = default;

        // 处理已触发事件列表的主要方法
        void processTriggeredEvents(double current_time);
        
        // 单个事件控制器执行方法（事件分发）
        void executeEventController(const GlobalSharedDataStruct::StandardEvent& event, double current_time);

    private:
        // 事件路由方法
        void routeEventToAgent(const std::string& agent_id, 
                              const GlobalSharedDataStruct::StandardEvent& event, 
                              double current_time);
        
        // 初始化控制器到代理的映射关系
        void initializeControllerMapping();
        
        // 根据控制器类型获取对应的代理ID
        std::string getAgentIdForController(const std::string& controller_type);

        // 辅助方法
        void clearProcessedEvents();
        bool isEventProcessed(const std::string& event_id);
        void markEventAsProcessed(const std::string& event_id);
    };

} // namespace VFT_SMF


