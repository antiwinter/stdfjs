/**
 * @file libstdf_internal.h
 * @brief All the fun internal details no one needs to know about
 * @internal
 */
/*
 * Copyright (C) 2005-2006 Mike Frysinger <vapier@gmail.com>
 * Released under the BSD license.  For more information,
 * please see: http://opensource.org/licenses/bsd-license.php
 *
 * $Header: /cvsroot/freestdf/libstdf/include/libstdf_internal.h,v 1.9 2006/01/08 05:39:38 vapier Exp $
 */

#ifndef _LIBSTDF_INTERNAL_H
#define _LIBSTDF_INTERNAL_H

/* STDF File structure */
#define	__STDF_HOST_BYTE_ORDER		BYTE_ORDER

typedef struct {
	int (*open)(void*, int, uint32_t);
	int (*read)(void*, void*, long);
	int (*close)(void*);
} __stdf_fops;

/**
 * @brief The main STDF file structure.
 */
typedef struct {
	rec_header	header;			/**< A processed version of the last record read */

	int			fd;				/**< Actual file descriptor for the backing file */
	union {
# if HAVE_ZIP
	ZZIP_FILE	*zip;
#  define fd_zip __fd.zip
# endif
# if HAVE_GZIP
	gzFile		*gzip;
#  define fd_gzip __fd.gzip
# endif
# if HAVE_BZIP2
	BZFILE		*bzip2;
#  define fd_bzip2 __fd.bzip2
# endif
# if HAVE_LZW
	lzwFile		*lzw;
#  define fd_lzw __fd.lzw
# endif
	uintptr_t	padding[4];
	} __fd;

	stdf_format	file_format;	/**< Compressed file format */
	char		*filename;		/**< Filename that was given to stdf_open() */
	__stdf_fops	*fops;			/**< Virtual file i/o functions to hide compression details */

	int			byte_order;		/**< Byte order of the file */
	uint32_t	opts;			/**< Misc options to control libstdf behavior */
	dtc_U1		ver;			/**< Spec version of the file */

	byte_t		*__data;
	byte_t		*rec_pos;
	byte_t		*rec_end;

	byte_t		*__output;
	byte_t		*_write_pos;
	dtc_U2		_write_chunk_size;
} stdf_file;



/**
 * @brief Helpful debug macros.
 */
#define _warn(fmt, args...)  fprintf(stderr, fmt , ## args)
#define warn(fmt, args...)   _warn(PACKAGE_STRING ": " fmt "\n" , ## args)
#define warnf(fmt, args...)  warn("%s(): " fmt, __FUNCTION__ , ## args)
#define warnp(fmt, args...)  warn(fmt ": %s" , ## args , strerror(errno))
#define warnfp(fmt, args...) warnp("%s(): " fmt, __FUNCTION__ , ## args)

#define dumphex(buf, cnt) \
	do { \
		size_t i = 0; \
		_warn("dumphex: "); \
		while (i < cnt) \
			_warn("%2.2X ", buf[i++]); \
		_warn("\n"); \
	} while (0)


/**
 * @brief Misc cruft for debugging fun.
 */
#if HAVE_MCHECK_H
# include <mcheck.h>
# define _stdf_mtrace()   mtrace()
# define _stdf_muntrace() muntrace()
#else
# define _stdf_mtrace()
# define _stdf_muntrace()
#endif

#if HAVE_DMALLOC_H
# include <dmalloc.h>
#endif

#if HAVE_EFENCE_H
# include <efence.h>
#endif


/**
 * @brief Internal macros for fun ELF tricks.
 */
#ifndef __GNUC_PREREQ
# define __GNUC_PREREQ(maj, min) 0
#endif
#if __GNUC_PREREQ(3,3) && defined(__ELF__)
# define attribute_unused __attribute__ ((unused))
# if !defined(weak_alias)
#  define weak_alias(name, aliasname) \
	extern __typeof (name) aliasname __attribute__ ((weak, alias (#name)))
# endif
# if !defined(strong_alias)
#  define strong_alias(name, aliasname) \
	extern __typeof (name) aliasname __attribute__ ((alias (#name)))
# endif
# if !defined(attribute_hidden)
#  define attribute_hidden __attribute__ ((visibility ("hidden")))
# endif
#else
# define attribute_unused
# if !defined(attribute_hidden)
#  define attribute_hidden
# endif
#endif

#endif /* _LIBSTDF_INTERNAL_H */
