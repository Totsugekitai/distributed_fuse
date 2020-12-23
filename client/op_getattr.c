#include <string.h>
#define FUSE_USE_VERSION 31
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fuse3/fuse.h>
#include <margo.h>
#include <mercury_proc_string.h>

#include "../common/rpc.h"
#include "../common/rpc_helper.h"

extern struct env env;
extern char server_addr_str[ADDR_STR_LEN_MAX];

void call_getattr_rpc(const char *path, stat_t *st)
{
    // サーバ側のアドレスをルックアップ
    hg_addr_t server_addr;
    hg_return_t ret = margo_addr_lookup(env.mid, server_addr_str, &server_addr);
    if (ret != HG_SUCCESS) {
        fprintf(stderr, "[client] margo address lookup error\n");
        exit(1);
    }

    // getattr RPCのハンドルを作成
    hg_handle_t h;
    ret = margo_create(env.mid, server_addr, env.getattr_rpc, &h);
    if (ret != HG_SUCCESS) {
        fprintf(stderr, "[client] margo getattr RPC handle create error\n");
        exit(1);
    }

    // getattr RPCの呼び出し
    printf("[client] sending path in getattr RPC: %s\n", path);
    ret = margo_forward_timed(h, &path, TIMEOUT_MSEC);
    if (ret != HG_SUCCESS) {
        fprintf(stderr, "[client] margo forward getattr RPC error\n");
        exit(1);
    }

    // サーバ側からのレスポンスを受け取る
    stat_t stat;
    ret = margo_get_output(h, &stat);
    if (ret != HG_SUCCESS) {
        fprintf(stderr, "[client] margo get getattr RPC receiving error\n");
        exit(1);
    }
    *st = stat;

    ret = margo_free_output(h, &stat);
    if (ret != HG_SUCCESS) {
        fprintf(stderr, "[client] margo get getattr RPC free output error\n");
        exit(1);
    }

    // 削除
    ret = margo_destroy(h);
    if (ret != HG_SUCCESS) {
        fprintf(stderr, "[client] margo get getattr RPC handle destroy error\n");
        exit(1);
    }

    printf("[client] call getattr RPC end\n");
}

int dfuse_getattr(const char *path, struct stat *st, struct fuse_file_info *fi)
{
    (void)fi;
    stat_t rpc_return;
    printf("[client] dfuse getattr path: %s\n", path);
    call_getattr_rpc(path, &rpc_return);
    printf("[client] call getattr RPC detach\n");
    error_t e = 0;
    convert_stat_t2stat(&rpc_return, st, &e);
    if (e != 0) {
        fprintf(stderr, "[client] getattr error: %d\n", e);
        exit(1);
    }

    return (0);
}

