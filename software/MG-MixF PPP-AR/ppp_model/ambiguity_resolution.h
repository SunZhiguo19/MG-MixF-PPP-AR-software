#ifndef AMBIGUITY_RESOLUTION_H
#define AMBIGUITY_RESOLUTION_H
#include "pos_algorith.h"
#include "AGNSS_APPData.h"
#include "pos_alg/coordinate_trans.h"
#include "filter_alg/coefficient_matrix.h"
#include "pos_alg/qlambda.h"
#include "pos_alg/mat_cal.h"
#include "pos_alg/search_file.h"
typedef struct _SmoothData
{//SmoothData file data format

    VectorXd  Fix_NL;
    VectorXd  Fix_a;
    VectorXd  sd_a;
    MatrixXd  Fix_Qa;
    VectorXd  Flo_a;
    VectorXd  Fix_b;
    MatrixXd  Fix_Qb;
    MatrixXd  Fix_Qab;

}SmoothData;

struct SD_dat
{


    double Sd_IFam;
    int Sd_flag;
    int PRNhf;
    SD_dat()
    {
        Sd_flag = PRNhf = 0;
        Sd_IFam = 0;
    }


};
typedef struct _SD_hold
{

//    int G_PRNhf[MAXPRNGPS] = {0};
    int fix_falg = 0;
    int Sd_maxEA_prn[5] = {0};
    SD_dat G_hold[MAXPRNGPS];
    SD_dat C_hold[MAXPRNBDS];
    SD_dat R_hold[MAXPRNGLO];
    SD_dat E_hold[MAXPRNGLE];

}SD_hold;
class Ambiguity_resolution
{
public:
    Ambiguity_resolution();
    int Float2Int(double SDWlint);
    bool Probability_Test(int &Smo_SD_WLint, double Smo_SD_WL, double std_dat, double lambda);
//    bool SD_AR_Lamda(QVector< SatlitData > &epochSatlitDataLa,SmoothData &SD_IFsmoot,MatrixXd &P,VectorXd &X,
//                     int &fix_flag,int Satfixflag,int *maxEASat_Prn);
    bool ambiguity_search(QVector< SatlitData > &epochSatlitDataLa, MatrixXd &P,
                                           SmoothData &SD_IFsmoot, int &fix_flag, int *Par_sat_epo, int *maxEASat_Prn, para_trans *trans_para);
    bool NarrowLaneAR(QVector< SatlitData > &epochSatlitDatanl, VectorXd &X, const corr_opt *opt_corr, para_trans *trans_para);
    int getSD_SatWl(QVector< SatlitData > &epochSatlitData, int *MAX_EAprn, int *Par_sat_epo);

    void Wide_lane_smoothing(QVector< SatlitData > &prevEpochSatlitData,QVector< SatlitData > &epochSatlitData, para_trans *trans_para);
    bool WidelaneAR(SatlitData &oneSatlitData, const corr_opt *opt_corr);
    void Standard_of_success_rate(QVector< SatlitData > &epochSatlitDataLa,int *Par_sat_epo,MatrixXd &La_SDP,double P_sr);
    bool unfixed_sat(int satellite_len, int *Par_sat_epo, para_trans *trans_para);
    bool post_resuduala(int satellite_len,int *Par_sat_epo, para_trans *trans_para);
    void angle_altitude_screen(QVector< SatlitData > &epochSatlitDataLa,int *Par_sat_epo);
    void Adop_screen(QVector< SatlitData > &epochSatlitDataLa,int *Par_sat_epo,MatrixXd &La_SDP);
    bool Par_sub_amb(QVector< SatlitData > &epochSatlitDataLa, SmoothData &SD_IFsmoot, MatrixXd &P, MatrixXd &La_SDP, MatrixXd &Qab, MatrixXd &Trans_SD,
                                           VectorXd &SD_GNSS_F, VectorXd &La_SDX, VectorXd &SD_nl_fcb, int *Par_sat_epo, int *maxEASat_Prn);
    void Boost_Parsub(QVector< SatlitData > &epochSatlitDataLa, int *Par_sat_epo, MatrixXd &La_SDP);
    void Refe_star_selec(QVector< SatlitData > &SatlitData_selec,double *maxEAsat,int *maxEAPRN);
    void Refe_star_selec_mixed(QVector< SatlitData > &SatlitData_selec,double *maxEAsat,int *maxEAPRN);
    void Diagonal_variance(QVector< SatlitData > &epochSatlitDataLa,int *Par_sat_NL_N,MatrixXd &AP_k);
    void Refe_sat_var_selec(QVector< SatlitData > &SatlitData_selec,int *minVarPRN,MatrixXd &P,int const_parm);
    void Refe_sat_succ_selec(QVector< SatlitData > &SatlitData_selec, int *max_suc_PRN, MatrixXd &P, int const_parm);
    void Refe_sat_ADOP_selec(QVector< SatlitData > &SatlitData_selec,int *max_suc_PRN,MatrixXd &P,int const_parm);
    bool par_BOOST_ambiguity_search(QVector< SatlitData > &epochSatlitDataLa,MatrixXd &P,
                                           SmoothData &SD_IFsmoot,int &fix_flag,int *Par_sat_epo,int *maxEASat_Prn,para_trans *trans_para,VectorXd &stat_zz);
    void descending_order(int de_sortN,int *sortMaxEpo);
    void GNSS_PAR_(QVector< SatlitData > &epochSatlitDataLa,int *Par_sat_epo,MatrixXd &La_SDP);
    void GNSS_PAR_angle(QVector< SatlitData > &epochSatlitDataLa,int *Par_sat_epo,MatrixXd &La_SDP);
    void weight_ordering(QVector< SatlitData > &epochSatlitDataLa,int *Par_sat_epo);
    void elevation_is_less_than_30(QVector< SatlitData > &epochSatlitDataLa,int *Par_sat_epo,bool &EA_minsat);
    void delet_bias_SSD_nl(QVector<SatlitData> &epochSatlitDataLa, int *Par_sat_epo, VectorXd UDUC_SD_X, double bias_nl);
private:

private:
    qlambda m_lambda;
    mat_cal m_mat_cal;
    search_file sea_fil;
public:
    SmoothData SmoothSD;//szg sd data
    SD_hold SD_ARholdNew;//szg debug
    int SDflag;
    int pos_ephon;
    int m_SYStem_satN[10];
    int maxEAepo[5] = {0};//szg 1230 debug
    int part_Maxprn[5]={0};//Maximum altitude angle satellite storage
private:
    VectorXd ADop_serceen;
};

#endif // AMBIGUITY_RESOLUTION_H
