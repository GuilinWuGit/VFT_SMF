/**
 * @file ConfigManager.cpp
 * @brief 仿真配置管理器实现
 * @details 实现配置文件的读取、解析和管理功能
 * @author VFT_SMF Framework
 * @date 2024
 */

#include "ConfigManager.hpp"
#include <filesystem>
#include <iostream>
#include <sstream> // Required for std::stringstream
#include <fstream> // Required for std::ifstream
#include <regex>   // Required for std::regex and std::smatch

namespace VFT_SMF {
namespace Config {

    ConfigManager::ConfigManager(const std::string& config_path)
        : config_file_path(config_path), config_loaded(false) {
    }

    bool ConfigManager::loadConfig() {
        // 检查配置文件是否存在
        if (!configFileExists()) {
            std::cout << "配置文件不存在: " << config_file_path << std::endl;
            std::cout << "尝试创建默认配置文件..." << std::endl;
            
            if (!createDefaultConfig()) {
                std::cout << "无法创建默认配置文件，使用内置默认配置" << std::endl;
                config_loaded = true; // 使用默认配置
                return true;
            }
        }

        try {
            // 读取配置文件
            std::ifstream file(config_file_path);
            if (!file.is_open()) {
                std::cout << "无法打开配置文件: " << config_file_path << std::endl;
                return false;
            }

            std::stringstream buffer;
            buffer << file.rdbuf();
            std::string json_str = buffer.str();
            file.close();

            // 解析配置
            parseConfig(json_str);
            config_loaded = true;

            std::cout << "配置文件加载成功: " << config_file_path << std::endl;
            return true;

        } catch (const std::exception& e) {
            std::cout << "配置文件解析失败: " << e.what() << std::endl;
            std::cout << "使用内置默认配置" << std::endl;
            config_loaded = true; // 使用默认配置
            return true;
        }
    }

    const SimulationConfig& ConfigManager::getSimulationConfig() const {
        return config;
    }

    std::string ConfigManager::getFlightPlanFile() const {
        return config.flight_plan_file;
    }

    const LogConfig& ConfigManager::getLogConfig() const {
        return config.log_config;
    }

    const DataRecorderConfig& ConfigManager::getDataRecorderConfig() const {
        return config.data_recorder_config;
    }

    const SimulationParams& ConfigManager::getSimulationParams() const {
        return config.simulation_params;
    }

    bool ConfigManager::configFileExists() const {
        return std::filesystem::exists(config_file_path);
    }

    bool ConfigManager::createDefaultConfig() {
        try {
            // 创建配置目录
            std::filesystem::path config_dir = std::filesystem::path(config_file_path).parent_path();
            if (!std::filesystem::exists(config_dir)) {
                std::filesystem::create_directories(config_dir);
            }

            // 创建默认配置JSON字符串
            std::string default_config = R"({
    "simulation_config": {
        "flight_plan_file": "input/FlightPlan.json",
        "log_config": {
            "brief_log_file": "output/B737_Taxi_brief.log",
            "detail_log_file": "output/B737_Taxi_detail.log",
            "console_output": true,
            "enable_logging": true
        },
        "data_recorder_config": {
            "output_directory": "output/B737_Taxi",
            "buffer_size": 1000
        },
        "simulation_params": {
            "time_scale": 1.0,
            "time_step": 0.01,
            "max_simulation_time": 300.0,
            "sync_tolerance": 0.001
        }
    }
})";

            // 写入文件
            std::ofstream file(config_file_path);
            if (!file.is_open()) {
                std::cout << "无法创建配置文件: " << config_file_path << std::endl;
                return false;
            }

            file << default_config;
            file.close();

            std::cout << "默认配置文件已创建: " << config_file_path << std::endl;
            return true;

        } catch (const std::exception& e) {
            std::cout << "创建默认配置文件失败: " << e.what() << std::endl;
            return false;
        }
    }

    void ConfigManager::parseConfig(const std::string& json_str) {
        try {
            // 解析飞行计划文件路径
            config.flight_plan_file = extractStringValue(json_str, "flight_plan_file", "input/FlightPlan.json");

            // 解析日志配置
            parseLogConfig(json_str);

            // 解析数据记录器配置
            parseDataRecorderConfig(json_str);

            // 解析仿真参数
            parseSimulationParams(json_str);
        } catch (const std::exception& e) {
            std::cout << "配置解析错误: " << e.what() << std::endl;
            throw;
        }
    }

    void ConfigManager::parseLogConfig(const std::string& json_str) {
        config.log_config.brief_log_file = extractStringValue(json_str, "brief_log_file", "output/B737_Taxi_brief.log");
        config.log_config.detail_log_file = extractStringValue(json_str, "detail_log_file", "output/B737_Taxi_detail.log");
        config.log_config.console_output = extractBoolValue(json_str, "console_output", true);
        config.log_config.enable_logging = extractBoolValue(json_str, "enable_logging", true);
    }

    void ConfigManager::parseDataRecorderConfig(const std::string& json_str) {
        config.data_recorder_config.output_directory = extractStringValue(json_str, "output_directory", "output/B737_Taxi");
        config.data_recorder_config.buffer_size = extractIntValue(json_str, "buffer_size", 1000);
    }

    void ConfigManager::parseSimulationParams(const std::string& json_str) {
        config.simulation_params.time_scale = extractDoubleValue(json_str, "time_scale", 1.0);
        config.simulation_params.time_step = extractDoubleValue(json_str, "time_step", 0.01);
        config.simulation_params.max_simulation_time = extractDoubleValue(json_str, "max_simulation_time", 300.0);
        config.simulation_params.sync_tolerance = extractDoubleValue(json_str, "sync_tolerance", 0.001);
    }

    std::string ConfigManager::extractStringValue(const std::string& json_str, const std::string& key, const std::string& default_value) {
        std::regex pattern("\"" + key + "\"\\s*:\\s*\"([^\"]+)\"");
        std::smatch match;
        if (std::regex_search(json_str, match, pattern)) {
            return match[1].str();
        }
        return default_value;
    }

    bool ConfigManager::extractBoolValue(const std::string& json_str, const std::string& key, bool default_value) {
        std::regex pattern("\"" + key + "\"\\s*:\\s*(true|false)");
        std::smatch match;
        if (std::regex_search(json_str, match, pattern)) {
            return match[1].str() == "true";
        }
        return default_value;
    }

    int ConfigManager::extractIntValue(const std::string& json_str, const std::string& key, int default_value) {
        std::regex pattern("\"" + key + "\"\\s*:\\s*(\\d+)");
        std::smatch match;
        if (std::regex_search(json_str, match, pattern)) {
            return std::stoi(match[1].str());
        }
        return default_value;
    }

    double ConfigManager::extractDoubleValue(const std::string& json_str, const std::string& key, double default_value) {
        std::regex pattern("\"" + key + "\"\\s*:\\s*(\\d+\\.\\d+)");
        std::smatch match;
        if (std::regex_search(json_str, match, pattern)) {
            return std::stod(match[1].str());
        }
        return default_value;
    }

} // namespace Config
} // namespace VFT_SMF
