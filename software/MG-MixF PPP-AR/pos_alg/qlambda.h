#ifndef QLAMBDA_H
#define QLAMBDA_H
#include "pos_algorith.h"

#include "mat_cal.h"

#define LOOPMAX     10000           /* maximum count of search loop */

#define SGN(x)      ((x)<=0.0?-1.0:1.0)
#define ROUND_RTKLIB(x)    (floor((x)+0.5))
#define SWAP(x,y)   do {double tmp_; tmp_=x; x=y; y=tmp_;} while (0)


class qlambda
{
public:
    qlambda();
    int lambda(int n, int m, const double *a, const double *Q, double *F,
                      double *s);
    int QLamdaSearch(VectorXd ambfloat, MatrixXd Q_mat, MatrixXd &amb_fix, double *ratio, int out_fixamb_num = 4);
    void QLamda_ParSearch(VectorXd ambfloat, MatrixXd Q_mat, MatrixXd &amb_fix, double *ratio, int out_fixamb_num, int *par_flag);
    int lambda_par(int n, int m, const double *a, const double *Q, double *F,
                   double *s,double *Qbz,int row,double *z_z);
    void QLamdaSearch_boost_Par(VectorXd ambfloat, MatrixXd Q_mat, double *ratio, int out_fixamb_num, MatrixXd Qba_mat,VectorXd &stat_zz);
    int bootstrapping(double *dit_p, MatrixXd Q_mat);
    double Sur_Success_rate(MatrixXd Q_mat, int *P_s_n, double Ps);
    double normalCDF(double value){ return 0.5 * erfc(-value * M_SQRT1_2); }
    void sort_double(int de_sortN,double *sortMaxEpo);
private:

    int LD(int n, const double *Q, double *L, double *D);
    void gauss(int n, double *L, double *Z, int i, int j);
    void perm(int n, double *L, double *D, int j, double del, double *Z);
    void reduction(int n, double *L, double *D, double *Z);
    int search(int n, int m, const double *L, const double *D,
                      const double *zs, double *zn, double *s);
    int parsearch(int n, int m, const double *L, const double *D,
                      const double *zs, double *zn, double *s, int *nfixed,const double *Qzhat);
    int lambda_par_rud(int n, int m, const double *a, const double *Q, double *F,
                      double *s,int k);
    int parsearch_rud(int n, int m, const double *L, const double *D,
                      const double *zs, double *zn, double *s,int *nfixed,const double *Qzhat,int k);
    void fatalerr(const char *format, ...);
    double *mat(int n, int m);
    double *zeros(int n, int m);
    double *eye(int n);
    int *imat(int n, int m);
    void matcpy(double *A, const double *B, int n, int m);
    int matinv(double *A, int n);
    int ludcmp(double *A, int n, int *indx, double *d);
    void lubksb(const double *A, int n, const int *indx, double *b);
    int solve(const char *tr, const double *A, const double *Y, int n,
                     int m, double *X);

public:

private:
    mat_cal labmat_cal;
};

#endif // QLAMBDA_H
