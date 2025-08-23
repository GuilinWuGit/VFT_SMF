/**
 * @file GlobalSharedDataSpace.cpp
 * @brief 全局共享数据空间实现文件
 * @author VFT_SMF Development Team
 * @date 2024
 */

#include "GlobalSharedDataSpace.hpp"

namespace VFT_SMF {
namespace GlobalShared_DataSpace {

// ==================== 线程同步管理实现 ====================

bool GlobalSharedDataSpace::registerThread(const std::string& thread_id, const std::string& thread_name, const std::string& thread_type) {
    // 检查线程是否已经注册
    if (thread_sync_manager.registered_threads.find(thread_id) != thread_sync_manager.registered_threads.end()) {
        if (VFT_SMF::globalLogger) {
            VFT_SMF::globalLogger->warning("线程 " + thread_id + " 已经注册");
        }
        return false;
    }
    
    // 创建新的线程注册信息
    VFT_SMF::GlobalSharedDataStruct::ThreadRegistrationInfo thread_info;
    thread_info.thread_id = thread_id;
    thread_info.thread_name = thread_name;
    thread_info.thread_type = thread_type;
    thread_info.is_registered = true;
    thread_info.is_ready = true;
    thread_info.sync_state = VFT_SMF::GlobalSharedDataStruct::ThreadSyncState::WAITING_FOR_CLOCK;
    thread_info.last_completion_time = 0.0;
    thread_info.current_step_time = 0.0;
    
    // 注册线程
    thread_sync_manager.registered_threads[thread_id] = thread_info;
    
    if (VFT_SMF::globalLogger) {
        VFT_SMF::globalLogger->info("线程 " + thread_id + " (" + thread_name + ") 注册成功");
    }
    
    return true;
}

bool GlobalSharedDataSpace::unregisterThread(const std::string& thread_id) {
    auto it = thread_sync_manager.registered_threads.find(thread_id);
    if (it == thread_sync_manager.registered_threads.end()) {
        if (VFT_SMF::globalLogger) {
            VFT_SMF::globalLogger->warning("线程 " + thread_id + " 未注册");
        }
        return false;
    }
    
    thread_sync_manager.registered_threads.erase(it);
    
    if (VFT_SMF::globalLogger) {
        VFT_SMF::globalLogger->info("线程 " + thread_id + " 注销成功");
    }
    
    return true;
}

void GlobalSharedDataSpace::updateThreadState(const std::string& thread_id, VFT_SMF::GlobalSharedDataStruct::ThreadSyncState state) {
    auto it = thread_sync_manager.registered_threads.find(thread_id);
    if (it == thread_sync_manager.registered_threads.end()) {
        if (VFT_SMF::globalLogger) {
            VFT_SMF::globalLogger->warning("线程 " + thread_id + " 未注册，无法更新状态");
        }
        return;
    }
    
    it->second.sync_state = state;
    
    // 降低日志频率：改为detail，避免每步大量info
    if (VFT_SMF::globalLogger) {
        VFT_SMF::globalLogger->debug("线程 " + thread_id + " 状态更新为: " + std::to_string(static_cast<int>(state)));
    }
    // 回退：不做条件变量通知
}

VFT_SMF::GlobalSharedDataStruct::ThreadSyncState GlobalSharedDataSpace::getThreadState(const std::string& thread_id) {
    auto it = thread_sync_manager.registered_threads.find(thread_id);
    if (it == thread_sync_manager.registered_threads.end()) {
        return VFT_SMF::GlobalSharedDataStruct::ThreadSyncState::ERROR_STATE;
    }
    
    return it->second.sync_state;
}

std::map<std::string, VFT_SMF::GlobalSharedDataStruct::ThreadRegistrationInfo> GlobalSharedDataSpace::getRegisteredThreads() {
    return thread_sync_manager.registered_threads;
}

void GlobalSharedDataSpace::setClockRunning(bool running) {
    thread_sync_manager.clock_running = running;
    
    if (VFT_SMF::globalLogger) {
        VFT_SMF::globalLogger->info("时钟运行状态设置为: " + std::string(running ? "运行" : "停止"));
    }
}

void GlobalSharedDataSpace::setSimulationOver(bool is_over) {
    thread_sync_manager.is_sim_over = is_over;
    
    if (VFT_SMF::globalLogger) {
        VFT_SMF::globalLogger->info("仿真结束标志设置为: " + std::string(is_over ? "结束" : "运行中"));
    }
    // 回退：不做条件变量通知
}

bool GlobalSharedDataSpace::isSimulationOver() {
    return thread_sync_manager.is_sim_over.load();
}

void GlobalSharedDataSpace::updateSyncSignal(double simulation_time, uint64_t step) {
    // 更新同步信号
    thread_sync_manager.current_sync_signal.current_simulation_time = simulation_time;
    thread_sync_manager.current_sync_signal.current_step = step;
    thread_sync_manager.current_sync_signal.step_ready = true;
    thread_sync_manager.current_sync_signal.all_threads_completed = false;
    thread_sync_manager.current_sync_signal.completed_threads.clear();
    thread_sync_manager.current_sync_signal.waiting_threads.clear();
    
    // 降低日志频率：改为detail
    if (VFT_SMF::globalLogger) {
        VFT_SMF::globalLogger->debug("同步信号已更新，仿真时间: " + std::to_string(simulation_time) + "s, 步骤: " + std::to_string(step));
    }
    // 回退：不做条件变量通知
}

void GlobalSharedDataSpace::resetSyncSignal() {
    // 重置同步信号，表示当前步骤已完成
    thread_sync_manager.current_sync_signal.step_ready = false;
    thread_sync_manager.current_sync_signal.all_threads_completed = true;
    
    // 降低日志频率：改为detail
    if (VFT_SMF::globalLogger) {
        VFT_SMF::globalLogger->debug("同步信号已重置，等待下一步骤");
    }
}

VFT_SMF::GlobalSharedDataStruct::ClockSyncSignal GlobalSharedDataSpace::getCurrentSyncSignal() {
    return thread_sync_manager.current_sync_signal;
}

// ==================== 代理事件队列管理实现 ====================

void GlobalSharedDataSpace::createAgentEventQueue(const std::string& agent_id) {
    agent_event_queue_manager.createAgentQueue(agent_id);
    
    if (VFT_SMF::globalLogger) {
        VFT_SMF::globalLogger->info("为代理 " + agent_id + " 创建事件队列");
    }
}

void GlobalSharedDataSpace::enqueueAgentEvent(const std::string& agent_id, 
                                             const VFT_SMF::GlobalSharedDataStruct::StandardEvent& event,
                                             double trigger_time, const std::string& ctrl_type, 
                                             const std::string& ctrl_name,
                                             const std::map<std::string, std::string>& params) {
    agent_event_queue_manager.enqueueAgentEvent(agent_id, event, trigger_time, ctrl_type, ctrl_name, params);
    
    if (VFT_SMF::globalLogger) {
        VFT_SMF::globalLogger->debug("向代理 " + agent_id + " 队列添加事件: " + event.event_name + 
                                    " (控制器: " + ctrl_type + "::" + ctrl_name + ")");
    }
}

bool GlobalSharedDataSpace::dequeueAgentEvent(const std::string& agent_id, 
                                             VFT_SMF::GlobalSharedDataStruct::AgentEventQueueItem& item) {
    bool success = agent_event_queue_manager.dequeueAgentEvent(agent_id, item);
    
    if (success && VFT_SMF::globalLogger) {
        VFT_SMF::globalLogger->debug("从代理 " + agent_id + " 队列取出事件: " + item.event.event_name);
    }
    
    return success;
}

size_t GlobalSharedDataSpace::getAgentEventQueueSize(const std::string& agent_id) const {
    return agent_event_queue_manager.getAgentQueueSize(agent_id);
}

bool GlobalSharedDataSpace::isAgentEventQueueEmpty(const std::string& agent_id) const {
    return agent_event_queue_manager.isAgentQueueEmpty(agent_id);
}

std::vector<std::string> GlobalSharedDataSpace::getAgentEventQueueIds() const {
    return agent_event_queue_manager.getAgentIds();
}

// 回退：不提供条件变量等待接口

} // namespace GlobalShared_DataSpace
} // namespace VFT_SMF
