#ifndef RINEX_SP3_H
#define RINEX_SP3_H

#include "pos_algorith.h"
#include "pos_alg/trans_epht.h"
#include "sgg_fcb.h"
/*the coordinate system comes with SP3*/
struct SP3_xyzData
{/*SP3 file data format*/
    double SP3_x;
    double SP3_y;
    double SP3_z;
    double SP3_clk;
    SP3_xyzData(){
        SP3_x=SP3_y=SP3_z=SP3_clk=0.0;
    }
};
struct SP3Data
{/*SP3 file data format*/
    int GPSWeek;/*GPS week*/
    int GPSTime;/*GPS week  seconds*/
    QVector< SP3_xyzData > GPSSP3_data;/*[MAXPRNGPS]*/
    QVector< SP3_xyzData > GLOSP3_data;/*[MAXPRNGLO]*/
    QVector< SP3_xyzData > BDSSP3_data;/*[MAXPRNBDS]*/
    QVector< SP3_xyzData > GLESP3_data;/*[MAXPRNGLE]*/
    SP3Data(){
        GPSWeek=GPSTime=0;
    }
};

class rinex_sp3:public sgg_fcb
{
public:
    rinex_sp3();
    void setSP3FileNames(QStringList SP3FileNames);
    void getPrcisePoint(int PRN, char SatType, double GPST, double *pXYZ, double *pdXYZ, double *pSp3Clk,double *pOrb_Var);
    void releaseSP3AllData();
private:
    void InitSp3DatStruct();
    void get8Point(int PRN,char SatType,double *pX,double *pY,double *pZ,int *pGPST,double GPST ,double *pClk);
    void initSp3Var();
    void lagrangeMethod(int PRN,char SatType,double GPST,double *pXYZ,double *pdXYZ, double *pSp3Clk,double *Orbit_Var);
    void readFileData2Vec(QStringList SP3FileNames);

public:

    QStringList m_SP3FileNames;
    bool  ACCELERATE;/*1: getPrcisePoint function time from small to large*/
    bool isReadSp3Head;/*whether to read header file*/
    static const int lagrangeSp3Fact;
    bool isReadAllSp3Data;/*whether to read the entire file data*/
    bool IsSigalSp3File;/*whether multiple files or a single file*/
    int m_WeekSp3Order;/*save the number of cross-week files*/
    QString m_SP3FileName;/*save individual file names*/
    QVector< SP3Data > m_allEpochSp3Data;/*coordinate data for each epoch. PRN X,Y and
                                       *Z are stored in the I column of time, minute and second*/
    int m_lastGPSTime_Sp3Flag;/*save the first GPS time data obtained by get8Point last time*/
    double m_lastCmpGPS_Sp3Time;/*save the last calculated GPST to prevent one epoch*/

private:
    SP3Data epochSp3Data_ref;/*store each epoch metadata*/

    QString tempSp3Line;/* Temporary storage of read row data*/
        /*variables used to optimize looking
     * for clock differences prevent looking all the way from 0*/
//    int m_EndHeadNum;/*the first and last clock difference between the number of read blocks*/

};

#endif // RINEX_SP3_H
