/**
 * @file DataRecorder.hpp
 * @brief 数据记录器定义
 * @author VFT_SMF Development Team
 * @date 2024
 */

#pragma once

#include "../../E_GlobalSharedDataSpace/GlobalSharedDataStruct.hpp"
#include "../LogAndData/Logger.hpp"

// 前向声明
namespace VFT_SMF {
    namespace GlobalShared_DataSpace {
        class GlobalSharedDataSpace;
    }
}
#include <vector>
#include <deque>
#include <string>
#include <fstream>
#include <mutex>
#include <memory>

namespace VFT_SMF {

class DataRecorder {
private:
    // 数据缓冲区 - 对应17个数据模块
    std::deque<std::pair<double, VFT_SMF::GlobalSharedDataStruct::FlightPlanData>> flight_plan_buffer;
    std::deque<std::pair<double, VFT_SMF::GlobalSharedDataStruct::AircraftFlightState>> aircraft_flight_state_buffer;
    std::deque<std::pair<double, VFT_SMF::GlobalSharedDataStruct::AircraftSystemState>> aircraft_system_state_buffer;
    std::deque<std::pair<double, VFT_SMF::GlobalSharedDataStruct::PilotGlobalState>> pilot_state_buffer;
    std::deque<std::pair<double, VFT_SMF::GlobalSharedDataStruct::EnvironmentGlobalState>> environment_state_buffer;
    std::deque<std::pair<double, VFT_SMF::GlobalSharedDataStruct::ATCGlobalState>> atc_state_buffer;
    std::deque<std::pair<double, VFT_SMF::GlobalSharedDataStruct::AircraftNetForce>> aircraft_net_force_buffer;
    std::deque<std::pair<double, VFT_SMF::GlobalSharedDataStruct::AircraftGlobalLogic>> aircraft_logic_buffer;
    std::deque<std::pair<double, VFT_SMF::GlobalSharedDataStruct::PilotGlobalLogic>> pilot_logic_buffer;
    std::deque<std::pair<double, VFT_SMF::GlobalSharedDataStruct::EnvironmentGlobalLogic>> environment_logic_buffer;
    std::deque<std::pair<double, VFT_SMF::GlobalSharedDataStruct::ATCGlobalLogic>> atc_logic_buffer;
    std::deque<std::pair<double, VFT_SMF::GlobalSharedDataStruct::PlannedEventLibrary>> planned_event_buffer;
    std::deque<std::pair<double, VFT_SMF::GlobalSharedDataStruct::TriggeredEventLibrary>> triggered_event_buffer;
    std::deque<std::pair<double, VFT_SMF::GlobalSharedDataStruct::ATC_Command>> atc_command_buffer;
    std::deque<std::pair<double, VFT_SMF::GlobalSharedDataStruct::PlanedControllersLibrary>> planed_controllers_buffer;
    std::deque<std::pair<double, VFT_SMF::GlobalSharedDataStruct::ControllerExecutionStatus>> controller_execution_status_buffer;
    std::deque<std::pair<double, VFT_SMF::GlobalSharedDataStruct::EventQueue>> event_queue_buffer;

    std::string output_directory;
    int buffer_size;
    bool is_initialized;
    mutable std::mutex buffer_mutex;

public:
    DataRecorder(const std::string& output_dir = "output/simulation", int buf_size = 1000);
    ~DataRecorder();
    
    bool initialize();
    void setBufferSize(int size);
    void setOutputDirectory(const std::string& dir);
    
    // 记录17个数据模块的方法
    void recordFlightPlanData(double simulation_time, const VFT_SMF::GlobalSharedDataStruct::FlightPlanData& data);
    void recordAircraftFlightState(double simulation_time, const VFT_SMF::GlobalSharedDataStruct::AircraftFlightState& data);
    void recordAircraftSystemState(double simulation_time, const VFT_SMF::GlobalSharedDataStruct::AircraftSystemState& data);
    void recordPilotState(double simulation_time, const VFT_SMF::GlobalSharedDataStruct::PilotGlobalState& data);
    void recordEnvironmentState(double simulation_time, const VFT_SMF::GlobalSharedDataStruct::EnvironmentGlobalState& data);
    void recordATCState(double simulation_time, const VFT_SMF::GlobalSharedDataStruct::ATCGlobalState& data);
    void recordAircraftNetForce(double simulation_time, const VFT_SMF::GlobalSharedDataStruct::AircraftNetForce& data);
    void recordAircraftLogic(double simulation_time, const VFT_SMF::GlobalSharedDataStruct::AircraftGlobalLogic& data);
    void recordPilotLogic(double simulation_time, const VFT_SMF::GlobalSharedDataStruct::PilotGlobalLogic& data);
    void recordEnvironmentLogic(double simulation_time, const VFT_SMF::GlobalSharedDataStruct::EnvironmentGlobalLogic& data);
    void recordATCLogic(double simulation_time, const VFT_SMF::GlobalSharedDataStruct::ATCGlobalLogic& data);
    void recordPlannedEvents(double simulation_time, const VFT_SMF::GlobalSharedDataStruct::PlannedEventLibrary& data);
    void recordTriggeredEvents(double simulation_time, const VFT_SMF::GlobalSharedDataStruct::TriggeredEventLibrary& data);
    void recordATCCommand(double simulation_time, const VFT_SMF::GlobalSharedDataStruct::ATC_Command& data);
    void recordPlanedControllers(double simulation_time, const VFT_SMF::GlobalSharedDataStruct::PlanedControllersLibrary& data);
    void recordControllerExecutionStatus(double simulation_time, const VFT_SMF::GlobalSharedDataStruct::ControllerExecutionStatus& data);
    void recordEventQueue(double simulation_time, const VFT_SMF::GlobalSharedDataStruct::EventQueue& data);
    
    void recordAllData(double simulation_time, VFT_SMF::GlobalShared_DataSpace::GlobalSharedDataSpace* shared_data_space);
    void flushAllBuffers();
    void clearAllBuffers();
    void clearOutputFiles();
    
    bool isInitialized() const { return is_initialized; }
    int getBufferSize() const { return buffer_size; }
    std::string getOutputDirectory() const { return output_directory; }
};

// 全局数据记录器实例
inline std::unique_ptr<DataRecorder> globalDataRecorder = nullptr;

// 初始化全局数据记录器
inline void initializeGlobalDataRecorder(const std::string& output_directory = "output/simulation", int buffer_size = 1000) {
    globalDataRecorder = std::make_unique<DataRecorder>(output_directory, buffer_size);
    if (!globalDataRecorder->initialize()) {
        throw std::runtime_error("Failed to initialize global data recorder");
    }
}

} // namespace VFT_SMF
