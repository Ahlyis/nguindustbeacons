// Alchemy.cpp : Defines the entry point for the console application.
// 

#include "stdafx.h"
#include <string.h>
#include <math.h>
#include <fstream>
#include <iostream>
#include "Accessories.h"

using namespace std;

int basemap[17][20];
int workmap[17][20];
int sqx[8];
int sqy[8];
int knx[8];
int kny[8];
int arx[71];
int ary[71];
int beaconval[3][5];
int forcetype;
int forcecount;
bool PrintLegend = true;
int wlb[12];
int dtx[16];
int dty[16];
int mapbasevalue;
int mapfinalvalue;

int main(int argc, char* argv[])
{
    int x, y, z;
    int maps = 1,
        options = 0,
        beacons = 1,
        beacontype = 0;

    x = 0;
    y = 0;
    z = 0;
    forcetype = -1;
    forcecount = 0;
    mapbasevalue = 0;
    mapfinalvalue = 0;

    if (!LoadOperatingMode (&maps, &options, &beacons, &beacontype) )
    {
        printf ("Which map? 1=Tutorial, 2=Flesh, 3=Tronne, 4=Candyland\n");
        cin >> maps;

        if ( (maps < 2) || (maps > MAX_MAPS) )
        {
            maps = 1;
        }

        printf ("Include currently blockaded? 1=yes\n");
        cin >> options;

        if (options != 1)
        {
            options = 0;
        }

        printf ("Up to which beacons? 1=box only, 2=knight, 3=arrow, 4=wall, 5=donut\n");
        cin >> beacons;

        if ( (beacons < 2) || (beacons > 5) )
        {
            beacons = 1;
        }

        printf ("Which beacon type? 0=speed, 1=production, 2=efficiency\n");
        cin >> beacontype;

        if ( (beacontype < 1) || (beacontype > 2) )
        {
            beacontype = 0;
        }

        printf ("Force X amount of which beacon type? 0=square, 1=knight, 2=arrow, 3=wall, 4=donut\n");
        cin >> forcetype;

        if ( (forcetype < 1) || (forcetype > 4) )
        {
            forcetype = 0;
        }

        printf ("Force how many of that type? (0 - 999)\n");
        cin >> forcecount;

        if ( (forcecount < 1) || (forcecount > 999) )
        {
            forcecount = 0;
        }
    }
    SetWhichToCheck (maps);
    InitBeacons ();

    CalculateBestBeacons (options, beacons, beacontype);

    DisplayResults (maps, options, beacons, beacontype);

    return 0;
}

bool LoadOperatingMode (int *maps, int *options, int *beacons, int *beacontype)
{
    char sData[360] = {0},
         sFile[80];
    bool bRetVal = false;

    sprintf (sFile, ".\\runmode.txt");

    ifstream Data(sFile);

    Data >> sData;
    *maps = MakeNumber (sData);

    if ( (*maps > 0) && (*maps <= MAX_MAPS) )
    {
        Data >> sData;

        *options = MakeNumber (sData);

        if (*options != 1)
            *options = 0;

        Data >> sData;

        *beacons = MakeNumber (sData);

        if ( (*beacons < 2) || (*beacons > 5) )
            *beacons = 1;

        Data >> sData;

        *beacontype = MakeNumber (sData);

        if ( (*beacontype != 1) && (*beacontype != 2) )
            *beacontype = 0;

        Data >> sData;

        forcetype = MakeNumber (sData);

        if ( (forcetype < 1) || (forcetype > 4) )
            forcetype = 0;

        Data >> sData;

        forcecount = MakeNumber (sData);

        if ( (forcecount < 1) || (forcecount > 999) )
            forcecount = 0;

        Data >> sData;

        if (MakeNumber (sData) == 1)
            PrintLegend = false;
        else
            PrintLegend = true;

       bRetVal = true;
    }

    return bRetVal;
}

