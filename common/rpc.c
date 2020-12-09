#include "rpc.h"
#include <stdlib.h>
#include <assert.h>

//void getattr_rpc(hg_handle_t h);
//DECLARE_MARGO_RPC_HANDLER(getattr_rpc)
//void readdir_rpc(hg_handle_t h);
//DECLARE_MARGO_RPC_HANDLER(readdir_rpc)
//void open_rpc(hg_handle_t h);
//DECLARE_MARGO_RPC_HANDLER(open_rpc)
//void read_rpc(hg_handle_t h);
//DECLARE_MARGO_RPC_HANDLER(read_rpc)
//void read_rdma_rpc(hg_handle_t h);
//DECLARE_MARGO_RPC_HANDLER(read_rdma_rpc)

struct env env;

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

void getattr_rpc(hg_handle_t h)
{}
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
