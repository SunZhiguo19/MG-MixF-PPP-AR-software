#include "trans_coor.h"

trans_coor::trans_coor()
{

}
//Save the phase entanglement of the previous epoch
double trans_coor::getPreEpochWindUp(QVector< SatlitData > &prevEpochSatlitData,int PRN,char SatType)
{
    int preEopchLen = prevEpochSatlitData.length();
    if (0 == preEopchLen) return 0;

    for (int i = 0;i < preEopchLen;i++)
    {
        SatlitData oneSatalite = prevEpochSatlitData.at(i);
        if (PRN == oneSatalite.PRN&&oneSatalite.SatType == SatType)
            return oneSatalite.AntWindup;
    }
    return 0;
}

//Calculate satellite antenna phase winding
double trans_coor::getWindUp(int Year,int Month,int Day,int Hours,int Minuts,double Seconds,
                             double *StaPos,double *RecPos,double &phw,double *psunpos)
{
    gtime_t obsGPST;
    double ep[6] = {(double)Year, (double)Month, (double)Day, (double)Hours, (double)Minuts, (double)Seconds};
    obsGPST = epoch2time(ep);
    windupcorr(obsGPST,StaPos,RecPos,&phw,psunpos);
    return phw;
}

//Calculate satellite antenna phase winding szg
double trans_coor::getWindUpFI(int Year,int Month,int Day,int Hours,int Minuts,double Seconds,
                               double *StaPos,double *RecPos,double &phw,double *psunpos,double *StaVel,double yaw)
{
    gtime_t obsGPST;
    double ep[6] = {(double)Year, (double)Month, (double)Day, (double)Hours, (double)Minuts, (double)Seconds};
    obsGPST = epoch2time(ep);
    windupcorrFI(obsGPST,StaPos,RecPos,&phw,psunpos,StaVel,yaw);
    return phw;
}
/* phase windup correction -----------------------------------------------------
* phase windup correction (ref [7] 5.1.2)
* args   : gtime_t time     I   time (GPST)
*          double  *rs      I   satellite position (ecef) {x,y,z} (m)
*          double  *rr      I   receiver  position (ecef) {x,y,z} (m)
*          double  *phw     IO  phase windup correction (cycle)
* return : none
* notes  : the previous value of phase windup correction should be set to *phw
*          as an input. the function assumes windup correction has no jump more
*          than 0.5 cycle.
*-----------------------------------------------------------------------------*/
void trans_coor::windupcorr(gtime_t time, const double *rs, const double *rr,
    double *phw,double *psunpos)
{
    double ek[3],exs[3],eys[3],ezs[3],ess[3],exr[3],eyr[3],eks[3],ekr[3],E[9];
    double dr[3],ds[3],drs[3],r[3],pos[3],rsun[3],cosp,ph,erpv[5]={0};
    int i;

    //trace(4,"windupcorr: time=%s\n",time_str(time,0));

    /* sun position in ecef */
    if (psunpos)
    {
        rsun[0] = psunpos[0];rsun[1] = psunpos[1];rsun[2] = psunpos[2];
    }
    else
        sunmoonpos(gpst2utc(time),erpv,rsun,NULL,NULL);
    /* unit vector satellite to receiver */
    for (i=0;i<3;i++) r[i]=rr[i]-rs[i];
    if (!normv3(r,ek)) return;

    /* unit vectors of satellite antenna */
    for (i=0;i<3;i++) r[i]=-rs[i];
    if (!normv3(r,ezs)) return;
    for (i=0;i<3;i++) r[i]=rsun[i]-rs[i];
    if (!normv3(r,ess)) return;
    cross3(ezs,ess,r);
    if (!normv3(r,eys)) return;
    cross3(eys,ezs,exs);

    /* unit vectors of receiver antenna */
    ecef2pos(rr,pos);
    xyz2enu(pos,E);
    exr[0]= E[1]; exr[1]= E[4]; exr[2]= E[7]; /* x = north */
    eyr[0]=-E[0]; eyr[1]=-E[3]; eyr[2]=-E[6]; /* y = west  */

    /* phase windup effect */
    cross3(ek,eys,eks);
    cross3(ek,eyr,ekr);
    for (i=0;i<3;i++) {
        ds[i]=exs[i]-ek[i]*dot(ek,exs,3)-eks[i];
        dr[i]=exr[i]-ek[i]*dot(ek,exr,3)+ekr[i];
    }
    cosp=dot(ds,dr,3)/norm(ds,3)/norm(dr,3);

    //RTKLAB
    if      (cosp<-1.0) cosp=-1.0;
    else if (cosp> 1.0) cosp= 1.0;
    ph=acos(cosp)/2.0/MM_PI;
    cross3(ds,dr,drs);
    if (dot(ek,drs,3)<0.0) ph=-ph;

    *phw=ph+floor(*phw-ph+0.5); /* in cycle */

}
//Calculate satellite antenna phase winding szg
double trans_coor::getWindUp_quater(double *StaPos,double *RecPos,double phw,double *StaVel,MatrixXd quater)
{
    double ek[3],exs[3],eys[3],ezs[3],exr[3],eyr[3],eks[3],ekr[3],E[9];
    double dr[3],ds[3],drs[3],r[3],pos[3],cosp,ph;
    /* satellite fixed x,y-vector */
    for (int i = 0;i < 3;i++)
    {
        ezs[i] = quater(2,i);
        eys[i] = quater(1,i);
        exs[i] = quater(0,i);
    }
    /* beta and orbit angle */
    StaVel[0] = StaVel[0]-OMGE*StaPos[1];
    StaVel[1] = StaVel[1]+OMGE*StaPos[0];
    /* unit vector satellite to receiver */
    for (int i=0;i<3;i++) r[i]=RecPos[i]-StaPos[i];
    if (!normv3(r,ek)) return 0;
    /* unit vectors of receiver antenna */
    ecef2pos(RecPos,pos);
    xyz2enu(pos,E);
    exr[0]= E[1]; exr[1]= E[4]; exr[2]= E[7]; /* x = north */
    eyr[0]=-E[0]; eyr[1]=-E[3]; eyr[2]=-E[6]; /* y = west  */

    /* phase windup effect */
    cross3(ek,eys,eks);
    cross3(ek,eyr,ekr);
    for (int i=0;i<3;i++) {
        ds[i]=exs[i]-ek[i]*dot(ek,exs,3)-eks[i];
        dr[i]=exr[i]-ek[i]*dot(ek,exr,3)+ekr[i];
    }
    cosp=dot(ds,dr,3)/norm(ds,3)/norm(dr,3);

    //RTKLAB
    if      (cosp<-1.0) cosp=-1.0;
    else if (cosp> 1.0) cosp= 1.0;
    ph=acos(cosp)/2.0/MM_PI;
    cross3(ds,dr,drs);
    if (dot(ek,drs,3)<0.0) ph=-ph;

    phw=ph+floor(phw-ph+0.5); /* in cycle */
    return phw;
}
//szg
void trans_coor::windupcorrFI(gtime_t time, const double *rs, const double *rr,
    double *phw,double *psunpos,double *StaVel_1,double yaw)
{
    double ek[3],exs[3],es[3],esun[3],eys[3],en[3],ep[3],ex[3],exr[3],eyr[3],eks[3],ekr[3],E[9],n[3],p[3];
    double dr[3],ds[3],drs[3],r[3],pos[3],rsun[3],cosp,ph,erpv[5]={0},StaVel[3];
    double cosy,siny;
    int i;

    /* sun position in ecef */
    if (psunpos)
    {
        rsun[0] = psunpos[0];rsun[1] = psunpos[1];rsun[2] = psunpos[2];
    }
    else
        sunmoonpos(gpst2utc(time),erpv,rsun,NULL,NULL);
    /* beta and orbit angle */
    StaVel[0] = StaVel_1[0]-OMGE*rs[1];
    StaVel[1] = StaVel_1[1]+OMGE*rs[0];
    StaVel[2] = StaVel_1[2];
    cross3(rs,StaVel,n);
    cross3(rsun,n,p);
    if (!normv3(rs,es)||!normv3(rsun,esun)||!normv3(n,en)||
        !normv3(p,ep)) return;
    /* satellite fixed x,y-vector */
    cross3(en,es,ex);
    cosy=cos(yaw);
    siny=sin(yaw);
    for (i=0;i<3;i++) {
        exs[i]=-siny*en[i]+cosy*ex[i];
        eys[i]=-cosy*en[i]-siny*ex[i];
    }


    /* unit vector satellite to receiver */
    for (i=0;i<3;i++) r[i]=rr[i]-rs[i];
    if (!normv3(r,ek)) return;
    /* unit vectors of receiver antenna */
    ecef2pos(rr,pos);
    xyz2enu(pos,E);
    exr[0]= E[1]; exr[1]= E[4]; exr[2]= E[7]; /* x = north */
    eyr[0]=-E[0]; eyr[1]=-E[3]; eyr[2]=-E[6]; /* y = west  */

    /* phase windup effect */
    cross3(ek,eys,eks);
    cross3(ek,eyr,ekr);
    for (i=0;i<3;i++) {
        ds[i]=exs[i]-ek[i]*dot(ek,exs,3)-eks[i];
        dr[i]=exr[i]-ek[i]*dot(ek,exr,3)+ekr[i];
    }
    cosp=dot(ds,dr,3)/norm(ds,3)/norm(dr,3);

    //RTKLAB
    if      (cosp<-1.0) cosp=-1.0;
    else if (cosp> 1.0) cosp= 1.0;
    ph=acos(cosp)/2.0/MM_PI;
    cross3(ds,dr,drs);
    if (dot(ek,drs,3)<0.0) ph=-ph;

    *phw=ph+floor(*phw-ph+0.5); /* in cycle */

}

