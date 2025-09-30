## mfile.c

```mfopen()``` maps a file according to its size, so if size of the file that ```fstat()``` returns is zero, ```mmap()``` will fail.

## net_stream.c

```host_ipstring()``` considers the size of ip to be INET_ADDRSTRLEN by default, so there is a possibility of buffer overflow if the ip doesn't have the required size.
