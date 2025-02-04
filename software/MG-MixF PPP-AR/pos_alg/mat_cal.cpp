#include "mat_cal.h"

mat_cal::mat_cal()
{

}


/*normalize 3d vector*/
int mat_cal::normv3(const double *a, double *b)
{
    double r;
    if ((r=norm(a,3))<=0.0) return 0;
    b[0]=a[0]/r;
    b[1]=a[1]/r;
    b[2]=a[2]/r;
    return 1;
}
/*euclid norm of vector*/
double mat_cal::norm(const double *a,int n)
{
    return sqrt(dot(a,a,n));
}
/*outer product of 3d vectors*/
void mat_cal::cross3(const double *a, const double *b, double *c)
{
    c[0]=a[1]*b[2]-a[2]*b[1];
    c[1]=a[2]*b[0]-a[0]*b[2];
    c[2]=a[0]*b[1]-a[1]*b[0];
}

double mat_cal::var_urassr(int ura)
{
    double std;
    if (ura<= 0) return SQR(DEFURASSR);
    if (ura>=63) return SQR(5.4665);
    std=(pow(3.0,(ura>>3)&7)*(1.0+(ura&7)/4.0)-1.0)*1E-3;
    return SQR(std);
}

/*inner product*/
double mat_cal::dot(const double *a, const double *b, int n)
{
    double c=0.0;
    while (--n>=0) c+=a[n]*b[n];
    return c;
}
/*diff vector*/
void mat_cal::diff_vec(const double *a, const double *b, double *c,int n)
{
    while (--n>=0) c[n]=a[n]-b[n];
}
/*inner product*/
double mat_cal::prod(const double *d, int n)
{
    double c=1.0;
    while (--n>=0) c=d[n]*c;
    return c;
}
/* glonass orbit differential equations --------------------------------------*/
void mat_cal::deq(const double *x, double *xdot, const double *acc)
{
    double a,b,c,r2=dot(x,x,3),r3=r2*sqrt(r2),omg2=SQR(OMGE_GLO);

    if (r2<=0.0) {
        xdot[0]=xdot[1]=xdot[2]=xdot[3]=xdot[4]=xdot[5]=0.0;
        return;
    }
    /* ref [2] A.3.1.2 with bug fix for xdot[4],xdot[5] */
    a=1.5*J2_GLO*MU_GLO*SQR(RE_GLO)/r2/r3; /* 3/2*J2*mu*Ae^2/r^5 */
    b=5.0*x[2]*x[2]/r2;                    /* 5*z^2/r^2 */
    c=-MU_GLO/r3-a*(1.0-b);                /* -mu/r^3-a(1-b) */
    xdot[0]=x[3]; xdot[1]=x[4]; xdot[2]=x[5];
    xdot[3]=(c+omg2)*x[0]+2.0*OMGE_GLO*x[4]+acc[0];
    xdot[4]=(c+omg2)*x[1]-2.0*OMGE_GLO*x[3]+acc[1];
    xdot[5]=(c-2.0*a)*x[2]+acc[2];
}
/* multiply matrix (wrapper of blas dgemm) -------------------------------------
* multiply matrix by matrix (C=alpha*A*B+beta*C)
* args   : char   *tr       I  transpose flags ("N":normal,"T":transpose)
*          int    n,k,m     I  size of (transposed) matrix A,B
*          double alpha     I  alpha
*          double *A,*B     I  (transposed) matrix A (n x m), B (m x k)
*          double beta      I  beta
*          double *C        IO matrix C (n x k)
* return : none
*-----------------------------------------------------------------------------*/
void mat_cal::matmul(const char *tr, int n, int k, int m, double alpha,
                   const double *A, const double *B, double beta, double *C)
{
    double d;
    int i,j,x,f=tr[0]=='N'?(tr[1]=='N'?1:2):(tr[1]=='N'?3:4);

    for (i=0;i<n;i++) for (j=0;j<k;j++) {
        d=0.0;
        switch (f) {
            case 1: for (x=0;x<m;x++) d+=A[i+x*n]*B[x+j*m]; break;
            case 2: for (x=0;x<m;x++) d+=A[i+x*n]*B[j+x*k]; break;
            case 3: for (x=0;x<m;x++) d+=A[x+i*m]*B[x+j*m]; break;
            case 4: for (x=0;x<m;x++) d+=A[x+i*m]*B[j+x*k]; break;
        }
        if (beta==0.0) C[i+j*n]=alpha*d; else C[i+j*n]=alpha*d+beta*C[i+j*n];
    }
}
/* copy matrix -
 * -n,m is matrix A Row col
 * -k,l is matrix A start coordinate
 * -g,h is matrix B copy Row col
 * --------------------------
 * -----------------------------*/
