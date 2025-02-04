#include "uduc_ambiguity_resolution.h"

uduc_ambiguity_resolution::uduc_ambiguity_resolution()
{

}
/*****MW Combined Fixed ambiguity**/
bool uduc_ambiguity_resolution::UDUC_WidelaneAR(SatlitData &oneSatlitData,int fre_num0,int fre_num1,int HMW_flag)
{//

    double F1 = oneSatlitData.Frq[fre_num0],F2 = oneSatlitData.Frq[fre_num1];//Get the frequency of this satellite
    double Lamta1 = oneSatlitData.Lambda[fre_num0],Lamta2 = oneSatlitData.Lambda[fre_num1];
    double lamdaMW = (M_C/(F1 - F2));
    double L1s = 0.0 ,L2s = 0.0 ;
    double C1s = 0.0 ,C2s = 0.0 ;
    L1s = oneSatlitData.PhaseL[fre_num0] + (oneSatlitData.PHASE_BIA[fre_num0])/Lamta1  + oneSatlitData.Sat_LxPCO[fre_num0];
    L2s = oneSatlitData.PhaseL[fre_num1] + (oneSatlitData.PHASE_BIA[fre_num1])/Lamta2  + oneSatlitData.Sat_LxPCO[fre_num1];//
    C1s = oneSatlitData.CodeP[fre_num0] + oneSatlitData.CODE_BIA[fre_num0] +Lamta1*(  oneSatlitData.Sat_LxPCO[fre_num0]);//
    C2s = oneSatlitData.CodeP[fre_num1] + oneSatlitData.CODE_BIA[fre_num1] +Lamta2*(  oneSatlitData.Sat_LxPCO[fre_num1]);

    double IF_wl = L1s - L2s - (F1*C1s + F2*C2s)/((F1+F2)*lamdaMW);
    if((F1 - F2)<0) IF_wl = -IF_wl;
    if(HMW_flag==0)
    {
        oneSatlitData.MW_Wl_Int = IF_wl;
        oneSatlitData.Lambda_Wl = lamdaMW;
    }
    else if(HMW_flag==1)
    {
        oneSatlitData.MW_EWl_Int23 = IF_wl;
        oneSatlitData.Lambda_EWl23 = lamdaMW;
    }
    else if(HMW_flag==2)
    {
        oneSatlitData.MW_EWl_Int24 = IF_wl;
        oneSatlitData.Lambda_EWl24 = lamdaMW;
    }
    else if(HMW_flag==3)
    {
        oneSatlitData.MW_EWl_Int25 = IF_wl;
        oneSatlitData.Lambda_EWl25 = lamdaMW;
    }
    return true;
}
//Smoothing wide lane ambiguity and fixing single difference wide lane ambiguity between satellites szg
void uduc_ambiguity_resolution::Wide_lane_uduc_smoothing(QVector< SatlitData > &prevEpochSatlitData,QVector< SatlitData > &epochSatlitData, para_trans *trans_para)
{
    int preEpochLen = prevEpochSatlitData.length();
    corr_opt *opt_corr = AGNSS_APPData::sGetAPPDataInstance()->mGetopt_corrData();
    if(preEpochLen==0||trans_para->cyclyslip_sat_n>=8)
    {
        trans_para->ambiguity_fix =true;
    }
    else
    {
        trans_para->ambiguity_fix=false;
    }
    int epochLen = epochSatlitData.length();
    if(epochLen == 0) return ;
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
    for(int i = 0;i < preEpochLen;i++)
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
    for(int i = 0;i < epochLen;i++)
    {
        SatlitData currentEpoSat = epochSatlitData.at(i);
        double SmoothwlSatlit = 0.0,SmoothwlStd = 0.0,SDWlar = 0.0,Smoothewl23Satlit=0.0,Smoothewl23Std = 0.0,Smoothewl24Satlit=0.0,Smoothewl24Std = 0.0,Smoothewl25Satlit=0.0,Smoothewl25Std = 0.0;
        int SmoWlNum = 0,SmoEWl23Num = 0,SmoEWl24Num = 0,SmoEWl25Num = 0,Pre_prn = -1;/*Number of smooth epochs*/
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
        if(Pre_prn!=-1&&currentEpoSat.cycly_slipF!=1&&currentEpoSat.Domain_of_doubt!=-1)//&&currentEpoSat.Domain_of_doubt!=-1
        {
            SatlitData preEpoSat = prevEpochSatlitData.at(Pre_prn);
            if(preEpoSat.Domain_of_doubt!=-1)
            {
                if(opt_corr->fre_model==double_fre)
                {
                    SmoothwlSatlit = preEpoSat.satMW_wlAmbi[0];
                    SmoWlNum     = preEpoSat.satMW_wlSmoN + 1;//smooth epo
                    SmoothwlStd    = preEpoSat.satMW_wlAmbi[1] * (SmoWlNum -1);
                }
                else if(opt_corr->fre_model==triple_fre)
                {
                    SmoothwlSatlit = preEpoSat.satMW_wlAmbi[0];
                    SmoWlNum     = preEpoSat.satMW_wlSmoN + 1;//smooth epo
                    SmoothwlStd    = preEpoSat.satMW_wlAmbi[1] * (SmoWlNum -1);
                    Smoothewl23Satlit = preEpoSat.satMW_ewl23Ambi[0];
                    SmoEWl23Num     = preEpoSat.satMW_ewl23SmoN + 1;//smooth epo
                    Smoothewl23Std    = preEpoSat.satMW_ewl23Ambi[1] * (SmoEWl23Num -1);
                }
                else if(opt_corr->fre_model==four_fre)
                {
                    SmoothwlSatlit = preEpoSat.satMW_wlAmbi[0];
                    SmoWlNum     = preEpoSat.satMW_wlSmoN + 1;//smooth epo
                    SmoothwlStd    = preEpoSat.satMW_wlAmbi[1] * (SmoWlNum -1);
                    Smoothewl23Satlit = preEpoSat.satMW_ewl23Ambi[0];
                    SmoEWl23Num     = preEpoSat.satMW_ewl23SmoN + 1;//smooth epo
                    Smoothewl23Std    = preEpoSat.satMW_ewl23Ambi[1] * (SmoEWl23Num -1);
                    Smoothewl24Satlit = preEpoSat.satMW_ewl24Ambi[0];
                    SmoEWl24Num     = preEpoSat.satMW_ewl24SmoN + 1;//smooth epo
                    Smoothewl24Std    = preEpoSat.satMW_ewl24Ambi[1] * (SmoEWl24Num -1);
                }
                else if(opt_corr->fre_model==five_fre)
                {
                    SmoothwlSatlit = preEpoSat.satMW_wlAmbi[0];
                    SmoWlNum     = preEpoSat.satMW_wlSmoN + 1;//smooth epo
                    SmoothwlStd    = preEpoSat.satMW_wlAmbi[1] * (SmoWlNum -1);
                    Smoothewl23Satlit = preEpoSat.satMW_ewl23Ambi[0];
                    SmoEWl23Num     = preEpoSat.satMW_ewl23SmoN + 1;//smooth epo
                    Smoothewl23Std    = preEpoSat.satMW_ewl23Ambi[1] * (SmoEWl23Num -1);
                    Smoothewl24Satlit = preEpoSat.satMW_ewl24Ambi[0];
                    SmoEWl24Num     = preEpoSat.satMW_ewl24SmoN + 1;//smooth epo
                    Smoothewl24Std    = preEpoSat.satMW_ewl24Ambi[1] * (SmoEWl24Num -1);
                    Smoothewl25Satlit = preEpoSat.satMW_ewl25Ambi[0];
                    SmoEWl25Num     = preEpoSat.satMW_ewl25SmoN + 1;//smooth epo
                    Smoothewl25Std    = preEpoSat.satMW_ewl25Ambi[1] * (SmoEWl25Num -1);
                }

            }
            else
            {
                SmoothwlSatlit = 0;
                SmoothwlStd    = 0;
                Smoothewl23Satlit =0;
                Smoothewl23Std=0;
                Smoothewl24Satlit =0;
                Smoothewl24Std=0;
                Smoothewl25Satlit =0;
                Smoothewl25Std=0;
                SmoWlNum     = 1;//smooth epo
                SmoEWl23Num     = 1;//smooth epo
                SmoEWl24Num     = 1;//smooth epo
                SmoEWl25Num     = 1;//smooth epo
            }

        }
        else
        {
            SmoothwlSatlit = 0;
            SmoothwlStd    = 0;
            Smoothewl23Satlit =0;
            Smoothewl23Std=0;
            Smoothewl24Satlit =0;
            Smoothewl24Std=0;
            Smoothewl25Satlit =0;
            Smoothewl25Std=0;
            SmoWlNum     = 1;//smooth epo
            SmoEWl23Num     = 1;//smooth epo
            SmoEWl24Num     = 1;//smooth epo
            SmoEWl25Num     = 1;//smooth epo
        }
        currentEpoSat.continue_eph = SmoWlNum;
        /*WL  Reducing power*/
        if(opt_corr->fre_model==double_fre)
        {
            SDWlar = currentEpoSat.MW_Wl_Int;/*MW wide roadway ambiguity*/
            currentEpoSat.satMW_wlAmbi[0] = SmoothwlSatlit*(SmoWlNum -1)/SmoWlNum + SDWlar/SmoWlNum;//平滑后宽巷模糊度
            currentEpoSat.satMW_wlSmoN = SmoWlNum;
        }
        else if(opt_corr->fre_model==triple_fre)
        {
            SDWlar = currentEpoSat.MW_Wl_Int;/*MW wide roadway ambiguity*/
            currentEpoSat.satMW_wlAmbi[0] = SmoothwlSatlit*(SmoWlNum -1)/SmoWlNum + SDWlar/SmoWlNum;//平滑后宽巷模糊度
            currentEpoSat.satMW_wlSmoN = SmoWlNum;
            SDWlar = currentEpoSat.MW_EWl_Int23;/*MW wide roadway ambiguity*/
            currentEpoSat.satMW_ewl23Ambi[0] = Smoothewl23Satlit*(SmoEWl23Num -1)/SmoEWl23Num + SDWlar/SmoEWl23Num;//平滑后宽巷模糊度
            currentEpoSat.satMW_ewl23SmoN = SmoEWl23Num;
        }
        else if(opt_corr->fre_model==four_fre)
        {
            SDWlar = currentEpoSat.MW_Wl_Int;/*MW wide roadway ambiguity*/
            currentEpoSat.satMW_wlAmbi[0] = SmoothwlSatlit*(SmoWlNum -1)/SmoWlNum + SDWlar/SmoWlNum;//平滑后宽巷模糊度
            currentEpoSat.satMW_wlSmoN = SmoWlNum;
            SDWlar = currentEpoSat.MW_EWl_Int23;/*MW wide roadway ambiguity*/
            currentEpoSat.satMW_ewl23Ambi[0] = Smoothewl23Satlit*(SmoEWl23Num -1)/SmoEWl23Num + SDWlar/SmoEWl23Num;//平滑后宽巷模糊度
            currentEpoSat.satMW_ewl23SmoN = SmoEWl23Num;
            SDWlar = currentEpoSat.MW_EWl_Int24;/*MW wide roadway ambiguity*/
            currentEpoSat.satMW_ewl24Ambi[0] = Smoothewl24Satlit*(SmoEWl24Num -1)/SmoEWl24Num + SDWlar/SmoEWl24Num;//平滑后宽巷模糊度
            currentEpoSat.satMW_ewl24SmoN = SmoEWl24Num;
        }
        else if(opt_corr->fre_model==five_fre)
        {
            SDWlar = currentEpoSat.MW_Wl_Int;/*MW wide roadway ambiguity*/
            currentEpoSat.satMW_wlAmbi[0] = SmoothwlSatlit*(SmoWlNum -1)/SmoWlNum + SDWlar/SmoWlNum;//平滑后宽巷模糊度
            currentEpoSat.satMW_wlSmoN = SmoWlNum;
            SDWlar = currentEpoSat.MW_EWl_Int23;/*MW wide roadway ambiguity*/
            currentEpoSat.satMW_ewl23Ambi[0] = Smoothewl23Satlit*(SmoEWl23Num -1)/SmoEWl23Num + SDWlar/SmoEWl23Num;//平滑后宽巷模糊度
            currentEpoSat.satMW_ewl23SmoN = SmoEWl23Num;
            SDWlar = currentEpoSat.MW_EWl_Int24;/*MW wide roadway ambiguity*/
            currentEpoSat.satMW_ewl24Ambi[0] = Smoothewl24Satlit*(SmoEWl24Num -1)/SmoEWl24Num + SDWlar/SmoEWl24Num;//平滑后宽巷模糊度
            currentEpoSat.satMW_ewl24SmoN = SmoEWl24Num;
            SDWlar = currentEpoSat.MW_EWl_Int25;/*MW wide roadway ambiguity*/
            currentEpoSat.satMW_ewl25Ambi[0] = Smoothewl25Satlit*(SmoEWl25Num -1)/SmoEWl25Num + SDWlar/SmoEWl25Num;//平滑后宽巷模糊度
            currentEpoSat.satMW_ewl25SmoN = SmoEWl25Num;
        }



        if(SmoWlNum<3)
            currentEpoSat.Par_sub = true;

        tempEpochSatlitData.append(currentEpoSat);
    }
    epochSatlitData.clear();
    epochSatlitData = tempEpochSatlitData;
    return ;
}
//Smoothing wide lane ambiguity and fixing single difference wide lane ambiguity between satellites szg
void uduc_ambiguity_resolution::get_uduc_ref_Sat(QVector< SatlitData > &epochSatlitData,int *MAX_EAprn)
{
    int epochLen = epochSatlitData.length();
    if(epochLen == 0) return ;
    int fixed_WL_number=0;
    para_trans *trans_para = AGNSS_APPData::sGetAPPDataInstance()->mGettrans_paraData();

    QVector< SatlitData > tempEpochSatlitData;
    tempEpochSatlitData = epochSatlitData;
    epochSatlitData.clear();
    //An epoch cycle begins
    /*Calculate the ambiguity and variance of single difference wide roadway between satellites*/
    for (int i = 0;i < epochLen;i++)
    {
        SatlitData Satlit_SD = tempEpochSatlitData.at(i);
        int SYS_PRN=0;
        switch (Satlit_SD.SatType) {
        case 'G':
            SYS_PRN = MAX_EAprn[0] ;
            break;
        case 'C':
            if(Satlit_SD.PRN<=18)
            {
                SYS_PRN = MAX_EAprn[1] ;
            }
            else
            {
                SYS_PRN = MAX_EAprn[2] ;
            }
            break;
        case 'R':
            SYS_PRN = MAX_EAprn[3] ;
            break;
        case 'E':
            SYS_PRN = MAX_EAprn[4] ;
            break;
        default:
            break;
        }

        if(Satlit_SD.PRN != SYS_PRN)//&&Satlit_SD.Domain_of_doubt!=-1
        {
            Satlit_SD.maxEA_PRN = SYS_PRN;
        }
        else
        {
            Satlit_SD.maxEA_PRN  = SYS_PRN;
        }
        epochSatlitData.append(Satlit_SD);
    }
    trans_para->fixed_WL_number=fixed_WL_number;
    return ;
}

