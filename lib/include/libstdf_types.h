/**
 * @file libstdf_types.h
 * @brief Public types.
 */
/*
 * Copyright (C) 2004-2006 Mike Frysinger <vapier@gmail.com>
 * Released under the BSD license.  For more information,
 * please see: http://opensource.org/licenses/bsd-license.php
 *
 * $Header: /cvsroot/freestdf/libstdf/include/libstdf_types.h,v 1.28 2005/11/13 12:09:51 vapier Exp $
 */

#ifndef _LIBSTDF_TYPES_H
#define _LIBSTDF_TYPES_H

/* Definitions for Record Types [page 7] */
typedef enum {
	REC_TYP_INFO		= 0,
	REC_TYP_PER_LOT		= 1,
	REC_TYP_PER_WAFER	= 2,
	REC_TYP_PER_PART	= 5,
	REC_TYP_PER_TEST	= 10,
	REC_TYP_PER_EXEC	= 15,
	REC_TYP_PER_PROG	= 20,
	REC_TYP_PER_SITE	= 25,
	REC_TYP_GENERIC		= 50,
	REC_TYP_RESV_IMAGE	= 180,
	REC_TYP_RESV_IG900	= 181,
	REC_TYP_UNKNOWN		= 0xFF
} rec_typ;
/* Definitions for Record Subtypes [page 7] */
typedef enum {
	REC_SUB_FAR			= 10,
	REC_SUB_ATR			= 20,
	REC_SUB_MIR			= 10,
	REC_SUB_MRR			= 20,
	REC_SUB_PCR			= 30,
	REC_SUB_HBR			= 40,
	REC_SUB_SBR			= 50,
	REC_SUB_PMR			= 60,
	REC_SUB_PGR			= 62,
	REC_SUB_PLR			= 63,
	REC_SUB_RDR			= 70,
	REC_SUB_SDR			= 80,
	REC_SUB_WIR			= 10,
	REC_SUB_WRR			= 20,
	REC_SUB_WCR			= 30,
	REC_SUB_PIR			= 10,
	REC_SUB_PRR			= 20,
#ifdef STDF_VER3
	REC_SUB_PDR			= 10,
	REC_SUB_FDR			= 20,
#endif
	REC_SUB_TSR			= 30,
	REC_SUB_PTR			= 10,
	REC_SUB_MPR			= 15,
	REC_SUB_FTR			= 20,
	REC_SUB_BPS			= 10,
	REC_SUB_EPS			= 20,
#ifdef STDF_VER3
	REC_SUB_SHB			= 10,
	REC_SUB_SSB			= 20,
	REC_SUB_STS			= 30,
	REC_SUB_SCR			= 40,
#endif
	REC_SUB_GDR			= 10,
	REC_SUB_DTR			= 30,
	REC_SUB_UNKNOWN		= 0xFF
} rec_sub;
/* Definitions that combine Record Types with Subtypes */
#define	MAKE_REC(typ,sub)	((typ << 8) + sub)
#define	BREAK_REC(h,t)		do { h.REC_TYP = (t >> 8); h.REC_SUB = (t & 0xFF); } while (0)
#define	SET_HEADER(h,t,l)	do { BREAK_REC(h, t); h.REC_LEN = l; } while (0)
#define	INIT_HEADER(h,t)	SET_HEADER(h,t,0)
#define	HEAD_TO_REC(h)		MAKE_REC(h.REC_TYP,h.REC_SUB)
#define	REC_FAR				MAKE_REC(REC_TYP_INFO, REC_SUB_FAR)
#define	REC_ATR				MAKE_REC(REC_TYP_INFO, REC_SUB_ATR)
#define	REC_MIR				MAKE_REC(REC_TYP_PER_LOT, REC_SUB_MIR)
#define	REC_MRR				MAKE_REC(REC_TYP_PER_LOT, REC_SUB_MRR)
#define	REC_PCR				MAKE_REC(REC_TYP_PER_LOT, REC_SUB_PCR)
#define	REC_HBR				MAKE_REC(REC_TYP_PER_LOT, REC_SUB_HBR)
#define	REC_SBR				MAKE_REC(REC_TYP_PER_LOT, REC_SUB_SBR)
#define	REC_PMR				MAKE_REC(REC_TYP_PER_LOT, REC_SUB_PMR)
#define	REC_PGR				MAKE_REC(REC_TYP_PER_LOT, REC_SUB_PGR)
#define	REC_PLR				MAKE_REC(REC_TYP_PER_LOT, REC_SUB_PLR)
#define	REC_RDR				MAKE_REC(REC_TYP_PER_LOT, REC_SUB_RDR)
#define	REC_SDR				MAKE_REC(REC_TYP_PER_LOT, REC_SUB_SDR)
#define	REC_WIR				MAKE_REC(REC_TYP_PER_WAFER, REC_SUB_WIR)
#define	REC_WRR				MAKE_REC(REC_TYP_PER_WAFER, REC_SUB_WRR)
#define	REC_WCR				MAKE_REC(REC_TYP_PER_WAFER, REC_SUB_WCR)
#define	REC_PIR				MAKE_REC(REC_TYP_PER_PART, REC_SUB_PIR)
#define	REC_PRR				MAKE_REC(REC_TYP_PER_PART, REC_SUB_PRR)
#ifdef	STDF_VER3
#define	REC_PDR				MAKE_REC(REC_TYP_PER_TEST, REC_SUB_PDR)
#define	REC_FDR				MAKE_REC(REC_TYP_PER_TEST, REC_SUB_FDR)
#endif
#define	REC_TSR				MAKE_REC(REC_TYP_PER_TEST, REC_SUB_TSR)
#define	REC_PTR				MAKE_REC(REC_TYP_PER_EXEC, REC_SUB_PTR)
#define	REC_MPR				MAKE_REC(REC_TYP_PER_EXEC, REC_SUB_MPR)
#define	REC_FTR				MAKE_REC(REC_TYP_PER_EXEC, REC_SUB_FTR)
#define	REC_BPS				MAKE_REC(REC_TYP_PER_PROG, REC_SUB_BPS)
#define	REC_EPS				MAKE_REC(REC_TYP_PER_PROG, REC_SUB_EPS)
#ifdef	STDF_VER3
#define	REC_SHB				MAKE_REC(REC_TYP_PER_SITE, REC_SUB_SHB)
#define	REC_SSB				MAKE_REC(REC_TYP_PER_SITE, REC_SUB_SSB)
#define	REC_STS				MAKE_REC(REC_TYP_PER_SITE, REC_SUB_STS)
#define	REC_SCR				MAKE_REC(REC_TYP_PER_SITE, REC_SUB_SCR)
#endif
#define	REC_GDR				MAKE_REC(REC_TYP_GENERIC, REC_SUB_GDR)
#define	REC_DTR				MAKE_REC(REC_TYP_GENERIC, REC_SUB_DTR)
#define	REC_UNKNOWN			MAKE_REC(REC_TYP_UNKNOWN, REC_SUB_UNKNOWN)

