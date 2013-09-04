/*
 * Copyright (c) 2000-2002,2005 Silicon Graphics, Inc.
 * All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it would be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write the Free Software Foundation,
 * Inc.,  51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
#ifndef	__XFS_TRANS_H__
#define	__XFS_TRANS_H__


struct xfs_log_item;

/*
 * Per-extent log reservation for the allocation btree changes
 * involved in freeing or allocating an extent.
 * 2 trees * (2 blocks/level * max depth - 1) * block size
 */
#define	XFS_ALLOCFREE_LOG_RES(mp,nx) \
	((nx) * (2 * XFS_FSB_TO_B((mp), 2 * XFS_AG_MAXLEVELS(mp) - 1)))
#define	XFS_ALLOCFREE_LOG_COUNT(mp,nx) \
	((nx) * (2 * (2 * XFS_AG_MAXLEVELS(mp) - 1)))

/*
 * Per-directory log reservation for any directory change.
 * dir blocks: (1 btree block per level + data block + free block) * dblock size
 * bmap btree: (levels + 2) * max depth * block size
 * v2 directory blocks can be fragmented below the dirblksize down to the fsb
 * size, so account for that in the DAENTER macros.
 */
#define	XFS_DIROP_LOG_RES(mp)	\
	(XFS_FSB_TO_B(mp, XFS_DAENTER_BLOCKS(mp, XFS_DATA_FORK)) + \
	 (XFS_FSB_TO_B(mp, XFS_DAENTER_BMAPS(mp, XFS_DATA_FORK) + 1)))
#define	XFS_DIROP_LOG_COUNT(mp)	\
	(XFS_DAENTER_BLOCKS(mp, XFS_DATA_FORK) + \
	 XFS_DAENTER_BMAPS(mp, XFS_DATA_FORK) + 1)


#define	XFS_WRITE_LOG_RES(mp)	((mp)->m_reservations.tr_write)
#define	XFS_ITRUNCATE_LOG_RES(mp)   ((mp)->m_reservations.tr_itruncate)
#define	XFS_RENAME_LOG_RES(mp)	((mp)->m_reservations.tr_rename)
#define	XFS_LINK_LOG_RES(mp)	((mp)->m_reservations.tr_link)
#define	XFS_REMOVE_LOG_RES(mp)	((mp)->m_reservations.tr_remove)
#define	XFS_SYMLINK_LOG_RES(mp)	((mp)->m_reservations.tr_symlink)
#define	XFS_CREATE_LOG_RES(mp)	((mp)->m_reservations.tr_create)
#define	XFS_MKDIR_LOG_RES(mp)	((mp)->m_reservations.tr_mkdir)
#define	XFS_IFREE_LOG_RES(mp)	((mp)->m_reservations.tr_ifree)
#define	XFS_ICHANGE_LOG_RES(mp)	((mp)->m_reservations.tr_ichange)
#define	XFS_GROWDATA_LOG_RES(mp)    ((mp)->m_reservations.tr_growdata)
#define	XFS_GROWRTALLOC_LOG_RES(mp)	((mp)->m_reservations.tr_growrtalloc)
#define	XFS_GROWRTZERO_LOG_RES(mp)	((mp)->m_reservations.tr_growrtzero)
#define	XFS_GROWRTFREE_LOG_RES(mp)	((mp)->m_reservations.tr_growrtfree)
#define	XFS_SWRITE_LOG_RES(mp)	((mp)->m_reservations.tr_swrite)
/*
 * Logging the inode timestamps on an fsync -- same as SWRITE
 * as long as SWRITE logs the entire inode core
 */
#define XFS_FSYNC_TS_LOG_RES(mp)        ((mp)->m_reservations.tr_swrite)
#define	XFS_WRITEID_LOG_RES(mp)	((mp)->m_reservations.tr_swrite)
#define	XFS_ADDAFORK_LOG_RES(mp)	((mp)->m_reservations.tr_addafork)
#define	XFS_ATTRINVAL_LOG_RES(mp)	((mp)->m_reservations.tr_attrinval)
#define	XFS_ATTRSETM_LOG_RES(mp)	((mp)->m_reservations.tr_attrsetm)
#define	XFS_ATTRSETRT_LOG_RES(mp)	((mp)->m_reservations.tr_attrsetrt)
#define	XFS_ATTRRM_LOG_RES(mp)		((mp)->m_reservations.tr_attrrm)
#define	XFS_CLEAR_AGI_BUCKET_LOG_RES(mp)  ((mp)->m_reservations.tr_clearagi)


