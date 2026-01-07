#include "file_stream.h"

static status_t create_file(const char *path, size_t size)
{
	status_t _stat = SUCCESS;
	struct statvfs info;
	FILE *file = fopen(path, "w+");
	LOGT("in function create_file()");
	CHECK_PTR(file, NOCREAT, "fopen() failed");
	if (size == 0) {
		fclose(file);
		CHECK_STAT(BADARGS, "invalid argument : size must be non-zero");
	}
	if (fstatvfs(fileno(file), &info) == -1) {
		fclose(file);
		CHECK_STAT(NOFSTAT, "fstatvfs() failed");
	}
	if (info.f_bavail * info.f_bsize <= size) {
		fclose(file);
		CHECK_STAT(NOAVAIL, "there is no available space to create file with size %zu", size);
	}
	if (ftruncate(fileno(file), (off_t) size) == -1) {
		fclose(file);
		CHECK_STAT(NOTRUNC, "ftruncate() failed");
	}
	LOGT("return from create_file()");
	return _stat;
}

status_t start_file_stream(FileContext* filec, const char *path, fmode_t mode)
{
	status_t _stat = SUCCESS;
	MFILE mfile;
	struct stat statbuf;
	char *filename;
	LOGT("in function start_file_stream()");
	if (mode == MWR) {
		LOGD("file mode MWR");
		CHECK_STAT(create_file(path, filec->size), "create_file() failed to create a file with size %zu", filec->size);
	}
	else {
		LOGD("file mode MRD");
		CHECK_INT(stat(path, &statbuf), NOFSTAT, "stat() failed");
		filec->size = (size_t) statbuf.st_size;
	}
	LOGD("map the specified file to the memory");
	mfile = mfopen(path, "r+", PROT_READ | PROT_WRITE, MAP_SHARED);
	CHECK_MFILE(mfile, "mfopen() failed");
	filec->mfile = mfile;
	filename = extract_file_name(path, strlen(path) + 1);
	CHECK_PTR(filename, INVPATH, "the specified path is invalid");
	LOGD("file name %s extracted from the specified path", filename);
	sstrncpy(filec->name, filename, MAXFILENAMESIZE);
	LOGT("return from start_file_stream()");
	return _stat;
}

status_t end_file_stream(FileContext *filec)
{
	status_t _stat = SUCCESS;
	LOGT("in function end_file_stream()");
	CHECK_EQUAL(0, mfclose(&filec->mfile), FAILURE, "mfclose() failed");
	LOGT("return from end_file_stream()");
	return _stat;
}
