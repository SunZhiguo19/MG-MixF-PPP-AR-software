#ifndef POS_ALGORITH_H
#define POS_ALGORITH_H
/*Necessary header file*/
#include <QVector>
#include <QString>
#include <QDir>
#include <QDebug>
#include <QFile>
#include <QRegExp>
#include <qmath.h>
#include <QTime>
#include <QFileInfo>
#include <QMessageBox>
#include <iostream>
#include <QPoint>
#include <numeric>
// next include model is for GUI
#include <QTextEdit>
#include <QApplication>
#include <QTextCursor>
#include <QTime>
#include <QCoreApplication>
using namespace std;
/*Eigen libraries
 * (some classes are independent without Eigen)*/
#include <Eigen/Dense>
using namespace Eigen;

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <ctype.h>

#ifdef WIN32
#include <winsock2.h>
#include <windows.h>
#else
#include <pthread.h>
#endif

/*print error message*/
/*qDebug()<<erroInfo<<endl;*/
#define ErroTrace(erroInfo) {QMessageBox::critical(NULL, "Error", erroInfo, QMessageBox::Yes);}
#define ErrorMsg(myerromsg) \
{char buff[256]; sprintf(buff, "%s, line:%d, fuction:%s", __FILE__, __LINE__, __FUNCTION__); myerromsg = "----"+QString(buff)+"----";}/*__func__ or __FUNCTION__*/
/*defines system newline characters*/
#if defined(_WIN32)
/*Windows system*/
static QString ENDLINE = "\r\n";
static QString PATHSEG = "/";
static bool SYSTEMTYPE = 0;
#else
/*Linux or MAC*/
static QString ENDLINE = "\n";
static QString PATHSEG = "/";
static bool SYSTEMTYPE = 1;
#endif
#ifdef WIN_DLL
#define EXPORT __declspec(dllexport) /* for Windows DLL */
#else
#define EXPORT
#endif
/*Mathematical Character Definition */
#define MM_PI 3.1415926535897932385/* pi */
#define D2R         (MM_PI/180.0)          /* deg to rad */
#define R2D         (180.0/MM_PI)          /* rad to deg */
#define M_C 299792458.0 /* speed of light (m/s) */
#define M_GM 3.986005e14 /*Earth's gravitational constant (m)(old value:3.986005e14)*/
#define M_GMK 398600.5 /* The Earth's Gravitational Constant (km)*/
#define M_GMK_GLO 398600.44 /* The Earth's Gravitational Constant (km)*/
#define M_We 0.000072921151467 /* Earth rotation angular velocity*/
#define M_We_OLD 0.00007292115 /* Earth rotation angular velocity*/
#define M_Re 6378137  /* Reference ellipse length radius (m) */
#define M_ReK 6378.137 /* Reference ellipse length radius (m) */
#define M_C20 -0.001082657 /* The Second Harmonic Coefficient of Gravity Potential */
#define M_nan 1e-15
#define M_Zgama_P_square  (3.0*3.0) /* (0.3*0.3)3.0*3.0 Deionospheric Composite P Code Variance */
#define M_IR  0.05 /*ionospheric residual M*/
/*Configuration Selection*/
#define POS_RT_PPP         1   /*Post-processing ppp model*/
#define POS_PP_PPP         2   /*real time ppp model*/
#define POS_RT_PPP_AR      3   /*real time ppp-ar model*/
#define POS_Real_PPP_AR    4   /*real time ppp-ar model*/
#define POS_PP_PPP_AR_1    5   /*Post-processing ppp-ar Integer clock model*/
#define POS_PP_PPP_AR_2    6   /*Post-processing ppp-ar FCB model*/
#define POS_UDUC_PPP_AR    7
#define POS_SPP            8   /*Standard single point positioning  model*/
#define POS_RT_PPP_B2B     9
#define Comb_IF_mode       1   /*iono-free combination model*/
#define Comb_UD_mode       2   /*undifferenced and uncombined observations model*/
/*filter model*/
#define SPP_SRIF           1    /*Single point positioning filtering mode*/
#define PPP_STATIC_SRIF    2    /*Static least square filtering mode for precise point positioning*/
#define PPP_KINEMATIC_SRIF 3    /*Dynamic least squares filtering mode for precise point positioning*/
#define PPP_STATIC_KALM    4    /*Dynamic Kalman filtering mode for precise point positioning*/
#define PPP_KINEMATIC_KALM 5    /*Static Kalman filtering mode for precise point positioning*/
/* select frequence */
#define double_fre         2
#define triple_fre         3
#define four_fre           4
#define five_fre           5
#define hybrid_fre         1
/*frequency */
#define fixed_frequency    1
#define mixed_frequency    2
/* select Wide lane and Exel wide lane resolution mode*/
#define poor_ambiguity     1
#define HMW_combinetion    2
#define filter_estimate    3
#define Do_not_consider_ionospheric 4
/*trop model*/
#define trop_UNB3m         1    /*MG_APP unb3m model*/
#define trop_saat_NMV      2    /* saat model*/
/*Different angle of altitude weighting models*/
#define stochastic_model_1   0  /*sine*/
#define stochastic_model_2   1  /*cosine*/
#define stochastic_model_3   2  /*Sinusoidal*/
#define stochastic_model_4   3  /*Panda*/
/*HMW Combination Wide Lane with Fixed Ambiguity Mark Position*/
#define WL_ambiguity_flag   0  /*sine*/
#define EWL23_ambiguity_flag   1  /*cosine*/
#define EWL24_ambiguity_flag   2  /*Sinusoidal*/
#define EWL25_ambiguity_flag   3  /*Panda*/
/*par subset*/
#define stand_post_res 0
#define angle_screen 1
#define Adop_demet_secreen 2
#define post_resudual 3
#define data_and_model_driven 5
#define Boost_Par 4
#define _weight_ranking 6
#define full_ambiguity_fixed 7
#define Diagonal_var 8
/*system error select est*/
#define ISBs_mode 0
#define only_est  1
/*ref satellite select*/
#define Max_el_sat 0
#define Min_vc_sat 1
#define Max_su_sat 2
#define Min_adop_sat 3
/*quality control method*/
#define IGG_III_method  1
#define Classic_3cigma_method  2
/*Define the RTKlib variable*/
#define D2R         (MM_PI/180.0)          /* deg to rad */
#define AU          149597870691.0      /* 1 AU (m) */
#define AS2R        (D2R/3600.0)        /* arc sec to radian */
#define RE_WGS84    6378137.0           /* earth semimajor axis (WGS84) (m) */
#define FE_WGS84    (1.0/298.257223563) /* earth flattening (WGS84) */
static double elipsePara[6] =  {RE_WGS84,6356752.3142,6399593.6258,FE_WGS84,0.00669437999014132,0.00673949674227};
#define SYS_NONE    0x00                /* navigation system: none */
#define SYS_GPS     0x01                /* navigation system: GPS */
#define SYS_GLO     0x02                /* navigation system: GLONASS */
#define SYS_GAL     0x04                /* navigation system: Galileo */
#define SYS_BD2     0x08                /* navigation system: BeiDou 2 */
#define SYS_BD3     0x16                /* navigation system: BeiDou 2 */
/*GPS system parameters*/

