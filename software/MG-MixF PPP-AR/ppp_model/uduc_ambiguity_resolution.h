#ifndef UDUC_AMBIGUITY_RESOLUTION_H
#define UDUC_AMBIGUITY_RESOLUTION_H
#include <algorithm>
#include "pos_algorith.h"
#include "AGNSS_APPData.h"
#include "pos_alg/coordinate_trans.h"
#include "filter_alg/coefficient_matrix.h"
#include "pos_alg/qlambda.h"
#include "pos_alg/mat_cal.h"
#include "pos_alg/search_file.h"
#include "ambiguity_resolution.h"
class uduc_ambiguity_resolution
{
public:
    uduc_ambiguity_resolution();
    bool UDUC_WidelaneAR(SatlitData &oneSatlitData,int fre_num0,int fre_num1,int HMW_flag);
    void Wide_lane_uduc_smoothing(QVector< SatlitData > &prevEpochSatlitData,QVector< SatlitData > &epochSatlitData, para_trans *trans_para);
    int get_uduc_SD_SatWl(QVector< SatlitData > &epochSatlitData,int *MAX_EAprn,int *Par_sat_epo);

    void get_uduc_ref_Sat(QVector< SatlitData > &epochSatlitData,int *MAX_EAprn);

    bool UDUC_NarrowLaneAR(QVector< SatlitData > &epochSatlitDatanl, VectorXd &X, MatrixXd &P,
                                                      int *Par_sat_epo, para_trans *trans_para, const corr_opt *opt_corr);
    bool UDUC_ambiguity_search(QVector< SatlitData > &epochSatlitDataLa,VectorXd &X,MatrixXd &P,
                                           int &fix_flag,int *Par_sat_epo,para_trans *trans_para,const corr_opt *opt_corr);
    void UDUC_Fixed_cascading_ambiguity(QVector< SatlitData > &epochSatlitDatanl, VectorXd &X, MatrixXd &P,
                                                      double *fre_1_coeff, double *fre_2_coeff, int *frequency_1, int *frequency_2, int Total_sat_fre,
                                                                   VectorXd &Par_sat_epo, int &Par_lenth, MatrixXd &UDUC_B, int pos_flag,
                                                      MatrixXd &UDUC_SD_P, MatrixXd &UDUC_B_SD, MatrixXd &UDUC_UDM, VectorXd &UDUC_SD_X, para_trans *trans_para);
    void UDUC_mixed_fre_cascading_ambiguity_resolution(QVector< SatlitData > &epochSatlitDataLa,VectorXd &X,MatrixXd &P,int Total_sat_fre,
                                                       VectorXd &UDUC_SD_X,MatrixXd &UDUC_SD_P,int &fix_flag,para_trans *trans_para);
    void UDUC_parametric_solution(VectorXd &UDUC_X, MatrixXd &UDUC_P,
                                                          VectorXd &UDUC_X_b, MatrixXd &UDUC_P_b, MatrixXd &UDUC_P_ab, MatrixXd &Trans_M,
                                                   int Total_sat_fre, VectorXd &Par_sat_epo, MatrixXd &UDUC_B_SD);
    void UDUC_Fixed_partial_NL_ambiguity(QVector< SatlitData > &epochSatlitDatanl,
                                                      int *Par_sat_epo,MatrixXd &UDUC_SD_P,VectorXd &UDUC_SD_X,VectorXd &UDUC_SD_WLX);
    void UDUC_Fixed_partial_ambiguity(QVector< SatlitData > &epochSatlitDatanl,
                                                      int *Par_sat_epo, MatrixXd &UDUC_SD_P, VectorXd &UDUC_SD_X);
    bool UDUC_LAMBDA_search(QVector< SatlitData > &epochSatlitDataLa,MatrixXd &UDUC_SD_P,VectorXd &UDUC_SD_X,int *Par_sat_epo,
                                                       MatrixXd UDUC_SD_Pa,VectorXd UDUC_SD_Xa,para_trans *trans_para,const corr_opt *opt_corr);
    void UDUC_cascading_ambiguity_resolution(QVector< SatlitData > &epochSatlitDataLa, VectorXd &X, MatrixXd &P, int Total_sat_fre, VectorXd &UDUC_SD_X,
                                                                        int &fix_flag, para_trans *trans_para, const corr_opt *opt_corr);
    bool UDUC_LAMBDA_search_NL(QVector< SatlitData > &epochSatlitDataLa,MatrixXd &UDUC_SD_P,VectorXd &UDUC_SD_X,int *Par_sat_epo,
                                                       MatrixXd UDUC_SD_Pa,VectorXd UDUC_SD_Xa,VectorXd UDUC_SD_WLa,para_trans *trans_para,const corr_opt *opt_corr);
    void UDUC_HMW_fixed_WL(QVector< SatlitData > &epochSatlitDatanl, int *Par_sat_epo, VectorXd &SD_X, int ambiguity_flag);
    void UDUC_HMW_Fixed_cascading_ambiguity(QVector< SatlitData > &epochSatlitDatanl,
                                                      int *frequency_flag, int Total_sat_fre, VectorXd &Par_sat_epo, int &Par_lenth, MatrixXd &UDUC_B_SD, MatrixXd &UDUC_UDM, para_trans *trans_para);
private:
    void UDUC_mixed_BSSD(QVector< SatlitData > &epochSatlitDataLa, int Total_sat_fre,
                                                                        MatrixXd &UDUC_SD_H_t, para_trans *trans_para);
    void UDUC_mixed_WL_IFNL(QVector< SatlitData > &epochSatlitDataLa, int UDUC_SD_H_t_row, MatrixXd &UDUC_B_IF_WL,
                                                                        MatrixXd &UDUC_AMB_T, VectorXd &Amb_flag, para_trans *trans_para);
    void UDUC_mixed_matrixX_trans(QVector< SatlitData > &epochSatlitDataLa, int UDUC_SD_H_t_row,
                                                                        MatrixXd &UDUC_B_SDEWLa, MatrixXd &UDUC_B_SDEWLb, MatrixXd &UDUC_B_SDWLa, MatrixXd &UDUC_B_SDWLb,
                                                             MatrixXd &UDUC_B_SDNLa, MatrixXd &UDUC_B_SDNLb, para_trans *trans_para);
    bool UDUC_CARWAR_Wide_Lane_AR(VectorXd &X,MatrixXd &P,VectorXd &Fixed_WAR);
    bool UDUC_CAR_FixedWAR_narrow_Lane_AR(VectorXd &X,MatrixXd &P,VectorXd &Fixed_WAR);
    void UDUC_mixed_LAMBDA_cascade_AR(VectorXd &X, MatrixXd &P, VectorXd &Amb_flag, MatrixXd &UDUC_B_SDEWLa, MatrixXd &UDUC_B_SDEWLb, MatrixXd &UDUC_B_SDWLa, MatrixXd &UDUC_B_SDWLb,
                                                                 MatrixXd &UDUC_B_IF_WL, MatrixXd &UDUC_B_SDNLa, MatrixXd &UDUC_B_SDNLb, VectorXd &B_X, MatrixXd &BP_k, int &fix_flag);
private:
    int m_SYStem_satN[10];
    Ambiguity_resolution Ambi_R;
    mat_cal m_mat_cal;
    qlambda m_lambda;
    search_file sea_fil;
};

#endif // UDUC_AMBIGUITY_RESOLUTION_H
