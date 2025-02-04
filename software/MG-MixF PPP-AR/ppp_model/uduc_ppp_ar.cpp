#include "uduc_ppp_ar.h"


uduc_ppp_ar::uduc_ppp_ar(QObject *parent) : QThread(parent)
{
    // 注册自定义类型信号槽
    qRegisterMetaType<wigdet_transmit>("wigdet_transmit");
}

uduc_ppp_ar::~uduc_ppp_ar()
{

}


void uduc_ppp_ar::run(){


    para_trans *trans_para = AGNSS_APPData::sGetAPPDataInstance()->mGettrans_paraData();
    corr_opt *opt_corr = AGNSS_APPData::sGetAPPDataInstance()->mGetopt_corrData();
    disply_satellite *sate_disp = AGNSS_APPData::sGetAPPDataInstance()->mGettrans_sate_dispData();
    file_dat *dat_fil = AGNSS_APPData::sGetAPPDataInstance()->mGetdat_filData();
    AGNSS_APPData::sGetAPPDataInstance()->mSetAppFunState(true);
    if(opt_corr->mult_day)
    {
        for(int i=0;i<opt_corr->multi_file_jia.length();i++)
        {
            dat_fil->dat_path = opt_corr->multi_file_jia.at(i);
            QVector< QStringList > multi_sta_OFileName = opt_corr->multi_day_OFileName.at(i);
            for(int j=0;j<multi_sta_OFileName.length();j++)
            {
                QStringList OFileNamesList = multi_sta_OFileName.at(j);
                uduc_ppp_ar_calculating(opt_corr,trans_para,dat_fil,sate_disp,OFileNamesList);
                if(!opt_corr->muilt_stations)
                    break;
            }
        }
    }
    else
    {
        for(int j=0;j<opt_corr->multi_sta_OFileName.length();j++)
        {
            QStringList OFileNamesList = opt_corr->multi_sta_OFileName.at(j);
            uduc_ppp_ar_calculating(opt_corr,trans_para,dat_fil,sate_disp,OFileNamesList);
            if(!opt_corr->muilt_stations)
                break;
        }
    }
    AGNSS_APPData::sGetAPPDataInstance()->mSetAppFunState(false);

}

