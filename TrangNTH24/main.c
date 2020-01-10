#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "hal.h"
#include "FAT12.h"
#include "menu.h"

const char *pathFile = "floppy.img";

int main()
{
    if(kmc_init(pathFile))
    {
        Menu();
    }
    kmc_deinit();
    return 0;
}
