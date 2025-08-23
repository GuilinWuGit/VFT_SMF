/**
 * @file VFT_SMF_Base.hpp
 * @brief VFT_SMF基础接口定义
 * @author VFT_SMF Development Team
 * @date 2024
 * 
 * 定义所有代理的基础接口和事件系统
 */

#pragma once

#include <string>
#include <map>
#include <chrono>
#include <memory>
#include <vector>

namespace VFT_SMF {

    /**
     * @brief 事件类型枚举
     */
    enum class EventType {
        SYSTEM_EVENT,        ///< 系统事件
        AGENT_EVENT,         ///< 代理事件
        SCENARIO_EVENT,      ///< 场景事件
        ENVIRONMENT_EVENT,   ///< 环境事件
        ATC_EVENT,          ///< ATC事件
        PILOT_EVENT,        ///< 飞行员事件
        AIRCRAFT_EVENT,     ///< 飞机事件
        FLIGHT_EVENT,       ///< 飞行事件
        SAFETY_EVENT,       ///< 安全事件
        PERFORMANCE_EVENT,  ///< 性能事件
        TIME_EVENT          ///< 时间事件
    };

    /**
     * @brief 事件结构体
     */
    struct Event {
        std::string id;                                     ///< 事件ID
        EventType type;                                     ///< 事件类型
        std::string source_agent;                          ///< 源代理
        std::string target_agent;                          ///< 目标代理
        std::string description;                           ///< 事件描述
        std::map<std::string, std::string> parameters;     ///< 事件参数
        std::chrono::system_clock::time_point timestamp;   ///< 时间戳
        int priority;                                       ///< 优先级 (0=低, 1=中, 2=高, 3=紧急)
        
        /**
         * @brief 构造函数
         * @param event_id 事件ID
         * @param t 事件类型
         * @param source 源代理
         * @param target 目标代理
         * @param desc 事件描述
         * @param prio 优先级
         */
        Event(const std::string& event_id, EventType t, const std::string& source, 
              const std::string& target, const std::string& desc, int prio = 0)
            : id(event_id), type(t), source_agent(source), target_agent(target), 
              description(desc), timestamp(std::chrono::system_clock::now()), priority(prio) {}
        
        /**
         * @brief 默认构造函数
         */
        Event() : type(EventType::SYSTEM_EVENT), 
                   timestamp(std::chrono::system_clock::now()), priority(0) {}
    };

    /**
     * @brief 代理状态枚举
     */
    enum class AgentState {
        UNINITIALIZED,   ///< 未初始化
        INITIALIZING,    ///< 初始化中
        READY,          ///< 就绪
        RUNNING,        ///< 运行中
        PAUSED,         ///< 暂停
        STOPPING,       ///< 停止中
        STOPPED,        ///< 已停止
        ERROR_STATE     ///< Error state
    };

    /**
     * @brief 基础代理接口
     * 
     * 所有VFT_SMF系统中的代理都必须实现此接口
     */
    class BaseAgent {
    protected:
        std::string agent_id;      ///< 代理ID
        std::string agent_name;    ///< 代理名称
        bool is_running;           ///< 运行状态
        AgentState current_state;  ///< 当前状态

    public:
        /**
         * @brief 虚析构函数
         */
        virtual ~BaseAgent() = default;
        
        /**
         * @brief 初始化代理
         * 
         * 在代理开始工作前调用，进行必要的初始化工作
         */
        virtual void initialize() = 0;
        
        /**
         * @brief 启动代理
         * 
         * 开始代理的正常工作流程
         */
        virtual void start() = 0;
        
        /**
         * @brief 暂停代理
         * 
         * 暂停代理的工作，但保持状态
         */
        virtual void pause() = 0;
        
        /**
         * @brief 恢复代理
         * 
         * 从暂停状态恢复工作
         */
        virtual void resume() = 0;
        
        /**
         * @brief 停止代理
         * 
         * 完全停止代理的工作，清理资源
         */
        virtual void stop() = 0;
        
        /**
         * @brief 更新代理状态
         * 
         * @param delta_time 时间步长（秒）
         */
        virtual void update(double delta_time) = 0;
        
        /**
         * @brief 处理事件
         * 
         * @param event 要处理的事件
         */
        virtual void handle_event(const Event& event) = 0;
        
        /**
         * @brief 发送事件
         * 
         * @param event 要发送的事件
         */
        virtual void send_event(const Event& event) = 0;
        
        /**
         * @brief 获取代理状态信息
         * 
         * @return 状态信息字符串
         */
        virtual std::string get_status() const = 0;
        
        /**
         * @brief 检查代理是否就绪
         * 
         * @return true如果代理已就绪，false否则
         */
        virtual bool is_ready() const = 0;
        
        /**
         * @brief 获取代理ID
         * 
         * @return 代理ID
         */
        std::string get_agent_id() const { return agent_id; }
        
        /**
         * @brief 获取代理名称
         * 
         * @return 代理名称
         */
        std::string get_agent_name() const { return agent_name; }
        
        /**
         * @brief 检查代理是否正在运行
         * 
         * @return true如果代理正在运行，false否则
         */
        bool is_agent_running() const { return is_running; }
        
        /**
         * @brief 获取代理当前状态
         * 
         * @return 当前状态
         */
        AgentState get_current_state() const { return current_state; }
        
        /**
         * @brief 设置代理状态
         * 
         * @param state 新状态
         */
        void set_current_state(AgentState state) { current_state = state; }
    };

    /**
     * @brief 事件处理器接口
     * 
     * 用于处理特定类型事件的接口
     */
    class EventHandler {
    public:
        virtual ~EventHandler() = default;
        
        /**
         * @brief 处理事件
         * 
         * @param event 要处理的事件
         * @return true如果事件被成功处理，false否则
         */
        virtual bool handle_event(const Event& event) = 0;
        
        /**
         * @brief 检查是否可以处理指定类型的事件
         * 
         * @param event_type 事件类型
         * @return true如果可以处理，false否则
         */
        virtual bool can_handle(EventType event_type) const = 0;
    };

    /**
     * @brief 事件监听器接口
     * 
     * 用于监听事件的接口
     */
    class EventListener {
    public:
        virtual ~EventListener() = default;
        
        /**
         * @brief 事件通知
         * 
         * @param event 发生的事件
         */
        virtual void on_event(const Event& event) = 0;
    };

    /**
     * @brief 事件管理器
     * 
     * 负责事件的注册、分发和管理
     */
    class EventManager {
    private:
        std::map<EventType, std::vector<std::shared_ptr<EventHandler>>> handlers;
        std::vector<std::shared_ptr<EventListener>> listeners;
        std::vector<Event> event_queue;
        
    public:
        /**
         * @brief 注册事件处理器
         * 
         * @param event_type 事件类型
         * @param handler 事件处理器
         */
        void register_handler(EventType event_type, std::shared_ptr<EventHandler> handler);
        
        /**
         * @brief 注册事件监听器
         * 
         * @param listener 事件监听器
         */
        void register_listener(std::shared_ptr<EventListener> listener);
        
        /**
         * @brief 发送事件
         * 
         * @param event 要发送的事件
         */
        void send_event(const Event& event);
        
        /**
         * @brief 处理事件队列
         */
        void process_events();
        
        /**
         * @brief 清空事件队列
         */
        void clear_events();
    };

} // namespace VFT_SMF 