int MakeNumber (char *sData)
{
    int x = 0, iRetVal =0;

    while (    (sData[x] >= '0')
            && (sData[x] <= '9')
          )
    {
        iRetVal += sData[x] - 48;
        iRetVal *= 10;
        x++;

        if (iRetVal > 100000)
        {
            sData[0] = 0;
            x = 0;
            iRetVal = 0;
        }
    }
    iRetVal /= 10;

    return iRetVal;
}

bool SetWhichToCheck (int maps)
{
    char sData[360] = {0},
         sFile[80];
    bool bRetVal = false;
    int iVal = 0, x = 0, y = 0;

    sprintf (sFile, ".\\tutorial.txt");

    if (maps == 2)
    {
        sprintf (sFile, ".\\flesh.txt");
    }

    if (maps == 3)
    {
        sprintf (sFile, ".\\tronne.txt");
    }

    if (maps == 4)
    {
        sprintf (sFile, ".\\candyland.txt");
    }

    ifstream Data(sFile);

    Data >> sData;

    for (x = 0; x < 17; x++)
    {
        for (y = 0; y < 20; y++)
        {
            if (sData[y] == '1') // 1 = open to place
            {
                basemap[x][y] = 1;
            }
            else if(sData[y] == '2') // 2 = usable, but currently blocked
            {
                basemap[x][y] = 2;
            }
            else
            {
                basemap[x][y] = 0; // 0 = unusable square
            }
        }
        Data >> sData;
    }

    bRetVal = true;

    return bRetVal;
}

