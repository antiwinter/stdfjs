/**
 * @file libstdf.c
 * @brief Implementation of all the public libstdf functions.
 * @internal
 */
/*
 * Copyright (C) 2004-2006 Mike Frysinger <vapier@gmail.com>
 * Released under the BSD license.  For more information,
 * please see: http://opensource.org/licenses/bsd-license.php
 *
 * $Header: /cvsroot/freestdf/libstdf/src/libstdf.c,v 1.39 2005/12/10 05:46:55 vapier Exp $
 */

#include <libstdf.h>
#include "dtc.h"
#include "rec.h"



static int __stdf_init(stdf_file *f, dtc_U1 cpu_type, dtc_U1 stdf_ver, uint32_t opts)
{
	switch (cpu_type) {
		case CPU_TYPE_DEC:
			f->byte_order = 0xBEEF;
			warnf("CPU_TYPE_DEC (PDP_ENDIAN) has no implementation");
			break;

		/*case CPU_TYPE_SUN_680XX:*/
		case CPU_TYPE_SPARC:
			f->byte_order = BIG_ENDIAN;
			break;

		/*case CPU_TYPE_SUN_80386:*/
		case CPU_TYPE_X86:
			f->byte_order = LITTLE_ENDIAN;
			break;

#ifdef STDF_VER3
		case CPU_TYPE_LTX:
			if (stdf_ver == 3) {
				warnf("CPU_TYPE_LTX (???_ENDIAN) has no implementation");
				break;
			}
		case CPU_TYPE_APOLLO:
			if (stdf_ver == 3) {
				warnf("CPU_TYPE_APOLLO (???_ENDIAN) has no implementation");
				break;
			}
#endif
		default:
			f->byte_order = __STDF_HOST_BYTE_ORDER;
			break;
	}

	if ((opts & STDF_OPTS_FORCE_V4) || (opts & STDF_OPTS_FORCE) || stdf_ver == 4) {
		f->ver = 4;
#ifdef STDF_VER3
	} else if ((opts & STDF_OPTS_FORCE_V3) || stdf_ver == 3) {
		f->ver = 3;
#endif
	} else {
		warnf("Unable to handle STDF ver%i !", stdf_ver);
		return 1;
	}

	f->opts = opts;

	f->rec_pos = NULL;
	f->rec_end = NULL;

	if (f->opts & STDF_OPTS_WRITE) {
		/* make the buffer big enough to hold the largest record possible */
		f->__output = (byte_t*)malloc(sizeof(byte_t) * (dtc_U2)-1);
		if (f->__output == NULL)
			return 1;
	} else
		f->__output = NULL;
	f->_write_pos = f->__output;
	f->_write_chunk_size = 4096;

	return 0;
}

int stdf_set_setting(stdf_file *f, uint32_t option, ...)
{
	va_list ap;
	dtc_U4 input;

	va_start(ap, option);
	input = va_arg(ap, dtc_U4);
	va_end(ap);

	switch (option) {
		case STDF_SETTING_WRITE_SIZE: f->_write_chunk_size = input; break;
		case STDF_SETTING_VERSION:    f->ver               = input; break;
		case STDF_SETTING_BYTE_ORDER: f->byte_order        = input; break;
	}

	return 0;
}
void stdf_get_setting(stdf_file *f, uint32_t option, ...)
{
	va_list ap;
	dtc_U4 *ret;

	va_start(ap, option);
	ret = va_arg(ap, dtc_U4*);
	va_end(ap);

	switch (option) {
		case STDF_SETTING_WRITE_SIZE: *ret = f->_write_chunk_size; break;
		case STDF_SETTING_VERSION:    *ret = f->ver;               break;
		case STDF_SETTING_BYTE_ORDER: *ret = f->byte_order;        break;
	}
}


/*
 * UNCOMPRESSED SUPPORT
 */
static int __stdf_open_reg(void *data, int flags, uint32_t mode)
{
	stdf_file *stdf = (stdf_file*)data;

	/* if filename is NULL we can assume that the fd is already set ... */
	if (stdf->filename) {
		if (stdf->filename[0] == '-' && stdf->filename[1] == '\0')
			stdf->fd = 0;
		else
			stdf->fd = open(stdf->filename, flags, mode);
	}

	return stdf->fd;
}
static int __stdf_read_reg(void *data, void *buf, long count)
{
	return read(((stdf_file*)data)->fd, buf, count);
}
static int __stdf_close_reg(void *data)
{
	stdf_file *stdf = (stdf_file*)data;
	if (stdf->__data) {
		free(stdf->__data);
		stdf->__data = NULL;
	}
	return close(stdf->fd);
}
static __stdf_fops __stdf_fops_reg = {
	__stdf_open_reg,
	__stdf_read_reg,
	__stdf_close_reg
};



