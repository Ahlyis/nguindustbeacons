// Alchemy.cpp : Defines the entry point for the console application.
// 

#include "stdafx.h"
#include <string.h>
#include <math.h>
#include <fstream>
#include <iostream>
#include <time.h>
#include <windows.h>
#include "Accessories.h"

using namespace std;

int basemap[MAX_ROW][MAX_COL];
int bestmap[MAX_ROW][MAX_COL];
int workmap[MAX_ROW][MAX_COL];
int baseworkmap[MAX_ROW][MAX_COL];
int sqx[8];
int sqy[8];
int knx[8];
int kny[8];
int arx[71];
int ary[71];
int beaconval[3][5];
int beaconxref[20];
int forcetype;
int forcecountmin;
int forcecountmax;
bool PrintLegend = true;
int wlb[12];
int dtx[16];
int dty[16];
int mapbasevalue;
int mapfinalvalue;
int iTotalArea = MAX_ROW * MAX_COL;
int searchorder[MAX_ROW * MAX_COL];
int iTries;
int iBestVal;
bool bDebug = false;
HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
int beaconcolor = 15;
bestmaps *maplist = 0;
int iTotalMaps = 0;

void WriteOneMap (ofstream& Data, int mapnum, int beacontype, int score, int map[MAX_ROW][MAX_COL], int beacons);

int main(int argc, char* argv[])
{
    int i;
    int maps = 1,
        options = 0,
        beacons = 0,
        beacontype = 0;

    forcetype = -1;
    forcecountmin = forcecountmax = 0;
    mapbasevalue = 0;
    mapfinalvalue = 0;
    iBestVal = 0;

    if (!LoadOperatingMode (&maps, &options, &beacons, &beacontype) )
    {
        iTries = 1;
        /* Initialize random seed. */
        srand ( time(NULL) );

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

        printf ("Up to which beacons? 0=box only, 1=knight, 2=arrow, 3=wall, 4=donut\n");
        cin >> beacons;

        if ( (beacons < 1) || (beacons > 4) )
        {
            beacons = 0;
        }

        printf ("Which beacon type? 0=speed, 1=production, 2=efficiency, 3=prod/eff mix\n");
        cin >> beacontype;

        if ( (beacontype < 1) || (beacontype > 3) )
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
        cin >> forcecountmin;

        if ( (forcecountmin < 1) || (forcecountmin > 999) )
        {
            forcecountmin = 0;
        }
        forcecountmax = forcecountmin; /* variable force count range only via input file. */
    }

    if (bDebug)
    {
        if (!DebugTestGrounds () )
        {
            return 0;
        }
    }
    SetWhichToCheck (maps, options);
    InitBeacons ();
    beaconcolor = 11;

    if (beacontype == 1)
        beaconcolor = 5;
    else if (beacontype == 2)
        beaconcolor = 14;


    for (i = 0; i < iTries; i++)
    {
        if (bDebug)
        {
            cout << "Try #" << i + 1;
        }

        if (beacontype != 3)
        {
            CalculateBestBeacons (options, beacons, beacontype);
        }
        else
        {
            CalculateMixedBeacons (options, beacons, beacontype);
        }
    }

    CopyMap (bestmap, workmap);
    mapfinalvalue = iBestVal;
    DisplayResults (maps, options, beacons, beacontype);

    if (NewBestMap (maps, beacontype, beacons) )
    {
        SaveBestMaps ();
    }

    Cleanup ();

    return 0;
}

