#ifndef RESOURCE_H
#define RESOURCE_H

#include "winapi.h"

#define BROWN_NOISE_CLASS L"BrownNoiseClass"
#define BROWN_NOISE_TITLE_ASCII "BrownNoise"
#define BROWN_NOISE_TITLE (L"" BROWN_NOISE_TITLE_ASCII)

#define VERSION_SEQUENCE      0,6,2
#define VERSION_SEQUENCE_STR "0.6.2"

#define GROUP1_COLOR RGB(153, 217, 234)
#define GROUP2_COLOR RGB(232, 218, 178)
#define GROUP3_COLOR RGB(196, 194, 214)
#define GROUP4_COLOR RGB(176, 212, 183)


#define IDB_NORM 101
#define IDB_DIFF 102
#define IDB_INT  103

#define IDD_DROPTYPE     110
#define IDT_NOISE        111
#define IDD_DROPNOISEU   112
#define IDT_BANDWIDTH    113
#define IDD_DROPBWUNIT   114
#define IDT_TEMP         115
#define IDD_DROPTUNIT    116
#define IDT_DESIREDN     117
#define IDD_DROPDESIREDN 118

#define IDT_INP     119
#define IDD_DROPINP 120
#define IDT_SNR     121
#define IDD_DROPSNR 122

#define IDM_RESET 123



#define PANESIZE_X 400
#define SIZE_X     2 * PANESIZE_X

#define BN_MARGIN    7
#define BN_STATIC_Y  22
#define BN_CONTROL_Y 27


#define BMP_SIZE_X PANESIZE_X
#define BMP_SIZE_Y 150

#define DROPTYPE_S_POS_X  BN_MARGIN
#define DROPTYPE_S_POS_Y  (BMP_SIZE_Y + BN_MARGIN)
#define DROPTYPE_S_SIZE_X (PANESIZE_X - 2 * BN_MARGIN)
#define DROPTYPE_S_SIZE_Y BN_STATIC_Y

#define DROPTYPE_POS_X  BN_MARGIN
#define DROPTYPE_POS_Y  (DROPTYPE_S_POS_Y + DROPTYPE_S_SIZE_Y)
#define DROPTYPE_SIZE_X (PANESIZE_X - 2 * BN_MARGIN)
#define DROPTYPE_SIZE_Y BN_CONTROL_Y


#define NOISE_S_POS_X  BN_MARGIN
#define NOISE_S_POS_Y  (DROPTYPE_POS_Y + DROPTYPE_SIZE_Y + BN_MARGIN)

#define DROPNOISEU_SIZE_X 75
#define DROPNOISEU_SIZE_Y BN_CONTROL_Y

#define NOISE_POS_X  BN_MARGIN
#define NOISE_POS_Y  (NOISE_S_POS_Y + BN_STATIC_Y)
#define NOISE_SIZE_X (PANESIZE_X - 2 * BN_MARGIN - DROPNOISEU_SIZE_X - BN_MARGIN)
#define NOISE_SIZE_Y BN_CONTROL_Y

#define DROPNOISEU_POS_X  (PANESIZE_X - DROPNOISEU_SIZE_X - BN_MARGIN)
#define DROPNOISEU_POS_Y  NOISE_POS_Y

#define NOISE_S_SIZE_X NOISE_SIZE_X
#define NOISE_S_SIZE_Y BN_STATIC_Y


#define BW_S_POS_X  BN_MARGIN
#define BW_S_POS_Y  (DROPNOISEU_POS_Y + DROPNOISEU_SIZE_Y + BN_MARGIN)

#define DROPBWUNIT_SIZE_X 75
#define DROPBWUNIT_SIZE_Y BN_CONTROL_Y

#define BANDWIDTH_POS_X  BN_MARGIN
#define BANDWIDTH_POS_Y  (BW_S_POS_Y + BN_STATIC_Y)
#define BANDWIDTH_SIZE_X (PANESIZE_X - 2 * BN_MARGIN - DROPBWUNIT_SIZE_X - BN_MARGIN)
#define BANDWIDTH_SIZE_Y BN_CONTROL_Y

#define DROPBWUNIT_POS_X  (PANESIZE_X - DROPBWUNIT_SIZE_X - BN_MARGIN)
#define DROPBWUNIT_POS_Y  BANDWIDTH_POS_Y

#define BW_S_SIZE_X BANDWIDTH_SIZE_X
#define BW_S_SIZE_Y BN_STATIC_Y


#define TEMP_S_POS_X  BN_MARGIN
#define TEMP_S_POS_Y  (DROPBWUNIT_POS_Y + DROPBWUNIT_SIZE_Y + BN_MARGIN)

#define DROPTUNIT_SIZE_X 75
#define DROPTUNIT_SIZE_Y BN_CONTROL_Y

