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
    char *read = (char *) malloc(sizeof (text));
    FILE *file = fopen(path, "r");
    if (file != NULL) {
        size_t rsize = fread(read, sizeof (char), strlen(text) + 1, file);
        TEST_ASSERT_EQUAL_MESSAGE(sizeof (text), rsize, "testing that DEMOFILE is healthy or not");
        TEST_ASSERT_EQUAL_STRING(text, read);
        fclose(file);
    }
    remove(path);
    free(read);
    return;
}

void test_start_file_stream(void) {
    MFILE mfile = {.file = NULL, .size = 0, .pos = 0, .buf = NULL};
    FileContext filec;

    filec.mfile = &mfile;
    filec.mfile->size = sizeof (text);
    filec.size = sizeof (text);

    status_t stat0 = start_file_stream(&filec, path, true);
    TEST_ASSERT_EQUAL_MESSAGE(SUCCESS, stat0, "testing start_file_stream() value return");
    TEST_ASSERT_EQUAL_MESSAGE(sizeof (text), fwrite(text, sizeof (char), strlen(text) + 1, filec.mfile->file), "testing filec.mfile->file is valid to write or not");
    status_t stat1 = end_file_stream(&filec);
    TEST_ASSERT_EQUAL_MESSAGE(SUCCESS, stat1, "testing closing and unmapping the file");
    return;
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_start_file_stream);
    return UNITY_END();
}