#define SQR(x)   ((x)*(x))

#define RE_GLO   6378136.0        /* radius of earth (m)            ref [2] */
#define MU_GPS   3.9860050E14     /* gravitational constant         ref [1] */
#define MU_GLO   3.9860044E14     /* gravitational constant         ref [2] */
#define MU_GAL   3.986004418E14   /* earth gravitational constant   ref [7] */
#define MU_CMP   3.986004418E14   /* earth gravitational constant   ref [9] */
#define J2_GLO   1.0826257E-3     /* 2nd zonal harmonic of geopot   ref [2] */
#define OMGE     7.2921151467E-5  /* earth angular velocity (IS-GPS) (rad/s)*/
#define OMGE_GLO 7.292115E-5      /* earth angular velocity (rad/s) ref [2] */
#define OMGE_GPS 7.2921151467E-5  /* earth angular velocity (rad/s) ref [7] */
#define OMGE_CMP 7.292115E-5      /* earth angular velocity (rad/s) ref [9] */

#define A_ref_MEO 27906100
#define A_ref_IGSO_GEO 42162200

#define SIN_5 -0.0871557427476582 /* sin(-5.0 deg) */
#define COS_5  0.9961946980917456 /* cos(-5.0 deg) */

#define ERREPH_GLO 5.0            /* error of glonass ephemeris (m) */
#define TSTEP    30.0             /* integration step glonass ephemeris (s) */
#define RTOL_KEPLER 1E-13         /* relative tolerance for Kepler equation */

#define DEFURASSR 0.15            /* default accurary of ssr corr (m) */
#define MAXECORSSR 10.0           /* max orbit correction of ssr (m) */
#define MAXCCORSSR (1E-6*M_C)  /* max clock correction of ssr (m) */
#define MAXAGESSR 180.0            /* max age of ssr orbit and clock (s) */
#define MAXAGESSR_HRCLK 10.0      /* max age of ssr high-rate clock (s) */
#define STD_BRDCCLK 30.0          /* error of broadcast clock (m) */
#define STD_GAL_NAPA 500.0        /* error of galileo ephemeris for NAPA (m) */

#define MAX_ITER_KEPLER 30        /* max number of iteration of Kelpler */

/*Lx wavelength*/
static double g_GPSFrq[6] = {0, 1.57542e9, 1.2276e9, 0, 0, 1.17645e9};
#define M_GPSLamta(k)  (M_C/g_GPSFrq[k])

/*GLONASS system parameters*/
/*Frequency Number Corresponding to Glonass PRN*/
static int g_GlonassK[24] = {1,-4,5,6,1,-4,5,6,-2,-7,0,-1,-2,-7,0,-1,4,-3,3,2,4,-3,3,2};
#define M_GLONASSF1(k) (1602 + k*0.5625)*1e6
#define M_GLONASSF2(k) (1246 + k*0.4375)*1e6
/*GLONASS Lx Wavelength Corresponding to Frequency Number*/
#define M_GLOSSLamta1k(k) (M_C/((1602 + k*0.5625)*1e6))
#define M_GLOSSLamta2k(k) (M_C/((1246 + k*0.4375)*1e6))


/*BDS system parameters*/
/*Storage Beidou Frequency*/
static double g_BDSFrq[9] = {0, 1.57542e9, 1.561098e9, 1.57542e9, 0, 1.17645e9, 1.26852e9, 1.20714e9, 1.191795e9};
#define M_BDSLamtak(k) (M_C/g_BDSFrq[k])/*Beidou wavelength*/

/*Galieo system parameters*/
static double g_GalieoFrq[9] = {0, 1.57542e9, 0, 0, 0, 1.17645e9, 1.27875e9, 1.20714e9, 1.191795e9};/*Galieo Band 1 5 6 7 8 Frequency*/
#define M_GalieoLamtak(k) (M_C/g_GalieoFrq[k])/*Galieo wavelength*/

/*Calculate LL3*/
#define M_GetLamta3(F1,F2) ( (F1*M_C)/(F1*F1 - F2*F2) - (F2*M_C)/(F1*F1 - F2*F2) )

/*eph update time */
#define MAXDTOE_GPS    7200.0
#define MAXEPHO_BDS    3600.0
#define MAXDTOE_BDS    3800.0
#define MAXDTOE_GLO    1800.0           /* max time difference to GLONASS Toe (s) */
#define MAXDTOE_GLE    600.0
#define MAXDTOE_QZS    7200.0              /* max time difference to QZSS Toe (s) */
#define MAXDTOE_GALE   14400.0             /* max time difference to Galileo Toe (s) */
#define MAXDTOE_CMP    21600.0             /* max time difference to BeiDou Toe (s) */

#define MAXDTOE_SBS    360.0               /* max time difference to SBAS Toe (s) */
#define MAXDTOE_S      86400.0             /* max time difference to ephem toe (s) for other */

#define HION        350000.0            /* ionosphere height (m) */

