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
#include <sys/types.h>
#include <dirent.h>

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
    printf("[server] getattr RPC received\n");
    // クライアントから送られてきた引数の受け取り
    // margo_get_inputは内部のシリアライザででmallocしてくれるのでこれで良い
    char *path;
    hg_return_t ret = margo_get_input(h, &path);
    if (ret != HG_SUCCESS) {
        fprintf(stderr, "[server] getattr RPC get input error\n");
        exit(1);
    }

    // pathからgetattrして情報を取る
    printf("[server] got path %s\n", path);
    struct stat stbuf;
    errno = 0;
    int err = stat(path, &stbuf);
    if (err == -1) {
        fprintf(stderr, "[server] local getattr failed: %d\n", errno);
        exit(1);
    }
    stat_t st = convert_stat2stat_t(&stbuf, errno);

    // クライアント側に情報を返却する
    ret = margo_respond(h, &st);
    if (ret != HG_SUCCESS) {
        fprintf(stderr, "[server] getattr RPC respond to client error\n");
        exit(1);
    }
    printf("[server] getattr RPC respond OK\n");

    // 入力引数の解放
    ret = margo_free_input(h, &path);
    if (ret != HG_SUCCESS) {
        fprintf(stderr, "[server] readdir RPC free input error\n");
        exit(1);
    }

    // 削除
    ret = margo_destroy(h);
    if (ret != HG_SUCCESS) {
        fprintf(stderr, "[server] getattr RPC destroy error\n");
        exit(1);
    }
    printf("[server] getattr RPC end\n");
}
DEFINE_MARGO_RPC_HANDLER(getattr_rpc)

void readdir_rpc(hg_handle_t h)
{
    printf("[server] readdir RPC received\n");
    // クライアントから送られてきた引数の受け取り
    char *path;
    hg_return_t ret = margo_get_input(h, &path);
    if (ret != HG_SUCCESS) {
        fprintf(stderr, "[server] readdir RPC get input error\n");
        exit(1);
    }
    printf("[server] readdir RPC get input OK\n");

    // pathからreaddirして情報を得る
    DIR *dirp = opendir(path);
    if (dirp == NULL) {
        fprintf(stderr, "[server] readdir RPC opendir error\n");
        exit(1);
    }
    dirent_t dirent_array[DIRENT_MAX];
    struct dirent *dent = NULL;
    int32_t i = 0;
    while ((dent = readdir(dirp)) != NULL) {
        printf("[server] struct dirent name: %s\n", dent->d_name);
        convert_dirent2dirent_t(dent, &dirent_array[i]);
        i++;
        if (i >= DIRENT_MAX) {
            fprintf(stderr, "error too many !!\n");
            exit(1);
        }
    }

    // dirents_t を構築する
    dirents_t dents;
    dents.error = 0;
    dents.n = i;
    dents.d = dirent_array;

    for (int i = 0; i < dents.n; i++) {
        printf("[server] dents names: [%02d] %s\n", i, dents.d[i].name);
    }

    // クライアント側に情報を返却する
    ret = margo_respond(h, &dents);
    if (ret != HG_SUCCESS) {
        fprintf(stderr, "[server] readdir RPC respond to client error\n");
        exit(1);
    }

    // 入力引数の解放
    ret = margo_free_input(h, &path);
    if (ret != HG_SUCCESS) {
        fprintf(stderr, "[server] readdir RPC free input error\n");
        exit(1);
    }

    // 削除
    ret = margo_destroy(h);
    if (ret != HG_SUCCESS) {
        fprintf(stderr, "[server] readdir RPC destroy error\n");
        exit(1);
    }
    closedir(dirp);
}
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
