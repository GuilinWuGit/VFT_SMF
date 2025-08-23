/**
 * @file visualize_aircraft_state.cpp
 * @brief 飞机状态数据可视化工具 - 通过调用MATLAB绘制图表
 * @author VFT_SMF Development Team
 * @date 2024
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <windows.h>

namespace fs = std::filesystem;

/**
 * @brief CSV数据解析器
 */
class CSVParser {
private:
    std::vector<std::string> headers;
    std::vector<std::vector<double>> data;
    std::vector<std::string> datasource;

public:
    bool loadCSV(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "错误: 无法打开文件 " << filename << std::endl;
            return false;
        }

        std::string line;
        
        // 读取表头
        if (std::getline(file, line)) {
            headers = splitFixedWidth(line);
            std::cout << "成功读取表头，共 " << headers.size() << " 列" << std::endl;
            
            // 显示表头信息
            std::cout << "表头内容:" << std::endl;
            for (size_t i = 0; i < headers.size(); ++i) {
                std::cout << "  列 " << i << ": '" << headers[i] << "'" << std::endl;
            }
        } else {
            std::cerr << "错误: 文件为空" << std::endl;
            return false;
        }

        // 初始化数据容器
        data.resize(headers.size());
        datasource.clear();

        // 读取数据行
        int rowCount = 0;
        while (std::getline(file, line)) {
            auto values = splitFixedWidth(line);
            if (values.size() != headers.size()) {
                std::cout << "警告: 第 " << (rowCount + 1) << " 行列数不匹配，跳过" << std::endl;
                continue;
            }

            // 解析数值数据
            for (size_t i = 0; i < values.size(); i++) {
                if (i == 1) { // datasource列
                    datasource.push_back(values[i]);
                } else {
                    try {
                        double val = std::stod(values[i]);
                        data[i].push_back(val);
                    } catch (...) {
                        data[i].push_back(0.0);
                    }
                }
            }
            rowCount++;
            
            // 显示前几行的解析结果
            if (rowCount <= 3) {
                std::cout << "第 " << rowCount << " 行解析结果:" << std::endl;
                for (size_t i = 0; i < values.size(); ++i) {
                    if (i == 1) {
                        std::cout << "  列 " << i << " (datasource): '" << values[i] << "'" << std::endl;
                    } else {
                        std::cout << "  列 " << i << " (" << headers[i] << "): " << data[i].back() << std::endl;
                    }
                }
            }
        }

        std::cout << "成功读取 " << rowCount << " 行数据" << std::endl;
        
        // 显示每列的数据统计
        std::cout << "数据统计:" << std::endl;
        for (size_t i = 0; i < headers.size(); ++i) {
            if (i == 1) {
                std::cout << "  列 " << i << " (datasource): " << datasource.size() << " 个字符串值" << std::endl;
            } else {
                std::cout << "  列 " << i << " (" << headers[i] << "): " << data[i].size() << " 个数值" << std::endl;
            }
        }
        
        return true;
    }

    const std::vector<std::string>& getHeaders() const { return headers; }
    const std::vector<std::vector<double>>& getData() const { return data; }
    const std::vector<std::string>& getDatasource() const { return datasource; }

private:
    std::vector<std::string> splitLine(const std::string& line, char delimiter) {
        std::vector<std::string> tokens;
        std::stringstream ss(line);
        std::string token;
        
        while (std::getline(ss, token, delimiter)) {
            // 去除首尾空格
            if (!token.empty()) {
                token.erase(0, token.find_first_not_of(" \t"));
                token.erase(token.find_last_not_of(" \t") + 1);
            }
            tokens.push_back(token);
        }
        
        return tokens;
    }

    std::vector<std::string> splitFixedWidth(const std::string& line) {
        std::vector<std::string> tokens;
        
        // 使用空格分割，然后过滤空字符串
        std::stringstream ss(line);
        std::string token;
        
        while (ss >> token) {
            // 去除首尾空格
            if (!token.empty()) {
                token.erase(0, token.find_first_not_of(" \t"));
                token.erase(token.find_last_not_of(" \t") + 1);
                if (!token.empty()) {
                    tokens.push_back(token);
                }
            }
        }
        
        return tokens;
    }
};



/**
 * @brief MATLAB脚本生成器
 */
