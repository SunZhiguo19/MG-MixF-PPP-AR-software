#include "quality_control.h"

quality_control::quality_control()
{

}
//Screening satellites that do not have missing data and detect cycle slips, high quality (height angles, ranging codes, etc.)
void quality_control::Avail_satellite_sele(QVector< SatlitData > &prevEpochSatlitData,QVector< SatlitData > &Current_sat,double eleAngle,QVector< SatlitData > &allBadSatlitData_Q,int Combination_mode,int *cyclyslip_satn)
{
    int preEpochLen = prevEpochSatlitData.length();
    int Avai_satN = Current_sat.length();
    if(Avai_satN == 0) return ;
    int cyclyslip_n = 0;
    //Cycle slip detection
    QVector< int > CycleFlag;//Record the position of the weekly jump
    CycleFlag.resize(Avai_satN);
    for (int i = 0;i < Avai_satN;i++) CycleFlag[i] = 0;
    for (int i = 0;i < Avai_satN;i++)
    {
        SatlitData epochData = Current_sat.at(i);
        //Quality control (height angle, pseudorange difference)
        if (epochData.EA[0] < eleAngle)
        {
            QString errorline;
            ErrorMsg(errorline);
            epochData.badMsg.append("elevation angle is " + QString::number(epochData.EA[0],'f',2)
                    + " less " + QString::number(eleAngle,'f',2) +errorline);
            allBadSatlitData_Q.append(epochData);
            CycleFlag[i] = -1;
        }

        /*Signal strength quality control to be done*/
        corr_opt *opt_corr = AGNSS_APPData::sGetAPPDataInstance()->mGetopt_corrData();
        //Cycle slip detection
        if(opt_corr->cycle_slip_detection)
        {
            for (int j = 0;j < preEpochLen;j++)//(jump == 0) not happen clock jump  && (jump == 0)
            {
                SatlitData preEpochData = prevEpochSatlitData.at(j);
                if (epochData.PRN == preEpochData.PRN&&epochData.SatType == preEpochData.SatType)//&&preEpochDatanum==epochDatanum
                {//Need to judge the system
                    switch (Combination_mode) {
                    case Comb_IF_mode:
                        if(!CycleSlip_IF(epochData,preEpochData,allBadSatlitData_Q))
                        {
                            CycleFlag[i] = -1;
                            preEpochData.cycly_slipF=1;
                        }
                        break;
                    case Comb_UD_mode:
                        if(!CycleSlip_UD(epochData,preEpochData,allBadSatlitData_Q))
                        {
                            CycleFlag[i] = -1;
                            preEpochData.cycly_slipF=1;
                        }
                        break;
                    default:
                        break;
                    }
                }
            }
        }

    }

    //Remove low quality and weekly hop satellites
    QVector< SatlitData > tempEpochSatlitData;
//    for (int i = 0;i < prevEpochSatlitData.length();i++)
//    {
//        SatlitData preEpochData = prevEpochSatlitData.at(i);
//        if(preEpochData.cycly_slipF==1)
//            continue;
//        else
//            preVecEpochData.append(preEpochData);
//    }

    for (int i = 0;i < Avai_satN;i++)
    {

        SatlitData Cyc_Current = Current_sat.at(i);
        Cyc_Current.SatWight_IF[0] = Cyc_Current.SatWight_IF[0]/((SQR(Cyc_Current.Frq[Cyc_Current.IF_Frq[0]])+SQR(Cyc_Current.Frq[Cyc_Current.IF_Frq[1]]))
                /(SQR(Cyc_Current.Frq[Cyc_Current.IF_Frq[0]])-SQR(Cyc_Current.Frq[Cyc_Current.IF_Frq[1]])));
        Cyc_Current.SatWight_IF[1] = Cyc_Current.SatWight_IF[0] ;
        if (CycleFlag.at(i) != -1)
        {
            tempEpochSatlitData.append(Cyc_Current);
        }
        else
        {
            Cyc_Current.cycly_slipF=1;
            Cyc_Current.SatWight_IF[0] = 0.5*Cyc_Current.SatWight_IF[0];
            Cyc_Current.SatWight_IF[1] = 0.5*Cyc_Current.SatWight_IF[1];
            cyclyslip_n++;
            tempEpochSatlitData.append(Cyc_Current);
        }
    }
    *cyclyslip_satn = cyclyslip_n;
    Current_sat = tempEpochSatlitData;
    system_num(Current_sat);
    tempEpochSatlitData.clear();
    CycleFlag.clear();
//    prevEpochSatlitData = preVecEpochData;

}
//Detect cycle hops: return pLP is a three-dimensional array, the first is the W-M combination (N2-N1 < 3.5) The second number ionospheric residual (<0.3) The third is (lamt2*N2-lamt1*N1 < 3.5)
bool quality_control::CycleSlip_IF(const SatlitData &Current_sat,const SatlitData &preSatlitData,QVector< SatlitData > &allBadSatlitData_Q)
{//
    double curPhaseL[2],curCodeP[2];
    double prePhaseL[2],preCodeP[2],lambda[2];
    double NLxy, IocLxy,IocLPxy;
    double preNLxy, preIocLxy,preIocLPxy;
    double diff_NLxy, diff_IocLxy;
    int fre_num0=Current_sat.IF_Frq[0],fre_num1=Current_sat.IF_Frq[1];
    curPhaseL[0] = Current_sat.PhaseL[fre_num0];
    prePhaseL[0] = preSatlitData.PhaseL[fre_num0];
    lambda[0] = Current_sat.Lambda[fre_num0];
    curCodeP[0] = Current_sat.CodeP[fre_num0];
    preCodeP[0] = preSatlitData.CodeP[fre_num0];
    curPhaseL[1] = Current_sat.PhaseL[fre_num1];
    prePhaseL[1] = preSatlitData.PhaseL[fre_num1];
    lambda[1] = Current_sat.Lambda[fre_num1];
    curCodeP[1] = Current_sat.CodeP[fre_num1];
    preCodeP[1] = preSatlitData.CodeP[fre_num1];
    double sample = epht_T.timediff(Current_sat.Otime,preSatlitData.Otime);
    double csThresGF = 0,csThresMW = 0;
    if(sample>0.0)
    {
        if(sample<=1.0)
        {
            csThresGF = 0;
            csThresMW = 0;
        }
        else if(sample<=20.0)
        {
            csThresGF = (0.10)/(20.0-0.0)*sample;
            csThresMW = (2.5)/(20.0-0.0)*sample;
        }
        else if(sample<=60.0)
        {
            csThresGF = 0.10;
            csThresMW = 2.5;
        }
        else if(sample<=100.0)
        {
            csThresGF = 0.20;
            csThresMW = 5;
        }
        else
        {
            csThresGF = 0.30;
            csThresMW = 5;
        }
    }


    for(int i=1;i<2;i++)
    {
        NLxy=curPhaseL[0] - curPhaseL[i] + (lambda[0]-lambda[i])/((lambda[0]+lambda[i])*lambda[0])*curCodeP[0]+(lambda[0]-lambda[i])/((lambda[0]+lambda[i])*lambda[i])*curCodeP[i];
        IocLxy = lambda[0]*curPhaseL[0] - lambda[i]*curPhaseL[i];
        IocLPxy = IocLxy+curCodeP[0] -curCodeP[i];
        preNLxy=prePhaseL[0] - prePhaseL[i] + (lambda[0]-lambda[i])/((lambda[0]+lambda[i])*lambda[0])*preCodeP[0]+(lambda[0]-lambda[i])/((lambda[0]+lambda[i])*lambda[i])*preCodeP[i];
        preIocLxy = lambda[0]*prePhaseL[0] - lambda[i]*prePhaseL[i];
        preIocLPxy = IocLxy+preCodeP[0] -preCodeP[i];
    }
    diff_NLxy = NLxy - preNLxy;
    diff_IocLxy = IocLxy - preIocLxy;
    if(qAbs(diff_NLxy)>(2.5+csThresMW)||qAbs(diff_IocLxy)>(M_IR+csThresGF)||qAbs(Current_sat.AntWindup - preSatlitData.AntWindup) > 0.3)
    {
        SatlitData Current_sat_1 = Current_sat;
        QString errorline;
        ErrorMsg(errorline);
        Current_sat_1.badMsg.append("Cycle slip detection, Ionospheric residual: "+ QString::number(diff_IocLxy,'f',4)+errorline);
        allBadSatlitData_Q.append(Current_sat_1);
        return false;
    }
    return true;
}
bool quality_control::CycleSlip_UD(const SatlitData &Current_sat,const SatlitData &preSatlitData,QVector< SatlitData > &allBadSatlitData_Q)
{//
    double test_stati[2]={0};
    int ObsLenfre = Current_sat.ObsLenfre;
    double curPhaseL[ObsLenfre],curCodeP[ObsLenfre];
    double prePhaseL[ObsLenfre],preCodeP[ObsLenfre],lambda[ObsLenfre];
    if(ObsLenfre==1&&preSatlitData.ObsLenfre==1)
    {
        for(int i=0;i<5;i++)
        {
            if(Current_sat.ObsFreflag[i]==1)
                if(preSatlitData.ObsFreflag[i]==1)
                {
                    test_stati[0] = Current_sat.CodeP[i] - Current_sat.PhaseL[i]*Current_sat.Lambda[i];
                    test_stati[1] = preSatlitData.CodeP[i] - preSatlitData.PhaseL[i]*preSatlitData.Lambda[i];
                }
        }
        if(fabs(test_stati[0]-test_stati[1])>2) return false;
    }
    else if(ObsLenfre==1)
    {
        return false;
    }
    else
    {
        if(ObsLenfre==preSatlitData.ObsLenfre)
        {
            int dat_num=0;
            for(int i=0;i<5;i++)
            {
                if(Current_sat.ObsFreflag[i]!=preSatlitData.ObsFreflag[i])
                    return false;
                if(Current_sat.ObsFreflag[i]==1)
                {
                    curPhaseL[dat_num] = Current_sat.PhaseL[i];
                    prePhaseL[dat_num] = preSatlitData.PhaseL[i];
                    lambda[dat_num] = Current_sat.Lambda[i];
                    curCodeP[dat_num] = Current_sat.CodeP[i];
                    preCodeP[dat_num] = preSatlitData.CodeP[i];
                    dat_num++;
                }
            }
        }
    }
    double NLxy[ObsLenfre-1], IocLxy[ObsLenfre-1],IocLPxy[ObsLenfre-1];
    double preNLxy[ObsLenfre-1], preIocLxy[ObsLenfre-1],preIocLPxy[ObsLenfre-1];
    double diff_NLxy[ObsLenfre-1], diff_IocLxy[ObsLenfre-1];
    for(int i=1;i<ObsLenfre;i++)
    {
        NLxy[i-1]=curPhaseL[0] - curPhaseL[i] + (lambda[0]-lambda[i])/((lambda[0]+lambda[i])*lambda[0])*curCodeP[0]+(lambda[0]-lambda[i])/((lambda[0]+lambda[i])*lambda[i])*curCodeP[i];
        IocLxy[i-1] = lambda[0]*curPhaseL[0] - lambda[i]*curPhaseL[i];
        IocLPxy[i-1] = IocLxy[i-1]+curCodeP[0] -curCodeP[i];
        preNLxy[i-1]=prePhaseL[0] - prePhaseL[i] + (lambda[0]-lambda[i])/((lambda[0]+lambda[i])*lambda[0])*preCodeP[0]+(lambda[0]-lambda[i])/((lambda[0]+lambda[i])*lambda[i])*preCodeP[i];
        preIocLxy[i-1] = lambda[0]*prePhaseL[0] - lambda[i]*prePhaseL[i];
        preIocLPxy[i-1] = IocLxy[i-1]+preCodeP[0] -preCodeP[i];
    }
    double sample = epht_T.timediff(Current_sat.Otime,preSatlitData.Otime);
    double csThresGF = 0,csThresMW = 0;
    if(sample>0.0)
    {
        if(sample<=1.0)
        {
            csThresGF = 0;
            csThresMW = 0;
        }
        else if(sample<=20.0)
        {
            csThresGF = (0.10)/(20.0-0.0)*sample;
            csThresMW = (2.5)/(20.0-0.0)*sample;
        }
        else if(sample<=60.0)
        {
            csThresGF = 0.10;
            csThresMW = 2.5;
        }
        else if(sample<=100.0)
        {
            csThresGF = 0.20;
            csThresMW = 5;
        }
        else
        {
            csThresGF = 0.30;
            csThresMW = 5;
        }
    }
    for(int i=0;i<ObsLenfre-1;i++)
    {
        diff_NLxy[i] = NLxy[i] - preNLxy[i];
        diff_IocLxy[i] = IocLxy[i] - preIocLxy[i];
        if(qAbs(diff_NLxy[i])>(2.5+csThresMW)||qAbs(diff_IocLxy[i])>(M_IR+csThresGF)||qAbs(Current_sat.AntWindup - preSatlitData.AntWindup) > 0.3)
        {
            SatlitData Current_sat_1 = Current_sat;
            QString errorline;
            ErrorMsg(errorline);
            Current_sat_1.badMsg.append("Cycle slip detection, Ionospheric residual: "+ QString::number(diff_IocLxy[i],'f',4)+errorline);
            allBadSatlitData_Q.append(Current_sat_1);
            return false;
        }

    }

    return true;
}
void quality_control::Availa_SysNum(char Sys,int *SYSNUM,int *Sys_num,int PRN)
{
    if (Sys == 'G'&&SYSNUM[0]==0)
    {
        SYSNUM[0] = 1;
        *Sys_num = *Sys_num+1;
    }
    else if (Sys == 'C'&&SYSNUM[1]==0&&PRN<=18)
    {
        SYSNUM[1] = 1;
        *Sys_num = *Sys_num+1;
    }
    else if (Sys == 'C'&&SYSNUM[2]==0&&PRN>18)
    {
        SYSNUM[2] = 1;
        *Sys_num = *Sys_num+1;
    }
    else if (Sys == 'R'&&SYSNUM[3]==0)
    {
        SYSNUM[3] = 1;
        *Sys_num = *Sys_num+1;
    }
    else if (Sys == 'E'&&SYSNUM[4]==0)
    {
        SYSNUM[4] = 1;
        *Sys_num = *Sys_num+1;
    }
}
void quality_control::Availa_SysFlag(QString Sys_str,VectorXd &SYSflg,int *SysNum)
{
    *SysNum  = Sat_Sys_num;
    int Bds_2 = 0;
    for(int i=0;i<Sys_str.length();i++)
    {
        if(Sys_str[i] == 'G')
        {
            SYSflg[Bds_2] = Sat_Sys_flag[0];
            Bds_2++;
        }
        else if(Sys_str[i] == 'C')
        {
            SYSflg[Bds_2] = Sat_Sys_flag[1];
            Bds_2++;
            SYSflg[Bds_2] = Sat_Sys_flag[2];
            Bds_2++;
        }
        else if(Sys_str[i] == 'R')
        {
            SYSflg[Bds_2] = Sat_Sys_flag[3];
            Bds_2++;
        }
        else if(Sys_str[i] == 'E')
        {
            SYSflg[Bds_2] = Sat_Sys_flag[4];
            Bds_2++;
        }
    }
}

