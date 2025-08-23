/**
 * @file EnvironmentConfigManager.cpp
 * @brief 环境配置文件管理器实现
 * @author VFT_SMF Development Team
 * @date 2024
 */

#include "EnvironmentConfigManager.hpp"
#include "../G_SimulationManager/LogAndData/Logger.hpp"
#include <filesystem>
#include <regex>

namespace VFT_SMF {

    EnvironmentConfigManager::EnvironmentConfigManager(const std::string& base_path)
        : base_config_path(base_path) {
        VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "环境配置管理器初始化，基础路径: " + base_path);
    }

    bool EnvironmentConfigManager::load_environment_config(const std::string& model_name) {
        VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "加载环境配置: " + model_name);
        
        // 检查缓存
        if (config_cache.find(model_name) != config_cache.end()) {
            VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "配置已缓存: " + model_name);
            return true;
        }
        
        // 创建配置对象
        EnvironmentConfig config;
        
        // 从文件加载配置
        if (!load_config_from_file(model_name, config)) {
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "加载配置失败: " + model_name);
            return false;
        }
        
        // 验证配置
        validate_config(config);
        
        // 缓存配置
        config_cache[model_name] = config;
        
        VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "环境配置加载成功: " + model_name);
        return true;
    }

    EnvironmentConfig EnvironmentConfigManager::get_environment_config(const std::string& model_name) {
        // 如果配置未加载，先加载
        if (!is_config_loaded(model_name)) {
            load_environment_config(model_name);
        }
        
        auto it = config_cache.find(model_name);
        if (it != config_cache.end()) {
            return it->second;
        }
        
        // 返回默认配置
        VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "返回默认配置: " + model_name);
        return EnvironmentConfig{};
    }

    bool EnvironmentConfigManager::is_config_loaded(const std::string& model_name) const {
        return config_cache.find(model_name) != config_cache.end();
    }

    std::vector<std::string> EnvironmentConfigManager::get_available_models() const {
        std::vector<std::string> models;
        
        try {
            std::filesystem::path base_path(base_config_path);
            if (!std::filesystem::exists(base_path)) {
                VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "基础路径不存在: " + base_config_path);
                return models;
            }
            
            for (const auto& entry : std::filesystem::directory_iterator(base_path)) {
                if (entry.is_directory()) {
                    std::string dir_name = entry.path().filename().string();
                    
                    // 检查是否有配置文件
                    std::filesystem::path config_path = entry.path() / "DataTwin" / "environment_config.json";
                    if (std::filesystem::exists(config_path)) {
                        models.push_back(dir_name);
                    }
                }
            }
        } catch (const std::exception& e) {
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "扫描可用模型时出错: " + std::string(e.what()));
        }
        
        return models;
    }

    bool EnvironmentConfigManager::validate_model_config(const std::string& model_name) {
        if (!is_config_loaded(model_name)) {
            load_environment_config(model_name);
        }
        
        auto it = config_cache.find(model_name);
        if (it == config_cache.end()) {
            return false;
        }
        
        try {
            validate_config(it->second);
            return true;
        } catch (const std::exception& e) {
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "配置验证失败: " + std::string(e.what()));
            return false;
        }
    }

    std::string EnvironmentConfigManager::get_config_validation_report(const std::string& model_name) {
        std::ostringstream report;
        report << "环境配置验证报告 - " << model_name << "\n";
        report << "=====================================\n";
        
        if (!is_config_loaded(model_name)) {
            report << "❌ 配置未加载\n";
            return report.str();
        }
        
        const auto& config = config_cache[model_name];
        
        // 验证基本信息
        report << "✅ 环境模型: " << config.environment_model.name << "\n";
        report << "✅ 机场代码: " << config.environment_model.airport_code << "\n";
        report << "✅ 跑道代码: " << config.environment_model.runway_code << "\n";
        
        // 验证跑道数据
        if (config.runway_data.length > 0 && config.runway_data.width > 0) {
            report << "✅ 跑道数据: 有效\n";
        } else {
            report << "❌ 跑道数据: 无效\n";
        }
        
        // 验证大气数据
        if (config.atmospheric_data.temperature >= -50 && config.atmospheric_data.temperature <= 60) {
            report << "✅ 大气数据: 有效\n";
        } else {
            report << "❌ 大气数据: 温度超出范围\n";
        }
        
        // 验证风数据
        if (config.wind_data.wind_speed >= 0 && config.wind_data.wind_direction >= 0 && config.wind_data.wind_direction <= 360) {
            report << "✅ 风数据: 有效\n";
        } else {
            report << "❌ 风数据: 无效\n";
        }
        
        // 验证天气模型
        if (config.weather_model.weather_stability >= 0 && config.weather_model.weather_stability <= 1) {
            report << "✅ 天气模型: 有效\n";
        } else {
            report << "❌ 天气模型: 稳定性参数无效\n";
        }
        
        report << "=====================================\n";
        return report.str();
    }

    bool EnvironmentConfigManager::update_config_cache(const std::string& model_name) {
        VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "更新配置缓存: " + model_name);
        
        // 从缓存中移除
        config_cache.erase(model_name);
        
        // 重新加载
        return load_environment_config(model_name);
    }

    void EnvironmentConfigManager::clear_config_cache() {
        VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "清空配置缓存");
        config_cache.clear();
    }

    void EnvironmentConfigManager::reload_all_configs() {
        VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "重新加载所有配置");
        
        std::vector<std::string> models = get_available_models();
        for (const auto& model : models) {
            update_config_cache(model);
        }
    }

    std::string EnvironmentConfigManager::get_config_summary(const std::string& model_name) {
        if (!is_config_loaded(model_name)) {
            return "配置未加载: " + model_name;
        }
        
        const auto& config = config_cache[model_name];
        std::ostringstream summary;
        
        summary << "环境配置摘要 - " << model_name << "\n";
        summary << "机场: " << config.environment_model.airport_code << "\n";
        summary << "跑道: " << config.environment_model.runway_code << "\n";
        summary << "跑道长度: " << config.runway_data.length << "m\n";
        summary << "温度: " << config.atmospheric_data.temperature << "°C\n";
        summary << "风速: " << config.wind_data.wind_speed << "m/s\n";
        summary << "风向: " << config.wind_data.wind_direction << "°\n";
        summary << "能见度: " << config.atmospheric_data.visibility << "m\n";
        summary << "天气稳定性: " << config.weather_model.weather_stability << "\n";
        
        return summary.str();
    }

    bool EnvironmentConfigManager::export_config_to_file(const std::string& model_name, const std::string& output_path) {
        if (!is_config_loaded(model_name)) {
            return false;
        }
        
        try {
            const auto& config = config_cache[model_name];
            nlohmann::json json_config;
            
            // 构建JSON对象
            json_config["environment_model"]["name"] = config.environment_model.name;
            json_config["environment_model"]["airport_code"] = config.environment_model.airport_code;
            json_config["environment_model"]["runway_code"] = config.environment_model.runway_code;
            json_config["environment_model"]["environment_type"] = config.environment_model.environment_type;
            json_config["environment_model"]["description"] = config.environment_model.description;
            
            // 跑道数据
            json_config["runway_data"]["length"] = config.runway_data.length;
            json_config["runway_data"]["width"] = config.runway_data.width;
            json_config["runway_data"]["surface_type"] = config.runway_data.surface_type;
            json_config["runway_data"]["friction_coefficient"] = config.runway_data.friction_coefficient;
            json_config["runway_data"]["condition"] = config.runway_data.condition;
            json_config["runway_data"]["is_available"] = config.runway_data.is_available;
            json_config["runway_data"]["elevation"] = config.runway_data.elevation;
            json_config["runway_data"]["slope"] = config.runway_data.slope;
            json_config["runway_data"]["heading"] = config.runway_data.heading;
            json_config["runway_data"]["ils_frequency"] = config.runway_data.ils_frequency;
            json_config["runway_data"]["approach_lights"] = config.runway_data.approach_lights;
            
            // 大气数据
            json_config["atmospheric_data"]["temperature"] = config.atmospheric_data.temperature;
            json_config["atmospheric_data"]["pressure"] = config.atmospheric_data.pressure;
            json_config["atmospheric_data"]["humidity"] = config.atmospheric_data.humidity;
            json_config["atmospheric_data"]["visibility"] = config.atmospheric_data.visibility;
            json_config["atmospheric_data"]["density_altitude"] = config.atmospheric_data.density_altitude;
            json_config["atmospheric_data"]["dew_point"] = config.atmospheric_data.dew_point;
            json_config["atmospheric_data"]["air_density"] = config.atmospheric_data.air_density;
            json_config["atmospheric_data"]["cloud_cover"] = config.atmospheric_data.cloud_cover;
            json_config["atmospheric_data"]["cloud_base"] = config.atmospheric_data.cloud_base;
            json_config["atmospheric_data"]["ceiling"] = config.atmospheric_data.ceiling;
            json_config["atmospheric_data"]["precipitation"] = config.atmospheric_data.precipitation;
            json_config["atmospheric_data"]["precipitation_intensity"] = config.atmospheric_data.precipitation_intensity;
            
            // 风数据
            json_config["wind_data"]["wind_speed"] = config.wind_data.wind_speed;
            json_config["wind_data"]["wind_direction"] = config.wind_data.wind_direction;
            json_config["wind_data"]["gust_speed"] = config.wind_data.gust_speed;
            json_config["wind_data"]["crosswind_component"] = config.wind_data.crosswind_component;
            json_config["wind_data"]["headwind_component"] = config.wind_data.headwind_component;
            json_config["wind_data"]["wind_shear"] = config.wind_data.wind_shear;
            json_config["wind_data"]["wind_condition"] = config.wind_data.wind_condition;
            json_config["wind_data"]["is_turbulent"] = config.wind_data.is_turbulent;
            json_config["wind_data"]["wind_altitude"] = config.wind_data.wind_altitude;
            
            // 天气模型
            json_config["weather_model"]["weather_stability"] = config.weather_model.weather_stability;
            json_config["weather_model"]["change_rate"] = config.weather_model.change_rate;
            json_config["weather_model"]["default_weather"] = config.weather_model.default_weather;
            
            // 更新参数
            json_config["update_parameters"]["temperature_change_range"] = {config.update_parameters.temperature_change_range.first, config.update_parameters.temperature_change_range.second};
            json_config["update_parameters"]["wind_change_range"] = {config.update_parameters.wind_change_range.first, config.update_parameters.wind_change_range.second};
            json_config["update_parameters"]["pressure_change_range"] = {config.update_parameters.pressure_change_range.first, config.update_parameters.pressure_change_range.second};
            json_config["update_parameters"]["update_frequency"] = config.update_parameters.update_frequency;
            json_config["update_parameters"]["random_seed"] = config.update_parameters.random_seed;
            
            // 写入文件
            std::ofstream file(output_path);
            if (file.is_open()) {
                file << json_config.dump(2);
                file.close();
                VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "配置导出成功: " + output_path);
                return true;
            }
        } catch (const std::exception& e) {
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "配置导出失败: " + std::string(e.what()));
        }
        
        return false;
    }

    // ==================== 私有方法实现 ====================

    bool EnvironmentConfigManager::load_config_from_file(const std::string& model_name, EnvironmentConfig& config) {
        std::string config_path = get_config_file_path(model_name);
        
        VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "加载配置文件: " + config_path);
        
        try {
            std::ifstream file(config_path);
            if (!file.is_open()) {
                VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "无法打开配置文件: " + config_path);
                return false;
            }
            
            nlohmann::json json_data;
            file >> json_data;
            file.close();
            
            return parse_json_config(json_data, config);
            
        } catch (const std::exception& e) {
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "解析配置文件失败: " + std::string(e.what()));
            return false;
        }
    }

    bool EnvironmentConfigManager::parse_json_config(const nlohmann::json& json_data, EnvironmentConfig& config) {
        try {
            // 解析环境模型信息
            if (json_data.contains("environment_model")) {
                const auto& env_model = json_data["environment_model"];
                config.environment_model.name = env_model.value("name", "");
                config.environment_model.airport_code = env_model.value("airport_code", "");
                config.environment_model.runway_code = env_model.value("runway_code", "");
                config.environment_model.environment_type = env_model.value("environment_type", "");
                config.environment_model.description = env_model.value("description", "");
            }
            
            // 解析跑道数据
            if (json_data.contains("runway_data")) {
                const auto& runway = json_data["runway_data"];
                config.runway_data.length = runway.value("length", 3800.0);
                config.runway_data.width = runway.value("width", 60.0);
                config.runway_data.surface_type = runway.value("surface_type", "沥青");
                config.runway_data.friction_coefficient = runway.value("friction_coefficient", 0.8);
                config.runway_data.condition = runway.value("condition", "干");
                config.runway_data.is_available = runway.value("is_available", true);
                config.runway_data.elevation = runway.value("elevation", 35.0);
                config.runway_data.slope = runway.value("slope", 0.0);
                config.runway_data.heading = runway.value("heading", 0.0);
                config.runway_data.ils_frequency = runway.value("ils_frequency", "");
                config.runway_data.approach_lights = runway.value("approach_lights", "");
            }
            
            // 解析大气数据
            if (json_data.contains("atmospheric_data")) {
                const auto& atmospheric = json_data["atmospheric_data"];
                config.atmospheric_data.temperature = atmospheric.value("temperature", 15.0);
                config.atmospheric_data.pressure = atmospheric.value("pressure", 1013.25);
                config.atmospheric_data.humidity = atmospheric.value("humidity", 50.0);
                config.atmospheric_data.visibility = atmospheric.value("visibility", 10000.0);
                config.atmospheric_data.density_altitude = atmospheric.value("density_altitude", 35.0);
                config.atmospheric_data.dew_point = atmospheric.value("dew_point", 5.0);
                config.atmospheric_data.air_density = atmospheric.value("air_density", 1.225);
                config.atmospheric_data.cloud_cover = atmospheric.value("cloud_cover", "少云");
                config.atmospheric_data.cloud_base = atmospheric.value("cloud_base", 1000.0);
                config.atmospheric_data.ceiling = atmospheric.value("ceiling", 1500.0);
                config.atmospheric_data.precipitation = atmospheric.value("precipitation", "无");
                config.atmospheric_data.precipitation_intensity = atmospheric.value("precipitation_intensity", 0.0);
            }
            
            // 解析风数据
            if (json_data.contains("wind_data")) {
                const auto& wind = json_data["wind_data"];
                config.wind_data.wind_speed = wind.value("wind_speed", 5.0);
                config.wind_data.wind_direction = wind.value("wind_direction", 0.0);
                config.wind_data.gust_speed = wind.value("gust_speed", 0.0);
                config.wind_data.crosswind_component = wind.value("crosswind_component", 0.0);
                config.wind_data.headwind_component = wind.value("headwind_component", 5.0);
                config.wind_data.wind_shear = wind.value("wind_shear", 0.0);
                config.wind_data.wind_condition = wind.value("wind_condition", "轻风");
                config.wind_data.is_turbulent = wind.value("is_turbulent", false);
                config.wind_data.wind_altitude = wind.value("wind_altitude", 10.0);
            }
            
            // 解析天气模型
            if (json_data.contains("weather_model")) {
                const auto& weather = json_data["weather_model"];
                config.weather_model.weather_stability = weather.value("weather_stability", 0.8);
                config.weather_model.change_rate = weather.value("change_rate", 0.1);
                config.weather_model.default_weather = weather.value("default_weather", "CLEAR");
            }
            
            // 解析更新参数
            if (json_data.contains("update_parameters")) {
                const auto& update = json_data["update_parameters"];
                
                if (update.contains("temperature_change_range") && update["temperature_change_range"].is_array()) {
                    auto temp_range = update["temperature_change_range"];
                    if (temp_range.size() >= 2) {
                        config.update_parameters.temperature_change_range = {temp_range[0], temp_range[1]};
                    }
                }
                
                if (update.contains("wind_change_range") && update["wind_change_range"].is_array()) {
                    auto wind_range = update["wind_change_range"];
                    if (wind_range.size() >= 2) {
                        config.update_parameters.wind_change_range = {wind_range[0], wind_range[1]};
                    }
                }
                
                if (update.contains("pressure_change_range") && update["pressure_change_range"].is_array()) {
                    auto pressure_range = update["pressure_change_range"];
                    if (pressure_range.size() >= 2) {
                        config.update_parameters.pressure_change_range = {pressure_range[0], pressure_range[1]};
                    }
                }
                
                config.update_parameters.update_frequency = update.value("update_frequency", 1.0);
                config.update_parameters.random_seed = update.value("random_seed", 42);
            }
            
            return true;
            
        } catch (const std::exception& e) {
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "解析JSON配置失败: " + std::string(e.what()));
            return false;
        }
    }

    std::string EnvironmentConfigManager::get_config_file_path(const std::string& model_name) {
        return base_config_path + model_name + "/DataTwin/environment_config.json";
    }

    void EnvironmentConfigManager::validate_config(const EnvironmentConfig& config) {
        // 基本验证
        if (config.environment_model.name.empty()) {
            throw std::runtime_error("环境模型名称不能为空");
        }
        
        if (config.runway_data.length <= 0 || config.runway_data.width <= 0) {
            throw std::runtime_error("跑道长度和宽度必须大于0");
        }
        
        if (config.atmospheric_data.temperature < -50 || config.atmospheric_data.temperature > 60) {
            throw std::runtime_error("温度超出合理范围");
        }
        
        if (config.wind_data.wind_speed < 0) {
            throw std::runtime_error("风速不能为负数");
        }
        
        if (config.wind_data.wind_direction < 0 || config.wind_data.wind_direction > 360) {
            throw std::runtime_error("风向必须在0-360度范围内");
        }
        
        if (config.weather_model.weather_stability < 0 || config.weather_model.weather_stability > 1) {
            throw std::runtime_error("天气稳定性必须在0-1范围内");
        }
        
        VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "配置验证通过: " + config.environment_model.name);
    }

} // namespace VFT_SMF
