#ifndef RINEX_CLK_H
#define RINEX_CLK_H

#include "pos_algorith.h"
#include "rinex_obs.h"
struct SatClkData
{
    gtime_t prec_clkT;
    double prec_clk;/*Precision clock correcting*/
    double prec_clkVar;/*Precision clock correcting*/
    SatClkData(){
        prec_clk=prec_clkVar=0.0;
    }
};
struct CLKData
{/*Clk file data format*/
    int GPSWeek;
    double GPSTime;/*GPS week within seconds*/
    QVector< SatClkData > GPSClkData;/*GPS [MAXPRNGPS]*/
    QVector< SatClkData > GLOClkData;/*Glonass [MAXPRNGLO]*/
    QVector< SatClkData > BDSClkData;/*BDS [MAXPRNBDS]*/
    QVector< SatClkData > GLEClkData;/*Galileo [MAXPRNGLE]*/
    CLKData(){
        GPSWeek=GPSTime=0;
    }
};
struct WL_UPD
{
    double wlupd_T;
    double UPD_value;/*WIDELANE SATELLITE FRACTIONNAL BIASES USED IN THIS SOLUTION */
    WL_UPD(){
        UPD_value=wlupd_T=0.0;
    }
};
struct Sat_WL_UPD
{
    QVector< WL_UPD > GPS_UPD;
    QVector< WL_UPD > BDS_UPD;
    QVector< WL_UPD > GLONASS_UPD;
    QVector< WL_UPD > GALILEO_UPD;
};

class rinex_clk:public rinex_obs
{
public:
    rinex_clk();
    void releaseCLKAllData();
    void setClkFileNames(QStringList ClkFileNames);
    void getStaliteClk(int PRN, char SatType, double GPST, double *pCLKT, double *pClkVar, double *G_UPDs, file_dat *dat_fil);
private:
    void initClkVar();
    void readFileData2Vec(QStringList ClkFileNames);
    void get8Point(int PRN,char SatType,double *pCLKT,double *pCLK_Var,double *pGPST,double GPST);
    void lagrangeMethod(int PRN,char SatType,double GPST,double *pCLKT,double *pCLKTVar,double *G_UPDs,file_dat *dat_fil);
    void InitClkDatStruct();
public:
    QStringList m_ClkFileNames;
    int m_WeekClkOrder;/* save the number of cross-week files*/
    bool IsSigalClkFile;/*whether multiple files or a single file*/
    bool isReadClkAllData;/*whether to read the entire file data*/
    QVector< CLKData > m_allEpochClkData;
    bool isReadClkHead;/*whether to read header file*/
    QVector< Sat_WL_UPD > grg_UPD;
    QString m_ClkFileName;
    int lagrangeFact;/*simple lagrangeFact = 2 can be used to judge whether the CLK file is 30s or 5min 30s; 5 min lagrangeFact = 8*/
    /*variables used to optimize looking for clock differences prevent looking all the way from 0*/
    int m_lastGPSTimeFlag;/*save the first GPS time data obtained by get8Point last time*/
    double m_lastCmpGPSTime;/*save the last calculated GPST*/
    int int_clk_ar_flag;
//    int m_EndHeadNum;/*the first and last clock difference between the number of read blocks*/

private:
    bool ACCELERATE_CLK;
    CLKData epochClkData_ref;
    Sat_WL_UPD upd_WL;
    QString tempCLkLine;

};

#endif // RINEX_CLK_H
