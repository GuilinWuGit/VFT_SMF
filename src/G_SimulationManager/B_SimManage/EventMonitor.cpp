/**
 * @file EventMonitor.cpp
 * @brief 事件监测器实现
 * @author VFT_SMF Development Team
 * @date 2024
 */

#include "EventMonitor.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace VFT_SMF {

EventMonitor::EventMonitor(std::shared_ptr<GlobalShared_DataSpace::GlobalSharedDataSpace> data_space)
    : shared_data_space(std::move(data_space)) {
    VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "事件监测器已创建");
}

void EventMonitor::initialize() {
    if (!shared_data_space) {
        VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "事件监测器初始化失败：共享数据空间为空");
        return;
    }
    
    // 注册条件解析器
    registerConditionParsers();
    
    // 初始化统计信息
    statistics = EventTriggerStatistics();
    
    // 清空触发状态记录
    event_trigger_status.clear();
    
    VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "事件监测器初始化完成");
}

std::vector<VFT_SMF::GlobalSharedDataStruct::StandardEvent> EventMonitor::monitorEvents(double current_time) {
    std::vector<VFT_SMF::GlobalSharedDataStruct::StandardEvent> newly_triggered_events;
    
    if (!shared_data_space) {
        return newly_triggered_events;
    }
    
    // 获取所有计划事件
    const auto& planned_events = shared_data_space->getPlannedEventLibrary().getPlannedEvents();
    
    // 更新总事件数（只在第一次调用时）
    if (statistics.total_events == 0) {
        statistics.total_events = planned_events.size();
    }
    
    // 检查每个事件的触发条件
    for (const auto& event : planned_events) {
        // 检查事件是否已经被触发
        if (event_trigger_status.find(event.getEventIdString()) != event_trigger_status.end() &&
            event_trigger_status[event.getEventIdString()]) {
            continue; // 事件已经触发过
        }
        
        // 检查触发条件
        if (checkEventTriggerCondition(event, current_time)) {
            // 创建事件副本并标记为已触发
            VFT_SMF::GlobalSharedDataStruct::StandardEvent triggered_event = event;
            triggered_event.is_triggered = true;
            
            // 记录事件触发
            recordEventTrigger(triggered_event, current_time);
            
            // 添加到新触发事件列表
            newly_triggered_events.push_back(triggered_event);
            
            // 标记为已触发
            event_trigger_status[event.getEventIdString()] = true;
            
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, 
                "事件监测器检测到新触发事件: " + event.event_name + " (ID: " + event.getEventIdString() + ")");
        }
    }
    
    return newly_triggered_events;
}

void EventMonitor::recordEventTrigger(const VFT_SMF::GlobalSharedDataStruct::StandardEvent& event, double trigger_time) {
    // 创建触发记录
    EventTriggerRecord record(
        event.getEventIdString(),
        event.event_name,
        event.trigger_condition.condition_expression,
        trigger_time,
        event.source_agent,
        event.description
    );
    
    // 添加到触发事件列表
    triggered_events.push_back(record);
    
    // 更新统计信息
    updateStatistics(event, trigger_time);
    
    VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, 
        "事件触发已记录: " + event.event_name + " at " + std::to_string(trigger_time) + "s");
}

void EventMonitor::markEventAsExecuted(const std::string& event_id) {
    // 查找并标记事件为已执行
    for (auto& record : triggered_events) {
        if (record.event_id == event_id && !record.is_executed) {
            record.is_executed = true;
            statistics.executed_events++;
            
            VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, 
                "事件已标记为执行: " + event_id);
            break;
        }
    }
}

void EventMonitor::reset() {
    triggered_events.clear();
    event_trigger_status.clear();
    statistics = EventTriggerStatistics();
    
    VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "事件监测器已重置");
}

