/**
 * @file EnvironmentAgent.hpp
 * @brief 环境代理模型定义
 * @author VFT_SMF Development Team
 * @date 2024
 * 
 * 环境代理模型负责模拟机场环境的各种条件，包括天气、跑道状况、大气条件等
 */

#pragma once

#include "../F_ScenarioModelling/B_ScenarioModel/VFT_SMF_Base.hpp"
#include "EnvironmentAgent_DataSpace.hpp"
#include <vector>
#include <queue>
#include <random>
#include <algorithm>
#include <memory>

// 前向声明：全局共享数据空间类型，避免在头文件中包含大型依赖
namespace VFT_SMF { namespace GlobalShared_DataSpace { class GlobalSharedDataSpace; } }

namespace VFT_SMF {

    /**
     * @brief 环境类型枚举
     */
    enum class EnvironmentType {
        AIRPORT_RUNWAY,     ///< 机场跑道环境
        TERMINAL_AREA,      ///< 航站区环境
        APPROACH_ZONE,      ///< 进近区环境
        DEPARTURE_ZONE,     ///< 离场区环境
        ENROUTE_ZONE        ///< 航路区环境
    };

    /**
     * @brief 天气状况枚举
     */
    enum class WeatherCondition {
        CLEAR,              ///< 晴朗
        CLOUDY,             ///< 多云
        RAIN,               ///< 降雨
        SNOW,               ///< 降雪
        FOG,                ///< 雾
        THUNDERSTORM,       ///< 雷暴
        WINDY,              ///< 大风
        TURBULENT           ///< 湍流
    };

    /**
     * @brief 环境事件类型枚举
     */
    enum class EnvironmentEventType {
        WEATHER_CHANGE,     ///< 天气变化
        WIND_SHIFT,         ///< 风向变化
        VISIBILITY_CHANGE,  ///< 能见度变化
        RUNWAY_CONDITION_CHANGE, ///< 跑道状况变化
        TURBULENCE_ALERT,   ///< 湍流警告
        WIND_SHEAR_ALERT,   ///< 风切变警告
        MICROBURST_ALERT,   ///< 微爆流警告
        THUNDERSTORM_APPROACH ///< 雷暴接近
    };

    /**
     * @brief 环境事件结构体
     */
    struct EnvironmentEvent {
        EnvironmentEventType type;
        std::string event_name;
        std::map<std::string, double> parameters;
        std::chrono::system_clock::time_point timestamp;
        double severity;  // 事件严重程度 [0.0, 1.0]
        
        EnvironmentEvent(EnvironmentEventType t, const std::string& name, double sev = 0.5)
            : type(t), event_name(name), timestamp(std::chrono::system_clock::now()), severity(sev) {}
    };

    /**
     * @brief 环境模型类
     */
    class EnvironmentModel {
    private:
        EnvironmentType env_type;
        WeatherCondition current_weather;
        double weather_stability;      // 天气稳定性 [0.0, 1.0]
        double change_rate;            // 变化速率 [0.0, 1.0]
        
        std::random_device rd;
        std::mt19937 gen;
        std::uniform_real_distribution<double> dist;

    public:
        EnvironmentModel(EnvironmentType type = EnvironmentType::AIRPORT_RUNWAY);
        
        void step(double delta_time); // 环境内部模型推进一步（仅代理内部调用）
        WeatherCondition get_current_weather() const { return current_weather; }
        double get_weather_stability() const { return weather_stability; }
        double get_change_rate() const { return change_rate; }
        
        // 设置方法
        void set_weather_condition(WeatherCondition weather) { current_weather = weather; }
        void set_weather_stability(double stability) { weather_stability = std::clamp(stability, 0.0, 1.0); }
        void set_change_rate(double rate) { change_rate = std::clamp(rate, 0.0, 1.0); }
    };

    /**
     * @brief 环境代理类
     */
    class EnvironmentAgent : public BaseAgent {
    private:
        // 环境模型
        std::unique_ptr<EnvironmentModel> environment_model;
        
        // 环境数据
        VFT_SMF::EnvirDataSpace::EnvironmentAgentData environment_data;
        VFT_SMF::EnvirDataSpace::EnvironmentAgentConfig config;
        
