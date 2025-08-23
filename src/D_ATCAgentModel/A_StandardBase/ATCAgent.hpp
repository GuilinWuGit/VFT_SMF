/**
 * @file ATCAgent.hpp
 * @brief ATC代理模型定义 - 重构版本
 * @author VFT_SMF Development Team
 * @date 2024
 * 
 * ATC代理模型负责根据飞行计划的逻辑线发出相应的指令，并写入全局共享数据空间
 */

#pragma once

#include "../../F_ScenarioModelling/B_ScenarioModel/VFT_SMF_Base.hpp"
#include "../../E_GlobalSharedDataSpace/GlobalSharedDataSpace.hpp"
#include <vector>
#include <map>
#include <string>
#include <memory>

// 前向声明
namespace VFT_SMF {
    class IATCStrategy;
}

// 包含策略接口的完整定义
#include "IATCStrategy.hpp"

namespace VFT_SMF {

    /**
     * @brief ATC指令类型枚举
     */
    enum class ATCInstructionType {
        CLEARANCE_GRANTED,    ///< 放行许可
        EMERGENCY_BRAKE,      ///< 紧急刹车
        TAXI_CLEARANCE,       ///< 滑行许可
        TAKEOFF_CLEARANCE,    ///< 起飞许可
        LANDING_CLEARANCE,    ///< 着陆许可
        HOLDING_INSTRUCTION,  ///< 等待指令
        SPEED_ADJUSTMENT,     ///< 速度调整
        ALTITUDE_CHANGE,      ///< 高度改变
        HEADING_CHANGE,       ///< 航向改变
        INFORMATION_BROADCAST ///< 信息广播
    };

    /**
     * @brief ATC指令结构体
     */
    struct ATCInstruction {
        std::string instruction_id;           ///< 指令ID
        ATCInstructionType type;              ///< 指令类型
        std::string target_aircraft_id;       ///< 目标飞机ID
        std::string target_pilot_id;          ///< 目标飞行员ID
        std::string instruction_content;      ///< 指令内容
        double issue_time;                    ///< 发出时间
        bool is_acknowledged;                 ///< 是否已确认
        bool is_executed;                     ///< 是否已执行
        
        ATCInstruction(ATCInstructionType t, const std::string& aircraft_id, 
                      const std::string& pilot_id, const std::string& content)
            : type(t), target_aircraft_id(aircraft_id), target_pilot_id(pilot_id),
              instruction_content(content), issue_time(0.0), 
              is_acknowledged(false), is_executed(false) {}
    };

    /**
     * @brief 逻辑线解析结果
     */
    struct LogicLineResult {
        std::string line_id;                  ///< 逻辑线ID
        std::string event_id;                 ///< 关联事件ID
        ATCInstructionType instruction_type;  ///< 指令类型
        std::string instruction_content;      ///< 指令内容
        double trigger_time;                  ///< 触发时间
        bool is_triggered;                    ///< 是否已触发
        
        LogicLineResult(const std::string& lid, const std::string& eid, 
                       ATCInstructionType itype, const std::string& content)
            : line_id(lid), event_id(eid), instruction_type(itype), 
              instruction_content(content), trigger_time(0.0), is_triggered(false) {}
    };

    /**
     * @brief ATC代理类 - 重构版本
     * 支持集成不同的指令处理器，实现不同的ATC逻辑
     */
    class ATCAgent : public BaseAgent {
    private:
        // 全局共享数据空间指针
        std::shared_ptr<GlobalShared_DataSpace::GlobalSharedDataSpace> shared_data_space;
        
        // 飞行计划数据
        VFT_SMF::GlobalSharedDataStruct::FlightPlanData flight_plan_data;
        
        // 逻辑线解析结果
        std::vector<LogicLineResult> logic_line_results;
        
        // 已发出的指令记录
        std::vector<ATCInstruction> issued_instructions;
        
        // ATC代理配置
        std::string atc_facility_id;          ///< ATC设施ID
        std::string atc_facility_name;        ///< ATC设施名称
        double current_simulation_time;       ///< 当前仿真时间
        
        // 性能统计
        int total_instructions_issued;        ///< 总发出指令数
        int total_instructions_acknowledged;  ///< 总确认指令数
        int total_instructions_executed;      ///< 总执行指令数
        
        // ATC策略（根据配置的ATC_ID动态选择）
        std::unique_ptr<IATCStrategy> atc_strategy;
        
