#include "rpc.h"
#include "rpc_helper.h"
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <margo.h>
#include <mercury_proc_string.h>
#include <sys/stat.h>

struct env env;

void get_self_addr_str(char *addr_str, uint64_t str_size)
{
    hg_addr_t addr;
    margo_addr_self(env.mid, &addr);
    margo_addr_to_string(env.mid, addr_str, &str_size, addr);
    margo_addr_free(env.mid, addr);
}

/*
  --- RPC template ---
void rpc_name(hg_handle_t h);
DECLARE_MARGO_RPC_HANDLER(rpc_name)

void rpc_name(hg_handle_t h)
{
    ...process
}
DEFINE_MARGO_RPC_HANDLER(rpc_name)
*/

/*
  getattr RPCの仕様
  - 入力：パス
  - 出力：stat型の情報
*/
void getattr_rpc(hg_handle_t h)
{
    // クライアントから送られてきた引数の受け取り
    char path[PATH_LEN_MAX] = {'\0'};
    hg_return_t ret = margo_get_input(h, path);
    if (ret != HG_SUCCESS) {
        fprintf(stderr, "[server] getattr RPC get input error\n");
        exit(1);
    }

    // pathからgetattrして情報を取る
    struct stat stbuf = {0};
    errno = 0;
    int err = stat(path, &stbuf);
    if (err == -1) {
        fprintf(stderr, "[server] local getattr failed: %d\n", errno);
        exit(1);
    }
    stat_t st = convert_stat2stat_t(&stbuf, errno);
    margo_free_input(h, &path);

    // クライアント側に情報を返却する
    ret = margo_respond(h, &st);
    if (ret != HG_SUCCESS) {
        fprintf(stderr, "[server] getattr RPC respond to client error\n");
        exit(1);
    }

    // 削除
    ret = margo_destroy(h);
    if (ret != HG_SUCCESS) {
        fprintf(stderr, "[server] getattr RPC destroy error\n");
        exit(1);
    }
}
DEFINE_MARGO_RPC_HANDLER(getattr_rpc)

void readdir_rpc(hg_handle_t h)
{}
DEFINE_MARGO_RPC_HANDLER(readdir_rpc)

void open_rpc(hg_handle_t h)
{}
DEFINE_MARGO_RPC_HANDLER(open_rpc)

void read_rpc(hg_handle_t h)
{}
DEFINE_MARGO_RPC_HANDLER(read_rpc)

void read_rdma_rpc(hg_handle_t h)
{}
DEFINE_MARGO_RPC_HANDLER(read_rdma_rpc)
