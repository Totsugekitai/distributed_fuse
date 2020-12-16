#ifndef __RPC_HELPER_H__
#define __RPC_HELPER_H__

#include <sys/stat.h>
#include <margo.h>
#include <mercury_proc_string.h>

#include "rpc.h"

void init_rpc(margo_instance_id mid, hg_addr_t addr);
stat_t convert_stat2stat_t(struct stat *s, error_t e);
void convert_stat_t2stat(stat_t *s, struct stat *new_stat, error_t *e);

#endif
