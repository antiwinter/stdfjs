/**
 * @file rec.c
 * @brief Record specific function implementations.
 * @internal
 */
/*
 * Copyright (C) 2004-2006 Mike Frysinger <vapier@gmail.com>
 * Released under the BSD license.  For more information,
 * please see: http://opensource.org/licenses/bsd-license.php
 *
 * $Header: /cvsroot/freestdf/libstdf/src/rec.c,v 1.41 2005/12/10 05:46:55 vapier Exp $
 */

#include <libstdf.h>
#include "dtc.h"
#include "rec.h"


/*
 * Misc utility functions
 */


char *stdf_rec_names[] = {
	"???",
	"FAR", "ATR", "MIR", "MRR", "PCR", "HBR",
	"SBR", "PMR", "PGR", "PLR", "RDR", "SDR",
	"WIR", "WRR", "WCR", "PIR", "PRR",
	"PDR", "FDR",
	"TSR", "PTR", "MPR", "FTR", "BPS", "EPS",
	"SHB", "SSB", "STS", "SCR",
	"GDR", "DTR"
};

static int _stdf_header_to_idx(rec_typ type, rec_sub sub)
{
	switch (MAKE_REC(type, sub)) {
		case REC_FAR:	return 1;
		case REC_ATR:	return 2;
		case REC_MIR:	return 3;
		case REC_MRR:	return 4;
		case REC_PCR:	return 5;
		case REC_HBR:	return 6;
		case REC_SBR:	return 7;
		case REC_PMR:	return 8;
		case REC_PGR:	return 9;
		case REC_PLR:	return 10;
		case REC_RDR:	return 11;
		case REC_SDR:	return 12;
		case REC_WIR:	return 13;
		case REC_WRR:	return 14;
		case REC_WCR:	return 15;
		case REC_PIR:	return 16;
		case REC_PRR:	return 17;
#ifdef STDF_VER3
		case REC_PDR:	return 18;
		case REC_FDR:	return 19;
#endif
		case REC_TSR:	return 20;
		case REC_PTR:	return 21;
		case REC_MPR:	return 22;
		case REC_FTR:	return 23;
		case REC_BPS:	return 24;
		case REC_EPS:	return 25;
#ifdef STDF_VER3
		case REC_SHB:	return 26;
		case REC_SSB:	return 27;
		case REC_STS:	return 28;
		case REC_SCR:	return 29;
#endif
		case REC_GDR:	return 30;
		case REC_DTR:	return 31;
		case REC_UNKNOWN:
		default:		return 0;
	}
}
int stdf_rec_to_idx_count()
{
	return 32;
}
int stdf_rec_name_to_idx(char *rec_name)
{
	int i = stdf_rec_to_idx_count();
	while (--i > 0)
		if (strncmp(rec_name, stdf_rec_names[i], 3) == 0)
			return i;
	return 0;
}
int stdf_rec_to_idx(void *rec_void)
{
	rec_unknown *rec = (rec_unknown*)rec_void;
	return _stdf_header_to_idx(rec->header.REC_TYP, rec->header.REC_SUB);
}
char* stdf_get_rec_name(rec_typ type, rec_sub sub)
{
	return stdf_rec_names[_stdf_header_to_idx(type, sub)];
}

#if WARN_UNTESTED
#define	warn_untested(type) \
	do { \
		warnf("*********************************************"); \
		warnf("This record type (" type ") has not been tested!"); \
		warnf("Please consider sending this file to"); \
		warnf("vapier@gmail.com to help out the"); \
		warnf("FreeSTDF project and make sure this code"); \
		warnf("works exactly the way it should!"); \
		warnf("*********************************************"); \
	} while (0)
#else
#define warn_untested(type)
#endif


/*
 * Memory handling functions
 */


#define __malloc_rec(r) ((r*)malloc(sizeof(r)))

void stdf_free_record(rec_unknown *rec)
{
	stdf_file *file;
	if (!rec)
		return;
	file = (stdf_file*)(rec->header.stdf_file);

	if (rec->header.state == REC_STATE_RAW) {
		free(rec->data);
		free(rec);
		return;
	}

	/* record order is based on frequency in 'standard' files
	 * Note: keep order in sync with libstdf.c
	 */
	switch (HEAD_TO_REC(rec->header)) {
		/* REC_TYP_PER_EXEC */
		case REC_PTR: {
			rec_ptr *ptr = (rec_ptr*)rec;
			free(ptr->TEST_TXT);
			free(ptr->ALARM_ID);
			free(ptr->UNITS);
			free(ptr->C_RESFMT);
			free(ptr->C_LLMFMT);
			free(ptr->C_HLMFMT);
			free(rec);
			break;
		}
		case REC_FTR: {
			rec_ftr *ftr = (rec_ftr*)rec;
			free_xU2(ftr->RTN_INDX);
			free(ftr->RTN_STAT);
			free_xU2(ftr->PGM_INDX);
			free(ftr->PGM_STAT);
			free(ftr->FAIL_PIN);
			free(ftr->VECT_NAM);
			free(ftr->TIME_SET);
			free(ftr->OP_CODE);
			free(ftr->TEST_TXT);
			free(ftr->ALARM_ID);
			free(ftr->PROG_TXT);
			free(ftr->RSLT_TXT);
			free(ftr->SPIN_MAP);
			free(rec);
			break;
		}
		case REC_MPR: {
			rec_mpr *mpr = (rec_mpr*)rec;
			free(mpr->RTN_STAT);
			free_xR4(mpr->RTN_RSLT);
			free(mpr->TEST_TXT);
			free(mpr->ALARM_ID);
			free_xU2(mpr->RTN_INDX);
			free(mpr->UNITS);
			free(mpr->UNITS_IN);
			free(mpr->C_RESFMT);
			free(mpr->C_LLMFMT);
			free(mpr->C_HLMFMT);
			free(rec);
			break;
		}

		/* REC_TYP_PER_PART */
		case REC_PIR: {
#ifdef STDF_VER3
			rec_pir *pir = (rec_pir*)rec;
			if (file->ver == 3)
			free(pir->PART_ID);
#endif
			free(rec);
			break;
		}
		case REC_PRR: {
			rec_prr *prr = (rec_prr*)rec;
			free(prr->PART_ID);
			free(prr->PART_TXT);
			free(prr->PART_FIX);
			free(rec);
			break;
		}

		/* REC_TYP_PER_TEST */
		case REC_TSR: {
			rec_tsr *tsr = (rec_tsr*)rec;
			free(tsr->TEST_NAM);
			free(tsr->SEQ_NAME);
#ifdef STDF_VER3
			if (file->ver == 4)
#endif
			free(tsr->TEST_LBL);
			free(rec);
			break;
		}
#ifdef STDF_VER3
		case REC_PDR: {
			rec_pdr *pdr = (rec_pdr*)rec;
			free(pdr->UNITS);
			free(pdr->TEST_NAM);
			free(pdr->SEQ_NAME);
			free(rec);
			break;
		}
		case REC_FDR: {
			rec_fdr *fdr = (rec_fdr*)rec;
			free(fdr->TEST_NAM);
			free(fdr->SEQ_NAME);
			free(rec);
			break;
		}
#endif

		/* REC_TYP_GENERIC */
		case REC_DTR: {
			rec_dtr *dtr = (rec_dtr*)rec;
			free(dtr->TEXT_DAT);
			free(rec);
			break;
		}
		case REC_GDR: {
			rec_gdr *gdr = (rec_gdr*)rec;
			free_Vn(gdr->GEN_DATA, gdr->FLD_CNT);
			free(rec);
			break;
		}

		/* REC_TYP_PER_PROG */
		case REC_BPS: {
			rec_bps *bps = (rec_bps*)rec;
			free(bps->SEQ_NAME);
			free(rec);
			break;
		}
		case REC_EPS: {
			free(rec);
			break;
		}

		/* REC_TYP_PER_SITE */
#ifdef STDF_VER3
		case REC_SHB: {
			rec_shb *shb = (rec_shb*)rec;
			free(shb->HBIN_NAM);
			free(rec);
			break;
		}
		case REC_SSB: {
			rec_ssb *ssb = (rec_ssb*)rec;
			free(ssb->SBIN_NAM);
			free(rec);
			break;
		}
		case REC_STS: {
			rec_sts *sts = (rec_sts*)rec;
			free(sts->TEST_NAM);
			free(sts->SEQ_NAME);
			free(sts->TEST_LBL);
			free(rec);
			break;
		}
		case REC_SCR: {
			free(rec);
			break;
		}
#endif

		/* REC_TYP_PER_LOT */
		case REC_PMR: {
			rec_pmr *pmr = (rec_pmr*)rec;
			free(pmr->CHAN_NAM);
			free(pmr->PHY_NAM);
			free(pmr->LOG_NAM);
			free(rec);
			break;
		}
		case REC_PGR: {
			rec_pgr *pgr = (rec_pgr*)rec;
			free(pgr->GRP_NAM);
			free_xU2(pgr->PMR_INDX);
			free(rec);
			break;
		}
		case REC_HBR: {
			rec_hbr *hbr = (rec_hbr*)rec;
			free(hbr->HBIN_NAM);
			free(rec);
			break;
		}
		case REC_SBR: {
			rec_sbr *sbr = (rec_sbr*)rec;
			free(sbr->SBIN_NAM);
			free(rec);
			break;
		}
		case REC_PLR: {
			rec_plr *plr = (rec_plr*)rec;
			free_xU2(plr->GRP_INDX);
			free_xU2(plr->GRP_MODE);
			free_xU1(plr->GRP_RADX);
			free_xCn(plr->PGM_CHAR, plr->GRP_CNT);
			free_xCn(plr->RTN_CHAR, plr->GRP_CNT);
			free_xCn(plr->PGM_CHAL, plr->GRP_CNT);
			free_xCn(plr->RTN_CHAL, plr->GRP_CNT);
			free(rec);
			break;
		}
		case REC_RDR: {
			rec_rdr *rdr = (rec_rdr*)rec;
			free_xU2(rdr->RTST_BIN);
			free(rec);
			break;
		}
		case REC_SDR: {
			rec_sdr *sdr = (rec_sdr*)rec;
			free_xU1(sdr->SITE_NUM);
			free(sdr->HAND_TYP);
			free(sdr->HAND_ID);
			free(sdr->CARD_TYP);
			free(sdr->CARD_ID);
			free(sdr->LOAD_TYP);
			free(sdr->LOAD_ID);
			free(sdr->DIB_TYP);
			free(sdr->DIB_ID);
			free(sdr->CABL_TYP);
			free(sdr->CABL_ID);
			free(sdr->CONT_TYP);
			free(sdr->CONT_ID);
			free(sdr->LASR_TYP);
			free(sdr->LASR_ID);
			free(sdr->EXTR_TYP);
			free(sdr->EXTR_ID);
			free(rec);
			break;
		}
		case REC_MIR: {
			rec_mir *mir = (rec_mir*)rec;
#ifdef STDF_VER3
			if (file->ver == 4) {
#endif
			free(mir->LOT_ID);
			free(mir->PART_TYP);
			free(mir->NODE_NAM);
			free(mir->TSTR_TYP);
			free(mir->JOB_NAM);
			free(mir->JOB_REV);
			free(mir->SBLOT_ID);
			free(mir->OPER_NAM);
			free(mir->EXEC_TYP);
			free(mir->EXEC_VER);
			free(mir->TEST_COD);
			free(mir->TST_TEMP);
			free(mir->USER_TXT);
			free(mir->AUX_FILE);
			free(mir->PKG_TYP);
			free(mir->FAMILY_ID);
			free(mir->DATE_COD);
			free(mir->FACIL_ID);
			free(mir->FLOOR_ID);
			free(mir->PROC_ID);
			free(mir->OPER_FRQ);
			free(mir->SPEC_NAM);
			free(mir->SPEC_VER);
			free(mir->FLOW_ID);
			free(mir->SETUP_ID);
			free(mir->DSGN_REV);
			free(mir->ENG_ID);
			free(mir->ROM_COD);
			free(mir->SERL_NUM);
			free(mir->SUPR_NAM);
#ifdef STDF_VER3
			} else {
			free(mir->TEST_COD);
			free(mir->LOT_ID);
			free(mir->PART_TYP);
			free(mir->JOB_NAM);
			free(mir->OPER_NAM);
			free(mir->NODE_NAM);
			free(mir->TSTR_TYP);
			free(mir->EXEC_TYP);
			free(mir->SUPR_NAM);
			free(mir->HAND_ID);
			free(mir->SBLOT_ID);
			free(mir->JOB_REV);
			free(mir->PROC_ID);
			free(mir->PRB_CARD);
			}
#endif
			free(mir);
			break;
		}
		case REC_MRR: {
			rec_mrr *mrr = (rec_mrr*)rec;
			free(mrr->USR_DESC);
			free(mrr->EXC_DESC);
			free(rec);
			break;
		}
		case REC_PCR: {
			free(rec);
			break;
		}

		/* REC_TYP_PER_WAFER */
		case REC_WIR: {
			rec_wir *wir = (rec_wir*)rec;
			free(wir->WAFER_ID);
			free(rec);
			break;
		}
		case REC_WRR: {
			rec_wrr *wrr = (rec_wrr*)rec;
			free(wrr->WAFER_ID);
#ifdef STDF_VER3
			if (file->ver == 4) {
#endif
			free(wrr->FABWF_ID);
			free(wrr->FRAME_ID);
			free(wrr->MASK_ID);
#ifdef STDF_VER3
			} else {
			free(wrr->HAND_ID);
			free(wrr->PRB_CARD);
			}
#endif
			free(wrr->USR_DESC);
			free(wrr->EXC_DESC);
			free(rec);
			break;
		}
		case REC_WCR: {
			free(rec);
			break;
		}

		/* REC_TYP_INFO */
		case REC_FAR: {
			free(rec);
			break;
		}
		case REC_ATR: {
			rec_atr *atr = (rec_atr*)rec;
			free(atr->CMD_LINE);
			free(atr);
			break;
		}

		/* all the rest */
		case REC_UNKNOWN: {
			if (rec->data) free(rec->data);
			free(rec);
			break;
		}
		default: {
			warnf("Memory leak: unhandled record type (%s) !",
			       stdf_get_rec_name(rec->header.REC_TYP, rec->header.REC_SUB));
			break;
		}
	}

	rec = NULL;
}


/*
 * Reading records
 */