/* tropospheric delay correction -----------------------------------------------
* compute sbas tropospheric delay correction (mops model)
* args   : gtime_t time     I   time
*          double   *pos    I   receiver position {lat,lon,height} (rad/m)
*          double   *azel   I   satellite azimuth/elavation (rad)
*          double   *var    O   variance of troposphric error (m^2)
* return : slant tropospheric delay (m)
*-----------------------------------------------------------------------------*/
double trans_coor::sbstropcorr(gtime_t time, const double *pos, const double *azel,
                          double *var)
{
    double k1=77.604,k2=382000.0,rd=287.054,gm=9.784,g=9.80665;
    double pos_[3]={0},zh=0.0,zw=0.0;
    int i;
    double c,met[10],sinel=sin(azel[1]),h=pos[2],m;

    if (pos[2]<-100.0||10000.0<pos[2]||azel[1]<=0) {
        *var=0.0;
        return 0.0;
    }
    if (zh==0.0||fabs(pos[0]-pos_[0])>1E-7||fabs(pos[1]-pos_[1])>1E-7||
        fabs(pos[2]-pos_[2])>1.0) {
        getmet(pos[0]*R2D,met);
        c=cos(2.0*MM_PI*(time2doy(time)-(pos[0]>=0.0?28.0:211.0))/365.25);
        for (i=0;i<5;i++) met[i]-=met[i+5]*c;
        zh=1E-6*k1*rd*met[0]/gm;
        zw=1E-6*k2*rd/(gm*(met[4]+1.0)-met[3]*rd)*met[2]/met[1];
        zh*=pow(1.0-met[3]*h/met[1],g/(rd*met[3]));
        zw*=pow(1.0-met[3]*h/met[1],(met[4]+1.0)*g/(rd*met[3])-1.0);
        for (i=0;i<3;i++) pos_[i]=pos[i];
    }
    m=1.001/sqrt(0.002001+sinel*sinel);
    *var=0.12*0.12*m*m;
    return (zh+zw)*m;
}
/* get meterological parameters ----------------------------------------------*/
void trans_coor::getmet(double lat, double *met)
{
    double metprm[][10]={ /* lat=15,30,45,60,75 */
        {1013.25,299.65,26.31,6.30E-3,2.77,  0.00, 0.00,0.00,0.00E-3,0.00},
        {1017.25,294.15,21.79,6.05E-3,3.15, -3.75, 7.00,8.85,0.25E-3,0.33},
        {1015.75,283.15,11.66,5.58E-3,2.57, -2.25,11.00,7.24,0.32E-3,0.46},
        {1011.75,272.15, 6.78,5.39E-3,1.81, -1.75,15.00,5.36,0.81E-3,0.74},
        {1013.00,263.65, 4.11,4.53E-3,1.55, -0.50,14.50,3.39,0.62E-3,0.30}
    };
    int j;
    double a;
    lat=fabs(lat);
    if      (lat<=15.0) for (int i=0;i<10;i++) met[i]=metprm[0][i];
    else if (lat>=75.0) for (int i=0;i<10;i++) met[i]=metprm[4][i];
    else {
        j=(int)(lat/15.0); a=(lat-j*15.0)/15.0;
        for (int i=0;i<10;i++) met[i]=(1.0-a)*metprm[j-1][i]+a*metprm[j][i];
    }
}
/* precise tropospheric model ------------------------------------------------*/
double trans_coor::prectrop(gtime_t time, const double *pos, const double *azel,
                       double trop_pre, double *mf,double *var)
{

    const double zazel[]={0.0,MM_PI/2.0};
    double zhd,m_h,m_w;//

    /* zenith hydrostatic delay */
    zhd=trop_saasmodel(pos,zazel,0.0);

    /* mapping function */
    m_h=tropmapf(time,pos,azel,&m_w);

    /*zenith troposphere gradient*/

    *mf=m_w;
    *var=SQR(0.01);
    return m_h*zhd+m_w*(trop_pre-zhd);
}

