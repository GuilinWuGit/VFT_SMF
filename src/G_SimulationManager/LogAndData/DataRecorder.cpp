/**
 * @file DataRecorder.cpp
 * @brief 数据记录器实现
 * @author VFT_SMF Development Team
 * @date 2024
 */

#include "DataRecorder.hpp"
#include "../../E_GlobalSharedDataSpace/GlobalSharedDataSpace.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <cmath>

namespace VFT_SMF {

DataRecorder::DataRecorder(const std::string& output_dir, int buf_size)
    : output_directory(output_dir), buffer_size(buf_size), is_initialized(false) {
}

DataRecorder::~DataRecorder() {
    flushAllBuffers();
}

bool DataRecorder::initialize() {
    try {
        std::filesystem::create_directories(output_directory);
        
        // 清理之前的输出文件
        clearOutputFiles();
        
        is_initialized = true;
        // 预分配缓冲区容量，减少运行期重分配
        flight_plan_buffer.resize(0);
        aircraft_flight_state_buffer.resize(0);
        aircraft_system_state_buffer.resize(0);
        pilot_state_buffer.resize(0);
        environment_state_buffer.resize(0);
        atc_state_buffer.resize(0);
        aircraft_net_force_buffer.resize(0);
        aircraft_logic_buffer.resize(0);
        pilot_logic_buffer.resize(0);
        environment_logic_buffer.resize(0);
        atc_logic_buffer.resize(0);
        planned_event_buffer.resize(0);
        triggered_event_buffer.resize(0);
        atc_command_buffer.resize(0);
        planed_controllers_buffer.resize(0);
        controller_execution_status_buffer.resize(0);
        event_queue_buffer.resize(0);

        VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "数据记录器初始化成功，输出目录: " + output_directory);
        return true;
    } catch (const std::exception& e) {
        VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "数据记录器初始化失败: " + std::string(e.what()));
        return false;
    }
}

void DataRecorder::setBufferSize(int size) {
    buffer_size = size;
}

void DataRecorder::setOutputDirectory(const std::string& dir) {
    output_directory = dir;
}

void DataRecorder::recordFlightPlanData(double simulation_time, const VFT_SMF::GlobalSharedDataStruct::FlightPlanData& data) {
    std::lock_guard<std::mutex> lock(buffer_mutex);
    flight_plan_buffer.push_back({simulation_time, data});
    if (flight_plan_buffer.size() > buffer_size) {
        flight_plan_buffer.pop_front();
    }
}

void DataRecorder::recordAircraftFlightState(double simulation_time, const VFT_SMF::GlobalSharedDataStruct::AircraftFlightState& data) {
    std::lock_guard<std::mutex> lock(buffer_mutex);
    aircraft_flight_state_buffer.push_back({simulation_time, data});
    
    // 只有在缓冲区真正满了才删除最旧的记录
    if (aircraft_flight_state_buffer.size() > buffer_size) {
        aircraft_flight_state_buffer.pop_front();
        if (VFT_SMF::globalLogger) {
            VFT_SMF::globalLogger->warning("飞行状态缓冲区已满，删除最旧记录，当前大小: " + std::to_string(aircraft_flight_state_buffer.size()));
        }
    }
}

void DataRecorder::recordAircraftSystemState(double simulation_time, const VFT_SMF::GlobalSharedDataStruct::AircraftSystemState& data) {
    std::lock_guard<std::mutex> lock(buffer_mutex);
    aircraft_system_state_buffer.push_back({simulation_time, data});
    if (aircraft_system_state_buffer.size() > buffer_size) {
        aircraft_system_state_buffer.pop_front();
    }
}

void DataRecorder::recordPilotState(double simulation_time, const VFT_SMF::GlobalSharedDataStruct::PilotGlobalState& data) {
    std::lock_guard<std::mutex> lock(buffer_mutex);
    pilot_state_buffer.push_back({simulation_time, data});
    if (pilot_state_buffer.size() > buffer_size) {
        pilot_state_buffer.pop_front();
    }
}

void DataRecorder::recordEnvironmentState(double simulation_time, const VFT_SMF::GlobalSharedDataStruct::EnvironmentGlobalState& data) {
    std::lock_guard<std::mutex> lock(buffer_mutex);
    environment_state_buffer.push_back({simulation_time, data});
    if (environment_state_buffer.size() > buffer_size) {
        environment_state_buffer.pop_front();
    }
}

void DataRecorder::recordATCState(double simulation_time, const VFT_SMF::GlobalSharedDataStruct::ATCGlobalState& data) {
    std::lock_guard<std::mutex> lock(buffer_mutex);
    atc_state_buffer.push_back({simulation_time, data});
    if (atc_state_buffer.size() > buffer_size) {
        atc_state_buffer.pop_front();
    }
}

void DataRecorder::recordAircraftNetForce(double simulation_time, const VFT_SMF::GlobalSharedDataStruct::AircraftNetForce& data) {
    std::lock_guard<std::mutex> lock(buffer_mutex);
    aircraft_net_force_buffer.push_back({simulation_time, data});
    if (aircraft_net_force_buffer.size() > buffer_size) {
        aircraft_net_force_buffer.pop_front();
    }
}

void DataRecorder::recordAircraftLogic(double simulation_time, const VFT_SMF::GlobalSharedDataStruct::AircraftGlobalLogic& data) {
    std::lock_guard<std::mutex> lock(buffer_mutex);
    aircraft_logic_buffer.push_back({simulation_time, data});
    if (aircraft_logic_buffer.size() > buffer_size) {
        aircraft_logic_buffer.pop_front();
    }
}

