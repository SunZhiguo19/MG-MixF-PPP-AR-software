#include "qlambda.h"
qlambda::qlambda()
{

}

int qlambda::QLamdaSearch(VectorXd ambfloat, MatrixXd Q_mat, MatrixXd &amb_fix, double *ratio, int out_fixamb_num)
{
    int n = 0, m = 0;
    double *a = NULL, *Q = NULL, *F = NULL, *s = NULL;
    n = Q_mat.rows();
    m = out_fixamb_num;
    int info;
    float thresar = *ratio;
    a = (double *)malloc(sizeof(double) *n);
    Q = (double *)malloc(sizeof(double) *n * n);
    F = (double *)malloc(sizeof(double) *n * m);
    s = (double *)malloc(sizeof(double) *m);
    /*transation Matrix to double */
    for(int i = 0;i < n; i++)
        a[i] = ambfloat[i];
    for(int i = 0;i < n;i++)
        for(int j = 0;j < n;j++)
            Q[i * n + j] = Q_mat(i, j);
    /* use RTKLIB search(!(info=lambda(nb,2,y+na,Qb,b,s)))*/
    if(!(info=lambda(n, m, a, Q, F,s)))
    {
        *ratio=s[0]>0?(float)(s[1]/s[0]):0.0f;
        if (*ratio>999.9) *ratio=999.9f;
        if (s[0]<=0.0||s[1]/s[0]>=thresar)
        {
            amb_fix.resize(n, m);
            amb_fix.setZero();
            for(int i = 0;i < n;i++)
                for(int j = 0;j < m;j++)
                    amb_fix(i,j) = F[i + j * n];/*F[i * n + j] debug 1228*/
        }
    }
    free(a);free(Q);free(s);free(F);
    return info;
//    if(*ratio<2.1)
//    {
//        if(!(info=lambda_par(n, m, a, Q, F,s)))
//        {
//            *ratio=s[0]>0?(float)(s[1]/s[0]):0.0f;
//            if (*ratio>999.9) *ratio=999.9f;
//            if (s[0]<=0.0||s[1]/s[0]>=thresar)
//            {
//                amb_fix.resize(n, m);
//                amb_fix.setZero();
//                for(int i = 0;i < n;i++)
//                    for(int j = 0;j < m;j++)
//                        amb_fix(i,j) = F[i + j * n];/*F[i * n + j] debug 1228*/
//            }
//        }
//    }

}
void qlambda::QLamdaSearch_boost_Par(VectorXd ambfloat, MatrixXd Q_mat, double *ratio, int out_fixamb_num, MatrixXd Qba_mat,VectorXd &stat_zz)
{
    int n = 0, m = 0;
    double *a = NULL, *Q = NULL, *F = NULL, *s = NULL,*Qba = NULL,*z_z = NULL;
    n = Q_mat.rows();
    m = out_fixamb_num;
    int row = Qba_mat.rows();
    int info;
    float thresar = *ratio;
    a = (double *)malloc(sizeof(double) *n);
    Q = (double *)malloc(sizeof(double) *n * n);
    Qba = (double *)malloc(sizeof(double) *row * n);
    F = (double *)malloc(sizeof(double) *n * m);
    s = (double *)malloc(sizeof(double) *m);
    z_z = (double *)malloc(sizeof(double) *row);
    /*transation Matrix to double */
    for(int i = 0;i < n; i++)
        a[i] = ambfloat[i];
    for(int i = 0;i < n;i++)
        for(int j = 0;j < n;j++)
            Q[i * n + j] = Q_mat(i, j);
    for(int i = 0;i < row;i++)
        for(int j = 0;j < n;j++)
            Qba[i  * n+ j] = Qba_mat(i, j);
    /* use RTKLIB search(!(info=lambda(nb,2,y+na,Qb,b,s)))*/
    if(!(info=lambda_par(n,m,a,Q,F,s,Qba,row,z_z)))
    {
        *ratio=s[0]>0?(float)(s[1]/s[0]):0.0f;
        if (*ratio>999.9) *ratio=999.9f;
        if (s[0]<=0.0||s[1]/s[0]>=thresar)
        {
            stat_zz.resize(row);
            stat_zz.setZero();
            for(int i = 0;i < row;i++)
                stat_zz(i)=z_z[i];
        }
    }

}
void qlambda::QLamda_ParSearch(VectorXd ambfloat, MatrixXd Q_mat, MatrixXd &amb_fix, double *ratio, int out_fixamb_num,int *par_flag)
{
    int n = 0, m = 0;
    double *a = NULL, *Q = NULL, *F = NULL, *s = NULL;
    n = Q_mat.rows();
    m = out_fixamb_num;
    int info;
    float thresar = *ratio;
    a = (double *)malloc(sizeof(double) *n);
    Q = (double *)malloc(sizeof(double) *n * n);
    F = (double *)malloc(sizeof(double) *n * m);
    s = (double *)malloc(sizeof(double) *m);
    /*transation Matrix to double */
    for(int i = 0;i < n; i++)
        a[i] = ambfloat[i];
    for(int i = 0;i < n;i++)
        for(int j = 0;j < n;j++)
            Q[i * n + j] = Q_mat(i, j);
    /* use RTKLIB search(!(info=lambda(nb,2,y+na,Qb,b,s)))*/
    if(!(info=lambda(n, m, a, Q, F,s)))
    {
        *ratio=s[0]>0?(float)(s[1]/s[0]):0.0f;
        if (*ratio>999.9) *ratio=999.9f;
        if (s[0]<=0.0||s[1]/s[0]>=thresar)
        {
            amb_fix.resize(n, m);
            amb_fix.setZero();
            for(int i = 0;i < n;i++)
                for(int j = 0;j < m;j++)
                    amb_fix(i,j) = F[i + j * n];/*F[i * n + j] debug 1228*/
        }
    }
    if(*ratio<2.1)
    {
        for(int k=1;k<3;k++)
        {
            if(!(info=lambda_par_rud(n, m, a, Q, F,s,k)))
            {
                *ratio=s[0]>0?(float)(s[1]/s[0]):0.0f;
                if (*ratio>999.9) *ratio=999.9f;
                if (s[0]<=0.0||s[1]/s[0]>=thresar)
                {
                    *par_flag = 1;
                    amb_fix.resize(n, m);
                    amb_fix.setZero();
                    for(int i = 0;i < n;i++)
                        for(int j = 0;j < m;j++)
                            amb_fix(i,j) = F[i + j * n];/*F[i * n + j] debug 1228*/
                    return ;
                }
            }
        }
    }

}
/* lambda/mlambda integer least-square estimation ------------------------------
* integer least-square estimation. reduction is performed by lambda (ref.[1]),
* and search by mlambda (ref.[2]).
* args   : int    n      I  number of float parameters
*          int    m      I  number of fixed solutions
*          double *a     I  float parameters (n x 1)
*          double *Q     I  covariance matrix of float parameters (n x n)
*          double *F     O  fixed solutions (n x m)
*          double *s     O  sum of squared residulas of fixed solutions (1 x m)
* return : status (0:ok,other:error)
* notes  : matrix stored by column-major order (fortran convension)
*-----------------------------------------------------------------------------*/
int qlambda::lambda_par_rud(int n, int m, const double *a, const double *Q, double *F,
                  double *s,int k)
{
    int info;
    double *L,*D,*Z,*z,*E,*Qp,*Qp_,*Z_inv;

    if (n<=0||m<=0) return -1;
    L=zeros(n,n); D=mat(n,1); Z=eye(n); z=mat(n,1),E=mat(n,m);
    Qp=mat(n,n);Qp_=mat(n,n);Z_inv = mat(n,n);
    /* LD factorization */
    if (!(info=LD(n,Q,L,D))) {

        /* lambda reduction */
        reduction(n,L,D,Z);
//        matcpy(Z_inv,Z,n,n);
        for(int i=0;i<n;i++)    for(int j=0;j<n;j++)
            Z_inv[j + i*n] = Z[i + j*n];
        matinv(Z_inv,n);
        for(int i=0;i<n;i++)    for(int j=0;j<n;j++)
            Z_inv[i + j*n] = round(Z_inv[i + j*n]);
        MatrixXd Z_M;Z_M.resize(n,n);Z_M.setZero(n,n);
        for(int i=0;i<n;i++)    for(int j=0;j<n;j++)
            Z_M(i,j) = Z[i + j*n];
        labmat_cal.matmul("TN",n,n,n,1.0,Z,Q,0.0,Qp_);
        labmat_cal.matmul("NN",n,n,n,1.0,Qp_,Z,0.0,Qp);
        labmat_cal.matmul("TN",n,1,n,1.0,Z,a,0.0,z); /* z=Z'*a */

        /* mlambda search */
        int num_par = 0;
        if (!(info=parsearch_rud(n,m,L,D,z,E,s,&num_par,Qp,k))) {

            info=solve("T",Z,E,n,m,F); /* F=Z'\E */
        }
    }
    free(L); free(D); free(Z); free(z); free(E); free(Qp); free(Qp_);free(Z_inv);
    return info;
}
/* modified lambda (mlambda) parsearch (ref. [5]) -------------------------------*/
int qlambda::parsearch_rud(int n, int m, const double *L, const double *D,
                  const double *zs, double *zn, double *s,int *nfixed,const double *Qzhat,int k)
{
    int info;
    int Q_len = n-k;
    if(Q_len<5)    return -1;
    MatrixXd L_M,par_L_M;
    L_M.resize(n,n);par_L_M.resize(Q_len,Q_len);
    L_M.setZero(n,n);par_L_M.setZero(Q_len,Q_len);
    for(int ii=0;ii<n;ii++)
    {
        for(int jj=0;jj<n;jj++)
        {
            L_M(ii,jj) = L[ii+jj*n];//zn = [zfixed;par_E]
        }
    }
    for(int ii=0;ii<Q_len;ii++)
    {
        for(int jj=0;jj<Q_len;jj++)
        {
            par_L_M(ii,jj) = L[ii+k+(jj+k)*n];//zn = [zfixed;par_E]
        }
    }
    double *Qzpar = mat(Q_len,Q_len),*par_L = mat(Q_len,Q_len),*par_D = mat(Q_len,1),
            *par_z=mat(Q_len,1),*par_E=mat(Q_len,m),*par_z_head=mat(k,1);
    labmat_cal.copy_mat(n,n,k,k,Q_len,Q_len,L,par_L);//
    labmat_cal.copy_mat(n,1,k,0,Q_len,1,D,par_D);
    labmat_cal.copy_mat(n,1,k,0,Q_len,1,zs,par_z);
    labmat_cal.copy_mat(n,m,k,0,Q_len,m,zn,par_E);
    labmat_cal.copy_mat(n,1,0,0,k,1,zs,par_z_head);
    /* mlambda search */
    if(!(info=search(Q_len,m,par_L,par_D,par_z,par_E,s)))
    {
        labmat_cal.copy_mat(n,n,k,k,Q_len,Q_len,Qzhat,Qzpar);
//            labmat_cal.copy_mat(n,1,Q_len,0,1,0,zs,zpar);
        double *QP = mat(k,Q_len),*Q_p = mat(k,Q_len),*Qzpar_inv = mat(Q_len,Q_len);
        matcpy(Qzpar_inv, Qzpar,Q_len,Q_len);
        labmat_cal.copy_mat(n,n,0,k,k,Q_len,Qzhat,Q_p);
        double *zfixed = mat(k,1);
        matinv(Qzpar_inv,Q_len);
        labmat_cal.matmul("NN",k,Q_len,Q_len,1.0,Q_p,Qzpar_inv,0.0,QP);
        for(int ii=0;ii<m;ii++)
        {
            double *zpar_lie =mat(Q_len,1),*zpar_l =mat(Q_len,1),*zpar_Qp=mat(k,1);
            labmat_cal.copy_mat(Q_len,m,0,ii,Q_len,1,par_E,zpar_lie);//zpar_lie(k:end) = par_E(:,ii)
            labmat_cal.diff_vec(par_z, zpar_lie,zpar_l,Q_len);//zpar_l(k:end) = par_z(k:end) - zpar_lie(k:end)
            labmat_cal.matmul("NN",k,1,Q_len,1.0,QP,zpar_l,0.0,zpar_Qp);//zpar_Qp(1:k) = QP(1:k,k:end)*zpar_l(k:end)
            labmat_cal.diff_vec(par_z_head, zpar_Qp,zfixed,k);//zfixed(:,ii)=par_z_head(1:k)-zpar_Qp(1:k)
            for(int jj=0;jj<k;jj++) zn[jj+ii*n] = zfixed[jj];
            free(zpar_lie); free(zpar_l); free(zpar_Qp);
        }
        for(int ii=0;ii<Q_len;ii++)
        {
            for(int jj=0;jj<m;jj++)
            {
                zn[ii+k+jj*n] = par_E[ii+jj*Q_len];//zn = [zfixed;par_E]
            }
        }
//        if(Q_len>6)
//        {
//            labmat_cal.matmul("NN",k,Q_len,Q_len,1.0,Q_p,Qzpar_inv,0.0,QP);
//            for(int ii=0;ii<m;ii++)
//            {
//                double *zpar_lie =mat(Q_len,1),*zpar_l =mat(Q_len,1),*zpar_Qp=mat(k,1);
//                labmat_cal.copy_mat(Q_len,m,0,ii,Q_len,1,par_E,zpar_lie);//zpar_lie(k:end) = par_E(:,ii)
//                labmat_cal.diff_vec(par_z, zpar_lie,zpar_l,Q_len);//zpar_l(k:end) = par_z(k:end) - zpar_lie(k:end)
//                labmat_cal.matmul("NN",k,1,Q_len,1.0,QP,zpar_l,0.0,zpar_Qp);//zpar_Qp(1:k) = QP(1:k,k:end)*zpar_l(k:end)
//                labmat_cal.diff_vec(par_z_head, zpar_Qp,zfixed,k);//zfixed(:,ii)=par_z_head(1:k)-zpar_Qp(1:k)
//                for(int jj=0;jj<k;jj++) zn[jj+ii*n] = zfixed[jj];
//                free(zpar_lie); free(zpar_l); free(zpar_Qp);
//            }
//            for(int ii=0;ii<Q_len;ii++)
//            {
//                for(int jj=0;jj<m;jj++)
//                {
//                    zn[ii+k+jj*n] = par_E[ii+jj*Q_len];//zn = [zfixed;par_E]
//                }
//            }

//        }
//        else
//        {
//            for (int i=0;i<Q_len;i++) for (int j=0;j<m;j++) {
//                zn[i+k+j*n] = par_E[i+j*Q_len];
//            }
//            for (int i=0;i<k;i++) for (int j=0;j<m;j++) {
//                zn[i+j*n] = zs[i];
//            }
//        }
        free(QP); free(Q_p); free(Qzpar_inv);
        *nfixed =  Q_len;
    }
    free(Qzpar); free(par_L); free(par_D);free(par_z); free(par_E); free(par_z_head);
    return 0;
}
/* lambda/mlambda integer least-square estimation ------------------------------
* integer least-square estimation. reduction is performed by lambda (ref.[1]),
* and search by mlambda (ref.[2]).
* args   : int    n      I  number of float parameters
*          int    m      I  number of fixed solutions
*          double *a     I  float parameters (n x 1)
*          double *Q     I  covariance matrix of float parameters (n x n)
*          double *F     O  fixed solutions (n x m)
*          double *s     O  sum of squared residulas of fixed solutions (1 x m)
* return : status (0:ok,other:error)
* notes  : matrix stored by column-major order (fortran convension)
*-----------------------------------------------------------------------------*/
int qlambda::lambda(int n, int m, const double *a, const double *Q, double *F,
                  double *s)
{
    int info;
    double *L,*D,*Z,*z,*E;

    if (n<=0||m<=0) return -1;
    L=zeros(n,n); D=mat(n,1); Z=eye(n); z=mat(n,1),E=mat(n,m);

    /* LD factorization */
    if (!(info=LD(n,Q,L,D))) {

        /* lambda reduction */
        reduction(n,L,D,Z);
        labmat_cal.matmul("TN",n,1,n,1.0,Z,a,0.0,z); /* z=Z'*a */

        /* mlambda search */
        if (!(info=search(n,m,L,D,z,E,s))) {

            info=solve("T",Z,E,n,m,F); /* F=Z'\E */
        }
    }
    free(L); free(D); free(Z); free(z); free(E);
    return info;
}
/* lambda/mlambda integer least-square estimation ------------------------------
* integer least-square estimation. reduction is performed by lambda (ref.[1]),
* and search by mlambda (ref.[2]).
* args   : int    n      I  number of float parameters
*          int    m      I  number of fixed solutions
*          double *a     I  float parameters (n x 1)
*          double *Q     I  covariance matrix of float parameters (n x n)
*          double *F     O  fixed solutions (n x m)
*          double *s     O  sum of squared residulas of fixed solutions (1 x m)
* return : status (0:ok,other:error)
* notes  : matrix stored by column-major order (fortran convension)
*-----------------------------------------------------------------------------*/
int qlambda::lambda_par(int n, int m, const double *a, const double *Q, double *F,
                  double *s,double *Qbz,int row,double *z_z)
{
    int info;
    double *L,*D,*Z,*z,*E,*Qp,*Qp_,*Qbz_,*Qp_inv,*Zt_inv;

    if (n<=0||m<=0) return -1;
    L=zeros(n,n); D=mat(n,1); Z=eye(n); z=mat(n,1),E=mat(n,m);
    Qp=mat(n,n);Qp_inv=mat(n,n);Qp_=mat(n,n);Qbz_=mat(row,n);Zt_inv=mat(n,n);
    /* LD factorization */
    if (!(info=LD(n,Q,L,D))) {

        /* lambda reduction */
        reduction(n,L,D,Z);
        labmat_cal.matmul("TN",n,n,n,1.0,Z,Q,0.0,Qp_);
        labmat_cal.matmul("NN",n,n,n,1.0,Qp_,Z,0.0,Qp);
        labmat_cal.matmul("TN",n,1,n,1.0,Z,a,0.0,z); /* z=Z'*a */
        matcpy(Zt_inv, Z,n,n);
        matinv(Zt_inv,n);
        labmat_cal.matmul("NN",row,n,n,1.0,Qbz,Z,0.0,Qbz_); /* z=Z'*a */
        /* mlambda search */
        int num_par = 0;
        if (!(info=parsearch(n,m,L,D,z,E,s,&num_par,Qp))) {
            matcpy(Qp_inv, Qp,n,n);
            matinv(Qp_inv,n);
            double *zpar_lie =mat(n,1),*zpar_l =mat(n,1);
            labmat_cal.copy_mat(n,1,0,0,n,1,E,zpar_lie);//zpar_lie(k:end) = par_E(:,ii)
            labmat_cal.diff_vec(zpar_lie,z ,zpar_l,n);//zpar_l(k:end) = par_z(k:end) - zpar_lie(k:end)
            labmat_cal.matmul("NN",row,n,n,1.0,Qbz_,Qp_inv,0.0,Qbz);
            labmat_cal.matmul("NN",row,1,n,1.0,Qbz,zpar_l,0.0,z_z);//zpar_Qp(1:k) = QP(1:k,k:end)*zpar_l(k:end)

            free(zpar_lie); free(zpar_l);

//            info=solve("T",Z,E,n,m,F); /* F=Z'\E */
        }
    }
    free(L); free(D); free(Z); free(z); free(E); free(Qp); free(Qp_);free(Qbz_);free(Qp_inv);free(Zt_inv);
    return info;
}
int qlambda::bootstrapping(double *dit_p, MatrixXd Q_mat)
{
    int info;
    double *L,*D;
    int n  = Q_mat.cols();
    double *Q=NULL;
    Q = (double *)malloc(sizeof(double) *n * n);

    /*transation Matrix to double */
    for(int i = 0;i < n;i++)
        for(int j = 0;j < n;j++)
            Q[i * n + j] = Q_mat(i, j);
    if (n<=0) return -1;
    L=zeros(n,n); D=mat(n,1);
    if (!(info=LD(n,Q,L,D))){
        double cdf_p[n] = {0},cdf_d[n] = {0};
        for(int ii = 0;ii <n;ii++)
        {
            cdf_d[ii] = 1.0/(2*sqrt(D[ii]));
            cdf_p[ii] = normalCDF(cdf_d[ii]);
            dit_p[ii] = 2*cdf_p[ii] - 1;
        }

        free(L);free(D);
    }
    free(Q);
    return info;
}
double qlambda::Sur_Success_rate(MatrixXd Q_mat,int *P_s_n,double Ps)
{
    int info;
    double *L,*D;
    int n  = Q_mat.cols();
    double *Q=NULL;
    double success_rate=1;
    double SRC_Ps=1;
    double debug_SRC=0;
    double dit_p[n];
    Q = (double *)malloc(sizeof(double) *n * n);

    /*transation Matrix to double */
    for(int i = 0;i < n;i++)
        for(int j = 0;j < n;j++)
            Q[i * n + j] = Q_mat(i, j);
    if (n<=0) return -1;
    L=zeros(n,n); D=mat(n,1);
    if (!(info=LD(n,Q,L,D))){
        double cdf_p[n] = {0},cdf_d[n] = {0};
        for(int ii = 0;ii <n;ii++)
        {
            cdf_d[ii] = 1.0/(2*sqrt(D[ii]));
            cdf_p[ii] = normalCDF(cdf_d[ii]);
            dit_p[ii] = 2*cdf_p[ii] - 1;
        }
        for(int ii = 0;ii <n;ii++)
            success_rate = success_rate*dit_p[ii];
        for(int ii = 0;ii <n;ii++)
            debug_SRC = debug_SRC+dit_p[ii];
        free(L);free(D);
        sort_double(n,dit_p);
        for(int ii = 0;ii <n;ii++)
        {
            SRC_Ps = SRC_Ps*dit_p[ii];
            if(SRC_Ps<Ps)
            {
                *P_s_n=ii;
                break;
            }
        }
    }
    free(Q);
    return success_rate;
}
/* LD factorization (Q=L'*diag(D)*L) -----------------------------------------*/
int qlambda::LD(int n, const double *Q, double *L, double *D)
{
    int i,j,k,info=0;
    double a,*A=mat(n,n);

    memcpy(A,Q,sizeof(double)*n*n);
    for (i=n-1;i>=0;i--) {
        if ((D[i]=A[i+i*n])<=0.0) {info=-1; break;}
        a=sqrt(D[i]);
        for (j=0;j<=i;j++) L[i+j*n]=A[i+j*n]/a;
        for (j=0;j<=i-1;j++) for (k=0;k<=j;k++) A[j+k*n]-=L[i+k*n]*L[i+j*n];
        for (j=0;j<=i;j++) L[i+j*n]/=L[i+i*n];
    }
    free(A);
    if (info) fprintf(stderr,"%s : LD factorization error\n",__FILE__);
    return info;
}

