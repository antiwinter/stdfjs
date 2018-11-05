/**
 * @file dtc.h
 * @brief Data type specific function prototypes.
 * @internal
 */
/*
 * Copyright (C) 2004-2006 Mike Frysinger <vapier@gmail.com>
 * Released under the BSD license.  For more information,
 * please see: http://opensource.org/licenses/bsd-license.php
 *
 * $Header: /cvsroot/freestdf/libstdf/src/dtc.h,v 1.14 2005/12/08 04:05:10 vapier Exp $
 */

#ifndef _LIBSTDF_DTC_H
#define _LIBSTDF_DTC_H

#include <libstdf.h>

extern void __byte_order_change(int, int, byte_t*, int) attribute_hidden;
#define	__byte_order_to_host(b,p,l) __byte_order_change(b, __STDF_HOST_BYTE_ORDER, p, l)
#define	__byte_order_to_src(b,p,l) __byte_order_change(__STDF_HOST_BYTE_ORDER, b, p, l)
#define	_stdf_byte_order_to_host(f,v,l) __byte_order_to_host(f->byte_order, (byte_t*)(v), l)
#define	_stdf_byte_order_to_src(f,v,l) __byte_order_to_src(f->byte_order, (byte_t*)(v), l)

#ifdef	STDF_VER3
extern void _stdf_read_dtc_Cx(stdf_file*, dtc_Cn*, int) attribute_hidden;
#endif
extern void _stdf_read_dtc_Cn(stdf_file*, dtc_Cn*) attribute_hidden;
extern void _stdf_read_dtc_Bn(stdf_file*, dtc_Bn*) attribute_hidden;
extern void _stdf_read_dtc_Dn(stdf_file*, dtc_Dn*) attribute_hidden;

extern void _stdf_read_dtc_C1(stdf_file*, dtc_C1*) attribute_hidden;
extern void _stdf_read_dtc_B1(stdf_file*, dtc_B1*) attribute_hidden;
extern void _stdf_read_dtc_N1(stdf_file*, dtc_N1*) attribute_hidden;
extern void _stdf_read_dtc_U1(stdf_file*, dtc_U1*) attribute_hidden;
extern void _stdf_read_dtc_U2(stdf_file*, dtc_U2*) attribute_hidden;
extern void _stdf_read_dtc_U4(stdf_file*, dtc_U4*) attribute_hidden;
extern void _stdf_read_dtc_I1(stdf_file*, dtc_I1*) attribute_hidden;
extern void _stdf_read_dtc_I2(stdf_file*, dtc_I2*) attribute_hidden;
extern void _stdf_read_dtc_I4(stdf_file*, dtc_I4*) attribute_hidden;
extern void _stdf_read_dtc_R4(stdf_file*, dtc_R4*) attribute_hidden;
extern void _stdf_read_dtc_R8(stdf_file*, dtc_R8*) attribute_hidden;

extern void _stdf_read_dtc_xN1(stdf_file*, dtc_xN1*, dtc_U2) attribute_hidden;
extern void _stdf_read_dtc_xU1(stdf_file*, dtc_xU1*, dtc_U2) attribute_hidden;
extern void _stdf_read_dtc_xU2(stdf_file*, dtc_xU2*, dtc_U2) attribute_hidden;
extern void _stdf_read_dtc_xR4(stdf_file*, dtc_xR4*, dtc_U2) attribute_hidden;
#define	free_x(x) if (x) free(x)
#define	free_xU1(x) free_x(x)
#define	free_xU2(x) free_x(x)
#define	free_xR4(x) free_x(x)

extern void _stdf_read_dtc_xCn(stdf_file*, dtc_xCn*, dtc_U2) attribute_hidden;
extern void free_xCn(dtc_xCn, dtc_U2) attribute_hidden;

extern void _stdf_read_dtc_Vn(stdf_file*, dtc_Vn*, dtc_U2) attribute_hidden;
extern void free_Vn(dtc_Vn, dtc_U2) attribute_hidden;

#define _stdf_write_byte(file, byte) \
	*file->_write_pos++ = byte
#define _stdf_write_nbytes(file, inbuf, cnt) \
	do { \
		memcpy(file->_write_pos, inbuf, cnt); \
		file->_write_pos += cnt; \
	} while (0)
#define _stdf_write_cnt_nbytes(file, inbuf, cnt) \
	do { \
		_stdf_write_byte(file, cnt); \
		if (cnt) _stdf_write_nbytes(file, inbuf, cnt); \
	} while (0)

