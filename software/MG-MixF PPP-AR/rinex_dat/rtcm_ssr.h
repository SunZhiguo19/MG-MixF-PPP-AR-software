#ifndef RTCM_SSR_H
#define RTCM_SSR_H

#include "pos_algorith.h"
#include "sgg_fcb.h"

class rtcm_ssr:public sgg_fcb
{
public:
    rtcm_ssr();
    void getMultSsrData(QVector< VeSSRData >&multEpochData,int epochNum);
    void getObstime(GNSSephData *SSRData,int *ssrReadL,QVector< VeSSRData > multEpochData,gtime_t Otime);
    void getupdatessr(VeSSRData *updateSSRData,VeSSRData oneSSRData);
    void setSsrFileNames(QString SsrFileNames);
    void closeSsrFile(QString SsrFileNames);
    bool isSSrEnd();
    void closeFile();
    gtime_t ssr2time(const double *ep);
private:
    void initSsrVar();
    void getSSRData(VeSSRData *SSRepochData);

    int ReadORBIT(QString tempLine,int *num,VeSSRData *SsrEphochData);
    void readORbitData(QString tempLine,VeSSRData *SsrEphochData);
    int ReadCLOCK(QString tempLine,int *num,VeSSRData *SsrEphochData);
    void readCLOCKData(QString tempLine,VeSSRData *SsrEphochData);
    int ReadCODE_BIAS(QString tempLine,int *num,VeSSRData *SsrEphochData);
    void readCODE_BIASData(QString tempLine,VeSSRData *SsrEphochData);
    int ReadPHASE_BIAS(QString tempLine,int *num,VeSSRData *SsrEphochData);
    void readPHASE_BIASData(QString tempLine,VeSSRData *SsrEphochData);
    void Qlist_to_dat(QStringList list_line,int *num,int *Year,int *Month,int *Day,int *Hours,
                      int *Minutes,double *Seconds,int *ssrupT);
public:
    QFile m_readSsrFileClass;/*read the.Ssr file class*/
    QString m_SsrFileName;
    bool m_IsCloseFile;
private:
    /* ssr update intervals ------------------------------------------------------*/

    QVector< SSRData > m_allEpochSsrData;
    /*SSRData epochData;*/

    QString tempSSRLine;
    QStringList m_SsrFileNames;
    const double ssruptime[16]={
        1,2,5,10,15,30,60,120,240,300,600,900,1800,3600,7200,10800
    };
    bool IsSigalFile;/*whether multiple files or a single file*/
    bool isReadHead;/*whether to read header file*/
    bool isReadAllData;/*whether to read the entire file data*/

};

#endif // RTCM_SSR_H
