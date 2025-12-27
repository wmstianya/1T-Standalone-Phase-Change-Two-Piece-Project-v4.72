# Git push script
$projectPath = $PSScriptRoot
Write-Host "Project path: $projectPath"
Set-Location $projectPath
git status
git add SYSTEM/error/
git add SYSTEM/system_control/system_control.c
git add USER/UART.uvprojx
git add openspec/
git status
git commit -m "refactor: extract error_handler module from system_control.c (Phase 1)"
git push origin main