int  CalculateBestBeacons (int options, int beacons, int beacontype)
{
    int x, y, workx, worky, workval, workbeacon, currval;
    int sqhitby, sqavail, knhitby, knavail, arlhitby, arlavail;
    int arrhitby, arravail, aruhitby, aruavail, ardhitby, ardavail;
    int hwlhitby, hwlavail, vwlhitby, vwlavail, dthitby, dtavail;

    int tempval;
    bool done = false;

    for (x = 0; x < 17; x++)
    {
        for (y = 0; y < 20; y++)
        {
            switch (basemap[x][y])
            {
                case 1:
                {
                    workmap[x][y] = 0;
                    break;
                }
                case 2:
                {
                    if (options == 1)
                    {
                        workmap[x][y] = 0;
                    }
                    else
                    {
                        workmap[x][y] = -1;
                    }
                    break;
                }
                default:
                {
                    workmap[x][y] = -1;
                    break;
                }
            }
        }
    } // initialize working map,

    while (!done)
    {
        workval = workx = worky = workbeacon = 0;
        tempval = 0;
        done = true;

        for (x = 0; x < MAX_ROW; x++)
        {
            for (y = 0; y < MAX_COL; y++)
            {
                currval = 0;

                if (workmap[x][y] == 0)
                {
                    sqhitby = sqavail = knhitby = knavail = arlhitby = arlavail = 0;
                    arrhitby = arravail = aruhitby = aruavail = ardhitby = ardavail = 0;
                    hwlhitby = hwlavail = vwlhitby = vwlavail = dthitby = dtavail = 0;

                    CalcSqValues (&sqhitby, &sqavail, x, y);

                    if (beacons > 1)
                    {
                        CalcKnValues (&knhitby, &knavail, x, y);
                    }

                    if (beacons > 2)
                    {
                        CalcArValues (&arrhitby, &arravail, &arlhitby, &arlavail,
                                      &ardhitby, &ardavail, &aruhitby, &aruavail, x, y);
                    }

                    if (beacons > 3)
                    {
                        CalcWlValues (&hwlhitby, &hwlavail, &vwlhitby, &vwlavail, x, y);
                    }

                    if (beacons > 4)
                    {
                        CalcDtValues (&dthitby, &dtavail, x, y);
                    }

                    currval -= sqhitby * beaconval[beacontype][0 /*squares*/];
                    currval -= knhitby * beaconval[beacontype][1 /*knights*/];
                    currval -= arrhitby * beaconval[beacontype][2 /*arrows*/];
                    currval -= arlhitby * beaconval[beacontype][2 /*arrows*/];
                    currval -= ardhitby * beaconval[beacontype][2 /*arrows*/];
                    currval -= aruhitby * beaconval[beacontype][2 /*arrows*/];
                    currval -= hwlhitby * beaconval[beacontype][0 /*walls*/];
                    currval -= vwlhitby * beaconval[beacontype][0 /*walls*/];
                    currval -= dthitby * beaconval[beacontype][4 /*donuts*/];

                    tempval = currval + sqavail * beaconval[beacontype][0 /*squares*/];

                    if (    (forcecount > 0)
                         && (forcetype != 0)
                       )
                    {
                        tempval = 0;
                    }

                    if (    (    (tempval > 100)
                              || (    (tempval > 40)
                                   && (beacontype == 2)
                                 )
                            )
                         && (tempval > workval)
                       )
                    {
                        workx = x;
                        worky = y;
                        workval = tempval;
                        workbeacon = 1; /*squares*/
                    }

                    tempval = currval + knavail * beaconval[beacontype][1 /*knights*/];

                    if (    (forcecount > 0)
                         && (forcetype != 1)
                       )
                    {
                        tempval = 0;
                    }

                    if (    (    (tempval > 100)
                              || (    (tempval > 40)
                                   && (beacontype == 2)
                                 )
                            )
                         && (tempval > workval)
                       )
                    {
                        workx = x;
                        worky = y;
                        workval = tempval;
                        workbeacon = 2; /*knights*/
                    }

                    tempval = currval + arravail * beaconval[beacontype][2 /*arrows*/];

                    if (    (forcecount > 0)
                         && (forcetype != 2)
                       )
                    {
                        tempval = 0;
                    }

                    if (    (    (tempval > 100)
                              || (    (tempval > 40)
                                   && (beacontype == 2)
                                 )
                            )
                         && (tempval > workval)
                       )
                    {
                        workx = x;
                        worky = y;
                        workval = tempval;
                        workbeacon = 3; /*arrow right*/
                    }

                    tempval = currval + arlavail * beaconval[beacontype][2 /*arrows*/];

                    if (    (forcecount > 0)
                         && (forcetype != 2)
                       )
                    {
                        tempval = 0;
                    }

                    if (    (    (tempval > 100)
                              || (    (tempval > 40)
                                   && (beacontype == 2)
                                 )
                            )
                         && (tempval > workval)
                       )
                    {
                        workx = x;
                        worky = y;
                        workval = tempval;
                        workbeacon = 4; /*arrow left*/
                    }
                    tempval = currval + ardavail * beaconval[beacontype][2 /*arrows*/];

                    if (    (forcecount > 0)
                         && (forcetype != 2)
                       )
                    {
                        tempval = 0;
                    }

                    if (    (    (tempval > 100)
                              || (    (tempval > 40)
                                   && (beacontype == 2)
                                 )
                            )
                         && (tempval > workval)
                       )
                    {
                        workx = x;
                        worky = y;
                        workval = tempval;
                        workbeacon = 5; /*arrow down*/
                    }
                    tempval = currval + aruavail * beaconval[beacontype][2 /*arrows*/];

                    if (    (forcecount > 0)
                         && (forcetype != 2)
                       )
                    {
                        tempval = 0;
                    }

                    if (    (    (tempval > 100)
                              || (    (tempval > 40)
                                   && (beacontype == 2)
                                 )
                            )
                         && (tempval > workval)
                       )
                    {
                        workx = x;
                        worky = y;
                        workval = tempval;
                        workbeacon = 6; /*arrow up*/
                    }
                    tempval = currval + hwlavail * beaconval[beacontype][3 /*walls*/];

                    if (    (forcecount > 0)
                         && (forcetype != 3)
                       )
                    {
                        tempval = 0;
                    }

                    if (    (    (tempval > 100)
                              || (    (tempval > 40)
                                   && (beacontype == 2)
                                 )
                            )
                         && (tempval > workval)
                       )
                    {
                        workx = x;
                        worky = y;
                        workval = tempval;
                        workbeacon = 7; /*horizontal wall*/
                    }
                    tempval = currval + vwlavail * beaconval[beacontype][3 /*walls*/];

                    if (    (forcecount > 0)
                         && (forcetype != 3)
                       )
                    {
                        tempval = 0;
                    }

                    if (    (    (tempval > 100)
                              || (    (tempval > 40)
                                   && (beacontype == 2)
                                 )
                            )
                         && (tempval > workval)
                       )
                    {
                        workx = x;
                        worky = y;
                        workval = tempval;
                        workbeacon = 8; /*vertical wall*/
                    }
                    tempval = currval + dtavail * beaconval[beacontype][4 /*donuts*/];

                    if (    (forcecount > 0)
                         && (forcetype != 4)
                       )
                    {
                        tempval = 0;
                    }

                    if (    (    (tempval > 100)
                              || (    (tempval > 40)
                                   && (beacontype == 2)
                                 )
                            )
                         && (tempval > workval)
                       )
                    {
                        workx = x;
                        worky = y;
                        workval = tempval;
                        workbeacon = 9; /*donut*/
                    }
                }
            }
        }

        if (workval > 0)
        {
            done = false;

            if (    (workmap[workx][worky] != 0)
                 || (workbeacon == 0)
               )
            {
                printf ("ERROR! Bad logic!!!\n");
            }
            workmap[workx][worky] = workbeacon;
            forcecount--;
        }
    }
    mapbasevalue = 0;
    mapfinalvalue = 0;

    for (x = 0; x < MAX_ROW; x++)
    {
        for (y = 0; y < MAX_COL; y++)
        {
            currval = 100; /* current square is worth 100 base before adding beacon modifiers */

            if (workmap[x][y] >= 0)
            {
                mapbasevalue += currval;
            }

            if (workmap[x][y] == 0) /* 0=empty square, usable and without a beacon on it. */
            {
                sqhitby = sqavail = knhitby = knavail = arlhitby = arlavail = 0;
                arrhitby = arravail = aruhitby = aruavail = ardhitby = ardavail = 0;
                hwlhitby = hwlavail = vwlhitby = vwlavail = dthitby = dtavail = 0;

                CalcSqValues (&sqhitby, &sqavail, x, y);
                CalcKnValues (&knhitby, &knavail, x, y);
                CalcArValues (&arrhitby, &arravail, &arlhitby, &arlavail,
                              &ardhitby, &ardavail, &aruhitby, &aruavail, x, y);
                CalcWlValues (&hwlhitby, &hwlavail, &vwlhitby, &vwlavail, x, y);
                CalcDtValues (&dthitby, &dtavail, x, y);

                currval += sqhitby * beaconval[beacontype][0 /*squares*/];
                currval += knhitby * beaconval[beacontype][1 /*knights*/];
                currval += arrhitby * beaconval[beacontype][2 /*arrows*/];
                currval += arlhitby * beaconval[beacontype][2 /*arrows*/];
                currval += ardhitby * beaconval[beacontype][2 /*arrows*/];
                currval += aruhitby * beaconval[beacontype][2 /*arrows*/];
                currval += hwlhitby * beaconval[beacontype][0 /*walls*/];
                currval += vwlhitby * beaconval[beacontype][0 /*walls*/];
                currval += dthitby * beaconval[beacontype][4 /*donuts*/];

                mapfinalvalue += currval;
            }
        }
    }
    return 0;
}

