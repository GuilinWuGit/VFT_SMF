/**
 * @file visualize_environment_state.cpp
 * @brief 环境状态可视化工具
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
 * @brief 环境状态数据结构
 */
struct EnvironmentStateData {
    double time;                    ///< 仿真时间
    std::string datasource;         ///< 数据来源
    double runway_length;           ///< 跑道长度
    double runway_width;            ///< 跑道宽度
    double friction_coefficient;    ///< 摩擦系数
    double air_density;             ///< 空气密度
    double wind_speed;              ///< 风速
    double wind_direction;          ///< 风向
    
    EnvironmentStateData() : time(0.0), runway_length(0.0), runway_width(0.0), 
                            friction_coefficient(0.0), air_density(0.0), 
                            wind_speed(0.0), wind_direction(0.0) {}
};

/**
 * @brief CSV解析器类
 */
class CSVParser {
public:
    std::vector<EnvironmentStateData> data;  ///< 解析后的数据
    
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
            if (tokens.size() >= 8) {
                EnvironmentStateData record;
                record.time = std::stod(tokens[0]);
                record.datasource = tokens[1];
                record.runway_length = std::stod(tokens[2]);
                record.runway_width = std::stod(tokens[3]);
                record.friction_coefficient = std::stod(tokens[4]);
                record.air_density = std::stod(tokens[5]);
                record.wind_speed = std::stod(tokens[6]);
                record.wind_direction = std::stod(tokens[7]);
                
                data.push_back(record);
            }
        }
        
        file.close();
        
        std::cout << "成功加载 " << data.size() << " 条环境状态记录" << std::endl;
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
        std::string script_path = output_dir + "/plot_environment_state.m";
        std::ofstream script_file(script_path);
        
        // 获取绝对路径并转换为MATLAB兼容格式
        std::string abs_output_dir = std::filesystem::absolute(output_dir).string();
        std::replace(abs_output_dir.begin(), abs_output_dir.end(), '\\', '/');
        
        script_file << "% 环境状态可视化脚本\n";
        script_file << "clear; close all; clc;\n\n";
        
        // 生成数据数组
        script_file << "% 仿真时间数组\n";
        script_file << "time = [";
        for (size_t i = 0; i < parser.data.size(); ++i) {
            if (i > 0) script_file << ", ";
            script_file << std::fixed << std::setprecision(2) << parser.data[i].time;
        }
        script_file << "];\n\n";
        
        // 生成环境参数数组
        script_file << "% 环境参数数组\n";
        script_file << "runway_length = [";
        for (size_t i = 0; i < parser.data.size(); ++i) {
            if (i > 0) script_file << ", ";
            script_file << std::fixed << std::setprecision(2) << parser.data[i].runway_length;
        }
        script_file << "];\n\n";
        
        script_file << "runway_width = [";
        for (size_t i = 0; i < parser.data.size(); ++i) {
            if (i > 0) script_file << ", ";
            script_file << std::fixed << std::setprecision(2) << parser.data[i].runway_width;
        }
        script_file << "];\n\n";
        
        script_file << "friction_coefficient = [";
        for (size_t i = 0; i < parser.data.size(); ++i) {
            if (i > 0) script_file << ", ";
            script_file << std::fixed << std::setprecision(2) << parser.data[i].friction_coefficient;
        }
        script_file << "];\n\n";
        
        script_file << "air_density = [";
        for (size_t i = 0; i < parser.data.size(); ++i) {
            if (i > 0) script_file << ", ";
            script_file << std::fixed << std::setprecision(2) << parser.data[i].air_density;
        }
        script_file << "];\n\n";
        
        script_file << "wind_speed = [";
        for (size_t i = 0; i < parser.data.size(); ++i) {
            if (i > 0) script_file << ", ";
            script_file << std::fixed << std::setprecision(2) << parser.data[i].wind_speed;
        }
        script_file << "];\n\n";
        
        script_file << "wind_direction = [";
        for (size_t i = 0; i < parser.data.size(); ++i) {
            if (i > 0) script_file << ", ";
            script_file << std::fixed << std::setprecision(2) << parser.data[i].wind_direction;
        }
        script_file << "];\n\n";
        
        // 创建图形
        script_file << "% 创建图形\n";
        script_file << "figure('Position', [100, 100, 1400, 1000]);\n\n";
        
        // 子图1: 跑道尺寸
        script_file << "% 子图1: 跑道尺寸\n";
        script_file << "subplot(3, 2, 1);\n";
        script_file << "yyaxis left;\n";
        script_file << "plot(time, runway_length, 'b-', 'LineWidth', 2);\n";
        script_file << "ylabel('跑道长度 (m)', 'FontSize', 10);\n";
        script_file << "yyaxis right;\n";
        script_file << "plot(time, runway_width, 'r-', 'LineWidth', 2);\n";
        script_file << "ylabel('跑道宽度 (m)', 'FontSize', 10);\n";
        script_file << "xlabel('仿真时间 (s)', 'FontSize', 10);\n";
        script_file << "title('跑道尺寸', 'FontSize', 12);\n";
        script_file << "grid on;\n";
        script_file << "legend({'长度', '宽度'}, 'Location', 'best');\n\n";
        
        // 子图2: 摩擦系数
        script_file << "% 子图2: 摩擦系数\n";
        script_file << "subplot(3, 2, 2);\n";
        script_file << "plot(time, friction_coefficient, 'g-', 'LineWidth', 2);\n";
        script_file << "ylabel('摩擦系数', 'FontSize', 10);\n";
        script_file << "xlabel('仿真时间 (s)', 'FontSize', 10);\n";
        script_file << "title('跑道摩擦系数', 'FontSize', 12);\n";
        script_file << "grid on;\n";
        script_file << "ylim([0, 1]);\n\n";
        
        // 子图3: 空气密度
        script_file << "% 子图3: 空气密度\n";
        script_file << "subplot(3, 2, 3);\n";
        script_file << "plot(time, air_density, 'm-', 'LineWidth', 2);\n";
        script_file << "ylabel('空气密度 (kg/m³)', 'FontSize', 10);\n";
        script_file << "xlabel('仿真时间 (s)', 'FontSize', 10);\n";
        script_file << "title('空气密度', 'FontSize', 12);\n";
        script_file << "grid on;\n\n";
        
        // 子图4: 风速
        script_file << "% 子图4: 风速\n";
        script_file << "subplot(3, 2, 4);\n";
        script_file << "plot(time, wind_speed, 'c-', 'LineWidth', 2);\n";
        script_file << "ylabel('风速 (m/s)', 'FontSize', 10);\n";
        script_file << "xlabel('仿真时间 (s)', 'FontSize', 10);\n";
        script_file << "title('风速', 'FontSize', 12);\n";
        script_file << "grid on;\n\n";
        
        // 子图5: 风向
        script_file << "% 子图5: 风向\n";
        script_file << "subplot(3, 2, 5);\n";
        script_file << "plot(time, wind_direction, 'k-', 'LineWidth', 2);\n";
        script_file << "ylabel('风向 (度)', 'FontSize', 10);\n";
        script_file << "xlabel('仿真时间 (s)', 'FontSize', 10);\n";
        script_file << "title('风向', 'FontSize', 12);\n";
        script_file << "grid on;\n";
        script_file << "ylim([0, 360]);\n\n";
        
        // 子图6: 风速风向组合图
        script_file << "% 子图6: 风速风向组合图\n";
        script_file << "subplot(3, 2, 6);\n";
        script_file << "yyaxis left;\n";
        script_file << "plot(time, wind_speed, 'b-', 'LineWidth', 2);\n";
        script_file << "ylabel('风速 (m/s)', 'FontSize', 10);\n";
        script_file << "yyaxis right;\n";
        script_file << "plot(time, wind_direction, 'r-', 'LineWidth', 2);\n";
        script_file << "ylabel('风向 (度)', 'FontSize', 10);\n";
        script_file << "xlabel('仿真时间 (s)', 'FontSize', 10);\n";
        script_file << "title('风速和风向', 'FontSize', 12);\n";
        script_file << "grid on;\n";
        script_file << "legend({'风速', '风向'}, 'Location', 'best');\n\n";
        
        // 设置整体标题
        script_file << "% 设置整体标题\n";
        script_file << "sgtitle('环境状态监控', 'FontSize', 16, 'FontWeight', 'bold');\n\n";
        
        // 保存图片
        script_file << "% 保存图片\n";
        script_file << "output_file = '" << abs_output_dir << "/environment_state.png';\n";
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
    std::cout << "环境状态可视化工具" << std::endl;
    std::cout << "========================================" << std::endl;
    
    // 检查命令行参数
    if (argc != 2) {
        std::cerr << "用法: " << argv[0] << " <environment_state.csv文件路径>" << std::endl;
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
    std::string script_path = output_dir + "/plot_environment_state.m";
    MATLABExecutor executor;
    
    if (executor.executeScript(script_path)) {
        std::cout << "========================================" << std::endl;
        std::cout << "可视化完成!" << std::endl;
        std::cout << "输出文件: " << output_dir << "/environment_state.png" << std::endl;
        std::cout << "========================================" << std::endl;
    } else {
        std::cerr << "可视化失败!" << std::endl;
        return 1;
    }
    
    return 0;
}
