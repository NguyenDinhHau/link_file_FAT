#include<stdio.h>
#include<stdint.h>
#include<stdbool.h>
#include<string.h>
#include<stdlib.h>
#include "FAT12.h"

void Show_folder(list_t* head)
{
    node_t* current = NULL;
    current = head->pHead;
    printf("\n index          Name        Type       Size(byte)              Created\n");
    while (current != NULL) 
    {
        if (current->Data.attributes != 0x10)
        {
            printf("\n%d. %15s %10s %15d %15d/%.2d/%.2d   %d:%d\n", current->Data.index, current->Data.filename,current->Data.fileType,\
             current->Data.fileSize, current->Data.year, current->Data.month, current->Data.date, current->Data.hours, current->Data.minutes);
        }
        else
        {
            printf("\n%d. %15s        FOLDER\n", current->Data.index, current->Data.filename);
        }
        current = current->pNext;
    }
}

void Menu(void)
{
    uint32_t option = 0;
    node_t* node = NULL;
    list_t head;
    create_List(&head);
    Read_boot_sector();
    read_root_directory(&head);
    Show_folder(&head);
    while (1) {
        printf("Please! Enter your number you want: ");
        scanf("%d", &option);
        node = head.pHead;
        while (node != NULL) {
            if (option == node->Data.index) {
                if (node->Data.attributes == 0x10 && node->Data.firstCluster != 0x00) {
                    read_folder(node->Data.firstCluster, &head);
                    Show_folder(&head);
                    break;
                }
                else if (node->Data.attributes == 0x10 && node->Data.firstCluster == 0x00) {
                    read_root_directory(&head);
                    Show_folder(&head);
                    break;
                }
                else if (node->Data.attributes == 0x00) {
                    read_file(node->Data.firstCluster);
                    printf("\n");
                    break;
                }
            }
            node = node->pNext;
        }
    }
}
