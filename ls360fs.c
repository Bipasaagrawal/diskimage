#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include "disk.h"

//takes in month number and returns string name
char *month_to_string(short m) {
    switch(m) {
    case 1: return "Jan";
    case 2: return "Feb";
    case 3: return "Mar";
    case 4: return "Apr";
    case 5: return "May";
    case 6: return "Jun";
    case 7: return "Jul";
    case 8: return "Aug";
    case 9: return "Sep";
    case 10: return "Oct";
    case 11: return "Nov";
    case 12: return "Dec";
    default: return "?!?";
    }
}

//unpack_datetime takes in parameters for date and time
//it converts it into the required format and prints to console
void unpack_datetime(unsigned char *time, short year, short month, 
    short day, short hour, short minute, short second)
{
    assert(time != NULL);
    
    memcpy(&year, time, 2);
    year = htons(year);
    month = (unsigned short)(time[2]);
    day = (unsigned short)(time[3]);
    hour = (unsigned short)(time[4]);
    minute = (unsigned short)(time[5]);
    second = (unsigned short)(time[6]);
    
    char *m = month_to_string(month);
    printf("%4d-%s-%02d %02d:%02d:%02d ",year, m, day, hour , minute, second);
}


int main(int argc, char *argv[]) {
    superblock_entry_t sb;
    directory_entry_t dr;
    int  i;
    char *imagename = NULL;
    FILE *f;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--image") == 0 && i+1 < argc) {
            imagename = argv[i+1];
            i++;
        }
    }

    if (imagename == NULL)
    {
        fprintf(stderr, "usage: ls360fs --image <imagename>\n");
        exit(1);
    }
    
    f = fopen(imagename, "r");
    if (f==NULL) {                          //to handle errors if the imagename provided is not found
        printf("File not found\n");
        exit(1);
    }
    
    fread(&sb, sizeof(sb),1,f);               //reading superblock to struct
    
    sb.dir_start = ntohl(sb.dir_start);            //converts to host byte order
    sb.dir_blocks = ntohl(sb.dir_blocks);
    sb.block_size = ntohs(sb.block_size);
    
    fseek(f, sb.dir_start*sb.block_size, SEEK_SET);         //goes to start of the dir entries

    for (i = 0; i<(sb.dir_blocks*sb.block_size/64);i++)
    {
        fread(&dr, sizeof(dr),1,f);                  //reading directory entry block to struct  
        if(dr.status==1)                         //checks if it is not empty and prints filesize, modification time and filename 
        {
            dr.file_size= ntohl(dr.file_size);
            printf("%8d ",dr.file_size);
            unpack_datetime(dr.modify_time,0,0,0,0,0,0);
            printf("%s\n",dr.filename);
        }
    }
    fclose(f);
    return 0; 
}
