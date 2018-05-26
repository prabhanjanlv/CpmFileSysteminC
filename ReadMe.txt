diskSimulator.c/h
The diskSimulator.c/h files contain C functions to simulate the operation of the
device driver to read and write disk blocks given an LBA-­-style index. The disk itself is
simulated by a 2-­-dimensional array of bytes (C type uint8_t) The disk has 256 blocks,
each with 1024 bytes (so each block can be indexed by a single byte, also uint8_t). There
are also routines that can read and write the while array that simulates the disk to a file
that is a byte-­-for-­-byte copy of the array. This way I can give you files representing the
images of already-­-set-­-up disks you can use for testing. For debugging, there is also a
routine that prints the 1k bytes of a block to the screen (16 rows of 16 2-­-digit hex
numbers, similar to the Unix command od).

Makefile
Makefile is a standard Unix make file to compile all the source files and make an
executable called cpmRun.

cpmfsys.h
This file contains function and data structure declarations you will need to implement
the filesystem (in file cpmfsys.c which you will fill out – you will be given a stub for it).
For this assignment you will implement the functions listed below. The detailed
specification of each function and a prototype for it is to be found in the comments of
the cpmfsys.h file.
• mkDirStruct
• writeDirStruct
• makeFreeList
• printFreeList
• checkLegalName
• findExtentWithName
• cpmDir
• cpmDelete
• cpmRename

image1.img
This is a disk image file to be used in testing your functions.

fsysdriver.c
The fsysdriver.c file implements main() to test your functions. The driver program
reads the image file as its starting point (see previous item in this list), and should
produce output as shown in the similarly named output file (see next item in this list).

sampleOutput.txt
sampleOutput.txt -­--­- text file showing the output on the console you should see after
running the main() routine from the previous driver.
Important! The remaining functions with prototypes in cpmfsys.h may be
implemented in a future assignment. If you intend to further improve your
programming skills, you may implement these remaining functions as an optional task.