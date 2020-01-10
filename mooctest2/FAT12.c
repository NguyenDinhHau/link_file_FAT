#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include<stdint.h>
#include "HAL.h"
#include "FAT12.h"

static boot_sector_data_t  g_boot_sector_data_t;
uint32_t fat12_findnext_cluster(uint32_t first_cluster,uint32_t fat_name);

/* convert to littel endian */
uint32_t reverseByte(uint8_t* byte,uint8_t _count)
{
    uint32_t result = 0;
    int i = 0;
    
    for (i = _count-1; i >= 0; i--)
    {
         result = (result << 8) | byte[i];
    }
    
    return result;
}
/*read boot sector*/ 
bool fat12_read_bootsector()
{
    bool check = false;
    uint32_t next = 0;
    fat12_read_bootsector_struct_t *bootsector_ptr = NULL;     /*  bootsector_ptr is a adapter */
    bootsector_ptr = (fat12_read_bootsector_struct_t *)malloc(sizeof(fat12_read_bootsector_struct_t));   /*  allocate memory for bootsector_ptr */
    
    if(kmc_read_sector(0,(uint8_t*)bootsector_ptr))   /* check when read a sector */
    {
        g_boot_sector_data_t.byte_per_sector = reverseByte(bootsector_ptr->bytepersector,2);
        g_boot_sector_data_t.num_fat = reverseByte(bootsector_ptr->fatCopy,1);
        g_boot_sector_data_t.reservedSector = reverseByte(bootsector_ptr->reservedSector,2);
        g_boot_sector_data_t.num_root_entry = reverseByte(bootsector_ptr->numberrootEntry,2);
        g_boot_sector_data_t.sector_per_cluster = reverseByte(bootsector_ptr->sectorPerCluster,1);
        g_boot_sector_data_t.sector_per_fat = reverseByte(bootsector_ptr->sectorPerFAT,2);
        g_boot_sector_data_t.fat_number =  reverseByte(bootsector_ptr->fat_number_char,2);
        
        kmc_update_sector_size(g_boot_sector_data_t.byte_per_sector);   /* update byte per sector */
        next =  fat12_findnext_cluster(1259,g_boot_sector_data_t.fat_number);
        //printf("%d",g_boot_sector_data_t.num_root_entry);
       // printf("%d",g_boot_sector_data_t.num_fat);
       // printf("%d   ",next);
        check = true;
    }
    free(bootsector_ptr);     /* free memory when not use apdater */
       
    return check;
}
/*  create note of linked list */
static linkedList_ptr create_node()
{
    linkedList_ptr temp = NULL;
    
    temp->entry_direct =(struct linked_list*) malloc(sizeof(struct linked_list));
    temp->next = NULL;
    return temp;
}
/* add note to linked list */
static linkedList_ptr add_node(linkedList_ptr *node)
{
    linkedList_ptr temp = NULL;
    linkedList_ptr current = NULL;
    
    temp =  node;
    if(head== NULL)
    {
        head = temp;
    }
    else
    {
        current = head;
        while(current->next != NULL)
        {
            current = current->next;
        }
        current->next = temp;
    }
    
    return head;
}
/* free list */
linkedList_ptr fat_free_list(linkedList_ptr head)
{
    linkedList_ptr temp = NULL;
    linkedList_ptr current = NULL;
    
    temp = head;
    while(temp != NULL)
    {
        current = temp->next;
        free(current);   /* free heap memory */
    }
    head = current;
    
    return head;
}
/* read folder */
static linkedList_ptr fat12_read_folder(uint32_t index_sector_direct, linkedList_ptr head)
{
    fat12_read_entryDirec_struct_t *entries_direct_ptr = NULL;   /* create a adapter to connect buff[32] and entry_data_save */
    entry_directory_data_t * entry_data_save_ptr = NULL;    /* create a value to save entry data for linked list */
    uint32_t i = 0;
    uint32_t j = 0;
    uint32_t size = g_boot_sector_data_t.byte_per_sector;   /*size of buff */
    uint8_t buff_sector_direct[size];
    uint8_t buff_entry_direct[FAP12_NB_PER_ENTRY_DIREC];    /* create a buff  32 to copy entry from buff_sector_direct */
    uint8_t flag = 0;  /* create flag to know when empty entry */
    uint8_t count = 0;
    
    entries_direct_ptr = (fat12_read_entryDirec_struct_t *)malloc(sizeof(fat12_read_entryDirec_struct_t)); /* allocate memory for adapter*/
    if(kmc_read_sector(index_sector_direct,(uint8_t*)&buff_sector_direct[0])) /* check read a sector */
    {
        for(i = 0; i< size/FAP12_NB_PER_ENTRY_DIREC; i++)  /* size/32 */
        {
            for(j=0; j< FAP12_NB_PER_ENTRY_DIREC; j++)
            {
                buff_entry_direct[j] = buff_sector_direct[j + i*FAP12_NB_PER_ENTRY_DIREC]; /* copy entry direct from buff_sector_direct to buff_entry_direct*/
            }
            if(buff_entry_direct[11] != 0x0f && buff_entry_direct[0] != 0x00 )  /* check long file name */
            {
                entry_data_save_ptr = (entry_directory_data_t*)malloc(sizeof(entry_directory_data_t)); /* allocate memory for entry_data_save_ptr */
                entries_direct_ptr = (fat12_read_entryDirec_struct_t *)&buff_entry_direct[0];
                for(count = 0; count <11;count++)
                {
                    entry_data_save_ptr->name[count] = entries_direct_ptr->fileName[count];
                    
                }
                entry_data_save_ptr->size = reverseByte(entries_direct_ptr->filesize,4);
                entry_data_save_ptr->hour = (reverseByte(entries_direct_ptr->creationTime,2)>>11);
                entry_data_save_ptr->minute = (reverseByte(entries_direct_ptr->creationTime,2)>>5)&0x3f;
                entry_data_save_ptr->year = ((reverseByte(entries_direct_ptr->lastWriteDate,2)>>9) + 1980);
                entry_data_save_ptr->month = (reverseByte(entries_direct_ptr->lastWriteDate,2)>>5) & 0x0f;
                entry_data_save_ptr->day = (reverseByte(entries_direct_ptr->lastWriteDate,2)&0x1f);
                entry_data_save_ptr->first_cluster = (reverseByte(entries_direct_ptr->firstLogiClu,2));
                entry_data_save_ptr->attribute = (reverseByte(entries_direct_ptr->atributes,1));
                head = add_node(head,entry_data_save_ptr);
            }
        } 
    }
    else
    {
        printf("read sector fail \n");
    }
    return head;
}

