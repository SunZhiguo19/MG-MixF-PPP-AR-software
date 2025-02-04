#ifndef QUALITY_CONTROL_H
#define QUALITY_CONTROL_H
#include "pos_algorith.h"
#include "AGNSS_APPData.h"
#include "pos_alg/trans_epht.h"
//Single frequency Cycly slip
struct CyclySat
{
    double test_stati;//Test statistics
    int fre_num;//Frequency number
    CyclySat(){
        test_stati=0.0;
        fre_num=0;
    }
};
struct CyclySatsys
{
    CyclySat gps_cycly[MAXPRNGPS];//Test statistics
    CyclySat bds_cycly[MAXPRNBDS];//Test statistics
    CyclySat glo_cycly[MAXPRNGLO];//Test statistics
    CyclySat gle_cycly[MAXPRNGLE];//Test statistics
};
class quality_control
{
public:
    quality_control();
    void Avail_satellite_sele(QVector< SatlitData > &prevEpochSatlitData, QVector< SatlitData > &Current_sat, double eleAngle, QVector<SatlitData> &allBadSatlitData_Q, int Combination_mode, int *cyclyslip_satn);
    void Availa_SysFlag(QString Sys_str, VectorXd &SYSflg, int *SysNum);
    void system_num(QVector< SatlitData > &Current_sat);
    void GNSS_IFB_and_osbfre(QVector< SatlitData > epochSatlitData,para_trans *trans_para);
    int  fixed_WL_ambiguity_N(QVector< SatlitData > &Current_sat);
private:
    bool CycleSlip_IF(const SatlitData &Current_sat, const SatlitData &preSatlitData, QVector<SatlitData> &allBadSatlitData_Q);
    bool CycleSlip_UD(const SatlitData &Current_sat,const SatlitData &preSatlitData,QVector< SatlitData > &allBadSatlitData_Q);
    void Availa_SysNum(char Sys, int *SYSNUM, int *Sys_num, int PRN);
    void Calculate_curr_IFB_N(para_trans *trans_para);
public:
    int Sat_Sys_num;
    int Sat_Sys_flag[5];
private:
    trans_epht epht_T;
};

#endif // QUALITY_CONTROL_H
