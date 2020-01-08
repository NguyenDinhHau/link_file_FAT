#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include<stdint.h>
#include "HAL.h"
#include "FAT12.h"

static boot_sector_data_t  g_boot_sector_data_t;
static root_directory_data_t  g_root_directory_data_t;

/* convert to littel endian */
uint32_t reverseByte(uint8_t* _byte,uint8_t _count)
{
    uint32_t _result = 0;
    int i = 0;
    
    for (i = _count-1; i >= 0; i--)
    {
         _result = (_result << 8) | _byte[i];
    }
    
    return _result;
}
/*read boot sector*/
bool fat12_read_bootsector()
{
    bool _check = false;
    fat12_read_bootsector_struct_t *_bootsector_ptr = NULL;
    
    _bootsector_ptr = (fat12_read_bootsector_struct_t *)malloc(sizeof(fat12_read_bootsector_struct_t));
    
    if(kmc_read_sector(0,(uint8_t*)_bootsector_ptr))
    {
        g_boot_sector_data_t.byte_per_sector = reverseByte(_bootsector_ptr->bytepersector,2);
        g_boot_sector_data_t.num_fat = reverseByte(_bootsector_ptr->fatCopy,2);
        g_boot_sector_data_t.num_root_entry = reverseByte(_bootsector_ptr->numberrootEntry,2);
        g_boot_sector_data_t.sector_per_cluster = reverseByte(_bootsector_ptr->sectorPerCluster,1);
        g_boot_sector_data_t.sector_per_fat = reverseByte(_bootsector_ptr->sectorPerFAT,2);
        
        kmc_update_sector_size(g_boot_sector_data_t.byte_per_sector);
        _check = true;
    }
    free(_bootsector_ptr);
    return _check;
}
/*  create note of linked list */
static linkedList_ptr create_node(fat12_read_entryDirec_struct_t *_entryDirect)
{
    linkedList_ptr _temp = NULL;
    
    _temp = (linkedList_ptr)malloc(sizeof(struct linked_list));
    _temp->entry_direct = _entryDirect;
    _temp->next = NULL;
    
    return _temp;
}
/* add note to linked list */

static linkedList_ptr add_node(linkedList_ptr _head, fat12_read_entryDirec_struct_t *_entryDirec)
{
    linkedList_ptr _temp = NULL;
    linkedList_ptr _current = NULL;
    
    _temp = create_node(_entryDirec);
    if(_head== NULL)
    {
        _head = _temp;
    }
    else
    {
        _current = _head;
        while(_current->next != NULL)
        {
            _current = _current->next;
        }
        _current->next = _temp;
    }
    
    return _head;
}

linkedList_ptr fap12_read_entry_direct(uint32_t _first_cluster,linkedList_ptr head)
{
    fat12_read_entryDirec_struct_t *_entries_direct_ptr = NULL;
    uint32_t _number_clustet_direct = 0;
    
    _number_clustet_direct = g_boot_sector_data_t.num_root_entry * FAP12_NB_PER_ENTRY_DIREC/ g_boot_sector_data_t.byte_per_sector;
    if(0 == _first_cluster)
    {
        while()
        if(kmc_read_sector())
    }
    else if(0xfff == _first_cluster)
    {
        printf("folder was null");
    }
    else
    {
        
    }
    
    
    _head_ptr = (linkedList_ptr)malloc(sizeof(struct linked_list));
    
    while(_index_direct <= (_index_direct + _number_root_entries))
    {
        _entries_direct_ptr = (fat12_read_entryDirec_struct_t *)malloc(sizeof(fat12_read_entryDirec_struct_t));
        
        if(kmc_read_entries_directories(_index_direct,(uint8_t*)_entries_direct_ptr));
        {
            if(_entries_direct_ptr->fileName[0] == 0)
            {
                break;
            }
            _head_ptr = add_node(head,_entries_direct_ptr);
            _index_direct = _index_direct + 1;
        }
    }
    
    return _head_ptr;
}
float fat_numberByte_fat_entry(uint8_t *_fat_name)
{
    float _number_byte = 0;
    if(_fat_name == 'FAT12')
    {
        _number_byte = 1.5;
    }
    else if(_fat_name == 'FAT16')
    {
        _number_byte = 2;
    }
    else
    {
        _number_byte = 4;
    }
}
uint32_t fat12_find_next_cluster(uint32_t _index)
{
    uint32_t number = 0;
    uint32_t _next_index = 0;
    uint32_t _hold_number = 0;
    int i=0;
    uint8_t arr[3];
    
    if(_index %2 ==0)
    {
        number = _index*1.5;
    }
    else
    {
        number = _index*1.5-1;
    }
    kmc_read_threebyte_fat(number,&arr[0]);
    _hold_number = reverseByte(&arr[0],3);
    if(_index %2 == 0)
    {
        _next_index = (_hold_number & 0xfff);
    }
    else
    { 
        _next_index = (_hold_number & 0xfff000) >> 12;
    }
    
    return _next_index;
}
uint32_t fat12_read_file(uint32_t _first_cluster)
{
    uint32_t _next_cluster = 0;
    uint32_t _size = g_boot_sector_data_t.byte_per_sector * g_boot_sector_data_t.sector_per_cluster;
    uint8_t arr[_size];
    
    _next_cluster = _first_cluster;
    while(0xff != _next_cluster)
    {
        if(kmc_read_multi_sector(_next_cluster,g_boot_sector_data_t.sector_per_cluster,(uint8_t*)&arr[0]))
        {
            printf("%s",arr);
        }
        _next_cluster = fat12_find_next_cluster(_next_cluster);
    }
}
