@echo off
echo ================================================
echo   Running ParadoxOS in QEMU
echo ================================================

REM Use whichever QEMU you have
set QEMU=qemu-system-x86_64.exe

REM Check if QEMU is in PATH
where %QEMU% >nul 2>&1
if %errorlevel% neq 0 (
    echo QEMU not found in PATH.
    echo.
    echo Please install QEMU or add it to PATH
    echo Download from: https://www.qemu.org/download/
    pause
    exit /b 1
)

echo Running: %QEMU% -kernel paradox.bin -m 512
echo.

REM Run QEMU with the kernel
%QEMU% -kernel paradox.bin -m 512

echo.
echo QEMU closed.
pause
