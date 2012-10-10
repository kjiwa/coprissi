#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE
#endif

#include <errno.h>
#include <fcntl.h>
#include <ftw.h>
#include <libexif/exif-content.h>
#include <libexif/exif-data.h>
#include <libexif/exif-tag.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

int const _FTW_NOPENFD = 10;
char const static* _NAME;
char const static* _DST;

int static _cp(char const* src, char const* dst);
int static _ftw_callback(char const* fpath, struct stat const* sb, int typeflag);
int static _mkdir_recursive(char const* path);
void static _usage();

int static
_cp(const char *to, const char *from)
{
	int fd_to;
	int fd_from;
	char buf[4096];
	ssize_t nread;
	int saved_errno;

	fd_from = open(from, O_RDONLY);
	if (fd_from < 0) {
		return -1;
	}

	fd_to = open(to, O_WRONLY | O_CREAT | O_EXCL, 0666);
	if (fd_to < 0) {
		goto out_error;
	}

	while (nread = read(fd_from, buf, sizeof buf), nread > 0) {
		char *out_ptr = buf;
		ssize_t nwritten;

		do {
			nwritten = write(fd_to, out_ptr, nread);
			if (nwritten >= 0) {
				nread -= nwritten;
				out_ptr += nwritten;
			} else if (errno != EINTR) {
				goto out_error;
			}
		} while (nread > 0);
	}

	if (nread == 0) {
		if (close(fd_to) < 0) {
			fd_to = -1;
			goto out_error;
		}

		close(fd_from);
		return 0;
	}

out_error:
	saved_errno = errno;

	close(fd_from);
	if (fd_to >= 0) {
		close(fd_to);
	}

	errno = saved_errno;
	return -1;
}

int static
_ftw_callback(char const* fpath, struct stat const* sb, int typeflag)
{
	int rc;
	ExifData* exif_data;
	ExifEntry* exif_entry;
	char exif_entry_val[20];
	struct tm tm;

	if (!S_ISREG(sb->st_mode)) {
		return 0;
	}

	exif_data = exif_data_new_from_file(fpath);
	if (exif_data == NULL) {
		return 0;
	}

	rc = 0;
	exif_entry = exif_content_get_entry(*(exif_data->ifd), EXIF_TAG_DATE_TIME);

	if (exif_entry != NULL
	    && exif_entry_get_value(exif_entry, exif_entry_val, 20) != NULL
	    && strptime(exif_entry_val, "%Y:%m:%d %H:%M:%S", &tm) != 0) {
		size_t dst_size;
		char* dst;

		dst_size = strlen(_DST) + 12;
		dst = (char*) malloc(dst_size);

	  if (snprintf(dst, dst_size, "%s/%d/%02d/%02d", _DST, tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday) >= 0 && _mkdir_recursive(dst) == 0) {
			size_t offset;
			char* dst_fpath;

			offset = strlen(fpath);
			while (offset > 0 && fpath[offset - 1] != '/') {
				--offset;
			}

			dst_fpath = (char*) malloc(strlen(dst) + strlen(fpath + offset) + 2);
			sprintf(dst_fpath, "%s/%s", dst, fpath + offset);
			rc = _cp(dst_fpath, fpath);
			free(dst_fpath);

			if (rc == -1 && errno == EEXIST) {
				rc = 0;
			}
		}

		free(dst);
	}

	exif_data_unref(exif_data);
	return rc;
}

int static
_mkdir_recursive(char const* path)
{
	char* tmp;
	size_t size;
	int rc;
	size_t i;

	size = strlen(path);
	tmp = (char*) malloc(size + 1);
	strcpy(tmp, path);

	rc = 0;
	for (i = 1; i < size; ++i) {
		if (tmp[i] == '/') {
			tmp[i] = '\0';
			if (mkdir(tmp, S_IRWXU) != 0 && errno != EEXIST) {
				rc = -1;
				break;
			}

			tmp[i] = '/';
		} else if ((i + 1) == size
		    && mkdir(tmp, S_IRWXU) != 0
		    && errno != EEXIST) {
			rc = -1;
		}
	}

	free(tmp);
	return rc;
}

void static
_usage()
{
	printf("%s <source> <destination>\n", _NAME);
}

int
main(int argc, char** argv)
{
	char const* src;
	struct stat stat_buf;
	int rc;

	_NAME = argv[0];

	if (argc != 3) {
		_usage();
		exit(-1);
	}

	src = argv[1];
	_DST = argv[2];

	if (stat(src, &stat_buf) != 0 || !S_ISDIR(stat_buf.st_mode)) {
		printf("%s is not a directory.\n", src);
		_usage();
		exit(-1);
	}

	if (stat(_DST, &stat_buf) != 0 || !S_ISDIR(stat_buf.st_mode)) {
		printf("%s is not a directory.\n", _DST);
		_usage();
		exit(-1);
	}

	rc = ftw(src, _ftw_callback, _FTW_NOPENFD);

	return rc;
}
