#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdint.h>
#include "HAL.h"
#include "FAT12.h"

uint32_t FAP12_NUMBER_BYTE_PER_SECTOR = 512;

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
fat12_read_bootsector_struct_t* fat12_read_bootsector()
{
    fat12_read_bootsector_struct_t *_bootsector_data = NULL;
    
    _bootsector_data = (fat12_read_bootsector_struct_t *)malloc(sizeof(fat12_read_bootsector_struct_t));
    kmc_read_sector(0,(uint8_t*)_bootsector_data);
    FAP12_NUMBER_BYTE_PER_SECTOR = reverseByte(_bootsector_data->bytepersector,2);
    
    return _bootsector_data;
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

linkedList_ptr fap12_read_entry_direct(linkedList_ptr head)
{
    fat12_read_bootsector_struct_t *_bootsector_data_ptr = NULL;
    fat12_read_entryDirec_struct_t *_entries_direct_ptr = NULL;
    uint32_t _index_direct = 0;
    uint32_t _nb_entries_direc = 0;
    uint32_t _number_fat12_copies = 0;
    uint32_t _sector_per_fat12 = 0;
    uint32_t _number_root_entries = 0;
    
    _bootsector_data_ptr = fat12_read_bootsector();
    
    _number_root_entries = reverseByte(_bootsector_data_ptr->numberrootEntry,2);
    _sector_per_fat12 = reverseByte(_bootsector_data_ptr->sectorPerFAT,2);
    _number_fat12_copies = reverseByte(_bootsector_data_ptr->fatCopy,1);
    _index_direct = (_number_fat12_copies * _sector_per_fat12 + 1) * FAP12_NUMBER_BYTE_PER_SECTOR / FAP12_NB_PER_ENTRY_DIREC;    /* return number entries */
    while(_index_direct <= (_index_direct + _number_root_entries))
    {
        _entries_direct_ptr = (fat12_read_entryDirec_struct_t *)malloc(sizeof(fat12_read_entryDirec_struct_t));
        
        if(kmc_read_entries_directories(_index_direct,(uint8_t*)_entries_direct_ptr));
        {
            if(_entries_direct_ptr->fileName[0] == 0)
            {
                break;
            }
            head = add_node(head,_entries_direct_ptr);
            _index_direct = _index_direct + 1;
        }
    }
    
    return head;
}

uint32_t fat12_read_next_fatIndex(uint32_t _index)
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
uint32_t fat12_read_file(uint32_t _index)
{
    
}
