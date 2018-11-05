/**
 * @file rec.h
 * @brief Record specific function prototypes.
 * @internal
 */
/*
 * Copyright (C) 2004-2006 Mike Frysinger <vapier@gmail.com>
 * Released under the BSD license.  For more information,
 * please see: http://opensource.org/licenses/bsd-license.php
 *
 * $Header: /cvsroot/freestdf/libstdf/src/rec.h,v 1.10 2005/12/08 04:05:10 vapier Exp $
 */

#ifndef _LIBSTDF_REC_H
#define _LIBSTDF_REC_H

#include <libstdf.h>

extern rec_unknown* stdf_read_rec_unknown(stdf_file*) attribute_hidden;
extern rec_far* stdf_read_rec_far(stdf_file*) attribute_hidden;
extern rec_atr* stdf_read_rec_atr(stdf_file*) attribute_hidden;
extern rec_mir* stdf_read_rec_mir(stdf_file*) attribute_hidden;
extern rec_mrr* stdf_read_rec_mrr(stdf_file*) attribute_hidden;
extern rec_pcr* stdf_read_rec_pcr(stdf_file*) attribute_hidden;
extern rec_hbr* stdf_read_rec_hbr(stdf_file*) attribute_hidden;
extern rec_sbr* stdf_read_rec_sbr(stdf_file*) attribute_hidden;
extern rec_pmr* stdf_read_rec_pmr(stdf_file*) attribute_hidden;
extern rec_pgr* stdf_read_rec_pgr(stdf_file*) attribute_hidden;
extern rec_plr* stdf_read_rec_plr(stdf_file*) attribute_hidden;
extern rec_rdr* stdf_read_rec_rdr(stdf_file*) attribute_hidden;
extern rec_sdr* stdf_read_rec_sdr(stdf_file*) attribute_hidden;
extern rec_wir* stdf_read_rec_wir(stdf_file*) attribute_hidden;
extern rec_wrr* stdf_read_rec_wrr(stdf_file*) attribute_hidden;
extern rec_wcr* stdf_read_rec_wcr(stdf_file*) attribute_hidden;
extern rec_pir* stdf_read_rec_pir(stdf_file*) attribute_hidden;
extern rec_prr* stdf_read_rec_prr(stdf_file*) attribute_hidden;
#ifdef STDF_VER3
extern rec_pdr* stdf_read_rec_pdr(stdf_file*) attribute_hidden;
extern rec_fdr* stdf_read_rec_fdr(stdf_file*) attribute_hidden;
#endif
extern rec_tsr* stdf_read_rec_tsr(stdf_file*) attribute_hidden;
extern rec_ptr* stdf_read_rec_ptr(stdf_file*) attribute_hidden;
extern rec_mpr* stdf_read_rec_mpr(stdf_file*) attribute_hidden;
extern rec_ftr* stdf_read_rec_ftr(stdf_file*) attribute_hidden;
extern rec_bps* stdf_read_rec_bps(stdf_file*) attribute_hidden;
extern rec_eps* stdf_read_rec_eps(stdf_file*) attribute_hidden;
#ifdef STDF_VER3
extern rec_shb* stdf_read_rec_shb(stdf_file*) attribute_hidden;
extern rec_ssb* stdf_read_rec_ssb(stdf_file*) attribute_hidden;
extern rec_sts* stdf_read_rec_sts(stdf_file*) attribute_hidden;
extern rec_scr* stdf_read_rec_scr(stdf_file*) attribute_hidden;
#endif
extern rec_gdr* stdf_read_rec_gdr(stdf_file*) attribute_hidden;
extern rec_dtr* stdf_read_rec_dtr(stdf_file*) attribute_hidden;

extern ssize_t stdf_write_rec_far(stdf_file*, rec_far*) attribute_hidden;
extern ssize_t stdf_write_rec_atr(stdf_file*, rec_atr*) attribute_hidden;
extern ssize_t stdf_write_rec_mir(stdf_file*, rec_mir*) attribute_hidden;
extern ssize_t stdf_write_rec_mrr(stdf_file*, rec_mrr*) attribute_hidden;
extern ssize_t stdf_write_rec_pcr(stdf_file*, rec_pcr*) attribute_hidden;
extern ssize_t stdf_write_rec_hbr(stdf_file*, rec_hbr*) attribute_hidden;
extern ssize_t stdf_write_rec_sbr(stdf_file*, rec_sbr*) attribute_hidden;
extern ssize_t stdf_write_rec_pmr(stdf_file*, rec_pmr*) attribute_hidden;
extern ssize_t stdf_write_rec_pgr(stdf_file*, rec_pgr*) attribute_hidden;
extern ssize_t stdf_write_rec_plr(stdf_file*, rec_plr*) attribute_hidden;
extern ssize_t stdf_write_rec_rdr(stdf_file*, rec_rdr*) attribute_hidden;
extern ssize_t stdf_write_rec_sdr(stdf_file*, rec_sdr*) attribute_hidden;
extern ssize_t stdf_write_rec_wir(stdf_file*, rec_wir*) attribute_hidden;
extern ssize_t stdf_write_rec_wrr(stdf_file*, rec_wrr*) attribute_hidden;
extern ssize_t stdf_write_rec_wcr(stdf_file*, rec_wcr*) attribute_hidden;
extern ssize_t stdf_write_rec_pir(stdf_file*, rec_pir*) attribute_hidden;
extern ssize_t stdf_write_rec_prr(stdf_file*, rec_prr*) attribute_hidden;
#ifdef STDF_VER3
extern ssize_t stdf_write_rec_pdr(stdf_file*, rec_pdr*) attribute_hidden;
extern ssize_t stdf_write_rec_fdr(stdf_file*, rec_fdr*) attribute_hidden;
#endif
extern ssize_t stdf_write_rec_tsr(stdf_file*, rec_tsr*) attribute_hidden;
extern ssize_t stdf_write_rec_ptr(stdf_file*, rec_ptr*) attribute_hidden;
extern ssize_t stdf_write_rec_mpr(stdf_file*, rec_mpr*) attribute_hidden;
extern ssize_t stdf_write_rec_ftr(stdf_file*, rec_ftr*) attribute_hidden;
extern ssize_t stdf_write_rec_bps(stdf_file*, rec_bps*) attribute_hidden;
extern ssize_t stdf_write_rec_eps(stdf_file*, rec_eps*) attribute_hidden;
#ifdef STDF_VER3
extern ssize_t stdf_write_rec_shb(stdf_file*, rec_shb*) attribute_hidden;
extern ssize_t stdf_write_rec_ssb(stdf_file*, rec_ssb*) attribute_hidden;
extern ssize_t stdf_write_rec_sts(stdf_file*, rec_sts*) attribute_hidden;
extern ssize_t stdf_write_rec_scr(stdf_file*, rec_scr*) attribute_hidden;
#endif
extern ssize_t stdf_write_rec_gdr(stdf_file*, rec_gdr*) attribute_hidden;
extern ssize_t stdf_write_rec_dtr(stdf_file*, rec_dtr*) attribute_hidden;

ssize_t _stdf_write_flush(stdf_file*, size_t) attribute_hidden;

#endif /* _LIBSTDF_REC_H */
