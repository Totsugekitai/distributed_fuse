#include <stdint.h>
#define FUSE_USE_VERSION 31
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <fuse3/fuse.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include <margo.h>
#include <mercury_proc_string.h>

#include "ops.h"
#include "../common/rpc.h"
#include "../common/rpc_helper.h"

extern struct env env;
extern char server_addr_str[ADDR_STR_LEN_MAX];

int call_read_rpc(const char *path, void *buf, size_t size, int off,
                  struct fuse_file_info *fi)
{
    int32_t fd;
    if (fi == NULL) {
        fd = call_open_rpc(path, O_RDONLY);
    } else {
        fd = fi->fh;
    }
    if (fd == -1) return -errno;

    // サーバ側のアドレスをルックアップ
    hg_addr_t server_addr;
    hg_return_t ret = margo_addr_lookup(env.mid, server_addr_str, &server_addr);
    if (ret != HG_SUCCESS) {
        fprintf(stderr, "[client] margo address lookup error\n");
        exit(1);
    }

    // read RPCのハンドルを作成
    hg_handle_t h;
    ret = margo_create(env.mid, server_addr, env.read_rpc, &h);
    if (ret != HG_SUCCESS) {
        fprintf(stderr, "[client] margo read RPC handle create error\n");
        exit(1);
    }

    // read RPCの呼び出し
    printf("[client] sending path in read RPC: %s\n", path);
    read_in_t in;
    in.fd = fd;
    in.size = size;
    in.off = off;
    ret = margo_forward_timed(h, &in, TIMEOUT_MSEC);
    if (ret != HG_SUCCESS) {
        fprintf(stderr, "[client] margo forward read RPC error\n");
        exit(1);
    }

    // サーバ側からのレスポンスを受け取る
    read_out_t out;
    ret = margo_get_output(h, &out);
    if (ret != HG_SUCCESS) {
        fprintf(stderr, "[client] margo get read RPC receiving error\n");
        exit(1);
    }

    int ret_n;
    int err;
    if ((err = out.error) == 0) {
        memcpy(buf, out.buf, out.n);
        ret_n = out.n;
    }

    ret = margo_free_output(h, &out);
    if (ret != HG_SUCCESS) {
        fprintf(stderr, "[client] margo get read RPC free output error\n");
        exit(1);
    }

    // 削除
    ret = margo_destroy(h);
    if (ret != HG_SUCCESS) {
        fprintf(stderr, "[client] margo get read RPC handle destroy error\n");
        exit(1);
    }

    printf("[client] call read RPC end\n");
    if (err == 0) {
        return ret_n;
    } else {
        return -err;
    }
}

int dfuse_read(const char *path, char *buf, size_t size, off_t off,
               struct fuse_file_info *fi)
{
    int ret = call_read_rpc(path, buf, size, off, fi);
    return ret;
}

