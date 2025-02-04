#include "trans_epht.h"
trans_epht::trans_epht()
{

}



/*Calculate Julian Day*/
double trans_epht::computeJD(int Year,int Month,int Day,int HoursInt,int Minutes,double Seconds)
{
    double Hours = HoursInt + ((Minutes * 60) + Seconds)/3600.0;
    /*Get JD*/
    double JD = 0.0;
    if(Month<=2)
        JD = (int)(365.25*(Year-1)) + (int)(30.6001*(Month+12+1)) + Day + Hours/24.0 + 1720981.5;
    else
        JD = (int)(365.25*(Year)) + (int)(30.6001*(Month+1)) + Day + Hours/24.0 + 1720981.5;
    return JD;
}
/*Calculating leap seconds*/
/*Leap file: ftp://hpiers.obspm.fr/iers/bul/bulc/Leap_Second.dat*/
double trans_epht::getLeapSecond(int Year, int Month, int Day, int Hours/* =0 */, int Minutes/* =0 */, double Seconds/* =0 */)
{
    double jd = computeJD(Year,Month,Day,Hours, Minutes, Seconds);
    double leapseconds=0;
    double Leap_seconds[50]={0};
    double TAImUTCData[50]={0};
    Leap_seconds[0]=10;
    TAImUTCData[0]=computeJD(1972,1,1,0);
    Leap_seconds[1]=11;
    TAImUTCData[1]=computeJD(1972,7,1,0);
    Leap_seconds[2]=12;
    TAImUTCData[2]=computeJD(1973,1,1,0);
    Leap_seconds[3]=13;
    TAImUTCData[3]=computeJD(1974,1,1,0);
    Leap_seconds[4]=14;
    TAImUTCData[4]=computeJD(1975,1,1,0);
    Leap_seconds[5]=15;
    TAImUTCData[5]=computeJD(1976,1,1,0);
    Leap_seconds[6]=16;
    TAImUTCData[6]=computeJD(1977,1,1,0);
    Leap_seconds[7]=17;
    TAImUTCData[7]=computeJD(1978,1,1,0);
    Leap_seconds[8]=18;
    TAImUTCData[8]=computeJD(1979,1,1,0);
    Leap_seconds[9]=19;
    TAImUTCData[9]=computeJD(1980,1,1,0);
    Leap_seconds[10]=20;
    TAImUTCData[10]=computeJD(1981,7,1,0);
    Leap_seconds[11]=21;
    TAImUTCData[11]=computeJD(1982,7,1,0);
    Leap_seconds[12]=22;
    TAImUTCData[12]=computeJD(1983,7,1,0);
    Leap_seconds[13]=23;
    TAImUTCData[13]=computeJD(1985,7,1,0);
    Leap_seconds[14]=24;
    TAImUTCData[14]=computeJD(1988,1,1,0);
    Leap_seconds[15]=25;
    TAImUTCData[15]=computeJD(1990,1,1,0);
    Leap_seconds[16]=26;
    TAImUTCData[16]=computeJD(1991,1,1,0);
    Leap_seconds[17]=27;
    TAImUTCData[17]=computeJD(1992,7,1,0);
    Leap_seconds[18]=28;
    TAImUTCData[18]=computeJD(1993,7,1,0);
    Leap_seconds[19]=29;
    TAImUTCData[19]=computeJD(1994,7,1,0);
    Leap_seconds[20]=30;
    TAImUTCData[20]=computeJD(1996,1,1,0);
    Leap_seconds[21]=31;
    TAImUTCData[21]=computeJD(1997,7,1,0);
    Leap_seconds[22]=32;
    TAImUTCData[22]=computeJD(1999,1,1,0);
    Leap_seconds[23]=33;
    TAImUTCData[23]=computeJD(2006,1,1,0);
    Leap_seconds[24]=34;
    TAImUTCData[24]=computeJD(2009,1,1,0);
    Leap_seconds[25]=35;
    TAImUTCData[25]=computeJD(2012,7,1,0);
    Leap_seconds[26]=36;
    TAImUTCData[26]=computeJD(2015,7,1,0);
    Leap_seconds[27]=37;
    TAImUTCData[27]=computeJD(2017,1,1,0);
    if (jd<TAImUTCData[0])
    {
        leapseconds=0;
    }
    else if (jd>TAImUTCData[27])
    {
        leapseconds=Leap_seconds[27];
    }
    else
    {
        int iter=0;
        for (int i=1;i<28;i++)
        {
            if (jd<=TAImUTCData[i] && jd>TAImUTCData[i-1])
            {
                iter=i;
                break;
            }
        }
        leapseconds=Leap_seconds[iter-1];
    }
    return (leapseconds-19);
}
/* bds trans gps time --------------------------------------------------------------------
* Beidou time plus 14 seconds to GPS time
* args   : gtime_t t        I   gtime_t struct
* return : gtime_t gpst struct (t+sec)
*-----------------------------------------------------------------------------*/
gtime_t trans_epht::bdt2gpst(gtime_t t)
{
    return timeadd(t,14.0);
}
/* add time --------------------------------------------------------------------
* add time to gtime_t struct
* args   : gtime_t t        I   gtime_t struct
*          double sec       I   time to add (s)
* return : gtime_t struct (t+sec)
*-----------------------------------------------------------------------------*/
gtime_t trans_epht::timeadd(gtime_t t, double sec)
{
    double tt;

    t.sec+=sec; tt=floor(t.sec); t.time+=(int)tt; t.sec-=tt;
    return t;
}
/* adjust time considering week handover -------------------------------------*/
gtime_t trans_epht::adjweek(gtime_t t, gtime_t t0)
{
    double tt=timediff(t,t0);
    if (tt<-302400.0) return timeadd(t, 604800.0);
    if (tt> 302400.0) return timeadd(t,-604800.0);
    return t;
}
/* gps time to time ------------------------------------------------------------
* convert week and tow in gps time to gtime_t struct
* args   : int    week      I   week number in gps time
*          double sec       I   time of week in gps time (s)
* return : gtime_t struct
*-----------------------------------------------------------------------------*/
gtime_t trans_epht::gpst2time(int week, double sec)
{
    gtime_t t=epoch2time(gpst0);

    if (sec<-1E9||1E9<sec) sec=0.0;
    t.time+=(time_t)86400*7*week+(int)sec;
    t.sec=sec-(int)sec;
    return t;
}
/*bdst trans utct*/
gtime_t trans_epht::bdt2time(int week, double sec)
{
    gtime_t t=epoch2time(bdt0);

    if (sec<-1E9||1E9<sec) sec=0.0;
    t.time+=(time_t)86400*7*week+(int)sec;
    t.sec=sec-(int)sec;
    return t;
}
/* Time difference between t1 and t2  ------------------------------------------------------------
* convert difference between t1 and t2 to gtime_t struct
* args   : gtime_t    t1      I   time
*          gtime_t    t2      I   time
* return : gtime_t struct
*-----------------------------------------------------------------------------*/
double trans_epht::timediff(gtime_t t1, gtime_t t2)
{
    return difftime(t1.time,t2.time)+t1.sec-t2.sec;
}