//Smoothing wide lane ambiguity and fixing single difference wide lane ambiguity between satellites szg
int uduc_ambiguity_resolution::get_uduc_SD_SatWl(QVector< SatlitData > &epochSatlitData,int *MAX_EAprn,int *Par_sat_epo)
{
    int epochLen = epochSatlitData.length();
    if(epochLen == 0) return 0;
    corr_opt *opt_corr = AGNSS_APPData::sGetAPPDataInstance()->mGetopt_corrData();
    double maxEaWL[5]={0},maxEaEWL23[5]={0},maxEaEWL24[5]={0},maxEaEWL25[5]={0};
    int smoothwl_len[5] = {0},smoothewl23_len[5] = {0},smoothewl24_len[5] = {0},smoothewl25_len[5] = {0};
    int fixed_WL_number=0;
    para_trans *trans_para = AGNSS_APPData::sGetAPPDataInstance()->mGettrans_paraData();

    QVector< SatlitData > tempEpochSatlitData;
    for(int i = 0;i < epochLen;i++)
    {
        SatlitData currentEpoSat = epochSatlitData.at(i);
        switch (currentEpoSat.SatType) {
        case 'G':
            if(currentEpoSat.PRN == MAX_EAprn[0]) /*GPS system Max EA*/
            {
                maxEaWL[0] = currentEpoSat.satMW_wlAmbi[0];
                smoothwl_len[0] = currentEpoSat.satMW_wlSmoN;
                maxEaEWL23[0] = currentEpoSat.satMW_ewl23Ambi[0];
                smoothewl23_len[0] = currentEpoSat.satMW_ewl23SmoN;
                maxEaEWL24[0] = currentEpoSat.satMW_ewl24Ambi[0];
                smoothewl24_len[0] = currentEpoSat.satMW_ewl24SmoN;
                maxEaEWL25[0] = currentEpoSat.satMW_ewl25Ambi[0];
                smoothewl25_len[0] = currentEpoSat.satMW_ewl25SmoN;

            }
            break;
        case 'C':
            if(currentEpoSat.PRN<=18)
            {
                if(currentEpoSat.PRN == MAX_EAprn[1])
                {
                    maxEaWL[1] = currentEpoSat.satMW_wlAmbi[0];
                    smoothwl_len[1] = currentEpoSat.satMW_wlSmoN;
                    maxEaEWL23[1] = currentEpoSat.satMW_ewl23Ambi[0];
                    smoothewl23_len[1] = currentEpoSat.satMW_ewl23SmoN;
                    maxEaEWL24[1] = currentEpoSat.satMW_ewl24Ambi[0];
                    smoothewl24_len[1] = currentEpoSat.satMW_ewl24SmoN;
                    maxEaEWL25[1] = currentEpoSat.satMW_ewl25Ambi[0];
                    smoothewl25_len[1] = currentEpoSat.satMW_ewl25SmoN;
                }
            }
            else
            {
                if(currentEpoSat.PRN == MAX_EAprn[2])
                {
                    maxEaWL[2] = currentEpoSat.satMW_wlAmbi[0];
                    smoothwl_len[2] = currentEpoSat.satMW_wlSmoN;
                    maxEaEWL23[2] = currentEpoSat.satMW_ewl23Ambi[0];
                    smoothewl23_len[2] = currentEpoSat.satMW_ewl23SmoN;
                    maxEaEWL24[2] = currentEpoSat.satMW_ewl24Ambi[0];
                    smoothewl24_len[2] = currentEpoSat.satMW_ewl24SmoN;
                    maxEaEWL25[2] = currentEpoSat.satMW_ewl25Ambi[0];
                    smoothewl25_len[2] = currentEpoSat.satMW_ewl25SmoN;
                }
            }
            break;
        case 'R':
            if(currentEpoSat.PRN == MAX_EAprn[3])
            {
                maxEaWL[3] = currentEpoSat.satMW_wlAmbi[0];
                smoothwl_len[3] = currentEpoSat.satMW_wlSmoN;
                maxEaEWL23[3] = currentEpoSat.satMW_ewl23Ambi[0];
                smoothewl23_len[3] = currentEpoSat.satMW_ewl23SmoN;
                maxEaEWL24[3] = currentEpoSat.satMW_ewl24Ambi[0];
                smoothewl24_len[3] = currentEpoSat.satMW_ewl24SmoN;
                maxEaEWL25[3] = currentEpoSat.satMW_ewl25Ambi[0];
                smoothewl25_len[3] = currentEpoSat.satMW_ewl25SmoN;
            }
            break;
        case 'E':
            if(currentEpoSat.PRN == MAX_EAprn[4])
            {
                maxEaWL[4] = currentEpoSat.satMW_wlAmbi[0];
                smoothwl_len[4] = currentEpoSat.satMW_wlSmoN;
                maxEaEWL23[4] = currentEpoSat.satMW_ewl23Ambi[0];
                smoothewl23_len[4] = currentEpoSat.satMW_ewl23SmoN;
                maxEaEWL24[4] = currentEpoSat.satMW_ewl24Ambi[0];
                smoothewl24_len[4] = currentEpoSat.satMW_ewl24SmoN;
                maxEaEWL25[4] = currentEpoSat.satMW_ewl25Ambi[0];
                smoothewl25_len[4] = currentEpoSat.satMW_ewl25SmoN;
            }
            break;
        default:
            break;
        }
        tempEpochSatlitData.append(currentEpoSat);
    }
    int SDflag_1 = 2,SDflanum =1,delflag = 0;
    epochSatlitData.clear();
    //An epoch cycle begins
    /*Calculate the ambiguity and variance of single difference wide roadway between satellites*/
    for (int i = 0;i < epochLen;i++)
    {
        SatlitData Satlit_SD = tempEpochSatlitData.at(i);
        int SYS_PRN=0;
        double SYSmaxEaWL =0,SYSmaxEaEWL23=0,SYSmaxEaEWL24=0,SYSmaxEaEWL25=0;
        int smoothwl_lenth = 0,smoothewl23_lenth = 0,smoothewl24_lenth = 0,smoothewl25_lenth = 0;
        switch (Satlit_SD.SatType) {
        case 'G':
            SYS_PRN = MAX_EAprn[0] ;
            SYSmaxEaWL = maxEaWL[0];
            SYSmaxEaEWL23 = maxEaEWL23[0];
            SYSmaxEaEWL24 = maxEaEWL24[0];
            smoothwl_lenth = smoothwl_len[0];
            smoothewl23_lenth = smoothewl23_len[0];
            smoothewl24_lenth = smoothewl24_len[0];
            SYSmaxEaEWL25 = maxEaEWL25[0];
            smoothewl25_lenth = smoothewl25_len[0];
            break;
        case 'C':
            if(Satlit_SD.PRN<=18)
            {
                SYS_PRN = MAX_EAprn[1] ;
                SYSmaxEaWL = maxEaWL[1];
                SYSmaxEaEWL23 = maxEaEWL23[1];
                SYSmaxEaEWL24 = maxEaEWL24[1];
                smoothwl_lenth = smoothwl_len[1];
                smoothewl23_lenth = smoothewl23_len[1];
                smoothewl24_lenth = smoothewl24_len[1];
                SYSmaxEaEWL25 = maxEaEWL25[1];
                smoothewl25_lenth = smoothewl25_len[1];
            }
            else
            {
                SYS_PRN = MAX_EAprn[2] ;
                SYSmaxEaWL = maxEaWL[2];
                SYSmaxEaEWL23 = maxEaEWL23[2];
                SYSmaxEaEWL24 = maxEaEWL24[2];
                smoothwl_lenth = smoothwl_len[2];
                smoothewl23_lenth = smoothewl23_len[2];
                smoothewl24_lenth = smoothewl24_len[2];
                SYSmaxEaEWL25 = maxEaEWL25[2];
                smoothewl25_lenth = smoothewl25_len[2];
            }
            break;
        case 'R':
            SYS_PRN = MAX_EAprn[3] ;
            SYSmaxEaWL = maxEaWL[3];
            SYSmaxEaEWL23 = maxEaEWL23[3];
            SYSmaxEaEWL24 = maxEaEWL24[3];
            smoothwl_lenth = smoothwl_len[3];
            smoothewl23_lenth = smoothewl23_len[3];
            smoothewl24_lenth = smoothewl24_len[3];
            SYSmaxEaEWL25 = maxEaEWL25[3];
            smoothewl25_lenth = smoothewl25_len[3];
            break;
        case 'E':
            SYS_PRN = MAX_EAprn[4] ;
            SYSmaxEaWL = maxEaWL[4];
            SYSmaxEaEWL23 = maxEaEWL23[4];
            SYSmaxEaEWL24 = maxEaEWL24[4];
            smoothwl_lenth = smoothwl_len[4];
            smoothewl23_lenth = smoothewl23_len[4];
            smoothewl24_lenth = smoothewl24_len[4];
            SYSmaxEaEWL25 = maxEaEWL25[4];
            smoothewl25_lenth = smoothewl25_len[4];
            break;
        default:
            break;
        }

        if(Satlit_SD.PRN != SYS_PRN)//&&Satlit_SD.Domain_of_doubt!=-1
        {
            double SDSmoothSatlit = 0.0,SD_std_deviation=0.0;int SD_WlFixs = 0;int Smooth_n=0;
            bool flagP;
            Satlit_SD.maxEA_PRN = SYS_PRN;
            SDSmoothSatlit = Satlit_SD.satMW_wlAmbi[0] - SYSmaxEaWL;//Ambiguity of single difference wide roadway between stars
            Satlit_SD.SD_Wl_float = SDSmoothSatlit;
            if(Satlit_SD.satMW_wlSmoN>smoothwl_lenth)
                Smooth_n = smoothwl_lenth;
            else
                Smooth_n = Satlit_SD.satMW_wlSmoN;
            SD_std_deviation =  (SDSmoothSatlit - Ambi_R.Float2Int(SDSmoothSatlit))*(SDSmoothSatlit - Ambi_R.Float2Int(SDSmoothSatlit)) * 1.0/Smooth_n;

            if(Ambi_R.Probability_Test(SD_WlFixs,SDSmoothSatlit,SD_std_deviation,Satlit_SD.Lambda_Wl))
            {
                if(fabs(SDSmoothSatlit - SD_WlFixs) > 1.1)
                {
                    Satlit_SD.MWflag = 2;
                    Satlit_SD.SD_Wl_float = 0;
                    delflag++;Satlit_SD.WL_fixed = false;
                    continue;
                }
                else
                {
                    Satlit_SD.SD_Wl_Int = SD_WlFixs;
                    Satlit_SD.WL_fixed = true;
                    fixed_WL_number++;
                    SDflanum++;
                }
            }
            else
            {
                Satlit_SD.WL_fixed = false;
                Par_sat_epo[i]=2;
            }
            if(opt_corr->fre_model==triple_fre||opt_corr->fre_model==four_fre||opt_corr->fre_model==five_fre)
            {
                SDSmoothSatlit = Satlit_SD.satMW_ewl23Ambi[0] - SYSmaxEaEWL23;//Ambiguity of single difference wide roadway between stars
                Satlit_SD.SD_EWl23_float = SDSmoothSatlit;
                if(Satlit_SD.satMW_ewl23SmoN>smoothewl23_lenth)
                    Smooth_n = smoothewl23_lenth;
                else
                    Smooth_n = Satlit_SD.satMW_ewl23SmoN;
                SD_std_deviation =  (SDSmoothSatlit - Ambi_R.Float2Int(SDSmoothSatlit))*(SDSmoothSatlit - Ambi_R.Float2Int(SDSmoothSatlit)) * 1.0/Smooth_n;

                if(Ambi_R.Probability_Test(SD_WlFixs,SDSmoothSatlit,SD_std_deviation,Satlit_SD.Lambda_EWl23))
                {
                    if(fabs(SDSmoothSatlit - SD_WlFixs) > 1.1)
                    {
                        Satlit_SD.MWflag = 2;
                        Satlit_SD.SD_EWl23_float = 0;
                        delflag++;Satlit_SD.EWL23_fixed = false;
                        continue;
                    }
                    else
                    {
                        Satlit_SD.SD_EWl23_Int = SD_WlFixs;
                        Satlit_SD.EWL23_fixed = true;
                        fixed_WL_number++;
                        SDflanum++;
                    }
                }
                else
                {
                    Satlit_SD.EWL23_fixed = false;
                }
            }
            if(opt_corr->fre_model==four_fre||opt_corr->fre_model==five_fre)
            {
                SDSmoothSatlit = Satlit_SD.satMW_ewl24Ambi[0] - SYSmaxEaEWL24;//Ambiguity of single difference wide roadway between stars
                Satlit_SD.SD_EWl24_float = SDSmoothSatlit;
                if(Satlit_SD.satMW_ewl24SmoN>smoothewl24_lenth)
                    Smooth_n = smoothewl24_lenth;
                else
                    Smooth_n = Satlit_SD.satMW_ewl24SmoN;
                SD_std_deviation =  (SDSmoothSatlit - Ambi_R.Float2Int(SDSmoothSatlit))*(SDSmoothSatlit - Ambi_R.Float2Int(SDSmoothSatlit)) * 1.0/Smooth_n;

                if(Ambi_R.Probability_Test(SD_WlFixs,SDSmoothSatlit,SD_std_deviation,Satlit_SD.Lambda_EWl24))
                {
                    if(fabs(SDSmoothSatlit - SD_WlFixs) > 1.1)
                    {
                        Satlit_SD.MWflag = 2;
                        Satlit_SD.SD_EWl24_float = 0;
                        delflag++;Satlit_SD.EWL24_fixed = false;
                        continue;
                    }
                    else
                    {
                        Satlit_SD.SD_EWl24_Int = SD_WlFixs;
                        Satlit_SD.EWL24_fixed = true;
                        fixed_WL_number++;
                        SDflanum++;
                    }
                }
                else
                {
                    Satlit_SD.EWL24_fixed = false;
                }
            }
            if(opt_corr->fre_model==five_fre)
            {
                SDSmoothSatlit = Satlit_SD.satMW_ewl25Ambi[0] - SYSmaxEaEWL25;//Ambiguity of single difference wide roadway between stars
                Satlit_SD.SD_EWl25_float = SDSmoothSatlit;
                if(Satlit_SD.satMW_ewl25SmoN>smoothewl25_lenth)
                    Smooth_n = smoothewl25_lenth;
                else
                    Smooth_n = Satlit_SD.satMW_ewl25SmoN;
                SD_std_deviation =  (SDSmoothSatlit - Ambi_R.Float2Int(SDSmoothSatlit))*(SDSmoothSatlit - Ambi_R.Float2Int(SDSmoothSatlit)) * 1.0/Smooth_n;

                if(Ambi_R.Probability_Test(SD_WlFixs,SDSmoothSatlit,SD_std_deviation,Satlit_SD.Lambda_EWl25))
                {
                    if(fabs(SDSmoothSatlit - SD_WlFixs) > 1.1)
                    {
                        Satlit_SD.MWflag = 2;
                        Satlit_SD.SD_EWl25_float = 0;
                        delflag++;Satlit_SD.EWL25_fixed = false;
                        continue;
                    }
                    else
                    {
                        Satlit_SD.SD_EWl25_Int = SD_WlFixs;
                        Satlit_SD.EWL25_fixed = true;
                        fixed_WL_number++;
                        SDflanum++;
                    }
                }
                else
                {
                    Satlit_SD.EWL25_fixed = false;
                }
            }
        }
        else
        {
            Par_sat_epo[i]=1;
            Satlit_SD.maxEA_PRN  = SYS_PRN;
            Satlit_SD.SD_Wl_Int = 0;
            Satlit_SD.SD_Wl_float = -1;
        }
        epochSatlitData.append(Satlit_SD);
    }
    trans_para->fixed_WL_number=fixed_WL_number;
    return SDflag_1;
}
/**Floating point solution of ambiguity in WL combined narrow lane*****/
void uduc_ambiguity_resolution::UDUC_HMW_fixed_WL(QVector< SatlitData > &epochSatlitDatanl,int *Par_sat_epo,VectorXd &SD_X,int ambiguity_flag)
{
    int epoLen = epochSatlitDatanl.length();
    for (int epo=0;epo<epoLen;epo++)
    {
        SatlitData oneSatlitData = epochSatlitDatanl.at(epo);
        if(oneSatlitData.PRN == oneSatlitData.maxEA_PRN)
            continue;
        if(oneSatlitData.cycly_slipF==1||oneSatlitData.Par_sub)
            Par_sat_epo[epo] = 1;
        if(ambiguity_flag==WL_ambiguity_flag)
        {
            if(oneSatlitData.WL_fixed)
                SD_X(epo) = oneSatlitData.SD_Wl_Int;
            else
                Par_sat_epo[epo] = 1;
        }

        if(ambiguity_flag==EWL23_ambiguity_flag)
        {
            if(oneSatlitData.EWL23_fixed)
                SD_X(epo) = oneSatlitData.SD_EWl23_Int;
            else
                Par_sat_epo[epo] = 1;
        }
        if(ambiguity_flag==EWL24_ambiguity_flag)
        {
            if(oneSatlitData.EWL24_fixed)
                SD_X(epo) = oneSatlitData.SD_EWl24_Int;
            else
                Par_sat_epo[epo] = 1;
        }

        if(ambiguity_flag==EWL25_ambiguity_flag)
        {
            if(oneSatlitData.EWL25_fixed)
                SD_X(epo) = oneSatlitData.SD_EWl25_Int;
            else
                Par_sat_epo[epo] = 1;
        }

    }
}
/**Floating point solution of ambiguity in IF combined narrow lane*****/
bool uduc_ambiguity_resolution::UDUC_NarrowLaneAR(QVector< SatlitData > &epochSatlitDatanl,VectorXd &X,MatrixXd &P,
                                                  int *Par_sat_epo, para_trans *trans_para,const corr_opt *opt_corr)
{//
    int epoLen = epochSatlitDatanl.length();
    for(int i=0;i<10;i++)
    {
        m_SYStem_satN[i] = 0;
    }
    int a_n=opt_corr->fre_model;
    MatrixXd UDUC_B,UDUC_B_SD;
    MatrixXd Trans_G,Trans_C2,Trans_C3,Trans_E,Trans_R;
    UDUC_B.resize(a_n*epoLen,trans_para->const_parm+(a_n+1)*epoLen);
    UDUC_B.setZero(a_n*epoLen,trans_para->const_parm+(a_n+1)*epoLen);
    UDUC_B_SD.resize(a_n*epoLen,trans_para->const_parm+(a_n+1)*epoLen);
    UDUC_B_SD.setZero(a_n*epoLen,trans_para->const_parm+(a_n+1)*epoLen);
    Trans_G.resize(a_n*epoLen,a_n);Trans_C2.resize(a_n*epoLen,a_n);Trans_C3.resize(a_n*epoLen,a_n);
    Trans_E.resize(a_n*epoLen,a_n);Trans_R.resize(a_n*epoLen,a_n);
    Trans_G.setZero(a_n*epoLen,a_n);Trans_C2.setZero(a_n*epoLen,a_n);Trans_C3.setZero(a_n*epoLen,a_n);
    Trans_E.setZero(a_n*epoLen,a_n);Trans_R.setZero(a_n*epoLen,a_n);
    VectorXd GNSS_epoF;
    GNSS_epoF.resize(epoLen);
    GNSS_epoF.setZero(epoLen);
    int pos_Trans_G=0,pos_Trans_C2=0,pos_Trans_C3=0,pos_Trans_E=0,pos_Trans_R=0;
    int remoN=0;int remove_satA[a_n*epoLen];int sysTemflag[5];
    for(int i=0;i<5;i++)    sysTemflag[i]=0;
    for(int i=0;i<a_n*epoLen;i++) remove_satA[i] = 0;
    for (int epo=0;epo<epoLen;epo++)
    {
        SatlitData oneSatlitData = epochSatlitDatanl.at(epo);
        for(int i=0;i<a_n;i++)
        {
            UDUC_B(a_n*epo+i,trans_para->const_parm+epoLen+a_n*epo+i) = 1;
        }
        switch (oneSatlitData.SatType) {
        case 'G':
            for(int i=0;i<a_n;i++)   Trans_G(a_n*epo+i,i)=-1;
            if(oneSatlitData.PRN == oneSatlitData.maxEA_PRN)
            {
                GNSS_epoF(epo) = 0;
                sysTemflag[0]=1;
                pos_Trans_G = epo;
                for(int i=0;i<a_n;i++)
                {
                    remove_satA[remoN] = epo*a_n+i;
                    remoN++;
                }
            }
            else
            {
                GNSS_epoF(epo) = 1;
                if(Par_sat_epo[epo] != 0)
                {
                    for(int i=0;i<a_n;i++)
                    {
                        remove_satA[remoN] = epo*a_n+i;
                        remoN++;
                    }
                }
                else
                {
                    if(oneSatlitData.cycly_slipF==1)
                    {
                        Par_sat_epo[epo] = 1;
                        for(int i=0;i<a_n;i++)
                        {
                            remove_satA[remoN] = epo*a_n+i;
                            remoN++;
                        }
                    }
                }

            }
            break;
        case 'C':
            if(oneSatlitData.PRN<=18)
            {
                for(int i=0;i<a_n;i++)   Trans_C2(a_n*epo+i,i)=-1;
                if(oneSatlitData.PRN == oneSatlitData.maxEA_PRN)
                {
                    GNSS_epoF(epo) = 0;
                    sysTemflag[1]=2;
                    pos_Trans_C2 = epo;
                    for(int i=0;i<a_n;i++)
                    {
                        remove_satA[remoN] = epo*a_n+i;
                        remoN++;
                    }
                }
                else
                {
                    GNSS_epoF(epo) = 1;
                    if(Par_sat_epo[epo] != 0)
                    {
                        for(int i=0;i<a_n;i++)
                        {
                            remove_satA[remoN] = epo*a_n+i;
                            remoN++;
                        }
                    }
                    else
                    {
                        if(oneSatlitData.cycly_slipF==1)
                        {
                            Par_sat_epo[epo] = 1;
                            for(int i=0;i<a_n;i++)
                            {
                                remove_satA[remoN] = epo*a_n+i;
                                remoN++;
                            }
                        }
                    }
                }
            }
            else
            {
                for(int i=0;i<a_n;i++)   Trans_C3(a_n*epo+i,i)=-1;
                if(oneSatlitData.PRN == oneSatlitData.maxEA_PRN)
                {
                    GNSS_epoF(epo) = 0;
                    sysTemflag[2]=3;
                    pos_Trans_C3 = epo;
                    for(int i=0;i<a_n;i++)
                    {
                        remove_satA[remoN] = epo*a_n+i;
                        remoN++;
                    }
                }
                else
                {
                    GNSS_epoF(epo) = 1;
                    if(Par_sat_epo[epo] != 0)
                    {
                        for(int i=0;i<a_n;i++)
                        {
                            remove_satA[remoN] = epo*a_n+i;
                            remoN++;
                        }
                    }
                    else
                    {
                        if(oneSatlitData.cycly_slipF==1)
                        {
                            Par_sat_epo[epo] = 1;
                            for(int i=0;i<a_n;i++)
                            {
                                remove_satA[remoN] = epo*a_n+i;
                                remoN++;
                            }
                        }
                    }
                }
            }
            break;
        case 'R':
            for(int i=0;i<a_n;i++)   Trans_R(a_n*epo+i,i)=-1;
            if(oneSatlitData.PRN == oneSatlitData.maxEA_PRN)
            {
                GNSS_epoF(epo) = 0;
                sysTemflag[3]=4;
                pos_Trans_R = epo;
                for(int i=0;i<a_n;i++)
                {
                    remove_satA[remoN] = epo*a_n+i;
                    remoN++;
                }
            }
            else
            {
                GNSS_epoF(epo) = 1;
                if(Par_sat_epo[epo] != 0)
                {
                    for(int i=0;i<a_n;i++)
                    {
                        remove_satA[remoN] = epo*a_n+i;
                        remoN++;
                    }
                }
                else
                {
                    if(oneSatlitData.cycly_slipF==1)
                    {
                        Par_sat_epo[epo] = 1;
                        for(int i=0;i<a_n;i++)
                        {
                            remove_satA[remoN] = epo*a_n+i;
                            remoN++;
                        }
                    }
                }

            }
            break;
        case 'E':
            for(int i=0;i<a_n;i++)   Trans_E(a_n*epo+i,i)=-1;
            if(oneSatlitData.PRN == oneSatlitData.maxEA_PRN)
            {
                GNSS_epoF(epo) = 0;
                sysTemflag[4]=5;
                pos_Trans_E = epo;
                for(int i=0;i<a_n;i++)
                {
                    remove_satA[remoN] = epo*a_n+i;
                    remoN++;
                }
            }
            else
            {
                GNSS_epoF(epo) = 1;
                if(Par_sat_epo[epo] != 0)
                {
                    for(int i=0;i<a_n;i++)
                    {
                        remove_satA[remoN] = epo*a_n+i;
                        remoN++;
                    }
                }
                else
                {
                    if(oneSatlitData.cycly_slipF==1)
                    {
                        Par_sat_epo[epo] = 1;
                        for(int i=0;i<a_n;i++)
                        {
                            remove_satA[remoN] = epo*a_n+i;
                            remoN++;
                        }
                    }
                }

            }
            break;
        default:
            break;
        }
    }
    Ambi_R.descending_order(remoN,remove_satA);
    int sortMaxEpo[5];  for(int i =0;i<5;i++)   sortMaxEpo[i]=-1;
    for(int i =0;i<5;i++)
    {
        switch (sysTemflag[i]) {
        case 1:
            sortMaxEpo[0] = pos_Trans_G;
            UDUC_B.block(0,trans_para->const_parm+epoLen+pos_Trans_G*a_n,a_n*epoLen,a_n) = Trans_G;
            break;
        case 2:
            sortMaxEpo[1] = pos_Trans_C2;
            UDUC_B.block(0,trans_para->const_parm+epoLen+pos_Trans_C2*a_n,a_n*epoLen,a_n) = Trans_C2;
            break;
        case 3:
            sortMaxEpo[2] = pos_Trans_C3;
            UDUC_B.block(0,trans_para->const_parm+epoLen+pos_Trans_C3*a_n,a_n*epoLen,a_n) = Trans_C3;
            break;
        case 4:
            sortMaxEpo[3] = pos_Trans_R;
            UDUC_B.block(0,trans_para->const_parm+epoLen+pos_Trans_R*a_n,a_n*epoLen,a_n) = Trans_R;
            break;
        case 5:
            sortMaxEpo[4] = pos_Trans_E;
            UDUC_B.block(0,trans_para->const_parm+epoLen+pos_Trans_E*a_n,a_n*epoLen,a_n) = Trans_E;
            break;
        default:
            break;
        }
    }
    UDUC_B_SD=UDUC_B;
    for(int i=0;i<5;i++) {
        for(int j=i+1;j<5;j++) {
            if(sortMaxEpo[i]<sortMaxEpo[j]) {
                int temp=sortMaxEpo[i],temp1 = sysTemflag[i];
                sortMaxEpo[i]=sortMaxEpo[j];
                sortMaxEpo[j]=temp;
                sysTemflag[i]=sysTemflag[j];//max epo sort
                sysTemflag[j]=temp1;
            }
        }
    }
    for(int i =0;i<5;i++)
    {
        switch (sysTemflag[i]) {
        case 1:
            for(int j=a_n-1;j>=0;j--)  m_mat_cal.RemoveRow(UDUC_B,pos_Trans_G*a_n+j);
            break;
        case 2:
            for(int j=a_n-1;j>=0;j--)  m_mat_cal.RemoveRow(UDUC_B,pos_Trans_C2*a_n+j);
            break;
        case 3:
            for(int j=a_n-1;j>=0;j--)  m_mat_cal.RemoveRow(UDUC_B,pos_Trans_C3*a_n+j);
            break;
        case 4:
            for(int j=a_n-1;j>=0;j--)  m_mat_cal.RemoveRow(UDUC_B,pos_Trans_R*a_n+j);
            break;
        case 5:
            for(int j=a_n-1;j>=0;j--)  m_mat_cal.RemoveRow(UDUC_B,pos_Trans_E*a_n+j);
            break;
        default:
            break;
        }
    }
    for(int i =0;i<remoN;i++)
    {
        m_mat_cal.RemoveRow(UDUC_B_SD,remove_satA[i]);
    }
    Ambiguity_mat *Amb_mat = AGNSS_APPData::sGetAPPDataInstance()->mGetAmbiguity_matData();

    Amb_mat->La_SD_P = UDUC_B_SD * P * UDUC_B_SD.transpose();
    Amb_mat->Qab    = P.block(0,0,trans_para->const_parm+epoLen,trans_para->const_parm+(a_n+1)*epoLen) * UDUC_B_SD.transpose();
//    Amb_mat->SD_GNSS_F = UDUC_B_SD * GNSS_epoF;
    Amb_mat->La_SD_X =  UDUC_B_SD * X;
    Amb_mat->Fixed_Qa = UDUC_B * P * UDUC_B.transpose();
    Amb_mat->Fixed_Qab = P.block(0,0,trans_para->const_parm+epoLen,trans_para->const_parm+(a_n+1)*epoLen) * UDUC_B.transpose();

    return true;
}
bool uduc_ambiguity_resolution::UDUC_CAR_FixedWAR_narrow_Lane_AR(VectorXd &X,MatrixXd &P,VectorXd &Fixed_WAR)
{//
    int info = 0;
    para_trans *trans_para = AGNSS_APPData::sGetAPPDataInstance()->mGettrans_paraData();
    MatrixXd amb_fix;double ratio = trans_para->ratio_value;int out_fixamb_num = 2;
    trans_para->lambda_radio =0;
    trans_para->fixed_sat = 0;
    info = m_lambda.QLamdaSearch(X,P,amb_fix, &ratio,out_fixamb_num);
    if(info)    return false;
    trans_para->lambda_radio = ratio;
    if(ratio>=trans_para->ratio_value)
    {
        trans_para->fixed_sat = P.rows();
    }

    if(ratio>=trans_para->ratio_value)
        Fixed_WAR = amb_fix.topLeftCorner(P.rows(), 1);
    else
        return false;
    return true;
}
bool uduc_ambiguity_resolution::UDUC_CARWAR_Wide_Lane_AR(VectorXd &X,MatrixXd &P,VectorXd &Fixed_WAR)
{//
    int info = 0;
    para_trans *trans_para = AGNSS_APPData::sGetAPPDataInstance()->mGettrans_paraData();
    MatrixXd amb_fix;double ratio = trans_para->ratio_value;int out_fixamb_num = 2;
    info = m_lambda.QLamdaSearch(X,P,amb_fix, &ratio,out_fixamb_num);
    trans_para->lambda_radio =0;
    trans_para->fixed_sat = 0;
    if(info)    return false;
    trans_para->lambda_radio = ratio;
    if(ratio>=trans_para->ratio_value)
    {
        trans_para->fixed_sat = P.rows();
    }
//    trans_para->fixed_sat = P.rows();
    if(ratio>=trans_para->ratio_value)
        Fixed_WAR = amb_fix.topLeftCorner(P.rows(), 1);
    else
        return false;

    return true;
}

