#ifndef __RPC_H__
#define __RPC_H__

#define TIMEOUT_MSEC (3000)
#define PATH_LEN_MAX (0x100)
#define ADDR_STR_LEN_MAX (0x100)

#include <assert.h>
#include <stdint.h>
#include <margo.h>
#include <time.h>
#include <mercury_proc_string.h>

/* Data Definition */
typedef int error_t;

struct env {
    margo_instance_id mid;
    hg_addr_t addr;
    hg_id_t getattr_rpc, readdir_rpc, open_rpc, read_rpc, read_rdma_rpc;
};

typedef struct timespec timespec_t;

MERCURY_GEN_STRUCT_PROC(timespec_t,
                 ((int64_t)(tv_sec))((int64_t)(tv_nsec)))

MERCURY_GEN_PROC(stat_t,
                 ((int32_t)(error))             \
                 ((uint64_t)(ino))              \
                 ((uint32_t)(mode))             \
                 ((uint64_t)(nlink))            \
                 ((uint32_t)(uid))              \
                 ((uint32_t)(gid))              \
                 ((int64_t)(size))              \
                 ((timespec_t)(atime))          \
                 ((timespec_t)(mtime))          \
                 ((timespec_t)(ctime)))

MERCURY_GEN_PROC(dirent_t,
                 ((uint64_t)(ino))              \
                 ((uint8_t)(type))              \
                 ((hg_string_t)(name)))

struct dirents {
    int32_t error;
    int32_t n;
    dirent_t *d;
};
typedef struct dirents dirents_t;

static inline hg_return_t hg_proc_dirents_t(hg_proc_t proc, void *data)
{
    dirents_t *d = data;

    hg_return_t ret = hg_proc_int32_t(proc, &d->error);
    if (ret != HG_SUCCESS || d->error != 0) return (ret);
    ret = hg_proc_int32_t(proc, &d->n);
    //if (ret != HG_SUCCESS) return (ret);
    if (hg_proc_get_op(proc) == HG_DECODE) d->d = malloc(sizeof(dirent_t) * d->n);
    assert(d->d);

    for (int i = 0; i < d->n; ++i) {
        ret = hg_proc_dirent_t(proc, &d->d[i]);
        //if (ret != HG_SUCCESS) return (ret);
    }
    if (hg_proc_get_op(proc) == HG_FREE) free(d->d);

    return (ret);
}

MERCURY_GEN_PROC(open_in_t,
                 ((hg_const_string_t)(path))    \
                 ((int32_t)(flags)))

MERCURY_GEN_PROC(read_in_t,
                 ((int32_t)(fd))                \
                 ((int64_t)(size))              \
                 ((int64_t)(off)))

typedef struct read_out {
    int32_t error;
    int64_t n;
    char *buf;
} read_out_t;

static inline hg_return_t hg_proc_read_out_t(hg_proc_t proc, void *data)
{
    read_out_t *d = data;
    hg_return_t ret = hg_proc_int32_t(proc, &d->error);
    if (ret != HG_SUCCESS || d->error != 0) return (ret);
    ret = hg_proc_int64_t(proc, &d->n);
    //if (ret != HG_SUCCESS) return (ret);
    if (hg_proc_get_op(proc) == HG_DECODE) d->buf = malloc(d->n);
    assert(d->buf);

    ret = hg_proc_memcpy(proc, d->buf, d->n);
    //if (ret != HG_SUCCESS) return (ret);
    if (hg_proc_get_op(proc) == HG_FREE) free(d->buf);

    return (ret);
}

MERCURY_GEN_PROC(read_rdma_in_t,
                 ((int32_t)(fd))                \
                 ((int64_t)(size))              \
                 ((int64_t)(off))               \
                 ((hg_bulk_t)(buf)))

MERCURY_GEN_PROC(read_rdma_out_t,
                 ((int32_t)(error))             \
                 ((int32_t)(n)))

/* procedure in rpc.c */
void getattr_rpc(hg_handle_t h);
DECLARE_MARGO_RPC_HANDLER(getattr_rpc)
void readdir_rpc(hg_handle_t h);
DECLARE_MARGO_RPC_HANDLER(readdir_rpc)
void open_rpc(hg_handle_t h);
DECLARE_MARGO_RPC_HANDLER(open_rpc)
void read_rpc(hg_handle_t h);
DECLARE_MARGO_RPC_HANDLER(read_rpc)
void read_rdma_rpc(hg_handle_t h);
DECLARE_MARGO_RPC_HANDLER(read_rdma_rpc)

#endif
