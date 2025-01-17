/*
 * Copyright (c) 2014 Red Hat, Inc.
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

#include "xfs.h"
#include "xfs_shared.h"
#include "xfs_format.h"
#include "xfs_log_format.h"
#include "xfs_trans_resv.h"
#include "xfs_sysfs.h"
#include "xfs_log.h"
#include "xfs_log_priv.h"
#include "xfs_stats.h"
#include "xfs_mount.h"

struct xfs_sysfs_attr {
	struct attribute attr;
	ssize_t (*show)(struct kobject *kobject, char *buf);
	ssize_t (*store)(struct kobject *kobject, const char *buf,
			 size_t count);
};

static inline struct xfs_sysfs_attr *
to_attr(struct attribute *attr)
{
	return container_of(attr, struct xfs_sysfs_attr, attr);
}

#define XFS_SYSFS_ATTR_RW(name) \
	static struct xfs_sysfs_attr xfs_sysfs_attr_##name = __ATTR_RW(name)
#define XFS_SYSFS_ATTR_RO(name) \
	static struct xfs_sysfs_attr xfs_sysfs_attr_##name = __ATTR_RO(name)
#define XFS_SYSFS_ATTR_WO(name) \
	static struct xfs_sysfs_attr xfs_sysfs_attr_##name = __ATTR_WO(name)

#define ATTR_LIST(name) &xfs_sysfs_attr_##name.attr

/*
 * xfs_mount kobject. This currently has no attributes and thus no need for show
 * and store helpers. The mp kobject serves as the per-mount parent object that
 * is identified by the fsname under sysfs.
 */

struct kobj_type xfs_mp_ktype = {
	.release = xfs_sysfs_release,
};

STATIC ssize_t
xfs_sysfs_object_show(
	struct kobject		*kobject,
	struct attribute	*attr,
	char			*buf)
{
	struct xfs_sysfs_attr *xfs_attr = to_attr(attr);

	return xfs_attr->show ? xfs_attr->show(kobject, buf) : 0;
}

STATIC ssize_t
xfs_sysfs_object_store(
	struct kobject		*kobject,
	struct attribute	*attr,
	const char		*buf,
	size_t			count)
{
	struct xfs_sysfs_attr *xfs_attr = to_attr(attr);

	return xfs_attr->store ? xfs_attr->store(kobject, buf, count) : 0;
}

static const struct sysfs_ops xfs_sysfs_ops = {
	.show = xfs_sysfs_object_show,
	.store = xfs_sysfs_object_store,
};

#ifdef DEBUG
/* debug */

STATIC ssize_t
log_recovery_delay_store(
	struct kobject	*kobject,
	const char	*buf,
	size_t		count)
{
	int		ret;
	int		val;

	ret = kstrtoint(buf, 0, &val);
	if (ret)
		return ret;

	if (val < 0 || val > 60)
		return -EINVAL;

	xfs_globals.log_recovery_delay = val;

	return count;
}

STATIC ssize_t
log_recovery_delay_show(
	struct kobject	*kobject,
	char		*buf)
{
	return snprintf(buf, PAGE_SIZE, "%d\n", xfs_globals.log_recovery_delay);
}
XFS_SYSFS_ATTR_RW(log_recovery_delay);

static struct attribute *xfs_dbg_attrs[] = {
	ATTR_LIST(log_recovery_delay),
	NULL,
};

struct kobj_type xfs_dbg_ktype = {
	.release = xfs_sysfs_release,
	.sysfs_ops = &xfs_sysfs_ops,
	.default_attrs = xfs_dbg_attrs,
};

#endif /* DEBUG */

/* stats */

static inline struct xstats *
to_xstats(struct kobject *kobject)
{
	struct xfs_kobj *kobj = to_kobj(kobject);

	return container_of(kobj, struct xstats, xs_kobj);
}