double trans_coor::mapf(double el, double a, double b, double c)
{
    double sinel=sin(el);
    return (1.0+a/(1.0+b/(1.0+c)))/(sinel+(a/(sinel+b/(sinel+c))));
}
double trans_coor::nmf(gtime_t time, const double pos[], const double azel[],
                  double *mapfw)
{
    /* ref [5] table 3 */
    /* hydro-ave-a,b,c, hydro-amp-a,b,c, wet-a,b,c at latitude 15,30,45,60,75 */
    const double coef[][5]={
        { 1.2769934E-3, 1.2683230E-3, 1.2465397E-3, 1.2196049E-3, 1.2045996E-3},
        { 2.9153695E-3, 2.9152299E-3, 2.9288445E-3, 2.9022565E-3, 2.9024912E-3},
        { 62.610505E-3, 62.837393E-3, 63.721774E-3, 63.824265E-3, 64.258455E-3},

        { 0.0000000E-0, 1.2709626E-5, 2.6523662E-5, 3.4000452E-5, 4.1202191E-5},
        { 0.0000000E-0, 2.1414979E-5, 3.0160779E-5, 7.2562722E-5, 11.723375E-5},
        { 0.0000000E-0, 9.0128400E-5, 4.3497037E-5, 84.795348E-5, 170.37206E-5},

        { 5.8021897E-4, 5.6794847E-4, 5.8118019E-4, 5.9727542E-4, 6.1641693E-4},
        { 1.4275268E-3, 1.5138625E-3, 1.4572752E-3, 1.5007428E-3, 1.7599082E-3},
        { 4.3472961E-2, 4.6729510E-2, 4.3908931E-2, 4.4626982E-2, 5.4736038E-2}
    };
    const double aht[]={ 2.53E-5, 5.49E-3, 1.14E-3}; /* height correction */

    double y,cosy,ah[3],aw[3],dm,el=azel[0],lat=pos[0]*R2D,hgt=pos[2];

    if (el<=0.0) {
        if (mapfw) *mapfw=0.0;
        return 0.0;
    }
    /* year from doy 28, added half a year for southern latitudes */
    y=(time2doy(time)-28.0)/365.25+(lat<0.0?0.5:0.0);

    cosy=cos(2.0*MM_PI*y);
    lat=fabs(lat);

    for (int i=0;i<3;i++) {
        ah[i]=interpc(coef[i  ],lat)-interpc(coef[i+3],lat)*cosy;
        aw[i]=interpc(coef[i+6],lat);
    }
    /* ellipsoidal height is used instead of height above sea level */
    dm=(1.0/sin(el)-mapf(el,aht[0],aht[1],aht[2]))*hgt/1E3;

    if (mapfw) *mapfw=mapf(el,aw[0],aw[1],aw[2]);

    return mapf(el,ah[0],ah[1],ah[2])+dm;
}

/* troposphere mapping function ------------------------------------------------
* compute tropospheric mapping function by NMF
* args   : gtime_t t        I   time
*          double *pos      I   receiver position {lat,lon,h} (rad,m)
*          double *azel     I   azimuth/elevation angle {az,el} (rad)
*          double *mapfw    IO  wet mapping function (NULL: not output)
* return : dry mapping function
* note   : see ref [5] (NMF) and [9] (GMF)
*          original JGR paper of [5] has bugs in eq.(4) and (5). the corrected
*          paper is obtained from:
*          ftp://web.haystack.edu/pub/aen/nmf/NMF_JGR.pdf
*-----------------------------------------------------------------------------*/
double trans_coor::tropmapf(gtime_t time, const double pos[], const double azel[],
                       double *mapfw)
{
#ifdef IERS_MODEL
    const double ep[]={2000,1,1,12,0,0};
    double mjd,lat,lon,hgt,zd,gmfh,gmfw;
#endif

    if (pos[2]<-1000.0||pos[2]>20000.0) {
        if (mapfw) *mapfw=0.0;
        return 0.0;
    }
#ifdef IERS_MODEL
    mjd=51544.5+(timediff(time,epoch2time(ep)))/86400.0;
    lat=pos[0];
    lon=pos[1];
    hgt=pos[2]-geoidh(pos); /* height in m (mean sea level) */
    zd =M_PI/2.0-azel[0];

    /* call GMF */
    gmf_(&mjd,&lat,&lon,&hgt,&zd,&gmfh,&gmfw);

    if (mapfw) *mapfw=gmfw;
    return gmfh;
#else
    return nmf(time,pos,azel,mapfw); /* NMF */
#endif
}

