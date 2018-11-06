module.exports = {
  0: {
    10: {
      REC_TYP: 'FAR',
      data: {
        CPU_TYPE: '1U',
        STDF_VER: '1U'
      }
    }
  },
  1: {
    20: {
      REC_TYP: 'MRR',
      data: {
        FINISH_T: '4U',
        DISP_COD: '1C',
        USR_DESC: '?C',
        EXC_DESC: '?C'
      }
    }
  },
  5: {
    10: {
      REC_TYP: 'PIR',
      data: {
        HEAD_NUM: '1U',
        SITE_NUM: '1U'
      }
    },
    20: {
      REC_TYP: 'PRR',
      data: {
        HEAD_NUM: '1U',
        SITE_NUM: '1U',
        PART_FLG: '1B',
        NUM_TEST: '2U',
        HARD_BIN: '2U',
        SOFT_BIN: '2U',
        X_COORD: '2I',
        Y_COORD: '2I',
        TEST_T: '4U',
        PART_ID: '?C',
        PART_TXT: '?C',
        PART_FIX: '?B'
      }
    }
  },
  15: {
    10: {
      REC_TYP: 'PTR',
      data: {
        TEST_NUM: '4U',
        HEAD_NUM: '1U',
        SITE_NUM: '1U',
        TEST_FLG: '1B',
        PARM_FLG: '1B',
        RESULT: '4R',
        TEST_TXT: '?C',
        ALARM_ID: '?C',
        OPT_FLAG: '1B',
        RES_SCAL: '1I',
        LLM_SCAL: '1I',
        HLM_SCAL: '1I',
        LO_LIMIT: '4R',
        HI_LIMIT: '4R',
        UNITS: '?C',
        C_RESFMT: '?C',
        C_LLMFMT: '?C',
        C_HLMFMT: '?C',
        LO_SPEC: '4R',
        HI_SPEC: '4R'
      }
    }
  }
}
