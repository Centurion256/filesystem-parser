
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fat16.h>


//----private defines----
#define DATELENGTH 11
#define TIMELENGTH 6
#define TYPELENGTH 6
#define NAMELENGTH 9
#define EXTLENGTH 5
 
// int print_entries(struct fat16_file* files, )
// {
//     for
// }


// int fat16_subdirectory_walk(, uint8_t depth)
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

void* fat16_file_read(void* dst, uint16_t start_cluster, FILE* fd){
    
}


int fat16_directory_walk(struct fat16_file* directory, uint16_t max_dirsize/*, size_t depth */)
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
uint8_t fat16_str_length(char* fat_str, uint8_t max_len)
{
    for (uint8_t i = 0; i < max_len; i++)
    {
        if (fat_str[i] == ' ')
        {
            return i;
        }
    }
}

//converts fat16 time into cstring of the form "HH:MM" military time
int fat16_time_to_cstr(uint16_t fat_time, char* buf)
{
    if (!buf)
    {
        return 1; //nullptr
    }
    //HHHH_HMMM_MMMS_SSSS
    uint8_t HOUR = (fat_time >> 12) & 0x1f;
    uint8_t MINUTE = (fat_time >> 5) & 0x3f;
    //SECONDS: not used

    sprintf(buf, "%.02d:%.02d", HOUR, MINUTE);
    return 0;
}

//converts fat16 date into cstring of the form "MM/DD/YYYY"
int fat16_date_to_cstr(uint16_t fat_time, char* buf)
{
    if (!buf)
    {
        return 1; //nullptr
    }
    //YYYY_YYYM_MMMD_DDDD
    uint16_t YEAR = (fat_time >> 9) & 0x7f;
    uint8_t MONTH = (fat_time >> 5) & 0xf;
    uint8_t DAY = (fat_time >> 0) & 0x1f;

    sprintf(buf, "%.02d/%.02d/%d", 1+MONTH, 1+DAY, 1980+YEAR);
    return 0;
}

//Display all files in the root directory. Non-recursive version of root_directory_walk 
int fat16_root_directory_display(struct fat16_file* directory, uint16_t max_dirsize)
{
    for (uint8_t i = 0; i < max_dirsize; i++)
    {
        struct fat16_file file = directory[i];
        if (file.fname[0] == '\0')
        {
            break; //End of directory
        }
        else if (file.fname[0] == FILE_DELETED)
        {
            continue;
        }

        char datebuf[DATELENGTH]; //cstring with date
        char timebuf[TIMELENGTH]; //csting with time
        char typebuf[TYPELENGTH] = "";
        char fnamebuf[NAMELENGTH] = "";
        char fextbuf[EXTLENGTH] = "";
        // char sizebuf[SIZELENGTH] = ;
        fat16_date_to_cstr(file.modification_date, datebuf);
        fat16_time_to_cstr(file.modification_time, timebuf);

        // printf("fattr %.02x ", file.fattributes);
        if ((file.fattributes >> DIRECTORY_OFFSET) & 0x01)
        {
            //TODO: add directory support
            strcpy(typebuf, "<DIR>");
            // printf("TODO: add directory support \n");
            // continue; //TODO: replace with read and recursive call
        }
        else if ((file.fattributes >> VOLUME_OFFSET) & 0x01)
        {
            // printf("TODO: handle labels ");
            strcpy(typebuf, "<LAB>");
        }

        //get filename into null-terminated cstring
        uint8_t name_len = fat16_str_length(file.fname, NAMELENGTH-1);
        memmove(fnamebuf, file.fname, name_len);
        fnamebuf[name_len] = '\0';

        uint8_t ext_len = fat16_str_length(file.fextension, EXTLENGTH-1);
        if (ext_len != 0)
        {
            fextbuf[0] = '.';
            memmove(fextbuf+1, file.fextension, ext_len);
            fextbuf[ext_len+1] = '\0';
        }
        
        printf("%.10s\t%.5s\t%.6s\t%9.d %s%s\n",datebuf, timebuf, typebuf, file.fsize, fnamebuf, fextbuf);

        // printf("Filesize: '%.04x' \n", file.fsize);
        // printf("");
    }
    return 0;
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
    printf("FS label: %.11s \nFS type: %.8s\n", boot.ext_bios_params.label, boot.ext_bios_params.fs_type);
    printf("Signature: '%.4x'\n", boot.ext_bios_params.boot_sector_signature);
    printf("Reserved sectors: %d\n", boot.bios_params.reserved_sector_count);
    printf("FAT tables: %d\n", boot.bios_params.fat_count);
    // fatptr
    
    size_t fat_start = 0 + boot.bios_params.bytes_per_sector * boot.bios_params.reserved_sector_count; //+ boot.bios_params.reserved_sector_count
    size_t fat_size = boot.bios_params.sectors_per_fat * boot.bios_params.bytes_per_sector;
    printf("FAT size in sectors: %d, in bytes: %d\n", boot.bios_params.sectors_per_fat, fat_size);
    struct fat16_entry* fat = (struct fat16_entry *)malloc(fat_size);
    fseek(fd, fat_start, SEEK_SET);

    fread(fat, 1, fat_size, fd);
    printf("First two FAT entries: '%.4x', '%.4x', Media type: '%.2x'\n", fat[0].current, fat[0].next, boot.bios_params.media_descriptor);
    
    size_t root_start = fat_start + (boot.bios_params.fat_count * boot.bios_params.sectors_per_fat * boot.bios_params.bytes_per_sector);
    size_t root_size = boot.bios_params.max_root_entries * sizeof(struct fat16_file);
    printf("Max root directory size in entries: %d, in bytes: %d\n", boot.bios_params.max_root_entries, root_size);
    struct fat16_file* root_dir = (struct fat16_file *)malloc(root_size);
    fseek(fd, root_start, SEEK_SET);
    fread(root_dir, 1, root_size, fd);

    size_t data_start = root_start + root_size - 2 * boot.bios_params.bytes_per_sector; //add 2 clusters to compensate for first two inaccessible fats

    // printf("Root directory start: %x; Root directory size: %x; Data region start %x \n", root_start, root_size, data_start);
    printf("Root directory:\n");
    fat16_root_directory_display(root_dir, boot.bios_params.max_root_entries);

    return 0;

    // for (uint8_t i = 0; i < boot.bios_params.max_root_entries; i++)
    // {
    //     struct fat16_file file = root_dir[i];
    //     if (file.fname[0] == '\0')
    //     {
    //         break;
    //     }
    //     printf("fattr %.02x ", file.fattributes);
    //     if ((file.fattributes >> DIRECTORY_OFFSET) & 0x01)
    //     {
    //         printf("TODO: add directory support \n");
    //         continue; //TODO: replace with read and recursive call
    //     }
    //     if ((file.fattributes >> VOLUME_OFFSET) & 0x01)
    //     {
    //         printf("TODO: handle labels ");    
    //     }

    //     printf("Filesize: '%.04x' \n", file.fsize);
    // }

       
}
