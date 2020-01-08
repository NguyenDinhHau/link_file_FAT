#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include<stdint.h>
#include"HAL.h"

#define FILENAME "floppy.img"
FILE *file = NULL;
static uint32_t g_sector_per_cluster = FAP12_NUMBER_BYTE_PER_SECTOR;

/* function to init*/
bool fat_init(void)
{
    bool _check = false;
    file = fopen(FILENAME,"rb");
    if(file)
    {
        _check = true;
    }
    
    return _check;
}

void fat_deinit(void)
{
    fclose(file);
}
/* function to read a sector */
uint32_t kmc_read_sector(uint32_t index, uint8_t *buff)
{
    uint32_t _size_t = 0;
    
    if(!fseek(file,index*g_sector_per_cluster,SEEK_SET))  /* 512 are number bytes of a sector in fat12 */
    {  
        _size_t = fread(&buff[0],g_sector_per_cluster,1,file);
    }
    else
    {
        printf("Seek file fail! \n");
    }
    
    return _size_t*g_sector_per_cluster;
}
/* function to read multi sector */
uint32_t kmc_read_multi_sector(uint32_t index, uint32_t num, uint8_t *buff)
{
    uint32_t _size_t = 0;
    
    if(fseek(file,index*g_sector_per_cluster,SEEK_SET))
    {
        _size_t = fread(buff,g_sector_per_cluster*num,1,file);   /* 512 are number bytes of a sector in fat12 */
    }
    else
    {
        printf("Seek file fail! \n");
    }
    
    return FAP12_NUMBER_BYTE_PER_SECTOR*num*_size_t;
}
void kmc_update_sector_size(uint32_t _sector_size)
{
    if(0 == _sector_size % FAP12_NUMBER_BYTE_PER_SECTOR)
    {
        g_sector_per_cluster = _sector_size;
    }
    else
    {
        g_sector_per_cluster = _sector_size / FAP12_NUMBER_BYTE_PER_SECTOR;
    }
}
uint32_t kmc_read_entries_directories(uint32_t index, uint8_t *buff)
{
    uint32_t _size_t = 0;
    
    if(!fseek(file,index*FAP12_NB_PER_ENTRY_DIREC,SEEK_SET))
    {
        _size_t = fread(&buff[0],FAP12_NB_PER_ENTRY_DIREC,1,file);
    }
    else
    {
        printf("Seek file fail! \n");
    }
    
    return _size_t*FAP12_NB_PER_ENTRY_DIREC;
}
uint32_t kmc_read_threebyte_fat(uint32_t _index_byte, uint8_t *buff)
{
    uint32_t _size_t = 0;
    
    if(!fseek(file,FAP12_NUMBER_BYTE_PER_SECTOR+ _index_byte ,SEEK_SET))
    {
        _size_t = fread(&buff[0],3,1,file);
    }
    else
    {
        printf("Seek file fail! \n");
    }
    return _size_t*3;
}
