@echo off
chcp 65001 > nul
cd /d "%~dp0"
echo === Git Status ===
git status
echo.
echo === Adding files ===
git add SYSTEM/error/
git add SYSTEM/system_control/system_control.c
git add USER/UART.uvprojx
git add openspec/
echo.
echo === Git Status After Add ===
git status
echo.
echo === Committing ===
git commit -m "refactor: extract error_handler module from system_control.c (Phase 1)"
echo.
echo === Pushing to origin ===
git push origin main
echo.
echo === Done ===
pause

