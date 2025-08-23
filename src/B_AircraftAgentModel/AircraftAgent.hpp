/**
 * @file AircraftAgent.hpp
 * @brief 飞机代理接口 - 整合B737数字孪生模型
 * @author VFT_SMF Development Team
 * @date 2024
 * 
 * AircraftAgent作为代理接口，整合B737数字孪生模型的三个层次：
 * 1. 数据层 (DataTwin) - 飞机基础数据
 * 2. 模型层 (ModelTwin) - 飞机物理模型
 * 3. 服务层 (ServiceTwin) - 飞机业务服务
 */

#pragma once

#include "../F_ScenarioModelling/B_ScenarioModel/VFT_SMF_Base.hpp"
#include "../E_GlobalSharedDataSpace/GlobalSharedDataSpace.hpp"
#include "IAircraftDigitalTwin.hpp"
#include "AircraftTypes.hpp"
#include "BasicTypes.hpp"
#include <memory>
#include <string>
#include <cmath>

// 前向声明，避免循环包含
namespace VFT_SMF {
    class B737DigitalTwin;
}

namespace VFT_SMF {

    // ==================== 基础数据类型定义 ====================
    // 已移动到 BasicTypes.hpp 中

    // ==================== 数字孪生模型接口 ====================
    
    /**
     * @brief 数据层接口 - 飞机基础数据
     */
    class IDataTwin {
    public:
        virtual ~IDataTwin() = default;
        virtual void initialize() = 0;
        virtual bool is_initialized() const = 0;
        virtual std::string get_aircraft_id() const = 0;
        virtual AircraftType get_aircraft_type() const = 0;
    };

    /**
     * @brief 模型层接口 - 飞机物理模型
     */
    class IModelTwin {
    public:
        virtual ~IModelTwin() = default;
        virtual void initialize() = 0;
        virtual void start() = 0;
        virtual void pause() = 0;
        virtual void resume() = 0;
        virtual void stop() = 0;
        virtual void update(double delta_time) = 0;
        virtual bool is_running() const = 0;
        virtual bool is_paused() const = 0;
    };

    /**
     * @brief 服务层接口 - 飞机业务服务
     */
    class IServiceTwin {
    public:
        virtual ~IServiceTwin() = default;
        virtual void initialize() = 0;
        virtual void update(double delta_time) = 0;
        virtual std::string get_status() const = 0;
        virtual bool is_ready() const = 0;
    };

    // ==================== 飞机代理类 ====================
    
    /**
     * @brief 飞机代理类
     * 作为代理接口，整合飞行器数字孪生模型的三个层次
     */
    class AircraftAgent : public BaseAgent {
    private:
        // 统一数字孪生接口
        std::unique_ptr<IAircraftDigitalTwin> digital_twin;  ///< 统一数字孪生接口
        
        // 代理状态
        AircraftType aircraft_type;                ///< 飞机类型
        FlightPhase current_phase;                 ///< 当前飞行阶段
        bool digital_twin_initialized;             ///< 数字孪生是否已初始化    
        
        // 线程安全
        mutable std::mutex agent_mutex;           ///< 代理锁
        
        // 全局共享数据空间引用（数据制造者需要访问）
        std::shared_ptr<VFT_SMF::GlobalShared_DataSpace::GlobalSharedDataSpace> shared_data_space;

    public:
        /**
         * @brief 构造函数
         * @param id 飞机唯一标识
         * @param name 飞机名称
         */
        AircraftAgent(const std::string& id, const std::string& name);
        
        /**
         * @brief 析构函数
         */
        ~AircraftAgent() override = default;

        // BaseAgent接口实现
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

        // 数字孪生模型管理
        void initialize_digital_twin();
        bool is_digital_twin_ready() const;
        void set_digital_twin(std::unique_ptr<IAircraftDigitalTwin> twin);

        // 飞机基本属性
        void set_aircraft_type(AircraftType type) { aircraft_type = type; }
        AircraftType get_aircraft_type() const { return aircraft_type; }
        void set_flight_phase(FlightPhase phase) { current_phase = phase; }
        FlightPhase get_flight_phase() const { return current_phase; }
        
        // 数字孪生模型状态查询
        bool is_digital_twin_initialized() const;
        bool is_digital_twin_running() const;
        bool is_digital_twin_paused() const;

        // 飞行器系统状态更新
        void updateAircraftSystemState();
        
        // 获取飞行器系统状态
        VFT_SMF::GlobalSharedDataStruct::AircraftSystemState getAircraftSystemState() const;

        // ==================== 统一控制器接口 ====================
        bool executeController(const std::string& controller_name, 
                              const std::map<std::string, std::string>& params,
                              double current_time);
        int processAgentEventQueue(double current_time);

        // 设置全局共享数据空间（数据制造者需要）
        void set_global_data_space(std::shared_ptr<VFT_SMF::GlobalShared_DataSpace::GlobalSharedDataSpace> data_space);



        // 综合状态报告
        std::string get_comprehensive_status_report() const;
        std::string get_digital_twin_status() const;
        
    private:
        // 从飞机配置文件加载配置
        void load_aircraft_config();

        // 私有辅助方法（仅内部使用）



    private:
        // ==================== 飞机控制器具体实现 ====================
        bool executeLeftEngineOutController(const std::map<std::string, std::string>& params, double current_time);
        bool executeBreakHalfController(const std::map<std::string, std::string>& params, double current_time);

        // 私有辅助方法
        void handle_system_event(const Event& event);
        void handle_agent_event(const Event& event);
        void validate_digital_twin_components() const;
    };

} // namespace VFT_SMF 