/* Definitions for Data Type Codes and Representation [page 8] */
typedef	uint8_t			byte_t;
typedef	char*			dtc_Cn;
typedef	char			dtc_C1;
typedef	uint8_t			dtc_U1;
typedef	uint16_t		dtc_U2;
typedef	uint32_t		dtc_U4;
typedef	int8_t			dtc_I1;
typedef	int16_t			dtc_I2;
typedef	int32_t			dtc_I4;
typedef	float			dtc_R4;
typedef	double			dtc_R8;
typedef	unsigned char*	dtc_Bn;
typedef	unsigned char	dtc_B1;
typedef	unsigned char*	dtc_Dn;
typedef	unsigned char	dtc_N1;
typedef	dtc_Cn*			dtc_xCn;
typedef	dtc_U1*			dtc_xU1;
typedef	dtc_U2*			dtc_xU2;
typedef	dtc_R4*			dtc_xR4;
typedef	dtc_N1*			dtc_xN1;
typedef struct {
	dtc_Vn_type	type;
	void*		data;
} dtc_Vn_ele;
typedef	dtc_Vn_ele*		dtc_Vn;

typedef enum {
	REC_STATE_RAW,
	REC_STATE_PARSED
} rec_state;
/**
 * @brief The header found with every record in a STDF file.
 *
 * See the Definitions for Record Types [page 15+] of the spec
 * file for more information
 */
