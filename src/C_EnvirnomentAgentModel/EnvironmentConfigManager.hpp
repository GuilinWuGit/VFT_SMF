/**
 * @file EnvironmentConfigManager.hpp
 * @brief 环境配置文件管理器
 * @author VFT_SMF Development Team
 * @date 2024
 */

#pragma once

#include <string>
#include <memory>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cctype>
#include "../I_ThirdPartyTools/json.hpp"

namespace VFT_SMF {

    /**
     * @brief 环境配置数据结构
     */
    struct EnvironmentConfig {
        // 环境模型基本信息
        struct EnvironmentModel {
            std::string name;
            std::string airport_code;
            std::string runway_code;
            std::string environment_type;
            std::string description;
        } environment_model;

        // 跑道数据
        struct RunwayData {
            double length;
            double width;
            std::string surface_type;
            double friction_coefficient;
            std::string condition;
            bool is_available;
            double elevation;
            double slope;
            double heading;
            std::string ils_frequency;
            std::string approach_lights;
        } runway_data;

        // 大气数据
        struct AtmosphericData {
            double temperature;
            double pressure;
            double humidity;
            double visibility;
            double density_altitude;
            double dew_point;
            double air_density;
            std::string cloud_cover;
            double cloud_base;
            double ceiling;
            std::string precipitation;
            double precipitation_intensity;
        } atmospheric_data;

        // 风数据
        struct WindData {
            double wind_speed;
            double wind_direction;
            double gust_speed;
            double crosswind_component;
            double headwind_component;
            double wind_shear;
            std::string wind_condition;
            bool is_turbulent;
            double wind_altitude;
            std::map<std::string, std::pair<double, double>> wind_forecast;
        } wind_data;

        // 天气模型
        struct WeatherModel {
            double weather_stability;
            double change_rate;
            std::string default_weather;
            std::map<std::string, std::map<std::string, double>> weather_transitions;
        } weather_model;

        // 环境因素
        struct EnvironmentalFactors {
            double noise_level;
            std::string air_quality;
            int air_quality_index;
            double radiation_level;
            double magnetic_variation;
            std::string time_zone;
            bool daylight_savings;
        } environmental_factors;

        // 操作约束
        struct OperationalConstraints {
            double max_wind_speed;
            double max_crosswind;
            double min_visibility;
            double min_ceiling;
            double max_temperature;
            double min_temperature;
            std::map<std::string, std::pair<double, double>> runway_condition_limits;
        } operational_constraints;

        // 更新参数
        struct UpdateParameters {
            std::pair<double, double> temperature_change_range;
            std::pair<double, double> wind_change_range;
            std::pair<double, double> pressure_change_range;
            double update_frequency;
            int random_seed;
        } update_parameters;
    };

    /**
     * @brief 环境配置文件管理器类
     */
    class EnvironmentConfigManager {
    private:
        std::map<std::string, EnvironmentConfig> config_cache;
        std::string base_config_path;
        
        // 私有方法
        bool load_config_from_file(const std::string& model_name, EnvironmentConfig& config);
        bool parse_json_config(const nlohmann::json& json_data, EnvironmentConfig& config);
        std::string get_config_file_path(const std::string& model_name);
        void validate_config(const EnvironmentConfig& config);
        
    public:
        EnvironmentConfigManager(const std::string& base_path = "src/C_EnvirnomentAgentModel/");
        ~EnvironmentConfigManager() = default;

        // 配置管理方法
        bool load_environment_config(const std::string& model_name);
        EnvironmentConfig get_environment_config(const std::string& model_name);
        bool is_config_loaded(const std::string& model_name) const;
        std::vector<std::string> get_available_models() const;
        
        // 配置验证方法
        bool validate_model_config(const std::string& model_name);
        std::string get_config_validation_report(const std::string& model_name);
        
        // 配置更新方法
        bool update_config_cache(const std::string& model_name);
        void clear_config_cache();
        void reload_all_configs();
        
        // 工具方法
        std::string get_config_summary(const std::string& model_name);
        bool export_config_to_file(const std::string& model_name, const std::string& output_path);
    };

} // namespace VFT_SMF
