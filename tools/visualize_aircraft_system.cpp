/**
 * @file visualize_aircraft_system.cpp
 * @brief 飞机系统状态可视化工具
 * @author VFT_SMF Development Team
 * @date 2024
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <algorithm>
#include <iomanip>
#include <filesystem>

/**
 * @brief 飞机系统状态数据结构
 */
struct AircraftSystemData {
    double time;                    ///< 仿真时间
    std::string datasource;         ///< 数据来源
    double current_mass;            ///< 当前质量
    double current_fuel;            ///< 当前燃油
    double current_center_of_gravity; ///< 当前重心
    double current_brake_pressure;  ///< 当前刹车压力
    double current_landing_gear_deployed; ///< 起落架展开状态
    double current_flaps_deployed;  ///< 襟翼展开状态
    double current_spoilers_deployed; ///< 扰流板展开状态
    double current_throttle_position; ///< 油门位置
    double current_engine_rpm;      ///< 发动机转速
    bool left_engine_failed;        ///< 左发动机故障
    double left_engine_rpm;         ///< 左发动机转速
    bool right_engine_failed;       ///< 右发动机故障
    double right_engine_rpm;        ///< 右发动机转速
    double brake_efficiency;        ///< 刹车效率
    
    AircraftSystemData() : time(0.0), current_mass(0.0), current_fuel(0.0), 
                          current_center_of_gravity(0.0), current_brake_pressure(0.0),
                          current_landing_gear_deployed(0.0), current_flaps_deployed(0.0),
                          current_spoilers_deployed(0.0), current_throttle_position(0.0),
                          current_engine_rpm(0.0), left_engine_failed(false), left_engine_rpm(0.0),
                          right_engine_failed(false), right_engine_rpm(0.0), brake_efficiency(0.0) {}
};

/**
 * @brief CSV解析器类
 */
class CSVParser {
public:
    std::vector<AircraftSystemData> data;  ///< 解析后的数据
    
    /**
     * @brief 分割字符串（按空格分隔）
     */
    std::vector<std::string> splitLine(const std::string& line) {
        std::vector<std::string> tokens;
        std::stringstream ss(line);
        std::string token;
        
        while (ss >> token) {
            tokens.push_back(token);
        }
        
        return tokens;
    }
    
    /**
     * @brief 加载CSV文件
     */
    bool loadCSV(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "错误: 无法打开文件 " << filename << std::endl;
            return false;
        }
        
        std::string line;
        bool is_header = true;
        
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            
            auto tokens = splitLine(line);
            if (tokens.empty()) continue;
            
            if (is_header) {
                is_header = false;
                continue;
            }
            
            // 解析数据行
            if (tokens.size() >= 16) {
                AircraftSystemData record;
                record.time = std::stod(tokens[0]);
                record.datasource = tokens[1];
                record.current_mass = std::stod(tokens[2]);
                record.current_fuel = std::stod(tokens[3]);
                record.current_center_of_gravity = std::stod(tokens[4]);
                record.current_brake_pressure = std::stod(tokens[5]);
                record.current_landing_gear_deployed = std::stod(tokens[6]);
                record.current_flaps_deployed = std::stod(tokens[7]);
                record.current_spoilers_deployed = std::stod(tokens[8]);
                record.current_throttle_position = std::stod(tokens[9]);
                record.current_engine_rpm = std::stod(tokens[10]);
                record.left_engine_failed = (tokens[11] == "true");
                record.left_engine_rpm = std::stod(tokens[12]);
                record.right_engine_failed = (tokens[13] == "true");
                record.right_engine_rpm = std::stod(tokens[14]);
                record.brake_efficiency = std::stod(tokens[15]);
                
                data.push_back(record);
            }
        }
        
        file.close();
        
        std::cout << "成功加载 " << data.size() << " 条飞机系统状态记录" << std::endl;
        return true;
    }
};

/**
 * @brief MATLAB脚本生成器类
 */