#define FREQ1       1.57542E9           /* L1/E1  frequency (Hz) */
#define FREQ2       1.22760E9           /* L2     frequency (Hz) */
#define FREQ5       1.17645E9           /* L5/E5a frequency (Hz) */
#define FREQ6       1.27875E9           /* E6/LEX frequency (Hz) */
#define FREQ7       1.20714E9           /* E5b    frequency (Hz) */
#define FREQ8       1.191795E9          /* E5a+b  frequency (Hz) */
#define FREQ9       2.492028E9          /* S      frequency (Hz) */
#define FREQ1_GLO   1.60200E9           /* GLONASS G1 base frequency (Hz) */
#define DFRQ1_GLO   0.56250E6           /* GLONASS G1 bias frequency (Hz/n) */
#define FREQ2_GLO   1.24600E9           /* GLONASS G2 base frequency (Hz) */
#define DFRQ2_GLO   0.43750E6           /* GLONASS G2 bias frequency (Hz/n) */
#define FREQ3_GLO   1.202025E9          /* GLONASS G3 frequency (Hz) */
#define FREQ1_CMP   1.561098E9          /* BeiDou B1 frequency (Hz) */
#define FREQ2_CMP   1.20714E9           /* BeiDou B2 frequency (Hz) */
#define FREQ3_CMP   1.26852E9           /* BeiDou B3 frequency (Hz) */
#define MAXPRNGPS      32
#define MAXPRNBDS      63
#define MAXPRNGLE      38
#define MAXPRNGLO      24
#define MAXSAT         MAXPRNGPS+MAXPRNBDS+MAXPRNGLE+MAXPRNGLO
#define ALFA_IF(F1,F2)  ((F1)/((F1)-(F2)))/* f1^2*lambda1/(f1^2-f2^2)  */
#define BATA_IF(F1,F2)  (-(F2)/((F1)-(F2)))/* f2^2*lambda2/(f1^2-f2^2)  */
#define ALFA_HMW(F1,F2)  (M_C/F1)
#define BATA_HMW(F1,F2)  (F2/((F1-F2)))
#define NFREQ          5
#define MAXEXFILE   1024                /* max number of expanded files */
#define MAXBAND     10                  /* max SBAS band of IGP */
#define MAXNIGP     201                 /* max number of IGP in SBAS band */
#define MAXRCV      64                  /* max receiver number (1 to MAXRCV) */
#define MAX_FILE_WRITE 16
/*valid integer and decimal times from 1970-1-1 to 2038-1-18*/
struct config_init{        /* Configuration initialization */
    QString pos_Dat;        /* Positioning data initialization */
    QString meth_pos;         /*Positioning method parameters*/
    QString sat_sys;         /*Satellite system involved in positioning*/
    QString trop_mod;         /*Tropospheric delay model*/
    double cut_angle;         /*Satellite cut-off altitude angle*/
    bool motion_mod;         /*PPP motion model*/
    QString preci_pro;         /*Precision products*/
    QString remov_sat;         /*Removed satellite*/
    QString ppp_mod;         /*PPP model, including ionospheric free combination model,
                                non difference non combination model, fixed frequency
                                and non difference non combination multi frequency*/
    bool proce_sta;         /*Real time PPP and post PPP processing methods,false rt*/
    QVector<QStringList> obstypeset;         /*Satellite observation type setting*/
    QVector<QStringList> Qw_Pk_LPacc;/*Kalman filter noise matrix*/
    config_init()
    {
        pos_Dat=meth_pos=sat_sys=trop_mod=preci_pro=remov_sat=ppp_mod="";
        cut_angle=7.5;motion_mod=proce_sta=true;
        obstypeset = QVector<QStringList>();
        Qw_Pk_LPacc = QVector<QStringList>();
    }
};
struct para_trans{
    bool isInitPara;
    bool m_VarChang;
    bool for_and_backward_filter;
    bool backward_filter;
    int mode_solution;
    bool Helmert_flag;
    bool Sosr_flag;//Standard of success rate
    bool pre_GIFB_falg;
    bool pre_C2IFB_falg;
    bool pre_C3IFB3_falg;
    bool pre_C3IFB4_falg;
    bool pre_C3IFB5_falg;
    bool pre_EIFB3_falg;
    bool pre_EIFB4_falg;
    bool pre_EIFB5_falg;
    int pre_IFB_n;
    bool curr_GIFB_falg;
    bool curr_C2IFB_falg;
    bool curr_C3IFB3_falg;
    bool curr_C3IFB4_falg;
    bool curr_C3IFB5_falg;
    bool curr_EIFB3_falg;
    bool curr_EIFB4_falg;
    bool curr_EIFB5_falg;
    int curr_IFB_n;
    int cross_N;//
    int cyclyslip_sat_n;
    double sigma0;
    int sys_num;
    double lambda_radio;
    int reference_sat[5];
    int const_parm;
    int basd_HMWG;
    int basd_HMWC2;
    int basd_HMWC3;
    int basd_HMWE;
    int basd_HMWR;
    bool ambiguity_fix;
    double Ps;
    int m_minSatFlag;
    int Partial_subset;
    double bias_nl;
    bool EA_minsat;
    int Reference_sta;
    int fixed_WL_number;
    double Dops[4];
    double ratio_value;
    int fixed_sat;
    int satellite_N;
    double m_SPP_Pos[3];
    double receiver_pos[3];
    bool Initial_positioning_time;
    MatrixXd NL_X;
    MatrixXd state_Fi;
    MatrixXd SRIF_Rk;
    MatrixXd Nois_Q;
    VectorXd State_X;
    VectorXd posterior_V;
    VectorXd up_sys_len;VectorXd sys_len;
    MatrixXd Covar_P;MatrixXd whihet_P;
    MatrixXd Covar_Rk;
    MatrixXd Residual_Qvv;
    para_trans()
    {
        ambiguity_fix = true;
        pre_GIFB_falg=pre_C2IFB_falg=pre_C3IFB3_falg=pre_C3IFB4_falg=pre_C3IFB5_falg=pre_EIFB3_falg=pre_EIFB4_falg=pre_EIFB5_falg
                =curr_GIFB_falg=curr_C2IFB_falg=curr_C3IFB3_falg=curr_C3IFB4_falg=curr_C3IFB5_falg=curr_EIFB3_falg=curr_EIFB4_falg=curr_EIFB5_falg=false;
        isInitPara = false;//The first epoch is only initialized once
        cyclyslip_sat_n =pre_IFB_n=curr_IFB_n= 0;
        basd_HMWG=basd_HMWC2=basd_HMWC3=basd_HMWE=basd_HMWR=1;
        m_VarChang = false;
        Ps = 0.999;
        EA_minsat = true;
        Helmert_flag = false;
        backward_filter = false;
        for_and_backward_filter = false;
        Sosr_flag = false;
        Initial_positioning_time = true;
        mode_solution = 1;// 1 float 2 WL fixed 3 NL fixed
        ratio_value  = 3.0;
        bias_nl=0.4;
        fixed_WL_number=satellite_N=0;
        const_parm = 6;//[dx,dy,dz,mf,clki]
        m_minSatFlag = 6;
        Partial_subset = stand_post_res;
        sys_num = 2;
        m_SPP_Pos[0] = m_SPP_Pos[1]= m_SPP_Pos[2] =fixed_sat= 0;
        receiver_pos[0] = receiver_pos[1]= receiver_pos[2] = 0;
        sys_len.resize(sys_num);
        sys_len.setZero(sys_num);
        up_sys_len.resize(sys_num);
        up_sys_len.setZero(sys_num);
        State_X.resize(32);
        State_X.setZero(32);
        sigma0 = 1.0;
        lambda_radio = 0;
        for(int i=0;i<4;i++)    Dops[i]=0;
        for(int i=0;i<5;i++)    reference_sat[i]=-1;
    }
};
struct file_dat{
    QStringList SsrFileNameList;
    QString files_path;
    QString result_files_path;
    QString dat_path;// run the specified directory file
    QString out_dat_path;// run the specified directory file
    QString Output_system;
    QString Out_mod;
    double receiver_pos[3];
    int WLUPD_corr;
    int OBX_flie_flag;
    int bia_file_flag;
    int int_clk_ar_flag;
    file_dat(){
        OBX_flie_flag = 0;
        bia_file_flag = 0;
        int_clk_ar_flag = 0;
        WLUPD_corr = 0;
        receiver_pos[0] = receiver_pos[1]= receiver_pos[2] = 0;
    }
};
struct disply_satellite{
    double PDOP;
    double VDOP;
    double HDOP;
    double GDOP;
    QVector<double> az;
    QVector<double> el;
    QVector<char> sat_sys;
    QVector<int> sat_flag;
    QVector<int> sat_prn;
    int gpsSatNum[2];
    int bdsSatNum[2];
    int gloSatNum[2];
    int galSatNum[2];
    disply_satellite(){
        for(int i=0;i<2;i++)
            gpsSatNum[i]=gloSatNum[i]=bdsSatNum[i]=galSatNum[i]=0;
        GDOP= VDOP=HDOP=PDOP=0.0;
    }

};
struct disply_textEdit{
    QStringList data_text;
    int data_lenth;
    QTextEdit *mp_QTextEdit;
    disply_textEdit(){
        data_lenth = 0;
    }

};
struct obs_pravide{

