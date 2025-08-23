/**
 * @file B737DigitalTwin.cpp
 * @brief B737飞机数字孪生实现
 * @details 实现B737飞机的数字孪生功能，整合数据层、模型层和服务层
 * @author VFT_SMF Development Team
 * @date 2024
 */

#include "B737DigitalTwin.hpp"
#include "../../G_SimulationManager/LogAndData/Logger.hpp"
#include <iostream>
#include <sstream>

namespace VFT_SMF {

    // ==================== 构造函数 ====================
    
    B737DigitalTwin::B737DigitalTwin(const std::string& id)
        : aircraft_id(id), aircraft_name("B737_" + id), initialized(false), running(false), paused(false) {
        initialize_components();
        update_cached_states(); // 初始化缓存状态
    }

    B737DigitalTwin::B737DigitalTwin(const std::string& id, const std::string& name)
        : aircraft_id(id), aircraft_name(name), initialized(false), running(false), paused(false) {
        initialize_components();
        update_cached_states(); // 初始化缓存状态
    }

    // ==================== 初始化与生命周期 ====================
    
    void B737DigitalTwin::initialize() {
        if (initialized) {
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "B737数字孪生已经初始化: " + aircraft_id);
            return;
        }

        try {
            // 初始化数据层
            // 暂时注释掉，因为B737_DigitalTwin是header-only实现
            // if (data_twin) {
            //     // 数据层初始化逻辑
            //     VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "B737数据层初始化完成: " + aircraft_id);
            // }

            // 初始化模型层
            if (model_twin) {
                // 模型层初始化逻辑
                VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "B737模型层初始化完成: " + aircraft_id);
            }

