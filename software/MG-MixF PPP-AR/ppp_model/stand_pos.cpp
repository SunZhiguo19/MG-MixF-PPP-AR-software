#include "stand_pos.h"

stand_pos::stand_pos()
{

}
void stand_pos::SimpleSPP_model(QVector < SatlitData > &prevEpochSatlitData, QVector < SatlitData > &epochSatlitData, double *spp_pos,gtime_t epht,
                                const corr_opt *opt_corr, para_trans *trans_para,file_dat *dat_fil)
{
    double p_HEN[3] = {0};
    getAntHEN(p_HEN);//Get the antenna high
    GPSPosTime epochTime;//Obtaining observation time
    int sys_N[4];
    for(int i = 0;i<4;i++) sys_N[i]=0;
    system_sate_cual(epochSatlitData,sys_N);
    disply_satellite *sate_disp = AGNSS_APPData::sGetAPPDataInstance()->mGettrans_sate_dispData();
    sate_disp->gpsSatNum[0] = sys_N[0];
    sate_disp->bdsSatNum[0] = sys_N[1];
    sate_disp->gloSatNum[0] = sys_N[2];
    sate_disp->galSatNum[0] = sys_N[3];
    if(epochSatlitData.length() > 0)
        epochTime = epochSatlitData.at(0).UTCTime;//Obtaining observation time
    else
        return ;
    Vector3d tempPos3d, diff_3d;
    tempPos3d[0] = spp_pos[0]; tempPos3d[1] = spp_pos[1]; tempPos3d[2] = spp_pos[2];
    QVector< SatlitData > store_currEpoch;
    int max_iter = 30,Total_sat_fre=0;
    trans_para->sys_len.setZero();
    for(int iterj = 0; iterj < max_iter; iterj++)
    {
        disply_satellite satellite_disply;
        QVector< SatlitData > curr_Epoch;
        if(allBadSatlitData.length()!=0)
            allBadSatlitData.clear();
        if(trans_para->backward_filter)
        {
            curr_Epoch= epochSatlitData;
            system_num(curr_Epoch);
            Availa_SysFlag(opt_corr->m_sys_str,trans_para->sys_len,&trans_para->sys_num);
        }
        else
        {
            // get every satilite data
            for (int i = 0;i < epochSatlitData.length();i++)
            {
                SatlitData tempSatlitData = epochSatlitData.at(i);//Store calculated corrected satellite data
                if(!isInSystem(tempSatlitData.SatType))
                    continue;
                int IF_fre0=0,IF_fre1=1;
                IF_combination_fre(tempSatlitData,&IF_fre0,&IF_fre1,opt_corr);
        //Test whether the carrier and pseudorange are abnormal and terminate in time.
                if(opt_corr->Combination_mode==Comb_IF_mode&&!(tempSatlitData.CodeP[IF_fre0]&&tempSatlitData.CodeP[IF_fre1]&&tempSatlitData.PhaseL[IF_fre0]&&tempSatlitData.PhaseL[IF_fre1]))
                {
                    QString errorline;
                    ErrorMsg(errorline);
                    tempSatlitData.badMsg.append("Lack of observations"+errorline);
                    allBadSatlitData.append(tempSatlitData);
                    continue;
                }
                double stalitClock[1] = {0.0},stalitClockVar[1]={0},sat_UPD,pOrb_Var[1]={0};
                double pXYZ[3] = {0},pdXYZ[3] = {0}, sp3Clk = 0.0;//Unit m
                if(opt_corr->treat_method==POS_PP_PPP||opt_corr->treat_method==POS_PP_PPP_AR_2||opt_corr->treat_method==POS_PP_PPP_AR_1||opt_corr->treat_method==POS_Real_PPP_AR||opt_corr->treat_method==POS_UDUC_PPP_AR)
                {
                    //When seeking GPS
                    double m_PrnGpst = YMD2GPSTime(epochTime.Year,epochTime.Month,epochTime.Day,
                        epochTime.Hours,epochTime.Minutes,epochTime.Seconds);
                    getStaliteClk(tempSatlitData.PRN,tempSatlitData.SatType,m_PrnGpst - tempSatlitData.CodeP[0]/M_C,stalitClock,stalitClockVar,&sat_UPD,dat_fil);
                    //Obtain the coordinates of the epoch satellite from the SP3 data data
                    getPrcisePoint(tempSatlitData.PRN,tempSatlitData.SatType,m_PrnGpst - tempSatlitData.CodeP[0]/M_C - stalitClock[0]/M_C,pXYZ,pdXYZ,&sp3Clk,pOrb_Var);
                    tempSatlitData.StaClock = stalitClock[0];
                    tempSatlitData.X = pXYZ[0];tempSatlitData.Y = pXYZ[1];tempSatlitData.Z = pXYZ[2];
                }

                //Calculate the satellite's high sitting angle (as the receiver approximates the target)
                double EA[2]={0};
                getSatEA(pXYZ,spp_pos,EA);
                tempSatlitData.EA[0] = EA[0];tempSatlitData.EA[1] = EA[1];
                satellite_disply.el.append(EA[0]);

                satellite_disply.sat_sys.append(tempSatlitData.SatType);
                satellite_disply.sat_prn.append(tempSatlitData.PRN);
                EA[0] = EA[0]*MM_PI/180;EA[1] = EA[1]*MM_PI/180;//Go to the arc to facilitate the calculation below
                satellite_disply.az.append(EA[1]);
                tempSatlitData.SatWight = 0.01;// debug xiaogongwei 2018.11.16
                sine_Wight(tempSatlitData);
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
                    allBadSatlitData.append(tempSatlitData);
                    satellite_disply.sat_flag.append(1);
                    continue;
                }
        //Test the state of the precise ephemeris and the clock difference and whether the carrier and pseudorange are abnormal, and terminate the XYZ or the satellite with the clock difference of 0 in time.
                if (!(tempSatlitData.X&&tempSatlitData.Y&&tempSatlitData.Z&&tempSatlitData.StaClock))
                {
                    QString errorline;
                    ErrorMsg(errorline);
                    tempSatlitData.badMsg.append("Can't calculate the orbit and clock offset"+errorline);
                    allBadSatlitData.append(tempSatlitData);
                    satellite_disply.sat_flag.append(1);
                    continue;
                }
        //Quality control (height angle pseudorange difference)
                if (opt_corr->Combination_mode==Comb_IF_mode&&(qAbs(tempSatlitData.CodeP[IF_fre0] - tempSatlitData.CodeP[IF_fre1]) > 50))
                {
                    QString errorline;
                    ErrorMsg(errorline);
                    tempSatlitData.badMsg.append("C1-C2>50"+errorline);
                    allBadSatlitData.append(tempSatlitData);
                    satellite_disply.sat_flag.append(1);
                    continue;
                }
                if(spp_pos[0] !=0 && tempSatlitData.EA[0] < opt_corr->m_CutAngle)
                {
                    QString errorline;
                    ErrorMsg(errorline);
                    tempSatlitData.badMsg.append("elevation angle is " + QString::number(tempSatlitData.EA[0],'f',2)
                            + " less " + QString::number(opt_corr->m_CutAngle,'f',2) +errorline);
                    allBadSatlitData.append(tempSatlitData);
                    satellite_disply.sat_flag.append(1);
                    continue;
                }
                satellite_disply.sat_flag.append(0);
                //Computational relativity correction
                double relative = 0;
                if(spp_pos[0] !=0 )
                    relative = getRelativty(tempSatlitData.SatType, pXYZ,spp_pos,pdXYZ);
                tempSatlitData.Relativty = relative;
                //Calculate the autobiographic correction of the earth
                double earthW = 0;
                earthW = getSagnac(tempSatlitData.X,tempSatlitData.Y,tempSatlitData.Z,spp_pos);
                tempSatlitData.Sagnac = 0;
                if(spp_pos[0] !=0 ) tempSatlitData.Sagnac = earthW;
                //Calculate tropospheric dry delay!!!
                double MJD = computeJD(epochTime.Year,epochTime.Month,epochTime.Day,
                    epochTime.Hours,epochTime.Minutes,epochTime.Seconds) - 2400000.5;//Simplified Julian Day
                //Calculate and save the annual accumulation date
                double TDay = YearAccDay(epochTime.Year,epochTime.Month,epochTime.Day);
                double p_BLH[3] = {0},mf = 0, TropZPD = 0,trop_pre=0,trop_var=0;
                XYZ2BLH(spp_pos, p_BLH);
                if(spp_pos[0] !=0 )
                    trop_delay(epht,p_BLH,TDay,EA,trop_pre,opt_corr->trop_selec,&mf,&TropZPD,&trop_var);
                tempSatlitData.SatTrop = TropZPD;
                tempSatlitData.StaTropMap = mf;
                double Sationex=0;double ion_para[8];for(int i=0;i<8;i++)   ion_para[i]=0;
                if(spp_pos[0] !=0 )
                    Sationex=ionmodel(epht,ion_para,p_BLH,EA);
                for(int i=0;i<5;i++)
                {
                    if(fabs(Sationex)>150)
                        Sationex = 0;
                    tempSatlitData.SatIon[i] = Sationex*(tempSatlitData.Lambda[i]/M_C*g_GPSFrq[1])*(tempSatlitData.Lambda[i]/M_C*g_GPSFrq[1]);
                }
                if(spp_pos[0] !=0 ) tempSatlitData.StaTropMap = mf;
                //Calculate antenna high offset correction  Antenna Height
                if( spp_pos[0] !=0 )
                    tempSatlitData.AntHeight = p_HEN[0]*qSin(EA[0]) + p_HEN[1]*qCos(EA[0])*qSin(EA[1]) + p_HEN[2]*qCos(EA[0])*qCos(EA[1]);
                //Receiver L1 L2 offset correction
                double LxOffset[5]= {0};
                getRecvL12(EA[0],EA[1],tempSatlitData.SatType,LxOffset,tempSatlitData.wantObserType);
                for(int i=0;i<5;i++)
                {
                    if(tempSatlitData.Lambda[i]!=0)
                        tempSatlitData.LxOffset[i] = LxOffset[i]/tempSatlitData.Lambda[i];
                }
                //Satellite antenna phase center correction store data to (m_ReadAntClass.m_sunpos,m_ReadAntClass.m_moonpos,m_ReadAntClass.m_gmst)
                // and update sunpos and moonpos
                double SatL12Offset[10] = {0};
                double Obst[6] = {double(epochTime.Year),double(epochTime.Month),double(epochTime.Day),
                                  double(epochTime.Hours),double(epochTime.Minutes),epochTime.Seconds};
                tempSatlitData.Otime = epht;
                if(spp_pos[0] !=0 )
                    getSatOffSet(Obst,tempSatlitData.PRN,tempSatlitData.SatType,pXYZ,spp_pos,SatL12Offset, tempSatlitData.wantObserType);
                for(int i=0;i<5;i++)
                {
                    if(tempSatlitData.Lambda[i]!=0)
                    {
                        tempSatlitData.SatLxOffset[i] = SatL12Offset[i]/tempSatlitData.Lambda[i];
                        tempSatlitData.Sat_LxPCO[i] = SatL12Offset[5+i]/tempSatlitData.Lambda[i];
                    }
                }
                //Calculate antenna phase winding
                double AntWindup = 0,preAntWindup = 0;
                //Find the previous epoch. Is there a satellite present? The deposit is stored in preAntWindup or preAntWindup=0.
                if( spp_pos[0] !=0 )
                {
                    preAntWindup = getPreEpochWindUp(prevEpochSatlitData,tempSatlitData.PRN,tempSatlitData.SatType);//Get the previous epoch of WindUp
                    AntWindup = getWindUp(epochTime.Year,epochTime.Month,epochTime.Day,
                        epochTime.Hours,epochTime.Minutes,epochTime.Seconds - tempSatlitData.CodeP[0]/M_C,
                        pXYZ,spp_pos,preAntWindup,m_sunpos);
                }
                tempSatlitData.AntWindup = AntWindup;
                bool obs_find = false;
                //Computation to eliminate ionospheric pseudorange and carrier combinations (here absorbed receiver carrier deflection and WindUp) add SatL1Offset and SatL1Offset by xiaogongwei 2019.04.12
                for(int i=0;i<5;i++)
                {
                    if(tempSatlitData.CodeP[i]!=0&&tempSatlitData.PhaseL[i]!=0)
                    {
                        tempSatlitData.ObsLenfre++;
                        obs_find = true;
                        tempSatlitData.ObsFreflag[i] = 1;
                        tempSatlitData.CodePP[i]=tempSatlitData.Lambda[i]*(tempSatlitData.LxOffset[i] + tempSatlitData.SatLxOffset[i]) + tempSatlitData.CodeP[i]+tempSatlitData.CODE_BIA[i];
        //                tempSatlitData.PhaseLL[i]=tempSatlitData.Lambda[i]*(tempSatlitData.LxOffset[i] + tempSatlitData.SatLxOffset[i]-tempSatlitData.AntWindup+tempSatlitData.PhaseL[i]);//- tempSatlitData.PHASE_BIA[0]
                    }
                }
                if(!obs_find)   continue;
                if(opt_corr->Combination_mode==Comb_IF_mode)
                {

                    //Computation to eliminate ionospheric pseudorange and carrier combinations (here absorbed receiver carrier deflection and WindUp) add SatL1Offset and SatL1Offset
                    double alpha1 = (tempSatlitData.Frq[IF_fre0]*tempSatlitData.Frq[IF_fre0])/(tempSatlitData.Frq[IF_fre0]*tempSatlitData.Frq[IF_fre0] - tempSatlitData.Frq[IF_fre1]*tempSatlitData.Frq[IF_fre1]);
                    double alpha2 = (tempSatlitData.Frq[IF_fre1]*tempSatlitData.Frq[IF_fre1])/(tempSatlitData.Frq[IF_fre0]*tempSatlitData.Frq[IF_fre0] - tempSatlitData.Frq[IF_fre1]*tempSatlitData.Frq[IF_fre1]);
                    tempSatlitData.IF_Frq[0] = IF_fre0;
                    tempSatlitData.IF_Frq[1] = IF_fre1;
                    tempSatlitData.IF_P = alpha1 * tempSatlitData.CodePP[IF_fre0] - alpha2 * tempSatlitData.CodePP[IF_fre1];
                    tempSatlitData.IF_L = alpha1 * tempSatlitData.PhaseLL[IF_fre0] - alpha2 * tempSatlitData.PhaseLL[IF_fre1];
                }            // save data to curr_Epoch
                int BDS_t =0;
                for(int k=0;k<opt_corr->m_sys_str.length();k++)
                {
                    if(tempSatlitData.SatType == opt_corr->m_sys_str[k])
                    {
                        if(tempSatlitData.SatType=='C'&&tempSatlitData.BDS_2flag)
                            trans_para->sys_len[k] = 1;
                        else if(tempSatlitData.SatType=='C')
                            trans_para->sys_len[k+1] = 1;
                        else
                            trans_para->sys_len[k+BDS_t] = 1;
                    }
                    else if(tempSatlitData.SatType=='C')
                        BDS_t++;
                }
                curr_Epoch.append(tempSatlitData);
            }

        }
        // judge satilite number large 4
        if(curr_Epoch.length() < 5)
        {
            memset(spp_pos, 0, 3*sizeof(double));
            epochSatlitData = curr_Epoch;
            return ;
        }
        Total_sat_fre = 0;
        for(int mu_n=0;mu_n<curr_Epoch.length();mu_n++)//szg Mu
        {
            SatlitData MU_SatlitData = curr_Epoch.at(mu_n);
            Total_sat_fre+=MU_SatlitData.ObsLenfre;
        }
        // get equation
        MatrixXd mat_B, mat_P;
        VectorXd Vct_L, Xk;
        Vector3d XYZ_Pos;
        trans_para->sys_num = trans_para->sys_len.sum();
        if(opt_corr->Combination_mode==Comb_IF_mode)
        {
            Obtaining_SPP_IF_equation(curr_Epoch, spp_pos, mat_B, Vct_L, mat_P,opt_corr,trans_para);
        }
        else
        {
            Obtaining_SPP_NoCombination_equation(curr_Epoch, spp_pos, mat_B, Vct_L, mat_P,Total_sat_fre,opt_corr,trans_para);
        }

        // slover by least square
        Xk = (mat_B.transpose()*mat_P*mat_B).inverse()*mat_B.transpose()*mat_P*Vct_L;
        XYZ_Pos[0] = tempPos3d[0] + Xk[0];
        XYZ_Pos[1] = tempPos3d[1] + Xk[1];
        XYZ_Pos[2] = tempPos3d[2] + Xk[2];
        diff_3d = XYZ_Pos - tempPos3d;
        tempPos3d = XYZ_Pos;// save slover pos
        // update spp_pos
        spp_pos[0] = XYZ_Pos[0]; spp_pos[1] = XYZ_Pos[1]; spp_pos[2] = XYZ_Pos[2];
        // debug by xiaogongwei 2018.11.17
        sate_disp->az = satellite_disply.az;
        sate_disp->el = satellite_disply.el;
        sate_disp->sat_flag=satellite_disply.sat_flag;
        sate_disp->sat_prn=satellite_disply.sat_prn;
        sate_disp->sat_sys=satellite_disply.sat_sys;

        if(diff_3d.cwiseAbs().maxCoeff() < 1)
        {
            spp_pos[3] = Xk[3];// save base clk
            store_currEpoch = curr_Epoch;
            break;
        }
        if(diff_3d.cwiseAbs().maxCoeff() > 2e7 || !isnormal(diff_3d[0]) || iterj == max_iter - 1)
        {
            memset(spp_pos, 0, 4*sizeof(double));
            epochSatlitData = curr_Epoch;// debug by xiaogongwei 2019.09.25
            return ;
        }
    }
    //Monitor satellite quality and cycle slip
    if(store_currEpoch.length() < trans_para->const_parm)
    {
        memset(spp_pos, 0, 4*sizeof(double));
        epochSatlitData = store_currEpoch;
        return ;
    }
    bd2_multipath(store_currEpoch,opt_corr);
    epochSatlitData = store_currEpoch;
}

