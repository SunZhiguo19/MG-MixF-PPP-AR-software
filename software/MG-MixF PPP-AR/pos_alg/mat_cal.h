#ifndef MAT_CAL_H
#define MAT_CAL_H
#include "pos_algorith.h"


class mat_cal
{
public:
    mat_cal();
    int  normv3(const double *a, double *b);
    void cross3(const double *a, const double *b, double *c);
    double norm(const double *a,int n);
    double dot(const double *a, const double *b, int n);
    void deq(const double *x, double *xdot, const double *acc);
    double prod(const double *d, int n);
    void copy_mat(int n, int m, int k, int l, int g, int h, const double *A, double *B);
    double var_urassr(int ura);
    void diff_vec(const double *a, const double *b, double *c,int n);
    void matmul(const char *tr, int n, int k, int m, double alpha,
                       const double *A, const double *B, double beta, double *C);
    double str2num(const char *s, int i, int n);
    double geodist(const double *rs, const double *rr, double *e);
    void RemoveColumn(MatrixXd& matrix, unsigned int colToRemove);
    void RemoveRow(MatrixXd& matrix, unsigned int rowToRemove);
    void Remove_allZeroRow(MatrixXd& matrix);
    void autoScrollTextEdit(QTextEdit *textEdit,QString &add_text);
private:

};

#endif // MAT_CAL_H
