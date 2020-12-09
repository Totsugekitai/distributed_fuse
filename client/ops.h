#ifndef __OPS_H__
#define __OPS_H__

#define FUSE_USE_VERSION 31

#include <sys/stat.h>
#include <fuse3/fuse.h>

int dfuse_getattr(const char *path, struct stat *st, struct fuse_file_info *fi);
void *dfuse_init(struct fuse_conn_info *conn, struct fuse_config *fconfig);
int dfuse_open(const char *path, struct fuse_file_info *fi);
int dfuse_read(const char *path, char *buf, size_t size, off_t off,
               struct fuse_file_info *fi);
int dfuse_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset,
                  struct fuse_file_info *fi, enum fuse_readdir_flags flg);

#endif