/*
 * ZIP SUPPORT
 */
#if HAVE_ZIP
static int __stdf_open_zip(void *data, int flags, uint32_t mode)
{
	stdf_file *stdf = (stdf_file*)data;

	/* am i doing something wrong or does
	   zziplib really suck this much ? */
	ZZIP_DIR *d;
	ZZIP_DIRENT *de;
	zzip_error_t err;

	stdf->fd_zip = NULL;

	if (__stdf_open_reg(data, flags, mode) == -1)
		return -1;

	d = zzip_dir_fdopen(stdf->fd, &err);
	if (d == NULL)
		return -1;
	de = zzip_readdir(d);
	if (de == NULL) {
		zzip_dir_close(d);
		return -1;
	}
	stdf->fd_zip = zzip_file_open(d, de->d_name, O_RDONLY);
	zzip_dir_close(d);
	if (stdf->fd_zip == NULL)
		return -1;

	return 0;
}
static int __stdf_read_zip(void *data, void *buf, long count)
{
	return zzip_read(((stdf_file*)data)->fd_zip, buf, count);
}
static int __stdf_close_zip(void *data)
{
	stdf_file *stdf = (stdf_file*)data;
	return (stdf->fd_zip==NULL) ? -1 : zzip_close(stdf->fd_zip);
}
static __stdf_fops __stdf_fops_zip = {
	__stdf_open_zip,
	__stdf_read_zip,
	__stdf_close_zip
};
#endif



/*
 * GZIP SUPPORT
 */
#if HAVE_GZIP
static int __stdf_open_gzip(void *data, int flags, uint32_t mode)
{
	stdf_file *stdf = (stdf_file*)data;
	stdf->fd_gzip = NULL;

	if (__stdf_open_reg(data, flags, mode) == -1)
		return -1;

	stdf->fd_gzip = gzdopen(stdf->fd, "rb");
	if (stdf->fd_gzip == NULL)
		return -1;

	return stdf->fd;
}
static int __stdf_read_gzip(void *data, void *buf, long count)
{
	return gzread(((stdf_file*)data)->fd_gzip, buf, count);
}
static int __stdf_close_gzip(void *data)
{
	stdf_file *stdf = (stdf_file*)data;
	if (stdf->fd_gzip != NULL)
		gzclose(stdf->fd_gzip);
	return __stdf_close_reg(data);
}
static __stdf_fops __stdf_fops_gzip = {
	__stdf_open_gzip,
	__stdf_read_gzip,
	__stdf_close_gzip
};
#endif



/*
 * BZIP2 SUPPORT
 */
#if HAVE_BZIP2
#error fjowiefjoijoi
static int __stdf_open_bzip2(void *data, int flags, uint32_t mode)
{
	stdf_file *stdf = (stdf_file*)data;
	stdf->fd_bzip2 = NULL;

	if (__stdf_open_reg(data, flags, mode) == -1)
		return -1;

	stdf->fd_bzip2 = BZ2_bzdopen(stdf->fd, "rb");
	if (stdf->fd_bzip2 == NULL)
		return -1;

	return stdf->fd;
}
static int __stdf_read_bzip2(void *data, void *buf, long count)
{
	return BZ2_bzread(((stdf_file*)data)->fd_bzip2, buf, count);
}
static int __stdf_close_bzip2(void *data)
{
	stdf_file *stdf = (stdf_file*)data;
	if (stdf->fd_bzip2 != NULL)
		BZ2_bzclose(stdf->fd_bzip2);
	return __stdf_close_reg(data);
}
static __stdf_fops __stdf_fops_bzip2 = {
	__stdf_open_bzip2,
	__stdf_read_bzip2,
	__stdf_close_bzip2
};
#endif



/*
 * LZW SUPPORT
 */