rec_unknown* stdf_read_rec_unknown(stdf_file *file)
{
	rec_unknown *rec = __malloc_rec(rec_unknown);
#if 0
	rec->data = (void*)malloc(file->header.REC_LEN);
	memcpy(rec->data, file->rec_data, file->header.REC_LEN);
#else
	rec->data = NULL;
#endif
	file->rec_pos += file->header.REC_LEN;
	return rec;
}

rec_far* stdf_read_rec_far(stdf_file *file)
{
	rec_far *far = __malloc_rec(rec_far);
	_stdf_read_dtc_U1(file, &(far->CPU_TYPE));
	_stdf_read_dtc_U1(file, &(far->STDF_VER));
	return far;
}

rec_atr* stdf_read_rec_atr(stdf_file *file)
{
	rec_atr *atr = __malloc_rec(rec_atr);
	_stdf_read_dtc_U4(file, &(atr->MOD_TIM));
	_stdf_read_dtc_Cn(file, &(atr->CMD_LINE));
	return atr;
}

rec_mir* stdf_read_rec_mir(stdf_file *file)
{
	rec_mir *mir = __malloc_rec(rec_mir);
#ifdef STDF_VER3
	if (file->ver == 4) {
#endif
	_stdf_read_dtc_U4(file, &(mir->SETUP_T));
	_stdf_read_dtc_U4(file, &(mir->START_T));
	_stdf_read_dtc_U1(file, &(mir->STAT_NUM));
	_stdf_read_dtc_C1(file, &(mir->MODE_COD));
	_stdf_read_dtc_C1(file, &(mir->RTST_COD));
	_stdf_read_dtc_C1(file, &(mir->PROT_COD));
	_stdf_read_dtc_U2(file, &(mir->BURN_TIM));
	_stdf_read_dtc_C1(file, &(mir->CMOD_COD));
	_stdf_read_dtc_Cn(file, &(mir->LOT_ID));
	_stdf_read_dtc_Cn(file, &(mir->PART_TYP));
	_stdf_read_dtc_Cn(file, &(mir->NODE_NAM));
	_stdf_read_dtc_Cn(file, &(mir->TSTR_TYP));
	_stdf_read_dtc_Cn(file, &(mir->JOB_NAM));
	_stdf_read_dtc_Cn(file, &(mir->JOB_REV));
	_stdf_read_dtc_Cn(file, &(mir->SBLOT_ID));
	_stdf_read_dtc_Cn(file, &(mir->OPER_NAM));
	_stdf_read_dtc_Cn(file, &(mir->EXEC_TYP));
	_stdf_read_dtc_Cn(file, &(mir->EXEC_VER));
	_stdf_read_dtc_Cn(file, &(mir->TEST_COD));
	_stdf_read_dtc_Cn(file, &(mir->TST_TEMP));
	_stdf_read_dtc_Cn(file, &(mir->USER_TXT));
	_stdf_read_dtc_Cn(file, &(mir->AUX_FILE));
	_stdf_read_dtc_Cn(file, &(mir->PKG_TYP));
	_stdf_read_dtc_Cn(file, &(mir->FAMILY_ID));
	_stdf_read_dtc_Cn(file, &(mir->DATE_COD));
	_stdf_read_dtc_Cn(file, &(mir->FACIL_ID));
	_stdf_read_dtc_Cn(file, &(mir->FLOOR_ID));
	_stdf_read_dtc_Cn(file, &(mir->PROC_ID));
	_stdf_read_dtc_Cn(file, &(mir->OPER_FRQ));
	_stdf_read_dtc_Cn(file, &(mir->SPEC_NAM));
	_stdf_read_dtc_Cn(file, &(mir->SPEC_VER));
	_stdf_read_dtc_Cn(file, &(mir->FLOW_ID));
	_stdf_read_dtc_Cn(file, &(mir->SETUP_ID));
	_stdf_read_dtc_Cn(file, &(mir->DSGN_REV));
	_stdf_read_dtc_Cn(file, &(mir->ENG_ID));
	_stdf_read_dtc_Cn(file, &(mir->ROM_COD));
	_stdf_read_dtc_Cn(file, &(mir->SERL_NUM));
	_stdf_read_dtc_Cn(file, &(mir->SUPR_NAM));
#ifdef STDF_VER3
	} else {
	_stdf_read_dtc_U1(file, &(mir->CPU_TYPE));
	_stdf_read_dtc_U1(file, &(mir->STDF_VER));
	_stdf_read_dtc_C1(file, &(mir->MODE_COD));
	_stdf_read_dtc_U1(file, &(mir->STAT_NUM));
	_stdf_read_dtc_Cx(file, &(mir->TEST_COD), 3);
	_stdf_read_dtc_C1(file, &(mir->RTST_COD));
	_stdf_read_dtc_C1(file, &(mir->PROT_COD));
	_stdf_read_dtc_C1(file, &(mir->CMOD_COD));
	_stdf_read_dtc_U4(file, &(mir->SETUP_T));
	_stdf_read_dtc_U4(file, &(mir->START_T));
	_stdf_read_dtc_Cn(file, &(mir->LOT_ID));
	_stdf_read_dtc_Cn(file, &(mir->PART_TYP));
	_stdf_read_dtc_Cn(file, &(mir->JOB_NAM));
	_stdf_read_dtc_Cn(file, &(mir->OPER_NAM));
	_stdf_read_dtc_Cn(file, &(mir->NODE_NAM));
	_stdf_read_dtc_Cn(file, &(mir->TSTR_TYP));
	_stdf_read_dtc_Cn(file, &(mir->EXEC_TYP));
	_stdf_read_dtc_Cn(file, &(mir->SUPR_NAM));
	_stdf_read_dtc_Cn(file, &(mir->HAND_ID));
	_stdf_read_dtc_Cn(file, &(mir->SBLOT_ID));
	_stdf_read_dtc_Cn(file, &(mir->JOB_REV));
	_stdf_read_dtc_Cn(file, &(mir->PROC_ID));
	_stdf_read_dtc_Cn(file, &(mir->PRB_CARD));
	}
#endif
	return mir;
}

rec_mrr* stdf_read_rec_mrr(stdf_file *file)
{
	rec_mrr *mrr = __malloc_rec(rec_mrr);
	_stdf_read_dtc_U4(file, &(mrr->FINISH_T));
#ifdef STDF_VER3
	if (file->ver == 3) {
	_stdf_read_dtc_U4(file, &(mrr->PART_CNT));
	_stdf_read_dtc_U4(file, &(mrr->RTST_CNT));
	_stdf_read_dtc_U4(file, &(mrr->ABRT_CNT));
	_stdf_read_dtc_U4(file, &(mrr->GOOD_CNT));
	_stdf_read_dtc_U4(file, &(mrr->FUNC_CNT));
	}
#endif
	_stdf_read_dtc_C1(file, &(mrr->DISP_COD));
	_stdf_read_dtc_Cn(file, &(mrr->USR_DESC));
	_stdf_read_dtc_Cn(file, &(mrr->EXC_DESC));
	return mrr;
}

rec_pcr* stdf_read_rec_pcr(stdf_file *file)
{
	rec_pcr *pcr = __malloc_rec(rec_pcr);
	_stdf_read_dtc_U1(file, &(pcr->HEAD_NUM));
	_stdf_read_dtc_U1(file, &(pcr->SITE_NUM));
	_stdf_read_dtc_U4(file, &(pcr->PART_CNT));
	_stdf_read_dtc_U4(file, &(pcr->RTST_CNT));
	_stdf_read_dtc_U4(file, &(pcr->ABRT_CNT));
	_stdf_read_dtc_U4(file, &(pcr->GOOD_CNT));
	_stdf_read_dtc_U4(file, &(pcr->FUNC_CNT));
	return pcr;
}

rec_hbr* stdf_read_rec_hbr(stdf_file *file)
{
	rec_hbr *hbr = __malloc_rec(rec_hbr);
	_stdf_read_dtc_U1(file, &(hbr->HEAD_NUM));
	_stdf_read_dtc_U1(file, &(hbr->SITE_NUM));
	_stdf_read_dtc_U2(file, &(hbr->HBIN_NUM));
	_stdf_read_dtc_U4(file, &(hbr->HBIN_CNT));
	_stdf_read_dtc_C1(file, &(hbr->HBIN_PF));
	_stdf_read_dtc_Cn(file, &(hbr->HBIN_NAM));
	return hbr;
}

rec_sbr* stdf_read_rec_sbr(stdf_file *file)
{
	rec_sbr *sbr = __malloc_rec(rec_sbr);
	_stdf_read_dtc_U1(file, &(sbr->HEAD_NUM));
	_stdf_read_dtc_U1(file, &(sbr->SITE_NUM));
	_stdf_read_dtc_U2(file, &(sbr->SBIN_NUM));
	_stdf_read_dtc_U4(file, &(sbr->SBIN_CNT));
	_stdf_read_dtc_C1(file, &(sbr->SBIN_PF));
	_stdf_read_dtc_Cn(file, &(sbr->SBIN_NAM));
	return sbr;
}

rec_pmr* stdf_read_rec_pmr(stdf_file *file)
{
	rec_pmr *pmr = __malloc_rec(rec_pmr);
	_stdf_read_dtc_U2(file, &(pmr->PMR_INDX));
	_stdf_read_dtc_U2(file, &(pmr->CHAN_TYP));
	_stdf_read_dtc_Cn(file, &(pmr->CHAN_NAM));
	_stdf_read_dtc_Cn(file, &(pmr->PHY_NAM));
	_stdf_read_dtc_Cn(file, &(pmr->LOG_NAM));
	_stdf_read_dtc_U1(file, &(pmr->HEAD_NUM));
	_stdf_read_dtc_U1(file, &(pmr->SITE_NUM));
	return pmr;
}

rec_pgr* stdf_read_rec_pgr(stdf_file *file)
{
	rec_pgr *pgr = __malloc_rec(rec_pgr);
	_stdf_read_dtc_U2(file, &(pgr->GRP_INDX));
	_stdf_read_dtc_Cn(file, &(pgr->GRP_NAM));
	_stdf_read_dtc_U2(file, &(pgr->INDX_CNT));
	_stdf_read_dtc_xU2(file, &(pgr->PMR_INDX), pgr->INDX_CNT);
	return pgr;
}

rec_plr* stdf_read_rec_plr(stdf_file *file)
{
	rec_plr *plr = __malloc_rec(rec_plr);
	warn_untested("PLR");
	_stdf_read_dtc_U2(file, &(plr->GRP_CNT));
	_stdf_read_dtc_xU2(file, &(plr->GRP_INDX), plr->GRP_CNT);
	_stdf_read_dtc_xU2(file, &(plr->GRP_MODE), plr->GRP_CNT);
	_stdf_read_dtc_xU1(file, &(plr->GRP_RADX), plr->GRP_CNT);
	_stdf_read_dtc_xCn(file, &(plr->PGM_CHAR), plr->GRP_CNT);
	_stdf_read_dtc_xCn(file, &(plr->RTN_CHAR), plr->GRP_CNT);
	_stdf_read_dtc_xCn(file, &(plr->PGM_CHAL), plr->GRP_CNT);
	_stdf_read_dtc_xCn(file, &(plr->RTN_CHAL), plr->GRP_CNT);
	return plr;
}

rec_rdr* stdf_read_rec_rdr(stdf_file *file)
{
	rec_rdr *rdr = __malloc_rec(rec_rdr);
	_stdf_read_dtc_U2(file, &(rdr->NUM_BINS));
	_stdf_read_dtc_xU2(file, &(rdr->RTST_BIN), rdr->NUM_BINS);
	return rdr;
}

rec_sdr* stdf_read_rec_sdr(stdf_file *file)
{
	rec_sdr *sdr = __malloc_rec(rec_sdr);
	_stdf_read_dtc_U1(file, &(sdr->HEAD_NUM));
	_stdf_read_dtc_U1(file, &(sdr->SITE_GRP));
	_stdf_read_dtc_U1(file, &(sdr->SITE_CNT));
	_stdf_read_dtc_xU1(file, &(sdr->SITE_NUM), sdr->SITE_CNT);
	_stdf_read_dtc_Cn(file, &(sdr->HAND_TYP));
	_stdf_read_dtc_Cn(file, &(sdr->HAND_ID));
	_stdf_read_dtc_Cn(file, &(sdr->CARD_TYP));
	_stdf_read_dtc_Cn(file, &(sdr->CARD_ID));
	_stdf_read_dtc_Cn(file, &(sdr->LOAD_TYP));
	_stdf_read_dtc_Cn(file, &(sdr->LOAD_ID));
	_stdf_read_dtc_Cn(file, &(sdr->DIB_TYP));
	_stdf_read_dtc_Cn(file, &(sdr->DIB_ID));
	_stdf_read_dtc_Cn(file, &(sdr->CABL_TYP));
	_stdf_read_dtc_Cn(file, &(sdr->CABL_ID));
	_stdf_read_dtc_Cn(file, &(sdr->CONT_TYP));
	_stdf_read_dtc_Cn(file, &(sdr->CONT_ID));
	_stdf_read_dtc_Cn(file, &(sdr->LASR_TYP));
	_stdf_read_dtc_Cn(file, &(sdr->LASR_ID));
	_stdf_read_dtc_Cn(file, &(sdr->EXTR_TYP));
	_stdf_read_dtc_Cn(file, &(sdr->EXTR_ID));
	return sdr;
}

rec_wir* stdf_read_rec_wir(stdf_file *file)
{
	rec_wir *wir = __malloc_rec(rec_wir);
	_stdf_read_dtc_U1(file, &(wir->HEAD_NUM));
#ifdef STDF_VER3
	if (file->ver == 3)
	_stdf_read_dtc_B1(file, &(wir->PAD_BYTE));
	else
#endif
	_stdf_read_dtc_U1(file, &(wir->SITE_GRP));
	_stdf_read_dtc_U4(file, &(wir->START_T));
	_stdf_read_dtc_Cn(file, &(wir->WAFER_ID));
	return wir;
}

