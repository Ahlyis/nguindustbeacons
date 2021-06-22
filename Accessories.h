#define MIN_ROW -1
#define MIN_COL -1
#define MAX_ROW 17
#define MAX_COL 20
#define MAX_MAPS 5
#define MAX_BX 8
#define MAX_KN 8
#define MAX_AR 11
#define MAX_WALL 12
#define MAX_DONUT 16
#define SPEED_COLOR 11
#define PROD_COLOR 5
#define EFF_COLOR 14

typedef struct bestmaps
{
    int mapnum;
    int beacontype;
    int maxbeacon;
    int score;
    int mapID;
    int map[MAX_ROW][MAX_COL];
    bestmaps *next;
} bestmaps;

void SetWhichToCheck (int maps, int options);
int  MakeNumber (char *sData);
void LoadOperatingMode (int *maps, int *options, int *beacons, int *beacontype);
int  CalculateBestBeacons (int options, int beacons, int beacontype);
int  DisplayResults (int maps, int options, int beacons, int beacontype);
void InitBeacons ();
void CalcSqValues (int *sqhitby, int *sqavail, int x, int y);
void CalcKnValues (int *knhitby, int *knavail, int x, int y);
void CalcArValues (int *arrhitby, int *arravail, int *arlhitby, int *arlavail, 
                   int *ardhitby, int *ardavail, int *aruhitby, int *aruavail, int x, int y);
void CalcWlValues (int *hwlhitby, int *hwlavail, int *vwlhitby, int *vwlavail, int x, int y);
void CalcDtValues (int *dthitby, int *dtavail, int x, int y);
void Shuffle ();
void CopyMap (int sourcemap[MAX_ROW][MAX_COL], int destmap[MAX_ROW][MAX_COL]);
int  CalculateMixedBeacons (int options, int beacons, int beacontype);
bool DebugTestGrounds (bool *bTest);
void LoadBestMaps ();
void SaveBestMaps ();
int BeaconDisp2Num (char c);
char BeaconNum2Disp (int i);
void Cleanup ();
bool NewBestMap (int map, int beacontype, int beacons);
void RandomizeParameters (int *map, int *beacons, int *beacontype);
void DisplayMaps (int maps, int beacons, int beacontype);
int  CalcMapScore (int beacontype);
