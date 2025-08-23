/**
 * @file FlightPlanParser.hpp
 * @brief 飞行计划解析器
 * @author VFT_SMF Development Team
 * @date 2024
 * 
 * 负责解析JSON格式的飞行计划模板，转换为场景配置
 */

#pragma once

#include "../B_ScenarioModel/VFT_SMF_Base.hpp"
#include "../../E_GlobalSharedDataSpace/GlobalSharedDataSpace.hpp"
#include "../../I_ThirdPartyTools/json.hpp"
#include <fstream>
#include <string>
#include <memory>

namespace VFT_SMF {

    /**
     * @brief 飞行计划解析器
     * @details 解析JSON格式的飞行计划模板，转换为场景配置并初始化：
     * 1)提取场景配置，包括场景名称、类型等; 初始化场景配置，包括场景名称、场景类型、飞机代理的名称、飞行员代理的名称、环境代理的名称、ATC代理的名称等;
     * 2)提取全局初始状态，包括飞行动力学、飞行员、飞机、环境、ATC等;3)提取逻辑线，包括事件序列、条件判断等;4)创建场景事件;5)创建场景目标
     */
    class FlightPlanParser {
    private:
        std::string json_file_path;
        nlohmann::json flight_plan_data;
        bool is_parsed;

    public:
        /**
         * @brief 构造函数
         * @param file_path JSON文件路径
         */
        FlightPlanParser(const std::string& file_path);

        /**
         * @brief 解析JSON文件
         * @return 是否解析成功
         */
        bool parse_json_file();

        /**
         * @brief 验证JSON格式
         * @return 是否格式正确
         */
        bool validate_json_format() const;

        /**
         * @brief 提取场景配置
         * @return 场景配置对象
         */
        VFT_SMF::GlobalSharedDataStruct::FlightPlanData::ScenarioConfig extract_scenario_config() const;

        /**
         * @brief 提取全局初始状态
         * @return 初始状态映射
         */
        std::map<std::string, nlohmann::json> extract_global_initial_state() const;

        /**
         * @brief 提取逻辑线
         * @return 逻辑线映射
         */
        std::map<std::string, nlohmann::json> extract_logic_lines() const;

        /**
         * @brief 创建场景事件
         * @param logic_lines 逻辑线数据
         * @return 场景事件列表
         */
        std::vector<VFT_SMF::GlobalSharedDataStruct::FlightPlanData::ScenarioEvent> create_scenario_events(const std::map<std::string, nlohmann::json>& logic_lines) const;

        /**
         * @brief 提取所有被事件驱动的过程
         * @param logic_lines 逻辑线数据
         * @return 驱动过程映射，键为事件ID，值为驱动过程信息
         */
        std::map<std::string, nlohmann::json> extract_driven_processes(const std::map<std::string, nlohmann::json>& logic_lines) const;

        /**
         * @brief 获取解析状态
         * @return 是否已解析
         */
        bool is_parsed_successfully() const { return is_parsed; }

        /**
         * @brief 获取场景配置
         * @return 场景配置JSON对象
         */
        nlohmann::json get_scenario_config() const;

        /**
         * @brief 获取解析后的飞行员初始状态
         * @return 飞行员状态JSON对象
         */
        nlohmann::json get_parsed_pilot_state() const;
        
        /**
         * @brief 获取解析后的飞机系统初始状态
         * @return 飞机系统状态JSON对象
         */
        nlohmann::json get_parsed_aircraft_state() const;
        
        /**
         * @brief 解析完整的飞行计划数据并存储到共享数据空间
         * @param shared_data_space 全局共享数据空间指针
         * @param flight_plan_file 飞行计划文件路径
         * @return 是否解析成功
         */
        bool parse_and_store_flight_plan_data(
            VFT_SMF::GlobalShared_DataSpace::GlobalSharedDataSpace* shared_data_space,
            const std::string& flight_plan_file
        );
        
        /**
         * @brief 记录初始数据到数据记录器
         * @param shared_data_space 全局共享数据空间指针
         * @return 是否记录成功
         */
        bool record_initial_data(
            VFT_SMF::GlobalShared_DataSpace::GlobalSharedDataSpace* shared_data_space
        );
        
        /**
         * @brief 获取初始飞行状态
         * @param initial_flight_state 输出参数，初始飞行状态
         * @return 是否获取成功
         */
        bool get_initial_flight_state(VFT_SMF::GlobalSharedDataStruct::AircraftFlightState& initial_flight_state) const;

    private:
        /**
         * @brief 解析飞行动力学初始状态
         * @param flight_dynamics_data 飞行动力学数据
         * @return 解析后的状态数据
         */
        nlohmann::json parse_flight_dynamics_state(const nlohmann::json& flight_dynamics_data) const;

        /**
         * @brief 解析飞行员初始状态
         * @param pilot_data 飞行员数据
         * @return 解析后的状态数据
         */
        nlohmann::json parse_pilot_state(const nlohmann::json& pilot_data) const;

        /**
         * @brief 解析飞机初始状态
         * @param aircraft_data 飞机数据
         * @return 解析后的状态数据
         */
        nlohmann::json parse_aircraft_state(const nlohmann::json& aircraft_data) const;

        /**
         * @brief 解析环境初始状态
         * @param environment_data 环境数据
         * @return 解析后的状态数据
         */
        nlohmann::json parse_environment_state(const nlohmann::json& environment_data) const;

        /**
         * @brief 解析ATC初始状态
         * @param atc_data ATC数据
         * @return 解析后的状态数据
         */
        nlohmann::json parse_atc_state(const nlohmann::json& atc_data) const;

        /**
         * @brief 解析逻辑序列
         * @param logic_sequence 逻辑序列数据
         * @return 解析后的事件列表
         */
        std::vector<VFT_SMF::GlobalSharedDataStruct::FlightPlanData::ScenarioEvent> parse_logic_sequence(const nlohmann::json& logic_sequence) const;
    };

} // namespace VFT_SMF 