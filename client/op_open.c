#include <stdint.h>
#define FUSE_USE_VERSION 31
#include <errno.h>
#include <fcntl.h>
#include <fuse3/fuse.h>
#include <sys/stat.h>

#include <margo.h>
#include <mercury_proc_string.h>

#include "../common/rpc.h"
#include "../common/rpc_helper.h"

extern struct env env;
extern char server_addr_str[ADDR_STR_LEN_MAX];

int32_t call_open_rpc(const char *path, int flags)
{
    // サーバ側のアドレスをルックアップ
    hg_addr_t server_addr;
    hg_return_t ret = margo_addr_lookup(env.mid, server_addr_str, &server_addr);
    if (ret != HG_SUCCESS) {
        fprintf(stderr, "[client] margo address lookup error\n");
        exit(1);
    }

    // open RPCのハンドルを作成
    hg_handle_t h;
    ret = margo_create(env.mid, server_addr, env.open_rpc, &h);
    if (ret != HG_SUCCESS) {
        fprintf(stderr, "[client] margo open RPC handle create error\n");
        exit(1);
    }

    // getattr RPCの呼び出し
    printf("[client] sending path in open RPC: %s\n", path);
    open_in_t in;
    in.path = path;
    in.flags = flags;
    ret = margo_forward_timed(h, &in, TIMEOUT_MSEC);
    if (ret != HG_SUCCESS) {
        fprintf(stderr, "[client] margo forward open RPC error\n");
        exit(1);
    }

    // サーバ側からのレスポンスを受け取る
    int32_t fd = -1;
    ret = margo_get_output(h, &fd);
    if (ret != HG_SUCCESS) {
        fprintf(stderr, "[client] margo get open RPC receiving error\n");
        exit(1);
    }

    int32_t ret_fd = fd;

    ret = margo_free_output(h, &fd);
    if (ret != HG_SUCCESS) {
        fprintf(stderr, "[client] margo get open RPC free output error\n");
        exit(1);
    }

    // 削除
    ret = margo_destroy(h);
    if (ret != HG_SUCCESS) {
        fprintf(stderr, "[client] margo get open RPC handle destroy error\n");
        exit(1);
    }

    printf("[client] call open RPC end\n");

    return ret_fd;

}

int dfuse_open(const char *path, struct fuse_file_info *fi)
{
    fi->fh = call_open_rpc(path, fi->flags);
    return 0;
}

