#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include "disk.h"
#include <sys/mman.h>

//print_info takes in the superblock, imagename and free,res,alloc variables
//It prints the information in the required format
void print_info(superblock_entry_t sb,int free,int res,int alloc,char *imagename)
{
    printf("\n=========================================================================\n");
    printf("%s (%s)\n\n",sb.magic,imagename);
    printf("-------------------------------------------------------------------------\n");
    printf("Bsz   Bcnt   FATst   FATcnt   DIRst   DIRcnt\n");
    printf("%d   %d     %d      %d      %d      %d\n\n",
           sb.block_size,sb.num_blocks,sb.fat_start,sb.fat_blocks,sb.dir_start,sb.dir_blocks);
    printf("-------------------------------------------------------------------------\n");
    printf("Free   Resv   Alloc\n");
    printf("%d   %d     %d\n\n",free,res,alloc);
    
}

int main(int argc, char *argv[]) {
    superblock_entry_t sb;
    int  i;
    char *imagename = NULL;
    FILE  *f;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--image") == 0 && i+1 < argc) {
            imagename = argv[i+1];
            i++;
        }
    }

    if (imagename == NULL) {
        fprintf(stderr, "usage: stat360fs --image <imagename>\n");
        exit(1);
    }

    f = fopen(imagename, "r");
    
    if (f==NULL) {               //to handle errors if the imagename provided is not found
        printf("File not found\n");
        exit(1);
    }
    
    fread(&sb, sizeof(sb),1,f);            //reading superblock to struct
    
    //converting int and short values to host byte order
    sb.block_size = ntohs(sb.block_size);
    sb.num_blocks = ntohl(sb.num_blocks);
    sb.fat_start = ntohl(sb.fat_start);
    sb.fat_blocks = ntohl(sb.fat_blocks);
    sb.dir_start = ntohl(sb.dir_start);
    sb.dir_blocks = ntohl(sb.dir_blocks);

    fseek(f, sb.fat_start*sb.block_size, SEEK_SET);    //goes to start of the FAT tables
                 
    int block_int; 
    int free=0, res=0, alloc=0;

    //loops through all the values in FAT and checks each one to calculate free/res/alloc
    for(i = 0;i<sb.num_blocks; i++)
    {
        fread(&block_int, sizeof(int), 1, f);         //reads each FAT entry
        block_int = ntohl(block_int);           //converts to host byte order
        if(block_int==0) {
            free++;
        }
        else if (block_int==1) {
            res++;
        }
        else {
            alloc++;
        }
    }    
    print_info(sb,free,res,alloc,imagename);         //to print in the required format
    fclose(f);
    return 0; 
}