class MATLABScriptGenerator {
public:
    static std::string generatePlotScript(const CSVParser& parser, const std::string& output_dir) {
        std::stringstream script;
        
        // 脚本头部
        script << "% 飞机状态数据可视化脚本\n";
        script << "% 自动生成于 " << getCurrentTime() << "\n\n";
        
        // 数据定义
        script << "% 定义数据\n";
        const auto& headers = parser.getHeaders();
        const auto& data = parser.getData();
        
        // 生成时间数据
        if (!data.empty() && !data[0].empty()) {
            script << "simulation_time = [";
            for (size_t i = 0; i < data[0].size(); ++i) {
                if (i > 0) script << ", ";
                script << data[0][i];
            }
            script << "];\n\n";
        }

        // 生成各列数据（跳过datasource列）
        for (size_t i = 2; i < headers.size(); ++i) { // 从第3列开始（跳过SimulationTime和datasource）
            std::string var_name = "data_" + std::to_string(i-1); // 从data_1开始，对应latitude
            script << var_name << " = [";
            for (size_t j = 0; j < data[i].size(); ++j) {
                if (j > 0) script << ", ";
                script << data[i][j];
            }
            script << "];\n";
        }
        script << "\n";

        // 创建图形
        script << "% 创建图形\n";
        script << "figure('Position', [100, 100, 1200, 800]);\n\n";
        
        // 计算子图布局（跳过datasource列）
        int plot_count = 0;
        for (size_t i = 2; i < headers.size(); ++i) { // 从第3列开始（跳过SimulationTime和datasource）
            plot_count++;
        }
        int cols = 2;  // 改为2列
        int rows = 5;  // 固定5行
        
        script << "% 子图布局: " << rows << "行 " << cols << "列\n";
        
        // 生成子图（跳过datasource列）
        int plot_idx = 1;
        for (size_t i = 2; i < headers.size(); ++i) { // 从第3列开始（跳过SimulationTime和datasource）
            script << "subplot(" << rows << ", " << cols << ", " << plot_idx << ");\n";
            script << "plot(simulation_time, data_" << (i-1) << ", 'b-', 'LineWidth', 1.5);\n"; // 修正索引
            script << "xlabel('仿真时间 (s)');\n";
            script << "ylabel('" << headers[i] << "');\n";
            script << "grid on;\n";
            
            // 设置纵坐标格式，保留2位小数
            script << "ax = gca;\n";
            script << "ytick = get(ax, 'YTick');\n";
            script << "yticklabel = arrayfun(@(x) sprintf('%.2f', x), ytick, 'UniformOutput', false);\n";
            script << "set(ax, 'YTickLabel', yticklabel);\n";
            
            // 设置纵坐标标题左对齐
            script << "set(get(ax, 'YLabel'), 'HorizontalAlignment', 'left');\n\n";
            
            plot_idx++;
        }
        
        // 调整布局
        script << "% 调整布局\n";
        script << "set(gcf, 'Color', 'white');\n\n";
        
        // 保存图片
        std::string output_path = (fs::path(output_dir) / "aircraft_flight_state.png").string();
        // 转换为绝对路径和MATLAB兼容的路径格式
        fs::path abs_path = fs::absolute(output_path);
        std::string abs_path_str = abs_path.string();
        std::replace(abs_path_str.begin(), abs_path_str.end(), '\\', '/');
        script << "% 保存图片\n";
        script << "print('" << abs_path_str << "', '-dpng', '-r300');\n";
        script << "fprintf('图片已保存到: " << abs_path_str << "\\n');\n\n";
        
        script << "fprintf('可视化完成!\\n');\n";
        
        return script.str();
    }

private:
    static std::string getCurrentTime() {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
};

/**
 * @brief MATLAB调用器
 */
class MATLABCaller {
public:
    static bool callMATLAB(const std::string& script_content, const std::string& script_file) {
        // 保存脚本到文件
        std::ofstream script_file_stream(script_file);
        if (!script_file_stream.is_open()) {
            std::cerr << "错误: 无法创建MATLAB脚本文件" << std::endl;
            return false;
        }
        script_file_stream << script_content;
        script_file_stream.close();
        
        std::cout << "MATLAB脚本已保存到: " << script_file << std::endl;
        
        // 构建MATLAB命令
        std::string matlab_cmd = "matlab -batch \"run('" + script_file + "'); exit;\"";
        
        std::cout << "正在调用MATLAB..." << std::endl;
        std::cout << "命令: " << matlab_cmd << std::endl;
        
        // 执行MATLAB命令
        int result = system(matlab_cmd.c_str());
        
        if (result == 0) {
            std::cout << "MATLAB执行成功!" << std::endl;
            return true;
        } else {
            std::cerr << "MATLAB执行失败，错误代码: " << result << std::endl;
            std::cerr << "请确保MATLAB已安装并添加到系统PATH中" << std::endl;
            return false;
        }
    }
};

int main(int argc, char* argv[]) {

    // 设置控制台代码页为UTF-8，用于支持中文显示
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);

    std::cout << "========================================\n";
    std::cout << "飞机状态数据可视化工具 (MATLAB版本)\n";
    std::cout << "========================================\n\n";
    
    // 检查命令行参数
    if (argc < 2) {
        std::cout << "用法: " << argv[0] << " <CSV文件路径> [输出目录]\n";
        std::cout << "示例: " << argv[0] << " ../ScenarioExamples/B737_Taxi/output/aircraft_flight_state.csv\n";
        return 1;
    }
    
    std::string csv_file = argv[1];
    std::string output_dir = (argc > 2) ? argv[2] : fs::path(csv_file).parent_path().string();
    
    // 检查CSV文件是否存在
    if (!fs::exists(csv_file)) {
        std::cerr << "错误: CSV文件不存在: " << csv_file << std::endl;
        return 1;
    }
    
    std::cout << "CSV文件: " << csv_file << std::endl;
    std::cout << "输出目录: " << output_dir << std::endl;
    std::cout << std::endl;
    
    // 解析CSV数据
    CSVParser parser;
    if (!parser.loadCSV(csv_file)) {
        std::cerr << "错误: 无法解析CSV文件" << std::endl;
        return 1;
    }
    

    
    // 生成MATLAB脚本
    std::cout << "生成MATLAB脚本..." << std::endl;
    std::string script_content = MATLABScriptGenerator::generatePlotScript(parser, output_dir);
    
    // 保存脚本文件
    std::string script_file = (fs::path(output_dir) / "plot_aircraft_state.m").string();
    
    // 调用MATLAB
    std::cout << "调用MATLAB绘制图表..." << std::endl;
    if (MATLABCaller::callMATLAB(script_content, script_file)) {
        std::cout << "\n可视化完成!\n";
        std::cout << "输出文件:\n";
        std::cout << "- " << (fs::path(output_dir) / "aircraft_state_visualization.png").string() << std::endl;
        std::cout << "- " << (fs::path(output_dir) / "aircraft_state_summary.png").string() << std::endl;
    } else {
        std::cout << "\n可视化失败，但MATLAB脚本已保存到: " << script_file << std::endl;
        std::cout << "您可以手动在MATLAB中运行该脚本" << std::endl;
    }
    
    return 0;
}
