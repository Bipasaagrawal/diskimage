#include <arpa/inet.h>
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include "disk.h"


/*
 * Based on http://bit.ly/2vniWNb
 */

//returns date-time in the required format for the directory entry
void pack_current_datetime(unsigned char *entry) {
    assert(entry);

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    unsigned short year   = tm.tm_year + 1900;
    unsigned char  month  = (unsigned char)(tm.tm_mon + 1);
    unsigned char  day    = (unsigned char)(tm.tm_mday);
    unsigned char  hour   = (unsigned char)(tm.tm_hour);
    unsigned char  minute = (unsigned char)(tm.tm_min);
    unsigned char  second = (unsigned char)(tm.tm_sec);
    
    year = htons(year);

    memcpy(entry, &year, 2);
    entry[2] = month;
    entry[3] = day;
    entry[4] = hour;
    entry[5] = minute;
    entry[6] = second;  
}

//gives the address of the next free block in fat table, or returns -1 if full
int next_free_block(int *FAT, int max_blocks) {
    assert(FAT != NULL);
    int i;
    for (i = 0; i < max_blocks; i++) {
        if (FAT[i] == FAT_AVAILABLE) {
            return i;
        }
    }
    return -1;
}


int main(int argc, char *argv[]) {
    superblock_entry_t sb;
    directory_entry_t dr;
    int  i;
    char *imagename  = NULL;
    char *filename   = NULL;
    char *sourcename = NULL;
    FILE *f , *fromfile;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--image") == 0 && i+1 < argc) {
            imagename = argv[i+1];
            i++;
        } else if (strcmp(argv[i], "--file") == 0 && i+1 < argc) {
            filename = argv[i+1];
            i++;
        } else if (strcmp(argv[i], "--source") == 0 && i+1 < argc) {
            sourcename = argv[i+1];
            i++;
        }
    }

    if (imagename == NULL || filename == NULL || sourcename == NULL) {
        fprintf(stderr, "usage: stor360fs --image <imagename> " \
            "--file <filename in image> " \
            "--source <filename on host>\n");
        exit(1);
    }
    
    char ch;
    f = fopen(imagename, "r+");
    if (f==NULL) {                   //to handle errors if the imagename provided is not found
        printf("File not found\n");
        exit(1);
    }
    fread(&sb, sizeof(sb),1,f);       //reads superblock and converts to host byte order
    sb.dir_start = ntohl(sb.dir_start);
    sb.dir_blocks = ntohl(sb.dir_blocks);
    sb.block_size = ntohs(sb.block_size);
    sb.fat_start = ntohl(sb.fat_start);
    sb.num_blocks = ntohl(sb.num_blocks);
    
    int fat[sb.num_blocks];
    fseek(f, sb.fat_start*sb.block_size, SEEK_SET); 
    int block_int;
    for(i = 0;i<sb.num_blocks; i++)            //loops through fat table and stores all values in fat array
    {
        fread(&block_int, sizeof(int), 1, f);
        fat[i] = ntohl(block_int);

    }

    int num_of_dir;
    fseek(f, sb.dir_start*sb.block_size, SEEK_SET);
    for (i = 1; i<=(sb.dir_blocks*sb.block_size/64);i++)     //loops through each directory entry to check if file exists
    {
        fread(&dr, sizeof(dr),1,f);
        if(dr.status==1)
        {
            num_of_dir=i;
            dr.file_size= ntohl(dr.file_size);
            dr.num_blocks= ntohl(dr.num_blocks);
            if(strcmp (filename, dr.filename)==0)
            {
                printf("File already exists in the image\n");      //error handling if file already exists
                exit(1);
            }
        }
    }
     
    fromfile = fopen(sourcename, "r");
    long size = 0;
    if (fromfile==NULL) {                //to handle errors if the imagename provided is not found
        printf("Source file not found\n");
        exit(1);
    }
    else{                                //to calculate size of source file 
        fseek(fromfile, 0, SEEK_END);
        size = ftell(fromfile);
    }
    fseek(fromfile, 0, SEEK_SET);
    char rd[sb.block_size];
    int length;
    int total_length=0,next_free=0,prev_block =0,number_block=0;
    int is_starting_block=1;
    int next;
    
    while(1){
        number_block++;
        length = 0;
        while(length < sb.block_size && total_length < size) {    //loops through each blocksize in sourcefile
            ch = fgetc(fromfile);
            rd[length] = (char) ch;
            length++;
            total_length++;
        }
        if(length == 0){
            break;
        }
        next_free =next_free_block(fat, sb.num_blocks); 
        next = htonl(next_free);
        fat[next_free]=1;
        
        if(next_free==-1)                                     
        {
            printf("Full! Not enough room");
        }
        else
        {
            if(is_starting_block)                                 //sets some parameters when first block 
            {
                dr.start_block = htonl(next_free);
                prev_block = next_free;
                is_starting_block =0;
                memcpy(dr.filename, filename, 31);
                dr.status =1;
            }
            else
            {
                fseek(f, sb.block_size +  (prev_block*4), SEEK_SET);  //writes location of next free in current fat table entry 
                fwrite(&next, 1, 4, f);
                fat[prev_block] = next_free;
                prev_block = next_free;
            }
            if(length<sb.block_size)                               //indicates last block in sourcefile
            {
                fseek(f, sb.block_size +  (next_free*4), SEEK_SET);
                int last = htonl(0xffffffff);
                fwrite(&last, 1, 4, f);
                fat[next_free] = -1;
            }
            fseek(f, next_free*sb.block_size, SEEK_SET);
            fwrite(rd,1, length, f);
        }
    }
    
    dr.num_blocks = htonl(number_block);
    pack_current_datetime(dr.create_time);
    pack_current_datetime(dr.modify_time);
    dr.file_size = htonl(total_length);
    fseek(f, sb.dir_start*sb.block_size + num_of_dir*64, SEEK_SET);  
    fwrite(&dr,1,64,f);                                           //writes directory entry for the file written
    
    fclose(f);
    fclose(fromfile);
    return 0; 

}
