#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "HAL.h"

static uint32_t BytePerSector = DEFAULT_SECTOR_SIZE ;
FILE*s_file =NULL;

bool kmc_init(const char *pathFile)
{
    bool RetVal=true;
	s_file = fopen(pathFile,"rb");
	if(s_file==NULL)
	{
	    	RetVal=false;
    }		
	else
	{
	    return RetVal;
    }
}

bool kmc_deinit()
{
    bool RetVal=false;
	if(s_file!= NULL)
	{
		fclose(s_file);
		RetVal=true;
	}
	else
	{
	    return RetVal;
    }
}
uint32_t kmc_read_sector(uint32_t index, uint8_t *buff)
{
	uint8_t i=0;
	uint8_t size=0;
	
	if(!fseek(s_file,index*BytePerSector,SEEK_SET))
	{
	    size=fread((uint8_t*)&buff[0],BytePerSector,1,s_file);
    }

    return size;
}
uint32_t kmc_read_multi_sector(uint32_t index,uint32_t num, uint8_t *buff)
{
    uint8_t i=0;
	uint8_t size=0;
	
	if(!fseek(s_file,index*BytePerSector,SEEK_SET))
	{
	    size=fread((uint8_t*)&buff[0],BytePerSector*num,1,s_file);
    }
    return size;

}
bool kmc_update_sector_size(uint32_t SectorSize)
{
    bool RetVal=false;
    if(0==SectorSize % DEFAULT_SECTOR_SIZE )
    {
        SectorSize=BytePerSector;
        RetVal=true;
    }
    else
    {
        return RetVal;
    }
}


