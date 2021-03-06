// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2017 Oracle.  All Rights Reserved.
 * Author: Darrick J. Wong <darrick.wong@oracle.com>
 */
#ifndef	_WORKQUEUE_H_
#define	_WORKQUEUE_H_

#include <pthread.h>

struct workqueue;

typedef void workqueue_func_t(struct workqueue *wq, uint32_t index, void *arg);

struct workqueue_item {
	struct workqueue	*queue;
	struct workqueue_item	*next;
	workqueue_func_t	*function;
	void			*arg;
	uint32_t		index;
};

struct workqueue {
	void			*wq_ctx;
	pthread_t		*threads;
	struct workqueue_item	*next_item;
	struct workqueue_item	*last_item;
	pthread_mutex_t		lock;
	pthread_cond_t		wakeup;
	unsigned int		item_count;
	unsigned int		thread_count;
	bool			terminate;
};

int workqueue_create(struct workqueue *wq, void *wq_ctx,
		unsigned int nr_workers);
int workqueue_add(struct workqueue *wq, workqueue_func_t fn,
		uint32_t index, void *arg);
void workqueue_destroy(struct workqueue *wq);

#endif	/* _WORKQUEUE_H_ */
