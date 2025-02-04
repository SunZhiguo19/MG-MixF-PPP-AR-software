#include "ambiguity_resolution.h"

Ambiguity_resolution::Ambiguity_resolution()
{

}
/*****MW Combined Fixed ambiguity**/
bool Ambiguity_resolution::WidelaneAR(SatlitData &oneSatlitData, const corr_opt *opt_corr)
{//
    int fre_num0=oneSatlitData.IF_Frq[0],fre_num1=oneSatlitData.IF_Frq[1];
    double F1 = oneSatlitData.Frq[fre_num0],F2 = oneSatlitData.Frq[fre_num1];//Get the frequency of this satellite
    double Lamta1 = oneSatlitData.Lambda[fre_num0],Lamta2 = oneSatlitData.Lambda[fre_num1];
    double lamdaMW = M_C/(F1 - F2);
    double dcbB1 = F2*F2/(F1*F1 - F2*F2);
    double dcbB2 = F1*F1/(F1*F1 - F2*F2);
    double L1s = 0.0 ,L2s = 0.0 ;
    double C1s = 0.0 ,C2s = 0.0 ;
    if(opt_corr->treat_method==POS_RT_PPP_AR||opt_corr->treat_method==POS_Real_PPP_AR)
    {
//        L1s = oneSatlitData.PhaseL[fre_num0] + (oneSatlitData.PHASE_BIA[fre_num0])/Lamta1 + oneSatlitData.LxOffset[fre_num0] + oneSatlitData.SatLxOffset[fre_num0];
//        L2s = oneSatlitData.PhaseL[fre_num1] + (oneSatlitData.PHASE_BIA[fre_num1])/Lamta2 + oneSatlitData.LxOffset[fre_num1] + oneSatlitData.SatLxOffset[fre_num1];//
//        C1s = oneSatlitData.CodeP[fre_num0] + oneSatlitData.CODE_BIA[fre_num0] +Lamta1*( oneSatlitData.LxOffset[fre_num0] + oneSatlitData.SatLxOffset[fre_num0]);//
//        C2s = oneSatlitData.CodeP[fre_num1] + oneSatlitData.CODE_BIA[fre_num1] +Lamta2*( oneSatlitData.LxOffset[fre_num1] + oneSatlitData.SatLxOffset[fre_num1]);
        L1s = oneSatlitData.PhaseL[fre_num0] + (oneSatlitData.PHASE_BIA[fre_num0])/Lamta1  + oneSatlitData.Sat_LxPCO[fre_num0];
        L2s = oneSatlitData.PhaseL[fre_num1] + (oneSatlitData.PHASE_BIA[fre_num1])/Lamta2  + oneSatlitData.Sat_LxPCO[fre_num1];//
        C1s = oneSatlitData.CodeP[fre_num0] + oneSatlitData.CODE_BIA[fre_num0] +Lamta1*(  oneSatlitData.Sat_LxPCO[fre_num0]);//
        C2s = oneSatlitData.CodeP[fre_num1] + oneSatlitData.CODE_BIA[fre_num1] +Lamta2*(  oneSatlitData.Sat_LxPCO[fre_num1]);
//        L1s = oneSatlitData.PhaseL[fre_num0] + (oneSatlitData.PHASE_BIA[fre_num0])/Lamta1;//debug 0112 szg -oneSatlitData.stalitDCB
//        L2s = oneSatlitData.PhaseL[fre_num1] + (oneSatlitData.PHASE_BIA[fre_num1])/Lamta2;//-dcbB2*oneSatlitData.stalitDCB Lamta2 -oneSatlitData.stalitDCB*Lamta1/Lamta2
//        C1s = oneSatlitData.CodeP[fre_num0] + oneSatlitData.CODE_BIA[fre_num0];//
//        C2s = oneSatlitData.CodeP[fre_num1] + oneSatlitData.CODE_BIA[fre_num1];
    }
    else if(opt_corr->treat_method==POS_PP_PPP_AR_1||opt_corr->treat_method==POS_PP_PPP_AR_2)
    {

        if(oneSatlitData.SatType=='G'&&oneSatlitData.stalitDCBFlag == 1)
        {
//            L1s = oneSatlitData.PhaseL[fre_num0]+oneSatlitData.LxOffset[fre_num0] + oneSatlitData.Sat_LxPCO[fre_num0];//debug 0112 szg -oneSatlitData.stalitDCB
//            L2s = oneSatlitData.PhaseL[fre_num1]+oneSatlitData.LxOffset[fre_num1] + oneSatlitData.Sat_LxPCO[fre_num1];//
//            C1s = oneSatlitData.CodeP[fre_num0] + oneSatlitData.stalitDCB * M_C * 1e-9 + Lamta1*( oneSatlitData.LxOffset[fre_num0] + oneSatlitData.SatLxOffset[fre_num0]);//- oneSatlitData.CODE_BIA[0]
//            C2s = oneSatlitData.CodeP[fre_num1]+Lamta2*( oneSatlitData.LxOffset[fre_num1] + oneSatlitData.Sat_LxPCO[fre_num1]);//
            L1s = oneSatlitData.PhaseL[fre_num0];//debug 0112 szg -oneSatlitData.stalitDCB
            L2s = oneSatlitData.PhaseL[fre_num1];//
            if(oneSatlitData.stalitDCBFlag == 1)
                C1s = oneSatlitData.CodeP[fre_num0] + oneSatlitData.stalitDCB * M_C * 1e-9;//- oneSatlitData.CODE_BIA[0]
            else
                C1s = oneSatlitData.CodeP[fre_num0];//
            C2s = oneSatlitData.CodeP[fre_num1];//

        }
        else
        {
//            L1s = oneSatlitData.PhaseL[fre_num0]  + oneSatlitData.LxOffset[fre_num0] + oneSatlitData.SatLxOffset[fre_num0];//debug 0112 szg -oneSatlitData.stalitDCB
//            L2s = oneSatlitData.PhaseL[fre_num1]  + oneSatlitData.LxOffset[fre_num1] + oneSatlitData.SatLxOffset[fre_num1];//
//            C1s = oneSatlitData.CodeP[fre_num0]  +Lamta1*( oneSatlitData.LxOffset[fre_num0] + oneSatlitData.SatLxOffset[fre_num0]);//
//            C2s = oneSatlitData.CodeP[fre_num1]  +Lamta2*( oneSatlitData.LxOffset[fre_num1] + oneSatlitData.SatLxOffset[fre_num1]);

            L1s = oneSatlitData.PhaseL[fre_num0];//debug 0112 szg -oneSatlitData.stalitDCB
            L2s = oneSatlitData.PhaseL[fre_num1];//
            C1s = oneSatlitData.CodeP[fre_num0];
            C2s = oneSatlitData.CodeP[fre_num1];
        }



     }

    double IF_wl = (L1s - L2s) - (F1*C1s + F2*C2s)/((F1+F2)*lamdaMW);
    if(opt_corr->treat_method==POS_Real_PPP_AR)
    {
        oneSatlitData.MW_Wl_Int = IF_wl;
    }
    else if(opt_corr->treat_method==POS_PP_PPP_AR_2)
    {
        oneSatlitData.MW_Wl_Int = IF_wl - oneSatlitData.MW_wlfcbs;
    }
    else if(opt_corr->treat_method==POS_PP_PPP_AR_1)
    {
        if(opt_corr->WLUPD_corr==0)
            oneSatlitData.MW_Wl_Int = IF_wl - oneSatlitData.MW_wlfcbs;
        else
            oneSatlitData.MW_Wl_Int = IF_wl + oneSatlitData.MW_wlfcbs;
    }

    return true;
}

/*Reference star selection*/
void Ambiguity_resolution::Refe_star_selec(QVector< SatlitData > &SatlitData_selec,double *maxEAsat,int *maxEAPRN)
{
    for(int i=0;i<SatlitData_selec.length();i++)
    {
        SatlitData oneSatlitData = SatlitData_selec.at(i);
        double sat_EA = oneSatlitData.EA[0];
        switch (oneSatlitData.SatType) {
        case 'G':
            if(maxEAsat[0] < sat_EA)//debug   szg 1223
            {
              maxEAsat[0] = sat_EA;
              maxEAPRN[0] = oneSatlitData.PRN;//Single difference reference star between different systems
            }
            break;
        case 'C':
            if(oneSatlitData.PRN<=16)
            {
                if(maxEAsat[1] < sat_EA)//debug   szg 1223
                {
                  maxEAsat[1] = sat_EA;
                  maxEAPRN[1] = oneSatlitData.PRN;//Single difference reference star between different systems
                }
            }
            else if(oneSatlitData.PRN>16)
            {
                if(maxEAsat[2] < sat_EA)//debug   szg 1223
                {
                  maxEAsat[2] = sat_EA;
                  maxEAPRN[2] = oneSatlitData.PRN;//Single difference reference star between different systems
                }
            }
            break;
        case 'R':
            if(maxEAsat[3] < sat_EA)//debug   szg 1223
            {
              maxEAsat[3] = sat_EA;
              maxEAPRN[3] = oneSatlitData.PRN;//Single difference reference star between different systems
            }
            break;
        case 'E':
            if(maxEAsat[4] < sat_EA)//debug   szg 1223
            {
              maxEAsat[4] = sat_EA;
              maxEAPRN[4] = oneSatlitData.PRN;//Single difference reference star between different systems
            }
            break;
        default:
            break;
        }
    }

}
/*选择观测频率最多的卫星中最大高度角卫星作为参考星*/
void Ambiguity_resolution::Refe_star_selec_mixed(QVector< SatlitData > &SatlitData_selec,double *maxEAsat,int *maxEAPRN)
{
    int obser_Len[5] = {0};
    for(int i=0;i<SatlitData_selec.length();i++)
    {
        SatlitData oneSatlitData = SatlitData_selec.at(i);
        double sat_EA = oneSatlitData.EA[0];
        switch (oneSatlitData.SatType) {
        case 'G':
            if(maxEAsat[0] < sat_EA&&oneSatlitData.ObsLenfre>=obser_Len[0])//debug   szg 1223
            {
                obser_Len[0] = oneSatlitData.ObsLenfre;
                maxEAsat[0] = sat_EA;
                maxEAPRN[0] = oneSatlitData.PRN;//Single difference reference star between different systems
            }
            break;
        case 'C':
            if(oneSatlitData.PRN<=16)
            {
                if(maxEAsat[1] < sat_EA&&oneSatlitData.ObsLenfre>=obser_Len[1])//debug   szg 1223
                {
                    obser_Len[1] = oneSatlitData.ObsLenfre;
                    maxEAsat[1] = sat_EA;
                    maxEAPRN[1] = oneSatlitData.PRN;//Single difference reference star between different systems
                }
            }
            else if(oneSatlitData.PRN>16)
            {
                if(maxEAsat[2] < sat_EA&&oneSatlitData.ObsLenfre>=obser_Len[2])//debug   szg 1223
                {
                    obser_Len[2] = oneSatlitData.ObsLenfre;
                    maxEAsat[2] = sat_EA;
                    maxEAPRN[2] = oneSatlitData.PRN;//Single difference reference star between different systems
                }
            }
            break;
        case 'R':
            if(maxEAsat[3] < sat_EA&&oneSatlitData.ObsLenfre>=obser_Len[3])//debug   szg 1223
            {
                obser_Len[3] = oneSatlitData.ObsLenfre;
                maxEAsat[3] = sat_EA;
                maxEAPRN[3] = oneSatlitData.PRN;//Single difference reference star between different systems
            }
            break;
        case 'E':
            if(maxEAsat[4] < sat_EA&&oneSatlitData.ObsLenfre>=obser_Len[4])//debug   szg 1223
            {
                obser_Len[4] = oneSatlitData.ObsLenfre;
                maxEAsat[4] = sat_EA;
                maxEAPRN[4] = oneSatlitData.PRN;//Single difference reference star between different systems
            }
            break;
        default:
            break;
        }
    }

}

