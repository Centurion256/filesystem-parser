#include <stdint.h>
#include <stdio.h>
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

struct fat16_fs {
  FILE* fd;
  size_t sector_size;
  size_t sectors_per_cluster;
  size_t cluster_size;

  size_t fat_start;
  size_t fat_size;
  size_t root_start;
  size_t root_size;
  size_t data_start;

  struct fat16_boot_sector_t boot;
  uint16_t* fat;
  
};

struct fat16_fs* fat16_fs_new(FILE* fd);



struct fat16_fs* fat16_fs_new(FILE* fd)
{
  struct fat16_fs* self = malloc(sizeof(struct fat16_fs));
  self->fd = fd;
  fseek(fd, 0, SEEK_SET);
  fread(&(self->boot), 1, sizeof(self->boot), self->fd);
  self->sector_size = self->boot.bios_params.bytes_per_sector;
  self->sectors_per_cluster = self->boot.bios_params.sectors_per_cluster;
  self->cluster_size = self->sectors_per_cluster*self->sector_size;
  self->fat_start = 0 + self->boot.bios_params.reserved_sector_count * self->boot.bios_params.bytes_per_sector; //+ boot.bios_params.reserved_sector_count
  self->fat_size = self->boot.bios_params.sectors_per_fat * self->boot.bios_params.bytes_per_sector;
  self->fat = (uint16_t *)malloc(self->fat_size);
  fseek(self->fd, self->fat_start, SEEK_SET);
  fread(self->fat, 1, self->fat_size, self->fd);

  self->root_start = self->fat_start + (self->boot.bios_params.fat_count * self->boot.bios_params.sectors_per_fat * self->boot.bios_params.bytes_per_sector);
  self->root_size = self->boot.bios_params.max_root_entries * sizeof(struct fat16_file);



  self->data_start = self->root_start + self->root_size; //add 2 clusters to compensate for first two inaccessible fats
  return self;
}

int fat16_fs_free(struct fat16_fs * self){
  free(self->fat);
  fclose(self->fd);
  free(self);
}

void fat16_fs_read_cluster(struct fat16_fs * self, uint16_t cluster, void* dst){
  size_t offset = self->data_start+self->sector_size*self->sectors_per_cluster*(cluster-2);
  fseek(self->fd, offset, SEEK_SET);
  fread(dst, 1, self->sector_size * self->sectors_per_cluster, self->fd);
}


void fat16_fs_print_info(struct fat16_fs * self){
  struct fat16_boot_sector_t boot = self->boot;
  printf("Bytes per sector: %d \nSectors per cluster: %d\n", self->sector_size, self->sectors_per_cluster);
  printf("FS label: %.11s \nFS type: %.8s\n", boot.ext_bios_params.label, boot.ext_bios_params.fs_type);
  printf("Signature: '%.4x'\n", boot.ext_bios_params.boot_sector_signature);
  printf("Reserved sectors: %d\n", boot.bios_params.reserved_sector_count);
  printf("FAT tables: %d\n", boot.bios_params.fat_count);

  printf("FAT size in sectors: %d, in bytes: %d\n", boot.bios_params.sectors_per_fat, self->fat_size);

  printf("Max root directory size in entries: %d, in bytes: %d\n", boot.bios_params.max_root_entries, self->root_size);
  return;
}


struct fat16_file* fat16_fs_read_root(struct fat16_fs * self, struct fat16_file * dst){
  // struct fat16_file* root_dir;
  // if(dst){
  //   root_dir = dst;
  // }
  // else{
  //   root_dir = (struct fat16_file *)malloc(self->root_size);
  // }
  if(!dst)
  {
    dst = (struct fat16_file *)malloc(self->root_size);
  }
  fseek(self->fd, self->root_start, SEEK_SET);
  fread(dst, 1, self->root_size, self->fd);
  return dst;
}

size_t fat16_fs_file_size(struct fat16_fs * self, uint16_t start_cluster){
  uint16_t num_clusters = 0;
  uint16_t cur_cluster = start_cluster;
  while (cur_cluster > 0x0002 && cur_cluster < 0xfff7)
  {
    num_clusters++;
    cur_cluster = self->fat[cur_cluster];
  }
  return num_clusters*self->cluster_size;
}


uint8_t * fat16_fs_read_file(struct fat16_fs * self, uint16_t start_cluster){
  // TODO: Parse FAT to create vector of clusters
  // TODO: malloc enough space depending on cluster number
  // TODO: read each cluster
  size_t f_size = fat16_fs_file_size(self, start_cluster);

  uint8_t* dst = malloc(f_size + 1);
  uint16_t cur_cluster = start_cluster;
  for(uint8_t* c_ptr = dst; c_ptr-dst<f_size; c_ptr += self->cluster_size)
  {
    fat16_fs_read_cluster(self, cur_cluster, c_ptr);
    cur_cluster = self->fat[cur_cluster];    
  }
  dst[f_size] = '\0';
  return dst;
}

// struct fat16_file_metainfo {
//     // char 
// };

#define ACHIEVE_OFFSET 5
#define DIRECTORY_OFFSET 4
#define VOLUME_OFFSET 3
#define SYSTEM_OFFSET 2
#define HIDDEN_OFFSET 1
#define RDONLY_OFFSET 0

#define FILE_DELETED 0xef

#pragma pack(push)