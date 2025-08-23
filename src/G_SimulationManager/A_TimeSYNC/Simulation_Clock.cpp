/**
 * @file Simulation_Clock.cpp
 * @brief 仿真时钟管理系统实现
 * @author VFT_SMF Development Team
 * @date 2024
 */

#include "Simulation_Clock.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <mutex>
#include <memory>

namespace VFT_SMF {

// ==================== SimulationClock 实现 ====================

SimulationClock::SimulationClock(const SimulationConfig& config) 
    : config(config), current_simulation_time(0.0), current_step(0),
      start_time(std::chrono::high_resolution_clock::now()),
      last_update_time(std::chrono::high_resolution_clock::now()),
      is_running(false), is_paused(false), current_mode(config.mode),
      start_real_time(std::chrono::system_clock::now()), current_frame(0) {
    
    // 统计信息初始化已移除
    
}

// 析构函数已在头文件中声明为default

// ==================== 时钟启停与更新管理 ====================



void SimulationClock::start(std::shared_ptr<GlobalShared_DataSpace::GlobalSharedDataSpace> shared_data_space) {
    std::unique_lock<std::mutex> lock(clock_mutex);
    if (!is_running) {
        is_running = true;
        is_paused = false;
        start_real_time = std::chrono::system_clock::now();
        last_update_time = start_real_time;
        std::cout << "仿真时钟已启动" << std::endl;
    }
    if (shared_data_space) {
        shared_data_space->setClockRunning(true);
        // 设置初始同步信号，确保线程可以开始工作
        shared_data_space->updateSyncSignal(0.0, 0);
        VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "时钟启动时设置初始同步信号，仿真时间: 0.0s, 步骤: 0");
    }
}


void SimulationClock::stop(std::shared_ptr<GlobalShared_DataSpace::GlobalSharedDataSpace> shared_data_space) {
    std::unique_lock<std::mutex> lock(clock_mutex);
    if (is_running) {
        is_running = false;
        is_paused = false;
        std::cout << "仿真时钟已停止" << std::endl;
    }
    if (shared_data_space) {
        shared_data_space->setClockRunning(false);
        shared_data_space->setSimulationOver(true); // 设置仿真结束标志，通知所有线程立即结束
    }
}

void SimulationClock::pause() {
    std::unique_lock<std::mutex> lock(clock_mutex);
    
    if (is_running && !is_paused) {
        is_paused = true;
        current_mode = SimulationMode::PAUSED;
        
        std::cout << "仿真时钟已暂停" << std::endl;
    }
}

void SimulationClock::resume() {
    std::unique_lock<std::mutex> lock(clock_mutex);
    
    if (is_running && is_paused) {
        is_paused = false;
        current_mode = config.mode;
        last_update_time = std::chrono::system_clock::now();
        
        std::cout << "仿真时钟已恢复" << std::endl;
    }
}

void SimulationClock::reset() {
    std::unique_lock<std::mutex> lock(clock_mutex);
    
    current_simulation_time = 0.0;
    current_frame = 0;
    start_real_time = std::chrono::system_clock::now();
    last_update_time = start_real_time;
    
    std::cout << "仿真时钟已重置" << std::endl;
}

void SimulationClock::update(double delta_sim_time) {
    // 简单的时间更新，不涉及线程同步
    if (!is_running || is_paused) {
        return;
    }
    
    std::unique_lock<std::mutex> lock(clock_mutex);
    
    // 根据时间模式计算仿真时间增量
    double delta_simulation_time = 0.0;
    
    switch (current_mode) {
        case SimulationMode::SCALE_TIME:
            delta_simulation_time = delta_sim_time * config.time_scale;
            break;
            
        case SimulationMode::STEP_BY_STEP:
        case SimulationMode::PAUSED:
            delta_simulation_time = 0.0;
            break;
    }
    
    // 限制时间步长
    delta_simulation_time = clamp_time_step(delta_simulation_time);
    
    // 更新仿真时间
    double old_time = current_simulation_time.load();
    double new_time = old_time + delta_simulation_time;
    current_simulation_time.store(new_time);
    current_frame.fetch_add(1);
    
    // 更新最后更新时间
    last_update_time = std::chrono::system_clock::now();
}