        // 全局共享数据空间引用（数据制造者需要访问）
        std::shared_ptr<VFT_SMF::GlobalShared_DataSpace::GlobalSharedDataSpace> global_data_space;
        
        // 事件处理
        std::vector<EnvironmentEvent> recent_events;
        std::queue<EnvironmentEvent> event_queue;
        
        // 性能统计
        int total_events_generated;
        int total_weather_changes;
        double average_update_time;
        
        // 环境特定参数
        std::string airport_code;
        std::string runway_code;
        EnvironmentType environment_type;
        
        // 环境模型名称（用于配置驱动）
        std::string environment_model_name;
        
        // 随机数生成器
        std::random_device rd;
        std::mt19937 gen;
        std::uniform_real_distribution<double> dist;

    public:
        EnvironmentAgent(const std::string& id, const std::string& name, 
                        const VFT_SMF::EnvirDataSpace::EnvironmentAgentConfig& env_config,
                        EnvironmentType type = EnvironmentType::AIRPORT_RUNWAY);
        ~EnvironmentAgent() override = default;

        // BaseAgent接口实现
        void initialize() override;
        void start() override;
        void pause() override;
        void resume() override;
        void stop() override;
        void update(double delta_time) override; // 代理对外的更新入口（由仿真循环调用）
        void handle_event(const Event& event) override;
        void send_event(const Event& event) override;
        std::string get_status() const override;
        bool is_ready() const override;

        // 环境代理特定方法
        void set_weather_condition(WeatherCondition weather);
        void set_runway_condition(const std::string& condition);
        void set_wind_conditions(double speed, double direction);
        void set_atmospheric_conditions(double temperature, double pressure, double humidity);
        
        // 环境数据访问
        VFT_SMF::EnvirDataSpace::EnvironmentAgentData get_environment_data() const;
        VFT_SMF::EnvirDataSpace::EnvironmentAgentConfig get_config() const;
        
        // 事件生成和处理
        EnvironmentEvent generate_environment_event();
        void process_environment_event(const EnvironmentEvent& event);
        std::vector<EnvironmentEvent> get_recent_events() const;
        
        // 性能评估
        double get_performance_score() const;
        std::string get_performance_report() const;
        
        // Getter方法
        EnvironmentType get_environment_type() const { return environment_type; }
        WeatherCondition get_current_weather() const;
        std::string get_airport_code() const { return airport_code; }
        std::string get_runway_code() const { return runway_code; }
        int get_total_events_generated() const { return total_events_generated; }
        int get_total_weather_changes() const { return total_weather_changes; }
        
        // 新增访问方法
        const EnvironmentModel* get_environment_model() const { return environment_model.get(); }
        
        // 设置全局共享数据空间（数据制造者需要）
        void set_global_data_space(std::shared_ptr<VFT_SMF::GlobalShared_DataSpace::GlobalSharedDataSpace> data_space) {
            global_data_space = data_space;
        }

        // ==================== 环境模型配置驱动 ====================
        void initializeEnvironmentModel(const std::string& model_name);
        std::string getEnvironmentModelName() const { return environment_model_name; }
        std::string getEnvironmentModelConfig() const;

        // ==================== 统一控制器接口 ====================
        bool executeController(const std::string& controller_name, 
                              const std::map<std::string, std::string>& params,
                              double current_time);
        int processAgentEventQueue(double current_time);
        
    private:
        // ==================== 私有方法 ====================
        
        /**
         * @brief 初始化环境数据
         */
        void initialize_environment_data();
        
        /**
         * @brief 更新环境数据
         * @param delta_time 时间间隔
         */
        void update_environment_data(double delta_time);
        
        /**
         * @brief 更新空气密度
         */
        void update_air_density();
        
        /**
         * @brief 将环境数据写入全局共享数据空间
         * 作为数据制造者，环境代理负责主动更新全局状态
         */
        void publish_to_global_data_space();

        // ==================== 环境控制器具体实现 ====================
        bool executeRunwayConditionChangeController(const std::map<std::string, std::string>& params, double current_time);
    };

} // namespace VFT_SMF