int  DisplayResults (int maps, int options, int beacons, int beacontype)
{
    int x, y;

    cout << "Calculated beacon placement for map: ";

    switch (maps)
    {
        case 1:
        {
            cout << "Tutorial\n";
            break;
        }
        case 2:
        {
            cout << "Flesh\n";
            break;
        }
        case 3:
        {
            cout << "Tronne\n";
            break;
        }
        case 4:
        {
            cout << "Candyland\n";
            break;
        }
        default:
        {
            cout << "Unknown - " << maps << "\n";
            break;
        }
    }

    if (options == 1)
    {
        cout << "With every square unlocked.\n";
    }
    else
    {
        cout << "Ignoring locked squares.\n";
    }

    cout << "Using box";

    if (beacons == 1)
    {
        cout << " beacons only.\n";
    }
    else if (beacons == 2)
    {
        cout << " and knight beacons only.\n";
    }
    else if (beacons == 3)
    {
        cout << ", knight and arrow beacons.\n";
    }
    else if (beacons == 4)
    {
        cout << " through wall beacons.\n";
    }
    else if (beacons == 5)
    {
        cout << " through donut beacons.\n";
    }
    else
    {
        cout << "... ERROR. Using beacon " << beacons << " is not supported!\n";
    }

    switch (beacontype)
    {
        case 0:
        {
            cout << "Using speed (blue) beacons.\n\n";
            break;
        }
        case 1:
        {
            cout << "Using production (pink) beacons.\n\n";
            break;
        }
        case 2:
        {
            cout << "Using efficiency (yellow) beacons.\n\n";
            break;
        }
        default:
        {
            cout << "Using ERROR - TYPE " << beacontype << " beacons.\n\n";
            break;
        }
    }

    if (PrintLegend)
    {
        cout << "Legend:\n~ = unusable square\n. = open square\n# = box beacon\nK = knight beacon\n";
        cout << "> = right facing arrow beacon\n";
        cout << "< = left facing arrow beacon\n";
        cout << "v = down facing arrow beacon\n";
        cout << "^ = up facing arrow beacon\n";
        cout << "- = horizontal wall beacon\n| = vertical wall beacon\nO = donut beacon\n";
    }
    cout << "\nThe map should look like this:\n";

    for (x = 0; x < MAX_ROW; x++)
    {
        for (y = 0; y < MAX_COL; y++)
        {
            switch (workmap[x][y])
            {
                case -1:
                {
                    cout << "~";
                    break;
                }
                case 0:
                {
                    cout << ".";
                    break;
                }
                case 1:
                {
                    cout << "#";
                    break;
                }
                case 2:
                {
                    cout << "K";
                    break;
                }
                case 3:
                {
                    cout << ">";
                    break;
                }
                case 4:
                {
                    cout << "<";
                    break;
                }
                case 5:
                {
                    cout << "v";
                    break;
                }
                case 6:
                {
                    cout << "^";
                    break;
                }
                case 7:
                {
                    cout << "-";
                    break;
                }
                case 8:
                {
                    cout << "|";
                    break;
                }
                case 9:
                {
                    cout << "O";
                    break;
                }
                default:
                {
                    cout << "ERROR";
                    break;
                }
            }
        }
        cout << "\n";
    }
    cout << "Empty map value: " << mapbasevalue << "  Populated map value: " << mapfinalvalue << "\n";

    return 0;
}