#if HAVE_LZW
static int __stdf_open_lzw(void *data, int flags, uint32_t mode)
{
	stdf_file *stdf = (stdf_file*)data;
	stdf->fd_lzw = NULL;

	if (__stdf_open_reg(data, flags, mode) == -1)
		return -1;

	stdf->fd_lzw = lzw_fdopen(stdf->fd);
	if (stdf->fd_lzw == NULL)
		return -1;

	return stdf->fd;
}
static int __stdf_read_lzw(void *data, void *buf, long count)
{
	return lzw_read(((stdf_file*)data)->fd_lzw, buf, count);
}
static int __stdf_close_lzw(void *data)
{
	return lzw_close(((stdf_file*)data)->fd_lzw);
}
static __stdf_fops __stdf_fops_lzw = {
	__stdf_open_lzw,
	__stdf_read_lzw,
	__stdf_close_lzw
};
#endif



static stdf_file* _stdf_open(char *pathname, int fd, uint32_t opts, uint32_t mode)
{
	int flags, ret_errno = EINVAL;
	stdf_file *ret;

	_stdf_mtrace();
	ret = (stdf_file*)malloc(sizeof(stdf_file));

	if (!pathname || pathname[0] == '\0') {
		if (fd == -1) {
			free(ret);
			goto set_errno_and_ret;
		}
		ret->filename = NULL;
		ret->fd = fd;
	} else
		ret->filename = strdup(pathname);
	ret->fops = NULL;

	if (opts == STDF_OPTS_DEFAULT)
		opts = STDF_OPTS_READ;

	if (opts & STDF_OPTS_ZIP)
		ret->file_format = STDF_FORMAT_ZIP;
	else if (opts & STDF_OPTS_GZIP)
		ret->file_format = STDF_FORMAT_GZIP;
	else if (opts & STDF_OPTS_BZIP2)
		ret->file_format = STDF_FORMAT_BZIP2;
	else if (opts & STDF_OPTS_LZW)
		ret->file_format = STDF_FORMAT_LZW;
	else if (ret->filename) {
		/* try to guess from the filename if it's compressed */
		if (strrchr(ret->filename, '.') != NULL) {
			struct {
				char *filetype;
				stdf_format fmt;
			} guesses[] = {
				{ ".zip", STDF_FORMAT_ZIP   },
				{ ".gz",  STDF_FORMAT_GZIP  },
				{ ".Z",   STDF_FORMAT_LZW   },
				{ ".bz",  STDF_FORMAT_BZIP2 },
				{ ".bz2", STDF_FORMAT_BZIP2 },
				{ NULL,   STDF_FORMAT_REG   }
			};
			int i;
			ret->file_format = STDF_FORMAT_REG;
			for (i = 0; guesses[i].filetype; ++i)
				if (strstr(ret->filename, guesses[i].filetype) != NULL) {
					ret->file_format = guesses[i].fmt;
					break;
				}
		} else
			ret->file_format = STDF_FORMAT_REG;
	}

	switch (ret->file_format) {
#if HAVE_ZIP
		case STDF_FORMAT_ZIP:
			ret->fops = &__stdf_fops_zip;
			break;
#endif
#if HAVE_GZIP
		case STDF_FORMAT_GZIP:
			ret->fops = &__stdf_fops_gzip;
			break;
#endif
#if HAVE_BZIP2
		case STDF_FORMAT_BZIP2:
			ret->fops = &__stdf_fops_bzip2;
			break;
#endif
#if HAVE_LZW
		case STDF_FORMAT_LZW:
			ret->fops = &__stdf_fops_lzw;
			break;
#endif
		default:
			warn("format not supported");
			goto out_err;
		case STDF_FORMAT_REG:
			ret->fops = &__stdf_fops_reg;
	}

	flags = O_BINARY;
	if ((opts & STDF_OPTS_READ) && (opts & STDF_OPTS_WRITE))
		flags |= O_RDWR;
	else if (opts & STDF_OPTS_WRITE)
		flags |= O_WRONLY;
	else
		flags |= O_RDONLY;
	if (opts & STDF_OPTS_CREATE)
		flags |= O_CREAT | O_TRUNC;

	if (ret->fops->open(ret, flags, mode) == -1) {
		ret_errno = ENOENT;
		goto out_err;
	}

	if (!(opts & STDF_OPTS_WRITE)) {
		/* try to peek at the FAR record to figure out the CPU type/STDF ver */
		ret->__data = (byte_t*)malloc(6);
		if (ret->fops->read(ret, ret->__data, 6) != 6)
			goto out_err;
		if ((MAKE_REC(ret->__data[2], ret->__data[3]) != REC_FAR)
#ifdef STDF_VER3
		    /* STDF v3 can have either a FAR or a MIR record */
		    && (MAKE_REC(ret->__data[2], ret->__data[3]) != REC_MIR)
#endif
		   )
			goto out_err;
		if (__stdf_init(ret, ret->__data[4], ret->__data[5], opts))
			goto out_err;
	} else {
		if (__stdf_init(ret, -1, 4, opts))
			goto out_err;
	}

	_stdf_muntrace();
	errno = 0;
	return ret;

out_err:
	if (ret->fops)
		ret->fops->close(ret);
	free(ret->filename);
	free(ret);

set_errno_and_ret:
	errno = ret_errno;
	_stdf_muntrace();
	return NULL;
}
stdf_file* stdf_open_ex(char *pathname, uint32_t opts, ...)
{
	uint32_t mode = 0;
	if (opts & STDF_OPTS_CREATE) {
		va_list ap;
		va_start(ap, opts);
		mode = va_arg(ap, uint32_t);
		va_end(ap);
	}
	return _stdf_open(pathname, -1, opts, mode);
}
stdf_file* stdf_open(char *pathname)
{
	return _stdf_open(pathname, -1, STDF_OPTS_DEFAULT, 0);
}
stdf_file* stdf_dopen(int fd)
{
	return _stdf_open(NULL, fd, STDF_OPTS_DEFAULT, 0);
}
stdf_file* stdf_dopen_ex(int fd, uint32_t opts, ...)
{
	uint32_t mode = 0;
	if (opts & STDF_OPTS_CREATE) {
		va_list ap;
		va_start(ap, opts);
		mode = va_arg(ap, uint32_t);
		va_end(ap);
	}
	return _stdf_open(NULL, fd, opts, mode);
}

