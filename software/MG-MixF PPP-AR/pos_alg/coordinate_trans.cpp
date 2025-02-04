#include "coordinate_trans.h"

coordinate_trans::coordinate_trans(void)
{
    coo_init_var();
}
void coordinate_trans::coo_init_var()
{
    IsaddGPS=false;//Whether to add GPS (default on)
    IsaddGLOSS=false;//Whether to add GLONASS or not
    IsaddBDS=false;//Whether to join Beidou
    IsaddGalieo=false;//Whether to join Galieo or not
}
void coordinate_trans::XYZ2BLH(double *pXYZ,double *m_pBLH)
{
    double X=pXYZ[0],Y=pXYZ[1],Z=pXYZ[2];
    if (X == 0)
        return;
    if (X > 0&&Y>0)
        m_pBLH[1] = atan(Y/X);
    else if(X < 0&&Y > 0)
        m_pBLH[1] = atan(Y/X) + MM_PI;
    else if (X < 0&&Y < 0)
        m_pBLH[1] = -(MM_PI - atan(Y/X));
    else
        m_pBLH[1] = atan(Y/X);
    //Compute B
    if(X == 0&&Y == 0)
        return;
    double t0 = Z/qSqrt(X*X + Y*Y);
    double P = (elipsePara[2]*elipsePara[4])/qSqrt(X*X + Y*Y);
    double k = 1 + elipsePara[5];
    double eps = 1e-12;
    double ti = t0;
    double ti1 = 0;
    double Bi = qAtan(ti);
    double Bi1 = 0;
    do
    {
        ti1 = t0 + P*ti/qSqrt(k + ti*ti);
        Bi = qAtan(ti);
        Bi1 = qAtan(ti1);
        ti = ti1;
    } while (qAbs(Bi1-Bi) > eps);
    m_pBLH[0] = Bi1;
    double W = qSqrt(1 - elipsePara[4]*qSin(Bi1)*qSin(Bi1));
    double N = elipsePara[0]/W;
    m_pBLH[2] = Z/qSin(Bi1) - N*(1 - elipsePara[4]);
}
void coordinate_trans::XYZ2ENU(double *pXYZ,double *m_pENU,double *PX)
{

    double dx = pXYZ[0] - PX[0];
    double dy = pXYZ[1] - PX[1];
    double dz = pXYZ[2] - PX[2];
    double PX_BLH[3] = {0};
    XYZ2BLH(PX,PX_BLH);//Calculate L to appear bug
    double B = PX_BLH[0];
    double L = PX_BLH[1];
    m_pENU[0] = (-qSin(L)*dx + qCos(L)*dy);
    m_pENU[1] = (-qSin(B)*qCos(L)*dx - qSin(B)*qSin(L)*dy + qCos(B)*dz);
    m_pENU[2] = (qCos(B)*qCos(L)*dx + qCos(B)*qSin(L)*dy + qSin(B)*dz);
}
void coordinate_trans::XYZ2SAZ(double *pXYZ,double *m_pSAZ,double *PX)
{
    double dx = pXYZ[0] - PX[0];
    double dy = pXYZ[1] - PX[1];
    double dz = pXYZ[2] - PX[2];
    double PX_BLH[3] = {0};
    XYZ2BLH(PX,PX_BLH);//Calculate L to appear bug
    double B = PX_BLH[0];
    double L = PX_BLH[1];
    double xx = (-qSin(B)*qCos(L)*dx - qSin(B)*qSin(L)*dy + qCos(B)*dz);
    double yy = (-qSin(L)*dx + qCos(L)*dy + 0*dz);
    double zz = (qCos(B)*qCos(L)*dx + qCos(B)*qSin(L)*dy + qSin(B)*dz);
    m_pSAZ[0] = qSqrt(xx*xx + yy*yy + zz*zz);//S
    if (xx == 0)
        return;
    if (xx > 0&&yy>0)
        m_pSAZ[1] = atan(yy/xx);
    else if(xx < 0&&yy > 0)
        m_pSAZ[1] = atan(yy/xx) + MM_PI;
    else if (xx < 0&&yy < 0)
        m_pSAZ[1] = atan(yy/xx) + MM_PI;
    else
        m_pSAZ[1] = atan(yy/xx) + 2*MM_PI;
    if (m_pSAZ[0] == 0)
        return;
    m_pSAZ[2] = qAcos(qAbs(zz/m_pSAZ[0]));//Z(0,pi/2)appear bug
}
/* transform ecef to geodetic postion ------------------------------------------
* transform ecef position to geodetic position
* args   : double *r        I   ecef position {x,y,z} (m)
*          double *pos      O   geodetic position {lat,lon,h} (rad,m)
* return : none
* notes  : WGS84, ellipsoidal height
*-----------------------------------------------------------------------------*/
void coordinate_trans::ecef2pos(const double *r, double *pos)
{
    double e2=FE_WGS84*(2.0-FE_WGS84),r2=dot(r,r,2),z,zk,v=RE_WGS84,sinp;

    for (z=r[2],zk=0.0;fabs(z-zk)>=1E-4;) {
        zk=z;
        sinp=z/sqrt(r2+z*z);
        v=RE_WGS84/sqrt(1.0-e2*sinp*sinp);
        z=r[2]+v*e2*sinp;
    }
    pos[0]=r2>1E-12?atan(z/sqrt(r2)):(r[2]>0.0?MM_PI/2.0:-MM_PI/2.0);
    pos[1]=r2>1E-12?atan2(r[1],r[0]):0.0;
    pos[2]=sqrt(r2+z*z)-v;
}
/* ecef to local coordinate transfromation matrix ------------------------------
* compute ecef to local coordinate transfromation matrix
* args   : double *pos      I   geodetic position {lat,lon} (rad)
*          double *E        O   ecef to local coord transformation matrix (3x3)
* return : none
* notes  : matirix stored by column-major order (fortran convention)
*-----------------------------------------------------------------------------*/
void coordinate_trans::xyz2enu(const double *pos, double *E)
{
    double sinp=sin(pos[0]),cosp=cos(pos[0]),sinl=sin(pos[1]),cosl=cos(pos[1]);

    E[0]=-sinl;      E[3]=cosl;       E[6]=0.0;
    E[1]=-sinp*cosl; E[4]=-sinp*sinl; E[7]=cosp;
    E[2]=cosp*cosl;  E[5]=cosp*sinl;  E[8]=sinp;
}
/*Setting up the file system. SystemStr:"G"(Turn on the GPS system);
 * "GR":(Turn on the GPS+GLONASS system);"GRCE"(Open all)et al
 *GPS, GLONASS, BDS, and Galieo are used respectively: the letters G, R, C, E*/
bool coordinate_trans::setSatlitSys(QString SystemStr)
{
    if (!(SystemStr.contains("G")||SystemStr.contains("R")||SystemStr.contains("C")||SystemStr.contains("E")))
        return	false;
    if (SystemStr.contains("G"))
    {
        IsaddGPS = true;
    }
    else
        IsaddGPS = false;
    if (SystemStr.contains("R"))
    {
        IsaddGLOSS = true;
    }
    else
        IsaddGLOSS = false;
    if (SystemStr.contains("C"))
    {
        IsaddBDS = true;
    }
    else
        IsaddBDS = false;
    if (SystemStr.contains("E"))
    {
        IsaddGalieo = true;
    }
    else
        IsaddGalieo = false;
    return true;
}
/*Sys = G R C E(Representing GPS, GLONASS, BDS, Galieo systems)
 * Determine if the system data is needed*/
bool coordinate_trans::isInSystem(char Sys)
{
    if (IsaddGPS&&Sys == 'G')
        return true;
    else if (IsaddGLOSS&&Sys == 'R')
        return true;
    else if (IsaddBDS&&Sys == 'C')
        return true;
    else if (IsaddGalieo&&Sys == 'E')
        return true;
    return false;
}

