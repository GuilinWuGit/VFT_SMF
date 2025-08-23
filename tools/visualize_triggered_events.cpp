/**
 * @file visualize_triggered_events.cpp
 * @brief 事件触发数据可视化工具 - 通过调用MATLAB绘制散点图
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
#include <map>
#include <climits>

namespace fs = std::filesystem;

/**
 * @brief 事件数据结构
 */
struct EventData {
    double time;
    int eventId;
    std::string eventName;
    int stepNumber;
};

/**
 * @brief CSV数据解析器
 */
class CSVParser {
private:
    std::vector<std::string> headers;
    std::vector<EventData> events;
    std::map<std::string, int> eventNameToId;

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

        // 读取数据行
        int rowCount = 0;
        int eventIdCounter = 1;
        
        while (std::getline(file, line)) {
            auto values = splitFixedWidth(line);
            if (values.size() != headers.size()) {
                std::cout << "警告: 第 " << (rowCount + 1) << " 行列数不匹配，跳过" << std::endl;
                continue;
            }

            // 解析数据
            try {
                double time = std::stod(values[0]);
                int stepNumber = std::stoi(values[1]);
                int eventCount = std::stoi(values[2]);
                std::string eventList = values[3];

                // 如果有事件触发
                if (eventCount > 0 && eventList != "[]") {
                    // 解析事件列表
                    auto eventPairs = parseEventList(eventList);
                    for (const auto& eventPair : eventPairs) {
                        std::string eventName = eventPair.first;
                        int eventId = eventPair.second;
                        
                        // 记录事件名称到ID的映射
                        eventNameToId[eventName] = eventId;
                        
                        EventData event;
                        event.time = time;
                        event.eventId = eventId;
                        event.eventName = eventName;
                        event.stepNumber = stepNumber;
                        events.push_back(event);
                    }
                }
            } catch (...) {
                std::cout << "警告: 第 " << (rowCount + 1) << " 行数据解析失败，跳过" << std::endl;
            }
            
            rowCount++;
            
            // 显示前几行的解析结果
            if (rowCount <= 5) {
                std::cout << "第 " << rowCount << " 行解析结果:" << std::endl;
                for (size_t i = 0; i < values.size(); ++i) {
                    std::cout << "  列 " << i << " (" << headers[i] << "): '" << values[i] << "'" << std::endl;
                }
            }
        }

        std::cout << "成功读取 " << rowCount << " 行数据" << std::endl;
        std::cout << "发现 " << events.size() << " 个事件触发" << std::endl;
        std::cout << "共有 " << eventNameToId.size() << " 种不同的事件类型" << std::endl;
        
        // 显示事件类型统计
        std::cout << "事件类型统计:" << std::endl;
        for (const auto& pair : eventNameToId) {
            std::cout << "  事件ID " << pair.second << ": " << pair.first << std::endl;
        }
        
        return true;
    }

    const std::vector<EventData>& getEvents() const { return events; }
    const std::map<std::string, int>& getEventNameToId() const { return eventNameToId; }