void quality_control::system_num(QVector< SatlitData > &Current_sat)
{
    int SystemN = 0;int SYSNUM[5]={0};
    for (int i = 0;i < Current_sat.length();i++)
    {

        SatlitData Cyc_Current = Current_sat.at(i);
        Availa_SysNum(Cyc_Current.SatType,SYSNUM,&SystemN,Cyc_Current.PRN);
    }
    Sat_Sys_num = SystemN;
    for (int i = 0;i < 5;i++)   Sat_Sys_flag[i] = SYSNUM[i];
}
int quality_control::fixed_WL_ambiguity_N(QVector< SatlitData > &Current_sat)
{
    int fixed_N = 0;
    for (int i = 0;i < Current_sat.length();i++)
    {

        SatlitData Cyc_Current = Current_sat.at(i);
        if(Cyc_Current.WL_fixed)
            fixed_N++;
        if(Cyc_Current.EWL23_fixed)
            fixed_N++;
        if(Cyc_Current.EWL24_fixed)
            fixed_N++;
        if(Cyc_Current.EWL25_fixed)
            fixed_N++;
    }
    return fixed_N;
}
void quality_control::GNSS_IFB_and_osbfre(QVector< SatlitData > epochSatlitData,para_trans *trans_para)
{
    bool IFB_flag3 = false,IFB_flag4 = false,IFB_flag5 = false;
    corr_opt *opt_corr = AGNSS_APPData::sGetAPPDataInstance()->mGetopt_corrData();
    trans_para->curr_GIFB_falg = false;
    trans_para->curr_C2IFB_falg = false;
    trans_para->curr_C3IFB3_falg = false;
    trans_para->curr_C3IFB4_falg = false;
    trans_para->curr_C3IFB5_falg = false;
    trans_para->curr_EIFB3_falg = false;
    trans_para->curr_EIFB4_falg = false;
    trans_para->curr_EIFB5_falg = false;

    for(int i=0;i<epochSatlitData.length();i++)
    {
        IFB_flag3 = false;IFB_flag4 = false;IFB_flag5 = false;
        SatlitData tempSatlitData=epochSatlitData.at(i);
        for(int ii=0;ii<5;ii++)
        {
            if(tempSatlitData.ObsFreflag[ii]==0)  continue;
            if((opt_corr->treat_method==POS_PP_PPP_AR_2||opt_corr->treat_method==POS_PP_PPP_AR_1||opt_corr->treat_method==POS_RT_PPP_AR
                ||opt_corr->treat_method==POS_UDUC_PPP_AR||opt_corr->treat_method==POS_Real_PPP_AR)&&(tempSatlitData.CODE_BIA[ii]==0||tempSatlitData.PHASE_BIA[ii]==0))
                continue;
            if(ii==2)
                IFB_flag3 = true;
            if(ii==3)
                IFB_flag4 = true;
            if(ii==4)
                IFB_flag5 = true;
        }
        switch (tempSatlitData.SatType) {
        case 'G':
            if(IFB_flag3)
                trans_para->curr_GIFB_falg=IFB_flag3;
            break;
        case 'C':
            if(tempSatlitData.PRN<=18)
            {
                if(IFB_flag3)
                    trans_para->curr_C2IFB_falg=IFB_flag3;
            }
            else
            {
                if(IFB_flag3)
                    trans_para->curr_C3IFB3_falg=IFB_flag3;
                if(IFB_flag4)
                    trans_para->curr_C3IFB4_falg=IFB_flag4;
                if(IFB_flag5)
                    trans_para->curr_C3IFB5_falg=IFB_flag5;
            }
            break;
        case 'R':

            break;
        case 'E':
            if(IFB_flag3)
                trans_para->curr_EIFB3_falg=IFB_flag3;
            if(IFB_flag4)
                trans_para->curr_EIFB4_falg=IFB_flag4;
            if(IFB_flag5)
                trans_para->curr_EIFB5_falg=IFB_flag5;
            break;
        default:
            break;
        }
    }
    Calculate_curr_IFB_N(trans_para);

}
void quality_control::Calculate_curr_IFB_N(para_trans *trans_para)
{
    int IFB_N=0;
    if(trans_para->curr_EIFB3_falg)
        IFB_N++;
    if(trans_para->curr_EIFB4_falg)
        IFB_N++;
    if(trans_para->curr_EIFB5_falg)
        IFB_N++;
    if(trans_para->curr_C3IFB3_falg)
        IFB_N++;
    if(trans_para->curr_C3IFB4_falg)
        IFB_N++;
    if(trans_para->curr_C3IFB5_falg)
        IFB_N++;
    if(trans_para->curr_C2IFB_falg)
        IFB_N++;
    if(trans_para->curr_GIFB_falg)
        IFB_N++;

    trans_para->curr_IFB_n=IFB_N;
}
