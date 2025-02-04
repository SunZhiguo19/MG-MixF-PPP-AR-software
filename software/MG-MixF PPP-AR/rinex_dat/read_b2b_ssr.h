#ifndef READ_B2B_SSR_H
#define READ_B2B_SSR_H
#include "pos_algorith.h"
#include "rtcm_ssr.h"
class read_b2b_ssr:public rtcm_ssr
{
public:
    read_b2b_ssr();
    void getMult_b2b_SsrData(QVector< Ve_B2B_CLKData >&multB2B_CLKData,
                                           QVector< Ve_B2B_ORBData >&multB2B_ORBData,
                                           QVector< Ve_B2B_CBSData >&multB2B_CBSData,int epochNum);
    void getPPP_B2b_time(PPP_B2b_DAT *Obs_B2b_Data,int *ssrRead_CLK_L,
                                       int *ssrRead_ORB_L,int *ssrRead_CBS_L,QVector< Ve_B2B_CLKData > multB2B_CLKData,
                                       QVector< Ve_B2B_ORBData > multB2B_ORBData,QVector< Ve_B2B_CBSData > multB2B_CBSData,gtime_t Otime);
    void init_b2b_SsrVar();
    void set_b2b_Ssr_FileNames(QString SsrFileNames);
    bool isSSr_B2B_End();
    void close_B2B_File();

private:
    void getB2BSSRData(Ve_B2B_CLKData *B2B_CLKData, Ve_B2B_ORBData *B2B_ORBData, Ve_B2B_CBSData *B2B_CBSData, int *b2b_clk_flag, int *b2b_cbs_flag, int *b2b_orb_flag);
public:
    QFile m_read_b2b_SsrFileClass;/*read the.Ssr file class*/
    QString m_b2b_SsrFileName;
    bool frist_falg;
    bool m_IsClose_b2b_File;
private:
    /* ssr update intervals ------------------------------------------------------*/
    /*SSRData epochData;*/

    QString temp_b2b_SSRLine;
    QStringList m__b2b_SsrFileNames;

    bool IsSigal_b2b_File;/*whether multiple files or a single file*/
    bool isRead_b2b_Head;/*whether to read header file*/
    bool isRead_b2b_AllData;/*whether to read the entire file data*/

};

#endif // READ_B2B_SSR_H