/* integer gauss transformation -------------------------------------Q[i+i*n]---------*/
void qlambda::gauss(int n, double *L, double *Z, int i, int j)
{
    int k,mu;

    if ((mu=(int)ROUND_RTKLIB(L[i+j*n]))!=0) {
        for (k=i;k<n;k++) L[k+n*j]-=(double)mu*L[k+i*n];
        for (k=0;k<n;k++) Z[k+n*j]-=(double)mu*Z[k+i*n];
    }
}

/* permutations --------------------------------------------------------------*/
void qlambda::perm(int n, double *L, double *D, int j, double del, double *Z)
{
    int k;
    double eta,lam,a0,a1;

    eta=D[j]/del;
    lam=D[j+1]*L[j+1+j*n]/del;
    D[j]=eta*D[j+1]; D[j+1]=del;
    for (k=0;k<=j-1;k++) {
        a0=L[j+k*n]; a1=L[j+1+k*n];
        L[j+k*n]=-L[j+1+j*n]*a0+a1;
        L[j+1+k*n]=eta*a0+lam*a1;
    }
    L[j+1+j*n]=lam;
    for (k=j+2;k<n;k++) SWAP(L[k+j*n],L[k+(j+1)*n]);
    for (k=0;k<n;k++) SWAP(Z[k+j*n],Z[k+(j+1)*n]);
}

