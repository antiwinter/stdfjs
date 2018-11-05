/**
 * @file libstdf.h
 * @brief Top level include and the main stdf structure definition.
 */
/*
 * Copyright (C) 2004-2006 Mike Frysinger <vapier@gmail.com>
 * Released under the BSD license.  For more information,
 * please see: http://opensource.org/licenses/bsd-license.php
 *
 * $Header: /cvsroot/freestdf/libstdf/include/libstdf.h,v 1.25 2005/12/08 04:07:07 vapier Exp $
 */

#ifndef _LIBSTDF_H
#define _LIBSTDF_H

#ifdef __cplusplus
extern "C" {
#endif

#include <libstdf_systems.h>
#include <libstdf_bswap.h>

#include <libstdf_const.h>
#include <libstdf_types.h>

/**
 * Compressed file format
 */
typedef enum {
	STDF_FORMAT_REG    = 0x0,    /**< Regular file */
	STDF_FORMAT_ZIP    = 0x1,    /**< Zipped file */
	STDF_FORMAT_GZIP   = 0x2,    /**< gzipped file */
	STDF_FORMAT_BZIP2  = 0x3,    /**< bzipped file */
	STDF_FORMAT_LZW    = 0x4     /**< LZW compressed file */
} stdf_format;

/**
 * Misc options to control libstdf behavior
 */
typedef enum {
	STDF_OPTS_DEFAULT  = 0x000,  /**< Default options */
	STDF_OPTS_FORCE    = 0x001,  /**< Force reading even if some sanity checks fail */
	STDF_OPTS_FORCE_V3 = 0x002,  /**< Force STDFv3 behavior */
	STDF_OPTS_FORCE_V4 = 0x004,  /**< Force STDFv4 behavior */
	STDF_OPTS_ZIP      = 0x008,  /**< File is compressed with zip */
	STDF_OPTS_GZIP     = 0x010,  /**< File is compressed with gzip */
	STDF_OPTS_BZIP2    = 0x020,  /**< File is compressed with bzip2 */
	STDF_OPTS_LZW      = 0x040,  /**< File is compressed with lzw */
	STDF_OPTS_READ     = 0x080,  /**< Allow reading from file */
	STDF_OPTS_WRITE    = 0x100,  /**< Allow writing to file */
	STDF_OPTS_CREATE   = 0x200   /**< Create file from scratch */
} stdf_initial_options;

typedef enum {
	STDF_SETTING_WRITE_SIZE = 0x001, /**< Set the output blocksize for writing */
	STDF_SETTING_VERSION    = 0x002, /**< Query the STDF spec version */
	STDF_SETTING_BYTE_ORDER = 0x003  /**< Query the byte order */
} stdf_runtime_settings;


#ifdef __IN_LIBSTDF
# include <libstdf_internal.h>
#else
typedef void stdf_file;
#endif

#include <libstdf_funcs.h>

#ifdef __cplusplus
}
#endif

#endif /* _LIBSTDF_H */
