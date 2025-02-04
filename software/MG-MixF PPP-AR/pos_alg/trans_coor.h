#ifndef TRANS_COOR_H
#define TRANS_COOR_H

#include "pos_algorith.h"
#include "trans_epht.h"
#include "mat_cal.h"
#include "ionex.h"
#include "rinex_dat/read_antcorr.h"

class trans_coor:public ionex,public read_antcorr
{
public:
    trans_coor();
    double getPreEpochWindUp(QVector< SatlitData > &prevEpochSatlitData,int PRN,char SatType);
    double getWindUp(int Year,int Month,int Day,int Hours,int Minuts,double Seconds,
                                 double *StaPos,double *RecPos,double &phw,double *psunpos);
    double getWindUpFI(int Year,int Month,int Day,int Hours,int Minuts,double Seconds,
                                   double *StaPos,double *RecPos,double &phw,double *psunpos,double *StaVel,double yaw);
    double getWindUp_quater(double *StaPos, double *RecPos,double phw, double *StaVel, MatrixXd quater);
    double sbstropcorr(gtime_t time, const double *pos, const double *azel,
                              double *var);
    double prectrop(gtime_t time, const double *pos, const double *azel, double trop_pre, double *mf, double *var);
    double trop_saasmodel(const double *pos, const double *azel,double humi);
    int sbsioncorr(gtime_t time, const nav_t *nav, const double *pos,
                          const double *azel, double *delay, double *var);
    double ionmodel(gtime_t t, const double *ion, const double *pos,
                           const double *azel);
    double getUNB3mDelay(double *pBLH, double TDay, double E, double *mf, double *ZPD, double *tZHD);
    double getRelativty(char SatType, double *pSatXYZ,double *pRecXYZ,double *pSatdXYZ);
    double getSagnac(double X,double Y,double Z,double *approxRecvXYZ);
    void getSatEA(double *pXYZ,double *approxRecvXYZ,double *EA);
    void trop_delay(gtime_t time, double *pBLH, double TDay, double *E, double trop_pre,int trop_mode, double *mf = NULL, double *ZHD_s = NULL, double *var=NULL);
    void bd2_multipath(QVector<SatlitData> &Sats, const corr_opt *opt_corr);
private:
    void windupcorrFI(gtime_t time, const double *rs, const double *rr,
        double *phw,double *psunpos,double *StaVel,double yaw);
    void windupcorr(gtime_t time, const double *rs, const double *rr,	double *phw,double *psunpos = NULL);
    void getmet(double lat, double *met);
    double mapf(double el, double a, double b, double c);
    double tropmapf(gtime_t time, const double pos[], const double azel[],double *mapfw);
    double nmf(gtime_t time, const double pos[], const double azel[],double *mapfw);
    double ionmapf(const double *pos, const double *azel);
    void searchigp(gtime_t time, const double *pos, const sbsion_t *ion,
                          const sbsigp_t **igp, double *x, double *y);
    double varicorr(int give);
    VectorXd UNB3M(Vector3d &BLH, double DAYOYEAR, double ELEVRAD);
public:

private:
};

#endif // TRANS_COOR_H