/* lambda reduction (z=Z'*a, Qz=Z'*Q*Z=L'*diag(D)*L) (ref.[1]) ---------------*/
void qlambda::reduction(int n, double *L, double *D, double *Z)
{
    int i,j,k;
    double del;

    j=n-2; k=n-2;
    while (j>=0) {
        if (j<=k) for (i=j+1;i<n;i++) gauss(n,L,Z,i,j);
        del=D[j]+L[j+1+j*n]*L[j+1+j*n]*D[j+1];
        if (del+1E-6<D[j+1]) { /* compared considering numerical error */
            perm(n,L,D,j,del,Z);
            k=j; j=n-2;
        }
        else j--;
    }
}

/* modified lambda (mlambda) search (ref. [2]) -------------------------------*/
int qlambda::search(int n, int m, const double *L, const double *D,
                  const double *zs, double *zn, double *s)
{
    int i,j,k,c,nn=0,imax=0;
    double newdist,maxdist=1E99,y;
    double *S=zeros(n,n),*dist=mat(n,1),*zb=mat(n,1),*z=mat(n,1),*step=mat(n,1);

    k=n-1; dist[k]=0.0;
    zb[k]=zs[k];
    z[k]=ROUND_RTKLIB(zb[k]); y=zb[k]-z[k]; step[k]=SGN(y);
    for (c=0;c<LOOPMAX;c++) {
        newdist=dist[k]+y*y/D[k];
        if (newdist<maxdist) {
            if (k!=0) {
                dist[--k]=newdist;
                for (i=0;i<=k;i++)
                    S[k+i*n]=S[k+1+i*n]+(z[k+1]-zb[k+1])*L[k+1+i*n];
                zb[k]=zs[k]+S[k+k*n];
                z[k]=ROUND_RTKLIB(zb[k]); y=zb[k]-z[k]; step[k]=SGN(y);
            }
            else {
                if (nn<m) {
                    if (nn==0||newdist>s[imax]) imax=nn;
                    for (i=0;i<n;i++) zn[i+nn*n]=z[i];
                    s[nn++]=newdist;
                }
                else {
                    if (newdist<s[imax]) {
                        for (i=0;i<n;i++) zn[i+imax*n]=z[i];
                        s[imax]=newdist;
                        for (i=imax=0;i<m;i++) if (s[imax]<s[i]) imax=i;
                    }
                    maxdist=s[imax];
                }
                z[0]+=step[0]; y=zb[0]-z[0]; step[0]=-step[0]-SGN(step[0]);
            }
        }
        else {
            if (k==n-1) break;
            else {
                k++;
                z[k]+=step[k]; y=zb[k]-z[k]; step[k]=-step[k]-SGN(step[k]);
            }
        }
    }
    for (i=0;i<m-1;i++) { /* sort by s */
        for (j=i+1;j<m;j++) {
            if (s[i]<s[j]) continue;
            SWAP(s[i],s[j]);
            for (k=0;k<n;k++) SWAP(zn[k+i*n],zn[k+j*n]);
        }
    }
    free(S); free(dist); free(zb); free(z); free(step);

    if (c>=LOOPMAX) {
        fprintf(stderr,"%s : search loop count overflow\n",__FILE__);
        return -1;
    }
    return 0;
}
/* modified lambda (mlambda) parsearch (ref. [5]) -------------------------------*/
int qlambda::parsearch(int n, int m, const double *L, const double *D,
                  const double *zs, double *zn, double *s,int *nfixed,const double *Qzhat)
{
    int k,info;
    double *dit_p;
    double Ps = 0,P0=0.995;k=0;
    dit_p =mat(n-k,1);
    double cdf_p[n] = {0},cdf_d[n] = {0};
    for(int ii = k;ii <n;ii++)
    {
        cdf_d[ii] = 1.0/(2*sqrt(D[ii]));
        cdf_p[ii] = normalCDF(cdf_d[ii]);
        dit_p[ii-k] = 2*cdf_p[ii] - 1;
    }
    Ps = labmat_cal.prod(dit_p,n-k);
    free(dit_p);
    while(Ps<P0&&k<n)
    {
        k++;
        int dit_pn = n-k;
        dit_p =mat(dit_pn,1);
        for(int ii = k;ii <n;ii++)  dit_p[ii-k] = 2*normalCDF(1.0/(2*sqrt(D[ii]))) - 1;
        Ps = labmat_cal.prod(dit_p,dit_pn);
        free(dit_p);
    }
    if(Ps>P0&&n>k+3)
    {
        int Q_len = n-k;
        double *Qzpar = mat(Q_len,Q_len),*par_L = mat(Q_len,Q_len),*par_D = mat(Q_len,1),
                *par_z=mat(Q_len,1),*par_E=mat(Q_len,m),*par_z_head=mat(k,1);
        labmat_cal.copy_mat(n,n,k,k,Q_len,Q_len,L,par_L);//
        labmat_cal.copy_mat(n,1,k,0,Q_len,1,D,par_D);
        labmat_cal.copy_mat(n,1,k,0,Q_len,1,zs,par_z);
//        labmat_cal.copy_mat(n,m,k,0,Q_len,m,zn,par_E);
        labmat_cal.copy_mat(n,1,0,0,k,1,zs,par_z_head);
        /* mlambda search */
        if(!(info=search(Q_len,m,par_L,par_D,par_z,par_E,s)))
        {
            labmat_cal.copy_mat(n,n,k,k,Q_len,Q_len,Qzhat,Qzpar);
//            labmat_cal.copy_mat(n,1,Q_len,0,1,0,zs,zpar);
            double *QP = mat(k,Q_len),*Q_p = mat(k,Q_len),*Qzpar_inv = mat(Q_len,Q_len);
            matcpy(Qzpar_inv, Qzpar,Q_len,Q_len);
            labmat_cal.copy_mat(n,n,0,k,k,Q_len,Qzhat,Q_p);
            double *zfixed = mat(k,1);
            matinv(Qzpar_inv,Q_len);
            if(Q_len>60)
            {
                labmat_cal.matmul("NN",k,Q_len,Q_len,1.0,Q_p,Qzpar_inv,0.0,QP);
                for(int ii=0;ii<m;ii++)
                {
                    double *zpar_lie =mat(Q_len,1),*zpar_l =mat(Q_len,1),*zpar_Qp=mat(k,1);
                    labmat_cal.copy_mat(Q_len,m,0,ii,Q_len,1,par_E,zpar_lie);//zpar_lie(k:end) = par_E(:,ii)
                    labmat_cal.diff_vec(par_z, zpar_lie,zpar_l,Q_len);//zpar_l(k:end) = par_z(k:end) - zpar_lie(k:end)
                    labmat_cal.matmul("NN",k,1,Q_len,1.0,QP,zpar_l,0.0,zpar_Qp);//zpar_Qp(1:k) = QP(1:k,k:end)*zpar_l(k:end)
                    labmat_cal.diff_vec(par_z_head, zpar_Qp,zfixed,k);//zfixed(:,ii)=par_z_head(1:k)-zpar_Qp(1:k)
                    for(int jj=0;jj<k;jj++) zn[jj+ii*n] = zfixed[jj];
                    free(zpar_lie); free(zpar_l); free(zpar_Qp);
                }
                for(int ii=0;ii<Q_len;ii++)
                {
                    for(int jj=0;jj<m;jj++)
                    {
                        zn[ii+k+jj*n] = par_E[ii+jj*Q_len];//zn = [zfixed;par_E]
                    }
                }

            }
            else
            {
                for (int i=0;i<Q_len;i++) for (int j=0;j<m;j++) {
                    zn[i+k+j*n] = par_E[i+j*Q_len];
                }
                for (int i=0;i<k;i++) for (int j=0;j<m;j++) {
                    zn[i+j*n] = zs[i+j*n];
                }
            }
            free(QP); free(Q_p); free(Qzpar_inv);
            *nfixed =  Q_len;
        }
        free(Qzpar); free(par_L); free(par_D);free(par_z); free(par_E); free(par_z_head);

    }
    else
        return -1;
    return 0;
}
// ***************other fuctions***********************