void stand_pos::IF_combination_fre(const SatlitData &IF_satlit, int *fre_0, int *fre_1,const corr_opt *opt_corr)
{
    if(IF_satlit.SatType=='G')
    {
        *fre_0 = opt_corr->IF0_combination[0];
        *fre_1 = opt_corr->IF1_combination[0];
    }
    else if(IF_satlit.SatType=='C')
    {
        if(IF_satlit.BDS_2flag)
        {
            *fre_0 = opt_corr->IF0_combination[1];
            *fre_1 = opt_corr->IF1_combination[1];
        }
        else
        {
            *fre_0 = opt_corr->IF0_combination[2];
            *fre_1 = opt_corr->IF1_combination[2];
        }
    }
    else if(IF_satlit.SatType=='R')
    {
        *fre_0 = opt_corr->IF0_combination[3];
        *fre_1 = opt_corr->IF1_combination[3];
    }
    else if(IF_satlit.SatType=='E')
    {
        *fre_0 = opt_corr->IF0_combination[4];
        *fre_1 = opt_corr->IF1_combination[4];
    }
}
//szg ssr file name arragement
void stand_pos::arrange(int epocha,QStringList SsrFileNameList)
{
    if(epocha == 0)
    {
        int ssrfilenum =  SsrFileNameList.length();
        QString SSRfilename;
        int ssrTim[ssrfilenum];
        for(int i=0;i<ssrfilenum;i++)
        {
            m_SSRFileName[i] = SsrFileNameList.at(i);
            ssrTim[i] = m_SSRFileName[i].mid(m_SSRFileName[i].length()-8,4).toInt();
        }
        for(int i=0;i<ssrfilenum;i++)
        {
            int ssrnumS;
            for(int j=0;j<ssrfilenum-i-1;j++)
            {
                if(ssrTim[j]>ssrTim[j+1])
                {
                     ssrnumS = ssrTim[j];
                     ssrTim[j] = ssrTim[j+1];
                     ssrTim[j+1] = ssrnumS;
                     SSRfilename = m_SSRFileName[j];
                     m_SSRFileName[j] = m_SSRFileName[j+1];
                     m_SSRFileName[j+1] = SSRfilename;
                }
            }
        }
    }
}
void stand_pos::saveResult2Class(VectorXd X, double *spp_vct, GPSPosTime epochTime, QVector< SatlitData > epochResultSatlitData,
                                 int epochNum, const corr_opt *opt_corr, para_trans *trans_para, MatrixXd *P)
{
    //Store coordinate data
    RecivePos epochRecivePos;
    epochTime.epochNum = epochNum;
    epochRecivePos.UTCtime = epochTime;

    epochRecivePos.totolEpochStalitNum = epochResultSatlitData.length();
    epochRecivePos.dX = X[0];
    epochRecivePos.dY = X[1];
    epochRecivePos.dZ = X[2];
    epochRecivePos.spp_pos[0] = spp_vct[0];
    epochRecivePos.spp_pos[1] = spp_vct[1];
    epochRecivePos.spp_pos[2] = spp_vct[2];
    epochRecivePos.radio = trans_para->lambda_radio;
    epochRecivePos.fixed_sat = trans_para->fixed_sat;
    for(int i=0;i<4;i++)   epochRecivePos.dops[i] =  trans_para->Dops[i];
//    if(!m_IS_MAX_OBS) allReciverPos.append(epochRecivePos);
    //Save wet delay and receiver clock error
    double epoch_ZHD = 0.0;
    if(epochResultSatlitData.length() >= trans_para->m_minSatFlag) epoch_ZHD = epochResultSatlitData.at(0).UTCTime.TropZHD;
    ClockData epochRecClock;
    epochRecClock.UTCTime= epochRecivePos.UTCtime;
    if(X(3) == 0)
        epochRecClock.ZTD_W = 0;
    else
        epochRecClock.ZTD_W = X(3) + epoch_ZHD;//Storage zenith wet delay + zenith dry delay
    // save clock
    memset(epochRecClock.clockData, 0, 6*sizeof(double));
    int BDS_clock=0,Effective_clock=0;
    //Stores the receiver skew of the first system, and the relative offset of its other systems  GCRE
    for(int i = 0;i < opt_corr->m_sys_str.length();i++)
    {
        switch (opt_corr->m_sys_str.at(i).toLatin1()) {
        case 'G':
            if(trans_para->up_sys_len[i+BDS_clock]==1)
            {
                epochRecClock.clockData[0] = X(4+Effective_clock);
                Effective_clock++;
            }
            break;
        case 'C':
            if(trans_para->up_sys_len[i+BDS_clock]==1)
            {
                epochRecClock.clockData[1] = X(4+Effective_clock);
                Effective_clock++;
            }
            BDS_clock++;
            if(trans_para->up_sys_len[i+BDS_clock]==1)
            {
                epochRecClock.clockData[2] = X(4+Effective_clock);
                Effective_clock++;
            }
            break;
        case 'R':
            if(trans_para->up_sys_len[i+BDS_clock]==1)
            {
                epochRecClock.clockData[3] = X(4+Effective_clock);
                Effective_clock++;
            }
            break;
        case 'E':
            if(trans_para->up_sys_len[i+BDS_clock]==1)
            {
                epochRecClock.clockData[4] = X(4+Effective_clock);
                Effective_clock++;
            }
            break;
        default:
            break;
        }
    }
    if(opt_corr->Combination_mode == Comb_UD_mode)
    {
        int sat_num = epochResultSatlitData.length();
        //Save satellite ambiguity
        Ambiguity oneSatAmb;
        for (int i = 0;i < sat_num;i++)
        {
            SatlitData oneSat = epochResultSatlitData.at(i);
            oneSatAmb.PRN = oneSat.PRN;
            oneSatAmb.SatType = oneSat.SatType;
            oneSatAmb.UTCTime = epochRecClock.UTCTime;
            oneSatAmb.isIntAmb = false;
            memcpy(oneSatAmb.EA, oneSat.EA, 2*sizeof(double));
            oneSatAmb.ionL1 = X(i+trans_para->const_parm);
//            oneSatAmb.Amb1 = X(i+trans_para->const_parm+sat_num);
//            oneSatAmb.Amb2 = X(i+trans_para->const_parm+2*sat_num);
            oneSatAmb.Amb = 0.0;
            epochResultSatlitData[i].ionL1 = X(i+trans_para->const_parm);
//            epochResultSatlitData[i].Amb1 = X(i+trans_para->const_parm+sat_num);
//            epochResultSatlitData[i].Amb2 = X(i+trans_para->const_parm+2*sat_num);
            epochResultSatlitData[i].Amb = 0.0;
            oneSatAmb.UTCTime.epochNum = epochNum;
        }
    }
    else if(opt_corr->Combination_mode == Comb_IF_mode)
    {
        //Save satellite ambiguity
        Ambiguity oneSatAmb;
        for (int i = 0;i < epochResultSatlitData.length();i++)
        {
            SatlitData oneSat = epochResultSatlitData.at(i);
            oneSatAmb.PRN = oneSat.PRN;
            oneSatAmb.SatType = oneSat.SatType;
            oneSatAmb.UTCTime = epochRecClock.UTCTime;
            oneSatAmb.isIntAmb = false;
            memcpy(oneSatAmb.EA, oneSat.EA, 2*sizeof(double));
            oneSatAmb.ionL1 = 0.0;
            oneSatAmb.Amb1 = 0.0;
            oneSatAmb.Amb2 = 0.0;
            oneSatAmb.Amb = X(i+trans_para->const_parm);
            epochResultSatlitData[i].ionL1 = 0.0;
            epochResultSatlitData[i].Amb1 = 0.0;
            epochResultSatlitData[i].Amb2 = 0.0;
            epochResultSatlitData[i].Amb = X(i+trans_para->const_parm);
            oneSatAmb.UTCTime.epochNum = epochNum;
//            if(!m_IS_MAX_OBS) allAmbiguity.append(oneSatAmb);
        }
    }
    if(trans_para->for_and_backward_filter&&!trans_para->backward_filter)
        allPPPSatlitData.append(epochResultSatlitData);
    if(P)   
    {
        writeRecivePos2Txt(epochRecivePos, P);
        writeAmbiguity_information(epochResultSatlitData,opt_corr->Combination_mode,X,P);
    }
    else
    {
        MatrixXd tempP = MatrixXd::Identity(32,32) * 1e10;
        writeRecivePos2Txt(epochRecivePos, &tempP);
    }
    writeambupd(epochResultSatlitData);
    writeAzEL2Txt(epochResultSatlitData);
    writePPP2Txt(epochResultSatlitData,opt_corr->Combination_mode);
    write_dops(epochRecivePos);
    writeClockZTDW2Txt(epochRecClock,opt_corr->m_sys_str);
    int system_flag[4]={0};
    if(IsaddGPS) system_flag[0]=1;
    if(IsaddBDS) system_flag[1]=1;
    if(IsaddGLOSS) system_flag[2]=1;
    if(IsaddGalieo) system_flag[3]=1;

    QVector<SatlitData>::iterator iter;
    for (iter=allBadSatlitData.end()-1;iter>=allBadSatlitData.begin();iter--)
    {
        SatlitData tempBadSat = *iter;
        if(epochNum != tempBadSat.UTCTime.epochNum)
            break;
        writeBadSatliteData(tempBadSat);
        if(iter == allBadSatlitData.begin())
            break;
    }
    allBadSatlitData.clear();

}