    QString The_same_ant;
    QString ambupd_file;
    QStringList mult_OfileName;/*save the mult O file name*/
    QString m_OfileName;/*save the O file name*/
    int year_;
    int year_acc_day;
    obs_pravide(){
        The_same_ant =m_OfileName=ambupd_file= "";
        year_acc_day = year_=0;

    }
};
struct RTwriteFile{
    QFile m_QFileVct[MAX_FILE_WRITE];
    QTextStream m_QTextStreamVct[MAX_FILE_WRITE];
    QString  m_QFileNames[MAX_FILE_WRITE];
    int m_save_filenum;
    bool m_ZTDW_Clock_head;
    bool m_AZ_EL_head;
    RTwriteFile(){
        m_save_filenum = 0;
        m_ZTDW_Clock_head = false;
        m_AZ_EL_head = false;
    }

};
struct wigdet_transmit{

    double transmit_xyz[3];
    double test;

    QString data;
    QString time;
    int Year;
    int Month;
    int Day;
    int Hours;
    int Minutes;
    double Seconds;

    double DOP;
    QString height;
    QString lat;
    QString lot;
    QString sog;
    QString cog;
    QString hdop;
    QList<QString> sat;
    QStringList data_text;
    int data_lenth;
    int epoch;
    QVector<QPointF> list;


    wigdet_transmit()
    {
        lat="";
        lot="";
        sog="";
        cog="";
        height="";
        hdop="0";
        sat.clear();
        list.clear();
        data_text.clear();
        for(int i=0;i<3;i++)    transmit_xyz[i] = 0;
        test = 0;
        epoch = 0;
        data_lenth =0;
    }

};

struct corr_opt{

    bool muilt_stations;// determines whether the first epoch is initialized
    double xyz_static;
    double xyz_kinematic;
    double trop_covarP;
    double rcv_clkP;
    double sat_ambP;
    double sat_ionP;
    double wl_fixed_weight;
    bool mult_station;
    bool mult_day;
    int WLUPD_corr;
    int Combination_model;
    int frequency_model;/*用于判断mixed还是fixed的频率模式*/
    int sys_error_select;
    bool GNSS_AR_Select;
    bool Wide_lane_restriction;
    int motion_state;
    int Filtering;
    int data_process_state;
    QString m_deleteSats;
    double trop_covarQ;double rcv_clkQ;double sat_ambQ;double sat_ionQ;
    int filter_model;int Combination_mode;int treat_method;
    bool cycle_slip_detection;
    int trop_selec;
    bool quality_flag;
    bool Helmert_flag;
    int ambig_out;
    double G_whihet_diff;
    double G_code_whihet;
    double C2_whihet_diff;
    double C2_code_whihet;
    double C3_whihet_diff;
    double C3_code_whihet;
    double E_whihet_diff;
    double E_code_whihet;
    double R_whihet_diff;
    double R_code_whihet;

    int Ambiguity_acquisition;
    double m_CutAngle;
    double SNR;
    int IF0_combination[5];
    int IF1_combination[5];
    bool Reinitalization_timebox;
    int quality_control_method;
    int Reinitalization_time;
    int UDUC_fre;
    int flag_AR;//Partial ambiguity fixation and full ambiguity fixation
    int sto_model;//Height angle weighted stochastic model
    bool IsaddGPS;//Whether to add GPS (default on)
    bool IsaddGLOSS;//Whether to add GLONASS or not
    bool IsaddBDS;//Whether to join Beidou
    bool IsaddGalieo;//Whether to join Galieo or not
    QString m_sys_str;
    QStringList OFileNamesList;
    QVector< QStringList > multi_sta_OFileName;
    QVector<QVector< QStringList >> multi_day_OFileName;
    QVector< QString > multi_file_jia;
    //ZCL
    int Frequently_BDS[5];
    int BDS_fre_Num;
    int Frequently_GPS[5];
    int GPS_fre_Num;
    int Frequently_GAL[5];
    int GLE_fre_Num;
    int Frequently_GLO[5];
    int fre_model;/*用于判断是IF组合还是UC组合*/
    int GLO_fre_Num;
    bool position,ZTDW_Clock,Az_El_coff,
         dop_satN_AR,bad_satellites,Satellite_info,ambiguity_inf;

    corr_opt()
    {
        m_deleteSats = "";
        m_sys_str = "C";
        Reinitalization_time = 7200;
        IsaddGPS =IsaddGLOSS=IsaddGalieo= false;
        IsaddBDS = true;
        quality_flag = false;
        UDUC_fre = double_fre;
        Ambiguity_acquisition = poor_ambiguity;
        frequency_model = fixed_frequency;
        mult_day =mult_station=Wide_lane_restriction= true;
        Reinitalization_timebox = false;

        cycle_slip_detection = true;
        ambiguity_inf = false;
        muilt_stations = true;
        Helmert_flag = false;
        sys_error_select=Combination_model = fre_model = motion_state = Filtering = data_process_state =0;
        BDS_fre_Num=GPS_fre_Num=GLE_fre_Num=GLO_fre_Num = 0;
        for(int i=0;i<5;i++)    IF0_combination[i] = 0;
        for(int i=0;i<5;i++)    IF1_combination[i] = 1;
        for(int i=0;i<5;i++)    Frequently_BDS[i]  = 0;
        for(int i=0;i<5;i++)    Frequently_GPS[i]  = 0;
        for(int i=0;i<5;i++)    Frequently_GAL[i]  = 0;
        for(int i=0;i<5;i++)    Frequently_GLO[i]  = 0;
        filter_model = PPP_KINEMATIC_KALM;
        WLUPD_corr = 0;SNR=30;
        Combination_mode = Comb_IF_mode;
        G_code_whihet=3.0;G_whihet_diff = 1e4;C2_code_whihet=3.0;C2_whihet_diff = 1e4;C3_code_whihet=3.0;C3_whihet_diff = 1e4;
        E_code_whihet=3.0;E_whihet_diff = 1e4;R_code_whihet=3.0;R_whihet_diff = 1e4;
        xyz_kinematic = 1e3; trop_covarQ = 3e-8; rcv_clkQ = 1e6; sat_ambQ = 1e-12; sat_ionQ = 1e6;wl_fixed_weight= 1e6;
        xyz_static = 1e3; trop_covarP = 10; rcv_clkP = 1e6; sat_ambP = 1e6; sat_ionP = 10;

        position         = false;
        GNSS_AR_Select   = false;
        ZTDW_Clock       = false;
        Az_El_coff       = false;
        dop_satN_AR      = false;
        bad_satellites   = false;
        Satellite_info   = false;



    }
};
struct Parameter_transfer{
    corr_opt opt_corr;
    file_dat dat_fil;
    para_trans tra_par;
    wigdet_transmit wig_tra;
};
struct gtime_t{        /* time struct */
    time_t time;        /* time (s) expressed by standard time_t */
    double sec;         /* fraction of second under 1 s */
    gtime_t()
    {
        time=0;
        sec=0;
    }
};
struct erpd_t{        /* earth rotation parameter data type */
    double mjd;         /* mjd (days) */
    double xp,yp;       /* pole offset (rad) */
    double xpr,ypr;     /* pole offset rate (rad/day) */
    double ut1_utc;     /* ut1-utc (s) */
    double lod;         /* length of day (s/day) */
    erpd_t()
    {
        mjd=xp=yp=xpr=ypr=ut1_utc=lod=0.0;
    }
} ;

