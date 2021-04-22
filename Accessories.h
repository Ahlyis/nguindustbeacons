#define MIN_ROW -1
#define MIN_COL -1
#define MAX_ROW 17
#define MAX_COL 20
#define MAX_MAPS 4
#define MAX_SQ 8
#define MAX_KN 8
#define MAX_AR 11
#define AR_RIGHT 0
#define AR_LEFT 20
#define AR_DOWN 40
#define AR_UP 60
#define MAX_WALL 12
#define MAX_DONUT 16

void SetWhichToCheck (int maps, int options);
int  MakeNumber (char *sData);
bool LoadOperatingMode (int *maps, int *options, int *beacons, int *beacontype);
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