void Ambiguity_resolution::Refe_sat_var_selec(QVector< SatlitData > &SatlitData_selec,int *minVarPRN,MatrixXd &P,int const_parm)
{
    double minvar[5];
    for(int i=0;i<5;i++)
        minvar[i]=100000;

    for(int i=0;i<SatlitData_selec.length();i++)
    {
        SatlitData oneSatlitData = SatlitData_selec.at(i);
        double sat_var = P(i+const_parm,i+const_parm);
        switch (oneSatlitData.SatType) {
        case 'G':
            if(minvar[0] > sat_var)//debug   szg 1223
            {
              minvar[0] = sat_var;
              minVarPRN[0] = oneSatlitData.PRN;//Single difference reference star between different systems
            }
            break;
        case 'C':
            if(oneSatlitData.PRN<=16)
            {
                if(minvar[1] > sat_var)//debug   szg 1223
                {
                  minvar[1] = sat_var;
                  minVarPRN[1] = oneSatlitData.PRN;//Single difference reference star between different systems
                }
            }
            else if(oneSatlitData.PRN>16)
            {
                if(minvar[2] > sat_var)//debug   szg 1223
                {
                  minvar[2] = sat_var;
                  minVarPRN[2] = oneSatlitData.PRN;//Single difference reference star between different systems
                }
            }
            break;
        case 'R':
            if(minvar[3] > sat_var)//debug   szg 1223
            {
              minvar[3] = sat_var;
              minVarPRN[3] = oneSatlitData.PRN;//Single difference reference star between different systems
            }
            break;
        case 'E':
            if(minvar[4] > sat_var)//debug   szg 1223
            {
              minvar[4] = sat_var;
              minVarPRN[4] = oneSatlitData.PRN;//Single difference reference star between different systems
            }
            break;
        default:
            break;
        }
    }

}
void Ambiguity_resolution::Refe_sat_succ_selec(QVector< SatlitData > &SatlitData_selec,int *max_suc_PRN,MatrixXd &P,int const_parm)
{
    MatrixXd G_P,C2_P,C3_P,E_P,R_P;
    MatrixXd G_P_I,C2_P_I,C3_P_I,E_P_I,R_P_I,Amb_P;
    int G_P_l,C2_P_l,C3_P_l,E_P_l,R_P_l;
    int Amb_len = SatlitData_selec.length();
    Amb_P = P.block(const_parm,const_parm,Amb_len,Amb_len);
    G_P_l=C2_P_l=C3_P_l=E_P_l=R_P_l=0;
    int sys_GNSS[Amb_len]={0};
    int Gremo_Sys[Amb_len],C2remo_Sys[Amb_len],C3remo_Sys[Amb_len],Rremo_Sys[Amb_len],Eremo_Sys[Amb_len];
    int Gremo_N,C2remo_N,C3remo_N,Rremo_N,Eremo_N;
    Gremo_N=C2remo_N=C3remo_N=Rremo_N=Eremo_N=0;
    para_trans *trans_para = AGNSS_APPData::sGetAPPDataInstance()->mGettrans_paraData();
    int G_ref_sat,C2_ref_sat,C3_ref_sat,R_ref_sat,E_ref_sat;
    int G_ref_sat_N,C2_ref_sat_N,C3_ref_sat_N,R_ref_sat_N,E_ref_sat_N;
    G_ref_sat_N=C2_ref_sat_N=C3_ref_sat_N=R_ref_sat_N=E_ref_sat_N=0;
    for(int i=0;i<Amb_len;i++)
    {
        SatlitData oneSatlitData = SatlitData_selec.at(i);
        switch (oneSatlitData.SatType) {
        case 'G':
            if(oneSatlitData.Par_sub&&oneSatlitData.EA[0]<30)   {
                Gremo_Sys[Gremo_N]=i;C2remo_Sys[C2remo_N]=i;C3remo_Sys[C3remo_N]=i;Rremo_Sys[Rremo_N]=i;Eremo_Sys[Eremo_N]=i;
                Gremo_N++;C2remo_N++;C3remo_N++;Rremo_N++;Eremo_N++;
                break;}
            sys_GNSS[i]=SYS_GPS;
            G_P_l++;
            C2remo_Sys[C2remo_N]=i;C3remo_Sys[C3remo_N]=i;Rremo_Sys[Rremo_N]=i;Eremo_Sys[Eremo_N]=i;
            C2remo_N++;C3remo_N++;Rremo_N++;Eremo_N++;
            break;
        case 'C':
            if(oneSatlitData.PRN<=16)
            {
                if(oneSatlitData.Par_sub&&oneSatlitData.EA[0]<30)   {
                    Gremo_Sys[Gremo_N]=i;C2remo_Sys[C2remo_N]=i;C3remo_Sys[C3remo_N]=i;Rremo_Sys[Rremo_N]=i;Eremo_Sys[Eremo_N]=i;
                    Gremo_N++;C2remo_N++;C3remo_N++;Rremo_N++;Eremo_N++;
                    break;}
                sys_GNSS[i]=SYS_BD2;
                C2_P_l++;
                Gremo_Sys[Gremo_N]=i;C3remo_Sys[C3remo_N]=i;Rremo_Sys[Rremo_N]=i;Eremo_Sys[Eremo_N]=i;
                Gremo_N++;C3remo_N++;Rremo_N++;Eremo_N++;
            }
            else if(oneSatlitData.PRN>16)
            {
                if(oneSatlitData.Par_sub&&oneSatlitData.EA[0]<30)   {
                    Gremo_Sys[Gremo_N]=i;C2remo_Sys[C2remo_N]=i;C3remo_Sys[C3remo_N]=i;Rremo_Sys[Rremo_N]=i;Eremo_Sys[Eremo_N]=i;
                    Gremo_N++;C2remo_N++;C3remo_N++;Rremo_N++;Eremo_N++;
                    break;}
                sys_GNSS[i]=SYS_BD3;
                C3_P_l++;
                Gremo_Sys[Gremo_N]=i;C2remo_Sys[C2remo_N]=i;Rremo_Sys[Rremo_N]=i;Eremo_Sys[Eremo_N]=i;
                Gremo_N++;C2remo_N++;Rremo_N++;Eremo_N++;
            }
            break;
        case 'R':
            if(oneSatlitData.Par_sub&&oneSatlitData.EA[0]<30)   {
                Gremo_Sys[Gremo_N]=i;C2remo_Sys[C2remo_N]=i;C3remo_Sys[C3remo_N]=i;Rremo_Sys[Rremo_N]=i;Eremo_Sys[Eremo_N]=i;
                Gremo_N++;C2remo_N++;C3remo_N++;Rremo_N++;Eremo_N++;
                break;}
            sys_GNSS[i]=SYS_GLO;
            R_P_l++;
            Gremo_Sys[Gremo_N]=i;C2remo_Sys[C2remo_N]=i;C3remo_Sys[C3remo_N]=i;Eremo_Sys[Eremo_N]=i;
            Gremo_N++;C2remo_N++;C3remo_N++;Eremo_N++;
            break;
        case 'E':
            if(oneSatlitData.Par_sub&&oneSatlitData.EA[0]<30)   {
                Gremo_Sys[Gremo_N]=i;C2remo_Sys[C2remo_N]=i;C3remo_Sys[C3remo_N]=i;Rremo_Sys[Rremo_N]=i;Eremo_Sys[Eremo_N]=i;
                Gremo_N++;C2remo_N++;C3remo_N++;Rremo_N++;Eremo_N++;
                break;}
            sys_GNSS[i]=SYS_GAL;
            E_P_l++;
            Gremo_Sys[Gremo_N]=i;C2remo_Sys[C2remo_N]=i;C3remo_Sys[C3remo_N]=i;Rremo_Sys[Rremo_N]=i;
            Gremo_N++;C2remo_N++;C3remo_N++;Rremo_N++;
            break;
        default:
            break;
        }
    }
    double G_Succ[G_P_l],C2_Succ[C2_P_l],C3_Succ[C3_P_l],R_Succ[R_P_l],E_Succ[E_P_l];
    int G_Succ_PS[G_P_l],C2_Succ_PS[C2_P_l],C3_Succ_PS[C3_P_l],R_Succ_PS[R_P_l],E_Succ_PS[E_P_l];
    if(G_P_l!=0)    {
        G_P.resize(G_P_l,G_P_l);G_P.setZero(G_P_l,G_P_l);
        G_P_I.resize(Amb_len,Amb_len);G_P_I.setIdentity();
        descending_order(Gremo_N,Gremo_Sys);
        for(int i =0;i<Gremo_N;i++)  m_mat_cal.RemoveRow(G_P_I,Gremo_Sys[i]);
        G_P = G_P_I * Amb_P * G_P_I.transpose();

        if(G_P_l>1)
        {
            for(int i=0;i<G_P_l;i++)
            {
                MatrixXd SD_P,remo_N;
                remo_N.resize(G_P_l,G_P_l);remo_N.setIdentity();
                for(int j=0;j<G_P_l;j++)    remo_N(j,i) = -1;
                m_mat_cal.RemoveRow(remo_N,i);
                SD_P = remo_N * G_P *remo_N.transpose();
                G_Succ[i]= m_lambda.Sur_Success_rate(SD_P,&G_Succ_PS[i],trans_para->Ps);
            }
            double Max_Success_rate = 0;int Max_Success_rate_N = 0;
            for(int i=0;i<G_P_l;i++)
            {
                if(Max_Success_rate<G_Succ[i])
                {
                    Max_Success_rate = G_Succ[i];
                    G_ref_sat=i;
                }
                if(Max_Success_rate_N<=G_Succ_PS[i])
                {
                    if(Max_Success_rate_N==G_Succ_PS[i])
                    {
                        if(G_Succ[G_ref_sat_N]<G_Succ[i])
                        {
                            Max_Success_rate_N = G_Succ_PS[i];
                            G_ref_sat_N=i;
                        }
                        else
                        {
                            G_ref_sat_N=Max_Success_rate_N;
                        }
                    }
                    else
                    {
                        Max_Success_rate_N = G_Succ_PS[i];
                        G_ref_sat_N=i;
                    }
                }
            }
        }

    }
    if(C2_P_l!=0)    {
        C2_P.resize(C2_P_l,C2_P_l);C2_P.setZero(C2_P_l,C2_P_l);
        C2_P_I.resize(Amb_len,Amb_len);C2_P_I.setIdentity();
        descending_order(C2remo_N,C2remo_Sys);
        for(int i =0;i<C2remo_N;i++)  m_mat_cal.RemoveRow(C2_P_I,C2remo_Sys[i]);
        C2_P = C2_P_I * Amb_P * C2_P_I.transpose();

        if(C2_P_l>1)
        {
            for(int i=0;i<C2_P_l;i++)
            {
                MatrixXd SD_P,remo_N;
                remo_N.resize(C2_P_l,C2_P_l);remo_N.setIdentity();
                for(int j=0;j<C2_P_l;j++)    remo_N(j,i) = -1;
                m_mat_cal.RemoveRow(remo_N,i);
                SD_P = remo_N * C2_P *remo_N.transpose();
                C2_Succ[i]= m_lambda.Sur_Success_rate(SD_P,&C2_Succ_PS[i],trans_para->Ps);
            }
            double Max_Success_rate = 0;int Max_Success_rate_N = 0;
            for(int i=0;i<C2_P_l;i++)
            {
                if(Max_Success_rate<C2_Succ[i])
                {
                    Max_Success_rate = C2_Succ[i];
                    C2_ref_sat=i;
                }
                if(Max_Success_rate_N<=C2_Succ_PS[i])
                {
                    if(Max_Success_rate_N==C2_Succ_PS[i])
                    {
                        if(C2_Succ[C2_ref_sat_N]<C2_Succ[i])
                        {
                            Max_Success_rate_N = C2_Succ_PS[i];
                            C2_ref_sat_N=i;
                        }
                        else
                        {
                            C2_ref_sat_N=Max_Success_rate_N;
                        }
                    }
                    else
                    {
                        Max_Success_rate_N = C2_Succ_PS[i];
                        C2_ref_sat_N=i;
                    }
                }
            }
        }
    }
    if(C3_P_l!=0)    {
        C3_P.resize(C3_P_l,C3_P_l);C3_P.setZero(C3_P_l,C3_P_l);
        C3_P_I.resize(Amb_len,Amb_len);C3_P_I.setIdentity();
        descending_order(C3remo_N,C3remo_Sys);
        for(int i =0;i<C3remo_N;i++)  m_mat_cal.RemoveRow(C3_P_I,C3remo_Sys[i]);
        C3_P = C3_P_I * Amb_P * C3_P_I.transpose();

        if(C3_P_l>1)
        {
            for(int i=0;i<C3_P_l;i++)
            {
                MatrixXd SD_P,remo_N;
                remo_N.resize(C3_P_l,C3_P_l);remo_N.setIdentity();
                for(int j=0;j<C3_P_l;j++)    remo_N(j,i) = -1;
                m_mat_cal.RemoveRow(remo_N,i);
                SD_P = remo_N * C3_P *remo_N.transpose();
                C3_Succ[i]= m_lambda.Sur_Success_rate(SD_P,&C3_Succ_PS[i],trans_para->Ps);
            }
            double Max_Success_rate = 0;int Max_Success_rate_N = 0;
            for(int i=0;i<C3_P_l;i++)
            {
                if(Max_Success_rate<C3_Succ[i])
                {
                    Max_Success_rate = C3_Succ[i];
                    C3_ref_sat=i;
                }
                if(Max_Success_rate_N<=C3_Succ_PS[i])
                {
                    if(Max_Success_rate_N==C3_Succ_PS[i])
                    {
                        if(C3_Succ[C3_ref_sat_N]<C3_Succ[i])
                        {
                            Max_Success_rate_N = C3_Succ_PS[i];
                            C3_ref_sat_N=i;
                        }
                        else
                        {
                            C3_ref_sat_N=Max_Success_rate_N;
                        }
                    }
                    else
                    {
                        Max_Success_rate_N = C3_Succ_PS[i];
                        C3_ref_sat_N=i;
                    }
                }
            }
        }
    }
    if(R_P_l!=0)    {
        R_P.resize(R_P_l,R_P_l);R_P.setZero(R_P_l,R_P_l);
        R_P_I.resize(Amb_len,Amb_len);R_P_I.setIdentity();
        descending_order(Rremo_N,Rremo_Sys);
        for(int i =0;i<Rremo_N;i++)  m_mat_cal.RemoveRow(R_P_I,Rremo_Sys[i]);
        R_P = R_P_I * Amb_P * R_P_I.transpose();
        if(R_P_l>1)
        {
            for(int i=0;i<R_P_l;i++)
            {
                MatrixXd SD_P,remo_N;
                remo_N.resize(R_P_l,R_P_l);remo_N.setIdentity();
                for(int j=0;j<R_P_l;j++)    remo_N(j,i) = -1;
                m_mat_cal.RemoveRow(remo_N,i);
                SD_P = remo_N * R_P *remo_N.transpose();
                R_Succ[i]= m_lambda.Sur_Success_rate(SD_P,&R_Succ_PS[i],trans_para->Ps);
            }
            double Max_Success_rate = 0;int Max_Success_rate_N = 0;
            for(int i=0;i<R_P_l;i++)
            {
                if(Max_Success_rate<R_Succ[i])
                {
                    Max_Success_rate = R_Succ[i];
                    R_ref_sat=i;
                }
                if(Max_Success_rate_N<=R_Succ_PS[i])
                {
                    if(Max_Success_rate_N==R_Succ_PS[i])
                    {
                        if(R_Succ[R_ref_sat_N]<R_Succ[i])
                        {
                            Max_Success_rate_N = R_Succ_PS[i];
                            R_ref_sat_N=i;
                        }
                        else
                        {
                            R_ref_sat_N=Max_Success_rate_N;
                        }
                    }
                    else
                    {
                        Max_Success_rate_N = R_Succ_PS[i];
                        R_ref_sat_N=i;
                    }
                }
            }
        }
    }
    if(E_P_l!=0)    {
        E_P.resize(E_P_l,E_P_l);E_P.setZero(E_P_l,E_P_l);
        E_P_I.resize(Amb_len,Amb_len);E_P_I.setIdentity();
        descending_order(Eremo_N,Eremo_Sys);
        for(int i =0;i<Eremo_N;i++)  m_mat_cal.RemoveRow(E_P_I,Eremo_Sys[i]);
        E_P = E_P_I * Amb_P * E_P_I.transpose();

        if(E_P_l>1)
        {
            for(int i=0;i<E_P_l;i++)
            {
                MatrixXd SD_P,remo_N;
                remo_N.resize(E_P_l,E_P_l);remo_N.setIdentity();
                for(int j=0;j<E_P_l;j++)    remo_N(j,i) = -1;
                m_mat_cal.RemoveRow(remo_N,i);
                SD_P = remo_N * E_P *remo_N.transpose();
                E_Succ[i]= m_lambda.Sur_Success_rate(SD_P,&E_Succ_PS[i],trans_para->Ps);
            }
            double Max_Success_rate = 0;int Max_Success_rate_N = 0;
            for(int i=0;i<E_P_l;i++)
            {
                if(Max_Success_rate<E_Succ[i])
                {
                    Max_Success_rate = E_Succ[i];
                    E_ref_sat=i;
                }
                if(Max_Success_rate_N<=E_Succ_PS[i])
                {
                    if(Max_Success_rate_N==E_Succ_PS[i])
                    {
                        if(E_Succ[E_ref_sat_N]<E_Succ[i])
                        {
                            Max_Success_rate_N = E_Succ_PS[i];
                            E_ref_sat_N=i;
                        }
                        else
                        {
                            E_ref_sat_N=Max_Success_rate_N;
                        }
                    }
                    else
                    {
                        Max_Success_rate_N = E_Succ_PS[i];
                        E_ref_sat_N=i;
                    }
                }
            }
        }
    }
    bool max_SRC ;
    max_SRC = false;
    int G_n,C2_n,C3_n,E_n,R_n;
    G_n=C2_n=C3_n=E_n=R_n=0;
    for(int i=0;i<Amb_len;i++)
    {
        SatlitData oneSatlitData = SatlitData_selec.at(i);
        switch (oneSatlitData.SatType) {
        case 'G':
            if(oneSatlitData.Par_sub&&oneSatlitData.EA[0]<30)   break;
            if(G_n == G_ref_sat&&max_SRC)
                max_suc_PRN[0] = oneSatlitData.PRN;
            else if(G_n == G_ref_sat_N)
                max_suc_PRN[0] = oneSatlitData.PRN;
            G_n++;
            break;
        case 'C':
            if(oneSatlitData.PRN<=16)
            {
                if(oneSatlitData.Par_sub&&oneSatlitData.EA[0]<30)   break;
                if(C2_n == C2_ref_sat&&max_SRC)
                    max_suc_PRN[1] = oneSatlitData.PRN;
                else if(C2_n == C2_ref_sat_N)
                    max_suc_PRN[1] = oneSatlitData.PRN;
                C2_n++;
            }
            else if(oneSatlitData.PRN>16)
            {
                if(oneSatlitData.Par_sub&&oneSatlitData.EA[0]<30)   break;
                if(C3_n == C3_ref_sat&&max_SRC)
                    max_suc_PRN[2] = oneSatlitData.PRN;
                else if(C3_n == C3_ref_sat_N)
                    max_suc_PRN[2] = oneSatlitData.PRN;
                C3_n++;
            }
            break;
        case 'R':
            if(oneSatlitData.Par_sub&&oneSatlitData.EA[0]<30)   break;
            if(R_n == R_ref_sat&&max_SRC)
                max_suc_PRN[3] = oneSatlitData.PRN;
            else if(R_n == R_ref_sat_N)
                max_suc_PRN[3] = oneSatlitData.PRN;
            R_n++;
            break;
        case 'E':
            if(oneSatlitData.Par_sub&&oneSatlitData.EA[0]<30)   break;
            if(E_n == E_ref_sat&&max_SRC)
                max_suc_PRN[4] = oneSatlitData.PRN;
            else if(E_n == E_ref_sat_N)
                max_suc_PRN[4] = oneSatlitData.PRN;
            E_n++;
            break;
        default:
            break;
        }
    }

}
void Ambiguity_resolution::Refe_sat_ADOP_selec(QVector< SatlitData > &SatlitData_selec,int *max_suc_PRN,MatrixXd &P,int const_parm)
{
    MatrixXd G_P,C2_P,C3_P,E_P,R_P;
    MatrixXd G_P_I,C2_P_I,C3_P_I,E_P_I,R_P_I,Amb_P;
    int G_P_l,C2_P_l,C3_P_l,E_P_l,R_P_l;
    int Amb_len = SatlitData_selec.length();
    Amb_P = P.block(const_parm,const_parm,Amb_len,Amb_len);
    G_P_l=C2_P_l=C3_P_l=E_P_l=R_P_l=0;
    int sys_GNSS[Amb_len]={0};
    int Gremo_Sys[Amb_len],C2remo_Sys[Amb_len],C3remo_Sys[Amb_len],Rremo_Sys[Amb_len],Eremo_Sys[Amb_len];
    int Gremo_N,C2remo_N,C3remo_N,Rremo_N,Eremo_N;
    Gremo_N=C2remo_N=C3remo_N=Rremo_N=Eremo_N=0;
    int G_ref_sat,C2_ref_sat,C3_ref_sat,R_ref_sat,E_ref_sat;
    int G_ref_sat_N,C2_ref_sat_N,C3_ref_sat_N,R_ref_sat_N,E_ref_sat_N;
    G_ref_sat_N=C2_ref_sat_N=C3_ref_sat_N=R_ref_sat_N=E_ref_sat_N=0;
    for(int i=0;i<Amb_len;i++)
    {
        SatlitData oneSatlitData = SatlitData_selec.at(i);
        switch (oneSatlitData.SatType) {
        case 'G':
            if(oneSatlitData.Par_sub)   {
                Gremo_Sys[Gremo_N]=i;C2remo_Sys[C2remo_N]=i;C3remo_Sys[C3remo_N]=i;Rremo_Sys[Rremo_N]=i;Eremo_Sys[Eremo_N]=i;
                Gremo_N++;C2remo_N++;C3remo_N++;Rremo_N++;Eremo_N++;
                break;}
            sys_GNSS[i]=SYS_GPS;
            G_P_l++;
            C2remo_Sys[C2remo_N]=i;C3remo_Sys[C3remo_N]=i;Rremo_Sys[Rremo_N]=i;Eremo_Sys[Eremo_N]=i;
            C2remo_N++;C3remo_N++;Rremo_N++;Eremo_N++;
            break;
        case 'C':
            if(oneSatlitData.PRN<=16)
            {
                if(oneSatlitData.Par_sub)   {
                    Gremo_Sys[Gremo_N]=i;C2remo_Sys[C2remo_N]=i;C3remo_Sys[C3remo_N]=i;Rremo_Sys[Rremo_N]=i;Eremo_Sys[Eremo_N]=i;
                    Gremo_N++;C2remo_N++;C3remo_N++;Rremo_N++;Eremo_N++;
                    break;}
                sys_GNSS[i]=SYS_BD2;
                C2_P_l++;
                Gremo_Sys[Gremo_N]=i;C3remo_Sys[C3remo_N]=i;Rremo_Sys[Rremo_N]=i;Eremo_Sys[Eremo_N]=i;
                Gremo_N++;C3remo_N++;Rremo_N++;Eremo_N++;
            }
            else if(oneSatlitData.PRN>16)
            {
                if(oneSatlitData.Par_sub)   {
                    Gremo_Sys[Gremo_N]=i;C2remo_Sys[C2remo_N]=i;C3remo_Sys[C3remo_N]=i;Rremo_Sys[Rremo_N]=i;Eremo_Sys[Eremo_N]=i;
                    Gremo_N++;C2remo_N++;C3remo_N++;Rremo_N++;Eremo_N++;
                    break;}
                sys_GNSS[i]=SYS_BD3;
                C3_P_l++;
                Gremo_Sys[Gremo_N]=i;C2remo_Sys[C2remo_N]=i;Rremo_Sys[Rremo_N]=i;Eremo_Sys[Eremo_N]=i;
                Gremo_N++;C2remo_N++;Rremo_N++;Eremo_N++;
            }
            break;
        case 'R':
            if(oneSatlitData.Par_sub)   {
                Gremo_Sys[Gremo_N]=i;C2remo_Sys[C2remo_N]=i;C3remo_Sys[C3remo_N]=i;Rremo_Sys[Rremo_N]=i;Eremo_Sys[Eremo_N]=i;
                Gremo_N++;C2remo_N++;C3remo_N++;Rremo_N++;Eremo_N++;
                break;}
            sys_GNSS[i]=SYS_GLO;
            R_P_l++;
            Gremo_Sys[Gremo_N]=i;C2remo_Sys[C2remo_N]=i;C3remo_Sys[C3remo_N]=i;Eremo_Sys[Eremo_N]=i;
            Gremo_N++;C2remo_N++;C3remo_N++;Eremo_N++;
            break;
        case 'E':
            if(oneSatlitData.Par_sub)   {
                Gremo_Sys[Gremo_N]=i;C2remo_Sys[C2remo_N]=i;C3remo_Sys[C3remo_N]=i;Rremo_Sys[Rremo_N]=i;Eremo_Sys[Eremo_N]=i;
                Gremo_N++;C2remo_N++;C3remo_N++;Rremo_N++;Eremo_N++;
                break;}
            sys_GNSS[i]=SYS_GAL;
            E_P_l++;
            Gremo_Sys[Gremo_N]=i;C2remo_Sys[C2remo_N]=i;C3remo_Sys[C3remo_N]=i;Rremo_Sys[Rremo_N]=i;
            Gremo_N++;C2remo_N++;C3remo_N++;Rremo_N++;
            break;
        default:
            break;
        }
    }
    double G_ADop[G_P_l],C2_ADop[C2_P_l],C3_ADop[C3_P_l],R_ADop[R_P_l],E_ADop[E_P_l];
    int G_ADop_PS[G_P_l],C2_ADop_PS[C2_P_l],C3_ADop_PS[C3_P_l],R_ADop_PS[R_P_l],E_ADop_PS[E_P_l];
    if(G_P_l!=0)    {
        G_P.resize(G_P_l,G_P_l);G_P.setZero(G_P_l,G_P_l);
        G_P_I.resize(Amb_len,Amb_len);G_P_I.setIdentity();
        descending_order(Gremo_N,Gremo_Sys);
        for(int i =0;i<Gremo_N;i++)  m_mat_cal.RemoveRow(G_P_I,Gremo_Sys[i]);
        G_P = G_P_I * Amb_P * G_P_I.transpose();

        if(G_P_l>1)
        {
            for(int i=0;i<G_P_l;i++)
            {
                MatrixXd SD_P,remo_N;
                remo_N.resize(G_P_l,G_P_l);remo_N.setIdentity();
                for(int j=0;j<G_P_l;j++)    remo_N(j,i) = -1;
                m_mat_cal.RemoveRow(remo_N,i);
                SD_P = remo_N * G_P *remo_N.transpose();
                G_ADop[i] = SD_P.determinant();
            }
            double Max_ADopess_rate = 10000;int Max_ADopess_rate_N = 0;
            for(int i=0;i<G_P_l;i++)
            {
                if(Max_ADopess_rate>G_ADop[i])
                {
                    Max_ADopess_rate = G_ADop[i];
                    G_ref_sat=i;
                }
                if(Max_ADopess_rate_N<=G_ADop_PS[i])
                {
                    if(Max_ADopess_rate_N==G_ADop_PS[i])
                    {
                        if(G_ADop[G_ref_sat_N]<G_ADop[i])
                        {
                            Max_ADopess_rate_N = G_ADop_PS[i];
                            G_ref_sat_N=i;
                        }
                        else
                        {
                            G_ref_sat_N=Max_ADopess_rate_N;
                        }
                    }
                    else
                    {
                        Max_ADopess_rate_N = G_ADop_PS[i];
                        G_ref_sat_N=i;
                    }
                }
            }
        }

    }
    if(C2_P_l!=0)    {
        C2_P.resize(C2_P_l,C2_P_l);C2_P.setZero(C2_P_l,C2_P_l);
        C2_P_I.resize(Amb_len,Amb_len);C2_P_I.setIdentity();
        descending_order(C2remo_N,C2remo_Sys);
        for(int i =0;i<C2remo_N;i++)  m_mat_cal.RemoveRow(C2_P_I,C2remo_Sys[i]);
        C2_P = C2_P_I * Amb_P * C2_P_I.transpose();

        if(C2_P_l>1)
        {
            for(int i=0;i<C2_P_l;i++)
            {
                MatrixXd SD_P,remo_N;
                remo_N.resize(C2_P_l,C2_P_l);remo_N.setIdentity();
                for(int j=0;j<C2_P_l;j++)    remo_N(j,i) = -1;
                m_mat_cal.RemoveRow(remo_N,i);
                SD_P = remo_N * C2_P *remo_N.transpose();
                C2_ADop[i] = SD_P.determinant();
            }
            double Max_ADopess_rate = 10000;int Max_ADopess_rate_N = 0;
            for(int i=0;i<C2_P_l;i++)
            {
                if(Max_ADopess_rate>C2_ADop[i])
                {
                    Max_ADopess_rate = C2_ADop[i];
                    C2_ref_sat=i;
                }
                if(Max_ADopess_rate_N<=C2_ADop_PS[i])
                {
                    if(Max_ADopess_rate_N==C2_ADop_PS[i])
                    {
                        if(C2_ADop[C2_ref_sat_N]<C2_ADop[i])
                        {
                            Max_ADopess_rate_N = C2_ADop_PS[i];
                            C2_ref_sat_N=i;
                        }
                        else
                        {
                            C2_ref_sat_N=Max_ADopess_rate_N;
                        }
                    }
                    else
                    {
                        Max_ADopess_rate_N = C2_ADop_PS[i];
                        C2_ref_sat_N=i;
                    }
                }
            }
        }
    }
    if(C3_P_l!=0)    {
        C3_P.resize(C3_P_l,C3_P_l);C3_P.setZero(C3_P_l,C3_P_l);
        C3_P_I.resize(Amb_len,Amb_len);C3_P_I.setIdentity();
        descending_order(C3remo_N,C3remo_Sys);
        for(int i =0;i<C3remo_N;i++)  m_mat_cal.RemoveRow(C3_P_I,C3remo_Sys[i]);
        C3_P = C3_P_I * Amb_P * C3_P_I.transpose();

        if(C3_P_l>1)
        {
            for(int i=0;i<C3_P_l;i++)
            {
                MatrixXd SD_P,remo_N;
                remo_N.resize(C3_P_l,C3_P_l);remo_N.setIdentity();
                for(int j=0;j<C3_P_l;j++)    remo_N(j,i) = -1;
                m_mat_cal.RemoveRow(remo_N,i);
                SD_P = remo_N * C3_P *remo_N.transpose();
                C3_ADop[i] = SD_P.determinant();
            }
            double Max_ADopess_rate = 10000;int Max_ADopess_rate_N = 0;
            for(int i=0;i<C3_P_l;i++)
            {
                if(Max_ADopess_rate>C3_ADop[i])
                {
                    Max_ADopess_rate = C3_ADop[i];
                    C3_ref_sat=i;
                }
                if(Max_ADopess_rate_N<=C3_ADop_PS[i])
                {
                    if(Max_ADopess_rate_N==C3_ADop_PS[i])
                    {
                        if(C3_ADop[C3_ref_sat_N]<C3_ADop[i])
                        {
                            Max_ADopess_rate_N = C3_ADop_PS[i];
                            C3_ref_sat_N=i;
                        }
                        else
                        {
                            C3_ref_sat_N=Max_ADopess_rate_N;
                        }
                    }
                    else
                    {
                        Max_ADopess_rate_N = C3_ADop_PS[i];
                        C3_ref_sat_N=i;
                    }
                }
            }
        }
    }
    if(R_P_l!=0)    {
        R_P.resize(R_P_l,R_P_l);R_P.setZero(R_P_l,R_P_l);
        R_P_I.resize(Amb_len,Amb_len);R_P_I.setIdentity();
        descending_order(Rremo_N,Rremo_Sys);
        for(int i =0;i<Rremo_N;i++)  m_mat_cal.RemoveRow(R_P_I,Rremo_Sys[i]);
        R_P = R_P_I * Amb_P * R_P_I.transpose();
        if(R_P_l>1)
        {
            for(int i=0;i<R_P_l;i++)
            {
                MatrixXd SD_P,remo_N;
                remo_N.resize(R_P_l,R_P_l);remo_N.setIdentity();
                for(int j=0;j<R_P_l;j++)    remo_N(j,i) = -1;
                m_mat_cal.RemoveRow(remo_N,i);
                SD_P = remo_N * R_P *remo_N.transpose();
                R_ADop[i] = SD_P.determinant();
            }
            double Max_ADopess_rate = 10000;int Max_ADopess_rate_N = 0;
            for(int i=0;i<R_P_l;i++)
            {
                if(Max_ADopess_rate>R_ADop[i])
                {
                    Max_ADopess_rate = R_ADop[i];
                    R_ref_sat=i;
                }
                if(Max_ADopess_rate_N<=R_ADop_PS[i])
                {
                    if(Max_ADopess_rate_N==R_ADop_PS[i])
                    {
                        if(R_ADop[R_ref_sat_N]<R_ADop[i])
                        {
                            Max_ADopess_rate_N = R_ADop_PS[i];
                            R_ref_sat_N=i;
                        }
                        else
                        {
                            R_ref_sat_N=Max_ADopess_rate_N;
                        }
                    }
                    else
                    {
                        Max_ADopess_rate_N = R_ADop_PS[i];
                        R_ref_sat_N=i;
                    }
                }
            }
        }
    }
    if(E_P_l!=0)    {
        E_P.resize(E_P_l,E_P_l);E_P.setZero(E_P_l,E_P_l);
        E_P_I.resize(Amb_len,Amb_len);E_P_I.setIdentity();
        descending_order(Eremo_N,Eremo_Sys);
        for(int i =0;i<Eremo_N;i++)  m_mat_cal.RemoveRow(E_P_I,Eremo_Sys[i]);
        E_P = E_P_I * Amb_P * E_P_I.transpose();

        if(E_P_l>1)
        {
            for(int i=0;i<E_P_l;i++)
            {
                MatrixXd SD_P,remo_N;
                remo_N.resize(E_P_l,E_P_l);remo_N.setIdentity();
                for(int j=0;j<E_P_l;j++)    remo_N(j,i) = -1;
                m_mat_cal.RemoveRow(remo_N,i);
                SD_P = remo_N * E_P *remo_N.transpose();
                E_ADop[i] = SD_P.determinant();
            }
            double Max_ADopess_rate = 1e16;int Max_ADopess_rate_N = 0;
            for(int i=0;i<E_P_l;i++)
            {
                if(Max_ADopess_rate>E_ADop[i])
                {
                    Max_ADopess_rate = E_ADop[i];
                    E_ref_sat=i;
                }
                if(Max_ADopess_rate_N<=E_ADop_PS[i])
                {
                    if(Max_ADopess_rate_N==E_ADop_PS[i])
                    {
                        if(E_ADop[E_ref_sat_N]<E_ADop[i])
                        {
                            Max_ADopess_rate_N = E_ADop_PS[i];
                            E_ref_sat_N=i;
                        }
                        else
                        {
                            E_ref_sat_N=Max_ADopess_rate_N;
                        }
                    }
                    else
                    {
                        Max_ADopess_rate_N = E_ADop_PS[i];
                        E_ref_sat_N=i;
                    }
                }
            }
        }
    }

    int G_n,C2_n,C3_n,E_n,R_n;
    G_n=C2_n=C3_n=E_n=R_n=0;
    for(int i=0;i<Amb_len;i++)
    {
        SatlitData oneSatlitData = SatlitData_selec.at(i);
        switch (oneSatlitData.SatType) {
        case 'G':
            if(oneSatlitData.Par_sub)   break;
            if(G_n == G_ref_sat)
                max_suc_PRN[0] = oneSatlitData.PRN;
            G_n++;
            break;
        case 'C':
            if(oneSatlitData.PRN<=16)
            {
                if(oneSatlitData.Par_sub)   break;
                if(C2_n == C2_ref_sat)
                    max_suc_PRN[1] = oneSatlitData.PRN;
                C2_n++;
            }
            else if(oneSatlitData.PRN>16)
            {
                if(oneSatlitData.Par_sub)   break;
                if(C3_n == C3_ref_sat)
                    max_suc_PRN[2] = oneSatlitData.PRN;
                C3_n++;
            }
            break;
        case 'R':
            if(oneSatlitData.Par_sub)   break;
            if(R_n == R_ref_sat)
                max_suc_PRN[3] = oneSatlitData.PRN;
            R_n++;
            break;
        case 'E':
            if(oneSatlitData.Par_sub)   break;
            if(E_n == E_ref_sat)
                max_suc_PRN[4] = oneSatlitData.PRN;
            E_n++;
            break;
        default:
            break;
        }
    }

}
//Smoothing wide lane ambiguity and fixing single difference wide lane ambiguity between satellites szg
void Ambiguity_resolution::Wide_lane_smoothing(QVector< SatlitData > &prevEpochSatlitData,QVector< SatlitData > &epochSatlitData, para_trans *trans_para)
{
    int preEpochLen = prevEpochSatlitData.length();
    if(preEpochLen==0||trans_para->cyclyslip_sat_n>=8)
    {
        SD_hold prehold;
        SD_ARholdNew = prehold;
        trans_para->ambiguity_fix =true;
        pos_ephon = 0;
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
        if(Pre_prn!=-1&&currentEpoSat.cycly_slipF!=1&&currentEpoSat.Domain_of_doubt!=-1)//&&currentEpoSat.Domain_of_doubt!=-1
        {
            SatlitData preEpoSat = prevEpochSatlitData.at(Pre_prn);
            if(preEpoSat.Domain_of_doubt!=-1)
            {
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

        }
        else
        {
            SmoothSatlit = 0;
            SmoothStd    = 0;
            SmoEpNum     = 1;//smooth epo
        }
        /*WL  Reducing power*/
        double variance_coeff_P = sqrt(SQR(currentEpoSat.Frq[currentEpoSat.IF_Frq[0]])+SQR(currentEpoSat.Frq[currentEpoSat.IF_Frq[1]]))
                /(currentEpoSat.Frq[currentEpoSat.IF_Frq[0]]+currentEpoSat.Frq[currentEpoSat.IF_Frq[1]]);
        double variance_coeff_L = sqrt(SQR(currentEpoSat.Frq[currentEpoSat.IF_Frq[0]])+SQR(currentEpoSat.Frq[currentEpoSat.IF_Frq[1]]))
                /(currentEpoSat.Frq[currentEpoSat.IF_Frq[0]]-currentEpoSat.Frq[currentEpoSat.IF_Frq[1]]);
        double v_c_1 = variance_coeff_L*sqrt(1.0/(currentEpoSat.SatWight*currentEpoSat.SatWight_L*currentEpoSat.SatWight_IF[0]))+variance_coeff_P*sqrt(1.0/(currentEpoSat.SatWight*currentEpoSat.SatWight_IF[1]));
        double v_c_2 = variance_coeff_L*sqrt(1.0/(currentEpoSat.SatWight*currentEpoSat.SatWight_L))+variance_coeff_P*sqrt(1.0/(currentEpoSat.SatWight));
        double V_c_LP = 1.0;
        if(SmoEpNum>1&&trans_para->Partial_subset==_weight_ranking)
            V_c_LP=v_c_2/v_c_1;
        currentEpoSat.continue_eph = SmoEpNum;
        SDWlar = currentEpoSat.MW_Wl_Int;/*MW wide roadway ambiguity*/
        currentEpoSat.satMW_wlAmbi[0] = SmoothSatlit*(SmoEpNum -V_c_LP)/SmoEpNum + SDWlar*(V_c_LP)/SmoEpNum;//平滑后宽巷模糊度
        /*WL  Reducing power*/
        if(SmoEpNum == 1)
            currentEpoSat.std_devia =  (SDWlar - (int)SDWlar)*(SDWlar - (int)SDWlar) * 1.0/SmoEpNum;
        else
            currentEpoSat.std_devia =  SmoothStd * (SmoEpNum -1)/SmoEpNum + (currentEpoSat.satMW_wlAmbi[0] - SDWlar)*(currentEpoSat.satMW_wlAmbi[0] - SDWlar) * 1.0/SmoEpNum;
        currentEpoSat.satMW_wlAmbi[1] = currentEpoSat.std_devia/SmoEpNum;
        currentEpoSat.satMW_wlSmoN = SmoEpNum;
        if(SmoEpNum<3)
            currentEpoSat.Par_sub = true;

        tempEpochSatlitData.append(currentEpoSat);
    }
    epochSatlitData.clear();
    epochSatlitData = tempEpochSatlitData;
    return ;
}
//Smoothing wide lane ambiguity and fixing single difference wide lane ambiguity between satellites szg
int Ambiguity_resolution::getSD_SatWl(QVector< SatlitData > &epochSatlitData,int *MAX_EAprn,int *Par_sat_epo)
{
    int epochLen = epochSatlitData.length();
    if(epochLen == 0) return 0;

    double maxEaWL[10]={0};int smooth_len[5] = {0};
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
                maxEaWL[1] = currentEpoSat.satMW_wlAmbi[1];
                smooth_len[0] = currentEpoSat.satMW_wlSmoN;
            }
            break;
        case 'C':
            if(currentEpoSat.PRN<=16)
            {
                if(currentEpoSat.PRN == MAX_EAprn[1])
                {
                    maxEaWL[2] = currentEpoSat.satMW_wlAmbi[0];
                    maxEaWL[3] = currentEpoSat.satMW_wlAmbi[1];
                    smooth_len[1] = currentEpoSat.satMW_wlSmoN;
                }
            }
            else
            {
                if(currentEpoSat.PRN == MAX_EAprn[2])
                {
                    maxEaWL[4] = currentEpoSat.satMW_wlAmbi[0];
                    maxEaWL[5] = currentEpoSat.satMW_wlAmbi[1];
                    smooth_len[2] = currentEpoSat.satMW_wlSmoN;
                }
            }
            break;
        case 'R':
            if(currentEpoSat.PRN == MAX_EAprn[3])
            {
                maxEaWL[6] = currentEpoSat.satMW_wlAmbi[0];
                maxEaWL[7] = currentEpoSat.satMW_wlAmbi[1];
                smooth_len[3] = currentEpoSat.satMW_wlSmoN;
            }
            break;
        case 'E':
            if(currentEpoSat.PRN == MAX_EAprn[4])
            {
                maxEaWL[8] = currentEpoSat.satMW_wlAmbi[0];
                maxEaWL[9] = currentEpoSat.satMW_wlAmbi[1];
                smooth_len[4] = currentEpoSat.satMW_wlSmoN;
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
        double SYSmaxEaWL[2] ={0};int Smooth_lenth = 0;
        switch (Satlit_SD.SatType) {
        case 'G':
            SYS_PRN = MAX_EAprn[0] ;
            SYSmaxEaWL[0] = maxEaWL[0];
            SYSmaxEaWL[1] = maxEaWL[1];
            Smooth_lenth = smooth_len[0];
            break;
        case 'C':
            if(Satlit_SD.PRN<=16)
            {
                SYS_PRN = MAX_EAprn[1] ;
                SYSmaxEaWL[0] = maxEaWL[2];
                SYSmaxEaWL[1] = maxEaWL[3];
                Smooth_lenth = smooth_len[1];
            }
            else
            {
                SYS_PRN = MAX_EAprn[2] ;
                SYSmaxEaWL[0] = maxEaWL[4];
                SYSmaxEaWL[1] = maxEaWL[5];
                Smooth_lenth = smooth_len[2];
            }
            break;
        case 'R':
            SYS_PRN = MAX_EAprn[3] ;
            SYSmaxEaWL[0] = maxEaWL[6];
            SYSmaxEaWL[1] = maxEaWL[7];
            Smooth_lenth = smooth_len[3];
            break;
        case 'E':
            SYS_PRN = MAX_EAprn[4] ;
            SYSmaxEaWL[0] = maxEaWL[8];
            SYSmaxEaWL[1] = maxEaWL[9];
            Smooth_lenth = smooth_len[4];
            break;
        default:
            break;
        }

        if(Satlit_SD.PRN != SYS_PRN)//&&Satlit_SD.Domain_of_doubt!=-1
        {
            double SDSmoothSatlit = 0.0,SD_std_deviation=0.0;int SD_WlFixs = 0;int Smooth_n=0;
            bool flagP;
            SDSmoothSatlit = Satlit_SD.satMW_wlAmbi[0] - SYSmaxEaWL[0];//Ambiguity of single difference wide roadway between stars
            Satlit_SD.SD_Wl_float = SDSmoothSatlit;
            if(Satlit_SD.satMW_wlSmoN>Smooth_lenth)
                Smooth_n = Smooth_lenth;
            else
                Smooth_n = Satlit_SD.satMW_wlSmoN;
            SD_std_deviation =  (SDSmoothSatlit - Float2Int(SDSmoothSatlit))*(SDSmoothSatlit - Float2Int(SDSmoothSatlit)) * 1.0/Smooth_n;
//            SD_std_deviation = Satlit_SD.satMW_wlAmbi[1] + SYSmaxEaWL[1];
            Satlit_SD.maxEA_PRN = SYS_PRN;
            if(flagP = Probability_Test(SD_WlFixs,SDSmoothSatlit,SD_std_deviation,Satlit_SD.Lambda_Wl))
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
                /*Setting when selecting reference star with partial ambiguity fixed*/
//                if(Satlit_SD.SatType=='G'&&Satlit_SD.EA[0]>20)
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
/**Floating point solution of ambiguity in IF combined narrow lane*****/
bool Ambiguity_resolution::NarrowLaneAR(QVector< SatlitData > &epochSatlitDatanl,VectorXd &X, const corr_opt *opt_corr, para_trans *trans_para)
{//
    int epoLen = epochSatlitDatanl.length();
    double IF_N[5] = {0.0},maxEAnlFCBs[5] = {0.0};
    double S_if_Am = 0.0,D_if_Am = 0.0;
    for(int i=0;i<10;i++)
    {
        m_SYStem_satN[i] = 0;
    }
    int nlepo[5] = {0};
    for(int i=0;i<5;i++)
    {
        nlepo[i] = -1;
    }
    QVector< SatlitData > epochSatlitDataTemp;
    for (int epo=0;epo<epoLen;epo++)
    {
        SatlitData oneSatlitData = epochSatlitDatanl.at(epo);
        if(oneSatlitData.PRN == oneSatlitData.maxEA_PRN)
        {
            switch (oneSatlitData.SatType) {
            case 'G':
                IF_N[0]= X(4+trans_para->sys_num+epo);
                maxEAnlFCBs[0] = oneSatlitData.MW_nlfcbs;
                nlepo[0] = epo;
                break;
            case 'C':
                if(oneSatlitData.PRN<=16)
                {
                    IF_N[1]= X(4+trans_para->sys_num+epo);
                    maxEAnlFCBs[1] = oneSatlitData.MW_nlfcbs;
                    nlepo[1] = epo;
                }
                else
                {
                    IF_N[2]= X(4+trans_para->sys_num+epo);
                    maxEAnlFCBs[2] = oneSatlitData.MW_nlfcbs;
                    nlepo[2] = epo;
                }
                break;
            case 'R':
                IF_N[3]= X(4+trans_para->sys_num+epo);
                maxEAnlFCBs[3] = oneSatlitData.MW_nlfcbs;
                nlepo[3] = epo;
                break;
            case 'E':
                IF_N[4]= X(4+trans_para->sys_num+epo);
                maxEAnlFCBs[4] = oneSatlitData.MW_nlfcbs;
                nlepo[4] = epo;
                break;
            default:
                break;
            }

        }
    }

    for (int epo=0;epo<epoLen;epo++)
    {
        SatlitData oneSatlitData2 = epochSatlitDatanl.at(epo);
        int fre_num0=oneSatlitData2.IF_Frq[0],fre_num1=oneSatlitData2.IF_Frq[1];
        double F1 = oneSatlitData2.Frq[fre_num0],F2 = oneSatlitData2.Frq[fre_num1];//Get the frequency of this satellite
//        double F1 = oneSatlitData2.Frq[0],F2 = oneSatlitData2.Frq[1];//Get the frequency of this satellite
//        double Lamta1 = M_C/F1,Lamta2 = M_C/F2;
//        double lamdaIFwl = M_C*F1/(F1*F1 - F2*F2);
//        double lamdaIFnl = M_C/(F1+F2);
        double lamdaIF_Wl = F2/(F1 - F2);
        double IF_lamda = M_C/(F1 + F2);
        double SD_Nl_Fcbs = 0.0;
        S_if_Am = X(4+trans_para->sys_num+epo);
        if(oneSatlitData2.PRN != oneSatlitData2.maxEA_PRN)
        {
            switch (oneSatlitData2.SatType) {
            case 'G':
                if(opt_corr->treat_method==POS_PP_PPP_AR_2)
                    SD_Nl_Fcbs = oneSatlitData2.MW_nlfcbs - maxEAnlFCBs[0];//Inter satellite single difference FCB
                oneSatlitData2.SD_Nl_Float = (S_if_Am - IF_N[0]) + lamdaIF_Wl *oneSatlitData2.SD_Wl_Int + SD_Nl_Fcbs;//szg debug 0303
                oneSatlitData2.SD_nl_FCBs  = SD_Nl_Fcbs;
                m_SYStem_satN[0]++;
                break;
            case 'C':
                if(oneSatlitData2.PRN<=16)
                {
                    if(opt_corr->treat_method==POS_PP_PPP_AR_2)
                        SD_Nl_Fcbs = oneSatlitData2.MW_nlfcbs - maxEAnlFCBs[1];//Inter satellite single difference FCB
                    oneSatlitData2.SD_Nl_Float = (S_if_Am - IF_N[1]) + lamdaIF_Wl *oneSatlitData2.SD_Wl_Int + SD_Nl_Fcbs;// szg debug 0303
                    oneSatlitData2.SD_nl_FCBs  = SD_Nl_Fcbs;
                    m_SYStem_satN[1]++;
                }
                else
                {
                    if(opt_corr->treat_method==POS_PP_PPP_AR_2)
                        SD_Nl_Fcbs = oneSatlitData2.MW_nlfcbs - maxEAnlFCBs[2];//Inter satellite single difference FCB
                    oneSatlitData2.SD_Nl_Float = (S_if_Am - IF_N[2]) + lamdaIF_Wl *oneSatlitData2.SD_Wl_Int + SD_Nl_Fcbs;// szg debug 0303
                    oneSatlitData2.SD_nl_FCBs  = SD_Nl_Fcbs;
                    m_SYStem_satN[2]++;
                }
                break;
            case 'R':
                if(opt_corr->treat_method==POS_PP_PPP_AR_2)
                    SD_Nl_Fcbs = oneSatlitData2.MW_nlfcbs - maxEAnlFCBs[3];//Inter satellite single difference FCB
                oneSatlitData2.SD_Nl_Float = (S_if_Am - IF_N[3]) + lamdaIF_Wl *oneSatlitData2.SD_Wl_Int + SD_Nl_Fcbs;//  szg debug 0303
                oneSatlitData2.SD_nl_FCBs  = SD_Nl_Fcbs;
                m_SYStem_satN[3]++;
                break;
            case 'E':
                if(opt_corr->treat_method==POS_PP_PPP_AR_2)
                    SD_Nl_Fcbs = oneSatlitData2.MW_nlfcbs - maxEAnlFCBs[4];//Inter satellite single difference FCB
                oneSatlitData2.SD_Nl_Float = (S_if_Am - IF_N[4]) + lamdaIF_Wl *oneSatlitData2.SD_Wl_Int + SD_Nl_Fcbs;// szg debug 0303
                oneSatlitData2.SD_nl_FCBs  = SD_Nl_Fcbs;
                m_SYStem_satN[4]++;
                break;
            default:
                break;
            }

//            if(fabs(oneSatlitData2.SD_Nl_Float) > 0.01)
//                oneSatlitData2.SD_Nl_fix = Float2Int(oneSatlitData2.SD_Nl_Float);
        }

        epochSatlitDataTemp.append(oneSatlitData2);
    }
    for(int i=0;i<5;i++)
    {
        maxEAepo[i]   = nlepo[i];
    }

    epochSatlitDatanl = epochSatlitDataTemp;

    return true;
}

bool Ambiguity_resolution::unfixed_sat(int satellite_len,int *Par_sat_epo, para_trans *trans_para)
{
    double residual_max=0;int max_par = 0;int delet_amb_n=0;
    for(int i=0;i<satellite_len;i++)
    {
        if(Par_sat_epo[i]==0)
        {
            double L_result = fabs(trans_para->posterior_V(2*i)/(trans_para->sigma0*sqrt(trans_para->Residual_Qvv(2*i,2*i))));
            if(L_result>residual_max)
            {
                residual_max = L_result;
                max_par=i;    
            }
            delet_amb_n++;
        }
    }
    Par_sat_epo[max_par]=satellite_len-delet_amb_n+2;

}
bool Ambiguity_resolution::post_resuduala(int satellite_len,int *Par_sat_epo, para_trans *trans_para)
{
    double residual_max=0;int max_par = 0;int delet_amb_n = 0;
    for(int i=0;i<satellite_len;i++)
    {
        if(Par_sat_epo[i]==0)
        {
            double L_result = fabs(trans_para->posterior_V(2*i));
            if(L_result>residual_max)
            {
                residual_max = L_result;
                max_par=i;
            }
            delet_amb_n++;
        }
    }
    Par_sat_epo[max_par]=satellite_len-delet_amb_n+2;

}
void Ambiguity_resolution::angle_altitude_screen(QVector< SatlitData > &epochSatlitDataLa,int *Par_sat_epo)
{
    double min_angle=90;int min_par = 0;int delet_amb_n = 0;
    int epoLen = epochSatlitDataLa.length();
    for(int i=0;i<epoLen;i++)
    {
        SatlitData epoch_angle = epochSatlitDataLa.at(i);
        if(Par_sat_epo[i]==0)
        {
            double angle_sat = epoch_angle.EA[0];
            if(angle_sat<min_angle)
            {
                min_angle = angle_sat;
                min_par=i;
            }
            delet_amb_n++;
        }
    }
    Par_sat_epo[min_par]=epoLen-delet_amb_n+2;
}
void Ambiguity_resolution::delet_bias_SSD_nl(QVector< SatlitData > &epochSatlitDataLa,int *Par_sat_epo,VectorXd UDUC_SD_X,double bias_nl)
{
    int delet_amb_n = 0;
    int data_SSD_s=0;
    int epoLen=epochSatlitDataLa.length();
    for(int i=0;i<epoLen;i++)
    {
        SatlitData epoch_angle = epochSatlitDataLa.at(i);
        if(epoch_angle.PRN==epoch_angle.maxEA_PRN)  continue;
        if(Par_sat_epo[i]==0)
        {
            double delet_bias = UDUC_SD_X(data_SSD_s);
            double nl_bias = fabs(delet_bias-Float2Int(delet_bias));
            if(nl_bias>bias_nl)
            {
                Par_sat_epo[i]=epoLen-delet_amb_n+2;
                delet_amb_n++;
            }
            data_SSD_s++;
        }
    }
}
void Ambiguity_resolution::elevation_is_less_than_30(QVector< SatlitData > &epochSatlitDataLa,int *Par_sat_epo,bool &EA_minsat)
{
    double min_angle=30;
    EA_minsat = false;
    int epoLen = epochSatlitDataLa.length();
    for(int i=0;i<epoLen;i++)
    {
        SatlitData epoch_angle = epochSatlitDataLa.at(i);
        if(Par_sat_epo[i]==0)
        {
            double angle_sat = epoch_angle.EA[0];
            if(angle_sat<min_angle)
            {
                EA_minsat = true;
            }
        }
    }
}

void Ambiguity_resolution::weight_ordering(QVector< SatlitData > &epochSatlitDataLa,int *Par_sat_epo)
{
    double min_weight=1e8;int min_par = 0;int delet_amb_n=0;
    int epoLen = epochSatlitDataLa.length();
    for(int i=0;i<epoLen;i++)
    {
        SatlitData epoch_weight = epochSatlitDataLa.at(i);
        if(Par_sat_epo[i]==0)
        {
            double weight_sat = epoch_weight.SatWight*epoch_weight.SatWight_IF[0];
            if(weight_sat<min_weight)
            {
                min_weight = weight_sat;
                min_par=i;
            }
            delet_amb_n++;
        }
    }
    Par_sat_epo[min_par]=epoLen-delet_amb_n+2;
}
void Ambiguity_resolution::Adop_screen(QVector< SatlitData > &epochSatlitDataLa,int *Par_sat_epo,MatrixXd &La_SDP)
{
    double min_adop=0;int min_adop_i = 0;int min_adop_BDS2 = -1;
    bool first_eph = true;
    int epoLen = epochSatlitDataLa.length();
    ADop_serceen.resize(epoLen);
    ADop_serceen.setZero(epoLen);
    int Adop_pos = 0;
    for(int i=0;i<epoLen;i++)
    {
        if(Par_sat_epo[i]==0)
        {
            SatlitData BDS2_sat =  epochSatlitDataLa.at(i);
            MatrixXd Adop_mat = La_SDP;
            m_mat_cal.RemoveRow(Adop_mat,Adop_pos);
            m_mat_cal.RemoveColumn(Adop_mat,Adop_pos);
            ADop_serceen(i) = Adop_mat.determinant();
            if(first_eph)
            {
                min_adop = ADop_serceen(i);
                if(BDS2_sat.BDS_2flag)
                    min_adop_BDS2 = i;
                else
                    min_adop_i = i;
                first_eph=false;
            }
            else if(min_adop>ADop_serceen(i))
            {
                min_adop =ADop_serceen(i);
                if(BDS2_sat.BDS_2flag)
                    min_adop_BDS2 = i;
                else
                    min_adop_i = i;
            }
            Adop_pos++;

        }
        else
            ADop_serceen(i)=-1;
    }
    if(min_adop_BDS2==-1)
        Par_sat_epo[min_adop_i]=epoLen-Adop_pos+2;
    else
        Par_sat_epo[min_adop_BDS2]=epoLen-Adop_pos+2;
}
void Ambiguity_resolution::Boost_Parsub(QVector< SatlitData > &epochSatlitDataLa,int *Par_sat_epo,MatrixXd &La_SDP)
{
    int min_par=0;
    double Ps=0;
    double dit_p[La_SDP.cols()],dit_p_[La_SDP.cols()];
    double max_dit = 1.1;
    if(m_lambda.bootstrapping(dit_p,La_SDP))
        return ;
    int epoLen = epochSatlitDataLa.length();
    int Adit_pos = 0,dite_pos=0;
    for(int i=0;i<epoLen;i++)
    {
        if(Par_sat_epo[i]==0)
        {
            double dit = dit_p[Adit_pos];
            dit_p_[Adit_pos]=dit;
            if(max_dit>dit)
            {
                max_dit = dit;
                min_par=i;
                dite_pos = Adit_pos;
            }
            Adit_pos++;
        }
    }
    dit_p_[dite_pos] = 1;
    Ps = m_mat_cal.prod(dit_p_,La_SDP.cols());
    Par_sat_epo[min_par]=epoLen-Adit_pos+2;
}
void Ambiguity_resolution::Diagonal_variance(QVector< SatlitData > &epochSatlitDataLa,int *Par_sat_NL_N,MatrixXd &AP_k)
{

    int epoLen = epochSatlitDataLa.length();
    double max_var = -1;int max_var_pos = -1;int Adit_pos = 0;
    for(int i=0;i<epoLen;i++)
    {
        SatlitData Sat_dv =  epochSatlitDataLa.at(i);
        if(Par_sat_NL_N[i]==0&&Sat_dv.PRN!=Sat_dv.maxEA_PRN)
        {
            if(max_var<AP_k(Adit_pos,Adit_pos))
            {
                max_var = AP_k(Adit_pos,Adit_pos);
                max_var_pos = i;
            }
            Adit_pos++;
        }
    }
    if(max_var_pos!=-1)
    {
        Par_sat_NL_N[max_var_pos]  = 1;
    }
}
void Ambiguity_resolution::Standard_of_success_rate(QVector< SatlitData > &epochSatlitDataLa,int *Par_sat_epo,MatrixXd &La_SDP,double P_sr)
{
    int min_par=0;
    double Ps=0;
    double dit_p[La_SDP.cols()],dit_p_[La_SDP.cols()];
    double max_dit = 1.1;
    if(m_lambda.bootstrapping(dit_p,La_SDP))
        return ;
    int epoLen = epochSatlitDataLa.length();
    for(int j=0;j<epoLen;j++)
    {
        int Adit_pos = 0,dite_pos=0;
        for(int i=0;i<epoLen;i++)
        {
            if(Par_sat_epo[i]==0)
            {
                double dit = dit_p[Adit_pos];
                dit_p_[Adit_pos]=dit;
                if(max_dit>dit)
                {
                    max_dit = dit;
                    min_par=i;
                    dite_pos = Adit_pos;
                }
                Adit_pos++;
            }
        }
        dit_p_[dite_pos] = 1;
        Ps = m_mat_cal.prod(dit_p_,La_SDP.cols());
        if(Ps>P_sr) break;
        Par_sat_epo[min_par]=epoLen-Adit_pos+2;
    }

}

bool Ambiguity_resolution::Par_sub_amb(QVector< SatlitData > &epochSatlitDataLa,SmoothData &SD_IFsmoot,MatrixXd &P,MatrixXd &La_SDP,MatrixXd &Qab,MatrixXd &Trans_SD,
                                       VectorXd &SD_GNSS_F,VectorXd &La_SDX,VectorXd &SD_nl_fcb,int *Par_sat_epo,int *maxEASat_Prn)
{//
    int epoLen = epochSatlitDataLa.length();
    int eponumber = P.rows() - epoLen,SysSat_nFalg = 0;
    SD_hold SD_ARhold;//szg debug
    for(int i=0;i<5;i++)
    {
        if(m_SYStem_satN[i]>1)
        {
            SysSat_nFalg++;
        }
    }
    VectorXd SD_X,SD_Wl,Trans_G,Trans_C,Trans_BDS2,Trans_R,Trans_E;MatrixXd SD_P,Trans_matrix;
    VectorXd SD_NLfcb;
    SD_NLfcb.resize(epoLen);SD_NLfcb.setZero(epoLen);
    SD_nl_fcb.resize(epoLen - SysSat_nFalg);SD_nl_fcb.setZero(epoLen - SysSat_nFalg);
    SD_X.resize(epoLen);La_SDX.resize(epoLen - SysSat_nFalg);SD_P.resize(epoLen,epoLen);La_SDP.resize(epoLen - SysSat_nFalg,epoLen - SysSat_nFalg);
    SD_X.setZero(epoLen);La_SDX.setZero(epoLen - SysSat_nFalg);SD_P.setZero(epoLen,epoLen);La_SDP.setZero(epoLen - SysSat_nFalg,epoLen - SysSat_nFalg);
    Qab.resize(eponumber,epoLen - SysSat_nFalg);Qab.setZero(eponumber,epoLen - SysSat_nFalg);//Qab
    Trans_matrix.resize(epoLen - SysSat_nFalg,epoLen);Trans_matrix.setZero(epoLen - SysSat_nFalg,epoLen);
//    SD_IFsmoot.Fix_a.resize(epoLen - SysSat_nFalg);SD_IFsmoot.Fix_a.setZero(epoLen - SysSat_nFalg);/*Fixed single-difference ambiguity between satellites*/
//    SD_IFsmoot.Flo_a.resize(epoLen - SysSat_nFalg);SD_IFsmoot.Flo_a.setZero(epoLen - SysSat_nFalg);/*Single-difference ambiguity between floating-point satellites*/
//    SD_IFsmoot.Fix_NL.resize(epoLen - SysSat_nFalg);SD_IFsmoot.Fix_NL.setZero(epoLen - SysSat_nFalg);//Fixed narrow lane ambiguity
    SD_IFsmoot.Fix_Qab.resize(eponumber,epoLen - SysSat_nFalg);SD_IFsmoot.Fix_Qab.setZero(eponumber,epoLen - SysSat_nFalg);//Matrix Qab
    SD_IFsmoot.Fix_Qa.resize(epoLen - SysSat_nFalg,epoLen - SysSat_nFalg);SD_IFsmoot.Fix_Qa.setZero(epoLen - SysSat_nFalg,epoLen - SysSat_nFalg);//Matrix Qab
    SD_Wl.resize(epoLen);SD_Wl.setZero(epoLen);
    //Inter satellite single difference conversion vector of each system
    Trans_G.resize(epoLen);Trans_G.setZero(epoLen);
    Trans_C.resize(epoLen);Trans_C.setZero(epoLen);
    Trans_BDS2.resize(epoLen);Trans_BDS2.setZero(epoLen);

    Trans_R.resize(epoLen);Trans_R.setZero(epoLen);
    Trans_E.resize(epoLen);Trans_E.setZero(epoLen);
    Trans_SD.resize(epoLen,epoLen);Trans_SD.setIdentity();
//    Trans_SD1.resize(epoLen-SysSat_nFalg,epoLen);Trans_SD1.setZero(epoLen-SysSat_nFalg,epoLen);
    int maxEAPRNepo[5] = {-1},sortMaxEpo[5] = {-1};
    for(int i=0;i<5;i++)//m_sys_str
    {
        maxEAPRNepo[i]   = maxEAepo[i];
        sortMaxEpo[i]    = maxEAepo[i];
//        if(maxEAPRNepo[i]!=0)
//        maxEAepo[i]      = -1;
        part_Maxprn[i]   = maxEASat_Prn[i];
    }
    int sysTemFlag[5] = {0},las_n=0,remo_Sys[epoLen] = {0},
            remo_N =0,sd_fix=0,sd_fix_C=0,sd_fix_R=0,sd_fix_E=0;
    for(int i=0;i<epoLen;i++)    remo_Sys[i] = -1;
    double residual_amb = 0.5;
    VectorXd GNSS_epoF;
    GNSS_epoF.resize(epoLen);SD_GNSS_F.resize(epoLen - SysSat_nFalg);
    GNSS_epoF.setZero(epoLen);SD_GNSS_F.setZero(epoLen - SysSat_nFalg);
    for(int epo=0;epo<epoLen;epo++)
    {
        SatlitData Trans_Sat = epochSatlitDataLa.at(epo);
        int fre_num0=Trans_Sat.IF_Frq[0],fre_num1=Trans_Sat.IF_Frq[1];
        double F1 = Trans_Sat.Frq[fre_num0],F2 = Trans_Sat.Frq[fre_num1];//Get the frequency of this satellite
//        double F1 = Trans_Sat.Frq[0],F2 = Trans_Sat.Frq[1];//Get the frequency of this satellite
        double lamdaIF_Wl = F2/(F1 - F2);
        switch (Trans_Sat.SatType) {
        case 'G':
            Trans_G(epo) = -1;//maxEAPRNepo[0]
            if(maxEAPRNepo[0]!=epo)
            {
                GNSS_epoF(epo) = 1;
                SD_Wl(epo)  = lamdaIF_Wl * Trans_Sat.SD_Wl_Int;
                SD_X(epo) = Trans_Sat.SD_Nl_Float;
                SD_NLfcb(epo) = Trans_Sat.SD_nl_FCBs;
                las_n++;
                if(Par_sat_epo[epo]!=0)
                {
                    remo_Sys[remo_N] = epo;
                    remo_N++;
                }
                else
                {
                    SD_ARhold.G_hold[sd_fix].PRNhf = Trans_Sat.PRN;
                    if(Trans_Sat.cycly_slipF!=1&&fabs(Trans_Sat.SD_Nl_Float-Float2Int(Trans_Sat.SD_Nl_Float))<residual_amb&&!Trans_Sat.Par_sub)//&&!Trans_Sat.Par_sub
                    {
                        SD_ARhold.G_hold[Trans_Sat.PRN-1].Sd_flag = 1;
                        sd_fix++;
                    }
                    else
                    {
                        Par_sat_epo[epo] = -1;
                        remo_Sys[remo_N] = epo;
                        remo_N++;
                        SD_ARhold.G_hold[Trans_Sat.PRN-1].Sd_flag = 0;
                        sd_fix++;
                    }
                }
            }
            else
            {
                GNSS_epoF(epo) = 0;
                if(Trans_Sat.cycly_slipF!=1&&fabs(Trans_Sat.SD_Nl_Float-Float2Int(Trans_Sat.SD_Nl_Float))<residual_amb&&!Trans_Sat.Par_sub)//&&!Trans_Sat.Par_sub
                {
                    SD_ARhold.G_hold[Trans_Sat.PRN-1].Sd_flag = 1;
                }
                else
                {
                    SD_ARhold.G_hold[Trans_Sat.PRN-1].Sd_flag = 0;
                }
                SD_ARhold.Sd_maxEA_prn[0] = part_Maxprn[0];
                remo_Sys[remo_N] = epo;
                remo_N++;
            }
            sysTemFlag[0] = 1;
            break;
        case 'C':
            if(Trans_Sat.PRN<=16)
            {
                Trans_BDS2(epo) = -1;
                if(maxEAPRNepo[1]!=epo)
                {
                    GNSS_epoF(epo) = 2;
    //                La_SDX(las_n) = Trans_Sat.SD_Nl_Float;
                    SD_Wl(epo)  = lamdaIF_Wl * Trans_Sat.SD_Wl_Int;
                    SD_X(epo) = Trans_Sat.SD_Nl_Float;
                    SD_NLfcb(epo) = Trans_Sat.SD_nl_FCBs;
                    las_n++;
                    if(Par_sat_epo[epo]!=0)
                    {
                        remo_Sys[remo_N] = epo;
                        remo_N++;
                    }
                    else
                    {
                        SD_ARhold.C_hold[sd_fix_C].PRNhf = Trans_Sat.PRN;
                        if(Trans_Sat.cycly_slipF!=1&&fabs(Trans_Sat.SD_Nl_Float-Float2Int(Trans_Sat.SD_Nl_Float))<residual_amb&&!Trans_Sat.Par_sub)//&&!Trans_Sat.Par_sub
                        {
                            SD_ARhold.C_hold[Trans_Sat.PRN-1].Sd_flag = 1;
                            sd_fix_C++;
                        }
                        else
                        {
                            Par_sat_epo[epo] = -1;
                            remo_Sys[remo_N] = epo;
                            remo_N++;
                            SD_ARhold.C_hold[Trans_Sat.PRN-1].Sd_flag = 0;
                            sd_fix_C++;
                        }
                    }
                }
                else
                {
                    GNSS_epoF(epo) = 0;
                    if(Trans_Sat.cycly_slipF!=1&&fabs(Trans_Sat.SD_Nl_Float-Float2Int(Trans_Sat.SD_Nl_Float))<residual_amb&&!Trans_Sat.Par_sub)//&&!Trans_Sat.Par_sub
                    {
                        SD_ARhold.C_hold[Trans_Sat.PRN-1].Sd_flag = 1;
                    }
                    else
                    {
                        SD_ARhold.C_hold[Trans_Sat.PRN-1].Sd_flag = 0;
                    }
                    SD_ARhold.Sd_maxEA_prn[1] = part_Maxprn[1];
                    remo_Sys[remo_N] = epo;
                    remo_N++;
                }
                sysTemFlag[1] = 2;
            }
            else
            {
                Trans_C(epo) = -1;
                if(maxEAPRNepo[2]!=epo)
                {
                    GNSS_epoF(epo) = 3;
    //                La_SDX(las_n) = Trans_Sat.SD_Nl_Float;
                    SD_Wl(epo)  = lamdaIF_Wl * Trans_Sat.SD_Wl_Int;
                    SD_X(epo) = Trans_Sat.SD_Nl_Float;
                    SD_NLfcb(epo) = Trans_Sat.SD_nl_FCBs;
                    las_n++;
                    if(Par_sat_epo[epo]!=0)
                    {
                        remo_Sys[remo_N] = epo;
                        remo_N++;
                    }
                    else
                    {
                        SD_ARhold.C_hold[sd_fix_C].PRNhf = Trans_Sat.PRN;
                        if(Trans_Sat.cycly_slipF!=1&&fabs(Trans_Sat.SD_Nl_Float-Float2Int(Trans_Sat.SD_Nl_Float))<residual_amb&&!Trans_Sat.Par_sub)//&&!Trans_Sat.Par_sub
                        {
                            SD_ARhold.C_hold[Trans_Sat.PRN-1].Sd_flag = 1;
                            sd_fix_C++;
                        }
                        else
                        {
                            Par_sat_epo[epo] = -1;
                            remo_Sys[remo_N] = epo;
                            remo_N++;
                            SD_ARhold.C_hold[Trans_Sat.PRN-1].Sd_flag = 0;
                            sd_fix_C++;
                        }
                    }
                }
                else
                {
                    GNSS_epoF(epo) = 0;
                    if(Trans_Sat.cycly_slipF!=1&&fabs(Trans_Sat.SD_Nl_Float-Float2Int(Trans_Sat.SD_Nl_Float))<residual_amb&&!Trans_Sat.Par_sub)//&&!Trans_Sat.Par_sub
                    {
                        SD_ARhold.C_hold[Trans_Sat.PRN-1].Sd_flag = 1;
                    }
                    else
                    {
                        SD_ARhold.C_hold[Trans_Sat.PRN-1].Sd_flag = 0;
                    }
                    SD_ARhold.Sd_maxEA_prn[2] = part_Maxprn[2];
                    remo_Sys[remo_N] = epo;
                    remo_N++;
                }
                sysTemFlag[2] = 3;
            }
            break;
        case 'R':
            Trans_R(epo) = -1;
            if(maxEAPRNepo[3]!=epo)
            {
                GNSS_epoF(epo) = 4;
//                La_SDX(las_n) = Trans_Sat.SD_Nl_Float;
                SD_Wl(epo)  = lamdaIF_Wl * Trans_Sat.SD_Wl_Int;
                SD_X(epo) = Trans_Sat.SD_Nl_Float;
                SD_NLfcb(epo) = Trans_Sat.SD_nl_FCBs;
                las_n++;
                if(Par_sat_epo[epo]!=0)
                {
                    remo_Sys[remo_N] = epo;
                    remo_N++;
                }
                else
                {
                    SD_ARhold.R_hold[sd_fix_R].PRNhf = Trans_Sat.PRN;
                    if(Trans_Sat.cycly_slipF!=1&&fabs(Trans_Sat.SD_Nl_Float-Float2Int(Trans_Sat.SD_Nl_Float))<residual_amb&&!Trans_Sat.Par_sub)//&&!Trans_Sat.Par_sub
                    {
                        SD_ARhold.R_hold[Trans_Sat.PRN-1].Sd_flag = 1;
                        sd_fix_R++;
                    }
                    else
                    {
                        Par_sat_epo[epo] = -1;
                        remo_Sys[remo_N] = epo;
                        remo_N++;
                        SD_ARhold.R_hold[Trans_Sat.PRN-1].Sd_flag = 0;
                        sd_fix_R++;
                    }
                }
            }
            else
            {
                GNSS_epoF(epo) = 0;
                SD_ARhold.R_hold[Trans_Sat.PRN-1].Sd_flag = 1;
                if(Trans_Sat.cycly_slipF!=1&&fabs(Trans_Sat.SD_Nl_Float-Float2Int(Trans_Sat.SD_Nl_Float))<residual_amb&&!Trans_Sat.Par_sub)//&&!Trans_Sat.Par_sub
                {
                    SD_ARhold.R_hold[Trans_Sat.PRN-1].Sd_flag = 1;
                }
                else
                {
                    SD_ARhold.R_hold[Trans_Sat.PRN-1].Sd_flag = 0;
                }
                SD_ARhold.Sd_maxEA_prn[3] = part_Maxprn[3];
                remo_Sys[remo_N] = epo;
                remo_N++;
            }
            sysTemFlag[3] = 4;
            break;
        case 'E':
            Trans_E(epo) = -1;
            if(maxEAPRNepo[4]!=epo)
            {
                GNSS_epoF(epo) = 5;
//                La_SDX(las_n) = Trans_Sat.SD_Nl_Float;
                SD_Wl(epo)  = lamdaIF_Wl * Trans_Sat.SD_Wl_Int;
                SD_X(epo) = Trans_Sat.SD_Nl_Float;
                SD_NLfcb(epo) = Trans_Sat.SD_nl_FCBs;
                las_n++;
                if(Par_sat_epo[epo]!=0)
                {
                    remo_Sys[remo_N] = epo;
                    remo_N++;
                }
                else
                {
                    SD_ARhold.E_hold[sd_fix_E].PRNhf = Trans_Sat.PRN;
                    if(Trans_Sat.cycly_slipF!=1&&fabs(Trans_Sat.SD_Nl_Float-Float2Int(Trans_Sat.SD_Nl_Float))<residual_amb&&!Trans_Sat.Par_sub)//&&!Trans_Sat.Par_sub
                    {
                        SD_ARhold.E_hold[Trans_Sat.PRN-1].Sd_flag = 1;
                        sd_fix_E++;
                    }
                    else
                    {
                        Par_sat_epo[epo] = -1;
                        remo_Sys[remo_N] = epo;
                        remo_N++;
                        SD_ARhold.E_hold[Trans_Sat.PRN-1].Sd_flag = 0;
                        sd_fix_E++;
                    }
                }
            }
            else
            {
                GNSS_epoF(epo) = 0;
                SD_ARhold.E_hold[Trans_Sat.PRN-1].Sd_flag = 1;
                if(Trans_Sat.cycly_slipF!=1&&fabs(Trans_Sat.SD_Nl_Float-Float2Int(Trans_Sat.SD_Nl_Float))<residual_amb&&!Trans_Sat.Par_sub)//&&Trans_Sat.EA[0] > 20
                {
                    SD_ARhold.E_hold[Trans_Sat.PRN-1].Sd_flag = 1;
                }
                else
                {
                    remo_Sys[remo_N] = epo;
                }
                SD_ARhold.Sd_maxEA_prn[4] = part_Maxprn[4];
                remo_Sys[remo_N] = epo;
                remo_N++;
            }
            sysTemFlag[4] = 5;
            break;
        default:
            break;
        }
    }
//    int sortMaxEpo[4] = {-1},maxepo = -1,minepo = 60;
//sortord
    for(int i=0;i<5;i++) {
        for(int j=i+1;j<5;j++) {
            if(sortMaxEpo[i]<sortMaxEpo[j]) {
                int temp=sortMaxEpo[i],temp1 = sysTemFlag[i];
                sortMaxEpo[i]=sortMaxEpo[j];
                sortMaxEpo[j]=temp;
                sysTemFlag[i]=sysTemFlag[j];//max epo sort
                sysTemFlag[j]=temp1;
            }
        }
    }
    descending_order(remo_N,remo_Sys);
    for(int i =0;i<5;i++)
    {
        switch (sysTemFlag[i]) {
        case 1:
            if(maxEAPRNepo[0]<0)    break;
            Trans_SD.block(0,maxEAPRNepo[0],epoLen,1) = Trans_G;
            break;
        case 2:
            if(maxEAPRNepo[1]<0)    break;
            Trans_SD.block(0,maxEAPRNepo[1],epoLen,1) = Trans_BDS2;
            break;
        case 3:
            if(maxEAPRNepo[2]<0)    break;
            Trans_SD.block(0,maxEAPRNepo[2],epoLen,1) = Trans_C;
            break;
        case 4:
            if(maxEAPRNepo[3]<0)    break;
            Trans_SD.block(0,maxEAPRNepo[3],epoLen,1) = Trans_R;
            break;
        case 5:
            if(maxEAPRNepo[4]<0)    break;
            Trans_SD.block(0,maxEAPRNepo[4],epoLen,1) = Trans_E;
            break;
        default:
            break;
        }
    }
    Trans_matrix = Trans_SD;
    for(int i =0;i<5;i++)
    {
        switch (sysTemFlag[i]) {
        case 1:
            if(maxEAPRNepo[0]<0)    break;
            m_mat_cal.RemoveRow(Trans_matrix,maxEAPRNepo[0]);
            break;
        case 2:
            if(maxEAPRNepo[1]<0)    break;
            m_mat_cal.RemoveRow(Trans_matrix,maxEAPRNepo[1]);
            break;
        case 3:
            if(maxEAPRNepo[2]<0)    break;
            m_mat_cal.RemoveRow(Trans_matrix,maxEAPRNepo[2]);
            break;
        case 4:
            if(maxEAPRNepo[3]<0)    break;
            m_mat_cal.RemoveRow(Trans_matrix,maxEAPRNepo[3]);
            break;
        case 5:
            if(maxEAPRNepo[4]<0)    break;
            m_mat_cal.RemoveRow(Trans_matrix,maxEAPRNepo[4]);
            break;
        default:
            break;
        }
    }
    for(int i =0;i<remo_N;i++)
    {
        m_mat_cal.RemoveRow(Trans_SD,remo_Sys[i]);
    }
    SD_P   = P.block(eponumber,eponumber,P.rows()-eponumber,P.rows()-eponumber);
    La_SDP = Trans_SD * SD_P * Trans_SD.transpose();
    SD_nl_fcb = Trans_SD * SD_NLfcb;
    Qab    = P.block(0,eponumber,eponumber,P.rows()-eponumber) * Trans_SD.transpose();
    SD_GNSS_F = Trans_SD * GNSS_epoF;
    La_SDX =  Trans_SD * SD_X;
    SD_IFsmoot.Fix_Qa = Trans_matrix * SD_P * Trans_matrix.transpose();
    SD_IFsmoot.Fix_Qab = P.block(0,eponumber,eponumber,P.rows()-eponumber) * Trans_matrix.transpose();
}
bool Ambiguity_resolution::ambiguity_search(QVector< SatlitData > &epochSatlitDataLa,MatrixXd &P,
                                       SmoothData &SD_IFsmoot,int &fix_flag,int *Par_sat_epo,int *maxEASat_Prn,para_trans *trans_para)
{
    MatrixXd La_SDP,Qab,Trans_SD;
    VectorXd SD_nl_fcb,La_SDX,SD_GNSS_F;
    int satellite_len = epochSatlitDataLa.length();
    Par_sub_amb(epochSatlitDataLa,SD_IFsmoot,P,La_SDP,Qab,Trans_SD,SD_GNSS_F,La_SDX,SD_nl_fcb,Par_sat_epo,maxEASat_Prn);
    if(trans_para->Sosr_flag)
    {
        Standard_of_success_rate(epochSatlitDataLa,Par_sat_epo,La_SDP,trans_para->Ps);
        Par_sub_amb(epochSatlitDataLa,SD_IFsmoot,P,La_SDP,Qab,Trans_SD,SD_GNSS_F,La_SDX,SD_nl_fcb,Par_sat_epo,maxEASat_Prn);
    }
    int par_number=La_SDP.cols(),Full_sub_len=SD_IFsmoot.Fix_Qa.cols();//first_search=0,
    int info;QString wirte_debug;QVector< SatlitData > epoch_fixed_SatlitData;
    corr_opt *opt_corr = AGNSS_APPData::sGetAPPDataInstance()->mGetopt_corrData();
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
        MatrixXd amb_fix;double ratio = trans_para->ratio_value;int out_fixamb_num = 2;//
        wirte_debug ="ratio = 3.0";
        sea_fil.writeQdebug2Txt(wirte_debug);
        info = m_lambda.QLamdaSearch(La_SDX,La_SDP,amb_fix, &ratio,out_fixamb_num);//,&par_flag Par_sat_epo[6]
        trans_para->lambda_radio = ratio;
        if(info)    return false;

        if(ratio>trans_para->ratio_value)
        {
            wirte_debug ="ratio>3.0";
            trans_para->fixed_sat = La_SDP.rows();
            sea_fil.writeQdebug2Txt(wirte_debug);
            VectorXd La_SDnl,par_fix_amb,Optimal_solution;
            La_SDnl.resize(La_SDP.rows());La_SDnl.setZero(La_SDP.rows());
            Optimal_solution.resize(La_SDP.rows());Optimal_solution.setZero(La_SDP.rows());
            La_SDnl = amb_fix.topLeftCorner(La_SDP.rows(), 1);
            Optimal_solution = La_SDX - La_SDnl + SD_nl_fcb;//top left corner;
            par_fix_amb.resize(Full_sub_len);par_fix_amb.setZero(Full_sub_len);
            SD_IFsmoot.sd_a.resize(Full_sub_len);SD_IFsmoot.sd_a.setZero(Full_sub_len);
            int par_sat_len=0,full_ar_epo=0;bool max_sat_epo;

            for(int i=0;i<satellite_len;i++)
            {
                SatlitData epoch_sat = epochSatlitDataLa.at(i);
                max_sat_epo = false;
                for(int j=0;j<5;j++)
                {
                    if(maxEAepo[j] ==i)
                        max_sat_epo = true;
                }
                if(max_sat_epo)
                {
                    epoch_fixed_SatlitData.append(epoch_sat);
                    continue;
                }
                if(Par_sat_epo[i]==0)
                {
                    epoch_sat.SD_Nl_fix = ( int )La_SDnl(par_sat_len);
                    par_fix_amb(full_ar_epo)=Optimal_solution(par_sat_len);
                    par_sat_len++;
                }
                full_ar_epo++;
                epoch_fixed_SatlitData.append(epoch_sat);
            }
            epochSatlitDataLa = epoch_fixed_SatlitData;
            epoch_fixed_SatlitData.clear();
            wirte_debug ="full_ar_epo++;";
            sea_fil.writeQdebug2Txt(wirte_debug);
            fix_flag = 3;
            SD_IFsmoot.sd_a = par_fix_amb;
            break;
        }
        else
        {
            trans_para->fixed_sat =0;
            if(trans_para->Partial_subset==angle_screen)
            {
                angle_altitude_screen(epochSatlitDataLa,Par_sat_epo);
            }
            else if(trans_para->Partial_subset==stand_post_res)
                unfixed_sat(satellite_len,Par_sat_epo,trans_para);
            else if(trans_para->Partial_subset==post_resudual)
            {
                post_resuduala(satellite_len,Par_sat_epo,trans_para);

//                GNSS_PAR_(epochSatlitDataLa,Par_sat_epo,La_SDP);
            }
            else if(trans_para->Partial_subset==Adop_demet_secreen)
                Adop_screen(epochSatlitDataLa,Par_sat_epo,La_SDP);
            else if(trans_para->Partial_subset==Boost_Par)
                Boost_Parsub(epochSatlitDataLa,Par_sat_epo,La_SDP);
            else if(trans_para->Partial_subset==full_ambiguity_fixed)
                break;
            Par_sub_amb(epochSatlitDataLa,SD_IFsmoot,P,La_SDP,Qab,Trans_SD,SD_GNSS_F,La_SDX,SD_nl_fcb,Par_sat_epo,maxEASat_Prn);
            par_number=La_SDP.cols();
        }
    }

    for(int j=0;j<5;j++)
    {
        trans_para->reference_sat[j] = maxEAepo[j];
    }
    for(int i=0;i<satellite_len;i++)
    {
        SatlitData epoch_sat = epochSatlitDataLa.at(i);
        epoch_sat.par_sat_rank = Par_sat_epo[i];
        epoch_fixed_SatlitData.append(epoch_sat);
    }
    epochSatlitDataLa = epoch_fixed_SatlitData;
    epoch_fixed_SatlitData.clear();
}
bool Ambiguity_resolution::par_BOOST_ambiguity_search(QVector< SatlitData > &epochSatlitDataLa,MatrixXd &P,
                                       SmoothData &SD_IFsmoot,int &fix_flag,int *Par_sat_epo,int *maxEASat_Prn,para_trans *trans_para,VectorXd &stat_zz)
{
    MatrixXd La_SDP,Qab,Trans_SD;
    VectorXd SD_nl_fcb,La_SDX,SD_GNSS_F;
    int satellite_len = epochSatlitDataLa.length();
    Par_sub_amb(epochSatlitDataLa,SD_IFsmoot,P,La_SDP,Qab,Trans_SD,SD_GNSS_F,La_SDX,SD_nl_fcb,Par_sat_epo,maxEASat_Prn);
    int par_number=La_SDP.cols(),Full_sub_len=SD_IFsmoot.Fix_Qa.cols();//first_search=0,
    while(par_number>6&&!trans_para->ambiguity_fix)
    {
        MatrixXd amb_fix;double ratio = 2.0;int out_fixamb_num = 2;//
        m_lambda.QLamdaSearch_boost_Par(La_SDX,La_SDP, &ratio,out_fixamb_num,Qab,stat_zz);
        trans_para->lambda_radio = ratio;
        if(ratio>2)
        {
            fix_flag = 1;
            break;
        }
        else
            break;

    }
}



/***************************************************************************
* function：  Probability_Test
* input：
*
* Functional area：If combined inter satellite single difference ambiguity rounding
*
* output：
* return：
***************************************************************************/
bool Ambiguity_resolution::Probability_Test(int &Smo_SD_WLint,double Smo_SD_WL,double std_dat,double lambda)
{//
    double P_0 = 0.0;
    Smo_SD_WLint = 0;

//    Smo_SD_WLint = qRound(Smo_SD_WL);//Rounding
    Smo_SD_WLint = Float2Int(Smo_SD_WL);//Float2Int
    double res_float = fabs(Smo_SD_WLint-Smo_SD_WL);

    if(res_float>=0.4)    return false;
    if(fabs(lambda)>2.5)  return true;
    if(std_dat>=0.3) return false;

    double P_s = 2*erfc(res_float/std_dat) - 1;
    double thresholds_T = (1-res_float/0.4)*(1-std_dat/0.3);
    for(int test_num=1;test_num<10;test_num++)
    {
        double coefficient_1 = (test_num - fabs(Smo_SD_WL - Smo_SD_WLint))/sqrtl(2*std_dat);
        double coefficient_2 = (test_num + fabs(Smo_SD_WL - Smo_SD_WLint))/sqrtl(2*std_dat);
        P_0 = P_0 + erfc(coefficient_1) - erfc(coefficient_2);
    }
    double decision_function = thresholds_T * 1.0/P_0;
    if(decision_function < 1000)     return false;

    return true;
}
int Ambiguity_resolution::Float2Int(double SDWlint)
{
    double FloatNum = 0.0;int IntNum = 0;
    FloatNum = SDWlint;
    if((FloatNum - (int)FloatNum) > 0.5)
    {
        IntNum = (int)FloatNum + 1;
    }
    else if((FloatNum - (int)FloatNum) < -0.5)
    {
        IntNum = (int)FloatNum - 1;
    }
    else if(fabs(FloatNum - (int)FloatNum) == 0.5)
    {
        IntNum = (int)(FloatNum + 0.1);
    }
    else
        IntNum = (int)FloatNum;
    return IntNum;
}
void Ambiguity_resolution::descending_order(int de_sortN,int *sortMaxEpo)
{
    for(int i=0;i<de_sortN;i++) {
        for(int j=i+1;j<de_sortN;j++) {
            if(sortMaxEpo[i]<sortMaxEpo[j]) {
                int temp=sortMaxEpo[i];//,temp1 = sysTemFlag[i]
                sortMaxEpo[i]=sortMaxEpo[j];
                sortMaxEpo[j]=temp;
//                sysTemFlag[i]=sysTemFlag[j];//max epo sort
//                sysTemFlag[j]=temp1;
            }
        }
    }
}

void Ambiguity_resolution::GNSS_PAR_(QVector< SatlitData > &epochSatlitDataLa,int *Par_sat_epo,MatrixXd &La_SDP)
{
//    int min_par=0;

    GNSS_PAR_auto *auto_par = AGNSS_APPData::sGetAPPDataInstance()->mGetGNSS_PAR_autoData();
    bool frist_par = false;
    int epoLen = epochSatlitDataLa.length();
    if(auto_par->systemf.length()==0)
        frist_par = true;
    if(frist_par)
    {
        double Ps[4]={1,1,1,1};
        double dit_p[La_SDP.cols()],dit_p_[La_SDP.cols()];
        if(m_lambda.bootstrapping(dit_p,La_SDP))
            return ;

        int Adit_pos = 0,dite_pos=0;double angle_=60;int eph_continue=10;
        for(int i=0;i<epoLen;i++)
        {
            SatlitData GNSS_Satlite = epochSatlitDataLa.at(i);
            auto_par->PS_fixed.append(1.0);
            auto_par->systemf.append(0);
            auto_par->dele_flag.append(0);
            if(Par_sat_epo[i]==0)
            {
                double dit = dit_p[Adit_pos];
                if(GNSS_Satlite.SatType=='G')
                {
                    auto_par->PS_fixed[i] = dit;
                    auto_par->systemf[i] = 1;
                    Ps[0] = Ps[0]*dit;
                    if(GNSS_Satlite.EA[0]<angle_||GNSS_Satlite.continue_eph<eph_continue)
                    {
                        auto_par->dele_flag[i]=1;
                    }
                }
                else if(GNSS_Satlite.SatType=='C')
                {
                    auto_par->PS_fixed[i] = dit;
                    if(GNSS_Satlite.BDS_2flag)
                        auto_par->systemf[i] = 5;
                    else
                    {
                        auto_par->systemf[i] = 2;
                        Ps[1] = Ps[1]*dit;
                    }
                    if(GNSS_Satlite.EA[0]<angle_||GNSS_Satlite.continue_eph<eph_continue||GNSS_Satlite.BDS_2flag)
                    {
                        auto_par->dele_flag[i]=1;
                    }
                }
                else if(GNSS_Satlite.SatType=='E')
                {

                    auto_par->PS_fixed[i] = dit;
                    auto_par->systemf[i] = 3;
                    Ps[2] = Ps[2]*dit;
                    if(GNSS_Satlite.EA[0]<angle_||GNSS_Satlite.continue_eph<eph_continue)
                    {
                        auto_par->dele_flag[i]=1;
                    }
                }
                else if(GNSS_Satlite.SatType=='R')
                {
                    auto_par->PS_fixed[i] = dit;
                    auto_par->systemf[i] = 4;
                    Ps[3] = Ps[3]*dit;
                    if(GNSS_Satlite.EA[0]<angle_||GNSS_Satlite.continue_eph<eph_continue)
                    {
                        auto_par->dele_flag[i]=1;
                    }
                }

                Adit_pos++;
            }
        }

        int flag_sys[4] = {1,2,3,4};
        for(int i=0;i<4;i++)
        {
            for(int j=i+1;j<4;j++)
            {
                if(Ps[i]<Ps[j])
                {
                    double PS_M = Ps[i];
                    int sys = flag_sys[i];
                    flag_sys[i] = flag_sys[j];
                    flag_sys[j] = sys;
                    Ps[i] = Ps[j];
                    Ps[j] = PS_M;
                }
            }

        }
        for(int i=0;i<4;i++)
        {
            auto_par->sys_f[0][i] = flag_sys[i];
        }

    }
//    auto_par->sys_f[0]=4;
//    auto_par->sys_f[1]=1;
//    auto_par->sys_f[2]=2;
//    auto_par->sys_f[3]=3;
    int delete_amb[5];double min_ps[5];double maxPs = 1.1;int delete_amb_up = -1;
    for(int i=0;i<5;i++)
    {
        delete_amb[i]=-1;
        min_ps[i] = 1.1;
    }
    for(int i=0;i<epoLen;i++)
    {
        SatlitData delet_sat =  epochSatlitDataLa.at(i);
        if(Par_sat_epo[i]==0)
        {
            if(auto_par->dele_flag.at(i)==1)
            {
                if(delet_sat.BDS_2flag)
                {
                    if(min_ps[4]>auto_par->PS_fixed[i])
                    {
                        min_ps[4] = auto_par->PS_fixed[i];
                        delete_amb[4] = i;
                    }
                }
                else if(delet_sat.SatType=='G')
                {
                    if(min_ps[0]>auto_par->PS_fixed[i])
                    {
                        min_ps[0] = auto_par->PS_fixed[i];
                        delete_amb[0] = i;
                    }
                }
                else if(delet_sat.SatType=='C')
                {
                    if(min_ps[1]>auto_par->PS_fixed[i])
                    {
                        min_ps[1] = auto_par->PS_fixed[i];
                        delete_amb[1] = i;
                    }
                }
                else if(delet_sat.SatType=='E')
                {
                    if(min_ps[2]>auto_par->PS_fixed[i])
                    {
                        min_ps[2] = auto_par->PS_fixed[i];
                        delete_amb[2] = i;
                    }
                }
                else if(delet_sat.SatType=='R')
                {
                    if(min_ps[3]>auto_par->PS_fixed[i])
                    {
                        min_ps[3] = auto_par->PS_fixed[i];
                        delete_amb[3] = i;
                    }
                }
            }
            else
            {
                if(maxPs>auto_par->PS_fixed[i])
                {
                    maxPs = auto_par->PS_fixed[i];
                    delete_amb_up = i;
                }
            }

        }
    }
    if(delete_amb[4]!=-1)
        Par_sat_epo[delete_amb[4]]=1;
    else
    {
       for(int i=3;i>-1;i--)
       {
           if(delete_amb[auto_par->sys_f[0][i]-1]!=-1)
           {
               Par_sat_epo[delete_amb[auto_par->sys_f[0][i]-1]]=1;
               break;
           }
       }
       Par_sat_epo[delete_amb_up] = 1;
    }

}
void Ambiguity_resolution::GNSS_PAR_angle(QVector< SatlitData > &epochSatlitDataLa,int *Par_sat_epo,MatrixXd &La_SDP)
{
//    int min_par=0;

    GNSS_PAR_auto *auto_par = AGNSS_APPData::sGetAPPDataInstance()->mGetGNSS_PAR_autoData();
    bool frist_par = false;
    int epoLen = epochSatlitDataLa.length();
    if(auto_par->systemf.length()==0)
        frist_par = true;
    if(frist_par)
    {

        double dit_p[La_SDP.cols()],dit_p_[La_SDP.cols()];
        if(m_lambda.bootstrapping(dit_p,La_SDP))
            return ;

        int Adit_pos = 0,dite_pos=0;double angle_=10;int eph_continue=10;
        for(int j=1;j<9;j++)
        {
            Adit_pos = 0;
            for(int i=0;i<epoLen;i++)
            {
                SatlitData GNSS_Satlite = epochSatlitDataLa.at(i);
                if(j==1)
                {
                    auto_par->PS_fixed.append(1.0);
                    auto_par->systemf.append(0);
                    auto_par->dele_flag.append(0);
                }
                if(Par_sat_epo[i]==0)
                {
                    double dit = dit_p[Adit_pos];
                    if(GNSS_Satlite.SatType=='G')
                    {

                        if(GNSS_Satlite.EA[0]<angle_+10&&GNSS_Satlite.EA[0]>=angle_)
                        {
                            auto_par->dele_flag[i]=angle_+10;
                            auto_par->PS_fixed[i] = dit;
                            auto_par->systemf[i] = 1;
                        }
                    }
                    else if(GNSS_Satlite.SatType=='C')
                    {

                        if(GNSS_Satlite.EA[0]<angle_+10&&GNSS_Satlite.EA[0]>=angle_)
                        {
                            auto_par->dele_flag[i]=angle_+10;
                            auto_par->PS_fixed[i] = dit;
                            if(GNSS_Satlite.BDS_2flag)
                                auto_par->systemf[i] = 5;
                            else
                            {
                                auto_par->systemf[i] = 2;
                            }
                        }
                    }
                    else if(GNSS_Satlite.SatType=='E')
                    {


                        if(GNSS_Satlite.EA[0]<angle_+10&&GNSS_Satlite.EA[0]>=angle_)
                        {
                            auto_par->dele_flag[i]=angle_+10;
                            auto_par->PS_fixed[i] = dit;
                            auto_par->systemf[i] = 3;
                        }
                    }
                    else if(GNSS_Satlite.SatType=='R')
                    {

                        if(GNSS_Satlite.EA[0]<angle_+10&&GNSS_Satlite.EA[0]>=angle_)
                        {
                            auto_par->dele_flag[i]=angle_+10;
                            auto_par->PS_fixed[i] = dit;
                            auto_par->systemf[i] = 4;
                        }
                    }

                    Adit_pos++;
                }
            }
            angle_ = angle_+10;
        }
//        double angle_ = 10;
        for(int i=0;i<8;i++)
        {
            double Ps[4]={0,0,0,0};
            int PSN[4]={0,0,0,0};
            int flag_sys[4] = {1,2,3,4};
            for(int j=0;j<epoLen;j++)
            {
                SatlitData GNSS_Satlite = epochSatlitDataLa.at(j);
                if(Par_sat_epo[j]==0)
                {
                    if(auto_par->dele_flag[j]!=i*10+20)
                        continue;
                    if(GNSS_Satlite.SatType=='G')
                    {
                        Ps[0]=Ps[0]+auto_par->PS_fixed[j];
                        PSN[0] = PSN[0]+1;
                    }
                    else if(GNSS_Satlite.SatType=='C')
                    {
                        if(!GNSS_Satlite.BDS_2flag)
                        {
                            Ps[1]=Ps[1]+auto_par->PS_fixed[j];
                            PSN[1] = PSN[1]+1;
                        }
                    }
                    else if(GNSS_Satlite.SatType=='E')
                    {

                        Ps[2]=Ps[2]+auto_par->PS_fixed[j];
                        PSN[2] = PSN[2]+1;
                    }
                    else if(GNSS_Satlite.SatType=='R')
                    {
                        Ps[3]=Ps[3]+auto_par->PS_fixed[j];
                        PSN[3] = PSN[3]+1;
                    }
                }
            }
            for(int j=0;j<4;j++)
            {
                if(PSN[j]!=0)
                    Ps[j]=Ps[j]/PSN[j];
            }
            for(int k=0;k<4;k++)
            {
                for(int j=k+1;j<4;j++)
                {
                    if(Ps[k]>Ps[j])
                    {
                        double PS_M = Ps[k];
                        int sys = flag_sys[k];
                        flag_sys[k] = flag_sys[j];
                        flag_sys[j] = sys;
                        Ps[k] = Ps[j];
                        Ps[j] = PS_M;
                    }
                }
            }
            for(int j=0;j<4;j++)
            {
              auto_par->sys_f[i][j]  = flag_sys[j];
            }

        }
    }

    int delete_amb[5];double min_ps[5];double maxPs = 1.1;int delete_amb_up = -1;
    auto_par->fixed_sat = La_SDP.cols();
    double angle_ = 10;
    for(int l=0;l<8;l++)
    {
        for(int i=0;i<5;i++)
        {
            delete_amb[i]=-1;
            min_ps[i] = 1.1;
        }
        int delete_amb_n=0;
        for(int i=0;i<epoLen;i++)
        {
            SatlitData delet_sat =  epochSatlitDataLa.at(i);
            if(Par_sat_epo[i]==0)
            {
                if(auto_par->dele_flag[i]==angle_+10)
                {
                    if(delet_sat.BDS_2flag)
                    {
                        if(min_ps[4]>auto_par->PS_fixed[i])
                        {
                            min_ps[4] = auto_par->PS_fixed[i];
                            delete_amb[4] = i;
                        }
                    }
                    else if(delet_sat.SatType=='G')
                    {
                        if(min_ps[0]>auto_par->PS_fixed[i])
                        {
                            min_ps[0] = auto_par->PS_fixed[i];
                            delete_amb[0] = i;
                        }
                    }
                    else if(delet_sat.SatType=='C')
                    {
                        if(min_ps[1]>auto_par->PS_fixed[i])
                        {
                            min_ps[1] = auto_par->PS_fixed[i];
                            delete_amb[1] = i;
                        }
                    }
                    else if(delet_sat.SatType=='E')
                    {
                        if(min_ps[2]>auto_par->PS_fixed[i])
                        {
                            min_ps[2] = auto_par->PS_fixed[i];
                            delete_amb[2] = i;
                        }
                    }
                    else if(delet_sat.SatType=='R')
                    {
                        if(min_ps[3]>auto_par->PS_fixed[i])
                        {
                            min_ps[3] = auto_par->PS_fixed[i];
                            delete_amb[3] = i;
                        }
                    }
                }
                delete_amb_n++;

            }
        }
        bool alh = false;
        for(int i=0;i<5;i++)
        {
            if(delete_amb[i]!=-1)
            {
                alh=true;
            }
        }
        if(alh)
        {
            if(delete_amb[4]!=-1)
                Par_sat_epo[delete_amb[4]]=epoLen-delete_amb_n+2;
            else
            {
               for(int i=0;i<4;i++)
               {
                   if(delete_amb[auto_par->sys_f[l][i]-1]!=-1)
                   {
                       Par_sat_epo[delete_amb[auto_par->sys_f[l][i]-1]]=epoLen-delete_amb_n+2;
                       break;
                   }
               }
            }
            break;
        }

        angle_=angle_+10;
    }


}
