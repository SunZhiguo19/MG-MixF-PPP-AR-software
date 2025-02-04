#ifndef IONEX_H
#define IONEX_H
#include "pos_algorith.h"
#include "mat_cal.h"
#include "trans_epht.h"
class ionex
{
public:
    ionex();
    int getindex(double value, const double *range);
    int nitem(const double *range);
    int dataindex(int i, int j, int k, const int *ndata);
    tec_t *addtec(const double *lats, const double *lons, const double *hgts,
                         double rb, nav_t *nav);
    void readionexdcb(FILE *fp, double *dcb, double *rms);
    void readtec(const char *file, nav_t *nav, int opt);
    int interptec(const tec_t *tec, int k, const double *posp, double *value,
                         double *rms);
    int iondelay(gtime_t time, const tec_t *tec, const double *pos,
                        const double *azel, int opt, double *delay, double *var);
    int iontec(gtime_t time, const nav_t *nav, const double *pos,
                      const double *azel, int opt, double *delay, double *var);
    int expath(const char *path, char *paths[], int nmax);
    double ionppp(const double *pos, const double *azel, double re,
                         double hion, double *posp);
    int readionexb(FILE *fp, const double *lats, const double *lons,
                          const double *hgts, double rb, double nexp, nav_t *nav);
    void combtec(nav_t *nav);
    double readionexh(FILE *fp, double *lats, double *lons, double *hgts,
                             double *rb, double *nexp, double *dcb, double *rms);
private:
    mat_cal m_mat_cal;
    trans_epht eph_T;
};

#endif // IONEX_H
