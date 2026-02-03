#include "unity.h"
#include "utils/fname.h"

void setUp(void) {}

void tearDown(void) {}

void test_extract_file_name(void) {
	char path0[] = "./path/to/ehlo";
	char path1[] = "/path/to/ehlo";
	char path2[] = "path/to/ehlo";
	char path3[] = "ehlo";
	char path4[] = "./ehlo";
	char path5[] = "/ehlo";
	char *dst;
	TEST_ASSERT_EQUAL_STRING("ehlo", extract_file_name(dst, path0, sizeof(path0)));
	TEST_ASSERT_EQUAL_STRING("ehlo", extract_file_name(dst, path1, sizeof(path1)));
	TEST_ASSERT_EQUAL_STRING("ehlo", extract_file_name(dst, path2, sizeof(path2)));
	TEST_ASSERT_EQUAL_STRING("ehlo", extract_file_name(dst, path3, sizeof(path3)));
	TEST_ASSERT_EQUAL_STRING("ehlo", extract_file_name(dst, path4, sizeof(path4)));
	TEST_ASSERT_EQUAL_STRING("ehlo", extract_file_name(dst, path5, sizeof(path5)));
	return;
}

int main(void) {
	UNITY_BEGIN();
	RUN_TEST(test_extract_file_name);
	return UNITY_END();
}