int stdf_close(stdf_file *file)
{
	int ret, ret_errno;
	_stdf_mtrace();
	if (file->__output) {
		_stdf_write_flush(file, (size_t)-1);
		free(file->__output);
	}
	ret = file->fops->close(file);
	ret_errno = errno;
	if (file->filename) free(file->filename);
	free(file);
	_stdf_muntrace();
	errno = ret_errno;
	return ret;
}

rec_unknown* stdf_read_record_raw(stdf_file *file)
{
	rec_unknown *raw_rec = NULL;
	char header[6];
	int cheated;

	_stdf_mtrace();

	if (!file->__data) {
		/* read the record header to find out how big this next record is */
		if (file->fops->read(file, header, 4) != 4)
			goto ret_null;
		cheated = 0;
	} else {
		memcpy(header, file->__data, 6);
		free(file->__data);
		file->__data = NULL;
		cheated = 1;
	}
	raw_rec = (rec_unknown*)malloc(sizeof(rec_unknown));
	if (raw_rec == NULL) {
		warnfp("malloc.1");
		goto ret_null;
	}
	raw_rec->header.stdf_file = (void*)file;
	raw_rec->header.state = REC_STATE_RAW;
	memcpy(&(raw_rec->header.REC_LEN), header, 2);
	raw_rec->header.REC_TYP = header[2];
	raw_rec->header.REC_SUB = header[3];
	memcpy(&(file->header), &(raw_rec->header), sizeof(rec_header));
	_stdf_byte_order_to_host(file, &(file->header.REC_LEN), sizeof(dtc_U2));

	/* buffer the whole record in memory */
	raw_rec->data = (void*)malloc(file->header.REC_LEN+4);
	if (raw_rec->data == NULL) {
		warnfp("malloc.2");
		free(raw_rec);
		goto ret_null;
	}
	if (cheated) {
		file->fops->read(file, ((byte_t*)raw_rec->data)+6, file->header.REC_LEN-2);
		memcpy(raw_rec->data, header, 6);
	} else {
		file->fops->read(file, ((byte_t*)raw_rec->data)+4, file->header.REC_LEN);
		memcpy(raw_rec->data, header, 4);
	}

	_stdf_muntrace();
	return raw_rec;
ret_null:
	_stdf_muntrace();
	return NULL;
}