/* fatal error ---------------------------------------------------------------*/
void qlambda::fatalerr(const char *format, ...)
{
    va_list ap;
    va_start(ap,format); vfprintf(stderr,format,ap); va_end(ap);
    exit(-9);
}

/* new matrix ------------------------------------------------------------------
* allocate memory of matrix
* args   : int    n,m       I   number of rows and columns of matrix
* return : matrix pointer (if n<=0 or m<=0, return NULL)
*-----------------------------------------------------------------------------*/
double *qlambda::mat(int n, int m)
{
    double *p;

    if (n<=0||m<=0) return NULL;
    if (!(p=(double *)malloc(sizeof(double)*n*m))) {
        fatalerr("matrix memory allocation error: n=%d,m=%d\n",n,m);
    }
    return p;
}

/* zero matrix -----------------------------------------------------------------
* generate new zero matrix
* args   : int    n,m       I   number of rows and columns of matrix
* return : matrix pointer (if n<=0 or m<=0, return NULL)
*-----------------------------------------------------------------------------*/
double *qlambda::zeros(int n, int m)
{
    double *p;

#if NOCALLOC
    if ((p=mat(n,m))) for (n=n*m-1;n>=0;n--) p[n]=0.0;
#else
    if (n<=0||m<=0) return NULL;
    if (!(p=(double *)calloc(sizeof(double),n*m))) {
        fatalerr("matrix memory allocation error: n=%d,m=%d\n",n,m);
    }
#endif
    return p;
}