std::string EventMonitor::generateReport() const {
    std::ostringstream oss;
    oss << "=== 事件监测器报告 ===\n";
    oss << "总事件数: " << statistics.total_events << "\n";
    oss << "已触发事件数: " << statistics.triggered_events << "\n";
    oss << "已执行事件数: " << statistics.executed_events << "\n";
    oss << "首次触发时间: " << std::fixed << std::setprecision(3) << statistics.first_trigger_time << "s\n";
    oss << "最后触发时间: " << std::fixed << std::setprecision(3) << statistics.last_trigger_time << "s\n";
    
    if (!statistics.trigger_by_condition_type.empty()) {
        oss << "按条件类型统计:\n";
        for (const auto& [condition_type, count] : statistics.trigger_by_condition_type) {
            oss << "  " << condition_type << ": " << count << " 个事件\n";
        }
    }
    
    if (!triggered_events.empty()) {
        oss << "已触发事件详情:\n";
        for (const auto& record : triggered_events) {
            oss << "  " << record.event_id << " - " << record.event_name 
                << " (触发时间: " << std::fixed << std::setprecision(3) << record.trigger_time << "s"
                << ", 条件: " << record.trigger_condition
                << ", 执行状态: " << (record.is_executed ? "已执行" : "未执行") << ")\n";
        }
    }
    
    return oss.str();
}

bool EventMonitor::checkEventTriggerCondition(const VFT_SMF::GlobalSharedDataStruct::StandardEvent& event, double current_time) {
    if (!shared_data_space) {
        return false;
    }
    
    // 获取当前状态
    const auto& aircraft_state = shared_data_space->getAircraftFlightState();
    const auto& atc_command = shared_data_space->getATCCommand();
    
    // 基于条件的触发（唯一触发方式）
    const auto& condition = event.trigger_condition.condition_expression;
    if (!condition.empty()) {
        bool triggered = parseCompoundCondition(condition, current_time, aircraft_state, atc_command);
        if (triggered) {
            VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, 
                "事件条件触发: " + event.event_name + " (条件: " + condition + 
                ", 时间: " + std::to_string(current_time) + ")");
        }
        return triggered;
    }
    
    return false;
}

bool EventMonitor::parseTimeCondition(const std::string& condition, double current_time) {
    if (condition.find("time >") != std::string::npos) {
        size_t pos = condition.find("time >");
        if (pos != std::string::npos) {
            std::string time_str = condition.substr(pos + 6);
            try {
                double trigger_time = std::stod(time_str);
                return current_time > trigger_time;  // 修复：使用 > 而不是 >=
            } catch (...) {
                VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, 
                    "时间条件解析失败: " + condition);
            }
        }
    }
    return false;
}

bool EventMonitor::parseDistanceCondition(const std::string& condition, double current_time,
                                         const VFT_SMF::GlobalSharedDataStruct::AircraftFlightState& aircraft_state) {
    if (condition.find("distance >") != std::string::npos) {
        size_t pos = condition.find("distance >");
        if (pos != std::string::npos) {
            std::string distance_str = condition.substr(pos + 10);
            try {
                double trigger_distance = std::stod(distance_str);
                double current_distance = calculateDistance(current_time, aircraft_state);
                return current_distance >= trigger_distance;
            } catch (...) {
                VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, 
                    "距离条件解析失败: " + condition);
            }
        }
    }
    return false;
}

bool EventMonitor::parseSpeedCondition(const std::string& condition,
                                     const VFT_SMF::GlobalSharedDataStruct::AircraftFlightState& aircraft_state) {
    if (condition.find("speed >") != std::string::npos) {
        size_t pos = condition.find("speed >");
        if (pos != std::string::npos) {
            std::string speed_str = condition.substr(pos + 7);
            try {
                double trigger_speed = std::stod(speed_str);
                return aircraft_state.groundspeed >= trigger_speed;
            } catch (...) {
                VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, 
                    "速度条件解析失败: " + condition);
            }
        }
    }
    return false;
}

