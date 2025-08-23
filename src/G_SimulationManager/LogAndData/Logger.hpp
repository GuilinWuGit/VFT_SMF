#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <memory>
#include <thread>
#include <windows.h>
#include <filesystem>

namespace VFT_SMF {

enum class LogLevel {
    Brief,
    Detail
};

class Logger {
private:
    std::ofstream log_brief_file;
    std::ofstream log_detail_file;
    std::mutex brief_mutex;
    std::mutex detail_mutex;
    bool console_output;
    
    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
        return ss.str();
    }
    
    std::string getThreadName() {
        // 获取当前线程名称，如果没有设置则返回线程ID
        DWORD threadId = GetCurrentThreadId();
        return "Thread-" + std::to_string(threadId);
    }
    
    std::string levelToString(LogLevel level) {
        switch (level) {
            case LogLevel::Brief: return "Brief";
            case LogLevel::Detail: return "Detail";
            default: return "UNKNOWN";
        }
    }
    
    void writeToFile(std::ofstream& file, std::mutex& mutex, 
                    LogLevel level, const std::string& message) {
        std::lock_guard<std::mutex> lock(mutex);
        if (file.is_open()) {
            file << "[" << getCurrentTimestamp() << "] "
                 << "[" << getThreadName() << "] "
                 << "[" << levelToString(level) << "] "
                 << message << std::endl;
            file.flush();
        }
    }
    
    void writeToConsole(LogLevel level, const std::string& message) {
        if (console_output) {
            std::lock_guard<std::mutex> lock(brief_mutex);
            std::cout << "[" << getCurrentTimestamp() << "] "
                     << "[" << getThreadName() << "] "
                     << "[" << levelToString(level) << "] "
                     << message << std::endl;
        }
    }

public:
    Logger() : console_output(true) {}
    
    ~Logger() {
        if (log_brief_file.is_open()) {
            log_brief_file.close();
        }
        if (log_detail_file.is_open()) {
            log_detail_file.close();
        }
    }
    
    bool initialize(const std::string& brief_filename = "log_brief.txt",
                   const std::string& detail_filename = "log_detail.txt",
                   bool enable_console = true) {
        console_output = enable_console;
        
        // 清理之前的日志文件
        clearLogFiles(brief_filename, detail_filename);
        
        // 打开brief日志文件
        log_brief_file.open(brief_filename, std::ios::app);
        if (!log_brief_file.is_open()) {
            std::cerr << "Failed to open brief log file: " << brief_filename << std::endl;
            return false;
        }
        
        // 打开detail日志文件
        log_detail_file.open(detail_filename, std::ios::app);
        if (!log_detail_file.is_open()) {
            std::cerr << "Failed to open detail log file: " << detail_filename << std::endl;
            log_brief_file.close();
            return false;
        }
        
        // 写入初始化日志
        logBrief(LogLevel::Brief, "Logger system initialized successfully");
        logDetail(LogLevel::Detail, "Logger system initialized with brief file: " + brief_filename + 
                                 ", detail file: " + detail_filename);
        
        return true;
    }
    
    // 写入brief日志（事件触发器、控制器驱动日志、高级流程、状态更新）
    // brief level的日志既输出到brief log又输出到detail log
    void logBrief(LogLevel level, const std::string& message) {
        writeToFile(log_brief_file, brief_mutex, level, message);
        writeToFile(log_detail_file, detail_mutex, level, message);  // 同时写入detail log
        writeToConsole(level, message);
    }
    
    // 写入detail日志（其他所有日志）
    void logDetail(LogLevel level, const std::string& message) {
        writeToFile(log_detail_file, detail_mutex, level, message);
    }
    
    // 便捷方法
    void debug(const std::string& message) { logDetail(LogLevel::Detail, message); }
    void info(const std::string& message) { logBrief(LogLevel::Brief, message); }
    void warning(const std::string& message) { logBrief(LogLevel::Brief, message); }
    void errorBrief(const std::string& message) { logBrief(LogLevel::Brief, message); }
    void errorDetail(const std::string& message) { logDetail(LogLevel::Detail, message); }
    void critical(const std::string& message) { logBrief(LogLevel::Brief, message); }
    
    // 设置控制台输出
    void setConsoleOutput(bool enable) {
        console_output = enable;
    }
    
    // 检查日志文件是否打开
    bool isInitialized() const {
        return log_brief_file.is_open() && log_detail_file.is_open();
    }
    
    // 清理日志文件
    void clearLogFiles(const std::string& brief_filename, const std::string& detail_filename) {
        try {
            if (std::filesystem::exists(brief_filename)) {
                std::filesystem::remove(brief_filename);
            }
            if (std::filesystem::exists(detail_filename)) {
                std::filesystem::remove(detail_filename);
            }
        } catch (const std::exception& e) {
            std::cerr << "清理日志文件时出错: " << e.what() << std::endl;
        }
    }
};

// 全局日志实例
inline std::unique_ptr<Logger> globalLogger = nullptr;

// 初始化全局日志
inline void initializeGlobalLogger(const std::string& brief_filename = "log_brief.txt",
                                 const std::string& detail_filename = "log_detail.txt",
                                 bool enable_console = true) {
    globalLogger = std::make_unique<Logger>();
    if (!globalLogger->initialize(brief_filename, detail_filename, enable_console)) {
        throw std::runtime_error("Failed to initialize global logger");
    }
}

// 便捷的全局日志函数
inline void logBrief(LogLevel level, const std::string& message) {
    if (globalLogger) {
        globalLogger->logBrief(level, message);
    }
}

inline void logDetail(LogLevel level, const std::string& message) {
    if (globalLogger) {
        globalLogger->logDetail(level, message);
    }
}

} // namespace VFT_SMF

#endif // LOGGER_HPP
