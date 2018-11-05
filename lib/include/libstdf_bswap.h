/**
 * @file libstdf_bswap.h
 * @brief Byte swapping functions.
 */
/*
 * Copyright (C) 2004-2006 Mike Frysinger <vapier@gmail.com>
 * Released under the BSD license.  For more information,
 * please see: http://opensource.org/licenses/bsd-license.php
 *
 * $Header: /cvsroot/freestdf/libstdf/include/libstdf_bswap.h,v 1.6 2005/05/31 23:08:32 vapier Exp $
 */

#ifndef _LIBSTDF_BSWAP_H
#define _LIBSTDF_BSWAP_H

#if defined(bswap_16)
# define stdf_bswap_16(x) x = bswap_16(x)
# define stdf_bswap_32(x) x = bswap_32(x)
# define stdf_bswap_64(x) x = bswap_64(x)
#endif
#if defined(bswap16)
# define stdf_bswap_16(x) x = bswap16(x)
# define stdf_bswap_32(x) x = bswap32(x)
# define stdf_bswap_64(x) x = bswap64(x)
#endif

/**
 * @brief Swap bytes in a 16 bit value
 */
#ifndef stdf_bswap_16
# define stdf_bswap_16(x) \
	x = ((((x) & 0xff00) >> 8) | \
	     (((x) & 0x00ff) << 8))
#endif

/**
 * @brief Swap bytes in a 32 bit value
 */
#ifndef stdf_bswap_32
# define stdf_bswap_32(x) \
	x = ((((x) & 0xff000000) >> 24) | \
	     (((x) & 0x00ff0000) >>  8) | \
	     (((x) & 0x0000ff00) <<  8) | \
	     (((x) & 0x000000ff) << 24))
#endif

/**
 * @brief Swap bytes in a 64 bit value
 */
#ifndef stdf_bswap_64
#if defined(__GNUC__)
/* gcc sometimes stupidly types these wrong */
# define stdf_bswap_64(x) \
	x = ((((x) & 0xff00000000000000ull) >> 56) | \
	     (((x) & 0x00ff000000000000ull) >> 40) | \
	     (((x) & 0x0000ff0000000000ull) >> 24) | \
	     (((x) & 0x000000ff00000000ull) >>  8) | \
	     (((x) & 0x00000000ff000000ull) <<  8) | \
	     (((x) & 0x0000000000ff0000ull) << 24) | \
	     (((x) & 0x000000000000ff00ull) << 40) | \
	     (((x) & 0x00000000000000ffull) << 56))
#else
# define stdf_bswap_64(x) \
	x = ((((x) & 0xff00000000000000) >> 56) | \
	     (((x) & 0x00ff000000000000) >> 40) | \
	     (((x) & 0x0000ff0000000000) >> 24) | \
	     (((x) & 0x000000ff00000000) >>  8) | \
	     (((x) & 0x00000000ff000000) <<  8) | \
	     (((x) & 0x0000000000ff0000) << 24) | \
	     (((x) & 0x000000000000ff00) << 40) | \
	     (((x) & 0x00000000000000ff) << 56))
#endif /* __GNUC__ */
#endif

#endif /* _LIBSTDF_BSWAP_H */
