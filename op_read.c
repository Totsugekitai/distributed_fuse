#define FUSE_USE_VERSION 31
#include <errno.h>
#include <fcntl.h>
#include <fuse3/fuse.h>
#include <sys/stat.h>
#include <unistd.h>

int dfuse_read(const char *path, char *buf, size_t size, off_t off,
               struct fuse_file_info *fi)
{
    int ret, fd;

    if (fi == NULL) {
        fd = open(path, O_RDONLY);
    } else {
        fd = fi->fh;
    }
    if (fd == -1) return (-errno);
    ret = pread(fd, buf, size, off);
    if (ret == -1) ret = -errno;
    if (fi == NULL) close(fd);

    return (ret);
}