void DataRecorder::recordPilotLogic(double simulation_time, const VFT_SMF::GlobalSharedDataStruct::PilotGlobalLogic& data) {
    std::lock_guard<std::mutex> lock(buffer_mutex);
    pilot_logic_buffer.push_back({simulation_time, data});
    if (pilot_logic_buffer.size() > buffer_size) {
        pilot_logic_buffer.pop_front();
    }
}

void DataRecorder::recordEnvironmentLogic(double simulation_time, const VFT_SMF::GlobalSharedDataStruct::EnvironmentGlobalLogic& data) {
    std::lock_guard<std::mutex> lock(buffer_mutex);
    environment_logic_buffer.push_back({simulation_time, data});
    if (environment_logic_buffer.size() > buffer_size) {
        environment_logic_buffer.pop_front();
    }
}

void DataRecorder::recordATCLogic(double simulation_time, const VFT_SMF::GlobalSharedDataStruct::ATCGlobalLogic& data) {
    std::lock_guard<std::mutex> lock(buffer_mutex);
    atc_logic_buffer.push_back({simulation_time, data});
    if (atc_logic_buffer.size() > buffer_size) {
        atc_logic_buffer.pop_front();
    }
}

void DataRecorder::recordPlannedEvents(double simulation_time, const VFT_SMF::GlobalSharedDataStruct::PlannedEventLibrary& data) {
    std::lock_guard<std::mutex> lock(buffer_mutex);
    planned_event_buffer.push_back({simulation_time, data});
    if (planned_event_buffer.size() > buffer_size) {
        planned_event_buffer.pop_front();
    }
}

void DataRecorder::recordTriggeredEvents(double simulation_time, const VFT_SMF::GlobalSharedDataStruct::TriggeredEventLibrary& data) {
    std::lock_guard<std::mutex> lock(buffer_mutex);
    triggered_event_buffer.push_back({simulation_time, data});
    if (triggered_event_buffer.size() > buffer_size) {
        triggered_event_buffer.pop_front();
    }
}

void DataRecorder::recordATCCommand(double simulation_time, const VFT_SMF::GlobalSharedDataStruct::ATC_Command& data) {
    std::lock_guard<std::mutex> lock(buffer_mutex);
    atc_command_buffer.push_back({simulation_time, data});
    if (atc_command_buffer.size() > buffer_size) {
        atc_command_buffer.pop_front();
    }
}

void DataRecorder::recordPlanedControllers(double simulation_time, const VFT_SMF::GlobalSharedDataStruct::PlanedControllersLibrary& data) {
    std::lock_guard<std::mutex> lock(buffer_mutex);
    planed_controllers_buffer.push_back({simulation_time, data});
    if (planed_controllers_buffer.size() > buffer_size) {
        planed_controllers_buffer.pop_front();
    }
}

void DataRecorder::recordControllerExecutionStatus(double simulation_time, const VFT_SMF::GlobalSharedDataStruct::ControllerExecutionStatus& data) {
    std::lock_guard<std::mutex> lock(buffer_mutex);
    controller_execution_status_buffer.push_back({simulation_time, data});
    if (controller_execution_status_buffer.size() > buffer_size) {
        controller_execution_status_buffer.pop_front();
    }
}

void DataRecorder::recordEventQueue(double simulation_time, const VFT_SMF::GlobalSharedDataStruct::EventQueue& data) {
    std::lock_guard<std::mutex> lock(buffer_mutex);
    event_queue_buffer.push_back({simulation_time, data});
    if (event_queue_buffer.size() > buffer_size) {
        event_queue_buffer.pop_front();
    }
}

void DataRecorder::recordAllData(double simulation_time, VFT_SMF::GlobalShared_DataSpace::GlobalSharedDataSpace* shared_data_space) {
    if (!shared_data_space) return;
    
    recordFlightPlanData(simulation_time, shared_data_space->getFlightPlanData());
    recordAircraftFlightState(simulation_time, shared_data_space->getAircraftFlightState());
    recordAircraftSystemState(simulation_time, shared_data_space->getAircraftSystemState());
    recordPilotState(simulation_time, shared_data_space->getPilotState());
    recordEnvironmentState(simulation_time, shared_data_space->getEnvironmentState());
    recordATCState(simulation_time, shared_data_space->getATCState());
    recordAircraftNetForce(simulation_time, shared_data_space->getAircraftNetForce());
    recordAircraftLogic(simulation_time, shared_data_space->getAircraftLogic());
    recordPilotLogic(simulation_time, shared_data_space->getPilotLogic());
    recordEnvironmentLogic(simulation_time, shared_data_space->getEnvironmentLogic());
    recordATCLogic(simulation_time, shared_data_space->getATCLogic());
    recordPlannedEvents(simulation_time, shared_data_space->getPlannedEventLibrary());
    recordTriggeredEvents(simulation_time, shared_data_space->getTriggeredEventLibrary());
    recordATCCommand(simulation_time, shared_data_space->getATCCommand());
    recordPlanedControllers(simulation_time, shared_data_space->getPlanedControllersLibrary());
    recordControllerExecutionStatus(simulation_time, shared_data_space->getControllerExecutionStatus());
    recordEventQueue(simulation_time, shared_data_space->getEventQueue());
}

