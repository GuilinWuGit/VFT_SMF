/**
 * @file SoftwareSettings.hpp
 * @brief 软件设置定义
 * @author VFT_SMF Development Team
 * @date 2024
 * 
 * 软件设置负责管理系统配置、参数设置和用户偏好
 */

#pragma once

#include "../F_ScenarioModelling/B_ScenarioModel/VFT_SMF_Base.hpp"
#include <vector>
#include <map>
#include <memory>
#include <functional>

// 飞行动力学耗时监测开关（编译期）：0 关闭（默认），1 开启
#ifndef VFT_ENABLE_FD_TIMING
#define VFT_ENABLE_FD_TIMING 0
#endif

namespace VFT_SMF {

    /**
     * @brief 设置类型枚举
     */
    enum class SettingType {
        BOOLEAN,            ///< 布尔值
        INTEGER,            ///< 整数值
        FLOAT,              ///< 浮点值
        STRING,             ///< 字符串
        ENUM,               ///< 枚举值
        ARRAY,              ///< 数组
        OBJECT              ///< 对象
    };

    /**
     * @brief 设置分类枚举
     */
    enum class SettingCategory {
        GENERAL,            ///< 通用设置
        SIMULATION,         ///< 仿真设置
        DISPLAY,            ///< 显示设置
        AUDIO,              ///< 音频设置
        INPUT,              ///< 输入设置
        NETWORK,            ///< 网络设置
        SECURITY,           ///< 安全设置
        PERFORMANCE,        ///< 性能设置
        DEBUG,              ///< 调试设置
        CUSTOM              ///< 自定义设置
    };

    /**
     * @brief 设置验证规则
     */
    struct SettingValidation {
        bool required;
        double min_value;
        double max_value;
        std::vector<std::string> allowed_values;
        std::string pattern;  // 正则表达式模式
        std::function<bool(const std::string&)> custom_validator;
        
        SettingValidation() : required(false), min_value(0.0), max_value(0.0) {}
    };

    /**
     * @brief 设置项结构体
     */
    struct SettingItem {
        std::string key;
        std::string name;
        std::string description;
        SettingType type;
        SettingCategory category;
        std::string default_value;
        std::string current_value;
        SettingValidation validation;
        bool is_modified;
        std::chrono::system_clock::time_point last_modified;
        
        SettingItem(const std::string& k, const std::string& n, SettingType t, SettingCategory c)
            : key(k), name(n), type(t), category(c), is_modified(false) {}
    };

    /**
     * @brief 设置组结构体
     */
    struct SettingGroup {
        std::string name;
        std::string description;
        SettingCategory category;
        std::vector<SettingItem> settings;
        bool is_expanded;
        
        SettingGroup(const std::string& n, SettingCategory c) : name(n), category(c), is_expanded(true) {}
    };

    /**
     * @brief 配置变更事件
     */
    struct SettingChangeEvent {
        std::string setting_key;
        std::string old_value;
        std::string new_value;
        std::chrono::system_clock::time_point timestamp;
        std::string user_id;
        
        SettingChangeEvent(const std::string& key, const std::string& old_val, const std::string& new_val)
            : setting_key(key), old_value(old_val), new_value(new_val), 
              timestamp(std::chrono::system_clock::now()) {}
    };

    /**
     * @brief 设置管理器
     */
    class SettingsManager {
    private:
        std::map<std::string, SettingItem> settings;
        std::map<SettingCategory, SettingGroup> setting_groups;
        std::vector<SettingChangeEvent> change_history;
        std::vector<std::function<void(const SettingChangeEvent&)>> change_callbacks;
        
        std::string config_file;
        std::mutex settings_mutex;
        std::mutex history_mutex;
        
        bool auto_save;
        int max_history_size;
        
    public:
        SettingsManager(const std::string& config_file = "vft_smf_settings.json");
        
        // 设置管理
        void add_setting(const SettingItem& setting);
        void remove_setting(const std::string& key);
        SettingItem* get_setting(const std::string& key);
        const SettingItem* get_setting(const std::string& key) const;
        std::vector<SettingItem> get_settings_by_category(SettingCategory category) const;
        std::vector<std::string> get_all_setting_keys() const;
        
        // 值操作
        bool set_value(const std::string& key, const std::string& value, const std::string& user_id = "");
        std::string get_value(const std::string& key, const std::string& default_value = "") const;
        bool has_setting(const std::string& key) const;
        
        // 验证
        bool validate_setting(const std::string& key, const std::string& value) const;
        std::vector<std::string> get_validation_errors(const std::string& key, const std::string& value) const;
        
        // 变更历史
        void add_change_callback(std::function<void(const SettingChangeEvent&)> callback);
        std::vector<SettingChangeEvent> get_change_history() const;
        void clear_change_history();
        
        // 持久化
        bool load_settings();
        bool save_settings();
        bool export_settings(const std::string& filename) const;
        bool import_settings(const std::string& filename);
        
        // 配置
        void set_auto_save(bool enable) { auto_save = enable; }
        void set_max_history_size(int size) { max_history_size = size; }
        void set_config_file(const std::string& file) { config_file = file; }
    };

