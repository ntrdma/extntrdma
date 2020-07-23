/*
 * Copyright (c) 2014, 2015 EMC Corporation.  All rights reserved.
 *
 * This software is available to you under a choice of one of two
 * licenses.  You may choose to be licensed under the terms of the GNU
 * General Public License (GPL) Version 2, available from the file
 * COPYING in the main directory of this source tree, or the
 * OpenIB.org BSD license below:
 *
 *     Redistribution and use in source and binary forms, with or
 *     without modification, are permitted provided that the following
 *     conditions are met:
 *
 *      - Redistributions of source code must retain the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer.
 *
 *      - Redistributions in binary form must reproduce the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer in the documentation and/or other materials
 *        provided with the distribution.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef NTRDMA_MR_H
#define NTRDMA_MR_H

#include <linux/ntc.h>

#include <rdma/ib_verbs.h>

#include "ntrdma_cmd.h"
#include "ntrdma_res.h"
#include "ntrdma_sg.h"

struct ntrdma_dev;
struct ntrdma_mr;

struct ntrdma_mr_cmd_cb {
	struct ntrdma_cmd_cb cb;
	struct ntrdma_mr *mr;
	u32 sg_pos, sg_count;
};

/* Memory Region */
struct ntrdma_mr {
	/* Ofed mr structure */
	struct ib_mr			ibmr;

	/* debugfs */
	struct dentry			*debug;

	/* Ntrdma resource bookkeeping structure */
	struct ntrdma_res		res;

	struct ib_umem			*ib_umem;
	struct ntrdma_mr_cmd_cb		enable_mrcb;
	struct completion		*done;

	u32				pd_key;
	u32				access;

	u64				addr;
	u64				len;

	u32				sg_count;
	struct ntc_mr_buf		sg_list[];
};

#define ntrdma_mr_dev(__mr) ntrdma_res_dev(&(__mr)->res)
#define ntrdma_res_mr(__res) \
	container_of(__res, struct ntrdma_mr, res)
#define ntrdma_ib_mr(__ib) \
	container_of(__ib, struct ntrdma_mr, ibmr)

void ntrdma_mr_enable(struct ntrdma_mr *mr);

/* Remote Memory Region */
struct ntrdma_rmr {
	/* debugfs */
	struct dentry			*debug;

	/* Ntrdma remote resource bookkeeping structure */
	struct ntrdma_rres		rres;
	struct completion		*done;

	u32				pd_key;
	u32				access;

	u64				addr;
	u64				len;

	u32				sg_count;
	struct ntc_remote_buf		sg_list[];
};

#define ntrdma_rmr_dev(__rmr) ntrdma_rres_dev(&(__rmr)->rres)
#define ntrdma_rres_rmr(__rres) \
	container_of(__rres, struct ntrdma_rmr, rres)

int ntrdma_mr_init(struct ntrdma_mr *mr, struct ntrdma_dev *dev);
void ntrdma_mr_deinit(struct ntrdma_mr *mr, struct ntrdma_dev *dev);

static inline void ntrdma_mr_get(struct ntrdma_mr *mr)
{
	ntrdma_res_get(&mr->res);
}

void ntrdma_mr_put(struct ntrdma_mr *mr);

void ntrdma_rmr_init(struct ntrdma_rmr *rmr,
		    struct ntrdma_dev *dev,
		    u32 pd_key, u32 access,
		    u64 addr, u64 len,
		    u32 sg_count, u32 key);

static inline int ntrdma_rmr_add(struct ntrdma_rmr *rmr)
{
	ntrdma_debugfs_rmr_add(rmr);
	return ntrdma_rres_add(&rmr->rres);
}

static inline void ntrdma_rmr_get(struct ntrdma_rmr *rmr)
{
	ntrdma_rres_get(&rmr->rres);
}

void ntrdma_rmr_put(struct ntrdma_rmr *rmr);

struct ntrdma_mr *ntrdma_dev_mr_look(struct ntrdma_dev *dev, u32 key);
struct ntrdma_rmr *ntrdma_dev_rmr_look(struct ntrdma_dev *dev, u32 key);


/* from intel spec, canonical address is when bits 48-63 are identical and equal
 * to the 47th bit (starting count from 0).
 * When 0 its a user-space and 1 is for kernel space.
 * on aslr enabled systems, we can get 1's for user space addresses as well.
 */
#define MASK 0xffff800000000000
static inline bool is_canonical(u64 addr)
{
	return ((addr & MASK) == 0 || (addr & MASK) == MASK);
}
#endif
