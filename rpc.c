#include "rpc.h"
#include <stdlib.h>
#include <assert.h>

static inline hg_return_t hg_proc_dirents_t(hg_proc_t proc, void *data)
{
    dirents_t *d = data;

    hg_return_t ret = hg_proc_int32_t(proc, &d->error);
    if (ret != HG_SUCCESS || d->error != 0) return (ret);
    ret = hg_proc_int32_t(proc, &d->n);
    if (ret != HG_SUCCESS) return (ret);
    if (hg_proc_get_op(proc) == HG_DECODE) d->d = malloc(sizeof(dirent_t) * d->n);
    assert(d->d);

    for (int i = 0; i < d->n; ++i) {
        ret = hg_proc_dirent_t(proc, &d->d[i]);
        if (ret != HG_SUCCESS) return (ret);
    }
    if (hg_proc_get_op(proc) == HG_FREE) free(d->d);

    return (ret);
}

static inline hg_return_t hg_proc_read_out_t(hg_proc_t proc, void *data)
{
    read_out_t *d = data;
    hg_return_t ret = hg_proc_int32_t(proc, &d->error);
    if (ret != HG_SUCCESS || d->error != 0) return (ret);
    ret = hg_proc_int64_t(proc, &d->n);
    if (ret != HG_SUCCESS) return (ret);
    if (hg_proc_get_op(proc) == HG_DECODE) d->buf = malloc(d->n);
    assert(d->buf);

    ret = hg_proc_memcpy(proc, d->buf, d->n);
    if (ret != HG_SUCCESS) return (ret);
    if (hg_proc_get_op(proc) == HG_FREE) free(d->buf);

    return (ret);
}

/*
  --- RPC template ---
void rpc_name(hg_handle_t h);
DECLARE_MARGO_RPC_HANDLER(rpc_name)

void rpc_name(hg_handle_t h)
{
    ...process
    return h;
}
DEFINE_MARGO_RPC_HANDLER(rpc_name)
*/
