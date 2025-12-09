#include "file_stream.h"

static status_t create_file(const char *path, size_t size)
{
	status_t _stat = SUCCESS;
	struct statvfs info;
	FILE *file = fopen(path, "w+");
	LOGT("in function create_file()");
	CHECK_PTR(file, NOCREAT);
	if (size == 0) {
		fclose(file);
		return _stat = BADARGS;
	}
	if (fstatvfs(fileno(file), &info) == -1) {
		fclose(file);
		return _stat = NOFSTAT;
	}
	if (info.f_bavail * info.f_bsize <= size) {
		fclose(file);
		return _stat = NOAVAIL;
	}
	if (ftruncate(fileno(file), size) == -1) {
		fclose(file);
		return _stat = NOTRUNC;
	}
	LOGT("return from create_file()");
	return _stat;
}

status_t start_file_stream(FileContext* filec, const char *path, fmode_t mode)
{
	status_t _stat = SUCCESS;
	MFILE mfile;
	char *filename;
	LOGT("in function start_file_stream()");
	if (mode == MWR)
		CHECK_STAT(create_file(path, filec->size));
	mfile = mfopen(path, "r+", PROT_READ | PROT_WRITE, MAP_SHARED);
	CHECK_MFILE(mfile);
	filec->mfile = mfile;
	extract_file_name(filename, path, MAXFILENAMESIZE);
	CHECK_PTR(filename, INVPATH);
	sstrncpy(filec->name, filename, MAXFILENAMESIZE);
	LOGT("return from start_file_stream()");
	return _stat;
}

status_t end_file_stream(FileContext *filec)
{
	status_t _stat = SUCCESS;
	LOGT("in function end_file_stream()");
	CHECK_EQUAL(0, mfclose(&(filec->mfile)), FAILURE);
	LOGT("return from end_file_stream()");
	return _stat;
}