void uduc_ambiguity_resolution::UDUC_mixed_matrixX_trans(QVector< SatlitData > &epochSatlitDataLa,int UDUC_SD_H_t_row,
                                                                    MatrixXd &UDUC_B_SDEWLa, MatrixXd &UDUC_B_SDEWLb,MatrixXd &UDUC_B_SDWLa, MatrixXd &UDUC_B_SDWLb,
                                                         MatrixXd &UDUC_B_SDNLa, MatrixXd &UDUC_B_SDNLb,para_trans *trans_para)
{
    VectorXd Fixed_WL_flag;
    UDUC_B_SDEWLa.resize(UDUC_SD_H_t_row,UDUC_SD_H_t_row);
    UDUC_B_SDEWLa.setIdentity(UDUC_SD_H_t_row,UDUC_SD_H_t_row);
    UDUC_B_SDEWLb.resize(UDUC_SD_H_t_row,UDUC_SD_H_t_row);
    UDUC_B_SDEWLb.setIdentity(UDUC_SD_H_t_row,UDUC_SD_H_t_row);

    UDUC_B_SDWLa.resize(UDUC_SD_H_t_row,UDUC_SD_H_t_row);
    UDUC_B_SDWLa.setIdentity(UDUC_SD_H_t_row,UDUC_SD_H_t_row);
    UDUC_B_SDWLb.resize(UDUC_SD_H_t_row,UDUC_SD_H_t_row);
    UDUC_B_SDWLb.setIdentity(UDUC_SD_H_t_row,UDUC_SD_H_t_row);

    Fixed_WL_flag.resize(UDUC_SD_H_t_row);
    Fixed_WL_flag.setZero(UDUC_SD_H_t_row);
    VectorXd Fixed_NL_flag;
    UDUC_B_SDNLa.resize(UDUC_SD_H_t_row,UDUC_SD_H_t_row);
    UDUC_B_SDNLa.setIdentity(UDUC_SD_H_t_row,UDUC_SD_H_t_row);
//    UDUC_B_IF_WL.resize(UDUC_SD_H_t_row,UDUC_SD_H_t_row);
//    UDUC_B_IF_WL.setIdentity(UDUC_SD_H_t_row,UDUC_SD_H_t_row);
    UDUC_B_SDNLb.resize(UDUC_SD_H_t_row,UDUC_SD_H_t_row);
    UDUC_B_SDNLb.setIdentity(UDUC_SD_H_t_row,UDUC_SD_H_t_row);
    Fixed_NL_flag.resize(UDUC_SD_H_t_row);
    Fixed_NL_flag.setZero(UDUC_SD_H_t_row);
    int epoLen = epochSatlitDataLa.length();
    int pos_ambiguity=trans_para->const_parm+epoLen;
    {
        int remo_EWa=0;int remove_satEWa[UDUC_B_SDEWLa.rows()];
        int remo_EWb=0;int remove_satEWb[UDUC_B_SDEWLb.rows()];
        int remo_Wa=0;int remove_satWa[UDUC_B_SDWLa.rows()];
        int remo_Wb=0;int remove_satWb[UDUC_B_SDWLb.rows()];
        int remo_Na=0;int remove_satNa[UDUC_B_SDNLa.rows()];
        int remo_Nb=0;int remove_satNb[UDUC_B_SDNLb.rows()];
        for(int i=0;i<pos_ambiguity;i++)
        {
            remove_satEWa[remo_EWa] = i;//作为求取EWL模糊度的行变化，删除行
            remo_EWa++;
            remove_satWa[remo_Wa] = i;//作为求取WL模糊度的行变化，删除行
            remo_Wa++;
            remove_satNa[remo_Na] = i;//作为求取NL模糊度的行变化，删除行
            remo_Na++;
        }
        for (int epo=0;epo<epoLen;epo++)
        {
            SatlitData oneSatlitData = epochSatlitDataLa.at(epo);
            if(oneSatlitData.PRN != oneSatlitData.maxEA_PRN)
            {
                /*一旦取出IF组合的频率，则该频率组合同样作为WL固定的频率*/
                bool frist_flag = true;bool Secod_flag = true;
                for(int mufre=0;mufre<5;mufre++)
                {
                    if(oneSatlitData.ObsFreflag[mufre]!=1)  continue;
                    if(frist_flag)
                    {
                        remove_satEWa[remo_EWa] = pos_ambiguity;//作为求取EWL模糊度的行变化，删除行
                        remo_EWa++;
                        remove_satWa[remo_Wa] = pos_ambiguity;//作为求取WL模糊度的行变化，删除行
                        remo_Wa++;
                        remove_satNb[remo_Nb] = pos_ambiguity;//作为求取NL模糊度的b行变化，删除行
                        remo_Nb++;
                        frist_flag = false;
                    }
                    else if(Secod_flag)
                    {
                        remove_satEWa[remo_EWa] = pos_ambiguity;//作为求取EWL模糊度的行变化，删除行
                        remo_EWa++;
                        remove_satWb[remo_Wb] = pos_ambiguity;//作为求取WL模糊度的行变化，删除行
                        remo_Wb++;
                        remove_satNb[remo_Nb] = pos_ambiguity;//作为求取NL模糊度的b行变化，删除行
                        remo_Nb++;
                        remove_satNa[remo_Na] = pos_ambiguity;//作为求取NL模糊度的b行变化，删除行
                        remo_Na++;
                        Secod_flag = false;
                    }
                    else
                    {
                        remove_satWa[remo_Wa] = pos_ambiguity;//作为求取WL模糊度的行变化，删除行
                        remo_Wa++;
                        remove_satEWb[remo_EWb] = pos_ambiguity;//作为求取WL模糊度的行变化，删除行
                        remo_EWb++;
                        remove_satWb[remo_Wb] = pos_ambiguity;//作为求取WL模糊度的行变化，删除行
                        remo_Wb++;
                        remove_satNb[remo_Nb] = pos_ambiguity;//作为求取NL模糊度的b行变化，删除行
                        remo_Nb++;
                        remove_satNa[remo_Na] = pos_ambiguity;//作为求取NL模糊度的b行变化，删除行
                        remo_Na++;
                    }
                    pos_ambiguity++;
                }
            }
        }
        Ambi_R.descending_order(remo_EWa,remove_satEWa);
        Ambi_R.descending_order(remo_EWb,remove_satEWb);
        Ambi_R.descending_order(remo_Wa,remove_satWa);
        Ambi_R.descending_order(remo_Wb,remove_satWb);
        Ambi_R.descending_order(remo_Na,remove_satNa);
        Ambi_R.descending_order(remo_Nb,remove_satNb);
        for(int i =0;i<remo_EWb;i++)
        {
            m_mat_cal.RemoveRow(UDUC_B_SDEWLb,remove_satEWb[i]);
        }
        for(int i =0;i<remo_EWa;i++)
        {
            m_mat_cal.RemoveRow(UDUC_B_SDEWLa,remove_satEWa[i]);
        }
        for(int i =0;i<remo_Wb;i++)
        {
            m_mat_cal.RemoveRow(UDUC_B_SDWLb,remove_satWb[i]);
        }
        for(int i =0;i<remo_Wa;i++)
        {
            m_mat_cal.RemoveRow(UDUC_B_SDWLa,remove_satWa[i]);
        }
        for(int i =0;i<remo_Nb;i++)
        {
            m_mat_cal.RemoveRow(UDUC_B_SDNLb,remove_satNb[i]);
        }
        for(int i =0;i<remo_Na;i++)
        {
            m_mat_cal.RemoveRow(UDUC_B_SDNLa,remove_satNa[i]);
        }
    }

}


void uduc_ambiguity_resolution::UDUC_mixed_WL_IFNL(QVector< SatlitData > &epochSatlitDataLa,int UDUC_SD_H_t_row,
                                                                    MatrixXd &UDUC_B_IF_WL,MatrixXd &UDUC_AMB_T,VectorXd &Amb_flag,para_trans *trans_para)
{
    int epoLen = epochSatlitDataLa.length();
    UDUC_AMB_T.resize(UDUC_SD_H_t_row,UDUC_SD_H_t_row);
    UDUC_AMB_T.setIdentity(UDUC_SD_H_t_row,UDUC_SD_H_t_row);
    Amb_flag.resize(UDUC_SD_H_t_row);
    Amb_flag.setZero(UDUC_SD_H_t_row);
    int frequency_select[5];for(int i=0;i<5;i++)    frequency_select[i]=-1;
    int select_fre_n = 0;double fre1,fre2,IF1,IF2;

    {
        int pos_ambiguity=trans_para->const_parm+epoLen;//设置起始量，其中参考卫星已删除略过参考星
        for (int epo=0;epo<epoLen;epo++)
        {
            QVector <int> cof_i;QVector <int> cof_j;//系数
            QVector <double> IF_combination;
            QVector <double> WL_combination;
            SatlitData oneSatlitData = epochSatlitDataLa.at(epo);
            //qDebug()<<"mixed_WL_IFNL 1126";
            if(oneSatlitData.PRN != oneSatlitData.maxEA_PRN)
            {
                if(oneSatlitData.ObsLenfre<2)   {
                    Amb_flag(pos_ambiguity)=-1;//不固定卫星模糊度
                    pos_ambiguity++;
                    continue;
                }
                select_fre_n = 0;
                for(int mufre=0;mufre<5;mufre++)
                {
                    if(oneSatlitData.ObsFreflag[mufre]!=1)  continue;
                    frequency_select[select_fre_n] = mufre;
                    select_fre_n++;
                }
                /*取出，IF组合放大的噪声以及频率组合，WL组合*/
                for(int i=0;i<select_fre_n;i++)
                {
                    fre1 = oneSatlitData.Frq[frequency_select[i]];
                    for(int j=i+1;j<select_fre_n;j++)
                    {
                        fre2 = oneSatlitData.Frq[frequency_select[j]];
                        IF1 = ALFA_IF(fre1,fre2);
                        IF2 = BATA_IF(fre1,fre2);
                        IF1 = SQR(IF1)+SQR(IF2);
                        cof_i.append(frequency_select[i]);
                        cof_j.append(frequency_select[j]);
                        IF_combination.append(IF1);
                        IF1 = M_C/(fre1-fre2);
                        WL_combination.append(IF1);
                    }
                }
                //qDebug()<<"mixed_WL_IFNL 1158";
                /*一旦取出IF组合的频率，则该频率组合同样作为WL固定的频率*/
                int fre_pos1,fre_pos2;
                int select_fre_1 = 0,select_fre_2 = 0;
                bool frist_flag = true;
                QVector <int> frequency_r;
                int frist_amb_pos = pos_ambiguity;
                for(int mufre=0;mufre<5;mufre++)
                {
                    if(oneSatlitData.ObsFreflag[mufre]!=1)  continue;
                    //qDebug()<<"mixed_WL_IFNL 1158";
                    if(frist_flag)
                    {
                        auto pos_min = std::distance(std::begin(IF_combination),std::min_element(IF_combination.begin(), IF_combination.end()));//找出噪声最小的IF组合
                        int pos_min_value = pos_min;
                        //qDebug()<<"mixed_WL_IFNL 1172";
//                        if(oneSatlitData.ObsLenfre==3&&(oneSatlitData.SatType=='G'||oneSatlitData.SatType=='C'))
//                            pos_min_value = 0;
                        fre_pos1 = cof_i[pos_min_value];//找出第二个频率的位置
                        fre_pos2 = cof_j[pos_min_value];

                        UDUC_B_IF_WL(pos_ambiguity,pos_ambiguity+1) = BATA_HMW(oneSatlitData.Frq[fre_pos1],oneSatlitData.Frq[fre_pos2]);
//                        if(IF_combination[pos_min_value]>10)
//                            Amb_flag(pos_ambiguity)=-1;
                        WL_combination[pos_min_value]=0;
                        select_fre_1 = 0;select_fre_2 = 0;
                        select_fre_n = 0;
                        for(int i=0;i<5;i++)
                        {
                            if(oneSatlitData.ObsFreflag[i]!=1)  continue;
                            if(i==fre_pos1) select_fre_1 = select_fre_n;
                            if(i==fre_pos2) select_fre_2 = select_fre_n;
                            select_fre_n++;
                        }
                        UDUC_AMB_T(pos_ambiguity,pos_ambiguity) =0;
                        UDUC_AMB_T(pos_ambiguity,pos_ambiguity+select_fre_1) =ALFA_IF(oneSatlitData.Frq[fre_pos1],oneSatlitData.Frq[fre_pos2]);
                        UDUC_AMB_T(pos_ambiguity,pos_ambiguity+select_fre_2) = BATA_IF(oneSatlitData.Frq[fre_pos1],oneSatlitData.Frq[fre_pos2]);
                        frist_flag=false;
                        UDUC_AMB_T(pos_ambiguity+1,pos_ambiguity+1) =0;
                        UDUC_AMB_T(pos_ambiguity+1,pos_ambiguity+select_fre_1) =  1;
                        UDUC_AMB_T(pos_ambiguity+1,pos_ambiguity+select_fre_2) = -1;
                        //qDebug()<<"mixed_WL_IFNL 1196";
                    }
                    else
                    {
                        int pos_max_value; bool find_WL = false;int find_n = 0;

                        while(find_n<WL_combination.length())
                        {
                            //qDebug()<<"mixed_WL_IFNL 1204";
                            auto pos_max = std::distance(std::begin(WL_combination), std::max_element(WL_combination.begin(), WL_combination.end()));//找出噪声最小的IF组合
                            pos_max_value = pos_max;
                            fre_pos1= cof_i[pos_max_value];//找出第二个频率的位置
                            fre_pos2=cof_j[pos_max_value];
                            WL_combination[pos_max_value]=0;
                            select_fre_1 = 0;select_fre_2 = 0;
                            select_fre_n = 0;
                            for(int i=0;i<5;i++)
                            {
                                if(oneSatlitData.ObsFreflag[i]!=1)  continue;
                                if(i==fre_pos1) select_fre_1 = select_fre_n;
                                if(i==fre_pos2) select_fre_2 = select_fre_n;
                                select_fre_n++;
                            }
                            /*如果两个频率均出现了，则不会增加模糊度矩阵的秩，对固定没有提升，因此要找未出现的频率*/
                            if(!frequency_r.contains(select_fre_1)||!frequency_r.contains(select_fre_2))
                            {
                                find_WL=true;
                                break;
                            }
                            find_n++;
                        }
                        if(find_WL)
                        {
                            //qDebug()<<"mixed_WL_IFNL 1229";
                            UDUC_AMB_T(pos_ambiguity+1,pos_ambiguity+1) =0;
                            UDUC_AMB_T(pos_ambiguity+1,frist_amb_pos+select_fre_1) =  1;
                            UDUC_AMB_T(pos_ambiguity+1,frist_amb_pos+select_fre_2) = -1;
                            //qDebug()<<"mixed_WL_IFNL 1233";
                        }
                    }
                    frequency_r.append(select_fre_1);frequency_r.append(select_fre_2);
                    pos_ambiguity++;
                }
                //qDebug()<<"mixed_WL_IFNL 1232";
                frequency_r.clear();
            }
            WL_combination.clear();
            IF_combination.clear();
            cof_i.clear();
            cof_j.clear();
        }

    }

}
void uduc_ambiguity_resolution::UDUC_mixed_BSSD(QVector< SatlitData > &epochSatlitDataLa,int Total_sat_fre,
                                                                    MatrixXd &UDUC_SD_H_t,para_trans *trans_para)
{
    int epoLen = epochSatlitDataLa.length();
    UDUC_SD_H_t.resize(trans_para->const_parm+epoLen+Total_sat_fre,trans_para->const_parm+epoLen+Total_sat_fre);
    UDUC_SD_H_t.setIdentity(trans_para->const_parm+epoLen+Total_sat_fre,trans_para->const_parm+epoLen+Total_sat_fre);
    int pos_ambiguity=trans_para->const_parm+epoLen;
    MatrixXd SD_Sys_pos;//定义用于存放各系统参考卫星的卫星G C2 C3 R E
    SD_Sys_pos.resize(5,5);
    SD_Sys_pos.setZero(5,5);
    {
        for (int epo=0;epo<epoLen;epo++)
        {
            SatlitData oneSatlitData = epochSatlitDataLa.at(epo);
            switch (oneSatlitData.SatType) {
            case 'G':
                if(oneSatlitData.PRN == oneSatlitData.maxEA_PRN)
                {
                    for(int mufre=0;mufre<5;mufre++)
                    {
                        if(oneSatlitData.ObsFreflag[mufre]!=1)  continue;
                        SD_Sys_pos(0,mufre) = pos_ambiguity;
                        pos_ambiguity++;
                    }
                }
                else
                {
                    for(int mufre=0;mufre<5;mufre++)
                    {
                        if(oneSatlitData.ObsFreflag[mufre]!=1)  continue;
                        pos_ambiguity++;
                    }
                }
                break;
            case 'C':
                if(oneSatlitData.PRN<=18)
                {
                    if(oneSatlitData.PRN == oneSatlitData.maxEA_PRN)
                    {
                        for(int mufre=0;mufre<5;mufre++)
                        {
                            if(oneSatlitData.ObsFreflag[mufre]!=1)  continue;
                            SD_Sys_pos(1,mufre) = pos_ambiguity;
                            pos_ambiguity++;
                        }
                    }
                    else
                    {
                        for(int mufre=0;mufre<5;mufre++)
                        {
                            if(oneSatlitData.ObsFreflag[mufre]!=1)  continue;
                            pos_ambiguity++;
                        }
                    }
                }
                else
                {
                    if(oneSatlitData.PRN == oneSatlitData.maxEA_PRN)
                    {
                        for(int mufre=0;mufre<5;mufre++)
                        {
                            if(oneSatlitData.ObsFreflag[mufre]!=1)  continue;
                            SD_Sys_pos(2,mufre) = pos_ambiguity;
                            pos_ambiguity++;
                        }
                    }
                    else
                    {
                        for(int mufre=0;mufre<5;mufre++)
                        {
                            if(oneSatlitData.ObsFreflag[mufre]!=1)  continue;
                            pos_ambiguity++;
                        }
                    }
                }
                break;
            case 'R':
                if(oneSatlitData.PRN == oneSatlitData.maxEA_PRN)
                {
                    for(int mufre=0;mufre<5;mufre++)
                    {
                        if(oneSatlitData.ObsFreflag[mufre]!=1)  continue;
                        SD_Sys_pos(3,mufre) = pos_ambiguity;
                        pos_ambiguity++;
                    }
                }
                else
                {
                    for(int mufre=0;mufre<5;mufre++)
                    {
                        if(oneSatlitData.ObsFreflag[mufre]!=1)  continue;
                        pos_ambiguity++;
                    }
                }
                break;
            case 'E':
                if(oneSatlitData.PRN == oneSatlitData.maxEA_PRN)
                {
                    for(int mufre=0;mufre<5;mufre++)
                    {
                        if(oneSatlitData.ObsFreflag[mufre]!=1)  continue;
                        SD_Sys_pos(4,mufre) = pos_ambiguity;
                        pos_ambiguity++;
                    }
                }
                else
                {
                    for(int mufre=0;mufre<5;mufre++)
                    {
                        if(oneSatlitData.ObsFreflag[mufre]!=1)  continue;
                        pos_ambiguity++;
                    }
                }
                break;
            default:
                break;
            }
        }
        int remoN=0;int remove_satA[25];
        for(int i=0;i<25;i++)
        {
            remove_satA[i] = 0;
        }
        for(int i=0;i<5;i++)
            for(int j=0;j<5;j++)
            {
                if(SD_Sys_pos(i,j)!=0)
                {
                    remove_satA[remoN]=SD_Sys_pos(i,j);
                    remoN++;
                }
            }
        pos_ambiguity=trans_para->const_parm+epoLen;
        for (int epo=0;epo<epoLen;epo++)
        {
            SatlitData oneSatlitData = epochSatlitDataLa.at(epo);
            switch (oneSatlitData.SatType) {
            case 'G':
                if(oneSatlitData.PRN != oneSatlitData.maxEA_PRN)
                {
                    for(int mufre=0;mufre<5;mufre++)
                    {
                        if(oneSatlitData.ObsFreflag[mufre]!=1)  continue;
                        UDUC_SD_H_t(pos_ambiguity,SD_Sys_pos(0,mufre)) = -1;
                        pos_ambiguity++;
                    }
                }
                else
                {
                    for(int mufre=0;mufre<5;mufre++)
                    {
                        if(oneSatlitData.ObsFreflag[mufre]!=1)  continue;
                        pos_ambiguity++;
                    }
                }
                break;
            case 'C':
                if(oneSatlitData.PRN<=18)
                {
                    if(oneSatlitData.PRN != oneSatlitData.maxEA_PRN)
                    {
                        for(int mufre=0;mufre<5;mufre++)
                        {
                            if(oneSatlitData.ObsFreflag[mufre]!=1)  continue;
                            UDUC_SD_H_t(pos_ambiguity,SD_Sys_pos(1,mufre)) = -1;
                            pos_ambiguity++;
                        }
                    }
                    else
                    {
                        for(int mufre=0;mufre<5;mufre++)
                        {
                            if(oneSatlitData.ObsFreflag[mufre]!=1)  continue;
                            pos_ambiguity++;
                        }
                    }
                }
                else
                {
                    if(oneSatlitData.PRN != oneSatlitData.maxEA_PRN)
                    {
                        for(int mufre=0;mufre<5;mufre++)
                        {
                            if(oneSatlitData.ObsFreflag[mufre]!=1)  continue;
                            UDUC_SD_H_t(pos_ambiguity,SD_Sys_pos(2,mufre)) = -1;
                            pos_ambiguity++;
                        }
                    }
                    else
                    {
                        for(int mufre=0;mufre<5;mufre++)
                        {
                            if(oneSatlitData.ObsFreflag[mufre]!=1)  continue;
                            pos_ambiguity++;
                        }
                    }
                }
                break;
            case 'R':
                if(oneSatlitData.PRN != oneSatlitData.maxEA_PRN)
                {
                    for(int mufre=0;mufre<5;mufre++)
                    {
                        if(oneSatlitData.ObsFreflag[mufre]!=1)  continue;
                        UDUC_SD_H_t(pos_ambiguity,SD_Sys_pos(3,mufre)) = -1;
                        pos_ambiguity++;
                    }
                }
                else
                {
                    for(int mufre=0;mufre<5;mufre++)
                    {
                        if(oneSatlitData.ObsFreflag[mufre]!=1)  continue;
                        pos_ambiguity++;
                    }
                }
                break;
            case 'E':
                if(oneSatlitData.PRN != oneSatlitData.maxEA_PRN)
                {
                    for(int mufre=0;mufre<5;mufre++)
                    {
                        if(oneSatlitData.ObsFreflag[mufre]!=1)  continue;
                        UDUC_SD_H_t(pos_ambiguity,SD_Sys_pos(4,mufre)) = -1;
                        pos_ambiguity++;
                    }
                }
                else
                {
                    for(int mufre=0;mufre<5;mufre++)
                    {
                        if(oneSatlitData.ObsFreflag[mufre]!=1)  continue;
                        pos_ambiguity++;
                    }
                }
                break;
            default:
                break;
            }
        }
        Ambi_R.descending_order(remoN,remove_satA);
        for(int i =0;i<remoN;i++)
        {
            m_mat_cal.RemoveRow(UDUC_SD_H_t,remove_satA[i]);
        }
    }
}