struct erp_t{        /* earth rotation parameter type */
    int n,nmax;         /* number and max number of data */
    erpd_t *data;       /* earth rotation parameter data */
    erp_t()
    {
        n=0;
        nmax=10;
        data = (erpd_t*)malloc(sizeof(erpd_t)*nmax);
    }
};
struct StationInfo
{
    QString AntType;
    double AntCorr[3];
    /* Antenna Correction: according deltype(Usually you can't accept HENs that need to look at O files.)*/
    double satpos[3];
    int deltype;/* Antenna delta type (0:HEN,1:XYZ) */
    QString locationName;/*station location*/
    QString RecverType;/*reciver type*/
    StationInfo(){
        AntType=locationName=RecverType="";
        deltype=0;
        for(int i=0;i<3;i++)
        {
            AntCorr[i]=0;
            satpos[i] =0;
        }
    }
};

struct GPSPosTime
{
    int Year;
    int Month;
    int Day;
    int Hours;
    int Minutes;
    double Seconds;
    double TropZHD;/*Current epoch ZHD all Satlite have same ZHD in epoch*/
    int epochNum;/*The number of epochs to which it belongs*/
    GPSPosTime(){
        Year=Month=Day=Hours=Minutes=epochNum=0;
        TropZHD=Seconds=0.0;
    }
};
/*Receiver Approximate Coordinate Correction by Single Point Positioning*/
struct RecivePos
{
    double dX;
    double dY;
    double dZ;
    double spp_pos[3];
    double dops[4];
    double radio;
    int fixed_sat;
    /*Number of satellites in an epoch*/
    int totolEpochStalitNum;
    GPSPosTime UTCtime;
    RecivePos(){
        dX=dY=dZ=0.0;
        radio = 0.0;
        fixed_sat =0;
        totolEpochStalitNum=0;
        for(int i=0;i<3;i++) spp_pos[i]=0.0;
        for(int i=0;i<4;i++)    dops[i]=0.0;
    }
};
/*Parameters used to detect cycle slips*/
struct CyclySlipNum
{
    double MW;
    double dL;
    double LP;
    CyclySlipNum(){
        MW=dL=LP=0.0;
    }
};


/*Broadcast ephemeris data format for each ephemeris*/
struct BrdData
{
    char SatType;/*Satellite type*/
    int PRN;/*Satellite Number*/
    GPSPosTime UTCTime;/*UTC time*/
    double TimeDiv;/*Clock deviation*/
    double TimeMove;/*Clock drift*/
    double TimeMoveSpeed;/*Clock drift acceleration*/
    int GLONASS_IODE;
    /*Store a data segment (GPS, BDS and Galieo are 28 7 rows GLONASS is 12 3 rows)*/
    QVector< double > epochNData;
    /*geph_t temp_geph_t;
    eph_t temp_eph_t;*/
    BrdData()
    {
        SatType=' ';
        PRN=GLONASS_IODE=0;
        TimeDiv=TimeMove=TimeMoveSpeed=0.0;
    }
};
struct G_C_E_ephemer
{
    int prn;            /* satellite prn */
    int iode,iodc;      /* IODE,IODC */
    int sva;            /* SV accuracy (URA index) */
    int svh;            /* SV health (0:ok) */
    int week;           /* GPS/QZS: gps week, GAL: galileo week */
    int code;           /* GPS/QZS: code on L2 */
    int sat_Type;
                        /* GAL: data source defined as rinex 3.03 */
                        /* BDS: data source (0:unknown,1:B1I,2:B1Q,3:B2I,4:B2Q,5:B3I,6:B3Q) */
    int flag;           /* GPS/QZS: L2 P data flag */
                        /* BDS: nav type (0:unknown,1:IGSO/MEO,2:GEO) */
    gtime_t toe,toc,ttr; /* Toe,Toc,T_trans */
                        /* SV orbit parameters */
    double A,e,i0,OMG0,omg,M0,deln,OMGd,idot;
    double crc,crs,cuc,cus,cic,cis;
    double Delta_n0_dot;
    double toes;        /* Toe (s) in week */
    double fit;         /* fit interval (h) */
    double f0,f1,f2;    /* SV clock parameters (af0,af1,af2) */
    double tgd[4];      /* group delay parameters */
                        /* GPS/QZS:tgd[0]=TGD */
                        /* GAL    :tgd[0]=BGD E5a/E1,tgd[1]=BGD E5b/E1 */
                        /* CMP    :tgd[0]=BGD1,tgd[1]=BGD2 */
    double Adot,ndot;   /* Adot,ndot for CNAV */
    G_C_E_ephemer(){
        iode=iodc=prn=svh=sva=week=code=flag=sat_Type=0;
        A=e=i0=OMG0=omg=M0=deln=OMGd=idot=Delta_n0_dot=0.0;
        crc=crs=cuc=cus=cic=cis=toes=f0=f1=f2=fit=0.0;
        for(int i=0;i<4;i++)
            tgd[i]=0.0;
    }

};
struct R_ephemer
{   /* GLONASS broadcast ephemeris type */
    int prn;            /* satellite prn */
    int iode;           /* IODE (0-6 bit of tb field) */
    int frq;            /* satellite frequency number */
    int svh,sva,age;    /* satellite health, accuracy, age of operation */
    gtime_t toe;        /* epoch of epherides (gpst) */
    gtime_t tof;        /* message frame time (gpst) */
    double pos[3];      /* satellite position (ecef) (m) */
    double vel[3];      /* satellite velocity (ecef) (m/s) */
    double acc[3];      /* satellite acceleration (ecef) (m/s^2) */
    double taun,gamn;   /* SV clock bias (s)/relative freq bias */
    double dtaun;       /* delay between L1 and L2 (s) */
    R_ephemer(){
        prn=iode=frq=svh=sva=age=0;
        taun=gamn=dtaun=0.0;
        for(int i=0;i<3;i++)
            pos[i]=vel[i]=acc[i]=0.0;
    }

};
/*  The matrix used for Helmert variance component estimation */
struct Helmert_mat
{
    VectorXd obs_Lx;
    VectorXd resi_vx;
    MatrixXd coef_Nx;
    MatrixXd weight_Px;
    MatrixXd coef_Bx;
    VectorXd conver_Wx;

