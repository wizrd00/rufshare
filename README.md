# RUFShare
![RUF](https://img.shields.io/badge/RUF-Reliable%20UDP%20File%20Transfer-blue)
![CRC](https://img.shields.io/badge/Integrity-CRC16%20%7C%20CRC32-green)
![Language](https://img.shields.io/badge/C-C99-orange)
![Platform](https://img.shields.io/badge/Linux-POSIX-red)

RUFShare (Reliable UDP File Sharing) is a lightweight file transfer library built on top of UDP, designed to provide reliable file delivery while maintaining the performance advantages of connectionless communication.

The library implements a custom protocol that handles:

- Peer discovery
- File transfer negotiation
- Chunked data transmission
- Integrity verification using CRC16/CRC32
- Acknowledgement and retransmission mechanisms

RUFShare is written in pure C99 and targets Unix-like systems.

---

## Features

- Reliable file transfer over UDP
- Custom packet-based protocol
- Peer discovery through broadcasting
- Chunked file transmission
- CRC-based data integrity validation
- Shared library interface
- Minimal external dependencies
- Suitable for embedded, networking, and systems programming projects

---

## Protocol Overview

RUFShare uses four packet types:

| Packet | Purpose |
|----------|----------|
| CAST | Peer discovery and broadcasting |
| SEND | File transfer metadata |
| FLOW | File data chunks |
| RECV | ACK/NACK responses |

### Transfer Flow

```text
Sender                     Receiver
  |                             |
  |-------- CAST -------------> |
  | <------ CAST -------------- |
  |                             |
  |-------- SEND -------------> |
  | <------ ACK --------------- |
  |                             |
  |-------- FLOW -------------> |
  | <------ ACK/NACK ---------- |
  |                             |
  |-------- FLOW -------------> |
  | <------ ACK/NACK ---------- |
```

---

## Project Structure

```text
include/
├── broadcast.h
├── cntl.h
├── data.h
├── file_stream.h
├── net_stream.h
├── puller.h
├── pusher.h
├── scanpair.h
├── tryexec.h
└── protocol/

source/
├── broadcast.c
├── cntl.c
├── data.c
├── file_stream.c
├── net_stream.c
├── puller.c
├── pusher.c
└── ...

library/
├── librufshare.so
└── libcrc.a
```

---

## Building

### PCC (default)

```bash
make
```

### GCC

```bash
make CC=gcc
```

Generated library:

```text
library/librufshare.so
```

---

## Public API

Initialize the library:

```c
status_t initiate(const char *logpath, size_t logcount);
```

Shutdown:

```c
status_t deinitiate(void);
```

Send a file:

```c
status_t push_file(
    InitConfig *config,
    const char *path
);
```

Receive a file:

```c
status_t pull_file(
    InitConfig *config,
    char *remote_name
);
```

Broadcast presence:

```c
status_t broadcast(
    InitConfig *config
);
```

Discover peers:

```c
status_t scanpair(
    InitConfig *config,
    PairInfo **info,
    size_t *len
);
```

---

## Example

```c
#include <rufshare.h>

int main(void)
{
    if (initiate("./logs", 10) != SUCCESS)
        return 1;

    InitConfig cfg = {
        /* configuration */
    };

    push_file(&cfg, "example.bin");

    deinitiate();

    return 0;
}
```

---

## Reliability

RUFShare achieves reliability through:

- Sequence numbers
- ACK/NACK packets
- CRC16 validation for control packets
- CRC32 validation for data packets
- Chunk-based transmission

Unlike TCP, the protocol is application-defined and optimized specifically for file transfer workloads.

---

## Use Cases

- Local network file sharing
- Embedded device communication
- Custom networking applications
- Educational networking projects
- UDP protocol experimentation

---

## Requirements

- POSIX-compatible operating system
- C99 compiler
- pthread
- mmap support

Tested with:

- PCC
- GCC

---

## License

This project is licensed under the terms provided in the LICENSE file.
