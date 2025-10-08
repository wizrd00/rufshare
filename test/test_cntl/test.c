#include "unity.h"
#include "cntl.h"

void setUp(void) {
    return;
}

void tearDown(void) {
    return;
}

void test_start_cntl(void) {
    CntlAddrs addrs = {
        .filename = "DEMOFILE0",
        .name = "TestMan",
        .local_ip = "0.0.0.0",
        .local_port = 4096,
        .remote_ip = "1.1.1.1",
        .remote_port = 80
    };
    sockfd_t sock;
    status_t stat = start_cntl(&addrs, &sock);
    // next test needs internet access to three way tcp handshake with remote server
    TEST_ASSERT_EQUAL_MESSAGE(SUCCESS, stat, "start_cntl failed");
    TEST_ASSERT_EQUAL_MESSAGE(SUCCESS, end_cntl(sock), "end_cntl failed");
    return;
}

void test_push_CAST_header(void) {
    sockfd_t fsock = 8;
    HeaderArgs args = {
        .cast = {
            .packet = pack_RUFShare_CastPacket(0x1337),
            .info = {
                .filename = "DEMOFILE1",
                .name = "TestMan",
                .local_ip = "0.0.0.0",
                .local_port = 4096
            }
        }
    };
    status_t stat = push_CAST_header(fsock, &args, 0.5);
    TEST_ASSERT_EQUAL_MESSAGE(SUCCESS, stat, "first push_CAST_header test failed");
    return;
}

void test_push_FLOW_header(void) {
    sockfd_t fsock = 8;
    RUFShareChunkSize csize = 16777216;
    HeaderArgs args = {
        .flow = {
            .packet = pack_RUFShare_FlowPacket(csize, 1, 0x1337)
        }
    };
    status_t stat = push_FLOW_header(fsock, &args, 0.5);
    TEST_ASSERT_EQUAL_MESSAGE(SUCCESS, stat, "first push_FLOW_header test failed");
    return;
}

void test_pull_CAST_header(void) {
    sockfd_t fsock = 8;
    HeaderArgs args;
    status_t stat = pull_CAST_header(fsock, &args, 0.1);
    TEST_ASSERT_EQUAL_MESSAGE(SUCCESS, stat, "first pull_CAST_header test failed");
    TEST_ASSERT_EQUAL_MESSAGE(CAST, args.cast.packet.type, "invalid type");
    TEST_ASSERT_EQUAL_MESSAGE(0x1337, args.cast.packet.crc, "invalid crc");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("DEMOFILE0", args.cast.info.filename, "invalid filename");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("TestMan", args.cast.info.name, "invalid name");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("192.168.43.81", args.cast.info.remote_ip, "invalid remote_ip");
    TEST_ASSERT_EQUAL_MESSAGE(4096, args.cast.info.remote_port, "invalid remote_port");
    return;
}

void test_pull_FLOW_header(void) {
    sockfd_t fsock = 64;
    HeaderArgs args;
    status_t stat = pull_FLOW_header(fsock, &args, 0.1);
    TEST_ASSERT_EQUAL_MESSAGE(SUCCESS, stat, "first pull_FLOW_header test failed");
    TEST_ASSERT_EQUAL_MESSAGE(FLOW, args.flow.packet.type, "invalid type");
    TEST_ASSERT_EQUAL_MESSAGE(1337, args.flow.packet.chunk_size, "invalid chunk_size");
    TEST_ASSERT_EQUAL_MESSAGE(1337, args.flow.packet.sequence, "invalid sequence");
    TEST_ASSERT_EQUAL_MESSAGE(0x1337, args.flow.packet.crc, "invalid crc");
    return;
}

void test_pull_SEND_header(void) {
    sockfd_t fsock = 512;
    HeaderArgs args;
    status_t stat = pull_SEND_header(fsock, &args, 0.1);
    TEST_ASSERT_EQUAL_MESSAGE(SUCCESS, stat, "first pull_SEND_header test failed");
    TEST_ASSERT_EQUAL_MESSAGE(SEND, args.send.packet.type, "invalid type");
    TEST_ASSERT_EQUAL_MESSAGE(1337, args.send.packet.chunk_size, "invalid chunk_size");
    TEST_ASSERT_EQUAL_MESSAGE(1337, args.send.packet.chunk_count, "invalid chunk_count");
    TEST_ASSERT_EQUAL_MESSAGE(2, args.send.packet.partial_chunk_size, "invalid partial_chunk_size");
    TEST_ASSERT_EQUAL_MESSAGE(0x1337, args.send.packet.crc, "invalid crc");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("DEMOFILE1", args.send.info.filename, "invalid filename");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("TestMan", args.send.info.name, "invalid name");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("192.168.43.81", args.send.info.remote_ip, "invalid remote_ip");
    TEST_ASSERT_EQUAL_MESSAGE(4096, args.send.info.remote_port, "invalid remote_port");
    return;
}

void test_pull_RECV_header(void) {
    sockfd_t fsock = 4096;
    HeaderArgs args;
    status_t stat = pull_RECV_header(fsock, &args, 0.1);
    TEST_ASSERT_EQUAL_MESSAGE(SUCCESS, stat, "first pull_RECV_header test failed");
    TEST_ASSERT_EQUAL_MESSAGE(RECV, args.recv.packet.type, "invalid type");
    TEST_ASSERT_EQUAL_MESSAGE(1, args.recv.packet.ack, "invalid ack");
    TEST_ASSERT_EQUAL_MESSAGE(1337, args.recv.packet.sequence, "invalid sequence");
    TEST_ASSERT_EQUAL_MESSAGE(0x1337, args.recv.packet.crc, "invalid crc");
    return;
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_start_cntl);
    RUN_TEST(test_push_CAST_header);
    RUN_TEST(test_push_FLOW_header);
    RUN_TEST(test_pull_CAST_header);
    RUN_TEST(test_pull_FLOW_header);
    RUN_TEST(test_pull_SEND_header);
    RUN_TEST(test_pull_RECV_header);
    return UNITY_END();
}