class MATLABScriptGenerator {
public:
    /**
     * @brief 生成绘图脚本
     */
    void generatePlotScript(const CSVParser& parser, const std::string& output_dir) {
        std::string script_path = output_dir + "/plot_aircraft_system.m";
        std::ofstream script_file(script_path);
        
        // 获取绝对路径并转换为MATLAB兼容格式
        std::string abs_output_dir = std::filesystem::absolute(output_dir).string();
        std::replace(abs_output_dir.begin(), abs_output_dir.end(), '\\', '/');
        
        script_file << "% 飞机系统状态可视化脚本\n";
        script_file << "clear; close all; clc;\n\n";
        
        // 生成数据数组
        script_file << "% 仿真时间数组\n";
        script_file << "time = [";
        for (size_t i = 0; i < parser.data.size(); ++i) {
            if (i > 0) script_file << ", ";
            script_file << std::fixed << std::setprecision(2) << parser.data[i].time;
        }
        script_file << "];\n\n";
        
        // 生成各系统参数数组
        script_file << "% 飞机系统参数数组\n";
        script_file << "mass = [";
        for (size_t i = 0; i < parser.data.size(); ++i) {
            if (i > 0) script_file << ", ";
            script_file << std::fixed << std::setprecision(2) << parser.data[i].current_mass;
        }
        script_file << "];\n\n";
        
        script_file << "fuel = [";
        for (size_t i = 0; i < parser.data.size(); ++i) {
            if (i > 0) script_file << ", ";
            script_file << std::fixed << std::setprecision(2) << parser.data[i].current_fuel;
        }
        script_file << "];\n\n";
        
        script_file << "throttle = [";
        for (size_t i = 0; i < parser.data.size(); ++i) {
            if (i > 0) script_file << ", ";
            script_file << std::fixed << std::setprecision(2) << parser.data[i].current_throttle_position;
        }
        script_file << "];\n\n";
        
        script_file << "brake_pressure = [";
        for (size_t i = 0; i < parser.data.size(); ++i) {
            if (i > 0) script_file << ", ";
            script_file << std::fixed << std::setprecision(2) << parser.data[i].current_brake_pressure;
        }
        script_file << "];\n\n";
        
        script_file << "left_engine_rpm = [";
        for (size_t i = 0; i < parser.data.size(); ++i) {
            if (i > 0) script_file << ", ";
            script_file << std::fixed << std::setprecision(2) << parser.data[i].left_engine_rpm;
        }
        script_file << "];\n\n";
        
        script_file << "right_engine_rpm = [";
        for (size_t i = 0; i < parser.data.size(); ++i) {
            if (i > 0) script_file << ", ";
            script_file << std::fixed << std::setprecision(2) << parser.data[i].right_engine_rpm;
        }
        script_file << "];\n\n";
        
        // 创建图形
        script_file << "% 创建图形\n";
        script_file << "figure('Position', [100, 100, 1400, 1000]);\n\n";
        
        // 子图1: 质量和燃油
        script_file << "% 子图1: 质量和燃油\n";
        script_file << "subplot(3, 2, 1);\n";
        script_file << "yyaxis left;\n";
        script_file << "plot(time, mass, 'b-', 'LineWidth', 2);\n";
        script_file << "ylabel('质量 (kg)', 'FontSize', 10);\n";
        script_file << "yyaxis right;\n";
        script_file << "plot(time, fuel, 'r-', 'LineWidth', 2);\n";
        script_file << "ylabel('燃油 (kg)', 'FontSize', 10);\n";
        script_file << "xlabel('仿真时间 (s)', 'FontSize', 10);\n";
        script_file << "title('飞机质量和燃油', 'FontSize', 12);\n";
        script_file << "grid on;\n";
        script_file << "legend({'质量', '燃油'}, 'Location', 'best');\n\n";
        
        // 子图2: 油门位置
        script_file << "% 子图2: 油门位置\n";
        script_file << "subplot(3, 2, 2);\n";
        script_file << "plot(time, throttle, 'g-', 'LineWidth', 2);\n";
        script_file << "ylabel('油门位置', 'FontSize', 10);\n";
        script_file << "xlabel('仿真时间 (s)', 'FontSize', 10);\n";
        script_file << "title('油门位置', 'FontSize', 12);\n";
        script_file << "grid on;\n";
        script_file << "ylim([0, 1]);\n\n";
        
        // 子图3: 刹车压力
        script_file << "% 子图3: 刹车压力\n";
        script_file << "subplot(3, 2, 3);\n";
        script_file << "plot(time, brake_pressure, 'm-', 'LineWidth', 2);\n";
        script_file << "ylabel('刹车压力', 'FontSize', 10);\n";
        script_file << "xlabel('仿真时间 (s)', 'FontSize', 10);\n";
        script_file << "title('刹车压力', 'FontSize', 12);\n";
        script_file << "grid on;\n\n";
        
        // 子图4: 发动机转速
        script_file << "% 子图4: 发动机转速\n";
        script_file << "subplot(3, 2, 4);\n";
        script_file << "plot(time, left_engine_rpm, 'b-', 'LineWidth', 2, 'DisplayName', '左发动机');\n";
        script_file << "hold on;\n";
        script_file << "plot(time, right_engine_rpm, 'r-', 'LineWidth', 2, 'DisplayName', '右发动机');\n";
        script_file << "ylabel('发动机转速', 'FontSize', 10);\n";
        script_file << "xlabel('仿真时间 (s)', 'FontSize', 10);\n";
        script_file << "title('发动机转速', 'FontSize', 12);\n";
        script_file << "grid on;\n";
        script_file << "legend('Location', 'best');\n";
        script_file << "hold off;\n\n";
        
        // 子图5: 起落架、襟翼、扰流板状态
        script_file << "% 子图5: 起落架、襟翼、扰流板状态\n";
        script_file << "subplot(3, 2, 5);\n";
        script_file << "landing_gear = [";
        for (size_t i = 0; i < parser.data.size(); ++i) {
            if (i > 0) script_file << ", ";
            script_file << std::fixed << std::setprecision(2) << parser.data[i].current_landing_gear_deployed;
        }
        script_file << "];\n";
        
        script_file << "flaps = [";
        for (size_t i = 0; i < parser.data.size(); ++i) {
            if (i > 0) script_file << ", ";
            script_file << std::fixed << std::setprecision(2) << parser.data[i].current_flaps_deployed;
        }
        script_file << "];\n";
        
        script_file << "spoilers = [";
        for (size_t i = 0; i < parser.data.size(); ++i) {
            if (i > 0) script_file << ", ";
            script_file << std::fixed << std::setprecision(2) << parser.data[i].current_spoilers_deployed;
        }
        script_file << "];\n";
        
        script_file << "plot(time, landing_gear, 'b-', 'LineWidth', 2, 'DisplayName', '起落架');\n";
        script_file << "hold on;\n";
        script_file << "plot(time, flaps, 'r-', 'LineWidth', 2, 'DisplayName', '襟翼');\n";
        script_file << "plot(time, spoilers, 'g-', 'LineWidth', 2, 'DisplayName', '扰流板');\n";
        script_file << "ylabel('展开状态', 'FontSize', 10);\n";
        script_file << "xlabel('仿真时间 (s)', 'FontSize', 10);\n";
        script_file << "title('起落架、襟翼、扰流板状态', 'FontSize', 12);\n";
        script_file << "grid on;\n";
        script_file << "legend('Location', 'best');\n";
        script_file << "ylim([0, 1]);\n";
        script_file << "hold off;\n\n";
        
        // 子图6: 刹车效率
        script_file << "% 子图6: 刹车效率\n";
        script_file << "subplot(3, 2, 6);\n";
        script_file << "brake_eff = [";
        for (size_t i = 0; i < parser.data.size(); ++i) {
            if (i > 0) script_file << ", ";
            script_file << std::fixed << std::setprecision(2) << parser.data[i].brake_efficiency;
        }
        script_file << "];\n";
        
        script_file << "plot(time, brake_eff, 'k-', 'LineWidth', 2);\n";
        script_file << "ylabel('刹车效率', 'FontSize', 10);\n";
        script_file << "xlabel('仿真时间 (s)', 'FontSize', 10);\n";
        script_file << "title('刹车效率', 'FontSize', 12);\n";
        script_file << "grid on;\n";
        script_file << "ylim([0, 1]);\n\n";
        
        // 设置整体标题
        script_file << "% 设置整体标题\n";
        script_file << "sgtitle('飞机系统状态监控', 'FontSize', 16, 'FontWeight', 'bold');\n\n";
        
        // 保存图片
        script_file << "% 保存图片\n";
        script_file << "output_file = '" << abs_output_dir << "/aircraft_system_state.png';\n";
        script_file << "print(output_file, '-dpng', '-r300');\n";
        script_file << "fprintf('图片已保存到: %s\\n', output_file);\n";
        
        script_file.close();
        std::cout << "MATLAB脚本已生成: " << script_path << std::endl;
    }
};

