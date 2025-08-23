/**
 * @file EventDrivenMain_NewArchitecture.cpp
 * @brief 虚拟试飞主架构：使用时钟系统进行严格同步控制，使用事件驱动多线程仿真，使用数字孪生的5大代理模型，使用集中式的事件监测和控制器管理2大处理单元，支持大型分布式仿真系统进而全要素场景下的虚拟试飞实验。
 * @author VFT_SMF Development Team
 * @date 2025-08-21
 */

// 包含C++系统头文件
#include <iostream>
#include <memory>
#include <chrono>
#include <thread>
#include <iomanip>
#include <sstream>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>

// 包含VFT_SMF仿真系统头文件
#include "AgentThreadFunctions.hpp"
#include "../../F_ScenarioModelling/A_FlightPlanParser/FlightPlanParser.hpp"
#include "../../C_EnvirnomentAgentModel/EnvironmentAgent.hpp"
#include "../../B_AircraftAgentModel/AircraftAgent.hpp"
#include "../../A_PilotAgentModel/PilotAgent.hpp"
#include "../../G_SimulationManager/LogAndData/DataRecorder.hpp"
#include "../../G_SimulationManager/A_TimeSYNC/Simulation_Clock.hpp"
#include "../../G_SimulationManager/C_ConfigManager/ConfigManager.hpp"
#include "../../G_SimulationManager/B_SimManage/Sim_Performance.hpp"

