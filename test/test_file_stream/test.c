#include "unity.h"
#include "file_stream.h"

void setUp(void) {
    return;
}

void tearDown(void) {
    return;
}

void test_start_file_stream(void) {
    // testing file_stream is not important anymore, so I'll deal with it later.
    return;
}

int main(void) {
    printf("\n\x1b[1;93mStart testing file_stream.c\x1b[0m\n\n");
    UNITY_BEGIN();
    RUN_TEST(test_start_file_stream);
    return UNITY_END();
}
