#ifndef TIDAL_CORR_H
#define TIDAL_CORR_H
#include "pos_algorith.h"
#include "trans_coor.h"

struct OCEANData
{
    QString StationName;// uppercase station name
    double amp[3][11];
    double phasedats[3][11];
    bool isRead;// to determine whether the data is read correctly, it may fail to read, true is correct, and flase fails
    OCEANData(){
        StationName="";
        isRead = false;
        for(int i=0;i<3;i++)
        {
            for(int j=0;j<11;j++)
            {
                amp[i][j] = phasedats[i][j]=0.0;
            }
        }
    }
};
class tidal_corr:public trans_coor
{
public:
    tidal_corr();
    ~tidal_corr(void);
    void setTideFileName(QString OCEANFileName = "",QString erpFileName = "");
    void getAllTideEffectENU(int Year,int Month,int Day,int Hours,int Minuts,double Seconds,double *pXYZ,double *pENU,double *psunpos=NULL,
        double *pmoonpos=NULL,double gmst = 0,QString StationName = "");
    double getAllTideEffect(int Year,int Month,int Day,int Hours,int Minuts,double Seconds,double *pXYZ,double *EA,double *psunpos=NULL,
        double *pmoonpos=NULL,double gmst = 0,QString StationName = "");
    void getPoleTide(int Year,int Month,int Day,int Hours,int Minuts,double Seconds,double *pBLH,double *pTideENU);
    void getSoildTide(int Year,int Month,int Day,int Hours,int Minuts,double Seconds,double *pXYZ,double *pTideENU,bool isElimate = false);
    void getOCEANTide(int Year,int Month,int Day,int Hours,int Minuts,double Seconds,double *pXYZ,double *pTideENU,QString StationName = "");
    void setStationName(QString StationName = "");
    void getAlltidalData();
    bool readRepFile(QString m_erpFileName);
private:
    void initTidarVar();
    bool readRepFile();// Read the ERP file if it exists
    bool getErpV(gtime_t obsGPST,double *erpV);// Read ERP file data to private variables
    void tide_pole(const double *pos, const double *erpv, double *denu);
    void subSolidTide(double *sunmoonPos,double *pXYZ,double *pTideXYZ,int flag);// Flag:0 represents the calculation of the moon and 1 represents the sun.
    int readerp(const char *file, erp_t *erp);
    bool readOCEANFile(QString  StationName,OCEANData &oceaData,QString  OCEANFileName = "");
public:
    QString m_erpFileName;
    erp_t m_erp_Data;
    bool isOCEANTide;
    bool isPoleEffect;// Whether to use erp tide default true
    bool isSolidTide;// Whether to use  solid tide the default true
    // Solid Tide Required Parameters
    double m_GMi[3];// The gravitational constants of the earth, moon and sun are stored separately.
    double loveShida2[2];// Initialization parameters of Love and Shadoi parameters of secondary solid tide
    bool isgetLoveShida2;// Whether to Obtain Love and Shadoi Parameters of Secondary Solid Tide
    double loveShida3[2];// Love and Shadoi parameters of three solid tides
    double m_SationBLH[3];// Station BLH
    double m_SecondFlag;// Save epoch seconds, prevent multiple calculations, reduce the amount of calculation
    double m_AllTideENU[3];// Preserve the ENU of this epoch to prevent multiple calculations and reduce the amount of calculation
    double m_pSolidENU[3];// Storage Computation of Solid Tide
    double m_pPoleENU[3];// The Tide of Storage Computing
    double m_pOCEANENU[3];// Storage Computing Tides
    double m_erpV[5];// Storing polar shift parameters of the first epoch
    bool isReadErp;// Determine whether to read the ERP file (read only once, even if it fails, it is true, not read, the corresponding tide will be closed, no effect)
    bool isReadOCEAN;// Determine whether OCEAN data has been read (read only once, even if it fails, it is true, and if it is not read, the corresponding tide will be closed, without effect)
    // Reading marine data classes
    QString m_OCEANFileName;// Name of Ocean Data File
    QFile m_readOCEANClass;
    OCEANData m_OCEANData;// Preserving station data
    QString m_StationName;
    static const double args[][5];
    double LeapSeconds;// Hop seconds for the current year, used to transfer to UTC on the day of the year, month and day
    bool isGetPos;// Determine whether or not to obtain the above data
private:

};

#endif // TIDAL_CORR_H