private:
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

    std::vector<std::pair<std::string, int>> parseEventList(const std::string& eventList) {
        std::vector<std::pair<std::string, int>> events;
        
        // 移除方括号
        std::string content = eventList;
        if (content.length() >= 2 && content[0] == '[' && content[content.length()-1] == ']') {
            content = content.substr(1, content.length()-2);
        }
        
        // 如果为空，返回空向量
        if (content.empty() || content == "[]") {
            return events;
        }
        
        // 尝试解析JSON格式的事件
        if (content.find("'id':") != std::string::npos && content.find("'name':") != std::string::npos) {
            // 提取事件ID和名称
            size_t pos = 0;
            while ((pos = content.find("'id':", pos)) != std::string::npos) {
                // 提取ID
                pos += 5; // 跳过 "'id':"
                size_t idStart = content.find("'", pos);
                if (idStart != std::string::npos) {
                    idStart += 1;
                    size_t idEnd = content.find("'", idStart);
                    if (idEnd != std::string::npos) {
                        std::string idStr = content.substr(idStart, idEnd - idStart);
                        int eventId = std::stoi(idStr);
                        
                        // 查找对应的名称
                        size_t namePos = content.find("'name':", idEnd);
                        if (namePos != std::string::npos) {
                            namePos += 7; // 跳过 "'name':"
                            size_t nameStart = content.find("'", namePos);
                            if (nameStart != std::string::npos) {
                                nameStart += 1;
                                size_t nameEnd = content.find("'", nameStart);
                                if (nameEnd != std::string::npos) {
                                    std::string eventName = content.substr(nameStart, nameEnd - nameStart);
                                    events.push_back(std::make_pair(eventName, eventId));
                                    pos = nameEnd + 1;
                                } else {
                                    break;
                                }
                            } else {
                                break;
                            }
                        } else {
                            break;
                        }
                    } else {
                        break;
                    }
                } else {
                    break;
                }
            }
        } else {
            // 分割事件（假设用逗号分隔）
            std::stringstream ss(content);
            std::string event;
            int counter = 1;
            while (std::getline(ss, event, ',')) {
                // 去除引号和空格
                event.erase(0, event.find_first_not_of(" \"'"));
                event.erase(event.find_last_not_of(" \"'") + 1);
                if (!event.empty()) {
                    events.push_back(std::make_pair(event, counter++));
                }
            }
        }
        
        return events;
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
        script << "% 事件触发数据可视化脚本\n";
        script << "% 自动生成于 " << getCurrentTime() << "\n\n";
        
        const auto& events = parser.getEvents();
        const auto& eventNameToId = parser.getEventNameToId();
        
        if (events.empty()) {
            script << "% 没有发现事件触发\n";
            script << "figure('Position', [100, 100, 1200, 800]);\n";
            script << "text(0.5, 0.5, '没有发现事件触发', 'HorizontalAlignment', 'center', 'FontSize', 16);\n";
            script << "axis([0 1 0 1]);\n";
            script << "axis off;\n";
        } else {
            // 生成事件数据
            script << "% 生成事件数据\n";
            script << "event_times = [";
            for (size_t i = 0; i < events.size(); ++i) {
                if (i > 0) script << ", ";
                script << events[i].time;
            }
            script << "];\n";
            
            script << "event_ids = [";
            for (size_t i = 0; i < events.size(); ++i) {
                if (i > 0) script << ", ";
                script << events[i].eventId;
            }
            script << "];\n";
            
            script << "event_names = {";
            for (size_t i = 0; i < events.size(); ++i) {
                if (i > 0) script << ", ";
                script << "'" << events[i].eventName << "'";
            }
            script << "};\n\n";
            
            // 创建图形
            script << "% 创建图形\n";
            script << "figure('Position', [100, 100, 1400, 900]);\n\n";
            
            // 绘制散点图
            script << "% 绘制事件触发散点图\n";
            script << "scatter(event_times, event_ids, 100, 'filled', 'MarkerFaceColor', 'red', 'MarkerEdgeColor', 'black');\n";
            script << "hold on;\n\n";
            
            // 添加事件名称标注
            script << "% 添加事件名称标注\n";
            script << "for i = 1:length(event_times)\n";
            script << "    text(event_times(i), event_ids(i) + 0.1, event_names{i}, ...\n";
            script << "         'HorizontalAlignment', 'center', ...\n";
            script << "         'VerticalAlignment', 'bottom', ...\n";
            script << "         'FontSize', 10, ...\n";
            script << "         'Rotation', 0, ...\n";
            script << "         'Color', 'blue');\n";
            script << "end\n\n";
            
            // 设置坐标轴
            script << "% 设置坐标轴\n";
            script << "xlabel('仿真时间 (s)', 'FontSize', 12, 'FontWeight', 'bold');\n";
            script << "ylabel('事件ID', 'FontSize', 12, 'FontWeight', 'bold');\n";
            script << "grid on;\n";
            
            // 设置Y轴刻度
            script << "% 设置Y轴刻度\n";
            // 找到最小和最大的事件ID
            int minId = INT_MAX, maxId = INT_MIN;
            for (const auto& pair : eventNameToId) {
                if (pair.second < minId) minId = pair.second;
                if (pair.second > maxId) maxId = pair.second;
            }
            script << "yticks(" << minId << ":" << maxId << ");\n";
            // 使用数字作为Y轴标签，而不是事件名称
            script << "yticklabels(" << minId << ":" << maxId << ");\n";
            
            // 设置坐标轴范围，让坐标轴在(0,0)交叉
            script << "% 设置坐标轴范围\n";
            script << "xlim([0 max(event_times)]);\n";
            script << "ylim([0 " << (maxId + 1) << "]);\n";
            
            // 调整布局
            script << "% 调整布局\n";
            script << "set(gca, 'FontSize', 10);\n";
            script << "set(gcf, 'Color', 'white');\n";
            script << "hold off;\n\n";
        }
        
        // 保存图片
        std::string output_path = (fs::path(output_dir) / "triggered_events.png").string();
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
    std::cout << "事件触发数据可视化工具 (MATLAB版本)\n";
    std::cout << "========================================\n\n";
    
    // 检查命令行参数
    if (argc < 2) {
        std::cout << "用法: " << argv[0] << " <CSV文件路径> [输出目录]\n";
        std::cout << "示例: " << argv[0] << " ../ScenarioExamples/B737_Taxi/output/triggered_events.csv\n";
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
    std::string script_file = (fs::path(output_dir) / "plot_triggered_events.m").string();
    
    // 调用MATLAB
    std::cout << "调用MATLAB绘制图表..." << std::endl;
    if (MATLABCaller::callMATLAB(script_content, script_file)) {
        std::cout << "\n可视化完成!\n";
        std::cout << "输出文件:\n";
        std::cout << "- " << (fs::path(output_dir) / "triggered_events.png").string() << std::endl;
    } else {
        std::cout << "\n可视化失败，但MATLAB脚本已保存到: " << script_file << std::endl;
        std::cout << "您可以手动在MATLAB中运行该脚本" << std::endl;
    }
    
    return 0;
}