/* time to gps time ------------------------------------------------------------
* convert gtime_t struct to week and tow in gps time
* args   : gtime_t t        I   gtime_t struct
*          int    *week     IO  week number in gps time (NULL: no output)
* return : time of week in gps time (s)
*-----------------------------------------------------------------------------*/
double trans_epht::time2gpst(gtime_t t, int *week)
{
    gtime_t t0=epoch2time(gpst0);
    time_t sec=t.time-t0.time;
    int w=(int)(sec/(86400*7));

    if (week) *week=w;
    return (double)(sec-(double)w*86400*7)+t.sec;
}
/* adjust time considering week handover -------------------------------------*/
gtime_t trans_epht::adjday(gtime_t t, gtime_t t0)
{
    double tt=timediff(t,t0);
    if (tt<-43200.0) return timeadd(t, 86400.0);
    if (tt> 43200.0) return timeadd(t,-86400.0);
    return t;
}
/* utc to gpstime --------------------------------------------------------------
* convert utc to gpstime considering leap seconds
* args   : gtime_t t        I   time expressed in utc
* return : time expressed in gpstime
* notes  : ignore slight time offset under 100 ns
*-----------------------------------------------------------------------------*/
gtime_t trans_epht::utc2gpst(gtime_t t)
{
    int i;

    for (i=0;leaps[i][0]>0;i++) {
        if (timediff(t,epoch2time(leaps[i]))>=0.0) return timeadd(t,-leaps[i][6]);
    }
    return t;
}
/* sun and moon position -------------------------------------------------------
* get sun and moon position in ecef
* args   : gtime_t tut      I   time in ut1
*          double *erpv     I   erp value {xp,yp,ut1_utc,lod} (rad,rad,s,s/d)
*          double *rsun     IO  sun position in ecef  (m) (NULL: not output)
*          double *rmoon    IO  moon position in ecef (m) (NULL: not output)
*          double *gmst     O   gmst (rad)
* return : none
*-----------------------------------------------------------------------------*/
void trans_epht::sunmoonpos(gtime_t tutc, const double *erpv, double *rsun,
                       double *rmoon, double *gmst)
{
    gtime_t tut;
    double rs[3],rm[3],U[9],gmst_;

    /*trace(4,"sunmoonpos: tutc=%s\n",time_str(tutc,3));*/

    tut=timeadd(tutc,erpv[2]); /* utc -> ut1 */

    /* sun and moon position in eci */
    sunmoonpos_eci(tut,rsun?rs:NULL,rmoon?rm:NULL);

    /* eci to ecef transformation matrix */
    eci2ecef(tutc,erpv,U,&gmst_);

    /* sun and moon postion in ecef */
    if (rsun ) matmul("NN",3,1,3,1.0,U,rs,0.0,rsun );
    if (rmoon) matmul("NN",3,1,3,1.0,U,rm,0.0,rmoon);
    if (gmst ) *gmst=gmst_;
}
/* sun and moon position in eci (ref [4] 5.1.1, 5.2.1) -----------------------*/
void trans_epht::sunmoonpos_eci(gtime_t tut, double *rsun, double *rmoon)
{
    const double ep2000[]={2000,1,1,12,0,0};
    double t,f[5],eps,Ms,ls,rs,lm,pm,rm,sine,cose,sinp,cosp,sinl,cosl;

//    trace(4,"sunmoonpos_eci: tut=%s\n",time_str(tut,3));

    t=timediff(tut,epoch2time(ep2000))/86400.0/36525.0;

    /* astronomical arguments */
    ast_args(t,f);

    /* obliquity of the ecliptic */
    eps=23.439291-0.0130042*t;
    sine=sin(eps*D2R); cose=cos(eps*D2R);

    /* sun position in eci */
    if (rsun) {
        Ms=357.5277233+35999.05034*t;
        ls=280.460+36000.770*t+1.914666471*sin(Ms*D2R)+0.019994643*sin(2.0*Ms*D2R);
        rs=AU*(1.000140612-0.016708617*cos(Ms*D2R)-0.000139589*cos(2.0*Ms*D2R));
        sinl=sin(ls*D2R); cosl=cos(ls*D2R);
        rsun[0]=rs*cosl;
        rsun[1]=rs*cose*sinl;
        rsun[2]=rs*sine*sinl;

//        trace(5,"rsun =%.3f %.3f %.3f\n",rsun[0],rsun[1],rsun[2]);
    }
    /* moon position in eci */
    if (rmoon) {
        lm=218.32+481267.883*t+6.29*sin(f[0])-1.27*sin(f[0]-2.0*f[3])+
           0.66*sin(2.0*f[3])+0.21*sin(2.0*f[0])-0.19*sin(f[1])-0.11*sin(2.0*f[2]);
        pm=5.13*sin(f[2])+0.28*sin(f[0]+f[2])-0.28*sin(f[2]-f[0])-
           0.17*sin(f[2]-2.0*f[3]);
        rm=RE_WGS84/sin((0.9508+0.0518*cos(f[0])+0.0095*cos(f[0]-2.0*f[3])+
                   0.0078*cos(2.0*f[3])+0.0028*cos(2.0*f[0]))*D2R);
        sinl=sin(lm*D2R); cosl=cos(lm*D2R);
        sinp=sin(pm*D2R); cosp=cos(pm*D2R);
        rmoon[0]=rm*cosp*cosl;
        rmoon[1]=rm*(cose*cosp*sinl-sine*sinp);
        rmoon[2]=rm*(sine*cosp*sinl+cose*sinp);

//        trace(5,"rmoon=%.3f %.3f %.3f\n",rmoon[0],rmoon[1],rmoon[2]);
    }
}
/* astronomical arguments: f={l,l',F,D,OMG} (rad) ----------------------------*/
void trans_epht::ast_args(double t, double *f)
{
    static const double fc[][5]={ /* coefficients for iau 1980 nutation */
        { 134.96340251, 1717915923.2178,  31.8792,  0.051635, -0.00024470},
        { 357.52910918,  129596581.0481,  -0.5532,  0.000136, -0.00001149},
        {  93.27209062, 1739527262.8478, -12.7512, -0.001037,  0.00000417},
        { 297.85019547, 1602961601.2090,  -6.3706,  0.006593, -0.00003169},
        { 125.04455501,   -6962890.2665,   7.4722,  0.007702, -0.00005939}
    };
    double tt[4];
    int i,j;

    for (tt[0]=t,i=1;i<4;i++) tt[i]=tt[i-1]*t;
    for (i=0;i<5;i++) {
        f[i]=fc[i][0]*3600.0;
        for (j=0;j<4;j++) f[i]+=fc[i][j+1]*tt[j];
        f[i]=fmod(f[i]*AS2R,2.0*MM_PI);
    }
}
/* iau 1980 nutation ---------------------------------------------------------*/
void trans_epht::nut_iau1980(double t, const double *f, double *dpsi, double *deps)
{
    static const double nut[106][10]={
        {   0,   0,   0,   0,   1, -6798.4, -171996, -174.2, 92025,   8.9},
        {   0,   0,   2,  -2,   2,   182.6,  -13187,   -1.6,  5736,  -3.1},
        {   0,   0,   2,   0,   2,    13.7,   -2274,   -0.2,   977,  -0.5},
        {   0,   0,   0,   0,   2, -3399.2,    2062,    0.2,  -895,   0.5},
        {   0,  -1,   0,   0,   0,  -365.3,   -1426,    3.4,    54,  -0.1},
        {   1,   0,   0,   0,   0,    27.6,     712,    0.1,    -7,   0.0},
        {   0,   1,   2,  -2,   2,   121.7,    -517,    1.2,   224,  -0.6},
        {   0,   0,   2,   0,   1,    13.6,    -386,   -0.4,   200,   0.0},
        {   1,   0,   2,   0,   2,     9.1,    -301,    0.0,   129,  -0.1},
        {   0,  -1,   2,  -2,   2,   365.2,     217,   -0.5,   -95,   0.3},
        {  -1,   0,   0,   2,   0,    31.8,     158,    0.0,    -1,   0.0},
        {   0,   0,   2,  -2,   1,   177.8,     129,    0.1,   -70,   0.0},
        {  -1,   0,   2,   0,   2,    27.1,     123,    0.0,   -53,   0.0},
        {   1,   0,   0,   0,   1,    27.7,      63,    0.1,   -33,   0.0},
        {   0,   0,   0,   2,   0,    14.8,      63,    0.0,    -2,   0.0},
        {  -1,   0,   2,   2,   2,     9.6,     -59,    0.0,    26,   0.0},
        {  -1,   0,   0,   0,   1,   -27.4,     -58,   -0.1,    32,   0.0},
        {   1,   0,   2,   0,   1,     9.1,     -51,    0.0,    27,   0.0},
        {  -2,   0,   0,   2,   0,  -205.9,     -48,    0.0,     1,   0.0},
        {  -2,   0,   2,   0,   1,  1305.5,      46,    0.0,   -24,   0.0},
        {   0,   0,   2,   2,   2,     7.1,     -38,    0.0,    16,   0.0},
        {   2,   0,   2,   0,   2,     6.9,     -31,    0.0,    13,   0.0},
        {   2,   0,   0,   0,   0,    13.8,      29,    0.0,    -1,   0.0},
        {   1,   0,   2,  -2,   2,    23.9,      29,    0.0,   -12,   0.0},
        {   0,   0,   2,   0,   0,    13.6,      26,    0.0,    -1,   0.0},
        {   0,   0,   2,  -2,   0,   173.3,     -22,    0.0,     0,   0.0},
        {  -1,   0,   2,   0,   1,    27.0,      21,    0.0,   -10,   0.0},
        {   0,   2,   0,   0,   0,   182.6,      17,   -0.1,     0,   0.0},
        {   0,   2,   2,  -2,   2,    91.3,     -16,    0.1,     7,   0.0},
        {  -1,   0,   0,   2,   1,    32.0,      16,    0.0,    -8,   0.0},
        {   0,   1,   0,   0,   1,   386.0,     -15,    0.0,     9,   0.0},
        {   1,   0,   0,  -2,   1,   -31.7,     -13,    0.0,     7,   0.0},
        {   0,  -1,   0,   0,   1,  -346.6,     -12,    0.0,     6,   0.0},
        {   2,   0,  -2,   0,   0, -1095.2,      11,    0.0,     0,   0.0},
        {  -1,   0,   2,   2,   1,     9.5,     -10,    0.0,     5,   0.0},
        {   1,   0,   2,   2,   2,     5.6,      -8,    0.0,     3,   0.0},
        {   0,  -1,   2,   0,   2,    14.2,      -7,    0.0,     3,   0.0},
        {   0,   0,   2,   2,   1,     7.1,      -7,    0.0,     3,   0.0},
        {   1,   1,   0,  -2,   0,   -34.8,      -7,    0.0,     0,   0.0},
        {   0,   1,   2,   0,   2,    13.2,       7,    0.0,    -3,   0.0},
        {  -2,   0,   0,   2,   1,  -199.8,      -6,    0.0,     3,   0.0},
        {   0,   0,   0,   2,   1,    14.8,      -6,    0.0,     3,   0.0},
        {   2,   0,   2,  -2,   2,    12.8,       6,    0.0,    -3,   0.0},
        {   1,   0,   0,   2,   0,     9.6,       6,    0.0,     0,   0.0},
        {   1,   0,   2,  -2,   1,    23.9,       6,    0.0,    -3,   0.0},
        {   0,   0,   0,  -2,   1,   -14.7,      -5,    0.0,     3,   0.0},
        {   0,  -1,   2,  -2,   1,   346.6,      -5,    0.0,     3,   0.0},
        {   2,   0,   2,   0,   1,     6.9,      -5,    0.0,     3,   0.0},
        {   1,  -1,   0,   0,   0,    29.8,       5,    0.0,     0,   0.0},
        {   1,   0,   0,  -1,   0,   411.8,      -4,    0.0,     0,   0.0},
        {   0,   0,   0,   1,   0,    29.5,      -4,    0.0,     0,   0.0},
        {   0,   1,   0,  -2,   0,   -15.4,      -4,    0.0,     0,   0.0},
        {   1,   0,  -2,   0,   0,   -26.9,       4,    0.0,     0,   0.0},
        {   2,   0,   0,  -2,   1,   212.3,       4,    0.0,    -2,   0.0},
        {   0,   1,   2,  -2,   1,   119.6,       4,    0.0,    -2,   0.0},
        {   1,   1,   0,   0,   0,    25.6,      -3,    0.0,     0,   0.0},
        {   1,  -1,   0,  -1,   0, -3232.9,      -3,    0.0,     0,   0.0},
        {  -1,  -1,   2,   2,   2,     9.8,      -3,    0.0,     1,   0.0},
        {   0,  -1,   2,   2,   2,     7.2,      -3,    0.0,     1,   0.0},
        {   1,  -1,   2,   0,   2,     9.4,      -3,    0.0,     1,   0.0},
        {   3,   0,   2,   0,   2,     5.5,      -3,    0.0,     1,   0.0},
        {  -2,   0,   2,   0,   2,  1615.7,      -3,    0.0,     1,   0.0},
        {   1,   0,   2,   0,   0,     9.1,       3,    0.0,     0,   0.0},
        {  -1,   0,   2,   4,   2,     5.8,      -2,    0.0,     1,   0.0},
        {   1,   0,   0,   0,   2,    27.8,      -2,    0.0,     1,   0.0},
        {  -1,   0,   2,  -2,   1,   -32.6,      -2,    0.0,     1,   0.0},
        {   0,  -2,   2,  -2,   1,  6786.3,      -2,    0.0,     1,   0.0},
        {  -2,   0,   0,   0,   1,   -13.7,      -2,    0.0,     1,   0.0},
        {   2,   0,   0,   0,   1,    13.8,       2,    0.0,    -1,   0.0},
        {   3,   0,   0,   0,   0,     9.2,       2,    0.0,     0,   0.0},
        {   1,   1,   2,   0,   2,     8.9,       2,    0.0,    -1,   0.0},
        {   0,   0,   2,   1,   2,     9.3,       2,    0.0,    -1,   0.0},
        {   1,   0,   0,   2,   1,     9.6,      -1,    0.0,     0,   0.0},
        {   1,   0,   2,   2,   1,     5.6,      -1,    0.0,     1,   0.0},
        {   1,   1,   0,  -2,   1,   -34.7,      -1,    0.0,     0,   0.0},
        {   0,   1,   0,   2,   0,    14.2,      -1,    0.0,     0,   0.0},
        {   0,   1,   2,  -2,   0,   117.5,      -1,    0.0,     0,   0.0},
        {   0,   1,  -2,   2,   0,  -329.8,      -1,    0.0,     0,   0.0},
        {   1,   0,  -2,   2,   0,    23.8,      -1,    0.0,     0,   0.0},
        {   1,   0,  -2,  -2,   0,    -9.5,      -1,    0.0,     0,   0.0},
        {   1,   0,   2,  -2,   0,    32.8,      -1,    0.0,     0,   0.0},
        {   1,   0,   0,  -4,   0,   -10.1,      -1,    0.0,     0,   0.0},
        {   2,   0,   0,  -4,   0,   -15.9,      -1,    0.0,     0,   0.0},
        {   0,   0,   2,   4,   2,     4.8,      -1,    0.0,     0,   0.0},
        {   0,   0,   2,  -1,   2,    25.4,      -1,    0.0,     0,   0.0},
        {  -2,   0,   2,   4,   2,     7.3,      -1,    0.0,     1,   0.0},
        {   2,   0,   2,   2,   2,     4.7,      -1,    0.0,     0,   0.0},
        {   0,  -1,   2,   0,   1,    14.2,      -1,    0.0,     0,   0.0},
        {   0,   0,  -2,   0,   1,   -13.6,      -1,    0.0,     0,   0.0},
        {   0,   0,   4,  -2,   2,    12.7,       1,    0.0,     0,   0.0},
        {   0,   1,   0,   0,   2,   409.2,       1,    0.0,     0,   0.0},
        {   1,   1,   2,  -2,   2,    22.5,       1,    0.0,    -1,   0.0},
        {   3,   0,   2,  -2,   2,     8.7,       1,    0.0,     0,   0.0},
        {  -2,   0,   2,   2,   2,    14.6,       1,    0.0,    -1,   0.0},
        {  -1,   0,   0,   0,   2,   -27.3,       1,    0.0,    -1,   0.0},
        {   0,   0,  -2,   2,   1,  -169.0,       1,    0.0,     0,   0.0},
        {   0,   1,   2,   0,   1,    13.1,       1,    0.0,     0,   0.0},
        {  -1,   0,   4,   0,   2,     9.1,       1,    0.0,     0,   0.0},
        {   2,   1,   0,  -2,   0,   131.7,       1,    0.0,     0,   0.0},
        {   2,   0,   0,   2,   0,     7.1,       1,    0.0,     0,   0.0},
        {   2,   0,   2,  -2,   1,    12.8,       1,    0.0,    -1,   0.0},
        {   2,   0,  -2,   0,   1,  -943.2,       1,    0.0,     0,   0.0},
        {   1,  -1,   0,  -2,   0,   -29.3,       1,    0.0,     0,   0.0},
        {  -1,   0,   0,   1,   1,  -388.3,       1,    0.0,     0,   0.0},
        {  -1,  -1,   0,   2,   1,    35.0,       1,    0.0,     0,   0.0},
        {   0,   1,   0,   1,   0,    27.3,       1,    0.0,     0,   0.0}
    };
    double ang;
    int i,j;

    *dpsi=*deps=0.0;

    for (i=0;i<106;i++) {
        ang=0.0;
        for (j=0;j<5;j++) ang+=nut[i][j]*f[j];
        *dpsi+=(nut[i][6]+nut[i][7]*t)*sin(ang);
        *deps+=(nut[i][8]+nut[i][9]*t)*cos(ang);
    }
    *dpsi*=1E-4*AS2R; /* 0.1 mas -> rad */
    *deps*=1E-4*AS2R;
}
/* eci to ecef transformation matrix -------------------------------------------
* compute eci to ecef transformation matrix
* args   : gtime_t tutc     I   time in utc
*          double *erpv     I   erp values {xp,yp,ut1_utc,lod} (rad,rad,s,s/d)
*          double *U        O   eci to ecef transformation matrix (3 x 3)
*          double *gmst     IO  greenwich mean sidereal time (rad)
*                               (NULL: no output)
* return : none
* note   : see ref [3] chap 5
*          not thread-safe
*-----------------------------------------------------------------------------*/
void trans_epht::eci2ecef(gtime_t tutc, const double *erpv, double *U, double *gmst)
{
    const double ep2000[]={2000,1,1,12,0,0};
    static gtime_t tutc_;
    static double U_[9],gmst_;
    gtime_t tgps;
    double eps,ze,th,z,t,t2,t3,dpsi,deps,gast,f[5];
    double R1[9],R2[9],R3[9],R[9],W[9],N[9],P[9],NP[9];
    int i;

//    trace(4,"eci2ecef: tutc=%s\n",time_str(tutc,3));

    if (fabs(timediff(tutc,tutc_))<0.01) { /* read cache */
        for (i=0;i<9;i++) U[i]=U_[i];
        if (gmst) *gmst=gmst_;
        return;
    }
    tutc_=tutc;

    /* terrestrial time */
    tgps=utc2gpst(tutc_);
    t=(timediff(tgps,epoch2time(ep2000))+19.0+32.184)/86400.0/36525.0;
    t2=t*t; t3=t2*t;

    /* astronomical arguments */
    ast_args(t,f);

    /* iau 1976 precession */
    ze=(2306.2181*t+0.30188*t2+0.017998*t3)*AS2R;
    th=(2004.3109*t-0.42665*t2-0.041833*t3)*AS2R;
    z =(2306.2181*t+1.09468*t2+0.018203*t3)*AS2R;
    eps=(84381.448-46.8150*t-0.00059*t2+0.001813*t3)*AS2R;
    Rz(-z,R1); Ry(th,R2); Rz(-ze,R3);
    matmul("NN",3,3,3,1.0,R1,R2,0.0,R);
    matmul("NN",3,3,3,1.0,R, R3,0.0,P); /* P=Rz(-z)*Ry(th)*Rz(-ze) */

    /* iau 1980 nutation */
    nut_iau1980(t,f,&dpsi,&deps);
    Rx(-eps-deps,R1); Rz(-dpsi,R2); Rx(eps,R3);
    matmul("NN",3,3,3,1.0,R1,R2,0.0,R);
    matmul("NN",3,3,3,1.0,R ,R3,0.0,N); /* N=Rx(-eps)*Rz(-dspi)*Rx(eps) */

    /* greenwich aparent sidereal time (rad) */
    gmst_=utc2gmst(tutc_,erpv[2]);
    gast=gmst_+dpsi*cos(eps);
    gast+=(0.00264*sin(f[4])+0.000063*sin(2.0*f[4]))*AS2R;

    /* eci to ecef transformation matrix */
    Ry(-erpv[0],R1); Rx(-erpv[1],R2); Rz(gast,R3);
    matmul("NN",3,3,3,1.0,R1,R2,0.0,W );
    matmul("NN",3,3,3,1.0,W ,R3,0.0,R ); /* W=Ry(-xp)*Rx(-yp) */
    matmul("NN",3,3,3,1.0,N ,P ,0.0,NP);
    matmul("NN",3,3,3,1.0,R ,NP,0.0,U_); /* U=W*Rz(gast)*N*P */

    for (i=0;i<9;i++) U[i]=U_[i];
    if (gmst) *gmst=gmst_;

//    trace(5,"gmst=%.12f gast=%.12f\n",gmst_,gast);
//    trace(5,"P=\n"); tracemat(5,P,3,3,15,12);
//    trace(5,"N=\n"); tracemat(5,N,3,3,15,12);
//    trace(5,"W=\n"); tracemat(5,W,3,3,15,12);
//    trace(5,"U=\n"); tracemat(5,U,3,3,15,12);
}
/*Set sun, moon, gmst data*/
void trans_epht::setSunMoonPos(double *psun,double *pmoon,double gmst/* =0 */)
{
    for (int i = 0;i < 3;i++)
    {
        m_sunpos[i] = psun[i];
        m_moonpos[i] = pmoon[i];
    }
    m_gmst = gmst;
//    isGetPos = true;
}
bool trans_epht::getSunMoonPos(int Year,int Month,int Day,int HoursInt,int Minutes,double Seconds,double *sunpos,double *moonpos,double *gmst)
{
    int tWeekN = 0;
    double tSecond = 0.0;
    tSecond = YMD2GPSTime(Year,Month,Day,HoursInt,Minutes,Seconds,&tWeekN);
    gtime_t obsGPST,obsUTC;
    obsGPST = gpst2time(tWeekN,tSecond);
    obsUTC = gpst2utc(obsGPST);
    /*Convert UTC to UT1 using erp file*/
    double erpv[5] = {0};
    if (isuseErp)
    {
        geterp(&m_erpData,obsGPST,erpv);
    }
    /*Calculate UT1*/
    sunmoonpos(obsUTC,erpv,sunpos,moonpos,gmst);
    return true;
}
/*Go to GPS time*/
double trans_epht::YMD2GPSTime(int Year,int Month,int Day,int HoursInt,int Minutes,double Seconds,int *GPSWeek, int *day)
{
    double Hours = HoursInt + ((Minutes * 60) + Seconds)/3600.0;
    /*Get JD*/
    double JD = 0.0;
    if(Month<=2)
        JD = (int)(365.25*(Year-1)) + (int)(30.6001*(Month+12+1)) + Day + Hours/24.0 + 1720981.5;
    else
        JD = (int)(365.25*(Year)) + (int)(30.6001*(Month+1)) + Day + Hours/24.0 + 1720981.5;

    int Week = (int)((JD - 2444244.5) / 7);
    int N =(int)(JD + 1.5)%7;
    if (GPSWeek)	*GPSWeek = Week;
    return (N*24*3600 + HoursInt*3600 + Minutes*60 + Seconds);
}