            // 初始化服务层
            if (state_manager) {
                state_manager->initialize();
                VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "B737状态管理器初始化完成: " + aircraft_id);
            }

            initialized = true;
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "B737数字孪生初始化完成: " + aircraft_id);
        } catch (const std::exception& e) {
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "B737数字孪生初始化失败: " + std::string(e.what()));
            initialized = false;
        }
    }

    void B737DigitalTwin::start() {
        if (!initialized) {
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "B737数字孪生未初始化，无法启动: " + aircraft_id);
            return;
        }

        running = true;
        paused = false;
        VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "B737数字孪生启动: " + aircraft_id);
    }

    void B737DigitalTwin::pause() {
        if (!running) {
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "B737数字孪生未运行，无法暂停: " + aircraft_id);
            return;
        }

        paused = true;
        VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "B737数字孪生暂停: " + aircraft_id);
    }

    void B737DigitalTwin::resume() {
        if (!paused) {
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "B737数字孪生未暂停，无法恢复: " + aircraft_id);
            return;
        }

        paused = false;
        VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "B737数字孪生恢复: " + aircraft_id);
    }

    void B737DigitalTwin::stop() {
        running = false;
        paused = false;
        VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "B737数字孪生停止: " + aircraft_id);
    }

    void B737DigitalTwin::update(double delta_time) {
        if (!running || paused) {
            return;
        }

        // 更新状态管理器
        if (state_manager) {
            // 更新系统状态
            GlobalSharedDataStruct::AircraftSystemState current_state;
            current_state.current_mass = cached_current_mass;
            current_state.current_fuel = cached_fuel_remaining;
            current_state.current_center_of_gravity = cached_center_of_gravity;
            current_state.current_brake_pressure = cached_brake_pressure;
            current_state.current_landing_gear_deployed = cached_gear_position;
            current_state.current_flaps_deployed = cached_flap_position;
            current_state.current_spoilers_deployed = cached_spoiler_position;
            current_state.current_aileron_deflection = cached_aileron_position;
            current_state.current_elevator_deflection = cached_elevator_position;
            current_state.current_rudder_deflection = cached_rudder_position;
            current_state.current_throttle_position = cached_throttle_position;
            current_state.current_engine_rpm = cached_engine_rpm;
            current_state.timestamp = VFT_SMF::SimulationTimePoint{};
            
            state_manager->update_system_state("main", current_state);
        }

        // 更新模型层
        if (model_twin) {
            // 模型层更新逻辑
        }

        // 更新缓存状态
        update_cached_states();
    }

    bool B737DigitalTwin::is_initialized() const {
        return initialized;
    }

    bool B737DigitalTwin::is_running() const {
        return running;
    }

    bool B737DigitalTwin::is_paused() const {
        return paused;
    }

    // ==================== 状态查询接口 ====================
    
    GlobalSharedDataStruct::AircraftSystemState B737DigitalTwin::getAircraftSystemState() const {
        GlobalSharedDataStruct::AircraftSystemState system_state;
        system_state.datasource = "B737_DigitalTwin_" + aircraft_id;
        
        // 设置系统状态数据
        system_state.current_mass = cached_current_mass;
        system_state.current_fuel = cached_fuel_remaining;
        system_state.current_center_of_gravity = cached_center_of_gravity;
        system_state.current_brake_pressure = cached_brake_pressure;
        system_state.current_landing_gear_deployed = cached_gear_position;
        system_state.current_flaps_deployed = cached_flap_position;
        system_state.current_spoilers_deployed = cached_spoiler_position;
        system_state.current_aileron_deflection = cached_aileron_position;
        system_state.current_elevator_deflection = cached_elevator_position;
        system_state.current_rudder_deflection = cached_rudder_position;
        system_state.current_throttle_position = cached_throttle_position;
        system_state.current_engine_rpm = cached_engine_rpm;
        
        // 发动机状态
        system_state.left_engine_failed = false;
        system_state.left_engine_rpm = cached_engine_rpm;
        system_state.right_engine_failed = false;
        system_state.right_engine_rpm = cached_engine_rpm;
        system_state.brake_efficiency = 1.0;
        
        // 时间戳
        system_state.timestamp = VFT_SMF::SimulationTimePoint{};
        
        return system_state;
    }

    // ==================== 控制接口 ====================
    
    void B737DigitalTwin::set_control_inputs(double elevator, double aileron, double rudder, double throttle) {
        cached_elevator_position = elevator;
        cached_aileron_position = aileron;
        cached_rudder_position = rudder;
        cached_throttle_position = throttle;

        if (model_twin) {
            // 通过模型层设置控制输入
        }
    }

    void B737DigitalTwin::set_flap_position(double position) {
        cached_flap_position = position;
    }

    void B737DigitalTwin::set_gear_position(double position) {
        cached_gear_position = position;
    }

    void B737DigitalTwin::set_brake_pressure(double pressure) {
        cached_brake_pressure = pressure;
    }

    void B737DigitalTwin::set_spoiler_position(double position) {
        cached_spoiler_position = position;
    }

    void B737DigitalTwin::set_trim_position(double position) {
        // 设置配平位置
    }

    // ==================== 系统管理接口 ====================
    
    void B737DigitalTwin::add_system(const std::string& name) {
        // 添加系统
    }

    void B737DigitalTwin::update_system_status(const std::string& name, SystemStatus status) {
        // 更新系统状态
    }

    void B737DigitalTwin::update_system_health(const std::string& name, double health) {
        // 更新系统健康度
    }

    void B737DigitalTwin::enable_system_auto_management(const std::string& system_name, bool enabled) {
        // 启用系统自动管理
    }

    // ==================== 自动驾驶和导航接口 ====================
    
    void B737DigitalTwin::enable_auto_pilot(bool enabled) {
        // 启用自动驾驶
    }

    void B737DigitalTwin::set_target_waypoint(const Position3D& waypoint) {
        // 设置目标航路点
    }

    void B737DigitalTwin::execute_flight_plan(const std::string& plan_id) {
        // 执行飞行计划
    }

    bool B737DigitalTwin::is_auto_pilot_enabled() const {
        return false; // 默认未启用
    }

    bool B737DigitalTwin::is_navigation_mode() const {
        return false; // 默认未启用
    }

    // ==================== 安全监控接口 ====================
    
    bool B737DigitalTwin::is_airworthy() const {
        return true; // 默认适航
    }

    bool B737DigitalTwin::check_stall_conditions() const {
        return false; // 默认无失速
    }

    bool B737DigitalTwin::check_overspeed_conditions() const {
        return false; // 默认无超速
    }

    bool B737DigitalTwin::check_altitude_limits() const {
        return true; // 默认在限制内
    }

    std::vector<std::string> B737DigitalTwin::get_warnings() const {
        return std::vector<std::string>();
    }

    std::vector<std::string> B737DigitalTwin::get_active_warnings() const {
        return std::vector<std::string>();
    }

    std::vector<std::string> B737DigitalTwin::get_active_cautions() const {
        return std::vector<std::string>();
    }

    void B737DigitalTwin::emergency_procedures() {
        VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "B737执行紧急程序: " + aircraft_id);
    }

    // ==================== 性能监控接口 ====================
    
    double B737DigitalTwin::get_performance_score() const {
        return 100.0; // 默认满分
    }

    std::string B737DigitalTwin::get_performance_report() const {
        return "B737性能报告: 正常";
    }

    double B737DigitalTwin::get_total_flight_time() const {
        // 暂时返回默认值，后续可以通过状态管理器获取
        return 0.0;
    }

    double B737DigitalTwin::get_total_distance() const {
        // 暂时返回默认值，后续可以通过状态管理器获取
        return 0.0;
    }

    double B737DigitalTwin::get_max_altitude_reached() const {
        // 暂时返回默认值，后续可以通过状态管理器获取
        return 0.0;
    }

    double B737DigitalTwin::get_max_speed_reached() const {
        // 暂时返回默认值，后续可以通过状态管理器获取
        return 0.0;
    }

    // ==================== 状态报告接口 ====================
    
    std::string B737DigitalTwin::get_status() const {
        std::ostringstream oss;
        oss << "B737数字孪生 [" << aircraft_id << "] - " << aircraft_name << "\n";
        oss << "状态: " << (running ? "运行中" : "已停止") << "\n";
        oss << "初始化: " << (initialized ? "已初始化" : "未初始化") << "\n";
        oss << "暂停: " << (paused ? "是" : "否") << "\n";
        return oss.str();
    }

    std::string B737DigitalTwin::get_comprehensive_status_report() const {
        std::ostringstream oss;
        oss << "=== B737数字孪生综合状态报告 ===\n";
        oss << get_status() << "\n";
        oss << "位置: (" << cached_position.x << ", " << cached_position.y << ", " << cached_position.z << ")\n";
        oss << "速度: (" << cached_velocity.vx << ", " << cached_velocity.vy << ", " << cached_velocity.vz << ")\n";
        oss << "高度: " << cached_altitude << " m\n";
        oss << "空速: " << cached_airspeed << " m/s\n";
        oss << "燃油: " << cached_fuel_remaining << " kg\n";
        return oss.str();
    }

    std::string B737DigitalTwin::get_digital_twin_status() const {
        std::ostringstream oss;
        oss << "数据层: " << "header-only实现" << "\n";
        oss << "模型层: " << (model_twin ? "已创建" : "未创建") << "\n";
        oss << "状态管理器: " << (state_manager ? "已创建" : "未创建") << "\n";
        return oss.str();
    }

    // ==================== 飞机系统状态更新接口 ====================
    
    void B737DigitalTwin::updateAircraftSystemState() {
        // 更新缓存状态
        update_cached_states();
        
        VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "B737数字孪生状态已更新: " + aircraft_id);
    }

    // ==================== 私有辅助方法 ====================
    
    void B737DigitalTwin::initialize_components() {
        try {
            // 创建数据层组件
            // 暂时注释掉，因为B737_DigitalTwin是header-only实现
            // data_twin = std::make_shared<SMF::AircraftDigitalTwin::B737::B737DigitalTwin>();
            
            // 创建模型层组件
            model_twin = std::make_unique<SMF::AircraftDigitalTwin::B737::B737AutoFlightControlLaw>();
            
            // 创建服务层组件
            state_manager = std::make_unique<ServiceTwin_StateManager>(aircraft_id, AircraftType::BOEING_737);
            
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "B737数字孪生组件创建完成: " + aircraft_id);
        } catch (const std::exception& e) {
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "B737数字孪生组件创建失败: " + std::string(e.what()));
        }
    }

    void B737DigitalTwin::update_cached_states() {
        // 从飞行计划数据读取初始状态，而不是从状态缓冲区读取
        if (global_data_space) {
            auto flight_plan_data = global_data_space->getFlightPlanData();
            
            // 从飞行计划的全局初始状态中读取飞机系统初始数据
            if (flight_plan_data.global_initial_state.find("aircraft") != flight_plan_data.global_initial_state.end()) {
                try {
                    // 解析飞机初始状态JSON字符串
                    auto aircraft_json = nlohmann::json::parse(flight_plan_data.global_initial_state["aircraft"]);
                    
                    // 使用飞行计划中的初始值更新缓存
                    cached_current_mass = 70000.0; // B737典型质量
                    cached_fuel_remaining = aircraft_json.value("Fuel Quantity", 10000.0);
                    cached_center_of_gravity = 0.25; // 重心位置
                    
                    // 解析刹车状态
                    std::string brake_status = aircraft_json.value("brake_status", "applied");
                    cached_brake_pressure = (brake_status == "applied") ? 100.0 : 0.0;
                    
                    // 解析起落架状态
                    std::string landing_gear_pos = aircraft_json.value("landing_gear_position", "down_locked");
                    cached_gear_position = (landing_gear_pos == "down_locked") ? 1.0 : 0.0;
                    
                    // 解析襟翼状态
                    cached_flap_position = aircraft_json.value("flaps_position", 0.0);
                    cached_spoiler_position = 0.0; // 扰流板收起
                    
                    // 解析操纵面状态
                    cached_aileron_position = aircraft_json.value("aileron_position", 0.0);
                    cached_elevator_position = aircraft_json.value("elevator_position", 0.0);
                    cached_rudder_position = aircraft_json.value("rudder_position", 0.0);
                    cached_throttle_position = aircraft_json.value("throttle_position", 0.3);
                    cached_engine_rpm = 0.0;
                    cached_thrust = 0.0;
                    cached_power_output = 0.0;
                    
                    VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, 
                        "B737数字孪生从飞行计划更新缓存状态: 油门=" + std::to_string(cached_throttle_position) +
                        ", 燃油=" + std::to_string(cached_fuel_remaining));
                } catch (const std::exception& e) {
                    VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, 
                        "B737数字孪生解析飞行计划数据失败: " + std::string(e.what()) + "，使用默认值");
                    // 解析失败时使用默认值
                    set_default_cached_states();
                }
            } else {
                VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, 
                    "B737数字孪生未找到飞行计划中的飞机初始状态，使用默认值");
                // 未找到飞机初始状态时使用默认值
                set_default_cached_states();
            }
        } else {
            VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, 
                "B737数字孪生没有全局数据空间，使用默认值");
            // 没有全局数据空间时使用默认值
            set_default_cached_states();
        }
    }
    
    void B737DigitalTwin::set_default_cached_states() {
        cached_current_mass = 70000.0; // B737典型质量
        cached_fuel_remaining = 20000.0; // 典型燃油量
        cached_center_of_gravity = 0.25; // 重心位置
        cached_brake_pressure = 0.0;
        cached_gear_position = 1.0; // 起落架放下
        cached_flap_position = 0.0;
        cached_spoiler_position = 0.0;
        cached_aileron_position = 0.0;
        cached_elevator_position = 0.0;
        cached_rudder_position = 0.0;
        cached_throttle_position = 0.0;
        cached_engine_rpm = 0.0;
        cached_thrust = 0.0;
        cached_power_output = 0.0;
        
        VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, 
            "B737数字孪生使用默认缓存状态: 油门=" + std::to_string(cached_throttle_position));
    }

    void B737DigitalTwin::validate_initialization() const {
        if (!initialized) {
            throw std::runtime_error("B737数字孪生未初始化: " + aircraft_id);
        }
    }

} // namespace VFT_SMF
