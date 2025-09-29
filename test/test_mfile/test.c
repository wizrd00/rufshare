#include "unity.h"
#include "mfile.h"
#include <fcntl.h>

void setUp(void) {
    creat("./DEMOFILE0", S_IRWXU);
    creat("./DEMOFILE1", S_IRWXU);
    creat("./DEMOFILE2", S_IRWXU);
    return;
}

void tearDown(void) {
    remove("./DEMOFILE0");
    remove("./DEMOFILE1");
    remove("./DEMOFILE2");
    return;
}

void test_mfopen_mfclose(void) {
    char path[] = "./DEMOFILE0";
    // this test checks mfopen() with zero size file
    // so mmap() must return error and mfile.buf must be NULL
    MFILE mfile = mfopen(path, "r+", PROT_READ, MAP_PRIVATE);
    TEST_ASSERT_NOT_NULL_MESSAGE(mfile.file, "file pointer is NULL");
    TEST_ASSERT_EQUAL_MESSAGE(0, mfile.pos, "mfile.pos is NOT zero!!!");
    TEST_ASSERT_NULL_MESSAGE(mfile.buf, "buf is NOT NULL!");
    TEST_ASSERT_EQUAL_MESSAGE(0, mfclose(&mfile), "closing fails!");
    return;
}

int fakefile(char *path, size_t size) {
    FILE *file = fopen(path, "w");
    return posix_fallocate(fileno(file), 0, size) | fclose(file);
}

void test_mfwrite_mfsync(void) {
    char path[] = "./DEMOFILE1";
    char text[] = "In computing, mmap(2) is a POSIX-compliant Unix system call that maps files or devices into memory."
        "It is a method of memory-mapped file I/O."
        "It implements demand paging because file contents are not immediately read from disk and initially use no physical RAM at all."
        "The actual reads from disk are performed after a specific location is accessed, in a lazy manner";

    fakefile(path, sizeof (text));
    MFILE mfile = mfopen(path, "r+", PROT_WRITE | PROT_READ, MAP_SHARED);
    TEST_ASSERT_NOT_NULL_MESSAGE(mfile.file, "file pointer is NULL");
    // the size of DEMOFILE1 must be sizeof (text)
    TEST_ASSERT_EQUAL_MESSAGE(sizeof (text), mfile.size, "size of the file is not equal to size of text");
    TEST_ASSERT_NOT_NULL_MESSAGE(mfile.buf, "mmap() returns NULL to mfile.buf");
    TEST_ASSERT_EQUAL_MESSAGE(mfwrite(text, sizeof (char), strlen(text) + 1, &mfile), sizeof (text), "mfwrite returns a size less than sizeof (text), maybe because mfile.size was <= sizeof (text)");
    TEST_ASSERT_EQUAL_MESSAGE(mfile.pos, mfile.size, "mfile.pos != mfile.size");
    TEST_ASSERT_EQUAL_MESSAGE(0, mfsync(mfile.buf, mfile.size, MS_SYNC), "mfsync fails");
    TEST_ASSERT_EQUAL_MESSAGE(0, mfclose(&mfile), "mfclose fails!");
    return;
}

int main(void) {
    printf("\n\x1b[1;93mStart testing mfile.c\x1b[0m\n\n");
    UNITY_BEGIN();
    RUN_TEST(test_mfopen_mfclose);
    RUN_TEST(test_mfwrite_mfsync);
    return UNITY_END();
}