bool EventMonitor::parseATCCondition(const std::string& condition,
                                   const VFT_SMF::GlobalSharedDataStruct::ATC_Command& atc_command) {
    // 紧急刹车指令触发
    if (condition.find("atc_brake_command_received") != std::string::npos) {
        return atc_command.emergency_brake;
    }

    // 收到ATC滑行许可触发（兼容两种写法）
    // 1) taxi_clearance_received
    if (condition.find("taxi_clearance_received") != std::string::npos) {
        return atc_command.clearance_granted;
    }

    // 2) clearance_granted / clearance_granted == true / clearance_granted = true / 对应的 false
    if (condition.find("clearance_granted") != std::string::npos) {
        // 默认将出现该标识视为需要其为真
        bool expected = true;

        // 查找是否显式比较 true/false
        auto pos_eq = condition.find("=");
        if (pos_eq != std::string::npos) {
            std::string rhs = condition.substr(pos_eq + 1);
            // 去除空白
            rhs.erase(std::remove_if(rhs.begin(), rhs.end(), ::isspace), rhs.end());
            // 兼容 ==true, =true, ==false, =false（大小写不敏感）
            std::string rhs_lower;
            rhs_lower.resize(rhs.size());
            std::transform(rhs.begin(), rhs.end(), rhs_lower.begin(), [](unsigned char c){ return std::tolower(c); });
            if (rhs_lower.find("false") != std::string::npos || rhs_lower == "0") {
                expected = false;
            } else if (rhs_lower.find("true") != std::string::npos || rhs_lower == "1") {
                expected = true;
            }
        }

        return atc_command.clearance_granted == expected;
    }

    return false;
}

bool EventMonitor::parseCompoundCondition(const std::string& condition, double current_time,
                                         const VFT_SMF::GlobalSharedDataStruct::AircraftFlightState& aircraft_state,
                                         const VFT_SMF::GlobalSharedDataStruct::ATC_Command& atc_command) {
    // 处理OR条件
    if (condition.find("||") != std::string::npos) {
        size_t or_pos = condition.find("||");
        std::string left_condition = condition.substr(0, or_pos);
        std::string right_condition = condition.substr(or_pos + 2);
        
        bool left_result = parseTimeCondition(left_condition, current_time) ||
                          parseDistanceCondition(left_condition, current_time, aircraft_state) ||
                          parseSpeedCondition(left_condition, aircraft_state) ||
                          parseATCCondition(left_condition, atc_command);
        
        bool right_result = parseTimeCondition(right_condition, current_time) ||
                           parseDistanceCondition(right_condition, current_time, aircraft_state) ||
                           parseSpeedCondition(right_condition, aircraft_state) ||
                           parseATCCondition(right_condition, atc_command);
        
        return left_result || right_result;
    }
    
    // 处理单一条件
    return parseTimeCondition(condition, current_time) ||
           parseDistanceCondition(condition, current_time, aircraft_state) ||
           parseSpeedCondition(condition, aircraft_state) ||
           parseATCCondition(condition, atc_command);
}

double EventMonitor::calculateDistance(double current_time, 
                                     const VFT_SMF::GlobalSharedDataStruct::AircraftFlightState& aircraft_state) {
    // 简化计算：基于时间和速度
    // 这里可以扩展为更复杂的距离计算逻辑
    return aircraft_state.groundspeed * current_time;
}

void EventMonitor::updateStatistics(const VFT_SMF::GlobalSharedDataStruct::StandardEvent& event, double trigger_time) {
    statistics.triggered_events++;
    
    // 更新首次和最后触发时间
    if (statistics.first_trigger_time == 0.0) {
        statistics.first_trigger_time = trigger_time;
    }
    statistics.last_trigger_time = trigger_time;
    
    // 按条件类型统计
    const auto& condition = event.trigger_condition.condition_expression;
    std::string condition_type = "unknown";
    
    if (condition.find("time >") != std::string::npos) {
        condition_type = "time_based";
    } else if (condition.find("distance >") != std::string::npos) {
        condition_type = "distance_based";
    } else if (condition.find("speed >") != std::string::npos) {
        condition_type = "speed_based";
    } else if (condition.find("atc_brake_command_received") != std::string::npos) {
        condition_type = "atc_command_based";
    } else if (condition.find("||") != std::string::npos) {
        condition_type = "compound_condition";
    }
    
    statistics.trigger_by_condition_type[condition_type]++;
}

void EventMonitor::registerConditionParsers() {
    // 这里可以注册更多的条件解析器
    // 目前使用内联解析方法，未来可以扩展为插件式架构
    VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "条件解析器注册完成");
}

} // namespace VFT_SMF