void uduc_ambiguity_resolution::UDUC_mixed_LAMBDA_cascade_AR(VectorXd &X,MatrixXd &P,VectorXd &Amb_flag,MatrixXd &UDUC_B_SDEWLa, MatrixXd &UDUC_B_SDEWLb,MatrixXd &UDUC_B_SDWLa, MatrixXd &UDUC_B_SDWLb,
                                                             MatrixXd &UDUC_B_IF_WL,MatrixXd &UDUC_B_SDNLa, MatrixXd &UDUC_B_SDNLb,VectorXd &B_X,MatrixXd &BP_k,int &fix_flag)
{
    /*定义状态转移矩阵*/
    VectorXd Fixed_WAR,A_X,PAR_AX,Par_sat_EWL_N,Par_sat_WL_N,Par_sat_NL_N,fixed_WL_flag;
    MatrixXd AP_EWL_k_Cov,BP_EWL_k_Cov,ABP_EWL_k_Cov,AP_WL_k_Cov,BP_WL_k_Cov,ABP_WL_k_Cov,
            AP_NL_k_Cov,BP_NL_k_Cov,ABP_NL_k_Cov,AP_k,ABP_k,fixed_BPk,PAR_AXP;
    MatrixXd Trans_WAR_AX;
    {
        AP_EWL_k_Cov = UDUC_B_SDEWLa * P * UDUC_B_SDEWLa.transpose();//MWaa = HWa * Px * HWa T;
        BP_EWL_k_Cov = UDUC_B_SDEWLb * P * UDUC_B_SDEWLb.transpose();//MWbb = HWb * Px * HWb T;
        ABP_EWL_k_Cov = UDUC_B_SDEWLa * P * UDUC_B_SDEWLb.transpose();//MWab = HWa * Px * HWb T;
        Par_sat_EWL_N.resize(AP_EWL_k_Cov.rows());
        Par_sat_EWL_N.setZero(AP_EWL_k_Cov.rows());
        A_X = UDUC_B_SDEWLa * X;
        B_X = UDUC_B_SDEWLb * X;
        AP_k = AP_EWL_k_Cov;
        BP_k = BP_EWL_k_Cov;
        ABP_k = ABP_EWL_k_Cov;
        PAR_AX = A_X;
        PAR_AXP = AP_k;
        fix_flag = 0;//NO FIXED
        bool fixed_Su = false;
        bool fixed_nl = false;
        bool fixed_WL  = true;
        while(PAR_AXP.rows()>=4)
        {
            fixed_Su = UDUC_CARWAR_Wide_Lane_AR(PAR_AX,PAR_AXP,Fixed_WAR);
            if(fixed_Su)
            {
                PAR_AX = A_X;  PAR_AXP=AP_k;
                int pos_fixed = 0;
                for(int i=0;i<AP_k.rows();i++)
                {
                    if(Par_sat_EWL_N(i)==0)
                    {
                        PAR_AX(i) = Fixed_WAR(pos_fixed);
                        pos_fixed++;
                    }
                }
                break;
            }
            else
            {
                Trans_WAR_AX.resize(AP_k.rows(),AP_k.rows());
                Trans_WAR_AX.setIdentity(AP_k.rows(),AP_k.rows());
                int rem_Con_X=0;
                int remove_Con_XROW[AP_k.rows()];
                for(int i=0;i<AP_k.rows();i++)
                {
                    remove_Con_XROW[i] = 0;
                }
                if(fixed_WL)//WL首次固定失败
                {
                    fixed_WL = false;
                    for(int i=0;i<AP_k.rows();i++){
                        if(fabs(A_X(i)-qRound(A_X(i)))>0.4)//删除就近取整大于0.4的WL模糊度
                        {
                            Par_sat_EWL_N(i)=1;
                        }
                    }
                    for(int i=0;i<AP_k.rows();i++){
                        if(Par_sat_EWL_N(i)!=0)//删除就近取整大于0.4的WL模糊度
                        {
                            remove_Con_XROW[rem_Con_X] = i;
                            rem_Con_X++;
                        }
                    }
                }
                else{
                    double max_var = -1;int max_var_pos = -1;
                    for(int i=0;i<AP_k.rows();i++){
                        if(Par_sat_EWL_N(i)==0)//删除就近取整大于0.4的WL模糊度
                        {
                            if(max_var<AP_k(i,i))
                            {
                                max_var = AP_k(i,i);
                                max_var_pos = i;
                            }
                        }

                    }
                    if(max_var_pos!=-1)
                    {
                        Par_sat_EWL_N(max_var_pos) = 1;
                    }
                    for(int i=0;i<AP_k.rows();i++){
                        if(Par_sat_EWL_N(i)!=0)//删除就近取整大于0.4的WL模糊度
                        {
                            remove_Con_XROW[rem_Con_X] = i;
                            rem_Con_X++;
                        }
                    }
                }
                Ambi_R.descending_order(rem_Con_X,remove_Con_XROW);
                for(int i =0;i<rem_Con_X;i++)
                {
                    m_mat_cal.RemoveRow(Trans_WAR_AX,remove_Con_XROW[i]);
                }
                PAR_AX = Trans_WAR_AX*A_X;PAR_AXP = Trans_WAR_AX*AP_k*Trans_WAR_AX.transpose();
            }
        }
        if(fixed_Su)
        {
            fix_flag = 1;//NO FIXED
            B_X = B_X - ABP_k.transpose() * AP_k.inverse() * (A_X-PAR_AX);
            fixed_BPk = BP_k - ABP_k.transpose() * AP_k.inverse() * ABP_k;
            AP_WL_k_Cov = UDUC_B_SDWLa * UDUC_B_SDEWLb.transpose() * fixed_BPk  * UDUC_B_SDEWLb * UDUC_B_SDWLa.transpose();//MNaa = HNa * HWb T * MWbb * HWb *HNa  T;
            BP_WL_k_Cov = UDUC_B_SDWLb * UDUC_B_SDEWLb.transpose() * fixed_BPk  * UDUC_B_SDEWLb * UDUC_B_SDWLb.transpose();//MNbb = HNb * HWb T * MWbb * HWb *HNb  T;
            ABP_WL_k_Cov = UDUC_B_SDWLa * UDUC_B_SDEWLb.transpose() * fixed_BPk  * UDUC_B_SDEWLb * UDUC_B_SDWLb.transpose();//MNab = HNa * HWb T * MWbb * HWb *HNb  T;

            A_X = UDUC_B_SDWLa * UDUC_B_SDEWLb.transpose() *B_X ;
            B_X = UDUC_B_SDWLb * UDUC_B_SDEWLb.transpose()*B_X;
            AP_k = AP_WL_k_Cov;
            BP_k = BP_WL_k_Cov;
            ABP_k = ABP_WL_k_Cov;
            PAR_AX = A_X;
            PAR_AXP = AP_k;
        }
        else
        {
            AP_WL_k_Cov = UDUC_B_SDWLa * UDUC_B_SDEWLb.transpose() * BP_k  * UDUC_B_SDEWLb * UDUC_B_SDWLa.transpose();//MNaa = HNa * HWb T * MWbb * HWb *HNa  T;
            BP_WL_k_Cov = UDUC_B_SDWLb * UDUC_B_SDEWLb.transpose() * BP_k  * UDUC_B_SDEWLb * UDUC_B_SDWLb.transpose();//MNbb = HNb * HWb T * MWbb * HWb *HNb  T;
            ABP_WL_k_Cov = UDUC_B_SDWLa * UDUC_B_SDEWLb.transpose() * BP_k  * UDUC_B_SDEWLb * UDUC_B_SDWLb.transpose();//MNab = HNa * HWb T * MWbb * HWb *HNb  T;
            A_X = UDUC_B_SDWLa * UDUC_B_SDEWLb.transpose() *B_X ;
            B_X = UDUC_B_SDWLb * UDUC_B_SDEWLb.transpose()*B_X;
            AP_k = AP_WL_k_Cov;
            BP_k = BP_WL_k_Cov;
            ABP_k = ABP_WL_k_Cov;
            PAR_AX = A_X;
            PAR_AXP = AP_k;
        }
        Par_sat_WL_N.resize(AP_WL_k_Cov.rows());
        Par_sat_WL_N.setZero(AP_WL_k_Cov.rows());
        fixed_Su = false;
        while(PAR_AXP.rows()>=4)
        {
            fixed_Su = UDUC_CARWAR_Wide_Lane_AR(PAR_AX,PAR_AXP,Fixed_WAR);
            if(fixed_Su)
            {
                PAR_AX = A_X;  PAR_AXP=AP_k;
                int pos_fixed = 0;
                for(int i=0;i<AP_k.rows();i++)
                {
                    if(Par_sat_WL_N(i)==0)
                    {
                        PAR_AX(i) = Fixed_WAR(pos_fixed);
                        pos_fixed++;
                    }
                }
                break;
            }
            else
            {
                Trans_WAR_AX.resize(AP_k.rows(),AP_k.rows());
                Trans_WAR_AX.setIdentity(AP_k.rows(),AP_k.rows());
                int rem_Con_X=0;
                int remove_Con_XROW[AP_k.rows()];
                for(int i=0;i<AP_k.rows();i++)
                {
                    remove_Con_XROW[i] = 0;
                }
                if(fixed_WL)//WL首次固定失败
                {
                    fixed_WL = false;
                    for(int i=0;i<AP_k.rows();i++){
                        if(fabs(A_X(i)-qRound(A_X(i)))>0.4)//删除就近取整大于0.4的WL模糊度
                        {
                            Par_sat_WL_N(i)=1;
                        }
                    }
                    for(int i=0;i<AP_k.rows();i++){
                        if(Par_sat_WL_N(i)!=0)//删除就近取整大于0.4的WL模糊度
                        {
                            remove_Con_XROW[rem_Con_X] = i;
                            rem_Con_X++;
                        }
                    }
                }
                else{
                    double max_var = -1;int max_var_pos = -1;
                    for(int i=0;i<AP_k.rows();i++){
                        if(Par_sat_WL_N(i)==0)//删除就近取整大于0.4的WL模糊度
                        {
                            if(max_var<AP_k(i,i))
                            {
                                max_var = AP_k(i,i);
                                max_var_pos = i;
                            }
                        }

                    }
                    if(max_var_pos!=-1)
                    {
                        Par_sat_WL_N(max_var_pos) = 1;
                    }
                    for(int i=0;i<AP_k.rows();i++){
                        if(Par_sat_WL_N(i)!=0)//删除就近取整大于0.4的WL模糊度
                        {
                            remove_Con_XROW[rem_Con_X] = i;
                            rem_Con_X++;
                        }
                    }
                }
                Ambi_R.descending_order(rem_Con_X,remove_Con_XROW);
                for(int i =0;i<rem_Con_X;i++)
                {
                    m_mat_cal.RemoveRow(Trans_WAR_AX,remove_Con_XROW[i]);
                }
                PAR_AX = Trans_WAR_AX*A_X;PAR_AXP = Trans_WAR_AX*AP_k*Trans_WAR_AX.transpose();
            }
        }
        if(fixed_Su)
        {
            fix_flag = 2;//WL FIXED
            B_X = B_X - ABP_k.transpose() * AP_k.inverse() * (A_X-PAR_AX);
            fixed_BPk = BP_k - ABP_k.transpose() * AP_k.inverse() * ABP_k;
            AP_NL_k_Cov = UDUC_B_SDNLa * UDUC_B_SDWLb.transpose() * fixed_BPk  * UDUC_B_SDWLb * UDUC_B_SDNLa.transpose();//MNaa = HNa * HWb T * MWbb * HWb *HNa  T;
            BP_NL_k_Cov = UDUC_B_SDNLb * UDUC_B_SDWLb.transpose() * fixed_BPk  * UDUC_B_SDWLb * UDUC_B_SDNLb.transpose();//MNbb = HNb * HWb T * MWbb * HWb *HNb  T;
            ABP_NL_k_Cov = UDUC_B_SDNLa * UDUC_B_SDWLb.transpose() * fixed_BPk  * UDUC_B_SDWLb * UDUC_B_SDNLb.transpose();//MNab = HNa * HWb T * MWbb * HWb *HNb  T;
            MatrixXd IF_WL_coff = UDUC_B_SDNLa* UDUC_B_IF_WL *UDUC_B_SDWLa.transpose();
            A_X = UDUC_B_SDNLa * UDUC_B_SDWLb.transpose()*B_X - IF_WL_coff*PAR_AX;
            B_X = UDUC_B_SDNLb * UDUC_B_SDWLb.transpose()*B_X;
            AP_k = AP_NL_k_Cov;
            BP_k = BP_NL_k_Cov;
            ABP_k = ABP_NL_k_Cov;
            PAR_AX = A_X;
            PAR_AXP = AP_k;
            /*部分模糊度固定方法研究 szg NL 未减WL1202*/
            fixed_WL_flag = IF_WL_coff*Par_sat_WL_N;
            Par_sat_NL_N = UDUC_B_SDNLa * Amb_flag-fixed_WL_flag;
            Trans_WAR_AX.resize(AP_k.rows(),AP_k.rows());
            Trans_WAR_AX.setIdentity(AP_k.rows(),AP_k.rows());
            int rem_Con_X=0;
            int remove_Con_XROW[AP_k.rows()];
            for(int i=0;i<AP_k.rows();i++)
            {
                remove_Con_XROW[i] = 0;
            }
            for(int i=0;i<AP_k.rows();i++){
                if(Par_sat_NL_N(i)!=0)//删除就近取整大于0.4的WL模糊度
                {
                    remove_Con_XROW[rem_Con_X] = i;
                    rem_Con_X++;
                }
            }
            Ambi_R.descending_order(rem_Con_X,remove_Con_XROW);
            for(int i =0;i<rem_Con_X;i++)
            {
                m_mat_cal.RemoveRow(Trans_WAR_AX,remove_Con_XROW[i]);
            }
            PAR_AX = Trans_WAR_AX*A_X;PAR_AXP = Trans_WAR_AX*AP_k*Trans_WAR_AX.transpose();
            while(PAR_AXP.rows()>=4)
            {
                fixed_nl = UDUC_CAR_FixedWAR_narrow_Lane_AR(PAR_AX,PAR_AXP,Fixed_WAR);
                if(fixed_nl)
                {
                    PAR_AX = A_X;  PAR_AXP=AP_k;
                    int pos_fixed = 0;
                    for(int i=0;i<AP_k.rows();i++)
                    {
                        if(Par_sat_NL_N(i)==0)
                        {
                            PAR_AX(i) = Fixed_WAR(pos_fixed);
                            pos_fixed++;
                        }
                    }
                    break;
                }
                else
                {
                    Trans_WAR_AX.resize(AP_k.rows(),AP_k.rows());
                    Trans_WAR_AX.setIdentity(AP_k.rows(),AP_k.rows());
                    rem_Con_X=0;
                    for(int i=0;i<AP_k.rows();i++)
                    {
                        remove_Con_XROW[i] = 0;
                    }
                    double max_var = -1;int max_var_pos = -1;
                    for(int i=0;i<AP_k.rows();i++){
                        if(Par_sat_NL_N(i)==0)//删除就近取整大于0.4的WL模糊度
                        {
                            if(max_var<AP_k(i,i))
                            {
                                max_var = AP_k(i,i);
                                max_var_pos = i;
                            }
                        }
                    }
                    if(max_var_pos!=-1)
                    {
                        Par_sat_NL_N(max_var_pos) = 1;
                    }
                    for(int i=0;i<AP_k.rows();i++){
                        if(Par_sat_NL_N(i)!=0)//删除就近取整大于0.4的WL模糊度
                        {
                            remove_Con_XROW[rem_Con_X] = i;
                            rem_Con_X++;
                        }
                    }
                    Ambi_R.descending_order(rem_Con_X,remove_Con_XROW);
                    for(int i =0;i<rem_Con_X;i++)
                    {
                        m_mat_cal.RemoveRow(Trans_WAR_AX,remove_Con_XROW[i]);
                    }
                    PAR_AX = Trans_WAR_AX*A_X;PAR_AXP = Trans_WAR_AX*AP_k*Trans_WAR_AX.transpose();
                }

            }
        }
        if(fixed_nl)
        {
            fix_flag = 3;//NL FIXED
            B_X = B_X - ABP_k.transpose() * AP_k.inverse() * (A_X-PAR_AX);
            fixed_BPk = BP_k - ABP_k.transpose() * AP_k.inverse() * ABP_k;
        }
    }

}
/*计算混合多频下的PPP-AR，通过设置不同的WL和NL 选择和处理策略实现*/
void uduc_ambiguity_resolution::UDUC_mixed_fre_cascading_ambiguity_resolution(QVector< SatlitData > &epochSatlitDataLa,VectorXd &X,MatrixXd &P,int Total_sat_fre,
                                                                    VectorXd &UDUC_SD_X,MatrixXd &UDUC_SD_P,int &fix_flag,para_trans *trans_para)
{
    int epoLen = epochSatlitDataLa.length();

    /*定义标志向量*/
    VectorXd Par_sat_epoWL;VectorXd Par_sat_epoNL;
    Par_sat_epoWL.resize(trans_para->const_parm+epoLen+Total_sat_fre);Par_sat_epoWL.setZero(trans_para->const_parm+epoLen+Total_sat_fre);
    Par_sat_epoNL.resize(trans_para->const_parm+epoLen+Total_sat_fre);Par_sat_epoNL.setZero(trans_para->const_parm+epoLen+Total_sat_fre);
    //qDebug()<<"UCAR 1687";
    /*为了避免因丢失频率后导致所选WL模型不一致，无法星间单差的问题，首先基于原始观测值模糊度，构建星间单差矩阵，完成星间单差*/
    /*星间单差的参考星选择为频率最多，卫星高度角最大的卫星为参考下，默认所有卫星都可进行星间单差，无法星间单差的模糊度不考虑固定*/
    MatrixXd UDUC_SD_H_t;
    UDUC_mixed_BSSD(epochSatlitDataLa,Total_sat_fre,UDUC_SD_H_t,trans_para);
    /*计算EWL WL NL，将星间单差的原始观测向量和矩阵转化为EWL WL 和IF组合NL的向量和矩阵*/
    //qDebug()<<"UCAR 1693";
    MatrixXd UDUC_AMB_T,UDUC_B_IF_WL; VectorXd Amb_flag;
    UDUC_B_IF_WL.resize(UDUC_SD_H_t.rows(),UDUC_SD_H_t.rows());
    UDUC_B_IF_WL.setZero(UDUC_SD_H_t.rows(),UDUC_SD_H_t.rows());
    UDUC_mixed_WL_IFNL(epochSatlitDataLa,UDUC_SD_H_t.rows(),UDUC_B_IF_WL,UDUC_AMB_T,Amb_flag,trans_para);
    /*定义选择一个WL模糊度最小和多个超宽巷EWL作为状态转移矩阵的选择条件，定义高度角排序作为NL部分模糊度筛选的条件*/
    //qDebug()<<"UCAR 1699";
    /*定义，IF组合噪声最小的选择作为NL模糊度构建的条件，确定状态转移矩阵*/
    MatrixXd UDUC_B_SDEWLa,UDUC_B_SDEWLb,UDUC_B_SDWLa,UDUC_B_SDWLb;
    MatrixXd UDUC_B_SDNLa,UDUC_B_SDNLb;
    UDUC_mixed_matrixX_trans(epochSatlitDataLa,UDUC_SD_H_t.rows(),UDUC_B_SDEWLa,UDUC_B_SDEWLb,UDUC_B_SDWLa,UDUC_B_SDWLb,UDUC_B_SDNLa,UDUC_B_SDNLb,trans_para);
    //qDebug()<<"UCAR 1704";
    /*定义，找到用于获取NL IF组合整数特性的WL固定向量，并获得NL模糊度*/
    VectorXd SD_X;MatrixXd SD_P;
    SD_X = UDUC_SD_H_t * X;
    SD_X = UDUC_AMB_T * SD_X;
    SD_P = UDUC_AMB_T * UDUC_SD_H_t * P *UDUC_SD_H_t.transpose()* UDUC_AMB_T.transpose();
    /*顺序模糊度固定*/
    UDUC_mixed_LAMBDA_cascade_AR(SD_X,SD_P,Amb_flag,UDUC_B_SDEWLa,UDUC_B_SDEWLb,UDUC_B_SDWLa,UDUC_B_SDWLb,
                                     UDUC_B_IF_WL,UDUC_B_SDNLa,UDUC_B_SDNLb,UDUC_SD_X,UDUC_SD_P,fix_flag);
    //qDebug()<<"UCAR 1712";
}


