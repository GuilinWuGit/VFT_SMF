/**
 * @file B737DigitalTwin.hpp
 * @brief B737飞机数字孪生体
 * @details 实现B737飞机的数字孪生功能，整合数据层、模型层和服务层
 * @author VFT_SMF Development Team
 * @date 2024
 */

#ifndef B737_DIGITAL_TWIN_HPP
#define B737_DIGITAL_TWIN_HPP

#include "../IAircraftDigitalTwin.hpp"
#include "../AircraftAgent.hpp"
#include "DataTwin/B737_DigitalTwin.hpp"
#include "ModelTwin/FlightControl/B737_AutoFlightControlLaw.hpp"
#include "ServiceTwin/ServiceTwin_StateManager.hpp"
#include <memory>
#include <string>

namespace VFT_SMF {

    /**
     * @brief B737飞机数字孪生类
     * @details 整合B737飞机的数据层、模型层和服务层，提供完整的数字孪生功能
     */
    class B737DigitalTwin : public IAircraftDigitalTwin {
    private:
        std::string aircraft_id;
        std::string aircraft_name;
        
        // ==================== 数据层组件 ====================
        // 暂时注释掉，因为B737_DigitalTwin是header-only实现
        // std::shared_ptr<SMF::AircraftDigitalTwin::B737::B737DigitalTwin> data_twin;
        
        // ==================== 模型层组件 ====================
        std::unique_ptr<SMF::AircraftDigitalTwin::B737::B737AutoFlightControlLaw> model_twin;
        
        // ==================== 服务层组件 ====================
        std::unique_ptr<ServiceTwin_StateManager> state_manager;
        
        // ==================== 状态管理 ====================
        bool initialized;
        bool running;
        bool paused;
        
        // ==================== 实时状态缓存 ====================
        mutable Position3D cached_position;
        mutable Velocity3D cached_velocity;
        mutable Attitude cached_attitude;
        mutable double cached_altitude;
        mutable double cached_airspeed;
        mutable double cached_ground_speed;
        mutable double cached_heading;
        mutable double cached_vertical_speed;
        mutable double cached_fuel_remaining;
        mutable double cached_engine_rpm;
        mutable double cached_throttle_position;
        mutable double cached_thrust;
        mutable double cached_power_output;
        mutable double cached_elevator_position;
        mutable double cached_aileron_position;
        mutable double cached_rudder_position;
        mutable double cached_flap_position;
        mutable double cached_gear_position;
        mutable double cached_brake_pressure;
        mutable double cached_current_mass;
        mutable double cached_center_of_gravity;
        mutable double cached_spoiler_position;

    public:
        // ==================== 构造和析构 ====================
        explicit B737DigitalTwin(const std::string& id);
        explicit B737DigitalTwin(const std::string& id, const std::string& name);
        ~B737DigitalTwin() override = default;

        // ==================== 初始化与生命周期 ====================
        void initialize() override;
        void start() override;
        void pause() override;
        void resume() override;
        void stop() override;
        void update(double delta_time) override;
        bool is_initialized() const override;
        bool is_running() const override;
        bool is_paused() const override;

        // ==================== 统一的状态查询接口 ====================
        GlobalSharedDataStruct::AircraftSystemState getAircraftSystemState() const override;

        // ==================== 统一的控制接口 ====================
        void set_control_inputs(double elevator, double aileron, double rudder, double throttle) override;
        void set_flap_position(double position) override;
        void set_gear_position(double position) override;
        void set_brake_pressure(double pressure) override;
        void set_spoiler_position(double position) override;
        void set_trim_position(double position) override;

        // ==================== 统一的系统管理接口 ====================
        void add_system(const std::string& name) override;
        void update_system_status(const std::string& name, SystemStatus status) override;
        void update_system_health(const std::string& name, double health) override;
        void enable_system_auto_management(const std::string& system_name, bool enabled) override;

        // ==================== 统一的自动驾驶和导航接口 ====================
        void enable_auto_pilot(bool enabled) override;
        void set_target_waypoint(const Position3D& waypoint) override;
        void execute_flight_plan(const std::string& plan_id) override;
        bool is_auto_pilot_enabled() const override;
        bool is_navigation_mode() const override;

        // ==================== 统一的安全监控接口 ====================
        bool is_airworthy() const override;
        bool check_stall_conditions() const override;
        bool check_overspeed_conditions() const override;
        bool check_altitude_limits() const override;
        std::vector<std::string> get_warnings() const override;
        std::vector<std::string> get_active_warnings() const override;
        std::vector<std::string> get_active_cautions() const override;
        void emergency_procedures() override;

        // ==================== 统一的性能监控接口 ====================
        double get_performance_score() const override;
        std::string get_performance_report() const override;
        double get_total_flight_time() const override;
        double get_total_distance() const override;
        double get_max_altitude_reached() const override;
        double get_max_speed_reached() const override;

        // ==================== 状态报告接口 ====================
        std::string get_status() const override;
        std::string get_comprehensive_status_report() const override;
        std::string get_digital_twin_status() const override;

        // ==================== 飞机系统状态更新接口 ====================
        void updateAircraftSystemState() override;

        // ==================== B737特有方法 ====================
        std::string get_aircraft_id() const { return aircraft_id; }
        std::string get_aircraft_name() const { return aircraft_name; }
        
        // 获取B737特有的数据层接口
        // 暂时注释掉，因为B737_DigitalTwin是header-only实现
        // const SMF::AircraftDigitalTwin::B737::B737DigitalTwin& get_data_twin() const { return *data_twin; }
        
        // 获取B737特有的模型层接口
        const SMF::AircraftDigitalTwin::B737::B737AutoFlightControlLaw& get_model_twin() const { return *model_twin; }

    private:
        // ==================== 私有辅助方法 ====================
        void initialize_components();
        void update_cached_states();
        void validate_initialization() const;
    };

} // namespace VFT_SMF

#endif // B737_DIGITAL_TWIN_HPP
