#define FUSE_USE_VERSION 31
#include <dirent.h>
#include <errno.h>
#include <fuse3/fuse.h>
#include <string.h>
#include <sys/types.h>

int dfuse_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset,
                  struct fuse_file_info *fi, enum fuse_readdir_flags flg)
{
    (void)flg;
    (void)offset;
    (void)fi;
    struct dirent *dent;
    struct stat sb;

    DIR *dp = opendir(path);
    if (dp == NULL) return (-errno);
    while ((dent = readdir(dp)) != NULL) {
        memset(&sb, 0, sizeof(sb));
        sb.st_ino = dent->d_ino;
        sb.st_mode = dent->d_type << 12;
        if (filler(buf, dent->d_name, &sb, 0, FUSE_FILL_DIR_PLUS)) break;
    }
    closedir(dp);
    return (0);
}

