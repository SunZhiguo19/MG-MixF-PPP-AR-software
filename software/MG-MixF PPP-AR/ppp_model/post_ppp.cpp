#include "post_ppp.h"

post_ppp::post_ppp(QObject *parent) : QThread(parent)
{
    // 注册自定义类型信号槽
    qRegisterMetaType<wigdet_transmit>("wigdet_transmit");
}

post_ppp::~post_ppp()
{

}


void post_ppp::run(){


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
                post_ppp_calculating(opt_corr,trans_para,dat_fil,sate_disp,OFileNamesList);
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
            post_ppp_calculating(opt_corr,trans_para,dat_fil,sate_disp,OFileNamesList);
            if(!opt_corr->muilt_stations)
                break;
        }
    }
    AGNSS_APPData::sGetAPPDataInstance()->mSetAppFunState(false);

}

void post_ppp::sysMain()
{

   this->start();

}

//Read O files, sp3 files, clk files, and various error calculations, Kalman filtering ......................
//isDisplayEveryEpoch represent is disply every epoch information?(ENU or XYZ)
void post_ppp::post_ppp_calculating(const corr_opt *opt_corr, para_trans *trans_para,file_dat *dat_fil,disply_satellite *sate_disp,QStringList OFileNamesList,bool isDisplayEveryEpoch)
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
    //Traversing data one by one epoch, reading O file data
    QString disPlayQTextEdit = "";// display for QTextEdit
    QVector < SatlitData > prevEpochSatlitData;//Store satellite data of an epoch, use cycle slip detection（Put it on top, otherwise read multReadOFile epochs, the life cycle will expire when reading）
    double spp_pos[trans_para->const_parm] = {0};// store SPP pos and clk
    memcpy(spp_pos, trans_para->receiver_pos, 3*sizeof(double));
    int epoch_num = 0, continue_bad_epoch = 0;//Record the first epoch
    bool isInitSpp = false;
    if(spp_pos[0] !=0 ) isInitSpp = true;
    gtime_t Init_pos_time;
    trans_para->Initial_positioning_time=true;
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
            ephT =  Otime;
            if(epoch_num==970||epoch_num==8320||epoch_num==7020)
            {
                 int debuga = 666;
                 debuga=0;
            }
            //Set the epoch of the satellite
            for(int i = 0;i < epochSatlitData.length();i++)
                epochSatlitData[i].UTCTime.epochNum = epoch_num;
            // use spp compute postion and smooth pesudorange get clk at spp_pos[3]
            // !isInitSpp || m_isKinematic
//            SimpleSPP(prevEpochSatlitData, epochSatlitData, spp_pos,ephT,OtimeGNSSData);
//            if(opt_corr->filter_model==PPP_STATIC_KALM||opt_corr->filter_model==PPP_STATIC_SRIF)
//                memcpy(trans_para->receiver_pos, spp_pos, 3*sizeof(double));
            spp_p.SimpleSPP_model(prevEpochSatlitData,epochSatlitData,spp_pos,ephT,opt_corr,trans_para,dat_fil);
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