bool LoadOperatingMode (int *maps, int *options, int *beacons, int *beacontype)
{
    char sData[360] = {0},
         sFile[80];
    bool bRetVal = false;
    unsigned int iSeed = 0;

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

        if ( (*beacons < 1) || (*beacons > 4) )
            *beacons = 0;

        Data >> sData;

        *beacontype = MakeNumber (sData);

        if ( (*beacontype < 1) || (*beacontype > 3) )
            *beacontype = 0;

        Data >> sData;

        forcetype = MakeNumber (sData);

        if ( (forcetype < 1) || (forcetype > 4) )
            forcetype = 0;

        Data >> sData;

        forcecountmin = MakeNumber (sData);

        if ( (forcecountmin < 1) || (forcecountmin > 999) )
            forcecountmin = 0;

        Data >> sData;

        forcecountmax = MakeNumber (sData);

        if ( (forcecountmax < forcecountmin) || (forcecountmax > 999) )
            forcecountmax = forcecountmin;

        Data >> sData;

        if (MakeNumber (sData) == 1)
            PrintLegend = false;
        else
            PrintLegend = true;

        Data >> sData;

        iSeed = MakeNumber (sData);

        if (iSeed > 0)
        {
            srand (iSeed);
        }
        else
        {
            /* Initialize random seed. */
            srand (time (NULL) );
        }

        Data >> sData;

        iTries = MakeNumber (sData);

        if (iTries < 1)
            iTries = 1;

        cout << "We will be running " << iTries << " attempts this run.\n";

       bRetVal = true;
    }

        Data >> sData;

        if (MakeNumber (sData) == 1)
            bDebug = true;

    return bRetVal;
}