rec_unknown* stdf_parse_raw_record(rec_unknown *raw_rec)
{
	rec_unknown *rec;
	stdf_file *file;

	if (!raw_rec)
		return NULL;

	_stdf_mtrace();

	file = (stdf_file*)(raw_rec->header.stdf_file);

	/* setup the buffer so the rest of the library can parse it if need be */
	file->__data = (byte_t*)raw_rec->data;
	file->rec_pos = file->__data + 4;
	file->rec_end = file->rec_pos + file->header.REC_LEN;

	/* record order is based on frequency in 'standard' files
	 * Note: keep order in sync with rec.c
	 */
	switch (HEAD_TO_REC(file->header)) {
		/* REC_TYP_PER_EXEC */
		case REC_PTR: rec = (rec_unknown*)stdf_read_rec_ptr(file); break;
		case REC_FTR: rec = (rec_unknown*)stdf_read_rec_ftr(file); break;
		case REC_MPR: rec = (rec_unknown*)stdf_read_rec_mpr(file); break;

		/* REC_TYP_PER_PART */
		case REC_PIR: rec = (rec_unknown*)stdf_read_rec_pir(file); break;
		case REC_PRR: rec = (rec_unknown*)stdf_read_rec_prr(file); break;

		/* REC_TYP_PER_TEST */
		case REC_TSR: rec = (rec_unknown*)stdf_read_rec_tsr(file); break;
#ifdef STDF_VER3
		case REC_PDR: rec = (rec_unknown*)stdf_read_rec_pdr(file); break;
		case REC_FDR: rec = (rec_unknown*)stdf_read_rec_fdr(file); break;
#endif

		/* REC_TYP_GENERIC */
		case REC_DTR: rec = (rec_unknown*)stdf_read_rec_dtr(file); break;
		case REC_GDR: rec = (rec_unknown*)stdf_read_rec_gdr(file); break;

		/* REC_TYP_PER_PROG */
		case REC_BPS: rec = (rec_unknown*)stdf_read_rec_bps(file); break;
		case REC_EPS: rec = (rec_unknown*)stdf_read_rec_eps(file); break;

		/* REC_TYP_PER_SITE */
#ifdef STDF_VER3
		case REC_SHB: rec = (rec_unknown*)stdf_read_rec_shb(file); break;
		case REC_SSB: rec = (rec_unknown*)stdf_read_rec_ssb(file); break;
		case REC_STS: rec = (rec_unknown*)stdf_read_rec_sts(file); break;
		case REC_SCR: rec = (rec_unknown*)stdf_read_rec_scr(file); break;
#endif

		/* REC_TYP_PER_LOT */
		case REC_PMR: rec = (rec_unknown*)stdf_read_rec_pmr(file); break;
		case REC_PGR: rec = (rec_unknown*)stdf_read_rec_pgr(file); break;
		case REC_HBR: rec = (rec_unknown*)stdf_read_rec_hbr(file); break;
		case REC_SBR: rec = (rec_unknown*)stdf_read_rec_sbr(file); break;
		case REC_PLR: rec = (rec_unknown*)stdf_read_rec_plr(file); break;
		case REC_RDR: rec = (rec_unknown*)stdf_read_rec_rdr(file); break;
		case REC_SDR: rec = (rec_unknown*)stdf_read_rec_sdr(file); break;
		case REC_MIR: rec = (rec_unknown*)stdf_read_rec_mir(file); break;
		case REC_MRR: rec = (rec_unknown*)stdf_read_rec_mrr(file); break;
		case REC_PCR: rec = (rec_unknown*)stdf_read_rec_pcr(file); break;

		/* REC_TYP_PER_WAFER */
		case REC_WIR: rec = (rec_unknown*)stdf_read_rec_wir(file); break;
		case REC_WRR: rec = (rec_unknown*)stdf_read_rec_wrr(file); break;
		case REC_WCR: rec = (rec_unknown*)stdf_read_rec_wcr(file); break;

		/* REC_TYP_INFO */
		case REC_FAR: rec = (rec_unknown*)stdf_read_rec_far(file); break;
		case REC_ATR: rec = (rec_unknown*)stdf_read_rec_atr(file); break;

		default:
			rec = stdf_read_rec_unknown(file);
			file->header.REC_TYP = REC_TYP_UNKNOWN;
			file->header.REC_SUB = REC_SUB_UNKNOWN;
			break;
	}
	file->header.state = REC_STATE_PARSED;
	memcpy(&(rec->header), &(file->header), sizeof(rec_header));

	file->__data = NULL;

	_stdf_muntrace();
	return rec;
}

