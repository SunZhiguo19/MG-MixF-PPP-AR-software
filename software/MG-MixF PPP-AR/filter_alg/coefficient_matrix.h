#ifndef COEFFICIENT_MATRIX_H
#define COEFFICIENT_MATRIX_H
#include <Eigen/Dense>
#include "pos_algorith.h"
#include "pos_alg/mat_cal.h"
#include "inf_write/QRTWrite2File.h"
#include "AGNSS_APPData.h"

class coefficient_matrix
{
public:
    coefficient_matrix();
    void init_CoeffVar(corr_opt *opt_corr, para_trans *trans_para);
    void init_IF_matrix(int Avai_satN, MatrixXd &Coef_B, VectorXd &Obs_L, const corr_opt *opt_corr, para_trans *trans_para);
    void init_NOcombina_matrix(QVector< SatlitData > &Current_sat, MatrixXd &Coef_B, VectorXd &Obs_L, int Total_sat_fre, const corr_opt *opt_corr, para_trans *trans_para);
    void Obtaining_IF_equation(QVector< SatlitData > &Current_sat, double *m_ApproxRecPos, MatrixXd &mat_B, VectorXd &Vct_L,
                                 MatrixXd &mat_P, const corr_opt *opt_corr, para_trans *trans_para);
    void Obtaining_NoCombination_equation(QVector< SatlitData > &Current_sat, double *m_ApproxRecPos, MatrixXd &mat_B, VectorXd &Vct_L,
                                 MatrixXd &mat_P, int Total_sat_fre, const corr_opt *opt_corr, para_trans *trans_para);
    void change_IF_Paramatrix(QVector< SatlitData > &preEpoch, QVector< SatlitData > &current_Sat,
                                                  QVector< int >oldPrnFlag, int preEpochLen, QVector< int > &NewPrnFlag, const corr_opt *opt_corr, para_trans *trans_para);
    void change_NoCombination_Para(QVector< SatlitData > &preEpoch, QVector< SatlitData > &current_Sat,
                                                  QVector< int >oldPrnFlag, int preEpochLen, QVector< int > &NewPrnFlag, int Total_sat_fre, const corr_opt *opt_corr, para_trans *trans_para);
    void update_IF_Rk(QVector< SatlitData > &current_Sat, int B_len, const corr_opt *opt_corr, para_trans *trans_para);
    void update_NoCombination_Rk(QVector< SatlitData > &current_Sat, int B_len,const corr_opt *opt_corr, para_trans *trans_para);
    void update_helmet_IF_Rk(QVector< SatlitData > &current_Sat, int B_len,const corr_opt *opt_corr, para_trans *trans_para);
    void Calcu_CONV_PreB_CurB(MatrixXd &pre_B, MatrixXd &curr_B, QVector< int > &oldPrnFlag, QVector< int > &NewPrnFlag);
    bool isSatelliteChange(QVector< SatlitData > &preEpoch,QVector< SatlitData > &currEpoch, QVector< int > &oldPrnFlag,QVector< int > &NewPrnFlag);
    bool isSatelliteChange_mixed_fre(QVector< SatlitData > &preEpoch,QVector< SatlitData > &currEpoch, QVector< int > &oldPrnFlag,QVector< int > &NewPrnFlag);
    bool is_clock_ifb_ion_Change(const corr_opt *opt_corr,para_trans *trans_para, QVector< int > &oldPrnFlag,QVector< int > &NewPrnFlag);
    void change_mixed_fre_Combination(QVector< SatlitData > &preEpoch, QVector< SatlitData > &current_Sat,
                                                  QVector< int >oldPrnFlag, QVector< int > &NewPrnFlag, int Total_sat_fre, const corr_opt *opt_corr, para_trans *trans_para);
    void Obtaining_SPP_NoCombination_equation(QVector< SatlitData > &Current_sat, double *m_ApproxRecPos, MatrixXd &mat_B, VectorXd &Vct_L,
                                 MatrixXd &mat_P, int Total_sat_fre, const corr_opt *opt_corr, para_trans *trans_para);
    void Obtaining_SPP_IF_equation(QVector< SatlitData > &Current_sat, double *m_ApproxRecPos, MatrixXd &mat_B, VectorXd &Vct_L,
                                 MatrixXd &mat_P,const corr_opt *opt_corr,para_trans *trans_para);
    void Domain_of_doubt_NoCom(QVector< SatlitData > &preEpoch, QVector< SatlitData > &current_Sat, const corr_opt *opt_corr, para_trans *trans_para);
    void Domain_of_doubt_IF(QVector< SatlitData > &preEpoch, QVector< SatlitData > &current_Sat, const corr_opt *opt_corr, para_trans *trans_para);
    void Obtaining_WL_constraint_equation(QVector< SatlitData > &Current_sat, double *m_ApproxRecPos, MatrixXd &mat_B, VectorXd &Vct_L,
                                 MatrixXd &mat_P, int Total_sat_fre, const corr_opt *opt_corr, para_trans *trans_para);
    void update_WLNoCombination_Rk(QVector< SatlitData > &current_Sat, int B_len, para_trans *trans_para);
    void init_NO_WL_combina_matrix(QVector< SatlitData > &Current_sat,MatrixXd &Coef_B,VectorXd &Obs_L,
                                                   int Total_sat_fre,const corr_opt *opt_corr,para_trans *trans_para);
    void change_No_WL_Combination_Para(QVector< SatlitData > &preEpoch, QVector< SatlitData > &current_Sat,
                                                  QVector< int >oldPrnFlag,int preEpochLen,QVector< int > &NewPrnFlag,int Total_sat_fre,const corr_opt *opt_corr,para_trans *trans_para);
    void sort_order(int de_sortN,int *sortMaxEpo);


private:
    void inctrase_len(VectorXd &tempXk_1,MatrixXd &tempPk_1,MatrixXd &tempQk_1,int head_len);
    void reduce_len(VectorXd &tempXk_1,MatrixXd &tempPk_1,MatrixXd &tempQk_1,int head_len);
    void inctrase_rclk_len(VectorXd &tempXk_1,MatrixXd &tempPk_1,MatrixXd &tempQk_1,int head_len,int sys_error_select);
    void reduce_rclk_len(VectorXd &tempXk_1,MatrixXd &tempPk_1,MatrixXd &tempQk_1,int head_len,int sys_error_select);
public:

private:
    mat_cal m_mat_cal;
    QRTWrite2File write_qdebug;

};

#endif // COEFFICIENT_MATRIX_H