    /**
     * @brief 默认设置提供者
     */
    class DefaultSettingsProvider {
    public:
        static std::vector<SettingItem> get_default_settings();
        static std::map<SettingCategory, SettingGroup> get_default_setting_groups();
        
    private:
        static void add_general_settings(std::vector<SettingItem>& settings);
        static void add_simulation_settings(std::vector<SettingItem>& settings);
        static void add_display_settings(std::vector<SettingItem>& settings);
        static void add_audio_settings(std::vector<SettingItem>& settings);
        static void add_input_settings(std::vector<SettingItem>& settings);
        static void add_network_settings(std::vector<SettingItem>& settings);
        static void add_security_settings(std::vector<SettingItem>& settings);
        static void add_performance_settings(std::vector<SettingItem>& settings);
        static void add_debug_settings(std::vector<SettingItem>& settings);
    };

    /**
     * @brief 设置验证器
     */
    class SettingsValidator {
    public:
        static bool validate_boolean(const std::string& value);
        static bool validate_integer(const std::string& value, int min_val, int max_val);
        static bool validate_float(const std::string& value, double min_val, double max_val);
        static bool validate_string(const std::string& value, size_t min_length, size_t max_length);
        static bool validate_enum(const std::string& value, const std::vector<std::string>& allowed_values);
        static bool validate_pattern(const std::string& value, const std::string& pattern);
        
        static std::vector<std::string> get_validation_errors(const SettingItem& setting, const std::string& value);
    };

    /**
     * @brief 软件设置类
     */
    class SoftwareSettings : public BaseAgent {
    private:
        SettingsManager settings_manager;
        std::map<std::string, std::string> cached_values;
        std::vector<std::string> modified_settings;
        
        // 设置特定参数
        std::string settings_directory;
        std::string backup_directory;
        bool enable_backup;
        int backup_interval;
        std::chrono::system_clock::time_point last_backup_time;
        
        // 性能统计
        int total_settings_loaded;
        int total_settings_saved;
        int total_settings_modified;
        double average_load_time;
        double average_save_time;
        
    public:
        SoftwareSettings(const std::string& id, const std::string& name);
        ~SoftwareSettings() override = default;

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

        // 软件设置特定方法
        void load_default_settings();
        void initialize_settings_from_file(const std::string& filename);
        void save_settings_to_file(const std::string& filename);
        
        // 设置操作
        void add_setting(const SettingItem& setting);
        void remove_setting(const std::string& key);
        bool set_setting_value(const std::string& key, const std::string& value, const std::string& user_id = "");
        std::string get_setting_value(const std::string& key, const std::string& default_value = "") const;
        bool has_setting(const std::string& key) const;
        
        // 类型化设置访问
        bool get_bool_setting(const std::string& key, bool default_value = false) const;
        int get_int_setting(const std::string& key, int default_value = 0) const;
        double get_float_setting(const std::string& key, double default_value = 0.0) const;
        std::string get_string_setting(const std::string& key, const std::string& default_value = "") const;
        
        void set_bool_setting(const std::string& key, bool value, const std::string& user_id = "");
        void set_int_setting(const std::string& key, int value, const std::string& user_id = "");
        void set_float_setting(const std::string& key, double value, const std::string& user_id = "");
        void set_string_setting(const std::string& key, const std::string& value, const std::string& user_id = "");
        
        // 设置查询
        std::vector<SettingItem> get_settings_by_category(SettingCategory category) const;
        std::vector<SettingItem> get_modified_settings() const;
        std::vector<SettingChangeEvent> get_setting_history() const;
        
        // 设置验证
        bool validate_setting(const std::string& key, const std::string& value) const;
        std::vector<std::string> get_setting_validation_errors(const std::string& key, const std::string& value) const;
        
        // 设置组管理
        void add_setting_group(const SettingGroup& group);
        std::vector<SettingGroup> get_all_setting_groups() const;
        SettingGroup* get_setting_group(const std::string& name);
        
        // 备份和恢复
        void set_backup_enabled(bool enable);
        void set_backup_interval(int interval);
        void create_backup();
        bool restore_from_backup(const std::string& backup_file);
        std::vector<std::string> get_available_backups() const;
        
        // 导入导出
        bool export_settings(const std::string& filename) const;
        bool import_settings(const std::string& filename);
        bool export_settings_by_category(SettingCategory category, const std::string& filename) const;
        
        // 性能评估
        double get_performance_score() const;
        std::string get_performance_report() const;
        bool is_settings_valid() const;
        
        // 配置
        void set_settings_directory(const std::string& dir);
        void set_backup_directory(const std::string& dir);
        void set_auto_save(bool enable);
        
        // Getter方法
        std::string get_settings_directory() const { return settings_directory; }
        std::string get_backup_directory() const { return backup_directory; }
        bool is_backup_enabled() const { return enable_backup; }
        int get_backup_interval() const { return backup_interval; }
        const SettingsManager& get_settings_manager() const { return settings_manager; }
        
    private:
        void initialize_default_settings();
        void setup_setting_callbacks();
        void process_setting_changes();
        void perform_backup_if_needed();
        void validate_all_settings();
        void cache_setting_values();
    };

} // namespace VFT_SMF 