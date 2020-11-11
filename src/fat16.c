
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fat16.h>

// int print_entries(struct fat16_file* files, )
// {
//     for
// }

// int fat16_subdirectory_walk(struct fat16_file* directory, uint8_t depth)
// {
//     // if (!depth || depth < )

//     for (size_t i = 0;; i++)
//     {
//         struct fat16_file file = root_dir[i];
//         if (file.fname[0] == '\0')
//         {
//             break;
//         }

//         if ((file.fattributes >> DIRECTORY_OFFSET) & 0x01)
//         {
//             printf("TODO: add directory support \n");
//             fat16_subdirectory_walk(file, depth+1); //TODO: replace with read and recursive call
//         }

//     }
// }

int fat16_root_directory_walk(struct fat16_file* directory, uint16_t max_dirsize/*, size_t depth */)
{
    for (uint8_t i = 0; i < max_dirsize; i++)
    {
        struct fat16_file file = directory[i];
        if (file.fname[0] == '\0')
        {
            break;
        }
        printf("fattr %.02x ", file.fattributes);
        if ((file.fattributes >> DIRECTORY_OFFSET) & 0x01)
        {
            printf("TODO: add directory support \n");
            continue; //TODO: replace with read and recursive call
            //add ptr to queue... with depth++ and max_dirsize=cluster_size
        }
        if ((file.fattributes >> VOLUME_OFFSET) & 0x01)
        {
            printf("TODO: handle labels ");    
        }

        printf("Filesize: '%.04x' \n", file.fsize);
    }
    //if (depth)
    //{
        // handle read next cluster from fat and continue reading...
    //}
}

int fat16_root_directory_display(struct fat16_file* directory, uint16_t max_dirsize)
{
    for (uint8_t i = 0; i < max_dirsize; i++)
    {
        struct fat16_file file = directory[i];
        if (file.fname[0] == '\0')
        {
            break;
        }
        printf("");
    }
}

int main(int argc, char** argv) {

    //TODO: rewrite with getopt
    if (argc != 2)
    {
        //Incorrect number of arguments
        return 1;
    }

    struct fat16_boot_sector_t boot;
    FILE* fd = fopen(argv[1], "r"); // "hd0_just_FAT16_without_MBR.img"
    fread(&boot, 1, sizeof(boot), fd);
    // printf("Boot se size: %d\n", sizeof(boot));
    printf("Bytes per sector: %d \nSectors per cluster: %d\n", boot.bios_params.bytes_per_sector, boot.bios_params.sectors_per_cluster);
    printf("FS label: '%.11s' \nFS type: '%.8s'\n", boot.ext_bios_params.label, boot.ext_bios_params.fs_type);
    printf("Signature: '%.4x'\n", boot.ext_bios_params.boot_sector_signature);
    // fatptr
    
    size_t fat_start = 0 + boot.bios_params.bytes_per_sector * boot.bios_params.reserved_sector_count; //+ boot.bios_params.reserved_sector_count
    size_t fat_size = boot.bios_params.sectors_per_fat * boot.bios_params.bytes_per_sector;
    struct fat16_entry* fat = (struct fat16_entry *)malloc(fat_size);
    fseek(fd, fat_start, SEEK_SET);

    fread(fat, 1, fat_size, fd);
    printf("First two FAT entries: '%.4x', '%.4x', Media type: '%.2x'\n", fat[0].current, fat[0].next, boot.bios_params.media_descriptor);
    
    size_t root_start = fat_start + (boot.bios_params.fat_count * boot.bios_params.sectors_per_fat * boot.bios_params.bytes_per_sector);
    size_t root_size = boot.bios_params.max_root_entries * sizeof(struct fat16_file);
    struct fat16_file* root_dir = (struct fat16_file *)malloc(root_size);
    fseek(fd, root_start, SEEK_SET);
    fread(root_dir, 1, root_size, fd);

    size_t data_start = root_start + root_size - 2 * boot.bios_params.bytes_per_sector; //add 2 clusters to compensate for first two inaccessible fats

    printf("Root directory start: %x; Root directory size: %x; Data region start %x \n", root_start, root_size, data_start);
    

    for (uint8_t i = 0; i < boot.bios_params.max_root_entries; i++)
    {
        struct fat16_file file = root_dir[i];
        if (file.fname[0] == '\0')
        {
            break;
        }
        printf("fattr %.02x ", file.fattributes);
        if ((file.fattributes >> DIRECTORY_OFFSET) & 0x01)
        {
            printf("TODO: add directory support \n");
            continue; //TODO: replace with read and recursive call
        }
        if ((file.fattributes >> VOLUME_OFFSET) & 0x01)
        {
            printf("TODO: handle labels ");    
        }

        printf("Filesize: '%.04x' \n", file.fsize);
    }

       
}
