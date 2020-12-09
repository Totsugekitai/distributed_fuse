#define FUSE_USE_VERSION 31
#include <errno.h>
#include <sys/stat.h>
#include <fuse3/fuse.h>

int dfuse_getattr(const char *path, struct stat *st, struct fuse_file_info *fi)
{
    (void)fi;
    int ret = lstat(path, st);
    if (ret == -1) return (-errno);

    return (0);
}