    Helmert_mat()
    {
        obs_Lx.resize(32);resi_vx.resize(32);conver_Wx.resize(32);
        obs_Lx.setZero(32);resi_vx.setZero(32);conver_Wx.setZero(32);
        coef_Nx.resize(32,32);coef_Nx.setZero(32,32);
        weight_Px.resize(32,32);weight_Px.setZero(32,32);
        coef_Bx.resize(32,32);coef_Bx.setZero(32,32);
    }
};
/*PPP saves the calculated data*/
/*Preservation of satellite data and various corrections*/
struct SatlitData
{
    int PRN;/*Satellite PRN*/
    char SatType;/*Satellite type*/
    bool BDS_2flag;
    /*Time stored in minutes and seconds, month, day, year, not necessarily UTC time*/
    GPSPosTime UTCTime;
    gtime_t Otime;
    int EpochFlag;
    /* (rarely or hardly used: satellite signal out of lock or cycle-skip signal 0: OK or unknow. > : maybe ambiguity/Slip)*/
    int LL1_LOCK;
    int Domain_of_doubt;
    /*Satellite status signal
     * (generally not used; -1:unknow, Signal intensity range:1-9.
     * If EpochFlag>0 and EpochFlag < 3 can't use)*/
    int SigInten;
    int cycly_slipF;
    double CodeP[5];/*Store P1 or C1(m)*/
    double PhaseL[5];/*(cycle)*/    
    double CODE_BIA[5];/*code bias*/
    double PHASE_BIA[5];/*phase bias*/

    /*uncombination variable*/
    double CodePP[5];/*Store P1 or C1(m)*/
    double PhaseLL[5];/*(cycle)*/
    double VCodePP[5];/*Filtering residue of VCodePP (m)*/
    double VPhaseLL[5];/*Filtering residue of VPhaseLL (m)*/
    double CC1_Smooth;/*Phase smoothing pseudo-distance (m)*/
    double CC2_Smooth;/*Phase smoothing pseudo-distance (m)*/
    double CC1_Smooth_Q; /*The covariance of PP3_Smooth*/
    double CC2_Smooth_Q; /*The covariance of PP3_Smooth; Note:smooth number use PP3_Smooth_NUM*/
    double ionL1;/*Calculated L1 ionospheric delay (m)*/

    /*combination variable*/
    int IF_Frq[2];
    int par_sat_rank;
    double IF_L;/*Eliminate ionospheric carrier (m)!!!!*/
    double IF_P;/*Elimination of ionospheric pseudo-distance (m)*/
    double VIF_L;/*Filtering residue of LL3 (m)*/
    double VIF_P;/*Filtering residue of PP3 (m)*/
    double PP3_Smooth;/*Phase smoothing pseudo-distance (m)*/
    double PP3_Smooth_NUM;/*Phase smoothing pseudo-distance number*/
    double PP3_Smooth_Q; /*The covariance of PP3_Smooth*/
    int ObsLenfre;//szg debug The frequency length is observed
    int ObsFreflag[5];//szg Available frequency mark
    double L_mw;/*HMW Eliminate*/
    /*PPP-ar based on FCB modification  */
    bool WL_fixed;
    bool EWL23_fixed;
    bool EWL24_fixed;
    bool EWL25_fixed;
    double MW_wlfcbs;/*wlfcbs (m)*/
    double MW_nlfcbs;/*nlfcbs (m)*/
    double MW_Wl_Int;/*Wide lane Integer ambiguity*/
    double Lambda_Wl;
    double MW_EWl_Int23;/*Wide lane Integer ambiguity*/
    double Lambda_EWl23;
    double MW_EWl_Int24;/*Wide lane Integer ambiguity*/
    double Lambda_EWl24;
    double MW_EWl_Int25;/*Wide lane Integer ambiguity*/
    double Lambda_EWl25;
    double WL_weight;
    double Smoot_Wl;/*smooth wide lane*/
    double Smoot_stdWl;/*smooth wide lane standard deviation*/
    double SD_nl_FCBs;/*Ambiguity FCB of single difference narrow lane between stars*/
    int    SD_Wl_Int;/*Integer Ambiguity of single difference wide lane between stars*/
    int    SD_EWl23_Int;/*Integer Ambiguity of single difference wide lane between stars*/
    int    SD_EWl24_Int;/*Integer Ambiguity of single difference wide lane between stars*/
    int    SD_EWl25_Int;/*Integer Ambiguity of single difference wide lane between stars*/
    double SD_Wl_float;/*Inter satellite single difference wide lane float resolution*/
    double SD_EWl23_float;/*Inter satellite single difference wide lane float resolution*/
    double SD_EWl24_float;/*Inter satellite single difference wide lane float resolution*/
    double SD_EWl25_float;/*Inter satellite single difference wide lane float resolution*/
    int    SD_Nl_Int;/*Inter satellite single difference narrow lane Integer resolution*/
    double SD_FCBs;/*Inter satellite single difference FCB*/
    double SD_Nl_Float;/*Inter satellite single difference narrow lane float resolution*/
    int SD_Nl_fix;/*Inter satellite single difference narrow lane fix resolution*/
    double IF_Fcbs;/*IF combination FCB correction*/
    double VarIF_Fcbs;/*IF combination FCB Var*/
    double max_EA;/*Maximum altitude angle of current epoch*/
    int    maxEA_PRN;/*Maximum altitude angle of prn*/
    double maxEAsatWL;
    double std_devia;/*standard deviation*/
    int    Sat_epo;/*Dimension of satellite storing maximum altitude angle*/
    int    MWflag;
    /*DCB correction*/
    double stalitDCB;
    double SatDCB_Var;
    int stalitDCBFlag;
    double yawW;/*Yaw angle*/
    int yawFlag;