rec_wrr* stdf_read_rec_wrr(stdf_file *file)
{
	rec_wrr *wrr = __malloc_rec(rec_wrr);
#ifdef STDF_VER3
	if (file->ver == 4) {
#endif
	_stdf_read_dtc_U1(file, &(wrr->HEAD_NUM));
	_stdf_read_dtc_U1(file, &(wrr->SITE_GRP));
	_stdf_read_dtc_U4(file, &(wrr->FINISH_T));
#ifdef STDF_VER3
	} else {
	_stdf_read_dtc_U4(file, &(wrr->FINISH_T));
	_stdf_read_dtc_U1(file, &(wrr->HEAD_NUM));
	_stdf_read_dtc_B1(file, &(wrr->PAD_BYTE));
	}
#endif
	_stdf_read_dtc_U4(file, &(wrr->PART_CNT));
	_stdf_read_dtc_U4(file, &(wrr->RTST_CNT));
	_stdf_read_dtc_U4(file, &(wrr->ABRT_CNT));
	_stdf_read_dtc_U4(file, &(wrr->GOOD_CNT));
	_stdf_read_dtc_U4(file, &(wrr->FUNC_CNT));
	_stdf_read_dtc_Cn(file, &(wrr->WAFER_ID));
#ifdef STDF_VER3
	if (file->ver == 4) {
#endif
	_stdf_read_dtc_Cn(file, &(wrr->FABWF_ID));
	_stdf_read_dtc_Cn(file, &(wrr->FRAME_ID));
	_stdf_read_dtc_Cn(file, &(wrr->MASK_ID));
#ifdef STDF_VER3
	} else {
	_stdf_read_dtc_Cn(file, &(wrr->HAND_ID));
	_stdf_read_dtc_Cn(file, &(wrr->PRB_CARD));
	}
#endif
	_stdf_read_dtc_Cn(file, &(wrr->USR_DESC));
	_stdf_read_dtc_Cn(file, &(wrr->EXC_DESC));
	return wrr;
}

rec_wcr* stdf_read_rec_wcr(stdf_file *file)
{
	rec_wcr *wcr = __malloc_rec(rec_wcr);
	_stdf_read_dtc_R4(file, &(wcr->WAFR_SIZ));
	_stdf_read_dtc_R4(file, &(wcr->DIE_HT));
	_stdf_read_dtc_R4(file, &(wcr->DIE_WID));
	_stdf_read_dtc_U1(file, &(wcr->WF_UNITS));
	_stdf_read_dtc_C1(file, &(wcr->WF_FLAT));
	_stdf_read_dtc_I2(file, &(wcr->CENTER_X));
	_stdf_read_dtc_I2(file, &(wcr->CENTER_Y));
	_stdf_read_dtc_C1(file, &(wcr->POS_X));
	_stdf_read_dtc_C1(file, &(wcr->POS_Y));
	return wcr;
}

rec_pir* stdf_read_rec_pir(stdf_file *file)
{
	rec_pir *pir = __malloc_rec(rec_pir);
	_stdf_read_dtc_U1(file, &(pir->HEAD_NUM));
	_stdf_read_dtc_U1(file, &(pir->SITE_NUM));
#ifdef STDF_VER3
	if (file->ver == 3) {
	_stdf_read_dtc_I2(file, &(pir->X_COORD));
	_stdf_read_dtc_I2(file, &(pir->Y_COORD));
	_stdf_read_dtc_Cn(file, &(pir->PART_ID));
	}
#endif
	return pir;
}

rec_prr* stdf_read_rec_prr(stdf_file *file)
{
	rec_prr *prr = __malloc_rec(rec_prr);
	_stdf_read_dtc_U1(file, &(prr->HEAD_NUM));
	_stdf_read_dtc_U1(file, &(prr->SITE_NUM));
#ifdef STDF_VER3
	if (file->ver == 4)
#endif
	_stdf_read_dtc_B1(file, &(prr->PART_FLG));
	_stdf_read_dtc_U2(file, &(prr->NUM_TEST));
	_stdf_read_dtc_U2(file, &(prr->HARD_BIN));
	_stdf_read_dtc_U2(file, &(prr->SOFT_BIN));
#ifdef STDF_VER3
	if (file->ver == 3) {
	_stdf_read_dtc_B1(file, &(prr->PART_FLG));
	_stdf_read_dtc_B1(file, &(prr->PAD_BYTE));
	}
#endif
	_stdf_read_dtc_I2(file, &(prr->X_COORD));
	_stdf_read_dtc_I2(file, &(prr->Y_COORD));
#ifdef STDF_VER3
	if (file->ver == 4)
#endif
	_stdf_read_dtc_U4(file, &(prr->TEST_T));
	_stdf_read_dtc_Cn(file, &(prr->PART_ID));
	_stdf_read_dtc_Cn(file, &(prr->PART_TXT));
	_stdf_read_dtc_Bn(file, &(prr->PART_FIX));
	return prr;
}

#ifdef STDF_VER3

rec_pdr* stdf_read_rec_pdr(stdf_file *file)
{
	rec_pdr *pdr = __malloc_rec(rec_pdr);
	_stdf_read_dtc_U4(file, &(pdr->TEST_NUM));
	_stdf_read_dtc_B1(file, &(pdr->DESC_FLG));
	_stdf_read_dtc_B1(file, &(pdr->OPT_FLAG));
	_stdf_read_dtc_I1(file, &(pdr->RES_SCAL));
	_stdf_read_dtc_Cx(file, &(pdr->UNITS), 7);
	_stdf_read_dtc_U1(file, &(pdr->RES_LDIG));
	_stdf_read_dtc_U1(file, &(pdr->RES_RDIG));
	_stdf_read_dtc_I1(file, &(pdr->LLM_SCAL));
	_stdf_read_dtc_I1(file, &(pdr->HLM_SCAL));
	_stdf_read_dtc_U1(file, &(pdr->LLM_LDIG));
	_stdf_read_dtc_U1(file, &(pdr->LLM_RDIG));
	_stdf_read_dtc_U1(file, &(pdr->HLM_LDIG));
	_stdf_read_dtc_U1(file, &(pdr->HLM_RDIG));
	_stdf_read_dtc_R4(file, &(pdr->LO_LIMIT));
	_stdf_read_dtc_R4(file, &(pdr->HI_LIMIT));
	_stdf_read_dtc_Cn(file, &(pdr->TEST_NAM));
	_stdf_read_dtc_Cn(file, &(pdr->SEQ_NAME));
	return pdr;
}

rec_fdr* stdf_read_rec_fdr(stdf_file *file)
{
	rec_fdr *fdr = __malloc_rec(rec_fdr);
	_stdf_read_dtc_U4(file, &(fdr->TEST_NUM));
	_stdf_read_dtc_B1(file, &(fdr->DESC_FLG));
	_stdf_read_dtc_Cn(file, &(fdr->TEST_NAM));
	_stdf_read_dtc_Cn(file, &(fdr->SEQ_NAME));
	return fdr;
}

#endif

rec_tsr* stdf_read_rec_tsr(stdf_file *file)
{
	rec_tsr *tsr = __malloc_rec(rec_tsr);
	_stdf_read_dtc_U1(file, &(tsr->HEAD_NUM));
	_stdf_read_dtc_U1(file, &(tsr->SITE_NUM));
#ifdef STDF_VER3
	if (file->ver == 4)
#endif
	_stdf_read_dtc_C1(file, &(tsr->TEST_TYP));
	_stdf_read_dtc_U4(file, &(tsr->TEST_NUM));
	_stdf_read_dtc_U4(file, &(tsr->EXEC_CNT));
	_stdf_read_dtc_U4(file, &(tsr->FAIL_CNT));
	_stdf_read_dtc_U4(file, &(tsr->ALRM_CNT));
#ifdef STDF_VER3
	if (file->ver == 4) {
#endif
	_stdf_read_dtc_Cn(file, &(tsr->TEST_NAM));
	_stdf_read_dtc_Cn(file, &(tsr->SEQ_NAME));
	_stdf_read_dtc_Cn(file, &(tsr->TEST_LBL));
	_stdf_read_dtc_B1(file, &(tsr->OPT_FLAG));
	_stdf_read_dtc_R4(file, &(tsr->TEST_TIM));
	_stdf_read_dtc_R4(file, &(tsr->TEST_MIN));
	_stdf_read_dtc_R4(file, &(tsr->TEST_MAX));
	_stdf_read_dtc_R4(file, &(tsr->TST_SUMS));
	_stdf_read_dtc_R4(file, &(tsr->TST_SQRS));
#ifdef STDF_VER3
	} else {
	_stdf_read_dtc_B1(file, &(tsr->OPT_FLAG));
	_stdf_read_dtc_B1(file, &(tsr->PAD_BYTE));
	_stdf_read_dtc_R4(file, &(tsr->TEST_MIN));
	_stdf_read_dtc_R4(file, &(tsr->TEST_MAX));
	_stdf_read_dtc_R4(file, &(tsr->TST_MEAN));
	_stdf_read_dtc_R4(file, &(tsr->TST_SDEV));
	_stdf_read_dtc_R4(file, &(tsr->TST_SUMS));
	_stdf_read_dtc_R4(file, &(tsr->TST_SQRS));
	_stdf_read_dtc_Cn(file, &(tsr->TEST_NAM));
	_stdf_read_dtc_Cn(file, &(tsr->SEQ_NAME));
	}
#endif
	return tsr;
}

rec_ptr* stdf_read_rec_ptr(stdf_file *file)
{
	rec_ptr *ptr = __malloc_rec(rec_ptr);
	_stdf_read_dtc_U4(file, &(ptr->TEST_NUM));
	_stdf_read_dtc_U1(file, &(ptr->HEAD_NUM));
	_stdf_read_dtc_U1(file, &(ptr->SITE_NUM));
	_stdf_read_dtc_B1(file, &(ptr->TEST_FLG));
	_stdf_read_dtc_B1(file, &(ptr->PARM_FLG));
	_stdf_read_dtc_R4(file, &(ptr->RESULT));
	_stdf_read_dtc_Cn(file, &(ptr->TEST_TXT));
	_stdf_read_dtc_Cn(file, &(ptr->ALARM_ID));
	_stdf_read_dtc_B1(file, &(ptr->OPT_FLAG));
	_stdf_read_dtc_I1(file, &(ptr->RES_SCAL));
	_stdf_read_dtc_I1(file, &(ptr->LLM_SCAL));
	_stdf_read_dtc_I1(file, &(ptr->HLM_SCAL));
	_stdf_read_dtc_R4(file, &(ptr->LO_LIMIT));
	_stdf_read_dtc_R4(file, &(ptr->HI_LIMIT));
	_stdf_read_dtc_Cn(file, &(ptr->UNITS));
	_stdf_read_dtc_Cn(file, &(ptr->C_RESFMT));
	_stdf_read_dtc_Cn(file, &(ptr->C_LLMFMT));
	_stdf_read_dtc_Cn(file, &(ptr->C_HLMFMT));
	_stdf_read_dtc_R4(file, &(ptr->LO_SPEC));
	_stdf_read_dtc_R4(file, &(ptr->HI_SPEC));
	return ptr;
}

rec_mpr* stdf_read_rec_mpr(stdf_file *file)
{
	rec_mpr *mpr = __malloc_rec(rec_mpr);
	_stdf_read_dtc_U4(file, &(mpr->TEST_NUM));
	_stdf_read_dtc_U1(file, &(mpr->HEAD_NUM));
	_stdf_read_dtc_U1(file, &(mpr->SITE_NUM));
	_stdf_read_dtc_B1(file, &(mpr->TEST_FLG));
	_stdf_read_dtc_B1(file, &(mpr->PARM_FLG));
	_stdf_read_dtc_U2(file, &(mpr->RTN_ICNT));
	_stdf_read_dtc_U2(file, &(mpr->RSLT_CNT));
	_stdf_read_dtc_xN1(file, &(mpr->RTN_STAT), mpr->RTN_ICNT);
	_stdf_read_dtc_xR4(file, &(mpr->RTN_RSLT), mpr->RSLT_CNT);
	_stdf_read_dtc_Cn(file, &(mpr->TEST_TXT));
	_stdf_read_dtc_Cn(file, &(mpr->ALARM_ID));
	_stdf_read_dtc_B1(file, &(mpr->OPT_FLAG));
	_stdf_read_dtc_I1(file, &(mpr->RES_SCAL));
	_stdf_read_dtc_I1(file, &(mpr->LLM_SCAL));
	_stdf_read_dtc_I1(file, &(mpr->HLM_SCAL));
	_stdf_read_dtc_R4(file, &(mpr->LO_LIMIT));
	_stdf_read_dtc_R4(file, &(mpr->HI_LIMIT));
	_stdf_read_dtc_R4(file, &(mpr->START_IN));
	_stdf_read_dtc_R4(file, &(mpr->INCR_IN));
	_stdf_read_dtc_xU2(file, &(mpr->RTN_INDX), mpr->RTN_ICNT);
	_stdf_read_dtc_Cn(file, &(mpr->UNITS));
	_stdf_read_dtc_Cn(file, &(mpr->UNITS_IN));
	_stdf_read_dtc_Cn(file, &(mpr->C_RESFMT));
	_stdf_read_dtc_Cn(file, &(mpr->C_LLMFMT));
	_stdf_read_dtc_Cn(file, &(mpr->C_HLMFMT));
	_stdf_read_dtc_R4(file, &(mpr->LO_SPEC));
	_stdf_read_dtc_R4(file, &(mpr->HI_SPEC));
	return mpr;
}

rec_ftr* stdf_read_rec_ftr(stdf_file *file)
{
	rec_ftr *ftr = __malloc_rec(rec_ftr);
	_stdf_read_dtc_U4(file, &(ftr->TEST_NUM));
	_stdf_read_dtc_U1(file, &(ftr->HEAD_NUM));
	_stdf_read_dtc_U1(file, &(ftr->SITE_NUM));
	_stdf_read_dtc_B1(file, &(ftr->TEST_FLG));
	_stdf_read_dtc_B1(file, &(ftr->OPT_FLAG));
	_stdf_read_dtc_U4(file, &(ftr->CYCL_CNT));
	_stdf_read_dtc_U4(file, &(ftr->REL_VADR));
	_stdf_read_dtc_U4(file, &(ftr->REPT_CNT));
	_stdf_read_dtc_U4(file, &(ftr->NUM_FAIL));
	_stdf_read_dtc_I4(file, &(ftr->XFAIL_AD));
	_stdf_read_dtc_I4(file, &(ftr->YFAIL_AD));
	_stdf_read_dtc_I2(file, &(ftr->VECT_OFF));
	_stdf_read_dtc_U2(file, &(ftr->RTN_ICNT));
	_stdf_read_dtc_U2(file, &(ftr->PGM_ICNT));
	_stdf_read_dtc_xU2(file, &(ftr->RTN_INDX), ftr->RTN_ICNT);
	_stdf_read_dtc_xN1(file, &(ftr->RTN_STAT), ftr->RTN_ICNT);
	_stdf_read_dtc_xU2(file, &(ftr->PGM_INDX), ftr->PGM_ICNT);
	_stdf_read_dtc_xN1(file, &(ftr->PGM_STAT), ftr->PGM_ICNT);
	_stdf_read_dtc_Dn(file, &(ftr->FAIL_PIN));
	_stdf_read_dtc_Cn(file, &(ftr->VECT_NAM));
	_stdf_read_dtc_Cn(file, &(ftr->TIME_SET));
	_stdf_read_dtc_Cn(file, &(ftr->OP_CODE));
	_stdf_read_dtc_Cn(file, &(ftr->TEST_TXT));
	_stdf_read_dtc_Cn(file, &(ftr->ALARM_ID));
	_stdf_read_dtc_Cn(file, &(ftr->PROG_TXT));
	_stdf_read_dtc_Cn(file, &(ftr->RSLT_TXT));
	_stdf_read_dtc_U1(file, &(ftr->PATG_NUM));
	_stdf_read_dtc_Dn(file, &(ftr->SPIN_MAP));
	return ftr;
}

