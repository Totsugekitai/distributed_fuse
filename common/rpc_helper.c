#include <assert.h>
#include <dirent.h>
#include <errno.h>
#include <stdint.h>
#include <sys/stat.h>
#include <margo.h>
#include <mercury_proc_string.h>

#include "rpc.h"
#include "rpc_helper.h"
extern struct env env;

void init_rpc(margo_instance_id mid, hg_addr_t addr)
{
    env.mid = mid;
    env.addr = addr;
    env.getattr_rpc   = MARGO_REGISTER(mid, "getattr",   hg_string_t,    stat_t,          NULL);
    env.readdir_rpc   = MARGO_REGISTER(mid, "readdir",   hg_string_t,    dirents_t,       NULL);
    env.open_rpc      = MARGO_REGISTER(mid, "open",      open_in_t,      int32_t,         NULL);
    env.read_rpc      = MARGO_REGISTER(mid, "read",      read_in_t,      read_out_t,      NULL);
    env.read_rdma_rpc = MARGO_REGISTER(mid, "read_rdma", read_rdma_in_t, read_rdma_out_t, NULL);
}

stat_t convert_stat2stat_t(struct stat *s, error_t e)
{
    stat_t new_stat = {0};
    new_stat.error = e;
    new_stat.ino   = s->st_ino;
    new_stat.mode  = s->st_mode;
    new_stat.nlink = s->st_nlink;
    new_stat.uid   = s->st_uid;
    new_stat.gid   = s->st_gid;
    new_stat.size  = s->st_size;
    new_stat.atime = s->st_atim;
    new_stat.mtime = s->st_mtim;
    new_stat.ctime = s->st_ctim;
    return new_stat;
}

void convert_stat_t2stat(stat_t *s, struct stat *new_stat, error_t *e)
{
    *e = s->error;
    new_stat->st_ino   = s->ino;
    new_stat->st_mode  = s->mode;
    new_stat->st_nlink = s->nlink;
    new_stat->st_uid   = s->uid;
    new_stat->st_gid   = s->gid;
    new_stat->st_size  = s->size;
    new_stat->st_atim  = s->atime;
    new_stat->st_mtim  = s->mtime;
    new_stat->st_ctim  = s->ctime;
}

void convert_dirent2dirent_t(struct dirent *d, dirent_t *newd)
{
    newd->ino = d->d_ino;
    newd->type = d->d_type;
    newd->name = d->d_name;
}