typedef struct {
	void		*stdf_file;
	rec_state	state;
	dtc_U2		REC_LEN;		/**< Record length (in bytes) */
	rec_typ		REC_TYP;		/**< The 'type' of record (class) */
	rec_sub		REC_SUB;		/**< The 'sub type' of record */
} rec_header;
/**
 * @brief The basic record type.
 *
 * You figure out what kind of record this really represents
 * by looking at the header.  Then simply cast it as the
 * appropriate record type.
 */
typedef struct {
	rec_header	header;
	void		*data;
} rec_unknown;
/* FAR: File Attributes Record [page 18] */
typedef struct {
	rec_header	header;
	dtc_U1		CPU_TYPE;
	dtc_U1		STDF_VER;
} rec_far;
/* ATR: Audit Trail Record [page 19] */
typedef struct {
	rec_header	header;
	dtc_U4		MOD_TIM;
	dtc_Cn		CMD_LINE;
} rec_atr;
/* MIR: Master Information Record [page 20] */
typedef struct {
	rec_header	header;
#ifdef STDF_VER3
	dtc_U1		CPU_TYPE;
	dtc_U1		STDF_VER;
#endif
	dtc_U4		SETUP_T;
	dtc_U4		START_T;
	dtc_U1		STAT_NUM;
	dtc_C1		MODE_COD;
	dtc_C1		RTST_COD;
	dtc_C1		PROT_COD;
	dtc_U2		BURN_TIM;
	dtc_C1		CMOD_COD;
	dtc_Cn		LOT_ID;
	dtc_Cn		PART_TYP;
	dtc_Cn		NODE_NAM;
	dtc_Cn		TSTR_TYP;
#ifdef STDF_VER3
	dtc_Cn		HAND_ID;
	dtc_Cn		PRB_CARD;
#endif
	dtc_Cn		JOB_NAM;
	dtc_Cn		JOB_REV;
	dtc_Cn		SBLOT_ID;
	dtc_Cn		OPER_NAM;
	dtc_Cn		EXEC_TYP;
	dtc_Cn		EXEC_VER;
	dtc_Cn		TEST_COD;
	dtc_Cn		TST_TEMP;
	dtc_Cn		USER_TXT;
	dtc_Cn		AUX_FILE;
	dtc_Cn		PKG_TYP;
	dtc_Cn		FAMILY_ID;
	dtc_Cn		DATE_COD;
	dtc_Cn		FACIL_ID;
	dtc_Cn		FLOOR_ID;
	dtc_Cn		PROC_ID;
	dtc_Cn		OPER_FRQ;
	dtc_Cn		SPEC_NAM;
	dtc_Cn		SPEC_VER;
	dtc_Cn		FLOW_ID;
	dtc_Cn		SETUP_ID;
	dtc_Cn		DSGN_REV;
	dtc_Cn		ENG_ID;
	dtc_Cn		ROM_COD;
	dtc_Cn		SERL_NUM;
	dtc_Cn		SUPR_NAM;
} rec_mir;
/* MRR: Master Results Record [page 23] */
typedef struct {
	rec_header	header;
	dtc_U4		FINISH_T;
#ifdef STDF_VER3
	dtc_U4		PART_CNT;
	dtc_U4		RTST_CNT;
	dtc_U4		ABRT_CNT;
	dtc_U4		GOOD_CNT;
	dtc_U4		FUNC_CNT;
#endif
	dtc_C1		DISP_COD;
	dtc_Cn		USR_DESC;
	dtc_Cn		EXC_DESC;
} rec_mrr;
/* PCR: Part Count Record [page 24] */
typedef struct {
	rec_header	header;
	dtc_U1		HEAD_NUM;
	dtc_U1		SITE_NUM;
	dtc_U4		PART_CNT;
	dtc_U4		RTST_CNT;
	dtc_U4		ABRT_CNT;
	dtc_U4		GOOD_CNT;
	dtc_U4		FUNC_CNT;
} rec_pcr;
/* HBR: Hardware Bin Record [page 25] */
typedef struct {
	rec_header	header;
	dtc_U1		HEAD_NUM;
	dtc_U1		SITE_NUM;
	dtc_U2		HBIN_NUM;
	dtc_U4		HBIN_CNT;
	dtc_C1		HBIN_PF;
	dtc_Cn		HBIN_NAM;
} rec_hbr;
/* SBR: Software Bin Record [page 27] */
typedef struct {
	rec_header	header;
	dtc_U1		HEAD_NUM;
	dtc_U1		SITE_NUM;
	dtc_U2		SBIN_NUM;
	dtc_U4		SBIN_CNT;
	dtc_C1		SBIN_PF;
	dtc_Cn		SBIN_NAM;
} rec_sbr;
/* PMR: Pin Map Record [page 29] */
typedef struct {
	rec_header	header;
	dtc_U2		PMR_INDX;
	dtc_U2		CHAN_TYP;
	dtc_Cn		CHAN_NAM;
	dtc_Cn		PHY_NAM;
	dtc_Cn		LOG_NAM;
	dtc_U1		HEAD_NUM;
	dtc_U1		SITE_NUM;
} rec_pmr;
/* PGR: Pin Group Record [page 31] */
typedef struct {
	rec_header	header;
	dtc_U2		GRP_INDX;
	dtc_Cn		GRP_NAM;
	dtc_U2		INDX_CNT;
	dtc_xU2		PMR_INDX;
} rec_pgr;
/* PLR: Pin List Record [page 32] */
typedef struct {
	rec_header	header;
	dtc_U2		GRP_CNT;
	dtc_xU2		GRP_INDX;
	dtc_xU2		GRP_MODE;
	dtc_xU1		GRP_RADX;
	dtc_xCn		PGM_CHAR;
	dtc_xCn		RTN_CHAR;
	dtc_xCn		PGM_CHAL;
	dtc_xCn		RTN_CHAL;
} rec_plr;
/* RDR: Retest Data Record [page 34] */
typedef struct {
	rec_header	header;
	dtc_U2		NUM_BINS;
	dtc_xU2		RTST_BIN;
} rec_rdr;
/* SDR: Site Description Record [page 35] */
typedef struct {
	rec_header	header;
	dtc_U1		HEAD_NUM;
	dtc_U1		SITE_GRP;
	dtc_U1		SITE_CNT;
	dtc_xU1		SITE_NUM;
	dtc_Cn		HAND_TYP;
	dtc_Cn		HAND_ID;
	dtc_Cn		CARD_TYP;
	dtc_Cn		CARD_ID;
	dtc_Cn		LOAD_TYP;
	dtc_Cn		LOAD_ID;
	dtc_Cn		DIB_TYP;
	dtc_Cn		DIB_ID;
	dtc_Cn		CABL_TYP;
	dtc_Cn		CABL_ID;
	dtc_Cn		CONT_TYP;
	dtc_Cn		CONT_ID;
	dtc_Cn		LASR_TYP;
	dtc_Cn		LASR_ID;
	dtc_Cn		EXTR_TYP;
	dtc_Cn		EXTR_ID;
} rec_sdr;
/* WIR: Wafer Information Record [page 37] */
typedef struct {
	rec_header	header;
	dtc_U1		HEAD_NUM;
	dtc_U1		SITE_GRP;
#ifdef STDF_VER3
	dtc_B1		PAD_BYTE;
#endif
	dtc_U4		START_T;
	dtc_Cn		WAFER_ID;
} rec_wir;
/* WRR: Wafer Results Record [page 38] */
typedef struct {
	rec_header	header;
	dtc_U1		HEAD_NUM;
	dtc_U1		SITE_GRP;
#ifdef STDF_VER3
	dtc_B1		PAD_BYTE;
#endif
	dtc_U4		FINISH_T;
	dtc_U4		PART_CNT;
	dtc_U4		RTST_CNT;
	dtc_U4		ABRT_CNT;
	dtc_U4		GOOD_CNT;
	dtc_U4		FUNC_CNT;
#ifdef STDF_VER3
	dtc_Cn		HAND_ID;
	dtc_Cn		PRB_CARD;
#endif
	dtc_Cn		WAFER_ID;
	dtc_Cn		FABWF_ID;
	dtc_Cn		FRAME_ID;
	dtc_Cn		MASK_ID;
	dtc_Cn		USR_DESC;
	dtc_Cn		EXC_DESC;
} rec_wrr;
/* WCR: Wafer Configuration Record [page 40] */
typedef struct {
	rec_header	header;
	dtc_R4		WAFR_SIZ;
	dtc_R4		DIE_HT;
	dtc_R4		DIE_WID;
	dtc_U1		WF_UNITS;
	dtc_C1		WF_FLAT;
	dtc_I2		CENTER_X;
	dtc_I2		CENTER_Y;
	dtc_C1		POS_X;
	dtc_C1		POS_Y;
} rec_wcr;
/* PIR: Part Information Record [page 42] */
typedef struct {
	rec_header	header;
	dtc_U1		HEAD_NUM;
	dtc_U1		SITE_NUM;
#ifdef STDF_VER3
	dtc_I2		X_COORD;
	dtc_I2		Y_COORD;
	dtc_Cn		PART_ID;
#endif
} rec_pir;
/* PRR: Part Results Record [page 43] */
typedef struct {
	rec_header	header;
	dtc_U1		HEAD_NUM;
	dtc_U1		SITE_NUM;
#ifdef STDF_VER3
	dtc_B1		PAD_BYTE;
#endif
	dtc_B1		PART_FLG;
	dtc_U2		NUM_TEST;
	dtc_U2		HARD_BIN;
	dtc_U2		SOFT_BIN;
	dtc_I2		X_COORD;
	dtc_I2		Y_COORD;
	dtc_U4		TEST_T;
	dtc_Cn		PART_ID;
	dtc_Cn		PART_TXT;
	dtc_Bn		PART_FIX;
} rec_prr;
#ifdef	STDF_VER3
/* PDR: Parametric Test Description */
typedef struct {
	rec_header	header;
	dtc_U4		TEST_NUM;
	dtc_B1		DESC_FLG;
	dtc_B1		OPT_FLAG;
	dtc_I1		RES_SCAL;
	dtc_Cn		UNITS;	/*dtc_C7*/
	dtc_U1		RES_LDIG;
	dtc_U1		RES_RDIG;
	dtc_I1		LLM_SCAL;
	dtc_I1		HLM_SCAL;
	dtc_U1		LLM_LDIG;
	dtc_U1		LLM_RDIG;
	dtc_U1		HLM_LDIG;
	dtc_U1		HLM_RDIG;
	dtc_R4		LO_LIMIT;
	dtc_R4		HI_LIMIT;
	dtc_Cn		TEST_NAM;
	dtc_Cn		SEQ_NAME;
} rec_pdr;
/* FDR: Functional Test Description */
typedef struct {
	rec_header	header;
	dtc_U4		TEST_NUM;
	dtc_B1		DESC_FLG;
	dtc_Cn		TEST_NAM;
	dtc_Cn		SEQ_NAME;
} rec_fdr;
#endif
/* TSR: Test Synopsis Record [page 45] */
typedef struct {
	rec_header	header;
	dtc_U1		HEAD_NUM;
	dtc_U1		SITE_NUM;
#ifdef STDF_VER3
	dtc_B1		PAD_BYTE;
	dtc_R4		TST_MEAN;
	dtc_R4		TST_SDEV;
#endif
	dtc_C1		TEST_TYP;
	dtc_U4		TEST_NUM;
	dtc_U4		EXEC_CNT;
	dtc_U4		FAIL_CNT;
	dtc_U4		ALRM_CNT;
	dtc_Cn		TEST_NAM;
	dtc_Cn		SEQ_NAME;
	dtc_Cn		TEST_LBL;
	dtc_B1		OPT_FLAG;
	dtc_R4		TEST_TIM;
	dtc_R4		TEST_MIN;
	dtc_R4		TEST_MAX;
	dtc_R4		TST_SUMS;
	dtc_R4		TST_SQRS;
} rec_tsr;
/* PTR: Parametric Test Record [page 47] */
typedef struct {
	rec_header	header;
	dtc_U4		TEST_NUM;
	dtc_U1		HEAD_NUM;
	dtc_U1		SITE_NUM;
	dtc_B1		TEST_FLG;
	dtc_B1		PARM_FLG;
	dtc_R4		RESULT;
	dtc_Cn		TEST_TXT;
	dtc_Cn		ALARM_ID;
	dtc_B1		OPT_FLAG;
	dtc_I1		RES_SCAL;
	dtc_I1		LLM_SCAL;
	dtc_I1		HLM_SCAL;
	dtc_R4		LO_LIMIT;
	dtc_R4		HI_LIMIT;
	dtc_Cn		UNITS;
	dtc_Cn		C_RESFMT;
	dtc_Cn		C_LLMFMT;
	dtc_Cn		C_HLMFMT;
	dtc_R4		LO_SPEC;
	dtc_R4		HI_SPEC;
} rec_ptr;
/* MPR: Multiple-Result Parametric Record [page 53] */
typedef struct {
	rec_header	header;
	dtc_U4		TEST_NUM;
	dtc_U1		HEAD_NUM;
	dtc_U1		SITE_NUM;
	dtc_B1		TEST_FLG;
	dtc_B1		PARM_FLG;
	dtc_U2		RTN_ICNT;
	dtc_U2		RSLT_CNT;
	dtc_xN1		RTN_STAT;
	dtc_xR4		RTN_RSLT;
	dtc_Cn		TEST_TXT;
	dtc_Cn		ALARM_ID;
	dtc_B1		OPT_FLAG;
	dtc_I1		RES_SCAL;
	dtc_I1		LLM_SCAL;
	dtc_I1		HLM_SCAL;
	dtc_R4		LO_LIMIT;
	dtc_R4		HI_LIMIT;
	dtc_R4		START_IN;
	dtc_R4		INCR_IN;
	dtc_xU2		RTN_INDX;
	dtc_Cn		UNITS;
	dtc_Cn		UNITS_IN;
	dtc_Cn		C_RESFMT;
	dtc_Cn		C_LLMFMT;
	dtc_Cn		C_HLMFMT;
	dtc_R4		LO_SPEC;
	dtc_R4		HI_SPEC;
} rec_mpr;
/* FTR: Functional Test Record [page 57] */
typedef struct {
	rec_header header;
	dtc_U4		TEST_NUM;
	dtc_U1		HEAD_NUM;
	dtc_U1		SITE_NUM;
	dtc_B1		TEST_FLG;
	dtc_B1		OPT_FLAG;
	dtc_U4		CYCL_CNT;
	dtc_U4		REL_VADR;
	dtc_U4		REPT_CNT;
	dtc_U4		NUM_FAIL;
	dtc_I4		XFAIL_AD;
	dtc_I4		YFAIL_AD;
	dtc_I2		VECT_OFF;
	dtc_U2		RTN_ICNT;
	dtc_U2		PGM_ICNT;
	dtc_xU2		RTN_INDX;
	dtc_xN1		RTN_STAT;
	dtc_xU2		PGM_INDX;
	dtc_xN1		PGM_STAT;
	dtc_Dn		FAIL_PIN;
	dtc_Cn		VECT_NAM;
	dtc_Cn		TIME_SET;
	dtc_Cn		OP_CODE;
	dtc_Cn		TEST_TXT;
	dtc_Cn		ALARM_ID;
	dtc_Cn		PROG_TXT;
	dtc_Cn		RSLT_TXT;
	dtc_U1		PATG_NUM;
	dtc_Dn		SPIN_MAP;
} rec_ftr;
/* BPS: Begin Program Section [page 62] */
typedef struct {
	rec_header	header;
	dtc_Cn		SEQ_NAME;
} rec_bps;
/* EPS: End Program Section [page 63] */
typedef struct {
	rec_header	header;
} rec_eps;
#ifdef	STDF_VER3
/* SHB: Site-Specific Hardware Bin Record */
typedef struct {
	rec_header	header;
	dtc_U1		HEAD_NUM;
	dtc_U1		SITE_NUM;
	dtc_U2		HBIN_NUM;
	dtc_U4		HBIN_CNT;
	dtc_Cn		HBIN_NAM;
} rec_shb;
/* SSB: Site-Specific Software Bin Record */
typedef struct {
	rec_header	header;
	dtc_U1		HEAD_NUM;
	dtc_U1		SITE_NUM;
	dtc_U2		SBIN_NUM;
	dtc_U4		SBIN_CNT;
	dtc_Cn		SBIN_NAM;
} rec_ssb;
/* STS: Site-Specific Test Synopsis Record */
typedef struct {
	rec_header	header;
	dtc_U1		HEAD_NUM;
	dtc_U1		SITE_NUM;
	dtc_U4		TEST_NUM;
	dtc_I4		EXEC_CNT;
	dtc_I4		FAIL_CNT;
	dtc_I4		ALRM_CNT;
	dtc_B1		OPT_FLAG;
	dtc_B1		PAD_BYTE;
	dtc_R4		TEST_MIN;
	dtc_R4		TEST_MAX;
	dtc_R4		TST_MEAN;
	dtc_R4		TST_SDEV;
	dtc_R4		TST_SUMS;
	dtc_R4		TST_SQRS;
	dtc_Cn		TEST_NAM;
	dtc_Cn		SEQ_NAME;
	dtc_Cn		TEST_LBL;
} rec_sts;
/* SCR: Site-Specific Part Count Record */
typedef struct {
	rec_header	header;
	dtc_U1		HEAD_NUM;
	dtc_U1		SITE_NUM;
	dtc_U4		FINISH_T;
	dtc_U4		PART_CNT;
	dtc_I4		RTST_CNT;
	dtc_I4		ABRT_CNT;
	dtc_I4		GOOD_CNT;
	dtc_I4		FUNC_CNT;
} rec_scr;
#endif
/* GDR: Generic Data Record [page 64] */
typedef struct {
	rec_header	header;
	dtc_U2		FLD_CNT;
	dtc_Vn		GEN_DATA;
} rec_gdr;
/* DTR: Datalog Text Record [page 66] */
typedef struct {
	rec_header	header;
	dtc_Cn		TEXT_DAT;
} rec_dtr;

#endif /* _LIBSTDF_TYPES_H */