rec_bps* stdf_read_rec_bps(stdf_file *file)
{
	rec_bps *bps = __malloc_rec(rec_bps);
	_stdf_read_dtc_Cn(file, &(bps->SEQ_NAME));
	return bps;
}

rec_eps* stdf_read_rec_eps(attribute_unused stdf_file *file)
{
	rec_eps *eps = __malloc_rec(rec_eps);
	return eps;
}

#ifdef STDF_VER3

rec_shb* stdf_read_rec_shb(stdf_file *file)
{
	rec_shb *shb = __malloc_rec(rec_shb);
	_stdf_read_dtc_U1(file, &(shb->HEAD_NUM));
	_stdf_read_dtc_U1(file, &(shb->SITE_NUM));
	_stdf_read_dtc_U2(file, &(shb->HBIN_NUM));
	_stdf_read_dtc_U4(file, &(shb->HBIN_CNT));
	_stdf_read_dtc_Cn(file, &(shb->HBIN_NAM));
	return shb;
}

rec_ssb* stdf_read_rec_ssb(stdf_file *file)
{
	rec_ssb *ssb = __malloc_rec(rec_ssb);
	_stdf_read_dtc_U1(file, &(ssb->HEAD_NUM));
	_stdf_read_dtc_U1(file, &(ssb->SITE_NUM));
	_stdf_read_dtc_U2(file, &(ssb->SBIN_NUM));
	_stdf_read_dtc_U4(file, &(ssb->SBIN_CNT));
	_stdf_read_dtc_Cn(file, &(ssb->SBIN_NAM));
	return ssb;
}

rec_sts* stdf_read_rec_sts(stdf_file *file)
{
	rec_sts *sts = __malloc_rec(rec_sts);
	_stdf_read_dtc_U1(file, &(sts->HEAD_NUM));
	_stdf_read_dtc_U1(file, &(sts->SITE_NUM));
	_stdf_read_dtc_U4(file, &(sts->TEST_NUM));
	_stdf_read_dtc_I4(file, &(sts->EXEC_CNT));
	_stdf_read_dtc_I4(file, &(sts->FAIL_CNT));
	_stdf_read_dtc_I4(file, &(sts->ALRM_CNT));
	_stdf_read_dtc_B1(file, &(sts->OPT_FLAG));
	_stdf_read_dtc_B1(file, &(sts->PAD_BYTE));
	_stdf_read_dtc_R4(file, &(sts->TEST_MIN));
	_stdf_read_dtc_R4(file, &(sts->TEST_MAX));
	_stdf_read_dtc_R4(file, &(sts->TST_MEAN));
	_stdf_read_dtc_R4(file, &(sts->TST_SDEV));
	_stdf_read_dtc_R4(file, &(sts->TST_SUMS));
	_stdf_read_dtc_R4(file, &(sts->TST_SQRS));
	_stdf_read_dtc_Cn(file, &(sts->TEST_NAM));
	_stdf_read_dtc_Cn(file, &(sts->SEQ_NAME));
	_stdf_read_dtc_Cn(file, &(sts->TEST_LBL));
	return sts;
}

rec_scr* stdf_read_rec_scr(stdf_file *file)
{
	rec_scr *scr = __malloc_rec(rec_scr);
	_stdf_read_dtc_U1(file, &(scr->HEAD_NUM));
	_stdf_read_dtc_U1(file, &(scr->SITE_NUM));
	_stdf_read_dtc_U4(file, &(scr->FINISH_T));
	_stdf_read_dtc_U4(file, &(scr->PART_CNT));
	_stdf_read_dtc_I4(file, &(scr->RTST_CNT));
	_stdf_read_dtc_I4(file, &(scr->ABRT_CNT));
	_stdf_read_dtc_I4(file, &(scr->GOOD_CNT));
	_stdf_read_dtc_I4(file, &(scr->FUNC_CNT));
	return scr;
}

#endif

rec_gdr* stdf_read_rec_gdr(stdf_file *file)
{
	rec_gdr *gdr = __malloc_rec(rec_gdr);
	_stdf_read_dtc_U2(file, &(gdr->FLD_CNT));
	_stdf_read_dtc_Vn(file, &(gdr->GEN_DATA), gdr->FLD_CNT);
	return gdr;
}

rec_dtr* stdf_read_rec_dtr(stdf_file *file)
{
	rec_dtr *dtr = __malloc_rec(rec_dtr);
	_stdf_read_dtc_Cn(file, &(dtr->TEXT_DAT));
	return dtr;
}


/*
 * Record utility funcs
 */


static inline size_t _lenCn(dtc_Cn Cn)
{
	return (Cn && Cn[0] ? strlen(Cn) : 1);
}
static inline size_t _lenDn(dtc_Dn Dn)
{
	return (Dn && (*(dtc_U2*)Dn) ? strlen(Dn) : 2);
}
static inline size_t _len_dtcXCN(dtc_xCn xCn, dtc_U2 cnt)
{
	size_t ret = 0;
	while (cnt--)
		ret += _lenCn(xCn[cnt]);
	return ret;
}
#define _len_dtcX(x, cnt) (sizeof(*(x)) * cnt)
#define _len_dtcXN(xn, cnt) (sizeof(*(xn)) * (cnt / 2 + cnt % 2))
#define booga(a,b) 0

static inline size_t _calc_rec_len_far(attribute_unused stdf_file *f, rec_far *r)
{
	return sizeof(r->CPU_TYPE) + sizeof(r->STDF_VER);
}

static inline size_t _calc_rec_len_atr(attribute_unused stdf_file *f, rec_atr *r)
{
	return sizeof(r->MOD_TIM) + _lenCn(r->CMD_LINE);
}

static inline size_t _calc_rec_len_mir(stdf_file *f, rec_mir *r)
{
#define _CALC_REC_LEN_MIR_v3(r) \
	( \
	sizeof(r->CPU_TYPE) + sizeof(r->STDF_VER) + sizeof(r->MODE_COD) + \
	sizeof(r->STAT_NUM) + booga(r->TEST_COD, 3) + sizeof(r->RTST_COD) + \
	sizeof(r->PROT_COD) + sizeof(r->CMOD_COD) + sizeof(r->SETUP_T) + \
	sizeof(r->START_T) + _lenCn(r->LOT_ID) + _lenCn(r->PART_TYP) + \
	_lenCn(r->JOB_NAM) + _lenCn(r->OPER_NAM) + _lenCn(r->NODE_NAM) + \
	_lenCn(r->TSTR_TYP) + _lenCn(r->EXEC_TYP) + _lenCn(r->SUPR_NAM) + \
	_lenCn(r->HAND_ID) + _lenCn(r->SBLOT_ID) + _lenCn(r->JOB_REV) + \
	_lenCn(r->PROC_ID) + _lenCn(r->PRB_CARD) \
	)
#define _CALC_REC_LEN_MIR_v4(r) \
	( \
	sizeof(r->SETUP_T) + sizeof(r->START_T) + sizeof(r->STAT_NUM) + \
	sizeof(r->MODE_COD) + sizeof(r->RTST_COD) + sizeof(r->PROT_COD) + \
	sizeof(r->BURN_TIM) + sizeof(r->CMOD_COD) + \
	_lenCn(r->LOT_ID) + _lenCn(r->PART_TYP) + _lenCn(r->NODE_NAM) + \
	_lenCn(r->TSTR_TYP) + _lenCn(r->JOB_NAM) + _lenCn(r->JOB_REV) + \
	_lenCn(r->SBLOT_ID) + _lenCn(r->OPER_NAM) + _lenCn(r->EXEC_TYP) + \
	_lenCn(r->EXEC_VER) + _lenCn(r->TEST_COD) + _lenCn(r->TST_TEMP) + \
	_lenCn(r->USER_TXT) + _lenCn(r->AUX_FILE) + _lenCn(r->PKG_TYP) + \
	_lenCn(r->FAMILY_ID) + _lenCn(r->DATE_COD) + _lenCn(r->FACIL_ID) + \
	_lenCn(r->FLOOR_ID) + _lenCn(r->PROC_ID) + _lenCn(r->OPER_FRQ) + \
	_lenCn(r->SPEC_NAM) + _lenCn(r->SPEC_VER) + _lenCn(r->FLOW_ID) + \
	_lenCn(r->SETUP_ID) + _lenCn(r->DSGN_REV) + _lenCn(r->ENG_ID) + \
	_lenCn(r->ROM_COD) + _lenCn(r->SERL_NUM) + _lenCn(r->SUPR_NAM) \
	)
#ifndef STDF_VER3
# undef _CALC_REC_LEN_MIR_v3
# define _CALC_REC_LEN_MIR_v3(r) 0
#endif
	return (f->ver == 3 ? _CALC_REC_LEN_MIR_v3(r) : _CALC_REC_LEN_MIR_v4(r));
}

static inline size_t _calc_rec_len_mrr(stdf_file *f, rec_mrr *r)
{
#define _CALC_REC_LEN_MRR_v3(r) \
	( \
	sizeof(r->FINISH_T) + sizeof(r->DISP_COD) + \
	sizeof(r->PART_CNT) + sizeof(r->RTST_CNT) + sizeof(r->ABRT_CNT) + \
	sizeof(r->GOOD_CNT) + sizeof(r->FUNC_CNT) + \
	_lenCn(r->USR_DESC) + _lenCn(r->EXC_DESC) \
	)
#define _CALC_REC_LEN_MRR_v4(r) \
	( \
	sizeof(r->FINISH_T) + sizeof(r->DISP_COD) + \
	_lenCn(r->USR_DESC) + _lenCn(r->EXC_DESC) \
	)
#ifndef STDF_VER3
# undef _CALC_REC_LEN_MRR_v3
# define _CALC_REC_LEN_MRR_v3(r) 0
#endif
	return (f->ver == 3 ? _CALC_REC_LEN_MRR_v3(r) : _CALC_REC_LEN_MRR_v4(r));
}

static inline size_t _calc_rec_len_pcr(attribute_unused stdf_file *f, rec_pcr *r)
{
	return 
		sizeof(r->HEAD_NUM) + sizeof(r->SITE_NUM) + sizeof(r->PART_CNT) +
		sizeof(r->RTST_CNT) + sizeof(r->ABRT_CNT) + sizeof(r->GOOD_CNT) +
		sizeof(r->FUNC_CNT);
}

static inline size_t _calc_rec_len_hbr(attribute_unused stdf_file *f, rec_hbr *r)
{
	return
		sizeof(r->HEAD_NUM) + sizeof(r->SITE_NUM) + sizeof(r->HBIN_NUM) +
		sizeof(r->HBIN_CNT) + sizeof(r->HBIN_PF) + _lenCn(r->HBIN_NAM);
}

static inline size_t _calc_rec_len_sbr(attribute_unused stdf_file *f, rec_sbr *r)
{
	return
		sizeof(r->HEAD_NUM) + sizeof(r->SITE_NUM) + sizeof(r->SBIN_NUM) +
		sizeof(r->SBIN_CNT) + sizeof(r->SBIN_PF) + _lenCn(r->SBIN_NAM);
}

static inline size_t _calc_rec_len_pmr(attribute_unused stdf_file *f, rec_pmr *r)
{
	return
		sizeof(r->PMR_INDX) + sizeof(r->CHAN_TYP) + _lenCn(r->CHAN_NAM) +
		_lenCn(r->PHY_NAM) + _lenCn(r->LOG_NAM) + sizeof(r->HEAD_NUM) +
		sizeof(r->SITE_NUM);
}

static inline size_t _calc_rec_len_pgr(attribute_unused stdf_file *f, rec_pgr *r)
{
	return
		sizeof(r->GRP_INDX) + _lenCn(r->GRP_NAM) +
		sizeof(r->INDX_CNT) + _len_dtcX(r->PMR_INDX, r->INDX_CNT);
}

static inline size_t _calc_rec_len_plr(attribute_unused stdf_file *f, rec_plr *r)
{
	return
		sizeof(r->GRP_CNT) +
		_len_dtcX(r->GRP_INDX, r->GRP_CNT) +
		_len_dtcX(r->GRP_MODE, r->GRP_CNT) + _len_dtcX(r->GRP_RADX, r->GRP_CNT) +
		_len_dtcXCN(r->PGM_CHAR, r->GRP_CNT) + _len_dtcXCN(r->RTN_CHAR, r->GRP_CNT) +
		_len_dtcXCN(r->PGM_CHAL, r->GRP_CNT) + _len_dtcXCN(r->RTN_CHAL, r->GRP_CNT);
}

static inline size_t _calc_rec_len_rdr(attribute_unused stdf_file *f, rec_rdr *r)
{
	return sizeof(r->NUM_BINS) + _len_dtcX(r->RTST_BIN, r->NUM_BINS);
}

static inline size_t _calc_rec_len_sdr(attribute_unused stdf_file *f, rec_sdr *r)
{
	return
		sizeof(r->HEAD_NUM) + sizeof(r->SITE_GRP) + sizeof(r->SITE_CNT) +
		_len_dtcX(r->SITE_NUM, r->SITE_CNT) +
		_lenCn(r->HAND_TYP) + _lenCn(r->HAND_ID) + _lenCn(r->CARD_TYP) +
		_lenCn(r->CARD_ID) + _lenCn(r->LOAD_TYP) + _lenCn(r->LOAD_ID) +
		_lenCn(r->DIB_TYP) + _lenCn(r->DIB_ID) + _lenCn(r->CABL_TYP) +
		_lenCn(r->CABL_ID) + _lenCn(r->CONT_TYP) + _lenCn(r->CONT_ID) +
		_lenCn(r->LASR_TYP) + _lenCn(r->LASR_ID) + _lenCn(r->EXTR_TYP) +
		_lenCn(r->EXTR_ID);
}

