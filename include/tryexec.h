#ifndef TRYEXEC_H
#define TRYEXEC_H

#include "types.h"

static inline void tryexec_start_broadcast(status_t _stat); 

static inline void tryexec_start_file_stream(status_t _stat) {
	switch (_stat) {
		case NOCREAT : raise_nocreat_error(__func__); break;
		case BADARGS : raise_badargs_error(__func__); break;
		case NOFSTAT : raise_nofstat_error(__func__); break;
		case NOAVAIL : raise_noavail_error(__func__); break;
		case NOTRUNC : raise_notrunc_error(__func__); break;
		case NOCLOSE : raise_noclose_error(__func__); break;
		default : return;
	}
	return;
}

static inline void tryexec_start_cntl(status_t _stat) {
	switch (_stat) {
		case BADIPV4 : raise_badipv4_error(__func__); break;
		case BADPORT : raise_badport_error(__func__); break;
		case BADARGS : raise_badargs_error(__func__); break;
		case INVSOCK : raise_invsock_error(__func__); break;
		case FAILSET : raise_failset_error(__func__); break;
		case ERRBIND : raise_errbind_error(__func__); break;
		case ERRCONN : raise_errconn_error(__func__); break;
		default : return;
	}
	return;
}

static inline void tryexec_accept_cntl(status_t _stat) {
	switch (_stat) {
		case FAILURE : raise_failure_error(__func__); break;
		case INVSOCK : raise_invsock_error(__func__); break;
		case BADINET : raise_badinet_error(__func__); break;
		case BADARGS : raise_badargs_error(__func__); break;
		default : return;
	}
	return;
}

static inline void tryexec_start_data(status_t _stat) {
	switch (_stat) {
		case BADIPV4 : raise_badipv4_error(__func__); break;
		case BADPORT : raise_badport_error(__func__); break;
		case BADARGS : raise_badargs_error(__func__); break;
		case INVSOCK : raise_invsock_error(__func__); break;
		case FAILSET : raise_failset_error(__func__); break;
		case ERRBIND : raise_errbind_error(__func__); break;
		case ERRCONN : raise_errconn_error(__func__); break;
		default : return;
	}
	return;
}

static inline void tryexec_push_handshake(status_t _stat) {
	switch (_stat) {
		case EMALLOC : raise_emalloc_error(__func__); break;
		case FAILURE : raise_failure_error(__func__); break;
		case TIMEOUT : raise_timeout_error(__func__); break;
		case ERRPOLL : raise_errpoll_error(__func__); break;
		case ERRRECV : raise_errrecv_error(__func__); break;
		case ERRSEND : raise_errsend_error(__func__); break;
		case LOWSIZE : raise_lowsize_error(__func__); break;
		case BADTYPE : raise_badtype_error(__func__); break;
		case ZEROACK : raise_zeroack_error(__func__); break;
		default : return;
	}
	return;
}

static inline void tryexec_pull_handshake(status_t _stat) {
	switch (_stat) {
		case FAILURE : raise_failure_error(__func__); break;
		case TIMEOUT : raise_timeout_error(__func__); break;
		case ERRRECV : raise_errrecv_error(__func__); break;
		case ERRSEND : raise_errsend_error(__func__); break;
		case LOWSIZW : raise_lowsize_error(__func__); break;
		case ERRPOLL : raise_errpoll_error(__func__); break;
		case BADTYPE : raise_badtype_error(__func__); break;
		case FAILSET : raise_failset_error(__func__); break;
		default : return;
	}
	return;
}

static inline void tryexec_push_transfer(status_t _stat) {
	switch (_stat) {
		case ZEROSEQ : raise_zeroseq_error(__func__); break;
		case EMALLOC : raise_emalloc_error(__func__); break;
		case FAILURE : raise_failure_error(__func__); break;
		case TIMEOUT : raise_timeout_error(__func__); break;
		case ERRSEND : raise_errsend_error(__func__); break;
		case ERRRECV : raise_errrecv_error(__func__); break;
		case LOWSIZE : raise_lowsize_error(__func__); break;
		case ERRPOLL : raise_errpoll_error(__func__); break;
		case BADTYPE : raise_badtype_error(__func__); break;
		case EXPTRY0 : raise_exptry0_error(__func__); break;
		case EXPTRY1 : raise_exptry1_error(__func__); break;
		case NOMFILE : raise_nomfile_error(__func__); break;
		default : return;
	}
	return;
}

static inline void tryexec_pull_transfer(status_t _stat) {
	switch (_stat) {
		case ZEROSEQ : raise_zeroseq_error(__func__); break;
		case FAILURE : raise_failure_error(__func__); break;
		case TIMEOUT : raise_timeout_error(__func__); break;
		case ERRRECV : raise_errrecv_error(__func__); break;
		case ERRSEND : raise_errsend_error(__func__); break;
		case LOWSIZE : raise_lowsize_error(__func__); break;
		case ERRPOLL : raise_errpoll_error(__func__); break;
		case BADTYPE : raise_badtype_error(__func__); break;
		case FAILSET : raise_failset_error(__func__); break;
		case EXPTRY0 : raise_exptry0_error(__func__); break;
		case BADFLOW : raise_badflow_error(__func__); break;
		default : return;
	}
	return;
}

static inline void tryexec_push_verification(status_t _stat) {
	switch (_stat) {
		case EMALLOC : raise_emalloc_error(__func__); break;
		case FAILURE : raise_failure_error(__func__); break;
		case TIMEOUT : raise_timeout_error(__func__); break;
		case ERRSEND : raise_errsend_error(__func__); break;
		case ERRRECV : raise_errrecv_error(__func__); break;
		case LOWSIZE : raise_lowsize_error(__func__); break;
		case BADTYPE : raise_badtype_error(__func__); break;
		case ERRPOLL : raise_errpoll_error(__func__); break;
		case ZEROACK : raise_zeroack_error(__func__); break;
		default : return;
	}
	return;
}

static inline void tryexec_end_file_stream(status_t _stat) {
	switch (_stat) {
		case FAILURE : raise_failure_error(__func__); break;
		default : return;
	}
	return;
}

static inline void tryexec_end_cntl(status_t _stat) {
	switch (_stat) {
		case FAILURE : raise_failure_error(__func__); break;
		default : return;
	}
	return;
}

static inline void tryexec_end_data(status_t _stat) {
	switch (_stat) {
		case FAILURE : raise_failure_error(__func__); break;
		default : return;
	}
	return;
}

static inline void tryexec_pull_SEND_header(status_t _stat) {
	switch (_stat) {
		case FAILURE : raise_failure_error(__func__); break;
		case ERRRECV : raise_errrecv_error(__func__); break;
		case LOWSIZE : raise_lowsize_error(__func__); break;
		case ERRPOLL : raise_errpoll_error(__func__); break;
		default : return;
	}
	return;
}

static inline void tryexec_push_RECV_header(status_t _stat) {
	switch (_stat) {
		case EMALLOC : raise_emalloc_error(__func__); break;
		case FAILURE : raise_failure_error(__func__); break;
		case TIMEOUT : raise_timeout_error(__func__); break;
		case ERRSEND : raise_errsene_error(__func__); break;
		case LOWSIZE : raise_lowsize_error(__func__); break;
		case ERRPOLL : raise_errpoll_error(__func__); break;
	}
}

#endif
