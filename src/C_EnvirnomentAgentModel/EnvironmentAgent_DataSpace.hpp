/**
 * @file EnvironmentAgent_DataSpace.hpp
 * @brief 环境代理模型数据结构定义
 * @author VFT_SMF Development Team
 * @date 2024
 * 
 * 这个头文件定义了环境代理模型使用的所有数据结构，
 * 包括跑道数据、大气数据和环境风数据。
 */

#pragma once

#include <string>

namespace VFT_SMF {
    namespace EnvirDataSpace {
        // ==================== 环境代理模型配置 ====================
        
        /**
         * @brief 环境代理模型配置结构体
         */
        struct EnvironmentAgentConfig {
            std::string environment_model_name;    ///< 环境模型名称
            std::string airport_code;              ///< 机场代码
            std::string runway_code;               ///< 跑道代码
            std::string weather_code;              ///< 天气代码
            
            EnvironmentAgentConfig() = default;
            
            EnvironmentAgentConfig(const std::string& model_name, 
                                  const std::string& airport, 
                                  const std::string& runway, 
                                  const std::string& weather)
                : environment_model_name(model_name), airport_code(airport), 
                  runway_code(runway), weather_code(weather) {}
        };

        // ==================== 合并环境数据结构体 ====================
        
        /**
         * @brief 环境代理数据合并结构体
         * 包含跑道数据、大气数据和环境风数据的完整环境信息
         */
        struct EnvironmentAgentData {
            // ==================== 跑道数据 ====================
            struct RunwayData {
                double length;                    ///< 跑道长度 (米)
                double width;                     ///< 跑道宽度 (米)
                std::string surface_type;         ///< 跑道表面类型
                double friction_coefficient;      ///< 摩擦系数 [0.0, 1.0]
                std::string condition;            ///< 跑道状况 (干/湿/雪/冰)
                bool is_available;                ///< 是否可用
                double elevation;                 ///< 跑道海拔高度 (米)
                double slope;                     ///< 跑道坡度 (度)
                
                RunwayData() : length(0.0), width(0.0), friction_coefficient(0.0), 
                              is_available(false), elevation(0.0), slope(0.0) {}
            } runway_data;                        ///< 跑道数据
            
            // ==================== 大气数据 ====================
            struct AtmosphericData {
                double temperature;               ///< 温度 (°C)
                double pressure;                  ///< 气压 (hPa)
                double humidity;                  ///< 湿度 (%)
                double visibility;                ///< 能见度 (米)
                double density_altitude;          ///< 密度高度 (米)
                double dew_point;                 ///< 露点温度 (°C)
                double air_density;               ///< 空气密度 (kg/m³)
                std::string cloud_cover;          ///< 云层覆盖情况
                double cloud_base;                ///< 云底高度 (米)
                
                AtmosphericData() : temperature(15.0), pressure(1013.25), humidity(50.0),
                                   visibility(10000.0), density_altitude(0.0), dew_point(5.0),
                                   air_density(1.225), cloud_base(1000.0) {}
            } atmospheric_data;                   ///< 大气数据
            
            // ==================== 环境风数据 ====================
            struct WindData {
                double wind_speed;                ///< 风速 (m/s)
                double wind_direction;            ///< 风向 (度，0-360)
                double gust_speed;                ///< 阵风风速 (m/s)
                double crosswind_component;       ///< 侧风分量 (m/s)
                double headwind_component;        ///< 顶风分量 (m/s)
                double wind_shear;                ///< 风切变 (m/s)
                std::string wind_condition;       ///< 风况描述
                bool is_turbulent;                ///< 是否湍流
                
                WindData() : wind_speed(0.0), wind_direction(0.0), gust_speed(0.0),
                             crosswind_component(0.0), headwind_component(0.0), wind_shear(0.0),
                             is_turbulent(false) {}
            } wind_data;                          ///< 环境风数据
            
            EnvironmentAgentData() = default;
            
            EnvironmentAgentData(const RunwayData& runway, 
                                const AtmosphericData& atmospheric, 
                                const WindData& wind)
                : runway_data(runway), atmospheric_data(atmospheric), wind_data(wind) {}
        };

    } // namespace EnvirDataSpace
} // namespace VFT_SMF
