# VFT_SMF Gitee Open Source Setup Script
# PowerShell version to avoid encoding issues

Write-Host "========================================" -ForegroundColor Green
Write-Host "VFT_SMF Gitee Open Source Setup Script" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Green

Write-Host ""
Write-Host "Please follow these steps:" -ForegroundColor Yellow
Write-Host ""
Write-Host "1. First, create a repository on Gitee:" -ForegroundColor Cyan
Write-Host "   - Visit https://gitee.com" -ForegroundColor White
Write-Host "   - Login to your account" -ForegroundColor White
Write-Host "   - Click the '+' in the top right, select 'New Repository'" -ForegroundColor White
Write-Host "   - Repository name: VFT_SMF" -ForegroundColor White
Write-Host "   - Repository description: Virtual Flight Test Simulation and Modeling Framework" -ForegroundColor White
Write-Host "   - Repository type: Open Source" -ForegroundColor White
Write-Host "   - Language: C++" -ForegroundColor White
Write-Host "   - License: MIT License" -ForegroundColor White
Write-Host "   - Check 'Initialize with README'" -ForegroundColor White

Write-Host ""
Write-Host "2. After creation, please enter your Gitee username:" -ForegroundColor Cyan
$gitee_username = Read-Host "Enter your Gitee username"

Write-Host ""
Write-Host "3. Configuring Git remote repository..." -ForegroundColor Yellow

# Show current remote repositories
Write-Host "Current remote repository configuration:" -ForegroundColor Cyan
git remote -v

Write-Host ""
Write-Host "4. Adding Gitee remote repository..." -ForegroundColor Yellow
git remote add gitee "https://gitee.com/$gitee_username/VFT_SMF.git"

Write-Host ""
Write-Host "5. Pushing code to Gitee..." -ForegroundColor Yellow
git push gitee main

Write-Host ""
Write-Host "6. Verifying configuration..." -ForegroundColor Yellow
git remote -v

Write-Host ""
Write-Host "========================================" -ForegroundColor Green
Write-Host "Setup Complete!" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Green

Write-Host ""
Write-Host "Next steps:" -ForegroundColor Yellow
Write-Host "1. Visit https://gitee.com/$gitee_username/VFT_SMF to view your repository" -ForegroundColor White
Write-Host "2. Configure project information on the repository page" -ForegroundColor White
Write-Host "3. Add project tags and description" -ForegroundColor White
Write-Host "4. Enable Issues and Wiki features" -ForegroundColor White
Write-Host ""
Write-Host "For detailed guide, see: docs/Gitee开源指南.md" -ForegroundColor Cyan

Write-Host ""
Write-Host "Press any key to continue..." -ForegroundColor Gray
$null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")
