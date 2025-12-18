#ifndef RUFSHARE_PROTOCOL_H
#define RUFSHARE_PROTOCOL_H

#include <stdint.h>
#include <arpa/inet.h>

#define CAST 0x11
#define FLOW 0x22
#define SEND 0x33
#define RECV 0x44

typedef uint8_t RUFShareType;
typedef uint8_t RUFShareACK;
typedef uint8_t RUFSharePad;
typedef uint16_t RUFShareChunkSize;
typedef uint16_t RUFSharePartialChunkSize;
typedef uint16_t RUFShareCRC16;
typedef uint32_t RUFShareSequence;
typedef uint32_t RUFShareChunkCount;
typedef uint32_t RUFShareCRC32;

typedef struct {
	RUFShareType type;
	RUFSharePad pad0;
	RUFShareCRC16 crc;
} CastPacket;

typedef struct {
	RUFShareType type;
	RUFSharePad pad0;
	RUFShareChunkSize chunk_size;
	RUFShareSequence sequence;
	RUFShareCRC32 crc;
} FlowPacket;

typedef struct {
	RUFShareType type;
	RUFSharePad pad0;
	RUFShareChunkSize chunk_size;
	RUFShareChunkCount chunk_count;
	RUFSharePartialChunkSize partial_chunk_size;
	RUFShareCRC16 crc;
} SendPacket;

typedef struct {
	RUFShareType type;
	RUFShareACK ack;
	RUFShareCRC16 crc;
	RUFShareSequence sequence;
} RecvPacket;

static inline CastPacket pack_RUFShare_CastPacket(RUFShareCRC16 crc)
{
	CastPacket packet = {
		.type = CAST,
		.pad0 = 0,
		.crc = htons(crc)
	};
	return packet;
}

static inline FlowPacket pack_RUFShare_FlowPacket(RUFShareChunkSize chunk_size, RUFShareSequence sequence, RUFShareCRC32 crc)
{
	FlowPacket packet = {
		.type = FLOW,
		.pad0 = 0,
		.chunk_size = htons(chunk_size),
		.sequence = htonl(sequence),
		.crc = htonl(crc)
	};
	return packet;
}

static inline SendPacket pack_RUFShare_SendPacket(RUFShareChunkSize chunk_size, RUFShareChunkCount chunk_count, RUFSharePartialChunkSize partial_chunk_size, RUFShareCRC16 crc)
{
	SendPacket packet = {
		.type = SEND,
		.pad0 = 0,
		.chunk_size = htons(chunk_size),
		.chunk_count = htonl(chunk_count),
		.partial_chunk_size = htons(partial_chunk_size),
		.crc = htons(crc)
	};
	return packet;
}

static inline RecvPacket pack_RUFShare_RecvPacket(RUFShareACK ack, RUFShareCRC16 crc, RUFShareSequence sequence)
{
	RecvPacket packet = {
		.type = RECV,
		.ack = ack,
		.crc = htons(crc),
		.sequence = htonl(sequence)
	};
	return packet;
}

static inline CastPacket convert_CastPacket_byteorder(CastPacket *packet)
{
	CastPacket new_packet = {
		.type = packet->type,
		.pad0 = 0,
		.crc = ntohs(packet->crc)
	};
	return new_packet;
}

static inline FlowPacket convert_FlowPacket_byteorder(FlowPacket *packet)
{
	FlowPacket new_packet = {
		.type = packet->type,
		.pad0 = 0,
		.chunk_size = ntohs(packet->chunk_size),
		.sequence = ntohl(packet->sequence),
		.crc = ntohl(packet->crc)
	};
	return new_packet;
}

static inline SendPacket convert_SendPacket_byteorder(SendPacket *packet)
{
	SendPacket new_packet = {
		.type = packet->type,
		.pad0 = 0,
		.chunk_size = ntohs(packet->chunk_size),
		.chunk_count = ntohl(packet->chunk_count),
		.partial_chunk_size = ntohs(packet->partial_chunk_size),
		.crc = ntohs(packet->crc)
	};
	return new_packet;
}

static inline RecvPacket convert_RecvPacket_byteorder(RecvPacket *packet)
{
	RecvPacket new_packet = {
		.type = packet->type,
		.ack = packet->ack,
		.crc = ntohs(packet->crc),
		.sequence = ntohl(packet->sequence)
	};
	return new_packet;
}

#endif
