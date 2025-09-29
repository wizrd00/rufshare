## mfile.c

```mfopen()``` maps a file according to its size, so if size of the file that ```fstat()``` returns is zero, ```mmap()``` will fail.
