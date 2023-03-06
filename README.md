# diskimage
Goals:
(1) printing information about a disk image (stat360fs); 
(2) printing a directory listing of files in a disk image (ls360fs); 
(3) outputting the contents of a file in disk image to the host console’s stdout stream (cat360fs); 
(4) copying a host-system file to a disk image (stor360fs).

Task 1: stat360fs.c
Displays information about a 360fs file-system image. Reads in the superblock of the disk image and uses that information to read entries in the FAT. 

Task 2: ls360fs.c
Displays the root-directory listing for a 360fs file-system image. Here is an example of the program’s output for IMAGES/disk04.img in the distributed files.

$ ./ls360fs --image IMAGES/disk04.img
     159 2022-Jul-14 15:20:26 alphabet_short.txt
    6784 2022-Jul-14 15:20:26 alphabet.txt
      93 2022-Jul-14 15:20:26 digits_short.txt
   18228 2022-Jul-14 15:20:26 digits.txt

Task 3: cat360fs.c
Copies a file from a disk image to the stdout stream of the host console. 

$ ./cat360fs --image IMAGES/disk04.img --file alphabet_short.txt
abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ
abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ
abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ

Task 4: stor360fs.c
Copies a file from host system to a specified disk image. 
