#define FUSE_USE_VERSION 31
#include <fuse3/fuse.h>

void *dfuse_init(struct fuse_conn_info *conn, struct fuse_config *fconfig)
{
    (void)conn;
    (void)fconfig;
    return (NULL);
}