/* convert calendar day/time to time -------------------------------------------
* convert calendar day/time to gtime_t struct
* args   : double *ep       I   day/time {year,month,day,hour,min,sec}
* return : gtime_t struct
* notes  : proper in 1970-2037 or 1970-2099 (64bit time_t)
*-----------------------------------------------------------------------------*/
gtime_t trans_epht::epoch2time(const double *ep)
{
    const int doy[]={1,32,60,91,121,152,182,213,244,274,305,335};
    gtime_t time;
    int days,sec,year=(int)ep[0],mon=(int)ep[1],day=(int)ep[2];

    if (year<1970||2099<year||mon<1||12<mon) return time;

    /* leap year if year%4==0 in 1901-2099 */
    days=(year-1970)*365+(year-1969)/4+doy[mon-1]+day-2+(year%4==0&&mon>=3?1:0);
    sec=(int)floor(ep[5]);
    time.time=(time_t)days*86400+(int)ep[3]*3600+(int)ep[4]*60+sec;
    time.sec=ep[5]-sec;
    return time;
}


/* ura value (m) to ura index ------------------------------------------------*/
int trans_epht::uraindex(double value)
{
    static const double ura_eph[]={
        2.4,3.4,4.85,6.85,9.65,13.65,24.0,48.0,96.0,192.0,384.0,768.0,1536.0,
        3072.0,6144.0,0.0
    };
    int i;
    for (i=0;i<15;i++) if (ura_eph[i]>=value) break;
    return i;
}
/* gpstime to utc --------------------------------------------------------------
* convert gpstime to utc considering leap seconds
* args   : gtime_t t        I   time expressed in gpstime
* return : time expressed in utc
* notes  : ignore slight time offset under 100 ns
*-----------------------------------------------------------------------------*/
gtime_t trans_epht::gpst2utc(gtime_t t)
{
    gtime_t tu;
    int i;
    for (i=0;i<(int)sizeof(leaps)/(int)sizeof(*leaps);i++) {
        tu=timeadd(t,leaps[i][6]);
        if (timediff(tu,epoch2time(leaps[i]))>=0.0) return tu;
    }
    return t;
}
/* utc to gmst -----------------------------------------------------------------
* convert utc to gmst (Greenwich mean sidereal time)
* args   : gtime_t t        I   time expressed in utc
*          double ut1_utc   I   UT1-UTC (s)
* return : gmst (rad)
*-----------------------------------------------------------------------------*/
double trans_epht::utc2gmst(gtime_t t, double ut1_utc)
{
    const double ep2000[]={2000,1,1,12,0,0};
    gtime_t tut,tut0;
    double ut,t1,t2,t3,gmst0,gmst;

    tut=timeadd(t,ut1_utc);
    ut=time2sec(tut,&tut0);
    t1=timediff(tut0,epoch2time(ep2000))/86400.0/36525.0;
    t2=t1*t1; t3=t2*t1;
    gmst0=24110.54841+8640184.812866*t1+0.093104*t2-6.2E-6*t3;
    gmst=gmst0+1.002737909350795*ut;

    return fmod(gmst,86400.0)*MM_PI/43200.0; /* 0 <= gmst <= 2*MM_PI */
}
void trans_epht::time2epoch(gtime_t t, double *ep)
{
    const int mday[]={ /* # of days in a month */
        31,28,31,30,31,30,31,31,30,31,30,31,31,28,31,30,31,30,31,31,30,31,30,31,
        31,29,31,30,31,30,31,31,30,31,30,31,31,28,31,30,31,30,31,31,30,31,30,31
    };
    int days,sec,mon,day;

    /* leap year if year%4==0 in 1901-2099 */
    days=(int)(t.time/86400);
    sec=(int)(t.time-(time_t)days*86400);
    for (day=days%1461,mon=0;mon<48;mon++) {
        if (day>=mday[mon]) day-=mday[mon]; else break;
    }
    ep[0]=1970+days/1461*4+mon/12; ep[1]=mon%12+1; ep[2]=day+1;
    ep[3]=sec/3600; ep[4]=sec%3600/60; ep[5]=sec%60+t.sec;
}
/* time to day and sec -------------------------------------------------------*/
double trans_epht::time2sec(gtime_t time, gtime_t *day)
{
    double ep[6],sec;
    time2epoch(time,ep);
    sec=ep[3]*3600.0+ep[4]*60.0+ep[5];
    ep[3]=ep[4]=ep[5]=0.0;
    *day=epoch2time(ep);
    return sec;
}
//The following reference RTKLAB to obtain the sun coordinates
/* get earth rotation parameter values -----------------------------------------
* get earth rotation parameter values
* args   : erp_t  *erp        I   earth rotation parameters
*          gtime_t time       I   time (gpst)
*          double *erpv       O   erp values {xp,yp,ut1_utc,lod} (rad,rad,s,s/d)
* return : status (1:ok,0:error)
*-----------------------------------------------------------------------------*/
int trans_epht::geterp(const erp_t *erp, gtime_t time, double *erpv)
{
    const double ep[]={2000,1,1,12,0,0};
    double mjd,day,a;
    int i=0,j,k;

    //trace(4,"geterp:\n");

    if (erp->n<=0) return 0;

    mjd=51544.5+(timediff(gpst2utc(time),epoch2time(ep)))/86400.0;

    if (mjd<=erp->data[0].mjd) {
        day=mjd-erp->data[0].mjd;
        erpv[0]=erp->data[0].xp     +erp->data[0].xpr*day;
        erpv[1]=erp->data[0].yp     +erp->data[0].xpr*day;
        erpv[2]=erp->data[0].ut1_utc-erp->data[0].lod*day;
        erpv[3]=erp->data[0].lod;
        return 1;
    }
    if (mjd>=erp->data[erp->n-1].mjd) {
        day=mjd-erp->data[erp->n-1].mjd;
        erpv[0]=erp->data[erp->n-1].xp     +erp->data[erp->n-1].xpr*day;
        erpv[1]=erp->data[erp->n-1].yp     +erp->data[erp->n-1].ypr*day;
        erpv[2]=erp->data[erp->n-1].ut1_utc-erp->data[erp->n-1].lod*day;
        erpv[3]=erp->data[erp->n-1].lod;
        return 1;
    }
    for (j=0,k=erp->n-1;j<=k;) {
        i=(j+k)/2;
        if (mjd<erp->data[i].mjd) k=i-1; else j=i+1;
    }
    //If crossing the border
    if (i >= erp->n - 1)
        i--;

    if (erp->data[i].mjd==mjd-erp->data[i+1].mjd) {
        a=0.5;
    }
    else {
        //a=(mjd-erp->data[i+1].mjd)/(erp->data[i].mjd-mjd-erp->data[i+1].mjd);
        a = (mjd - erp->data[i].mjd)/(erp->data[i+1].mjd - erp->data[i].mjd);
    }
    erpv[0]=(1.0-a)*erp->data[i].xp     +a*erp->data[i+1].xp;
    erpv[1]=(1.0-a)*erp->data[i].yp     +a*erp->data[i+1].yp;
    erpv[2]=(1.0-a)*erp->data[i].ut1_utc+a*erp->data[i+1].ut1_utc;
    erpv[3]=(1.0-a)*erp->data[i].lod    +a*erp->data[i+1].lod;
    return 1;
}
double trans_epht::time2doy(gtime_t t)
{
    double ep[6];
    time2epoch(t,ep);//?
    ep[1]=ep[2]=1.0; ep[3]=ep[4]=ep[5]=0.0;
    return timediff(t,epoch2time(ep))/86400.0+1.0;
}
double trans_epht::interpc(const double coef[], double lat)
{
    int i=(int)(lat/15.0);
    if (i<1) return coef[0]; else if (i>4) return coef[4];
    return coef[i-1]*(1.0-lat/15.0+i)+coef[i]*(lat/15.0-i);
}
void trans_epht::DOP_vlaue(const QVector < SatlitData > &Sat_Dop,double *Dops)
{
    MatrixXd DOP,DDOP;
    int sat_len = Sat_Dop.length();
    DOP.resize(sat_len,4);DDOP.resize(4,4);
    DOP.setZero(sat_len,4);DDOP.setZero(4,4);

    for(int i = 0; i < sat_len;i++)
    {
        SatlitData epochata = Sat_Dop.at(i);
        // EA1
        double EA1,EA2;
        EA1 = epochata.EA[0]*M_PI/180.0;
        EA2 = epochata.EA[1]*M_PI/180.0;
        DOP(i,0)=-cos(EA1)*sin(EA2);DOP(i,1)=-cos(EA1)*cos(EA2);DOP(i,2)=-sin(EA1);DOP(i,3)=1;
    }
    DDOP = (DOP.transpose()*DOP).inverse();
    Dops[0]=sqrt(DDOP(0,0)+DDOP(1,1)+DDOP(2,2)+DDOP(3,3)); /*GDOP*/
    Dops[1]=sqrt(DDOP(0,0)+DDOP(1,1)+DDOP(2,2));           /*PDOP*/
    Dops[2]=sqrt(DDOP(0,0)+DDOP(1,1));                     /*HDOP*/
    Dops[3]=sqrt(DDOP(2,2));                               /*VDOP*/
}
/* string to time --------------------------------------------------------------
* convert substring in string to gtime_t struct
* args   : char   *s        I   string ("... yyyy mm dd hh mm ss ...")
*          int    i,n       I   substring position and width
*          gtime_t *t       O   gtime_t struct
* return : status (0:ok,0>:error)
*-----------------------------------------------------------------------------*/
int trans_epht::str2time(const char *s, int i, int n, gtime_t *t)
{
    double ep[6];
    char str[256],*p=str;

    if (i<0||(int)strlen(s)<i||(int)sizeof(str)-1<i) return -1;
    for (s+=i;*s&&--n>=0;) *p++=*s++;
    *p='\0';
    if (sscanf(str,"%lf %lf %lf %lf %lf %lf",ep,ep+1,ep+2,ep+3,ep+4,ep+5)<6)
        return -1;
    if (ep[0]<100.0) ep[0]+=ep[0]<80.0?2000.0:1900.0;
    *t=epoch2time(ep);
    return 0;
}
//Calculate the annual accumulation date
int trans_epht::YearAccDay(int Year, int Month, int Day)
{

    if(Month == 2 &&(Day <0 || Day > 29))
        return -1;
    if (Month < 1||Month > 12)
        return -1;
    if(Day< 1 || Day > 31)
        return -1;
    int YearDay = 0;
    int YearArray[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
    if((Year%4==0 && Year%100!=0) || Year%400==0)
        YearArray[1]++;
    for(int i = 0;i < Month-1;i++)
    {
        YearDay+=YearArray[i];
    }
    YearDay+=Day;
    return YearDay;
}
double trans_epht::DayAccSec(int hours, int min, int sec)
{
    return hours*3600+min*60+sec;
}