//                disPlayQTextEdit = "GPST: " + QString::number(epochTime.Hours) + ":" + QString::number(epochTime.Minutes)
//                        + ":" + QString::number(epochTime.Seconds) ;
//                autoScrollTextEdit(mp_QTextEditforDisplay, disPlayQTextEdit);// display for QTextEdit
//                disPlayQTextEdit = "Satellite number: " + QString::number(epochSatlitData.length())
//                                        + " jump satellites number is less than 5.";
//                autoScrollTextEdit(mp_QTextEditforDisplay, disPlayQTextEdit);// display for QTextEdit
//                // translation to ENU
                VectorXd ENU_Vct;
                double spp_vct[3] = {0};
                int param_len = 3*epochSatlitData.length() + 32;
                ENU_Vct.resize(param_len);
                ENU_Vct.fill(0);
                // debug by xiaogongwei 2020.04.24
                spp_p.saveResult2Class(ENU_Vct, spp_vct, epochTime, epochSatlitData, epoch_num,opt_corr,trans_para);
                epochResultSatlitData.clear();
                epochSatlitData.clear();
                epoch_num++;
                mutex.unlock();
                continue;
            }
            if(!trans_para->backward_filter)
            {
                //An epoch cycle begins
                for (int i = 0;i < epochSatlitData.length();i++)
                {

                    SatlitData tempSatlitData = epochSatlitData.at(i);//Store calculated corrected satellite data
                    if(!spp_p.isInSystem(tempSatlitData.SatType))
                        continue;
                    //Test whether the carrier and pseudorange are abnormal and terminate in time.
                    int IF_fre0=0,IF_fre1=1;
                    spp_p.IF_combination_fre(tempSatlitData,&IF_fre0,&IF_fre1,opt_corr);
                    if(opt_corr->fre_model != hybrid_fre&&opt_corr->Combination_mode==Comb_UD_mode&&opt_corr->frequency_model!=mixed_frequency)
                    {
                        if(opt_corr->fre_model!=tempSatlitData.ObsLenfre)
                            continue;
                    }
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
                     double pXYZ[3] = {0},pdXYZ[3] = {0},sp3Clk = 0.0;//Unit m
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
                    //PPP removes  satellites
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
                    //DCB
                    double stalitDCB = 0.0,satDcb_Var = 0.0;int dcbflag=0;
                    if(tempSatlitData.wantObserType[0].contains("1C",Qt::CaseInsensitive)&&tempSatlitData.SatType =='G')
                    {
                        tempSatlitData.stalitDCBFlag=1;
                        spp_p.getStaliteDCB(tempSatlitData.PRN,tempSatlitData.SatType,&stalitDCB,&satDcb_Var,tempSatlitData.stalitDCBFlag);
                        tempSatlitData.stalitDCB = stalitDCB;//szg debug 1230 dcb
                        tempSatlitData.SatDCB_Var = 0.0;//szg debug 0314 var  satDcb_Var
                        dcbflag =1;
                    }
                    //Calculate the autobiographic correction of the earth
                    double earthW = 0;
                    earthW = spp_p.getSagnac(tempSatlitData.X,tempSatlitData.Y,tempSatlitData.Z,spp_pos);
                    tempSatlitData.Sagnac = earthW;//earthW;
                    //Calculate tropospheric dry delay
                    double MJD = spp_p.computeJD(epochTime.Year,epochTime.Month,epochTime.Day,
                        epochTime.Hours,epochTime.Minutes,epochTime.Seconds) - 2400000.5;//Simplified Julian Day
                    tempSatlitData.MJD = (int)MJD;
                    tempSatlitData.seconds_day = epochTime.Hours*3600+epochTime.Minutes*60+epochTime.Seconds;
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
                    double SatL12Offset[10] = {0};
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
                    AntWindup = spp_p.getWindUp(epochTime.Year,epochTime.Month,epochTime.Day,
                                           epochTime.Hours,epochTime.Minutes,epochTime.Seconds - tempSatlitData.CodeP[0]/M_C,
                                           pXYZ,spp_pos,preAntWindup,spp_p.m_sunpos);
                    tempSatlitData.AntWindup = AntWindup;
                    for(int i=0;i<5;i++)
                    {
                        if(tempSatlitData.CodeP[i]&&tempSatlitData.PhaseL[i])
                        {
                            if(tempSatlitData.stalitDCBFlag==1&&tempSatlitData.wantObserType[2*i].contains("1C",Qt::CaseInsensitive))
                                tempSatlitData.CodePP[i]=tempSatlitData.Lambda[i]*(tempSatlitData.LxOffset[i] + tempSatlitData.SatLxOffset[i]) + tempSatlitData.CodeP[i]+tempSatlitData.stalitDCB* M_C*1e-9;
                            else
                                tempSatlitData.CodePP[i]=tempSatlitData.Lambda[i]*(tempSatlitData.LxOffset[i] + tempSatlitData.SatLxOffset[i]) + tempSatlitData.CodeP[i];
                            tempSatlitData.PhaseLL[i]=tempSatlitData.Lambda[i]*(tempSatlitData.LxOffset[i] + tempSatlitData.SatLxOffset[i]-tempSatlitData.AntWindup+tempSatlitData.PhaseL[i]);//- tempSatlitData.PHASE_BIA[0]
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
                    pritf_Widelane(tempSatlitData);//pritf widelane ambiguity
                    epochResultSatlitData.append(tempSatlitData);
                }//End of an epoch
            }
            else
            {
                epochResultSatlitData = epochSatlitData;
            }
            //Satellite number not sufficient
            if(epochResultSatlitData.length() < trans_para->m_minSatFlag)
            {
                if((opt_corr->filter_model==PPP_KINEMATIC_KALM||opt_corr->filter_model==PPP_KINEMATIC_SRIF)&&continue_bad_epoch++ > 8)
                {
//                    prevEpochSatlitData.clear();// Exception reinitialization
                    continue_bad_epoch = 0;
                }
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
                // debug by xiaogongwei 2020.04.24
                spp_p.saveResult2Class(ENU_Vct, spp_vct, epochTime, epochResultSatlitData, epoch_num,opt_corr,trans_para);
                epoch_num++;
                mutex.unlock();
                continue;
            }
            //Monitor satellite quality and cycle slip
            spp_p.Avail_satellite_sele(prevEpochSatlitData,epochResultSatlitData,opt_corr->m_CutAngle,spp_p.allBadSatlitData,opt_corr->Combination_mode,&trans_para->cyclyslip_sat_n);
            spp_p.Availa_SysFlag(opt_corr->m_sys_str,trans_para->sys_len,&trans_para->sys_num);
            trans_para->m_minSatFlag=4+trans_para->sys_num+trans_para->curr_IFB_n;

            int Total_sat_fre=0;
            for(int mu_n=0;mu_n<epochResultSatlitData.length();mu_n++)//szg Mu
            {
                SatlitData MU_SatlitData = epochResultSatlitData.at(mu_n);
                Total_sat_fre+=MU_SatlitData.ObsLenfre;
            }
            get_smooth_Wl(prevEpochSatlitData,epochResultSatlitData);
            // Choose solve method Kalman or SRIF
            MatrixXd P;
            VectorXd X;//Respectively dX, dY, dZ, dT (zenith tropospheric residual), dVt (receiver clock error), N1, N2...Nm (fuzzy)[dx,dy,dz,dTrop,dClock,N1,N2,...Nn]
            double spp_vct[3] = {0};// save spp pos
            bool is_filter_good = false;
            X.resize(trans_para->const_parm+epochResultSatlitData.length());
            X.setZero();
            // store spp position
            spp_vct[0] = spp_pos[0]; spp_vct[1] = spp_pos[1]; spp_vct[2] = spp_pos[2];
            if (opt_corr->filter_model==PPP_STATIC_SRIF||opt_corr->filter_model==PPP_KINEMATIC_SRIF)
                spp_p.pos_srif_calculating(prevEpochSatlitData,epochResultSatlitData,spp_pos,X,P,Total_sat_fre,opt_corr,trans_para);
            else if(opt_corr->filter_model==PPP_STATIC_KALM||opt_corr->filter_model==PPP_KINEMATIC_KALM)
                spp_p.pos_calculating(prevEpochSatlitData,epochResultSatlitData,spp_pos,X,P,Total_sat_fre,opt_corr,trans_para);
            if(spp_p.delet_BadSat.length()>0)
            {
                for(int dele_i = 0;dele_i<spp_p.delet_BadSat.length();dele_i++)
                {
                    SatlitData delet_sat =  spp_p.delet_BadSat.at(dele_i);
                    for(int flagi=0;flagi<sate_disp->sat_flag.length();flagi++)
                    {
                        if(delet_sat.PRN == sate_disp->sat_prn[flagi]&&delet_sat.SatType == sate_disp->sat_sys[flagi])
                            sate_disp->sat_flag[flagi] = 1;
                    }
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
            //Save the last epoch satellite data
            prevEpochSatlitData = epochResultSatlitData;
            spp_p.DOP_vlaue(epochResultSatlitData,trans_para->Dops);
            for(int i=0;i<trans_para->sys_len.size();i++)
                trans_para->up_sys_len[i] = trans_para->sys_len[i];


            // translation to ENU
            VectorXd ENU_Vct;
            ENU_Vct = X;// [dx,dy,dz,dTrop,dClock,N1,N2,...Nn]
            //ENU_Vct[0] = P(1,1); ENU_Vct[1] = P(2,2); ENU_Vct[2] = P(3,3);
            ENU_Vct[0] = spp_pos[0]; ENU_Vct[1] = spp_pos[1]; ENU_Vct[2] = spp_pos[2];
           // transmit_x = spp_pos[0]; transmit_y = spp_pos[1]; transmit_z = spp_pos[2];


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
            disply_textEdit *text_disp= AGNSS_APPData::sGetAPPDataInstance()->mGettrans_textEdit();
            for(int datalen=0;datalen<text_disp->data_lenth;datalen++)
            {
                QString fprintf_text = text_disp->data_text.at(datalen);
                result.data_text.append(fprintf_text);
            }
            result.data_lenth = text_disp->data_lenth;
            text_disp->data_lenth=0;
            text_disp->data_text.clear();
            mutex.unlock();
            emit(changexyz(result));



            /*经纬度输出*/
//            QString str,str1,str2,str3,str4,str5;
//            double rr[6]={0,0,0,0,0,0},pos[3]={0,0,0},min[2]={0,0};
//            int deg[2]={0,0};
//            for(int r = 0; r <3;r++ )
//            {
//               rr[r] =  spp_pos[r];
//            }


//            spp_p.ecef2Pos(rr,pos);
//            for(int i=0;i<2;i++)
//            {
//                spp_p.rad2DMS(pos[i],deg[i],min[i]);
//            }



//            for(int i=0;i<2;i++)
//            {
//                spp_p.rad2DMS(pos[i],deg[i],min[i]);
//            }
//            result.lat=QString("%1 %2 %3 %4%5").arg(str.sprintf("%3d",deg[0])).arg(tr("°")).arg(str1.sprintf("%7.5f",min[0])).arg(tr("’")).arg(pos[1]>0?"N":"S");
//            result.lot=QString("%1 %2 %3 %4%5").arg(str.sprintf("%3d",deg[1])).arg(tr("°")).arg(str1.sprintf("%7.5f",min[1])).arg(tr("’")).arg(pos[0]>0?"E":"W");
//            result.height=QString("%1 %2").arg(str.sprintf("%.2f",pos[2])).arg(tr("m"));
//            result.hdop=str.sprintf("%4.2f",trans_para->Dops[2]);
            result.sog="0";
            result.cog="0";

            spp_p.saveResult2Class(ENU_Vct, spp_vct, epochTime, epochResultSatlitData, epoch_num,opt_corr,trans_para, &P);
            epochResultSatlitData.clear();
            epochSatlitData.clear();
            epoch_num++;//Increase in epoch
        }//End of multiple epochs.  (int n = 0; n < multepochSatlitData.length();n++)
        if(spp_p.isObsEnd()&&trans_para->for_and_backward_filter)
            trans_para->backward_filter = true;

        // clear multepochSatlitData
        multepochSatlitData.clear();
    }//Arrive at the end of the file. while (!m_ReadOFileClass.isEnd())
    epoch_num = 0;
    trans_para->backward_filter = false;
    spp_p.closeobsFile();

}

/*****GREAT UPD MW Combined Fixed ambiguity**/
bool post_ppp::pritf_Widelane(SatlitData &oneSatlitData)
{//
    int fre_num0=oneSatlitData.IF_Frq[0],fre_num1=oneSatlitData.IF_Frq[1];
    double F1 = oneSatlitData.Frq[fre_num0],F2 = oneSatlitData.Frq[fre_num1];//Get the frequency of this satellite
//    double Lamta1 = oneSatlitData.Lambda[fre_num0],Lamta2 = oneSatlitData.Lambda[fre_num1];
    double lamdaMW = M_C/(F1 - F2);
//    double dcbB1 = F2*F2/(F1*F1 - F2*F2);
//    double dcbB2 = F1*F1/(F1*F1 - F2*F2);
    double L1s = 0.0 ,L2s = 0.0 ;
    double C1s = 0.0 ,C2s = 0.0 ;
    L1s = oneSatlitData.PhaseL[fre_num0];//debug 0112 szg -oneSatlitData.stalitDCB
    L2s = oneSatlitData.PhaseL[fre_num1];//
    if(oneSatlitData.stalitDCBFlag == 1)
        C1s = oneSatlitData.CodeP[fre_num0] + oneSatlitData.stalitDCB * M_C * 1e-9;//- oneSatlitData.CODE_BIA[0]
    else
        C1s = oneSatlitData.CodeP[fre_num0];//
    C2s = oneSatlitData.CodeP[fre_num1];//
    double IF_wl = (L1s - L2s) - (F1*C1s + F2*C2s)/((F1+F2)*lamdaMW);
    oneSatlitData.MW_Wl_Int = IF_wl;

    return true;
}
//Smoothing wide lane ambiguity and fixing single difference wide lane ambiguity between satellites szg
void post_ppp::get_smooth_Wl(QVector< SatlitData > &prevEpochSatlitData,QVector< SatlitData > &epochSatlitData)
{
    int preG_PRN[MAXPRNGPS],preC_PRN[MAXPRNBDS],preR_PRN[MAXPRNGLO],preE_PRN[MAXPRNGLE];
    for(int i=0;i<MAXPRNBDS;i++)
    {
        preC_PRN[i] = -1;
        if(i<MAXPRNGPS)
            preG_PRN[i] = -1;
        if(i<MAXPRNGLO)
            preR_PRN[i] = -1;
        if(i<MAXPRNGLE)
            preE_PRN[i] = -1;
    }
    for(int i = 0;i < prevEpochSatlitData.length();i++)
    {
        SatlitData preEpoSat = prevEpochSatlitData.at(i);
        switch (preEpoSat.SatType) {
        case 'G':
            preG_PRN[preEpoSat.PRN-1] = i;
            break;
        case 'C':
            preC_PRN[preEpoSat.PRN-1] = i;
            break;
        case 'R':
            preR_PRN[preEpoSat.PRN-1] = i;
            break;
        case 'E':
            preE_PRN[preEpoSat.PRN-1] = i;
            break;
        default:
            break;
        }
    }
    QVector< SatlitData > tempEpochSatlitData;
    for(int i = 0;i < epochSatlitData.length();i++)
    {
        SatlitData currentEpoSat = epochSatlitData.at(i);
        double SmoothSatlit = 0.0,SmoothStd = 0.0,SDWlar = 0.0;
        int SmoEpNum = 0,Pre_prn = -1;/*Number of smooth epochs*/
        switch (currentEpoSat.SatType) {
        case 'G':
            Pre_prn= preG_PRN[currentEpoSat.PRN-1] ;
            break;
        case 'C':
            Pre_prn= preC_PRN[currentEpoSat.PRN-1] ;
            break;
        case 'R':
            Pre_prn= preR_PRN[currentEpoSat.PRN-1] ;
            break;
        case 'E':
            Pre_prn= preE_PRN[currentEpoSat.PRN-1] ;
            break;
        default:
            break;
        }
        if(Pre_prn!=-1&&currentEpoSat.cycly_slipF!=1&&currentEpoSat.Domain_of_doubt!=-1)
        {
            SatlitData preEpoSat = prevEpochSatlitData.at(Pre_prn);
            SmoothSatlit = preEpoSat.satMW_wlAmbi[0];
            SmoEpNum     = preEpoSat.satMW_wlSmoN + 1;//smooth epo
            SmoothStd    = preEpoSat.satMW_wlAmbi[1] * (SmoEpNum -1);
        }
        else
        {
            SmoothSatlit = 0;
            SmoothStd    = 0;
            SmoEpNum     = 1;//smooth epo
        }
        SDWlar = currentEpoSat.MW_Wl_Int;/*MW wide roadway ambiguity*/
        currentEpoSat.satMW_wlAmbi[0] = SmoothSatlit*(SmoEpNum -1)/SmoEpNum + SDWlar*1.0/SmoEpNum;//平滑后宽巷模糊度
        if(SmoEpNum == 1)
            currentEpoSat.std_devia =  (SDWlar - (int)SDWlar)*(SDWlar - (int)SDWlar) * 1.0/SmoEpNum;
        else
            currentEpoSat.std_devia =  SmoothStd * (SmoEpNum -1)/SmoEpNum + (currentEpoSat.satMW_wlAmbi[0] - SDWlar)*(currentEpoSat.satMW_wlAmbi[0] - SDWlar) * 1.0/SmoEpNum;
        currentEpoSat.satMW_wlAmbi[1] = currentEpoSat.std_devia/SmoEpNum;
        currentEpoSat.satMW_wlSmoN = SmoEpNum;
        tempEpochSatlitData.append(currentEpoSat);
    }
    epochSatlitData.clear();
    epochSatlitData = tempEpochSatlitData;
}