void stand_pos::system_sate_cual(QVector< SatlitData > ephon_satellite,int *sys_N)
{
    for(int i=0;i<ephon_satellite.length();i++)
    {
        SatlitData oneSatlite = ephon_satellite.at(i);
        if (oneSatlite.SatType == 'G')
        {
            sys_N[0]++;
        }
        else if (oneSatlite.SatType == 'R')
        {
            sys_N[2]++;
        }
        else if (oneSatlite.SatType == 'C')
        {
            sys_N[1]++;
        }
        else if (oneSatlite.SatType == 'E')
        {
            sys_N[3]++;
        }
    }

}
void stand_pos::ecef2Pos(const double *r, double *pos)
{
    double e2=FE_WGS84*(2.0-FE_WGS84),z,zk,v=RE_WGS84,sinp;
    double r2=pow(r[0],2)+pow(r[1],2);

    for (z=r[2],zk=0.0;fabs(z-zk)>=1E-4;) {
        zk=z;
        sinp=z/sqrt(r2+z*z);
        v=RE_WGS84/sqrt(1.0-e2*sinp*sinp);
        z=r[2]+v*e2*sinp;
    }
    pos[0]=r2>1E-12?atan(z/sqrt(r2)):(r[2]>0.0?M_PI/2.0:-M_PI/2.0);
    pos[1]=r2>1E-12?atan2(r[1],r[0]):0.0;
    pos[2]=sqrt(r2+z*z)-v;
}

void stand_pos::rad2DMS(double rad, int &deg, double &min)
{
    double degT=rad*R2D;
    deg=(int)degT>0?(int)degT:-(int)degT;
    min=((degT-deg)*60)>0?((degT-deg)*60):(-(degT-deg)*60);
}
