#include<stdio.h>
#include<stdint.h>
#include<stdbool.h>
#include<string.h>
#include<stdlib.h>
#include "hal.h"
#include "Fat12.h"

uint32_t index_Root=0;
uint32_t SecofRoot=0;
uint32_t ResSector=0;
uint32_t FatCopies=0;
uint32_t RootDirec=0;
uint32_t TotalSec=0;
uint32_t SecPerFat=0;

static fatfs_read_boot_sector_t g_bootSector;
extern FILE*s_file;

static node_t * CreateNode(fatfs_read_entry_per_directory_t value)
{
    node_t *p = NULL;
    
    p=(node_t*)malloc(sizeof(node_t));
    p->Data=value;
    p->pNext=NULL;
    
    return p;
}

void create_List(list_t*l)
{
    l->pHead=l->pTail=NULL;
}
static void Add_List(list_t * l , node_t * p )
{
    if (l->pHead == NULL) 
    {
        l->pHead = p;
        l->pTail = p;
    }
    else
    {
        l->pTail->pNext = p;
        l->pTail = p;
    }
}

static void free_list(list_t* l)
{
    node_t* p = l->pHead;
    node_t* q = NULL;
    while (p != NULL) {
        q = p->pNext;
        free(p);
        p = q;
    }
    l->pTail = l->pHead;
    l->pHead = NULL;
}

bool Read_boot_sector(void)
{
    uint8_t buff[DEFAULT_SECTOR_SIZE];
    bool check_read = false;
    
    if(kmc_read_sector(0,buff))
    {
        g_bootSector.BytePerSector=(buff[12]<<8|buff[11]);
        g_bootSector.SecPerClu= buff[13];
        g_bootSector.ResSector=buff[15]<<8|buff[14];
        g_bootSector.FatCopies=buff[16];
        g_bootSector.RootDirec=buff[18]<<8|buff[17];
        g_bootSector.TotalSec=buff[20]<<8|buff[19];
        g_bootSector.SecPerFat=buff[23]<<8|buff[22];//9
        index_Root= g_bootSector.ResSector+(g_bootSector.SecPerFat* g_bootSector.FatCopies);//19
        SecofRoot=(g_bootSector.RootDirec * 32 / g_bootSector.BytePerSector);//14
        
        kmc_update_sector_size(g_bootSector.BytePerSector); 
        check_read = true;
    }
    
    return check_read;
}


void EnPerSector(uint32_t index_Sector,list_t*l)
{
    node_t *node = NULL;
    fatfs_read_entry_per_directory_t Entry;
    uint8_t temp[BYTE_PER_ENTRY_ROOT];
    uint32_t count=1;
    uint32_t size = g_bootSector.BytePerSector*g_bootSector.SecPerClu;
    uint8_t buff[size];
    uint32_t i=0;
    uint32_t flag = 0;
    uint32_t j=0;
    uint32_t count_byte=0;
    
    
    if(Read_boot_sector())
    {
        kmc_read_multi_sector(index_Sector*g_bootSector.SecPerClu,g_bootSector.SecPerClu, (uint8_t*)&buff[0]);
        for(i=0;i< size/BYTE_PER_ENTRY_ROOT;i++)
        {
            for(j=0;j<BYTE_PER_ENTRY_ROOT;j++)
            {
                temp[j]=buff[j + i*BYTE_PER_ENTRY_ROOT];
            }
            
            if(temp[11]!= 0x0F && temp[0]!=0x00)
            {
                
                for (count_byte=0;count_byte<8;count_byte++)
                {
                    Entry.filename[count_byte]= temp[count_byte];
                }
                
                for (count_byte=0;count_byte<3;count_byte++)
                {
                    Entry.fileType[count_byte]= temp[count_byte+8];
                }
                Entry.index=count++;
                Entry.attributes = temp[11];
                Entry.hours = (temp[15]>>3);
                Entry.minutes= ((temp[15]<<8| temp[14])>>5)& 0x1f;
                Entry.year= ((temp[17])>>1) + 1980;
                Entry.month=((temp[17]<<8| temp[16])>>5) & 0xf;
                Entry.date =((temp[17]<<8| temp[16])& 0x001F);
                Entry.firstCluster = temp[27]<<8 | temp[26];
                Entry.fileSize = ((temp[31]<<24)| (temp[30]<<8) |(temp[29]>>8) | (temp[28]>>24));
                
                node = CreateNode(Entry);
                Add_List (l,node);
            }
        }
    }
    
    
}

void read_folder (uint32_t first_cluster,list_t*l)
{
    node_t* p =l->pHead;
    uint32_t k=0;
    uint32_t index_start_data=0;
    
    free_list(l);
    
    index_start_data =(g_bootSector.SecPerFat* g_bootSector.FatCopies + g_bootSector.ResSector)+\
    (g_bootSector.RootDirec*32/g_bootSector.BytePerSector)-2 + first_cluster ;
    
    EnPerSector(index_start_data, l);
}

void read_root_directory(list_t* l)
{
    uint32_t i = 0;
    uint32_t index_last_root = 0;
    uint32_t start_index_Root = 0;
    
    start_index_Root = index_Root;
    index_last_root = index_Root + SecofRoot;
    
    free_list(l);
    while(start_index_Root != index_last_root) 
    { 
       EnPerSector(start_index_Root, l);
        start_index_Root = start_index_Root +1;
    }
}

static uint32_t cluster_next (uint32_t cluster )
{
	uint32_t value_cluster = 0;
	uint32_t index_byte_fat = 0;
	uint8_t buff[2];
	index_byte_fat = cluster*1.5 + g_bootSector.ResSector*g_bootSector.BytePerSector;
	fseek(s_file,index_byte_fat,SEEK_SET);
	fread(buff,1,2,s_file);
	if(cluster%2==0)
	{
		value_cluster = ((buff[1]& 0x0F)<<8)|buff[0];
	}
	else
		value_cluster = ((buff[0] & 0xF0)>>4)|(buff[1]<<4);
	return value_cluster;
}
void read_file(uint32_t firstCluster)
{
    uint32_t size = g_bootSector.BytePerSector * g_bootSector.SecPerClu ;
    uint8_t buff[size];
    uint32_t k = 0;
    uint32_t num_sector = g_bootSector.SecPerClu ;
    uint32_t cluster = firstCluster;
    uint32_t index_start_data=0;
    
    index_start_data =(g_bootSector.SecPerFat* g_bootSector.FatCopies + 1)+(g_bootSector.RootDirec*32\
    /g_bootSector.BytePerSector)/g_bootSector.SecPerClu-2 ;
    
    while (cluster != 0xFFF)
    {
        kmc_read_multi_sector((cluster + index_start_data)*g_bootSector.SecPerClu, num_sector, (uint8_t*)&buff[0]);
        for (k = 0; k < size; k++)
         {
                printf("%c", buff[k]);
         }
        cluster = cluster_next(cluster);
    }
}