STATIC ssize_t
stats_show(
	struct kobject	*kobject,
	char		*buf)
{
	struct xstats	*stats = to_xstats(kobject);

	return xfs_stats_format(stats->xs_stats, buf);
}
XFS_SYSFS_ATTR_RO(stats);

STATIC ssize_t
stats_clear_store(
	struct kobject	*kobject,
	const char	*buf,
	size_t		count)
{
	int		ret;
	int		val;
	struct xstats	*stats = to_xstats(kobject);

	ret = kstrtoint(buf, 0, &val);
	if (ret)
		return ret;

	if (val != 1)
		return -EINVAL;

	xfs_stats_clearall(stats->xs_stats);
	return count;
}
XFS_SYSFS_ATTR_WO(stats_clear);

static struct attribute *xfs_stats_attrs[] = {
	ATTR_LIST(stats),
	ATTR_LIST(stats_clear),
	NULL,
};

struct kobj_type xfs_stats_ktype = {
	.release = xfs_sysfs_release,
	.sysfs_ops = &xfs_sysfs_ops,
	.default_attrs = xfs_stats_attrs,
};

/* xlog */

static inline struct xlog *
to_xlog(struct kobject *kobject)
{
	struct xfs_kobj *kobj = to_kobj(kobject);

	return container_of(kobj, struct xlog, l_kobj);
}

STATIC ssize_t
log_head_lsn_show(
	struct kobject	*kobject,
	char		*buf)
{
	int cycle;
	int block;
	struct xlog *log = to_xlog(kobject);

	spin_lock(&log->l_icloglock);
	cycle = log->l_curr_cycle;
	block = log->l_curr_block;
	spin_unlock(&log->l_icloglock);

	return snprintf(buf, PAGE_SIZE, "%d:%d\n", cycle, block);
}
XFS_SYSFS_ATTR_RO(log_head_lsn);

STATIC ssize_t
log_tail_lsn_show(
	struct kobject	*kobject,
	char		*buf)
{
	int cycle;
	int block;
	struct xlog *log = to_xlog(kobject);

	xlog_crack_atomic_lsn(&log->l_tail_lsn, &cycle, &block);
	return snprintf(buf, PAGE_SIZE, "%d:%d\n", cycle, block);
}
XFS_SYSFS_ATTR_RO(log_tail_lsn);

STATIC ssize_t
reserve_grant_head_show(
	struct kobject	*kobject,
	char		*buf)

{
	int cycle;
	int bytes;
	struct xlog *log = to_xlog(kobject);

	xlog_crack_grant_head(&log->l_reserve_head.grant, &cycle, &bytes);
	return snprintf(buf, PAGE_SIZE, "%d:%d\n", cycle, bytes);
}
XFS_SYSFS_ATTR_RO(reserve_grant_head);

STATIC ssize_t
write_grant_head_show(
	struct kobject	*kobject,
	char		*buf)
{
	int cycle;
	int bytes;
	struct xlog *log = to_xlog(kobject);

	xlog_crack_grant_head(&log->l_write_head.grant, &cycle, &bytes);
	return snprintf(buf, PAGE_SIZE, "%d:%d\n", cycle, bytes);
}
XFS_SYSFS_ATTR_RO(write_grant_head);

static struct attribute *xfs_log_attrs[] = {
	ATTR_LIST(log_head_lsn),
	ATTR_LIST(log_tail_lsn),
	ATTR_LIST(reserve_grant_head),
	ATTR_LIST(write_grant_head),
	NULL,
};

struct kobj_type xfs_log_ktype = {
	.release = xfs_sysfs_release,
	.sysfs_ops = &xfs_sysfs_ops,
	.default_attrs = xfs_log_attrs,
};

/*
 * Metadata IO error configuration
 *
 * The sysfs structure here is:
 *	...xfs/<dev>/error/<class>/<errno>/<error_attrs>
 *
 * where <class> allows us to discriminate between data IO and metadata IO,
 * and any other future type of IO (e.g. special inode or directory error
 * handling) we care to support.
 */
