## mfile.c

```mfopen()``` maps a file according to its size, so if size of the file that ```fstat()``` returns is zero, ```mmap()``` will fail.

## net_stream.c

```host_ipstring()``` considers the size of ip to be INET_ADDRSTRLEN by default, so there is a possibility of buffer overflow if the ip doesn't have the required size.

## data.c

### push_chunk_data()
    -this function sets socket low-water mark size to 2 * SEGMENTSIZE
    -in this function, poll() will wait until the current available space in socket send buffer is equal ot greater than 2 * SEGMENTSIZE
