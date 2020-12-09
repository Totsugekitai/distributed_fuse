#include <stdio.h>
#include <assert.h>

#include <margo.h>
#include <mercury_proc_string.h>

#include "../common/rpc.h"

int main(void)
{
    // サーバの初期化
    margo_instance_id mid = margo_init("tcp", MARGO_SERVER_MODE, 1, 10);
    assert(mid);

    // サーバアドレスの取得
    hg_addr_t server_address;
    margo_addr_self(mid, &server_address);
    char addr_str[PATH_MAX];
    size_t addr_str_size = sizeof(addr_str);
    margo_addr_to_string(mid, addr_str, &addr_str_size, server_address);
    //margo_addr_free(mid, my_address);

    printf("Server running at address %s\n", addr_str);

    // RPCの登録
    init_rpc(mid, server_address);

    // メインループ
    margo_wait_for_finalize(mid);

    return 0;

}