/*
 * Various log count values.
 */
#define	XFS_DEFAULT_LOG_COUNT		1
#define	XFS_DEFAULT_PERM_LOG_COUNT	2
#define	XFS_ITRUNCATE_LOG_COUNT		2
#define XFS_INACTIVE_LOG_COUNT		2
#define	XFS_CREATE_LOG_COUNT		2
#define	XFS_MKDIR_LOG_COUNT		3
#define	XFS_SYMLINK_LOG_COUNT		3
#define	XFS_REMOVE_LOG_COUNT		2
#define	XFS_LINK_LOG_COUNT		2
#define	XFS_RENAME_LOG_COUNT		2
#define	XFS_WRITE_LOG_COUNT		2
#define	XFS_ADDAFORK_LOG_COUNT		2
#define	XFS_ATTRINVAL_LOG_COUNT		1
#define	XFS_ATTRSET_LOG_COUNT		3
#define	XFS_ATTRRM_LOG_COUNT		3

#ifdef __KERNEL__

struct xfs_buf;
struct xfs_buftarg;
struct xfs_efd_log_item;
struct xfs_efi_log_item;
struct xfs_inode;
struct xfs_item_ops;
struct xfs_log_iovec;
struct xfs_log_item_desc;
struct xfs_mount;
struct xfs_trans;
struct xfs_dquot_acct;
struct xfs_busy_extent;

typedef struct xfs_log_item {
	struct list_head		li_ail;		/* AIL pointers */
	xfs_lsn_t			li_lsn;		/* last on-disk lsn */
	struct xfs_log_item_desc	*li_desc;	/* ptr to current desc*/
	struct xfs_mount		*li_mountp;	/* ptr to fs mount */
	struct xfs_ail			*li_ailp;	/* ptr to AIL */
	uint				li_type;	/* item type */
	uint				li_flags;	/* misc flags */
	struct xfs_log_item		*li_bio_list;	/* buffer item list */
	void				(*li_cb)(struct xfs_buf *,
						 struct xfs_log_item *);
							/* buffer item iodone */
							/* callback func */
	const struct xfs_item_ops	*li_ops;	/* function list */

	/* delayed logging */
	struct list_head		li_cil;		/* CIL pointers */
	struct xfs_log_vec		*li_lv;		/* active log vector */
	xfs_lsn_t			li_seq;		/* CIL commit seq */
} xfs_log_item_t;

#define	XFS_LI_IN_AIL	0x1
#define XFS_LI_ABORTED	0x2

#define XFS_LI_FLAGS \
	{ XFS_LI_IN_AIL,	"IN_AIL" }, \
	{ XFS_LI_ABORTED,	"ABORTED" }

struct xfs_item_ops {
	uint (*iop_size)(xfs_log_item_t *);
	void (*iop_format)(xfs_log_item_t *, struct xfs_log_iovec *);
	void (*iop_pin)(xfs_log_item_t *);
	void (*iop_unpin)(xfs_log_item_t *, int remove);
	uint (*iop_push)(struct xfs_log_item *, struct list_head *);
	void (*iop_unlock)(xfs_log_item_t *);
	xfs_lsn_t (*iop_committed)(xfs_log_item_t *, xfs_lsn_t);
	void (*iop_committing)(xfs_log_item_t *, xfs_lsn_t);
};

#define IOP_SIZE(ip)		(*(ip)->li_ops->iop_size)(ip)
#define IOP_FORMAT(ip,vp)	(*(ip)->li_ops->iop_format)(ip, vp)
#define IOP_PIN(ip)		(*(ip)->li_ops->iop_pin)(ip)
#define IOP_UNPIN(ip, remove)	(*(ip)->li_ops->iop_unpin)(ip, remove)
#define IOP_PUSH(ip, list)	(*(ip)->li_ops->iop_push)(ip, list)
#define IOP_UNLOCK(ip)		(*(ip)->li_ops->iop_unlock)(ip)
#define IOP_COMMITTED(ip, lsn)	(*(ip)->li_ops->iop_committed)(ip, lsn)
#define IOP_COMMITTING(ip, lsn) (*(ip)->li_ops->iop_committing)(ip, lsn)

/*
 * Return values for the IOP_PUSH() routines.
 */
