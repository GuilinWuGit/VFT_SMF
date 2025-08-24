@echo off
chcp 65001 >nul
echo ========================================
echo VFT_SMF Gitee Open Source Setup Script
echo ========================================

echo.
echo Please follow these steps:
echo.
echo 1. First, create a repository on Gitee:
echo    - Visit https://gitee.com
echo    - Login to your account
echo    - Click the "+" in the top right, select "New Repository"
echo    - Repository name: VFT_SMF
echo    - Repository description: Virtual Flight Test Simulation and Modeling Framework
echo    - Repository type: Open Source
echo    - Language: C++
echo    - License: MIT License
echo    - Check "Initialize with README"
echo.
echo 2. After creation, please enter your Gitee username:
set /p gitee_username=Enter your Gitee username: 

echo.
echo 3. Configuring Git remote repository...

REM Show current remote repositories
echo Current remote repository configuration:
git remote -v

echo.
echo 4. Adding Gitee remote repository...
git remote add gitee https://gitee.com/%gitee_username%/VFT_SMF.git

echo.
echo 5. Pushing code to Gitee...
git push gitee main

echo.
echo 6. Verifying configuration...
git remote -v

echo.
echo ========================================
echo Setup Complete!
echo ========================================
echo.
echo Next steps:
echo 1. Visit https://gitee.com/%GuilinWu%/VFT_SMF to view your repository
echo 2. Configure project information on the repository page
echo 3. Add project tags and description
echo 4. Enable Issues and Wiki features
echo.
echo For detailed guide, see: docs/Gitee开源指南.md
echo.
pause