    /*add satellite vector*/
    double satMW_wlAmbi[3];
    int    satMW_wlSmoN;
    /*add satellite vector*/
    double satMW_ewl23Ambi[3];
    int    satMW_ewl23SmoN;
    /*add satellite vector*/
    double satMW_ewl24Ambi[3];
    int    satMW_ewl24SmoN;
    /*add satellite vector*/
    double satMW_ewl25Ambi[3];
    int    satMW_ewl25SmoN;
    /*Partial subset ambiguity fixed*/
    bool Par_sub;

    /*Ambiguity and ion*/
    double Amb1;/*Ambiguity (stored in Amb for both integer and floating point Numbers of L1)*/
    double Amb2;/*Ambiguity (stored in Amb for both integer and floating point Numbers of L2)*/
    double Amb;/*Ambiguity (stored in Amb for both integer and floating point Numbers Of Ion_free LL3)*/
    /*others correction*/
    QVector< QString > wantObserType;/*Store the observed value type corresponding to the read dual-frequency (store order as C1, L1, C2, L2)*/
    QVector< double > obserValue;/*Observation data corresponding to obserType*/
    QVector< QString > obserType;/*Observation types (Rinex_2: C1, L1, D1, S1, C2, L2, etc. Rinex_3: C1C, L1C, S1C, C2W, L2W, etc.)*/
    QVector<QString> badMsg;/*Storage Elimination Satellites message*/
    QVector< int > obsSigInten;
    QVector< int > obsLL1_LOCK;
    double Frq[5];/*Record the frequencies of Lx*/
    double Lambda[5];
    double X;/* Wgs-84 satellite coordinates need to be converted from file (m)*/
    double Y;
    double Z;
    int continue_eph;
    double EA[2];/*Satellite altitude Angle and azimuth (degrees)*/
    /*Model correction*/
    double Relativty;/*Relativistic correction (m)*/
    double Sagnac;/*Correction of earth autobiography (m)*/
    double StaClock;/*Satellite clock difference (m)*/
    double SatClock_Var;
    double SatOrbit_Var;
    double StaClockRate;/*Satellite clock difference (m/s)*/
    double SatTrop;/*Signal direction tropospheric dry delay correction(ZPD) (m)*/
    int MJD;
    double seconds_day;
    double SatIon[5];
    double SatTrop_Var;
    double StaTropMap;/*The wet projection function of the troposphere(mf)*/
    double AntHeight;/*Correction of Antenna Height (m)*/
    double LxOffset[5];/*Lx receiver antenna number correction (cycle)*/
    double SatLxOffset[5];/* L1 satellite antenna weeks correction (weeks)*/
    double Sat_LxPCO[5];
    double TideEffect;/*Tidal correction (m)*/
    double AntWindup;/*Phase windup correction (cycle)*/
    double SatWight;/*Weighted value of satellite altitude angle*/
    double SatWight_L;/*Weighted value of satellite altitude angle*/
    double Helmert_Wight_L;/*Weighted value of satellite altitude angle*/
    double Helmert_Wight_P;/*Weighted value of satellite altitude angle*/
    double SatWight_IF[2];
    double SatWight_UD[10];/*Weighted value of satellite altitude angle*/
    SatlitData(){
        BDS_2flag=Par_sub =WL_fixed=EWL23_fixed=EWL24_fixed=EWL25_fixed= false;
        IF_Frq[0]=PRN=EpochFlag=LL1_LOCK=SigInten=SD_Wl_Int=SD_EWl23_Int=SD_EWl24_Int=SD_EWl25_Int=SD_Nl_Int=SD_Nl_fix=cycly_slipF=0;
        maxEA_PRN=Sat_epo=MWflag=stalitDCBFlag=yawFlag=satMW_wlSmoN=satMW_ewl23SmoN=satMW_ewl24SmoN=satMW_ewl25SmoN=ObsLenfre=Domain_of_doubt=0;
        Helmert_Wight_L = Helmert_Wight_P =1;par_sat_rank=0;
        for(int i=0;i<10;i++)  SatWight_UD[i]=1.0;
        WL_weight = 1e4;
        SatWight_IF[0]=SatWight_IF[1]=1.0;IF_Frq[1]=1;
        continue_eph = 1;MJD=0;seconds_day=0;
        for(int i=0;i<5;i++)
            ObsFreflag[i]=CodeP[i]=CODE_BIA[i]=PHASE_BIA[i]=PhaseL[i]=LxOffset[i]=SatIon[i]=VPhaseLL[i]
                    =PhaseLL[i]=CodePP[i]=VCodePP[i]=Frq[i]=Lambda[i]=VCodePP[i]=SatLxOffset[i]=Sat_LxPCO[i]=0.0;
        for(int i=0;i<3;i++)    satMW_wlAmbi[i]=satMW_ewl23Ambi[i]=satMW_ewl24Ambi[i]=satMW_ewl25Ambi[i]=0;
        CC1_Smooth=CC2_Smooth=CC1_Smooth_Q=CC2_Smooth_Q=ionL1=L_mw=MW_wlfcbs=MW_nlfcbs
                =MW_Wl_Int=Smoot_Wl=Smoot_stdWl=MW_EWl_Int23=MW_EWl_Int24=MW_EWl_Int25=Lambda_Wl=Lambda_EWl23=Lambda_EWl24=Lambda_EWl25=0.0;
        SD_FCBs=SD_Nl_Float=SD_Wl_float=IF_Fcbs=VarIF_Fcbs=max_EA=maxEA_PRN=maxEAsatWL=std_devia
                =stalitDCB=SatDCB_Var=IF_L=IF_P=VIF_L=VIF_P=PP3_Smooth=PP3_Smooth_NUM=PP3_Smooth_Q
                =Amb1=Amb2=Amb=X=EA[0]=EA[1]=Y=Z=Relativty=Sagnac=StaClock=StaClockRate=SatTrop
                =StaTropMap=AntHeight=TideEffect=yawW=SD_nl_FCBs=SatWight_L
                =AntWindup=SatWight=SatTrop_Var=SatClock_Var=SatOrbit_Var=SD_EWl23_float=SD_EWl24_float=SD_EWl25_float=0.0;

    }
};
struct Ambiguity_mat
{
  MatrixXd SD_P;
  MatrixXd Fixed_Qa;
  MatrixXd Fixed_Qab;
  MatrixXd SD_X;
  MatrixXd SD_GNSS_F;
  MatrixXd La_SD_P;
  VectorXd La_SD_X;
  MatrixXd Qab;
  MatrixXd UDUC_B;
  MatrixXd SD_UDUC_B;
  VectorXd SD_a;
};


struct GNSS_PAR_auto
{
    QVector< int > systemf;
    QVector< int > dele_flag;
    QVector< double > PS_fixed;
    QVector< int > sort_GNSS;
    int fixed_sat;
    int sys_f[8][4];
    GNSS_PAR_auto(){
        for(int i=0;i<4;i++)
            for(int j=0;j<8;j++)
                sys_f[j][i]=0;
        fixed_sat=0;
    }
};

