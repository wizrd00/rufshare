#include "unity.h"
#include "net_stream.h"

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
    status_t stat = init_tcp_socket(&sock, sip, sport, dip, dport);
    TEST_ASSERT_EQUAL_MESSAGE(SUCCESS, stat, "first init_tcp_socket test failed");
    TEST_ASSERT_EQUAL_MESSAGE(SUCCESS, close_socket(sock), "close_socket has failed");
    return;
}

void test_init_tcp_socket1(void) {
    sockfd_t sock;
    ipv4str_t sip = "301.10.50.1";
    port_t sport = 2048;
    ipv4str_t dip = "10.10.257.193";
    port_t dport = 4096;
    status_t stat = init_tcp_socket(&sock, sip, sport, dip, dport);
    TEST_ASSERT_EQUAL_MESSAGE(BADIPV4, stat, "second init_tcp_socket test must fails but didn't");
    return;
}

void test_init_tcp_socket2(void) {
    sockfd_t sock;
    ipv4str_t sip = "10.10.50.4";
    port_t sport = 2048;
    ipv4str_t dip = "1.1.1.1";
    port_t dport = 80;
    status_t stat = init_tcp_socket(&sock, sip, sport, dip, dport);
    TEST_ASSERT_EQUAL_MESSAGE(ERRBIND, stat, "third init_tcp_socket test must fails but didn't");
    return;
}

int main(void) {
    UNITY_BEGIN();
    // tests must not run back to back(system needs at least 30s interval because of TCP TIME-WAIT)
    RUN_TEST(test_init_tcp_socket0);
    RUN_TEST(test_init_tcp_socket1);
    RUN_TEST(test_init_tcp_socket2);
    return UNITY_END();
}
