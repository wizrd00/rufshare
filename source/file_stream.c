#include "file_stream.h"

static status_t create_file(const char *path, size_t size) {
	status_t _stat = SUCCESS;
	struct statvfs info;
	FILE *file = fopen(path, "w+");
	CHECK_PTR(file, NOCREAT);
	CHECK_NOTEQUAL(0, size, BADARGS);
	CHECK_INT(fstatvfs(fileno(file), &info), NOFSTAT);
	if (info.f_bavail * info.f_bsize <= size)
		return _stat = NOAVAIL;
	CHECK_INT(ftruncate(fileno(file), size), NOTRUNC);
	CHECK_NOTEQUAL(EOF, fclose(file), NOCLOSE);
	return _stat;
}

status_t start_file_stream(FileContext* filec, const char *path, fmode_t mode) {
	status_t _stat = SUCCESS;
	MFILE mfile;
	LOGT(__FILE__, __func__, "start file stream with mode %d", mode);
	if (mode == MWR)
		CHECK_STAT(create_file(path, filec->size));
	mfile = mfopen(path, "r+", PROT_READ | PROT_WRITE, MAP_SHARED);
	CHECK_MFILE(mfile);
	filec->mfile = mfile;
	return _stat;
}

status_t end_file_stream(FileContext *filec) {
	status_t _stat = SUCCESS;
	LOGT(__FILE__, __func__, "end file stream");
	CHECK_EQUAL(0, mfclose(&(filec->mfile)), FAILURE);
	return _stat;
}