void DataRecorder::flushAllBuffers() {
    std::lock_guard<std::mutex> lock(buffer_mutex);
    
    try {
        // 输出16个CSV文件
        std::ofstream flight_plan_file(output_directory + "/flight_plan.csv");
        flight_plan_file << std::left << std::setw(15) << "SimulationTime" << " "
                       << std::setw(15) << "datasource" << " "
                       << std::setw(15) << "ScenarioName" << " "
                       << std::setw(15) << "Description" << " "
                       << std::setw(15) << "Author" << " "
                       << std::setw(15) << "CreationDate" << " "
                       << std::setw(15) << "ScenarioType" << " "
                       << std::setw(10) << "Pilot_ID" << " "
                       << std::setw(10) << "Aircraft_ID" << " "
                       << std::setw(10) << "ATC_ID" << " "
                       << std::setw(15) << "Environment_Name" << " "
               
                       << std::setw(10) << "is_parsed" << "\n";
        for (const auto& record : flight_plan_buffer) {
            flight_plan_file << std::left << std::setw(15) << std::fixed << std::setprecision(2) << record.first << " "
                           << std::setw(15) << record.second.datasource << " "
                           << std::setw(15) << record.second.scenario_config.ScenarioName << " "
                           << std::setw(15) << record.second.scenario_config.Description << " "
                           << std::setw(15) << record.second.scenario_config.Author << " "
                           << std::setw(15) << record.second.scenario_config.CreationDate << " "
                           << std::setw(15) << record.second.scenario_config.ScenarioType << " "
                           << std::setw(10) << record.second.scenario_config.Pilot_ID << " "
                           << std::setw(10) << record.second.scenario_config.Aircraft_ID << " "
                           << std::setw(10) << record.second.scenario_config.ATC_ID << " "
                           << std::setw(15) << record.second.scenario_config.Environment_Name << " "
                   
                           << std::setw(10) << (record.second.is_parsed ? "true" : "false") << "\n";
        }
        flight_plan_file.close();

        std::ofstream aircraft_flight_file(output_directory + "/aircraft_flight_state.csv");
        aircraft_flight_file << std::right << std::setw(15) << "SimulationTime"
                           << std::setw(20) << "datasource"
                           << std::setw(15) << "latitude"
                           << std::setw(15) << "longitude"
                           << std::setw(10) << "altitude"
                           << std::setw(10) << "heading"
                           << std::setw(10) << "pitch"
                           << std::setw(10) << "roll"
                           << std::setw(15) << "airspeed"
                           << std::setw(15) << "groundspeed"
                           << std::setw(15) << "vertical_speed"
                           << std::setw(15) << "distance_m" << "\n";
        {
            const double EARTH_RADIUS_M = 6371000.0;
            auto deg2rad = [](double deg) { return deg * (3.14159265358979323846 / 180.0); };
            bool has_prev = false;
            double prev_lat_deg = 0.0;
            double prev_lon_deg = 0.0;
            double cumulative_distance_m = 0.0;

            for (const auto& record : aircraft_flight_state_buffer) {
                double lat_deg = record.second.latitude;
                double lon_deg = record.second.longitude;

                double distance_increment = 0.0;
                if (has_prev) {
                    // equirectangular approximation for small distances
                    double lat1 = deg2rad(prev_lat_deg);
                    double lat2 = deg2rad(lat_deg);
                    double dlat = lat2 - lat1;
                    double dlon = deg2rad(lon_deg - prev_lon_deg);
                    double x = dlon * std::cos((lat1 + lat2) * 0.5);
                    double y = dlat;
                    distance_increment = std::sqrt(x * x + y * y) * EARTH_RADIUS_M;
                    if (!std::isfinite(distance_increment) || distance_increment < 0) {
                        distance_increment = 0.0;
                    }
                } else {
                    has_prev = true;
                }

                cumulative_distance_m += distance_increment;
                prev_lat_deg = lat_deg;
                prev_lon_deg = lon_deg;

                aircraft_flight_file << std::right << std::setw(15) << std::fixed << std::setprecision(2) << record.first
                                   << std::setw(20) << record.second.datasource
                                   << std::setw(15) << std::fixed << std::setprecision(2) << record.second.latitude
                                   << std::setw(15) << std::fixed << std::setprecision(2) << record.second.longitude
                                   << std::setw(10) << std::fixed << std::setprecision(2) << record.second.altitude
                                   << std::setw(10) << std::fixed << std::setprecision(2) << record.second.heading
                                   << std::setw(10) << std::fixed << std::setprecision(2) << record.second.pitch
                                   << std::setw(10) << std::fixed << std::setprecision(2) << record.second.roll
                                   << std::setw(15) << std::fixed << std::setprecision(2) << record.second.airspeed
                                   << std::setw(15) << std::fixed << std::setprecision(2) << record.second.groundspeed
                                   << std::setw(15) << std::fixed << std::setprecision(2) << record.second.vertical_speed
                                   << std::setw(15) << std::fixed << std::setprecision(2) << cumulative_distance_m << "\n";
            }
        }
        aircraft_flight_file.close();

        std::ofstream aircraft_system_file(output_directory + "/aircraft_system_state.csv");
        aircraft_system_file << std::left << std::setw(15) << "SimulationTime" << " "
                           << std::setw(20) << "datasource" << " "
                           << std::setw(15) << "current_mass" << " "
                           << std::setw(15) << "current_fuel" << " "
                           << std::setw(30) << "current_center_of_gravity" << " "
                           << std::setw(30) << "current_brake_pressure" << " "
                           << std::setw(30) << "current_landing_gear_deployed" << " "
                           << std::setw(30) << "current_flaps_deployed" << " "
                           << std::setw(30) << "current_spoilers_deployed" << " "
                           << std::setw(30) << "current_throttle_position" << " "
                           << std::setw(20) << "current_engine_rpm" << " "
                           << std::setw(20) << "left_engine_failed" << " "
                           << std::setw(20) << "left_engine_rpm" << " "
                           << std::setw(20) << "right_engine_failed" << " "
                           << std::setw(20) << "right_engine_rpm" << " "
                           << std::setw(20) << "brake_efficiency" << "\n";
        for (const auto& record : aircraft_system_state_buffer) {
            aircraft_system_file << std::left << std::setw(15) << std::fixed << std::setprecision(2) << record.first << " "
                               << std::setw(20) << record.second.datasource << " "
                               << std::setw(15) << std::fixed << std::setprecision(2) << record.second.current_mass << " "
                               << std::setw(15) << std::fixed << std::setprecision(2) << record.second.current_fuel << " "
                               << std::setw(30) << std::fixed << std::setprecision(2) << record.second.current_center_of_gravity << " "
                               << std::setw(30) << std::fixed << std::setprecision(2) << record.second.current_brake_pressure << " "
                               << std::setw(30) << std::fixed << std::setprecision(2) << record.second.current_landing_gear_deployed << " "
                               << std::setw(30) << std::fixed << std::setprecision(2) << record.second.current_flaps_deployed << " "
                               << std::setw(30) << std::fixed << std::setprecision(2) << record.second.current_spoilers_deployed << " "
                               << std::setw(30) << std::fixed << std::setprecision(2) << record.second.current_throttle_position << " "
                               << std::setw(20) << std::fixed << std::setprecision(2) << record.second.current_engine_rpm << " "
                               << std::setw(20) << (record.second.left_engine_failed ? "true" : "false") << " "
                               << std::setw(20) << std::fixed << std::setprecision(2) << record.second.left_engine_rpm << " "
                               << std::setw(20) << (record.second.right_engine_failed ? "true" : "false") << " "
                               << std::setw(20) << std::fixed << std::setprecision(2) << record.second.right_engine_rpm << " "
                               << std::setw(20) << std::fixed << std::setprecision(2) << record.second.brake_efficiency << "\n";
        }
        aircraft_system_file.close();

        std::ofstream pilot_state_file(output_directory + "/pilot_state.csv");
        pilot_state_file << std::left << std::setw(15) << "SimulationTime" << " "
                       << std::setw(15) << "datasource" << " "
                       << std::setw(15) << "attention_level" << " "
                       << std::setw(15) << "skill_level" << "\n";
        for (const auto& record : pilot_state_buffer) {
            pilot_state_file << std::left << std::setw(15) << std::fixed << std::setprecision(2) << record.first << " "
                           << std::setw(15) << record.second.datasource << " "
                           << std::setw(15) << std::fixed << std::setprecision(2) << record.second.attention_level << " "
                           << std::setw(15) << std::fixed << std::setprecision(2) << record.second.skill_level << "\n";
        }
        pilot_state_file.close();

        std::ofstream environment_state_file(output_directory + "/environment_state.csv");
        environment_state_file << std::left << std::setw(15) << "SimulationTime" << " "
                             << std::setw(20) << "datasource" << " "
                             << std::setw(15) << "runway_length" << " "
                             << std::setw(15) << "runway_width" << " "
                             << std::setw(20) << "friction_coefficient" << " "
                             << std::setw(15) << "air_density" << " "
                             << std::setw(15) << "wind_speed" << " "
                             << std::setw(15) << "wind_direction" << "\n";
        for (const auto& record : environment_state_buffer) {
            environment_state_file << std::left << std::setw(15) << std::fixed << std::setprecision(2) << record.first << " "
                                   << std::setw(20) << record.second.datasource << " "
                                   << std::setw(15) << std::fixed << std::setprecision(2) << record.second.runway_length << " "
                                   << std::setw(15) << std::fixed << std::setprecision(2) << record.second.runway_width << " "
                                   << std::setw(20) << std::fixed << std::setprecision(2) << record.second.friction_coefficient << " "
                                   << std::setw(15) << std::fixed << std::setprecision(2) << record.second.air_density << " "
                                   << std::setw(15) << std::fixed << std::setprecision(2) << record.second.wind_speed << " "
                                   << std::setw(15) << std::fixed << std::setprecision(2) << record.second.wind_direction << "\n";
        }
        environment_state_file.close();

        std::ofstream atc_state_file(output_directory + "/atc_state.csv");
        atc_state_file << std::left << std::setw(15) << "SimulationTime" << " "
                      << std::setw(20) << "datasource" << " "
                      << std::setw(20) << "controller_workload" << " "
                      << std::setw(20) << "controller_attention" << " "
                      << std::setw(20) << "active_aircraft_count" << " "
                      << std::setw(20) << "pending_commands" << " "
                      << std::setw(20) << "airspace_congestion" << " "
                      << std::setw(15) << "conflict_count" << " "
                      << std::setw(20) << "separation_violations" << " "
                      << std::setw(20) << "communication_load" << " "
                      << std::setw(20) << "active_frequencies" << " "
                      << std::setw(15) << "response_time" << " "
                      << std::setw(20) << "radar_operational" << " "
                      << std::setw(25) << "communication_system_operational" << " "
                      << std::setw(15) << "current_phase" << "\n";
        for (const auto& record : atc_state_buffer) {
            atc_state_file << std::left << std::setw(15) << std::fixed << std::setprecision(2) << record.first << " "
                          << std::setw(20) << record.second.datasource << " "
                          << std::setw(20) << std::fixed << std::setprecision(2) << record.second.controller_workload << " "
                          << std::setw(20) << std::fixed << std::setprecision(2) << record.second.controller_attention << " "
                          << std::setw(20) << std::fixed << std::setprecision(2) << record.second.active_aircraft_count << " "
                          << std::setw(20) << std::fixed << std::setprecision(2) << record.second.pending_commands << " "
                          << std::setw(20) << std::fixed << std::setprecision(2) << record.second.airspace_congestion << " "
                          << std::setw(15) << std::fixed << std::setprecision(2) << record.second.conflict_count << " "
                          << std::setw(20) << std::fixed << std::setprecision(2) << record.second.separation_violations << " "
                          << std::setw(20) << std::fixed << std::setprecision(2) << record.second.communication_load << " "
                          << std::setw(20) << std::fixed << std::setprecision(2) << record.second.active_frequencies << " "
                          << std::setw(15) << std::fixed << std::setprecision(2) << record.second.response_time << " "
                          << std::setw(20) << (record.second.radar_operational ? "true" : "false") << " "
                          << std::setw(25) << (record.second.communication_system_operational ? "true" : "false") << " "
                          << std::setw(15) << record.second.current_phase << "\n";
        }
        atc_state_file.close();

        // 输出六分量合外力数据
        std::ofstream aircraft_net_force_file(output_directory + "/aircraft_net_force.csv");
        aircraft_net_force_file << std::left << std::setw(15) << "SimulationTime" << " "
                               << std::setw(20) << "datasource" << " "
                               << std::setw(20) << "longitudinal_force" << " "
                               << std::setw(15) << "lateral_force" << " "
                               << std::setw(15) << "vertical_force" << " "
                               << std::setw(15) << "roll_moment" << " "
                               << std::setw(15) << "pitch_moment" << " "
                               << std::setw(15) << "yaw_moment" << " "
                               << std::setw(15) << "thrust_force" << " "
                               << std::setw(15) << "drag_force" << " "
                               << std::setw(15) << "lift_force" << " "
                               << std::setw(15) << "weight_force" << " "
                               << std::setw(15) << "side_force" << "\n";
        for (const auto& record : aircraft_net_force_buffer) {
            aircraft_net_force_file << std::left << std::setw(15) << std::fixed << std::setprecision(2) << record.first << " "
                                   << std::setw(20) << record.second.datasource << " "
                                   << std::setw(20) << std::fixed << std::setprecision(2) << record.second.longitudinal_force << " "
                                   << std::setw(15) << std::fixed << std::setprecision(2) << record.second.lateral_force << " "
                                   << std::setw(15) << std::fixed << std::setprecision(2) << record.second.vertical_force << " "
                                   << std::setw(15) << std::fixed << std::setprecision(2) << record.second.roll_moment << " "
                                   << std::setw(15) << std::fixed << std::setprecision(2) << record.second.pitch_moment << " "
                                   << std::setw(15) << std::fixed << std::setprecision(2) << record.second.yaw_moment << " "
                                   << std::setw(15) << std::fixed << std::setprecision(2) << record.second.thrust_force << " "
                                   << std::setw(15) << std::fixed << std::setprecision(2) << record.second.drag_force << " "
                                   << std::setw(15) << std::fixed << std::setprecision(2) << record.second.lift_force << " "
                                   << std::setw(15) << std::fixed << std::setprecision(2) << record.second.weight_force << " "
                                   << std::setw(15) << std::fixed << std::setprecision(2) << record.second.side_force << "\n";
        }
        aircraft_net_force_file.close();

        std::ofstream aircraft_logic_file(output_directory + "/aircraft_logic.csv");
        aircraft_logic_file << std::left << std::setw(15) << "SimulationTime" << " "
                          << std::setw(20) << "datasource" << " "
                          << std::setw(15) << "flight_plan_id" << " "
                          << std::setw(20) << "departure_airport" << " "
                          << std::setw(20) << "arrival_airport" << " "
                          << std::setw(20) << "planned_altitude" << " "
                          << std::setw(15) << "planned_speed" << " "
                          << std::setw(15) << "current_phase" << " "
                          << std::setw(15) << "next_phase" << " "
                          << std::setw(15) << "phase_progress" << " "
                          << std::setw(20) << "autopilot_engaged" << " "
                          << std::setw(15) << "autopilot_mode" << " "
                          << std::setw(20) << "auto_throttle_engaged" << " "
                          << std::setw(15) << "navigation_mode" << " "
                          << std::setw(15) << "performance_index" << " "
                          << std::setw(15) << "fuel_efficiency" << " "
                          << std::setw(15) << "optimal_speed" << " "
                          << std::setw(15) << "optimal_altitude" << "\n";
        for (const auto& record : aircraft_logic_buffer) {
            aircraft_logic_file << std::left << std::setw(15) << std::fixed << std::setprecision(2) << record.first << " "
                              << std::setw(20) << record.second.datasource << " "
                              << std::setw(15) << record.second.flight_plan_id << " "
                              << std::setw(20) << record.second.departure_airport << " "
                              << std::setw(20) << record.second.arrival_airport << " "
                              << std::setw(20) << std::fixed << std::setprecision(2) << record.second.planned_altitude << " "
                              << std::setw(15) << std::fixed << std::setprecision(2) << record.second.planned_speed << " "
                              << std::setw(15) << record.second.current_phase << " "
                              << std::setw(15) << record.second.next_phase << " "
                              << std::setw(15) << std::fixed << std::setprecision(2) << record.second.phase_progress << " "
                              << std::setw(20) << (record.second.autopilot_engaged ? "true" : "false") << " "
                              << std::setw(15) << record.second.autopilot_mode << " "
                              << std::setw(20) << (record.second.auto_throttle_engaged ? "true" : "false") << " "
                              << std::setw(15) << record.second.navigation_mode << " "
                              << std::setw(15) << std::fixed << std::setprecision(2) << record.second.performance_index << " "
                              << std::setw(15) << std::fixed << std::setprecision(2) << record.second.fuel_efficiency << " "
                              << std::setw(15) << std::fixed << std::setprecision(2) << record.second.optimal_speed << " "
                              << std::setw(15) << std::fixed << std::setprecision(2) << record.second.optimal_altitude << "\n";
        }
        aircraft_logic_file.close();

        std::ofstream pilot_logic_file(output_directory + "/pilot_logic.csv");
        pilot_logic_file << std::left << std::setw(15) << "SimulationTime" << " "
                       << std::setw(20) << "datasource" << " "
                       << std::setw(20) << "decision_strategy" << " "
                       << std::setw(15) << "risk_tolerance" << " "
                       << std::setw(15) << "priority_task" << " "
                       << std::setw(15) << "attention_focus" << " "
                       << std::setw(15) << "mental_model" << " "
                       << std::setw(20) << "situation_awareness" << " "
                       << std::setw(15) << "behavior_pattern" << " "
                       << std::setw(15) << "adaptability" << " "
                       << std::setw(20) << "communication_style" << " "
                       << std::setw(15) << "learning_rate" << " "
                       << std::setw(20) << "performance_improvement" << "\n";
        for (const auto& record : pilot_logic_buffer) {
            pilot_logic_file << std::left << std::setw(15) << std::fixed << std::setprecision(2) << record.first << " "
                           << std::setw(20) << record.second.datasource << " "
                           << std::setw(20) << record.second.decision_strategy << " "
                           << std::setw(15) << std::fixed << std::setprecision(2) << record.second.risk_tolerance << " "
                           << std::setw(15) << record.second.priority_task << " "
                           << std::setw(15) << record.second.attention_focus << " "
                           << std::setw(15) << record.second.mental_model << " "
                           << std::setw(20) << std::fixed << std::setprecision(2) << record.second.situation_awareness << " "
                           << std::setw(15) << record.second.behavior_pattern << " "
                           << std::setw(15) << std::fixed << std::setprecision(2) << record.second.adaptability << " "
                           << std::setw(20) << record.second.communication_style << " "
                           << std::setw(15) << std::fixed << std::setprecision(2) << record.second.learning_rate << " "
                           << std::setw(20) << std::fixed << std::setprecision(2) << record.second.performance_improvement << "\n";
        }
        pilot_logic_file.close();

        std::ofstream environment_logic_file(output_directory + "/environment_logic.csv");
        environment_logic_file << std::left << std::setw(15) << "SimulationTime" << " "
                             << std::setw(20) << "datasource" << " "
                             << std::setw(15) << "weather_pattern" << " "
                             << std::setw(15) << "weather_severity" << " "
                             << std::setw(15) << "weather_trend" << " "
                             << std::setw(20) << "terrain_complexity" << " "
                             << std::setw(15) << "terrain_risk_level" << " "
                             << std::setw(15) << "airspace_class" << " "
                             << std::setw(20) << "airspace_restrictions" << " "
                             << std::setw(15) << "time_of_day" << " "
                             << std::setw(15) << "season" << " "
                             << std::setw(20) << "daylight_availability" << "\n";
        for (const auto& record : environment_logic_buffer) {
            environment_logic_file << std::left << std::setw(15) << std::fixed << std::setprecision(2) << record.first << " "
                                 << std::setw(20) << record.second.datasource << " "
                                 << std::setw(15) << record.second.weather_pattern << " "
                                 << std::setw(15) << std::fixed << std::setprecision(2) << record.second.weather_severity << " "
                                 << std::setw(15) << record.second.weather_trend << " "
                                 << std::setw(20) << std::fixed << std::setprecision(2) << record.second.terrain_complexity << " "
                                 << std::setw(15) << std::fixed << std::setprecision(2) << record.second.terrain_risk_level << " "
                                 << std::setw(15) << record.second.airspace_class << " "
                                 << std::setw(20) << record.second.airspace_restrictions << " "
                                 << std::setw(15) << record.second.time_of_day << " "
                                 << std::setw(15) << record.second.season << " "
                                 << std::setw(20) << record.second.daylight_availability << "\n";
        }
        environment_logic_file.close();

        std::ofstream atc_logic_file(output_directory + "/atc_logic.csv");
        atc_logic_file << std::left << std::setw(15) << "SimulationTime" << " "
                      << std::setw(20) << "datasource" << " "
                      << std::setw(20) << "control_strategy" << " "
                      << std::setw(20) << "separation_standards" << " "
                      << std::setw(25) << "traffic_flow_management" << " "
                      << std::setw(25) << "conflict_resolution_strategy" << " "
                      << std::setw(25) << "conflict_detection_threshold" << " "
                      << std::setw(20) << "communication_protocol" << " "
                      << std::setw(20) << "communication_priority" << " "
                      << std::setw(15) << "system_mode" << " "
                      << std::setw(15) << "automation_level" << "\n";
        for (const auto& record : atc_logic_buffer) {
            atc_logic_file << std::left << std::setw(15) << std::fixed << std::setprecision(2) << record.first << " "
                          << std::setw(20) << record.second.datasource << " "
                          << std::setw(20) << record.second.control_strategy << " "
                          << std::setw(20) << record.second.separation_standards << " "
                          << std::setw(25) << record.second.traffic_flow_management << " "
                          << std::setw(25) << record.second.conflict_resolution_strategy << " "
                          << std::setw(25) << std::fixed << std::setprecision(2) << record.second.conflict_detection_threshold << " "
                          << std::setw(20) << record.second.communication_protocol << " "
                          << std::setw(20) << record.second.communication_priority << " "
                          << std::setw(15) << record.second.system_mode << " "
                          << std::setw(15) << std::fixed << std::setprecision(2) << record.second.automation_level << "\n";
        }
        atc_logic_file.close();

        std::ofstream planned_event_file(output_directory + "/planned_events.csv");
        planned_event_file << std::left 
                        << std::setw(20) << "datasource" << " "
                         << std::setw(20) << "event_id" << " "
                         << std::setw(35) << "event_name" << " "
                         << std::setw(50) << "description" << " "
                         << std::setw(20) << "source_agent" << " "
                         << std::setw(20) << "is_triggered" << "\n";
        // 计划事件库是静态的，只需要输出一次（取第一条记录）
        if (!planned_event_buffer.empty()) {
            auto events = planned_event_buffer.begin()->second.getPlannedEvents();
            for (const auto& event : events) {
                planned_event_file << std::left << std::setw(20) << planned_event_buffer.begin()->second.datasource << " "
                                 << std::setw(20) << event.event_id << " "
                                 << std::setw(35) << event.event_name << " "
                                 << std::setw(50) << event.description << " "
                                 << std::setw(20) << event.source_agent << " "
                                 << std::setw(20) << (event.is_triggered ? "true" : "false") << "\n";
            }
        }
        planned_event_file.close();

        std::ofstream triggered_event_file(output_directory + "/triggered_events.csv");
        triggered_event_file << std::left << std::setw(15) << "SimulationTime" << " "
                           << std::setw(15) << "StepNumber" << " "
                           << std::setw(15) << "EventCount" << " "
                           << std::setw(200) << "EventList" << "\n";
        
        // 添加强制调试日志
        VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, 
            "DataRecorder: 开始处理triggered_events.csv, triggered_event_buffer大小: " + 
            std::to_string(triggered_event_buffer.size()));
        
        // 获取所有时间步的事件映射
        std::map<double, std::vector<VFT_SMF::GlobalSharedDataStruct::StandardEvent>> all_step_events;
        if (!triggered_event_buffer.empty()) {
            // 使用最后一条记录的事件库来获取所有时间步的事件
            all_step_events = triggered_event_buffer.back().second.getStepEventsMap();
            
            // 添加调试日志
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, 
                "DataRecorder获取到的step_events_map大小: " + std::to_string(all_step_events.size()) + 
                ", triggered_event_buffer大小: " + std::to_string(triggered_event_buffer.size()));
            
            for (const auto& [time, events] : all_step_events) {
                VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, 
                    "时间步 " + std::to_string(time) + "s 有 " + std::to_string(events.size()) + " 个事件");
            }
        } else {
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "triggered_event_buffer为空！");
        }
        
        // 为每个时间步输出事件数据
        // 计算需要输出的总步数：根据仿真时间和时间步长
        uint64_t total_steps = 0;
        if (!triggered_event_buffer.empty()) {
            // 从最后一条记录的时间计算总步数
            double max_time = triggered_event_buffer.back().first;
            total_steps = static_cast<uint64_t>(max_time / 0.01) + 1;  // 向上取整
        } else {
            // 如果没有记录，使用默认值
            total_steps = 1000;
        }
        
        for (uint64_t step = 0; step <= total_steps; step++) {
            double time = static_cast<double>(step) * 0.01;  // 使用与事件监测线程相同的时间计算方法
            uint64_t step_number = step + 1;
            
            // 使用精确的时间匹配，避免浮点精度问题
            auto it = all_step_events.find(time);
            size_t event_count = 0;
            std::string event_list = "[]";
            
            if (it != all_step_events.end()) {
                event_count = it->second.size();
                event_list = triggered_event_buffer.back().second.generateEventListString(time);
            } else {
                // 不使用容差匹配，严格按精确时间输出，避免重复
            }
            
            triggered_event_file << std::left << std::setw(15) << std::fixed << std::setprecision(2) << time << " "
                               << std::setw(15) << step_number << " "
                               << std::setw(15) << event_count << " "
                               << std::setw(200) << event_list << "\n";
        }
        triggered_event_file.close();

        std::ofstream atc_command_file(output_directory + "/atc_command.csv");
        atc_command_file << std::left << std::setw(15) << "SimulationTime" << " "
                       << std::setw(25) << "datasource" << " "
                       << std::setw(20) << "clearance_granted" << " "
                       << std::setw(20) << "emergency_brake" << "\n";
        for (const auto& record : atc_command_buffer) {
            atc_command_file << std::left << std::setw(15) << std::fixed << std::setprecision(2) << record.first << " "
                           << std::setw(25) << record.second.datasource << " "
                           << std::setw(20) << (record.second.clearance_granted ? "true" : "false") << " "
                           << std::setw(20) << (record.second.emergency_brake ? "true" : "false") << "\n";
        }
        atc_command_file.close();

        // 输出计划控制器数据
        std::ofstream planed_controllers_file(output_directory + "/planed_controllers.csv");
        planed_controllers_file << std::left << std::setw(15) << "SimulationTime" << " "
                              << std::setw(20) << "datasource" << " "
                              << std::setw(40) << "controller_name" << " "
                              << std::setw(40) << "TriggerEvent_name" << " "
                              << std::setw(30) << "controller_type" << " "
                              << std::setw(50) << "description" << " "
                              << std::setw(25) << "termination_condition" << "\n";
        
        // 计划控制器库是静态的，只需要输出一次（取第一条记录）
        if (!planed_controllers_buffer.empty()) {
            const auto& controllers = planed_controllers_buffer.begin()->second.getAllControllers();
            for (const auto& controller : controllers) {
                planed_controllers_file << std::left << std::setw(15) << std::fixed << std::setprecision(2) << planed_controllers_buffer.begin()->first << " "
                                      << std::setw(20) << planed_controllers_buffer.begin()->second.datasource << " "
                                      << std::setw(40) << controller.controller_name << " "
                                      << std::setw(40) << controller.event_name << " "
                                      << std::setw(30) << controller.controller_type << " "
                                      << std::setw(50) << controller.description << " "
                                      << std::setw(25) << controller.termination_condition << "\n";
            }
        }
        planed_controllers_file.close();

        // 输出控制器执行状态跟踪数据
        std::ofstream controller_execution_status_file(output_directory + "/controller_execution_status.csv");
        
        // 获取所有控制器名称（从计划控制器库中）
        std::vector<std::string> all_controller_names;
        if (!planed_controllers_buffer.empty()) {
            const auto& controllers = planed_controllers_buffer.begin()->second.getAllControllers();
            for (const auto& controller : controllers) {
                all_controller_names.push_back(controller.controller_name);
            }
        }
        
        // 写入表头 - 使用固定宽度，左对齐
        controller_execution_status_file << std::left << std::setw(15) << "SimulationTime";
        for (const auto& controller_name : all_controller_names) {
            controller_execution_status_file << " " << std::setw(25) << controller_name;
        }
        controller_execution_status_file << "\n";
        
        // 写入数据 - 使用固定宽度，左对齐
        for (const auto& record : controller_execution_status_buffer) {
            controller_execution_status_file << std::left << std::setw(15) << std::fixed << std::setprecision(2) << record.first;
            for (const auto& controller_name : all_controller_names) {
                bool is_running = record.second.getControllerStatus(controller_name);
                controller_execution_status_file << " " << std::setw(25) << (is_running ? "1" : "0");
            }
            controller_execution_status_file << "\n";
        }
        controller_execution_status_file.close();

        // 输出事件队列event_queue.csv
        {
            std::ofstream event_queue_file(output_directory + "/event_queue.csv");
            event_queue_file << std::left << std::setw(15) << "SimulationTime" << " "
                              << std::setw(20) << "datasource" << " "
                              << std::setw(15) << "queue_size" << " "
                              << std::setw(15) << "processed_count" << " "
                              << std::setw(50) << "pending_events" << "\n";

            if (!event_queue_buffer.empty()) {
                const auto& [time, queue] = event_queue_buffer.back();
                // 展平待处理事件用于可读性输出（仅输出前N个以避免超长）
                const size_t max_list = 10;
                auto pending_list = queue.getPendingEvents();
                std::stringstream ss;
                ss << "[";
                for (size_t i = 0; i < pending_list.size() && i < max_list; ++i) {
                    if (i > 0) ss << ",";
                    ss << pending_list[i].event.event_name << "@" << std::fixed << std::setprecision(2) << pending_list[i].trigger_time;
                }
                if (pending_list.size() > max_list) {
                    ss << ",...";
                }
                ss << "]";

                event_queue_file << std::left << std::setw(15) << std::fixed << std::setprecision(2) << time << " "
                                  << std::setw(20) << queue.datasource << " "
                                  << std::setw(15) << queue.getQueueSize() << " "
                                  << std::setw(15) << queue.getProcessedCount() << " "
                                  << std::setw(50) << ss.str() << "\n";
            }

            event_queue_file.close();
        }

        VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "数据记录器已将所有17个数据模块输出到文件，输出目录: " + output_directory);
        
    } catch (const std::exception& e) {
        VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "数据记录器输出文件失败: " + std::string(e.what()));
    }
}

