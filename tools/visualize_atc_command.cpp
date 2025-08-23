/**
 * @file visualize_atc_command.cpp
 * @brief ATC指令可视化工具
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
 * @brief ATC指令数据结构
 */
struct ATCCommandData {
    double time;                    ///< 仿真时间
    std::string datasource;         ///< 数据来源
    bool clearance_granted;         ///< 放行许可
    bool emergency_brake;           ///< 紧急刹车
    
    ATCCommandData() : time(0.0), clearance_granted(false), emergency_brake(false) {}
};

/**
 * @brief CSV解析器类
 */
class CSVParser {
public:
    std::vector<ATCCommandData> data;  ///< 解析后的数据
    
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
            if (tokens.size() >= 4) {
                ATCCommandData record;
                record.time = std::stod(tokens[0]);
                record.datasource = tokens[1];
                record.clearance_granted = (tokens[2] == "true");
                record.emergency_brake = (tokens[3] == "true");
                
                data.push_back(record);
            }
        }
        
        file.close();
        
        std::cout << "成功加载 " << data.size() << " 条ATC指令记录" << std::endl;
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
        std::string script_path = output_dir + "/plot_atc_command.m";
        std::ofstream script_file(script_path);
        
        // 获取绝对路径并转换为MATLAB兼容格式
        std::string abs_output_dir = std::filesystem::absolute(output_dir).string();
        std::replace(abs_output_dir.begin(), abs_output_dir.end(), '\\', '/');
        
        script_file << "% ATC指令可视化脚本\n";
        script_file << "clear; close all; clc;\n\n";
        
        // 生成数据数组
        script_file << "% 仿真时间数组\n";
        script_file << "time = [";
        for (size_t i = 0; i < parser.data.size(); ++i) {
            if (i > 0) script_file << ", ";
            script_file << std::fixed << std::setprecision(2) << parser.data[i].time;
        }
        script_file << "];\n\n";
        
        // 生成ATC指令状态数组
        script_file << "% ATC指令状态数组\n";
        script_file << "clearance_granted = [";
        for (size_t i = 0; i < parser.data.size(); ++i) {
            if (i > 0) script_file << ", ";
            script_file << (parser.data[i].clearance_granted ? "1" : "0");
        }
        script_file << "];\n\n";
        
        script_file << "emergency_brake = [";
        for (size_t i = 0; i < parser.data.size(); ++i) {
            if (i > 0) script_file << ", ";
            script_file << (parser.data[i].emergency_brake ? "1" : "0");
        }
        script_file << "];\n\n";
        
        // 创建图形
        script_file << "% 创建图形\n";
        script_file << "figure('Position', [100, 100, 1200, 600]);\n\n";
        
        // 子图1: 放行许可状态
        script_file << "% 子图1: 放行许可状态\n";
        script_file << "subplot(1, 2, 1);\n";
        script_file << "stairs(time, clearance_granted, 'b-', 'LineWidth', 3);\n";
        script_file << "ylabel('放行许可状态', 'FontSize', 12);\n";
        script_file << "xlabel('仿真时间 (s)', 'FontSize', 12);\n";
        script_file << "title('ATC放行许可状态', 'FontSize', 14);\n";
        script_file << "grid on;\n";
        script_file << "ylim([-0.1, 1.1]);\n";
        script_file << "yticks([0, 1]);\n";
        script_file << "yticklabels({'未放行', '已放行'});\n\n";
        
        // 子图2: 紧急刹车状态
        script_file << "% 子图2: 紧急刹车状态\n";
        script_file << "subplot(1, 2, 2);\n";
        script_file << "stairs(time, emergency_brake, 'r-', 'LineWidth', 3);\n";
        script_file << "ylabel('紧急刹车状态', 'FontSize', 12);\n";
        script_file << "xlabel('仿真时间 (s)', 'FontSize', 12);\n";
        script_file << "title('ATC紧急刹车指令', 'FontSize', 14);\n";
        script_file << "grid on;\n";
        script_file << "ylim([-0.1, 1.1]);\n";
        script_file << "yticks([0, 1]);\n";
        script_file << "yticklabels({'未激活', '已激活'});\n\n";
        
        // 设置整体标题
        script_file << "% 设置整体标题\n";
        script_file << "sgtitle('ATC指令监控', 'FontSize', 16, 'FontWeight', 'bold');\n\n";
        
        // 保存图片
        script_file << "% 保存图片\n";
        script_file << "output_file = '" << abs_output_dir << "/atc_command.png';\n";
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
    std::cout << "ATC指令可视化工具" << std::endl;
    std::cout << "========================================" << std::endl;
    
    // 检查命令行参数
    if (argc != 2) {
        std::cerr << "用法: " << argv[0] << " <atc_command.csv文件路径>" << std::endl;
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
    std::string script_path = output_dir + "/plot_atc_command.m";
    MATLABExecutor executor;
    
    if (executor.executeScript(script_path)) {
        std::cout << "========================================" << std::endl;
        std::cout << "可视化完成!" << std::endl;
        std::cout << "输出文件: " << output_dir << "/atc_command.png" << std::endl;
        std::cout << "========================================" << std::endl;
    } else {
        std::cerr << "可视化失败!" << std::endl;
        return 1;
    }
    
    return 0;
}
