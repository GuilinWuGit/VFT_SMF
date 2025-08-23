/**
 * @file AircraftDigitalTwinFactory.cpp
 * @brief 飞机数字孪生工厂实现
 * @details 实现数字孪生工厂的创建逻辑
 * @author VFT_SMF Development Team
 * @date 2024
 */

#include "AircraftDigitalTwinFactory.hpp"
#include "B737/B737DigitalTwin.hpp"
#include <stdexcept>
#include <vector>

namespace VFT_SMF {

    std::unique_ptr<IAircraftDigitalTwin> AircraftDigitalTwinFactory::create_digital_twin(AircraftType type, const std::string& id) {
        switch (type) {
            case AircraftType::BOEING_737:
                return std::make_unique<B737DigitalTwin>(id);
            case AircraftType::AIRBUS_A320:
                // TODO: 实现A320数字孪生
                throw std::runtime_error("A320数字孪生尚未实现");
            case AircraftType::BOEING_777:
                // TODO: 实现B777数字孪生
                throw std::runtime_error("B777数字孪生尚未实现");
            case AircraftType::AIRBUS_A350:
                // TODO: 实现A350数字孪生
                throw std::runtime_error("A350数字孪生尚未实现");
            default:
                throw std::runtime_error("不支持的飞机类型: " + std::to_string(static_cast<int>(type)));
        }
    }

    std::unique_ptr<IAircraftDigitalTwin> AircraftDigitalTwinFactory::create_digital_twin(const std::string& type_name, const std::string& id) {
        if (type_name == "B737" || type_name == "BOEING_737" || type_name == "Boeing737") {
            return std::make_unique<B737DigitalTwin>(id);
        } else if (type_name == "A320" || type_name == "AIRBUS_A320" || type_name == "AirbusA320") {
            // TODO: 实现A320数字孪生
            throw std::runtime_error("A320数字孪生尚未实现");
        } else if (type_name == "B777" || type_name == "BOEING_777" || type_name == "Boeing777") {
            // TODO: 实现B777数字孪生
            throw std::runtime_error("B777数字孪生尚未实现");
        } else if (type_name == "A350" || type_name == "AIRBUS_A350" || type_name == "AirbusA350") {
            // TODO: 实现A350数字孪生
            throw std::runtime_error("A350数字孪生尚未实现");
        } else {
            throw std::runtime_error("不支持的飞机类型: " + type_name);
        }
    }

    bool AircraftDigitalTwinFactory::is_supported_aircraft_type(AircraftType type) {
        switch (type) {
            case AircraftType::BOEING_737:
                return true;
            case AircraftType::AIRBUS_A320:
            case AircraftType::BOEING_777:
            case AircraftType::AIRBUS_A350:
                return false; // 尚未实现
            default:
                return false;
        }
    }

    bool AircraftDigitalTwinFactory::is_supported_aircraft_type(const std::string& type_name) {
        return type_name == "B737" || type_name == "BOEING_737" || type_name == "Boeing737";
    }

    std::vector<std::string> AircraftDigitalTwinFactory::get_supported_aircraft_types() {
        return {"B737", "BOEING_737", "Boeing737"};
    }

} // namespace VFT_SMF
