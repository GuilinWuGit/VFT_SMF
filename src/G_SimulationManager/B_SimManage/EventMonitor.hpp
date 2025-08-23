/**
 * @file EventMonitor.hpp
 * @brief 事件监测器定义
 * @author VFT_SMF Development Team
 * @date 2024
 * 
 * 事件监测器负责：
 * 1. 监测事件触发条件
 * 2. 记录已触发的事件
 * 3. 提供事件触发统计
 * 4. 支持多种触发条件类型
 */

#pragma once

#include "../../E_GlobalSharedDataSpace/GlobalSharedDataSpace.hpp"
#include "../LogAndData/Logger.hpp"
#include <vector>
#include <map>
#include <string>
#include <memory>
#include <functional>
#include <chrono>

namespace VFT_SMF {

/**
 * @brief 事件触发记录结构
 */
struct EventTriggerRecord {
    std::string event_id;           ///< 事件ID
    std::string event_name;         ///< 事件名称
    std::string trigger_condition;  ///< 触发条件
    double trigger_time;            ///< 触发时间
    double planned_time;            ///< 计划时间
    std::string source_agent;       ///< 源代理
    std::string target_agent;       ///< 目标代理
    std::string description;        ///< 描述
    bool is_executed;               ///< 是否已执行
    
    EventTriggerRecord() : trigger_time(0.0), planned_time(0.0), is_executed(false) {}
    
    EventTriggerRecord(const std::string& id, const std::string& name, 
                      const std::string& condition, double time, 
                      const std::string& source, const std::string& desc)
        : event_id(id), event_name(name), trigger_condition(condition), 
          trigger_time(time), planned_time(0.0), source_agent(source), 
          target_agent(""), description(desc), is_executed(false) {}
};

/**
 * @brief 事件触发统计结构
 */
struct EventTriggerStatistics {
    size_t total_events;            ///< 总事件数
    size_t triggered_events;        ///< 已触发事件数
    size_t executed_events;         ///< 已执行事件数
    double first_trigger_time;      ///< 首次触发时间
    double last_trigger_time;       ///< 最后触发时间
    std::map<std::string, size_t> trigger_by_condition_type; ///< 按条件类型统计
    
    EventTriggerStatistics() : total_events(0), triggered_events(0), 
                              executed_events(0), first_trigger_time(0.0), 
                              last_trigger_time(0.0) {}
};

/**
 * @brief 事件监测器类
 */
class EventMonitor {
private:
    std::shared_ptr<GlobalShared_DataSpace::GlobalSharedDataSpace> shared_data_space;
    std::vector<EventTriggerRecord> triggered_events;
    std::map<std::string, bool> event_trigger_status; // 记录每个事件的触发状态
    EventTriggerStatistics statistics;
    
    // 条件解析函数类型
    using ConditionParser = std::function<bool(const std::string&, double, 
                                                                                             const VFT_SMF::GlobalSharedDataStruct::AircraftFlightState&,
                                               const VFT_SMF::GlobalSharedDataStruct::ATC_Command&)>;
    
    std::map<std::string, ConditionParser> condition_parsers;
    
public:
    /**
     * @brief 构造函数
     * @param data_space 共享数据空间指针
     */
    explicit EventMonitor(std::shared_ptr<GlobalShared_DataSpace::GlobalSharedDataSpace> data_space);
    
    /**
     * @brief 析构函数
     */
    ~EventMonitor() = default;
    
    /**
     * @brief 初始化事件监测器
     */
    void initialize();
    
    /**
     * @brief 监测所有事件的触发条件
     * @param current_time 当前仿真时间
     * @return 新触发的事件列表
     */
    std::vector<VFT_SMF::GlobalSharedDataStruct::StandardEvent> monitorEvents(double current_time);
    
    /**
     * @brief 记录事件触发
     * @param event 触发的事件
     * @param trigger_time 触发时间
     */
    void recordEventTrigger(const VFT_SMF::GlobalSharedDataStruct::StandardEvent& event, double trigger_time);
    
    /**
     * @brief 标记事件为已执行
     * @param event_id 事件ID
     */
    void markEventAsExecuted(const std::string& event_id);
    
    /**
     * @brief 获取触发统计
     * @return 事件触发统计
     */
    const EventTriggerStatistics& getStatistics() const { return statistics; }
    
    /**
     * @brief 获取已触发事件列表
     * @return 已触发事件列表
     */
    const std::vector<EventTriggerRecord>& getTriggeredEvents() const { return triggered_events; }
    
    /**
     * @brief 重置监测器
     */
    void reset();
    
    /**
     * @brief 生成事件监测报告
     * @return 监测报告字符串
     */
    std::string generateReport() const;

private:
    /**
     * @brief 检查单个事件的触发条件
     * @param event 事件
     * @param current_time 当前时间
     * @return 是否触发
     */
    bool checkEventTriggerCondition(const VFT_SMF::GlobalSharedDataStruct::StandardEvent& event, double current_time);
    
    /**
     * @brief 解析时间条件
     * @param condition 条件表达式
     * @param current_time 当前时间
     * @return 是否满足条件
     */
    bool parseTimeCondition(const std::string& condition, double current_time);
    
    /**
     * @brief 解析距离条件
     * @param condition 条件表达式
     * @param current_time 当前时间
     * @param aircraft_state 飞机状态
     * @return 是否满足条件
     */
    bool parseDistanceCondition(const std::string& condition, double current_time,
                               const VFT_SMF::GlobalSharedDataStruct::AircraftFlightState& aircraft_state);
    
    /**
     * @brief 解析速度条件
     * @param condition 条件表达式
     * @param aircraft_state 飞机状态
     * @return 是否满足条件
     */
    bool parseSpeedCondition(const std::string& condition,
                            const VFT_SMF::GlobalSharedDataStruct::AircraftFlightState& aircraft_state);
    
    /**
     * @brief 解析ATC指令条件
     * @param condition 条件表达式
     * @param atc_command ATC指令
     * @return 是否满足条件
     */
    bool parseATCCondition(const std::string& condition,
                          const VFT_SMF::GlobalSharedDataStruct::ATC_Command& atc_command);
    
    /**
     * @brief 解析复合条件（OR逻辑）
     * @param condition 条件表达式
     * @param current_time 当前时间
     * @param aircraft_state 飞机状态
     * @param atc_command ATC指令
     * @return 是否满足条件
     */
    bool parseCompoundCondition(const std::string& condition, double current_time,
                               const VFT_SMF::GlobalSharedDataStruct::AircraftFlightState& aircraft_state,
                               const VFT_SMF::GlobalSharedDataStruct::ATC_Command& atc_command);
    
    /**
     * @brief 计算滑行距离
     * @param current_time 当前时间
     * @param aircraft_state 飞机状态
     * @return 滑行距离（米）
     */
    double calculateDistance(double current_time, 
                           const VFT_SMF::GlobalSharedDataStruct::AircraftFlightState& aircraft_state);
    
    /**
     * @brief 更新统计信息
     * @param event 触发的事件
     * @param trigger_time 触发时间
     */
    void updateStatistics(const VFT_SMF::GlobalSharedDataStruct::StandardEvent& event, double trigger_time);
    
    /**
     * @brief 注册条件解析器
     */
    void registerConditionParsers();
};

} // namespace VFT_SMF