static inline size_t _calc_rec_len_wir(stdf_file *f, rec_wir *r)
{
#define _CALC_REC_LEN_WIR_v3(r) \
	( \
	sizeof(r->HEAD_NUM) + sizeof(r->PAD_BYTE) + \
	sizeof(r->START_T) + _lenCn(r->WAFER_ID) \
	)
#define _CALC_REC_LEN_WIR_v4(r) \
	( \
	sizeof(r->HEAD_NUM) + sizeof(r->SITE_GRP) + \
	sizeof(r->START_T) + _lenCn(r->WAFER_ID) \
	)
#ifndef STDF_VER3
# undef _CALC_REC_LEN_WIR_v3
# define _CALC_REC_LEN_WIR_v3(r) 0
#endif
	return (f->ver == 3 ? _CALC_REC_LEN_WIR_v3(r) : _CALC_REC_LEN_WIR_v4(r));
}

static inline size_t _calc_rec_len_wrr(stdf_file *f, rec_wrr *r)
{
#define _CALC_REC_LEN_WRR_v3(r) \
	( \
	sizeof(r->FINISH_T) + sizeof(r->HEAD_NUM) + sizeof(r->PAD_BYTE) + \
	sizeof(r->PART_CNT) + sizeof(r->RTST_CNT) + sizeof(r->ABRT_CNT) + \
	sizeof(r->GOOD_CNT) + sizeof(r->FUNC_CNT) + _lenCn(r->WAFER_ID) + \
	_lenCn(r->HAND_ID) + _lenCn(r->PRB_CARD) + \
	_lenCn(r->USR_DESC) + _lenCn(r->EXC_DESC) \
	)
#define _CALC_REC_LEN_WRR_v4(r) \
	( \
	sizeof(r->HEAD_NUM) + sizeof(r->SITE_GRP) + sizeof(r->FINISH_T) + \
	sizeof(r->PART_CNT) + sizeof(r->RTST_CNT) + sizeof(r->ABRT_CNT) + \
	sizeof(r->GOOD_CNT) + sizeof(r->FUNC_CNT) + _lenCn(r->WAFER_ID) + \
	_lenCn(r->FABWF_ID) + _lenCn(r->FRAME_ID) + _lenCn(r->MASK_ID) + \
	_lenCn(r->USR_DESC) + _lenCn(r->EXC_DESC) \
	)
#ifndef STDF_VER3
# undef _CALC_REC_LEN_WRR_v3
# define _CALC_REC_LEN_WRR_v3(r) 0
#endif
	return (f->ver == 3 ? _CALC_REC_LEN_WRR_v3(r) : _CALC_REC_LEN_WRR_v4(r));
}

static inline size_t _calc_rec_len_wcr(attribute_unused stdf_file *f, rec_wcr *r)
{
	return
		sizeof(r->WAFR_SIZ) + sizeof(r->DIE_HT) + sizeof(r->DIE_WID) +
		sizeof(r->WF_UNITS) + sizeof(r->WF_FLAT) + sizeof(r->CENTER_X) +
		sizeof(r->CENTER_Y) + sizeof(r->POS_X) + sizeof(r->POS_Y);
}

static inline size_t _calc_rec_len_pir(stdf_file *f, rec_pir *r)
{
#define _CALC_REC_LEN_PIR_v3(r) \
	( \
	sizeof(r->HEAD_NUM) + sizeof(r->SITE_NUM) + \
	sizeof(r->X_COORD) + sizeof(r->Y_COORD) + sizeof(r->PART_ID) \
	)
#define _CALC_REC_LEN_PIR_v4(r) \
	( \
	sizeof(r->HEAD_NUM) + sizeof(r->SITE_NUM) \
	)
#ifndef STDF_VER3
# undef _CALC_REC_LEN_PIR_v3
# define _CALC_REC_LEN_PIR_v3(r) 0
#endif
	return (f->ver == 3 ? _CALC_REC_LEN_PIR_v3(r) : _CALC_REC_LEN_PIR_v4(r));
}

static inline size_t _calc_rec_len_prr(stdf_file *f, rec_prr *r)
{
#define _CALC_REC_LEN_PRR_v3(r) \
	( \
	sizeof(r->HEAD_NUM) + sizeof(r->SITE_NUM) + \
	sizeof(r->NUM_TEST) + sizeof(r->HARD_BIN) + sizeof(r->SOFT_BIN) + \
	sizeof(r->PART_FLG) + sizeof(r->PAD_BYTE) + \
	sizeof(r->X_COORD) + sizeof(r->Y_COORD) + \
	_lenCn(r->PART_ID) + _lenCn(r->PART_TXT) + _lenCn(r->PART_FIX) \
	)
#define _CALC_REC_LEN_PRR_v4(r) \
	( \
	sizeof(r->HEAD_NUM) + sizeof(r->SITE_NUM) + \
	sizeof(r->PART_FLG) + \
	sizeof(r->NUM_TEST) + sizeof(r->HARD_BIN) + sizeof(r->SOFT_BIN) + \
	sizeof(r->X_COORD) + sizeof(r->Y_COORD) + \
	sizeof(r->TEST_T) + \
	_lenCn(r->PART_ID) + _lenCn(r->PART_TXT) + _lenCn(r->PART_FIX) \
	)
#ifndef STDF_VER3
# undef _CALC_REC_LEN_PRR_v3
# define _CALC_REC_LEN_PRR_v3(r) 0
#endif
	return (f->ver == 3 ? _CALC_REC_LEN_PRR_v3(r) : _CALC_REC_LEN_PRR_v4(r));
}

#ifdef STDF_VER3

static inline size_t _calc_rec_len_pdr(attribute_unused stdf_file *f, rec_pdr *r)
{
	return
		sizeof(r->TEST_NUM) + sizeof(r->DESC_FLG) + sizeof(r->OPT_FLAG) +
		sizeof(r->RES_SCAL) + booga(r->UNITS, 7) +
		sizeof(r->RES_LDIG) + sizeof(r->RES_RDIG) + sizeof(r->LLM_SCAL) +
		sizeof(r->HLM_SCAL) + sizeof(r->LLM_LDIG) + sizeof(r->LLM_RDIG) +
		sizeof(r->HLM_LDIG) + sizeof(r->HLM_RDIG) + sizeof(r->LO_LIMIT) +
		sizeof(r->HI_LIMIT) + _lenCn(r->TEST_NAM) + _lenCn(r->SEQ_NAME);
}

static inline size_t _calc_rec_len_fdr(attribute_unused stdf_file *f, rec_fdr *r)
{
	return
		sizeof(r->TEST_NUM) + sizeof(r->DESC_FLG) +
		_lenCn(r->TEST_NAM) + _lenCn(r->SEQ_NAME);
}

#endif

static inline size_t _calc_rec_len_tsr(stdf_file *f, rec_tsr *r)
{
#define _CALC_REC_LEN_TSR_v3(r) \
	( \
	sizeof(r->HEAD_NUM) + sizeof(r->SITE_NUM) + \
	sizeof(r->TEST_NUM) + sizeof(r->EXEC_CNT) + sizeof(r->FAIL_CNT) + \
	sizeof(r->ALRM_CNT) + \
	sizeof(r->OPT_FLAG) + sizeof(r->PAD_BYTE) + sizeof(r->TEST_MIN) + \
	sizeof(r->TEST_MAX) + sizeof(r->TST_MEAN) + sizeof(r->TST_SDEV) + \
	sizeof(r->TST_SUMS) + sizeof(r->TST_SQRS) + _lenCn(r->TEST_NAM) + \
	_lenCn(r->SEQ_NAME) \
	)
#define _CALC_REC_LEN_TSR_v4(r) \
	( \
	sizeof(r->HEAD_NUM) + sizeof(r->SITE_NUM) + \
	sizeof(r->TEST_TYP) + \
	sizeof(r->TEST_NUM) + sizeof(r->EXEC_CNT) + sizeof(r->FAIL_CNT) + \
	sizeof(r->ALRM_CNT) + \
	_lenCn(r->TEST_NAM) + _lenCn(r->SEQ_NAME) + _lenCn(r->TEST_LBL) + \
	sizeof(r->OPT_FLAG) + sizeof(r->TEST_TIM) + sizeof(r->TEST_MIN) + \
	sizeof(r->TEST_MAX) + sizeof(r->TST_SUMS) + sizeof(r->TST_SQRS) \
	)
#ifndef STDF_VER3
# undef _CALC_REC_LEN_TSR_v3
# define _CALC_REC_LEN_TSR_v3(r) 0
#endif
	return (f->ver == 3 ? _CALC_REC_LEN_TSR_v3(r) : _CALC_REC_LEN_TSR_v4(r));
}

static inline size_t _calc_rec_len_ptr(attribute_unused stdf_file *f, rec_ptr *r)
{
	return
		sizeof(r->TEST_NUM) + sizeof(r->HEAD_NUM) + sizeof(r->SITE_NUM) +
		sizeof(r->TEST_FLG) + sizeof(r->PARM_FLG) + sizeof(r->RESULT) +
		_lenCn(r->TEST_TXT) + _lenCn(r->ALARM_ID) +
		sizeof(r->OPT_FLAG) + sizeof(r->RES_SCAL) + sizeof(r->LLM_SCAL) +
		sizeof(r->HLM_SCAL) + sizeof(r->LO_LIMIT) + sizeof(r->HI_LIMIT) +
		_lenCn(r->UNITS) + _lenCn(r->C_RESFMT) + _lenCn(r->C_LLMFMT) +
		_lenCn(r->C_HLMFMT) + sizeof(r->LO_SPEC) + sizeof(r->HI_SPEC);
}

static inline size_t _calc_rec_len_mpr(attribute_unused stdf_file *f, rec_mpr *r)
{
	return
		sizeof(r->TEST_NUM) + sizeof(r->HEAD_NUM) + sizeof(r->SITE_NUM) +
		sizeof(r->TEST_FLG) + sizeof(r->PARM_FLG) + sizeof(r->RTN_ICNT) +
		sizeof(r->RSLT_CNT) +
		_len_dtcXN(r->RTN_STAT, r->RTN_ICNT) + _len_dtcX(r->RTN_RSLT, r->RSLT_CNT) +
		_lenCn(r->TEST_TXT) + _lenCn(r->ALARM_ID) + sizeof(r->OPT_FLAG) +
		sizeof(r->RES_SCAL) + sizeof(r->LLM_SCAL) + sizeof(r->HLM_SCAL) +
		sizeof(r->LO_LIMIT) + sizeof(r->HI_LIMIT) + sizeof(r->START_IN) +
		sizeof(r->INCR_IN) + _len_dtcX(r->RTN_INDX, r->RTN_ICNT) +
		_lenCn(r->UNITS) + _lenCn(r->UNITS_IN) + _lenCn(r->C_RESFMT) +
		_lenCn(r->C_LLMFMT) + _lenCn(r->C_HLMFMT) + sizeof(r->LO_SPEC) +
		sizeof(r->HI_SPEC);
}

static inline size_t _calc_rec_len_ftr(attribute_unused stdf_file *f, rec_ftr *r)
{
	return
		sizeof(r->TEST_NUM) + sizeof(r->HEAD_NUM) + sizeof(r->SITE_NUM) +
		sizeof(r->TEST_FLG) + sizeof(r->OPT_FLAG) + sizeof(r->CYCL_CNT) +
		sizeof(r->REL_VADR) + sizeof(r->REPT_CNT) + sizeof(r->NUM_FAIL) +
		sizeof(r->XFAIL_AD) + sizeof(r->YFAIL_AD) + sizeof(r->VECT_OFF) +
		sizeof(r->RTN_ICNT) + sizeof(r->PGM_ICNT) +
		_len_dtcX(r->RTN_INDX, r->RTN_ICNT) + _len_dtcXN(r->RTN_STAT, r->RTN_ICNT) +
		_len_dtcX(r->PGM_INDX, r->PGM_ICNT) + _len_dtcXN(r->PGM_STAT, r->PGM_ICNT) +
		_lenDn(r->FAIL_PIN) + _lenCn(r->VECT_NAM) + _lenCn(r->TIME_SET) +
		_lenCn(r->OP_CODE) + _lenCn(r->TEST_TXT) + _lenCn(r->ALARM_ID) +
		_lenCn(r->PROG_TXT) + _lenCn(r->RSLT_TXT) + sizeof(r->PATG_NUM) +
		_lenDn(r->SPIN_MAP);
}

static inline size_t _calc_rec_len_bps(attribute_unused stdf_file *f, attribute_unused rec_bps *r)
{
	return _lenCn(r->SEQ_NAME);
}

static inline size_t _calc_rec_len_eps(attribute_unused stdf_file *f, attribute_unused rec_eps *r)
{
	return 0;
}

#ifdef STDF_VER3

static inline size_t _calc_rec_len_shb(attribute_unused stdf_file *f, rec_shb *r)
{
	return
		sizeof(r->HEAD_NUM) + sizeof(r->SITE_NUM) + sizeof(r->HBIN_NUM) +
		sizeof(r->HBIN_CNT) + _lenCn(r->HBIN_NAM);
}

static inline size_t _calc_rec_len_ssb(attribute_unused stdf_file *f, rec_ssb *r)
{
	return
		sizeof(r->HEAD_NUM) + sizeof(r->SITE_NUM) + sizeof(r->SBIN_NUM) +
		sizeof(r->SBIN_CNT) + _lenCn(r->SBIN_NAM);
}

static inline size_t _calc_rec_len_sts(attribute_unused stdf_file *f, rec_sts *r)
{
	return
		sizeof(r->HEAD_NUM) + sizeof(r->SITE_NUM) + sizeof(r->TEST_NUM) +
		sizeof(r->EXEC_CNT) + sizeof(r->FAIL_CNT) + sizeof(r->ALRM_CNT) +
		sizeof(r->OPT_FLAG) + sizeof(r->PAD_BYTE) + sizeof(r->TEST_MIN) +
		sizeof(r->TEST_MAX) + sizeof(r->TST_MEAN) + sizeof(r->TST_SDEV) +
		sizeof(r->TST_SUMS) + sizeof(r->TST_SQRS) + _lenCn(r->TEST_NAM) +
		_lenCn(r->SEQ_NAME) + _lenCn(r->TEST_LBL);
}

