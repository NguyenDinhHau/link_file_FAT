#ifndef _HEADER_FILENAME_HAL
#define _HEADER_FILENAME_HAL

#define FAP12_NB_PER_ENTRY_DIREC 32

bool fat_init();
uint32_t kmc_read_sector(uint32_t index, uint8_t *buff);
uint32_t kmc_read_multi_sector(uint32_t index, uint32_t num, uint8_t *buff);
uint32_t kmc_read_entries_directories(uint32_t index, uint8_t *buff);
uint32_t kmc_read_threebyte_fat(uint32_t _index_byte, uint8_t *buff);
void fat_deinit();

#endif /* _HEADER_FILENAME_HAL */
