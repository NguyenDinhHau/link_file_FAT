#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<string.h>
#include "HAL.h"
#include "FAT12.h"

static void show_directories(linkedList_ptr head)
{   
    linkedList_ptr current = NULL;
    uint32_t count = 1;
    current = head;
    printf("\n Number\t\t   File Name\t\t   Date Time\t\t  Size   \n");
    while(current!= NULL)
    {
        printf("%d\t\t   %s\t\t   %d   %d  %d\t\t   %d\t\t \n",count,current->entry_direct->name,current->entry_direct->day,current->entry_direct->month,current->entry_direct->year,current->entry_direct->size);
        current = current->next;
        count++;
    }
}

void MenuFat12()
{
    fat12_read_bootsector_struct_t *bootsector =NULL;
    linkedList_ptr head = NULL;
    linkedList_ptr cur = NULL;
    uint32_t count_number = 1;
    uint8_t first_cluser = 0;
    uint32_t number_choose = 0;
    fat12_read_bootsector();
    fat12_read_file(3);
    head = fap12_read_entry_direct(0,head);
    show_directories(head);

    printf("Enter your number you want ");
    scanf("%d",&number_choose);
   
    while(1)
    {
        if(number_choose == count_number)
        {
            
        }
    }
    
}

