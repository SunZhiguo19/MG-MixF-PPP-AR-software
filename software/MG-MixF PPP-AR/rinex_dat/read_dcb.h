#ifndef READ_DCB_H
#define READ_DCB_H

#include "pos_algorith.h"
#include "rinex_clk.h"
struct DCBData
{/*DCB file data format*/
    double P1_C1_DCB;
    double P1_C1_DCBVar;
    double P1_P2_DCB;
    double P1_P2_DCBVar;
    double P2_C2_DCB;
    double P2_C2_DCBVar;
    DCBData(){
        P1_C1_DCB=P1_C1_DCBVar=P1_P2_DCB=P1_P2_DCBVar=P2_C2_DCB=P2_C2_DCBVar=0.0;
    }
};
struct SatDCBData
{/*DCB file data format*/
    DCBData GPSdcbData[MAXPRNGPS];
    DCBData GLOdcbData[MAXPRNGLO];
    DCBData BDSdcbData[MAXPRNBDS];
    DCBData GLEdcbData[MAXPRNGLE];
};


class read_dcb :public rinex_clk
{
public:
    read_dcb();
    void getStaliteDCB(int PRN,char SatType,double *DCBswl,double *DCBVar,int DCBtype);
    void setDCBFileNames(QStringList DCBFileNames);
private:
    void SearchDCBFile(int PRN,char SatType,double *DCBs,double *DCBVar,int DCBtype);
    void readFileData2Vec(QStringList DCBFileNames);
    void initDCBVar();
public:

    SatDCBData epochDcbData;
    QStringList m_DCBFileNames;

    int DCBTypeflag;
private:
    QFile m_readDCBFileClass;/*read the.dcbs file class*/
    QString tempLine;
    bool IsSigalFile;/*whether multiple files or a single file*/
    bool isReadHead;/*whether to read header file*/
    bool isReadAllData;/*whether to read the entire file data*/

};

#endif // READ_DCB_H
