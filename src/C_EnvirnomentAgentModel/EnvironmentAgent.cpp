/**
 * @file EnvironmentAgent.cpp
 * @brief 环境代理实现
 * @author VFT_SMF Development Team
 * @date 2024
 */

#include "EnvironmentAgent.hpp"
#include "../E_GlobalSharedDataSpace/GlobalSharedDataSpace.hpp"
#include "../G_SimulationManager/LogAndData/Logger.hpp"
#include <iostream>
#include <sstream>
#include <cmath>

// 定义M_PI（Windows上可能未定义）
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace VFT_SMF {

    // ==================== EnvironmentModel实现 ====================
    
    EnvironmentModel::EnvironmentModel(EnvironmentType type)
        : env_type(type),
          current_weather(WeatherCondition::CLEAR),
          weather_stability(0.8),
          change_rate(0.1),
          gen(rd()),
          dist(0.0, 1.0) {
    }

    void EnvironmentModel::step(double delta_time) {
        // 基于天气稳定性决定是否发生天气变化
        if (dist(gen) > weather_stability) {
            // 天气可能发生变化
            double change_probability = change_rate * delta_time;
            if (dist(gen) < change_probability) {
                // 随机选择新的天气状况
                int weather_options = static_cast<int>(WeatherCondition::TURBULENT) + 1;
                int new_weather = static_cast<int>(dist(gen) * weather_options);
                current_weather = static_cast<WeatherCondition>(new_weather);
            }
        }
    }

    // ==================== EnvironmentAgent实现 ====================
    
    EnvironmentAgent::EnvironmentAgent(const std::string& id, const std::string& name, 
                                     const VFT_SMF::EnvirDataSpace::EnvironmentAgentConfig& env_config,
                                     EnvironmentType type)
        : config(env_config),
          environment_type(type),
          total_events_generated(0),
          total_weather_changes(0),
          average_update_time(0.0),
          airport_code(env_config.airport_code),
          runway_code(env_config.runway_code),
          gen(rd()),
          dist(0.0, 1.0) {
        
        agent_id = id;
        agent_name = name;
        is_running = false;
        current_state = AgentState::UNINITIALIZED;
        
        // 创建环境模型
        environment_model = std::make_unique<EnvironmentModel>(type);
        
        // 创建配置管理器
        config_manager = std::make_unique<EnvironmentConfigManager>("../../src/C_EnvirnomentAgentModel/");
        
        // 初始化环境数据
        initialize_environment_data();
        
        // 设置默认环境模型名称
        environment_model_name = "Default_Environment";
        
        VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "环境代理创建完成");
    }

    void EnvironmentAgent::initialize() {
        VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "环境代理初始化: " + get_agent_name());
        set_current_state(AgentState::READY);
    }

    void EnvironmentAgent::start() {
        VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "环境代理启动: " + get_agent_name());
        set_current_state(AgentState::RUNNING);
    }

    void EnvironmentAgent::pause() {
        VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "环境代理暂停: " + get_agent_name());
        set_current_state(AgentState::PAUSED);
    }

    void EnvironmentAgent::resume() {
        VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "环境代理恢复: " + get_agent_name());
        set_current_state(AgentState::RUNNING);
    }

    void EnvironmentAgent::stop() {
        VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "环境代理停止: " + get_agent_name());
        set_current_state(AgentState::STOPPED);
    }

    void EnvironmentAgent::update(double delta_time) {
        if (get_current_state() != AgentState::RUNNING) {
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "环境代理状态不是RUNNING，当前状态: " + std::to_string(static_cast<int>(get_current_state())));
            return;
        }
        
        // 处理代理事件队列
        processAgentEventQueue(delta_time);
        
        // 记录时钟通知
        VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "环境代理 [" + get_agent_id() + "] 收到时钟通知，时间步长: " + 
                           std::to_string(delta_time) + " 秒");
        
        // 更新环境模型
        environment_model->step(delta_time);
        
        // 更新环境数据
        update_environment_data(delta_time);
        
        // 生成环境事件
        EnvironmentEvent current_event = generate_environment_event();
        
        // 记录事件生成
        VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "环境代理生成事件: " + current_event.event_name + 
                           " (严重程度: " + std::to_string(current_event.severity) + ")");
        
        // 更新性能统计
        total_events_generated++;
        
        // 记录当前状态
        VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "环境代理状态 - 天气: " + std::to_string(static_cast<int>(get_current_weather())) + 
                           ", 稳定性: " + std::to_string(environment_model->get_weather_stability()) + 
                           ", 变化率: " + std::to_string(environment_model->get_change_rate()));
        
        // 作为数据制造者，将环境数据发布到全局共享数据空间
        publish_to_global_data_space();
    }

    void EnvironmentAgent::handle_event(const Event& event) {
        VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "环境代理处理事件: " + event.id);
        
        // 根据事件类型处理
        switch (event.type) {
            case EventType::ENVIRONMENT_EVENT:
                // 处理环境相关事件
                VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "处理环境事件: " + event.description);
                break;
            case EventType::SYSTEM_EVENT:
                // 处理系统事件
                VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "处理系统事件: " + event.description);
                break;
            default:
                // 其他类型事件
                VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "处理其他类型事件: " + event.description);
                break;
        }
    }

    void EnvironmentAgent::send_event(const Event& event) {
        VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "环境代理发送事件: " + event.id);
        // 这里可以添加事件发送逻辑
    }

    std::string EnvironmentAgent::get_status() const {
        std::ostringstream oss;
        oss << "环境代理 [" << get_agent_id() << "] - " << get_agent_name() << "\n";
        oss << "状态: " << (get_current_state() == AgentState::RUNNING ? "运行中" : "已停止") << "\n";
        oss << "机场代码: " << airport_code << "\n";
        oss << "跑道代码: " << runway_code << "\n";
        oss << "环境类型: " << static_cast<int>(environment_type) << "\n";
        oss << "当前天气: " << static_cast<int>(get_current_weather()) << "\n";
        oss << "天气稳定性: " << environment_model->get_weather_stability() << "\n";
        return oss.str();
    }

    bool EnvironmentAgent::is_ready() const {
        return get_current_state() == AgentState::READY || get_current_state() == AgentState::RUNNING;
    }

    // ==================== 环境代理特定方法 ====================
    
    void EnvironmentAgent::set_weather_condition(WeatherCondition weather) {
        environment_model->set_weather_condition(weather);
        total_weather_changes++;
    }

    void EnvironmentAgent::set_runway_condition(const std::string& condition) {
        environment_data.runway_data.condition = condition;
        
        // 根据跑道状况调整摩擦系数
        if (condition == "干") {
            environment_data.runway_data.friction_coefficient = 0.8;
        } else if (condition == "湿") {
            environment_data.runway_data.friction_coefficient = 0.6;
        } else if (condition == "雪") {
            environment_data.runway_data.friction_coefficient = 0.3;
        } else if (condition == "冰") {
            environment_data.runway_data.friction_coefficient = 0.1;
        }
    }

    void EnvironmentAgent::set_wind_conditions(double speed, double direction) {
        environment_data.wind_data.wind_speed = speed;
        environment_data.wind_data.wind_direction = direction;
        
        // 计算风分量
        double wind_rad = direction * M_PI / 180.0;
        environment_data.wind_data.headwind_component = speed * cos(wind_rad);
        environment_data.wind_data.crosswind_component = speed * sin(wind_rad);
    }

    void EnvironmentAgent::set_atmospheric_conditions(double temperature, double pressure, double humidity) {
        environment_data.atmospheric_data.temperature = temperature;
        environment_data.atmospheric_data.pressure = pressure;
        environment_data.atmospheric_data.humidity = humidity;
        
        // 更新空气密度
        update_air_density();
    }

    VFT_SMF::EnvirDataSpace::EnvironmentAgentData EnvironmentAgent::get_environment_data() const {
        return environment_data;
    }

    VFT_SMF::EnvirDataSpace::EnvironmentAgentConfig EnvironmentAgent::get_config() const {
        return config;
    }

    EnvironmentEvent EnvironmentAgent::generate_environment_event() {
        // 基于当前环境状态生成事件
        EnvironmentEventType event_type;
        std::string event_name;
        double severity = 0.5;
        
        // 根据天气状况选择事件类型
        WeatherCondition current_weather = get_current_weather();
        switch (current_weather) {
            case WeatherCondition::THUNDERSTORM:
                event_type = EnvironmentEventType::THUNDERSTORM_APPROACH;
                event_name = "雷暴接近警告";
                severity = 0.8;
                break;
            case WeatherCondition::TURBULENT:
                event_type = EnvironmentEventType::TURBULENCE_ALERT;
                event_name = "湍流警告";
                severity = 0.7;
                break;
            case WeatherCondition::WINDY:
                if (environment_data.wind_data.wind_speed > 15.0) {
                    event_type = EnvironmentEventType::WIND_SHEAR_ALERT;
                    event_name = "风切变警告";
                    severity = 0.6;
                } else {
                    event_type = EnvironmentEventType::WIND_SHIFT;
                    event_name = "风向变化";
                    severity = 0.4;
                }
                break;
            case WeatherCondition::FOG:
                event_type = EnvironmentEventType::VISIBILITY_CHANGE;
                event_name = "能见度变化";
                severity = 0.5;
                break;
            default:
                event_type = EnvironmentEventType::WEATHER_CHANGE;
                event_name = "天气变化";
                severity = 0.3;
                break;
        }
        
        EnvironmentEvent event(event_type, event_name, severity);
        
        // 添加事件参数
        event.parameters["wind_speed"] = environment_data.wind_data.wind_speed;
        event.parameters["temperature"] = environment_data.atmospheric_data.temperature;
        event.parameters["visibility"] = environment_data.atmospheric_data.visibility;
        
        return event;
    }

    void EnvironmentAgent::process_environment_event(const EnvironmentEvent& event) {
        // 处理环境事件
        std::cout << "处理环境事件: " << event.event_name << std::endl;
        
        // 将事件添加到最近事件列表
        recent_events.push_back(event);
        
        // 保持最近事件列表大小
        if (recent_events.size() > 10) {
            recent_events.erase(recent_events.begin());
        }
    }

    std::vector<EnvironmentEvent> EnvironmentAgent::get_recent_events() const {
        return recent_events;
    }

    double EnvironmentAgent::get_performance_score() const {
        // 基于事件生成数量和天气变化计算性能分数
        double event_score = total_events_generated > 0 ? 
            std::min(1.0, total_events_generated / 100.0) : 0.0;
        double weather_score = total_weather_changes > 0 ? 
            std::min(1.0, total_weather_changes / 50.0) : 0.0;
        
        return (event_score + weather_score) / 2.0;
    }

    std::string EnvironmentAgent::get_performance_report() const {
        std::ostringstream oss;
        oss << "环境代理性能报告 [" << get_agent_id() << "]\n";
        oss << "总生成事件数: " << total_events_generated << "\n";
        oss << "总天气变化数: " << total_weather_changes << "\n";
        oss << "性能分数: " << get_performance_score() * 100 << "%\n";
        return oss.str();
    }

    WeatherCondition EnvironmentAgent::get_current_weather() const {
        return environment_model->get_current_weather();
    }

    // ==================== 私有方法 ====================
    
    void EnvironmentAgent::initialize_environment_data() {
        VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "初始化环境数据");
        
        // 尝试从配置文件加载数据
        if (config_manager && !environment_model_name.empty() && environment_model_name != "Default_Environment") {
            if (config_manager->load_environment_config(environment_model_name)) {
                current_config = config_manager->get_environment_config(environment_model_name);
                
                VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "从配置文件加载环境数据: " + environment_model_name);
                
                // 从配置文件初始化跑道数据
                environment_data.runway_data.length = current_config.runway_data.length;
                environment_data.runway_data.width = current_config.runway_data.width;
                environment_data.runway_data.surface_type = current_config.runway_data.surface_type;
                environment_data.runway_data.friction_coefficient = current_config.runway_data.friction_coefficient;
                environment_data.runway_data.condition = current_config.runway_data.condition;
                environment_data.runway_data.is_available = current_config.runway_data.is_available;
                environment_data.runway_data.elevation = current_config.runway_data.elevation;
                environment_data.runway_data.slope = current_config.runway_data.slope;
                
                // 从配置文件初始化大气数据
                environment_data.atmospheric_data.temperature = current_config.atmospheric_data.temperature;
                environment_data.atmospheric_data.pressure = current_config.atmospheric_data.pressure;
                environment_data.atmospheric_data.humidity = current_config.atmospheric_data.humidity;
                environment_data.atmospheric_data.visibility = current_config.atmospheric_data.visibility;
                environment_data.atmospheric_data.density_altitude = current_config.atmospheric_data.density_altitude;
                environment_data.atmospheric_data.dew_point = current_config.atmospheric_data.dew_point;
                environment_data.atmospheric_data.air_density = current_config.atmospheric_data.air_density;
                environment_data.atmospheric_data.cloud_cover = current_config.atmospheric_data.cloud_cover;
                environment_data.atmospheric_data.cloud_base = current_config.atmospheric_data.cloud_base;
                
                // 从配置文件初始化风数据
                environment_data.wind_data.wind_speed = current_config.wind_data.wind_speed;
                environment_data.wind_data.wind_direction = current_config.wind_data.wind_direction;
                environment_data.wind_data.gust_speed = current_config.wind_data.gust_speed;
                environment_data.wind_data.crosswind_component = current_config.wind_data.crosswind_component;
                environment_data.wind_data.headwind_component = current_config.wind_data.headwind_component;
                environment_data.wind_data.wind_shear = current_config.wind_data.wind_shear;
                environment_data.wind_data.wind_condition = current_config.wind_data.wind_condition;
                environment_data.wind_data.is_turbulent = current_config.wind_data.is_turbulent;
                
                // 更新机场和跑道代码
                airport_code = current_config.environment_model.airport_code;
                runway_code = current_config.environment_model.runway_code;
                
                // 设置环境模型的天气参数
                if (environment_model) {
                    environment_model->set_weather_stability(current_config.weather_model.weather_stability);
                    environment_model->set_change_rate(current_config.weather_model.change_rate);
                }
                
                VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "配置文件加载成功: " + current_config.environment_model.name);
                return;
            } else {
                VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "配置文件加载失败，使用默认值: " + environment_model_name);
            }
        }
        
        // 使用默认值（原有的硬编码数据）
        VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "使用默认环境数据");
        
        // 初始化跑道数据
        environment_data.runway_data.length = 3800.0;  // 3800米
        environment_data.runway_data.width = 60.0;     // 60米
        environment_data.runway_data.surface_type = "沥青";
        environment_data.runway_data.friction_coefficient = 0.8;
        environment_data.runway_data.condition = "干";
        environment_data.runway_data.is_available = true;
        environment_data.runway_data.elevation = 35.0;  // 35米海拔
        environment_data.runway_data.slope = 0.0;       // 无坡度
        
        // 初始化大气数据
        environment_data.atmospheric_data.temperature = 15.0;  // 15°C
        environment_data.atmospheric_data.pressure = 1013.25;  // 1013.25 hPa
        environment_data.atmospheric_data.humidity = 50.0;     // 50%
        environment_data.atmospheric_data.visibility = 10000.0; // 10公里
        environment_data.atmospheric_data.density_altitude = 35.0;
        environment_data.atmospheric_data.dew_point = 5.0;
        environment_data.atmospheric_data.air_density = 1.225; // kg/m³
        environment_data.atmospheric_data.cloud_cover = "少云";
        environment_data.atmospheric_data.cloud_base = 1000.0; // 1000米
        
        // 初始化风数据
        environment_data.wind_data.wind_speed = 5.0;      // 5 m/s
        environment_data.wind_data.wind_direction = 0.0;   // 0度
        environment_data.wind_data.gust_speed = 0.0;
        environment_data.wind_data.crosswind_component = 0.0;
        environment_data.wind_data.headwind_component = 5.0;
        environment_data.wind_data.wind_shear = 0.0;
        environment_data.wind_data.wind_condition = "轻风";
        environment_data.wind_data.is_turbulent = false;
    }

    void EnvironmentAgent::update_environment_data(double delta_time) {
        // 使用配置文件中的更新参数，如果没有则使用默认值
        double temp_min = -0.1, temp_max = 0.1;
        double wind_min = -2.0, wind_max = 2.0;
        double pressure_min = -0.5, pressure_max = 0.5;
        
        // 如果配置文件已加载，使用配置文件中的参数
        if (config_manager && !environment_model_name.empty() && environment_model_name != "Default_Environment") {
            temp_min = current_config.update_parameters.temperature_change_range.first;
            temp_max = current_config.update_parameters.temperature_change_range.second;
            wind_min = current_config.update_parameters.wind_change_range.first;
            wind_max = current_config.update_parameters.wind_change_range.second;
            pressure_min = current_config.update_parameters.pressure_change_range.first;
            pressure_max = current_config.update_parameters.pressure_change_range.second;
        }
        
        // 创建分布对象
        std::uniform_real_distribution<> temp_change(temp_min, temp_max);
        std::uniform_real_distribution<> wind_change(wind_min, wind_max);
        std::uniform_real_distribution<> pressure_change(pressure_min, pressure_max);
        
        // 温度变化
        environment_data.atmospheric_data.temperature += temp_change(gen) * delta_time;
        
        // 风速变化
        environment_data.wind_data.wind_speed += wind_change(gen) * delta_time;
        environment_data.wind_data.wind_speed = std::max(0.0, environment_data.wind_data.wind_speed);
        
        // 气压变化
        environment_data.atmospheric_data.pressure += pressure_change(gen) * delta_time;
        
        // 更新空气密度
        update_air_density();
        
        // 更新风分量
        double wind_rad = environment_data.wind_data.wind_direction * M_PI / 180.0;
        environment_data.wind_data.headwind_component = environment_data.wind_data.wind_speed * cos(wind_rad);
        environment_data.wind_data.crosswind_component = environment_data.wind_data.wind_speed * sin(wind_rad);
    }

    void EnvironmentAgent::update_air_density() {
        // 使用修正的理想气体定律计算空气密度
        double temp_kelvin = environment_data.atmospheric_data.temperature + 273.15;
        double pressure_pa = environment_data.atmospheric_data.pressure * 100.0;
        
        // 计算饱和水汽压（Magnus公式）
        double saturation_vapor_pressure = 6.1094 * exp(17.625 * environment_data.atmospheric_data.temperature / 
                                                       (environment_data.atmospheric_data.temperature + 243.04));
        
        // 计算实际水汽压
        double actual_vapor_pressure = saturation_vapor_pressure * environment_data.atmospheric_data.humidity / 100.0;
        
        // 气体常数 R = 287.05 J/(kg·K) for dry air
        const double R_dry = 287.05;
        
        // 计算空气密度 (kg/m³)
        double air_density = (pressure_pa - 0.378 * actual_vapor_pressure) / (R_dry * temp_kelvin);
        
        // 确保空气密度在合理范围内
        air_density = std::max(0.5, std::min(2.0, air_density));
        
        environment_data.atmospheric_data.air_density = air_density;
    }

    void EnvironmentAgent::publish_to_global_data_space() {
        if (!global_data_space) {
            VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "警告：环境代理未设置全局共享数据空间，无法发布数据");
            return;
        }
        
        // 创建环境全局状态
        VFT_SMF::GlobalSharedDataStruct::EnvironmentGlobalState env_state;
        
        // 从环境数据填充全局状态
        env_state.runway_length = environment_data.runway_data.length;
        env_state.runway_width = environment_data.runway_data.width;
        env_state.friction_coefficient = environment_data.runway_data.friction_coefficient;
        env_state.air_density = environment_data.atmospheric_data.air_density;
        env_state.wind_speed = environment_data.wind_data.wind_speed;
        env_state.wind_direction = environment_data.wind_data.wind_direction;
        
        // 设置时间戳（使用当前仿真时间）
        // 注意：这里需要从仿真时钟获取时间，暂时使用系统时间
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        env_state.timestamp = VFT_SMF::SimulationTimePoint{};
        
        // 将环境状态写入全局共享数据空间，设置正确的数据源
        global_data_space->setEnvironmentState(env_state, get_agent_id());
        
        VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "环境代理 [" + get_agent_id() + "] 已将环境数据发布到全局共享数据空间");
        VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "  - 跑道宽度: " + std::to_string(env_state.runway_width) + " 米");
        VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "  - 风速: " + std::to_string(env_state.wind_speed) + " m/s");
        VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "  - 空气密度: " + std::to_string(env_state.air_density) + " kg/m³");
    }

    // ==================== 统一控制器接口实现 ====================

    bool EnvironmentAgent::executeController(const std::string& controller_name, 
                                          const std::map<std::string, std::string>& params,
                                          double current_time) {
        VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, 
            "环境代理执行控制器: " + controller_name + " (时间: " + std::to_string(current_time) + "s)");
        
        bool executed = false;
        
        if (controller_name == "Runway_Condition_Change") {
            executed = executeRunwayConditionChangeController(params, current_time);
        } else {
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, 
                "环境代理: 未知的控制器名称: " + controller_name);
        }
        
        if (executed) {
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, 
                "环境代理控制器执行成功: " + controller_name);
        } else {
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, 
                "环境代理控制器执行失败: " + controller_name);
        }
        
        return executed;
    }

    int EnvironmentAgent::processAgentEventQueue(double current_time) {
        if (!global_data_space) {
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "环境代理: 全局共享数据空间未设置");
            return 0;
        }
        
        int processed_count = 0;
        VFT_SMF::GlobalSharedDataStruct::AgentEventQueueItem queue_item;
        
        // 处理代理事件队列中的所有事件
        while (global_data_space->dequeueAgentEvent(get_agent_id(), queue_item)) {
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, 
                "环境代理处理事件: " + queue_item.event.event_name + 
                " (控制器: " + queue_item.controller_type + "::" + queue_item.controller_name + ")");
            
            // 执行对应的控制器
            bool executed = executeController(queue_item.controller_name, queue_item.parameters, current_time);
            
            if (executed) {
                processed_count++;
                VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, 
                    "环境代理事件处理成功: " + queue_item.event.event_name);
            } else {
                VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, 
                    "环境代理事件处理失败: " + queue_item.event.event_name);
            }
        }
        
        if (processed_count > 0) {
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, 
                "环境代理本步处理事件数量: " + std::to_string(processed_count));
        }
        
        return processed_count;
    }

    // ==================== 环境控制器具体实现 ====================

    bool EnvironmentAgent::executeRunwayConditionChangeController(const std::map<std::string, std::string>& params, double current_time) {
        VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "环境代理: 执行跑道条件变化控制器");
        
        if (!global_data_space) {
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "环境代理: 全局共享数据空间未设置");
            return false;
        }
        
        // 更新环境状态：跑道条件变化
        auto env_state = global_data_space->getEnvironmentState();
        env_state.friction_coefficient = 0.3;  // 跑道摩擦系数降低到0.3（湿滑跑道）
        env_state.datasource = "Environment_001_Runway_Condition_Change_Controller";
        global_data_space->setEnvironmentState(env_state);
        
        // 同时更新内部环境数据
        environment_data.runway_data.friction_coefficient = 0.3;
        environment_data.runway_data.condition = "湿滑";
        
        VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "环境代理: 跑道条件变化，friction_coefficient设置为0.3，condition设置为湿滑");
        return true;
    }

    // ==================== 环境模型配置驱动实现 ====================

    void EnvironmentAgent::initializeEnvironmentModel(const std::string& model_name) {
        VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "环境代理: 初始化环境模型: " + model_name);
        
        environment_model_name = model_name;
        
        // 使用配置管理器加载环境配置
        if (config_manager && config_manager->load_environment_config(model_name)) {
            current_config = config_manager->get_environment_config(model_name);
            
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "环境代理: 配置文件加载成功: " + current_config.environment_model.name);
            
            // 从配置文件设置环境参数
            airport_code = current_config.environment_model.airport_code;
            runway_code = current_config.environment_model.runway_code;
            
            // 设置环境类型
            if (current_config.environment_model.environment_type == "AIRPORT_RUNWAY") {
                environment_type = EnvironmentType::AIRPORT_RUNWAY;
            } else if (current_config.environment_model.environment_type == "TERMINAL_AREA") {
                environment_type = EnvironmentType::TERMINAL_AREA;
            } else if (current_config.environment_model.environment_type == "APPROACH_ZONE") {
                environment_type = EnvironmentType::APPROACH_ZONE;
            } else if (current_config.environment_model.environment_type == "DEPARTURE_ZONE") {
                environment_type = EnvironmentType::DEPARTURE_ZONE;
            } else if (current_config.environment_model.environment_type == "ENROUTE_ZONE") {
                environment_type = EnvironmentType::ENROUTE_ZONE;
            }
            
            // 设置环境模型的天气参数
            if (environment_model) {
                environment_model->set_weather_stability(current_config.weather_model.weather_stability);
                environment_model->set_change_rate(current_config.weather_model.change_rate);
            }
            
            // 重新初始化环境数据
            initialize_environment_data();
            
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "环境代理: " + model_name + " 模型初始化完成");
            
        } else {
            // 配置文件加载失败，使用默认配置
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "环境代理: 配置文件加载失败，使用默认配置: " + model_name);
            
            // 根据模型名称设置特定的环境参数（原有的硬编码逻辑）
            if (model_name == "PVG_Runway_05") {
                airport_code = "PVG";
                runway_code = "05";
                environment_type = EnvironmentType::AIRPORT_RUNWAY;
                
                if (environment_model) {
                    environment_model->set_weather_stability(0.85);
                    environment_model->set_change_rate(0.08);
                }
                
            } else if (model_name == "PEK_Runway_02") {
                airport_code = "PEK";
                runway_code = "02";
                environment_type = EnvironmentType::AIRPORT_RUNWAY;
                
                if (environment_model) {
                    environment_model->set_weather_stability(0.75);
                    environment_model->set_change_rate(0.12);
                }
                
            } else {
                // 默认配置
                VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "环境代理: 使用默认环境模型配置");
            }
        }
        
        VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "环境代理: 环境模型初始化完成 - " + model_name);
    }

    std::string EnvironmentAgent::getEnvironmentModelConfig() const {
        std::string config_info = "环境模型配置信息:\n";
        config_info += "  - 模型名称: " + environment_model_name + "\n";
        config_info += "  - 机场代码: " + airport_code + "\n";
        config_info += "  - 跑道代码: " + runway_code + "\n";
        config_info += "  - 环境类型: " + std::to_string(static_cast<int>(environment_type)) + "\n";
        
        if (environment_model) {
            config_info += "  - 天气稳定性: " + std::to_string(environment_model->get_weather_stability()) + "\n";
            config_info += "  - 天气变化率: " + std::to_string(environment_model->get_change_rate()) + "\n";
            config_info += "  - 当前天气: " + std::to_string(static_cast<int>(environment_model->get_current_weather())) + "\n";
        }
        
        // 如果配置文件已加载，显示配置文件信息
        if (config_manager && !environment_model_name.empty() && environment_model_name != "Default_Environment") {
            config_info += "\n配置文件信息:\n";
            config_info += config_manager->get_config_summary(environment_model_name);
        }
        
        return config_info;
    }

} // namespace VFT_SMF
