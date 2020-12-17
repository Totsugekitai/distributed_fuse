#define FUSE_USE_VERSION 31

#include <stddef.h>
#include <stdio.h>
#include <fuse3/fuse.h>
#include "ops.h"
#include "../common/rpc.h"
#include "../common/rpc_helper.h"

char server_addr_str[ADDR_STR_LEN_MAX] = {'\0'};

static const struct fuse_operations dfs_op = {
    .init           = dfuse_init,
    .getattr        = dfuse_getattr,
    .readdir        = dfuse_readdir,
    .open           = dfuse_open,
    .read           = dfuse_read,
};

struct options {
    const char *server;
    int usage;
};

static struct options options;

static void usage(const char *progname) {
    printf("usage: %s [options] <mountpoint>\n\n", progname);
    printf(
        "file-system specific options:\n"
        "    --server=<s>        server name\n"
        "\n");
}

#define OPTION(t, p) { t, offsetof(struct options, p), 1 }
static const struct fuse_opt option_spec[] = {
    OPTION("--server=%s", server),
    OPTION("-h", usage),
    OPTION("--help", usage),
    FUSE_OPT_END
};

int main(int argc, char *argv[])
{
    struct fuse_args args = FUSE_ARGS_INIT(argc, argv);

    options.server = NULL;
    if (fuse_opt_parse(&args, &options, option_spec, NULL) == -1) return (1);

    if (options.usage || options.server == NULL) {
        usage(argv[0]);
        fuse_opt_add_arg(&args, "-ho");
    }

    int ret = fuse_main(args.argc, args.argv, &dfs_op, (void *)options.server);
    fuse_opt_free_args(&args);
    return (ret);
}
