#ifndef COORDINATE_TRANS_H
#define COORDINATE_TRANS_H

#include "pos_algorith.h"
#include "mat_cal.h"
#include "filter_alg/coefficient_matrix.h"

class coordinate_trans:public mat_cal
{
public:
    coordinate_trans(void);

    void XYZ2BLH(double *pXYZ, double *m_pBLH);
    void XYZ2ENU(double *pXYZ,double *m_pENU,double *PX);
    void XYZ2SAZ(double *pXYZ,double *m_pSAZ,double *PX);
    void ecef2pos(const double *r, double *pos);
    void xyz2enu(const double *pos, double *E);
    bool setSatlitSys(QString SystemStr);
    bool isInSystem(char Sys);

private:
     void coo_init_var();
public:

protected:
    bool IsaddGPS;//Whether to add GPS (default on)
    bool IsaddGLOSS;//Whether to add GLONASS or not
    bool IsaddBDS;//Whether to join Beidou
    bool IsaddGalieo;//Whether to join Galieo or not

};

#endif // COORDINATE_TRANS_H