/* troposphere model -----------------------------------------------------------
* compute tropospheric delay by standard atmosphere and saastamoinen model
* args   : gtime_t time     I   time
*          double *pos      I   receiver position {lat,lon,h} (rad,m)
*          double *azel     I   azimuth/elevation angle {az,el} (rad)
*          double humi      I   relative humidity
* return : tropospheric delay (m)
*-----------------------------------------------------------------------------*/
double trans_coor::trop_saasmodel(const double *pos, const double *azel,double humi)
{
    const double temp0=15.0; /* temparature at sea level */
    double hgt,pres,temp,e,z,trph,trpw;

    if (pos[2]<-100.0||1E4<pos[2]||azel[1]<=0) return 0.0;

    /* standard atmosphere */
    hgt=pos[2]<0.0?0.0:pos[2];

    pres=1013.25*pow(1.0-2.2557E-5*hgt,5.2568);
    temp=temp0-6.5E-3*hgt+273.16;
    e=6.108*humi*exp((17.15*temp-4684.0)/(temp-38.45));

    /* saastamoninen model */
    z=MM_PI/2.0-azel[1];
    trph=0.0022768*pres/(1.0-0.00266*cos(2.0*pos[0])-0.00028*hgt/1E3)/cos(z);
    trpw=0.002277*(1255.0/temp+0.05)*e/cos(z);
    return trph+trpw;
}
/* sbas ionospheric delay correction -------------------------------------------
* compute sbas ionosphric delay correction
* args   : gtime_t  time    I   time
*          nav_t    *nav    I   navigation data
*          double   *pos    I   receiver position {lat,lon,height} (rad/m)
*          double   *azel   I   satellite azimuth/elavation angle (rad)
*          double   *delay  O   slant ionospheric delay (L1) (m)
*          double   *var    O   variance of ionospheric delay (m^2)
* return : status (1:ok, 0:no correction)
* notes  : before calling the function, sbas ionosphere correction parameters
*          in navigation data (nav->sbsion) must be set by callig
*          sbsupdatecorr()
*-----------------------------------------------------------------------------*/
int trans_coor::sbsioncorr(gtime_t time, const nav_t *nav, const double *pos,
                      const double *azel, double *delay, double *var)
{
    const double re=6378.1363,hion=350.0;
    int i,err=0;
    double fp,posp[2],x=0.0,y=0.0,t,w[4]={0};
    const sbsigp_t *igp[4]={0}; /* {ws,wn,es,en} */

//    trace(4,"sbsioncorr: pos=%.3f %.3f azel=%.3f %.3f\n",pos[0]*R2D,pos[1]*R2D,
//          azel[0]*R2D,azel[1]*R2D);

    *delay=*var=0.0;
    if (pos[2]<-100.0||azel[1]<=0) return 1;

    /* ipp (ionospheric pierce point) position */
    fp=ionppp(pos,azel,re,hion,posp);

    /* search igps around ipp */
    searchigp(time,posp,nav->sbsion,igp,&x,&y);

    /* weight of igps */
    if (igp[0]&&igp[1]&&igp[2]&&igp[3]) {
        w[0]=(1.0-x)*(1.0-y); w[1]=(1.0-x)*y; w[2]=x*(1.0-y); w[3]=x*y;
    }
    else if (igp[0]&&igp[1]&&igp[2]) {
        w[1]=y; w[2]=x;
        if ((w[0]=1.0-w[1]-w[2])<0.0) err=1;
    }
    else if (igp[0]&&igp[2]&&igp[3]) {
        w[0]=1.0-x; w[3]=y;
        if ((w[2]=1.0-w[0]-w[3])<0.0) err=1;
    }
    else if (igp[0]&&igp[1]&&igp[3]) {
        w[0]=1.0-y; w[3]=x;
        if ((w[1]=1.0-w[0]-w[3])<0.0) err=1;
    }
    else if (igp[1]&&igp[2]&&igp[3]) {
        w[1]=1.0-x; w[2]=1.0-y;
        if ((w[3]=1.0-w[1]-w[2])<0.0) err=1;
    }
    else err=1;

    if (err) {
//        trace(2,"no sbas iono correction: lat=%3.0f lon=%4.0f\n",posp[0]*R2D,
//              posp[1]*R2D);
        return 0;
    }
    for (i=0;i<4;i++) {
        if (!igp[i]) continue;
        t=timediff(time,igp[i]->t0);
        *delay+=w[i]*igp[i]->delay;
        *var+=w[i]*varicorr(igp[i]->give)*9E-8*fabs(t);
    }
    *delay*=fp; *var*=fp*fp;

//    trace(5,"sbsioncorr: dion=%7.2f sig=%7.2f\n",*delay,sqrt(*var));
    return 1;
}
/* ionosphere model ------------------------------------------------------------
* compute ionospheric delay by broadcast ionosphere model (klobuchar model)
* args   : gtime_t t        I   time (gpst)
*          double *ion      I   iono model parameters {a0,a1,a2,a3,b0,b1,b2,b3}
*          double *pos      I   receiver position {lat,lon,h} (rad,m)
*          double *azel     I   azimuth/elevation angle {az,el} (rad)
* return : ionospheric delay (L1) (m)
*-----------------------------------------------------------------------------*/
double trans_coor::ionmodel(gtime_t t, const double *ion, const double *pos,
                       const double *azel)
{
    const double ion_default[]={ /* 2004/1/1 */
        0.1118E-07,-0.7451E-08,-0.5961E-07, 0.1192E-06,
        0.1167E+06,-0.2294E+06,-0.1311E+06, 0.1049E+07
    };
    double tt,f,psi,phi,lam,amp,per,x;
    int week;

    if (pos[2]<-1E3||azel[0]<=0) return 0.0;
    if (norm(ion,8)<=0.0) ion=ion_default;

    /* earth centered angle (semi-circle) */
    psi=0.0137/(azel[0]/MM_PI+0.11)-0.022;

    /* subionospheric latitude/longitude (semi-circle) */
    phi=pos[0]/MM_PI+psi*cos(azel[1]);
    if      (phi> 0.416) phi= 0.416;
    else if (phi<-0.416) phi=-0.416;
    lam=pos[1]/MM_PI+psi*sin(azel[1])/cos(phi*MM_PI);

    /* geomagnetic latitude (semi-circle) */
    phi+=0.064*cos((lam-1.617)*MM_PI);

    /* local time (s) */
    tt=43200.0*lam+time2gpst(t,&week);
    tt-=floor(tt/86400.0)*86400.0; /* 0<=tt<86400 */

    /* slant factor */
    f=1.0+16.0*pow(0.53-azel[0]/MM_PI,3.0);

    /* ionospheric delay */
    amp=ion[0]+phi*(ion[1]+phi*(ion[2]+phi*ion[3]));
    per=ion[4]+phi*(ion[5]+phi*(ion[6]+phi*ion[7]));
    amp=amp<    0.0?    0.0:amp;
    per=per<72000.0?72000.0:per;
    x=2.0*MM_PI*(tt-50400.0)/per;

    return M_C*f*(fabs(x)<1.57?5E-9+amp*(1.0+x*x*(-0.5+x*x/24.0)):5E-9);
}
/* ionosphere mapping function -------------------------------------------------
* compute ionospheric delay mapping function by single layer model
* args   : double *pos      I   receiver position {lat,lon,h} (rad,m)
*          double *azel     I   azimuth/elevation angle {az,el} (rad)
* return : ionospheric mapping function
*-----------------------------------------------------------------------------*/
double trans_coor::ionmapf(const double *pos, const double *azel)
{
    if (pos[2]>=HION) return 1.0;
    return 1.0/cos(asin((RE_WGS84+pos[2])/(RE_WGS84+HION)*sin(MM_PI/2.0-azel[1])));
}