#define __stdf_write_even_bytes(type, file, bytes, even) \
	do { \
		type mapme = bytes; \
		_stdf_byte_order_to_src(file, &mapme, even); \
		uchar *map = (uchar*)&mapme; \
		_stdf_write_nbytes(file, map, even); \
	} while (0)
#define _stdf_write_2bytes(file, bytes)       __stdf_write_even_bytes(uint16_t, file, bytes, 2)
#define _stdf_write_4bytes(file, bytes)       __stdf_write_even_bytes(uint32_t, file, bytes, 4)
#define _stdf_write_4bytes_float(file, bytes) __stdf_write_even_bytes(float, file, bytes, 4)
#define _stdf_write_8bytes_float(file, bytes) __stdf_write_even_bytes(double, file, bytes, 8)

#define _stdf_write_dtc_C1(file, C1) _stdf_write_byte(file, C1)
#define _stdf_write_dtc_B1(file, B1) _stdf_write_byte(file, B1)
#define _stdf_write_dtc_N1(file, N1) _stdf_write_byte(file, N1)
#define _stdf_write_dtc_U1(file, U1) _stdf_write_byte(file, U1)
#define _stdf_write_dtc_I1(file, I1) _stdf_write_byte(file, I1)

#define _stdf_write_dtc_U2(file, U2) _stdf_write_2bytes(file, U2)
#define _stdf_write_dtc_I2(file, I2) _stdf_write_2bytes(file, I2)

#define _stdf_write_dtc_U4(file, U4) _stdf_write_4bytes(file, U4)
#define _stdf_write_dtc_I4(file, I4) _stdf_write_4bytes(file, I4)
#define _stdf_write_dtc_R4(file, R4) _stdf_write_4bytes_float(file, R4)

#define _stdf_write_dtc_R8(file, R8) _stdf_write_8bytes_float(file, R8)

#define _stdf_qwrite_dtc_header(file, REC_LEN, REC_TYP, REC_SUB) \
	do { \
		_stdf_write_dtc_U2(file, REC_LEN); \
		_stdf_write_dtc_U1(file, REC_TYP); \
		_stdf_write_dtc_U1(file, REC_SUB); \
	} while (0)
#define _stdf_write_dtc_header(file, header) \
	_stdf_qwrite_dtc_header(file, (header)->REC_LEN, (header)->REC_TYP, (header)->REC_SUB)

#define _stdf_write_dtc_Cn(file, Cn) _stdf_write_cnt_nbytes(file, Cn+1, (Cn ? *Cn : 0))
#define _stdf_write_dtc_Bn(file, Bn) _stdf_write_dtc_Cn(file, Bn)
#define _stdf_write_dtc_Dn(file, Dn) \
	do { \
		if (Dn) { \
			dtc_U2 cnt = *((dtc_U2*)Dn); \
			_stdf_write_dtc_U2(file, cnt); \
			_stdf_write_nbytes(file, Dn+2, cnt); \
		} \
	} while (0)

#define _stdf_write_dtc_Vn(file, Vn, cnt) warnf("writing dtc_Vn is not implemented")

#define _stdf_write_dtc_x(file, x, cnt, func) \
	do { \
		dtc_U2 i; \
		for (i=0; i<cnt; ++i) \
			func(file, x[i]); \
	} while (0)

#define _stdf_write_dtc_xU1(file, xU1, cnt) _stdf_write_nbytes(file, xU1, cnt)
#define _stdf_write_dtc_xU2(file, xU2, cnt) _stdf_write_dtc_x(file, xU2, cnt, _stdf_write_dtc_U2)
#define _stdf_write_dtc_xN1(file, xN1, cnt) _stdf_write_nbytes(file, xN1, ((cnt/2)+(cnt%2)))
#define _stdf_write_dtc_xR4(file, xR4, cnt) _stdf_write_dtc_x(file, xR4, cnt, _stdf_write_dtc_R4)
#define _stdf_write_dtc_xCn(file, xCn, cnt) _stdf_write_dtc_x(file, xCn, cnt, _stdf_write_dtc_Cn)
#define _stdf_write_dtc_xBn(file, xBn, cnt) _stdf_write_dtc_xCn(file, xBn, cnt)

#ifdef STDF_VER3
#define _stdf_write_dtc_Cx(file, Cx, cnt) warnf("writing dtc_Cx is not implemented");
#endif

#endif /* _LIBSTDF_DTC_H */
