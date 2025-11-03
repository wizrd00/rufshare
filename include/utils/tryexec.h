#ifndef TRYEXEC_H
#define TRYEXEC_H

#include "error.h"

static inline void tryexec_start_file_stream(status_t stat) {
	switch (stat) {
		case NOCREAT :
			raise_nocreat_error(__func__);
			break;
		case BADARGS :
			raise_badargs_error(__func__);
			break;
		case NOFSTAT :
			raise_nofstat_error(__func__);
			break;
		case NOAVAIL :
			raise_noavail_error(__func__);
			break;
		case NOTRUNC :
			raise_notrunc_error(__func__);
			break;
		case NOCLOSE :
			raise_noclose_error(__func__);
			break;
	}	
}

#endif
