#ifndef _HAL_H_
#define _HAL_H_


#define DEFAULT_SECTOR_SIZE 512


bool kmc_init(const char *pathFile);
uint32_t kmc_read_sector(uint32_t index, uint8_t *buff);
uint32_t kmc_read_multi_sector(uint32_t index,uint32_t num, uint8_t *buff);
bool kmc_deinit();
bool kmc_update_sector_size(uint32_t SectorSize);

#endif
