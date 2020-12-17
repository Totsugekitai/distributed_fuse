#define FUSE_USE_VERSION 31
#include <fuse3/fuse.h>
#include <margo.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../common/rpc.h"
#include "../common/rpc_helper.h"

extern struct env env;
extern char server_addr_str[ADDR_STR_LEN_MAX];

int dfs_init(const char *server)
{
    margo_instance_id mid = margo_init("na+sm", MARGO_CLIENT_MODE, 1, 3);
    assert(mid);

    hg_addr_t addr;
    hg_return_t ret = margo_addr_lookup(mid, server, &addr);
    if (ret != HG_SUCCESS) {
        fprintf(stderr, "%s: %s, abort\n", server, HG_Error_to_string(ret));
        exit(1);
    }
    init_rpc(mid, addr);

    return 0;
}

void *dfuse_init(struct fuse_conn_info *conn, struct fuse_config *fconfig)
{
    (void)conn;
    (void)fconfig;

    struct fuse_context *ctx = fuse_get_context();
    const char *server = ctx->private_data;
    strcpy(server_addr_str, server);
    printf("%s\n", server_addr_str);
    dfs_init(server);

    return (NULL);
}
