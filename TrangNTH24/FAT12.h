#ifndef _FAT12_H_
#define _FAT12_H_

#define BYTE_PER_ENTRY_ROOT 32
#define SWAP2BYTE (x)  (uint16_t)( (x[1]<<8)| x[0] )
#define SWAP4BYTE (x)  (uint32_t)( (x[3]<<24)|(uint32_t) (x[2]<<8) |(uint32_t)(x[1]>>8) | (uint32_t) (x[0]>>24) ) 

typedef struct 
{
    uint32_t BytePerSector;
    uint32_t SecPerClu;
    uint32_t ResSector;
    uint32_t FatCopies;
    uint32_t RootDirec;
    uint32_t TotalSec;
    uint32_t SecPerFat;
}fatfs_read_boot_sector_t;

typedef struct 
{
    uint8_t filename[8];
    uint8_t fileType[3];
    uint32_t index;
    uint32_t attributes ;
    uint32_t hours;
    uint32_t minutes;
    uint32_t date ;
    uint32_t month;
    uint32_t year;
    uint32_t fileSize;
    uint32_t firstCluster;   
} fatfs_read_entry_per_directory_t;

typedef struct Node {
    fatfs_read_entry_per_directory_t Data;
    struct Node* pNext;
} node_t;

typedef struct LIST {
    node_t* pHead;
    node_t* pTail;
} list_t;

bool Read_boot_sector(void);
void read_file(uint32_t firstCluster);
void read_root_directory(list_t* l);
void read_folder (uint32_t first_cluster,list_t*l);
void create_List(list_t*l);
//read_folder();

#endif
