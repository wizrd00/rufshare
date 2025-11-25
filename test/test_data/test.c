#include "unity.h"
#include "data.h"
#include "file_stream.h"
#include <stdio.h>
#include <string.h>

void setUp(void) {
	FILE *file = fopen("DEMOFILE0", "w");
	if (file == NULL)
		printf("%s\n", strerror(errno));
	char text[] = 
	"A computer file is a collection of data on a computer storage device, primarily identified by its filename."
	" Just as words can be written on paper, so too can data be written to a computer file."
	" Files can be shared with and transferred between computers and mobile devices via removable media or networks.";
	fwrite(text, strlen(text) + 1, sizeof (char), file);
	fclose(file);
	return;
}

void tearDown(void) {
	remove("DEMOFILE0");
	remove("DEMOFILE1");
	return;
}

void test_start_data(void) {
	sockfd_t sock;
	CntlAddrs addrs = {
		.filename = "DEMOFILE",
		.name = "TestMan",
		.local_ip = "0.0.0.0",
		.local_port = 4096,
		.remote_ip = "1.1.1.1",
		.remote_port = 80
	};
	status_t _stat = start_data(&addrs, &sock);
	TEST_ASSERT_EQUAL_MESSAGE(SUCCESS, _stat, "start_data did not return SUCCESS");
	TEST_ASSERT_EQUAL_MESSAGE(SUCCESS, end_data(sock), "end_data did not return SUCCESS");
	return;
}

void test_push_chunk_data(void) {
	FileContext filec;
	ChunkContext chunkc = {.start_pos = 0, .chunk_size = 16};
	sockfd_t sock;
	CntlAddrs addrs = {
		.filename = "DEMOFILE0",
		.name = "TestMan",
		.local_ip = "0.0.0.0",
		.local_port = 4096,
		.remote_ip = "1.1.1.1",
		.remote_port = 80
	};
	TEST_ASSERT_EQUAL_MESSAGE(SUCCESS, start_file_stream(&filec, "DEMOFILE0", MRD), "start_file_stream has failed");
	TEST_ASSERT_EQUAL_MESSAGE(SUCCESS, start_data(&addrs, &sock), "start_data did not return SUCCESS");
	status_t _stat = push_chunk_data(sock, &filec, &chunkc, 1000);
	TEST_ASSERT_EQUAL_MESSAGE(SUCCESS, _stat, "push_chunk_data has failed");
	TEST_ASSERT_EQUAL_MESSAGE(SUCCESS, end_data(sock), "end_data has failed");
	TEST_ASSERT_EQUAL_MESSAGE(SUCCESS, end_file_stream(&filec), "end_file_stream has failed");
	return;
}

void test_pull_chunk_data(void) {
	FileContext filec = {.size = 64};
	ChunkContext chunkc = {.start_pos = 0, .chunk_size = 16};
	sockfd_t sock;
	CntlAddrs addrs = {
		.filename = "DEMOFILE1",
		.name = "TestMan",
		.local_ip = "0.0.0.0",
		.local_port = 4096,
		.remote_ip = "1.1.1.1",
		.remote_port = 80
	};
	TEST_ASSERT_EQUAL_MESSAGE(SUCCESS, start_file_stream(&filec, "DEMOFILE1", MWR), "start_file_stream has failed");
	TEST_ASSERT_EQUAL_MESSAGE(SUCCESS, start_data(&addrs, &sock), "start_data did not return SUCCESS");
	status_t _stat = pull_chunk_data(sock, &filec, &chunkc, 1000);
	TEST_ASSERT_EQUAL_MESSAGE(SUCCESS, _stat, "pull_chunk_data has failed");
	TEST_ASSERT_EQUAL_MESSAGE(SUCCESS, end_data(sock), "end_data has failed");
	TEST_ASSERT_EQUAL_MESSAGE(SUCCESS, end_file_stream(&filec), "end_file_stream has failed");
	return;
}

int main(void) {
	printf("\n\x1b[1;93mStart testing data.c\x1b[0m\n");
	printf("\n\x1b[1;31mThis test required internet access\x1b[0m\n\n");
	UNITY_BEGIN();
	RUN_TEST(test_start_data);
	RUN_TEST(test_push_chunk_data);
	RUN_TEST(test_pull_chunk_data);
	return UNITY_END();
}
