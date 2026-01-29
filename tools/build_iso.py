import sys
import struct
import os
import shutil

def make_iso(kernel_path, output_path):
    # Sector size
    SECTOR_SIZE = 2048
    
    # Read kernel
    with open(kernel_path, 'rb') as f:
        kernel_data = f.read()
    
    # Pad kernel to sector size
    kernel_sectors = (len(kernel_data) + SECTOR_SIZE - 1) // SECTOR_SIZE
    kernel_padded = kernel_data + b'\x00' * (kernel_sectors * SECTOR_SIZE - len(kernel_data))
    
    # Primary Volume Descriptor (PVD)
    pvd = bytearray(SECTOR_SIZE)
    pvd[0] = 1 # Type
    pvd[1:6] = b'CD001' # ID
    pvd[6] = 1 # Version
    pvd[40:72] = b'PARADOXOS'.ljust(32) # Volume ID
    
    # Root Directory Record
    root_dir = bytearray(34)
    root_dir[0] = 34 # Length
    root_dir[2] = 0 # Extent location (filled later)
    root_dir[6] = 0 # Data length (filled later)
    root_dir[25] = 2 # Flags (Directory)
    root_dir[32] = 1 # Name length
    root_dir[33] = 0 # Name (root)
    
    # Boot Record
    boot_record = bytearray(SECTOR_SIZE)
    boot_record[0] = 0 # Type (Boot Record)
    boot_record[1:6] = b'CD001'
    boot_record[6] = 1
    boot_record[7:39] = b'EL TORITO SPECIFICATION'.ljust(32)
    boot_record[71] = 0x13 # Pointer to catalog (filled later)
    
    # Boot Catalog
    catalog = bytearray(SECTOR_SIZE)
    catalog[0] = 1 # Header ID
    catalog[0x20] = 0x88 # Bootable header
    catalog[0x24] = 0 # Boot media type (no emulation)
    catalog[0x26] = 0 # Load segment
    catalog[0x28] = 0 # System type
    catalog[0x2A] = 1 # Sector count (1 virtual sector = 512 bytes?) No, load sector count
    catalog[0x2C] = 0 # Load LBA (filled later)
    
    # Write ISO
    with open(output_path, 'wb') as iso:
        # System Area (16 sectors)
        iso.write(b'\x00' * 16 * SECTOR_SIZE)
        
        # Calculate LBAs
        pvd_lba = 16
        boot_record_lba = 17
        terminator_lba = 18
        root_dir_lba = 19
        catalog_lba = 20
        kernel_lba = 21
        
        # Fill PVD Root Dir info
        struct.pack_into('<I', pvd, 158, root_dir_lba) # LBA (LE)
        struct.pack_into('>I', pvd, 162, root_dir_lba) # LBA (BE)
        struct.pack_into('<I', pvd, 166, SECTOR_SIZE) # Size (LE)
        struct.pack_into('>I', pvd, 170, SECTOR_SIZE) # Size (BE)
        pvd[156:190] = root_dir # Copy root dir record to PVD
        
        # Fill Boot Record
        struct.pack_into('<I', boot_record, 71, catalog_lba)
        
        # Fill Boot Catalog
        struct.pack_into('<H', catalog, 0x2A, kernel_sectors * 4) # 512-byte sectors
        struct.pack_into('<I', catalog, 0x2C, kernel_lba)
        
        # Write Descriptors
        iso.write(pvd)
        iso.write(boot_record)
        iso.write(b'\xFF' + b'\x00' * (SECTOR_SIZE - 1)) # Terminator
        
        # Write Root Directory (Basic)
        # Entry for Kernel
        kernel_entry = bytearray(34 + 12) # 12 char name "PARADOX.BIN"
        kernel_entry[0] = 46 # Length
        kernel_entry[25] = 0 # Flags (File)
        struct.pack_into('<I', kernel_entry, 2, kernel_lba) # Location
        struct.pack_into('>I', kernel_entry, 6, kernel_lba)
        struct.pack_into('<I', kernel_entry, 10, len(kernel_padded)) # Size
        struct.pack_into('>I', kernel_entry, 14, len(kernel_padded))
        kernel_entry[32] = 11 # Name Len
        kernel_entry[33:44] = b'PARADOX.BIN'
        kernel_entry[44:46] = b';1'
        
        # Self entry (.) and Parent (..) required? For simple boot, maybe not.
        iso.write(kernel_entry)
        iso.write(b'\x00' * (SECTOR_SIZE - 46))
        
        # Write Boot Catalog
        iso.write(catalog)
        
        # Write Kernel
        iso.write(kernel_padded)
        
    print(f"Success! ISO created at {output_path}")

try:
    make_iso('paradox.bin', 'paradox.iso')
except Exception as e:
    print(f"Error: {e}")
