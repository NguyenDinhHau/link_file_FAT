#ifndef _HEADER_FILENAME_FAT
#define _HEADER_FILENAME_FAT

#define SWAPE_2BYTE(x) (uint16_t) (x[1] >> 8) | x[0]
#define SWAPE_4BYTE(x) (uint32_t) (x[3] >> 24) | (uint32_t) (x[2] >> 8) | (uint32_t) (x[1] << 8) | (uint32_t) (x[0] << 24)

/* Create a struct to save element of bootSector */
typedef struct
{
    uint8_t bootstrapJump[3];		/* 0-2		Code to jump to bootstrap */
    uint8_t oem[8];					/* 3-10		OEM name/version */
	uint8_t bytepersector[2];		/* 11-12	Byte per sector */
    uint8_t sectorPerCluster[1];		/* 13		Sector per cluster */
    uint8_t reservedSector[2];		/* 14-15	Number of reserved sectors */
	uint8_t fatCopy[1];				/* 16		Number of FAT copies */
	uint8_t numberrootEntry[2];			/* 17-18	Number of root directory entries */
	uint8_t sectorInFS[2];			/* 19-20	Total number of sector in the filesystem */
	uint8_t mediaType;				/* 21		Media descriptor type */
	uint8_t sectorPerFAT[2];		/* 22-23	Sector per FAT, 0 for FAT32 */
	uint8_t sectorPerTrack[2];		/* 24-25	Sector per track */
    uint8_t head[2];				/* 26-27	Number of heads */
    uint8_t hiddenSector[4];        /* 28-31	Number of hidden sectors */
    uint8_t largeOfSector[4];       /* 32 - 35 */
    uint8_t driveNumber;            /* 36  */
    uint8_t reserved;               /* 37 */
    uint8_t extendedBootSignature;  /* 38 */
    uint8_t volumeSerialNum[4];     /* 39-42 */
    uint8_t volumeLable[11];        /* 43-53 */
    uint8_t fileSystemType[3];      /* 54 - 61 */ 
    uint8_t fat_number_char[2];
    uint8_t extentfile[3];
    uint8_t bootstrap[448];			/* 62-509	Bootstrap code */
    uint8_t signature[2];			/* 510-511	Signature 0x55 0xaa */
} fat12_read_bootsector_struct_t;
/* save main elements to this*/
typedef struct
    {
    uint32_t num_root_entry;
    uint32_t byte_per_sector;
    uint32_t sector_per_cluster;
    uint32_t num_fat;
    uint32_t sector_per_fat;
    uint32_t fat_number;
    
} boot_sector_data_t;

/* Create a struct to save elemement of Directories entry */
typedef struct
{
    uint8_t fileName[11];						
	uint8_t atributes;		
    uint8_t reservedOfWindow;	
    uint8_t createMillStamp;		
	uint8_t creationTime[2];			
	uint8_t creationDate[2];		
	uint8_t lastAccessDate[2];			
	uint8_t reservedForFat[2];			
	uint8_t lastWriteTime[2];	
	uint8_t lastWriteDate[2];	
    uint8_t firstLogiClu[2];			
    uint8_t filesize[4];          
} fat12_read_entryDirec_struct_t;

typedef struct
{
    uint8_t name[11];
    uint32_t size;
    uint32_t year;
    uint32_t day;
    uint32_t month;
    uint32_t minute;
    uint32_t hour;
    uint32_t first_cluster;
    uint8_t attribute;
    uint32_t index;    
} entry_directory_data_t;

typedef struct
{
   uint8_t sequenceNo;            // 0 Sequence number, 0xe5 for deleted entry
   uint8_t fileName_Part1[10];    // 1 -> 10 file name part
   uint8_t fileattribute;         // 11 File attibute
   uint8_t reserved_1;            // 12
   uint8_t checksum;              // 13 Checksum
   uint8_t fileName_Part2[12];    // 14 - 25 WORD reserved_2;
   uint8_t reserved_2[2];         // 26 - 27 reverved 2
   uint8_t fileName_Part3[4];     // 28 - 31 file name charaters
}fat12_lfn_entry_direct;

struct linked_list
{
    entry_directory_data_t *entry_direct;
    struct linked_list *next;
};
typedef struct linked_list * linkedList_ptr;

bool fat12_read_bootsector();
float fat_numberByte_fat_entry(uint32_t _fat_name);
uint32_t reverseByte(uint8_t* _byte,uint8_t _count);
linkedList_ptr fap12_read_entry_direct(uint32_t _first_cluster,linkedList_ptr _head_ptr);
uint32_t fat12_read_next_fatIndex(uint32_t _index);

#endif /* _HEADER_FILENAME_FAT */
