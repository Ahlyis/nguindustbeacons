#include "stdafx.h"
#include <string.h>
#include "Accessories.h"


int BeaconDisp2Num (char c)
{
    int iData;

    switch (c)
    {
        case '~':
        {
            iData = -1;
            break;
        }
        case '.':
        {
            iData = 0;
            break;
        }
        case '#':
        {
            iData = 1;
            break;
        }
        case 'K':
        {
            iData = 2;
            break;
        }
        case '>':
        {
            iData = 3;
            break;
        }
        case '<':
        {
            iData = 4;
            break;
        }
        case 'v':
        {
            iData = 5;
            break;
        }
        case '^':
        {
            iData = 6;
            break;
        }
        case '-':
        {
            iData = 7;
            break;
        }
        case '|':
        {
            iData = 8;
            break;
        }
        case 'O':
        {
            iData = 9;
            break;
        }
        default:
        {
            iData = -1;
            break;
        }
    }
    return iData;
}

char BeaconNum2Disp (int i)
{
    char c;

    switch (i)
    {
        case -1:
        {
            c = '~';
            break;
        }
        case 0:
        {
            c = '.';
            break;
        }
        case 1:
        {
            c = '#';
            break;
        }
        case 2:
        {
            c = 'K';
            break;
        }
        case 3:
        {
            c = '>';
            break;
        }
        case 4:
        {
            c = '<';
            break;
        }
        case 5:
        {
            c = 'v';
            break;
        }
        case 6:
        {
            c = '^';
            break;
        }
        case 7:
        {
            c = '-';
            break;
        }
        case 8:
        {
            c = '|';
            break;
        }
        case 9:
        {
            c = 'O';
            break;
        }
        default:
        {
            c = 'X';
            break;
        }
    }
    return c;
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

        if (iRetVal > 1000000000)
        {
            sData[0] = 0;
            x = 0;
            iRetVal = 0;
        }
    }
    iRetVal /= 10;

    return iRetVal;
}