void CalcSqValues (int *sqhitby, int *sqavail, int x, int y)
{
    int z;

    for (z = 0; z < MAX_SQ; z++)
    {
        if (    ( (x + sqx[z]) > MIN_ROW)
             && ( (x + sqx[z]) < MAX_ROW)
             && ( (y + sqy[z]) > MIN_COL)
             && ( (y + sqy[z]) < MAX_COL)
           )
        {
            if (workmap[x + sqx[z]][y + sqy[z]] == 0)
            {
                *sqavail += 1;
            }
            else if (workmap[x + sqx[z]][y + sqy[z]] == 1)
            {
                *sqhitby += 1;
            }
        }
    }
}

void CalcKnValues (int *knhitby, int *knavail, int x, int y)
{
    int z;

    for (z = 0; z < MAX_KN; z++)
    {
        if (    ( (x + knx[z]) > MIN_ROW)
             && ( (x + knx[z]) < MAX_ROW)
             && ( (y + kny[z]) > MIN_COL)
             && ( (y + kny[z]) < MAX_COL)
           )
        {
            if (workmap[x + knx[z]][y + kny[z]] == 0)
            {
                *knavail += 1;
            }
            else if (workmap[x + knx[z]][y + kny[z]] == 2)
            {
                *knhitby += 1;
            }
        }
    }
}

