/**
 * @file FlightPlanParser.cpp
 * @brief 飞行计划解析器实现
 * @author VFT_SMF Development Team
 * @date 2024
 */

#include "FlightPlanParser.hpp"
#include "../../G_SimulationManager/LogAndData/Logger.hpp"
#include "../../E_GlobalSharedDataSpace/GlobalSharedDataSpace.hpp"
#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>

// 定义M_PI（Windows上可能未定义）
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace VFT_SMF {

    FlightPlanParser::FlightPlanParser(const std::string& file_path) 
        : json_file_path(file_path), is_parsed(false) {
    }

    bool FlightPlanParser::parse_json_file() {
        try {
            std::ifstream file(json_file_path);
            if (!file.is_open()) {
                std::cerr << "Error: Cannot open file: " << json_file_path << std::endl;
                return false;
            }

            file >> flight_plan_data;
            file.close();

            if (!validate_json_format()) {
                std::cerr << "Error: Invalid JSON format in file: " << json_file_path << std::endl;
                return false;
            }

            is_parsed = true;
            return true;
        }
        catch (const std::exception& e) {
            std::cerr << "Error parsing JSON file: " << e.what() << std::endl;
            return false;
        }
    }

    bool FlightPlanParser::validate_json_format() const {
        try {
            std::cout << "开始验证JSON格式..." << std::endl;
            
            // 检查基本结构
            if (!flight_plan_data.contains("flight_plan")) {
                std::cout << "错误: 缺少 'flight_plan' 字段" << std::endl;
                return false;
            }
            std::cout << "✓ 找到 'flight_plan' 字段" << std::endl;

            auto& plan = flight_plan_data["flight_plan"];
            
            // 检查必需字段
            if (!plan.contains("scenario_config")) {
                std::cout << "错误: 缺少 'scenario_config' 字段" << std::endl;
                return false;
            }
            std::cout << "✓ 找到 'scenario_config' 字段" << std::endl;
            
            if (!plan.contains("global_initial_state")) {
                std::cout << "错误: 缺少 'global_initial_state' 字段" << std::endl;
                return false;
            }
            std::cout << "✓ 找到 'global_initial_state' 字段" << std::endl;
            
            if (!plan.contains("logic_lines")) {
                std::cout << "错误: 缺少 'logic_lines' 字段" << std::endl;
                return false;
            }
            std::cout << "✓ 找到 'logic_lines' 字段" << std::endl;

            // 检查全局初始状态
            auto& global_state = plan["global_initial_state"];
            if (!global_state.contains("flight_dynamics_initial_state")) {
                std::cout << "错误: 缺少 'flight_dynamics_initial_state' 字段" << std::endl;
                return false;
            }
            std::cout << "✓ 找到 'flight_dynamics_initial_state' 字段" << std::endl;
            
            if (!global_state.contains("pilot_initial_state")) {
                std::cout << "错误: 缺少 'pilot_initial_state' 字段" << std::endl;
                return false;
            }
            std::cout << "✓ 找到 'pilot_initial_state' 字段" << std::endl;
            
            if (!global_state.contains("aircraft_initial_state")) {
                std::cout << "错误: 缺少 'aircraft_initial_state' 字段" << std::endl;
                return false;
            }
            std::cout << "✓ 找到 'aircraft_initial_state' 字段" << std::endl;
            
            if (!global_state.contains("environment_initial_state")) {
                std::cout << "错误: 缺少 'environment_initial_state' 字段" << std::endl;
                return false;
            }
            std::cout << "✓ 找到 'environment_initial_state' 字段" << std::endl;
            
            if (!global_state.contains("atc_control_initial_state")) {
                std::cout << "错误: 缺少 'atc_control_initial_state' 字段" << std::endl;
                return false;
            }
            std::cout << "✓ 找到 'atc_control_initial_state' 字段" << std::endl;

            // 检查逻辑线
            auto& logic_lines = plan["logic_lines"];
            if (!logic_lines.contains("pilot_logic_line")) {
                std::cout << "错误: 缺少 'pilot_logic_line' 字段" << std::endl;
                return false;
            }
            std::cout << "✓ 找到 'pilot_logic_line' 字段" << std::endl;
            
            if (!logic_lines.contains("aircraft_system_logic_line")) {
                std::cout << "错误: 缺少 'aircraft_system_logic_line' 字段" << std::endl;
                return false;
            }
            std::cout << "✓ 找到 'aircraft_system_logic_line' 字段" << std::endl;
            
            if (!logic_lines.contains("environment_logic_line")) {
                std::cout << "错误: 缺少 'environment_logic_line' 字段" << std::endl;
                return false;
            }
            std::cout << "✓ 找到 'environment_logic_line' 字段" << std::endl;
            
            if (!logic_lines.contains("ATC_logic_line")) {
                std::cout << "错误: 缺少 'ATC_logic_line' 字段" << std::endl;
                return false;
            }
            std::cout << "✓ 找到 'ATC_logic_line' 字段" << std::endl;

            std::cout << "✓ JSON格式验证通过" << std::endl;
            return true;
        }
        catch (const std::exception& e) {
            std::cerr << "Error validating JSON format: " << e.what() << std::endl;
            return false;
        }
    }

    VFT_SMF::GlobalSharedDataStruct::FlightPlanData::ScenarioConfig FlightPlanParser::extract_scenario_config() const {
        VFT_SMF::GlobalSharedDataStruct::FlightPlanData::ScenarioConfig config;
        
        try {
            auto& plan = flight_plan_data["flight_plan"];
            auto& scenario_config = plan["scenario_config"];
        
        config.ScenarioName = scenario_config.value("ScenarioName", "滑行场景");
        config.Description = scenario_config.value("Description", "基于ParaSAFE飞行计划标准规范的滑行场景模板");
        config.Author = scenario_config.value("Author", "ParaSAFE团队");
        config.CreationDate = scenario_config.value("CreationDate", "2025-08-01");
        config.ScenarioType = scenario_config.value("ScenarioType", "taxi_template");
        config.Pilot_ID = scenario_config.value("Pilot_ID", "Pilot_001");
        config.Aircraft_ID = scenario_config.value("Aircraft_ID", "Aircraft_001");
        config.ATC_ID = scenario_config.value("ATC_ID", "ATC_001");
        config.Environment_Name = scenario_config.value("Environment_Name", "PVG_Runway_005");


            // 记录场景配置信息到日志
            VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "场景名称: " + config.ScenarioName);
            VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "场景类型: " + config.ScenarioType);
            VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "场景描述: " + config.Description);
            VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "作者: " + config.Author);
            VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "创建日期: " + config.CreationDate);
            VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "飞行员ID: " + config.Pilot_ID);
            VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "飞机ID: " + config.Aircraft_ID);
            VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "ATC ID: " + config.ATC_ID);
            VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "环境名称: " + config.Environment_Name);
    

            return config;
        }
        catch (const std::exception& e) {
            std::cerr << "Error extracting scenario config: " << e.what() << std::endl;
            return config;
        }
    }

    std::map<std::string, nlohmann::json> FlightPlanParser::extract_global_initial_state() const {
        std::map<std::string, nlohmann::json> initial_states;
        
        try {
            auto& plan = flight_plan_data["flight_plan"];
            auto& global_state = plan["global_initial_state"];

            initial_states["flight_dynamics"] = parse_flight_dynamics_state(global_state["flight_dynamics_initial_state"]);
            initial_states["pilot"] = parse_pilot_state(global_state["pilot_initial_state"]);
            initial_states["aircraft"] = parse_aircraft_state(global_state["aircraft_initial_state"]);
            initial_states["environment"] = parse_environment_state(global_state["environment_initial_state"]);
            initial_states["atc"] = parse_atc_state(global_state["atc_control_initial_state"]);

            return initial_states;
        }
        catch (const std::exception& e) {
            std::cerr << "Error extracting global initial state: " << e.what() << std::endl;
            return initial_states;
        }
    }

    std::map<std::string, nlohmann::json> FlightPlanParser::extract_logic_lines() const {
        std::map<std::string, nlohmann::json> logic_lines;
        
        try {
            auto& plan = flight_plan_data["flight_plan"];
            auto& lines = plan["logic_lines"];

            logic_lines["pilot"] = lines["pilot_logic_line"];
            logic_lines["aircraft_system"] = lines["aircraft_system_logic_line"];
            logic_lines["environment"] = lines["environment_logic_line"];
            logic_lines["atc"] = lines["ATC_logic_line"];

            return logic_lines;
        }
        catch (const std::exception& e) {
            std::cerr << "Error extracting logic lines: " << e.what() << std::endl;
            return logic_lines;
        }
    }

    std::vector<VFT_SMF::GlobalSharedDataStruct::FlightPlanData::ScenarioEvent> FlightPlanParser::create_scenario_events(const std::map<std::string, nlohmann::json>& logic_lines) const {
        std::vector<VFT_SMF::GlobalSharedDataStruct::FlightPlanData::ScenarioEvent> events;
        
        try {
            int global_event_id = 1; // 全局唯一事件ID
            
            for (const auto& [line_name, line_data] : logic_lines) {
                if (line_data.contains("logic_sequence")) {
                    auto sequence_events = parse_logic_sequence(line_data["logic_sequence"]);
                    
                    // 为每个事件分配唯一的全局ID
                    for (auto& event : sequence_events) {
                        event.event_id = std::to_string(global_event_id++);
                    }
                    
                    events.insert(events.end(), sequence_events.begin(), sequence_events.end());
                }
            }

            return events;
        }
        catch (const std::exception& e) {
            std::cerr << "Error creating scenario events: " << e.what() << std::endl;
            return events;
        }
    }

    nlohmann::json FlightPlanParser::get_scenario_config() const {
    try {   
        auto& plan = flight_plan_data["flight_plan"];
        return plan["ScenarioConfig"];
    }
    catch (const std::exception& e) {
        std::cerr << "Error getting ScenarioConfig: " << e.what() << std::endl;
        return nlohmann::json();
    }
}

    nlohmann::json FlightPlanParser::parse_flight_dynamics_state(const nlohmann::json& flight_dynamics_data) const {
        return flight_dynamics_data;
    }

    nlohmann::json FlightPlanParser::parse_pilot_state(const nlohmann::json& pilot_data) const {
        nlohmann::json parsed_state;
        
        try {
            // 解析飞行员基本信息
            parsed_state["pilot_id"] = pilot_data.value("pilot_id", "PILOT_001");
            parsed_state["pilot_class"] = pilot_data.value("pilot_class", "standard");
            parsed_state["pilot_state"] = pilot_data.value("pilot_state", "normal_operation");
            parsed_state["work_status"] = pilot_data.value("work_status", "就绪");
            
            // 解析注意力水平（从百分比字符串转换为浮点数）
            std::string attention_str = pilot_data.value("attention_level", "100%");
            double attention_level = 1.0; // 默认值
            if (attention_str.find("%") != std::string::npos) {
                std::string num_str = attention_str.substr(0, attention_str.find("%"));
                try {
                    attention_level = std::stod(num_str) / 100.0;
                } catch (...) {
                    attention_level = 1.0;
                }
            }
            parsed_state["attention_level"] = attention_level;
            
            // 解析技能水平（从经验等级转换为数值）
            std::string experience = pilot_data.value("experience_level", "expert");
            double skill_level = 0.0; // 默认值
            if (experience == "expert" || experience == "专家") {
                skill_level = 0.9;
            } else if (experience == "senior" || experience == "高级") {
                skill_level = 0.8;
            } else if (experience == "intermediate" || experience == "中级") {
                skill_level = 0.6;
            } else if (experience == "junior" || experience == "初级") {
                skill_level = 0.4;
            }
            parsed_state["skill_level"] = skill_level;
            
            // 解析疲劳水平
            parsed_state["fatigue_level"] = pilot_data.value("fatigue_level", 0);
            
            // 解析准备状态
            parsed_state["checklist_completed"] = pilot_data.value("checklist_completed", true);
            parsed_state["aircraft_systems_checked"] = pilot_data.value("aircraft_systems_checked", true);
            parsed_state["communication_ready"] = pilot_data.value("communication_ready", true);
            parsed_state["control_ready"] = pilot_data.value("control_ready", true);
            
            return parsed_state;
        }
        catch (const std::exception& e) {
            std::cerr << "Error parsing pilot state: " << e.what() << std::endl;
            return parsed_state;
        }
    }

    nlohmann::json FlightPlanParser::parse_aircraft_state(const nlohmann::json& aircraft_data) const {
        nlohmann::json parsed_state;
        
        try {
            // 解析飞机系统状态
            parsed_state["landing_gear_position"] = aircraft_data.value("landing_gear_position", "down_locked");
            parsed_state["flaps_position"] = aircraft_data.value("flaps_position", 0.0);
            parsed_state["throttle_position"] = aircraft_data.value("throttle_position", 0.05);
            parsed_state["brake_status"] = aircraft_data.value("brake_status", "applied");
            parsed_state["rudder_position"] = aircraft_data.value("rudder_position", 0.0);
            parsed_state["aileron_position"] = aircraft_data.value("aileron_position", 0.0);
            parsed_state["elevator_position"] = aircraft_data.value("elevator_position", 0.0);
            parsed_state["engine_status"] = aircraft_data.value("engine_status", "idle");
            parsed_state["fuel_quantity"] = aircraft_data.value("fuel_quantity", 10000.0);
            parsed_state["oil_quantity"] = aircraft_data.value("oil_quantity", 1000.0);
            
            // 解析飞行动力学状态（如果存在）
            if (aircraft_data.contains("flight_dynamics_initial_state")) {
                const auto& flight_dynamics = aircraft_data["flight_dynamics_initial_state"];
                
                // 位置信息
                if (flight_dynamics.contains("position")) {
                    const auto& position = flight_dynamics["position"];
                    parsed_state["position_x"] = position.value("x", 0.0);
                    parsed_state["position_y"] = position.value("y", 0.0);
                    parsed_state["position_z"] = position.value("z", 0.0);
                }
                
                // 姿态信息
                if (flight_dynamics.contains("attitude")) {
                    const auto& attitude = flight_dynamics["attitude"];
                    parsed_state["roll"] = attitude.value("roll", 0.0);
                    parsed_state["pitch"] = attitude.value("pitch", 0.0);
                    parsed_state["yaw"] = attitude.value("yaw", 90.0);
                }
                
                // 速度信息
                if (flight_dynamics.contains("velocity")) {
                    const auto& velocity = flight_dynamics["velocity"];
                    parsed_state["velocity_x"] = velocity.value("vx", 0.0);
                    parsed_state["velocity_y"] = velocity.value("vy", 0.0);
                    parsed_state["velocity_z"] = velocity.value("vz", 0.0);
                }
            }
            
            VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "飞机系统状态解析完成");
            return parsed_state;
        }
        catch (const std::exception& e) {
            VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "飞机系统状态解析失败: " + std::string(e.what()));
            return aircraft_data; // 返回原始数据作为后备
        }
    }

    nlohmann::json FlightPlanParser::parse_environment_state(const nlohmann::json& environment_data) const {
        return environment_data;
    }

    nlohmann::json FlightPlanParser::parse_atc_state(const nlohmann::json& atc_data) const {
        return atc_data;
    }

    std::vector<VFT_SMF::GlobalSharedDataStruct::FlightPlanData::ScenarioEvent> FlightPlanParser::parse_logic_sequence(const nlohmann::json& logic_sequence) const {
        std::vector<VFT_SMF::GlobalSharedDataStruct::FlightPlanData::ScenarioEvent> events;
        
        try {
            for (const auto& event_data : logic_sequence) {
                if (event_data.contains("event_id") && 
                    event_data.contains("event_name") && 
                    event_data.contains("trigger_condition")) {
                    
                    std::string event_id = std::to_string(event_data["event_id"].get<int>());
                    std::string event_name = event_data["event_name"].get<std::string>();
                    
                    // 解析触发条件
                    double trigger_delay = 0.0;
                    std::string condition_expression = "";
                    std::string condition_description = "";
                    
                    if (event_data["trigger_condition"].contains("condition_expression")) {
                        condition_expression = event_data["trigger_condition"]["condition_expression"].get<std::string>();
                        
                        // 简单的条件解析，提取时间值（仅用于向后兼容）
                        if (condition_expression.find("time >") != std::string::npos) {
                            size_t pos = condition_expression.find("time >");
                            if (pos != std::string::npos) {
                                std::string time_str = condition_expression.substr(pos + 6);
                                try {
                                    trigger_delay = std::stod(time_str);
                                } catch (...) {
                                    trigger_delay = 0.0;
                                }
                            }
                        }
                    }
                    
                    if (event_data["trigger_condition"].contains("description")) {
                        condition_description = event_data["trigger_condition"]["description"].get<std::string>();
                    }
                    
                    VFT_SMF::GlobalSharedDataStruct::FlightPlanData::ScenarioEvent event(
                        event_id, event_name, trigger_delay, condition_expression, condition_description);
                    events.push_back(event);
                }
            }

            return events;
        }
        catch (const std::exception& e) {
            std::cerr << "Error parsing logic sequence: " << e.what() << std::endl;
            return events;
        }
    }

    std::map<std::string, nlohmann::json> FlightPlanParser::extract_driven_processes(const std::map<std::string, nlohmann::json>& logic_lines) const {
        std::map<std::string, nlohmann::json> driven_processes;
        
        try {
            for (const auto& [line_name, line_data] : logic_lines) {
                if (line_data.contains("logic_sequence")) {
                    const auto& sequence = line_data["logic_sequence"];
                    
                    for (const auto& event_data : sequence) {
                        if (event_data.contains("event_id") && event_data.contains("driven_process")) {
                            std::string event_id = std::to_string(event_data["event_id"].get<int>());
                            const auto& process = event_data["driven_process"];
                            
                            // 创建唯一的标识符：逻辑线名称 + 事件ID
                            std::string unique_id = line_name + "_" + event_id;
                            
                            // 创建驱动过程信息
                            nlohmann::json process_info;
                            process_info["line_name"] = line_name;
                            process_info["event_id"] = event_id;
                            process_info["event_name"] = event_data.value("event_name", "");
                            process_info["controller_type"] = process.value("controller_type", "");
                            process_info["controller_name"] = process.value("controller_name", "");
                            process_info["description"] = process.value("description", "");
                            process_info["termination_condition"] = process.value("termination_condition", "");
                            
                            driven_processes[unique_id] = process_info;
                        }
                    }
                }
            }

            return driven_processes;
        }
        catch (const std::exception& e) {
            std::cerr << "Error extracting driven processes: " << e.what() << std::endl;
            return driven_processes;
        }
    }

    nlohmann::json FlightPlanParser::get_parsed_pilot_state() const {
        try {
            // 使用extract_global_initial_state方法获取已解析的数据
            auto initial_states = extract_global_initial_state();
            if (initial_states.find("pilot") != initial_states.end()) {
                return initial_states["pilot"];
            }
            return nlohmann::json();
        }
        catch (const std::exception& e) {
            std::cerr << "Error getting parsed pilot state: " << e.what() << std::endl;
            return nlohmann::json();
        }
    }

    nlohmann::json FlightPlanParser::get_parsed_aircraft_state() const {
        try {
            // 使用extract_global_initial_state方法获取已解析的数据
            auto initial_states = extract_global_initial_state();
            if (initial_states.find("aircraft") != initial_states.end()) {
                return initial_states["aircraft"];
            }
            return nlohmann::json();
        }
        catch (const std::exception& e) {
            std::cerr << "Error getting parsed aircraft state: " << e.what() << std::endl;
            return nlohmann::json();
        }
    }

    bool FlightPlanParser::parse_and_store_flight_plan_data(
        VFT_SMF::GlobalShared_DataSpace::GlobalSharedDataSpace* shared_data_space,
        const std::string& flight_plan_file
    ) {
        try {
            // 0. 首先解析JSON文件（如果还没有解析的话）
            if (!is_parsed) {
                if (!parse_json_file()) {
                    VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "JSON文件解析失败: " + flight_plan_file);
                    return false;
                }
            }
            
            // 1. 提取场景配置
            VFT_SMF::GlobalSharedDataStruct::FlightPlanData::ScenarioConfig scenario_config = extract_scenario_config();
            
            // 2. 提取全局初始状态
            std::map<std::string, nlohmann::json> global_initial_state = extract_global_initial_state();
            VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "全局初始状态提取完成，包含 " + 
                               std::to_string(global_initial_state.size()) + " 个状态组");
            
            // 3. 提取逻辑线
            std::map<std::string, nlohmann::json> logic_lines = extract_logic_lines();
            
            // 4. 创建场景事件
            std::vector<VFT_SMF::GlobalSharedDataStruct::FlightPlanData::ScenarioEvent> scenario_events = 
                create_scenario_events(logic_lines);
            VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "场景事件创建完成，包含 " + 
                               std::to_string(scenario_events.size()) + " 个事件");
            
            // 5. 提取驱动过程
            std::map<std::string, nlohmann::json> driven_processes = extract_driven_processes(logic_lines);
            VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "驱动过程提取完成，包含 " + 
                               std::to_string(driven_processes.size()) + " 个驱动过程");
            
            // 6. 组装飞行计划数据
            VFT_SMF::GlobalSharedDataStruct::FlightPlanData flight_plan_data;
            flight_plan_data.scenario_config = scenario_config;
            
            // 转换全局初始状态
            flight_plan_data.global_initial_state.clear();
            for (const auto& [key, value] : global_initial_state) {
                flight_plan_data.global_initial_state[key] = value.dump();
            }
            
            // 转换逻辑线
            flight_plan_data.logic_lines.clear();
            for (const auto& [key, value] : logic_lines) {
                flight_plan_data.logic_lines[key] = value.dump();
            }
            
            // 设置其他属性
            flight_plan_data.is_parsed = true;
            flight_plan_data.file_path = flight_plan_file;
            
            // 7. 存储到共享数据空间
            shared_data_space->setFlightPlanData(flight_plan_data);
            
            // 8. 设置飞行员初始状态到共享数据空间
            if (global_initial_state.find("pilot") != global_initial_state.end()) {
                const auto& pilot_data = global_initial_state["pilot"];
                VFT_SMF::GlobalSharedDataStruct::PilotGlobalState pilot_state;
                // 解析飞行员状态
                pilot_state.attention_level = pilot_data.value("fatigue_level", 0.0) == 0.0 ? 1.0 : 0.5; // 疲劳度为0时注意力为1.0
                pilot_state.skill_level = 1.0; // 默认技能水平
                pilot_state.timestamp = VFT_SMF::SimulationTimePoint{};
                shared_data_space->setPilotState(pilot_state);
                
                VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "飞行员初始状态已设置: 注意力=" + 
                                   std::to_string(pilot_state.attention_level) + 
                                   ", 技能=" + std::to_string(pilot_state.skill_level));
            }
            
            // 9. 设置飞机系统初始状态到共享数据空间
            if (global_initial_state.find("aircraft") != global_initial_state.end()) {
                const auto& aircraft_data = global_initial_state["aircraft"];
                
                // 设置飞机系统状态
                VFT_SMF::GlobalSharedDataStruct::AircraftSystemState aircraft_system_state;
                aircraft_system_state.current_mass = 70000.0; // 默认质量
                aircraft_system_state.current_fuel = aircraft_data.value("Fuel Quantity", 10000.0); // 使用正确的字段名
                aircraft_system_state.current_center_of_gravity = 0.0; // 默认重心位置
                
                // 解析刹车状态
                std::string brake_status = aircraft_data.value("brake_status", "applied");
                aircraft_system_state.current_brake_pressure = (brake_status == "applied") ? 100.0 : 0.0;
                
                // 解析起落架状态
                std::string landing_gear_pos = aircraft_data.value("landing_gear_position", "down_locked");
                aircraft_system_state.current_landing_gear_deployed = (landing_gear_pos == "down_locked") ? 1.0 : 0.0;
                
                // 解析襟翼状态
                double flaps_pos = aircraft_data.value("flaps_position", 0.0);
                aircraft_system_state.current_flaps_deployed = flaps_pos;
                aircraft_system_state.current_spoilers_deployed = 0.0; // 扰流板收起
                
                // 解析操纵面状态
                aircraft_system_state.current_aileron_deflection = aircraft_data.value("aileron_position", 0.0);
                aircraft_system_state.current_elevator_deflection = aircraft_data.value("elevator_position", 0.0);
                aircraft_system_state.current_rudder_deflection = aircraft_data.value("rudder_position", 0.0);
                aircraft_system_state.current_throttle_position = aircraft_data.value("throttle_position", 0.3); // 使用JSON中的默认值0.3
                aircraft_system_state.timestamp = VFT_SMF::SimulationTimePoint{};
                
                shared_data_space->setAircraftSystemState(aircraft_system_state);
                
                VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "飞机系统状态已从飞行计划解析并设置: 起落架=" + 
                                   landing_gear_pos + ", 襟翼=" + std::to_string(flaps_pos) + 
                                   ", 油门=" + std::to_string(aircraft_system_state.current_throttle_position) +
                                   ", 刹车=" + brake_status + ", 燃油=" + std::to_string(aircraft_system_state.current_fuel));
            }
            
            // 10. 设置飞行动力学初始状态到共享数据空间
            if (global_initial_state.find("flight_dynamics") != global_initial_state.end()) {
                const auto& flight_dynamics_data = global_initial_state["flight_dynamics"];
                
                VFT_SMF::GlobalSharedDataStruct::AircraftFlightState flight_state;
                
                // 解析位置信息（NED坐标系转换为经纬度）
                if (flight_dynamics_data.contains("position")) {
                    const auto& position = flight_dynamics_data["position"];
                    double x = position.value("x", 0.0);
                    double y = position.value("y", 0.0);
                    double z = position.value("z", 0.0);
                    
                    // 简化的NED到经纬度转换（假设起始点为北京首都机场）
                    // 北京首都机场大致坐标：39.9083°N, 116.3975°E
                    const double REF_LAT = 39.9083;  // 参考纬度
                    const double REF_LON = 116.3975; // 参考经度
                    const double EARTH_RADIUS = 6371000.0; // 地球半径（米）
                    
                    // 将NED坐标转换为经纬度偏移
                    double lat_offset = -x / EARTH_RADIUS * (180.0 / M_PI); // 北向为负
                    double lon_offset = y / (EARTH_RADIUS * cos(REF_LAT * M_PI / 180.0)) * (180.0 / M_PI); // 东向为正
                    
                    flight_state.latitude = REF_LAT + lat_offset;
                    flight_state.longitude = REF_LON + lon_offset;
                    flight_state.altitude = -z; // NED坐标系中z向下为正，转换为高度
                } else {
                    // 默认位置（北京首都机场）
                    flight_state.latitude = 39.9083;
                    flight_state.longitude = 116.3975;
                    flight_state.altitude = 0.0;
                }
                
                // 解析姿态信息
                if (flight_dynamics_data.contains("attitude")) {
                    const auto& attitude = flight_dynamics_data["attitude"];
                    flight_state.roll = attitude.value("roll", 0.0);
                    flight_state.pitch = attitude.value("pitch", 0.0);
                    flight_state.heading = attitude.value("yaw", 90.0);
                } else {
                    flight_state.roll = 0.0;
                    flight_state.pitch = 0.0;
                    flight_state.heading = 90.0;
                }
                
                // 解析速度信息
                if (flight_dynamics_data.contains("velocity")) {
                    const auto& velocity = flight_dynamics_data["velocity"];
                    double vx = velocity.value("vx", 0.0);
                    double vy = velocity.value("vy", 0.0);
                    double vz = velocity.value("vz", 0.0);
                    
                    // 计算空速和地速
                    flight_state.airspeed = sqrt(vx * vx + vy * vy);
                    flight_state.groundspeed = flight_state.airspeed; // 简化，不考虑风速
                    flight_state.vertical_speed = -vz; // NED坐标系中z向下为正
                } else {
                    flight_state.airspeed = 0.0;
                    flight_state.groundspeed = 0.0;
                    flight_state.vertical_speed = 0.0;
                }
                
                // 设置其他飞行状态参数
                flight_state.landing_gear_deployed = true;
                flight_state.flaps_deployed = false;
                flight_state.spoilers_deployed = false;
                flight_state.brake_pressure = 100.0; // 刹车已应用
                flight_state.center_of_gravity = 0.0;
                flight_state.wing_loading = 0.0; // 如需翼载，请在系统层用质量派生后再计算
                flight_state.timestamp = VFT_SMF::SimulationTimePoint{};
                
                // 确保初始状态正确设置
                VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "飞行动力学初始状态解析完成: 航向=" + 
                                   std::to_string(flight_state.heading) + "°, 空速=" + 
                                   std::to_string(flight_state.airspeed) + " m/s, 地速=" + 
                                   std::to_string(flight_state.groundspeed) + " m/s");
                
                shared_data_space->setAircraftFlightState(flight_state);
                
                VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "飞行动力学初始状态已设置: 位置=(" + 
                                   std::to_string(flight_state.latitude) + ", " + 
                                   std::to_string(flight_state.longitude) + "), 高度=" + 
                                   std::to_string(flight_state.altitude) + "m, 航向=" + 
                                   std::to_string(flight_state.heading) + "°, 空速=" + 
                                   std::to_string(flight_state.airspeed) + " m/s");
            }
            
            // 10. 设置环境初始状态到共享数据空间
            VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "检查global_initial_state中的键数量: " + std::to_string(global_initial_state.size()));
            for (const auto& pair : global_initial_state) {
                VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "键: " + pair.first);
            }
            if (global_initial_state.find("environment") != global_initial_state.end()) {
                const auto& env_data = global_initial_state["environment"];
                
                VFT_SMF::GlobalSharedDataStruct::EnvironmentGlobalState env_state;
                
                // 解析跑道信息
                if (env_data.contains("runway")) {
                    const auto& runway_data = env_data["runway"];
                    env_state.runway_length = runway_data.value("length", 3800.0);
                    env_state.runway_width = runway_data.value("width", 60.0);
                    env_state.friction_coefficient = runway_data.value("friction_coefficient", 0.7);
                } else {
                    env_state.runway_length = 3800.0;
                    env_state.runway_width = 60.0;
                    env_state.friction_coefficient = 0.7;
                }
                
                // 解析天气信息
                if (env_data.contains("weather")) {
                    const auto& weather_data = env_data["weather"];
                    // 从大气压力计算空气密度 (简化计算)
                    double atmospheric_pressure = weather_data.value("atmospheric_pressure", 1013.25);
                    double temperature = weather_data.value("temperature", 15.0);
                    // 使用理想气体定律计算空气密度 (简化)
                    env_state.air_density = atmospheric_pressure / (287.0 * (temperature + 273.15));
                } else {
                    env_state.air_density = 1.225; // 标准大气密度
                }
                
                // 解析风信息
                if (env_data.contains("wind")) {
                    const auto& wind_data = env_data["wind"];
                    env_state.wind_speed = wind_data.value("speed", 0.0);
                    env_state.wind_direction = wind_data.value("direction", 0.0);
                } else {
                    env_state.wind_speed = 0.0;
                    env_state.wind_direction = 0.0;
                }
                
                env_state.timestamp = VFT_SMF::SimulationTimePoint{};
                shared_data_space->setEnvironmentState(env_state);
                
                VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "环境初始状态已从飞行计划解析并设置: 跑道长度=" + 
                                   std::to_string(env_state.runway_length) + "m, 跑道宽度=" + 
                                   std::to_string(env_state.runway_width) + "m, 摩擦系数=" + 
                                   std::to_string(env_state.friction_coefficient) + ", 风速=" + 
                                   std::to_string(env_state.wind_speed) + " m/s, 风向=" + 
                                   std::to_string(env_state.wind_direction) + "°");
            }
            
            // 11. 将解析出的事件添加到全局共享数据空间的事件库中
            VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "开始将解析出的事件添加到事件库中...");
            
            // 从飞行计划中解析所有事件和控制器
            auto event_logic_lines = extract_logic_lines();
            
            // 创建事件ID到driven_process的映射
            std::map<std::string, VFT_SMF::GlobalSharedDataStruct::DrivenProcess> event_driven_process_map;
            
            // 重新创建scenario_events以获取正确的全局ID映射
            auto scenario_events_with_ids = create_scenario_events(event_logic_lines);
            
            // 遍历scenario_events，为每个事件找到对应的driven_process
            for (const auto& scenario_event : scenario_events_with_ids) {
                // 在逻辑线中查找对应的事件
                for (const auto& [line_name, line_data] : event_logic_lines) {
                    if (line_data.contains("logic_sequence")) {
                        const auto& logic_sequence = line_data["logic_sequence"];
                        
                        for (const auto& event : logic_sequence) {
                            // 通过事件名称匹配
                            if (event.value("event_name", "") == scenario_event.event_type) {
                                if (event.contains("driven_process")) {
                                    const auto& driven_process = event["driven_process"];
                                    
                                    VFT_SMF::GlobalSharedDataStruct::DrivenProcess driven_proc(
                                        driven_process.value("controller_type", ""),
                                        driven_process.value("controller_name", ""),
                                        driven_process.value("description", ""),
                                        driven_process.value("termination_condition", "")
                                    );
                                    
                                    event_driven_process_map[scenario_event.event_id] = driven_proc;
                                    
                                    VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, 
                                        "解析到事件驱动过程: " + scenario_event.event_id + " (" + scenario_event.event_type + ") -> " + 
                                        driven_proc.controller_type + "::" + driven_proc.controller_name);
                                }
                                break;
                            }
                        }
                    }
                }
            }
            
            for (const auto& scenario_event : scenario_events) {
                // 创建触发条件 - 使用从JSON解析的condition_expression
                VFT_SMF::GlobalSharedDataStruct::TriggerCondition trigger_cond(
                    scenario_event.condition_expression, // condition_expression
                    scenario_event.condition_description // description
                );
                
                // 查找对应的driven_process
                VFT_SMF::GlobalSharedDataStruct::DrivenProcess driven_proc;
                auto it = event_driven_process_map.find(scenario_event.event_id);
                if (it != event_driven_process_map.end()) {
                    driven_proc = it->second;
                } else {
                    // 如果没有找到，使用默认值
                    driven_proc = VFT_SMF::GlobalSharedDataStruct::DrivenProcess(
                        "NULL", "NULL", "NULL", "NULL"
                    );
                }
                
                // 使用构造函数创建StandardEvent
                VFT_SMF::GlobalSharedDataStruct::StandardEvent standard_event(
                    std::stoi(scenario_event.event_id),  // event_id (转换为int)
                    scenario_event.event_type,           // event_name
                    "从飞行计划解析的事件",              // description
                    trigger_cond,                        // trigger_condition
                    driven_proc,                         // driven_process
                    "NULL"                              // source_agent (未知项使用NULL)
                );
                
                // 使用GlobalSharedDataSpace提供的公共接口添加事件
                shared_data_space->addPlannedEventToLibrary(standard_event);
                
                VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "事件已添加到事件库: " + 
                                   standard_event.getEventIdString() + " (" + standard_event.event_name + 
                                   ", 控制器: " + driven_proc.controller_type + "::" + driven_proc.controller_name + ")");
            }
            
            VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "事件库更新完成，共添加 " + 
                               std::to_string(scenario_events.size()) + " 个事件");
            
            VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "飞行计划数据解析并存储完成");
            return true;
        }
        catch (const std::exception& e) {
            std::cerr << "Error parsing and storing flight plan data: " << e.what() << std::endl;
            VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "飞行计划数据解析并存储失败: " + std::string(e.what()));
            return false;
        }
    }

    bool FlightPlanParser::record_initial_data(
        VFT_SMF::GlobalShared_DataSpace::GlobalSharedDataSpace* shared_data_space
    ) {
        try {
            // 解析并初始化计划控制器库
            if (shared_data_space && is_parsed) {
                VFT_SMF::GlobalSharedDataStruct::PlanedControllersLibrary planed_controllers;
                
           // 从飞行计划中解析所有事件和控制器
            auto logic_lines = extract_logic_lines();
            
            VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, 
                "FlightPlanParser: 解析到 " + std::to_string(logic_lines.size()) + " 个逻辑线");
            
            // 遍历所有逻辑线，提取控制器信息
            for (const auto& [line_name, line_data] : logic_lines) {
                VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, 
                    "FlightPlanParser: 处理逻辑线: " + line_name);
                                    if (line_data.contains("logic_sequence")) {
                    const auto& logic_sequence = line_data["logic_sequence"];
                    
                    VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, 
                        "FlightPlanParser: 逻辑线 " + line_name + " 包含 " + 
                        std::to_string(logic_sequence.size()) + " 个事件");
                    
                                            for (const auto& event : logic_sequence) {
                            VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, 
                                "FlightPlanParser: 检查事件: " + event.value("event_name", "unknown"));
                            
                            if (event.contains("driven_process")) {
                                VFT_SMF::GlobalSharedDataStruct::PlanedController controller;
                                
                                // 设置事件信息
                                controller.event_id = std::to_string(event.value("event_id", 0));
                                controller.event_name = event.value("event_name", "");
                                
                                // 设置控制器信息
                                const auto& driven_process = event["driven_process"];
                                controller.controller_type = driven_process.value("controller_type", "");
                                controller.controller_name = driven_process.value("controller_name", "");
                                controller.description = driven_process.value("description", "");
                                controller.termination_condition = driven_process.value("termination_condition", "");
                                
                                // 解析控制器参数（如果有的话）
                                if (driven_process.contains("controller_parameters")) {
                                    const auto& params = driven_process["controller_parameters"];
                                    for (auto it = params.begin(); it != params.end(); ++it) {
                                        controller.controller_parameters[it.key()] = it.value().get<std::string>();
                                    }
                                }
                                
                                // 添加到控制器库
                                planed_controllers.addController(controller);
                                
                                VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, 
                                    "解析到控制器: " + controller.controller_name + 
                                    " (事件: " + controller.event_name + ", 类型: " + controller.controller_type + ")");
                            }
                        }
                    }
                }
                
                // 设置数据源和时间戳
                planed_controllers.datasource = "FlightPlanParser";
                planed_controllers.timestamp = VFT_SMF::SimulationTimePoint{};
                
                // 存储到共享数据空间
                shared_data_space->setPlanedControllersLibrary(planed_controllers, "FlightPlanParser");
                
                VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, 
                    "计划控制器库初始化完成，共解析到 " + 
                    std::to_string(planed_controllers.getAllControllers().size()) + " 个控制器");
            }
            
            VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "飞行计划解析器初始数据记录完成");
            return true;
        }
        catch (const std::exception& e) {
            std::cerr << "Error recording initial data: " << e.what() << std::endl;
            VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "初始数据记录失败: " + std::string(e.what()));
            return false;
        }
    }

    bool FlightPlanParser::get_initial_flight_state(VFT_SMF::GlobalSharedDataStruct::AircraftFlightState& initial_flight_state) const {
        try {
            if (!is_parsed) {
                VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "飞行计划尚未解析，无法获取初始飞行状态");
                return false;
            }

            // 从解析的JSON数据中提取飞行动力学初始状态
            auto global_initial_state = extract_global_initial_state();
            
            if (global_initial_state.find("flight_dynamics") != global_initial_state.end()) {
                const auto& flight_dynamics_data = global_initial_state["flight_dynamics"];
                
                // 解析位置数据（NED坐标系）
                double ned_x = flight_dynamics_data.value("position", nlohmann::json::object())["x"];
                double ned_y = flight_dynamics_data.value("position", nlohmann::json::object())["y"];
                double ned_z = flight_dynamics_data.value("position", nlohmann::json::object())["z"];
                
                // 转换为地理坐标（这里使用北京首都机场作为参考点）
                // 北京首都机场坐标：40.0799°N, 116.6031°E
                double ref_lat = 40.0799;
                double ref_lon = 116.6031;
                
                // 简化的NED到地理坐标转换（这里使用近似转换）
                initial_flight_state.latitude = ref_lat + (ned_y / 111320.0); // 1度纬度约等于111320米
                initial_flight_state.longitude = ref_lon + (ned_x / (111320.0 * cos(ref_lat * M_PI / 180.0)));
                initial_flight_state.altitude = -ned_z; // NED坐标系中Z轴向下为正
                
                // 解析姿态数据
                initial_flight_state.heading = flight_dynamics_data.value("attitude", nlohmann::json::object())["heading"];
                initial_flight_state.pitch = flight_dynamics_data.value("attitude", nlohmann::json::object())["pitch"];
                initial_flight_state.roll = flight_dynamics_data.value("attitude", nlohmann::json::object())["roll"];
                
                // 解析速度数据
                initial_flight_state.airspeed = flight_dynamics_data.value("velocity", nlohmann::json::object())["airspeed"];
                initial_flight_state.groundspeed = flight_dynamics_data.value("velocity", nlohmann::json::object())["groundspeed"];
                initial_flight_state.vertical_speed = flight_dynamics_data.value("velocity", nlohmann::json::object())["vertical_speed"];
                
                // 设置其他飞行状态参数
                initial_flight_state.landing_gear_deployed = true;
                initial_flight_state.flaps_deployed = false;
                initial_flight_state.spoilers_deployed = false;
                initial_flight_state.brake_pressure = 100.0; // 刹车已应用
                initial_flight_state.center_of_gravity = 0.0;
                initial_flight_state.wing_loading = 0.0; // 如需翼载，请在系统层用质量派生后再计算
                initial_flight_state.timestamp = VFT_SMF::SimulationTimePoint{};
                
                VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "成功获取初始飞行状态: 位置=(" + 
                                   std::to_string(initial_flight_state.latitude) + ", " + 
                                   std::to_string(initial_flight_state.longitude) + "), 高度=" + 
                                   std::to_string(initial_flight_state.altitude) + "m, 航向=" + 
                                   std::to_string(initial_flight_state.heading) + "°, 空速=" + 
                                   std::to_string(initial_flight_state.airspeed) + " m/s");
                
                return true;
            } else {
                VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "未找到飞行动力学初始状态数据");
                return false;
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Error getting initial flight state: " << e.what() << std::endl;
            VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "获取初始飞行状态失败: " + std::string(e.what()));
            return false;
        }
    }

} // namespace VFT_SMF 