/* identity matrix -------------------------------------------------------------
* generate new identity matrix
* args   : int    n         I   number of rows and columns of matrix
* return : matrix pointer (if n<=0, return NULL)
*-----------------------------------------------------------------------------*/
double *qlambda::eye(int n)
{
    double *p;
    int i;

    if ((p=zeros(n,n))) for (i=0;i<n;i++) p[i+i*n]=1.0;
    return p;
}

/* new integer matrix ----------------------------------------------------------
* allocate memory of integer matrix
* args   : int    n,m       I   number of rows and columns of matrix
* return : matrix pointer (if n<=0 or m<=0, return NULL)
*-----------------------------------------------------------------------------*/
int *qlambda::imat(int n, int m)
{
    int *p;

    if (n<=0||m<=0) return NULL;
    if (!(p=(int *)malloc(sizeof(int)*n*m))) {
        fatalerr("integer matrix memory allocation error: n=%d,m=%d\n",n,m);
    }
    return p;
}

/* copy matrix -----------------------------------------------------------------
* copy matrix
* args   : double *A        O   destination matrix A (n x m)
*          double *B        I   source matrix B (n x m)
*          int    n,m       I   number of rows and columns of matrix
* return : none
*-----------------------------------------------------------------------------*/
void qlambda::matcpy(double *A, const double *B, int n, int m)
{
    memcpy(A,B,sizeof(double)*n*m);
}