void DataRecorder::clearAllBuffers() {
    std::lock_guard<std::mutex> lock(buffer_mutex);
    
    flight_plan_buffer.clear();
    aircraft_flight_state_buffer.clear();
    aircraft_system_state_buffer.clear();
    pilot_state_buffer.clear();
    environment_state_buffer.clear();
    atc_state_buffer.clear();
    aircraft_logic_buffer.clear();
    pilot_logic_buffer.clear();
    environment_logic_buffer.clear();
    atc_logic_buffer.clear();
    planned_event_buffer.clear();
    triggered_event_buffer.clear();
    atc_command_buffer.clear();
    planed_controllers_buffer.clear();
    event_queue_buffer.clear();
    
    VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "数据记录器缓冲区已清空");
}

void DataRecorder::clearOutputFiles() {
    try {
        // 清理CSV文件
        std::vector<std::string> csv_files = {
            "flight_plan.csv",
            "aircraft_flight_state.csv",
            "aircraft_system_state.csv",
            "pilot_state.csv",
            "environment_state.csv",
            "atc_state.csv",
            "aircraft_net_force.csv",
            "aircraft_logic.csv",
            "pilot_logic.csv",
            "environment_logic.csv",
            "atc_logic.csv",
            "planned_events.csv",
            "triggered_events.csv",
            "atc_command.csv",
            "planed_controllers.csv",
            "controller_execution_status.csv",
            "event_queue.csv"
        };
        
        for (const auto& file : csv_files) {
            std::string file_path = output_directory + "/" + file;
            if (std::filesystem::exists(file_path)) {
                std::filesystem::remove(file_path);
            }
        }
        
        // 清理日志文件
        std::vector<std::string> log_files = {
            "Brief.log",
            "Detail.log"
        };
        
        for (const auto& file : log_files) {
            std::string file_path = output_directory + "/" + file;
            if (std::filesystem::exists(file_path)) {
                std::filesystem::remove(file_path);
            }
        }
        
        VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "已清理输出目录中的旧文件: " + output_directory);
    } catch (const std::exception& e) {
        VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "清理输出文件时出错: " + std::string(e.what()));
    }
}

} // namespace VFT_SMF