/*define ambiguity*/
struct Ambiguity
{
    int PRN;/*Satellite PRN*/
    char SatType;/*Satellite type*/
    double ionL1;/*Calculated L1 ionospheric delay (m)*/
    double EA[2];/*Satellite altitude Angle and azimuth (degrees)*/
    double Amb1;/*Ambiguity (stored in Amb for both integer and floating point Numbers of L1)*/
    double Amb2;
    double Amb;
    bool isIntAmb;/*Is integer ambiguity*/
    GPSPosTime UTCTime;/*Time stored in minutes and seconds, month, day, year, not necessarily UTC time*/
    Ambiguity(){
        PRN=0;SatType=' ';
        ionL1=EA[0]=EA[1]=Amb1=Amb2=Amb=0.0;
    }
};
typedef struct {        /* TEC grid type */
    gtime_t time;       /* epoch time (GPST) */
    int ndata[3];       /* TEC grid data size {nlat,nlon,nhgt} */
    double rb;          /* earth radius (km) */
    double lats[3];     /* latitude start/interval (deg) */
    double lons[3];     /* longitude start/interval (deg) */
    double hgts[3];     /* heights start/interval (km) */
    double *data;       /* TEC grid data (tecu) */
    float *rms;         /* RMS values (tecu) */
} tec_t;
typedef struct {        /* SBAS ionospheric correction type */
    gtime_t t0;         /* correction time */
    short lat,lon;      /* latitude/longitude (deg) */
    short give;         /* GIVI+1 */
    float delay;        /* vertical delay estimate (m) */
} sbsigp_t;
typedef struct {        /* SBAS ionospheric corrections type */
    int iodi;           /* IODI (issue of date ionos corr) */
    int nigp;           /* number of igps */
    sbsigp_t igp[MAXNIGP]; /* ionospheric correction */
} sbsion_t;
typedef struct {        /* navigation data type */
    int n,nmax;         /* number of broadcast ephemeris */
    int ng,ngmax;       /* number of glonass ephemeris */
    int ns,nsmax;       /* number of sbas ephemeris */
    int ne,nemax;       /* number of precise ephemeris */
    int nc,ncmax;       /* number of precise clock */
    int na,namax;       /* number of almanac data */
    int nt,ntmax;       /* number of tec grid data */
    int nf,nfmax;       /* number of satellite fcb data */
//    eph_t *eph;         /* GPS/QZS/GAL ephemeris */
//    geph_t *geph;       /* GLONASS ephemeris */
//    seph_t *seph;       /* SBAS ephemeris */
//    peph_t *peph;       /* precise ephemeris */
//    pclk_t *pclk;       /* precise clock */
//    alm_t *alm;         /* almanac data */
    tec_t *tec;         /* tec grid data */
//    fcbd_t *fcb;        /* satellite fcb data */
    erp_t  erp;         /* earth rotation parameters */
    double utc_gps[4];  /* GPS delta-UTC parameters {A0,A1,T,W} */
    double utc_glo[4];  /* GLONASS UTC GPS time parameters */
    double utc_gal[4];  /* Galileo UTC GPS time parameters */
    double utc_qzs[4];  /* QZS UTC GPS time parameters */
    double utc_cmp[4];  /* BeiDou UTC parameters */
    double utc_irn[4];  /* IRNSS UTC parameters */
    double utc_sbs[4];  /* SBAS UTC parameters */
    double ion_gps[8];  /* GPS iono model parameters {a0,a1,a2,a3,b0,b1,b2,b3} */
    double ion_gal[4];  /* Galileo iono model parameters {ai0,ai1,ai2,0} */
    double ion_qzs[8];  /* QZSS iono model parameters {a0,a1,a2,a3,b0,b1,b2,b3} */
    double ion_cmp[8];  /* BeiDou iono model parameters {a0,a1,a2,a3,b0,b1,b2,b3} */
    double ion_irn[8];  /* IRNSS iono model parameters {a0,a1,a2,a3,b0,b1,b2,b3} */
    int leaps;          /* leap seconds (s) */
    double lam[MAXSAT][NFREQ]; /* carrier wave lengths (m) */
    double cbias[MAXSAT][3]; /* satellite dcb (0:p1-p2,1:p1-c1,2:p2-c2) (m) */
    double rbias[MAXRCV][2][3]; /* receiver dcb (0:p1-p2,1:p1-c1,2:p2-c2) (m) */
    double wlbias[MAXSAT];   /* wide-lane bias (cycle) */
    double glo_cpbias[4];    /* glonass code-phase bias {1C,1P,2C,2P} (m) */
    char glo_fcn[MAXPRNGLO+1]; /* glonass frequency channel number + 8 */
//    pcv_t pcvs[MAXSAT]; /* satellite antenna pcv */
//    sbssat_t sbssat;    /* SBAS satellite corrections */
    sbsion_t sbsion[MAXBAND+1]; /* SBAS ionosphere corrections */
//    dgps_t dgps[MAXSAT]; /* DGPS corrections */
//    ssr_t ssr[MAXSAT];  /* SSR corrections */
//    lexeph_t lexeph[MAXSAT]; /* LEX ephemeris */
//    lexion_t lexion;    /* LEX ionosphere correction */
//    pppcorr_t pppcorr;  /* ppp corrections */
} nav_t;
/*define storage clock difference and zenith wet delay*/
struct ClockData
{
    double clockData[6];/*X system receiver clock error,Xi system relative to X system receiver clock deviation*/
    double ZTD_W;/*Zenith wet delay*/
    GPSPosTime UTCTime;/*UTC year, month, day, hour, minute and second*/
};
struct proopt
{
    int mode;               /*Positioning mode*/
    int nsys;
    int format;             /*Receiver type*/
    int outType;            /*type of output*/
    int nf;                 /*Positioning frequency (1: L1, 2: L1 + L2, 3: L1 + L2 + L3)*/
    bool navSys[4];         /*Navigation system*/
    int enhSys;             /*Enhanced system*/
    double elMin;            /*Cut off height angle*/
    int posForm;            /*Positioning data output format*/
    int ionoOpt;            /*Ionospheric correction*/
    int tropOpt;            /*Tropospheric correction*/
    bool inteSet;           /*Integrity setting flag bit*/
    int  posopt[6];         /* positioning options */
    proopt()
    {
       mode=0;/*PMODE_PPP_STATIC*/
       nf=2;
       format=0;
       enhSys=0;
       nsys=0;
       elMin=tropOpt=0;
       ionoOpt=tropOpt=2;
       for(int i=0;i<4;i++)
       {
           navSys[i]=false;/*SYS_GPS|SYS_BDS|SYS_GLO*/
       }
       inteSet=false;
       posForm=0;
    }
};
/* global variables ----------------------------------------------------------*/





#endif