static inline size_t _calc_rec_len_scr(attribute_unused stdf_file *f, rec_scr *r)
{
	return
		sizeof(r->HEAD_NUM) + sizeof(r->SITE_NUM) + sizeof(r->FINISH_T) +
		sizeof(r->PART_CNT) + sizeof(r->RTST_CNT) + sizeof(r->ABRT_CNT) +
		sizeof(r->GOOD_CNT) + sizeof(r->FUNC_CNT);
}

#endif

static inline size_t _calc_rec_len_gdr(attribute_unused stdf_file *f, attribute_unused rec_gdr *r)
{
	warnf("Not implemented");
	return 0;
}

static inline size_t _calc_rec_len_dtr(attribute_unused stdf_file *f, rec_dtr *r)
{
	return _lenCn(r->TEXT_DAT);
}


/*
 * Writing records
 */


ssize_t _stdf_write_flush(stdf_file *file, size_t count)
{
	size_t write_ret = 1;

	if (count == (size_t)-1) {
		count = file->_write_pos - file->__output;
		if (count == 0)
			return 0;
	} else {
		if (count < file->_write_chunk_size)
			return 0;
		count = file->_write_pos - file->__output;
	}

	file->_write_pos = file->__output;
#if 0
	{
		/* debug */
		ssize_t ret;
		ret = write(file->fd, file->__output, count);
		memset(file->__output, '|', (dtc_U2)-1);
		return ret;
	}
#else
	while (count > 0 && write_ret > 0) {
		write_ret = write(file->fd, file->_write_pos, count);
		if (write_ret > 0) {
			count -= write_ret;
			file->_write_pos += write_ret;
		}
	}

	/* everything was written, happy happy joy joy */
	if (count == 0) {
		errno = 0;
		count = file->_write_pos - file->__output;
		file->_write_pos = file->__output;
		return count;
	}

	/* did we have a partial write ?  if so, report how many bytes we 
	 * managed to write, but keep the rest in the buffer for next time */
	if (file->_write_pos != file->__output) {
		memmove(file->__output, file->_write_pos, count);
		count = file->_write_pos - file->__output;
		file->_write_pos = file->__output;
		return count;
	}

	/* did we totally fail ? */
	return -1;
#endif
}

static inline ssize_t _stdf_check_write_buffer(stdf_file *file, size_t count)
{
	if ((file->_write_pos - file->__output) + count >= (dtc_U2)-1)
		return _stdf_write_flush(file, (size_t)-1);
	else
		return 0;
}

ssize_t stdf_write_rec_far(stdf_file *file, rec_far *far)
{
	if (!far->header.REC_LEN)
		SET_HEADER(far->header, REC_FAR, _calc_rec_len_far(file, far));
	_stdf_check_write_buffer(file, far->header.REC_LEN);
	_stdf_write_dtc_header(file, &(far->header));
	_stdf_write_dtc_U1(file, far->CPU_TYPE);
	_stdf_write_dtc_U1(file, far->STDF_VER);
	return _stdf_write_flush(file, far->header.REC_LEN);
}

ssize_t stdf_write_rec_atr(stdf_file *file, rec_atr *atr)
{
	if (!atr->header.REC_LEN)
		SET_HEADER(atr->header, REC_ATR, _calc_rec_len_atr(file, atr));
	_stdf_check_write_buffer(file, atr->header.REC_LEN);
	_stdf_write_dtc_header(file, &(atr->header));
	_stdf_write_dtc_U4(file, atr->MOD_TIM);
	_stdf_write_dtc_Cn(file, atr->CMD_LINE);
	return _stdf_write_flush(file, atr->header.REC_LEN);
}

ssize_t stdf_write_rec_mir(stdf_file *file, rec_mir *mir)
{
	if (!mir->header.REC_LEN)
		SET_HEADER(mir->header, REC_MIR, _calc_rec_len_mir(file, mir));
	_stdf_check_write_buffer(file, mir->header.REC_LEN);
	_stdf_write_dtc_header(file, &(mir->header));
#ifdef STDF_VER3
	if (file->ver == 4) {
#endif
	_stdf_write_dtc_U4(file, mir->SETUP_T);
	_stdf_write_dtc_U4(file, mir->START_T);
	_stdf_write_dtc_U1(file, mir->STAT_NUM);
	_stdf_write_dtc_C1(file, mir->MODE_COD);
	_stdf_write_dtc_C1(file, mir->RTST_COD);
	_stdf_write_dtc_C1(file, mir->PROT_COD);
	_stdf_write_dtc_U2(file, mir->BURN_TIM);
	_stdf_write_dtc_C1(file, mir->CMOD_COD);
	_stdf_write_dtc_Cn(file, mir->LOT_ID);
	_stdf_write_dtc_Cn(file, mir->PART_TYP);
	_stdf_write_dtc_Cn(file, mir->NODE_NAM);
	_stdf_write_dtc_Cn(file, mir->TSTR_TYP);
	_stdf_write_dtc_Cn(file, mir->JOB_NAM);
	_stdf_write_dtc_Cn(file, mir->JOB_REV);
	_stdf_write_dtc_Cn(file, mir->SBLOT_ID);
	_stdf_write_dtc_Cn(file, mir->OPER_NAM);
	_stdf_write_dtc_Cn(file, mir->EXEC_TYP);
	_stdf_write_dtc_Cn(file, mir->EXEC_VER);
	_stdf_write_dtc_Cn(file, mir->TEST_COD);
	_stdf_write_dtc_Cn(file, mir->TST_TEMP);
	_stdf_write_dtc_Cn(file, mir->USER_TXT);
	_stdf_write_dtc_Cn(file, mir->AUX_FILE);
	_stdf_write_dtc_Cn(file, mir->PKG_TYP);
	_stdf_write_dtc_Cn(file, mir->FAMILY_ID);
	_stdf_write_dtc_Cn(file, mir->DATE_COD);
	_stdf_write_dtc_Cn(file, mir->FACIL_ID);
	_stdf_write_dtc_Cn(file, mir->FLOOR_ID);
	_stdf_write_dtc_Cn(file, mir->PROC_ID);
	_stdf_write_dtc_Cn(file, mir->OPER_FRQ);
	_stdf_write_dtc_Cn(file, mir->SPEC_NAM);
	_stdf_write_dtc_Cn(file, mir->SPEC_VER);
	_stdf_write_dtc_Cn(file, mir->FLOW_ID);
	_stdf_write_dtc_Cn(file, mir->SETUP_ID);
	_stdf_write_dtc_Cn(file, mir->DSGN_REV);
	_stdf_write_dtc_Cn(file, mir->ENG_ID);
	_stdf_write_dtc_Cn(file, mir->ROM_COD);
	_stdf_write_dtc_Cn(file, mir->SERL_NUM);
	_stdf_write_dtc_Cn(file, mir->SUPR_NAM);
#ifdef STDF_VER3
	} else {
	_stdf_write_dtc_U1(file, mir->CPU_TYPE);
	_stdf_write_dtc_U1(file, mir->STDF_VER);
	_stdf_write_dtc_C1(file, mir->MODE_COD);
	_stdf_write_dtc_U1(file, mir->STAT_NUM);
	_stdf_write_dtc_Cx(file, mir->TEST_COD, 3);
	_stdf_write_dtc_C1(file, mir->RTST_COD);
	_stdf_write_dtc_C1(file, mir->PROT_COD);
	_stdf_write_dtc_C1(file, mir->CMOD_COD);
	_stdf_write_dtc_U4(file, mir->SETUP_T);
	_stdf_write_dtc_U4(file, mir->START_T);
	_stdf_write_dtc_Cn(file, mir->LOT_ID);
	_stdf_write_dtc_Cn(file, mir->PART_TYP);
	_stdf_write_dtc_Cn(file, mir->JOB_NAM);
	_stdf_write_dtc_Cn(file, mir->OPER_NAM);
	_stdf_write_dtc_Cn(file, mir->NODE_NAM);
	_stdf_write_dtc_Cn(file, mir->TSTR_TYP);
	_stdf_write_dtc_Cn(file, mir->EXEC_TYP);
	_stdf_write_dtc_Cn(file, mir->SUPR_NAM);
	_stdf_write_dtc_Cn(file, mir->HAND_ID);
	_stdf_write_dtc_Cn(file, mir->SBLOT_ID);
	_stdf_write_dtc_Cn(file, mir->JOB_REV);
	_stdf_write_dtc_Cn(file, mir->PROC_ID);
	_stdf_write_dtc_Cn(file, mir->PRB_CARD);
	}
#endif
	return _stdf_write_flush(file, mir->header.REC_LEN);
}

ssize_t stdf_write_rec_mrr(stdf_file *file, rec_mrr *mrr)
{
	if (!mrr->header.REC_LEN)
		SET_HEADER(mrr->header, REC_MRR, _calc_rec_len_mrr(file, mrr));
	_stdf_check_write_buffer(file, mrr->header.REC_LEN);
	_stdf_write_dtc_header(file, &(mrr->header));
	_stdf_write_dtc_U4(file, mrr->FINISH_T);
#ifdef STDF_VER3
	if (file->ver == 3) {
	_stdf_write_dtc_U4(file, mrr->PART_CNT);
	_stdf_write_dtc_U4(file, mrr->RTST_CNT);
	_stdf_write_dtc_U4(file, mrr->ABRT_CNT);
	_stdf_write_dtc_U4(file, mrr->GOOD_CNT);
	_stdf_write_dtc_U4(file, mrr->FUNC_CNT);
	}
#endif
	_stdf_write_dtc_C1(file, mrr->DISP_COD);
	_stdf_write_dtc_Cn(file, mrr->USR_DESC);
	_stdf_write_dtc_Cn(file, mrr->EXC_DESC);
	return _stdf_write_flush(file, mrr->header.REC_LEN);
}

ssize_t stdf_write_rec_pcr(stdf_file *file, rec_pcr *pcr)
{
	if (!pcr->header.REC_LEN)
		SET_HEADER(pcr->header, REC_PCR, _calc_rec_len_pcr(file, pcr));
	_stdf_check_write_buffer(file, pcr->header.REC_LEN);
	_stdf_write_dtc_header(file, &(pcr->header));
	_stdf_write_dtc_U1(file, pcr->HEAD_NUM);
	_stdf_write_dtc_U1(file, pcr->SITE_NUM);
	_stdf_write_dtc_U4(file, pcr->PART_CNT);
	_stdf_write_dtc_U4(file, pcr->RTST_CNT);
	_stdf_write_dtc_U4(file, pcr->ABRT_CNT);
	_stdf_write_dtc_U4(file, pcr->GOOD_CNT);
	_stdf_write_dtc_U4(file, pcr->FUNC_CNT);
	return _stdf_write_flush(file, pcr->header.REC_LEN);
}

ssize_t stdf_write_rec_hbr(stdf_file *file, rec_hbr *hbr)
{
	if (!hbr->header.REC_LEN)
		SET_HEADER(hbr->header, REC_HBR, _calc_rec_len_hbr(file, hbr));
	_stdf_check_write_buffer(file, hbr->header.REC_LEN);
	_stdf_write_dtc_header(file, &(hbr->header));
	_stdf_write_dtc_U1(file, hbr->HEAD_NUM);
	_stdf_write_dtc_U1(file, hbr->SITE_NUM);
	_stdf_write_dtc_U2(file, hbr->HBIN_NUM);
	_stdf_write_dtc_U4(file, hbr->HBIN_CNT);
	_stdf_write_dtc_C1(file, hbr->HBIN_PF);
	_stdf_write_dtc_Cn(file, hbr->HBIN_NAM);
	return _stdf_write_flush(file, hbr->header.REC_LEN);
}

ssize_t stdf_write_rec_sbr(stdf_file *file, rec_sbr *sbr)
{
	if (!sbr->header.REC_LEN)
		SET_HEADER(sbr->header, REC_SBR, _calc_rec_len_sbr(file, sbr));
	_stdf_check_write_buffer(file, sbr->header.REC_LEN);
	_stdf_write_dtc_header(file, &(sbr->header));
	_stdf_write_dtc_U1(file, sbr->HEAD_NUM);
	_stdf_write_dtc_U1(file, sbr->SITE_NUM);
	_stdf_write_dtc_U2(file, sbr->SBIN_NUM);
	_stdf_write_dtc_U4(file, sbr->SBIN_CNT);
	_stdf_write_dtc_C1(file, sbr->SBIN_PF);
	_stdf_write_dtc_Cn(file, sbr->SBIN_NAM);
	return _stdf_write_flush(file, sbr->header.REC_LEN);
}

ssize_t stdf_write_rec_pmr(stdf_file *file, rec_pmr *pmr)
{
	if (!pmr->header.REC_LEN)
		SET_HEADER(pmr->header, REC_PMR, _calc_rec_len_pmr(file, pmr));
	_stdf_check_write_buffer(file, pmr->header.REC_LEN);
	_stdf_write_dtc_header(file, &(pmr->header));
	_stdf_write_dtc_U2(file, pmr->PMR_INDX);
	_stdf_write_dtc_U2(file, pmr->CHAN_TYP);
	_stdf_write_dtc_Cn(file, pmr->CHAN_NAM);
	_stdf_write_dtc_Cn(file, pmr->PHY_NAM);
	_stdf_write_dtc_Cn(file, pmr->LOG_NAM);
	_stdf_write_dtc_U1(file, pmr->HEAD_NUM);
	_stdf_write_dtc_U1(file, pmr->SITE_NUM);
	return _stdf_write_flush(file, pmr->header.REC_LEN);
}

ssize_t stdf_write_rec_pgr(stdf_file *file, rec_pgr *pgr)
{
	if (!pgr->header.REC_LEN)
		SET_HEADER(pgr->header, REC_PGR, _calc_rec_len_pgr(file, pgr));
	_stdf_check_write_buffer(file, pgr->header.REC_LEN);
	_stdf_write_dtc_header(file, &(pgr->header));
	_stdf_write_dtc_U2(file, pgr->GRP_INDX);
	_stdf_write_dtc_Cn(file, pgr->GRP_NAM);
	_stdf_write_dtc_U2(file, pgr->INDX_CNT);
	_stdf_write_dtc_xU2(file, pgr->PMR_INDX, pgr->INDX_CNT);
	return _stdf_write_flush(file, pgr->header.REC_LEN);
}

