#ifndef KALMAN_FILTER_H
#define KALMAN_FILTER_H
#include "pos_algorith.h"
#include "coefficient_matrix.h"
#include "filter_alg/quality_control.h"
#include "inf_write/QRTWrite2File.h"

using namespace Eigen;

class kalman_filter:public coefficient_matrix,public quality_control,public QRTWrite2File
{
public:
    kalman_filter();
    void KalmanforStaticOu(MatrixXd Bk,VectorXd Lk,MatrixXd F,
                           MatrixXd Qwk,MatrixXd PBk,VectorXd &tXk_1,MatrixXd &tPk_1);
    void KalmanforStatic(MatrixXd Bk, VectorXd Lk, MatrixXd F, MatrixXd Qwk,
                                        MatrixXd Rk, VectorXd &tXk_1, MatrixXd &tPk_1,const corr_opt *opt_corr, para_trans *trans_para);
    void pos_calculating(QVector< SatlitData > &preEpoch, QVector< SatlitData > &current_Sat,
                                        double *m_ApproxRecPos, VectorXd &X, MatrixXd &P, int Total_sat_fre, const corr_opt *opt_corr, para_trans *trans_para);

    void Classical_gross_error_detection(QVector< SatlitData > &current_Sat, QVector< SatlitData > &CrossE_current_Sat, int *Total_sat_fre, const corr_opt *opt_corr, para_trans *trans_para);
    void helmert_cate(QVector< SatlitData > &current_Sat,MatrixXd Bk,VectorXd Lk,
                                     VectorXd &tXk_1,const corr_opt *opt_corr);
    void pos_WL_constraint_calculating(QVector< SatlitData > &preEpoch,QVector< SatlitData > &current_Sat,
                                        double *m_ApproxRecPos,VectorXd &X,MatrixXd &P,int Total_sat_fre,const corr_opt *opt_corr,para_trans *trans_para);
    void filter_WL_constraint_alogrithm(QVector< SatlitData > &preEpoch,QVector< SatlitData > &current_Sat,
                                        int Total_sat_fre,const corr_opt *opt_corr,para_trans *trans_para);
private:
    void filter_alogrithm(QVector< SatlitData > &preEpoch, QVector< SatlitData > &current_Sat, int Total_sat_fre, const corr_opt *opt_corr, para_trans *trans_para);

public:
    QVector< SatlitData > delet_BadSat;
    bool quality_control_flag;
//    bool quality_flag;
};

#endif // KALMAN_FILTER_H
