#ifndef STAND_POS_H
#define STAND_POS_H
#include "pos_algorith.h"
#include "pos_alg/search_file.h"
#include "stochastic_model.h"
//#include "pos_alg/ionex.h"
//#include "filter_alg/srif_algorithm.h"
class stand_pos:public search_file,public stochastic_model
{
public:
    stand_pos();
    void SimpleSPP_model(QVector < SatlitData > &prevEpochSatlitData, QVector < SatlitData > &epochSatlitData,
                         double *spp_pos, gtime_t epht,  const corr_opt *opt_corr, para_trans *trans_para, file_dat *dat_fil);
    void IF_combination_fre(const SatlitData &IF_satlit,int *fre_0,int *fre_1,const corr_opt *opt_corr);
    void arrange(int epocha,QStringList SsrFileNameList);
    void saveResult2Class(VectorXd X, double *spp_vct, GPSPosTime epochTime, QVector< SatlitData > epochResultSatlitData,
                                     int epochNum, const corr_opt *opt_corr, para_trans *trans_para,  MatrixXd *P = NULL);
    void system_sate_cual(QVector< SatlitData > ephon_satellite,int *sys_N);
    void ecef2Pos(const double *r,double *pos);
    void rad2DMS(double rad, int &deg, double &min);
private:

public:
    bool m_ssrflag;

//    int m_minSatFlag;// the minimum number of satellites required
//    QString m_deleteSats;

    QString m_SSRFileName[10];// SSR file szg debug 0515
};

#endif // STAND_POS_H
