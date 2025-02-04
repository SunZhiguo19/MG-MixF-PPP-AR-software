#ifndef SGG_FCB_H
#define SGG_FCB_H

#include "pos_algorith.h"
#include "pos_alg/trans_epht.h"
#include "pos_alg/mat_cal.h"
#include "AGNSS_APPData.h"
struct FCBWlData
{/*FCB file data format*/
    int GPSWlWeek;
    gtime_t GPSWlTime;/*GPS week within seconds*/
    int WlFlag;/* Flag*/
    double WlFCB;
    double WlFCBvar;
    FCBWlData(){
        GPSWlWeek=WlFlag=0;
        WlFCB=WlFCBvar=0.0;
    }
};
struct FCBNlData
{/*FCB file data format*/
    int GPSNlWeek;
    gtime_t GPSNlTime;/*GPS week within seconds*/
    double NlFCB;
    double NlFCBvar;
    FCBNlData(){
        GPSNlWeek=0;
        NlFCBvar=NlFCB=0.0;
    }
};
struct FCBData
{/*FCB file data format*/
    int GPSWeek;
    gtime_t GPSTime;/*GPS week within seconds*/
    FCBWlData GPSDataWl[MAXPRNGPS];
    FCBWlData GLODataWl[MAXPRNGLO];
    FCBWlData BDSDataWl[MAXPRNBDS];
    FCBWlData GLEDataWl[MAXPRNGLE];
    FCBNlData GPSDataNl[MAXPRNGPS];
    FCBNlData GLODataNl[MAXPRNGLO];
    FCBNlData BDSDataNl[MAXPRNBDS];
    FCBNlData GLEDataNl[MAXPRNGLE];
};


class sgg_fcb:public trans_epht
{
public:
    sgg_fcb();
    void getStaliteFCBs(int PRN,char SatType,double *Fcbswl,double *Fcbsnl,double *NL_fcbs_std,gtime_t GPSOTime);
    void releasefcbAllData();
    void setFcbsFileNames(QStringList FCBsFileNames);
    QVector< FCBData > getFCBAllData();
private:
    void initFCBVar();
    void readFileData2Vec(QStringList FCBsFileNames);
    void SearchFCBsFile(int PRN,char SatType,double *Fcbswl,double *Fcbsnl,double *NL_fcbs_std,gtime_t GPSOTime);

public:
    QVector< FCBData > m_allEpochFcbData;
    QStringList m_FCBsFileNames;
    bool isReadFcbAllData;/*whether to read the entire file data*/
private:

    QString m_FCBsFileName;
    QString tempLine;
    bool IsSigalFile;/*whether multiple files or a single file*/
    bool isReadHead;/*whether to read header file*/

};

#endif // SGG_FCB_H
