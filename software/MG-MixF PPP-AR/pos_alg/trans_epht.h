#ifndef TRANS_EPHT_H
#define TRANS_EPHT_H
#include "pos_algorith.h"
#include "mat_cal.h"
#include "coordinate_trans.h"

const  double gpst0[]={1980,1, 6,0,0,0}; /* gps time reference */
const  double gst0 []={1999,8,22,0,0,0}; /* galileo system time reference */
const  double bdt0 []={2006,1, 1,0,0,0}; /* beidou time reference */


const static double leaps[][7]={ /* leap seconds {y,m,d,h,m,s,utc-gpst,...} */
    {2017,1,1,0,0,0,-18},
    {2015,7,1,0,0,0,-17},
    {2012,7,1,0,0,0,-16},
    {2009,1,1,0,0,0,-15},
    {2006,1,1,0,0,0,-14},
    {1999,1,1,0,0,0,-13},
    {1997,7,1,0,0,0,-12},
    {1996,1,1,0,0,0,-11},
    {1994,7,1,0,0,0,-10},
    {1993,7,1,0,0,0, -9},
    {1992,7,1,0,0,0, -8},
    {1991,1,1,0,0,0, -7},
    {1990,1,1,0,0,0, -6},
    {1988,1,1,0,0,0, -5},
    {1985,7,1,0,0,0, -4},
    {1983,7,1,0,0,0, -3},
    {1982,7,1,0,0,0, -2},
    {1981,7,1,0,0,0, -1},
    {1980,1,6,0,0,0, 0}
};
/* coordinate rotation matrix ------------------------------------------------*/
#define Rx(t,X) do { \
    (X)[0]=1.0; (X)[1]=(X)[2]=(X)[3]=(X)[6]=0.0; \
    (X)[4]=(X)[8]=cos(t); (X)[7]=sin(t); (X)[5]=-(X)[7]; \
} while (0)

#define Ry(t,X) do { \
    (X)[4]=1.0; (X)[1]=(X)[3]=(X)[5]=(X)[7]=0.0; \
    (X)[0]=(X)[8]=cos(t); (X)[2]=sin(t); (X)[6]=-(X)[2]; \
} while (0)

#define Rz(t,X) do { \
    (X)[8]=1.0; (X)[2]=(X)[5]=(X)[6]=(X)[7]=0.0; \
    (X)[0]=(X)[4]=cos(t); (X)[3]=sin(t); (X)[1]=-(X)[3]; \
} while (0)

class trans_epht:public coordinate_trans
{
public:
    trans_epht();
    double getLeapSecond(int Year,int Month,int Day,int Hours=0,int Minutes=0,double Seconds = 0.0);/*gey leap seconds*/
    double computeJD(int Year,int Month,int Day,int HoursInt,int Minutes = 0,double Seconds = 0.0);
    gtime_t bdt2gpst(gtime_t t);
    gtime_t timeadd(gtime_t t, double sec);
    gtime_t adjweek(gtime_t t, gtime_t t0);
    gtime_t gpst2time(int week, double sec);
    gtime_t bdt2time(int week, double sec);
    gtime_t adjday(gtime_t t, gtime_t t0);
    gtime_t utc2gpst(gtime_t t);
    double time2gpst(gtime_t t, int *week);
    bool getSunMoonPos(int Year,int Month,int Day,int HoursInt,int Minutes,double Seconds,double *sunpos,double *moonpos,double *gmst = NULL);//Calculating WGS84 coordinates of the Sun and Moon and GMST Pingrini Time
    void sunmoonpos(gtime_t tutc, const double *erpv, double *rsun,
        double *rmoon, double *gmst);/*Calculating Solar Coordinate Reference RTKlab*/
    void setSunMoonPos(double *psun,double *pmoon,double gmst=0);
    int uraindex(double value);
    gtime_t epoch2time(const double *ep);
    gtime_t gpst2utc(gtime_t t);
    void time2epoch(gtime_t t, double *ep);
    void eci2ecef(gtime_t tutc, const double *erpv, double *U, double *gmst);
    double YMD2GPSTime(int Year, int Month, int Day, int HoursInt, int Minutes, double Seconds,int *WeekN = NULL, int *day = NULL);
    double timediff(gtime_t t1, gtime_t t2);
    double utc2gmst(gtime_t t, double ut1_utc);
    double time2sec(gtime_t time, gtime_t *day);
    int geterp(const erp_t *erp, gtime_t time, double *erpv);
    double time2doy(gtime_t t);
    double interpc(const double coef[], double lat);
    int str2time(const char *s, int i, int n, gtime_t *t);
    int YearAccDay(int Year, int Month, int Day);
    double DayAccSec(int hours, int min, int sec);
    void DOP_vlaue(const QVector<SatlitData> &Sat_Dop, double *Dops);
private:
    void sunmoonpos_eci(gtime_t tut, double *rsun, double *rmoon);
    void ast_args(double t, double *f);
    void nut_iau1980(double t, const double *f, double *dpsi, double *deps);
public:
    erp_t m_erpData;//Save ERP file data
    bool isuseErp;/*Read ERP file data*/
    double m_sunpos[3];/*each epoch is updated once to provide coordinates for other programs, reducing repeated calculations*/
    double m_moonpos[3];
    double m_gmst;
private:

};

#endif // TRANS_EPHT_H