static inline struct xfs_error_cfg *
to_error_cfg(struct kobject *kobject)
{
	struct xfs_kobj *kobj = to_kobj(kobject);
	return container_of(kobj, struct xfs_error_cfg, kobj);
}

static inline struct xfs_mount *
err_to_mp(struct kobject *kobject)
{
	struct xfs_kobj *kobj = to_kobj(kobject);
	return container_of(kobj, struct xfs_mount, m_error_kobj);
}

static ssize_t
max_retries_show(
	struct kobject	*kobject,
	char		*buf)
{
	struct xfs_error_cfg *cfg = to_error_cfg(kobject);

	return snprintf(buf, PAGE_SIZE, "%d\n", cfg->max_retries);
}

static ssize_t
max_retries_store(
	struct kobject	*kobject,
	const char	*buf,
	size_t		count)
{
	struct xfs_error_cfg *cfg = to_error_cfg(kobject);
	int		ret;
	int		val;

	ret = kstrtoint(buf, 0, &val);
	if (ret)
		return ret;

	if (val < -1)
		return -EINVAL;

	cfg->max_retries = val;
	return count;
}
XFS_SYSFS_ATTR_RW(max_retries);

static ssize_t
retry_timeout_seconds_show(
	struct kobject	*kobject,
	char		*buf)
{
	struct xfs_error_cfg *cfg = to_error_cfg(kobject);

	return snprintf(buf, PAGE_SIZE, "%ld\n",
			jiffies_to_msecs(cfg->retry_timeout) / MSEC_PER_SEC);
}

static ssize_t
retry_timeout_seconds_store(
	struct kobject	*kobject,
	const char	*buf,
	size_t		count)
{
	struct xfs_error_cfg *cfg = to_error_cfg(kobject);
	int		ret;
	int		val;

	ret = kstrtoint(buf, 0, &val);
	if (ret)
		return ret;

	/* 1 day timeout maximum */
	if (val < 0 || val > 86400)
		return -EINVAL;

	cfg->retry_timeout = msecs_to_jiffies(val * MSEC_PER_SEC);
	return count;
}
XFS_SYSFS_ATTR_RW(retry_timeout_seconds);

static ssize_t
fail_at_unmount_show(
	struct kobject	*kobject,
	char		*buf)
{
	struct xfs_mount	*mp = err_to_mp(kobject);

	return snprintf(buf, PAGE_SIZE, "%d\n", mp->m_fail_unmount);
}

static ssize_t
fail_at_unmount_store(
	struct kobject	*kobject,
	const char	*buf,
	size_t		count)
{
	struct xfs_mount	*mp = err_to_mp(kobject);
	int		ret;
	int		val;

	ret = kstrtoint(buf, 0, &val);
	if (ret)
		return ret;

	if (val < 0 || val > 1)
		return -EINVAL;

	mp->m_fail_unmount = val;
	return count;
}
XFS_SYSFS_ATTR_RW(fail_at_unmount);

static struct attribute *xfs_error_attrs[] = {
	ATTR_LIST(max_retries),
	ATTR_LIST(retry_timeout_seconds),
	NULL,
};


struct kobj_type xfs_error_cfg_ktype = {
	.release = xfs_sysfs_release,
	.sysfs_ops = &xfs_sysfs_ops,
	.default_attrs = xfs_error_attrs,
};

struct kobj_type xfs_error_ktype = {
	.release = xfs_sysfs_release,
	.sysfs_ops = &xfs_sysfs_ops,
};

/*
 * Error initialization tables. These need to be ordered in the same
 * order as the enums used to index the array. All class init tables need to
 * define a "default" behaviour as the first entry, all other entries can be
 * empty.
 */
struct xfs_error_init {
	char		*name;
	int		max_retries;
	int		retry_timeout;	/* in seconds */
};