void SimulationClock::update(double delta_sim_time, std::shared_ptr<GlobalShared_DataSpace::GlobalSharedDataSpace> shared_data_space) {
    if (!is_running || is_paused) {
        return;
    }
    
    std::unique_lock<std::mutex> lock(clock_mutex);
    
    // 根据时间模式计算仿真时间增量
    double delta_simulation_time = 0.0;
    
    switch (current_mode) {
        case SimulationMode::SCALE_TIME:
            delta_simulation_time = delta_sim_time * config.time_scale; // 仿真时间增量 = 仿真时间步长 * 时间缩放因子
            break;
            
        case SimulationMode::STEP_BY_STEP:
        case SimulationMode::PAUSED:
            delta_simulation_time = 0.0; // 暂停模式下，仿真时间增量为0
            break;
    }
    
    // 限制时间步长
    delta_simulation_time = clamp_time_step(delta_simulation_time);
    
    // 更新仿真时间
    double old_time = current_simulation_time.load();
    double new_time = old_time + delta_simulation_time;
    current_simulation_time.store(new_time);
    current_frame.fetch_add(1);
    
    // 更新最后更新时间
    last_update_time = std::chrono::system_clock::now();
    
    // 更新同步信号，通知所有线程新步骤开始
    shared_data_space->updateSyncSignal(new_time, current_frame.load());
    VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "时钟更新同步信号，仿真时间: " + std::to_string(new_time) + "s, 步骤: " + std::to_string(current_frame.load()));

    // 读取注册列表
    auto registered_threads = shared_data_space->getRegisteredThreads();

    if (!registered_threads.empty()) {
       // 释放时钟锁，避免等待期间占用锁
        lock.unlock();

        bool all_completed = false;
         int wait_count = 0;
            const int max_wait_count = 2000; // 降低上限，避免长时间忙等
            int sleep_us = 50;
            const int sleep_us_max = 500;

            // 自适应退避等待，直到所有线程完成
            while (!all_completed && wait_count < max_wait_count) {
                all_completed = true;
                for (const auto& thread_pair : registered_threads) {
                    auto thread_state = shared_data_space->getThreadState(thread_pair.first);
                    if (thread_state != VFT_SMF::GlobalSharedDataStruct::ThreadSyncState::COMPLETED) {
                        all_completed = false;
                        break;
                    }
                }

                if (!all_completed) {
                    std::this_thread::sleep_for(std::chrono::microseconds(sleep_us));
                    wait_count++;
                    sleep_us = std::min(sleep_us + 50, sleep_us_max);
                }
            }

            if (wait_count >= max_wait_count) {
                // 超出软等待上限后，进入严格但较小负担的等待模式
                VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "等待线程完成超时，进入严格等待模式，仿真时间: " + std::to_string(new_time));
                while (!all_completed) {
                    all_completed = true;
                    for (const auto& thread_pair : registered_threads) {
                        auto thread_state = shared_data_space->getThreadState(thread_pair.first);
                        if (thread_state != VFT_SMF::GlobalSharedDataStruct::ThreadSyncState::COMPLETED) {
                            all_completed = false;
                            break;
                        }
                    }
                    if (!all_completed) {
                        std::this_thread::sleep_for(std::chrono::microseconds(100));
                    }
                }
                VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "所有线程已完成，继续推进仿真时间");
            }

            // 所有线程完成后，重置同步信号，准备下一步
            if (all_completed) {
                shared_data_space->resetSyncSignal();
                VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "时钟重置同步信号，准备下一步，仿真时间: " + std::to_string(new_time) + "s");
            }

            // 重新获取时钟锁，继续后续流程
            lock.lock();
    }
   
            
}


// ==================== 时间查询 ====================

double SimulationClock::get_current_simulation_time() const {
    return current_simulation_time.load();
}

uint64_t SimulationClock::get_current_step() const {
    return current_frame.load();
}

SimulationTimePoint SimulationClock::get_current_simulation_time_point() const {
    return SimulationTimePoint(current_simulation_time.load(), current_frame.load());
}

SimulationDuration SimulationClock::get_simulation_duration() const {
    return SimulationDuration(current_simulation_time.load(), current_frame.load());
}

double SimulationClock::get_real_time_duration() const {
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - start_real_time);
    return duration.count();
}

