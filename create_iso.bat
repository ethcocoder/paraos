@echo off
echo ================================================
echo   Creating ParadoxOS ISO with GRUB
echo ================================================

REM Create ISO structure
if not exist iso\boot\grub mkdir iso\boot\grub

REM Copy kernel
copy paradox.bin iso\boot\paradox.bin

REM Create GRUB config
echo set timeout=0 > iso\boot\grub\grub.cfg
echo set default=0 >> iso\boot\grub\grub.cfg
echo. >> iso\boot\grub\grub.cfg
echo menuentry "ParadoxOS - Observer Birth" { >> iso\boot\grub\grub.cfg
echo     multiboot /boot/paradox.bin >> iso\boot\grub\grub.cfg
echo     boot >> iso\boot\grub\grub.cfg
echo } >> iso\boot\grub\grub.cfg

echo ISO structure created in iso\

echo.
echo To create bootable ISO, you need grub-mkrescue:
echo   grub-mkrescue -o paradox.iso iso
echo.
echo Or test with direct QEMU framebuffer:
echo   qemu-system-i386 -serial stdio paradox.bin

pause
