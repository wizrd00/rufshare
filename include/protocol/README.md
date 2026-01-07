# RUFShare library
Reliable UDP File Sharing protocol, implemented exclusively for Cyphare

- [Protocol Specification](#Protocol-Specification)
    - [RUFShare with type CAST](#RUFShare-with-type-CAST)
    - [RUFShare with type FLOW](#RUFShare-with-type-FLOW)
    - [RUFShare with type SEND](#RUFShare-with-type-SEND)
    - [RUFShare with type RECV](#RUFShare-with-type-RECV)
- [Functions](#Functions)
    - [CAST packet](#CAST-packet)
    - [FLOW packet](#FLOW-packet)
    - [SEND packet](#SEND-packet)
    - [RECV packet](#RECV-packet)
    - [packet converter to host byte order](#packet-converter-to-host-byte-order)

## Protocol Specification
RUFShare protocol splits into five types :  
- CAST
- FLOW
- SEND
- RECV

#### RUFShare with type CAST
```
 0 0 0 0 0 0 0 0 0 0 1 1 1 1 1 1 1 1 1 1 2 2 2 2 2 2 2 2 2 2 3 3
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|  type(0x11)   |      pad      |            CRC16              |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
.                          name@ip:port                         .
```
``` C
typedef struct {
    uint8_t type;
    uint8_t pad0;
    uint16_t crc;
} CastPacket;
```
| Fields | describe |
|--------|----------|
| type  | value 0x11 for CAST packets |
| CRC16 | CRC16 value of entire payload    |

#### RUFShare with type FLOW
```
 0 0 0 0 0 0 0 0 0 0 1 1 1 1 1 1 1 1 1 1 2 2 2 2 2 2 2 2 2 2 3 3
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|  type(0x22)   |      pad      |          chunk size           |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                            sequence                           |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                             CRC32                             |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
.                                                               .
.                              data                             .
.                                                               .
```
``` C
typedef struct {
    uint8_t type;
    uint8_t pad0;
    uint16_t chunk_size;
    uint32_t sequence;
    uint32_t crc;
} FlowPacket;
```
| Fields | describe |
|--------|----------|
|    type    | value 0x22 for FLOW packets   |
| chunk size | specifies chunk size in Byte  |
|  sequence  | specifies sequence of packets |
|    CRC32   | CRC32 value of entire packet  |

#### RUFShare with type SEND
```
 0 0 0 0 0 0 0 0 0 0 1 1 1 1 1 1 1 1 1 1 2 2 2 2 2 2 2 2 2 2 3 3
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|  type(0x33)   |      pad      |          chunk size           |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                         chunk count                           |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|      partial chunk size       |            CRC16              |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
.                          name@ip:port                         .
```
``` C
typedef struct {
    uint8_t type;
    uint8_t pad0;
    uint16_t chunk_size;
    uint32_t chunk_count;
    uint16_t partial_chunk_size;
    uint16_t crc;
} SendPacket;
```
| Fields | describe |
|--------|----------|
|        type        | value 0x33 for SEND packets  |
|     chunk size     | specifies chunk size in Byte |
|     chunk count    | number of fixed-size chunks  |
| partial chunk size | tail of file < chunk size    |
|        CRC16       | CRC16 value of entire packet |

#### RUFShare with type RECV
```
 0 0 0 0 0 0 0 0 0 0 1 1 1 1 1 1 1 1 1 1 2 2 2 2 2 2 2 2 2 2 3 3
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|  type(0x44)   | | | | | | | |A|            CRC16              |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                            sequence                           |
```
``` C
typedef struct {
    uint8_t type;
    uint8_t ack;
    uint16_t crc;
    uint32_t sequence;
} RecvPacket;
```
| Fields | describe |
|--------|----------|
|  type | value 0x44 for RECV packets  |
|  ACK  | 1 for ACK, 0 for NACK        |
| CRC16 | CRC16 value of entire packet |


## Functions

#### CAST packet
``` C
CastPacket pack_RUFShare_CastPacket(RUFShareCRC16 crc);
```
- the function returns a ```CastPacket``` in network byte order

| Argument | size |
|----------|------|
| crc | unsigned 16-bit int |

#### FLOW packet
``` C
FlowPacket pack_RUFShare_FlowPacket(RUFShareChunkSize chunk_size, RUFShareSequence sequence, RUFShareCRC32 crc);
```
- the function returns a ```FlowPacket``` in network byte order

| Argument | size |
|----------|------|
| chunk_size | unsigned 16-bit int |
| sequence | unsigned 32-bit int |
| crc | unsigned 32-bit int |

#### SEND packet
``` C
SendPacket pack_RUFShare_SendPacket(RUFShareChunkSize chunk_size, RUFShareChunkCount chunk_count, RUFSharePartialChunkSize partial_chunk_size, RUFShareCRC16 crc);
```
- the function returns a ```SendPacket``` in network byte order

| Argument | size |
|----------|------|
| chunk_size | unsigned 16-bit int |
| chunk_count | unsigned 32-bit int |
| partial_chunk_size | unsigned 16-bit int |
| crc | unsigned 16-bit int |

#### RECV packet
``` C
RecvPacket pack_RUFShare_RecvPacket(RUFShareACK ack, RUFShareCRC16 crc, RUFShareSequence sequence);
```
- the function returns a ```RecvPacket``` in network byte order

| Argument | size |
|----------|------|
| ack | unsigned 8-bit int |
| crc | unsigned 16-bit int |
| sequence | unsigned 32-bit int |

#### packet converter to host byte order
``` C
CastPacket convert_CastPacket_byteorder(CastPacket* packet);

FlowPacket convert_FlowPacket_byteorder(FlowPacket* packet);

SendPacket convert_SendPacket_byteorder(SendPacket* packet);

RecvPacket convert_RecvPacket_byteorder(RecvPacket* packet);
```