void uduc_ambiguity_resolution::UDUC_cascading_ambiguity_resolution(QVector< SatlitData > &epochSatlitDataLa,VectorXd &X,MatrixXd &P,int Total_sat_fre,
                                                                    VectorXd &UDUC_SD_X,int &fix_flag,para_trans *trans_para,const corr_opt *opt_corr)
{
    int epoLen = epochSatlitDataLa.length();

    int Par_lenth=0;
    int Par_sat_epo[epoLen];
    VectorXd Par_sat_epoEWL;VectorXd Par_sat_epoWL;VectorXd Par_sat_epoNL;
    Par_sat_epoEWL.resize(trans_para->const_parm+epoLen+Total_sat_fre);Par_sat_epoEWL.setZero(trans_para->const_parm+epoLen+Total_sat_fre);
    Par_sat_epoWL.resize(trans_para->const_parm+epoLen+Total_sat_fre);Par_sat_epoWL.setZero(trans_para->const_parm+epoLen+Total_sat_fre);
    Par_sat_epoNL.resize(trans_para->const_parm+epoLen+Total_sat_fre);Par_sat_epoNL.setZero(trans_para->const_parm+epoLen+Total_sat_fre);

    double fre_1_coeff[5];double fre_2_coeff[5];int frequency_1[5];int frequency_2[5];
    if(opt_corr->Ambiguity_acquisition==poor_ambiguity)
    {
        MatrixXd UDUC_B; MatrixXd UDUC_SD_P;MatrixXd UDUC_Pa;VectorXd UDUC_Xa;
        VectorXd UDUC_X_b;MatrixXd UDUC_P_b;MatrixXd UDUC_P_ab;
        MatrixXd UDUC_B_SDEWL;MatrixXd UDUC_B_SDWL;MatrixXd UDUC_B_SDNL;MatrixXd Trans_M;
        MatrixXd UDUC_UDEWL;MatrixXd UDUC_UDWL;MatrixXd UDUC_UDNL;
        UDUC_B.resize(trans_para->const_parm+epoLen+Total_sat_fre,trans_para->const_parm+epoLen+Total_sat_fre);
        UDUC_B.setIdentity(trans_para->const_parm+epoLen+Total_sat_fre,trans_para->const_parm+epoLen+Total_sat_fre);
        UDUC_B_SDEWL.resize(epoLen,trans_para->const_parm+epoLen+Total_sat_fre);
        UDUC_B_SDEWL.setZero(epoLen,trans_para->const_parm+epoLen+Total_sat_fre);
        UDUC_B_SDWL.resize(epoLen,trans_para->const_parm+epoLen+Total_sat_fre);
        UDUC_B_SDWL.setZero(epoLen,trans_para->const_parm+epoLen+Total_sat_fre);
        UDUC_B_SDNL.resize(epoLen,trans_para->const_parm+epoLen+Total_sat_fre);
        UDUC_B_SDNL.setZero(epoLen,trans_para->const_parm+epoLen+Total_sat_fre);
        int pos_flag;int ambiguity_pos = 0;int fixed_ambiguity=0;
        if(opt_corr->UDUC_fre==triple_fre)
        {
            for(int i=0;i<5;i++)
            {
                fre_1_coeff[i]=1;
                fre_2_coeff[i]=-1;
                frequency_1[i]=1;
                frequency_2[i]=2;
            }
            pos_flag=1;
            UDUC_Fixed_cascading_ambiguity(epochSatlitDataLa,X,P,fre_1_coeff,fre_2_coeff,frequency_1,frequency_2,Total_sat_fre,
                                           Par_sat_epoEWL,Par_lenth,UDUC_B,pos_flag,UDUC_SD_P,UDUC_B_SDEWL,UDUC_UDEWL,UDUC_SD_X,trans_para);
            Par_sat_epoWL=Par_sat_epoEWL;

            for(int i=0;i<5;i++)
            {
                fre_1_coeff[i]=1;
                fre_2_coeff[i]=-1;
                frequency_1[i]=0;
                frequency_2[i]=1;
            }
            UDUC_Fixed_cascading_ambiguity(epochSatlitDataLa,X,P,fre_1_coeff,fre_2_coeff,frequency_1,frequency_2,Total_sat_fre,
                                           Par_sat_epoWL,Par_lenth,UDUC_B,pos_flag,UDUC_SD_P,UDUC_B_SDWL,UDUC_UDWL,UDUC_SD_X,trans_para);
            Par_sat_epoNL=Par_sat_epoWL;

            for(int i=0;i<5;i++)
            {
                frequency_1[i]=0;
                frequency_2[i]=1;
            }
            for(int i=0;i<epoLen;i++)
            {
                SatlitData oneSatlitData = epochSatlitDataLa.at(i);
                if(!(oneSatlitData.Frq[0]&&oneSatlitData.Frq[1]))
                    continue;
                double frequency_F1,frequency_F2;
                frequency_F1 = oneSatlitData.Frq[0];
                frequency_F2 = oneSatlitData.Frq[1];

                switch (oneSatlitData.SatType) {
                case 'G':
                    fre_1_coeff[0]=ALFA_IF(frequency_F1,frequency_F2);
                    fre_2_coeff[0]=BATA_IF(frequency_F1,frequency_F2);
                    break;
                case 'C':
                    if(oneSatlitData.BDS_2flag)
                    {
                        fre_1_coeff[1]=ALFA_IF(frequency_F1,frequency_F2);
                        fre_2_coeff[1]=BATA_IF(frequency_F1,frequency_F2);
                    }
                    else
                    {
                        fre_1_coeff[2]=ALFA_IF(frequency_F1,frequency_F2);
                        fre_2_coeff[2]=BATA_IF(frequency_F1,frequency_F2);
                    }
                    break;
                case 'R':
                    {
                        fre_1_coeff[3]=ALFA_IF(frequency_F1,frequency_F2);
                        fre_2_coeff[3]=BATA_IF(frequency_F1,frequency_F2);
                    }
                    break;
                case 'E':
                    {
                        fre_1_coeff[4]=ALFA_IF(frequency_F1,frequency_F2);
                        fre_2_coeff[4]=BATA_IF(frequency_F1,frequency_F2);
                    }
                    break;
                default:
                    break;
                }
            }

            pos_flag=0;
            UDUC_Fixed_cascading_ambiguity(epochSatlitDataLa,X,P,fre_1_coeff,fre_2_coeff,frequency_1,frequency_2,Total_sat_fre,
                                           Par_sat_epoNL,Par_lenth,UDUC_B,pos_flag,UDUC_SD_P,UDUC_B_SDNL,UDUC_UDNL,UDUC_SD_X,trans_para);
            UDUC_SD_X = UDUC_B * X;
            UDUC_SD_P = UDUC_B * P * UDUC_B.transpose();
            /* EWL ambiguity resolution */
            UDUC_parametric_solution(UDUC_SD_X,UDUC_SD_P,UDUC_X_b,UDUC_P_b,UDUC_P_ab,Trans_M,Total_sat_fre,Par_sat_epoEWL,UDUC_B_SDEWL);
            Par_sat_epoWL = Trans_M*Par_sat_epoWL;
            Par_sat_epoNL = Trans_M*Par_sat_epoNL;
            UDUC_UDWL = UDUC_UDWL*Trans_M.transpose();
            UDUC_B_SDWL = UDUC_B_SDWL*Trans_M.transpose();
            UDUC_UDNL = UDUC_UDNL*Trans_M.transpose();
            UDUC_B_SDNL = UDUC_B_SDNL*Trans_M.transpose();
            UDUC_Xa = UDUC_UDEWL * UDUC_SD_X;
            UDUC_Pa = UDUC_UDEWL * UDUC_SD_P *UDUC_UDEWL.transpose();
            for(int i=0;i<epoLen;i++)   Par_sat_epo[i]=0;
            MatrixXd UDUC_SD_Pa;VectorXd UDUC_SD_Xa;
            UDUC_SD_Xa=UDUC_Xa;UDUC_SD_Pa=UDUC_Pa;
            UDUC_Fixed_partial_ambiguity(epochSatlitDataLa,Par_sat_epo,UDUC_SD_Pa,UDUC_SD_Xa);
            if(trans_para->Sosr_flag)
            {
                Ambi_R.Standard_of_success_rate(epochSatlitDataLa,Par_sat_epo,UDUC_SD_Pa,trans_para->Ps);
                UDUC_SD_Xa=UDUC_Xa;UDUC_SD_Pa=UDUC_Pa;
                UDUC_Fixed_partial_ambiguity(epochSatlitDataLa,Par_sat_epo,UDUC_SD_Pa,UDUC_SD_Xa);
            }
            if(UDUC_LAMBDA_search(epochSatlitDataLa,UDUC_SD_Pa,UDUC_SD_Xa,Par_sat_epo,UDUC_Pa,UDUC_Xa,trans_para,opt_corr))
            {
                fix_flag =1;
                MatrixXd UDUC_fixed_Pa;VectorXd UDUC_fixed_Xa;VectorXd UDUC_float_Xa;
                UDUC_float_Xa = UDUC_B_SDEWL * UDUC_SD_X;
                UDUC_fixed_Xa = UDUC_float_Xa;
                UDUC_fixed_Pa = UDUC_B_SDEWL * UDUC_SD_P *UDUC_B_SDEWL.transpose();
                ambiguity_pos = 0;fixed_ambiguity=0;
                for(int i=0;i<epoLen;i++)
                {
                    SatlitData oneSatlitData = epochSatlitDataLa.at(i);
                    if(oneSatlitData.PRN == oneSatlitData.maxEA_PRN)
                        continue;
                    if(Par_sat_epo[i]==0)
                    {
                        UDUC_fixed_Xa(ambiguity_pos)=UDUC_SD_Xa(fixed_ambiguity);
                        fixed_ambiguity++;
                    }
                    ambiguity_pos++;
                }
                UDUC_SD_X = UDUC_X_b - UDUC_P_ab * UDUC_fixed_Pa.inverse() * (UDUC_float_Xa-UDUC_fixed_Xa);
                UDUC_SD_P = UDUC_P_b - UDUC_P_ab * UDUC_fixed_Pa.inverse() * UDUC_P_ab.transpose();
                /* WL ambiguity resolution */
                UDUC_parametric_solution(UDUC_SD_X,UDUC_SD_P,UDUC_X_b,UDUC_P_b,UDUC_P_ab,Trans_M,Total_sat_fre,Par_sat_epoWL,UDUC_B_SDWL);
                Par_sat_epoNL = Trans_M*Par_sat_epoNL;
                UDUC_UDNL = UDUC_UDNL*Trans_M.transpose();
                UDUC_B_SDNL = UDUC_B_SDNL*Trans_M.transpose();
                UDUC_Xa = UDUC_UDWL * UDUC_SD_X;
                UDUC_Pa = UDUC_UDWL * UDUC_SD_P *UDUC_UDWL.transpose();
                for(int i=0;i<epoLen;i++)   Par_sat_epo[i]=0;
//                MatrixXd UDUC_SD_Pa;VectorXd UDUC_SD_Xa;
                UDUC_SD_Xa=UDUC_Xa;UDUC_SD_Pa=UDUC_Pa;
                UDUC_Fixed_partial_ambiguity(epochSatlitDataLa,Par_sat_epo,UDUC_SD_Pa,UDUC_SD_Xa);
                int wide_line = 0;
                QVector< SatlitData > epochSatlitDataLa_WL;
                for(int i=0;i<epoLen;i++)
                {
                    SatlitData oneSatlitData = epochSatlitDataLa.at(i);
                    if(Par_sat_epo[i]==0&&oneSatlitData.PRN!=oneSatlitData.maxEA_PRN)
                    {
                        oneSatlitData.SD_Wl_float = UDUC_SD_Xa(wide_line);
                        wide_line++;
                    }
                    epochSatlitDataLa_WL.append(oneSatlitData);
                }
                epochSatlitDataLa=epochSatlitDataLa_WL;
                if(trans_para->Sosr_flag)
                {
                    Ambi_R.Standard_of_success_rate(epochSatlitDataLa,Par_sat_epo,UDUC_SD_Pa,trans_para->Ps);
                    UDUC_SD_Xa=UDUC_Xa;UDUC_SD_Pa=UDUC_Pa;
                    UDUC_Fixed_partial_ambiguity(epochSatlitDataLa,Par_sat_epo,UDUC_SD_Pa,UDUC_SD_Xa);
                }
                if(UDUC_LAMBDA_search(epochSatlitDataLa,UDUC_SD_Pa,UDUC_SD_Xa,Par_sat_epo,UDUC_Pa,UDUC_Xa,trans_para,opt_corr))
                {
                    fix_flag =2;
//                    MatrixXd UDUC_fixed_Pa;VectorXd UDUC_fixed_Xa;VectorXd UDUC_float_Xa;
                    UDUC_float_Xa = UDUC_B_SDWL * UDUC_SD_X;
                    UDUC_fixed_Xa = UDUC_float_Xa;
                    UDUC_fixed_Pa = UDUC_B_SDWL * UDUC_SD_P *UDUC_B_SDWL.transpose();
                    ambiguity_pos = 0;fixed_ambiguity=0;VectorXd UDUC_fixed_WLa;
                    UDUC_fixed_WLa.resize(epoLen);UDUC_fixed_WLa.setZero(epoLen);
                    epochSatlitDataLa_WL.clear();
                    for(int i=0;i<epoLen;i++)
                    {
                        SatlitData oneSatlitData = epochSatlitDataLa.at(i);
                        if(oneSatlitData.PRN == oneSatlitData.maxEA_PRN)
                        {
                            epochSatlitDataLa_WL.append(oneSatlitData);
                            continue;
                        }
                        double frequency_corr = 0;
                        if(Par_sat_epo[i]==0)
                        {
                            UDUC_fixed_Xa(ambiguity_pos)=UDUC_SD_Xa(fixed_ambiguity);
                            frequency_corr=oneSatlitData.Frq[1]/(oneSatlitData.Frq[0]-oneSatlitData.Frq[1]);
                            UDUC_fixed_WLa(i)=frequency_corr*UDUC_SD_Xa(fixed_ambiguity);
                            fixed_ambiguity++;
                            oneSatlitData.SD_Wl_Int = UDUC_fixed_Xa(ambiguity_pos);
                            epochSatlitDataLa_WL.append(oneSatlitData);
                        }
                        else
                            epochSatlitDataLa_WL.append(oneSatlitData);
                        ambiguity_pos++;
                    }
                    epochSatlitDataLa=epochSatlitDataLa_WL;


                    UDUC_SD_X = UDUC_X_b - UDUC_P_ab * UDUC_fixed_Pa.inverse() * (UDUC_float_Xa-UDUC_fixed_Xa);
                    UDUC_SD_P = UDUC_P_b - UDUC_P_ab * UDUC_fixed_Pa.inverse() * UDUC_P_ab.transpose();
                    /* NL ambiguity resolution */
                    UDUC_parametric_solution(UDUC_SD_X,UDUC_SD_P,UDUC_X_b,UDUC_P_b,UDUC_P_ab,Trans_M,Total_sat_fre,Par_sat_epoNL,UDUC_B_SDNL);
                    UDUC_Xa = UDUC_UDNL * UDUC_SD_X;
                    UDUC_Pa = UDUC_UDNL * UDUC_SD_P *UDUC_UDNL.transpose();
//                    for(int i=0;i<epoLen;i++)   Par_sat_epo[i]=0;
    //                MatrixXd UDUC_SD_Pa;VectorXd UDUC_SD_Xa;
                    UDUC_SD_Xa=UDUC_Xa;UDUC_SD_Pa=UDUC_Pa;
                    UDUC_Fixed_partial_NL_ambiguity(epochSatlitDataLa,Par_sat_epo,UDUC_SD_Pa,UDUC_SD_Xa,UDUC_fixed_WLa);
                    int narrow_line = 0;
                    QVector< SatlitData > epochSatlitDataLa_NL;
                    for(int i=0;i<epoLen;i++)
                    {
                        SatlitData oneSatlitData = epochSatlitDataLa.at(i);
                        if(Par_sat_epo[i]==0&&oneSatlitData.PRN!=oneSatlitData.maxEA_PRN)
                        {
                            oneSatlitData.SD_Nl_Float = UDUC_SD_Xa(narrow_line);
                            narrow_line++;
                        }
                        epochSatlitDataLa_NL.append(oneSatlitData);
                    }
                    epochSatlitDataLa=epochSatlitDataLa_NL;
                    if(trans_para->Sosr_flag)
                    {
                        Ambi_R.Standard_of_success_rate(epochSatlitDataLa,Par_sat_epo,UDUC_SD_Pa,trans_para->Ps);
                        UDUC_SD_Xa=UDUC_Xa;UDUC_SD_Pa=UDUC_Pa;
                        UDUC_Fixed_partial_NL_ambiguity(epochSatlitDataLa,Par_sat_epo,UDUC_SD_Pa,UDUC_SD_Xa,UDUC_fixed_WLa);
                    }
                    if(UDUC_LAMBDA_search_NL(epochSatlitDataLa,UDUC_SD_Pa,UDUC_SD_Xa,Par_sat_epo,UDUC_Pa,UDUC_Xa,UDUC_fixed_WLa,trans_para,opt_corr))
                    {
                        fix_flag =3;
    //                    MatrixXd UDUC_fixed_Pa;VectorXd UDUC_fixed_Xa;VectorXd UDUC_float_Xa;
                        UDUC_float_Xa = UDUC_B_SDNL * UDUC_SD_X ;
                        UDUC_fixed_Xa = UDUC_float_Xa;
                        UDUC_fixed_Pa = UDUC_B_SDNL * UDUC_SD_P *UDUC_B_SDNL.transpose();
                        ambiguity_pos = 0;fixed_ambiguity=0;
                        epochSatlitDataLa_NL.clear();
                        for(int i=0;i<epoLen;i++)
                        {
                            SatlitData oneSatlitData = epochSatlitDataLa.at(i);
                            if(oneSatlitData.PRN == oneSatlitData.maxEA_PRN)
                            {
                                epochSatlitDataLa_NL.append(oneSatlitData);
                                continue;
                            }
                            if(Par_sat_epo[i]==0)
                            {
                                UDUC_fixed_Xa(ambiguity_pos)=UDUC_SD_Xa(fixed_ambiguity);
                                UDUC_float_Xa(ambiguity_pos)=UDUC_float_Xa(ambiguity_pos)-UDUC_fixed_WLa(i);
                                fixed_ambiguity++;
                                oneSatlitData.SD_Nl_fix = UDUC_fixed_Xa(ambiguity_pos);
                                epochSatlitDataLa_NL.append(oneSatlitData);
                            }
                            else
                                epochSatlitDataLa_NL.append(oneSatlitData);
                            ambiguity_pos++;
                        }
                        epochSatlitDataLa=epochSatlitDataLa_NL;
                        UDUC_SD_X = UDUC_X_b - UDUC_P_ab * UDUC_fixed_Pa.inverse() * (UDUC_float_Xa-UDUC_fixed_Xa);
                        UDUC_SD_P = UDUC_P_b - UDUC_P_ab * UDUC_fixed_Pa.inverse() * UDUC_P_ab.transpose();
                    }
                }
            }
        }
        if(opt_corr->UDUC_fre==double_fre)
        {
            MatrixXd UDUC_SD_Pa;VectorXd UDUC_SD_Xa;
            MatrixXd UDUC_fixed_Pa;VectorXd UDUC_fixed_Xa;VectorXd UDUC_float_Xa;
            pos_flag=1;
            for(int i=0;i<5;i++)
            {
                fre_1_coeff[i]=1;
                fre_2_coeff[i]=-1;
                frequency_1[i]=0;
                frequency_2[i]=1;
            }
            UDUC_Fixed_cascading_ambiguity(epochSatlitDataLa,X,P,fre_1_coeff,fre_2_coeff,frequency_1,frequency_2,Total_sat_fre,
                                           Par_sat_epoWL,Par_lenth,UDUC_B,pos_flag,UDUC_SD_P,UDUC_B_SDWL,UDUC_UDWL,UDUC_SD_X,trans_para);
            Par_sat_epoNL=Par_sat_epoWL;

            for(int i=0;i<5;i++)
            {
                frequency_1[i]=0;
                frequency_2[i]=1;
            }

            for(int i=0;i<epoLen;i++)
            {
                SatlitData oneSatlitData = epochSatlitDataLa.at(i);
                if(!(oneSatlitData.Frq[0]&&oneSatlitData.Frq[1]))
                    continue;
                double frequency_F1,frequency_F2;
                frequency_F1 = oneSatlitData.Frq[0];
                frequency_F2 = oneSatlitData.Frq[1];

                switch (oneSatlitData.SatType) {
                case 'G':
                    fre_1_coeff[0]=ALFA_IF(frequency_F1,frequency_F2);
                    fre_2_coeff[0]=BATA_IF(frequency_F1,frequency_F2);
                    break;
                case 'C':
                    if(oneSatlitData.BDS_2flag)
                    {
                        fre_1_coeff[1]=ALFA_IF(frequency_F1,frequency_F2);
                        fre_2_coeff[1]=BATA_IF(frequency_F1,frequency_F2);
                    }
                    else
                    {
                        fre_1_coeff[2]=ALFA_IF(frequency_F1,frequency_F2);
                        fre_2_coeff[2]=BATA_IF(frequency_F1,frequency_F2);
                    }
                    break;
                case 'R':
                    {
                        fre_1_coeff[3]=ALFA_IF(frequency_F1,frequency_F2);
                        fre_2_coeff[3]=BATA_IF(frequency_F1,frequency_F2);
                    }
                    break;
                case 'E':
                    {
                        fre_1_coeff[4]=ALFA_IF(frequency_F1,frequency_F2);
                        fre_2_coeff[4]=BATA_IF(frequency_F1,frequency_F2);
                    }
                    break;
                default:
                    break;
                }
            }


            pos_flag=0;
            UDUC_Fixed_cascading_ambiguity(epochSatlitDataLa,X,P,fre_1_coeff,fre_2_coeff,frequency_1,frequency_2,Total_sat_fre,
                                           Par_sat_epoNL,Par_lenth,UDUC_B,pos_flag,UDUC_SD_P,UDUC_B_SDNL,UDUC_UDNL,UDUC_SD_X,trans_para);
            UDUC_SD_X = UDUC_B * X;
            UDUC_SD_P = UDUC_B * P * UDUC_B.transpose();
            /* WL ambiguity resolution */
            UDUC_parametric_solution(UDUC_SD_X,UDUC_SD_P,UDUC_X_b,UDUC_P_b,UDUC_P_ab,Trans_M,Total_sat_fre,Par_sat_epoWL,UDUC_B_SDWL);
            Par_sat_epoNL = Trans_M*Par_sat_epoNL;
            UDUC_UDNL = UDUC_UDNL*Trans_M.transpose();
            UDUC_B_SDNL = UDUC_B_SDNL*Trans_M.transpose();
            UDUC_Xa = UDUC_UDWL * UDUC_SD_X;
            UDUC_Pa = UDUC_UDWL * UDUC_SD_P *UDUC_UDWL.transpose();
            for(int i=0;i<epoLen;i++)   Par_sat_epo[i]=0;
//                MatrixXd UDUC_SD_Pa;VectorXd UDUC_SD_Xa;
            UDUC_SD_Xa=UDUC_Xa;UDUC_SD_Pa=UDUC_Pa;
            UDUC_Fixed_partial_ambiguity(epochSatlitDataLa,Par_sat_epo,UDUC_SD_Pa,UDUC_SD_Xa);
            int wide_line = 0;
            QVector< SatlitData > epochSatlitDataLa_WL;
            for(int i=0;i<epoLen;i++)
            {
                SatlitData oneSatlitData = epochSatlitDataLa.at(i);
                if(Par_sat_epo[i]==0&&oneSatlitData.PRN!=oneSatlitData.maxEA_PRN)
                {
                    oneSatlitData.SD_Wl_float = UDUC_SD_Xa(wide_line);
                    wide_line++;
                }
                epochSatlitDataLa_WL.append(oneSatlitData);
            }
            epochSatlitDataLa=epochSatlitDataLa_WL;
            if(trans_para->Sosr_flag)
            {
                Ambi_R.Standard_of_success_rate(epochSatlitDataLa,Par_sat_epo,UDUC_SD_Pa,trans_para->Ps);
                UDUC_SD_Xa=UDUC_Xa;UDUC_SD_Pa=UDUC_Pa;
                UDUC_Fixed_partial_ambiguity(epochSatlitDataLa,Par_sat_epo,UDUC_SD_Pa,UDUC_SD_Xa);
            }
            if(UDUC_LAMBDA_search(epochSatlitDataLa,UDUC_SD_Pa,UDUC_SD_Xa,Par_sat_epo,UDUC_Pa,UDUC_Xa,trans_para,opt_corr))
            {
                fix_flag =2;
//                    MatrixXd UDUC_fixed_Pa;VectorXd UDUC_fixed_Xa;VectorXd UDUC_float_Xa;
                UDUC_float_Xa = UDUC_B_SDWL * UDUC_SD_X;
                UDUC_fixed_Xa = UDUC_float_Xa;
                UDUC_fixed_Pa = UDUC_B_SDWL * UDUC_SD_P *UDUC_B_SDWL.transpose();
                ambiguity_pos = 0;fixed_ambiguity=0;VectorXd UDUC_fixed_WLa;
                UDUC_fixed_WLa.resize(epoLen);UDUC_fixed_WLa.setZero(epoLen);
                epochSatlitDataLa_WL.clear();
                for(int i=0;i<epoLen;i++)
                {
                    SatlitData oneSatlitData = epochSatlitDataLa.at(i);
                    if(oneSatlitData.PRN == oneSatlitData.maxEA_PRN)
                    {
                        epochSatlitDataLa_WL.append(oneSatlitData);
                        continue;
                    }
                    double frequency_corr = 0;
                    if(Par_sat_epo[i]==0)
                    {
                        UDUC_fixed_Xa(ambiguity_pos)=UDUC_SD_Xa(fixed_ambiguity);
                        frequency_corr=oneSatlitData.Frq[1]/(oneSatlitData.Frq[0]-oneSatlitData.Frq[1]);
                        UDUC_fixed_WLa(i)=frequency_corr*UDUC_SD_Xa(fixed_ambiguity);
                        fixed_ambiguity++;
                        oneSatlitData.SD_Wl_Int = UDUC_fixed_Xa(ambiguity_pos);
                        epochSatlitDataLa_WL.append(oneSatlitData);
                    }
                    else
                        epochSatlitDataLa_WL.append(oneSatlitData);
                    ambiguity_pos++;
                }
                epochSatlitDataLa=epochSatlitDataLa_WL;
                UDUC_SD_X = UDUC_X_b - UDUC_P_ab * UDUC_fixed_Pa.inverse() * (UDUC_float_Xa-UDUC_fixed_Xa);
                UDUC_SD_P = UDUC_P_b - UDUC_P_ab * UDUC_fixed_Pa.inverse() * UDUC_P_ab.transpose();
//                UDUC_SD_X = UDUC_X_b;
//                UDUC_SD_P = UDUC_P_b;
                /* NL ambiguity resolution */
                UDUC_parametric_solution(UDUC_SD_X,UDUC_SD_P,UDUC_X_b,UDUC_P_b,UDUC_P_ab,Trans_M,Total_sat_fre,Par_sat_epoNL,UDUC_B_SDNL);
                UDUC_Xa = UDUC_UDNL * UDUC_SD_X;
                UDUC_Pa = UDUC_UDNL * UDUC_SD_P *UDUC_UDNL.transpose();
//                    for(int i=0;i<epoLen;i++)   Par_sat_epo[i]=0;
//                MatrixXd UDUC_SD_Pa;VectorXd UDUC_SD_Xa;
                UDUC_SD_Xa=UDUC_Xa;UDUC_SD_Pa=UDUC_Pa;
                UDUC_Fixed_partial_NL_ambiguity(epochSatlitDataLa,Par_sat_epo,UDUC_SD_Pa,UDUC_SD_Xa,UDUC_fixed_WLa);
                int narrow_line = 0;
                QVector< SatlitData > epochSatlitDataLa_NL;
                for(int i=0;i<epoLen;i++)
                {
                    SatlitData oneSatlitData = epochSatlitDataLa.at(i);
                    if(Par_sat_epo[i]==0&&oneSatlitData.PRN!=oneSatlitData.maxEA_PRN)
                    {
                        oneSatlitData.SD_Nl_Float = UDUC_SD_Xa(narrow_line);
                        narrow_line++;
                    }
                    epochSatlitDataLa_NL.append(oneSatlitData);
                }
                epochSatlitDataLa=epochSatlitDataLa_NL;
                if(trans_para->Sosr_flag)
                {
                    Ambi_R.Standard_of_success_rate(epochSatlitDataLa,Par_sat_epo,UDUC_SD_Pa,trans_para->Ps);
                    UDUC_SD_Xa=UDUC_Xa;UDUC_SD_Pa=UDUC_Pa;
                    UDUC_Fixed_partial_NL_ambiguity(epochSatlitDataLa,Par_sat_epo,UDUC_SD_Pa,UDUC_SD_Xa,UDUC_fixed_WLa);
                }
                if(UDUC_LAMBDA_search_NL(epochSatlitDataLa,UDUC_SD_Pa,UDUC_SD_Xa,Par_sat_epo,UDUC_Pa,UDUC_Xa,UDUC_fixed_WLa,trans_para,opt_corr))
                {
                    fix_flag =3;
//                    MatrixXd UDUC_fixed_Pa;VectorXd UDUC_fixed_Xa;VectorXd UDUC_float_Xa;
                    UDUC_float_Xa = UDUC_B_SDNL * UDUC_SD_X ;
                    UDUC_fixed_Xa = UDUC_float_Xa;
                    UDUC_fixed_Pa = UDUC_B_SDNL * UDUC_SD_P *UDUC_B_SDNL.transpose();
                    ambiguity_pos = 0;fixed_ambiguity=0;
                    epochSatlitDataLa_NL.clear();
                    for(int i=0;i<epoLen;i++)
                    {
                        SatlitData oneSatlitData = epochSatlitDataLa.at(i);
                        if(oneSatlitData.PRN == oneSatlitData.maxEA_PRN)
                        {
                            epochSatlitDataLa_NL.append(oneSatlitData);
                            continue;
                        }
                        if(Par_sat_epo[i]==0)
                        {
                            UDUC_fixed_Xa(ambiguity_pos)=UDUC_SD_Xa(fixed_ambiguity);
                            UDUC_float_Xa(ambiguity_pos)=UDUC_float_Xa(ambiguity_pos)-UDUC_fixed_WLa(i);
                            fixed_ambiguity++;
                            oneSatlitData.SD_Nl_fix = UDUC_fixed_Xa(ambiguity_pos);
                            epochSatlitDataLa_NL.append(oneSatlitData);
                        }
                        else
                            epochSatlitDataLa_NL.append(oneSatlitData);
                        ambiguity_pos++;
                    }
                    epochSatlitDataLa=epochSatlitDataLa_NL;
                    UDUC_SD_X = UDUC_X_b - UDUC_P_ab * UDUC_fixed_Pa.inverse() * (UDUC_float_Xa-UDUC_fixed_Xa);
                    UDUC_SD_P = UDUC_P_b - UDUC_P_ab * UDUC_fixed_Pa.inverse() * UDUC_P_ab.transpose();
                }
            }
        }
    }
}
bool uduc_ambiguity_resolution::UDUC_LAMBDA_search(QVector< SatlitData > &epochSatlitDataLa,MatrixXd &UDUC_SD_P,VectorXd &UDUC_SD_X,int *Par_sat_epo,
                                                   MatrixXd UDUC_SD_Pa,VectorXd UDUC_SD_Xa,para_trans *trans_para,const corr_opt *opt_corr)
{
    int satellite_len = epochSatlitDataLa.length();
    int par_number=UDUC_SD_P.cols();
    int info;
    int min_par_num;
    if(opt_corr->GNSS_AR_Select)
        min_par_num = 2;
    else
        min_par_num = 4;
    trans_para->fixed_sat = 0;
    trans_para->lambda_radio = 0;
    trans_para->EA_minsat = true;
    while(par_number>min_par_num&&!trans_para->ambiguity_fix)
    {
        MatrixXd amb_fix;double ratio = trans_para->ratio_value;int out_fixamb_num = 2;
        info = m_lambda.QLamdaSearch(UDUC_SD_X,UDUC_SD_P,amb_fix, &ratio,out_fixamb_num);
        if(info)    return false;
        trans_para->lambda_radio = ratio;
        if(ratio>trans_para->ratio_value)
        {
            trans_para->fixed_sat = UDUC_SD_P.rows();
            UDUC_SD_X = amb_fix.topLeftCorner(UDUC_SD_P.rows(), 1);
            return true;
        }
        else
        {
            trans_para->fixed_sat =0;
            Ambi_R.delet_bias_SSD_nl(epochSatlitDataLa,Par_sat_epo,UDUC_SD_X,trans_para->bias_nl);
            if(trans_para->Partial_subset==angle_screen)
            {
                Ambi_R.angle_altitude_screen(epochSatlitDataLa,Par_sat_epo);
            }
            else if(trans_para->Partial_subset==stand_post_res)
                Ambi_R.unfixed_sat(satellite_len,Par_sat_epo,trans_para);
            else if(trans_para->Partial_subset==post_resudual)
            {
                Ambi_R.post_resuduala(satellite_len,Par_sat_epo,trans_para);
            }
            else if(trans_para->Partial_subset==Adop_demet_secreen)
                Ambi_R.Adop_screen(epochSatlitDataLa,Par_sat_epo,UDUC_SD_P);
            else if(trans_para->Partial_subset==Boost_Par)
                Ambi_R.Boost_Parsub(epochSatlitDataLa,Par_sat_epo,UDUC_SD_P);
            else if(trans_para->Partial_subset==Diagonal_var)
                Ambi_R.Diagonal_variance(epochSatlitDataLa,Par_sat_epo,UDUC_SD_P);
            else if(trans_para->Partial_subset==full_ambiguity_fixed)
                break;
            UDUC_SD_X=UDUC_SD_Xa;UDUC_SD_P=UDUC_SD_Pa;
            UDUC_Fixed_partial_ambiguity(epochSatlitDataLa,Par_sat_epo,UDUC_SD_P,UDUC_SD_X);
            par_number=UDUC_SD_P.cols();
        }
    }
    return false;
}
bool uduc_ambiguity_resolution::UDUC_LAMBDA_search_NL(QVector< SatlitData > &epochSatlitDataLa,MatrixXd &UDUC_SD_P,VectorXd &UDUC_SD_X,int *Par_sat_epo,
                                                   MatrixXd UDUC_SD_Pa,VectorXd UDUC_SD_Xa,VectorXd UDUC_SD_WLa,para_trans *trans_para,const corr_opt *opt_corr)
{
    int satellite_len = epochSatlitDataLa.length();
    int par_number=UDUC_SD_P.cols();
    int info;
    int min_par_num;
    if(opt_corr->GNSS_AR_Select)
        min_par_num = 2;
    else
        min_par_num = 4;
    trans_para->fixed_sat = 0;
    trans_para->lambda_radio = 0;
    trans_para->EA_minsat = true;
    while(par_number>=min_par_num&&!trans_para->ambiguity_fix)
    {
        MatrixXd amb_fix;double ratio = trans_para->ratio_value;int out_fixamb_num = 2;
        info = m_lambda.QLamdaSearch(UDUC_SD_X,UDUC_SD_P,amb_fix, &ratio,out_fixamb_num);
        if(info)    return false;
        trans_para->lambda_radio = ratio;
        if(ratio>trans_para->ratio_value)
        {
            trans_para->fixed_sat = UDUC_SD_P.rows();
            UDUC_SD_X = amb_fix.topLeftCorner(UDUC_SD_P.rows(), 1);
            return true;
        }
        else
        {
            trans_para->fixed_sat =0;
            Ambi_R.delet_bias_SSD_nl(epochSatlitDataLa,Par_sat_epo,UDUC_SD_X,trans_para->bias_nl);
            if(trans_para->Partial_subset==angle_screen)
            {
                Ambi_R.angle_altitude_screen(epochSatlitDataLa,Par_sat_epo);
            }
            else if(trans_para->Partial_subset==stand_post_res)
                Ambi_R.unfixed_sat(satellite_len,Par_sat_epo,trans_para);
            else if(trans_para->Partial_subset==post_resudual)
            {
                Ambi_R.post_resuduala(satellite_len,Par_sat_epo,trans_para);
            }
            else if(trans_para->Partial_subset==Adop_demet_secreen)
                Ambi_R.Adop_screen(epochSatlitDataLa,Par_sat_epo,UDUC_SD_P);
            else if(trans_para->Partial_subset==Boost_Par)
                Ambi_R.Boost_Parsub(epochSatlitDataLa,Par_sat_epo,UDUC_SD_P);
            else if(trans_para->Partial_subset==Diagonal_var)
                Ambi_R.Diagonal_variance(epochSatlitDataLa,Par_sat_epo,UDUC_SD_P);
            else if(trans_para->Partial_subset==full_ambiguity_fixed)
                break;
            UDUC_SD_X=UDUC_SD_Xa;UDUC_SD_P=UDUC_SD_Pa;
            UDUC_Fixed_partial_NL_ambiguity(epochSatlitDataLa,Par_sat_epo,UDUC_SD_P,UDUC_SD_X,UDUC_SD_WLa);
            par_number=UDUC_SD_P.cols();
        }
    }
    return false;
}
void uduc_ambiguity_resolution::UDUC_HMW_Fixed_cascading_ambiguity(QVector< SatlitData > &epochSatlitDatanl,
                                                  int *frequency_flag,int Total_sat_fre,VectorXd &Par_sat_epo,int &Par_lenth,
                                                  MatrixXd &UDUC_B_SD,MatrixXd &UDUC_UDM,para_trans *trans_para)
{
    int epoLen = epochSatlitDatanl.length();
    VectorXd Trans_G,Trans_C2,Trans_C3,Trans_E,Trans_R;
    UDUC_B_SD.resize(epoLen,trans_para->const_parm+epoLen+Total_sat_fre);
    UDUC_B_SD.setZero(epoLen,trans_para->const_parm+epoLen+Total_sat_fre);
    Trans_G.resize(epoLen);Trans_C2.resize(epoLen);Trans_C3.resize(epoLen);
    Trans_E.resize(epoLen);Trans_R.resize(epoLen);
    Trans_G.setZero(epoLen);Trans_C2.setZero(epoLen);Trans_C3.setZero(epoLen);
    Trans_E.setZero(epoLen);Trans_R.setZero(epoLen);
    int pos_Trans_G=0,pos_Trans_C2=0,pos_Trans_C3=0,pos_Trans_E=0,pos_Trans_R=0;
    int pos_Trans_G_=0,pos_Trans_C2_=0,pos_Trans_C3_=0,pos_Trans_E_=0,pos_Trans_R_=0;
    int sysTemflag[5];int pos_ambiguity=trans_para->const_parm+epoLen;
    for(int i=0;i<5;i++)    sysTemflag[i]=0;
    for (int epo=0;epo<epoLen;epo++)
    {
        SatlitData oneSatlitData = epochSatlitDatanl.at(epo);
        switch (oneSatlitData.SatType) {
        case 'G':
            Trans_G(epo)=-1;
            if(oneSatlitData.PRN == oneSatlitData.maxEA_PRN)
            {
                sysTemflag[0]=1;
                for(int mufre=0;mufre<5;mufre++)
                {
                    if(oneSatlitData.ObsFreflag[mufre]!=1)  continue;
                    if(frequency_flag[0]==mufre)
                    {
                        pos_Trans_G = pos_ambiguity;
                        pos_Trans_G_ = epo;
                        UDUC_B_SD(epo,pos_ambiguity) = 1;
                        Par_sat_epo(pos_ambiguity) = pos_ambiguity;
                        Par_lenth++;
                    }
                    pos_ambiguity++;
                }
            }
            else
            {
                for(int mufre=0;mufre<5;mufre++)
                {
                    if(oneSatlitData.ObsFreflag[mufre]!=1)  continue;
                    if(frequency_flag[0]==mufre)
                    {
                        UDUC_B_SD(epo,pos_ambiguity) = 1;
                        Par_sat_epo(pos_ambiguity) = pos_ambiguity;
                        Par_lenth++;
                    }
                    pos_ambiguity++;
                }
            }
            break;
        case 'C':
            if(oneSatlitData.PRN<=18)
            {
                Trans_C2(epo)=-1;
                if(oneSatlitData.PRN == oneSatlitData.maxEA_PRN)
                {
                    sysTemflag[1]=2;
                    for(int mufre=0;mufre<5;mufre++)
                    {
                        if(oneSatlitData.ObsFreflag[mufre]!=1)  continue;
                        if(frequency_flag[1]==mufre)
                        {
                            pos_Trans_C2 = pos_ambiguity;
                            pos_Trans_C2_ = epo;
                            UDUC_B_SD(epo,pos_ambiguity) = 1;
                            Par_sat_epo(pos_ambiguity) = pos_ambiguity;
                            Par_lenth++;
                        }
                        pos_ambiguity++;
                    }
                }
                else
                {
                    for(int mufre=0;mufre<5;mufre++)
                    {
                        if(oneSatlitData.ObsFreflag[mufre]!=1)  continue;
                        if(frequency_flag[1]==mufre)
                        {
                            UDUC_B_SD(epo,pos_ambiguity) = 1;
                            Par_sat_epo(pos_ambiguity) = pos_ambiguity;
                            Par_lenth++;
                        }
                        pos_ambiguity++;
                    }
                }
            }
            else
            {
                Trans_C3(epo)=-1;
                if(oneSatlitData.PRN == oneSatlitData.maxEA_PRN)
                {
                    sysTemflag[2]=3;
                    for(int mufre=0;mufre<5;mufre++)
                    {
                        if(oneSatlitData.ObsFreflag[mufre]!=1)  continue;
                        if(frequency_flag[2]==mufre)
                        {
                            pos_Trans_C3 = pos_ambiguity;
                            pos_Trans_C3_ = epo;
                            UDUC_B_SD(epo,pos_ambiguity) = 1;
                            Par_sat_epo(pos_ambiguity) = pos_ambiguity;
                            Par_lenth++;
                        }
                        pos_ambiguity++;
                    }
                }
                else
                {
                    for(int mufre=0;mufre<5;mufre++)
                    {
                        if(oneSatlitData.ObsFreflag[mufre]!=1)  continue;
                        if(frequency_flag[2]==mufre)
                        {
                            UDUC_B_SD(epo,pos_ambiguity) = 1;
                            Par_sat_epo(pos_ambiguity) = pos_ambiguity;
                            Par_lenth++;
                        }
                        pos_ambiguity++;
                    }
                }
            }
            break;
        case 'R':
            Trans_R(epo)=-1;
            if(oneSatlitData.PRN == oneSatlitData.maxEA_PRN)
            {
                sysTemflag[3]=4;
                for(int mufre=0;mufre<5;mufre++)
                {
                    if(oneSatlitData.ObsFreflag[mufre]!=1)  continue;
                    if(frequency_flag[3]==mufre)
                    {
                        pos_Trans_R = pos_ambiguity;
                        pos_Trans_R_ = epo;
                        UDUC_B_SD(epo,pos_ambiguity) = 1;
                        Par_sat_epo(pos_ambiguity) = pos_ambiguity;
                        Par_lenth++;
                    }
                    pos_ambiguity++;
                }
            }
            else
            {
                for(int mufre=0;mufre<5;mufre++)
                {
                    if(oneSatlitData.ObsFreflag[mufre]!=1)  continue;
                    if(frequency_flag[3]==mufre)
                    {
                        UDUC_B_SD(epo,pos_ambiguity) = 1;
                        Par_sat_epo(pos_ambiguity) = pos_ambiguity;
                        Par_lenth++;
                    }
                    pos_ambiguity++;
                }
            }
            break;
        case 'E':
            Trans_E(epo)=-1;
            if(oneSatlitData.PRN == oneSatlitData.maxEA_PRN)
            {
                sysTemflag[4]=5;
                for(int mufre=0;mufre<5;mufre++)
                {
                    if(oneSatlitData.ObsFreflag[mufre]!=1)  continue;
                    if(frequency_flag[4]==mufre)
                    {
                        pos_Trans_E = pos_ambiguity;
                        pos_Trans_E_ = epo;
                        UDUC_B_SD(epo,pos_ambiguity) = 1;
                        Par_sat_epo(pos_ambiguity) = pos_ambiguity;
                        Par_lenth++;
                    }
                    pos_ambiguity++;
                }
            }
            else
            {
                for(int mufre=0;mufre<5;mufre++)
                {
                    if(oneSatlitData.ObsFreflag[mufre]!=1)  continue;
                    if(frequency_flag[4]==mufre)
                    {
                        UDUC_B_SD(epo,pos_ambiguity) = 1;
                        Par_sat_epo(pos_ambiguity) = pos_ambiguity;
                        Par_lenth++;
                    }
                    pos_ambiguity++;
                }
            }
            break;
        default:
            break;
        }
    }
    UDUC_UDM = UDUC_B_SD;

    int sortMaxEpo[5];  for(int i =0;i<5;i++)   sortMaxEpo[i]=-1;
    for(int i =0;i<5;i++)
    {
        switch (sysTemflag[i]) {
        case 1:
            sortMaxEpo[0] = pos_Trans_G_;
            UDUC_B_SD.block(0,pos_Trans_G, epoLen,1) = Trans_G;
            break;
        case 2:
            sortMaxEpo[1] = pos_Trans_C2_;
            UDUC_B_SD.block(0,pos_Trans_C2, epoLen,1) = Trans_C2;
            break;
        case 3:
            sortMaxEpo[2] = pos_Trans_C3_;
            UDUC_B_SD.block(0,pos_Trans_C3, epoLen,1) = Trans_C3;
            break;
        case 4:
            sortMaxEpo[3] = pos_Trans_R_;
            UDUC_B_SD.block(0,pos_Trans_R, epoLen,1) = Trans_R;
            break;
        case 5:
            sortMaxEpo[4] = pos_Trans_E_;
            UDUC_B_SD.block(0,pos_Trans_E, epoLen,1) = Trans_E;
            break;
        default:
            break;
        }
    }
    for(int i=0;i<5;i++) {
        for(int j=i+1;j<5;j++) {
            if(sortMaxEpo[i]<sortMaxEpo[j]) {
                int temp=sortMaxEpo[i],temp1 = sysTemflag[i];
                sortMaxEpo[i]=sortMaxEpo[j];
                sortMaxEpo[j]=temp;
                sysTemflag[i]=sysTemflag[j];//max epo sort
                sysTemflag[j]=temp1;
            }
        }
    }
    for(int i =0;i<5;i++)
    {
        switch (sysTemflag[i]) {
        case 1:
            m_mat_cal.RemoveRow(UDUC_B_SD,pos_Trans_G_);
            break;
        case 2:
            m_mat_cal.RemoveRow(UDUC_B_SD,pos_Trans_C2_);
            break;
        case 3:
            m_mat_cal.RemoveRow(UDUC_B_SD,pos_Trans_C3_);
            break;
        case 4:
            m_mat_cal.RemoveRow(UDUC_B_SD,pos_Trans_R_);
            break;
        case 5:
            m_mat_cal.RemoveRow(UDUC_B_SD,pos_Trans_E_);
            break;
        default:
            break;
        }
    }
    m_mat_cal.Remove_allZeroRow(UDUC_B_SD);
}
void uduc_ambiguity_resolution::UDUC_Fixed_cascading_ambiguity(QVector< SatlitData > &epochSatlitDatanl,VectorXd &X,MatrixXd &P,
                                                  double *fre_1_coeff,double *fre_2_coeff,int *frequency_1,int *frequency_2,int Total_sat_fre,
                                                               VectorXd &Par_sat_epo,int &Par_lenth,MatrixXd &UDUC_B,int pos_flag,
                                                  MatrixXd &UDUC_SD_P,MatrixXd &UDUC_B_SD,MatrixXd &UDUC_UDM,VectorXd &UDUC_SD_X,para_trans *trans_para)
{//
    int epoLen = epochSatlitDatanl.length();
    VectorXd Trans_G,Trans_C2,Trans_C3,Trans_E,Trans_R;
    UDUC_B_SD.resize(epoLen,trans_para->const_parm+epoLen+Total_sat_fre);
    UDUC_B_SD.setZero(epoLen,trans_para->const_parm+epoLen+Total_sat_fre);
    Trans_G.resize(epoLen);Trans_C2.resize(epoLen);Trans_C3.resize(epoLen);
    Trans_E.resize(epoLen);Trans_R.resize(epoLen);
    Trans_G.setZero(epoLen);Trans_C2.setZero(epoLen);Trans_C3.setZero(epoLen);
    Trans_E.setZero(epoLen);Trans_R.setZero(epoLen);
    int pos_Trans_G=0,pos_Trans_C2=0,pos_Trans_C3=0,pos_Trans_E=0,pos_Trans_R=0;
    int pos_Trans_G_=0,pos_Trans_C2_=0,pos_Trans_C3_=0,pos_Trans_E_=0,pos_Trans_R_=0;
    int sysTemflag[5];int pos_ambiguity=trans_para->const_parm+epoLen;
    for(int i=0;i<5;i++)    sysTemflag[i]=0;
    for (int epo=0;epo<epoLen;epo++)
    {
        SatlitData oneSatlitData = epochSatlitDatanl.at(epo);
        int frequency_1pos=0;
        switch (oneSatlitData.SatType) {
        case 'G':
            Trans_G(epo)=-1;
            if(oneSatlitData.PRN == oneSatlitData.maxEA_PRN)
            {
                sysTemflag[0]=1;
                frequency_1pos=-1;
                for(int mufre=0;mufre<5;mufre++)
                {
                    if(oneSatlitData.ObsFreflag[mufre]!=1)  continue;
                    if(frequency_1[0]==mufre)
                    {
                        frequency_1pos = pos_ambiguity;
                    }
                    if(frequency_2[0]==mufre&&frequency_1pos!=-1)
                    {
                        if(pos_flag==0)
                        {
                            UDUC_B(frequency_1pos,frequency_1pos) = fre_1_coeff[0];
                            UDUC_B(frequency_1pos,pos_ambiguity) = fre_2_coeff[0];
                            pos_Trans_G = frequency_1pos;
                            pos_Trans_G_ = epo;
                            UDUC_B_SD(epo,frequency_1pos) = 1;
                            Par_sat_epo(frequency_1pos) = frequency_1pos;
                            Par_lenth++;
                        }
                        else
                        {
                            UDUC_B(pos_ambiguity,frequency_1pos) = fre_1_coeff[0];
                            UDUC_B(pos_ambiguity,pos_ambiguity) = fre_2_coeff[0];
                            pos_Trans_G = pos_ambiguity;
                            pos_Trans_G_ = epo;
                            UDUC_B_SD(epo,pos_ambiguity) = 1;
                            Par_sat_epo(pos_ambiguity) = pos_ambiguity;
                            Par_lenth++;
                        }
                    }
                    pos_ambiguity++;
                }
            }
            else
            {
                frequency_1pos=-1;
                for(int mufre=0;mufre<5;mufre++)
                {
                    if(oneSatlitData.ObsFreflag[mufre]!=1)  continue;
                    if(frequency_1[0]==mufre)
                    {
                        frequency_1pos = pos_ambiguity;
                    }
                    if(frequency_2[0]==mufre&&frequency_1pos!=-1)
                    {
                        if(pos_flag==0)
                        {
                            UDUC_B(frequency_1pos,frequency_1pos) = fre_1_coeff[0];
                            UDUC_B(frequency_1pos,pos_ambiguity) = fre_2_coeff[0];
                            UDUC_B_SD(epo,frequency_1pos) = 1;
                            Par_sat_epo(frequency_1pos) = frequency_1pos;
                            Par_lenth++;
                        }
                        else
                        {
                            UDUC_B(pos_ambiguity,frequency_1pos) = fre_1_coeff[0];
                            UDUC_B(pos_ambiguity,pos_ambiguity) = fre_2_coeff[0];
                            UDUC_B_SD(epo,pos_ambiguity) = 1;
                            Par_sat_epo(pos_ambiguity) = pos_ambiguity;
                            Par_lenth++;
                        }
                    }
                    pos_ambiguity++;
                }
            }
            break;
        case 'C':
            if(oneSatlitData.PRN<=18)
            {
                Trans_C2(epo)=-1;
                if(oneSatlitData.PRN == oneSatlitData.maxEA_PRN)
                {
                    sysTemflag[1]=2;
                    frequency_1pos=-1;
                    for(int mufre=0;mufre<5;mufre++)
                    {
                        if(oneSatlitData.ObsFreflag[mufre]!=1)  continue;
                        if(frequency_1[1]==mufre)
                        {
                            frequency_1pos = pos_ambiguity;
                        }
                        if(frequency_2[1]==mufre&&frequency_1pos!=-1)
                        {
                            if(pos_flag==0)
                            {
                                UDUC_B(frequency_1pos,frequency_1pos) = fre_1_coeff[1];
                                UDUC_B(frequency_1pos,pos_ambiguity) = fre_2_coeff[1];
                                pos_Trans_C2 = frequency_1pos;
                                pos_Trans_C2_ = epo;
                                UDUC_B_SD(epo,frequency_1pos) = 1;
                                Par_sat_epo(frequency_1pos) = frequency_1pos;
                                Par_lenth++;
                            }
                            else
                            {
                                UDUC_B(pos_ambiguity,frequency_1pos) = fre_1_coeff[1];
                                UDUC_B(pos_ambiguity,pos_ambiguity) = fre_2_coeff[1];
                                pos_Trans_C2 = pos_ambiguity;
                                pos_Trans_C2_ = epo;
                                UDUC_B_SD(epo,pos_ambiguity) = 1;
                                Par_sat_epo(pos_ambiguity) = pos_ambiguity;
                                Par_lenth++;
                            }

                        }
                        pos_ambiguity++;
                    }
                }
                else
                {
                    frequency_1pos=-1;
                    for(int mufre=0;mufre<5;mufre++)
                    {
                        if(oneSatlitData.ObsFreflag[mufre]!=1)  continue;
                        if(frequency_1[1]==mufre)
                        {
                            frequency_1pos = pos_ambiguity;
                        }
                        if(frequency_2[1]==mufre&&frequency_1pos!=-1)
                        {
                            if(pos_flag==0)
                            {
                                UDUC_B(frequency_1pos,frequency_1pos) = fre_1_coeff[1];
                                UDUC_B(frequency_1pos,pos_ambiguity) = fre_2_coeff[1];
                                UDUC_B_SD(epo,frequency_1pos) = 1;
                                Par_sat_epo(frequency_1pos) = frequency_1pos;
                                Par_lenth++;
                            }
                            else
                            {
                                UDUC_B(pos_ambiguity,frequency_1pos) = fre_1_coeff[1];
                                UDUC_B(pos_ambiguity,pos_ambiguity) = fre_2_coeff[1];
                                UDUC_B_SD(epo,pos_ambiguity) = 1;
                                Par_sat_epo(pos_ambiguity) = pos_ambiguity;
                                Par_lenth++;
                            }

                        }
                        pos_ambiguity++;
                    }
                }
            }
            else
            {
                Trans_C3(epo)=-1;
                if(oneSatlitData.PRN == oneSatlitData.maxEA_PRN)
                {
                    sysTemflag[2]=3;
                    frequency_1pos=-1;
                    for(int mufre=0;mufre<5;mufre++)
                    {
                        if(oneSatlitData.ObsFreflag[mufre]!=1)  continue;
                        if(frequency_1[2]==mufre)
                        {
                            frequency_1pos = pos_ambiguity;
                        }
                        if(frequency_2[2]==mufre&&frequency_1pos!=-1)
                        {
                            if(pos_flag==0)
                            {
                                UDUC_B(frequency_1pos,frequency_1pos) = fre_1_coeff[2];
                                UDUC_B(frequency_1pos,pos_ambiguity) = fre_2_coeff[2];
                                pos_Trans_C3 = frequency_1pos;
                                pos_Trans_C3_ = epo;
                                UDUC_B_SD(epo,frequency_1pos) = 1;
                                Par_sat_epo(frequency_1pos) = frequency_1pos;
                                Par_lenth++;
                            }
                            else
                            {
                                UDUC_B(pos_ambiguity,frequency_1pos) = fre_1_coeff[2];
                                UDUC_B(pos_ambiguity,pos_ambiguity) = fre_2_coeff[2];
                                pos_Trans_C3 = pos_ambiguity;
                                pos_Trans_C3_ = epo;
                                UDUC_B_SD(epo,pos_ambiguity) = 1;
                                Par_sat_epo(pos_ambiguity) = pos_ambiguity;
                                Par_lenth++;
                            }

                        }
                        pos_ambiguity++;
                    }
                }
                else
                {
                    frequency_1pos=-1;
                    for(int mufre=0;mufre<5;mufre++)
                    {
                        if(oneSatlitData.ObsFreflag[mufre]!=1)  continue;
                        if(frequency_1[2]==mufre)
                        {
                            frequency_1pos = pos_ambiguity;
                        }
                        if(frequency_2[2]==mufre&&frequency_1pos!=-1)
                        {
                            if(pos_flag==0)
                            {
                                UDUC_B(frequency_1pos,frequency_1pos) = fre_1_coeff[2];
                                UDUC_B(frequency_1pos,pos_ambiguity) = fre_2_coeff[2];
                                UDUC_B_SD(epo,frequency_1pos) = 1;
                                Par_sat_epo(frequency_1pos) = frequency_1pos;
                                Par_lenth++;
                            }
                            else
                            {
                                UDUC_B(pos_ambiguity,frequency_1pos) = fre_1_coeff[2];
                                UDUC_B(pos_ambiguity,pos_ambiguity) = fre_2_coeff[2];
                                UDUC_B_SD(epo,pos_ambiguity) = 1;
                                Par_sat_epo(pos_ambiguity) = pos_ambiguity;
                                Par_lenth++;
                            }

                        }
                        pos_ambiguity++;
                    }
                }
            }
            break;
        case 'R':
            Trans_R(epo)=-1;
            if(oneSatlitData.PRN == oneSatlitData.maxEA_PRN)
            {
                sysTemflag[3]=4;
                frequency_1pos=-1;
                for(int mufre=0;mufre<5;mufre++)
                {
                    if(oneSatlitData.ObsFreflag[mufre]!=1)  continue;
                    if(frequency_1[3]==mufre)
                    {
                        frequency_1pos = pos_ambiguity;
                    }
                    if(frequency_2[3]==mufre&&frequency_1pos!=-1)
                    {
                        if(pos_flag==0)
                        {
                            UDUC_B(frequency_1pos,frequency_1pos) = fre_1_coeff[3];
                            UDUC_B(frequency_1pos,pos_ambiguity) = fre_2_coeff[3];
                            pos_Trans_R = frequency_1pos;
                            pos_Trans_R_ = epo;
                            UDUC_B_SD(epo,frequency_1pos) = 1;
                            Par_sat_epo(frequency_1pos) = frequency_1pos;
                            Par_lenth++;
                        }
                        else
                        {
                            UDUC_B(pos_ambiguity,frequency_1pos) = fre_1_coeff[3];
                            UDUC_B(pos_ambiguity,pos_ambiguity) = fre_2_coeff[3];
                            pos_Trans_R = pos_ambiguity;
                            pos_Trans_R_ = epo;
                            UDUC_B_SD(epo,pos_ambiguity) = 1;
                            Par_sat_epo(pos_ambiguity) = pos_ambiguity;
                            Par_lenth++;
                        }

                    }
                    pos_ambiguity++;
                }
            }
            else
            {
                frequency_1pos=-1;
                for(int mufre=0;mufre<5;mufre++)
                {
                    if(oneSatlitData.ObsFreflag[mufre]!=1)  continue;
                    if(frequency_1[3]==mufre)
                    {
                        frequency_1pos = pos_ambiguity;
                    }
                    if(frequency_2[3]==mufre&&frequency_1pos!=-1)
                    {
                        if(pos_flag==0)
                        {
                            UDUC_B(frequency_1pos,frequency_1pos) = fre_1_coeff[3];
                            UDUC_B(frequency_1pos,pos_ambiguity) = fre_2_coeff[3];
                            UDUC_B_SD(epo,frequency_1pos) = 1;
                            Par_sat_epo(frequency_1pos) = frequency_1pos;
                            Par_lenth++;
                        }
                        else
                        {
                            UDUC_B(pos_ambiguity,frequency_1pos) = fre_1_coeff[3];
                            UDUC_B(pos_ambiguity,pos_ambiguity) = fre_2_coeff[3];
                            UDUC_B_SD(epo,pos_ambiguity) = 1;
                            Par_sat_epo(pos_ambiguity) = pos_ambiguity;
                            Par_lenth++;
                        }

                    }
                    pos_ambiguity++;
                }
            }
            break;
        case 'E':
            Trans_E(epo)=-1;
            if(oneSatlitData.PRN == oneSatlitData.maxEA_PRN)
            {
                sysTemflag[4]=5;
                frequency_1pos=-1;
                for(int mufre=0;mufre<5;mufre++)
                {
                    if(oneSatlitData.ObsFreflag[mufre]!=1)  continue;
                    if(frequency_1[4]==mufre)
                    {
                        frequency_1pos = pos_ambiguity;
                    }
                    if(frequency_2[4]==mufre&&frequency_1pos!=-1)
                    {
                        if(pos_flag==0)
                        {
                            UDUC_B(frequency_1pos,frequency_1pos) = fre_1_coeff[4];
                            UDUC_B(frequency_1pos,pos_ambiguity) = fre_2_coeff[4];
                            pos_Trans_E = frequency_1pos;
                            pos_Trans_E_ = epo;
                            UDUC_B_SD(epo,frequency_1pos) = 1;
                            Par_sat_epo(frequency_1pos) = frequency_1pos;
                            Par_lenth++;
                        }
                        else
                        {
                            UDUC_B(pos_ambiguity,frequency_1pos) = fre_1_coeff[4];
                            UDUC_B(pos_ambiguity,pos_ambiguity) = fre_2_coeff[4];
                            pos_Trans_E = pos_ambiguity;
                            pos_Trans_E_ = epo;
                            UDUC_B_SD(epo,pos_ambiguity) = 1;
                            Par_sat_epo(pos_ambiguity) = pos_ambiguity;
                            Par_lenth++;
                        }

                    }
                    pos_ambiguity++;
                }
            }
            else
            {
                frequency_1pos=-1;
                for(int mufre=0;mufre<5;mufre++)
                {
                    if(oneSatlitData.ObsFreflag[mufre]!=1)  continue;
                    if(frequency_1[4]==mufre)
                    {
                        frequency_1pos = pos_ambiguity;
                    }
                    if(frequency_2[4]==mufre&&frequency_1pos!=-1)
                    {
                        if(pos_flag==0)
                        {
                            UDUC_B(frequency_1pos,frequency_1pos) = fre_1_coeff[4];
                            UDUC_B(frequency_1pos,pos_ambiguity) = fre_2_coeff[4];
                            UDUC_B_SD(epo,frequency_1pos) = 1;
                            Par_sat_epo(frequency_1pos) = frequency_1pos;
                            Par_lenth++;
                        }
                        else
                        {
                            UDUC_B(pos_ambiguity,frequency_1pos) = fre_1_coeff[4];
                            UDUC_B(pos_ambiguity,pos_ambiguity) = fre_2_coeff[4];
                            UDUC_B_SD(epo,pos_ambiguity) = 1;
                            Par_sat_epo(pos_ambiguity) = pos_ambiguity;
                            Par_lenth++;
                        }

                    }
                    pos_ambiguity++;
                }
            }
            break;
        default:
            break;
        }
    }
    UDUC_UDM = UDUC_B_SD;
    UDUC_SD_X = UDUC_B * X;
    UDUC_SD_P = UDUC_B * P * UDUC_B.transpose();

    int sortMaxEpo[5];  for(int i =0;i<5;i++)   sortMaxEpo[i]=-1;
    for(int i =0;i<5;i++)
    {
        switch (sysTemflag[i]) {
        case 1:
            sortMaxEpo[0] = pos_Trans_G_;
            UDUC_B_SD.block(0,pos_Trans_G, epoLen,1) = Trans_G;
            break;
        case 2:
            sortMaxEpo[1] = pos_Trans_C2_;
            UDUC_B_SD.block(0,pos_Trans_C2, epoLen,1) = Trans_C2;
            break;
        case 3:
            sortMaxEpo[2] = pos_Trans_C3_;
            UDUC_B_SD.block(0,pos_Trans_C3, epoLen,1) = Trans_C3;
            break;
        case 4:
            sortMaxEpo[3] = pos_Trans_R_;
            UDUC_B_SD.block(0,pos_Trans_R, epoLen,1) = Trans_R;
            break;
        case 5:
            sortMaxEpo[4] = pos_Trans_E_;
            UDUC_B_SD.block(0,pos_Trans_E, epoLen,1) = Trans_E;
            break;
        default:
            break;
        }
    }
    for(int i=0;i<5;i++) {
        for(int j=i+1;j<5;j++) {
            if(sortMaxEpo[i]<sortMaxEpo[j]) {
                int temp=sortMaxEpo[i],temp1 = sysTemflag[i];
                sortMaxEpo[i]=sortMaxEpo[j];
                sortMaxEpo[j]=temp;
                sysTemflag[i]=sysTemflag[j];//max epo sort
                sysTemflag[j]=temp1;
            }
        }
    }
    for(int i =0;i<5;i++)
    {
        switch (sysTemflag[i]) {
        case 1:
            m_mat_cal.RemoveRow(UDUC_B_SD,pos_Trans_G_);
            break;
        case 2:
            m_mat_cal.RemoveRow(UDUC_B_SD,pos_Trans_C2_);
            break;
        case 3:
            m_mat_cal.RemoveRow(UDUC_B_SD,pos_Trans_C3_);
            break;
        case 4:
            m_mat_cal.RemoveRow(UDUC_B_SD,pos_Trans_R_);
            break;
        case 5:
            m_mat_cal.RemoveRow(UDUC_B_SD,pos_Trans_E_);
            break;
        default:
            break;
        }
    }
    m_mat_cal.Remove_allZeroRow(UDUC_B_SD);
}
void uduc_ambiguity_resolution::UDUC_parametric_solution(VectorXd &UDUC_X,MatrixXd &UDUC_P,
                                                         VectorXd &UDUC_X_b,MatrixXd &UDUC_P_b,MatrixXd &UDUC_P_ab,MatrixXd &Trans_M,
                                                  int Total_sat_fre,VectorXd &Par_sat_epo,MatrixXd &UDUC_B_SD)
{//
    int remoN=0;int remove_satA[Total_sat_fre];
    for(int i=0;i<Total_sat_fre;i++)
    {
        remove_satA[i] = 0;
    }
    MatrixXd Trans_MB;int matrix_len = UDUC_P.cols();
    Trans_M.resize(matrix_len,matrix_len);
    Trans_M.setIdentity(matrix_len,matrix_len);
    Trans_MB = Trans_M;
    for (int epo=0;epo<matrix_len;epo++)
    {
        if(Par_sat_epo(epo) != 0)
        {
            remove_satA[remoN] = epo;
            remoN++;
        }
    }

    Ambi_R.descending_order(remoN,remove_satA);
    for(int i =0;i<remoN;i++)
    {
        m_mat_cal.RemoveRow(Trans_M,remove_satA[i]);
    }
    UDUC_X_b = Trans_M*UDUC_X;
    UDUC_P_b = Trans_M*UDUC_P;
    UDUC_P_ab = UDUC_P_b;
    UDUC_P_b = UDUC_P_b *Trans_M.transpose();
    UDUC_P_ab=UDUC_P_ab*UDUC_B_SD.transpose();
}
void uduc_ambiguity_resolution::UDUC_Fixed_partial_ambiguity(QVector< SatlitData > &epochSatlitDatanl,
                                                  int *Par_sat_epo,MatrixXd &UDUC_SD_P,VectorXd &UDUC_SD_X)
{//
    int epoLen = epochSatlitDatanl.length();
    for(int i=0;i<10;i++)
    {
        m_SYStem_satN[i] = 0;
    }
    MatrixXd UDUC_B;
    VectorXd Trans_G,Trans_C2,Trans_C3,Trans_E,Trans_R;
    UDUC_B.resize(epoLen,epoLen);
    UDUC_B.setIdentity(epoLen,epoLen);
    Trans_G.resize(epoLen);Trans_C2.resize(epoLen);Trans_C3.resize(epoLen);
    Trans_E.resize(epoLen);Trans_R.resize(epoLen);
    Trans_G.setZero(epoLen);Trans_C2.setZero(epoLen);Trans_C3.setZero(epoLen);
    Trans_E.setZero(epoLen);Trans_R.setZero(epoLen);
    int pos_Trans_G=0,pos_Trans_C2=0,pos_Trans_C3=0,pos_Trans_E=0,pos_Trans_R=0;
    int remoN=0;int remove_satA[epoLen];int sysTemflag[5];
    for(int i=0;i<5;i++)    sysTemflag[i]=0;
    for(int i=0;i<epoLen;i++) remove_satA[i] = 0;
    for (int epo=0;epo<epoLen;epo++)
    {
        SatlitData oneSatlitData = epochSatlitDatanl.at(epo);

        switch (oneSatlitData.SatType) {
        case 'G':
            Trans_G(epo)=-1;
            if(oneSatlitData.PRN == oneSatlitData.maxEA_PRN)
            {
                sysTemflag[0]=1;
                pos_Trans_G = epo;
                remove_satA[remoN] = epo;
                remoN++;
            }
            else
            {
                if(Par_sat_epo[epo] != 0)
                {
                    remove_satA[remoN] = epo;
                    remoN++;
                }
                else if(oneSatlitData.cycly_slipF==1||oneSatlitData.Par_sub)
                {
                    Par_sat_epo[epo] = 1;
                    remove_satA[remoN] = epo;
                    remoN++;
                }
            }
            break;
        case 'C':
            if(oneSatlitData.PRN<=18)
            {
                Trans_C2(epo)=-1;
                if(oneSatlitData.PRN == oneSatlitData.maxEA_PRN)
                {
                    sysTemflag[1]=2;
                    pos_Trans_C2 = epo;
                    remove_satA[remoN] = epo;
                    remoN++;
                }
                else
                {
                    if(Par_sat_epo[epo] != 0)
                    {
                        remove_satA[remoN] = epo;
                        remoN++;
                    }
                    else if(oneSatlitData.cycly_slipF==1||oneSatlitData.Par_sub)
                    {
                        Par_sat_epo[epo] = 1;
                        remove_satA[remoN] = epo;
                        remoN++;
                    }
                }
            }
            else
            {
                Trans_C3(epo)=-1;
                if(oneSatlitData.PRN == oneSatlitData.maxEA_PRN)
                {
                    sysTemflag[2]=3;
                    pos_Trans_C3 = epo;
                    remove_satA[remoN] = epo;
                    remoN++;
                }
                else
                {
                    if(Par_sat_epo[epo] != 0)
                    {
                        remove_satA[remoN] = epo;
                        remoN++;
                    }
                    else if(oneSatlitData.cycly_slipF==1||oneSatlitData.Par_sub)
                    {
                        Par_sat_epo[epo] = 1;
                        remove_satA[remoN] = epo;
                        remoN++;
                    }
                }
            }
            break;
        case 'R':
            Trans_R(epo)=-1;
            if(oneSatlitData.PRN == oneSatlitData.maxEA_PRN)
            {
                sysTemflag[3]=4;
                pos_Trans_R = epo;
                remove_satA[remoN] = epo;
                remoN++;
            }
            else
            {
                if(Par_sat_epo[epo] != 0)
                {
                    remove_satA[remoN] = epo;
                    remoN++;
                }
                else if(oneSatlitData.cycly_slipF==1||oneSatlitData.Par_sub)
                {
                    Par_sat_epo[epo] = 1;
                    remove_satA[remoN] = epo;
                    remoN++;
                }
            }
            break;
        case 'E':
            Trans_E(epo)=-1;
            if(oneSatlitData.PRN == oneSatlitData.maxEA_PRN)
            {
                sysTemflag[4]=5;
                pos_Trans_E = epo;
                remove_satA[remoN] = epo;
                remoN++;
            }
            else
            {
                if(Par_sat_epo[epo] != 0)
                {
                    remove_satA[remoN] = epo;
                    remoN++;
                }
                else if(oneSatlitData.cycly_slipF==1||oneSatlitData.Par_sub)
                {
                    Par_sat_epo[epo] = 1;
                    remove_satA[remoN] = epo;
                    remoN++;
                }
            }
            break;
        default:
            break;
        }
    }


    Ambi_R.descending_order(remoN,remove_satA);
    for(int i =0;i<5;i++)
    {
        switch (sysTemflag[i]) {
        case 1:
            UDUC_B.block(0,pos_Trans_G,epoLen,1) = Trans_G;
            break;
        case 2:
            UDUC_B.block(0,pos_Trans_C2,epoLen,1) = Trans_C2;
            break;
        case 3:
            UDUC_B.block(0,pos_Trans_C3,epoLen,1) = Trans_C3;
            break;
        case 4:
            UDUC_B.block(0,pos_Trans_R,epoLen,1) = Trans_R;
            break;
        case 5:
            UDUC_B.block(0,pos_Trans_E,epoLen,1) = Trans_E;
            break;
        default:
            break;
        }
    }

    for(int i =0;i<remoN;i++)
    {
        m_mat_cal.RemoveRow(UDUC_B,remove_satA[i]);
    }
    UDUC_SD_X = UDUC_B * UDUC_SD_X;
    UDUC_SD_P = UDUC_B * UDUC_SD_P * UDUC_B.transpose();
}
void uduc_ambiguity_resolution::UDUC_Fixed_partial_NL_ambiguity(QVector< SatlitData > &epochSatlitDatanl,
                                                  int *Par_sat_epo,MatrixXd &UDUC_SD_P,VectorXd &UDUC_SD_X,VectorXd &UDUC_SD_WLX)
{//
    int epoLen = epochSatlitDatanl.length();
    for(int i=0;i<10;i++)
    {
        m_SYStem_satN[i] = 0;
    }
    MatrixXd UDUC_B;
    VectorXd Trans_G,Trans_C2,Trans_C3,Trans_E,Trans_R;
    UDUC_B.resize(epoLen,epoLen);
    UDUC_B.setIdentity(epoLen,epoLen);
    Trans_G.resize(epoLen);Trans_C2.resize(epoLen);Trans_C3.resize(epoLen);
    Trans_E.resize(epoLen);Trans_R.resize(epoLen);
    Trans_G.setZero(epoLen);Trans_C2.setZero(epoLen);Trans_C3.setZero(epoLen);
    Trans_E.setZero(epoLen);Trans_R.setZero(epoLen);
    int pos_Trans_G=0,pos_Trans_C2=0,pos_Trans_C3=0,pos_Trans_E=0,pos_Trans_R=0;
    int remoN=0;int remove_satA[epoLen];int sysTemflag[5];
    for(int i=0;i<5;i++)    sysTemflag[i]=0;
    for(int i=0;i<epoLen;i++) remove_satA[i] = 0;
    for (int epo=0;epo<epoLen;epo++)
    {
        SatlitData oneSatlitData = epochSatlitDatanl.at(epo);

        switch (oneSatlitData.SatType) {
        case 'G':
            Trans_G(epo)=-1;
            if(oneSatlitData.PRN == oneSatlitData.maxEA_PRN)
            {
                sysTemflag[0]=1;
                pos_Trans_G = epo;
                remove_satA[remoN] = epo;
                remoN++;
            }
            else
            {
                if(Par_sat_epo[epo] != 0)
                {
                    remove_satA[remoN] = epo;
                    remoN++;
                }
                else if(oneSatlitData.cycly_slipF==1||oneSatlitData.Par_sub)
                {
                    Par_sat_epo[epo] = 1;
                    remove_satA[remoN] = epo;
                    remoN++;
                }
            }
            break;
        case 'C':
            if(oneSatlitData.PRN<=18)
            {
                Trans_C2(epo)=-1;
                if(oneSatlitData.PRN == oneSatlitData.maxEA_PRN)
                {
                    sysTemflag[1]=2;
                    pos_Trans_C2 = epo;
                    remove_satA[remoN] = epo;
                    remoN++;
                }
                else
                {
                    if(Par_sat_epo[epo] != 0)
                    {
                        remove_satA[remoN] = epo;
                        remoN++;
                    }
                    else if(oneSatlitData.cycly_slipF==1||oneSatlitData.Par_sub)
                    {
                        Par_sat_epo[epo] = 1;
                        remove_satA[remoN] = epo;
                        remoN++;
                    }
                }
            }
            else
            {
                Trans_C3(epo)=-1;
                if(oneSatlitData.PRN == oneSatlitData.maxEA_PRN)
                {
                    sysTemflag[2]=3;
                    pos_Trans_C3 = epo;
                    remove_satA[remoN] = epo;
                    remoN++;
                }
                else
                {
                    if(Par_sat_epo[epo] != 0)
                    {
                        remove_satA[remoN] = epo;
                        remoN++;
                    }
                    else if(oneSatlitData.cycly_slipF==1||oneSatlitData.Par_sub)
                    {
                        Par_sat_epo[epo] = 1;
                        remove_satA[remoN] = epo;
                        remoN++;
                    }
                }
            }
            break;
        case 'R':
            Trans_R(epo)=-1;
            if(oneSatlitData.PRN == oneSatlitData.maxEA_PRN)
            {
                sysTemflag[3]=4;
                pos_Trans_R = epo;
                remove_satA[remoN] = epo;
                remoN++;
            }
            else
            {
                if(Par_sat_epo[epo] != 0)
                {
                    remove_satA[remoN] = epo;
                    remoN++;
                }
                else if(oneSatlitData.cycly_slipF==1||oneSatlitData.Par_sub)
                {
                    Par_sat_epo[epo] = 1;
                    remove_satA[remoN] = epo;
                    remoN++;
                }
            }
            break;
        case 'E':
            Trans_E(epo)=-1;
            if(oneSatlitData.PRN == oneSatlitData.maxEA_PRN)
            {
                sysTemflag[4]=5;
                pos_Trans_E = epo;
                remove_satA[remoN] = epo;
                remoN++;
            }
            else
            {
                if(Par_sat_epo[epo] != 0)
                {
                    remove_satA[remoN] = epo;
                    remoN++;
                }
                else if(oneSatlitData.cycly_slipF==1||oneSatlitData.Par_sub)
                {
                    Par_sat_epo[epo] = 1;
                    remove_satA[remoN] = epo;
                    remoN++;
                }
            }
            break;
        default:
            break;
        }
    }


    Ambi_R.descending_order(remoN,remove_satA);
    for(int i =0;i<5;i++)
    {
        switch (sysTemflag[i]) {
        case 1:
            UDUC_B.block(0,pos_Trans_G,epoLen,1) = Trans_G;
            break;
        case 2:
            UDUC_B.block(0,pos_Trans_C2,epoLen,1) = Trans_C2;
            break;
        case 3:
            UDUC_B.block(0,pos_Trans_C3,epoLen,1) = Trans_C3;
            break;
        case 4:
            UDUC_B.block(0,pos_Trans_R,epoLen,1) = Trans_R;
            break;
        case 5:
            UDUC_B.block(0,pos_Trans_E,epoLen,1) = Trans_E;
            break;
        default:
            break;
        }
    }

    for(int i =0;i<remoN;i++)
    {
        m_mat_cal.RemoveRow(UDUC_B,remove_satA[i]);
    }
    UDUC_SD_X = UDUC_B * UDUC_SD_X - UDUC_B * UDUC_SD_WLX;
    UDUC_SD_P = UDUC_B * UDUC_SD_P * UDUC_B.transpose();
}
bool uduc_ambiguity_resolution::UDUC_ambiguity_search(QVector< SatlitData > &epochSatlitDataLa,VectorXd &X,MatrixXd &P,
                                       int &fix_flag,int *Par_sat_epo,para_trans *trans_para,const corr_opt *opt_corr)
{
    MatrixXd La_SDP,Qab,Trans_SD;
    VectorXd La_SDX,SD_GNSS_F;
    Ambiguity_mat *Amb_mat = AGNSS_APPData::sGetAPPDataInstance()->mGetAmbiguity_matData();
    La_SDP=Amb_mat->La_SD_P;La_SDX=Amb_mat->La_SD_X;
    int satellite_len = epochSatlitDataLa.length();

//    Par_sub_amb(epochSatlitDataLa,SD_IFsmoot,P,La_SDP,Qab,Trans_SD,SD_GNSS_F,La_SDX,SD_nl_fcb,Par_sat_epo,maxEASat_Prn);
    int par_number=Amb_mat->La_SD_P.cols(),Full_sub_len=Amb_mat->Fixed_Qa.cols();//first_search=0,
    int info;QString wirte_debug;
    while(par_number>4*opt_corr->fre_model&&!trans_para->ambiguity_fix)
    {
        MatrixXd amb_fix;double ratio = trans_para->ratio_value;int out_fixamb_num = 2;//
        sea_fil.writeQdebug2Txt(wirte_debug);
        info = m_lambda.QLamdaSearch(Amb_mat->La_SD_X,Amb_mat->La_SD_P,amb_fix, &ratio,out_fixamb_num);//,&par_flag Par_sat_epo[6]
        if(info)    return false;
        wirte_debug ="m_lambda.QLamdaSearch";
        sea_fil.writeQdebug2Txt(wirte_debug);
        trans_para->lambda_radio = ratio;
        if(ratio>trans_para->ratio_value)
        {
            wirte_debug ="ratio>3.0";
            trans_para->fixed_sat = La_SDP.rows();
            sea_fil.writeQdebug2Txt(wirte_debug);
            VectorXd La_SDnl,par_fix_amb,Optimal_solution;
            La_SDnl.resize(La_SDP.rows());La_SDnl.setZero(La_SDP.rows());
            Optimal_solution.resize(La_SDP.rows());Optimal_solution.setZero(La_SDP.rows());
            La_SDnl = amb_fix.topLeftCorner(La_SDP.rows(), 1);
            Optimal_solution = La_SDX - La_SDnl;//top left corner;
            par_fix_amb.resize(Full_sub_len);par_fix_amb.setZero(Full_sub_len);
            int par_sat_len=0,full_ar_epo=0;
            for(int i=0;i<satellite_len;i++)
            {
                SatlitData Trans_Sat = epochSatlitDataLa.at(i);
                if(Trans_Sat.PRN == Trans_Sat.maxEA_PRN)
                    continue;
                if(Par_sat_epo[i]==0)
                {
                    for(int j=0;j<opt_corr->fre_model;j++)
                    {
                        par_fix_amb(full_ar_epo)=Optimal_solution(par_sat_len);
                        par_sat_len++;full_ar_epo++;
                    }
                }
                else
                {
                    for(int j=0;j<opt_corr->fre_model;j++)
                    {
                        full_ar_epo++;
                    }
                }
            }
            wirte_debug ="full_ar_epo++;";
            sea_fil.writeQdebug2Txt(wirte_debug);
            fix_flag = 1;
            Amb_mat->SD_a = par_fix_amb;
            break;
        }
        else
        {
            trans_para->fixed_sat =0;
            if(trans_para->Partial_subset==angle_screen)
                Ambi_R.angle_altitude_screen(epochSatlitDataLa,Par_sat_epo);
            else if(trans_para->Partial_subset==stand_post_res)
                Ambi_R.unfixed_sat(satellite_len,Par_sat_epo,trans_para);
            else if(trans_para->Partial_subset==post_resudual)
            {
                Ambi_R.post_resuduala(satellite_len,Par_sat_epo,trans_para);
            }
            else if(trans_para->Partial_subset==Adop_demet_secreen)
                Ambi_R.Adop_screen(epochSatlitDataLa,Par_sat_epo,La_SDP);
            else if(trans_para->Partial_subset==Boost_Par)
                Ambi_R.Boost_Parsub(epochSatlitDataLa,Par_sat_epo,La_SDP);
            else if(trans_para->Partial_subset==full_ambiguity_fixed)
                break;
            UDUC_NarrowLaneAR(epochSatlitDataLa,X,P,Par_sat_epo,trans_para,opt_corr);
            par_number=Amb_mat->La_SD_P.cols();
            La_SDP=Amb_mat->La_SD_P;La_SDX=Amb_mat->La_SD_X;
        }
    }
}
