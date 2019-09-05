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

#ifndef NTRDMA_SG_H
#define NTRDMA_SG_H

#include <linux/ntc.h>
#include <linux/err.h>
#include <linux/types.h>

struct ntrdma_wr_snd_sge {
	u32				key;
	union {
		/* key != NTRDMA_RESERVED_DMA_LEKY */
		struct {
			u64		addr;
			u32		len;
		};
		/* key == NTRDMA_RESERVED_DMA_LEKY */
		struct ntc_local_buf	snd_dma_buf;
	};
};

struct ntrdma_wr_rcv_sge {
	u32				key;
	union {
		/* key != NTRDMA_RESERVED_DMA_LEKY */
		struct {
			u64		addr;
			u32		len;
		};
		/* key == NTRDMA_RESERVED_DMA_LEKY */
		struct {
			struct ntc_local_buf	rcv_dma_buf;
			struct ntc_export_buf	exp_buf;
			struct ntc_remote_buf_desc desc;
		};
	};
};

#endif