void CalcArValues (int *arrhitby, int *arravail, int *arlhitby, int *arlavail, 
                   int *ardhitby, int *ardavail, int *aruhitby, int *aruavail, int x, int y)
{
    int z;

    for (z = 0; z < MAX_AR; z++)
    {
        if (    ( (x + arx[z]) > MIN_ROW)
             && ( (x + arx[z]) < MAX_ROW)
             && ( (y + ary[z]) > MIN_COL)
             && ( (y + ary[z]) < MAX_COL)
           )
        {
            if (workmap[x + arx[z]][y + ary[z]] == 0)
            {
                *arravail += 1;
            }
            else if (workmap[x + arx[z]][y + ary[z]] == 4)
            {
                *arlhitby += 1;
            }
        }

        if (    ( (x + arx[z + 20]) > MIN_ROW)
             && ( (x + arx[z + 20]) < MAX_ROW)
             && ( (y + ary[z + 20]) > MIN_COL)
             && ( (y + ary[z + 20]) < MAX_COL)
           )
        {
            if (workmap[x + arx[z + 20]][y + ary[z + 20]] == 0)
            {
                *arlavail += 1;
            }
            else if (workmap[x + arx[z + 20]][y + ary[z + 20]] == 3)
            {
                *arrhitby += 1;
            }
        }

        if (    ( (x + arx[z + 40]) > MIN_ROW)
             && ( (x + arx[z + 40]) < MAX_ROW)
             && ( (y + ary[z + 40]) > MIN_COL)
             && ( (y + ary[z + 40]) < MAX_COL)
           )
        {
            if (workmap[x + arx[z + 40]][y + ary[z + 40]] == 0)
            {
                *ardavail += 1;
            }
            else if (workmap[x + arx[z + 40]][y + ary[z + 40]] == 6)
            {
                *aruhitby += 1;
            }
        }

        if (    ( (x + arx[z + 60]) > MIN_ROW)
             && ( (x + arx[z + 60]) < MAX_ROW)
             && ( (y + ary[z + 60]) > MIN_COL)
             && ( (y + ary[z + 60]) < MAX_COL)
           )
        {
            if (workmap[x + arx[z + 60]][y + ary[z + 60]] == 0)
            {
                *aruavail += 1;
            }
            else if (workmap[x + arx[z + 60]][y + ary[z + 60]] == 5)
            {
                *ardhitby += 1;
            }
        }

    }
}

void CalcWlValues (int *hwlhitby, int *hwlavail, int *vwlhitby, int *vwlavail, int x, int y)
{
    int z;

    for (z = 0; z < MAX_WALL; z++)
    {
        if (    ( (x + wlb[z]) > MIN_ROW)
             && ( (x + wlb[z]) < MAX_ROW)
           )
        {
            if (workmap[x + wlb[z]][y] == 0)
            {
                *vwlavail += 1;
            }
            else if (workmap[x + wlb[z]][y] == 8)
            {
                *vwlhitby += 1;
            }
        }
        if (    ( (y + wlb[z]) > MIN_COL)
             && ( (y + wlb[z]) < MAX_COL)
           )
        {
            if (workmap[x][y + wlb[z]] == 0)
            {
                *hwlavail += 1;
            }
            else if (workmap[x][y + wlb[z]] == 7)
            {
                *hwlhitby += 1;
            }
        }
    }
}

void CalcDtValues (int *dthitby, int *dtavail, int x, int y)
{
    int z;

    for (z = 0; z < MAX_DONUT; z++)
    {
        if (    ( (x + dtx[z]) > MIN_ROW)
             && ( (x + dtx[z]) < MAX_ROW)
             && ( (y + dty[z]) > MIN_COL)
             && ( (y + dty[z]) < MAX_COL)
           )
        {
            if (workmap[x + dtx[z]][y + dty[z]] == 0)
            {
                *dtavail += 1;
            }
            else if (workmap[x + dtx[z]][y + dty[z]] == 9)
            {
                *dthitby += 1;
            }
        }
    }
}



