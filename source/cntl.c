#include "cntl.h"

sockfd_t TCPsock;
time_t SNDtimeout;
time_t RCVtimeout;

status_t start_cntl(CntlAddrs *addr) {
}

status_t push_header(RUFShareType type, HeaderArgs *args) {
    status_t stat = SUCCESS;
    Buffer infostr[INFOSTRSIZE];
    Buffer buf;
    size_t bufsize;
    struct pollfd sock = {
        .fd = TCPsock,
        .events = POLLWRNORM
    };
    switch (type) {
        case CAST :
            CastPacket packet = pack_RUFShare_CastPacket(args->cast.crc);
            infostr = get_infostring(infostr, args->cast.info);
            bufsize = sizeof (packet) + strlen(infostr) + 1;
            CHECK_PTR(buf = (Buffer) malloc(bufsize), EMALLOC);
            memcpy(buf, (Buffer) &packet, sizeof (packet));
            strcpy(buf, infostr);
            break;
        case FLOW :
            FlowPacket packet = pack_RUFShare_FlowPacket(args.flow.chunk_size, args.flow.sequence, args.flow.crc);
            bufsize = sizeof (packet);
            CHECK_PTR(buf = (Buffer) malloc(bufsize), EMALLOC);
            memcpy(buf, (Buffer) &packet, sizeof (packet));
            break;
        case SEND :
            SendPacket packet = pack_RUFShare_SendPacket(args.send.chunk_size, args.send.chunk_count, args.send.partial_chunk_size, args.send.crc);
            infostr = get_infostring(infostr, args->send.info);
            bufsize = sizeof (packet) + strlen(infostr) + 1;
            CHECK_PTR(buf = (Buffer) malloc(bufsize), EMALLOC);
            memcpy(buf, (Buffer) &packet, sizeof (packet));
            strcpy(buf, infostr);
            break;
        case RECV :
            RecvPacket packet = pack_RUFShare_RecvPacket(args.recv.ack, args.recv.crc, args.recv.sequence);
            bufsize = sizeof (packet);
            CHECK_PTR(buf = (Buffer) malloc(bufsize), EMALLOC);
            memcpy(buf, (Buffer) &packet, sizeof (packet));
            break;
        default :
            stat = BADARGS;
            return stat;
    }
    switch (poll(&sock, 1, SNDtimeout)) {
        case -1 :
            stat = FAILURE;
            break;
        case 0 :
            stat = TIMEOUT;
            break;
        default :
            stat = (sock.revents > 0) ? push_tcp_data(TCPsock, buf, bufsize) : FAILURE;
            break;
    }
    return stat;
}
