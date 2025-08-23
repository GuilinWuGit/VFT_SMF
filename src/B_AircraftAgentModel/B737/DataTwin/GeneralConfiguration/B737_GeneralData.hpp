/**
 * @file B737_GeneralData.hpp
 * @brief B737飞机总体数据定义
 * @details 包含B737飞机的基本几何参数、重量分布、尺寸等总体数据
 * @author VFT_SMF Framework
 * @date 2024
 */

#ifndef B737_GENERAL_DATA_HPP
#define B737_GENERAL_DATA_HPP

#include <string>
#include <vector>
#include <map>

namespace SMF {
namespace AircraftDigitalTwin {
namespace B737 {

/**
 * @brief B737飞机总体数据结构体
 * @details 包含飞机的基本几何参数、重量分布、尺寸等总体数据
 */
struct B737GeneralData {
    // ==================== 基本信息 ====================
    std::string aircraft_type;           ///< 飞机型号 (B737-800)
    std::string manufacturer;            ///< 制造商 (Boeing)
    std::string icao_code;              ///< ICAO代码 (B738)
    std::string iata_code;              ///< IATA代码 (738)
    
    // ==================== 几何尺寸 ====================
    double length;                      ///< 机身长度 (m)
    double wingspan;                    ///< 翼展 (m)
    double height;                      ///< 机身高度 (m)
    double wing_area;                   ///< 机翼面积 (m²)
    double wing_chord_mean;             ///< 平均气动弦长 (m)
    double wing_chord_root;             ///< 根弦长 (m)
    double wing_chord_tip;              ///< 尖弦长 (m)
    double wing_sweep;                  ///< 后掠角 (度)
    double wing_dihedral;               ///< 上反角 (度)
    double wing_incidence;              ///< 安装角 (度)
    double aspect_ratio;                ///< 展弦比
    double taper_ratio;                 ///< 尖削比
    
    // ==================== 重量数据 ====================
    double empty_weight;                ///< 空重 (kg)
    double max_takeoff_weight;          ///< 最大起飞重量 (kg)
    double max_landing_weight;          ///< 最大着陆重量 (kg)
    double max_zero_fuel_weight;        ///< 最大零燃油重量 (kg)
    double max_fuel_capacity;           ///< 最大燃油容量 (kg)
    double max_payload;                 ///< 最大载重 (kg)
    
    // ==================== 重心数据 ====================
    double cg_empty;                    ///< 空机重心位置 (%MAC)
    double cg_forward_limit;            ///< 前重心限制 (%MAC)
    double cg_aft_limit;                ///< 后重心限制 (%MAC)
    double cg_range;                    ///< 重心范围 (%MAC)
    
    // ==================== 起落架数据 ====================
    double landing_gear_height;         ///< 起落架高度 (m)
    double main_gear_track;             ///< 主起落架轮距 (m)
    double nose_gear_to_main_gear;      ///< 前起落架到主起落架距离 (m)
    
    // ==================== 发动机数据 ====================
    std::string engine_type;            ///< 发动机型号
    int engine_count;                   ///< 发动机数量
    double engine_thrust_max;           ///< 单发最大推力 (N)
    double engine_thrust_takeoff;       ///< 起飞推力 (N)
    double engine_thrust_climb;         ///< 爬升推力 (N)
    double engine_thrust_cruise;        ///< 巡航推力 (N)
    
    // ==================== 性能数据 ====================
    double max_speed;                   ///< 最大速度 (m/s)
    double cruise_speed;                ///< 巡航速度 (m/s)
    double stall_speed_clean;           ///< 净构型失速速度 (m/s)
    double stall_speed_landing;         ///< 着陆构型失速速度 (m/s)
    double max_altitude;                ///< 最大高度 (m)
    double service_ceiling;             ///< 实用升限 (m)
    double max_range;                   ///< 最大航程 (km)
    double max_endurance;               ///< 最大续航时间 (小时)
    
    // ==================== 燃油消耗数据 ====================
    double fuel_flow_cruise;            ///< 巡航燃油流量 (kg/h)
    double fuel_flow_climb;             ///< 爬升燃油流量 (kg/h)
    double fuel_flow_descent;           ///< 下降燃油流量 (kg/h)
    double fuel_flow_idle;              ///< 慢车燃油流量 (kg/h)
    
    // ==================== 飞行包线数据 ====================
    double vne;                         ///< 最大允许速度 (m/s)
    double vno;                         ///< 最大正常操作速度 (m/s)
    double vfe;                         ///< 最大襟翼展开速度 (m/s)
    double vle;                         ///< 最大起落架放下速度 (m/s)
    double vlo;                         ///< 最大起落架操作速度 (m/s)
    
    // ==================== 构造方法 ====================
    B737GeneralData();
    
    // ==================== 数据验证方法 ====================
    bool validate_data() const;
    std::string get_validation_report() const;
    
    // ==================== 数据访问方法 ====================
    double get_wing_loading() const;    ///< 获取翼载 (kg/m²)
    double get_power_loading() const;   ///< 获取功率载荷 (kg/N)
    double get_thrust_to_weight_ratio() const; ///< 获取推重比
};

/**
 * @brief B737-800具体数据
 * @details B737-800型号的详细参数
 */
extern const B737GeneralData B737_800_DATA;

/**
 * @brief B737-700具体数据
 * @details B737-700型号的详细参数
 */
extern const B737GeneralData B737_700_DATA;

/**
 * @brief B737-900具体数据
 * @details B737-900型号的详细参数
 */
extern const B737GeneralData B737_900_DATA;

} // namespace B737
} // namespace AircraftDigitalTwin
} // namespace SMF

#endif // B737_GENERAL_DATA_HPP 