void uduc_ppp_ar::sysMain()
{

   this->start();

}
void uduc_ppp_ar::uduc_ppp_ar_calculating(const corr_opt *opt_corr, para_trans *trans_para,file_dat *dat_fil,disply_satellite *sate_disp,QStringList OFileNamesList)
{
    stand_pos spp_p;
    spp_p.setSatlitSys(opt_corr->m_sys_str);
    spp_p.sear_file(opt_corr->treat_method,OFileNamesList,dat_fil);
    for(int i=0;i<3;i++)    trans_para->receiver_pos[i] = dat_fil->receiver_pos[i];
    QTime myTime; // set timer
    myTime.start();// start timer
    //Externally initialize fixed variables to speed up calculations
    double p_HEN[3] = {0};//Get the antenna high
    spp_p.getAntHEN(p_HEN);
    bool bia_flag = true;//szg debug bia_flag initialize true
    bool obx_flag = true;//szg debug bia_flag initialize true
    QVector < SatlitData > prevEpochSatlitData;//Store satellite data of an epoch, use cycle slip detection（Put it on top, otherwise read multReadOFile epochs, the life cycle will expire when reading）
    double spp_pos[trans_para->const_parm] = {0};// store SPP pos and clk
    memcpy(spp_pos, trans_para->receiver_pos, 3*sizeof(double));
    int epoch_num = 0, continue_bad_epoch = 0;//Record the first epoch
    bool isInitSpp = false;
    double maxEAsat[5]={0.0};int ref_sat_prn[5]={0};//szg debug
    if(spp_pos[0] !=0 ) isInitSpp = true;
    trans_para->Initial_positioning_time=true;
    gtime_t Init_pos_time;
    int multReadOFile = 1000;
    while (!spp_p.isObsEnd()||(trans_para->backward_filter&&spp_p.allPPPSatlitData.length()!=0))
    {
        QVector< QVector < SatlitData > > multepochSatlitData;
        if(trans_para->backward_filter)
        {
            for(int epoch=spp_p.allPPPSatlitData.length()-2;epoch>0;epoch--)
                multepochSatlitData.append(spp_p.allPPPSatlitData.at(epoch));
            spp_p.allPPPSatlitData.clear();
        }
        else
        {
            //Store multiple epochs
            spp_p.getMultEpochData(multepochSatlitData,multReadOFile);//Read multReadOFile epochs
        }
        //Multiple epoch cycles
        for (int epoch = 0; epoch < multepochSatlitData.length();epoch++)
        {
            mutex.lock();
            QVector< SatlitData > epochSatlitData;//Temporary storage of uncalculated data for each epoch satellite
            QVector< SatlitData > epochResultSatlitData;// Store each epoch satellite to calculate the correction data
            epochSatlitData = multepochSatlitData.at(epoch);
            if(epochSatlitData.length() == 0)
            {
                mutex.unlock();
                continue;
            }
            GPSPosTime epochTime;
            gtime_t Otime,ephT,ZeroT;
            if(epochSatlitData.length() > 0)
            {
                epochTime= epochSatlitData.at(0).UTCTime;//Obtain the observation time (the epoch stores the observation time for each satellite)
                epochTime.epochNum = epoch_num;
            }
            double Obst[6] = {epochTime.Year,epochTime.Month,epochTime.Day,
                              epochTime.Hours,epochTime.Minutes,epochTime.Seconds};
            Otime = spp_p.epoch2time(Obst);//gtime get
            ephT =  Otime;
            if(opt_corr->Reinitalization_timebox)
            {
                if(trans_para->Initial_positioning_time)
                {
                    Init_pos_time = Otime;
                    trans_para->Initial_positioning_time=false;
                }
                if(fabs(spp_p.timediff(Otime,Init_pos_time))>=opt_corr->Reinitalization_time)
                {
                    prevEpochSatlitData.clear();// Exception reinitialization
                    Init_pos_time = Otime;
                }

            }

            //Set the epoch of the satellite
            for(int i = 0;i < epochSatlitData.length();i++)
                epochSatlitData[i].UTCTime.epochNum = epoch_num;
            // use spp compute postion and smooth pesudorange get clk at spp_pos[3]
//            if(opt_corr->filter_model==PPP_STATIC_KALM||opt_corr->filter_model==PPP_STATIC_SRIF)
//                memcpy(trans_para->receiver_pos, spp_pos, 3*sizeof(double));
            //qDebug()<<"SimpleSPP_model start"<<endl;

            spp_p.SimpleSPP_model(prevEpochSatlitData,epochSatlitData,spp_pos,ephT,opt_corr,trans_para,dat_fil);
            //qDebug()<<"SimpleSPP_model end"<<endl;
            if(!isInitSpp && spp_pos[0] != 0)
            {
                isInitSpp = true;
                memcpy(trans_para->receiver_pos, spp_pos, 3*sizeof(double));
            }
            if(opt_corr->filter_model==PPP_STATIC_KALM||opt_corr->filter_model==PPP_STATIC_SRIF)
                memcpy(spp_pos, trans_para->receiver_pos, 3*sizeof(double));
            if(!isnormal(spp_pos[0]))
                memset(spp_pos, 0, 4*sizeof(double));
            // The number of skipping satellites is less than m_minSatFlag
            if(epochSatlitData.length() < trans_para->m_minSatFlag || spp_pos[0] == 0)
            {
                if((opt_corr->filter_model==PPP_KINEMATIC_KALM||opt_corr->filter_model==PPP_KINEMATIC_SRIF)&&continue_bad_epoch++ > 8)
                {
//                    prevEpochSatlitData.clear();// Exception reinitialization
                    continue_bad_epoch = 0;
                }

                for(int i = 0;i < epochSatlitData.length();i++)
                {
                    epochSatlitData[i].EpochFlag = 888;// add bad flag 888 for SPP

                }
                VectorXd ENU_Vct;
                double spp_vct[3] = {0};
                int param_len = 3*epochSatlitData.length() + 32;
                ENU_Vct.resize(param_len);
                ENU_Vct.fill(0);
//                // debug by xiaogongwei 2020.04.24
                spp_p.saveResult2Class(ENU_Vct, spp_vct, epochTime, epochSatlitData, epoch_num,opt_corr,trans_para);
                epochResultSatlitData.clear();
                epochSatlitData.clear();
                epoch_num++;
                mutex.unlock();
                continue;
            }
            if(epoch_num==1001||epoch_num==1641||epoch_num==4889)
            {
                 int debuga = 666;
                 debuga=0;
            }
            /*no bias product*/
            int AR_flag = 0;
            //An epoch cycle begins

            if(!trans_para->backward_filter)
            {
                for (int i = 0;i < epochSatlitData.length();i++)
                {
                    SatlitData tempSatlitData = epochSatlitData.at(i);//Store calculated corrected satellite data
                    if(!spp_p.isInSystem(tempSatlitData.SatType))
                        continue;
                    //Test whether the carrier and pseudorange are abnormal and terminate in time.
                    if(opt_corr->fre_model != hybrid_fre&&opt_corr->Combination_mode==Comb_UD_mode&&opt_corr->frequency_model!=mixed_frequency)
                    {
                        if(opt_corr->fre_model!=tempSatlitData.ObsLenfre)
                            continue;
                    }
                    int IF_fre0=0,IF_fre1=1;
                    spp_p.IF_combination_fre(tempSatlitData,&IF_fre0,&IF_fre1,opt_corr);
                    if(opt_corr->Combination_mode==Comb_IF_mode&&!(tempSatlitData.CodeP[IF_fre0]&&tempSatlitData.CodeP[IF_fre1]&&tempSatlitData.PhaseL[IF_fre0]&&tempSatlitData.PhaseL[IF_fre1]))
                    {
                        QString errorline;
                        ErrorMsg(errorline);
                        tempSatlitData.badMsg.append("Lack of observations"+errorline);
                        spp_p.allBadSatlitData.append(tempSatlitData);
                        continue;
                    }
                    // Read satellite clock error from CLK file
                    double stalitClock[1] = {0},stalitClockVar[1]={0},sat_UPD,pOrb_Var[1]={0};//szg debug 0314[1] = {0.0}
                    //Obtain the coordinates of the epoch satellite from the SP3 data data
                    double pXYZ[3] = {0},pdXYZ[3] = {0},sp3Clk = 0.0,CODE_BIA[5]={0},PHASE_BIA[5]={0};//Unit m
                    double m_PrnGpst = spp_p.YMD2GPSTime(epochTime.Year,epochTime.Month,epochTime.Day,
                         epochTime.Hours,epochTime.Minutes,epochTime.Seconds);
                    spp_p.getStaliteClk(tempSatlitData.PRN,tempSatlitData.SatType,m_PrnGpst - tempSatlitData.CodeP[0]/M_C,stalitClock,stalitClockVar,&sat_UPD,dat_fil);
                    //Obtain the coordinates of the epoch satellite from the SP3 data data
                    spp_p.getPrcisePoint(tempSatlitData.PRN,tempSatlitData.SatType,m_PrnGpst - tempSatlitData.CodeP[0]/M_C - stalitClock[0]/M_C,pXYZ,pdXYZ,&sp3Clk,pOrb_Var);
                    tempSatlitData.StaClock = stalitClock[0];
                    tempSatlitData.X = pXYZ[0];tempSatlitData.Y = pXYZ[1];tempSatlitData.Z = pXYZ[2];

    //Test the state of the precise ephemeris and the clock difference and whether the carrier and pseudorange are abnormal, and terminate the XYZ or the satellite with the clock difference of 0 in time.
                    if (!(tempSatlitData.X&&tempSatlitData.Y&&tempSatlitData.Z&&tempSatlitData.StaClock))
                    {
                        QString errorline;
                        ErrorMsg(errorline);
                        tempSatlitData.badMsg.append("Can't calculate the orbit and clock offset"+errorline);
                        spp_p.allBadSatlitData.append(tempSatlitData);
                        continue;
                    }
                    //qDebug()<<"UCAR 233";
                    int obs_day=0;double Seconds_day=0;
                    obs_day  = spp_p.YearAccDay(epochTime.Year,epochTime.Month,epochTime.Day);
                    Seconds_day = spp_p.DayAccSec(epochTime.Hours,epochTime.Minutes,epochTime.Seconds);
                    spp_p.getbia_Dat(obs_day,Seconds_day,tempSatlitData.SatType,tempSatlitData.PRN,CODE_BIA,PHASE_BIA,tempSatlitData.wantObserType);
    //PPP removes  satellites
                    //qDebug()<<"UCAR 238";
                    QString removeSat = QString(tempSatlitData.SatType);
                    if(tempSatlitData.PRN < 10)
                        removeSat += "0" + QString::number(tempSatlitData.PRN);
                    else
                        removeSat += QString::number(tempSatlitData.PRN);
                    if(opt_corr->m_deleteSats.contains(removeSat, Qt::CaseInsensitive))
                    {
                        QString errorline;
                        ErrorMsg(errorline);
                        tempSatlitData.badMsg.append("remove " + removeSat +errorline);
                        spp_p.allBadSatlitData.append(tempSatlitData);
                        continue;
                    }
                    //Computational relativity correction
                    double relative = 0;
                    relative = spp_p.getRelativty(tempSatlitData.SatType, pXYZ,spp_pos,pdXYZ);
    //                tempSatlitData.Relativty = relative;
                    tempSatlitData.Relativty = relative;//szg debug 0315
                    //Calculate the satellite's high sitting angle (as the receiver approximates the target)
                    double EA[2]={0};
                    spp_p.getSatEA(pXYZ,trans_para->receiver_pos,EA);
                    tempSatlitData.EA[0] = EA[0];tempSatlitData.EA[1] = EA[1];
                    EA[0] = EA[0]*MM_PI/180;EA[1] = EA[1]*MM_PI/180;//Go to the arc to facilitate the calculation below
                    switch (opt_corr->sto_model) {
                    case stochastic_model_1:
                        spp_p.sine_Wight(tempSatlitData);
                        break;
                    case stochastic_model_2:
                        spp_p.cosine_Wight(tempSatlitData);
                        break;
                    case stochastic_model_3:
                        spp_p.sine1_Wight(tempSatlitData);
                        break;
                    case stochastic_model_4:
                        spp_p.panda_Wight(tempSatlitData);
                        break;
                    default:
                        break;
                    }
                    //Calculate the autobiographic correction of the earth
                    //szg ssr bia produrc
                    for(int bia = 0;bia<5;bia++)
                    {
                        tempSatlitData.CODE_BIA[bia] = -CODE_BIA[bia]*1e-9*M_C;
                        tempSatlitData.PHASE_BIA[bia] = -PHASE_BIA[bia]*1e-9*M_C;
                    }



                    double earthW = 0;
                    earthW = spp_p.getSagnac(tempSatlitData.X,tempSatlitData.Y,tempSatlitData.Z,spp_pos);
                    tempSatlitData.Sagnac = earthW;//earthW;
                    //Calculate tropospheric dry delay
                    double MJD = spp_p.computeJD(epochTime.Year,epochTime.Month,epochTime.Day,
                        epochTime.Hours,epochTime.Minutes,epochTime.Seconds) - 2400000.5;//Simplified Julian Day
                    //Calculate and save the annual accumulation date
                    double TDay = spp_p.YearAccDay(epochTime.Year,epochTime.Month,epochTime.Day);
                    double p_BLH[3] = {0},mf = 0, TropZHD_s = 0,trop_pre=0,trop_var=0;
                    spp_p.XYZ2BLH(spp_pos,p_BLH);
                    spp_p.trop_delay(Otime,p_BLH,TDay,EA,trop_pre,opt_corr->trop_selec,&mf,&TropZHD_s,&trop_var);
                    tempSatlitData.SatTrop = TropZHD_s;
                    tempSatlitData.StaTropMap = mf;
                    //Calculate antenna high offset correction Antenna Height天线高偏置
                    tempSatlitData.AntHeight = p_HEN[0]*qSin(EA[0]) + p_HEN[1]*qCos(EA[0])*qSin(EA[1]) + p_HEN[2]*qCos(EA[0])*qCos(EA[1]);
                    //Receiver L1 L2 offset correction

                    double LxOffset[5]= {0};
                    spp_p.getRecvL12(EA[0],EA[1],tempSatlitData.SatType,LxOffset,tempSatlitData.wantObserType);
                    for(int i=0;i<5;i++)
                    {
                        if(tempSatlitData.Lambda[i]!=0)
                            tempSatlitData.LxOffset[i] = LxOffset[i]/tempSatlitData.Lambda[i];
                    }
                    //Satellite antenna phase center correction store data to (m_ReadAntClass.m_sunpos,m_ReadAntClass.m_moonpos,m_ReadAntClass.m_gmst)
                    // and update sunpos and moonpos
                    MatrixXd quater;
                    quater.resize(3,3);
                    quater.setZero(3,3);
                    if(dat_fil->OBX_flie_flag==0)
                        spp_p.getATT_Dat(Otime,tempSatlitData.SatType,tempSatlitData.PRN,quater);
                    double sumq = quater(0,0)+quater(1,1)+quater(2,2);
                    if(sumq==0&&dat_fil->OBX_flie_flag==0)
                    {
                        QString errorline;
                        ErrorMsg(errorline);
                        tempSatlitData.badMsg.append("obxfile is not or sumq==0" +errorline);
                        spp_p.allBadSatlitData.append(tempSatlitData);
                        continue;
                    }
                    double SatL12Offset[10] = {0};
                    if(dat_fil->OBX_flie_flag==0)
                        spp_p.getSatOffSet_quater(Obst,tempSatlitData.PRN,tempSatlitData.SatType,pXYZ,spp_pos,
                                                      SatL12Offset, tempSatlitData.wantObserType,quater);
                    else
                        spp_p.getSatOffSet(Obst,tempSatlitData.PRN,tempSatlitData.SatType,pXYZ,spp_pos,
                                                      SatL12Offset, tempSatlitData.wantObserType);

                    for(int i=0;i<5;i++)
                    {
                        if(tempSatlitData.Lambda[i]!=0)
                        {
                            tempSatlitData.SatLxOffset[i] = SatL12Offset[i]/tempSatlitData.Lambda[i];
                            tempSatlitData.Sat_LxPCO[i] = SatL12Offset[5+i]/tempSatlitData.Lambda[i];
                        }
                    }
                    bool bias_flag = true;
                    for(int bia = 0;bia<5;bia++)
                    {
                        if(tempSatlitData.CodeP[bia]!=0.0&&tempSatlitData.PhaseL[bia]!=0.0)
                        {
                            if(PHASE_BIA[bia]==0.0||CODE_BIA[bia]==0.0||tempSatlitData.Sat_LxPCO[bia]==0)
                                bias_flag = false;
                        }
                    }
                    if(!bias_flag&&tempSatlitData.SatType!='R')
                    {
                        QString errorline;
                        ErrorMsg(errorline);
                        tempSatlitData.badMsg.append("PHASE_BIA||CODE_BIA is not " +errorline);
                        spp_p.allBadSatlitData.append(tempSatlitData);
                        continue;
                    }
                    if(tempSatlitData.CodeP[0]==0||tempSatlitData.PhaseL[0]==0||tempSatlitData.ObsLenfre<2)
                        continue;
                    //Calculate tide correction
                    double effctDistance = 0;
                    effctDistance = spp_p.getAllTideEffect(epochTime.Year,epochTime.Month,epochTime.Day,
                        epochTime.Hours,epochTime.Minutes,epochTime.Seconds,spp_pos,EA,
                        spp_p.m_sunpos,spp_p.m_moonpos,spp_p.m_gmst);
                    tempSatlitData.TideEffect = effctDistance;
                    //Calculate antenna phase winding
                    double AntWindup = 0,preAntWindup = 0;
                    //Find the previous epoch. Is there a satellite present? The deposit is stored in preAntWindup or preAntWindup=0.
                    preAntWindup = spp_p.getPreEpochWindUp(prevEpochSatlitData,tempSatlitData.PRN,tempSatlitData.SatType);//Get the previous epoch of WindUp
                    if(dat_fil->OBX_flie_flag==0)
                        AntWindup = spp_p.getWindUp_quater(pXYZ,spp_pos,preAntWindup,pdXYZ,quater);
                    else
                        AntWindup = spp_p.getWindUp(epochTime.Year,epochTime.Month,epochTime.Day,
                                    epochTime.Hours,epochTime.Minutes,epochTime.Seconds - tempSatlitData.CodeP[0]/M_C,
                                    pXYZ,spp_pos,preAntWindup,spp_p.m_sunpos);
                    tempSatlitData.AntWindup = AntWindup;
                    for(int i=0;i<5;i++)
                    {
                        if(tempSatlitData.CodeP[i]&&tempSatlitData.PhaseL[i])
                        {
                            tempSatlitData.CodePP[i]=tempSatlitData.Lambda[i]*(tempSatlitData.LxOffset[i] + tempSatlitData.SatLxOffset[i]) + tempSatlitData.CodeP[i]+tempSatlitData.CODE_BIA[i];//+tempSatlitData.CODE_BIA[i]
                            tempSatlitData.PhaseLL[i]=tempSatlitData.Lambda[i]*(tempSatlitData.LxOffset[i] + tempSatlitData.SatLxOffset[i] -tempSatlitData.AntWindup+tempSatlitData.PhaseL[i]) + tempSatlitData.PHASE_BIA[i];//+ tempSatlitData.PHASE_BIA[i]

                        }
                    }
                    if(opt_corr->Combination_mode==Comb_IF_mode)
                    {
                        //Computation to eliminate ionospheric pseudorange and carrier combinations (here absorbed receiver carrier deflection and WindUp) add SatL1Offset and SatL1Offset
                        double alpha1 = (tempSatlitData.Frq[IF_fre0]*tempSatlitData.Frq[IF_fre0])/(tempSatlitData.Frq[IF_fre0]*tempSatlitData.Frq[IF_fre0] - tempSatlitData.Frq[IF_fre1]*tempSatlitData.Frq[IF_fre1]);
                        double alpha2 = (tempSatlitData.Frq[IF_fre1]*tempSatlitData.Frq[IF_fre1])/(tempSatlitData.Frq[IF_fre0]*tempSatlitData.Frq[IF_fre0] - tempSatlitData.Frq[IF_fre1]*tempSatlitData.Frq[IF_fre1]);
                        tempSatlitData.IF_Frq[0] = IF_fre0;
                        tempSatlitData.IF_Frq[1] = IF_fre1;
                        tempSatlitData.IF_P = alpha1 * tempSatlitData.CodePP[IF_fre0] - alpha2 * tempSatlitData.CodePP[IF_fre1];
                        tempSatlitData.IF_L = alpha1 * tempSatlitData.PhaseLL[IF_fre0] - alpha2 * tempSatlitData.PhaseLL[IF_fre1];

                    }
                    if(opt_corr->frequency_model==fixed_frequency)
                    {
                        int HMW_basd_fre = 1;
                        switch (tempSatlitData.SatType) {
                        case 'G':
                            HMW_basd_fre = trans_para->basd_HMWG;
                            break;
                        case 'C':
                            HMW_basd_fre = trans_para->basd_HMWC3;
                            break;
                        case 'R':
                            HMW_basd_fre = trans_para->basd_HMWR;
                            break;
                        case 'E':
                            HMW_basd_fre = trans_para->basd_HMWE;
                            break;
                        default:
                            break;
                        }
                        int fre_mw0=-1,fre_mw1=-1,fre_mw2=-1,fre_mw3=-1;

                        int ObsFreflag=0;bool HMW_fre_falg=false;
                        for(int ii=0;ii<5;ii++)
                        {
                            if(tempSatlitData.ObsFreflag[ii]==0)  continue;
                            if(HMW_basd_fre==ii)    HMW_fre_falg = true;
                            if(ObsFreflag==0)
                                fre_mw0 = ii;
                            else if(ObsFreflag==1)
                                fre_mw1 = ii;
                            else if(ObsFreflag==2)
                                fre_mw2 = ii;
                            else if(ObsFreflag==3)
                                fre_mw3 = ii;
                            ObsFreflag++;
                        }
                        if(HMW_fre_falg)
                        {
                            if(opt_corr->fre_model==double_fre)
                            {
                                UDUC_AR.UDUC_WidelaneAR(tempSatlitData,fre_mw0,fre_mw1,0);
                            }
                            else if(opt_corr->fre_model==triple_fre)
                            {
                                UDUC_AR.UDUC_WidelaneAR(tempSatlitData,fre_mw0,fre_mw1,0);
                                UDUC_AR.UDUC_WidelaneAR(tempSatlitData,HMW_basd_fre,fre_mw2,1);
                            }
                            else if(opt_corr->fre_model==four_fre)
                            {
                                UDUC_AR.UDUC_WidelaneAR(tempSatlitData,fre_mw0,fre_mw1,0);
                                UDUC_AR.UDUC_WidelaneAR(tempSatlitData,HMW_basd_fre,fre_mw2,1);
                                UDUC_AR.UDUC_WidelaneAR(tempSatlitData,HMW_basd_fre,fre_mw3,2);
                            }
                            else if(opt_corr->fre_model==five_fre)
                            {
                                UDUC_AR.UDUC_WidelaneAR(tempSatlitData,fre_mw0,fre_mw1,0);
                                UDUC_AR.UDUC_WidelaneAR(tempSatlitData,HMW_basd_fre,fre_mw2,1);
                                UDUC_AR.UDUC_WidelaneAR(tempSatlitData,HMW_basd_fre,fre_mw3,2);
                                UDUC_AR.UDUC_WidelaneAR(tempSatlitData,HMW_basd_fre,fre_mw3,3);
                            }
                        }
                    }
                    epochResultSatlitData.append(tempSatlitData);
                }//End of an epoch
            }
            else
            {
                epochResultSatlitData = epochSatlitData;
            }
            //qDebug()<<"UCAR 468";
            if(epochResultSatlitData.length() < trans_para->m_minSatFlag)
            {
                if((opt_corr->filter_model==PPP_KINEMATIC_KALM||opt_corr->filter_model==PPP_KINEMATIC_SRIF)&&continue_bad_epoch++ > 8)
                {
//                    prevEpochSatlitData.clear();// Exception reinitialization
                    continue_bad_epoch = 0;
                }
                trans_para->curr_GIFB_falg = false;
                trans_para->curr_C2IFB_falg = false;
                trans_para->curr_C3IFB3_falg = false;
                trans_para->curr_C3IFB4_falg = false;
                trans_para->curr_C3IFB5_falg = false;
                trans_para->curr_EIFB3_falg = false;
                trans_para->curr_EIFB4_falg = false;
                trans_para->curr_EIFB5_falg = false;
                for(int i = 0;i < epochResultSatlitData.length();i++)
                {
                }

                // display clock jump
//                disPlayQTextEdit = "Valid Satellite Number: " + QString::number(epochResultSatlitData.length()) + ENDLINE +
//                        "Waring: ***************Satellite number not sufficient*****************";
//                autoScrollTextEdit(mp_QTextEditforDisplay, disPlayQTextEdit);// display for QTextEdit
//                // translation to ENU
                VectorXd ENU_Vct;
                double spp_vct[3] = {0};
                int param_len = 3*epochResultSatlitData.length() + 32;
                ENU_Vct.resize(param_len);
                ENU_Vct.fill(0);
//                // debug by xiaogongwei 2020.04.24
                spp_p.saveResult2Class(ENU_Vct, spp_vct, epochTime, epochResultSatlitData, epoch_num,opt_corr,trans_para);
                epoch_num++;
                mutex.unlock();
                continue;
            }
            //Monitor satellite quality and cycle slip
            spp_p.Avail_satellite_sele(prevEpochSatlitData,epochResultSatlitData,opt_corr->m_CutAngle,spp_p.allBadSatlitData,opt_corr->Combination_mode,&trans_para->cyclyslip_sat_n);
            spp_p.GNSS_IFB_and_osbfre(epochResultSatlitData,trans_para);
            spp_p.Availa_SysFlag(opt_corr->m_sys_str,trans_para->sys_len,&trans_para->sys_num);
            //Satellite number not sufficient
            trans_para->m_minSatFlag=4+trans_para->sys_num+trans_para->curr_IFB_n;

            int Total_sat_fre=0;
            for(int mu_n=0;mu_n<epochResultSatlitData.length();mu_n++)//szg Mu
            {
                SatlitData MU_SatlitData = epochResultSatlitData.at(mu_n);
                Total_sat_fre+=MU_SatlitData.ObsLenfre;
            }
            //qDebug()<<"UCAR 512";
            // Choose solve method Kalman or SRIF
            MatrixXd P;
            VectorXd X;//Respectively dX, dY, dZ, dT (zenith tropospheric residual), dVt (receiver clock error), N1, N2...Nm (fuzzy)[dx,dy,dz,dTrop,dClock,N1,N2,...Nn]
            double spp_vct[3] = {0};// save spp pos
            double ppp_fix[3] = {0};//debug 0307 szg ppp
            bool WL_Constraint = true;
            int x_length = trans_para->const_parm+epochResultSatlitData.length();
            X.resize(x_length);
            X.setZero(x_length);

            int Par_sat_epoWL[epochResultSatlitData.length()];
            for(int k=0;k<epochResultSatlitData.length();k++)
                Par_sat_epoWL[k] = 0;
            trans_para->fixed_WL_number = 0;
            if(opt_corr->frequency_model==fixed_frequency)
            {
                Amb_res.Refe_star_selec(epochResultSatlitData,maxEAsat,ref_sat_prn);
                UDUC_AR.Wide_lane_uduc_smoothing(prevEpochSatlitData,epochResultSatlitData,trans_para);
                Amb_res.SDflag = UDUC_AR.get_uduc_SD_SatWl(epochResultSatlitData,ref_sat_prn,Par_sat_epoWL);
            }
            else if(opt_corr->frequency_model==mixed_frequency)
            {
                Amb_res.Refe_star_selec_mixed(epochResultSatlitData,maxEAsat,ref_sat_prn);
                UDUC_AR.get_uduc_ref_Sat(epochResultSatlitData,ref_sat_prn);
            }

            //qDebug()<<"UCAR 548";
            spp_vct[0] = spp_pos[0]; spp_vct[1] = spp_pos[1]; spp_vct[2] = spp_pos[2];
            if (opt_corr->filter_model==PPP_STATIC_SRIF||opt_corr->filter_model==PPP_KINEMATIC_SRIF)
                spp_p.pos_WL_srif_calculating(prevEpochSatlitData,epochResultSatlitData,spp_pos,X,P,Total_sat_fre,opt_corr,trans_para);
            else if(opt_corr->filter_model==PPP_STATIC_KALM||opt_corr->filter_model==PPP_KINEMATIC_KALM)
            {
                if(!WL_Constraint)
                    spp_p.pos_calculating(prevEpochSatlitData,epochResultSatlitData,spp_pos,X,P,Total_sat_fre,opt_corr,trans_para);
                else
                    spp_p.pos_WL_constraint_calculating(prevEpochSatlitData,epochResultSatlitData,spp_pos,X,P,Total_sat_fre,opt_corr,trans_para);
            }
            //qDebug()<<"UCAR 559";
            spp_p.GNSS_IFB_and_osbfre(epochResultSatlitData,trans_para);
            trans_para->pre_GIFB_falg = trans_para->curr_GIFB_falg;
            trans_para->pre_C2IFB_falg = trans_para->curr_C2IFB_falg;
            trans_para->pre_C3IFB3_falg = trans_para->curr_C3IFB3_falg;
            trans_para->pre_C3IFB4_falg = trans_para->curr_C3IFB4_falg;
            trans_para->pre_C3IFB5_falg = trans_para->curr_C3IFB5_falg;
            trans_para->pre_EIFB3_falg = trans_para->curr_EIFB3_falg;
            trans_para->pre_EIFB4_falg = trans_para->curr_EIFB4_falg;
            trans_para->pre_EIFB5_falg = trans_para->curr_EIFB5_falg;
            trans_para->curr_GIFB_falg = false;
            trans_para->curr_C2IFB_falg = false;
            trans_para->curr_C3IFB3_falg = false;
            trans_para->curr_C3IFB4_falg = false;
            trans_para->curr_C3IFB5_falg = false;
            trans_para->curr_EIFB3_falg = false;
            trans_para->curr_EIFB4_falg = false;
            trans_para->curr_EIFB5_falg = false;
            if(X(0)==NAN)
                X.setZero();
            // store spp position
//            Amb_res.Refe_star_selec(epochResultSatlitData,maxEAsat,ref_sat_prn);
            int Par_sat_epo[epochResultSatlitData.length()];
            Total_sat_fre=0;
            for(int k=0;k<epochResultSatlitData.length();k++)
            {
                Par_sat_epo[k] = 0;
                SatlitData MU_SatlitData = epochResultSatlitData.at(k);
                Total_sat_fre+=MU_SatlitData.ObsLenfre;
            }

            if(spp_p.delet_BadSat.length()>0)
            {
                for(int dele_i = 0;dele_i<spp_p.delet_BadSat.length();dele_i++)
                {
                    SatlitData delet_sat =  spp_p.delet_BadSat.at(dele_i);
                    spp_p.allBadSatlitData.append(delet_sat);
                }
            }
            int sys_N[4];
            for(int i=0;i<4;i++)    sys_N[i]=0;
            spp_p.system_sate_cual(epochResultSatlitData,sys_N);
            sate_disp->gpsSatNum[1] = sys_N[0];
            sate_disp->bdsSatNum[1] = sys_N[1];
            sate_disp->gloSatNum[1] = sys_N[2];
            sate_disp->galSatNum[1] = sys_N[3];
            spp_p.delet_BadSat.clear();
            int fix_flag = 0;VectorXd UDUC_SD_X;MatrixXd UDUC_SD_P;
            //Ambiguity of single difference narrow roadway between stars
            if(opt_corr->frequency_model==fixed_frequency)
            {

                if(opt_corr->Ambiguity_acquisition==poor_ambiguity)
                    UDUC_AR.UDUC_cascading_ambiguity_resolution(epochResultSatlitData,X,P,Total_sat_fre,UDUC_SD_X,fix_flag,trans_para,opt_corr);
                if(fix_flag>1)
                {
                    ppp_fix[0] = spp_vct[0] + UDUC_SD_X(0);ppp_fix[1] = spp_vct[1] + UDUC_SD_X(1);ppp_fix[2] = spp_vct[2] + UDUC_SD_X(2);
                }
                else
                {
                    ppp_fix[0] = spp_pos[0];ppp_fix[1] = spp_pos[1];ppp_fix[2] = spp_pos[2];
                }
            }
            else if(opt_corr->frequency_model==mixed_frequency)
            {
                UDUC_AR.UDUC_mixed_fre_cascading_ambiguity_resolution(epochResultSatlitData,X,P,Total_sat_fre,UDUC_SD_X,UDUC_SD_P,fix_flag,trans_para);
                //qDebug()<<"UCAR 624";
                if(fix_flag>1)
                {
                    //qDebug()<<"UCAR 627";
                    ppp_fix[0] = spp_vct[0] + UDUC_SD_X(0);ppp_fix[1] = spp_vct[1] + UDUC_SD_X(1);ppp_fix[2] = spp_vct[2] + UDUC_SD_X(2);
                }
                else
                {
                    ppp_fix[0] = spp_pos[0];ppp_fix[1] = spp_pos[1];ppp_fix[2] = spp_pos[2];
                }
            }
            trans_para->mode_solution = fix_flag;
            //qDebug()<<"UCAR 636";
            GNSS_PAR_auto *auto_par = AGNSS_APPData::sGetAPPDataInstance()->mGetGNSS_PAR_autoData();
            auto_par->dele_flag.clear();
            auto_par->PS_fixed.clear();
            auto_par->systemf.clear();
            for(int i=0;i<4;i++)
                for(int j=0;j<8;j++)
                auto_par->sys_f[j][i]=0;
            for(int i=0;i<5;i++)
            {
                maxEAsat[i] = 0.0;
                ref_sat_prn[i] = 0;
            }
            spp_p.DOP_vlaue(epochResultSatlitData,trans_para->Dops);
            //Save the last epoch satellite data
            prevEpochSatlitData = epochResultSatlitData;
            for(int i=0;i<trans_para->sys_len.size();i++)
               trans_para->up_sys_len[i] = trans_para->sys_len[i];

            //qDebug()<<"UCAR 655";
            // translation to ENU
            VectorXd ENU_Vct;
            ENU_Vct = X;// [dx,dy,dz,dTrop,dClock,N1,N2,...Nn]
            //ENU_Vct[0] = P(1,1); ENU_Vct[1] = P(2,2); ENU_Vct[2] = P(3,3);
            ENU_Vct[0] = spp_pos[0]; ENU_Vct[1] = spp_pos[1]; ENU_Vct[2] = spp_pos[2];
            // 定位结果显示
            wigdet_transmit result;
            result.transmit_xyz[0] = ENU_Vct[0];
            result.transmit_xyz[1] = ENU_Vct[1];
            result.transmit_xyz[2] = ENU_Vct[2];
            result.Year = epochTime.Year;
            result.Month = epochTime.Month;
            result.Day  = epochTime.Day;
            result.Hours = epochTime.Hours;
            result.Minutes = epochTime.Minutes;
            result.Seconds = epochTime.Seconds;
            result.DOP = trans_para->Dops[0];
            result.epoch = epoch_num;
            sate_disp->GDOP =trans_para->Dops[0];
            sate_disp->PDOP =trans_para->Dops[1];
            sate_disp->HDOP =trans_para->Dops[2];
            sate_disp->VDOP =trans_para->Dops[3];



            result.sog="0";
            result.cog="0";
            disply_textEdit *text_disp= AGNSS_APPData::sGetAPPDataInstance()->mGettrans_textEdit();
            for(int datalen=0;datalen<text_disp->data_lenth;datalen++)
            {
                QString fprintf_text = text_disp->data_text.at(datalen);
                result.data_text.append(fprintf_text);
            }
            //qDebug()<<"UCAR 689";
            result.data_lenth = text_disp->data_lenth;
            text_disp->data_lenth=0;
            text_disp->data_text.clear();
            mutex.unlock();
            emit(changexyz(result));
            spp_p.saveResult2Class(ENU_Vct, ppp_fix, epochTime, epochResultSatlitData, epoch_num,opt_corr,trans_para, &P);
            //qDebug()<<"UCAR 696";
            epochResultSatlitData.clear();
            epochSatlitData.clear();
            epoch_num++;//Increase in epoch
        }//End of multiple epochs.  (int n = 0; n < multepochSatlitData.length();n++)

        if(spp_p.isObsEnd()&&trans_para->for_and_backward_filter)
            trans_para->backward_filter = true;
        // clear multepochSatlitData
        multepochSatlitData.clear();
    }//Arrive at the end of the file. while (!m_ReadOFileClass.isEnd())
    spp_p.closeobsFile();
    spp_p.closeOBXFile();
    spp_p.closeBIAFile();
    QString infor = "End of program operation";
    disply_textEdit *text_disp= AGNSS_APPData::sGetAPPDataInstance()->mGettrans_textEdit();
    text_disp->data_text.append(infor);
    text_disp->data_lenth++;
    trans_para->backward_filter = false;
    epoch_num = 0;
}
