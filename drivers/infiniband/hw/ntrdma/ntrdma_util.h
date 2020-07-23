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

#ifndef NTRDMA_UTIL_H
#define NTRDMA_UTIL_H

#include <linux/slab.h>
#include <linux/bitops.h>
#include <linux/log2.h>
#include <linux/spinlock.h>
#include <linux/types.h>

/* Resizable vector */
struct ntrdma_vec {
	/* Capacity of the vec */
	u32				cap;
	/* Key indexed lookup of elements, cap elements long */
	void				**look;
	rwlock_t			lock;
};

/* Allocate an empty vector with a capacity */
int ntrdma_vec_init(struct ntrdma_vec *vec, u32 cap, int node);
/* Destroy a vector */
void ntrdma_vec_deinit(struct ntrdma_vec *vec);

/* Resize a vector if cap is larger than the allocated capacity */
int ntrdma_vec_resize_larger(struct ntrdma_vec *vec, u32 key, int node);

static inline int ntrdma_vec_set(struct ntrdma_vec *vec, u32 key, void *value,
				int node)
{
	int rc;

 again:
	write_lock_bh(&vec->lock);

	if (key >= vec->cap) {
		write_unlock_bh(&vec->lock);
		rc = ntrdma_vec_resize_larger(vec, roundup_pow_of_two(key + 1),
					node);
		if (rc < 0)
			return rc;
		goto again;
	}

	vec->look[key] = value;

	write_unlock_bh(&vec->lock);

	return 0;
}

/* Resizable vector with key reservation */
struct ntrdma_kvec {
	/* Capacity of the vec */
	u32				cap;
	/* Preallocated and never deallocated */
	u32				num_reserved_keys;
	/* Next key to check when reserving */
	u32				next_key;
	/* Bitset of available/used keys, cap bits long */
	unsigned long			*keys;
	/* Key indexed lookup of elements, cap elements long */
	void				**look;
	rwlock_t			lock;
};

/* Allocate an empty vector with a capacity */
int ntrdma_kvec_init(struct ntrdma_kvec *vec, u32 cap, u32 num_reserved_keys,
		int node);
/* Destroy a vector */
void ntrdma_kvec_deinit(struct ntrdma_kvec *vec);
/* Reserve the next available key */
int ntrdma_kvec_reserve_key(struct ntrdma_kvec *vec, int node);

/* Dispose a key that no longer needs to be reserved */
static inline void ntrdma_kvec_dispose_key(struct ntrdma_kvec *vec, u32 key)
{
	write_lock_bh(&vec->lock);

	if (key < vec->num_reserved_keys)
		goto out;

	__clear_bit(key, vec->keys);
 out:
	write_unlock_bh(&vec->lock);
}

static inline void ntrdma_deinit_slab(struct kmem_cache **pslab)
{
	struct kmem_cache *slab = *pslab;

	if (!slab)
		return;

	*pslab = NULL;

	kmem_cache_destroy(slab);
}

#endif