/* search igps ---------------------------------------------------------------*/
void trans_coor::searchigp(gtime_t time, const double *pos, const sbsion_t *ion,
                      const sbsigp_t **igp, double *x, double *y)
{
    int i,latp[2],lonp[4];
    double lat=pos[0]*R2D,lon=pos[1]*R2D;
    const sbsigp_t *p;

//    trace(4,"searchigp: pos=%.3f %.3f\n",pos[0]*R2D,pos[1]*R2D);

    if (lon>=180.0) lon-=360.0;
    if (-55.0<=lat&&lat<55.0) {
        latp[0]=(int)floor(lat/5.0)*5;
        latp[1]=latp[0]+5;
        lonp[0]=lonp[1]=(int)floor(lon/5.0)*5;
        lonp[2]=lonp[3]=lonp[0]+5;
        *x=(lon-lonp[0])/5.0;
        *y=(lat-latp[0])/5.0;
    }
    else {
        latp[0]=(int)floor((lat-5.0)/10.0)*10+5;
        latp[1]=latp[0]+10;
        lonp[0]=lonp[1]=(int)floor(lon/10.0)*10;
        lonp[2]=lonp[3]=lonp[0]+10;
        *x=(lon-lonp[0])/10.0;
        *y=(lat-latp[0])/10.0;
        if (75.0<=lat&&lat<85.0) {
            lonp[1]=(int)floor(lon/90.0)*90;
            lonp[3]=lonp[1]+90;
        }
        else if (-85.0<=lat&&lat<-75.0) {
            lonp[0]=(int)floor((lon-50.0)/90.0)*90+40;
            lonp[2]=lonp[0]+90;
        }
        else if (lat>=85.0) {
            for (i=0;i<4;i++) lonp[i]=(int)floor(lon/90.0)*90;
        }
        else if (lat<-85.0) {
            for (i=0;i<4;i++) lonp[i]=(int)floor((lon-50.0)/90.0)*90+40;
        }
    }
    for (i=0;i<4;i++) if (lonp[i]==180) lonp[i]=-180;
    for (i=0;i<=MAXBAND;i++) {
        for (p=ion[i].igp;p<ion[i].igp+ion[i].nigp;p++) {
            if (p->t0.time==0) continue;
            if      (p->lat==latp[0]&&p->lon==lonp[0]&&p->give>0) igp[0]=p;
            else if (p->lat==latp[1]&&p->lon==lonp[1]&&p->give>0) igp[1]=p;
            else if (p->lat==latp[0]&&p->lon==lonp[2]&&p->give>0) igp[2]=p;
            else if (p->lat==latp[1]&&p->lon==lonp[3]&&p->give>0) igp[3]=p;
            if (igp[0]&&igp[1]&&igp[2]&&igp[3]) return;
        }
    }
}
/* variance of ionosphere correction (give=GIVEI+1) --------------------------*/
double trans_coor::varicorr(int give)
{
    const double var[15]={
        0.0084,0.0333,0.0749,0.1331,0.2079,0.2994,0.4075,0.5322,0.6735,0.8315,
        1.1974,1.8709,3.326,20.787,187.0826
    };
    return 0<give&&give<=15?var[give-1]:0.0;
}
//UNN3M model ref MG-APP
#ifdef EIGEN_CORE_H
//pBLH:[rad.rad,m];doy：Tday（int）； Satellite altitude angle E ：（rad）
//resulat =[Total delay, zenith dry delay, dry delay projection function, zenith wet delay, wet delay projection function];
double trans_coor::getUNB3mDelay(double *pBLH, double TDay, double E, double *mf, double *ZPD, double *tZHD)
{
    Vector3d BLH;
    BLH[0] = pBLH[0];BLH[1] = pBLH[1];BLH[2] = pBLH[2];
    VectorXd resulat = UNB3M(BLH,TDay,E);
    if(mf) *mf = resulat[4];//Wet delay function
    if(ZPD) *ZPD = resulat[0];
    if(tZHD) *tZHD = resulat[1];// dbug by xiaogongwei 2018.12.24
    return resulat[1]*resulat[2];//Return only dry delay！！！！！！！
    //return resulat[0];//Return total delay
}

