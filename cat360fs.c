#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include "disk.h"


int main(int argc, char *argv[]) {
    superblock_entry_t sb;
    directory_entry_t dr;
    int  i;
    char *imagename = NULL;
    char *filename  = NULL;
    FILE *f;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--image") == 0 && i+1 < argc) {
            imagename = argv[i+1];
            i++;
        } else if (strcmp(argv[i], "--file") == 0 && i+1 < argc) {
            filename = argv[i+1];
            i++;
        }
    }

    if (imagename == NULL || filename == NULL) {
        fprintf(stderr, "usage: cat360fs --image <imagename> " \
            "--file <filename in image>");
        exit(1);
    }
    
    f = fopen(imagename, "r");
    if (f==NULL) {                        //to handle errors if the imagename provided is not found
        printf("File not found\n");
        exit(1);
    }
    fread(&sb, sizeof(sb),1,f);
    
    sb.dir_start = ntohl(sb.dir_start);
    sb.dir_blocks = ntohl(sb.dir_blocks);
    sb.block_size = ntohs(sb.block_size);
    
    fseek(f, sb.dir_start*sb.block_size, SEEK_SET);
    int found_file=0;
    
    //loops through each directory entry
    for (i = 0; i<(sb.dir_blocks*sb.block_size/64);i++)
    {
        fseek(f, sb.dir_start*sb.block_size + i*64, SEEK_SET); //for each iteration, goes to the start of current entry no. 
        fread(&dr, sizeof(dr),1,f); 
        int cur_block=0;
        cur_block=dr.start_block;
        cur_block = ntohl(cur_block);
        
        int filesize = dr.file_size;
        filesize = ntohl(filesize);
        char ch;
        int length; int total_len=0;
        
        
        //for the filename, reads the contents of the file and prints to console
        if(dr.status==1 && strcmp (filename, dr.filename)==0)
        {
            found_file=1;
            char b[sb.block_size];
            while (cur_block!=-1)
            {
                fseek(f, cur_block*sb.block_size, SEEK_SET);         //goes to current block
                length = 0;
                while(length < sb.block_size && total_len < filesize){     //for each block or till end of file for last block
                    ch = fgetc(f);
                    b[length] = (char) ch;
                    length++; 
                    total_len++;
                }
                b[sb.block_size]='\0';
                fwrite(&b,length,1,stdout);
                fseek(f, sb.block_size +  (cur_block*4), SEEK_SET); //goes to assignned fat entry
                fread(&cur_block, sizeof(int),1,f);
                cur_block = ntohl(cur_block);                 //gets value of next block entry or -1 if last block of file
            }
        }
    }
    if(found_file==0)            //to handle errors if the file provided is not found
    {
        printf("File not found\n");
    }
    fclose(f);
    return 0; 
}
