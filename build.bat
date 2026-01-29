@echo off
setlocal
echo ==================================================
echo   ParadoxOS Build System - Graphical Edition
echo ==================================================

REM --- Configuration ---
set QEMU_PATH="C:\Program Files\qemu\qemu-system-x86_64.exe"
set KERNEL_PE=kernel.pe
set KERNEL_BIN=paradox.bin

REM --- Step 1: Clean ---
echo [1/6] Cleaning...
if exist %KERNEL_BIN% del %KERNEL_BIN%
if exist %KERNEL_PE% del %KERNEL_PE%
for /R src %%f in (*.o) do del "%%f"

REM --- Step 2: Compile Assembly Components ---
echo [2/6] Compiling Assembly...
gcc -m32 -c src/boot/boot.S -o src/boot/boot.o
gcc -m32 -c src/boot/gdt_flush.S -o src/boot/gdt_flush.o
gcc -m32 -c src/boot/interrupts.S -o src/boot/interrupts.o
if %errorlevel% neq 0 exit /b %errorlevel%

REM --- Step 3: Compile Core Kernel ---
echo [3/6] Compiling Kernel Core...
gcc -m32 -std=gnu99 -ffreestanding -O2 -Wall -Wextra -c src/kernel/kernel.c -o src/kernel/kernel.o
gcc -m32 -std=gnu99 -ffreestanding -O2 -Wall -Wextra -c src/kernel/field.c -o src/kernel/field.o
gcc -m32 -c src/kernel/gdt.c -o src/kernel/gdt.o
gcc -m32 -c src/kernel/idt.c -o src/kernel/idt.o
gcc -m32 -c src/kernel/pic.c -o src/kernel/pic.o
if %errorlevel% neq 0 exit /b %errorlevel%

REM --- Step 4: Compile Graphics & GUI ---
echo [4/6] Compiling Graphics Engine...
gcc -m32 -std=gnu99 -ffreestanding -O2 -Wall -Wextra -c src/kernel/graphics.c -o src/kernel/graphics.o
gcc -m32 -std=gnu99 -ffreestanding -O2 -Wall -Wextra -c src/kernel/universe.c -o src/kernel/universe.o
gcc -m32 -std=gnu99 -ffreestanding -O2 -Wall -Wextra -c src/kernel/gui.c -o src/kernel/gui.o
if %errorlevel% neq 0 exit /b %errorlevel%

REM --- Step 5: Link ---
echo [5/6] Linking...
ld -mi386pe -T linker.ld -o %KERNEL_PE% ^
    src/boot/boot.o src/boot/gdt_flush.o src/boot/interrupts.o ^
    src/kernel/kernel.o src/kernel/field.o src/kernel/gdt.o src/kernel/idt.o src/kernel/pic.o ^
    src/kernel/graphics.o src/kernel/universe.o src/kernel/gui.o
if %errorlevel% neq 0 exit /b %errorlevel%

REM --- Step 6: Convert to Binary ---
echo [6/6] Creating Binary...
objcopy -O binary %KERNEL_PE% %KERNEL_BIN%
if %errorlevel% neq 0 exit /b %errorlevel%

echo ==================================================
echo [SUCCESS] ParadoxOS Build Complete!
echo Binary: %KERNEL_BIN%
echo ==================================================

REM --- Launch QEMU ---
echo Launching QEMU...
if exist %QEMU_PATH% (
    REM Use -kernel for Multiboot loading
    %QEMU_PATH% -kernel %KERNEL_BIN% -m 512 -vga std
) else (
    echo [INFO] QEMU not found at %QEMU_PATH%
    echo Please run: qemu-system-x86_64 -kernel paradox.bin -m 512
)

endlocal