linkedList_ptr fap12_read_entry_direct(uint32_t first_cluster,linkedList_ptr head)
{
    uint32_t index_sector_direct = 0;
    uint32_t number_cluster_direct = 0;
    uint32_t last_sector_direct = 0;
    uint32_t index_data_sector = 0;
    uint32_t index_start_data = 0;
    
    index_start_data = (g_boot_sector_data_t.num_fat*g_boot_sector_data_t.sector_per_fat + 1)/g_boot_sector_data_t.sector_per_cluster;
    number_cluster_direct = g_boot_sector_data_t.num_root_entry * FAP12_NB_PER_ENTRY_DIREC/ g_boot_sector_data_t.byte_per_sector;
 
    //printf("%d jhfgh",index_sector_direct);
    if(0 == first_cluster)
    {
        index_sector_direct = g_boot_sector_data_t.num_fat*g_boot_sector_data_t.sector_per_fat + 1;
        last_sector_direct = index_sector_direct + number_cluster_direct;
       // printf("%d jhfgh",index_sector_direct);
        //printf("hahahha");
       
       while(index_sector_direct < last_sector_direct  )
        {
            //printf("hahahaha \n");
            head = fat12_read_folder(index_sector_direct,head);
            index_sector_direct = index_sector_direct + 1;
            //printf("%d",head);
       }
       
    }
    else if(0xfff == first_cluster)
    {
        printf("folder was null");
    }
    else
    {
        fat_free_list(head);
       // printf("sadjfaskdjfasd");
        index_data_sector = index_start_data + first_cluster + (g_boot_sector_data_t.num_root_entry*FAP12_NB_PER_ENTRY_DIREC/FAP12_NUMBER_BYTE_PER_SECTOR) -2;
        head =  fat12_read_folder(index_data_sector,head);
    }
   // printf("%d",head);
    return head;
}
float fat_numberbyte_fat_entry(uint32_t fat_name)
{
    float numberbyte = 0;
    if(fat_name == FAT12)
    {
        numberbyte = 1.5;
    }
    else if(fat_name ==FAT16 )
    {
        numberbyte = 2;
    }
    else
    {
        numberbyte = 4;
    }
    
    return numberbyte;
}
uint32_t fat12_findnext_cluster(uint32_t first_cluster,uint32_t fat_name)
{
    float number_byte = 0;
    uint32_t next_cluster = 0;
    uint32_t hold_number = 0;
    uint32_t temp = 0;
    uint32_t number_sector = 0;
    uint32_t size = g_boot_sector_data_t.byte_per_sector;
    uint8_t buff_one_sector[size];
    uint8_t buff_two_sector[2*size];
    uint32_t index_byte_fat = 0;
    uint32_t index_start_fat = 0;
    uint8_t arr[2];
    float result = 0;
    
    result = (first_cluster%2);
    
    number_byte = fat_numberbyte_fat_entry(g_boot_sector_data_t.fat_number);
    index_byte_fat = first_cluster*number_byte;
    index_start_fat = (g_boot_sector_data_t.byte_per_sector/FAP12_NUMBER_BYTE_PER_SECTOR);
    
    if(fat_name == FAT12)
    {
        if (index_byte_fat < g_boot_sector_data_t.byte_per_sector) {
            number_sector = 0;
        }
        else if (index_byte_fat % g_boot_sector_data_t.byte_per_sector == 0 && index_byte_fat >= g_boot_sector_data_t.byte_per_sector) {
            number_sector = index_byte_fat / g_boot_sector_data_t.byte_per_sector;
            index_byte_fat = 0;
        }
        else {
            temp = index_byte_fat;
            index_byte_fat = index_byte_fat % g_boot_sector_data_t.byte_per_sector;
            number_sector = temp / g_boot_sector_data_t.byte_per_sector;
        }
        if (first_cluster % 2 == 0 && index_byte_fat == g_boot_sector_data_t.byte_per_sector-1) {
            kmc_read_multi_sector(index_start_fat + number_sector, 2 * size, (uint8_t*)&buff_two_sector[0]);
            arr[0] = buff_two_sector[g_boot_sector_data_t.byte_per_sector-1];
            arr[1] = buff_two_sector[g_boot_sector_data_t.byte_per_sector];
            
            next_cluster = reverseByte((uint8_t*)&arr[0],2)&0x07;
        }
        else if (first_cluster % 2 != 0 && index_byte_fat == g_boot_sector_data_t.byte_per_sector-1) {
            kmc_read_multi_sector(index_start_fat+number_sector, 2 * size, (uint8_t*)&buff_two_sector[0]);
            arr[0] = buff_two_sector[g_boot_sector_data_t.byte_per_sector-1];
            arr[1] = buff_two_sector[g_boot_sector_data_t.byte_per_sector];
            
            next_cluster =(reverseByte((uint8_t*)&arr[0],2)&0x0e>>4) ;
        }
        else if (first_cluster % 2 == 0) {
            if(kmc_read_multi_sector(index_start_fat+ number_sector,size,(uint8_t*)&buff_one_sector[0]))
            {
                arr[0] = buff_one_sector[index_byte_fat];
                arr[1] = buff_one_sector[index_byte_fat+1];
                //printf("%X",buff_one_sector[4]);
                next_cluster =reverseByte((uint8_t*)&arr[0],2)&0x07 ;
            }
        }
        else if (first_cluster % 2 != 0) {
            kmc_read_multi_sector(index_start_fat+number_sector,size,(uint8_t*)&buff_one_sector[0] );
            arr[0] = buff_one_sector[index_byte_fat];
            arr[1] = buff_one_sector[index_byte_fat+1];
            
            next_cluster =(reverseByte((uint8_t*)&arr[0],2)&0x0e>>4) ;
        }
    }
    else if(fat_name == FAT16)
    {
        
    }
    else if(fat_name == FAT32)
    {
        
    }
    else
    {
        printf("Wrong file");
    }
    
    return next_cluster;
}
uint32_t fat12_read_file(uint32_t first_cluster)
{
    uint32_t index_start_data = 0;
    uint32_t size = g_boot_sector_data_t.byte_per_sector * g_boot_sector_data_t.sector_per_cluster;
    uint32_t next_cluster = 0;
    uint32_t i = 0;
    uint8_t arr[size];
    
    
    index_start_data = (g_boot_sector_data_t.num_fat*g_boot_sector_data_t.sector_per_fat + 1)/g_boot_sector_data_t.sector_per_cluster + (g_boot_sector_data_t.num_root_entry*FAP12_NB_PER_ENTRY_DIREC/FAP12_NUMBER_BYTE_PER_SECTOR) -2;
    next_cluster = first_cluster;
    while(0xff != next_cluster)
    {
        if(kmc_read_multi_sector((index_start_data + next_cluster)*g_boot_sector_data_t.sector_per_cluster,g_boot_sector_data_t.sector_per_cluster,(uint8_t*)&arr[0]))
        {
            
            for(i=0; i<size;i++)
            {
                printf("%c",arr[i]);
            }
            
        }
        next_cluster = fat12_findnext_cluster(next_cluster,g_boot_sector_data_t.fat_number);
    }
}

