#include "unity.h"
#include "net_stream.h"

void setUp(void) {

}

void tearDown(void) {

}

void test_func_init_tcp_socket(void) {
    sockfd_t fd;
    port_t port = 1337;
    ipv4str_t ip0 = "192.168.43.1";
    ipv4str_t ip1 = "10.10.45.1";
    ipv4str_t ip2 = "300.586.242.1";
    ipv4str_t ip3 = "helloworld";

    status_t stat0 = init_tcp_socket(&fd, ip0, port);
    TEST_ASSERT_EQUAL(stat0, SUCCESS);
    status_t stat1 = init_tcp_socket(&fd, ip1, port);
    TEST_ASSERT_EQUAL(stat1, SUCCESS);
    status_t stat2 = init_tcp_socket(&fd, ip2, port);
    TEST_ASSERT_EQUAL(stat2, BADARGS);
    status_t stat3 = init_tcp_socket(&fd, ip3, port);
    TEST_ASSERT_EQUAL(stat3, BADARGS);

    return;
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_func_init_tcp_socket);
    return UNITY_END();
}
