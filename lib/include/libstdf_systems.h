/**
 * @file libstdf_systems.h
 * @brief System specific include files / features.
 */
/*
 * Copyright (C) 2004-2006 Mike Frysinger <vapier@gmail.com>
 * Released under the BSD license.  For more information,
 * please see: http://opensource.org/licenses/bsd-license.php
 *
 * $Header: /cvsroot/freestdf/libstdf/include/libstdf_systems.h,v 1.19 2005/12/08 04:07:07 vapier Exp $
 */

#ifndef _LIBSTDF_SYSTEMS_H
#define _LIBSTDF_SYSTEMS_H

#ifdef WIN32
# include <libstdf_win32.h>
#else
# include <libstdf_sys_config.h>
#endif

#if defined(HAVE_STDINT_H)
# include <stdint.h>
#endif
#if defined(HAVE_INTTYPES_H)
# include <inttypes.h>
#endif
#if defined(HAVE_SYS_TYPES_H)
# include <sys/types.h>
#endif
#if defined(HAVE_SYS_STAT_H)
# include <sys/stat.h>
#endif
#if defined(HAVE_CTYPE_H)
# include <ctype.h>
#endif
#if defined(HAVE_ERRNO_H)
# include <errno.h>
#else
extern int errno;
#endif
#if defined(HAVE_UNISTD_H)
# include <unistd.h>
#endif
#if defined(HAVE_FCNTL_H)
# include <fcntl.h>
#endif
#if defined(HAVE_STDIO_H)
# include <stdio.h>
#endif
#if defined(HAVE_STDLIB_H)
# include <stdlib.h>
#endif
#if defined(HAVE_TIME_H)
# include <time.h>
#endif
#if defined(HAVE_STRING_H)
# include <string.h>
#endif
#if defined(HAVE_FEATURES_H)
# include <features.h>
#endif
#if defined(HAVE_STDARG_H)
# include <stdarg.h>
#endif
#if defined(HAVE_SYS_CDEFS_H)
# include <sys/cdefs.h>
#endif
#if defined(HAVE_ASSERT_H)
# include <assert.h>
#endif

#if defined(HAVE_ENDIAN_H)
# include <endian.h>
#endif
#if defined(HAVE_SYS_ENDIAN_H)
# include <sys/endian.h>
#endif
#if defined(HAVE_MACHINE_ENDIAN_H)
# include <machine/endian.h>
#endif
#if defined(HAVE_SYS_ISA_DEFS_H)
# include <sys/isa_defs.h>
#endif

#if defined(HAVE_BYTESWAP_H)
# include <byteswap.h>
#endif

/* Support for Tru64 */
#if defined(HAVE_CMPLRS_HOST_H)
# include <cmplrs/host.h>
typedef uint8 uint8_t;
typedef uint16 uint16_t;
typedef uint32 uint32_t;
typedef uint64 uint64_t;
typedef int8 int8_t;
typedef int16 int16_t;
typedef int32 int32_t;
typedef int64 int64_t;
#endif

typedef unsigned char uchar;

#if !defined(STDF_FORCE_ENDIAN) && !defined(BYTE_ORDER)

/* Workaround for random systems */
# if defined(WORDS_LITTLENDIAN)
#  define STDF_FORCE_ENDIAN 1234
# elif defined(WORDS_BIGENDIAN)
#  define STDF_FORCE_ENDIAN 4321

/* Workaround for crappy Solaris */
# elif defined(_LITTLE_ENDIAN)
#  define STDF_FORCE_ENDIAN 1234
# elif defined(_BIG_ENDIAN)
#  define STDF_FORCE_ENDIAN 4321

# endif

#endif

#if defined(STDF_FORCE_ENDIAN)
# undef LITTLE_ENDIAN
# undef BIG_ENDIAN
# undef BYTE_ORDER
# define LITTLE_ENDIAN 1234
# define BIG_ENDIAN    4321
# if STDF_FORCE_ENDIAN == LITTLE_ENDIAN
#  define BYTE_ORDER LITTLE_ENDIAN
# else
#  define BYTE_ORDER BIG_ENDIAN
# endif
#endif

#if !defined(BYTE_ORDER) || !defined(LITTLE_ENDIAN) || !defined(BIG_ENDIAN)
# error Unable to detect appropriate endian
# error settings for your system.  Please
# error send a bug report to the
# error freestdf-devel@lists.sourceforge.net
# error mailing list.
# error You can work around the problem
# error by re-running ./configure with
# error the --enable-endian option.
#endif

#if HAVE_ZIP
# include <zzip/zzip.h>
#endif
#if HAVE_GZIP
# include <zlib.h>
#endif
#if HAVE_BZIP2
# include <bzlib.h>
#endif
#if HAVE_LZW
# define __IN_LIBLZW
# include <lzw.h>
#endif

/* i'll see you in hell windows */
#if !defined(O_BINARY)
# define O_BINARY 0x00
#endif

#endif /* _LIBSTDF_SYSTEMS_H */
