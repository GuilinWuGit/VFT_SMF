/**
 * @file ATCAgent.cpp
 * @brief ATC代理实现 - 重构版本
 * @author VFT_SMF Development Team
 * @date 2024
 */

#include "ATCAgent.hpp"
#include "../../G_SimulationManager/LogAndData/Logger.hpp"
#include "IATCStrategy.hpp"
#include "../ATC_001/ATC_001_Strategy.hpp"
#include "../ATC_002/ATC_002_Strategy.hpp"
#include <iostream>
#include <sstream>

namespace VFT_SMF {

    ATCAgent::ATCAgent(const std::string& id, const std::string& name)
        : atc_facility_id(id),
          atc_facility_name(name),
          current_simulation_time(0.0),
          total_instructions_issued(0),
          total_instructions_acknowledged(0),
          total_instructions_executed(0) {
        agent_id = id;
        agent_name = name;
        is_running = false;
        current_state = AgentState::UNINITIALIZED;
        
        VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "ATC代理创建完成: " + name);
    }

    void ATCAgent::initialize() {
        std::cout << "ATC代理初始化: " << get_agent_name() << std::endl;
        current_state = AgentState::INITIALIZING;
        
        // 解析逻辑线并生成指令
        parse_logic_lines_and_generate_instructions();
        
        current_state = AgentState::READY;
        VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "ATC代理初始化完成: " + get_agent_name());
    }

    void ATCAgent::start() {
        std::cout << "ATC代理启动: " << get_agent_name() << std::endl;
        is_running = true;
        current_state = AgentState::RUNNING;
        VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "ATC代理启动: " + get_agent_name());
    }

    void ATCAgent::pause() {
        std::cout << "ATC代理暂停: " << get_agent_name() << std::endl;
        is_running = false;
        current_state = AgentState::PAUSED;
        VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "ATC代理暂停: " + get_agent_name());
    }

    void ATCAgent::resume() {
        std::cout << "ATC代理恢复: " << get_agent_name() << std::endl;
        is_running = true;
        current_state = AgentState::RUNNING;
        VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "ATC代理恢复: " + get_agent_name());
    }

    void ATCAgent::stop() {
        std::cout << "ATC代理停止: " << get_agent_name() << std::endl;
        is_running = false;
        current_state = AgentState::STOPPED;
        VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "ATC代理停止: " + get_agent_name());
    }

    void ATCAgent::update(double delta_time) {
        if (!is_running) return;
        
        // 更新当前仿真时间
        current_simulation_time += delta_time;
        
        // 处理代理事件队列中的事件（新增）
        processAgentEventQueue(current_simulation_time);
        
        // 检查事件触发条件并发出指令
        check_event_triggers_and_issue_instructions(current_simulation_time);
    }

    void ATCAgent::handle_event(const Event& event) {
        std::cout << "ATC代理处理事件: " << event.id << std::endl;
        VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "ATC代理处理事件: " + event.id);
        
        // 根据事件类型处理
        if (event.type == EventType::ATC_EVENT) {
            // 处理ATC事件
            check_event_triggers_and_issue_instructions(current_simulation_time);
        }
    }

    void ATCAgent::send_event(const Event& event) {
        std::cout << "ATC代理发送事件: " << event.id << std::endl;
        VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "ATC代理发送事件: " + event.id);
    }

    std::string ATCAgent::get_status() const {
        return "ATC代理 [" + get_agent_id() + "] - " + get_agent_name() + 
               " - 状态: " + (is_running ? "运行中" : "已停止") +
               " - 已发出指令: " + std::to_string(total_instructions_issued);
    }

    bool ATCAgent::is_ready() const {
        return current_state == AgentState::READY || current_state == AgentState::RUNNING;
    }

    void ATCAgent::set_shared_data_space(std::shared_ptr<GlobalShared_DataSpace::GlobalSharedDataSpace> data_space) {
        shared_data_space = data_space;
        VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "ATC代理设置全局共享数据空间");
    }

    void ATCAgent::setATCStrategy(std::unique_ptr<IATCStrategy> strategy) {
        atc_strategy = std::move(strategy);
        if (atc_strategy) {
            atc_strategy->initialize(shared_data_space, get_agent_id());
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "ATC代理设置策略成功: " + atc_strategy->getStrategyId());
        } else {
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "ATC代理设置策略失败");
        }
    }

    void ATCAgent::initializeATCStrategy(const std::string& atc_id) {
        VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "ATC代理初始化策略: " + atc_id);
        
        if (atc_id == "ATC_001") {
            setATCStrategy(std::make_unique<ATC_001_Strategy>());
        } else if (atc_id == "ATC_002") {
            setATCStrategy(std::make_unique<ATC_002_Strategy>());
        } else {
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "未知的ATC_ID: " + atc_id + "，使用默认策略ATC_001");
            setATCStrategy(std::make_unique<ATC_001_Strategy>());
        }
    }

    std::map<std::string, std::string> ATCAgent::getStrategyConfig() const {
        if (atc_strategy) {
            return atc_strategy->getStrategyConfig();
        }
        return std::map<std::string, std::string>();
    }

    void ATCAgent::set_flight_plan_data(const VFT_SMF::GlobalSharedDataStruct::FlightPlanData& plan_data) {
        flight_plan_data = plan_data;
        VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "ATC代理设置飞行计划数据");
    }

    void ATCAgent::parse_logic_lines_and_generate_instructions() {
        logic_line_results.clear();
        
        if (flight_plan_data.logic_lines.empty()) {
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "ATC代理: 飞行计划中没有逻辑线数据");
            return;
        }
        
        // 解析逻辑线数据
        for (const auto& logic_line : flight_plan_data.logic_lines) {
            std::string line_id = logic_line.first;
            std::string line_content = logic_line.second;
            
            // 解析指令类型
            ATCInstructionType instruction_type = parse_instruction_type_from_logic_line(line_content);
            
            // 生成指令内容
            std::string instruction_content = generate_instruction_content(
                LogicLineResult(line_id, "", instruction_type, line_content)
            );
            
            // 创建逻辑线结果
            LogicLineResult logic_result(line_id, "", instruction_type, instruction_content);
            logic_line_results.push_back(logic_result);
            
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, 
                "ATC代理解析逻辑线: " + line_id + " -> " + instruction_content);
        }
        
        VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, 
            "ATC代理完成逻辑线解析，共解析 " + std::to_string(logic_line_results.size()) + " 条逻辑线");
    }

    void ATCAgent::check_event_triggers_and_issue_instructions(double current_time) {
        if (!shared_data_space) {
                VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "ATC代理: 全局共享数据空间未设置");
            return;
        }
        
        // 检查每个逻辑线是否应该触发
        for (auto& logic_result : logic_line_results) {
            if (!logic_result.is_triggered && should_issue_instruction(logic_result, current_time)) {
                // 创建ATC指令
                ATCInstruction instruction(
                    logic_result.instruction_type,
                    flight_plan_data.scenario_config.Aircraft_ID,
                    flight_plan_data.scenario_config.Pilot_ID,
                    logic_result.instruction_content
                );
                
                // 发出指令
                issue_atc_instruction(instruction);
                
                // 标记为已触发
                logic_result.is_triggered = true;
                logic_result.trigger_time = current_time;
                
                VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, 
                    "ATC代理发出指令: " + instruction.instruction_content + 
                    " 时间: " + std::to_string(current_time));
            }
        }
    }

    void ATCAgent::issue_atc_instruction(const ATCInstruction& instruction) {
        if (!shared_data_space) {
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "ATC代理: 全局共享数据空间未设置，无法发出指令");
            return;
        }
        
        // 写入ATC指令到全局共享数据空间
        write_atc_command_to_shared_space(instruction);
        
        // 记录已发出的指令
        issued_instructions.push_back(instruction);
        total_instructions_issued++;
        
        VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, 
            "ATC代理指令已写入全局共享数据空间: " + instruction.instruction_content);
    }

    void ATCAgent::update_instruction_status(const std::string& instruction_id, bool acknowledged, bool executed) {
        for (auto& instruction : issued_instructions) {
            if (instruction.instruction_id == instruction_id) {
                if (acknowledged && !instruction.is_acknowledged) {
                    instruction.is_acknowledged = true;
                    total_instructions_acknowledged++;
                }
                if (executed && !instruction.is_executed) {
                    instruction.is_executed = true;
                    total_instructions_executed++;
                }
                break;
            }
        }
    }

    VFT_SMF::GlobalSharedDataStruct::ATC_Command ATCAgent::get_current_atc_command() const {
        if (!shared_data_space) {
            return VFT_SMF::GlobalSharedDataStruct::ATC_Command();
        }
        return shared_data_space->getATCCommand();
    }

    std::string ATCAgent::get_performance_summary() const {
        std::stringstream ss;
        ss << "ATC代理性能统计 [" << atc_facility_name << "]:\n";
        ss << "  总发出指令数: " << total_instructions_issued << "\n";
        ss << "  总确认指令数: " << total_instructions_acknowledged << "\n";
        ss << "  总执行指令数: " << total_instructions_executed << "\n";
        ss << "  逻辑线解析数: " << logic_line_results.size() << "\n";
        ss << "  当前仿真时间: " << std::fixed << std::setprecision(2) << current_simulation_time << "s";
        return ss.str();
    }

    // 私有辅助方法实现
    ATCInstructionType ATCAgent::parse_instruction_type_from_logic_line(const std::string& logic_line_content) {
        // 根据逻辑线内容解析指令类型
        if (logic_line_content.find("clearance") != std::string::npos || 
            logic_line_content.find("放行") != std::string::npos) {
            return ATCInstructionType::CLEARANCE_GRANTED;
        } else if (logic_line_content.find("emergency") != std::string::npos || 
                   logic_line_content.find("紧急") != std::string::npos) {
            return ATCInstructionType::EMERGENCY_BRAKE;
        } else if (logic_line_content.find("taxi") != std::string::npos || 
                   logic_line_content.find("滑行") != std::string::npos) {
            return ATCInstructionType::TAXI_CLEARANCE;
        } else if (logic_line_content.find("takeoff") != std::string::npos || 
                   logic_line_content.find("起飞") != std::string::npos) {
            return ATCInstructionType::TAKEOFF_CLEARANCE;
        } else if (logic_line_content.find("landing") != std::string::npos || 
                   logic_line_content.find("着陆") != std::string::npos) {
            return ATCInstructionType::LANDING_CLEARANCE;
        } else {
            // 默认为放行许可
            return ATCInstructionType::CLEARANCE_GRANTED;
        }
    }

    std::string ATCAgent::generate_instruction_content(const LogicLineResult& logic_result) {
        std::stringstream ss;
        ss << "ATC指令 [" << logic_result.line_id << "]: ";
        
        switch (logic_result.instruction_type) {
            case ATCInstructionType::CLEARANCE_GRANTED:
                ss << "放行许可已批准";
                break;
            case ATCInstructionType::EMERGENCY_BRAKE:
                ss << "紧急刹车指令";
                break;
            case ATCInstructionType::TAXI_CLEARANCE:
                ss << "滑行许可已批准";
                break;
            case ATCInstructionType::TAKEOFF_CLEARANCE:
                ss << "起飞许可已批准";
                break;
            case ATCInstructionType::LANDING_CLEARANCE:
                ss << "着陆许可已批准";
                break;
            default:
                ss << "标准ATC指令";
                break;
        }
        
        return ss.str();
    }

    bool ATCAgent::should_issue_instruction(const LogicLineResult& logic_result, double current_time) {
        // 简单的触发条件：基于时间或事件状态
        // 这里可以根据具体的业务逻辑进行扩展
        
        // 示例：在特定时间点触发
        if (current_time >= 5.0 && logic_result.line_id.find("taxi") != std::string::npos) {
            return true;
        }
        
        // 示例：在特定时间点触发放行许可
        if (current_time >= 0.0 && logic_result.line_id.find("clearance") != std::string::npos) {
            return true;
        }
        
        return false;
    }

    void ATCAgent::write_atc_command_to_shared_space(const ATCInstruction& instruction) {
        if (!shared_data_space) {
            return;
        }
        
        // 创建ATC_Command对象
        VFT_SMF::GlobalSharedDataStruct::ATC_Command atc_command;
        
        // 根据指令类型设置相应的标志
        switch (instruction.type) {
            case ATCInstructionType::CLEARANCE_GRANTED:
                atc_command.clearance_granted = true;
                atc_command.emergency_brake = false;
                break;
            case ATCInstructionType::EMERGENCY_BRAKE:
                atc_command.clearance_granted = false;
                atc_command.emergency_brake = true;
                break;
            default:
                atc_command.clearance_granted = false;
                atc_command.emergency_brake = false;
                break;
        }
        
        // 设置时间戳
        atc_command.timestamp = VFT_SMF::SimulationTimePoint{current_simulation_time};
        
        // 写入到全局共享数据空间
        shared_data_space->setATCCommand(atc_command);
    }

    // ==================== 统一控制器接口实现 ====================

                 bool ATCAgent::executeController(const std::string& controller_name, 
                                      const std::map<std::string, std::string>& params,
                                      double current_time) {
        VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, 
            "ATC代理执行控制器: " + controller_name + " (时间: " + std::to_string(current_time) + "s)");
        
        bool executed = false;
        
        // 优先使用ATC策略
        if (atc_strategy) {
            if (controller_name == "clearance_controller") {
                executed = atc_strategy->executeClearanceController(params, current_time);
            } else if (controller_name == "Emergency_Brake_Command") {
                executed = atc_strategy->executeEmergencyBrakeController(params, current_time);
            } else if (controller_name == "issue_takeoff_clearance") {
                executed = atc_strategy->executeTakeoffClearanceController(params, current_time);
            } else if (controller_name == "issue_landing_clearance") {
                executed = atc_strategy->executeLandingClearanceController(params, current_time);
            }
            
            if (executed) {
                VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, 
                    "ATC代理: 使用策略 " + atc_strategy->getStrategyId() + " 执行控制器: " + controller_name);
            }
        }
        
        // 如果没有策略或策略不支持，使用默认实现
        if (!executed) {
            if (controller_name == "clearance_controller") {
                executed = executeClearanceController(params, current_time);
            } else if (controller_name == "Emergency_Brake_Command") {
                executed = executeEmergencyBrakeController(params, current_time);
            } else if (controller_name == "issue_takeoff_clearance") {
                executed = executeTakeoffClearanceController(params, current_time);
            } else if (controller_name == "issue_landing_clearance") {
                executed = executeLandingClearanceController(params, current_time);
            } else {
                VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, 
                    "ATC代理: 未知的控制器名称: " + controller_name);
            }
            
            if (executed) {
                VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, 
                    "ATC代理: 使用默认实现执行控制器: " + controller_name);
            }
        }
        
        if (executed) {
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, 
                "ATC代理控制器执行成功: " + controller_name);
        } else {
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, 
                "ATC代理控制器执行失败: " + controller_name);
        }
        
        return executed;
    }

    int ATCAgent::processAgentEventQueue(double current_time) {
        if (!shared_data_space) {
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "ATC代理: 全局共享数据空间未设置");
            return 0;
        }
        
        int processed_count = 0;
        VFT_SMF::GlobalSharedDataStruct::AgentEventQueueItem queue_item;
        
        // 处理代理事件队列中的所有事件
        while (shared_data_space->dequeueAgentEvent(get_agent_id(), queue_item)) {
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, 
                "ATC代理处理事件: " + queue_item.event.event_name + 
                " (控制器: " + queue_item.controller_type + "::" + queue_item.controller_name + ")");
            
            // 执行对应的控制器
            bool executed = executeController(queue_item.controller_name, queue_item.parameters, current_time);
            
            if (executed) {
                processed_count++;
                VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, 
                    "ATC代理事件处理成功: " + queue_item.event.event_name);
            } else {
                VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, 
                    "ATC代理事件处理失败: " + queue_item.event.event_name);
            }
        }
        
        if (processed_count > 0) {
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, 
                "ATC代理本步处理事件数量: " + std::to_string(processed_count));
        }
        
        return processed_count;
    }

    // ==================== ATC控制器具体实现 ====================

    bool ATCAgent::executeClearanceController(const std::map<std::string, std::string>& params, double current_time) {
        VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "ATC代理: 执行滑行许可控制器");
        
        if (!shared_data_space) {
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "ATC代理: 全局共享数据空间未设置");
            return false;
        }
        
        // 更新ATC指令：设置滑行许可为true
        auto atc_command = shared_data_space->getATCCommand();
        atc_command.clearance_granted = true;
        atc_command.datasource = agent_id + "_clearance_controller";
        shared_data_space->setATCCommand(atc_command);
        
        VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "ATC代理: 滑行许可已发放，clearance_granted设置为true");
        return true;
    }

    bool ATCAgent::executeEmergencyBrakeController(const std::map<std::string, std::string>& params, double current_time) {
        VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "ATC代理: 执行紧急刹车控制器");
        
        if (!shared_data_space) {
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "ATC代理: 全局共享数据空间未设置");
            return false;
        }
        
        // 更新ATC指令：设置紧急刹车为true
        auto atc_command = shared_data_space->getATCCommand();
        atc_command.emergency_brake = true;
        atc_command.datasource = agent_id + "_emergency_brake_controller";
        shared_data_space->setATCCommand(atc_command);
        
        // 触发紧急刹车效果
        auto flight_state = shared_data_space->getAircraftFlightState();
        flight_state.airspeed = std::max(0.0, flight_state.airspeed - 5.0);  // 紧急减速
        flight_state.groundspeed = std::max(0.0, flight_state.groundspeed - 4.0);
        shared_data_space->setAircraftFlightState(flight_state);
        
        auto system_state = shared_data_space->getAircraftSystemState();
        system_state.current_brake_pressure = 2000000.0;  // 紧急刹车压力设为最大值
        shared_data_space->setAircraftSystemState(system_state);
        
        VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "ATC代理: 紧急刹车指令已执行，emergency_brake设置为true");
        return true;
    }

    bool ATCAgent::executeTakeoffClearanceController(const std::map<std::string, std::string>& params, double current_time) {
        VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "ATC代理: 执行起飞许可控制器");
        
        if (!shared_data_space) {
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "ATC代理: 全局共享数据空间未设置");
            return false;
        }
        
        // 更新ATC指令：设置起飞许可为true
        auto atc_command = shared_data_space->getATCCommand();
        atc_command.clearance_granted = true;
        atc_command.datasource = agent_id + "_takeoff_clearance_controller";
        shared_data_space->setATCCommand(atc_command);
        
        VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "ATC代理: 起飞许可已发放，clearance_granted设置为true");
        return true;
    }

    bool ATCAgent::executeLandingClearanceController(const std::map<std::string, std::string>& params, double current_time) {
        VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "ATC代理: 执行着陆许可控制器");
        
        if (!shared_data_space) {
            VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "ATC代理: 全局共享数据空间未设置");
            return false;
        }
        
        // 更新ATC指令：设置着陆许可为true
        auto atc_command = shared_data_space->getATCCommand();
        atc_command.clearance_granted = true;
        atc_command.datasource = agent_id + "_landing_clearance_controller";
        shared_data_space->setATCCommand(atc_command);
        
        VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "ATC代理: 着陆许可已发放，clearance_granted设置为true");
        return true;
    }

} // namespace VFT_SMF 