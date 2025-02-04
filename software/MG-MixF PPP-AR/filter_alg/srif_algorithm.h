#ifndef SRIF_ALGORITHM_H
#define SRIF_ALGORITHM_H
#include "pos_algorith.h"
#include "kalman_filter.h"

class srif_algorithm:public kalman_filter
{
public:
    srif_algorithm();
    void Sequential_Least_Squares(MatrixXd &B_k, VectorXd &L_k, VectorXd &X_k, MatrixXd &P_k, MatrixXd &R_k,MatrixXd Qwk, const corr_opt *opt_corr, para_trans *trans_para);
    void pos_srif_calculating(QVector< SatlitData > &preEpoch, QVector< SatlitData > &current_Sat,
                                        double *m_ApproxRecPos, VectorXd &X, MatrixXd &P, int Total_sat_fre, const corr_opt *opt_corr, para_trans *trans_para);
    void pos_WL_srif_calculating(QVector< SatlitData > &preEpoch,QVector< SatlitData > &current_Sat,
                                        double *m_ApproxRecPos,VectorXd &X,MatrixXd &P,int Total_sat_fre,const corr_opt *opt_corr,para_trans *trans_para);
private:
    void filter_srif(QVector< SatlitData > &preEpoch, QVector< SatlitData > &current_Sat,
                                     int Total_sat_fre, const corr_opt *opt_corr, para_trans *trans_para);
    void filter_WL_srif(QVector< SatlitData > &preEpoch,QVector< SatlitData > &current_Sat,
                                     int Total_sat_fre,const corr_opt *opt_corr,para_trans *trans_para);
};

#endif // SRIF_ALGORITHM_H
