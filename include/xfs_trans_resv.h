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
#ifndef	__XFS_TRANS_RESV_H__
#define	__XFS_TRANS_RESV_H__

struct xfs_mount;

/*
 * structure for maintaining pre-calculated transaction reservations.
 */
struct xfs_trans_res {
	uint	tr_logres;	/* log space unit in bytes per log ticket */
	int	tr_logcount;	/* number of log operations per log ticket */
	int	tr_logflags;	/* log flags, currently only used for indicating
				 * a reservation request is permanent or not */
};

struct xfs_trans_resv {
	struct xfs_trans_res	tr_write;	/* extent alloc trans */
	struct xfs_trans_res	tr_itruncate;	/* truncate trans */
	struct xfs_trans_res	tr_rename;	/* rename trans */
	struct xfs_trans_res	tr_link;	/* link trans */
	struct xfs_trans_res	tr_remove;	/* unlink trans */
	struct xfs_trans_res	tr_symlink;	/* symlink trans */
	struct xfs_trans_res	tr_create;	/* create trans */
	struct xfs_trans_res	tr_mkdir;	/* mkdir trans */
	struct xfs_trans_res	tr_ifree;	/* inode free trans */
	struct xfs_trans_res	tr_ichange;	/* inode update trans */
	struct xfs_trans_res	tr_growdata;	/* fs data section grow trans */
	struct xfs_trans_res	tr_swrite;	/* sync write inode trans */
	struct xfs_trans_res	tr_addafork;	/* add inode attr fork trans */
	struct xfs_trans_res	tr_writeid;	/* write setuid/setgid file */
	struct xfs_trans_res	tr_attrinval;	/* attr fork buffer
						 * invalidation */
	struct xfs_trans_res	tr_attrsetm;	/* set/create an attribute at
						 * mount time */
	struct xfs_trans_res	tr_attrsetrt;	/* set/create an attribute at
						 * runtime */
	struct xfs_trans_res	tr_attrrm;	/* remove an attribute */
	struct xfs_trans_res	tr_clearagi;	/* clear agi unlinked bucket */
	struct xfs_trans_res	tr_growrtalloc;	/* grow realtime allocations */
	struct xfs_trans_res	tr_growrtzero;	/* grow realtime zeroing */
	struct xfs_trans_res	tr_growrtfree;	/* grow realtime freeing */
	struct xfs_trans_res	tr_qm_sbchange;	/* change quota flags */
	struct xfs_trans_res	tr_qm_setqlim;	/* adjust quota limits */
	struct xfs_trans_res	tr_qm_dqalloc;	/* allocate quota on disk */
	struct xfs_trans_res	tr_qm_quotaoff;	/* turn quota off */
	struct xfs_trans_res	tr_qm_equotaoff;/* end of turn quota off */
	struct xfs_trans_res	tr_sb;		/* modify superblock */
	struct xfs_trans_res	tr_fsyncts;	/* update timestamps on fsync */
};

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


#define XFS_WRITE_LOG_RES(mp)	((mp)->m_resv.tr_write.tr_logres)
#define XFS_RENAME_LOG_RES(mp)	((mp)->m_resv.tr_rename.tr_logres)
#define XFS_LINK_LOG_RES(mp)	((mp)->m_resv.tr_link.tr_logres)
#define XFS_REMOVE_LOG_RES(mp)	((mp)->m_resv.tr_remove.tr_logres)
#define XFS_SYMLINK_LOG_RES(mp)	((mp)->m_resv.tr_symlink.tr_logres)
#define XFS_CREATE_LOG_RES(mp)	((mp)->m_resv.tr_create.tr_logres)
#define XFS_MKDIR_LOG_RES(mp)	((mp)->m_resv.tr_mkdir.tr_logres)
#define XFS_IFREE_LOG_RES(mp)	((mp)->m_resv.tr_ifree.tr_logres)
#define XFS_SWRITE_LOG_RES(mp)	((mp)->m_resv.tr_swrite.tr_logres)
#define XFS_ICHANGE_LOG_RES(mp)	((mp)->m_resv.tr_ichange.tr_logres)
#define XFS_GROWDATA_LOG_RES(mp)  ((mp)->m_resv.tr_growdata.tr_logres)
#define XFS_ITRUNCATE_LOG_RES(mp) ((mp)->m_resv.tr_itruncate.tr_logres)
#define XFS_GROWRTZERO_LOG_RES(mp) ((mp)->m_resv.tr_growrtzero.tr_logres)
#define XFS_GROWRTFREE_LOG_RES(mp) ((mp)->m_resv.tr_growrtfree.tr_logres)
#define XFS_GROWRTALLOC_LOG_RES(mp) ((mp)->m_resv.tr_growrtalloc.tr_logres)

/*
 * Logging the inode timestamps on an fsync -- same as SWRITE
 * as long as SWRITE logs the entire inode core
 */
#define XFS_FSYNC_TS_LOG_RES(mp) ((mp)->m_resv.tr_fsyncts.tr_logres)
#define XFS_WRITEID_LOG_RES(mp)	 ((mp)->m_resv.tr_writeid.tr_logres)
#define XFS_ADDAFORK_LOG_RES(mp) ((mp)->m_resv.tr_addafork.tr_logres)
#define XFS_ATTRSETM_LOG_RES(mp) ((mp)->m_resv.tr_attrsetm.tr_logres)
#define XFS_ATTRINVAL_LOG_RES(mp) ((mp)->m_resv.tr_attrinval.tr_logres)
#define XFS_ATTRSETRT_LOG_RES(mp) ((mp)->m_resv.tr_attrsetrt.tr_logres)
#define XFS_ATTRRM_LOG_RES(mp)	  ((mp)->m_resv.tr_attrrm.tr_logres)
#define XFS_SB_LOG_RES(mp)	  ((mp)->m_resv.tr_sb.tr_logres)
#define XFS_QM_SETQLIM_LOG_RES(mp) ((mp)->m_resv.tr_qm_setqlim.tr_logres)
#define XFS_QM_DQALLOC_LOG_RES(mp) ((mp)->m_resv.tr_qm_dqalloc.tr_logres)
#define XFS_QM_SBCHANGE_LOG_RES(mp) ((mp)->m_resv.tr_qm_sbchange.tr_logres)
#define XFS_QM_QUOTAOFF_LOG_RES(mp) ((mp)->m_resv.tr_qm_quotaoff.tr_logres)
#define XFS_QM_QUOTAOFF_END_LOG_RES(mp) ((mp)->m_resv.tr_qm_equotaoff.tr_logres)
#define XFS_CLEAR_AGI_BUCKET_LOG_RES(mp) ((mp)->m_resv.tr_clearagi.tr_logres)

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

void xfs_trans_resv_calc(struct xfs_mount *mp, struct xfs_trans_resv *resp);

#endif	/* __XFS_TRANS_RESV_H__ */