/* inverse of matrix ---------------------------------------------------------*/
int qlambda::matinv(double *A, int n)
{
    double d,*B;
    int i,j,*indx;

    indx=imat(n,1); B=mat(n,n); matcpy(B,A,n,n);
    if (ludcmp(B,n,indx,&d)) {free(indx); free(B); return -1;}
    for (j=0;j<n;j++) {
        for (i=0;i<n;i++) A[i+j*n]=0.0; A[j+j*n]=1.0;
        lubksb(B,n,indx,A+j*n);
    }
    free(indx); free(B);
    return 0;
}


/* LU decomposition ----------------------------------------------------------*/
int qlambda::ludcmp(double *A, int n, int *indx, double *d)
{
    double big,s,tmp,*vv=mat(n,1);
    int i,imax=0,j,k;

    *d=1.0;
    for (i=0;i<n;i++) {
        big=0.0; for (j=0;j<n;j++) if ((tmp=fabs(A[i+j*n]))>big) big=tmp;
        if (big>0.0) vv[i]=1.0/big; else {free(vv); return -1;}
    }
    for (j=0;j<n;j++) {
        for (i=0;i<j;i++) {
            s=A[i+j*n]; for (k=0;k<i;k++) s-=A[i+k*n]*A[k+j*n]; A[i+j*n]=s;
        }
        big=0.0;
        for (i=j;i<n;i++) {
            s=A[i+j*n]; for (k=0;k<j;k++) s-=A[i+k*n]*A[k+j*n]; A[i+j*n]=s;
            if ((tmp=vv[i]*fabs(s))>=big) {big=tmp; imax=i;}
        }
        if (j!=imax) {
            for (k=0;k<n;k++) {
                tmp=A[imax+k*n]; A[imax+k*n]=A[j+k*n]; A[j+k*n]=tmp;
            }
            *d=-(*d); vv[imax]=vv[j];
        }
        indx[j]=imax;
        if (A[j+j*n]==0.0) {free(vv); return -1;}
        if (j!=n-1) {
            tmp=1.0/A[j+j*n]; for (i=j+1;i<n;i++) A[i+j*n]*=tmp;
        }
    }
    free(vv);
    return 0;
}
/* LU back-substitution ------------------------------------------------------*/
void qlambda::lubksb(const double *A, int n, const int *indx, double *b)
{
    double s;
    int i,ii=-1,ip,j;

    for (i=0;i<n;i++) {
        ip=indx[i]; s=b[ip]; b[ip]=b[i];
        if (ii>=0) for (j=ii;j<i;j++) s-=A[i+j*n]*b[j]; else if (s) ii=i;
        b[i]=s;
    }
    for (i=n-1;i>=0;i--) {
        s=b[i]; for (j=i+1;j<n;j++) s-=A[i+j*n]*b[j]; b[i]=s/A[i+i*n];
    }
}


/* solve linear equation -----------------------------------------------------*/
int qlambda::solve(const char *tr, const double *A, const double *Y, int n,
                 int m, double *X)
{
    double *B=mat(n,n);
    int info;

    matcpy(B,A,n,n);
    if (!(info=matinv(B,n))) labmat_cal.matmul(tr[0]=='N'?"NN":"TN",n,m,n,1.0,B,Y,0.0,X);
    free(B);
    return info;
}

void qlambda::sort_double(int de_sortN,double *sortMaxEpo)
{
    for(int i=0;i<de_sortN;i++) {
        for(int j=i+1;j<de_sortN;j++) {
            if(sortMaxEpo[i]<sortMaxEpo[j]) {
                double temp=sortMaxEpo[i];//,temp1 = sysTemFlag[i]
                sortMaxEpo[i]=sortMaxEpo[j];
                sortMaxEpo[j]=temp;
            }
        }
    }
}

