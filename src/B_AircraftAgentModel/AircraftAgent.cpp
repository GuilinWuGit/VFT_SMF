/**
 * @file AircraftAgent.cpp
 * @brief 飞机代理实现
 *         1. 提供飞行器代理类，供飞行场景创建飞行器对象，一个场景可以创建多个飞行器对象
 *         2. 对于每一个飞行器对象，实时更新全局共享数据空间中的飞机系统的实时状态，以此向飞行场景提供服务
 *         3. 对于特定的飞行器对象，提供统一接口、与该飞行器的数字孪生模型对接
 *         
 * @author VFT_SMF Development Team
 * @date 2024
 */

#include "AircraftAgent.hpp"
#include "AircraftDigitalTwinFactory.hpp"
#include "../G_SimulationManager/LogAndData/Logger.hpp"
#include <iostream>
#include <sstream>

namespace VFT_SMF {

    // 飞机代理构造函数
    AircraftAgent::AircraftAgent(const std::string& id, const std::string& name)
        : aircraft_type(AircraftType::BOEING_737), // 默认值，将从飞机配置文件中读取
          current_phase(FlightPhase::PARKED),
          digital_twin_initialized(false) {
        
        // 初始化BaseAgent成员
        agent_id = id;
        agent_name = name;
        is_running = false;
        current_state = AgentState::UNINITIALIZED;
        
        // 从飞机配置文件读取飞机类型
        load_aircraft_config();
        
        VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "飞行器代理创建完成");
    }



    // 初始化飞机代理
    void AircraftAgent::initialize() {
        std::cout << "飞机代理初始化: " << get_agent_name() << std::endl;
        
        // 初始化数字孪生模型
        initialize_digital_twin();
        
        current_state = AgentState::READY;
        std::cout << "飞机代理初始化完成" << std::endl;
    }

    // 启动飞机代理
    void AircraftAgent::start() {
        std::cout << "飞机代理启动: " << get_agent_name() << std::endl;
        
        if (!digital_twin_initialized) {
            std::cout << "警告: 数字孪生模型未初始化，尝试重新初始化" << std::endl;
            initialize_digital_twin();
        }
        
        if (digital_twin) {
            digital_twin->start();
        }
        
        is_running = true;
        current_state = AgentState::RUNNING;
        current_phase = FlightPhase::PREFLIGHT;
    }

    // 暂停飞机代理
    void AircraftAgent::pause() {
        std::cout << "飞机代理暂停: " << get_agent_name() << std::endl;
        
        if (digital_twin) {
            digital_twin->pause();
        }
        
        is_running = false;
        current_state = AgentState::PAUSED;
    }

    // 恢复飞机代理
    void AircraftAgent::resume() {
        std::cout << "飞机代理恢复: " << get_agent_name() << std::endl;
        
        if (digital_twin) {
            digital_twin->resume();
        }
        
        is_running = true;
        current_state = AgentState::RUNNING;
    }

    // 停止飞机代理
    void AircraftAgent::stop() {
        std::cout << "飞机代理停止: " << get_agent_name() << std::endl;
        
        if (digital_twin) {
            digital_twin->stop();
        }
        
        is_running = false;
        current_state = AgentState::STOPPED;
        current_phase = FlightPhase::PARKED;
    }

    // 更新飞机代理状态
    void AircraftAgent::update(double delta_time) {
        if (!is_running) return;
        
        // 处理代理事件队列
        processAgentEventQueue(delta_time);
        
        // 更新数字孪生模型
        if (digital_twin) {
            digital_twin->update(delta_time);
        }
    }

    // 处理飞机代理事件
    void AircraftAgent::handle_event(const Event& event) {
        std::cout << "飞机代理处理事件: " << event.id << std::endl;
        
        switch (event.type) {
            case EventType::SYSTEM_EVENT:
                handle_system_event(event);
                break;
            case EventType::AGENT_EVENT:
                handle_agent_event(event);
                break;
            default:
                std::cout << "未知事件类型" << std::endl;
                break;
        }
    }

    // 发送飞机代理事件
    void AircraftAgent::send_event(const Event& event) {
        std::cout << "飞机代理发送事件: " << event.id << std::endl;
        // 这里可以添加事件发送逻辑
    }

    // 获取飞机代理状态信息
    std::string AircraftAgent::get_status() const {
        std::ostringstream oss;
        oss << "飞机代理 [" << get_agent_id() << "] - " << get_agent_name() << "\n";
        oss << "状态: " << (is_running ? "运行中" : "已停止") << "\n";
        oss << "飞机类型: " << static_cast<int>(aircraft_type) << "\n";
        oss << "飞行阶段: " << static_cast<int>(current_phase) << "\n";
        oss << "数字孪生: " << (digital_twin_initialized ? "已初始化" : "未初始化") << "\n";
        
        if (digital_twin) {
            oss << "数字孪生状态: " << digital_twin->get_status() << "\n";
        }
        
        return oss.str();
    }

    // 检查飞机代理是否就绪
    bool AircraftAgent::is_ready() const {
        return current_state == AgentState::READY && digital_twin_initialized;
    }

    // 从共享数据空间加载飞机配置（此时共享数据空间已经初始化）
    void AircraftAgent::load_aircraft_config() {
        try {
            // 从共享数据空间加载飞机配置
            // 暂时使用硬编码的映射，后续可以改为从共享数据空间读取配置
            if (agent_id == "Aircraft_001") {
                aircraft_type = AircraftType::BOEING_737; // B737-800
                VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "飞机 " + agent_id + " 配置加载完成: B737-800");
            } else if (agent_id == "Aircraft_002") {
                aircraft_type = AircraftType::AIRBUS_A320; // A320
                VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "飞机 " + agent_id + " 配置加载完成: A320");
            } else if (agent_id == "B737_Test") {
                aircraft_type = AircraftType::BOEING_737; // B737测试
                VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "飞机 " + agent_id + " 配置加载完成: B737测试");
            } else {
                aircraft_type = AircraftType::BOEING_737; // 默认B737
                VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "飞机 " + agent_id + " 使用默认配置: B737");
            }
            
            // TODO: 从共享数据空间读取详细配置
            // 例如：从GlobalSharedDataSpace中读取飞机型号、配置参数等
            // shared_data_space->get_aircraft_config(agent_id);
            
        }
        catch (const std::exception& e) {
            VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "飞机配置加载失败: " + std::string(e.what()) + "，使用默认配置");
            aircraft_type = AircraftType::BOEING_737; // 默认B737
        }
    }

    // 初始化数字孪生模型
    void AircraftAgent::initialize_digital_twin() {
        std::cout << "初始化数字孪生模型..." << std::endl;
        
        try {
            // 使用工厂模式创建数字孪生
            digital_twin = AircraftDigitalTwinFactory::create_digital_twin(aircraft_type, agent_id);
            
            if (digital_twin) {
                digital_twin->initialize();
                digital_twin_initialized = true;
                std::cout << "数字孪生模型初始化完成: " << agent_id << std::endl;
            } else {
                std::cout << "数字孪生模型创建失败" << std::endl;
            }
        } catch (const std::exception& e) {
            std::cout << "数字孪生模型初始化失败: " << e.what() << std::endl;
            digital_twin_initialized = false;
        }
    }

    // 检查数字孪生模型是否就绪
    bool AircraftAgent::is_digital_twin_ready() const {
        return digital_twin_initialized && digital_twin && digital_twin->is_initialized();
    }

    // 设置数字孪生
    void AircraftAgent::set_digital_twin(std::unique_ptr<IAircraftDigitalTwin> twin) {
        digital_twin = std::move(twin);
        
        if (digital_twin) {
            digital_twin_initialized = true;
            std::cout << "数字孪生设置完成" << std::endl;
        }
    }





    // 检查数字孪生是否初始化
    bool AircraftAgent::is_digital_twin_initialized() const {
        return digital_twin && digital_twin->is_initialized();
    }

    // 检查数字孪生是否运行
    bool AircraftAgent::is_digital_twin_running() const {
        return digital_twin && digital_twin->is_running();
    }

    // 检查数字孪生是否暂停
    bool AircraftAgent::is_digital_twin_paused() const {
        return digital_twin && digital_twin->is_paused();
    }

    // 更新飞机系统状态
    void AircraftAgent::updateAircraftSystemState() {
        if (digital_twin) {
            digital_twin->updateAircraftSystemState();
        }
    }
    
    // 获取飞机系统状态
    VFT_SMF::GlobalSharedDataStruct::AircraftSystemState AircraftAgent::getAircraftSystemState() const {
        if (digital_twin) {
            return digital_twin->getAircraftSystemState();
        }
        
        // 如果数字孪生不可用，返回默认状态
        VFT_SMF::GlobalSharedDataStruct::AircraftSystemState system_state;
        system_state.datasource = "aircraft_system";
        
        // 设置默认系统状态数据
        system_state.current_mass = 70000.0; // kg，B737典型质量
        system_state.current_fuel = 20000.0; // kg，典型燃油量
        system_state.current_center_of_gravity = 0.25; // 相对于机翼前缘的位置
        system_state.current_brake_pressure = 0.0;
        system_state.current_landing_gear_deployed = 1.0; // 起落架放下
        system_state.current_flaps_deployed = 0.0;
        system_state.current_spoilers_deployed = 0.0;
        system_state.current_aileron_deflection = 0.0;
        system_state.current_elevator_deflection = 0.0;
        system_state.current_rudder_deflection = 0.0;
        system_state.current_throttle_position = 0.0;
        system_state.current_engine_rpm = 0.0;
        
        // 默认发动机状态
        system_state.left_engine_failed = false;
        system_state.left_engine_rpm = 0.0;
        system_state.right_engine_failed = false;
        system_state.right_engine_rpm = 0.0;
        system_state.brake_efficiency = 1.0;
        
        // 更新时间戳
        system_state.timestamp = VFT_SMF::SimulationTimePoint{};
        
        return system_state;
    }



    // 获取综合状态报告
    std::string AircraftAgent::get_comprehensive_status_report() const {
        std::ostringstream oss;
        oss << "=== 飞机代理综合状态报告 ===\n";
        oss << get_status() << "\n";
        oss << "--- 数字孪生状态 ---\n";
        oss << get_digital_twin_status() << "\n";
        return oss.str();
    }

    // 获取数字孪生状态
    std::string AircraftAgent::get_digital_twin_status() const {
        std::ostringstream oss;
        if (digital_twin) {
            oss << "数字孪生: " << (digital_twin->is_initialized() ? "已初始化" : "未初始化") << "\n";
            oss << "运行状态: " << (digital_twin->is_running() ? "运行中" : 
                                 (digital_twin->is_paused() ? "暂停" : "停止")) << "\n";
            oss << digital_twin->get_digital_twin_status();
        } else {
            oss << "数字孪生: 未创建\n";
        }
        return oss.str();
    }

    // 处理系统事件
    void AircraftAgent::handle_system_event(const Event& event) {
        std::cout << "飞机处理系统事件: " << event.id << std::endl;
        // 这里可以添加系统事件处理逻辑
    }

    // 处理代理事件
    void AircraftAgent::handle_agent_event(const Event& event) {
        std::cout << "飞机处理代理事件: " << event.id << std::endl;
        // 这里可以添加代理事件处理逻辑
    }

    // 验证数字孪生组件
    void AircraftAgent::validate_digital_twin_components() const {
        if (!digital_twin_initialized) {
            throw std::runtime_error("数字孪生模型未初始化");
        }
    }

    // ==================== 统一控制器接口实现 ====================

    // 执行控制器
    bool AircraftAgent::executeController(const std::string& controller_name, 
                                        const std::map<std::string, std::string>& params,
                                        double current_time) {
        VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, 
            "飞机代理执行控制器: " + controller_name + " (时间: " + std::to_string(current_time) + "s)");
        
        bool executed = false;
        
        if (controller_name == "Left_Engine_Out") {
            executed = executeLeftEngineOutController(params, current_time);
        } else if (controller_name == "Break_Half") {
            executed = executeBreakHalfController(params, current_time);
        } else {
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, 
                "飞机代理: 未知的控制器名称: " + controller_name);
        }
        
        if (executed) {
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, 
                "飞机代理控制器执行成功: " + controller_name);
        } else {
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, 
                "飞机代理控制器执行失败: " + controller_name);
        }
        
        return executed;
    }

    // 处理代理事件队列
    int AircraftAgent::processAgentEventQueue(double current_time) {
        if (!shared_data_space) {
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "飞机代理: 全局共享数据空间未设置");
            return 0;
        }
        
        int processed_count = 0;
        VFT_SMF::GlobalSharedDataStruct::AgentEventQueueItem queue_item;
        
        // 处理代理事件队列中的所有事件
        while (shared_data_space->dequeueAgentEvent(get_agent_id(), queue_item)) {
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, 
                "飞机代理处理事件: " + queue_item.event.event_name + 
                " (控制器: " + queue_item.controller_type + "::" + queue_item.controller_name + ")");
            
            // 执行对应的控制器
            bool executed = executeController(queue_item.controller_name, queue_item.parameters, current_time);
            
            if (executed) {
                processed_count++;
                VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, 
                    "飞机代理事件处理成功: " + queue_item.event.event_name);
            } else {
                VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, 
                    "飞机代理事件处理失败: " + queue_item.event.event_name);
            }
        }
        
        if (processed_count > 0) {
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, 
                "飞机代理本步处理事件数量: " + std::to_string(processed_count));
        }
        
        return processed_count;
    }

    // ==================== 飞机控制器具体实现 ====================

    // 执行左发动机失效控制器
    bool AircraftAgent::executeLeftEngineOutController(const std::map<std::string, std::string>& params, double current_time) {
        VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "飞机代理: 执行左发动机失效控制器");
        
        if (!shared_data_space) {
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "飞机代理: 全局共享数据空间未设置");
            return false;
        }
        
        // 更新飞机系统状态：左发动机失效
        auto system_state = shared_data_space->getAircraftSystemState();
        system_state.left_engine_failed = true;
        system_state.left_engine_rpm = 0.0;  // 左发动机转速为0
        system_state.datasource = "Aircraft_001_Left_Engine_Out_Controller";
        shared_data_space->setAircraftSystemState(system_state);
        
        VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "飞机代理: 左发动机失效，left_engine_failed设置为true，left_engine_rpm设置为0");
        return true;
    }

    // 执行刹车效率降低控制器
    bool AircraftAgent::executeBreakHalfController(const std::map<std::string, std::string>& params, double current_time) {
        VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "飞机代理: 执行刹车效率降低控制器");
        
        if (!shared_data_space) {
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "飞机代理: 全局共享数据空间未设置");
            return false;
        }
        
        // 更新飞机系统状态：刹车效率降低
        auto system_state = shared_data_space->getAircraftSystemState();
        system_state.brake_efficiency = 0.5;  // 刹车效率降低到50%
        system_state.datasource = "Aircraft_001_Break_Half_Controller";
        shared_data_space->setAircraftSystemState(system_state);
        
        VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "飞机代理: 刹车效率降低，brake_efficiency设置为0.5");
        return true;
    }

} // namespace VFT_SMF 