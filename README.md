# diskimage
Goals:
(1) printing information about a disk image (stat360fs); 
(2) printing a directory listing of files in a disk image (ls360fs); 
(3) outputting the contents of a file in disk image to the host console’s stdout stream (cat360fs); 
(4) copying a host-system file to a disk image (stor360fs).

Task 1: stat360fs.c
Displays information about a 360fs file-system image. Amongst other things your code must read in the superblock of the disk image and use that information to read entries in the FAT. Here is the program’s output for IMAGES/disk03X.img in the distributed files (and please use this format):

$ ./stat360fs --image IMAGES/disk03X.img 
360fs (disk03X.img)

-------------------------------------------------
  Bsz   Bcnt  FATst FATcnt  DIRst DIRcnt
  256   7900      1    124    125     16

-------------------------------------------------
 Free   Resv  Alloc
 7759    125     16



Task 2: ls360fs.c
Displays the root-directory listing for a 360fs file-system image. Here is an example of the program’s output for IMAGES/disk04.img in the distributed files.

$ ./ls360fs --image IMAGES/disk04.img
     159 2022-Jul-14 15:20:26 alphabet_short.txt
    6784 2022-Jul-14 15:20:26 alphabet.txt
      93 2022-Jul-14 15:20:26 digits_short.txt
   18228 2022-Jul-14 15:20:26 digits.txt


Task 3: cat360fs.c
Copies a file from a disk image to the stdout stream of the host console. If the specified file is not found in the root directory, print a “file not found” message on a single line and exit the program.

$ ./cat360fs --image IMAGES/disk04.img --file alphabet_short.txt
abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ
abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ
abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ



Task 4: stor360fs.c
Copies a file from host system to a specified disk image. You should print an appropriate message if (a) the host-system file cannot be found, or (b) the file already exists on the disk image, or (c) there is not enough room to store the file on the disk image.

$ cat >foo.txt
Foo!
<ctrl-D>
$ ./stor360fs --image disk03.img --file foo.txt --source foo.txt
$ ./ls360fs --image disk03.img
     159 2022-Jul-16 19:58:56 alphabet_short.txt
      93 2022-Jul-16 19:58:56 digits_short.txt
       5 2022-Jul-17 13:20:32 foo.txt
$ ./stor360fs --image disk03.img --file foo.txt --source foo.txt
file already exists in the image
$ ./cat360fs --image disk03.img --file foo.txt | diff ./foo.txt -
$ # no output from diff -- that’s good as it means the files are identical
