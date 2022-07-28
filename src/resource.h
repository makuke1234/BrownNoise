#ifndef RESOURCE_H
#define RESOURCE_H

#include "winapi.h"

#define BROWN_NOISE_CLASS L"BrownNoiseClass"
#define BROWN_NOISE_TITLE_ASCII "BrownNoise"
#define BROWN_NOISE_TITLE (L"" BROWN_NOISE_TITLE_ASCII)

#define VERSION_SEQUENCE      0,1
#define VERSION_SEQUENCE_STR "0.1"

#define IDB_NORM 101
#define IDB_DIFF 102
#define IDB_INT  103

#define IDM_EXIT 110

#define IDD_DROP1 111




#define SIZE_X 400
#define SIZE_Y 400

#define BN_MARGIN 7


#define BMP_SIZE_X SIZE_X
#define BMP_SIZE_Y 150

#define DROP1S_POS_X  10
#define DROP1S_POS_Y  (BMP_SIZE_Y + BN_MARGIN)
#define DROP1S_SIZE_X 200
#define DROP1S_SIZE_Y 22

#define DROP1_POS_X  10
#define DROP1_POS_Y  (DROP1S_POS_Y + DROP1S_SIZE_Y + BN_MARGIN)
#define DROP1_SIZE_X 200
#define DROP1_SIZE_Y 27


#endif
