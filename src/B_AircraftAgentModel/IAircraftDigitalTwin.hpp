/**
 * @file IAircraftDigitalTwin.hpp
 * @brief 飞机数字孪生抽象接口
 * @details 定义统一的飞机数字孪生接口，支持不同机型的统一操作
 * @author VFT_SMF Development Team
 * @date 2024
 */

#ifndef I_AIRCRAFT_DIGITAL_TWIN_HPP
#define I_AIRCRAFT_DIGITAL_TWIN_HPP

#include "../F_ScenarioModelling/B_ScenarioModel/VFT_SMF_Base.hpp"
#include "../E_GlobalSharedDataSpace/GlobalSharedDataStruct.hpp"
#include <memory>
#include <string>
#include <vector>

namespace VFT_SMF {

    // ==================== 前向声明 ====================
    struct Position3D;
    struct Velocity3D;
    struct Attitude;
    enum class SystemStatus;
    enum class FlightPhase;

    /**
     * @brief 飞机数字孪生抽象接口
     * @details 为不同机型提供统一的数字孪生操作接口
     */
    class IAircraftDigitalTwin {
    public:
        virtual ~IAircraftDigitalTwin() = default;

        // ==================== 初始化与生命周期 ====================
        virtual void initialize() = 0;
        virtual void start() = 0;
        virtual void pause() = 0;
        virtual void resume() = 0;
        virtual void stop() = 0;
        virtual void update(double delta_time) = 0;
        virtual bool is_initialized() const = 0;
        virtual bool is_running() const = 0;
        virtual bool is_paused() const = 0;

        // ==================== 统一的状态查询接口 ====================
        // 获取完整的飞行器系统状态
        virtual GlobalSharedDataStruct::AircraftSystemState getAircraftSystemState() const = 0;

        // ==================== 统一的控制接口 ====================
        virtual void set_control_inputs(double elevator, double aileron, double rudder, double throttle) = 0;
        virtual void set_flap_position(double position) = 0;
        virtual void set_gear_position(double position) = 0;
        virtual void set_brake_pressure(double pressure) = 0;
        virtual void set_spoiler_position(double position) = 0;
        virtual void set_trim_position(double position) = 0;

        // ==================== 统一的系统管理接口 ====================
        virtual void add_system(const std::string& name) = 0;
        virtual void update_system_status(const std::string& name, SystemStatus status) = 0;
        virtual void update_system_health(const std::string& name, double health) = 0;
        virtual void enable_system_auto_management(const std::string& system_name, bool enabled) = 0;

        // ==================== 统一的自动驾驶和导航接口 ====================
        virtual void enable_auto_pilot(bool enabled) = 0;
        virtual void set_target_waypoint(const Position3D& waypoint) = 0;
        virtual void execute_flight_plan(const std::string& plan_id) = 0;
        virtual bool is_auto_pilot_enabled() const = 0;
        virtual bool is_navigation_mode() const = 0;

        // ==================== 统一的安全监控接口 ====================
        virtual bool is_airworthy() const = 0;
        virtual bool check_stall_conditions() const = 0;
        virtual bool check_overspeed_conditions() const = 0;
        virtual bool check_altitude_limits() const = 0;
        virtual std::vector<std::string> get_warnings() const = 0;
        virtual std::vector<std::string> get_active_warnings() const = 0;
        virtual std::vector<std::string> get_active_cautions() const = 0;
        virtual void emergency_procedures() = 0;

        // ==================== 统一的性能监控接口 ====================
        virtual double get_performance_score() const = 0;
        virtual std::string get_performance_report() const = 0;
        virtual double get_total_flight_time() const = 0;
        virtual double get_total_distance() const = 0;
        virtual double get_max_altitude_reached() const = 0;
        virtual double get_max_speed_reached() const = 0;

        // ==================== 状态报告接口 ====================
        virtual std::string get_status() const = 0;
        virtual std::string get_comprehensive_status_report() const = 0;
        virtual std::string get_digital_twin_status() const = 0;

        // ==================== 飞机系统状态更新接口 ====================
        virtual void updateAircraftSystemState() = 0;
    };

} // namespace VFT_SMF

#endif // I_AIRCRAFT_DIGITAL_TWIN_HPP
