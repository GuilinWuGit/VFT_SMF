/**
 * @file ServiceTwin_StateManager.cpp
 * @brief 飞机数字孪生状态管理服务 实现
 */

#include "ServiceTwin_StateManager.hpp"
#include "../../../G_SimulationManager/LogAndData/Logger.hpp"

namespace VFT_SMF {

    ServiceTwin_StateManager::ServiceTwin_StateManager(const std::string& id, AircraftType type)
        : aircraft_id(id), aircraft_type(type), last_update_time(SimulationTimePoint{}),
          initialized(false), running(false), paused(false) {}

    void ServiceTwin_StateManager::initialize() {
        if (initialized) return;
        initialized = true;
        VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "ServiceTwin_StateManager 初始化完成");
    }

    void ServiceTwin_StateManager::start() {
        if (!initialized) initialize();
        running = true;
        paused = false;
        VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "ServiceTwin_StateManager 启动");
    }

    void ServiceTwin_StateManager::pause() {
        if (running) {
            paused = true;
            VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "ServiceTwin_StateManager 暂停");
        }
    }

    void ServiceTwin_StateManager::resume() {
        if (running && paused) {
            paused = false;
            VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "ServiceTwin_StateManager 恢复");
        }
    }

    void ServiceTwin_StateManager::stop() {
        running = false;
        paused = false;
        VFT_SMF::logDetail(VFT_SMF::LogLevel::Detail, "ServiceTwin_StateManager 停止");
    }

    void ServiceTwin_StateManager::update(double /*delta_time*/) {
        if (!running || paused) return;
        update_timestamps();
    }

    VFT_SMF::GlobalSharedDataStruct::AircraftSystemState ServiceTwin_StateManager::get_system_state(const std::string& system_name) const {
        std::shared_lock<std::shared_mutex> lock(system_state_mutex);
        auto it = system_states.find(system_name);
        if (it != system_states.end()) {
            return it->second;
        }
        return VFT_SMF::GlobalSharedDataStruct::AircraftSystemState{};
    }

    void ServiceTwin_StateManager::update_system_state(const std::string& system_name, const VFT_SMF::GlobalSharedDataStruct::AircraftSystemState& state) {
        std::unique_lock<std::shared_mutex> lock(system_state_mutex);
        system_states[system_name] = state;
        last_update_time = state.timestamp;
    }

    void ServiceTwin_StateManager::add_system(const std::string& name) {
        std::unique_lock<std::shared_mutex> lock(system_state_mutex);
        system_states.emplace(name, VFT_SMF::GlobalSharedDataStruct::AircraftSystemState{});
    }

    void ServiceTwin_StateManager::enable_system_auto_management(const std::string& /*system_name*/, bool /*enabled*/) {
        // 预留：自动管理开关
    }

    std::string ServiceTwin_StateManager::get_system_state_summary() const {
        std::shared_lock<std::shared_mutex> lock(system_state_mutex);
        return "系统数: " + std::to_string(system_states.size());
    }

    VFT_SMF::SimulationTimePoint ServiceTwin_StateManager::get_last_update_time() const {
        return last_update_time;
    }

    bool ServiceTwin_StateManager::is_initialized() const { return initialized; }
    bool ServiceTwin_StateManager::is_running() const { return running; }
    bool ServiceTwin_StateManager::is_paused() const { return paused; }
    std::string ServiceTwin_StateManager::get_aircraft_id() const { return aircraft_id; }
    AircraftType ServiceTwin_StateManager::get_aircraft_type() const { return aircraft_type; }

    void ServiceTwin_StateManager::validate_state_consistency() {
        // 预留：一致性检查
    }

    void ServiceTwin_StateManager::update_timestamps() {
        // 预留：统一更新时间戳
    }

} // namespace VFT_SMF
