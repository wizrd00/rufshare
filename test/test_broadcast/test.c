#include "unity.h"
#include "broadcast.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>

void setUp(void) {}

void tearDown(void) {}

void test_start_broadcast(void) {
	sockfd_t tmp_sock;
	CntlAddrs addrs = {
		.filename = "TestMan",
		.name = "TestMan",
		.local_ip = "0.0.0.0",
		.remote_ip = "255.255.255.255",
		.local_port = 4096,
		.remote_port = 4096
	};
	status_t _stat = start_broadcast();
	printf("%d\n", errno);
	printf(strerror(errno));
	TEST_ASSERT_EQUAL(SUCCESS, _stat);
	return;
}

int main(void) {
	printf("\n\x1b[1;93mThis test contains infinite loop and\nit going to be tested by wireshark so it doesn't return\x1b[0m\n\n");
	UNITY_BEGIN();
	RUN_TEST(test_start_broadcast);
	return UNITY_END();
}