static const struct xfs_error_init xfs_error_meta_init[XFS_ERR_ERRNO_MAX] = {
	{ .name = "default",
	  .max_retries = XFS_ERR_RETRY_FOREVER,
	  .retry_timeout = 0,
	},
	{ .name = "EIO",
	  .max_retries = XFS_ERR_RETRY_FOREVER,
	  .retry_timeout = 0,
	},
	{ .name = "ENOSPC",
	  .max_retries = XFS_ERR_RETRY_FOREVER,
	  .retry_timeout = 0,
	},
	{ .name = "ENODEV",
	  .max_retries = 0,
	},
};

static int
xfs_error_sysfs_init_class(
	struct xfs_mount	*mp,
	int			class,
	const char		*parent_name,
	struct xfs_kobj		*parent_kobj,
	const struct xfs_error_init init[])
{
	struct xfs_error_cfg	*cfg;
	int			error;
	int			i;

	ASSERT(class < XFS_ERR_CLASS_MAX);

	error = xfs_sysfs_init(parent_kobj, &xfs_error_ktype,
				&mp->m_error_kobj, parent_name);
	if (error)
		return error;

	for (i = 0; i < XFS_ERR_ERRNO_MAX; i++) {
		cfg = &mp->m_error_cfg[class][i];
		error = xfs_sysfs_init(&cfg->kobj, &xfs_error_cfg_ktype,
					parent_kobj, init[i].name);
		if (error)
			goto out_error;

		cfg->max_retries = init[i].max_retries;
		cfg->retry_timeout = msecs_to_jiffies(
					init[i].retry_timeout * MSEC_PER_SEC);
	}
	return 0;

out_error:
	/* unwind the entries that succeeded */
	for (i--; i >= 0; i--) {
		cfg = &mp->m_error_cfg[class][i];
		xfs_sysfs_del(&cfg->kobj);
	}
	xfs_sysfs_del(parent_kobj);
	return error;
}

int
xfs_error_sysfs_init(
	struct xfs_mount	*mp)
{
	int			error;

	/* .../xfs/<dev>/error/ */
	error = xfs_sysfs_init(&mp->m_error_kobj, &xfs_error_ktype,
				&mp->m_kobj, "error");
	if (error)
		return error;

	error = sysfs_create_file(&mp->m_error_kobj.kobject,
				  ATTR_LIST(fail_at_unmount));

	if (error)
		goto out_error;

	/* .../xfs/<dev>/error/metadata/ */
	error = xfs_error_sysfs_init_class(mp, XFS_ERR_METADATA,
				"metadata", &mp->m_error_meta_kobj,
				xfs_error_meta_init);
	if (error)
		goto out_error;

	return 0;

out_error:
	xfs_sysfs_del(&mp->m_error_kobj);
	return error;
}

void
xfs_error_sysfs_del(
	struct xfs_mount	*mp)
{
	struct xfs_error_cfg	*cfg;
	int			i, j;

	for (i = 0; i < XFS_ERR_CLASS_MAX; i++) {
		for (j = 0; j < XFS_ERR_ERRNO_MAX; j++) {
			cfg = &mp->m_error_cfg[i][j];

			xfs_sysfs_del(&cfg->kobj);
		}
	}
	xfs_sysfs_del(&mp->m_error_meta_kobj);
	xfs_sysfs_del(&mp->m_error_kobj);
}

struct xfs_error_cfg *
xfs_error_get_cfg(
	struct xfs_mount	*mp,
	int			error_class,
	int			error)
{
	struct xfs_error_cfg	*cfg;

	switch (error) {
	case EIO:
		cfg = &mp->m_error_cfg[error_class][XFS_ERR_EIO];
		break;
	case ENOSPC:
		cfg = &mp->m_error_cfg[error_class][XFS_ERR_ENOSPC];
		break;
	case ENODEV:
		cfg = &mp->m_error_cfg[error_class][XFS_ERR_ENODEV];
		break;
	default:
		cfg = &mp->m_error_cfg[error_class][XFS_ERR_DEFAULT];
		break;
	}

	return cfg;
}