/*
 * aigs   :   BLH        I     Latitude, longitude, height
 *            DAYOYEAR   I     Day of year
 *            ELEVRAD    I     Elevation angle (radians)
 * return :   HZD     Hydrostatic zenith delay (m)
              HMF     Hydrostatic Niell mapping function
              WZD     Non-hyd. zenith delay (m)
              WMF     Non-hyd. Niell mapping function
              RTROP   Total slant delay (m)
VectorXd =[RTROP,HZD,HMF,WZD,WMF];[Total delay, zenith dry delay, dry delay projection function, zenith wet delay, wet delay projection function];
*/
VectorXd trans_coor::UNB3M(Vector3d &BLH, double DAYOYEAR, double ELEVRAD)
{
    double LATRAD=BLH[0];
    double HEIGHTM=BLH[2];

    MatrixXd AVG(5,6);

    AVG<<15.0  ,1013.25  ,299.65  ,75.00  ,6.30  ,2.77,
            30.0  ,1017.25  ,294.15  ,80.00 , 6.05  ,3.15,
            45.0  ,1015.75 , 283.15  ,76.00  ,5.58 , 2.57,
            60.0 , 1011.75  ,272.15  ,77.50  ,5.39  ,1.81,
            75.0  ,1013.00 , 263.65 , 82.50 , 4.53 , 1.55;
    MatrixXd AMP(5,6);
    AMP<< 15.0  , 0.00  , 0.00 ,  0.00  ,0.00,  0.00,
            30.0  ,-3.75  , 7.00   ,0.00 , 0.25 , 0.33,
            45.0  ,-2.25  ,11.00  ,-1.00 , 0.32 , 0.46,
            60.0  ,-1.75  ,15.00  ,-2.50  ,0.81 , 0.74,
            75.0  ,-0.50,  14.50 ,  2.50  ,0.62 , 0.30;

    double EXCEN2 = 6.6943799901413e-03;
    double MD     = 28.9644;
    double MW     = 18.0152;
    double K1     = 77.604;
    double K2     = 64.79;
    double K3     = 3.776e5;
    double R      = 8314.34;
    double C1     = 2.2768e-03;
    double K2PRIM = K2 - K1*(MW/MD);
    double RD     = R / MD;

    double DOY2RAD=(0.31415926535897935601e01)*2/365.25;
    MatrixXd ABC_AVG(5,4);
    ABC_AVG<<15.0 ,1.2769934e-3 ,2.9153695e-3 ,62.610505e-3,
            30.0, 1.2683230e-3 ,2.9152299e-3 ,62.837393e-3,
            45.0, 1.2465397e-3 ,2.9288445e-3 ,63.721774e-3,
            60.0 ,1.2196049e-3 ,2.9022565e-3, 63.824265e-3,
            75.0 ,1.2045996e-3 ,2.9024912e-3 ,64.258455e-3;
    MatrixXd ABC_AMP(5,4);
    ABC_AMP<<15.0, 0.0    ,      0.0      ,    0.0   ,
            30.0 ,1.2709626e-5 ,2.1414979e-5, 9.0128400e-5 ,
            45.0, 2.6523662e-5 ,3.0160779e-5 ,4.3497037e-5 ,
            60.0 ,3.4000452e-5 ,7.2562722e-5 ,84.795348e-5 ,
            75.0 ,4.1202191e-5 ,11.723375e-5 ,170.37206e-5;

    double A_HT = 2.53e-5;
    double B_HT= 5.49e-3;
    double C_HT = 1.14e-3;
    double HT_TOPCON = 1 + A_HT/(1 + B_HT/(1 + C_HT));

    MatrixXd ABC_W2P0(5,4);
    ABC_W2P0<< 15.0 ,5.8021897e-4 ,1.4275268e-3 ,4.3472961e-2,
            30.0 ,5.6794847e-4 ,1.5138625e-3 ,4.6729510e-2,
            45.0, 5.8118019e-4 ,1.4572752e-3, 4.3908931e-2,
            60.0 ,5.9727542e-4 ,1.5007428e-3 ,4.4626982e-2,
            75.0 ,6.1641693e-4 ,1.7599082e-3, 5.4736038e-2;

    double LATDEG = LATRAD * 180.0 / M_PI;
    double  TD_O_Y = DAYOYEAR;
    if (LATDEG<0)
    {
        TD_O_Y = TD_O_Y + 182.625;
    }
    double COSPHS = cos((TD_O_Y - 28) * DOY2RAD );
    double LAT = abs( LATDEG );
    double P1=0,P2=0,M=0;
    if (LAT>=75)
    {
        P1=5;
        P2=5;
        M=0;
    }
    else if (LAT<=15)
    {
        P1=1;
        P2=1;
        M=0;
    }
    else
    {
        P1 = int((LAT - 15)/15) + 1;
        P2 = P1 + 1;
        double aa=LAT - AVG(P1-1,0);
        double bb=AVG(P2-1,0) - AVG(P1-1,0);
        M = (aa ) / ( bb );
    }

    double PAVG = M * ( AVG(P2-1,1) - AVG(P1-1,1) ) + AVG(P1-1,1);
    double TAVG = M * ( AVG(P2-1,2) - AVG(P1-1,2) ) + AVG(P1-1,2);
    double EAVG = M * ( AVG(P2-1,3) - AVG(P1-1,3) ) + AVG(P1-1,3);
    double BETAAVG   = M * ( AVG(P2-1,4) - AVG(P1-1,4 ) )+ AVG(P1-1,4);
    double LAMBDAAVG = M * ( AVG(P2-1,5) - AVG(P1-1,5) ) + AVG(P1-1,5);

    double PAMP = M * ( AMP(P2-1,1) - AMP(P1-1,1) ) + AMP(P1-1,1);
    double TAMP = M * ( AMP(P2-1,2) - AMP(P1-1,2 ) )+ AMP(P1-1,2);
    double EAMP = M * ( AMP(P2-1,3) - AMP(P1-1,3) ) + AMP(P1-1,3);
    double BETAAMP   = M * (AMP(P2-1,4) - AMP(P1-1,4)) + AMP(P1-1,4);
    double LAMBDAAMP = M * ( AMP(P2-1,5) - AMP(P1-1,5) ) + AMP(P1-1,5);

    double P0 = PAVG - PAMP * COSPHS;
    double T0 = TAVG - TAMP * COSPHS;
    double E0 = EAVG - EAMP * COSPHS;
    double BETA = BETAAVG - BETAAMP * COSPHS;
    BETA   = BETA / 1000;
    double LAMBDA = LAMBDAAVG - LAMBDAAMP * COSPHS;

    double ES = 0.01 * exp(1.2378847e-5 * (pow(T0 ,2) )- 1.9121316e-2 * T0 + 3.393711047e1 - 6.3431645e3 * (pow(T0 ,-1)));
    double FW = 1.00062 + 3.14e-6 * P0 + 5.6e-7 * (pow((T0 - 273.15) , 2));
    E0 = (E0 / 1.00e2) * ES * FW;

    double EP = 9.80665 / 287.054 / BETA;

    double T = T0 - BETA * HEIGHTM;
    double P = P0 * pow(( T / T0 ) , EP);
    double E = E0 *pow(( T / T0 ),( EP * (LAMBDA+1) ));

    double GEOLAT = atan((1.0-EXCEN2)*tan(LATRAD));
    double DGREF = 1.0 - 2.66e-03*cos(2.0*GEOLAT) - 2.8e-07*HEIGHTM;
    double GM    = 9.784 * DGREF;
    double DEN   = ( LAMBDA + 1.0 ) * GM;

    double TM  = T * (1 - BETA * RD / DEN);

    double HZD = C1 / DGREF * P;

    double WZD = 1.0e-6 * ( K2PRIM + K3/TM) * RD * E/DEN;

    double A_AVG = M * ( ABC_AVG(P2-1,1) - ABC_AVG(P1-1,1) ) + ABC_AVG(P1-1,1);
    double B_AVG = M * ( ABC_AVG(P2-1,2) - ABC_AVG(P1-1,2) ) + ABC_AVG(P1-1,2);
    double C_AVG = M * ( ABC_AVG(P2-1,3) - ABC_AVG(P1-1,3) ) + ABC_AVG(P1-1,3);

    double A_AMP = M * ( ABC_AMP(P2-1,1) - ABC_AMP(P1-1,1) ) + ABC_AMP(P1-1,1);
    double B_AMP = M * ( ABC_AMP(P2-1,2) - ABC_AMP(P1-1,2) ) + ABC_AMP(P1-1,2);
    double C_AMP = M * ( ABC_AMP(P2-1,3) - ABC_AMP(P1-1,3) ) + ABC_AMP(P1-1,3);

    double A = A_AVG - A_AMP * COSPHS;
    double B = B_AVG - B_AMP * COSPHS;
    double C = C_AVG - C_AMP * COSPHS;

    double SINE = sin(ELEVRAD);

    double ALPHA  = B/(SINE + C );
    double GAMMA  = A/(SINE + ALPHA);
    double TOPCON = (1 + A/(1 + B/(1 + C)));
    double HMF    = TOPCON / ( SINE + GAMMA );

    ALPHA  = B_HT/( SINE + C_HT );
    GAMMA  = A_HT/( SINE + ALPHA);
    double HT_CORR_COEF = 1/SINE - HT_TOPCON/(SINE + GAMMA);
    double HT_CORR      = HT_CORR_COEF * HEIGHTM / 1000;
    HMF          = HMF + HT_CORR;

    A = M * ( ABC_W2P0(P2-1,1) - ABC_W2P0(P1-1,1) ) + ABC_W2P0(P1-1,1);
    B = M * ( ABC_W2P0(P2-1,2) - ABC_W2P0(P1-1,2) ) + ABC_W2P0(P1-1,2);
    C = M * ( ABC_W2P0(P2-1,3) - ABC_W2P0(P1-1,3)) + ABC_W2P0(P1-1,3);

    ALPHA = B/( SINE + C );
    GAMMA = A/( SINE + ALPHA);
    TOPCON = (1 + A/(1 + B/(1 + C)));
    double WMF    = TOPCON / ( SINE + GAMMA );
    double RTROP=HZD*HMF+WZD*WMF;

    VectorXd rr(5);
    rr<<RTROP,HZD,HMF,WZD,WMF;
    return rr;
}

