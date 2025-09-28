#include "unity.h"
#include "file_stream.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char path[] = "./DEMOFILE";
char text[] = "ehloworld!";

void setUp(void) {
    return;
}

void tearDown(void) {
    char rbuf[sizeof (text)];
    FILE *file = fopen(path, "r");
    if (file != NULL) {
        size_t rsize = fread(rbuf, sizeof (char), sizeof (rbuf), file);
        TEST_ASSERT_EQUAL(0, ferror(file));
        TEST_ASSERT_EQUAL_STRING_MESSAGE(text, rbuf, "testing that content of DEMOFILE is ehloworld! or not");
        fclose(file);
    }
    remove(path);
    return;
}

void test_start_file_stream(void) {
    MFILE mfile = {.file = NULL, .size = 0, .pos = 0, .buf = NULL};
    FileContext filec;

    filec.mfile = &mfile;
    filec.size = sizeof (text);

    TEST_ASSERT_EQUAL_MESSAGE(SUCCESS, start_file_stream(&filec, path, true), "testing start_file_stream() value return");
    rewind(filec.mfile->file);
    TEST_ASSERT_EQUAL_MESSAGE(sizeof (text), fwrite(text, sizeof (char), strlen(text) + 1, filec.mfile->file), "testing filec.mfile->file is valid to write or not");
    TEST_ASSERT_EQUAL_MESSAGE(SUCCESS, end_file_stream(&filec), "testing closing and unmapping the file");
    return;
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_start_file_stream);
    return UNITY_END();
}