int main() {
    // 设置控制台代码页为UTF-8，用于支持中文显示
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    
    // 创建仿真性能统计组件，开始监控仿真性能
    VFT_SMF::SimManage::SimPerformance performance_stats;
    performance_stats.start();

    // 清理工作目录，删除output目录下的所有文件
    std::string output_dir = "output";
    std::string command = "rm -rf " + output_dir + "/*";
    system(command.c_str());
    
    try {

        // ==================== 步骤1: 加载仿真配置文件，用于配置仿真参数 ====================
        VFT_SMF::Config::ConfigManager config_manager("config/SimulationConfig.json");
        if (!config_manager.loadConfig()) {
            std::cout << "配置文件加载失败，使用默认配置" << std::endl;
        }
        
        const auto& simulation_config = config_manager.getSimulationConfig();
        const auto& log_config = config_manager.getLogConfig();
        const auto& data_recorder_config = config_manager.getDataRecorderConfig();
        const auto& simulation_params = config_manager.getSimulationParams();
        
        std::cout << "\n主函数步骤1: 仿真配置加载完成" << std::endl;
        
        // ==================== 步骤2: 初始化日志系统（由配置决定是否启用） ====================
        if (log_config.enable_logging) {
            VFT_SMF::initializeGlobalLogger(log_config.brief_log_file, log_config.detail_log_file, log_config.console_output);
            std::cout << "\n主函数步骤2: 日志系统初始化完成 (console=" << (log_config.console_output ? "on" : "off") << ")" << std::endl;
        } else {
            std::cout << "\n主函数步骤2: 日志系统已禁用 (enable_logging=false)" << std::endl;
        }
        
        // ==================== 步骤3: 创建全局共享数据空间，用于存储仿真数据 ====================
        auto shared_data_space_ptr = std::make_shared<VFT_SMF::GlobalShared_DataSpace::GlobalSharedDataSpace>();
        std::cout << "\n主函数步骤3: 全局共享数据空间创建完成" << std::endl;
        
        // ==================== 步骤4: 解析并存储飞行计划数据到共享数据空间，用于仿真 ====================
        std::string flight_plan_file = simulation_config.flight_plan_file;
        std::unique_ptr<VFT_SMF::FlightPlanParser> flight_plan_parser = 
            std::make_unique<VFT_SMF::FlightPlanParser>(flight_plan_file);
        
        if (!flight_plan_parser->parse_and_store_flight_plan_data(shared_data_space_ptr.get(), flight_plan_file)) {
            std::cout << "\n主函数步骤4: 飞行计划数据存储失败" << std::endl;
            return -1;
        }
        else  std::cout << "\n主函数步骤4: 飞行计划解析完成" << std::endl;
        
        // 解析并初始化计划控制器库
        if (!flight_plan_parser->record_initial_data(shared_data_space_ptr.get())) {
            std::cout << "\n主函数步骤4.1: 计划控制器库初始化失败" << std::endl;
            return -1;
        }
        else  std::cout << "\n主函数步骤4.1: 计划控制器库初始化完成" << std::endl;
        
        // ==================== 步骤5: 创建数据记录器，用于记录仿真数据 ====================
        std::cout << "调试: 数据记录器配置 - output_directory: " << data_recorder_config.output_directory << ", buffer_size: " << std::to_string(data_recorder_config.buffer_size) << std::endl;
        VFT_SMF::initializeGlobalDataRecorder(data_recorder_config.output_directory, data_recorder_config.buffer_size);
        std::cout << "\n主函数步骤5: 数据记录器初始化完成" << std::endl;

        // ==================== 步骤6: 创建时钟系统，用于同步各线程 ====================    
        // 从配置文件创建仿真配置
        VFT_SMF::SimulationConfig config;
        config.mode = VFT_SMF::SimulationMode::SCALE_TIME;
        config.sync_strategy = VFT_SMF::TimeSyncStrategy::STEP_BASED_SYNC;
        config.time_scale = simulation_params.time_scale;
        config.time_step = simulation_params.time_step;
        config.step_time_increment = simulation_params.time_step; // 使用相同的时间步长
        config.max_simulation_time = simulation_params.max_simulation_time;
        config.sync_tolerance = simulation_params.sync_tolerance;
        config.enable_sync_monitoring = true;
        config.enable_performance_monitoring = true;
        
        // 创建Simulation_Clock实例
        auto simulation_clock = std::make_unique<VFT_SMF::SimulationClock>(config);
        
        std::cout << "\n主函数步骤6: Simulation_Clock创建完成" << std::endl;
        
        // ==================== 步骤7: 按依赖关系逐个创建代理并等待就绪 ====================
        // 第一层：环境代理（无依赖）
        std::thread environment_thread        (VFT_SMF::environment_thread_function,        shared_data_space_ptr);    // 代理模型：环（环境），未来可以升级为分布式仿真系统中的环境系统
        VFT_SMF::wait_for_environment_thread_ready();
        std::cout << "\n主函数步骤7.1: 环境代理初始化完成" << std::endl;
        
        // 第二层：飞机系统代理（依赖环境）
        std::thread aircraft_system_thread    (VFT_SMF::aircraft_system_thread_function,    shared_data_space_ptr);    // 代理模型：机（飞机系统），未来可以升级为分布式仿真系统中的飞机系统
        VFT_SMF::wait_for_aircraft_system_thread_ready();
        std::cout << "\n主函数步骤7.2: 飞机系统代理初始化完成" << std::endl;
        
        // 第三层：飞行动力学代理（依赖飞机系统和环境）
        std::thread flight_dynamics_thread    (VFT_SMF::flight_dynamics_thread_function,    shared_data_space_ptr);    // 代理模型：动（飞行动力学），未来可以升级为分布式仿真系统中的飞行动力学系统
        VFT_SMF::wait_for_flight_dynamics_thread_ready();
        std::cout << "\n主函数步骤7.3: 飞行动力学代理初始化完成" << std::endl;
        
        // 第四层：飞行员代理（依赖飞行动力学）
        std::thread pilot_thread              (VFT_SMF::pilot_thread_function,              shared_data_space_ptr);    // 代理模型：人（飞行员），未来可以升级为分布式仿真系统中的飞行员系统   
        VFT_SMF::wait_for_pilot_thread_ready();
        std::cout << "\n主函数步骤7.4: 飞行员代理初始化完成" << std::endl;
        
        // 第五层：ATC代理（依赖环境）
        std::thread atc_thread                (VFT_SMF::atc_thread_function,                shared_data_space_ptr);    // 代理模型：控（ATC），未来可以升级为分布式仿真系统中的ATC系统
        VFT_SMF::wait_for_atc_thread_ready();
        std::cout << "\n主函数步骤7.5: ATC代理初始化完成" << std::endl;
        
        // 第六层：事件处理单元（无严格依赖关系）
        std::thread event_monitor_thread      (VFT_SMF::event_monitor_thread_function,      shared_data_space_ptr);    // 处理单元：事件监测，未来可以升级为分布式仿真系统中的中央事件监测系统
        VFT_SMF::wait_for_event_monitor_thread_ready();
        std::cout << "\n主函数步骤7.6: 事件监测单元初始化完成" << std::endl;
        
        std::thread event_dispatcher_thread   (VFT_SMF::event_dispatcher_thread_function, shared_data_space_ptr);    // 处理单元：事件分发，未来可以升级为分布式仿真系统中的中央事件分发系统
        VFT_SMF::wait_for_event_dispatcher_thread_ready();
        std::cout << "\n主函数步骤7.7: 事件分发单元初始化完成" << std::endl;
        
        std::cout << "\n主函数步骤7: 所有代理线程创建并初始化完成" << std::endl;
               
        // ==================== 步骤8: 所有代理已就绪，准备开始仿真 ====================

        std::cout << "\n主函数步骤8: 各线程已就绪" << std::endl;
        
        // ==================== 步骤9: 共享数据空间发布初始化数据，准备开始仿真 ====================
        const double init_time = 0.0;
        shared_data_space_ptr->publishToDataRecorder(init_time);
        std::cout << "\n主函数步骤9: 已发布初始化数据到数据记录器，时间: 0.000000s" << std::endl;

        // ==================== 步骤10: 启动仿真时钟，仿真开始 ====================
       
        simulation_clock->start(shared_data_space_ptr);  // 启动仿真时钟并同步运行状态到共享数据空间
        
        std::cout << "\n主函数步骤10: 仿真时钟已启动，开始仿真" << std::endl;
        
        // ==================== 步骤11: 运行仿真主循环 ====================
        while (simulation_clock->get_current_simulation_time() < simulation_params.max_simulation_time - 0.001) {
            // 推进仿真（用时钟推进，带各工作线程的同步）
            simulation_clock->update(simulation_params.time_step, shared_data_space_ptr);
            
            // 更新控制器执行状态
            VFT_SMF::GlobalSharedDataStruct::ControllerExecutionStatus controller_status;
            controller_status.datasource = "main_thread";
            controller_status.timestamp = VFT_SMF::SimulationTimePoint{};
            
            // 获取计划控制器列表
            const auto& planed_controllers = shared_data_space_ptr->getPlanedControllersLibrary();
            const auto& controllers = planed_controllers.getAllControllers();
            
            // 根据当前仿真时间和事件状态更新控制器运行状态
            const double current_time = simulation_clock->get_current_simulation_time();
            const auto& triggered_events = shared_data_space_ptr->getTriggeredEventLibrary();
            
            for (const auto& controller : controllers) {
                bool is_running = false;
                
                // 检查控制器是否应该运行
                // 这里可以根据实际需求实现更复杂的逻辑
                // 目前简单实现：如果控制器对应的事件已触发，则标记为运行中
                auto events = triggered_events.getTriggeredEvents();
                for (const auto& event : events) {
                    if (event.event_name == controller.event_name) {
                        is_running = true;
                        break;
                    }
                }
                
                controller_status.setControllerStatus(controller.controller_name, is_running);
            }
            
            // 将控制器状态存储到共享数据空间
            shared_data_space_ptr->setControllerExecutionStatus(controller_status, "main_thread");
            
            // 记录每一步的数据：在时钟推进完成后，主线程手动发布本步数据到数据记录器
            const uint64_t step = simulation_clock->get_current_step();
            const double record_time = static_cast<double>(step) * config.time_step;
            shared_data_space_ptr->publishToDataRecorder(record_time);
            
            // 降低CPU占用，避免仿真时钟过快
            std::cout << "虚拟试飞正在运行，仿真时间: " << simulation_clock->get_current_simulation_time() << "s" << std::endl;
            std::this_thread::sleep_for(std::chrono::microseconds(100));
        }
        std::cout << "\n主函数步骤11: 仿真主循环结束" << std::endl;
        
        // ==================== 步骤12: 停止仿真时钟并等待各线程结束 ====================      
        simulation_clock->stop(shared_data_space_ptr); // 停止仿真时钟并同步运行状态到共享数据空间
        std::cout << "\n主函数步骤12: 仿真时钟已停止，等待各线程结束" << std::endl;

        // 等待所有线程结束
        environment_thread.join();
        flight_dynamics_thread.join();
        aircraft_system_thread.join();
        event_monitor_thread.join();
        event_dispatcher_thread.join();
        pilot_thread.join();
        atc_thread.join();
        
        // ==================== 步骤13: 数据记录器输出数据 ====================
        VFT_SMF::globalDataRecorder->flushAllBuffers();
        std::cout << "\n主函数步骤13: 仿真数据记录完成" << std::endl;
        // ==================== 步骤14: 性能统计和总结 ====================
        // 结束性能统计并输出结果
        performance_stats.finish();
        performance_stats.outputCompleteStats(
            simulation_clock->get_current_simulation_time(),
            config.time_step,
            simulation_clock->get_current_step(),
            "使用Simulation_Clock的时钟同步测试"
        );
        if (log_config.enable_logging) {
            std::cout << "如果您想让仿真速度更快，请禁用日志系统，仿真速度会翻倍！" << std::endl;
        } else {
            std::cout << "您已禁用日志系统，若想对程序进行调试，请打开日志系统，不过这将导致仿真速度变慢！" << std::endl;
        }
        std::cout << "\n主函数步骤14: 虚拟试飞结束，主程序成功退出，感谢您的使用，下次任务再见！\n" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "仿真异常: " << std::string(e.what()) << std::endl;      
        return -1;
    }
    
    return 0;
}
