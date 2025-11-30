#include "file_stream.h"

static status_t create_file(const char *path, size_t size)
{
	status_t _stat = SUCCESS;
	struct statvfs info;
	FILE *file = fopen(path, "w+");
	CHECK_PTR(file, NOCREAT);
	if (!size) {
		fclose(file);
		return _stat = BADARGS;
	}
	if (!fstatvfs(fileno(file), &info))
		fclose(file);
		return _stat = NOFSTAT;
	}
	if (info.f_bavail * info.f_bsize <= size) {
		fclose(file);
		return _stat = NOAVAIL;
	}
	if (!ftruncate(fileno(file), size))
		fclose(file);
		return _stat = NOTRUNC;
	return _stat;
}

status_t start_file_stream(FileContext* filec, const char *path, fmode_t mode)
{
	status_t _stat = SUCCESS;
	MFILE mfile;
	char *filename;
	if (mode == MWR)
		CHECK_STAT(create_file(path, filec->size));
	mfile = mfopen(path, "r+", PROT_READ | PROT_WRITE, MAP_SHARED);
	CHECK_MFILE(mfile);
	filec->mfile = mfile;
	extract_file_name(filename, path, MAXFILENAMESIZE);
	sstrncpy(filec->name, filename, MAXFILENAMESIZE);
	return _stat;
}

status_t end_file_stream(FileContext *filec)
{
	status_t _stat = SUCCESS;
	LOGT(__FILE__, __func__, "end file stream");
	CHECK_EQUAL(0, mfclose(&(filec->mfile)), FAILURE);
	return _stat;
}