#define XFS_ITEM_SUCCESS	0
#define XFS_ITEM_PINNED		1
#define XFS_ITEM_LOCKED		2
#define XFS_ITEM_FLUSHING	3

/*
 * This is the type of function which can be given to xfs_trans_callback()
 * to be called upon the transaction's commit to disk.
 */
typedef void (*xfs_trans_callback_t)(struct xfs_trans *, void *);

/*
 * This is the structure maintained for every active transaction.
 */
typedef struct xfs_trans {
	unsigned int		t_magic;	/* magic number */
	xfs_log_callback_t	t_logcb;	/* log callback struct */
	unsigned int		t_type;		/* transaction type */
	unsigned int		t_log_res;	/* amt of log space resvd */
	unsigned int		t_log_count;	/* count for perm log res */
	unsigned int		t_blk_res;	/* # of blocks resvd */
	unsigned int		t_blk_res_used;	/* # of resvd blocks used */
	unsigned int		t_rtx_res;	/* # of rt extents resvd */
	unsigned int		t_rtx_res_used;	/* # of resvd rt extents used */
	struct xlog_ticket	*t_ticket;	/* log mgr ticket */
	xfs_lsn_t		t_lsn;		/* log seq num of start of
						 * transaction. */
	xfs_lsn_t		t_commit_lsn;	/* log seq num of end of
						 * transaction. */
	struct xfs_mount	*t_mountp;	/* ptr to fs mount struct */
	struct xfs_dquot_acct   *t_dqinfo;	/* acctg info for dquots */
	unsigned int		t_flags;	/* misc flags */
	int64_t			t_icount_delta;	/* superblock icount change */
	int64_t			t_ifree_delta;	/* superblock ifree change */
	int64_t			t_fdblocks_delta; /* superblock fdblocks chg */
	int64_t			t_res_fdblocks_delta; /* on-disk only chg */
	int64_t			t_frextents_delta;/* superblock freextents chg*/
	int64_t			t_res_frextents_delta; /* on-disk only chg */
#ifdef DEBUG
	int64_t			t_ag_freeblks_delta; /* debugging counter */
	int64_t			t_ag_flist_delta; /* debugging counter */
	int64_t			t_ag_btree_delta; /* debugging counter */
#endif
	int64_t			t_dblocks_delta;/* superblock dblocks change */
	int64_t			t_agcount_delta;/* superblock agcount change */
	int64_t			t_imaxpct_delta;/* superblock imaxpct change */
	int64_t			t_rextsize_delta;/* superblock rextsize chg */
	int64_t			t_rbmblocks_delta;/* superblock rbmblocks chg */
	int64_t			t_rblocks_delta;/* superblock rblocks change */
	int64_t			t_rextents_delta;/* superblocks rextents chg */
	int64_t			t_rextslog_delta;/* superblocks rextslog chg */
	struct list_head	t_items;	/* log item descriptors */
	xfs_trans_header_t	t_header;	/* header for in-log trans */
	struct list_head	t_busy;		/* list of busy extents */
	unsigned long		t_pflags;	/* saved process flags state */
} xfs_trans_t;

/*
 * XFS transaction mechanism exported interfaces that are
 * actually macros.
 */
#define	xfs_trans_get_log_res(tp)	((tp)->t_log_res)
#define	xfs_trans_get_log_count(tp)	((tp)->t_log_count)
#define	xfs_trans_get_block_res(tp)	((tp)->t_blk_res)
#define	xfs_trans_set_sync(tp)		((tp)->t_flags |= XFS_TRANS_SYNC)

#ifdef DEBUG
#define	xfs_trans_agblocks_delta(tp, d)	((tp)->t_ag_freeblks_delta += (int64_t)d)
#define	xfs_trans_agflist_delta(tp, d)	((tp)->t_ag_flist_delta += (int64_t)d)
#define	xfs_trans_agbtree_delta(tp, d)	((tp)->t_ag_btree_delta += (int64_t)d)
#else
#define	xfs_trans_agblocks_delta(tp, d)
#define	xfs_trans_agflist_delta(tp, d)
#define	xfs_trans_agbtree_delta(tp, d)
#endif

/*
 * XFS transaction mechanism exported interfaces.
 */
xfs_trans_t	*xfs_trans_alloc(struct xfs_mount *, uint);
xfs_trans_t	*_xfs_trans_alloc(struct xfs_mount *, uint, xfs_km_flags_t);
xfs_trans_t	*xfs_trans_dup(xfs_trans_t *);
int		xfs_trans_reserve(xfs_trans_t *, uint, uint, uint,
				  uint, uint);
