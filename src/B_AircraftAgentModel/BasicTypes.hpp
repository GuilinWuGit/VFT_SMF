/**
 * @file BasicTypes.hpp
 * @brief 基础数据类型定义
 * @details 定义飞机代理模型中使用的基础数据结构
 * @author VFT_SMF Development Team
 * @date 2024
 */

#ifndef BASIC_TYPES_HPP
#define BASIC_TYPES_HPP

#include <cmath>

namespace VFT_SMF {

    // ==================== 基础数据类型定义 ====================

    /**
     * @brief 3D位置结构体
     */
    struct Position3D {
        double x, y, z;  // 米
        
        Position3D(double x = 0.0, double y = 0.0, double z = 0.0) 
            : x(x), y(y), z(z) {}
        
        double distance_to(const Position3D& other) const {
            return std::sqrt(std::pow(x - other.x, 2) + 
                           std::pow(y - other.y, 2) + 
                           std::pow(z - other.z, 2));
        }
    };

    /**
     * @brief 3D速度结构体
     */
    struct Velocity3D {
        double vx, vy, vz;  // 米/秒
        
        Velocity3D(double vx = 0.0, double vy = 0.0, double vz = 0.0) 
            : vx(vx), vy(vy), vz(vz) {}
        
        double magnitude() const {
            return std::sqrt(vx*vx + vy*vy + vz*vz);
        }
    };

    /**
     * @brief 姿态角结构体
     */
    struct Attitude {
        double roll;    // 滚转角 (度)
        double pitch;   // 俯仰角 (度)
        double yaw;     // 偏航角 (度)
        
        Attitude(double r = 0.0, double p = 0.0, double y = 0.0) 
            : roll(r), pitch(p), yaw(y) {}
    };

} // namespace VFT_SMF

#endif // BASIC_TYPES_HPP