/**
 * @brief MATLAB执行器类
 */
class MATLABExecutor {
public:
    /**
     * @brief 执行MATLAB脚本
     */
    bool executeScript(const std::string& script_path) {
        std::string command = "matlab -batch \"run('" + script_path + "')\"";
        
        std::cout << "正在执行MATLAB脚本..." << std::endl;
        
        int result = system(command.c_str());
        
        if (result == 0) {
            std::cout << "MATLAB脚本执行成功!" << std::endl;
            return true;
        } else {
            std::cerr << "错误: MATLAB脚本执行失败 (错误代码: " << result << ")" << std::endl;
            return false;
        }
    }
};

/**
 * @brief 主函数
 */
int main(int argc, char* argv[]) {
    std::cout << "========================================" << std::endl;
    std::cout << "飞机系统状态可视化工具" << std::endl;
    std::cout << "========================================" << std::endl;
    
    // 检查命令行参数
    if (argc != 2) {
        std::cerr << "用法: " << argv[0] << " <aircraft_system_state.csv文件路径>" << std::endl;
        return 1;
    }
    
    std::string csv_file = argv[1];
    
    // 解析CSV文件
    CSVParser parser;
    if (!parser.loadCSV(csv_file)) {
        return 1;
    }
    
    if (parser.data.empty()) {
        std::cerr << "错误: 没有找到有效的数据记录" << std::endl;
        return 1;
    }
    
    // 获取输出目录（CSV文件所在目录）
    std::string output_dir = std::filesystem::path(csv_file).parent_path().string();
    if (output_dir.empty()) {
        output_dir = ".";
    }
    
    // 生成MATLAB脚本
    MATLABScriptGenerator generator;
    generator.generatePlotScript(parser, output_dir);
    
    // 执行MATLAB脚本
    std::string script_path = output_dir + "/plot_aircraft_system.m";
    MATLABExecutor executor;
    
    if (executor.executeScript(script_path)) {
        std::cout << "========================================" << std::endl;
        std::cout << "可视化完成!" << std::endl;
        std::cout << "输出文件: " << output_dir << "/aircraft_system_state.png" << std::endl;
        std::cout << "========================================" << std::endl;
    } else {
        std::cerr << "可视化失败!" << std::endl;
        return 1;
    }
    
    return 0;
}