ssize_t stdf_write_rec_plr(stdf_file *file, rec_plr *plr)
{
	warn_untested("PLR");
	if (!plr->header.REC_LEN)
		SET_HEADER(plr->header, REC_PLR, _calc_rec_len_plr(file, plr));
	_stdf_check_write_buffer(file, plr->header.REC_LEN);
	_stdf_write_dtc_header(file, &(plr->header));
	_stdf_write_dtc_U2(file, plr->GRP_CNT);
	_stdf_write_dtc_xU2(file, plr->GRP_INDX, plr->GRP_CNT);
	_stdf_write_dtc_xU2(file, plr->GRP_MODE, plr->GRP_CNT);
	_stdf_write_dtc_xU1(file, plr->GRP_RADX, plr->GRP_CNT);
	_stdf_write_dtc_xCn(file, plr->PGM_CHAR, plr->GRP_CNT);
	_stdf_write_dtc_xCn(file, plr->RTN_CHAR, plr->GRP_CNT);
	_stdf_write_dtc_xCn(file, plr->PGM_CHAL, plr->GRP_CNT);
	_stdf_write_dtc_xCn(file, plr->RTN_CHAL, plr->GRP_CNT);
	return _stdf_write_flush(file, plr->header.REC_LEN);
}

ssize_t stdf_write_rec_rdr(stdf_file *file, rec_rdr *rdr)
{
	if (!rdr->header.REC_LEN)
		SET_HEADER(rdr->header, REC_RDR, _calc_rec_len_rdr(file, rdr));
	_stdf_check_write_buffer(file, rdr->header.REC_LEN);
	_stdf_write_dtc_header(file, &(rdr->header));
	_stdf_write_dtc_U2(file, rdr->NUM_BINS);
	_stdf_write_dtc_xU2(file, rdr->RTST_BIN, rdr->NUM_BINS);
	return _stdf_write_flush(file, rdr->header.REC_LEN);
}

ssize_t stdf_write_rec_sdr(stdf_file *file, rec_sdr *sdr)
{
	if (!sdr->header.REC_LEN)
		SET_HEADER(sdr->header, REC_SDR, _calc_rec_len_sdr(file, sdr));
	_stdf_check_write_buffer(file, sdr->header.REC_LEN);
	_stdf_write_dtc_header(file, &(sdr->header));
	_stdf_write_dtc_U1(file, sdr->HEAD_NUM);
	_stdf_write_dtc_U1(file, sdr->SITE_GRP);
	_stdf_write_dtc_U1(file, sdr->SITE_CNT);
	_stdf_write_dtc_xU1(file, sdr->SITE_NUM, sdr->SITE_CNT);
	_stdf_write_dtc_Cn(file, sdr->HAND_TYP);
	_stdf_write_dtc_Cn(file, sdr->HAND_ID);
	_stdf_write_dtc_Cn(file, sdr->CARD_TYP);
	_stdf_write_dtc_Cn(file, sdr->CARD_ID);
	_stdf_write_dtc_Cn(file, sdr->LOAD_TYP);
	_stdf_write_dtc_Cn(file, sdr->LOAD_ID);
	_stdf_write_dtc_Cn(file, sdr->DIB_TYP);
	_stdf_write_dtc_Cn(file, sdr->DIB_ID);
	_stdf_write_dtc_Cn(file, sdr->CABL_TYP);
	_stdf_write_dtc_Cn(file, sdr->CABL_ID);
	_stdf_write_dtc_Cn(file, sdr->CONT_TYP);
	_stdf_write_dtc_Cn(file, sdr->CONT_ID);
	_stdf_write_dtc_Cn(file, sdr->LASR_TYP);
	_stdf_write_dtc_Cn(file, sdr->LASR_ID);
	_stdf_write_dtc_Cn(file, sdr->EXTR_TYP);
	_stdf_write_dtc_Cn(file, sdr->EXTR_ID);
	return _stdf_write_flush(file, sdr->header.REC_LEN);
}

ssize_t stdf_write_rec_wir(stdf_file *file, rec_wir *wir)
{
	if (!wir->header.REC_LEN)
		SET_HEADER(wir->header, REC_WIR, _calc_rec_len_wir(file, wir));
	_stdf_check_write_buffer(file, wir->header.REC_LEN);
	_stdf_write_dtc_header(file, &(wir->header));
	_stdf_write_dtc_U1(file, wir->HEAD_NUM);
#ifdef STDF_VER3
	if (file->ver == 3)
	_stdf_write_dtc_B1(file, wir->PAD_BYTE);
	else
#endif
	_stdf_write_dtc_U1(file, wir->SITE_GRP);
	_stdf_write_dtc_U4(file, wir->START_T);
	_stdf_write_dtc_Cn(file, wir->WAFER_ID);
	return _stdf_write_flush(file, wir->header.REC_LEN);
}

ssize_t stdf_write_rec_wrr(stdf_file *file, rec_wrr *wrr)
{
	if (!wrr->header.REC_LEN)
		SET_HEADER(wrr->header, REC_WRR, _calc_rec_len_wrr(file, wrr));
	_stdf_check_write_buffer(file, wrr->header.REC_LEN);
	_stdf_write_dtc_header(file, &(wrr->header));
#ifdef STDF_VER3
	if (file->ver == 4) {
#endif
	_stdf_write_dtc_U1(file, wrr->HEAD_NUM);
	_stdf_write_dtc_U1(file, wrr->SITE_GRP);
	_stdf_write_dtc_U4(file, wrr->FINISH_T);
#ifdef STDF_VER3
	} else {
	_stdf_write_dtc_U4(file, wrr->FINISH_T);
	_stdf_write_dtc_U1(file, wrr->HEAD_NUM);
	_stdf_write_dtc_B1(file, wrr->PAD_BYTE);
	}
#endif
	_stdf_write_dtc_U4(file, wrr->PART_CNT);
	_stdf_write_dtc_U4(file, wrr->RTST_CNT);
	_stdf_write_dtc_U4(file, wrr->ABRT_CNT);
	_stdf_write_dtc_U4(file, wrr->GOOD_CNT);
	_stdf_write_dtc_U4(file, wrr->FUNC_CNT);
	_stdf_write_dtc_Cn(file, wrr->WAFER_ID);
#ifdef STDF_VER3
	if (file->ver == 4) {
#endif
	_stdf_write_dtc_Cn(file, wrr->FABWF_ID);
	_stdf_write_dtc_Cn(file, wrr->FRAME_ID);
	_stdf_write_dtc_Cn(file, wrr->MASK_ID);
#ifdef STDF_VER3
	} else {
	_stdf_write_dtc_Cn(file, wrr->HAND_ID);
	_stdf_write_dtc_Cn(file, wrr->PRB_CARD);
	}
#endif
	_stdf_write_dtc_Cn(file, wrr->USR_DESC);
	_stdf_write_dtc_Cn(file, wrr->EXC_DESC);
	return _stdf_write_flush(file, wrr->header.REC_LEN);
}

ssize_t stdf_write_rec_wcr(stdf_file *file, rec_wcr *wcr)
{
	if (!wcr->header.REC_LEN)
		SET_HEADER(wcr->header, REC_WCR, _calc_rec_len_wcr(file, wcr));
	_stdf_check_write_buffer(file, wcr->header.REC_LEN);
	_stdf_write_dtc_header(file, &(wcr->header));
	_stdf_write_dtc_R4(file, wcr->WAFR_SIZ);
	_stdf_write_dtc_R4(file, wcr->DIE_HT);
	_stdf_write_dtc_R4(file, wcr->DIE_WID);
	_stdf_write_dtc_U1(file, wcr->WF_UNITS);
	_stdf_write_dtc_C1(file, wcr->WF_FLAT);
	_stdf_write_dtc_I2(file, wcr->CENTER_X);
	_stdf_write_dtc_I2(file, wcr->CENTER_Y);
	_stdf_write_dtc_C1(file, wcr->POS_X);
	_stdf_write_dtc_C1(file, wcr->POS_Y);
	return _stdf_write_flush(file, wcr->header.REC_LEN);
}

ssize_t stdf_write_rec_pir(stdf_file *file, rec_pir *pir)
{
	if (!pir->header.REC_LEN)
		SET_HEADER(pir->header, REC_PIR, _calc_rec_len_pir(file, pir));
	_stdf_check_write_buffer(file, pir->header.REC_LEN);
	_stdf_write_dtc_header(file, &(pir->header));
	_stdf_write_dtc_U1(file, pir->HEAD_NUM);
	_stdf_write_dtc_U1(file, pir->SITE_NUM);
#ifdef STDF_VER3
	if (file->ver == 3) {
	_stdf_write_dtc_I2(file, pir->X_COORD);
	_stdf_write_dtc_I2(file, pir->Y_COORD);
	_stdf_write_dtc_Cn(file, pir->PART_ID);
	}
#endif
	return _stdf_write_flush(file, pir->header.REC_LEN);
}

ssize_t stdf_write_rec_prr(stdf_file *file, rec_prr *prr)
{
	if (!prr->header.REC_LEN)
		SET_HEADER(prr->header, REC_PRR, _calc_rec_len_prr(file, prr));
	_stdf_check_write_buffer(file, prr->header.REC_LEN);
	_stdf_write_dtc_header(file, &(prr->header));
	_stdf_write_dtc_U1(file, prr->HEAD_NUM);
	_stdf_write_dtc_U1(file, prr->SITE_NUM);
#ifdef STDF_VER3
	if (file->ver == 4)
#endif
	_stdf_write_dtc_B1(file, prr->PART_FLG);
	_stdf_write_dtc_U2(file, prr->NUM_TEST);
	_stdf_write_dtc_U2(file, prr->HARD_BIN);
	_stdf_write_dtc_U2(file, prr->SOFT_BIN);
#ifdef STDF_VER3
	if (file->ver == 3) {
	_stdf_write_dtc_B1(file, prr->PART_FLG);
	_stdf_write_dtc_B1(file, prr->PAD_BYTE);
	}
#endif
	_stdf_write_dtc_I2(file, prr->X_COORD);
	_stdf_write_dtc_I2(file, prr->Y_COORD);
#ifdef STDF_VER3
	if (file->ver == 4)
#endif
	_stdf_write_dtc_U4(file, prr->TEST_T);
	_stdf_write_dtc_Cn(file, prr->PART_ID);
	_stdf_write_dtc_Cn(file, prr->PART_TXT);
	_stdf_write_dtc_Bn(file, prr->PART_FIX);
	return _stdf_write_flush(file, prr->header.REC_LEN);
}

#ifdef STDF_VER3

ssize_t stdf_write_rec_pdr(stdf_file *file, rec_pdr *pdr)
{
	if (!pdr->header.REC_LEN)
		SET_HEADER(pdr->header, REC_PDR, _calc_rec_len_pdr(file, pdr));
	_stdf_check_write_buffer(file, pdr->header.REC_LEN);
	_stdf_write_dtc_header(file, &(pdr->header));
	_stdf_write_dtc_U4(file, pdr->TEST_NUM);
	_stdf_write_dtc_B1(file, pdr->DESC_FLG);
	_stdf_write_dtc_B1(file, pdr->OPT_FLAG);
	_stdf_write_dtc_I1(file, pdr->RES_SCAL);
	_stdf_write_dtc_Cx(file, pdr->UNITS, 7);
	_stdf_write_dtc_U1(file, pdr->RES_LDIG);
	_stdf_write_dtc_U1(file, pdr->RES_RDIG);
	_stdf_write_dtc_I1(file, pdr->LLM_SCAL);
	_stdf_write_dtc_I1(file, pdr->HLM_SCAL);
	_stdf_write_dtc_U1(file, pdr->LLM_LDIG);
	_stdf_write_dtc_U1(file, pdr->LLM_RDIG);
	_stdf_write_dtc_U1(file, pdr->HLM_LDIG);
	_stdf_write_dtc_U1(file, pdr->HLM_RDIG);
	_stdf_write_dtc_R4(file, pdr->LO_LIMIT);
	_stdf_write_dtc_R4(file, pdr->HI_LIMIT);
	_stdf_write_dtc_Cn(file, pdr->TEST_NAM);
	_stdf_write_dtc_Cn(file, pdr->SEQ_NAME);
	return _stdf_write_flush(file, pdr->header.REC_LEN);
}

ssize_t stdf_write_rec_fdr(stdf_file *file, rec_fdr *fdr)
{
	if (!fdr->header.REC_LEN)
		SET_HEADER(fdr->header, REC_FDR, _calc_rec_len_fdr(file, fdr));
	_stdf_check_write_buffer(file, fdr->header.REC_LEN);
	_stdf_write_dtc_header(file, &(fdr->header));
	_stdf_write_dtc_U4(file, fdr->TEST_NUM);
	_stdf_write_dtc_B1(file, fdr->DESC_FLG);
	_stdf_write_dtc_Cn(file, fdr->TEST_NAM);
	_stdf_write_dtc_Cn(file, fdr->SEQ_NAME);
	return _stdf_write_flush(file, fdr->header.REC_LEN);
}

#endif

ssize_t stdf_write_rec_tsr(stdf_file *file, rec_tsr *tsr)
{
	if (!tsr->header.REC_LEN)
		SET_HEADER(tsr->header, REC_TSR, _calc_rec_len_tsr(file, tsr));
	_stdf_check_write_buffer(file, tsr->header.REC_LEN);
	_stdf_write_dtc_header(file, &(tsr->header));
	_stdf_write_dtc_U1(file, tsr->HEAD_NUM);
	_stdf_write_dtc_U1(file, tsr->SITE_NUM);
#ifdef STDF_VER3
	if (file->ver == 4)
#endif
	_stdf_write_dtc_C1(file, tsr->TEST_TYP);
	_stdf_write_dtc_U4(file, tsr->TEST_NUM);
	_stdf_write_dtc_U4(file, tsr->EXEC_CNT);
	_stdf_write_dtc_U4(file, tsr->FAIL_CNT);
	_stdf_write_dtc_U4(file, tsr->ALRM_CNT);
#ifdef STDF_VER3
	if (file->ver == 4) {
#endif
	_stdf_write_dtc_Cn(file, tsr->TEST_NAM);
	_stdf_write_dtc_Cn(file, tsr->SEQ_NAME);
	_stdf_write_dtc_Cn(file, tsr->TEST_LBL);
	_stdf_write_dtc_B1(file, tsr->OPT_FLAG);
	_stdf_write_dtc_R4(file, tsr->TEST_TIM);
	_stdf_write_dtc_R4(file, tsr->TEST_MIN);
	_stdf_write_dtc_R4(file, tsr->TEST_MAX);
	_stdf_write_dtc_R4(file, tsr->TST_SUMS);
	_stdf_write_dtc_R4(file, tsr->TST_SQRS);
#ifdef STDF_VER3
	} else {
	_stdf_write_dtc_B1(file, tsr->OPT_FLAG);
	_stdf_write_dtc_B1(file, tsr->PAD_BYTE);
	_stdf_write_dtc_R4(file, tsr->TEST_MIN);
	_stdf_write_dtc_R4(file, tsr->TEST_MAX);
	_stdf_write_dtc_R4(file, tsr->TST_MEAN);
	_stdf_write_dtc_R4(file, tsr->TST_SDEV);
	_stdf_write_dtc_R4(file, tsr->TST_SUMS);
	_stdf_write_dtc_R4(file, tsr->TST_SQRS);
	_stdf_write_dtc_Cn(file, tsr->TEST_NAM);
	_stdf_write_dtc_Cn(file, tsr->SEQ_NAME);
	}
#endif
	return _stdf_write_flush(file, tsr->header.REC_LEN);
}

