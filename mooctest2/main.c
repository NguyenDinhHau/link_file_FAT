#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdint.h>
#include "HAL.h"
#include "FAT12.h"


int main()
{
    fat12_read_bootsector_struct_t *bootsector =NULL;
    linkedList_ptr head = NULL;
    linkedList_ptr cur = NULL;
    uint8_t number[3];
    
    uint32_t m=0;
//    m = fat12_read_next_fatIndex(6);
//    int a = 0;
//    int m = 0;
//    int n = 13;
//    m = n*1.5 + 2;
//    printf("%d\n",m);
    if(fat_init())
    {
        m= fat12_read_next_fatIndex(13);
        printf("%X",m);
//       head = fap12_read_entry_direct(head);
//       kmc_read_threebyte_fat(1259,&number[0]);
//       a = reverseByte(number,3);
//       printf("%x",a);
//       for(a=0; a<3;a++)
//       {
//           printf("%X",number[a]);
//       }
       //printf("%d",head);
//       cur = head;
//       while(cur != NULL)
//       {
//           printf("%X",cur->entry_direct->fileName[0]);
//           cur = cur->next;
//       }
      // printf("%x",head->entry_direct->fileName[0]);
    }
    else  
    {
        printf("hau");
    }
    
    fat_deinit();
    
    return 0;
}
