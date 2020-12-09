#ifndef __RPC_H__
#define __RPC_H__

#include <stdint.h>
#include <margo.h>
#include <mercury_proc_string.h>

struct env {
    margo_instance_id mid;
    hg_addr_t addr;
    hg_id_t getattr_rpc, readdir_rpc, open_rpc, read_rpc, read_rdma_rpc;
};

struct env env;

MERCURY_GEN_PROC(timespec_t,
                 ((int64_t)(sec))((int32_t)(nsec)))

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
    struct dirents *d;
};
typedef struct dirents dirents_t;

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

MERCURY_GEN_PROC(read_rdma_in_t,
                 ((int32_t)(fd))                \
                 ((int64_t)(size))              \
                 ((int64_t)(off))               \
                 ((hg_bulk_t)(buf)))

MERCURY_GEN_PROC(read_rdma_out_t,
                 ((int32_t)(error))             \
                 ((int32_t)(n)))

#endif