double SimulationClock::get_simulation_time_scale() const {
    return config.time_scale;
}

double SimulationClock::get_simulation_time_step() const {
    return config.time_step;
}

// ==================== 时间模式控制 ====================

void SimulationClock::set_time_mode(SimulationMode mode) {
    std::lock_guard<std::mutex> lock(clock_mutex);
    
    current_mode = mode;
    config.mode = mode;
    
    if (mode == SimulationMode::PAUSED) {
        is_paused = true;
    } else if (is_running) {
        is_paused = false;
    }
    
    std::cout << "时间模式已设置为: " << static_cast<int>(mode) << std::endl;
}

SimulationMode SimulationClock::get_time_mode() const {
    return current_mode;
}

// ==================== 同步管理 ====================

void SimulationClock::set_sync_strategy(TimeSyncStrategy strategy) {
    std::lock_guard<std::mutex> lock(clock_mutex);
    config.sync_strategy = strategy;
    std::cout << "同步策略已设置为: " << static_cast<int>(strategy) << std::endl;
}

TimeSyncStrategy SimulationClock::get_sync_strategy() const {
    return config.sync_strategy;
}

// synchronize 已移除

// 同步状态/容差接口已移除

void SimulationClock::set_step_time_increment(double increment) {
    std::unique_lock<std::mutex> lock(clock_mutex);
    config.step_time_increment = std::max(0.0001, increment);
    std::cout << "仿真步时间增量已设置为: " << config.step_time_increment << "s" << std::endl;
}

double SimulationClock::get_step_time_increment() const {
    return config.step_time_increment;
}

// 性能统计接口已移除

// ==================== 回调函数设置 ====================

void SimulationClock::set_time_update_callback(std::function<void(const SimulationTimePoint&)> callback) {
    time_update_callback = callback;
}

// 同步状态回调接口已移除

// ==================== 状态查询 ====================

bool SimulationClock::is_clock_running() const {
    return is_running.load();
}

bool SimulationClock::is_clock_paused() const {
    return is_paused.load();
}

SimulationConfig SimulationClock::get_config() const {
    std::unique_lock<std::mutex> lock(clock_mutex);
    return config;
}

void SimulationClock::set_config(const SimulationConfig& new_config) {
    std::lock_guard<std::mutex> lock(clock_mutex);
    config = new_config;
    current_mode = config.mode;
    std::cout << "时钟配置已更新" << std::endl;
}

// ==================== 工具方法 ====================

std::string SimulationClock::time_point_to_string(const SimulationTimePoint& time_point) {
    std::ostringstream oss;
    oss << "仿真时间: " << std::fixed << std::setprecision(3) << time_point.simulation_time << "s";
    oss << " (步: " << time_point.step_number << ")";
    return oss.str();
}

std::string SimulationClock::duration_to_string(const SimulationDuration& duration) {
    std::ostringstream oss;
    oss << "持续时间: " << std::fixed << std::setprecision(3) << duration.duration << "s";
    oss << " (帧: " << duration.frames << ")";
    return oss.str();
}

double SimulationClock::simulation_time_to_real_time(double sim_time) const {
    return sim_time / config.time_scale;
}

double SimulationClock::real_time_to_simulation_time(double real_time) const {
    return real_time * config.time_scale;
}

std::string SimulationClock::generate_status_report() const {
    std::ostringstream oss;
    oss << "=== 仿真时钟状态报告 ===\n";
    oss << "运行状态: " << (is_clock_running() ? "运行中" : "已停止") << "\n";
    oss << "暂停状态: " << (is_clock_paused() ? "已暂停" : "运行中") << "\n";
    oss << "时间模式: " << static_cast<int>(get_time_mode()) << "\n";
    oss << "当前时间: " << time_point_to_string(get_current_simulation_time_point()) << "\n";
    oss << "时间缩放因子: " << get_simulation_time_scale() << "\n";
    // 同步状态输出已移除
    return oss.str();
}

// 性能报告生成功能已移除

// ==================== 私有辅助方法 ====================

// FPS历史接口已移除

double SimulationClock::clamp_time_step(double step) const {
    return config.time_step; // 使用固定的时间步长
}

} // namespace VFT_SMF 