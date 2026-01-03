#include "unity.h"
#include "utils/infostr.h"

void setUp(void) {}

void tearDown(void) {}

void test_unpack(void)
{
	CntlAddrs addrs;
	char infostr[] = "file:cyphare1314930579@10.141.29.207:1308 ";
	unpack_from_infostring(infostr, &addrs);
	TEST_ASSERT_EQUAL_STRING("file", addrs.filename);
	TEST_ASSERT_EQUAL_STRING("cyphare1314930579", addrs.name);
	TEST_ASSERT_EQUAL_STRING("10.141.29.207", addrs.remote_ip);
	TEST_ASSERT_EQUAL(1308, addrs.remote_port);
	return;
}

int main(void) {
	UNITY_BEGIN();
	RUN_TEST(test_unpack);
	return UNITY_END();
}
