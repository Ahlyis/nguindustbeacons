#include "stdafx.h"
#include <string.h>
#include <fstream>
#include <iostream>
#include "Accessories.h"

extern int sqx[8];
extern int sqy[8];
extern int knx[8];
extern int kny[8];
extern int arx[71];
extern int ary[71];
extern int wlb[12];
extern int dtx[16];
extern int dty[16];

extern int beaconval[3][5]; /* [affects][shape] */
extern int mapcolor [10];

void InitBeacons ()
{
    sqx[0] = -1;
    sqy[0] = -1;
    sqx[1] = -1;
    sqy[1] = 0;
    sqx[2] = -1;
    sqy[2] = 1;
    sqx[3] = 0;
    sqy[3] = -1;
    sqx[4] = 0;
    sqy[4] = 1;
    sqx[5] = 1;
    sqy[5] = -1;
    sqx[6] = 1;
    sqy[6] = 0;
    sqx[7] = 1;
    sqy[7] = 1;

    knx[0] = -2;
    kny[0] = -1;
    knx[1] = -2;
    kny[1] = 1;
    knx[2] = -1;
    kny[2] = -2;
    knx[3] = -1;
    kny[3] = 2;
    knx[4] = 1;
    kny[4] = -2;
    knx[5] = 1;
    kny[5] = 2;
    knx[6] = 2;
    kny[6] = -1;
    knx[7] = 2;
    kny[7] = 1;

    arx[0] = 0;
    ary[0] = 1;
    arx[1] = 0;
    ary[1] = 2;
    arx[2] = -2;
    ary[2] = 3;
    arx[3] = -1;
    ary[3] = 3;
    arx[4] = 0;
    ary[4] = 3;
    arx[5] = 1;
    ary[5] = 3;
    arx[6] = 2;
    ary[6] = 3;
    arx[7] = -1;
    ary[7] = 4;
    arx[8] = 0;
    ary[8] = 4;
    arx[9] = 1;
    ary[9] = 4;
    arx[10] = 0;
    ary[10] = 5;

    arx[20] = 0;
    ary[20] = -1;
    arx[21] = 0;
    ary[21] = -2;
    arx[22] = -2;
    ary[22] = -3;
    arx[23] = -1;
    ary[23] = -3;
    arx[24] = 0;
    ary[24] = -3;
    arx[25] = 1;
    ary[25] = -3;
    arx[26] = 2;
    ary[26] = -3;
    arx[27] = -1;
    ary[27] = -4;
    arx[28] = 0;
    ary[28] = -4;
    arx[29] = 1;
    ary[29] = -4;
    arx[30] = 0;
    ary[30] = -5;

    ary[40] = 0;
    arx[40] = 1;
    ary[41] = 0;
    arx[41] = 2;
    ary[42] = -2;
    arx[42] = 3;
    ary[43] = -1;
    arx[43] = 3;
    ary[44] = 0;
    arx[44] = 3;
    ary[45] = 1;
    arx[45] = 3;
    ary[46] = 2;
    arx[46] = 3;
    ary[47] = -1;
    arx[47] = 4;
    ary[48] = 0;
    arx[48] = 4;
    ary[49] = 1;
    arx[49] = 4;
    ary[50] = 0;
    arx[50] = 5;

    ary[60] = 0;
    arx[60] = -1;
    ary[61] = 0;
    arx[61] = -2;
    ary[62] = -2;
    arx[62] = -3;
    ary[63] = -1;
    arx[63] = -3;
    ary[64] = 0;
    arx[64] = -3;
    ary[65] = 1;
    arx[65] = -3;
    ary[66] = 2;
    arx[66] = -3;
    ary[67] = -1;
    arx[67] = -4;
    ary[68] = 0;
    arx[68] = -4;
    ary[69] = 1;
    arx[69] = -4;
    ary[70] = 0;
    arx[70] = -5;

    wlb[0] = -6;
    wlb[1] = -5;
    wlb[2] = -4;
    wlb[3] = -3;
    wlb[4] = -2;
    wlb[5] = -1;
    wlb[6] = 6;
    wlb[7] = 5;
    wlb[8] = 4;
    wlb[9] = 3;
    wlb[10] = 2;
    wlb[11] = 1;

    dtx[0] = -2;
    dty[0] = -2;
    dtx[1] = -2;
    dty[1] = -1;
    dtx[2] = -2;
    dty[2] = 0;
    dtx[3] = -2;
    dty[3] = 1;
    dtx[4] = -2;
    dty[4] = 2;
    dtx[5] = -1;
    dty[5] = 2;
    dtx[6] = 0;
    dty[6] = 2;
    dtx[7] = 1;
    dty[7] = 2;
    dtx[8] = 2;
    dty[8] = 2;
    dtx[9] = 2;
    dty[9] = 1;
    dtx[10] = 2;
    dty[10] = 0;
    dtx[11] = 2;
    dty[11] = -1;
    dtx[12] = 2;
    dty[12] = -2;
    dtx[13] = -2;
    dty[13] = 1;
    dtx[14] = -2;
    dty[14] = 0;
    dtx[15] = -2;
    dty[15] = -1;

    beaconval [0][0] = 40;
    beaconval [0][1] = 35;
    beaconval [0][2] = 26;
    beaconval [0][3] = 27;
    beaconval [0][4] = 23;
    beaconval [1][0] = 30;
    beaconval [1][1] = 35;
    beaconval [1][2] = 22;
    beaconval [1][3] = 27;
    beaconval [1][4] = 26;
    beaconval [2][0] = 15;
    beaconval [2][1] = 13;
    beaconval [2][2] = 7;
    beaconval [2][3] = 9;
    beaconval [2][4] = 8;

    mapcolor[1] = 24;
    mapcolor[2] = 197;
    mapcolor[3] = 48;
    mapcolor[4] = 222;
    mapcolor[5] = 15;

    return;
}
