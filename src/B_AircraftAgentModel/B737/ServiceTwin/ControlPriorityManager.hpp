/**
 * @file ControlPriorityManager.hpp
 * @brief 控制优先级管理器 - 管理不同控制源的优先级和冲突解决
 * @author VFT_SMF Development Team
 * @date 2025-08-23
 */

#pragma once

#include "E_GlobalSharedDataSpace/GlobalSharedDataSpace.hpp"
#include "G_SimulationManager/LogAndData/Logger.hpp"
#include <memory>
#include <string>
#include <map>

namespace VFT_SMF {

    /**
     * @brief 控制优先级管理器类
     * @details 负责管理不同控制源（飞行员手动控制、自动驾驶仪、自动油门等）的优先级
     *          解决控制指令冲突，确保飞行安全
     */
    class ControlPriorityManager {
    private:
        std::shared_ptr<GlobalShared_DataSpace::GlobalSharedDataSpace> shared_data_space;
        
        // 控制源状态跟踪
        std::map<std::string, bool> control_source_status; ///< 各控制源的激活状态
        
        // 优先级配置
        std::map<std::string, GlobalSharedDataStruct::ControlPriority> source_priority_map; ///< 控制源到优先级的映射

    public:
        explicit ControlPriorityManager(std::shared_ptr<GlobalShared_DataSpace::GlobalSharedDataSpace> data_space)
            : shared_data_space(std::move(data_space)) {
            initializePriorityMap();
        }

        // ==================== 控制指令管理 ====================
        
        /**
         * @brief 设置飞行员手动控制指令
         * @param throttle 油门指令 [0.0, 1.0]
         * @param elevator 升降舵指令 [-1.0, 1.0]
         * @param aileron 副翼指令 [-1.0, 1.0]
         * @param rudder 方向舵指令 [-1.0, 1.0]
         * @param brake 刹车指令 [0.0, 1.0]
         * @param current_time 当前仿真时间
         */
        void setManualControlCommand(double throttle, double elevator, double aileron, 
                                   double rudder, double brake, double current_time);

        /**
         * @brief 设置自动驾驶仪控制指令
         * @param throttle 油门指令 [0.0, 1.0]
         * @param elevator 升降舵指令 [-1.0, 1.0]
         * @param aileron 副翼指令 [-1.0, 1.0]
         * @param rudder 方向舵指令 [-1.0, 1.0]
         * @param current_time 当前仿真时间
         */
        void setAutopilotControlCommand(double throttle, double elevator, double aileron, 
                                      double rudder, double current_time);

        /**
         * @brief 设置自动油门控制指令
         * @param throttle 油门指令 [0.0, 1.0]
         * @param current_time 当前仿真时间
         */
        void setAutothrottleControlCommand(double throttle, double current_time);

        /**
         * @brief 设置紧急控制指令（最高优先级）
         * @param throttle 油门指令 [0.0, 1.0]
         * @param elevator 升降舵指令 [-1.0, 1.0]
         * @param aileron 副翼指令 [-1.0, 1.0]
         * @param rudder 方向舵指令 [-1.0, 1.0]
         * @param brake 刹车指令 [0.0, 1.0]
         * @param current_time 当前仿真时间
         */
        void setEmergencyControlCommand(double throttle, double elevator, double aileron, 
                                      double rudder, double brake, double current_time);

        /**
         * @brief 清除指定优先级的控制指令
         * @param priority 要清除的优先级
         */
        void clearControlCommand(GlobalSharedDataStruct::ControlPriority priority);

        /**
         * @brief 清除所有控制指令
         */
        void clearAllControlCommands();

        // ==================== 控制源状态管理 ====================
        
        /**
         * @brief 激活控制源
         * @param source_name 控制源名称
         */
        void activateControlSource(const std::string& source_name);

        /**
         * @brief 停用控制源
         * @param source_name 控制源名称
         */
        void deactivateControlSource(const std::string& source_name);

        /**
         * @brief 检查控制源是否激活
         * @param source_name 控制源名称
         * @return 是否激活
         */
        bool isControlSourceActive(const std::string& source_name) const;

        // ==================== 优先级查询 ====================
        
        /**
         * @brief 获取当前激活的控制源
         * @return 激活的控制源名称
         */
        std::string getActiveControlSource() const;

        /**
         * @brief 获取最终控制指令
         * @return 最终控制指令
         */
        GlobalSharedDataStruct::ControlCommand getFinalControlCommand() const;

        /**
         * @brief 检查是否有手动超控
         * @return 是否有手动超控
         */
        bool hasManualOverride() const;

        // ==================== 安全检查和冲突解决 ====================
        
        /**
         * @brief 执行控制指令安全检查
         * @param command 控制指令
         * @return 是否通过安全检查
         */
        bool validateControlCommand(const GlobalSharedDataStruct::ControlCommand& command) const;

        /**
         * @brief 解决控制指令冲突
         * @param command1 指令1
         * @param command2 指令2
         * @return 解决后的指令
         */
        GlobalSharedDataStruct::ControlCommand resolveControlConflict(
            const GlobalSharedDataStruct::ControlCommand& command1,
            const GlobalSharedDataStruct::ControlCommand& command2) const;

        /**
         * @brief 应用最终控制指令到飞机系统
         * @param current_time 当前仿真时间
         */
        void applyFinalControlCommand(double current_time);

    private:
        /**
         * @brief 初始化优先级映射
         */
        void initializePriorityMap();

        /**
         * @brief 记录控制指令日志
         * @param command 控制指令
         * @param action 动作描述
         */
        void logControlCommand(const GlobalSharedDataStruct::ControlCommand& command, 
                             const std::string& action) const;

        /**
         * @brief 限制控制指令范围
         * @param command 控制指令
         */
        void limitControlCommand(GlobalSharedDataStruct::ControlCommand& command) const;
    };

} // namespace VFT_SMF