int  CalculateBestBeacons (int options, int beacons, int beacontype)
{
    int i, x, y, workx, worky, workval, workbeacon, currval;
    int sqhitby, sqavail, knhitby, knavail, arlhitby, arlavail;
    int arrhitby, arravail, aruhitby, aruavail, ardhitby, ardavail;
    int hwlhitby, hwlavail, vwlhitby, vwlavail, dthitby, dtavail;

    int tempval, currforcecount;
    bool done = false;

    Shuffle (); /* randomizes the searchorder. */
    memcpy (workmap, baseworkmap, iTotalArea * sizeof (int) );

    currforcecount = 0;

    if (forcecountmax > 0)
    {
        currforcecount = forcecountmin + rand() % (forcecountmax - forcecountmin + 1);
    }

    while (!done)
    {
        workval = workx = worky = workbeacon = 0;
        tempval = 0;
        done = true;

        for (i = 0; i < iTotalArea; i++)
        {
            x = searchorder[i] / MAX_COL;
            y = searchorder[i] % MAX_COL;
            currval = 0;

            if (workmap[x][y] == 0)
            {
                sqhitby = sqavail = knhitby = knavail = arlhitby = arlavail = 0;
                arrhitby = arravail = aruhitby = aruavail = ardhitby = ardavail = 0;
                hwlhitby = hwlavail = vwlhitby = vwlavail = dthitby = dtavail = 0;

                CalcSqValues (&sqhitby, &sqavail, x, y);

                if (beacons > 0)
                {
                    CalcKnValues (&knhitby, &knavail, x, y);
                }

                if (beacons > 1)
                {
                    CalcArValues (&arrhitby, &arravail, &arlhitby, &arlavail,
                                  &ardhitby, &ardavail, &aruhitby, &aruavail, x, y);
                }

                if (beacons > 2)
                {
                    CalcWlValues (&hwlhitby, &hwlavail, &vwlhitby, &vwlavail, x, y);
                }

                if (beacons > 3)
                {
                    CalcDtValues (&dthitby, &dtavail, x, y);
                }

                currval -= sqhitby * beaconval[beacontype][0 /*squares*/];
                currval -= knhitby * beaconval[beacontype][1 /*knights*/];
                currval -= arrhitby * beaconval[beacontype][2 /*arrows*/];
                currval -= arlhitby * beaconval[beacontype][2 /*arrows*/];
                currval -= ardhitby * beaconval[beacontype][2 /*arrows*/];
                currval -= aruhitby * beaconval[beacontype][2 /*arrows*/];
                currval -= hwlhitby * beaconval[beacontype][3 /*walls*/];
                currval -= vwlhitby * beaconval[beacontype][3 /*walls*/];
                currval -= dthitby * beaconval[beacontype][4 /*donuts*/];

                if (    (currforcecount <= 0)
                     || (forcetype == 0)
                   )
                {
                    tempval = currval + sqavail * beaconval[beacontype][0 /*squares*/];

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
                }

                if (    (currforcecount <= 0)
                     || (forcetype == 1)
                   )
                {
                    tempval = currval + knavail * beaconval[beacontype][1 /*knights*/];

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
                }

                if (    (currforcecount <= 0)
                     || (forcetype == 2)
                   )
                {
                    tempval = currval + arravail * beaconval[beacontype][2 /*arrows*/];

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
                }

                if (    (currforcecount <= 0)
                     || (forcetype == 2)
                   )
                {
                    tempval = currval + arlavail * beaconval[beacontype][2 /*arrows*/];

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
                }

                if (    (currforcecount <= 0)
                     || (forcetype == 2)
                   )
                {
                    tempval = currval + ardavail * beaconval[beacontype][2 /*arrows*/];

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
                }

                if (    (currforcecount <= 0)
                     || (forcetype == 2)
                   )
                {
                    tempval = currval + aruavail * beaconval[beacontype][2 /*arrows*/];

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
                }

                if (    (currforcecount <= 0)
                     || (forcetype == 3)
                   )
                {
                    tempval = currval + hwlavail * beaconval[beacontype][3 /*walls*/];

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
                }

                if (    (currforcecount <= 0)
                     || (forcetype == 3)
                   )
                {
                    tempval = currval + vwlavail * beaconval[beacontype][3 /*walls*/];

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
                }

                if (    (currforcecount <= 0)
                     || (forcetype == 4)
                   )
                {
                    tempval = currval + dtavail * beaconval[beacontype][4 /*donuts*/];

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
            currforcecount--;
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
                currval += hwlhitby * beaconval[beacontype][3 /*walls*/];
                currval += vwlhitby * beaconval[beacontype][3 /*walls*/];
                currval += dthitby * beaconval[beacontype][4 /*donuts*/];

                mapfinalvalue += currval;
            }
        }
    }

    if (bDebug)
    {
        cout << "  MapValue: " << mapfinalvalue << "\n";
    }

    if (mapfinalvalue > iBestVal)
    {
        iBestVal = mapfinalvalue;
        CopyMap (workmap, bestmap);
    }
    return 0;
}

int  DisplayResults (int maps, int options, int beacons, int beacontype)
{
    int x, y;
    SetConsoleTextAttribute(hConsole, 15);

    if (bDebug)
    {
        for (x = 0; x < 16; x++)
        {
            SetConsoleTextAttribute(hConsole, x);
            cout << "This is just a test.\n";
        }
        SetConsoleTextAttribute(hConsole, 15);
    }

    cout << "Calculated beacon placement for map: ";

    switch (maps)
    {
        case 1:
        {
            SetConsoleTextAttribute(hConsole, 1);
            cout << "Tutorial\n";
            break;
        }
        case 2:
        {
            SetConsoleTextAttribute(hConsole, 12);
            cout << "Flesh\n";
            break;
        }
        case 3:
        {
            SetConsoleTextAttribute(hConsole, 3);
            cout << "Tronne\n";
            break;
        }
        case 4:
        {
            SetConsoleTextAttribute(hConsole, 13);
            cout << "Candyland\n";
            break;
        }
        default:
        {
            cout << "Unknown - " << maps << "\n";
            break;
        }
    }
    SetConsoleTextAttribute(hConsole, 15);

    if (options == 1)
    {
        cout << "With every square unlocked.\n";
    }
    else
    {
        cout << "Ignoring locked squares.\n";
    }

    cout << "Using box";

    if (beacons == 0)
    {
        cout << " beacons only.\n";
    }
    else if (beacons == 1)
    {
        cout << " and knight beacons only.\n";
    }
    else if (beacons == 2)
    {
        cout << ", knight and arrow beacons.\n";
    }
    else if (beacons == 3)
    {
        cout << " through wall beacons.\n";
    }
    else if (beacons == 4)
    {
        cout << " through donut beacons.\n";
    }
    else
    {
        cout << "... ERROR. Using beacon " << beacons << " is not supported!\n";
    }

    cout << "Using ";
    switch (beacontype)
    {
        case 0:
        {
            SetConsoleTextAttribute(hConsole, 11);
            cout << "speed";
            break;
        }
        case 1:
        {
            SetConsoleTextAttribute(hConsole, 5);
            cout << "production";
            break;
        }
        case 2:
        {
            SetConsoleTextAttribute(hConsole, 14);
            cout << "efficiency";
            break;
        }
        default:
        {
            cout << "Using ERROR - TYPE " << beacontype << " beacons.\n\n";
            break;
        }
    }
    SetConsoleTextAttribute(hConsole, 15);
    cout << " beacons.\n";

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
                    if (maps == 1)
                        SetConsoleTextAttribute(hConsole, 24);
                    else if (maps == 2)
                        SetConsoleTextAttribute(hConsole, 197);
                    else if (maps == 3)
                        SetConsoleTextAttribute(hConsole, 48);
                    else if (maps == 4)
                        SetConsoleTextAttribute(hConsole, 222);

                    cout << "~";
                    break;
                }
                case 0:
                {
                    SetConsoleTextAttribute(hConsole, 15);
                    cout << ".";
                    break;
                }
                case 1:
                {
                    SetConsoleTextAttribute(hConsole, beaconcolor);
                    cout << "#";
                    break;
                }
                case 2:
                {
                    SetConsoleTextAttribute(hConsole, beaconcolor);
                    cout << "K";
                    break;
                }
                case 3:
                {
                    SetConsoleTextAttribute(hConsole, beaconcolor);
                    cout << ">";
                    break;
                }
                case 4:
                {
                    SetConsoleTextAttribute(hConsole, beaconcolor);
                    cout << "<";
                    break;
                }
                case 5:
                {
                    SetConsoleTextAttribute(hConsole, beaconcolor);
                    cout << "v";
                    break;
                }
                case 6:
                {
                    SetConsoleTextAttribute(hConsole, beaconcolor);
                    cout << "^";
                    break;
                }
                case 7:
                {
                    SetConsoleTextAttribute(hConsole, beaconcolor);
                    cout << "-";
                    break;
                }
                case 8:
                {
                    SetConsoleTextAttribute(hConsole, beaconcolor);
                    cout << "|";
                    break;
                }
                case 9:
                {
                    SetConsoleTextAttribute(hConsole, beaconcolor);
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
        SetConsoleTextAttribute(hConsole, 15);
        cout << " \n";
    }
    SetConsoleTextAttribute(hConsole, 15);
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

void Shuffle ()
{
    int x, i, temp;

    for (x = 0; x < iTotalArea; x++)
    {
        i = rand() % (iTotalArea);
        temp = searchorder[x];
        searchorder[x] = searchorder[i];
        searchorder[i] = temp;
    }

    return;
}

void CopyMap (int sourcemap[MAX_ROW][MAX_COL], int destmap[MAX_ROW][MAX_COL])
{
    int x, y;

    for (x = 0; x < MAX_ROW; x++)
    {
        for (y = 0; y < MAX_COL; y++)
        {
            destmap[x][y] = sourcemap[x][y];
        }
    }
}

int  CalculateMixedBeacons (int options, int beacons, int beacontype)
{
    short shMap[MAX_ROW * MAX_COL][20] = { 0 };
    int i, x, y, workx, worky, workval, workbeacon, currval;
    int sqhitby, sqavail, knhitby, knavail, arlhitby, arlavail;
    int arrhitby, arravail, aruhitby, aruavail, ardhitby, ardavail;
    int hwlhitby, hwlavail, vwlhitby, vwlavail, dthitby, dtavail;

    int tempval, currforcecount;
    bool done = false;

    Shuffle (); /* randomizes the searchorder. */
    memcpy (workmap, baseworkmap, iTotalArea * sizeof (int) );

    currforcecount = 0;

    if (forcecountmax > 0)
    {
        currforcecount = forcecountmin + rand() % (forcecountmax - forcecountmin + 1);
    }

    while (!done)
    {
        workval = workx = worky = workbeacon = 0;
        tempval = 0;
        done = true;

        for (i = 0; i < iTotalArea; i++)
        {
            x = searchorder[i] / MAX_COL;
            y = searchorder[i] % MAX_COL;
            currval = 0;

            if (workmap[x][y] == 0)
            {
                sqhitby = sqavail = knhitby = knavail = arlhitby = arlavail = 0;
                arrhitby = arravail = aruhitby = aruavail = ardhitby = ardavail = 0;
                hwlhitby = hwlavail = vwlhitby = vwlavail = dthitby = dtavail = 0;

                CalcSqValues (&sqhitby, &sqavail, x, y);

                if (beacons > 0)
                {
                    CalcKnValues (&knhitby, &knavail, x, y);
                }

                if (beacons > 1)
                {
                    CalcArValues (&arrhitby, &arravail, &arlhitby, &arlavail,
                                  &ardhitby, &ardavail, &aruhitby, &aruavail, x, y);
                }

                if (beacons > 2)
                {
                    CalcWlValues (&hwlhitby, &hwlavail, &vwlhitby, &vwlavail, x, y);
                }

                if (beacons > 3)
                {
                    CalcDtValues (&dthitby, &dtavail, x, y);
                }

                currval -= sqhitby * beaconval[beacontype][0 /*squares*/];
                currval -= knhitby * beaconval[beacontype][1 /*knights*/];
                currval -= arrhitby * beaconval[beacontype][2 /*arrows*/];
                currval -= arlhitby * beaconval[beacontype][2 /*arrows*/];
                currval -= ardhitby * beaconval[beacontype][2 /*arrows*/];
                currval -= aruhitby * beaconval[beacontype][2 /*arrows*/];
                currval -= hwlhitby * beaconval[beacontype][3 /*walls*/];
                currval -= vwlhitby * beaconval[beacontype][3 /*walls*/];
                currval -= dthitby * beaconval[beacontype][4 /*donuts*/];

                if (    (currforcecount <= 0)
                     || (forcetype == 0)
                   )
                {
                    tempval = currval + sqavail * beaconval[beacontype][0 /*squares*/];

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
                }

                if (    (currforcecount <= 0)
                     || (forcetype == 1)
                   )
                {
                    tempval = currval + knavail * beaconval[beacontype][1 /*knights*/];

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
                }

                if (    (currforcecount <= 0)
                     || (forcetype == 2)
                   )
                {
                    tempval = currval + arravail * beaconval[beacontype][2 /*arrows*/];

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
                }

                if (    (currforcecount <= 0)
                     || (forcetype == 2)
                   )
                {
                    tempval = currval + arlavail * beaconval[beacontype][2 /*arrows*/];

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
                }

                if (    (currforcecount <= 0)
                     || (forcetype == 2)
                   )
                {
                    tempval = currval + ardavail * beaconval[beacontype][2 /*arrows*/];

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
                }

                if (    (currforcecount <= 0)
                     || (forcetype == 2)
                   )
                {
                    tempval = currval + aruavail * beaconval[beacontype][2 /*arrows*/];

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
                }

                if (    (currforcecount <= 0)
                     || (forcetype == 3)
                   )
                {
                    tempval = currval + hwlavail * beaconval[beacontype][3 /*walls*/];

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
                }

                if (    (currforcecount <= 0)
                     || (forcetype == 3)
                   )
                {
                    tempval = currval + vwlavail * beaconval[beacontype][3 /*walls*/];

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
                }

                if (    (currforcecount <= 0)
                     || (forcetype == 4)
                   )
                {
                    tempval = currval + dtavail * beaconval[beacontype][4 /*donuts*/];

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
            currforcecount--;
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
                currval += hwlhitby * beaconval[beacontype][3 /*walls*/];
                currval += vwlhitby * beaconval[beacontype][3 /*walls*/];
                currval += dthitby * beaconval[beacontype][4 /*donuts*/];

                mapfinalvalue += currval;
            }
        }
    }

    if (bDebug)
    {
        cout << "  MapValue: " << mapfinalvalue << "\n";
    }

    if (mapfinalvalue > iBestVal)
    {
        iBestVal = mapfinalvalue;
        CopyMap (workmap, bestmap);
    }
    return 0;
}

void DebugCreateNew ()
{
    char sData[360] = {0},
         sFile[80];
    int iTest = 0;

    sprintf (sFile, "debugtest.txt");

    ofstream Data(sFile);

    Data << "Here is the new version of the file" << endl;
    Data << "This should be the second line." << endl << "1" << endl;
    return;
}

bool DebugTestGrounds ()
{
    bool bRetVal = true;

    char sData[360] = {0},
         sFile[80];
    int iTest = 0;

    sprintf (sFile, ".\\debugtest.txt");

    ifstream Data(sFile);

    while (iTest == 0)
    {
        Data >> sData;
        iTest = MakeNumber (sData);
        cout << sData << endl;
    }

//    system ("del debugtest.txt /q");

    DebugCreateNew();

    bRetVal = false;


    return bRetVal;
}

void WriteOneMap (ofstream& Data, int mapnum, int beacontype, int score, int map[MAX_ROW][MAX_COL], int beacons)
{
    int x, y;

    Data << endl << mapnum;
    
    switch (mapnum)
    {
        case 1:
        {
            Data << "-Tutorial" << endl;
            break;
        }
        case 2:
        {
            Data << "-Flesh" << endl;
            break;
        }
        case 3:
        {
            Data << "-Tronne" << endl;
            break;
        }
        case 4:
        {
            Data << "-Candyland" << endl;
            break;
        }
        default:
        {
            Data << "-unknown_map" << endl;
            break;
        }
    }
    Data << beacontype;

    switch (beacontype)
    {
        case 0:
        {
            Data << "-Speed" << endl;
            break;
        }
        case 1:
        {
            Data << "-Production" << endl;
            break;
        }
        case 2:
        {
            Data << "-Efficiency" << endl;
            break;
        }
        case 3:
        {
            Data << "-Speed/Production_mix" << endl;
            break;
        }
        default:
        {
            Data << "-unknown" << endl;
            break;
        }
    }

    Data << beacons;

    switch (beacons)
    {
        case 0:
        {
            Data << "-Boxes_only" << endl;
            break;
        }
        case 1:
        {
            Data << "-Up_to_Knights" << endl;
            break;
        }
        case 2:
        {
            Data << "-Up_to_Arrows" << endl;
            break;
        }
        case 3:
        {
            Data << "-Up_to_Walls" << endl;
            break;
        }
        case 4:
        {
            Data << "-Up_to_Donuts" << endl;
            break;
        }
        default:
        {
            Data << "-unknown" << endl;
            break;
        }
    }
    Data << score << "-Score" << endl;

    for (x = 0; x < MAX_ROW; x++)
    {
        for (y = 0; y < MAX_COL; y++)
        {
            Data << BeaconNum2Disp (map[x][y]);
        }
        Data << endl;
    }
    Data << endl;

}

void LoadBestMaps ()
{
    bestmaps *temp = maplist, *temp2 = 0;
    char sData[360] = {0},
         sFile[80];
    int i, x, y, iData;

    sprintf (sFile, ".\\bestmaps.txt");

    ifstream Data(sFile);

    while (temp)
    {
        temp = maplist->next;
        free (maplist);
        maplist = temp;
    }
    i = 0;
    Data >> sData;
    iTotalMaps = MakeNumber (sData);

    while (i < iTotalMaps)
    {
        i++;
        temp = (bestmaps *) calloc (1, sizeof (bestmaps) );

        if (!temp)
        {
            exit (0);
        }

        Data >> sData;
        iData = MakeNumber (sData);
        temp->mapnum = iData;

        Data >> sData;
        iData = MakeNumber (sData);
        temp->beacontype = iData;

        Data >> sData;
        iData = MakeNumber (sData);
        temp->maxbeacon = iData;

        temp->mapID = (temp->mapnum * 100) + (temp->beacontype * 10) + temp->maxbeacon;

        Data >> sData;
        iData = MakeNumber (sData);
        temp->score = iData;

        for (x = 0; x < MAX_ROW; x++)
        {
            Data >> sData;

            for (y = 0; y < MAX_COL; y++)
            {
                iData = BeaconDisp2Num (sData[y]);
                temp->map[x][y] = iData;
            }
        }

        if (!maplist)
        {
            maplist = temp;
        }
        else
        {
            temp2 = maplist;

            while (    temp2->next
                    && (    (temp->map > temp2->next->map)
                         || (    (temp->map == temp2->next->map)
                              && (temp->beacontype > temp2->next->beacontype)
                            )
                       )
                  )
            {
                temp2 = temp2->next;
            }

            if (    !temp2
                 || (temp->map != temp2->next->map)
                 || (temp->beacontype != temp2->next->beacontype)
               )
            {
                temp->next = temp2->next;
                temp2->next = temp;
            }
            else
            {
                temp->next = temp2->next->next;
                free (temp2->next);
                temp2->next = temp;
            }
        }
    }
    Data.close();
}

void SaveBestMaps ()
{
    bestmaps *temp = maplist, *temp2 = 0;
    char sData[360] = {0},
         sFile[80];

    sprintf (sFile, ".\\bestmaps.txt");

    ofstream Data(sFile);

    iTotalMaps = 0;

    while (temp)
    {
        iTotalMaps++;
        temp = temp->next;
    }

    Data << iTotalMaps << "-Total_number_of_maps_recorded" << endl;

    temp = maplist;

    while (temp)
    {
        WriteOneMap (Data, temp->mapnum, temp->beacontype, temp->score, temp->map, temp->maxbeacon);
        temp = temp->next;
    }
    Data.close ();
    return;
}

void Cleanup ()
{
    bestmaps *temp;

    while (maplist)
    {
        temp = maplist->next;
        free (maplist);
        maplist = temp;
    }
}

/* This originally was just to load the map. It now includes other setup functionality as well. */
void SetWhichToCheck (int maps, int options)
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

    for (x = 0; x < (MAX_ROW * MAX_COL); x++)
    {
        searchorder[x] = x;
    }

    for (x = 0; x < MAX_ROW; x++)
    {
        for (y = 0; y < MAX_COL; y++)
        {
            switch (basemap[x][y])
            {
                case 1:
                {
                    baseworkmap[x][y] = 0;
                    break;
                }
                case 2:
                {
                    if (options == 1)
                    {
                        baseworkmap[x][y] = 0;
                    }
                    else
                    {
                        baseworkmap[x][y] = -1;
                    }
                    break;
                }
                default:
                {
                    baseworkmap[x][y] = -1;
                    break;
                }
            }
        }
    } // initialize base working map,

    beaconxref[1] = 0; /* Boxes */
    beaconxref[2] = 1; /* Knights */
    beaconxref[3] = 2; /* Arrows */
    beaconxref[4] = 2; /* Arrows */
    beaconxref[5] = 2; /* Arrows */
    beaconxref[6] = 2; /* Arrows */
    beaconxref[7] = 3; /* Walls */
    beaconxref[8] = 3; /* Walls */
    beaconxref[9] = 4; /* Donuts */

    return;
}

bool NewBestMap (int map, int beacontype, int beacons)
{
    bestmaps *temp = 0, *temp2 = 0;
    bool bRetVal = false;
    int iMapID;

    iMapID = (map * 100) + (beacontype * 10) + beacons;

    if (!maplist)
    {
        LoadBestMaps ();

        if (!maplist)
        {
            maplist = (bestmaps*) calloc (1, sizeof (bestmaps) );

            if (!maplist)
            {
                exit (0);
            }
            maplist->mapnum = map;
            maplist->beacontype = beacontype;
            maplist->maxbeacon = beacons;
            maplist->score = iBestVal;
            maplist->mapID = (map * 100) + (beacontype * 10) + beacons;
            memcpy (maplist->map, bestmap, iTotalArea * sizeof (int) );
            bRetVal = true;
        }
    }

    temp = maplist;

    if (    temp
         && (iMapID < temp->mapID)
       ) /* Is new map lower than first recorded map? */
    {
        temp2 = (bestmaps*) calloc (1, sizeof (bestmaps) );

        if (!temp2)
        {
            exit (0);
        }
        temp2->mapnum = map;
        temp2->beacontype = beacontype;
        temp2->maxbeacon = beacons;
        temp2->mapID = iMapID;
        temp2->score = iBestVal;
        memcpy (temp2->map, bestmap, iTotalArea * sizeof (int) );
        temp2->next = maplist;
        maplist = temp2;
        temp = 0;
        bRetVal = true;
    }

    while (temp)
    {
        if (iMapID == temp->mapID)
        {
            /* Map exists. Need to see if new result is better. */
            if (iBestVal > temp->score)
            {
                temp->score = iBestVal;
                memcpy (temp->map, bestmap, iTotalArea * sizeof (int) );
                temp = 0;
                bRetVal = true;
            }
            temp = 0;
        }
        else if (    (temp->next)
                  && (iMapID >= temp->next->mapID)
                )
        {
            temp = temp->next;
        }
        else
        {
            temp2 = (bestmaps*) calloc (1, sizeof (bestmaps) );

            if (!temp2)
            {
                exit (0);
            }
            temp2->mapnum = map;
            temp2->beacontype = beacontype;
            temp2->maxbeacon = beacons;
            temp2->mapID = iMapID;
            temp2->score = iBestVal;
            memcpy (temp2->map, bestmap, iTotalArea * sizeof (int) );
            temp2->next = temp->next;
            temp->next = temp2;
            temp = 0;
            bRetVal = true;
        }
    }

    return bRetVal;
}

