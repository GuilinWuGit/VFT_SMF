/**
 * @file ServiceTwin_StateManager.hpp
 * @brief 飞机数字孪生状态管理服务
 * @author VFT_SMF Development Team
 * @date 2024
 * 
 * 简化的状态管理服务，只提供系统状态查询和管理功能
 */

#pragma once

#include "../../../F_ScenarioModelling/B_ScenarioModel/VFT_SMF_Base.hpp"
#include "../../../E_GlobalSharedDataSpace/GlobalSharedDataStruct.hpp"
#include "../../AircraftTypes.hpp"
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <map>

namespace VFT_SMF {

    /**
     * @brief 飞机状态管理服务
     * 统一管理飞机的系统状态信息，提供状态查询和管理功能
     */
    class ServiceTwin_StateManager {
    private:
        std::string aircraft_id;
        AircraftType aircraft_type;
        
        // 系统状态数据
        std::map<std::string, VFT_SMF::GlobalSharedDataStruct::AircraftSystemState> system_states;
        
        // 时间戳
        VFT_SMF::SimulationTimePoint last_update_time;
        
        // 线程安全
        mutable std::shared_mutex system_state_mutex;
        
        // 服务状态
        bool initialized;
        bool running;
        bool paused;

    public:
        /**
         * @brief 构造函数
         * @param id 飞机ID
         * @param type 飞机类型
         */
        ServiceTwin_StateManager(const std::string& id, AircraftType type);
        ~ServiceTwin_StateManager() = default;

        // ==================== 生命周期管理 ====================
        void initialize();
        void start();
        void pause();
        void resume();
        void stop();
        void update(double delta_time);

        // ==================== 系统状态查询接口 ====================
        VFT_SMF::GlobalSharedDataStruct::AircraftSystemState get_system_state(const std::string& system_name) const;
       

        // ==================== 系统状态更新接口 ====================
         void update_system_state(const std::string& system_name, const VFT_SMF::GlobalSharedDataStruct::AircraftSystemState& system_state);

        // ==================== 系统管理 ====================
        void add_system(const std::string& name);
        void enable_system_auto_management(const std::string& system_name, bool enabled);

        // ==================== 状态摘要和报告 ====================
        std::string get_system_state_summary() const;

        // ==================== 时间戳管理 ====================
        VFT_SMF::SimulationTimePoint get_last_update_time() const;

        // ==================== 服务状态查询 ====================
        bool is_initialized() const;
        bool is_running() const;
        bool is_paused() const;
        std::string get_aircraft_id() const;
        AircraftType get_aircraft_type() const;

    private:
        // 内部辅助方法
        void validate_state_consistency();
        void update_timestamps();
    };

} // namespace VFT_SMF