#endif
//Computational relativistic effect
double trans_coor::getRelativty(char SatType, double *pSatXYZ,double *pRecXYZ,double *pSatdXYZ)
{
    /*double c = 299792458.0;
    double dltaP = -2*(pSatXYZ[0]*pSatdXYZ[0] + pSatdXYZ[1]*pdXYZ[1] + pSatXYZ[2]*pSatdXYZ[2]) / c;*/
    double b[3] = {0},a = 0,R = 0,Rs = 0,Rr = 0,v_light = 299792458.0,dltaP = 0;
    b[0] = pRecXYZ[0] - pSatXYZ[0];
    b[1] = pRecXYZ[1] - pSatXYZ[1];
    b[2] = pRecXYZ[2] - pSatXYZ[2];
    a = pSatXYZ[0]*pSatdXYZ[0] + pSatXYZ[1]*pSatdXYZ[1] + pSatXYZ[2]*pSatdXYZ[2];
    R=norm(b,3);
    Rs = norm(pSatXYZ,3);
    Rr = norm(pRecXYZ,3);

    double oldM_GM = 3.986005e14, old_We = 7.2921151467E-5;// GPS
    switch(SatType)
    {
    case 'G':   oldM_GM = 3.986005e14;  old_We = 7.2921151467E-5;  break;
    case 'R':   oldM_GM = 3.9860044E14;  old_We = 7.292115E-5;  break;
    case 'E':   oldM_GM = 3.986004418E14; old_We = 7.2921151467E-5;   break;
    case 'C':   oldM_GM = 3.986004418E14;  old_We = 7.292115E-5;  break;
    }
    double gravity_delay = 0.0;
    gravity_delay =  -(2*oldM_GM/(v_light*v_light))*qLn((Rs+Rr+R)/(Rs+Rr-R));
    dltaP=-2*a/M_C + gravity_delay;
    return dltaP;//m
}
//Earth autobiography correction
double trans_coor::getSagnac(double X,double Y,double Z,double *approxRecvXYZ)
{//Calculate the autobiographic correction of the earth
    double dltaP = M_We*((X - approxRecvXYZ[0])*Y - (Y - approxRecvXYZ[1])*X)/M_C;
    return -dltaP;//Returns the opposite number such that p = p' + dltaP; can be added directly
}
//Calculate EA, E: satellite elevation angle, A: azimuth
void trans_coor::getSatEA(double *pXYZ,double *approxRecvXYZ,double *EA)
{//Calculate EA//BUG occurs, because when calculating XYZ to BLH, L (earth longitude) is actually opposite when y < 0, x > 0.L = -atan(y/x) error should be L = -atan(-y/x)
    double pSAZ[3] = {0};
    XYZ2SAZ(pXYZ,pSAZ,approxRecvXYZ);//BUG occurs
    EA[0] = (MM_PI/2 - pSAZ[2])*360/(2*MM_PI);
    EA[1] = pSAZ[1]*360/(2*MM_PI);
}
void trans_coor::trop_delay(gtime_t time, double *pBLH, double TDay, double *E, double trop_pre, int trop_mode, double *mf, double *ZHD_s, double *var)
{
    double tropDelayH = 0, tropDelayP = 0, tropDelayZHD = 0;
    switch (trop_mode) {
    case trop_UNB3m:
        tropDelayH = getUNB3mDelay(pBLH,TDay,E[0],mf, &tropDelayP, &tropDelayZHD);
        break;
    case trop_saat_NMV:
        tropDelayH = prectrop(time,pBLH,E,trop_pre,mf,var);
        break;
    default:
        break;
    }
    // juge tropDely is nan or no Meaningless
    if(qAbs(tropDelayH) > 100  || qAbs(pBLH[2]) > 50000)
        tropDelayH = 1e-6;
    else if(!isnormal(tropDelayH))
        tropDelayH = 1e-6;
    if(ZHD_s) *ZHD_s = tropDelayH;
}
/* 2023/4/11 lfh
 * 北斗2卫星多径计算 */
