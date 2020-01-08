#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include<stdint.h>
#include "HAL.h"
#include "FAT12.h"

static boot_sector_data_t  g_boot_sector_data_t;
uint32_t fat12_findnext_cluster(uint32_t index_cluster,uint32_t fat_name);

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
    fat12_read_bootsector_struct_t *bootsector_ptr = NULL;     /*  bootsector_ptr is a adapter */
    
    bootsector_ptr = (fat12_read_bootsector_struct_t *)malloc(sizeof(fat12_read_bootsector_struct_t));   /*  allocate memory for bootsector_ptr */
    
    if(kmc_read_sector(0,(uint8_t*)bootsector_ptr))   /* check when read a sector */
    {
        g_boot_sector_data_t.byte_per_sector = reverseByte(bootsector_ptr->bytepersector,2);
        g_boot_sector_data_t.num_fat = reverseByte(bootsector_ptr->fatCopy,1);
        g_boot_sector_data_t.num_root_entry = reverseByte(bootsector_ptr->numberrootEntry,2);
        g_boot_sector_data_t.sector_per_cluster = reverseByte(bootsector_ptr->sectorPerCluster,1);
        g_boot_sector_data_t.sector_per_fat = reverseByte(bootsector_ptr->sectorPerFAT,2);
        g_boot_sector_data_t.fat_number =  reverseByte(bootsector_ptr->fat_number_char,2);
        
        kmc_update_sector_size(g_boot_sector_data_t.byte_per_sector);   /* update byte per sector */
        
       // printf("%d",g_boot_sector_data_t.num_fat);
        check = true;
    }
    free(bootsector_ptr);     /* free memory when not use apdater */
       
    return check;
}
/*  create note of linked list */
static linkedList_ptr create_node(entry_directory_data_t *entryDirect)
{
    linkedList_ptr temp = NULL;
    
    temp = (linkedList_ptr)malloc(sizeof(entry_directory_data_t));
    temp->entry_direct = entryDirect;
    temp->next = NULL;
    
    return temp;
}
/* add note to linked list */
static linkedList_ptr add_node(linkedList_ptr head, entry_directory_data_t *entryDirec)
{
    linkedList_ptr temp = NULL;
    linkedList_ptr current = NULL;
    
    temp = create_node(entryDirec);
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
linkedList_ptr fat12_read_folder(uint32_t index_sector_direct, linkedList_ptr head)
{
    fat12_read_entryDirec_struct_t *entries_direct_ptr = NULL;   /* create a adapter to connect buff[32] and entry_data_save */
    entry_directory_data_t * entry_data_save_ptr = NULL;    /* create a value to save entry data for linked list */
    uint32_t i = 0;
    uint32_t j = 0;
    uint32_t size = g_boot_sector_data_t.byte_per_sector;   /*size of buff */
    uint8_t buff_sector_direct[size];
    uint8_t buff_entry_direct[FAP12_NB_PER_ENTRY_DIREC];    /* create a buff to copy entry from buff_sector_direct */
    uint8_t flag = 0;  /* create flag to know when empty entry */
    
    entries_direct_ptr = (fat12_read_entryDirec_struct_t *)malloc(sizeof(fat12_read_entryDirec_struct_t)); /* allocate memory for adapter*/
    fat_free_list(head);
    
    if(kmc_read_sector(index_sector_direct,(uint8_t*)&buff_sector_direct[0])) /* check read a sector */
    {
        for(i = 0; i< size/FAP12_NB_PER_ENTRY_DIREC; i++)  /* size/32 */
        {
            for(j=0; j< FAP12_NB_PER_ENTRY_DIREC; j++)
            {
                buff_entry_direct[j] = buff_sector_direct[j + i*FAP12_NB_PER_ENTRY_DIREC]; /* copy entry direct from buff_sector_direct to buff_entry_direct*/
            }
            if(buff_entry_direct[0] == 0x00)  /* check empty entry */
            {
                flag = flag + 1;
                free(entries_direct_ptr);   /* free memory of adapter */
                break;
            }
            else if(buff_entry_direct[11] != 0x0f )  /* check long file name */
            {
                printf("1 ");
                entry_data_save_ptr = (entry_directory_data_t*)malloc(sizeof(entry_directory_data_t)); /* allocate memory for entry_data_save_ptr */
                entries_direct_ptr = (fat12_read_entryDirec_struct_t *)&buff_entry_direct[0];
                printf("%x",entries_direct_ptr->atributes);
            }
            else
            {
                
            }
        } 
    }
    else
    {
        printf("read sector fail \n");
    }
    
}

linkedList_ptr fap12_read_entry_direct(uint32_t first_cluster,linkedList_ptr head)
{
    
    uint32_t index_sector_direct = 0;
    uint32_t number_cluster_direct = 0;
    uint32_t last_sector_direct = 0;
    
    number_cluster_direct = g_boot_sector_data_t.num_root_entry * FAP12_NB_PER_ENTRY_DIREC/ g_boot_sector_data_t.byte_per_sector;
    
    last_sector_direct = index_sector_direct + number_cluster_direct;
    //printf("%d jhfgh",index_sector_direct);
    if(0 == first_cluster)
    {
        index_sector_direct = g_boot_sector_data_t.num_fat*g_boot_sector_data_t.sector_per_fat + g_boot_sector_data_t.sector_per_cluster;
       // printf("%d jhfgh",index_sector_direct);
        //printf("hahahha");
       while(index_sector_direct != last_sector_direct  )
        {
            //printf("hahahaha \n");
            head = fat12_read_folder(index_sector_direct,head);
            index_sector_direct = index_sector_direct + 1;
       }
        
    }
    else if(0xfff == first_cluster)
    {
        printf("folder was null");
    }
    else
    {
        
    }
//    printf("%d",head);
//    
//    while(index_cluster_direct <= (index_cluster_direct + _number_root_entries))
//    {
//        entries_direct_ptr = (fat12_readentryDirec_struct_t *)malloc(sizeof(fat12_readentryDirec_struct_t));
//        
//        if(kmc_read_entries_directories(index_cluster_direct,(uint8_t*)entries_direct_ptr));
//        {
//            if(entries_direct_ptr->fileName[0] == 0)
//            {
//                break;
//            }
//            head = add_node(head,entries_direct_ptr);
//            index_cluster_direct = index_cluster_direct + 1;
//        }
//    }
    
    return head;
}
float fatnumberbyte_fat_entry(uint32_t fat_name)
{
    float numberbyte = 0;
    if(fat_name == 0x3231)
    {
        numberbyte = 1.5;
    }
    else if(fat_name ==0x3631 )
    {
        numberbyte = 2;
    }
    else
    {
        numberbyte = 4;
    }
    
    return numberbyte;
}
uint32_t fat12_findnext_cluster(uint32_t index_cluster,uint32_t fat_name)
{
    uint32_t number = 0;
    uint32_t nextindex_cluster = 0;
    uint32_t hold_number = 0;
    int i=0;
    uint8_t arr[2];
    
    if(fat_name == 0x3231)
    {
        
//        if(index_cluster %2 ==0)
//        {
//            number = index_cluster*1.5;
//        }
//        else
//        {
//            number = index_cluster*1.5-1;
//        }
//        kmc_read_threebyte_fat(number,&arr[0]);
//        hold_number = reverseByte(&arr[0],3);
//        
//        if(index_cluster %2 == 0)
//        {
//            nextindex_cluster = (hold_number & 0xfff);
//        }
//        else
//        { 
//            nextindex_cluster = (hold_number & 0xfff000) >> 12;
//        } 
    }
    else if(fat_name == 0x3631)
    {
        
    }
    else if(fat_name == 0x3233)
    {
        
    }
    else
    {
        printf("Wrong file");
    }
    
    return nextindex_cluster;
}
uint32_t fat12_read_file(uint32_t first_cluster)
{
    uint32_t next_cluster = 0;
    uint32_t size = g_boot_sector_data_t.byte_per_sector * g_boot_sector_data_t.sector_per_cluster;
    uint8_t arr[size];
    
    next_cluster = first_cluster;
    while(0xff != next_cluster)
    {
        if(kmc_read_multi_sector(next_cluster,g_boot_sector_data_t.sector_per_cluster,(uint8_t*)&arr[0]))
        {
            printf("%s",arr);
        }
        next_cluster = fat12_findnext_cluster(next_cluster,g_boot_sector_data_t.fat_number);
    }
}
