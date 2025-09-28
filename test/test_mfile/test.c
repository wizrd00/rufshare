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

void test_mfopen(void) {
    char path[] = "./DEMOFILE0";
    MFILE mfile = mfopen(path, "r+", 1, PROT_READ, MAP_PRIVATE);
    TEST_ASSERT_NOT_NULL_MESSAGE(mfile.file, "check FILE pointer is not NULL");
    TEST_ASSERT_EQUAL_MESSAGE(1, mfile.size, "check created file has \"size\" size");
    TEST_ASSERT_EQUAL_MESSAGE(0, mfile.pos, "check pos is zero by default (this test always pass)");
    TEST_ASSERT_NOT_NULL_MESSAGE(mfile.buf, "check main pointer is not NULL");
    return;
}

void test_mfwrite(void) {
    char path[] = "./DEMOFILE1";
    char text[] = "In computing, mmap(2) is a POSIX-compliant Unix system call that maps files or devices into memory."
        "It is a method of memory-mapped file I/O."
        "It implements demand paging because file contents are not immediately read from disk and initially use no physical RAM at all."
        "The actual reads from disk are performed after a specific location is accessed, in a lazy manner";

    MFILE mfile = mfopen(path, "w+", sizeof (text), PROT_WRITE | PROT_READ, MAP_PRIVATE);
    TEST_ASSERT_EQUAL_MESSAGE(0, posix_fallocate(fileno(mfile.file), 0, sizeof (text)), "allocating desired space for file to write");
    TEST_ASSERT_NOT_NULL_MESSAGE(mfile.file, "check FILE pointer is not NULL");
    TEST_ASSERT_EQUAL_MESSAGE(sizeof (text), mfile.size, "check created file has \"size\" size");
    TEST_ASSERT_EQUAL_MESSAGE(0, mfile.pos, "check pos is zero by default (this test always pass)");
    TEST_ASSERT_NOT_NULL_MESSAGE(mfile.buf, "check main pointer is not NULL");
    TEST_ASSERT_EQUAL_MESSAGE(mfwrite(text, sizeof (char), strlen(text) + 1, &mfile), sizeof (text), "mfwrite must write sizeof (text) exactly");
    return;
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_mfopen);
    RUN_TEST(test_mfwrite);
    return UNITY_END();
}
