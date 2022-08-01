#ifndef RESOURCE_H
#define RESOURCE_H

#include "winapi.h"

#define BROWN_NOISE_CLASS L"BrownNoiseClass"
#define BROWN_NOISE_TITLE_ASCII "BrownNoise"
#define BROWN_NOISE_TITLE (L"" BROWN_NOISE_TITLE_ASCII)

#define VERSION_SEQUENCE      0,3
#define VERSION_SEQUENCE_STR "0.3"

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

#define IDM_RESET 120



#define SIZE_X 400

#define BN_MARGIN 7


#define BMP_SIZE_X SIZE_X
#define BMP_SIZE_Y 150

#define DROPTYPE_S_POS_X  BN_MARGIN
#define DROPTYPE_S_POS_Y  (BMP_SIZE_Y + BN_MARGIN)
#define DROPTYPE_S_SIZE_X (SIZE_X - BN_MARGIN)
#define DROPTYPE_S_SIZE_Y 22

#define DROPTYPE_POS_X  BN_MARGIN
#define DROPTYPE_POS_Y  (DROPTYPE_S_POS_Y + DROPTYPE_S_SIZE_Y)
#define DROPTYPE_SIZE_X 200
#define DROPTYPE_SIZE_Y 27

#define NOISE_S_POS_X  BN_MARGIN
#define NOISE_S_POS_Y  (DROPTYPE_POS_Y + DROPTYPE_SIZE_Y + BN_MARGIN)
#define NOISE_S_SIZE_X (SIZE_X - BN_MARGIN)
#define NOISE_S_SIZE_Y 20

#define NOISE_POS_X  BN_MARGIN
#define NOISE_POS_Y  (NOISE_S_POS_Y + NOISE_S_SIZE_Y)
#define NOISE_SIZE_X 200
#define NOISE_SIZE_Y 27

#define DROPNOISEU_POS_X  (NOISE_POS_X + NOISE_SIZE_X + BN_MARGIN)
#define DROPNOISEU_POS_Y  NOISE_POS_Y
#define DROPNOISEU_SIZE_X 75
#define DROPNOISEU_SIZE_Y 27

#define BW_S_POS_X  BN_MARGIN
#define BW_S_POS_Y  (DROPNOISEU_POS_Y + DROPNOISEU_SIZE_Y + BN_MARGIN)
#define BW_S_SIZE_X (SIZE_X - BN_MARGIN)
#define BW_S_SIZE_Y 20

#define BANDWIDTH_POS_X  BN_MARGIN
#define BANDWIDTH_POS_Y  (BW_S_POS_Y + BW_S_SIZE_Y)
#define BANDWIDTH_SIZE_X 200
#define BANDWIDTH_SIZE_Y 27

#define DROPBWUNIT_POS_X  (BANDWIDTH_POS_X + BANDWIDTH_SIZE_X + BN_MARGIN)
#define DROPBWUNIT_POS_Y  BANDWIDTH_POS_Y
#define DROPBWUNIT_SIZE_X 75
#define DROPBWUNIT_SIZE_Y 27

#define TEMP_S_POS_X  BN_MARGIN
#define TEMP_S_POS_Y  (DROPBWUNIT_POS_Y + DROPBWUNIT_SIZE_Y + BN_MARGIN)
#define TEMP_S_SIZE_X (SIZE_X - BN_MARGIN)
#define TEMP_S_SIZE_Y 20

#define TEMP_POS_X  BN_MARGIN
#define TEMP_POS_Y  (TEMP_S_POS_Y + TEMP_S_SIZE_Y)
#define TEMP_SIZE_X 200
#define TEMP_SIZE_Y 27

#define DROPTUNIT_POS_X  (TEMP_POS_X + TEMP_SIZE_X + BN_MARGIN)
#define DROPTUNIT_POS_Y  TEMP_POS_Y
#define DROPTUNIT_SIZE_X 75
#define DROPTUNIT_SIZE_Y 27


#define DESIREDN_S_POS_X  BN_MARGIN
#define DESIREDN_S_POS_Y  (DROPTUNIT_POS_Y + DROPTUNIT_SIZE_Y + BN_MARGIN)
#define DESIREDN_S_SIZE_X (SIZE_X - BN_MARGIN)
#define DESIREDN_S_SIZE_Y 20

#define DESIREDN_POS_X  BN_MARGIN
#define DESIREDN_POS_Y  (DESIREDN_S_POS_Y + DESIREDN_S_SIZE_Y)
#define DESIREDN_SIZE_X 200
#define DESIREDN_SIZE_Y 27

#define DROPDESIREDN_POS_X  (DESIREDN_POS_X + DESIREDN_SIZE_X + BN_MARGIN)
#define DROPDESIREDN_POS_Y  DESIREDN_POS_Y
#define DROPDESIREDN_SIZE_X 75
#define DROPDESIREDN_SIZE_Y 27

#define RESULT_S_POS_X  BN_MARGIN
#define RESULT_S_POS_Y  (DROPDESIREDN_POS_Y + DROPDESIREDN_SIZE_Y + BN_MARGIN)
#define RESULT_S_SIZE_X (SIZE_X - BN_MARGIN)
#define RESULT_S_SIZE_Y 20

#define RESETBTN_POS_X  BN_MARGIN
#define RESETBTN_POS_Y  (RESULT_S_POS_Y + RESULT_S_SIZE_Y + BN_MARGIN)
#define RESETBTN_SIZE_X (SIZE_X - 2 * BN_MARGIN)
#define RESETBTN_SIZE_Y 27

#define SIZE_Y (RESETBTN_POS_Y + RESETBTN_SIZE_Y + 2 * BN_MARGIN)

#endif
