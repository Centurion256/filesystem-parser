
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fat16.h>
#include <vector.h>

//----private defines----
#define DATELENGTH 11
#define TIMELENGTH 6
#define TYPELENGTH 6
#define NAMELENGTH 9
#define EXTLENGTH 5
 

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

int fat16_root_directory_display(struct fat16_file* directory, uint16_t max_dirsize, uint8_t* dirname)
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
        
        printf("%.10s\t%.5s\t%.6s\t%9.d\t%.4x %s%s\n",datebuf, timebuf, typebuf, file.fsize, file.start_cluster, fnamebuf, fextbuf);

    }
    return 0;
}


struct dir_node{
    uint8_t name[NAMELENGTH];
    uint32_t start_cluster;
};

int __fat16_recursive_directory_display__inner(struct fat16_file* directory, uint16_t max_dirsize, vector dirs);

int fat16_recursive_directory_display(struct fat16_fs* fs, struct fat16_file* directory, uint16_t max_dirsize){
    vector dirs_to_visit = vector_new(sizeof(struct dir_node)*5);
    __fat16_recursive_directory_display__inner(directory, max_dirsize, dirs_to_visit);
    while(vector_size(dirs_to_visit)>0){
        struct dir_node* next_dir_node = vector_qpop(dirs_to_visit);
        struct fat16_file* next_dir_data = (struct fat16_file*)fat16_fs_read_file(fs, next_dir_node->start_cluster);
        printf("%s:\n", next_dir_node->name);
        
        __fat16_recursive_directory_display__inner(next_dir_data, fat16_fs_file_size(fs, next_dir_node->start_cluster), dirs_to_visit);
        free(next_dir_data);
        free(next_dir_node);
    }
    vector_free(dirs_to_visit);
}

int __fat16_recursive_directory_display__inner(struct fat16_file* directory, uint16_t max_dirsize, vector dirs)
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
        //get filename into null-terminated cstring
        uint8_t name_len = fat16_str_length(file.fname, NAMELENGTH-1);
        memmove(fnamebuf, file.fname, name_len);
        fnamebuf[name_len] = '\0';

        // printf("fattr %.02x ", file.fattributes);
        if ((file.fattributes >> DIRECTORY_OFFSET) & 0x01)
        {
            strcpy(typebuf, "<DIR>");
            if(fnamebuf[0]!='.')
            {
                struct dir_node* next_dir = malloc(sizeof(struct dir_node));
                memcpy(next_dir->name, fnamebuf, NAMELENGTH);
                next_dir->start_cluster = file.start_cluster;
                vector_push(dirs, next_dir);
            }
        }
        else if ((file.fattributes >> VOLUME_OFFSET) & 0x01)
        {
            strcpy(typebuf, "<LAB>");
        }


        uint8_t ext_len = fat16_str_length(file.fextension, EXTLENGTH-1);
        if (ext_len != 0)
        {
            fextbuf[0] = '.';
            memmove(fextbuf+1, file.fextension, ext_len);
            fextbuf[ext_len+1] = '\0';
        }
        
        printf("%.10s\t%.5s\t%.6s\t%9.d\t%.4x %s%s\n",datebuf, timebuf, typebuf, file.fsize, file.start_cluster, fnamebuf, fextbuf);

    }
    return 0;
}

int main(int argc, char** argv) {

    //TODO: rewrite with getopt
    if (argc != 2)
    {
        //Incorrect number of arguments
        fprintf(stderr, "fparse: Incorrect number of arguments\n");
        return 1;
    }

    FILE* fd = fopen(argv[1], "r"); // "hd0_just_FAT16_without_MBR.img"

    struct fat16_fs* fs = fat16_fs_new(fd);
    struct fat16_file* root_dir = fat16_fs_read_root(fs, 0);
    fat16_fs_print_info(fs);
    printf("Root directory:\n");
    fat16_recursive_directory_display(fs, root_dir, fs->boot.bios_params.max_root_entries);
    free(root_dir);


    fat16_fs_free(fs);

    return 0;

// Bytes per sector: 512 
// Sectors per cluster: 4
// FS label: PCDOS5      
// FS type: FAT16   
// Signature: 'aa55'
// Reserved sectors: 1
// FAT tables: 2
// FAT size in sectors: 34, in bytes: 17408
// First two FAT entries: 'fff8', 'ffff', Media type: 'f8'
// Max root directory size in entries: 512, in bytes: 16384

}
