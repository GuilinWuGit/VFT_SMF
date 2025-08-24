@echo off
echo ========================================
echo VFT_SMF Gitee开源设置脚本
echo ========================================

echo.
echo 请按照以下步骤操作：
echo.
echo 1. 首先在Gitee创建仓库：
echo    - 访问 https://gitee.com
echo    - 登录您的账号
echo    - 点击右上角"+"号，选择"新建仓库"
echo    - 仓库名称: VFT_SMF
echo    - 仓库介绍: 虚拟试飞仿真与建模框架 - 基于数字孪生和事件驱动的航空仿真平台
echo    - 仓库属性: 选择"开源"
echo    - 开发语言: C++
echo    - 开源协议: MIT License
echo    - 勾选"使用Readme文件初始化这个仓库"
echo.
echo 2. 创建完成后，请输入您的Gitee用户名：
set /p gitee_username=请输入您的Gitee用户名: 

echo.
echo 3. 正在配置Git远程仓库...

REM 查看当前远程仓库
echo 当前远程仓库配置：
git remote -v

echo.
echo 4. 添加Gitee远程仓库...
git remote add gitee https://gitee.com/%gitee_username%/VFT_SMF.git

echo.
echo 5. 推送代码到Gitee...
git push gitee main

echo.
echo 6. 验证配置...
git remote -v

echo.
echo ========================================
echo 设置完成！
echo ========================================
echo.
echo 接下来您可以：
echo 1. 访问 https://gitee.com/%gitee_username%/VFT_SMF 查看您的仓库
echo 2. 在仓库页面配置项目信息
echo 3. 添加项目标签和介绍
echo 4. 启用Issues和Wiki功能
echo.
echo 详细指南请查看: docs/Gitee开源指南.md
echo.
pause
