#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include<stdint.h>
#include"HAL.h"

extern uint32_t FAP12_NUMBER_BYTE_PER_SECTOR;

/* function to read a sector */
#define FILENAME "floppy.img"

FILE *file = NULL;

bool fat_init()
{
    bool _check = false;
    file = fopen(FILENAME,"rb");
    if(file)
    {
        _check = true;
    }
    
    return _check;
}

void fat_deinit()
{
    fclose(file);
}
/* function to read a sector */
uint32_t kmc_read_sector(uint32_t index, uint8_t *buff)
{
    uint32_t _size_t = 0;
    
    if(!fseek(file,index*FAP12_NUMBER_BYTE_PER_SECTOR,SEEK_SET))  /* 512 are number bytes of a sector in fat12 */
    {  
        _size_t = fread(&buff[0],FAP12_NUMBER_BYTE_PER_SECTOR,1,file);
    }
    else
    {
        printf("Seek file fail! \n");
    }
    
    return _size_t*FAP12_NUMBER_BYTE_PER_SECTOR;
}
/* function to read multi sector */
uint32_t kmc_read_multi_sector(uint32_t index, uint32_t num, uint8_t *buff)
{
    uint32_t _size_t = 0;
    
    if(fseek(file,index*FAP12_NUMBER_BYTE_PER_SECTOR,SEEK_SET))
    {
        _size_t = fread(buff,FAP12_NUMBER_BYTE_PER_SECTOR*num,1,file);   /* 512 are number bytes of a sector in fat12 */
    }
    else
    {
        printf("Seek file fail! \n");
    }
    
    return FAP12_NUMBER_BYTE_PER_SECTOR*num*_size_t;
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