rec_unknown* stdf_read_record(stdf_file *file)
{
	rec_unknown *raw_rec = NULL,
	            *rec = NULL;

	raw_rec = stdf_read_record_raw(file);
	if (raw_rec == NULL)
		return NULL;
	rec = stdf_parse_raw_record(raw_rec);
	stdf_free_record(raw_rec);

	return rec;
}

ssize_t stdf_write_record(stdf_file *file, void *rec_void)
{
	rec_unknown *rec = (rec_unknown*)rec_void;
	/* record order is based on frequency in 'standard' files
	 * Note: keep order in sync with rec.c
	 */
	switch (HEAD_TO_REC(rec->header)) {
		/* REC_TYP_PER_EXEC */
		case REC_PTR: return stdf_write_rec_ptr(file, (rec_ptr*)rec);
		case REC_FTR: return stdf_write_rec_ftr(file, (rec_ftr*)rec);
		case REC_MPR: return stdf_write_rec_mpr(file, (rec_mpr*)rec);

		/* REC_TYP_PER_PART */
		case REC_PIR: return stdf_write_rec_pir(file, (rec_pir*)rec);
		case REC_PRR: return stdf_write_rec_prr(file, (rec_prr*)rec);

		/* REC_TYP_PER_TEST */
		case REC_TSR: return stdf_write_rec_tsr(file, (rec_tsr*)rec);
#ifdef STDF_VER3
		case REC_PDR: return stdf_write_rec_pdr(file, (rec_pdr*)rec);
		case REC_FDR: return stdf_write_rec_fdr(file, (rec_fdr*)rec);
#endif

		/* REC_TYP_GENERIC */
		case REC_DTR: return stdf_write_rec_dtr(file, (rec_dtr*)rec);
		case REC_GDR: return stdf_write_rec_gdr(file, (rec_gdr*)rec);

		/* REC_TYP_PER_PROG */
		case REC_BPS: return stdf_write_rec_bps(file, (rec_bps*)rec);
		case REC_EPS: return stdf_write_rec_eps(file, (rec_eps*)rec);

		/* REC_TYP_PER_SITE */
#ifdef STDF_VER3
		case REC_SHB: return stdf_write_rec_shb(file, (rec_shb*)rec);
		case REC_SSB: return stdf_write_rec_ssb(file, (rec_ssb*)rec);
		case REC_STS: return stdf_write_rec_sts(file, (rec_sts*)rec);
		case REC_SCR: return stdf_write_rec_scr(file, (rec_scr*)rec);
#endif

		/* REC_TYP_PER_LOT */
		case REC_PMR: return stdf_write_rec_pmr(file, (rec_pmr*)rec);
		case REC_PGR: return stdf_write_rec_pgr(file, (rec_pgr*)rec);
		case REC_HBR: return stdf_write_rec_hbr(file, (rec_hbr*)rec);
		case REC_SBR: return stdf_write_rec_sbr(file, (rec_sbr*)rec);
		case REC_PLR: return stdf_write_rec_plr(file, (rec_plr*)rec);
		case REC_RDR: return stdf_write_rec_rdr(file, (rec_rdr*)rec);
		case REC_SDR: return stdf_write_rec_sdr(file, (rec_sdr*)rec);
		case REC_MIR: return stdf_write_rec_mir(file, (rec_mir*)rec);
		case REC_MRR: return stdf_write_rec_mrr(file, (rec_mrr*)rec);
		case REC_PCR: return stdf_write_rec_pcr(file, (rec_pcr*)rec);

		/* REC_TYP_PER_WAFER */
		case REC_WIR: return stdf_write_rec_wir(file, (rec_wir*)rec);
		case REC_WRR: return stdf_write_rec_wrr(file, (rec_wrr*)rec);
		case REC_WCR: return stdf_write_rec_wcr(file, (rec_wcr*)rec);

		/* REC_TYP_INFO */
		case REC_FAR: return stdf_write_rec_far(file, (rec_far*)rec);
		case REC_ATR: return stdf_write_rec_atr(file, (rec_atr*)rec);

		default:      return -1;
	}
}

#if 0
ssize_t stdf_write_record_r(stdf_file *file, void *rec_void)
{
	ssize_t ret;
	rec_unknown *rec = (rec_unknown*)rec_void;
	unsigned char *reentrant_buffer;
	reentrant_buffer = (unsigned char *)malloc(rec->header.REC_LEN);
	ret = _stdf_write_record(file, rec, reentrant_buffer);
	free(reentrant_buffer);
	return ret;
}
#endif
