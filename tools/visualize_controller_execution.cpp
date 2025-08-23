/**
 * @file visualize_controller_execution.cpp
 * @brief 控制器执行状态可视化工具
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
 * @brief 控制器执行数据结构
 */
struct ControllerExecutionData {
    double time;                                    ///< 仿真时间
    std::map<std::string, int> controller_status;  ///< 控制器状态 (0或1)
    
    ControllerExecutionData() : time(0.0) {}
};

/**
 * @brief CSV解析器类
 */
class CSVParser {
public:
    std::vector<ControllerExecutionData> data;  ///< 解析后的数据
    std::vector<std::string> controller_names;  ///< 控制器名称列表
    
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
                // 解析表头，获取控制器名称
                controller_names.clear();
                for (size_t i = 1; i < tokens.size(); ++i) {  // 跳过第一列 SimulationTime
                    controller_names.push_back(tokens[i]);
                }
                
                std::cout << "检测到 " << controller_names.size() << " 个控制器:" << std::endl;
                for (const auto& name : controller_names) {
                    std::cout << "  - " << name << std::endl;
                }
                
                is_header = false;
                continue;
            }
            
            // 解析数据行
            if (tokens.size() >= controller_names.size() + 1) {
                ControllerExecutionData record;
                record.time = std::stod(tokens[0]);
                
                for (size_t i = 0; i < controller_names.size(); ++i) {
                    if (i + 1 < tokens.size()) {
                        record.controller_status[controller_names[i]] = std::stoi(tokens[i + 1]);
                    }
                }
                
                data.push_back(record);
            }
        }
        
        file.close();
        
        std::cout << "成功加载 " << data.size() << " 条记录" << std::endl;
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
        std::string script_path = output_dir + "/plot_controller_execution.m";
        std::ofstream script_file(script_path);
        
        // 获取绝对路径并转换为MATLAB兼容格式
        std::string abs_output_dir = std::filesystem::absolute(output_dir).string();
        std::replace(abs_output_dir.begin(), abs_output_dir.end(), '\\', '/');
        
        script_file << "% 控制器执行状态可视化脚本\n";
        script_file << "clear; close all; clc;\n\n";
        
        // 生成数据数组
        script_file << "% 仿真时间数组\n";
        script_file << "time = [";
        for (size_t i = 0; i < parser.data.size(); ++i) {
            if (i > 0) script_file << ", ";
            script_file << std::fixed << std::setprecision(2) << parser.data[i].time;
        }
        script_file << "];\n\n";
        
        // 为每个控制器生成数据数组
        for (size_t ctrl_idx = 0; ctrl_idx < parser.controller_names.size(); ++ctrl_idx) {
            const std::string& controller_name = parser.controller_names[ctrl_idx];
            script_file << "% " << controller_name << " 状态数组\n";
            script_file << "ctrl_" << ctrl_idx << " = [";
            
            for (size_t i = 0; i < parser.data.size(); ++i) {
                if (i > 0) script_file << ", ";
                auto it = parser.data[i].controller_status.find(controller_name);
                int status = (it != parser.data[i].controller_status.end()) ? it->second : 0;
                script_file << status;
            }
            script_file << "];\n";
        }
        script_file << "\n";
        
        // 创建图形
        script_file << "% 创建图形\n";
        script_file << "figure('Position', [100, 100, 1200, 800]);\n";
        script_file << "hold on;\n\n";
        
        // 绘制每个控制器的状态
        for (size_t ctrl_idx = 0; ctrl_idx < parser.controller_names.size(); ++ctrl_idx) {
            const std::string& controller_name = parser.controller_names[ctrl_idx];
            double y_position = static_cast<double>(ctrl_idx + 1);
            
            script_file << "% 绘制 " << controller_name << " 的状态\n";
            script_file << "ctrl_data = ctrl_" << ctrl_idx << ";\n";
            script_file << "y_pos = " << y_position << ";\n";
            
                         // 只绘制状态为1的点（实心圆）
             script_file << "active_indices = find(ctrl_data == 1);\n";
             script_file << "if ~isempty(active_indices)\n";
             script_file << "    scatter(time(active_indices), y_pos * ones(size(active_indices)), 50, 'filled', 'MarkerFaceColor', 'blue', 'MarkerEdgeColor', 'black');\n";
             script_file << "end\n\n";
        }
        
        // 设置坐标轴
        script_file << "% 设置坐标轴\n";
        script_file << "xlabel('仿真时间 (s)', 'FontSize', 12);\n";
        script_file << "ylabel('控制器', 'FontSize', 12);\n";
        
        // 设置Y轴刻度和标签
        script_file << "yticks([";
        for (size_t i = 0; i < parser.controller_names.size(); ++i) {
            if (i > 0) script_file << ", ";
            script_file << (i + 1);
        }
        script_file << "]);\n";
        
        script_file << "yticklabels({";
        for (size_t i = 0; i < parser.controller_names.size(); ++i) {
            if (i > 0) script_file << ", ";
            script_file << "'" << parser.controller_names[i] << "'";
        }
        script_file << "});\n";
        
        // 设置坐标轴范围
        if (!parser.data.empty()) {
            double min_time = parser.data.front().time;
            double max_time = parser.data.back().time;
            script_file << "xlim([" << min_time << ", " << max_time << "]);\n";
        }
        script_file << "ylim([0.5, " << (parser.controller_names.size() + 0.5) << "]);\n";
        
                 // 添加网格和图例
         script_file << "grid on;\n";
         script_file << "legend({'运行中 (1)'}, 'Location', 'best');\n";
        
        // 设置其他属性
        script_file << "set(gca, 'FontSize', 10);\n";
        script_file << "hold off;\n\n";
        
        // 保存图片
        script_file << "% 保存图片\n";
        script_file << "output_file = '" << abs_output_dir << "/controller_execution_status.png';\n";
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
    // 设置控制台编码为UTF-8
    std::cout << std::unitbuf;
    
    std::cout << "========================================" << std::endl;
    std::cout << "控制器执行状态可视化工具" << std::endl;
    std::cout << "========================================" << std::endl;
    
    // 检查命令行参数
    if (argc != 2) {
        std::cerr << "用法: " << argv[0] << " <controller_execution_status.csv文件路径>" << std::endl;
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
    
    if (parser.controller_names.empty()) {
        std::cerr << "错误: 没有找到控制器定义" << std::endl;
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
    std::string script_path = output_dir + "/plot_controller_execution.m";
    MATLABExecutor executor;
    
    if (executor.executeScript(script_path)) {
        std::cout << "========================================" << std::endl;
        std::cout << "可视化完成!" << std::endl;
        std::cout << "输出文件: " << output_dir << "/controller_execution_status.png" << std::endl;
        std::cout << "========================================" << std::endl;
    } else {
        std::cerr << "可视化失败!" << std::endl;
        return 1;
    }
    
    return 0;
}
