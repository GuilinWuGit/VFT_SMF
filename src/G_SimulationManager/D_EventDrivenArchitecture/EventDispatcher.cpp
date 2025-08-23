/**
 * @file EventDispatcher.cpp
 * @brief 事件分发器实现（由 ControllerManager 重命名）
 */

#include "EventDispatcher.hpp"
#include <algorithm>

namespace VFT_SMF {

    EventDispatcher::EventDispatcher(std::shared_ptr<GlobalShared_DataSpace::GlobalSharedDataSpace> data_space)
        : shared_data_space(data_space) {
        logBrief(LogLevel::Brief, "EventDispatcher 创建完成");
        
        // 初始化控制器到代理的映射关系
        initializeControllerMapping();
    }

    void EventDispatcher::processTriggeredEvents(double current_time) {
        VFT_SMF::GlobalSharedDataStruct::EventQueueItem queue_item;
        int processed_count = 0;
        while (shared_data_space->dequeueEvent(queue_item)) {
            const auto& event = queue_item.event;
            logBrief(LogLevel::Brief, "EventDispatcher: 从队列取出事件 " + event.event_name +
                    " (ID: " + event.getEventIdString() + ") 于 " + std::to_string(queue_item.trigger_time) + "s");
            executeEventController(event, current_time);
            processed_count++;
        }
        if (processed_count > 0) {
            logBrief(LogLevel::Brief, "EventDispatcher: 本步处理事件数量: " + std::to_string(processed_count));
        }
    }

    void EventDispatcher::clearProcessedEvents() {
        processed_events.clear();
    }

    bool EventDispatcher::isEventProcessed(const std::string& event_id) {
        return processed_events.find(event_id) != processed_events.end();
    }

    void EventDispatcher::markEventAsProcessed(const std::string& event_id) {
        processed_events.insert(event_id);
    }

    void EventDispatcher::executeEventController(const GlobalSharedDataStruct::StandardEvent& event, double current_time) {
        const auto& driven_process = event.driven_process;
        const std::string& controller_type = driven_process.controller_type;
        const std::string& controller_name = driven_process.controller_name;
        
        logBrief(LogLevel::Brief, "EventDispatcher: 分发事件 " + event.event_name + 
                " (控制器: " + controller_type + "::" + controller_name + ")");
        
        std::string agent_id = getAgentIdForController(controller_type);
        if (!agent_id.empty()) {
            routeEventToAgent(agent_id, event, current_time);
        } else {
            logBrief(LogLevel::Brief, "EventDispatcher: 未知的控制器类型: " + controller_type + 
                    "，无法分发事件");
        }
    }

    void EventDispatcher::routeEventToAgent(const std::string& agent_id, 
                                           const GlobalSharedDataStruct::StandardEvent& event, 
                                           double current_time) {
        const auto& driven_process = event.driven_process;
        const std::string& controller_type = driven_process.controller_type;
        const std::string& controller_name = driven_process.controller_name;
        
        shared_data_space->createAgentEventQueue(agent_id);
        shared_data_space->enqueueAgentEvent(agent_id, event, current_time, 
                                           controller_type, controller_name);
        
        logBrief(LogLevel::Brief, "EventDispatcher: 事件已路由到代理 " + agent_id + 
                " (事件: " + event.event_name + ", 控制器: " + controller_type + "::" + controller_name + ")");
    }

    void EventDispatcher::initializeControllerMapping() {
        // 从共享数据空间获取配置的代理ID
        auto flight_plan_data = shared_data_space->getFlightPlanData();
        
        // 使用配置的代理ID，如果未配置则使用默认值
        std::string atc_id = flight_plan_data.scenario_config.ATC_ID;
        if (atc_id.empty()) {
            atc_id = "ATC_001";
            logBrief(LogLevel::Brief, "EventDispatcher: 未找到配置的ATC_ID，使用默认值: " + atc_id);
        } else {
            logBrief(LogLevel::Brief, "EventDispatcher: 使用配置的ATC_ID: " + atc_id);
        }
        
        std::string pilot_id = flight_plan_data.scenario_config.Pilot_ID;
        if (pilot_id.empty()) {
            pilot_id = "Pilot_001";
            logBrief(LogLevel::Brief, "EventDispatcher: 未找到配置的Pilot_ID，使用默认值: " + pilot_id);
        } else {
            logBrief(LogLevel::Brief, "EventDispatcher: 使用配置的Pilot_ID: " + pilot_id);
        }
        
        std::string aircraft_id = flight_plan_data.scenario_config.Aircraft_ID;
        if (aircraft_id.empty()) {
            aircraft_id = "Aircraft_001";
            logBrief(LogLevel::Brief, "EventDispatcher: 未找到配置的Aircraft_ID，使用默认值: " + aircraft_id);
        } else {
            logBrief(LogLevel::Brief, "EventDispatcher: 使用配置的Aircraft_ID: " + aircraft_id);
        }
        
        // 设置控制器到代理的映射关系
        controller_to_agent_mapping["ATC_command"] = atc_id;
        controller_to_agent_mapping["Pilot_Manual_Control"] = pilot_id;
        controller_to_agent_mapping["Pilot_Flight_Task_Control"] = pilot_id;
        controller_to_agent_mapping["Aircraft_AutoPilot"] = aircraft_id;
        controller_to_agent_mapping["Aircraft_Sysytem_State_Shift"] = aircraft_id;
        controller_to_agent_mapping["Environment_State_Shift"] = "Environment_001";
        
        logBrief(LogLevel::Brief, "EventDispatcher: 控制器到代理映射关系初始化完成");
        logBrief(LogLevel::Brief, "EventDispatcher: ATC_command -> " + atc_id);
        logBrief(LogLevel::Brief, "EventDispatcher: Pilot_Manual_Control -> " + pilot_id);
        logBrief(LogLevel::Brief, "EventDispatcher: Aircraft_AutoPilot -> " + aircraft_id);
    }

    std::string EventDispatcher::getAgentIdForController(const std::string& controller_type) {
        auto it = controller_to_agent_mapping.find(controller_type);
        if (it != controller_to_agent_mapping.end()) {
            return it->second;
        }
        return "";
    }

} // namespace VFT_SMF


