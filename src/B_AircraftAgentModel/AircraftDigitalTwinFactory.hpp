/**
 * @file AircraftDigitalTwinFactory.hpp
 * @brief 飞机数字孪生工厂
 * @details 根据飞机类型创建对应的数字孪生实例
 * @author VFT_SMF Development Team
 * @date 2024
 */

#ifndef AIRCRAFT_DIGITAL_TWIN_FACTORY_HPP
#define AIRCRAFT_DIGITAL_TWIN_FACTORY_HPP

#include "IAircraftDigitalTwin.hpp"
#include "../F_ScenarioModelling/B_ScenarioModel/VFT_SMF_Base.hpp"
#include "AircraftAgent.hpp"
#include <memory>
#include <string>

namespace VFT_SMF {

    /**
     * @brief 飞机数字孪生工厂类
     * @details 提供统一的数字孪生创建接口，支持不同机型的动态创建
     */
    class AircraftDigitalTwinFactory {
    public:
        /**
         * @brief 创建数字孪生实例
         * @param type 飞机类型
         * @param id 飞机标识
         * @return 数字孪生实例指针
         */
        static std::unique_ptr<IAircraftDigitalTwin> create_digital_twin(AircraftType type, const std::string& id);
        
        /**
         * @brief 创建数字孪生实例（重载版本）
         * @param type_name 飞机类型名称字符串
         * @param id 飞机标识
         * @return 数字孪生实例指针
         */
        static std::unique_ptr<IAircraftDigitalTwin> create_digital_twin(const std::string& type_name, const std::string& id);
        
        /**
         * @brief 检查是否支持指定机型
         * @param type 飞机类型
         * @return 是否支持
         */
        static bool is_supported_aircraft_type(AircraftType type);
        
        /**
         * @brief 检查是否支持指定机型（重载版本）
         * @param type_name 飞机类型名称字符串
         * @return 是否支持
         */
        static bool is_supported_aircraft_type(const std::string& type_name);
        
        /**
         * @brief 获取支持的机型列表
         * @return 支持的机型名称列表
         */
        static std::vector<std::string> get_supported_aircraft_types();

    private:
        // 私有构造函数，防止实例化
        AircraftDigitalTwinFactory() = delete;
    };

} // namespace VFT_SMF

#endif // AIRCRAFT_DIGITAL_TWIN_FACTORY_HPP
