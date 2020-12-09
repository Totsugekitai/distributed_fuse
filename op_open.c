#define FUSE_USE_VERSION 31
#include <errno.h>
#include <fcntl.h>
#include <fuse3/fuse.h>
#include <sys/stat.h>

int dfuse_open(const char *path, struct fuse_file_info *fi)
{
    int ret = open(path, fi->flags);
    if (ret == -1) return (-errno);
    fi->fh = ret;

    return (0);
}

