#include <stdint.h>

// struct fat16_struct_t {
//     char        oem_name[8];
//     uint16_t    bytes_per_sector;
//     uint8_t     sectors_per_cluster;
//     uint16_t    reversed_sector_count;
//     uint8_t     num_fats;
//     uint16_t    root_entry_count;
//     uint32_t    sector_count;
//     uint8_t     media_descriptor;
//     uint16_t    fat_size; /* in sectors */
//     uint32_t    volume_id;
//     char        label[11];
//     char        fs_type[8];
// };
#define FAT_LABEL_LENGTH 11
#define FAT_FS_TYPE_LENGTH 8
#define FAT_OS_BOOT_CODE_LENGTH 448

#pragma pack(1)
struct fat_preamble {
  unsigned char jump_instruction[3];
  char oem_name[8]; /* e.g. "mkdosfs\0" or "BSD  4.4" */
};

struct fat_bios_parameter_block {
  uint16_t bytes_per_sector; /* commonly 512 */
  uint8_t sectors_per_cluster; /* powers of 2, 1 ~ 128 */
  uint16_t reserved_sector_count; /* sectors before first FAT */
  uint8_t fat_count; /* almost always 2, sometimes 1, rarely more. */
  uint16_t max_root_entries;
  uint16_t total_sectors; /* if 0, check total_sectors_large */
  unsigned char media_descriptor; /* 0xF8 is fixed disk. First byte of each FAT should match this. */
  uint16_t sectors_per_fat;
  uint16_t sectors_per_track; /* for disks with geometry */
  uint16_t head_count; /* for disks with geometry, e.g. 2 for double-sided floppy. */
  uint32_t sectors_before_partition; /* zero for non-partitined disk. */
  uint32_t total_sectors_large; /* only used if total_sectors == 0, value > 65535 */
};

struct fat16_extended_bios_parameter_block {
  unsigned char physical_drive_number; /* 0x00 = removable, 0x80 = hard disk */
  unsigned char current_head; /* reserved, Windows NT uses bits one and two as flags. */
  unsigned char extended_boot_signature; /* 0x29 indicates the following three entries exist. */
  uint32_t serial_number;
  char label[FAT_LABEL_LENGTH]; /* volume label, padded with spaces (0x20) */
  char fs_type[FAT_FS_TYPE_LENGTH]; /* padded with spaces (0x20) e.g "FAT16   " */
  unsigned char os_boot_code[FAT_OS_BOOT_CODE_LENGTH];
  uint16_t boot_sector_signature; /* hex 55AA */
};

struct fat16_boot_sector_t {
  struct fat_preamble preamble;
  struct fat_bios_parameter_block bios_params;
  struct fat16_extended_bios_parameter_block ext_bios_params;
};

struct fat16_entry {
  uint16_t current;
  uint16_t next;
};

struct fat16_file {
  char fname[8];
  char fextension[3];
  uint8_t fattributes;
  uint8_t __nt_reserved;
  uint8_t creation_milis;
  uint16_t creation_time;
  uint16_t creation_date;
  uint16_t last_access_date;
  uint16_t __fat32_reserved;
  uint16_t modification_time;
  uint16_t modification_date;
  uint16_t start_cluster;
  uint32_t fsize;
  
};

// struct fat16_file_metainfo {
//     // char 
// };

#define ACHIEVE_OFFSET 5
#define DIRECTORY_OFFSET 4
#define VOLUME_OFFSET 3
#define SYSTEM_OFFSET 2
#define HIDDEN_OFFSET 1
#define RDONLY_OFFSET 0

#pragma pack(push)