void trans_coor::bd2_multipath(QVector< SatlitData > &Sats, const corr_opt *opt_corr)
{
#if 1
    const static double IGSOCOEF[3][10]={		/* m */
        {-0.55,-0.40,-0.34,-0.23,-0.15,-0.04,0.09,0.19,0.27,0.35},  /* B1 */
        {-0.71,-0.36,-0.33,-0.19,-0.14,-0.03,0.08,0.17,0.24,0.33},	/* B2 */
        {-0.27,-0.23,-0.21,-0.15,-0.11,-0.04,0.05,0.14,0.19,0.32},	/* B3 */
    };
    const static double MEOCOEF[3][10]={		/* m */
        {-0.47,-0.38,-0.32,-0.23,-0.11,0.06,0.34,0.69,0.97,1.05},	/* B1 */
        {-0.40,-0.31,-0.26,-0.18,-0.06,0.09,0.28,0.48,0.64,0.69},	/* B2 */
        {-0.22,-0.15,-0.13,-0.10,-0.04,0.05,0.14,0.27,0.36,0.47},	/* B3 */
    };
    int prn,b;
    double el,dmp[3]={0.0},a;

    for (int i=0; i<Sats.length(); i++) {
        if (Sats[i].BDS_2flag == false)
            continue;

        prn=Sats[i].PRN;
        el=Sats[i].EA[0];
        if (el < opt_corr->m_CutAngle)
            continue;

        a=el*0.1;
        b=(int)a;
        if ((prn>=6&&prn<=10)||(prn==13||prn==16)) { // IGSO(C06, C07, C08, C09, C10,C13,C16)
            if (b<0) {
                for (int j=0;j<3;j++) dmp[j]=IGSOCOEF[j][0];
            }
            else if (b>=9) {
                for (int j=0;j<3;j++) dmp[j]=IGSOCOEF[j][9];
            }
            else {
                for (int j=0;j<3;j++) dmp[j]=IGSOCOEF[j][b]*(1.0-a+b)+IGSOCOEF[j][b+1]*(a-b);
            }
        }
        else if (prn==11||prn==12||prn==14) {   // MEO(C11, C12, C13, C14)
            if (b<0) {
                for (int j=0;j<3;j++) dmp[j]=MEOCOEF[j][0];
            }
            else if (b>=9) {
                for (int j=0;j<3;j++) dmp[j]=MEOCOEF[j][9];
            }
            else {
                for (int j=0;j<3;j++) dmp[j]=MEOCOEF[j][b]*(1.0-a+b)+MEOCOEF[j][b+1]*(a-b);
            }
        }
        if(Sats[i].CodeP[0]!=0.0) Sats[i].CodeP[0]+=dmp[0];	/* B1 */
        if(Sats[i].CodeP[2]!=0.0) Sats[i].CodeP[2]+=dmp[1];	/* B2 */
        if(Sats[i].CodeP[1]!=0.0) Sats[i].CodeP[1]+=dmp[2];	/* B3 */

//        qDebug() << Sats[i].EpochNum << Sats[i].PRN << dmp[0] << dmp[1] << dmp[2];
    }
#else
    const static double geo_coef[3][3]={
        {-0.436, 1.158, -0.333}, /*B1: a1 a2 a3*/
        {-0.275, 1.087, -0.452}, /*B2: */
        {-0.048, 0.566, -0.185}, /*B3: */
    };
    const static double igso_coef[3][3]={
        {-0.590, 1.624, -0.645},
        {-0.257, 0.995, -0.381},
        {-0.102, 0.748, -0.307},
    };
    const static double meo_coef[3][3]={
        {-0.946, 2.158, -0.642},
        { 0.598, 1.635, -0.556},
        {-0.177, 0.652, -0.178},
    };
    int prn;
    double el,mp=0.0;

    for (int i=0; i<Sats.length(); i++) {
        if (Sats[i].BDS_2flag == false)
            continue;

        prn=Sats[i].PRN;
        el=Sats[i].EA[0]*D2R;   /* el:rad */
        if(prn <= 5){  /*GEO C01 C02 C03 C04 C05*/
            /*B1I*/
            if(Sats[i].CodeP[0]!=0.0){
                mp=geo_coef[0][0]*el+geo_coef[0][1]*SQR(el)+geo_coef[0][2]*SQR(el)*el;
                Sats[i].CodeP[0]+=mp;
            }
            /*B2I*/
            if(Sats[i].CodeP[2]!=0.0){
                mp=geo_coef[1][0]*el+geo_coef[1][1]*SQR(el)+geo_coef[1][2]*SQR(el)*el;
                Sats[i].CodeP[2]+=mp;
            }
            /*B3I*/
            if(Sats[i].CodeP[1]!=0.0){
                mp=geo_coef[2][0]*el+geo_coef[2][1]*SQR(el)+geo_coef[2][2]*SQR(el)*el;
                Sats[i].CodeP[1]+=mp;
            }
        }
        else if((prn>=6&&prn<=10)||(prn==13||prn==16)){ /*IGSO C06 C07 C08 C09 C10 C13 C14*/
            /*B1I*/
            if(Sats[i].CodeP[0]!=0.0){
                mp=igso_coef[0][0]*el+igso_coef[0][1]*SQR(el)+igso_coef[0][2]*SQR(el)*el;
                Sats[i].CodeP[0]+=mp;
            }
            /*B2I*/
            if(Sats[i].CodeP[2]!=0.0){
                mp=igso_coef[1][0]*el+igso_coef[1][1]*SQR(el)+igso_coef[1][2]*SQR(el)*el;
                Sats[i].CodeP[2]+=mp;
            }
            /*B3I*/
            if(Sats[i].CodeP[1]!=0.0){
                mp=igso_coef[2][0]*el+igso_coef[2][1]*SQR(el)+igso_coef[2][2]*SQR(el)*el;
                Sats[i].CodeP[1]+=mp;
            }
        }
        else if(prn==11||prn==12||prn==14){ /*MEO C11 C12 C14*/
            /*B1I*/
            if(Sats[i].CodeP[0]!=0.0){
                mp=meo_coef[0][0]*el+meo_coef[0][1]*SQR(el)+meo_coef[0][2]*SQR(el)*el;
                Sats[i].CodeP[0]+=mp;
            }
            /*B2I*/
            if(Sats[i].CodeP[2]!=0.0){
                mp=meo_coef[1][0]*el+meo_coef[1][1]*SQR(el)+meo_coef[1][2]*SQR(el)*el;
                Sats[i].CodeP[2]+=mp;
            }
            /*B3I*/
            if(Sats[i].CodeP[1]!=0.0){
                mp=meo_coef[2][0]*el+meo_coef[2][1]*SQR(el)+meo_coef[2][2]*SQR(el)*el;
                Sats[i].CodeP[1]+=mp;
            }
        }
    }
#endif
}
