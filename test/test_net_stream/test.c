#include "unity.h"
#include "net_stream.h"
#include <stdbool.h>
#include <stdlib.h>

void setUp(void) {
	return;
}

void tearDown(void) {
	return;
}

void test_init_tcp_socket0(void) {
	sockfd_t sock;
	ipv4str_t sip = "0.0.0.0";
	port_t sport = 2048;
	ipv4str_t dip = "1.1.1.1";
	port_t dport = 80;
	status_t _stat = init_tcp_socket(&sock, sip, sport, dip, dport, true);
	TEST_ASSERT_EQUAL_MESSAGE(SUCCESS, _stat, "first init_tcp_socket test failed");
	TEST_ASSERT_EQUAL_MESSAGE(SUCCESS, close_socket(sock), "close_socket has failed");
	return;
}

void test_init_tcp_socket1(void) {
	sockfd_t sock;
	ipv4str_t sip = "301.10.50.1";
	port_t sport = 2048;
	ipv4str_t dip = "10.10.257.193";
	port_t dport = 4096;
	status_t _stat = init_tcp_socket(&sock, sip, sport, dip, dport, true);
	TEST_ASSERT_EQUAL_MESSAGE(BADIPV4, _stat, "second init_tcp_socket test must fails but didn't");
	return;
}

void test_init_tcp_socket2(void) {
	sockfd_t sock;
	ipv4str_t sip = "10.10.50.4";
	port_t sport = 2048;
	ipv4str_t dip = "1.1.1.1";
	port_t dport = 80;
	status_t _stat = init_tcp_socket(&sock, sip, sport, dip, dport, true);
	TEST_ASSERT_EQUAL_MESSAGE(ERRBIND, _stat, "third init_tcp_socket test must fails but didn't");
	return;
}

void test_accept_new_connection(void) {
	sockfd_t sock;
	ipv4str_t sip = "0.0.0.0"; 
	port_t sport = 4096;
	ipv4str_t dip = "1.1.1.1";
	port_t dport = 80;
	status_t stat0 = init_tcp_socket(&sock, sip, sport, dip, dport, true);
	TEST_ASSERT_EQUAL_MESSAGE(SUCCESS, stat0, "init_tcp_socket failed");
	sockfd_t newsock;
	ipv4str_t newip = malloc(INET_ADDRSTRLEN);
	if (newip == NULL)
		TEST_FAIL_MESSAGE("malloc returned NULL");
	port_t newport;
	status_t stat1 = accept_new_connection(&newsock, sock, newip, &newport, 1000);
	TEST_ASSERT_EQUAL_MESSAGE(SUCCESS, stat1, "accept_new_connection failed");
	TEST_ASSERT_EQUAL_STRING_MESSAGE("1.1.1.1", newip, "invalid expected ip");
	TEST_ASSERT_EQUAL_MESSAGE(dport, newport, "invalid expected port");
	TEST_ASSERT_EQUAL_MESSAGE(SUCCESS, close_socket(sock), "close failed");
	free(newip);
	return;
}

void test_pull_tcp_data0(void) {
	sockfd_t fsock = 8; // this fake socket fd makes the fake recv to copy bytes completely
	size_t bufsize = 64;
	buffer_t buf = malloc(bufsize);
	status_t _stat = pull_tcp_data(fsock, buf, bufsize, false);
	TEST_ASSERT_EQUAL_MESSAGE(SUCCESS, _stat, "in first test of pull_tcp_data stat must be SUCCESS but it isn't");
	free(buf);
	return;
}

void test_pull_tcp_data1(void) {
	sockfd_t fsock = 64; // this fake socket fd makes the fake recv to copy bytes incompletely
	size_t bufsize = 64;
	buffer_t buf = malloc(bufsize);
	status_t _stat = pull_tcp_data(fsock, buf, bufsize, false);
	TEST_ASSERT_EQUAL_MESSAGE(LOWSIZE, _stat, "in second test of pull_tcp_data stat must be LOWSIZE but it isn't");
	free(buf);
	return;
}

void test_init_udp_socket(void) {
	sockfd_t sock;
	ipv4str_t sip = "0.0.0.0";
	port_t sport = 2048;
	ipv4str_t dip = "1.1.1.1";
	port_t dport = 53;
	status_t _stat = init_udp_socket(&sock, sip, sport, dip, dport, true);
	TEST_ASSERT_EQUAL_MESSAGE(SUCCESS, _stat, "first init_udp_socket test failed");
	TEST_ASSERT_EQUAL_MESSAGE(SUCCESS, close_socket(sock), "close_socket has failed");
	return;
}

void test_set_socket_timeout(void) {
	int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sockfd == -1)
		TEST_FAIL_MESSAGE("unable to create socket");
	status_t _stat = set_socket_timeout(sockfd, 10);
	TEST_ASSERT_EQUAL_MESSAGE(SUCCESS, _stat, "set_socket_timeout must return SUCCESS but didn't");
	TEST_ASSERT_EQUAL_MESSAGE(SUCCESS, close_socket(sockfd), "close_socket has failed");
	return;
}

int main(void) {
	printf("\n\x1b[1;93mStart testing net_stream.c\x1b[0m\n");
	printf("\n\x1b[1;31mThis test required internet access\x1b[0m\n\n");
	UNITY_BEGIN();
	RUN_TEST(test_init_tcp_socket0);
	RUN_TEST(test_init_tcp_socket1);
	RUN_TEST(test_init_tcp_socket2);
	RUN_TEST(test_accept_new_connection);
	RUN_TEST(test_pull_tcp_data0);
	RUN_TEST(test_pull_tcp_data1);
	RUN_TEST(test_init_udp_socket);
	RUN_TEST(test_set_socket_timeout);
	return UNITY_END();
}
