/**
 * @file AircraftTypes.hpp
 * @brief 飞机相关类型定义
 * @author VFT_SMF Development Team
 * @date 2024
 */

#ifndef AIRCRAFT_TYPES_HPP
#define AIRCRAFT_TYPES_HPP

#include <string>

namespace VFT_SMF {

    /**
     * @brief 飞机类型枚举
     */
    enum class AircraftType {
        CESSNA_172,     ///< 塞斯纳172
        BOEING_737,     ///< 波音737
        BOEING_777,     ///< 波音777
        AIRBUS_A320,    ///< 空客A320
        AIRBUS_A350,    ///< 空客A350
        F16_FIGHTER,    ///< F16战斗机
        HELICOPTER,     ///< 直升机
        CUSTOM          ///< 自定义飞机
    };

    /**
     * @brief 飞行阶段枚举
     */
    enum class FlightPhase {
        PREFLIGHT,      ///< 飞行前
        TAKEOFF,        ///< 起飞
        CLIMB,          ///< 爬升
        CRUISE,         ///< 巡航
        DESCENT,        ///< 下降
        APPROACH,       ///< 进近
        LANDING,        ///< 着陆
        TAXI,           ///< 滑行
        PARKED          ///< 停放
    };

    /**
     * @brief 飞机系统状态
     */
    enum class SystemStatus {
        NORMAL,         ///< 正常
        WARNING,        ///< 警告
        CAUTION,        ///< 注意
        FAILURE,        ///< 故障
        EMERGENCY       ///< 紧急
    };

} // namespace VFT_SMF

#endif // AIRCRAFT_TYPES_HPP