void		xfs_trans_mod_sb(xfs_trans_t *, uint, int64_t);

struct xfs_buf	*xfs_trans_get_buf_map(struct xfs_trans *tp,
				       struct xfs_buftarg *target,
				       struct xfs_buf_map *map, int nmaps,
				       uint flags);

static inline struct xfs_buf *
xfs_trans_get_buf(
	struct xfs_trans	*tp,
	struct xfs_buftarg	*target,
	xfs_daddr_t		blkno,
	int			numblks,
	uint			flags)
{
	DEFINE_SINGLE_BUF_MAP(map, blkno, numblks);
	return xfs_trans_get_buf_map(tp, target, &map, 1, flags);
}

int		xfs_trans_read_buf_map(struct xfs_mount *mp,
				       struct xfs_trans *tp,
				       struct xfs_buftarg *target,
				       struct xfs_buf_map *map, int nmaps,
				       xfs_buf_flags_t flags,
				       struct xfs_buf **bpp,
				       const struct xfs_buf_ops *ops);

static inline int
xfs_trans_read_buf(
	struct xfs_mount	*mp,
	struct xfs_trans	*tp,
	struct xfs_buftarg	*target,
	xfs_daddr_t		blkno,
	int			numblks,
	xfs_buf_flags_t		flags,
	struct xfs_buf		**bpp,
	const struct xfs_buf_ops *ops)
{
	DEFINE_SINGLE_BUF_MAP(map, blkno, numblks);
	return xfs_trans_read_buf_map(mp, tp, target, &map, 1,
				      flags, bpp, ops);
}

struct xfs_buf	*xfs_trans_getsb(xfs_trans_t *, struct xfs_mount *, int);

void		xfs_trans_brelse(xfs_trans_t *, struct xfs_buf *);
void		xfs_trans_bjoin(xfs_trans_t *, struct xfs_buf *);
void		xfs_trans_bhold(xfs_trans_t *, struct xfs_buf *);
void		xfs_trans_bhold_release(xfs_trans_t *, struct xfs_buf *);
void		xfs_trans_binval(xfs_trans_t *, struct xfs_buf *);
void		xfs_trans_inode_buf(xfs_trans_t *, struct xfs_buf *);
void		xfs_trans_stale_inode_buf(xfs_trans_t *, struct xfs_buf *);
void		xfs_trans_dquot_buf(xfs_trans_t *, struct xfs_buf *, uint);
void		xfs_trans_inode_alloc_buf(xfs_trans_t *, struct xfs_buf *);
void		xfs_trans_ichgtime(struct xfs_trans *, struct xfs_inode *, int);
void		xfs_trans_ijoin(struct xfs_trans *, struct xfs_inode *, uint);
void		xfs_trans_log_buf(xfs_trans_t *, struct xfs_buf *, uint, uint);
void		xfs_trans_log_inode(xfs_trans_t *, struct xfs_inode *, uint);
struct xfs_efi_log_item	*xfs_trans_get_efi(xfs_trans_t *, uint);
void		xfs_efi_release(struct xfs_efi_log_item *, uint);
void		xfs_trans_log_efi_extent(xfs_trans_t *,
					 struct xfs_efi_log_item *,
					 xfs_fsblock_t,
					 xfs_extlen_t);
struct xfs_efd_log_item	*xfs_trans_get_efd(xfs_trans_t *,
				  struct xfs_efi_log_item *,
				  uint);
void		xfs_trans_log_efd_extent(xfs_trans_t *,
					 struct xfs_efd_log_item *,
					 xfs_fsblock_t,
					 xfs_extlen_t);
int		xfs_trans_commit(xfs_trans_t *, uint flags);
void		xfs_trans_cancel(xfs_trans_t *, int);
int		xfs_trans_ail_init(struct xfs_mount *);
void		xfs_trans_ail_destroy(struct xfs_mount *);

extern kmem_zone_t	*xfs_trans_zone;
extern kmem_zone_t	*xfs_log_item_desc_zone;

#endif	/* __KERNEL__ */

void		xfs_trans_init(struct xfs_mount *);
int		xfs_trans_roll(struct xfs_trans **, struct xfs_inode *);

#endif	/* __XFS_TRANS_H__ */
