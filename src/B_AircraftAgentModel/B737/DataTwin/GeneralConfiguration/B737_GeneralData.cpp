/**
 * @file B737_GeneralData.cpp
 * @brief B737飞机总体数据实现
 * @details 包含B737飞机的实际数据，基于公开技术规格和手册
 * @author VFT_SMF Framework
 * @date 2024
 */

#include "B737_GeneralData.hpp"
#include <cmath>

namespace SMF {
namespace AircraftDigitalTwin {
namespace B737 {

// ==================== 构造方法实现 ====================
B737GeneralData::B737GeneralData() : aircraft_type(""), manufacturer(""), icao_code(""), iata_code(""),
                                    length(0.0), wingspan(0.0), height(0.0), wing_area(0.0),
                                    wing_chord_mean(0.0), wing_chord_root(0.0), wing_chord_tip(0.0),
                                    wing_sweep(0.0), wing_dihedral(0.0), wing_incidence(0.0),
                                    aspect_ratio(0.0), taper_ratio(0.0), empty_weight(0.0),
                                    max_takeoff_weight(0.0), max_landing_weight(0.0), max_zero_fuel_weight(0.0),
                                    max_fuel_capacity(0.0), max_payload(0.0), cg_empty(0.0),
                                    cg_forward_limit(0.0), cg_aft_limit(0.0), cg_range(0.0),
                                    landing_gear_height(0.0), main_gear_track(0.0), nose_gear_to_main_gear(0.0),
                                    engine_type(""), engine_count(0), engine_thrust_max(0.0),
                                    engine_thrust_takeoff(0.0), engine_thrust_climb(0.0), engine_thrust_cruise(0.0),
                                    max_speed(0.0), cruise_speed(0.0), stall_speed_clean(0.0), stall_speed_landing(0.0),
                                    max_altitude(0.0), service_ceiling(0.0), max_range(0.0), max_endurance(0.0),
                                    fuel_flow_cruise(0.0), fuel_flow_climb(0.0), fuel_flow_descent(0.0), fuel_flow_idle(0.0),
                                    vne(0.0), vno(0.0), vfe(0.0), vle(0.0), vlo(0.0) {}

// ==================== 数据验证方法实现 ====================
bool B737GeneralData::validate_data() const {
    if (aircraft_type.empty() || manufacturer.empty()) return false;
    if (length <= 0.0 || wingspan <= 0.0 || wing_area <= 0.0) return false;
    if (empty_weight <= 0.0 || max_takeoff_weight <= 0.0) return false;
    if (engine_count <= 0 || engine_thrust_max <= 0.0) return false;
    return true;
}

std::string B737GeneralData::get_validation_report() const {
    std::string report = "B737数据验证报告:\n";
    
    if (aircraft_type.empty()) report += "- 错误: 飞机型号为空\n";
    if (manufacturer.empty()) report += "- 错误: 制造商为空\n";
    if (length <= 0.0) report += "- 错误: 机身长度无效\n";
    if (wingspan <= 0.0) report += "- 错误: 翼展无效\n";
    if (wing_area <= 0.0) report += "- 错误: 机翼面积无效\n";
    if (empty_weight <= 0.0) report += "- 错误: 空重无效\n";
    if (max_takeoff_weight <= 0.0) report += "- 错误: 最大起飞重量无效\n";
    if (engine_count <= 0) report += "- 错误: 发动机数量无效\n";
    if (engine_thrust_max <= 0.0) report += "- 错误: 发动机推力无效\n";
    
    if (validate_data()) {
        report += "- 状态: 数据验证通过\n";
        report += "- 翼载: " + std::to_string(get_wing_loading()) + " kg/m²\n";
        report += "- 推重比: " + std::to_string(get_thrust_to_weight_ratio()) + "\n";
    }
    
    return report;
}

// ==================== 数据访问方法实现 ====================
double B737GeneralData::get_wing_loading() const {
    if (wing_area <= 0.0) return 0.0;
    return max_takeoff_weight / wing_area;
}

double B737GeneralData::get_power_loading() const {
    if (engine_thrust_max <= 0.0) return 0.0;
    return max_takeoff_weight / (engine_count * engine_thrust_max);
}

double B737GeneralData::get_thrust_to_weight_ratio() const {
    if (max_takeoff_weight <= 0.0) return 0.0;
    return (engine_count * engine_thrust_max) / max_takeoff_weight;
}

// ==================== B737-800数据 (基于公开技术规格) ====================
const B737GeneralData B737_800_DATA = []() {
    B737GeneralData data;
    
    // 基本信息
    data.aircraft_type = "B737-800";
    data.manufacturer = "Boeing";
    data.icao_code = "B738";
    data.iata_code = "738";
    
    // 几何尺寸
    data.length = 39.47;      // 机身长度 (m) - 129.6 ft
    data.wingspan = 35.79;    // 翼展 (m) - 117.4 ft
    data.height = 12.55;      // 机身高度 (m) - 41.2 ft
    data.wing_area = 124.6;   // 机翼面积 (m²) - 1341 ft²
    data.wing_chord_mean = 3.48;  // 平均气动弦长 (m) - 11.4 ft
    data.wing_chord_root = 4.17;  // 根弦长 (m) - 13.7 ft
    data.wing_chord_tip = 1.24;   // 尖弦长 (m) - 4.1 ft
    data.wing_sweep = 25.0;   // 后掠角 (度)
    data.wing_dihedral = 6.0; // 上反角 (度)
    data.wing_incidence = 1.0; // 安装角 (度)
    data.aspect_ratio = 10.3; // 展弦比
    data.taper_ratio = 0.297; // 尖削比
    
    // 重量数据
    data.empty_weight = 41413.0;        // 空重 (kg) - 91,300 lb
    data.max_takeoff_weight = 79015.0;  // 最大起飞重量 (kg) - 174,200 lb
    data.max_landing_weight = 66360.0;  // 最大着陆重量 (kg) - 146,300 lb
    data.max_zero_fuel_weight = 62773.0; // 最大零燃油重量 (kg) - 138,400 lb
    data.max_fuel_capacity = 20820.0;   // 最大燃油容量 (kg) - 45,900 lb
    data.max_payload = 21360.0;         // 最大载重 (kg) - 47,100 lb
    
    // 重心数据
    data.cg_empty = 25.0;      // 空机重心位置 (%MAC)
    data.cg_forward_limit = 15.0; // 前重心限制 (%MAC)
    data.cg_aft_limit = 35.0;  // 后重心限制 (%MAC)
    data.cg_range = 20.0;      // 重心范围 (%MAC)
    
    // 起落架数据
    data.landing_gear_height = 2.67;    // 起落架高度 (m) - 8.75 ft
    data.main_gear_track = 5.72;        // 主起落架轮距 (m) - 18.75 ft
    data.nose_gear_to_main_gear = 12.6; // 前起落架到主起落架距离 (m) - 41.3 ft
    
    // 发动机数据
    data.engine_type = "CFM56-7B26";
    data.engine_count = 2;
    data.engine_thrust_max = 117000.0;   // 单发最大推力 (N) - 26,300 lbf
    data.engine_thrust_takeoff = 117000.0; // 起飞推力 (N)
    data.engine_thrust_climb = 105300.0;   // 爬升推力 (N) - 23,670 lbf
    data.engine_thrust_cruise = 82000.0;   // 巡航推力 (N) - 18,430 lbf
    
    // 性能数据
    data.max_speed = 270.0;    // 最大速度 (m/s) - 0.82 Mach
    data.cruise_speed = 235.0; // 巡航速度 (m/s) - 0.785 Mach
    data.stall_speed_clean = 58.0;    // 净构型失速速度 (m/s) - 113 knots
    data.stall_speed_landing = 52.0;  // 着陆构型失速速度 (m/s) - 101 knots
    data.max_altitude = 12500.0;      // 最大高度 (m) - 41,000 ft
    data.service_ceiling = 12500.0;   // 实用升限 (m) - 41,000 ft
    data.max_range = 5760.0;          // 最大航程 (km) - 3,115 nm
    data.max_endurance = 7.5;         // 最大续航时间 (小时)
    
    // 燃油消耗数据
    data.fuel_flow_cruise = 2400.0;   // 巡航燃油流量 (kg/h) - 5,290 lb/h
    data.fuel_flow_climb = 3200.0;    // 爬升燃油流量 (kg/h) - 7,050 lb/h
    data.fuel_flow_descent = 1800.0;  // 下降燃油流量 (kg/h) - 3,970 lb/h
    data.fuel_flow_idle = 400.0;      // 慢车燃油流量 (kg/h) - 880 lb/h
    
    // 飞行包线数据
    data.vne = 270.0;  // 最大允许速度 (m/s) - VNE
    data.vno = 250.0;  // 最大正常操作速度 (m/s) - VNO
    data.vfe = 180.0;  // 最大襟翼展开速度 (m/s) - VFE
    data.vle = 200.0;  // 最大起落架放下速度 (m/s) - VLE
    data.vlo = 200.0;  // 最大起落架操作速度 (m/s) - VLO
    
    return data;
}();

// ==================== B737-700数据 ====================
const B737GeneralData B737_700_DATA = []() {
    B737GeneralData data;
    
    // 基本信息
    data.aircraft_type = "B737-700";
    data.manufacturer = "Boeing";
    data.icao_code = "B737";
    data.iata_code = "737";
    
    // 几何尺寸
    data.length = 33.63;      // 机身长度 (m) - 110.4 ft
    data.wingspan = 35.79;    // 翼展 (m) - 117.4 ft
    data.height = 12.55;      // 机身高度 (m) - 41.2 ft
    data.wing_area = 124.6;   // 机翼面积 (m²) - 1341 ft²
    data.wing_chord_mean = 3.48;  // 平均气动弦长 (m) - 11.4 ft
    data.wing_chord_root = 4.17;  // 根弦长 (m) - 13.7 ft
    data.wing_chord_tip = 1.24;   // 尖弦长 (m) - 4.1 ft
    data.wing_sweep = 25.0;   // 后掠角 (度)
    data.wing_dihedral = 6.0; // 上反角 (度)
    data.wing_incidence = 1.0; // 安装角 (度)
    data.aspect_ratio = 10.3; // 展弦比
    data.taper_ratio = 0.297; // 尖削比
    
    // 重量数据
    data.empty_weight = 38147.0;        // 空重 (kg) - 84,100 lb
    data.max_takeoff_weight = 70080.0;  // 最大起飞重量 (kg) - 154,500 lb
    data.max_landing_weight = 61235.0;  // 最大着陆重量 (kg) - 135,000 lb
    data.max_zero_fuel_weight = 58060.0; // 最大零燃油重量 (kg) - 128,000 lb
    data.max_fuel_capacity = 20820.0;   // 最大燃油容量 (kg) - 45,900 lb
    data.max_payload = 19913.0;         // 最大载重 (kg) - 43,900 lb
    
    // 重心数据
    data.cg_empty = 25.0;      // 空机重心位置 (%MAC)
    data.cg_forward_limit = 15.0; // 前重心限制 (%MAC)
    data.cg_aft_limit = 35.0;  // 后重心限制 (%MAC)
    data.cg_range = 20.0;      // 重心范围 (%MAC)
    
    // 起落架数据
    data.landing_gear_height = 2.67;    // 起落架高度 (m) - 8.75 ft
    data.main_gear_track = 5.72;        // 主起落架轮距 (m) - 18.75 ft
    data.nose_gear_to_main_gear = 10.8; // 前起落架到主起落架距离 (m) - 35.4 ft
    
    // 发动机数据
    data.engine_type = "CFM56-7B20";
    data.engine_count = 2;
    data.engine_thrust_max = 89000.0;    // 单发最大推力 (N) - 20,000 lbf
    data.engine_thrust_takeoff = 89000.0; // 起飞推力 (N)
    data.engine_thrust_climb = 80100.0;   // 爬升推力 (N) - 18,000 lbf
    data.engine_thrust_cruise = 62300.0;  // 巡航推力 (N) - 14,000 lbf
    
    // 性能数据
    data.max_speed = 270.0;    // 最大速度 (m/s) - 0.82 Mach
    data.cruise_speed = 235.0; // 巡航速度 (m/s) - 0.785 Mach
    data.stall_speed_clean = 56.0;    // 净构型失速速度 (m/s) - 109 knots
    data.stall_speed_landing = 50.0;  // 着陆构型失速速度 (m/s) - 97 knots
    data.max_altitude = 12500.0;      // 最大高度 (m) - 41,000 ft
    data.service_ceiling = 12500.0;   // 实用升限 (m) - 41,000 ft
    data.max_range = 6230.0;          // 最大航程 (km) - 3,365 nm
    data.max_endurance = 8.0;         // 最大续航时间 (小时)
    
    // 燃油消耗数据
    data.fuel_flow_cruise = 2000.0;   // 巡航燃油流量 (kg/h) - 4,410 lb/h
    data.fuel_flow_climb = 2600.0;    // 爬升燃油流量 (kg/h) - 5,730 lb/h
    data.fuel_flow_descent = 1500.0;  // 下降燃油流量 (kg/h) - 3,310 lb/h
    data.fuel_flow_idle = 350.0;      // 慢车燃油流量 (kg/h) - 770 lb/h
    
    // 飞行包线数据
    data.vne = 270.0;  // 最大允许速度 (m/s) - VNE
    data.vno = 250.0;  // 最大正常操作速度 (m/s) - VNO
    data.vfe = 180.0;  // 最大襟翼展开速度 (m/s) - VFE
    data.vle = 200.0;  // 最大起落架放下速度 (m/s) - VLE
    data.vlo = 200.0;  // 最大起落架操作速度 (m/s) - VLO
    
    return data;
}();

// ==================== B737-900数据 ====================
const B737GeneralData B737_900_DATA = []() {
    B737GeneralData data;
    
    // 基本信息
    data.aircraft_type = "B737-900";
    data.manufacturer = "Boeing";
    data.icao_code = "B739";
    data.iata_code = "739";
    
    // 几何尺寸
    data.length = 42.11;      // 机身长度 (m) - 138.2 ft
    data.wingspan = 35.79;    // 翼展 (m) - 117.4 ft
    data.height = 12.55;      // 机身高度 (m) - 41.2 ft
    data.wing_area = 124.6;   // 机翼面积 (m²) - 1341 ft²
    data.wing_chord_mean = 3.48;  // 平均气动弦长 (m) - 11.4 ft
    data.wing_chord_root = 4.17;  // 根弦长 (m) - 13.7 ft
    data.wing_chord_tip = 1.24;   // 尖弦长 (m) - 4.1 ft
    data.wing_sweep = 25.0;   // 后掠角 (度)
    data.wing_dihedral = 6.0; // 上反角 (度)
    data.wing_incidence = 1.0; // 安装角 (度)
    data.aspect_ratio = 10.3; // 展弦比
    data.taper_ratio = 0.297; // 尖削比
    
    // 重量数据
    data.empty_weight = 44452.0;        // 空重 (kg) - 98,000 lb
    data.max_takeoff_weight = 85140.0;  // 最大起飞重量 (kg) - 187,700 lb
    data.max_landing_weight = 71440.0;  // 最大着陆重量 (kg) - 157,500 lb
    data.max_zero_fuel_weight = 68039.0; // 最大零燃油重量 (kg) - 150,000 lb
    data.max_fuel_capacity = 20820.0;   // 最大燃油容量 (kg) - 45,900 lb
    data.max_payload = 23587.0;         // 最大载重 (kg) - 52,000 lb
    
    // 重心数据
    data.cg_empty = 25.0;      // 空机重心位置 (%MAC)
    data.cg_forward_limit = 15.0; // 前重心限制 (%MAC)
    data.cg_aft_limit = 35.0;  // 后重心限制 (%MAC)
    data.cg_range = 20.0;      // 重心范围 (%MAC)
    
    // 起落架数据
    data.landing_gear_height = 2.67;    // 起落架高度 (m) - 8.75 ft
    data.main_gear_track = 5.72;        // 主起落架轮距 (m) - 18.75 ft
    data.nose_gear_to_main_gear = 14.4; // 前起落架到主起落架距离 (m) - 47.2 ft
    
    // 发动机数据
    data.engine_type = "CFM56-7B27";
    data.engine_count = 2;
    data.engine_thrust_max = 121400.0;   // 单发最大推力 (N) - 27,300 lbf
    data.engine_thrust_takeoff = 121400.0; // 起飞推力 (N)
    data.engine_thrust_climb = 109260.0;   // 爬升推力 (N) - 24,570 lbf
    data.engine_thrust_cruise = 85000.0;   // 巡航推力 (N) - 19,100 lbf
    
    // 性能数据
    data.max_speed = 270.0;    // 最大速度 (m/s) - 0.82 Mach
    data.cruise_speed = 235.0; // 巡航速度 (m/s) - 0.785 Mach
    data.stall_speed_clean = 60.0;    // 净构型失速速度 (m/s) - 117 knots
    data.stall_speed_landing = 54.0;  // 着陆构型失速速度 (m/s) - 105 knots
    data.max_altitude = 12500.0;      // 最大高度 (m) - 41,000 ft
    data.service_ceiling = 12500.0;   // 实用升限 (m) - 41,000 ft
    data.max_range = 5925.0;          // 最大航程 (km) - 3,200 nm
    data.max_endurance = 7.2;         // 最大续航时间 (小时)
    
    // 燃油消耗数据
    data.fuel_flow_cruise = 2600.0;   // 巡航燃油流量 (kg/h) - 5,730 lb/h
    data.fuel_flow_climb = 3400.0;    // 爬升燃油流量 (kg/h) - 7,500 lb/h
    data.fuel_flow_descent = 1900.0;  // 下降燃油流量 (kg/h) - 4,190 lb/h
    data.fuel_flow_idle = 420.0;      // 慢车燃油流量 (kg/h) - 930 lb/h
    
    // 飞行包线数据
    data.vne = 270.0;  // 最大允许速度 (m/s) - VNE
    data.vno = 250.0;  // 最大正常操作速度 (m/s) - VNO
    data.vfe = 180.0;  // 最大襟翼展开速度 (m/s) - VFE
    data.vle = 200.0;  // 最大起落架放下速度 (m/s) - VLE
    data.vlo = 200.0;  // 最大起落架操作速度 (m/s) - VLO
    
    return data;
}();

} // namespace B737
} // namespace AircraftDigitalTwin
} // namespace SMF 