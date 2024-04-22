/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright (c) 2021-2024 Oracle.  All Rights Reserved.
 * Author: Darrick J. Wong <djwong@kernel.org>
 */
#ifndef __LIBXFS_XFILE_H__
#define __LIBXFS_XFILE_H__

struct xfile {
	int			fd;
};

int xfile_create(const char *description, struct xfile **xfilep);
void xfile_destroy(struct xfile *xf);

ssize_t xfile_load(struct xfile *xf, void *buf, size_t count, loff_t pos);
ssize_t xfile_store(struct xfile *xf, const void *buf, size_t count, loff_t pos);

unsigned long long xfile_bytes(struct xfile *xf);

#endif /* __LIBXFS_XFILE_H__ */
