#define FUSE_USE_VERSION 31
#include <dirent.h>
#include <errno.h>
#include <fuse3/fuse.h>
#include <string.h>
#include <sys/types.h>

hg_return_t call_readdir_rpc(const char *path, dirents_t *dirents)
{
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
    ret = margo_forward_timed(h, (void *)path, TIMEOUT_MSEC);
    if (ret != HG_SUCCESS) {
        fprintf(stderr, "[client] margo forward readdir RPC error\n");
        exit(1);
    }

    // サーバ側からのレスポンスを受け取る
    ret = margo_get_output(h, dirents);
    if (ret != HG_SUCCESS) {
        fprintf(stderr, "[client] margo get readdir RPC receiving error\n");
        exit(1);
    }

    // 削除
    ret = margo_destroy(h);
    if (ret != HG_SUCCESS) {
        fprintf(stderr, "[client] margo get readdir RPC handle destroy error\n");
        exit(1);
    }

    return ret;
}

int dfuse_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset,
                  struct fuse_file_info *fi, enum fuse_readdir_flags flg)
{
    (void)flg;
    (void)offset;
    (void)fi;
    struct dirent *dent;
    struct stat sb;

    DIR *dp = opendir(path);
    if (dp == NULL) return (-errno);
    while ((dent = readdir(dp)) != NULL) {
        memset(&sb, 0, sizeof(sb));
        sb.st_ino = dent->d_ino;
        sb.st_mode = dent->d_type << 12;
        if (filler(buf, dent->d_name, &sb, 0, FUSE_FILL_DIR_PLUS)) break;
    }
    closedir(dp);
    return (0);
}

