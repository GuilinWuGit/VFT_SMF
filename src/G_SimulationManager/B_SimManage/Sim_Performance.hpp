/*
 * @file Sim_Performance.hpp
 * @brief 仿真性能统计类
 * @author VFT_SMF Development Team
 * @date 2024
 * 
 * 仿真性能统计类负责：
 * 1. 记录程序开始和结束时间
 * 2. 计算程序总运行时间
 * 3. 计算仿真时间与真实时间的比例
 * 4. 输出性能统计信息到日志和控制台
 */

#pragma once

#include <chrono>
#include <string>
#include <iomanip>
#include <iostream>
#include "../LogAndData/Logger.hpp"

namespace VFT_SMF {
    namespace SimManage {

        /**
         * @brief 仿真性能统计类
         */
        class SimPerformance {
        private:
            std::chrono::high_resolution_clock::time_point start_time;
            std::chrono::high_resolution_clock::time_point end_time;
            bool is_started;
            bool is_finished;

        public:
            /**
             * @brief 构造函数
             */
            SimPerformance() : is_started(false), is_finished(false) {}

            /**
             * @brief 析构函数
             */
            ~SimPerformance() = default;

            /**
             * @brief 开始性能统计
             */
            void start() {
                start_time = std::chrono::high_resolution_clock::now();
                is_started = true;
                is_finished = false;
                
                if (VFT_SMF::globalLogger) {
                    VFT_SMF::globalLogger->info("性能统计已开始");
                }
            }

            /**
             * @brief 结束性能统计
             */
            void finish() {
                if (!is_started) {
                    if (VFT_SMF::globalLogger) {
                        VFT_SMF::globalLogger->warning("性能统计未开始，无法结束");
                    }
                    return;
                }
                
                end_time = std::chrono::high_resolution_clock::now();
                is_finished = true;
                
                if (VFT_SMF::globalLogger) {
                    VFT_SMF::globalLogger->info("性能统计已结束");
                }
            }

            /**
             * @brief 获取程序运行时间（毫秒）
             * @return 程序运行时间（毫秒）
             */
            long long getProgramDurationMs() const {
                if (!is_started || !is_finished) {
                    return 0;
                }
                
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
                return duration.count();
            }

            /**
             * @brief 获取程序运行时间（秒）
             * @return 程序运行时间（秒）
             */
            double getProgramDurationSeconds() const {
                return getProgramDurationMs() / 1000.0;
            }

            /**
             * @brief 计算仿真时间与真实时间的比例
             * @param simulation_time 仿真时间（秒）
             * @return 时间比例
             */
            double calculateTimeRatio(double simulation_time) const {
                double real_time = getProgramDurationSeconds();
                if (real_time <= 0.0) {
                    return 0.0;
                }
                return simulation_time / real_time;
            }

            /**
             * @brief 输出性能统计信息到日志
             * @param simulation_time 仿真时间（秒）
             * @param time_step 时间步长（秒）
             * @param total_steps 总时间步数
             * @param test_name 测试名称
             */
            void logPerformanceStats(double simulation_time, double time_step, uint64_t total_steps, 
                                   const std::string& test_name = "仿真测试") {
                if (!is_started || !is_finished) {
                    if (VFT_SMF::globalLogger) {
                        VFT_SMF::globalLogger->warning("性能统计未完成，无法输出统计信息");
                    }
                    return;
                }

                long long duration_ms = getProgramDurationMs();
                double duration_seconds = getProgramDurationSeconds();
                double time_ratio = calculateTimeRatio(simulation_time);

                // 输出测试完成信息
                VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "=== " + test_name + " 完成 ===");
                VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, 
                    "最终仿真时间: " + std::to_string(simulation_time) + " 秒");
                VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, 
                    "时间步长: " + std::to_string(time_step) + " 秒");
                VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, 
                    "总时间步数: " + std::to_string(total_steps));

                // 输出程序真实运行时间统计
                VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, "=== 程序真实运行时间统计 ===");
                VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, 
                    "程序总运行时间: " + std::to_string(duration_ms) + " 毫秒");
                VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, 
                    "程序总运行时间: " + std::to_string(duration_seconds) + " 秒");

                // 输出性能指标
                VFT_SMF::logBrief(VFT_SMF::LogLevel::Brief, 
                    "仿真时间/真实时间比例: " + std::to_string(time_ratio) + " (仿真时间比真实时间" + 
                    (time_ratio > 1.0 ? "快" : "慢") + ")");
            }

            /**
             * @brief 输出性能统计信息到控制台
             * @param simulation_time 仿真时间（秒）
             */
            void printPerformanceStats(double simulation_time) {
                if (!is_started || !is_finished) {
                    std::cout << "性能统计未完成，无法输出统计信息" << std::endl;
                    return;
                }

                long long duration_ms = getProgramDurationMs();
                double duration_seconds = getProgramDurationSeconds();
                double time_ratio = calculateTimeRatio(simulation_time);

                std::cout << "\n=== 程序真实运行时间统计 ===" << std::endl;
                std::cout << "程序总运行时间: " << duration_ms << " 毫秒" << std::endl;
                std::cout << "程序总运行时间: " << std::fixed << std::setprecision(3) << duration_seconds << " 秒" << std::endl;
                std::cout << "仿真时间/真实时间比例: " << std::fixed << std::setprecision(3) << time_ratio << std::endl;
            }

            /**
             * @brief 输出完整的性能统计信息（日志+控制台）
             * @param simulation_time 仿真时间（秒）
             * @param time_step 时间步长（秒）
             * @param total_steps 总时间步数
             * @param test_name 测试名称
             */
            void outputCompleteStats(double simulation_time, double time_step, uint64_t total_steps,
                                   const std::string& test_name = "仿真测试") {
                logPerformanceStats(simulation_time, time_step, total_steps, test_name);
                printPerformanceStats(simulation_time);
                // 控制台补充输出：仿真总时间
                std::cout << "仿真总时间: " << std::fixed << std::setprecision(2)
                          << simulation_time << " 秒" << std::endl;
            }

            /**
             * @brief 检查性能统计是否已完成
             * @return 是否已完成
             */
            bool isCompleted() const {
                return is_started && is_finished;
            }

            /**
             * @brief 重置性能统计
             */
            void reset() {
                is_started = false;
                is_finished = false;
            }
        };

    } // namespace SimManage
} // namespace VFT_SMF