ssize_t stdf_write_rec_ptr(stdf_file *file, rec_ptr *ptr)
{
	if (!ptr->header.REC_LEN)
		SET_HEADER(ptr->header, REC_PTR, _calc_rec_len_ptr(file, ptr));
	_stdf_check_write_buffer(file, ptr->header.REC_LEN);
	_stdf_write_dtc_header(file, &(ptr->header));
	_stdf_write_dtc_U4(file, ptr->TEST_NUM);
	_stdf_write_dtc_U1(file, ptr->HEAD_NUM);
	_stdf_write_dtc_U1(file, ptr->SITE_NUM);
	_stdf_write_dtc_B1(file, ptr->TEST_FLG);
	_stdf_write_dtc_B1(file, ptr->PARM_FLG);
	_stdf_write_dtc_R4(file, ptr->RESULT);
	_stdf_write_dtc_Cn(file, ptr->TEST_TXT);
	_stdf_write_dtc_Cn(file, ptr->ALARM_ID);
	_stdf_write_dtc_B1(file, ptr->OPT_FLAG);
	_stdf_write_dtc_I1(file, ptr->RES_SCAL);
	_stdf_write_dtc_I1(file, ptr->LLM_SCAL);
	_stdf_write_dtc_I1(file, ptr->HLM_SCAL);
	_stdf_write_dtc_R4(file, ptr->LO_LIMIT);
	_stdf_write_dtc_R4(file, ptr->HI_LIMIT);
	_stdf_write_dtc_Cn(file, ptr->UNITS);
	_stdf_write_dtc_Cn(file, ptr->C_RESFMT);
	_stdf_write_dtc_Cn(file, ptr->C_LLMFMT);
	_stdf_write_dtc_Cn(file, ptr->C_HLMFMT);
	_stdf_write_dtc_R4(file, ptr->LO_SPEC);
	_stdf_write_dtc_R4(file, ptr->HI_SPEC);
	return _stdf_write_flush(file, ptr->header.REC_LEN);
}

ssize_t stdf_write_rec_mpr(stdf_file *file, rec_mpr *mpr)
{
	if (!mpr->header.REC_LEN)
		SET_HEADER(mpr->header, REC_MPR, _calc_rec_len_mpr(file, mpr));
	_stdf_check_write_buffer(file, mpr->header.REC_LEN);
	_stdf_write_dtc_header(file, &(mpr->header));
	_stdf_write_dtc_U4(file, mpr->TEST_NUM);
	_stdf_write_dtc_U1(file, mpr->HEAD_NUM);
	_stdf_write_dtc_U1(file, mpr->SITE_NUM);
	_stdf_write_dtc_B1(file, mpr->TEST_FLG);
	_stdf_write_dtc_B1(file, mpr->PARM_FLG);
	_stdf_write_dtc_U2(file, mpr->RTN_ICNT);
	_stdf_write_dtc_U2(file, mpr->RSLT_CNT);
	_stdf_write_dtc_xN1(file, mpr->RTN_STAT, mpr->RTN_ICNT);
	_stdf_write_dtc_xR4(file, mpr->RTN_RSLT, mpr->RSLT_CNT);
	_stdf_write_dtc_Cn(file, mpr->TEST_TXT);
	_stdf_write_dtc_Cn(file, mpr->ALARM_ID);
	_stdf_write_dtc_B1(file, mpr->OPT_FLAG);
	_stdf_write_dtc_I1(file, mpr->RES_SCAL);
	_stdf_write_dtc_I1(file, mpr->LLM_SCAL);
	_stdf_write_dtc_I1(file, mpr->HLM_SCAL);
	_stdf_write_dtc_R4(file, mpr->LO_LIMIT);
	_stdf_write_dtc_R4(file, mpr->HI_LIMIT);
	_stdf_write_dtc_R4(file, mpr->START_IN);
	_stdf_write_dtc_R4(file, mpr->INCR_IN);
	_stdf_write_dtc_xU2(file, mpr->RTN_INDX, mpr->RTN_ICNT);
	_stdf_write_dtc_Cn(file, mpr->UNITS);
	_stdf_write_dtc_Cn(file, mpr->UNITS_IN);
	_stdf_write_dtc_Cn(file, mpr->C_RESFMT);
	_stdf_write_dtc_Cn(file, mpr->C_LLMFMT);
	_stdf_write_dtc_Cn(file, mpr->C_HLMFMT);
	_stdf_write_dtc_R4(file, mpr->LO_SPEC);
	_stdf_write_dtc_R4(file, mpr->HI_SPEC);
	return _stdf_write_flush(file, mpr->header.REC_LEN);
}

ssize_t stdf_write_rec_ftr(stdf_file *file, rec_ftr *ftr)
{
	if (!ftr->header.REC_LEN)
		SET_HEADER(ftr->header, REC_FTR, _calc_rec_len_ftr(file, ftr));
	_stdf_check_write_buffer(file, ftr->header.REC_LEN);
	_stdf_write_dtc_header(file, &(ftr->header));
	_stdf_write_dtc_U4(file, ftr->TEST_NUM);
	_stdf_write_dtc_U1(file, ftr->HEAD_NUM);
	_stdf_write_dtc_U1(file, ftr->SITE_NUM);
	_stdf_write_dtc_B1(file, ftr->TEST_FLG);
	_stdf_write_dtc_B1(file, ftr->OPT_FLAG);
	_stdf_write_dtc_U4(file, ftr->CYCL_CNT);
	_stdf_write_dtc_U4(file, ftr->REL_VADR);
	_stdf_write_dtc_U4(file, ftr->REPT_CNT);
	_stdf_write_dtc_U4(file, ftr->NUM_FAIL);
	_stdf_write_dtc_I4(file, ftr->XFAIL_AD);
	_stdf_write_dtc_I4(file, ftr->YFAIL_AD);
	_stdf_write_dtc_I2(file, ftr->VECT_OFF);
	_stdf_write_dtc_U2(file, ftr->RTN_ICNT);
	_stdf_write_dtc_U2(file, ftr->PGM_ICNT);
	_stdf_write_dtc_xU2(file, ftr->RTN_INDX, ftr->RTN_ICNT);
	_stdf_write_dtc_xN1(file, ftr->RTN_STAT, ftr->RTN_ICNT);
	_stdf_write_dtc_xU2(file, ftr->PGM_INDX, ftr->PGM_ICNT);
	_stdf_write_dtc_xN1(file, ftr->PGM_STAT, ftr->PGM_ICNT);
	_stdf_write_dtc_Dn(file, ftr->FAIL_PIN);
	_stdf_write_dtc_Cn(file, ftr->VECT_NAM);
	_stdf_write_dtc_Cn(file, ftr->TIME_SET);
	_stdf_write_dtc_Cn(file, ftr->OP_CODE);
	_stdf_write_dtc_Cn(file, ftr->TEST_TXT);
	_stdf_write_dtc_Cn(file, ftr->ALARM_ID);
	_stdf_write_dtc_Cn(file, ftr->PROG_TXT);
	_stdf_write_dtc_Cn(file, ftr->RSLT_TXT);
	_stdf_write_dtc_U1(file, ftr->PATG_NUM);
	_stdf_write_dtc_Dn(file, ftr->SPIN_MAP);
	return _stdf_write_flush(file, ftr->header.REC_LEN);
}

ssize_t stdf_write_rec_bps(stdf_file *file, rec_bps *bps)
{
	if (!bps->header.REC_LEN)
		SET_HEADER(bps->header, REC_BPS, _calc_rec_len_bps(file, bps));
	_stdf_check_write_buffer(file, bps->header.REC_LEN);
	_stdf_write_dtc_header(file, &(bps->header));
	_stdf_write_dtc_Cn(file, bps->SEQ_NAME);
	return _stdf_write_flush(file, bps->header.REC_LEN);
}

ssize_t stdf_write_rec_eps(stdf_file *file, rec_eps *eps)
{
	if (!eps->header.REC_LEN)
		SET_HEADER(eps->header, REC_EPS, _calc_rec_len_eps(file, eps));
	_stdf_check_write_buffer(file, eps->header.REC_LEN);
	_stdf_write_dtc_header(file, &(eps->header));
	return _stdf_write_flush(file, eps->header.REC_LEN);
}

#ifdef STDF_VER3

ssize_t stdf_write_rec_shb(stdf_file *file, rec_shb *shb)
{
	if (!shb->header.REC_LEN)
		SET_HEADER(shb->header, REC_SHB, _calc_rec_len_shb(file, shb));
	_stdf_check_write_buffer(file, shb->header.REC_LEN);
	_stdf_write_dtc_header(file, &(shb->header));
	_stdf_write_dtc_U1(file, shb->HEAD_NUM);
	_stdf_write_dtc_U1(file, shb->SITE_NUM);
	_stdf_write_dtc_U2(file, shb->HBIN_NUM);
	_stdf_write_dtc_U4(file, shb->HBIN_CNT);
	_stdf_write_dtc_Cn(file, shb->HBIN_NAM);
	return _stdf_write_flush(file, shb->header.REC_LEN);
}

ssize_t stdf_write_rec_ssb(stdf_file *file, rec_ssb *ssb)
{
	if (!ssb->header.REC_LEN)
		SET_HEADER(ssb->header, REC_SSB, _calc_rec_len_ssb(file, ssb));
	_stdf_check_write_buffer(file, ssb->header.REC_LEN);
	_stdf_write_dtc_header(file, &(ssb->header));
	_stdf_write_dtc_U1(file, ssb->HEAD_NUM);
	_stdf_write_dtc_U1(file, ssb->SITE_NUM);
	_stdf_write_dtc_U2(file, ssb->SBIN_NUM);
	_stdf_write_dtc_U4(file, ssb->SBIN_CNT);
	_stdf_write_dtc_Cn(file, ssb->SBIN_NAM);
	return _stdf_write_flush(file, ssb->header.REC_LEN);
}

ssize_t stdf_write_rec_sts(stdf_file *file, rec_sts *sts)
{
	if (!sts->header.REC_LEN)
		SET_HEADER(sts->header, REC_STS, _calc_rec_len_sts(file, sts));
	_stdf_check_write_buffer(file, sts->header.REC_LEN);
	_stdf_write_dtc_header(file, &(sts->header));
	_stdf_write_dtc_U1(file, sts->HEAD_NUM);
	_stdf_write_dtc_U1(file, sts->SITE_NUM);
	_stdf_write_dtc_U4(file, sts->TEST_NUM);
	_stdf_write_dtc_I4(file, sts->EXEC_CNT);
	_stdf_write_dtc_I4(file, sts->FAIL_CNT);
	_stdf_write_dtc_I4(file, sts->ALRM_CNT);
	_stdf_write_dtc_B1(file, sts->OPT_FLAG);
	_stdf_write_dtc_B1(file, sts->PAD_BYTE);
	_stdf_write_dtc_R4(file, sts->TEST_MIN);
	_stdf_write_dtc_R4(file, sts->TEST_MAX);
	_stdf_write_dtc_R4(file, sts->TST_MEAN);
	_stdf_write_dtc_R4(file, sts->TST_SDEV);
	_stdf_write_dtc_R4(file, sts->TST_SUMS);
	_stdf_write_dtc_R4(file, sts->TST_SQRS);
	_stdf_write_dtc_Cn(file, sts->TEST_NAM);
	_stdf_write_dtc_Cn(file, sts->SEQ_NAME);
	_stdf_write_dtc_Cn(file, sts->TEST_LBL);
	return _stdf_write_flush(file, sts->header.REC_LEN);
}

ssize_t stdf_write_rec_scr(stdf_file *file, rec_scr *scr)
{
	if (!scr->header.REC_LEN)
		SET_HEADER(scr->header, REC_SCR, _calc_rec_len_scr(file, scr));
	_stdf_check_write_buffer(file, scr->header.REC_LEN);
	_stdf_write_dtc_header(file, &(scr->header));
	_stdf_write_dtc_U1(file, scr->HEAD_NUM);
	_stdf_write_dtc_U1(file, scr->SITE_NUM);
	_stdf_write_dtc_U4(file, scr->FINISH_T);
	_stdf_write_dtc_U4(file, scr->PART_CNT);
	_stdf_write_dtc_I4(file, scr->RTST_CNT);
	_stdf_write_dtc_I4(file, scr->ABRT_CNT);
	_stdf_write_dtc_I4(file, scr->GOOD_CNT);
	_stdf_write_dtc_I4(file, scr->FUNC_CNT);
	return _stdf_write_flush(file, scr->header.REC_LEN);
}

#endif

ssize_t stdf_write_rec_gdr(stdf_file *file, rec_gdr *gdr)
{
	if (!gdr->header.REC_LEN)
		SET_HEADER(gdr->header, REC_GDR, _calc_rec_len_gdr(file, gdr));
	_stdf_check_write_buffer(file, gdr->header.REC_LEN);
	_stdf_write_dtc_header(file, &(gdr->header));
	_stdf_write_dtc_U2(file, gdr->FLD_CNT);
	_stdf_write_dtc_Vn(file, gdr->GEN_DATA, gdr->FLD_CNT);
	return _stdf_write_flush(file, gdr->header.REC_LEN);
}

ssize_t stdf_write_rec_dtr(stdf_file *file, rec_dtr *dtr)
{
	if (!dtr->header.REC_LEN)
		SET_HEADER(dtr->header, REC_DTR, _calc_rec_len_dtr(file, dtr));
	_stdf_check_write_buffer(file, dtr->header.REC_LEN);
	_stdf_write_dtc_header(file, &(dtr->header));
	_stdf_write_dtc_Cn(file, dtr->TEXT_DAT);
	return _stdf_write_flush(file, dtr->header.REC_LEN);
}
