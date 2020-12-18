#define FUSE_USE_VERSION 31
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <fuse3/fuse.h>
#include <string.h>
#include <sys/types.h>

#include <margo.h>
#include <mercury_proc_string.h>

#include "../common/rpc.h"
#include "../common/rpc_helper.h"

extern struct env env;
extern char server_addr_str[ADDR_STR_LEN_MAX];

void call_readdir_rpc(const char *path, dirents_t *dirents)
{
    printf("[client] call_readdir_rpc attach\n");
    // サーバ側のアドレスをルックアップ
    hg_addr_t server_addr;
    hg_return_t ret = margo_addr_lookup(env.mid, server_addr_str, &server_addr);
    if (ret != HG_SUCCESS) {
        fprintf(stderr, "[client] margo address lookup error\n");
        exit(1);
    }

    // readdir RPCのハンドルを作成
    hg_handle_t h;
    ret = margo_create(env.mid, server_addr, env.readdir_rpc, &h);
    if (ret != HG_SUCCESS) {
        fprintf(stderr, "[client] margo readdir RPC handle create error\n");
        exit(1);
    }

    // readdir RPCの呼び出し
    ret = margo_forward_timed(h, &path, TIMEOUT_MSEC);
    if (ret != HG_SUCCESS) {
        fprintf(stderr, "[client] margo forward readdir RPC error\n");
        exit(1);
    }

    // サーバ側からのレスポンスを受け取る
    dirents_t ds;
    ret = margo_get_output(h, &ds);
    if (ret != HG_SUCCESS) {
        fprintf(stderr, "[client] margo get readdir RPC receiving error\n");
        exit(1);
    }
    *dirents = ds;

    ret = margo_free_output(h, &ds);
    if (ret != HG_SUCCESS) {
        fprintf(stderr, "[client] margo get readdir RPC free output error\n");
        exit(1);
    }

    // 削除
    ret = margo_destroy(h);
    if (ret != HG_SUCCESS) {
        fprintf(stderr, "[client] margo get readdir RPC handle destroy error\n");
        exit(1);
    }
    printf("[client] call_readdir_rpc end\n");
}

int dfuse_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset,
                  struct fuse_file_info *fi, enum fuse_readdir_flags flg)
{
    (void)flg;
    (void)offset;
    (void)fi;
    //struct dirent *dent;
    struct stat sb;
    dirents_t dirents;

    call_readdir_rpc(path, &dirents);
    printf("[client] call_readdir_rpc detach\n");
    if (dirents.error != 0) {
        fprintf(stderr, "[client] readdir dirents error number is not zero\n");
        return -dirents.error;
    }

    for (int32_t i = 0; i < dirents.n; i++) {
        memset(&sb, 0, sizeof(sb));
        sb.st_ino = dirents.d[i].ino;
        sb.st_mode = dirents.d[i].type << 12;
        if (filler(buf, dirents.d[i].name, &sb, 0, FUSE_FILL_DIR_PLUS)) break;
    }

    //while ((dent = readdir(dp)) != NULL) {
    //    memset(&sb, 0, sizeof(sb));
    //    sb.st_ino = dent->d_ino;
    //    sb.st_mode = dent->d_type << 12;
    //    if (filler(buf, dent->d_name, &sb, 0, FUSE_FILL_DIR_PLUS)) break;
    //}
    //closedir(dp);
    return (0);
}