void mat_cal::copy_mat(int n,int m,int k,int l,int g,int h,const double *A,double *B)
{
    for (int i=0;i<g;i++) for (int j=0;j<h;j++) {
        B[i+j*g] = A[i+k+(j+l)*n];
    }
}
/* string to number ------------------------------------------------------------
* convert substring in string to number
* args   : char   *s        I   string ("... nnn.nnn ...")
*          int    i,n       I   substring position and width
* return : converted number (0.0:error)
*-----------------------------------------------------------------------------*/
double mat_cal::str2num(const char *s, int i, int n)
{
    double value;
    char str[256],*p=str;

    if (i<0||(int)strlen(s)<i||(int)sizeof(str)-1<n) return 0.0;
    for (s+=i;*s&&--n>=0;s++) *p++=*s=='d'||*s=='D'?'E':*s;
    *p='\0';
    return sscanf(str,"%lf",&value)==1?value:0.0;
}
/* geometric distance ----------------------------------------------------------
* compute geometric distance and receiver-to-satellite unit vector
* args   : double *rs       I   satellilte position (ecef at transmission) (m)
*          double *rr       I   receiver position (ecef at reception) (m)
*          double *e        O   line-of-sight vector (ecef)
* return : geometric distance (m) (0>:error/no satellite position)
* notes  : distance includes sagnac effect correction
*-----------------------------------------------------------------------------*/
double mat_cal::geodist(const double *rs, const double *rr, double *e)
{
    double r;
    int i;

    if (norm(rs,3)<RE_WGS84) return -1.0;
    if(std::isnan(norm(rs,3))) return -1.0; //NAN-2020.0229
    for (i=0;i<3;i++) e[i]=rs[i]-rr[i];
    r=norm(e,3);
    for (i=0;i<3;i++) e[i]/=r;
    return r+OMGE*(rs[0]*rr[1]-rs[1]*rr[0])/M_C;
}
void mat_cal::RemoveRow(MatrixXd& matrix, unsigned int rowToRemove) {
  unsigned int numRows = matrix.rows() - 1;
  unsigned int numCols = matrix.cols();

  if( rowToRemove < numRows ) {
    matrix.block(rowToRemove,0,numRows-rowToRemove,numCols) =
      matrix.block(rowToRemove+1,0,numRows-rowToRemove,numCols);
  }

  matrix.conservativeResize(numRows,numCols);
}

void mat_cal::RemoveColumn(MatrixXd &matrix, unsigned int colToRemove) {
  unsigned int numRows = matrix.rows();
  unsigned int numCols = matrix.cols() - 1;

  if( colToRemove < numCols ) {
    matrix.block(0, colToRemove, numRows, numCols - colToRemove) =
      matrix.block(0, colToRemove + 1, numRows, numCols - colToRemove);
  }

  matrix.conservativeResize(numRows,numCols);
}
void mat_cal::Remove_allZeroRow(MatrixXd& matrix)
{
  unsigned int numRows = matrix.rows();
  unsigned int numCols = matrix.cols();
  for(int i=numRows-1;i>=0;i--)
  {
      bool all_zero_row = true;
      for(int j=0;j<numCols;j++)
      {
          if(matrix(i,j)!=0)
              all_zero_row = false;
      }
      if(all_zero_row)
          RemoveRow(matrix,i);
  }
}
// The edit box automatically scrolls, adding one row or more lines at a time.
void mat_cal::autoScrollTextEdit(QTextEdit *textEdit,QString &add_text)
{
    if(textEdit == NULL) return ;
    int m_Display_Max_line = 99999;
    //Add line character and refresh edit box.
    QString insertText = add_text + ENDLINE;
    textEdit->insertPlainText(insertText);
    //Keep the editor in the last line of the cursor.
    QTextCursor cursor=textEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    textEdit->setTextCursor(cursor);
    textEdit->repaint();
    //If you exceed a certain number of lines, empty it.
    if(textEdit->document()->lineCount() > m_Display_Max_line)
    {
        textEdit->clear();
    }
}