#define TEMP_POS_X  BN_MARGIN
#define TEMP_POS_Y  (TEMP_S_POS_Y + BN_STATIC_Y)
#define TEMP_SIZE_X (PANESIZE_X - 2 * BN_MARGIN - DROPTUNIT_SIZE_X - BN_MARGIN)
#define TEMP_SIZE_Y BN_CONTROL_Y

#define DROPTUNIT_POS_X  (PANESIZE_X - DROPTUNIT_SIZE_X - BN_MARGIN)
#define DROPTUNIT_POS_Y  TEMP_POS_Y

#define TEMP_S_SIZE_X TEMP_SIZE_X
#define TEMP_S_SIZE_Y BN_STATIC_Y


#define DESIREDN_S_POS_X  BN_MARGIN
#define DESIREDN_S_POS_Y  (DROPTUNIT_POS_Y + DROPTUNIT_SIZE_Y + BN_MARGIN)

#define DROPDESIREDN_SIZE_X 75
#define DROPDESIREDN_SIZE_Y BN_CONTROL_Y

#define DESIREDN_POS_X  BN_MARGIN
#define DESIREDN_POS_Y  (DESIREDN_S_POS_Y + BN_STATIC_Y)
#define DESIREDN_SIZE_X (PANESIZE_X - 2 * BN_MARGIN - DROPDESIREDN_SIZE_X - BN_MARGIN)
#define DESIREDN_SIZE_Y BN_CONTROL_Y

#define DROPDESIREDN_POS_X  (DESIREDN_POS_X + DESIREDN_SIZE_X + BN_MARGIN)
#define DROPDESIREDN_POS_Y  DESIREDN_POS_Y

#define DESIREDN_S_SIZE_X DESIREDN_SIZE_X
#define DESIREDN_S_SIZE_Y BN_STATIC_Y


#define RESULT_S_POS_X  BN_MARGIN
#define RESULT_S_POS_Y  (DROPDESIREDN_POS_Y + DROPDESIREDN_SIZE_Y + BN_MARGIN)
#define RESULT_S_SIZE_X (PANESIZE_X - 2 * BN_MARGIN)
#define RESULT_S_SIZE_Y (2 * BN_STATIC_Y)

#define RESETBTN_POS_X  BN_MARGIN
#define RESETBTN_POS_Y  (RESULT_S_POS_Y + RESULT_S_SIZE_Y + BN_MARGIN)
#define RESETBTN_SIZE_X (PANESIZE_X - 2 * BN_MARGIN)
#define RESETBTN_SIZE_Y BN_CONTROL_Y


#define INP_S_POS_X (PANESIZE_X + BN_MARGIN)
#define INP_S_POS_Y BN_MARGIN

#define DROPINP_SIZE_X 75
#define DROPINP_SIZE_Y BN_CONTROL_Y

#define INP_POS_X  (PANESIZE_X + BN_MARGIN)
#define INP_POS_Y  (INP_S_POS_Y + BN_STATIC_Y)
#define INP_SIZE_X (PANESIZE_X - 2 * BN_MARGIN - DROPINP_SIZE_X - BN_MARGIN)
#define INP_SIZE_Y BN_CONTROL_Y

#define DROPINP_POS_X (INP_POS_X + INP_SIZE_X + BN_MARGIN)
#define DROPINP_POS_Y INP_POS_Y

#define INP_S_SIZE_X INP_SIZE_X
#define INP_S_SIZE_Y BN_STATIC_Y


#define SNR_S_POS_X (PANESIZE_X + BN_MARGIN)
#define SNR_S_POS_Y (DROPINP_POS_Y + DROPINP_SIZE_Y + BN_MARGIN)

#define DROPSNR_SIZE_X 75
#define DROPSNR_SIZE_Y BN_CONTROL_Y

#define SNR_POS_X  (PANESIZE_X + BN_MARGIN)
#define SNR_POS_Y  (SNR_S_POS_Y + BN_STATIC_Y)
#define SNR_SIZE_X (PANESIZE_X - 2 * BN_MARGIN - DROPSNR_SIZE_X - BN_MARGIN)
#define SNR_SIZE_Y BN_CONTROL_Y

#define DROPSNR_POS_X (SNR_POS_X + SNR_SIZE_X + BN_MARGIN)
#define DROPSNR_POS_Y SNR_POS_Y

#define SNR_S_SIZE_X SNR_SIZE_X
#define SNR_S_SIZE_Y BN_STATIC_Y

#define RESULTSNR_S_POS_X  (PANESIZE_X + BN_MARGIN)
#define RESULTSNR_S_POS_Y  (DROPSNR_POS_Y + DROPSNR_SIZE_Y + BN_MARGIN)
#define RESULTSNR_S_SIZE_X (PANESIZE_X - 2 * BN_MARGIN)
#define RESULTSNR_S_SIZE_Y (2 * BN_STATIC_Y)



#define SIZE_Y (RESETBTN_POS_Y + RESETBTN_SIZE_Y + 2 * BN_MARGIN)

#endif