    public:
        ATCAgent(const std::string& id, const std::string& name);
        ~ATCAgent() = default;

        // 基础代理接口
        void initialize() override;
        void start() override;
        void pause() override;
        void resume() override;
        void stop() override;
        void update(double delta_time) override;
        void handle_event(const Event& event) override;
        void send_event(const Event& event) override;
        std::string get_status() const override;
        bool is_ready() const override;

        // 设置全局共享数据空间
        void set_shared_data_space(std::shared_ptr<GlobalShared_DataSpace::GlobalSharedDataSpace> data_space);
        
        // 设置飞行计划数据
        void set_flight_plan_data(const VFT_SMF::GlobalSharedDataStruct::FlightPlanData& plan_data);
        
        // 解析逻辑线并生成指令
        void parse_logic_lines_and_generate_instructions();
        
        // 检查事件触发条件并发出指令
        void check_event_triggers_and_issue_instructions(double current_time);
        
        // 发出ATC指令到全局共享数据空间
        void issue_atc_instruction(const ATCInstruction& instruction);
        
        // 更新ATC指令状态
        void update_instruction_status(const std::string& instruction_id, bool acknowledged, bool executed);
        
        // 获取ATC指令状态
        VFT_SMF::GlobalSharedDataStruct::ATC_Command get_current_atc_command() const;
        
        // 获取性能统计
        std::string get_performance_summary() const;
        
        // ==================== 统一控制器接口 ====================
        
        /**
         * @brief 统一的控制器执行接口
         * @param controller_name 控制器名称
         * @param params 控制器参数
         * @param current_time 当前时间
         * @return 是否执行成功
         */
        bool executeController(const std::string& controller_name, 
                              const std::map<std::string, std::string>& params,
                              double current_time);
        
        /**
         * @brief 处理代理事件队列中的事件
         * @param current_time 当前时间
         * @return 处理的事件数量
         */
        int processAgentEventQueue(double current_time);
        
        // ==================== ATC策略管理 ====================
        
        /**
         * @brief 设置ATC策略
         * @param strategy ATC策略
         */
        void setATCStrategy(std::unique_ptr<IATCStrategy> strategy);
        
        /**
         * @brief 根据ATC_ID创建并设置策略
         * @param atc_id ATC标识符
         */
        void initializeATCStrategy(const std::string& atc_id);
        
        /**
         * @brief 获取ATC策略
         * @return 策略指针
         */
        IATCStrategy* getATCStrategy() const { return atc_strategy.get(); }
        
        /**
         * @brief 获取策略配置信息
         * @return 策略配置
         */
        std::map<std::string, std::string> getStrategyConfig() const;
        
        // Getter方法
        std::string get_facility_id() const { return atc_facility_id; }
        std::string get_facility_name() const { return atc_facility_name; }
        const std::vector<LogicLineResult>& get_logic_line_results() const { return logic_line_results; }
        const std::vector<ATCInstruction>& get_issued_instructions() const { return issued_instructions; }
        
    private:
        // 私有辅助方法
        ATCInstructionType parse_instruction_type_from_logic_line(const std::string& logic_line_content);
        std::string generate_instruction_content(const LogicLineResult& logic_result);
        bool should_issue_instruction(const LogicLineResult& logic_result, double current_time);
        void write_atc_command_to_shared_space(const ATCInstruction& instruction);
        
        // ==================== 默认ATC控制器实现方法 ====================
        // 这些方法在没有指令处理器时作为默认实现
        
        /**
         * @brief 执行滑行许可控制器
         * @param params 控制器参数
         * @param current_time 当前时间
         * @return 是否执行成功
         */
        bool executeClearanceController(const std::map<std::string, std::string>& params, double current_time);
        
        /**
         * @brief 执行紧急刹车控制器
         * @param params 控制器参数
         * @param current_time 当前时间
         * @return 是否执行成功
         */
        bool executeEmergencyBrakeController(const std::map<std::string, std::string>& params, double current_time);
        
        /**
         * @brief 执行起飞许可控制器
         * @param params 控制器参数
         * @param current_time 当前时间
         * @return 是否执行成功
         */
        bool executeTakeoffClearanceController(const std::map<std::string, std::string>& params, double current_time);
        
        /**
         * @brief 执行着陆许可控制器
         * @param params 控制器参数
         * @param current_time 当前时间
         * @return 是否执行成功
         */
        bool executeLandingClearanceController(const std::map<std::string, std::string>& params, double current_time);
    };

} // namespace VFT_SMF 