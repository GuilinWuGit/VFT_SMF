/**
 * @file ConfigManager.hpp
 * @brief 仿真配置管理器
 * @details 负责读取和管理仿真相关的配置文件
 * @author VFT_SMF Framework
 * @date 2024
 */

#ifndef CONFIG_MANAGER_HPP
#define CONFIG_MANAGER_HPP

#include <string>
#include <memory>
#include <fstream>
#include <iostream>
#include <sstream>
#include <regex>

namespace VFT_SMF {
namespace Config {

    /**
     * @brief 日志配置结构体
     */
    struct LogConfig {
        std::string brief_log_file;
        std::string detail_log_file;
        bool console_output;
        bool enable_logging; // 新增：是否启用日志系统
        
        LogConfig() : brief_log_file("output/simulation_brief.log"), 
                     detail_log_file("output/simulation_detail.log"), 
                     console_output(true),
                     enable_logging(false) {}
    };

    /**
     * @brief 数据记录器配置结构体
     */
    struct DataRecorderConfig {
        std::string output_directory;
        int buffer_size;
        
        DataRecorderConfig() : output_directory("output/simulation"), buffer_size(1000) {}
    };

    /**
     * @brief 仿真参数配置结构体
     */
    struct SimulationParams {
        double time_scale;
        double time_step;
        double max_simulation_time;
        double sync_tolerance;
        
        SimulationParams() : time_scale(1.0), time_step(0.01), max_simulation_time(300.0), sync_tolerance(0.001) {}
    };

    /**
     * @brief 仿真配置结构体
     */
    struct SimulationConfig {
        std::string flight_plan_file;
        LogConfig log_config;
        DataRecorderConfig data_recorder_config;
        SimulationParams simulation_params;
        
        SimulationConfig() : flight_plan_file("input/FlightPlan.json") {}
    };

    /**
     * @brief 配置管理器类
     */
    class ConfigManager {
    private:
        std::string config_file_path;
        SimulationConfig config;
        bool config_loaded;

    public:
        /**
         * @brief 构造函数
         * @param config_path 配置文件路径
         */
        ConfigManager(const std::string& config_path = "config/SimulationConfig.json");
        
        /**
         * @brief 析构函数
         */
        ~ConfigManager() = default;
        
        /**
         * @brief 加载配置文件
         * @return 是否成功加载
         */
        bool loadConfig();
        
        /**
         * @brief 获取仿真配置
         * @return 仿真配置引用
         */
        const SimulationConfig& getSimulationConfig() const;
        
        /**
         * @brief 获取飞行计划文件路径
         * @return 飞行计划文件路径
         */
        std::string getFlightPlanFile() const;
        
        /**
         * @brief 获取日志配置
         * @return 日志配置引用
         */
        const LogConfig& getLogConfig() const;
        
        /**
         * @brief 获取数据记录器配置
         * @return 数据记录器配置引用
         */
        const DataRecorderConfig& getDataRecorderConfig() const;
        
        /**
         * @brief 获取仿真参数
         * @return 仿真参数引用
         */
        const SimulationParams& getSimulationParams() const;
        
        /**
         * @brief 检查配置文件是否存在
         * @return 是否存在
         */
        bool configFileExists() const;
        
        /**
         * @brief 创建默认配置文件
         * @return 是否成功创建
         */
        bool createDefaultConfig();

    private:
        /**
         * @brief 从JSON字符串解析配置
         * @param json_str JSON字符串
         */
        void parseConfig(const std::string& json_str);
        
        /**
         * @brief 解析日志配置
         * @param json_str JSON字符串
         */
        void parseLogConfig(const std::string& json_str);
        
        /**
         * @brief 解析数据记录器配置
         * @param json_str JSON字符串
         */
        void parseDataRecorderConfig(const std::string& json_str);
        
        /**
         * @brief 解析仿真参数
         * @param json_str JSON字符串
         */
        void parseSimulationParams(const std::string& json_str);
        
        /**
         * @brief 从JSON字符串中提取字符串值
         * @param json_str JSON字符串
         * @param key 键名
         * @param default_value 默认值
         * @return 字符串值
         */
        std::string extractStringValue(const std::string& json_str, const std::string& key, const std::string& default_value = "");
        
        /**
         * @brief 从JSON字符串中提取布尔值
         * @param json_str JSON字符串
         * @param key 键名
         * @param default_value 默认值
         * @return 布尔值
         */
        bool extractBoolValue(const std::string& json_str, const std::string& key, bool default_value = false);
        
        /**
         * @brief 从JSON字符串中提取整数值
         * @param json_str JSON字符串
         * @param key 键名
         * @param default_value 默认值
         * @return 整数值
         */
        int extractIntValue(const std::string& json_str, const std::string& key, int default_value = 0);
        
        /**
         * @brief 从JSON字符串中提取双精度值
         * @param json_str JSON字符串
         * @param key 键名
         * @param default_value 默认值
         * @return 双精度值
         */
        double extractDoubleValue(const std::string& json_str, const std::string& key, double default_value = 0.0);
    };

} // namespace Config
} // namespace VFT_SMF

#endif // CONFIG_MANAGER_HPP
