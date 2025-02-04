#include "coefficient_matrix.h"

coefficient_matrix::coefficient_matrix()
{

}
void coefficient_matrix::init_CoeffVar(corr_opt *opt_corr,para_trans *trans_para)
{
    trans_para->isInitPara = false;//The first epoch is only initialized once
    trans_para->m_VarChang = false;
    trans_para->cyclyslip_sat_n = 0;
    trans_para->ambiguity_fix = true;
    trans_para->lambda_radio =0;
    for(int i=0;i<4;i++)    trans_para->Dops[i]=0;

    int sys_N = opt_corr->m_sys_str.length();
    if(opt_corr->m_sys_str.contains("C",Qt::CaseInsensitive))
        sys_N++;
    trans_para->sys_num = sys_N;
    trans_para->const_parm = sys_N+4;
    trans_para->sys_len.resize(sys_N);
    trans_para->sys_len.setZero(sys_N);
    trans_para->up_sys_len.resize(sys_N);
    trans_para->up_sys_len.setZero(sys_N);
    trans_para->m_SPP_Pos[0] = trans_para->m_SPP_Pos[1]= trans_para->m_SPP_Pos[2] = 0;
    trans_para->State_X.resize(32);
    trans_para->State_X.setZero(32);
    trans_para->sigma0 = 1.0;

//    opt_corr->whihet_diff = 1e4;
//    opt_corr->xyz_kinematic = 1e4; opt_corr->trop_covarQ = 3e-8; opt_corr->rcv_clkQ = 1e4; opt_corr->sat_ambQ = 1e-14; opt_corr->sat_ionQ = 1e6;
//    opt_corr->xyz_static = 1e4; opt_corr->trop_covarP = 10; opt_corr->rcv_clkP = 1e4; opt_corr->sat_ambP = 1e4; opt_corr->sat_ionP = 10;
}
//Initialize matrix
void coefficient_matrix::init_IF_matrix(int Avai_satN,MatrixXd &Coef_B,VectorXd &Obs_L,const corr_opt *opt_corr,para_trans *trans_para)
{
    int sys_num = trans_para->sys_num;
    int const_parm = 4+sys_num;
    trans_para->const_parm = const_parm;
    int filter_model = opt_corr->filter_model;
    double xyz_kinematic = opt_corr->xyz_kinematic;
    double xyz_static = opt_corr->xyz_static;
    double trop_covarP = opt_corr->trop_covarP;
    double trop_covarQ = opt_corr->trop_covarQ;
    double rcv_clkQ = opt_corr->rcv_clkQ;
    double sat_ambQ = opt_corr->sat_ambQ;
    double rcv_clkP = opt_corr->rcv_clkP;
    double sat_ambP = opt_corr->sat_ambP;

    VectorXd State_X;
    MatrixXd Covar_P,Nois_Q,state_Fi;

    switch (filter_model) {
    case PPP_STATIC_SRIF:
    case PPP_KINEMATIC_SRIF:
        State_X.resize(Avai_satN+const_parm);
        State_X.setZero(Avai_satN+const_parm);
        Covar_P.resize(Avai_satN+const_parm,Avai_satN+const_parm);
        Covar_P.setZero(Avai_satN+const_parm,Avai_satN+const_parm);
        Nois_Q.resize(Avai_satN+const_parm,Avai_satN+const_parm);
        Nois_Q.setZero(Avai_satN+const_parm,Avai_satN+const_parm);
        if(filter_model==PPP_STATIC_SRIF)
        {
            for(int i=0;i<3;i++)
            {
                Covar_P(i,i) = xyz_static;
            }
        }
        else
        {
            for(int i=0;i<3;i++)
            {
                Covar_P(i,i) = xyz_kinematic;
                Nois_Q(i,i) = xyz_kinematic;
            }

        }
        Covar_P(3,3) = trop_covarP;Nois_Q(3,3) = trop_covarQ;
        Covar_P(4,4) = rcv_clkP;Nois_Q(4,4) = rcv_clkQ;
        for(int i=5;i<const_parm;i++)
        {
            if(opt_corr->sys_error_select==ISBs_mode)
            {
                Covar_P(i,i) = rcv_clkP;
                Nois_Q(i,i) = 1;
            }
            else
            {
                Covar_P(i,i) = rcv_clkP;
                Nois_Q(i,i) = rcv_clkQ;
            }
        }
        for(int i=const_parm;i<Covar_P.cols();i++)
        {
            Covar_P(i,i) = sat_ambP;
            Nois_Q(i,i) = sat_ambQ;
        }
        break;
    case PPP_STATIC_KALM:
    case PPP_KINEMATIC_KALM:
        State_X.resize(Avai_satN+const_parm);
        State_X.setZero(Avai_satN+const_parm);
        Nois_Q.resize(Avai_satN+const_parm,Avai_satN+const_parm);
        Nois_Q.setZero(Avai_satN+const_parm,Avai_satN+const_parm);
        Covar_P.resize(Avai_satN+const_parm,Avai_satN+const_parm);
        Covar_P.setZero(Avai_satN+const_parm,Avai_satN+const_parm);
        state_Fi.resize(Avai_satN+const_parm,Avai_satN+const_parm);
        state_Fi.setIdentity(Avai_satN+const_parm,Avai_satN+const_parm);

        Covar_P(3,3) = trop_covarP;Nois_Q(3,3) = trop_covarQ;
        Covar_P(4,4) = rcv_clkP;Nois_Q(4,4) = rcv_clkQ;
        for(int i=5;i<const_parm;i++)
        {
            if(opt_corr->sys_error_select==ISBs_mode)
            {
                Covar_P(i,i) = rcv_clkP;
                Nois_Q(i,i) = 1;
            }
            else
            {
                Covar_P(i,i) = rcv_clkP;
                Nois_Q(i,i) = rcv_clkQ;
            }
        }
        for(int i=const_parm;i<Nois_Q.cols();i++)
        {
            Covar_P(i,i) = sat_ambP; Nois_Q(i,i) =sat_ambQ;
        }
        if(filter_model==PPP_KINEMATIC_KALM)
        {
            for(int i=0;i<3;i++)
            {
                Covar_P(i,i) = xyz_kinematic;
                Nois_Q(i,i) = xyz_kinematic;
            }
        }
        else
        {
            for(int i=0;i<3;i++)    Covar_P(i,i) = xyz_static;
        }
        break;
    default:
        break;
    }
    State_X = (Coef_B.transpose()*Coef_B).inverse()*Coef_B.transpose()*Obs_L;
    trans_para->State_X = State_X;
    trans_para->Covar_P=Covar_P;
    trans_para->Nois_Q=Nois_Q;
    trans_para->state_Fi=state_Fi;
    trans_para->isInitPara = true;//No longer initialized after
}
void coefficient_matrix::init_NOcombina_matrix(QVector< SatlitData > &Current_sat,MatrixXd &Coef_B,VectorXd &Obs_L,
                                               int Total_sat_fre,const corr_opt *opt_corr,para_trans *trans_para)
{
    int sys_num = trans_para->sys_num;
    int const_parm = 4+sys_num+trans_para->curr_IFB_n;
    trans_para->const_parm = const_parm;
    int filter_model = opt_corr->filter_model;
    double xyz_kinematic = opt_corr->xyz_kinematic;
    double xyz_static = opt_corr->xyz_static;
    double trop_covarP = opt_corr->trop_covarP;
    double trop_covarQ = opt_corr->trop_covarQ;
    double rcv_clkQ = opt_corr->rcv_clkQ;
    double sat_ambQ = opt_corr->sat_ambQ;
    double rcv_clkP = opt_corr->rcv_clkP;
    double sat_ambP = opt_corr->sat_ambP;
    double sat_ionQ = opt_corr->sat_ionQ;
    double sat_ionP = opt_corr->sat_ionP;

    VectorXd State_X;
    MatrixXd Covar_P,Nois_Q,state_Fi;

    int Avai_satN = Current_sat.length();
    int PAcoeff1 = 0;
    switch (filter_model) {
    case PPP_STATIC_SRIF:
    case PPP_KINEMATIC_SRIF:
        State_X.resize(Avai_satN+const_parm+Total_sat_fre);
        State_X.setZero(Avai_satN+const_parm+Total_sat_fre);
        Covar_P.resize(Avai_satN+const_parm+Total_sat_fre,Avai_satN+const_parm+Total_sat_fre);
        Covar_P.setZero(Avai_satN+const_parm+Total_sat_fre,Avai_satN+const_parm+Total_sat_fre);
        Nois_Q.resize(Avai_satN+const_parm+Total_sat_fre,Avai_satN+const_parm+Total_sat_fre);
        Nois_Q.setZero(Avai_satN+const_parm+Total_sat_fre,Avai_satN+const_parm+Total_sat_fre);

        if(filter_model==PPP_STATIC_SRIF)
        {
            for(int i=0;i<3;i++)
            {
                Covar_P(i,i) = xyz_static;
            }
        }
        else
        {
            for(int i=0;i<3;i++)
            {
                Covar_P(i,i) = xyz_kinematic;
                Nois_Q(i,i) = xyz_kinematic;
            }

        }
        Covar_P(3,3) = trop_covarP;Nois_Q(3,3) = trop_covarQ;
        Covar_P(4,4) = rcv_clkP;Nois_Q(4,4) = rcv_clkQ;
        for(int i=5;i<const_parm;i++)
        {
            if(opt_corr->sys_error_select==ISBs_mode)
            {
                Covar_P(i,i) = rcv_clkP;
                Nois_Q(i,i) = 1;
            }
            else
            {
                Covar_P(i,i) = rcv_clkP;
                Nois_Q(i,i) = rcv_clkQ;
            }
        }
        for(int i = 0; i < Avai_satN;i++)
        {
            SatlitData Pelement = Current_sat.at(i);
            Nois_Q(i+const_parm,i+const_parm) = sat_ionQ;
            Covar_P(i+const_parm,i+const_parm) = sat_ionP;
            for(int mufre=0;mufre<5;mufre++)
            {
                if(Pelement.ObsFreflag[mufre]!=1)  continue;
                Nois_Q(PAcoeff1+Avai_satN+const_parm,PAcoeff1+Avai_satN+const_parm) = sat_ambQ;
                Covar_P(PAcoeff1+Avai_satN+const_parm,PAcoeff1+Avai_satN+const_parm) = sat_ambP;// for Ambiguity
                PAcoeff1++;
            }
        }
        break;
    case PPP_STATIC_KALM:
    case PPP_KINEMATIC_KALM:
        State_X.resize(Avai_satN+const_parm+Total_sat_fre);
        State_X.setZero(Avai_satN+const_parm+Total_sat_fre);
        Nois_Q.resize(Avai_satN+const_parm+Total_sat_fre,Avai_satN+const_parm+Total_sat_fre);
        Nois_Q.setZero(Avai_satN+const_parm+Total_sat_fre,Avai_satN+const_parm+Total_sat_fre);
        Covar_P.resize(Avai_satN+const_parm+Total_sat_fre,Avai_satN+const_parm+Total_sat_fre);
        Covar_P.setZero(Avai_satN+const_parm+Total_sat_fre,Avai_satN+const_parm+Total_sat_fre);
        state_Fi.resize(Avai_satN+const_parm+Total_sat_fre,Avai_satN+const_parm+Total_sat_fre);
        state_Fi.setIdentity(Avai_satN+const_parm+Total_sat_fre,Avai_satN+const_parm+Total_sat_fre);

        Covar_P(3,3) =  trop_covarP;Nois_Q(3,3) = trop_covarQ;
        Covar_P(4,4) = rcv_clkP;Nois_Q(4,4) = rcv_clkQ;
        for(int i=5;i<const_parm;i++)
        {
            if(opt_corr->sys_error_select==ISBs_mode)
            {
                Covar_P(i,i) = rcv_clkP;
                Nois_Q(i,i) = 1;
            }
            else
            {
                Covar_P(i,i) = rcv_clkP;
                Nois_Q(i,i) = rcv_clkQ;
            }
        }

        for(int i = 0; i < Avai_satN;i++)
        {
            SatlitData Pelement = Current_sat.at(i);
            Nois_Q(i+const_parm,i+const_parm) = sat_ionQ;
            Covar_P(i+const_parm,i+const_parm) = sat_ionP;
            for(int mufre=0;mufre<5;mufre++)
            {
                if(Pelement.ObsFreflag[mufre]!=1)  continue;
                Nois_Q(PAcoeff1+Avai_satN+const_parm,PAcoeff1+Avai_satN+const_parm) = sat_ambQ;
                Covar_P(PAcoeff1+Avai_satN+const_parm,PAcoeff1+Avai_satN+const_parm) = sat_ambP;// for Ambiguity
                PAcoeff1++;
            }
        }
        if(filter_model==PPP_KINEMATIC_KALM)
        {
            for(int i=0;i<3;i++)
            {
                Covar_P(i,i) = xyz_kinematic;
                Nois_Q(i,i) = xyz_kinematic;
            }
        }
        else
        {
            for(int i=0;i<3;i++)    Covar_P(i,i) = xyz_static;
        }
        break;
    default:
        break;
    }
    State_X = (Coef_B.transpose()*Coef_B).inverse()*Coef_B.transpose()*Obs_L;
    trans_para->State_X = State_X;
    trans_para->Covar_P=Covar_P;
    trans_para->Nois_Q=Nois_Q;
    trans_para->state_Fi=state_Fi;
    trans_para->isInitPara = true;//No longer initialized after
}
void coefficient_matrix::init_NO_WL_combina_matrix(QVector< SatlitData > &Current_sat,MatrixXd &Coef_B,VectorXd &Obs_L,
                                               int Total_sat_fre,const corr_opt *opt_corr,para_trans *trans_para)
{
    int sys_num = trans_para->sys_num;
    int const_parm = 4+sys_num+trans_para->curr_IFB_n;
    trans_para->const_parm = const_parm;
    int filter_model = opt_corr->filter_model;
    double xyz_kinematic = opt_corr->xyz_kinematic;
    double xyz_static = opt_corr->xyz_static;
    double trop_covarP = opt_corr->trop_covarP;
    double trop_covarQ = opt_corr->trop_covarQ;
    double rcv_clkQ = opt_corr->rcv_clkQ;
    double sat_ambQ = opt_corr->sat_ambQ;
    double rcv_clkP = opt_corr->rcv_clkP;
    double sat_ambP = opt_corr->sat_ambP;
    double sat_ionQ = opt_corr->sat_ionQ;
    double sat_ionP = opt_corr->sat_ionP;
    double sat_IFBP = 1000000;
    double sat_IFBQ = 10;
    QString m_sys_str=opt_corr->m_sys_str;
    VectorXd sys_len = trans_para->sys_len;
    VectorXd State_X;
    MatrixXd Covar_P,Nois_Q,state_Fi;

    int Avai_satN = Current_sat.length();
    int PAcoeff1 = 0;
    int bds_flag=0,IFB_n=0,clk_ref=0;
    switch (filter_model) {
    case PPP_STATIC_SRIF:
    case PPP_KINEMATIC_SRIF:
        State_X.resize(Avai_satN+const_parm+Total_sat_fre);
        State_X.setZero(Avai_satN+const_parm+Total_sat_fre);
        Covar_P.resize(Avai_satN+const_parm+Total_sat_fre,Avai_satN+const_parm+Total_sat_fre);
        Covar_P.setZero(Avai_satN+const_parm+Total_sat_fre,Avai_satN+const_parm+Total_sat_fre);
        Nois_Q.resize(Avai_satN+const_parm+Total_sat_fre,Avai_satN+const_parm+Total_sat_fre);
        Nois_Q.setZero(Avai_satN+const_parm+Total_sat_fre,Avai_satN+const_parm+Total_sat_fre);

        if(filter_model==PPP_STATIC_SRIF)
        {
            for(int i=0;i<3;i++)
            {
                Covar_P(i,i) = xyz_static;
            }
        }
        else
        {
            for(int i=0;i<3;i++)
            {
                Covar_P(i,i) = xyz_kinematic;
                Nois_Q(i,i) = xyz_kinematic;
            }

        }
        Covar_P(3,3) = trop_covarP;Nois_Q(3,3) = trop_covarQ;
        bds_flag=0;IFB_n=0;clk_ref=0;
        for(int j=0;j<m_sys_str.length();j++)
        {
            if(m_sys_str[j]=='C')
            {
                if(sys_len[j] == 1)
                {
                    clk_ref++;
                    if(clk_ref==1)
                    {
                        Covar_P(4,4) = rcv_clkP;Nois_Q(4,4) = rcv_clkQ;
                    }
                    else
                    {
                        if(opt_corr->sys_error_select==ISBs_mode)
                        {
                            Covar_P(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = rcv_clkP;
                            Nois_Q(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = 10;
                        }
                        else
                        {
                            Covar_P(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = rcv_clkP;
                            Nois_Q(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = rcv_clkQ;
                        }
                    }
                    if(trans_para->curr_C2IFB_falg)
                    {
                        IFB_n++;
                        Covar_P(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = sat_IFBP;Nois_Q(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = sat_IFBQ;
                    }
                }
                bds_flag++;
                if(sys_len[j+bds_flag] == 1)
                {
                    clk_ref++;
                    if(clk_ref==1)
                    {
                        Covar_P(4,4) = rcv_clkP;Nois_Q(4,4) = rcv_clkQ;
                    }
                    else
                    {
                        if(opt_corr->sys_error_select==ISBs_mode)
                        {
                            Covar_P(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = rcv_clkP;
                            Nois_Q(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = 10;
                        }
                        else
                        {
                            Covar_P(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = rcv_clkP;
                            Nois_Q(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = rcv_clkQ;
                        }
                    }
                    if(trans_para->curr_C3IFB3_falg)
                    {
                        IFB_n++;
                        Covar_P(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = sat_IFBP;Nois_Q(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = sat_IFBQ;
                    }
                    if(trans_para->curr_C3IFB4_falg)
                    {
                        IFB_n++;
                        Covar_P(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = sat_IFBP;Nois_Q(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = sat_IFBQ;
                    }
                    if(trans_para->curr_C3IFB5_falg)
                    {
                        IFB_n++;
                        Covar_P(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = sat_IFBP;Nois_Q(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = sat_IFBQ;
                    }
                }
            }
            else if(m_sys_str[j]=='G')
            {
                if(sys_len[j+bds_flag] == 1)
                {
                    clk_ref++;
                    if(clk_ref==1)
                    {
                        Covar_P(4,4) = rcv_clkP;Nois_Q(4,4) = rcv_clkQ;
                    }
                    else
                    {
                        if(opt_corr->sys_error_select==ISBs_mode)
                        {
                            Covar_P(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = rcv_clkP;
                            Nois_Q(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = 10;
                        }
                        else
                        {
                            Covar_P(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = rcv_clkP;
                            Nois_Q(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = rcv_clkQ;
                        }
                    }
                    if(trans_para->curr_GIFB_falg)
                    {
                        IFB_n++;
                        Covar_P(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = sat_IFBP;Nois_Q(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = sat_IFBQ;
                    }
                }
            }
            else if(m_sys_str[j]=='E')
            {
                if(sys_len[j+bds_flag] == 1)
                {
                    clk_ref++;
                    if(clk_ref==1)
                    {
                        Covar_P(4,4) = rcv_clkP;Nois_Q(4,4) = rcv_clkQ;
                    }
                    else
                    {
                        if(opt_corr->sys_error_select==ISBs_mode)
                        {
                            Covar_P(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = rcv_clkP;
                            Nois_Q(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = 10;
                        }
                        else
                        {
                            Covar_P(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = rcv_clkP;
                            Nois_Q(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = rcv_clkQ;
                        }
                    }
                    if(trans_para->curr_EIFB3_falg)
                    {
                        IFB_n++;
                        Covar_P(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = sat_IFBP;Nois_Q(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = sat_IFBQ;
                    }
                    if(trans_para->curr_EIFB4_falg)
                    {
                        IFB_n++;
                        Covar_P(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = sat_IFBP;Nois_Q(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = sat_IFBQ;
                    }
                    if(trans_para->curr_EIFB5_falg)
                    {
                        IFB_n++;
                        Covar_P(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = sat_IFBP;Nois_Q(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = sat_IFBQ;
                    }
                }
            }
            else if(m_sys_str[j]=='R')
            {
                if(sys_len[j+bds_flag] == 1)
                {
                    clk_ref++;
                    if(clk_ref==1)
                    {
                        Covar_P(4,4) = rcv_clkP;Nois_Q(4,4) = rcv_clkQ;
                    }
                    else
                    {
                        if(opt_corr->sys_error_select==ISBs_mode)
                        {
                            Covar_P(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = rcv_clkP;
                            Nois_Q(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = 10;
                        }
                        else
                        {
                            Covar_P(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = rcv_clkP;
                            Nois_Q(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = rcv_clkQ;
                        }
                    }
                }
            }
        }

        for(int i = 0; i < Avai_satN;i++)
        {
            SatlitData Pelement = Current_sat.at(i);
            Nois_Q(i+const_parm,i+const_parm) = sat_ionQ;
            Covar_P(i+const_parm,i+const_parm) = sat_ionP;
            for(int mufre=0;mufre<5;mufre++)
            {
                if(Pelement.ObsFreflag[mufre]!=1)  continue;
                Nois_Q(PAcoeff1+Avai_satN+const_parm,PAcoeff1+Avai_satN+const_parm) = sat_ambQ;
                Covar_P(PAcoeff1+Avai_satN+const_parm,PAcoeff1+Avai_satN+const_parm) = sat_ambP;// for Ambiguity
                PAcoeff1++;
            }
        }
        break;
    case PPP_STATIC_KALM:
    case PPP_KINEMATIC_KALM:
        State_X.resize(Avai_satN+const_parm+Total_sat_fre);
        State_X.setZero(Avai_satN+const_parm+Total_sat_fre);
        Nois_Q.resize(Avai_satN+const_parm+Total_sat_fre,Avai_satN+const_parm+Total_sat_fre);
        Nois_Q.setZero(Avai_satN+const_parm+Total_sat_fre,Avai_satN+const_parm+Total_sat_fre);
        Covar_P.resize(Avai_satN+const_parm+Total_sat_fre,Avai_satN+const_parm+Total_sat_fre);
        Covar_P.setZero(Avai_satN+const_parm+Total_sat_fre,Avai_satN+const_parm+Total_sat_fre);
        state_Fi.resize(Avai_satN+const_parm+Total_sat_fre,Avai_satN+const_parm+Total_sat_fre);
        state_Fi.setIdentity(Avai_satN+const_parm+Total_sat_fre,Avai_satN+const_parm+Total_sat_fre);

        Covar_P(3,3) =  trop_covarP;Nois_Q(3,3) = trop_covarQ;
//        Covar_P(4,4) = rcv_clkP;Nois_Q(4,4) = rcv_clkQ;
        bds_flag=0;IFB_n=0;clk_ref=0;
        for(int j=0;j<m_sys_str.length();j++)
        {
            if(m_sys_str[j]=='C')
            {
                if(sys_len[j] == 1)
                {
                    clk_ref++;
                    if(clk_ref==1)
                    {
                        Covar_P(4,4) = rcv_clkP;Nois_Q(4,4) = rcv_clkQ;
                    }
                    else
                    {
                        if(opt_corr->sys_error_select==ISBs_mode)
                        {
                            Covar_P(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = rcv_clkP;
                            Nois_Q(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = 10;
                        }
                        else
                        {
                            Covar_P(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = rcv_clkP;
                            Nois_Q(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = rcv_clkQ;
                        }
                    }
                    if(trans_para->curr_C2IFB_falg)
                    {
                        IFB_n++;
                        Covar_P(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = sat_IFBP;Nois_Q(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = sat_IFBQ;
                    }
                }
                bds_flag++;
                if(sys_len[j+bds_flag] == 1)
                {
                    clk_ref++;
                    if(clk_ref==1)
                    {
                        Covar_P(4,4) = rcv_clkP;Nois_Q(4,4) = rcv_clkQ;
                    }
                    else
                    {
                        if(opt_corr->sys_error_select==ISBs_mode)
                        {
                            Covar_P(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = rcv_clkP;
                            Nois_Q(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = 10;
                        }
                        else
                        {
                            Covar_P(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = rcv_clkP;
                            Nois_Q(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = rcv_clkQ;
                        }
                    }
                    if(trans_para->curr_C3IFB3_falg)
                    {
                        IFB_n++;
                        Covar_P(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = sat_IFBP;Nois_Q(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = sat_IFBQ;
                    }
                    if(trans_para->curr_C3IFB4_falg)
                    {
                        IFB_n++;
                        Covar_P(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = sat_IFBP;Nois_Q(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = sat_IFBQ;
                    }
                    if(trans_para->curr_C3IFB5_falg)
                    {
                        IFB_n++;
                        Covar_P(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = sat_IFBP;Nois_Q(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = sat_IFBQ;
                    }
                }
            }
            else if(m_sys_str[j]=='G')
            {
                if(sys_len[j+bds_flag] == 1)
                {
                    clk_ref++;
                    if(clk_ref==1)
                    {
                        Covar_P(4,4) = rcv_clkP;Nois_Q(4,4) = rcv_clkQ;
                    }
                    else
                    {
                        if(opt_corr->sys_error_select==ISBs_mode)
                        {
                            Covar_P(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = rcv_clkP;
                            Nois_Q(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = 10;
                        }
                        else
                        {
                            Covar_P(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = rcv_clkP;
                            Nois_Q(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = rcv_clkQ;
                        }
                    }
                    if(trans_para->curr_GIFB_falg)
                    {
                        IFB_n++;
                        Covar_P(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = sat_IFBP;Nois_Q(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = sat_IFBQ;
                    }
                }
            }
            else if(m_sys_str[j]=='E')
            {
                if(sys_len[j+bds_flag] == 1)
                {
                    clk_ref++;
                    if(clk_ref==1)
                    {
                        Covar_P(4,4) = rcv_clkP;Nois_Q(4,4) = rcv_clkQ;
                    }
                    else
                    {
                        if(opt_corr->sys_error_select==ISBs_mode)
                        {
                            Covar_P(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = rcv_clkP;
                            Nois_Q(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = 10;
                        }
                        else
                        {
                            Covar_P(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = rcv_clkP;
                            Nois_Q(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = rcv_clkQ;
                        }
                    }
                    if(trans_para->curr_EIFB3_falg)
                    {
                        IFB_n++;
                        Covar_P(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = sat_IFBP;Nois_Q(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = sat_IFBQ;
                    }
                    if(trans_para->curr_EIFB4_falg)
                    {
                        IFB_n++;
                        Covar_P(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = sat_IFBP;Nois_Q(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = sat_IFBQ;
                    }
                    if(trans_para->curr_EIFB5_falg)
                    {
                        IFB_n++;
                        Covar_P(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = sat_IFBP;Nois_Q(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = sat_IFBQ;
                    }
                }
            }
            else if(m_sys_str[j]=='R')
            {
                if(sys_len[j+bds_flag] == 1)
                {
                    clk_ref++;
                    if(clk_ref==1)
                    {
                        Covar_P(4,4) = rcv_clkP;Nois_Q(4,4) = rcv_clkQ;
                    }
                    else
                    {
                        if(opt_corr->sys_error_select==ISBs_mode)
                        {
                            Covar_P(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = rcv_clkP;
                            Nois_Q(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = 10;
                        }
                        else
                        {
                            Covar_P(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = rcv_clkP;
                            Nois_Q(3+clk_ref+IFB_n,3+clk_ref+IFB_n) = rcv_clkQ;
                        }
                    }
                }
            }
        }


        for(int i = 0; i < Avai_satN;i++)
        {
            SatlitData Pelement = Current_sat.at(i);
            Nois_Q(i+const_parm,i+const_parm) = sat_ionQ;
            Covar_P(i+const_parm,i+const_parm) = sat_ionP;
            for(int mufre=0;mufre<5;mufre++)
            {
                if(Pelement.ObsFreflag[mufre]!=1)  continue;
                Nois_Q(PAcoeff1+Avai_satN+const_parm,PAcoeff1+Avai_satN+const_parm) = sat_ambQ;
                Covar_P(PAcoeff1+Avai_satN+const_parm,PAcoeff1+Avai_satN+const_parm) = sat_ambP;// for Ambiguity
                PAcoeff1++;
            }
        }
        if(filter_model==PPP_KINEMATIC_KALM)
        {
            for(int i=0;i<3;i++)
            {
                Covar_P(i,i) = xyz_kinematic;
                Nois_Q(i,i) = xyz_kinematic;
            }
        }
        else
        {
            for(int i=0;i<3;i++)    Covar_P(i,i) = xyz_static;
        }
        break;
    default:
        break;
    }
    State_X = (Coef_B.transpose()*Coef_B).inverse()*Coef_B.transpose()*Obs_L;
    trans_para->State_X = State_X;
    trans_para->Covar_P=Covar_P;
    trans_para->Nois_Q=Nois_Q;
    trans_para->state_Fi=state_Fi;
    trans_para->isInitPara = true;//No longer initialized after
}
// get matrix B and observer L for IF Combination
void coefficient_matrix::Obtaining_IF_equation(QVector< SatlitData > &Current_sat, double *m_ApproxRecPos, MatrixXd &mat_B, VectorXd &Vct_L,
                             MatrixXd &mat_P,const corr_opt *opt_corr,para_trans *trans_para)
{
    int Avai_satN = Current_sat.length();
    MatrixXd B, P;
    VectorXd L;
    VectorXd sys_len = trans_para->sys_len;
    QString m_sys_str=opt_corr->m_sys_str;
    int sys_num=trans_para->sys_num;
    int const_parm = 4+sys_num;
    trans_para->const_parm = const_parm;
    disply_textEdit *text_disp= AGNSS_APPData::sGetAPPDataInstance()->mGettrans_textEdit();
    QString aaa ;
    switch(opt_corr->filter_model)
    {
    case PPP_STATIC_SRIF:
    case PPP_KINEMATIC_SRIF:
    case PPP_STATIC_KALM:
    case PPP_KINEMATIC_KALM:
        B.resize(Avai_satN*2,Avai_satN+const_parm);
        P.resize(Avai_satN*2,Avai_satN*2);
        L.resize(Avai_satN*2);
        B.setZero(Avai_satN*2,Avai_satN+const_parm);
        L.setZero(Avai_satN*2);
        P.setIdentity();
        break;
    default:
        {
            aaa = "Obtaining_IF_equation you should use setModel().";
            text_disp->data_text.append(aaa);
            text_disp->data_lenth++;
        }
        break;
    }
    for (int i = 0; i < Avai_satN;i++)
    {
        SatlitData oneSatlit = Current_sat.at(i);
        double XYZ_coef[3],p0 = 0,satXYZ[3]={oneSatlit.X,oneSatlit.Y,oneSatlit.Z};
//        p0 = m_mat_cal.geodist(satXYZ,m_ApproxRecPos,XYZ_coef);
        double li = 0,mi = 0,ni = 0,dltaX = 0,dltaY = 0,dltaZ = 0;
        dltaX = oneSatlit.X - m_ApproxRecPos[0];
        dltaY = oneSatlit.Y - m_ApproxRecPos[1];
        dltaZ = oneSatlit.Z - m_ApproxRecPos[2];
        p0 = qSqrt(dltaX*dltaX+dltaY*dltaY+dltaZ*dltaZ);
        // compute li mi ni
        li = dltaX/p0;mi = dltaY/p0;ni = dltaZ/p0;
        //Correction of each
        double dlta = 0;
        dlta =  - oneSatlit.StaClock + oneSatlit.SatTrop - oneSatlit.Relativty -
            oneSatlit.Sagnac - oneSatlit.TideEffect - oneSatlit.AntHeight;
        // set B L P
        int clk_ref=0,bds_flag=0;
        switch(opt_corr->filter_model)
        {
        case PPP_STATIC_SRIF:
        case PPP_KINEMATIC_SRIF:
        case PPP_STATIC_KALM:
        case PPP_KINEMATIC_KALM:
            //Computational B matrix
            //L3 carrier matrix
            B(i*2,0) = li;B(i*2,1) = mi;B(i*2,2) = ni;
            B(i*2+1,0) = li;B(i*2+1,1) = mi;B(i*2+1,2) = ni;
            B(i*2,3) = -oneSatlit.StaTropMap;
            B(i*2+1,3) = -oneSatlit.StaTropMap;

            for(int j=0;j<m_sys_str.length();j++)
            {
                if(m_sys_str[j] == oneSatlit.SatType&&oneSatlit.SatType=='C'&&oneSatlit.BDS_2flag)
                {
                    B(i*2,4+clk_ref) = -1;
                    B(i*2+1,4+clk_ref) = -1;
                    if(clk_ref!=0&&opt_corr->sys_error_select==ISBs_mode)
                        B(i*2+1,4) = B(i*2,4) = -1;
                }
                else if(m_sys_str[j] == oneSatlit.SatType&&oneSatlit.SatType=='C')
                {
                    if(sys_len[j]==1)   clk_ref++;
                    B(i*2,4+clk_ref) = -1;
                    B(i*2+1,4+clk_ref) = -1;
                    if(clk_ref!=0&&opt_corr->sys_error_select==ISBs_mode)
                        B(i*2+1,4) = B(i*2,4) = -1;
                }
                else if(m_sys_str[j] == oneSatlit.SatType)
                {
                    if(clk_ref!=0&&opt_corr->sys_error_select==ISBs_mode)
                        B(i*2+1,4) = B(i*2,4) = -1;
                    B(i*2,4+clk_ref) = -1;
                    B(i*2+1,4+clk_ref) = -1;
                }
                if(m_sys_str[j]=='C')
                {
                    if(sys_len[j] == 1) clk_ref++;
                    bds_flag++;
                    if(sys_len[j+bds_flag] == 1) clk_ref++;
                }
                else
                    if(sys_len[j+bds_flag] == 1) clk_ref++;
            }
            for (int n = 0;n < Avai_satN;n++)//The diagonal part of the rear part initializes the wavelength of Lamta3, and the rest is 0.
                if (i == n)
                    B(i*2,const_parm+n) = M_GetLamta3(oneSatlit.Frq[oneSatlit.IF_Frq[0]],oneSatlit.Frq[oneSatlit.IF_Frq[1]]);//LL3 wavelength  M_GetLamta3(oneSatlit.Frq[oneSatlit.IF_Frq[0]],oneSatlit.Frq[oneSatlit.IF_Frq[1]])
            //Carrier L  pseudorange code L
            L(i*2) = p0 - oneSatlit.IF_L + dlta;
            L(i*2+1) = p0 - oneSatlit.IF_P + dlta;
            // Computing weight matrix P
            P(i*2, i*2) = oneSatlit.SatWight * oneSatlit.SatWight_L*oneSatlit.SatWight_IF[0];// Carrier L1 weight
            P(i*2+1, i*2+1) = oneSatlit.SatWight *oneSatlit.SatWight_IF[1];// Carrier L2 weight
            break;
        default:
            {
                aaa = "Obtaining_equation you should use setModel().";
                text_disp->data_text.append(aaa);
                text_disp->data_lenth++;
            }
            break;
        }

    }//B, L is calculated
    // save data to mat_B
    mat_B = B;
    Vct_L = L;
    mat_P = P;
}
// get matrix B and observer L for No Combination
void coefficient_matrix::Obtaining_NoCombination_equation(QVector< SatlitData > &Current_sat, double *m_ApproxRecPos, MatrixXd &mat_B, VectorXd &Vct_L,
                             MatrixXd &mat_P,int Total_sat_fre,const corr_opt *opt_corr,para_trans *trans_para)
{
    int Avai_satN = Current_sat.length();
    MatrixXd B, P;
    VectorXd L;
    VectorXd sys_len = trans_para->sys_len;
    QString m_sys_str=opt_corr->m_sys_str;
    int sys_num=trans_para->sys_num;
    int const_parm = 4+sys_num;
    trans_para->const_parm = const_parm;
    disply_textEdit *text_disp= AGNSS_APPData::sGetAPPDataInstance()->mGettrans_textEdit();

    QString aaa ;
    switch(opt_corr->filter_model)
    {
    case PPP_STATIC_SRIF:
    case PPP_KINEMATIC_SRIF:
    case PPP_STATIC_KALM:
    case PPP_KINEMATIC_KALM:
        B.resize(Total_sat_fre*2,const_parm+Avai_satN+Total_sat_fre);
        P.resize(Total_sat_fre*2,Total_sat_fre*2);
        L.resize(Total_sat_fre*2);
        B.setZero(Total_sat_fre*2,const_parm+Avai_satN+Total_sat_fre);
        L.setZero(Total_sat_fre*2);
        P.setIdentity();
        break;
    default:
        {
            aaa = "Obtaining_IF_equation you should use setModel().";
            text_disp->data_text.append(aaa);
            text_disp->data_lenth++;
        }
        break;
    }
    int column_pos =0;
    for (int i = 0; i < Avai_satN;i++)
    {
        SatlitData oneSatlit = Current_sat.at(i);
        double XYZ_coef[3],p0 = 0;
//        p0 = m_mat_cal.geodist(satXYZ,m_ApproxRecPos,XYZ_coef);
        double li = 0,mi = 0,ni = 0,dltaX = 0,dltaY = 0,dltaZ = 0;
        dltaX = oneSatlit.X - m_ApproxRecPos[0];
        dltaY = oneSatlit.Y - m_ApproxRecPos[1];
        dltaZ = oneSatlit.Z - m_ApproxRecPos[2];
        p0 = qSqrt(dltaX*dltaX+dltaY*dltaY+dltaZ*dltaZ);
        li = dltaX/p0;mi = dltaY/p0;ni = dltaZ/p0;
        XYZ_coef[0]=li;XYZ_coef[1]=mi;XYZ_coef[2]=ni;
        //Correction of each
        double dlta = 0;
        dlta =  - oneSatlit.StaClock + oneSatlit.SatTrop - oneSatlit.Relativty -
            oneSatlit.Sagnac - oneSatlit.TideEffect - oneSatlit.AntHeight;
        // set B L P
        int one_fre = 0;
        double fre1=0,fre_n=0;int clk_ref=0,bds_flag=0;
        switch(opt_corr->filter_model)
        {
        case PPP_STATIC_SRIF:
        case PPP_KINEMATIC_SRIF:
        case PPP_STATIC_KALM:
        case PPP_KINEMATIC_KALM:
            //Computational B matrix
            //L3 carrier matrix
            for(int mufre=0;mufre<5;mufre++)
            {
                if(oneSatlit.ObsFreflag[mufre]!=1)  continue;
                for(int j=0;j<3;j++)
                {
                    B(column_pos*2,j) = XYZ_coef[j];
                    B(column_pos*2+1,j) = XYZ_coef[j];
                }
                B(column_pos*2,3) = -oneSatlit.StaTropMap;
                B(column_pos*2+1,3) = -oneSatlit.StaTropMap;
                clk_ref=0;bds_flag=0;
                for(int j=0;j<m_sys_str.length();j++)
                {
                    if(m_sys_str[j] == oneSatlit.SatType&&oneSatlit.SatType=='C'&&oneSatlit.BDS_2flag)
                    {
                        B(column_pos*2,4+clk_ref) = -1;
                        B(column_pos*2+1,4+clk_ref) = -1;
                        if(clk_ref!=0&&opt_corr->sys_error_select==ISBs_mode)
                            B(column_pos*2+1,4) = B(column_pos*2,4) = -1;
                    }
                    else if(m_sys_str[j] == oneSatlit.SatType&&oneSatlit.SatType=='C')
                    {
                        if(sys_len[j]==1)   clk_ref++;
                        B(column_pos*2,4+clk_ref) = -1;
                        B(column_pos*2+1,4+clk_ref) = -1;
                        if(clk_ref!=0&&opt_corr->sys_error_select==ISBs_mode)
                            B(column_pos*2+1,4) = B(column_pos*2,4) = -1;
                    }
                    else if(m_sys_str[j] == oneSatlit.SatType)
                    {
                        B(column_pos*2,4+clk_ref) = -1;
                        B(column_pos*2+1,4+clk_ref) = -1;
                        if(clk_ref!=0&&opt_corr->sys_error_select==ISBs_mode)
                            B(column_pos*2+1,4) = B(column_pos*2,4) = -1;
                    }
                    if(m_sys_str[j]=='C')
                    {
                        if(sys_len[j] == 1) clk_ref++;
                        bds_flag++;
                        if(sys_len[j+bds_flag] == 1) clk_ref++;
                    }
                    else
                        if(sys_len[j+bds_flag] == 1) clk_ref++;
                }
                if(one_fre == 0)
                {
                    one_fre = 1;

                    fre1 = oneSatlit.Frq[mufre];
                }
                fre_n = oneSatlit.Frq[mufre];
                B(column_pos*2,const_parm+i) = (fre1*fre1)/(fre_n*fre_n);
                B(column_pos*2,const_parm+Avai_satN+column_pos) = oneSatlit.Lambda[mufre];
                B(column_pos*2+1,const_parm+i) = -(fre1*fre1)/(fre_n*fre_n);
                //Carrier L  pseudorange code L
                L(column_pos*2) = p0 - oneSatlit.PhaseLL[mufre] + dlta;
                L(column_pos*2+1) = p0 - oneSatlit.CodePP[mufre] + dlta;
                // Computing weight matrix P
                P(column_pos*2, column_pos*2) = oneSatlit.SatWight * oneSatlit.SatWight_L *oneSatlit.SatWight_UD[2*mufre];// Carrier L1 weight
                P(column_pos*2+1, column_pos*2+1) = oneSatlit.SatWight*oneSatlit.SatWight_UD[2*mufre+1];// Carrier L2 weight
                column_pos++;
            }
            break;
        default:
            {
                aaa = "Obtaining_equation you should use setModel().";
                text_disp->data_text.append(aaa);
                text_disp->data_lenth++;
            }
            break;
        }

    }//B, L is calculated
    // save data to mat_B
    mat_B = B;
    Vct_L = L;
    mat_P = P;

}
//Change the parameter size (only PPP can change paramater)
void coefficient_matrix::change_IF_Paramatrix(QVector< SatlitData > &preEpoch, QVector< SatlitData > &current_Sat,
                                              QVector< int >oldPrnFlag,int preEpochLen,QVector< int > &NewPrnFlag,const corr_opt *opt_corr,para_trans *trans_para)
{
    int sys_num = trans_para->sys_num;
    int const_parm = 4+sys_num;
    trans_para->const_parm = const_parm;
    double rcv_clkQ = opt_corr->rcv_clkQ;
    double sat_ambQ = opt_corr->sat_ambQ;
    double rcv_clkP = opt_corr->rcv_clkP;
    double sat_ambP = opt_corr->sat_ambP;
    trans_para->m_VarChang = false;
    VectorXd State_X = trans_para->State_X;
    MatrixXd Covar_P = trans_para->Covar_P;
    MatrixXd Nois_Q = trans_para->Nois_Q;
    VectorXd sys_len = trans_para->sys_len;
    VectorXd up_sys_len=trans_para->up_sys_len;
    MatrixXd state_Fi;
    int Avai_satN = current_Sat.length();
//MU_num Number of ambiguity szg  Avai_satN Number of ionosphere unknowns
    state_Fi.resize(const_parm+Avai_satN,const_parm+Avai_satN);
    state_Fi.setIdentity();

    VectorXd tempXk_1 = State_X;
    MatrixXd tempPk_1 = Covar_P;
    MatrixXd tempQk_1 = Nois_Q;
    State_X.resize(Avai_satN+const_parm);
    State_X.setZero(Avai_satN+const_parm);
    Covar_P.resize(Avai_satN+const_parm, Avai_satN+const_parm);
    Nois_Q.resize(Avai_satN+const_parm, Avai_satN+const_parm);

    int clk_n =0;
    for(int i=0;i<sys_len.size();i++)
    {
        if(sys_len[i]==up_sys_len[i]&&up_sys_len[i]==1)
        {
            clk_n++;
        }
        else if(sys_len[i]==1)
        {
            VectorXd head_Xn = tempXk_1.head(4+clk_n);
            VectorXd tail_Xn = tempXk_1.tail(tempXk_1.size()-4-clk_n);
            tempXk_1.resize(head_Xn.size()+tail_Xn.size()+1);//szg
            tempXk_1.head(head_Xn.size()) = head_Xn;
            tempXk_1.tail(tail_Xn.size()) = tail_Xn;
            if(head_Xn.size()>4)
            {
                if(opt_corr->sys_error_select==ISBs_mode)
                    tempXk_1(4+clk_n) = 0;
                else
                    tempXk_1(4+clk_n) = head_Xn(4);
            }
            else
            {
                if(opt_corr->sys_error_select==ISBs_mode)
                    tempXk_1(4+clk_n) = 0;
                else
                    tempXk_1(4+clk_n) = tail_Xn(0);
            }
            int Pkcols=tempPk_1.cols();

            MatrixXd tempPk_2;
            tempPk_2.resize(Pkcols+1,Pkcols+1);
            tempPk_2.setZero(Pkcols+1,Pkcols+1);
            tempPk_2.block(0,0,4+clk_n,4+clk_n) = tempPk_1.block(0,0,4+clk_n,4+clk_n);
            tempPk_2.block(5+clk_n,5+clk_n,Pkcols-4-clk_n,Pkcols-4-clk_n) = tempPk_1.block(4+clk_n,4+clk_n,Pkcols-4-clk_n,Pkcols-4-clk_n);
            tempPk_2.block(5+clk_n,0,Pkcols-4-clk_n,4+clk_n) = tempPk_1.block(4+clk_n,0,Pkcols-4-clk_n,4+clk_n);
            tempPk_2.block(0,5+clk_n,4+clk_n,Pkcols-4-clk_n) = tempPk_1.block(0,4+clk_n,4+clk_n,Pkcols-4-clk_n);

            if(clk_n!=0&&opt_corr->sys_error_select==ISBs_mode)
                tempPk_2(4+clk_n,4+clk_n) = 100;
            else
                tempPk_2(4+clk_n,4+clk_n) = rcv_clkP;
            int Pk2cols=tempPk_2.cols();
            tempPk_1.resize(Pk2cols,Pk2cols);
            tempPk_1 = tempPk_2;

            MatrixXd tempQk_2;
            int Qk1cols = tempQk_1.cols();
            tempQk_2.resize(Qk1cols+1,Qk1cols+1);
            tempQk_2.setZero(Qk1cols+1,Qk1cols+1);
            tempQk_2.block(0,0,4+clk_n,4+clk_n) = tempQk_1.block(0,0,4+clk_n,4+clk_n);
            tempQk_2.block(5+clk_n,5+clk_n,Qk1cols-4-clk_n,Qk1cols-4-clk_n) = tempQk_1.block(4+clk_n,4+clk_n,Qk1cols-4-clk_n,Qk1cols-4-clk_n);
            if(clk_n!=0&&opt_corr->sys_error_select==ISBs_mode)
                tempQk_2(4+clk_n,4+clk_n) = 1;
            else
                tempQk_2(4+clk_n,4+clk_n) = rcv_clkQ;
            int Qk2cols = tempQk_2.cols();
            tempQk_1.resize(Qk2cols,Qk2cols);
            tempQk_1 = tempQk_2;
            clk_n++;
        }
        else if(up_sys_len[i]==1)
        {
            VectorXd head_Xn = tempXk_1.head(4+clk_n);
            VectorXd tail_Xn = tempXk_1.tail(tempXk_1.size()-4-clk_n-1);
            tempXk_1.resize(head_Xn.size()+tail_Xn.size());//szg
            tempXk_1<<head_Xn,tail_Xn;
            m_mat_cal.RemoveRow(tempQk_1,4+clk_n);
            m_mat_cal.RemoveColumn(tempQk_1,4+clk_n);
            m_mat_cal.RemoveRow(tempPk_1,4+clk_n);
            m_mat_cal.RemoveColumn(tempPk_1,4+clk_n);
            if(clk_n==0&&opt_corr->sys_error_select==ISBs_mode)
            {
                tempPk_1(4+clk_n,4+clk_n) = rcv_clkP;
                tempQk_1(4+clk_n,4+clk_n) = rcv_clkQ;
                for(int j=5;j<const_parm;j++)
                {
                    tempPk_1(j,j) = 100;
                }
            }
        }
    }
    int lenVR = const_parm;
    for(int i = 0;i<preEpochLen;i++)
    {
        if(NewPrnFlag.at(i)!=-1)
        {
            lenVR++;
        }
        else
        {
            if(lenVR == tempXk_1.size()-1)
            {
                VectorXd head_Xn = tempXk_1.head(lenVR);
                tempXk_1.resize(head_Xn.size());
                tempXk_1=head_Xn;
            }
            else
            {
                VectorXd head_Xn = tempXk_1.head(lenVR);
                VectorXd tail_Xn = tempXk_1.tail(tempXk_1.size()-lenVR-1);
                tempXk_1.resize(head_Xn.size()+tail_Xn.size());//szg
                tempXk_1<<head_Xn,tail_Xn;
            }
            m_mat_cal.RemoveRow(tempQk_1,lenVR);
            m_mat_cal.RemoveColumn(tempQk_1,lenVR);
            m_mat_cal.RemoveRow(tempPk_1,lenVR);
            m_mat_cal.RemoveColumn(tempPk_1,lenVR);
        }
    }
    for (int n = 0; n < Avai_satN;n++)
    {
        SatlitData oneStalit = current_Sat.at(n);
        if (oldPrnFlag.at(n) == -1)
        {
            if(n==Avai_satN-1)
            {
                VectorXd head_Xn = tempXk_1.head(const_parm+n);
                tempXk_1.resize(tempXk_1.size()+1);
                tempXk_1.head(head_Xn.size()) = head_Xn;
                tempXk_1(const_parm+n) = (oneStalit.IF_P - oneStalit.IF_L)/M_GetLamta3(oneStalit.Frq[oneStalit.IF_Frq[0]],oneStalit.Frq[oneStalit.IF_Frq[1]]);
                MatrixXd tempPk_2;
                int Pkcols=tempPk_1.cols();
                tempPk_2.resize(Pkcols+1,Pkcols+1);
                tempPk_2.setZero(Pkcols+1,Pkcols+1);
                tempPk_2.block(0,0,const_parm+n,const_parm+n) = tempPk_1.block(0,0,const_parm+n,const_parm+n);
                tempPk_2(const_parm+n,const_parm+n) = sat_ambP;
                int Pk2cols=tempPk_2.cols();
                tempPk_1.resize(Pk2cols,Pk2cols);
                tempPk_1 = tempPk_2;
                MatrixXd tempQk_2;
                int Qk1cols = tempQk_1.cols();
                tempQk_2.resize(Qk1cols+1,Qk1cols+1);
                tempQk_2.setZero(Qk1cols+1,Qk1cols+1);
                tempQk_2.block(0,0,const_parm+n,const_parm+n) = tempQk_1.block(0,0,const_parm+n,const_parm+n);
                tempQk_2(const_parm+n,const_parm+n) = sat_ambQ;
                int Qk2cols = tempQk_2.cols();
                tempQk_1.resize(Qk2cols,Qk2cols);
                tempQk_1 = tempQk_2;
            }
            else
            {
                VectorXd head_Xn = tempXk_1.head(const_parm+n);
                VectorXd tail_Xn = tempXk_1.tail(tempXk_1.size()-const_parm-n);
                tempXk_1.resize(tempXk_1.size()+1);
                tempXk_1.head(head_Xn.size()) = head_Xn;
                tempXk_1.tail(tail_Xn.size()) = tail_Xn;
                tempXk_1(const_parm+n) = (oneStalit.IF_P - oneStalit.IF_L)/M_GetLamta3(oneStalit.Frq[oneStalit.IF_Frq[0]],oneStalit.Frq[oneStalit.IF_Frq[1]]);
                MatrixXd tempPk_2;
                int Pkcols=tempPk_1.cols();
                tempPk_2.resize(Pkcols+1,Pkcols+1);
                tempPk_2.setZero(Pkcols+1,Pkcols+1);
                tempPk_2.block(0,0,const_parm+n,const_parm+n) = tempPk_1.block(0,0,const_parm+n,const_parm+n);
                tempPk_2.block(0,const_parm+n+1,const_parm+n,Pkcols-const_parm-n) = tempPk_1.block(0,const_parm+n,const_parm+n,Pkcols-const_parm-n);
                tempPk_2.block(const_parm+n+1,0,Pkcols-const_parm-n,const_parm+n) = tempPk_1.block(const_parm+n,0,Pkcols-const_parm-n,const_parm+n);
                tempPk_2.block(const_parm+n+1,const_parm+n+1,Pkcols-const_parm-n,Pkcols-const_parm-n) = tempPk_1.block(const_parm+n,const_parm+n,Pkcols-const_parm-n,Pkcols-const_parm-n);
                tempPk_2(const_parm+n,const_parm+n) = sat_ambP;
                int Pk2cols=tempPk_2.cols();
                tempPk_1.resize(Pk2cols,Pk2cols);
                tempPk_1 = tempPk_2;
                MatrixXd tempQk_2;
                int Qk1cols = tempQk_1.cols();
                tempQk_2.resize(Qk1cols+1,Qk1cols+1);
                tempQk_2.setZero(Qk1cols+1,Qk1cols+1);
                tempQk_2.block(0,0,const_parm+n,const_parm+n) = tempQk_1.block(0,0,const_parm+n,const_parm+n);
                tempQk_2.block(const_parm+n+1,const_parm+n+1,Qk1cols-const_parm-n,Qk1cols-const_parm-n) = tempQk_1.block(const_parm+n,const_parm+n,Qk1cols-const_parm-n,Qk1cols-const_parm-n);
                tempQk_2(const_parm+n,const_parm+n) = sat_ambQ;
                int Qk2cols = tempQk_2.cols();
                tempQk_1.resize(Qk2cols,Qk2cols);
                tempQk_1 = tempQk_2;
            }
        }
        else if(oneStalit.cycly_slipF == 1)
        {
            tempXk_1(const_parm+n) = (oneStalit.IF_P - oneStalit.IF_L)/M_GetLamta3(oneStalit.Frq[oneStalit.IF_Frq[0]],oneStalit.Frq[oneStalit.IF_Frq[1]]);
            for(int j=0;j<tempXk_1.size();j++)
            {
                if(const_parm+n == j)
                    tempPk_1(const_parm+n,const_parm+n)=sat_ambP;
                else
                {
                    tempPk_1(const_parm+n,j) = 0;
                    tempPk_1(j,const_parm+n) = 0;
                }
            }
        }

    }

    int Pkcols=tempPk_1.cols();
    if(Covar_P.cols()!=Pkcols)
    {
//        qDebug()<<"Covar_P.cols()!=Pkcols"<<endl;
        return;
    }

    Covar_P = tempPk_1;
    State_X = tempXk_1;
    Nois_Q = tempQk_1;
    trans_para->State_X = State_X;
    trans_para->Covar_P=Covar_P;
    trans_para->Nois_Q=Nois_Q;
    trans_para->state_Fi=state_Fi;
    trans_para->m_VarChang = true;//No longer initialized after
}
//Change the parameter size (only PPP can change paramater)
void coefficient_matrix::change_NoCombination_Para(QVector< SatlitData > &preEpoch, QVector< SatlitData > &current_Sat,
                                              QVector< int >oldPrnFlag,int preEpochLen,QVector< int > &NewPrnFlag,int Total_sat_fre,const corr_opt *opt_corr,para_trans *trans_para)
{
    int sys_num = trans_para->sys_num;
    int const_parm = 4+sys_num;
    trans_para->const_parm = const_parm;
    double rcv_clkQ = opt_corr->rcv_clkQ;
    double sat_ambQ = opt_corr->sat_ambQ;
    double rcv_clkP = opt_corr->rcv_clkP;
    double sat_ambP = opt_corr->sat_ambP;
    double sat_ionQ = opt_corr->sat_ionQ;
    double sat_ionP = opt_corr->sat_ionP;
    trans_para->m_VarChang = false;
    VectorXd State_X = trans_para->State_X;
    MatrixXd Covar_P = trans_para->Covar_P;
    MatrixXd Nois_Q = trans_para->Nois_Q;
    VectorXd sys_len = trans_para->sys_len;
    VectorXd up_sys_len=trans_para->up_sys_len;
    MatrixXd state_Fi;
    int Avai_satN = current_Sat.length();
//MU_num Number of ambiguity szg  Avai_satN Number of ionosphere unknowns
    state_Fi.resize(const_parm+Total_sat_fre+Avai_satN,const_parm+Total_sat_fre+Avai_satN);
    state_Fi.setIdentity();

    VectorXd tempXk_1 = State_X;
    MatrixXd tempPk_1 = Covar_P;
    MatrixXd tempQk_1 = Nois_Q;
    State_X.resize(const_parm+Total_sat_fre+Avai_satN);
    State_X.setZero(const_parm+Total_sat_fre+Avai_satN);
    Covar_P.resize(const_parm+Total_sat_fre+Avai_satN, const_parm+Total_sat_fre+Avai_satN);
    Nois_Q.resize(const_parm+Total_sat_fre+Avai_satN, const_parm+Total_sat_fre+Avai_satN);

    int clk_n =0;
    for(int i=0;i<sys_len.size();i++)
    {
        if(sys_len[i]==up_sys_len[i]&&up_sys_len[i]==1)
        {
            clk_n++;
        }
        else if(sys_len[i]==1)
        {
            VectorXd head_Xn = tempXk_1.head(4+clk_n);
            VectorXd tail_Xn = tempXk_1.tail(tempXk_1.size()-4-clk_n);
            tempXk_1.resize(head_Xn.size()+tail_Xn.size()+1);//szg
            tempXk_1.head(head_Xn.size()) = head_Xn;
            tempXk_1.tail(tail_Xn.size()) = tail_Xn;
            if(head_Xn.size()>4)
            {
                if(opt_corr->sys_error_select==ISBs_mode)
                    tempXk_1(4+clk_n) = 0;
                else
                    tempXk_1(4+clk_n) = head_Xn(4);
            }
            else
            {
                if(opt_corr->sys_error_select==ISBs_mode)
                    tempXk_1(4+clk_n) = 0;
                else
                    tempXk_1(4+clk_n) = tail_Xn(0);
            }
            MatrixXd tempPk_2;
            int Pkcols=tempPk_1.cols();
            tempPk_2.resize(Pkcols+1,Pkcols+1);
            tempPk_2.setZero(Pkcols+1,Pkcols+1);
            tempPk_2.block(0,0,4+clk_n,4+clk_n) = tempPk_1.block(0,0,4+clk_n,4+clk_n);
            tempPk_2.block(5+clk_n,5+clk_n,Pkcols-4-clk_n,Pkcols-4-clk_n) = tempPk_1.block(4+clk_n,4+clk_n,Pkcols-4-clk_n,Pkcols-4-clk_n);
            tempPk_2.block(5+clk_n,0,Pkcols-4-clk_n,4+clk_n) = tempPk_1.block(4+clk_n,0,Pkcols-4-clk_n,4+clk_n);
            tempPk_2.block(0,5+clk_n,4+clk_n,Pkcols-4-clk_n) = tempPk_1.block(0,4+clk_n,4+clk_n,Pkcols-4-clk_n);
            if(clk_n!=0&&opt_corr->sys_error_select==ISBs_mode)
                tempPk_2(4+clk_n,4+clk_n) = rcv_clkP;
            else
                tempPk_2(4+clk_n,4+clk_n) = rcv_clkP;
            int Pk2cols=tempPk_2.cols();
            tempPk_1.resize(Pk2cols,Pk2cols);
            tempPk_1 = tempPk_2;
            MatrixXd tempQk_2;
            int Qk1cols = tempQk_1.cols();
            tempQk_2.resize(Qk1cols+1,Qk1cols+1);
            tempQk_2.setZero(Qk1cols+1,Qk1cols+1);
            tempQk_2.block(0,0,4+clk_n,4+clk_n) = tempQk_1.block(0,0,4+clk_n,4+clk_n);
            tempQk_2.block(5+clk_n,5+clk_n,Qk1cols-4-clk_n,Qk1cols-4-clk_n) = tempQk_1.block(4+clk_n,4+clk_n,Qk1cols-4-clk_n,Qk1cols-4-clk_n);
            if(clk_n!=0&&opt_corr->sys_error_select==ISBs_mode)
                tempQk_2(4+clk_n,4+clk_n) = 1;
            else
                tempQk_2(4+clk_n,4+clk_n) = rcv_clkQ;
            int Qk2cols = tempQk_2.cols();
            tempQk_1.resize(Qk2cols,Qk2cols);
            tempQk_1 = tempQk_2;
            clk_n++;
        }
        else if(up_sys_len[i]==1)
        {
            VectorXd head_Xn = tempXk_1.head(4+clk_n);
            VectorXd tail_Xn = tempXk_1.tail(tempXk_1.size()-4-clk_n-1);
            tempXk_1.resize(head_Xn.size()+tail_Xn.size());//szg
            tempXk_1<<head_Xn,tail_Xn;
            m_mat_cal.RemoveRow(tempQk_1,4+clk_n);
            m_mat_cal.RemoveColumn(tempQk_1,4+clk_n);
            m_mat_cal.RemoveRow(tempPk_1,4+clk_n);
            m_mat_cal.RemoveColumn(tempPk_1,4+clk_n);
            if(clk_n==0&&opt_corr->sys_error_select==ISBs_mode)
            {
                tempPk_1(4+clk_n,4+clk_n) = rcv_clkP;
                tempQk_1(4+clk_n,4+clk_n) = rcv_clkQ;
                for(int j=5;j<const_parm;j++)
                {
                    tempPk_1(j,j) = rcv_clkP;
                }
            }
        }
    }
    int lenVR = const_parm+preEpochLen;int delete_Num=0;
    for(int i = 0;i<preEpochLen;i++)
    {
        SatlitData preStalit = preEpoch.at(i);
        int prefre = preStalit.ObsLenfre;
        if(NewPrnFlag.at(i)!=-1)
        {
            lenVR=prefre+lenVR;
        }
        else
        {
            if(tempXk_1.size()-lenVR-prefre-1==0)
            {
//                VectorXd head_Xn = tempXk_1.head(const_parm+i);
//                VectorXd tail_Xn = tempXk_1.tail(tempXk_1.size()-const_parm-i-1);
//                tempXk_1.resize(head_Xn.size()+tail_Xn.size());
//                tempXk_1<<head_Xn,tail_Xn;
//                head_Xn = tempXk_1.head(lenVR);
//                tempXk_1=head_Xn;

                VectorXd head_Xn = tempXk_1.head(lenVR);
                tempXk_1.resize(head_Xn.size());
                tempXk_1=head_Xn;
                head_Xn = tempXk_1.head(const_parm+i-delete_Num);
                VectorXd tail_Xn = tempXk_1.tail(tempXk_1.size()-const_parm-i+delete_Num-1);
                tempXk_1.resize(head_Xn.size()+tail_Xn.size());
                tempXk_1<<head_Xn,tail_Xn;
            }
            else
            {
                VectorXd head_Xn = tempXk_1.head(lenVR);
                VectorXd tail_Xn = tempXk_1.tail(tempXk_1.size()-lenVR-prefre);
                tempXk_1.resize(head_Xn.size()+tail_Xn.size());//szg
                tempXk_1<<head_Xn,tail_Xn;
                head_Xn = tempXk_1.head(const_parm+i-delete_Num);
                tail_Xn = tempXk_1.tail(tempXk_1.size()-const_parm-i+delete_Num-1);
                tempXk_1.resize(head_Xn.size()+tail_Xn.size());
                tempXk_1<<head_Xn,tail_Xn;
            }

            for(int rem=0;rem<prefre;rem++)
            {
                m_mat_cal.RemoveRow(tempQk_1,lenVR);
                m_mat_cal.RemoveColumn(tempQk_1,lenVR);
                m_mat_cal.RemoveRow(tempPk_1,lenVR);
                m_mat_cal.RemoveColumn(tempPk_1,lenVR);
            }
            m_mat_cal.RemoveRow(tempPk_1,const_parm+i-delete_Num);
            m_mat_cal.RemoveColumn(tempPk_1,const_parm+i-delete_Num);
            m_mat_cal.RemoveRow(tempQk_1,const_parm+i-delete_Num);
            m_mat_cal.RemoveColumn(tempQk_1,const_parm+i-delete_Num);
            lenVR = lenVR-1;
            delete_Num++;
        }
    }
    int freIn = const_parm+preEpochLen-delete_Num;
    for (int n = 0; n < Avai_satN;n++)
    {
        SatlitData oneStalit = current_Sat.at(n);
        int Lenfre = oneStalit.ObsLenfre;
        if (oldPrnFlag.at(n) == -1)
        {
            if(tempXk_1.size()-freIn==0)
            {
                VectorXd head_Xn = tempXk_1.head(const_parm+n);
                VectorXd tail_Xn = tempXk_1.tail(tempXk_1.size()-const_parm-n);
                tempXk_1.resize(tempXk_1.size()+1);
                tempXk_1.head(head_Xn.size()) = head_Xn;
                tempXk_1.tail(tail_Xn.size()) = tail_Xn;
                tempXk_1(const_parm+n) = 0;
                head_Xn = tempXk_1;
                tempXk_1.resize(tempXk_1.size()+Lenfre);
                tempXk_1.head(head_Xn.size()) = head_Xn;
                int frequetN = 0;
                for(int j=0;j<5;j++)
                {
                    if(oneStalit.ObsFreflag[j]==1)
                    {
                        tempXk_1(freIn+1+frequetN) = (oneStalit.CodeP[j] - oneStalit.PhaseL[j]*oneStalit.Lambda[j])/(M_C/oneStalit.Frq[j]);
                        frequetN++;
                    }
                }
                MatrixXd tempPk_2;
                int Pkcols=tempPk_1.cols();
                tempPk_2.resize(Pkcols+1,Pkcols+1);
                tempPk_2.setZero(Pkcols+1,Pkcols+1);
                tempPk_2.block(0,0,const_parm+n,const_parm+n)=tempPk_1.block(0,0,const_parm+n,const_parm+n);
                tempPk_2(const_parm+n,const_parm+n) =  sat_ionP;
                tempPk_2.block(const_parm+n+1,const_parm+n+1,Pkcols-const_parm-n,Pkcols-const_parm-n)=tempPk_1.block(const_parm+n,const_parm+n,Pkcols-const_parm-n,Pkcols-const_parm-n);
                tempPk_2.block(0,const_parm+n+1,const_parm+n,Pkcols-const_parm-n)=tempPk_1.block(0,const_parm+n,const_parm+n,Pkcols-const_parm-n);
                tempPk_2.block(const_parm+n+1,0,Pkcols-const_parm-n,const_parm+n)=tempPk_1.block(const_parm+n,0,Pkcols-const_parm-n,const_parm+n);
                tempPk_1=tempPk_2;
                Pkcols = tempPk_1.cols();
                tempPk_2.resize(Pkcols+Lenfre,Pkcols+Lenfre);
                tempPk_2.setZero(Pkcols+Lenfre,Pkcols+Lenfre);
                tempPk_2.block(0,0,Pkcols,Pkcols) = tempPk_1.block(0,0,Pkcols,Pkcols);

                MatrixXd tempQk_2;
                int Qk1cols = tempQk_1.cols();
                tempQk_2.resize(Qk1cols+1,Qk1cols+1);
                tempQk_2.setZero(Qk1cols+1,Qk1cols+1);
                tempQk_2.block(0,0,const_parm+n,const_parm+n)=tempQk_1.block(0,0,const_parm+n,const_parm+n);
                tempQk_2(const_parm+n,const_parm+n) =  sat_ionQ;
                tempQk_2.block(const_parm+n+1,const_parm+n+1,Qk1cols-const_parm-n,Qk1cols-const_parm-n)=tempQk_1.block(const_parm+n,const_parm+n,Qk1cols-const_parm-n,Qk1cols-const_parm-n);
                tempQk_1=tempQk_2;
                Qk1cols = tempQk_1.cols();
                tempQk_2.resize(Qk1cols+Lenfre,Qk1cols+Lenfre);
                tempQk_2.setZero(Qk1cols+Lenfre,Qk1cols+Lenfre);
                tempQk_2.block(0,0,Qk1cols,Qk1cols) = tempQk_1.block(0,0,Qk1cols,Qk1cols);
                for(int j=0;j<Lenfre;j++)
                {
                    tempPk_2(Pkcols+j,Pkcols+j) =  sat_ambP;
                    tempQk_2(Qk1cols+j,Qk1cols+j) =  sat_ambQ;
                }
                int Pk2cols=tempPk_2.cols();
                tempPk_1.resize(Pk2cols,Pk2cols);
                tempPk_1 = tempPk_2;
                int Qk2cols = tempQk_2.cols();
                tempQk_1.resize(Qk2cols,Qk2cols);
                tempQk_1 = tempQk_2;
            }
            else
            {
                VectorXd head_Xn = tempXk_1.head(const_parm+n);
                VectorXd tail_Xn = tempXk_1.tail(tempXk_1.size()-const_parm-n);
                tempXk_1.resize(tempXk_1.size()+1);
                tempXk_1.head(head_Xn.size()) = head_Xn;
                tempXk_1.tail(tail_Xn.size()) = tail_Xn;
                tempXk_1(const_parm+n) = 0;
                head_Xn = tempXk_1.head(freIn+1);
                tail_Xn = tempXk_1.tail(tempXk_1.size()-freIn-1);
                tempXk_1.resize(tempXk_1.size()+Lenfre);
                tempXk_1.head(head_Xn.size()) = head_Xn;
                tempXk_1.tail(tail_Xn.size()) = tail_Xn;
                int frequetN = 0;
                for(int j=0;j<5;j++)
                {
                    if(oneStalit.ObsFreflag[j]==1)
                    {
                        tempXk_1(freIn+1+frequetN) = (oneStalit.CodeP[j] - oneStalit.PhaseL[j]*oneStalit.Lambda[j])/(M_C/oneStalit.Frq[j]);
                        frequetN++;
                    }
                }
                MatrixXd tempPk_2,transPCentre;
                int Pkcols=tempPk_1.cols();
                tempPk_2.resize(Pkcols+1,Pkcols+1);
                tempPk_2.setZero(Pkcols+1,Pkcols+1);
                tempPk_2.block(0,0,const_parm+n,const_parm+n)=tempPk_1.block(0,0,const_parm+n,const_parm+n);
                tempPk_2(const_parm+n,const_parm+n) =  sat_ionP;
                tempPk_2.block(const_parm+n+1,const_parm+n+1,Pkcols-const_parm-n,Pkcols-const_parm-n)=tempPk_1.block(const_parm+n,const_parm+n,Pkcols-const_parm-n,Pkcols-const_parm-n);
                tempPk_2.block(0,const_parm+n+1,const_parm+n,Pkcols-const_parm-n)=tempPk_1.block(0,const_parm+n,const_parm+n,Pkcols-const_parm-n);
                tempPk_2.block(const_parm+n+1,0,Pkcols-const_parm-n,const_parm+n)=tempPk_1.block(const_parm+n,0,Pkcols-const_parm-n,const_parm+n);
                tempPk_1=tempPk_2;

                Pkcols=tempPk_1.cols();
                tempPk_2.resize(Pkcols+Lenfre,Pkcols+Lenfre);
                tempPk_2.setZero(Pkcols+Lenfre,Pkcols+Lenfre);
                transPCentre.resize(Lenfre,Lenfre);
                transPCentre.setZero(Lenfre,Lenfre);
                MatrixXd tempQk_2,transQCentre;
                int Qk1cols = tempQk_1.cols();
                tempQk_2.resize(Qk1cols+1,Qk1cols+1);
                tempQk_2.setZero(Qk1cols+1,Qk1cols+1);
                tempQk_2.block(0,0,const_parm+n,const_parm+n)=tempQk_1.block(0,0,const_parm+n,const_parm+n);
                tempQk_2(const_parm+n,const_parm+n) =  sat_ionQ;
                tempQk_2.block(const_parm+n+1,const_parm+n+1,Qk1cols-const_parm-n,Qk1cols-const_parm-n)=tempQk_1.block(const_parm+n,const_parm+n,Qk1cols-const_parm-n,Qk1cols-const_parm-n);
                tempQk_1=tempQk_2;
                Qk1cols = tempQk_1.cols();
                tempQk_2.resize(Qk1cols+Lenfre,Qk1cols+Lenfre);
                tempQk_2.setZero(Qk1cols+Lenfre,Qk1cols+Lenfre);
                transQCentre.resize(Lenfre,Lenfre);
                transQCentre.setZero(Lenfre,Lenfre);
                for(int j=0;j<Lenfre;j++)
                {
                    transPCentre(j,j) =  sat_ambP;
                    transQCentre(j,j) =  sat_ambQ;
                }
                tempPk_2.block(0,0,freIn+1,freIn+1) = tempPk_1.block(0,0,freIn+1,freIn+1);
                tempPk_2.block(freIn+1+Lenfre,freIn+1+Lenfre,Pkcols-freIn-1,Pkcols-freIn-1) = tempPk_1.block(freIn+1,freIn+1,Pkcols-freIn-1,Pkcols-freIn-1);
                tempPk_2.block(0,(freIn+1)+Lenfre,(freIn+1),Pkcols-(freIn+1)) = tempPk_1.block(0,(freIn+1),(freIn+1),Pkcols-(freIn+1));
                tempPk_2.block((freIn+1)+Lenfre,0,Pkcols-(freIn+1),(freIn+1)) = tempPk_1.block((freIn+1),0,Pkcols-(freIn+1),(freIn+1));
                tempPk_2.block((freIn+1),(freIn+1),Lenfre,Lenfre) = transPCentre;
                int Pk2cols=tempPk_2.cols();
                tempPk_1.resize(Pk2cols,Pk2cols);
                tempPk_1 = tempPk_2;
                tempQk_2.block(0,0,(freIn+1),(freIn+1)) = tempQk_1.block(0,0,(freIn+1),(freIn+1));
                tempQk_2.block((freIn+1)+Lenfre,(freIn+1)+Lenfre,Qk1cols-(freIn+1),Qk1cols-(freIn+1)) = tempQk_1.block((freIn+1),(freIn+1),Qk1cols-(freIn+1),Qk1cols-(freIn+1));
                tempQk_2.block((freIn+1),(freIn+1),Lenfre,Lenfre) = transQCentre;
                int Qk2cols = tempQk_2.cols();
                tempQk_1.resize(Qk2cols,Qk2cols);
                tempQk_1 = tempQk_2;
            }
            freIn = freIn+1;

        }
        else if(oneStalit.cycly_slipF == 1)
        {
            int frequetN = 0;
            tempXk_1(const_parm+n) = 0;
            for(int j=0;j<5;j++)
            {
                if(oneStalit.ObsFreflag[j]==1)
                {
                    tempXk_1(freIn+frequetN) = (oneStalit.CodeP[j] - oneStalit.PhaseL[j]*oneStalit.Lambda[j])/(M_C/oneStalit.Frq[j]);
                    frequetN++;
                }
            }
            for(int k=0;k<tempXk_1.size();k++)
            {
                if(k==n+const_parm)
                    tempPk_1(k,k) =  sat_ionP;
                else
                {
                    tempPk_1(k,n+const_parm) = 0;
                    tempPk_1(n+const_parm,k) = 0;
                }
                for(int j=0;j<Lenfre;j++)
                {
                    if(k == freIn+j)
                        tempPk_1(freIn+j,freIn+j) =  sat_ambP;
                    else
                    {
                        tempPk_1(freIn+j,k) = 0;
                        tempPk_1(k,freIn+j) = 0;
                    }
                }
            }
        }
        freIn = freIn + Lenfre;

    }
    int Pkcols=tempPk_1.cols();
    if(Covar_P.cols()!=Pkcols)
        return;
    Covar_P = tempPk_1;
    State_X = tempXk_1;
    Nois_Q = tempQk_1;
    trans_para->State_X = State_X;
    trans_para->Covar_P=Covar_P;
    trans_para->Nois_Q=Nois_Q;
    trans_para->state_Fi=state_Fi;
    trans_para->m_VarChang = true;//No longer initialized after
}

//Change the parameter size (only PPP can change paramater)
void coefficient_matrix::change_mixed_fre_Combination(QVector< SatlitData > &preEpoch, QVector< SatlitData > &current_Sat,
                                              QVector< int >oldPrnFlag,QVector< int > &NewPrnFlag,int Total_sat_fre,const corr_opt *opt_corr,para_trans *trans_para)
{
    int sys_num = trans_para->sys_num;
    int const_parm = 4+sys_num+trans_para->curr_IFB_n;
    trans_para->const_parm = const_parm;
    double rcv_clkQ = opt_corr->rcv_clkQ;
    double sat_ambQ = opt_corr->sat_ambQ;
    double rcv_clkP = opt_corr->rcv_clkP;
    double sat_ambP = opt_corr->sat_ambP;
    double sat_ionQ = opt_corr->sat_ionQ;
    double sat_ionP = opt_corr->sat_ionP;
    trans_para->m_VarChang = false;
    VectorXd State_X = trans_para->State_X;
    MatrixXd Covar_P = trans_para->Covar_P;
    MatrixXd Nois_Q = trans_para->Nois_Q;
//    VectorXd sys_len = trans_para->sys_len;
//    VectorXd up_sys_len=trans_para->up_sys_len;
//    QString m_sys_str=opt_corr->m_sys_str;
    MatrixXd state_Fi;
    int Avai_satN = current_Sat.length();
//MU_num Number of ambiguity szg  Avai_satN Number of ionosphere unknowns
    state_Fi.resize(const_parm+Total_sat_fre+Avai_satN,const_parm+Total_sat_fre+Avai_satN);
    state_Fi.setIdentity();

    VectorXd tempXk_1 = State_X;
    MatrixXd tempPk_1 = Covar_P;
    MatrixXd tempQk_1 = Nois_Q;
    State_X.resize(const_parm+Total_sat_fre+Avai_satN);
    State_X.setZero(const_parm+Total_sat_fre+Avai_satN);
    Covar_P.resize(const_parm+Total_sat_fre+Avai_satN, const_parm+Total_sat_fre+Avai_satN);
    Nois_Q.resize(const_parm+Total_sat_fre+Avai_satN, const_parm+Total_sat_fre+Avai_satN);

    MatrixXd pre_B;MatrixXd curr_B;
    /*找到需要变化的钟差和电离层*/
    bool newflag;
    newflag = is_clock_ifb_ion_Change(opt_corr,trans_para,oldPrnFlag,NewPrnFlag);
    /*找到需要变化模糊度*/
    newflag = isSatelliteChange_mixed_fre(preEpoch,current_Sat,oldPrnFlag,NewPrnFlag);
    /*确定状态转移矩阵*/
    Calcu_CONV_PreB_CurB(pre_B,curr_B,oldPrnFlag,NewPrnFlag);

    State_X = pre_B * tempXk_1;
    Covar_P = pre_B * tempPk_1 * pre_B.transpose();
    Nois_Q  = pre_B * tempQk_1 * pre_B.transpose();

    tempXk_1 = curr_B * State_X;
    tempPk_1 = curr_B * Covar_P * curr_B.transpose();
    tempQk_1  = curr_B * Nois_Q * curr_B.transpose();

    int N_len = oldPrnFlag.length();
    for(int i=0;i<N_len;i++)
    {

        if(oldPrnFlag[i]==-1)
        {
            if(i<const_parm)
            {
                tempXk_1(i) = tempXk_1(4);
                tempPk_1(i,i) = rcv_clkP;
                tempQk_1(i,i) = rcv_clkQ;
            }
            else if(i<const_parm+Avai_satN)
            {
                SatlitData cur_S = current_Sat.at(i-const_parm);
                tempXk_1(i) = cur_S.SatIon[0];
                tempPk_1(i,i) = sat_ionP;
                tempQk_1(i,i) = sat_ionQ;
            }
            else
            {
                tempXk_1(i) = 0;
                tempPk_1(i,i) = sat_ambP;
                tempQk_1(i,i) = sat_ambQ;
            }
        }
        else if(oldPrnFlag[i]==-2){//-2表示IFB参数重新参数化
            tempXk_1(i) = 0;
            tempPk_1.row(i).setZero();
            tempPk_1.col(i).setZero();
            tempPk_1(i,i) = 1000000;
            tempQk_1.row(i).setZero();
            tempQk_1.col(i).setZero();
            tempQk_1(i,i) = 10;
        }
        else if(oldPrnFlag[i]==-3){//-3表示发生周跳重新参数化
            tempXk_1(i) = 0;
            tempPk_1.row(i).setZero();
            tempPk_1.col(i).setZero();
            tempPk_1(i,i) = sat_ambP;
            tempQk_1.row(i).setZero();
            tempQk_1.col(i).setZero();
            tempQk_1(i,i) = sat_ambQ;
        }
        else if(oldPrnFlag[i]==-4){//-4表示发生周跳ION重新参数化
//            tempXk_1(i) = 0;
            tempPk_1.row(i).setZero();
            tempPk_1.col(i).setZero();
            tempPk_1(i,i) = sat_ionP;
            tempQk_1.row(i).setZero();
            tempQk_1.col(i).setZero();
            tempQk_1(i,i) = sat_ionQ;
        }
    }
    Covar_P = tempPk_1;
    State_X = tempXk_1;
    Nois_Q = tempQk_1;
    trans_para->State_X = State_X;
    trans_para->Covar_P=Covar_P;
    trans_para->Nois_Q=Nois_Q;
    trans_para->state_Fi=state_Fi;
    trans_para->m_VarChang = true;//No longer initialized after
}

//Change the parameter size (only PPP can change paramater)
void coefficient_matrix::change_No_WL_Combination_Para(QVector< SatlitData > &preEpoch, QVector< SatlitData > &current_Sat,
                                              QVector< int >oldPrnFlag,int preEpochLen,QVector< int > &NewPrnFlag,int Total_sat_fre,const corr_opt *opt_corr,para_trans *trans_para)
{
    int sys_num = trans_para->sys_num;

    int const_parm = 4+sys_num+trans_para->curr_IFB_n;
    trans_para->const_parm = const_parm;
    double rcv_clkQ = opt_corr->rcv_clkQ;
    double sat_ambQ = opt_corr->sat_ambQ;
    double rcv_clkP = opt_corr->rcv_clkP;
    double sat_ambP = opt_corr->sat_ambP;
    double sat_ionQ = opt_corr->sat_ionQ;
    double sat_ionP = opt_corr->sat_ionP;
    trans_para->m_VarChang = false;
    VectorXd State_X = trans_para->State_X;
    MatrixXd Covar_P = trans_para->Covar_P;
    MatrixXd Nois_Q = trans_para->Nois_Q;
    VectorXd sys_len = trans_para->sys_len;
    VectorXd up_sys_len=trans_para->up_sys_len;
    QString m_sys_str=opt_corr->m_sys_str;
    MatrixXd state_Fi;
    int Avai_satN = current_Sat.length();
//MU_num Number of ambiguity szg  Avai_satN Number of ionosphere unknowns
    state_Fi.resize(const_parm+Total_sat_fre+Avai_satN,const_parm+Total_sat_fre+Avai_satN);
    state_Fi.setIdentity();
    QString wirte_debug;
    wirte_debug="change_No_WL_Combination_Para 0000";
    write_qdebug.writeQdebug2Txt(wirte_debug);
    VectorXd tempXk_1 = State_X;
    MatrixXd tempPk_1 = Covar_P;
    MatrixXd tempQk_1 = Nois_Q;
    State_X.resize(const_parm+Total_sat_fre+Avai_satN);
    State_X.setZero(const_parm+Total_sat_fre+Avai_satN);
    Covar_P.resize(const_parm+Total_sat_fre+Avai_satN, const_parm+Total_sat_fre+Avai_satN);
    Nois_Q.resize(const_parm+Total_sat_fre+Avai_satN, const_parm+Total_sat_fre+Avai_satN);
    wirte_debug.append(QString::number(const_parm+Total_sat_fre+Avai_satN));
    write_qdebug.writeQdebug2Txt(wirte_debug);


    int bds_flag=0,IFB_n=0,clk_n =0;
    for(int j=0;j<m_sys_str.length();j++)
    {
        if(m_sys_str[j]=='C')
        {
            if(sys_len[j+bds_flag]==up_sys_len[j+bds_flag]&&up_sys_len[j+bds_flag]==1)
            {
                clk_n++;
                if(trans_para->curr_C2IFB_falg&&trans_para->pre_C2IFB_falg)
                    IFB_n++;
                else if(trans_para->curr_C2IFB_falg&&!trans_para->pre_C2IFB_falg)
                {
                    int head_len = 4+clk_n+IFB_n;
                    inctrase_len(tempXk_1,tempPk_1,tempQk_1,head_len);
                    IFB_n++;
                }
                else if(!trans_para->curr_C2IFB_falg&&trans_para->pre_C2IFB_falg)
                {
                    int head_len = 4+clk_n+IFB_n;
                    reduce_len(tempXk_1,tempPk_1,tempQk_1,head_len);
                }
            }
            else if(sys_len[j+bds_flag]==1)
            {
                int head_len = 4+clk_n+IFB_n;
                inctrase_rclk_len(tempXk_1,tempPk_1,tempQk_1,head_len,opt_corr->sys_error_select);
                clk_n++;
                if(trans_para->curr_C2IFB_falg)
                {
                    head_len = 4+clk_n+IFB_n;
                    inctrase_len(tempXk_1,tempPk_1,tempQk_1,head_len);
                    IFB_n++;
                }
            }
            else if(up_sys_len[j+bds_flag]==1)
            {
                int head_len = 4+clk_n+IFB_n;
                reduce_rclk_len(tempXk_1,tempPk_1,tempQk_1,head_len,opt_corr->sys_error_select);
                if(trans_para->pre_C2IFB_falg)
                {
                    head_len = 4+clk_n+IFB_n;
                    reduce_len(tempXk_1,tempPk_1,tempQk_1,head_len);
                }
            }
            bds_flag++;
            if(sys_len[j+bds_flag]==up_sys_len[j+bds_flag]&&up_sys_len[j+bds_flag]==1)
            {
                clk_n++;
                if(trans_para->curr_C3IFB3_falg&&trans_para->pre_C3IFB3_falg)
                    IFB_n++;
                else if(trans_para->curr_C3IFB3_falg&&!trans_para->pre_C3IFB3_falg)
                {
                    int head_len = 4+clk_n+IFB_n;
                    inctrase_len(tempXk_1,tempPk_1,tempQk_1,head_len);
                    IFB_n++;
                }
                else if(!trans_para->curr_C3IFB3_falg&&trans_para->pre_C3IFB3_falg)
                {
                    int head_len = 4+clk_n+IFB_n;
                    reduce_len(tempXk_1,tempPk_1,tempQk_1,head_len);
                }
                if(trans_para->curr_C3IFB4_falg&&trans_para->pre_C3IFB4_falg)
                    IFB_n++;
                else if(trans_para->curr_C3IFB4_falg&&!trans_para->pre_C3IFB4_falg)
                {
                    int head_len = 4+clk_n+IFB_n;
                    inctrase_len(tempXk_1,tempPk_1,tempQk_1,head_len);
                    IFB_n++;
                }
                else if(!trans_para->curr_C3IFB4_falg&&trans_para->pre_C3IFB4_falg)
                {
                    int head_len = 4+clk_n+IFB_n;
                    reduce_len(tempXk_1,tempPk_1,tempQk_1,head_len);
                }
                if(trans_para->curr_C3IFB5_falg&&trans_para->pre_C3IFB5_falg)
                    IFB_n++;
                else if(trans_para->curr_C3IFB5_falg&&!trans_para->pre_C3IFB5_falg)
                {
                    int head_len = 4+clk_n+IFB_n;
                    inctrase_len(tempXk_1,tempPk_1,tempQk_1,head_len);
                    IFB_n++;
                }
                else if(!trans_para->curr_C3IFB5_falg&&trans_para->pre_C3IFB5_falg)
                {
                    int head_len = 4+clk_n+IFB_n;
                    reduce_len(tempXk_1,tempPk_1,tempQk_1,head_len);
                }

            }
            else if(sys_len[j+bds_flag]==1)
            {
                int head_len = 4+clk_n+IFB_n;
                inctrase_rclk_len(tempXk_1,tempPk_1,tempQk_1,head_len,opt_corr->sys_error_select);
                clk_n++;
                if(trans_para->curr_C3IFB3_falg)
                {
                    head_len = 4+clk_n+IFB_n;
                    inctrase_len(tempXk_1,tempPk_1,tempQk_1,head_len);
                    IFB_n++;
                }
                if(trans_para->curr_C3IFB4_falg)
                {
                    head_len = 4+clk_n+IFB_n;
                    inctrase_len(tempXk_1,tempPk_1,tempQk_1,head_len);
                    IFB_n++;
                }
                if(trans_para->curr_C3IFB5_falg)
                {
                    head_len = 4+clk_n+IFB_n;
                    inctrase_len(tempXk_1,tempPk_1,tempQk_1,head_len);
                    IFB_n++;
                }
            }
            else if(up_sys_len[j+bds_flag]==1)
            {
                int head_len = 4+clk_n+IFB_n;
                reduce_rclk_len(tempXk_1,tempPk_1,tempQk_1,head_len,opt_corr->sys_error_select);
                if(trans_para->pre_C3IFB3_falg)
                {
                    head_len = 4+clk_n+IFB_n;
                    reduce_len(tempXk_1,tempPk_1,tempQk_1,head_len);
                }
                if(trans_para->pre_C3IFB4_falg)
                {
                    head_len = 4+clk_n+IFB_n;
                    reduce_len(tempXk_1,tempPk_1,tempQk_1,head_len);
                }
                if(trans_para->pre_C3IFB5_falg)
                {
                    head_len = 4+clk_n+IFB_n;
                    reduce_len(tempXk_1,tempPk_1,tempQk_1,head_len);
                }
            }
        }
        else if(m_sys_str[j]=='G')
        {
            if(sys_len[j+bds_flag]==up_sys_len[j+bds_flag]&&up_sys_len[j+bds_flag]==1)
            {
                clk_n++;
                if(trans_para->curr_GIFB_falg&&trans_para->pre_GIFB_falg)
                    IFB_n++;
                else if(trans_para->curr_GIFB_falg&&!trans_para->pre_GIFB_falg)
                {
                    int head_len = 4+clk_n+IFB_n;
                    inctrase_len(tempXk_1,tempPk_1,tempQk_1,head_len);
                    IFB_n++;
                }
                else if(!trans_para->curr_GIFB_falg&&trans_para->pre_GIFB_falg)
                {
                    int head_len = 4+clk_n+IFB_n;
                    reduce_len(tempXk_1,tempPk_1,tempQk_1,head_len);
                }
            }
            else if(sys_len[j+bds_flag]==1)
            {
                int head_len = 4+clk_n+IFB_n;
                inctrase_rclk_len(tempXk_1,tempPk_1,tempQk_1,head_len,opt_corr->sys_error_select);
                clk_n++;
                if(trans_para->curr_GIFB_falg)
                {
                    head_len = 4+clk_n+IFB_n;
                    inctrase_len(tempXk_1,tempPk_1,tempQk_1,head_len);
                    IFB_n++;
                }
            }
            else if(up_sys_len[j+bds_flag]==1)
            {
                int head_len = 4+clk_n+IFB_n;
                reduce_rclk_len(tempXk_1,tempPk_1,tempQk_1,head_len,opt_corr->sys_error_select);
                if(trans_para->pre_GIFB_falg)
                {
                    head_len = 4+clk_n+IFB_n;
                    reduce_len(tempXk_1,tempPk_1,tempQk_1,head_len);
                }
            }
        }
        else if(m_sys_str[j]=='E')
        {

            if(sys_len[j+bds_flag]==up_sys_len[j+bds_flag]&&up_sys_len[j+bds_flag]==1)
            {
                clk_n++;
                if(trans_para->curr_EIFB3_falg&&trans_para->pre_EIFB3_falg)
                    IFB_n++;
                else if(trans_para->curr_EIFB3_falg&&!trans_para->pre_EIFB3_falg)
                {
                    int head_len = 4+clk_n+IFB_n;
                    inctrase_len(tempXk_1,tempPk_1,tempQk_1,head_len);
                    IFB_n++;
                }
                else if(!trans_para->curr_EIFB3_falg&&trans_para->pre_EIFB3_falg)
                {
                    int head_len = 4+clk_n+IFB_n;
                    reduce_len(tempXk_1,tempPk_1,tempQk_1,head_len);
                }
                if(trans_para->curr_EIFB4_falg&&trans_para->pre_EIFB4_falg)
                    IFB_n++;
                else if(trans_para->curr_EIFB4_falg&&!trans_para->pre_EIFB4_falg)
                {
                    int head_len = 4+clk_n+IFB_n;
                    inctrase_len(tempXk_1,tempPk_1,tempQk_1,head_len);
                    IFB_n++;
                }
                else if(!trans_para->curr_EIFB4_falg&&trans_para->pre_EIFB4_falg)
                {
                    int head_len = 4+clk_n+IFB_n;
                    reduce_len(tempXk_1,tempPk_1,tempQk_1,head_len);
                }
                if(trans_para->curr_EIFB5_falg&&trans_para->pre_EIFB5_falg)
                    IFB_n++;
                else if(trans_para->curr_EIFB5_falg&&!trans_para->pre_EIFB5_falg)
                {
                    int head_len = 4+clk_n+IFB_n;
                    inctrase_len(tempXk_1,tempPk_1,tempQk_1,head_len);
                    IFB_n++;
                }
                else if(!trans_para->curr_EIFB5_falg&&trans_para->pre_EIFB5_falg)
                {
                    int head_len = 4+clk_n+IFB_n;
                    reduce_len(tempXk_1,tempPk_1,tempQk_1,head_len);
                }

            }
            else if(sys_len[j+bds_flag]==1)
            {
                int head_len = 4+clk_n+IFB_n;
                inctrase_rclk_len(tempXk_1,tempPk_1,tempQk_1,head_len,opt_corr->sys_error_select);
                clk_n++;
                if(trans_para->curr_EIFB3_falg)
                {
                    head_len = 4+clk_n+IFB_n;
                    inctrase_len(tempXk_1,tempPk_1,tempQk_1,head_len);
                    IFB_n++;
                }
                if(trans_para->curr_EIFB4_falg)
                {
                    head_len = 4+clk_n+IFB_n;
                    inctrase_len(tempXk_1,tempPk_1,tempQk_1,head_len);
                    IFB_n++;
                }
                if(trans_para->curr_EIFB5_falg)
                {
                    head_len = 4+clk_n+IFB_n;
                    inctrase_len(tempXk_1,tempPk_1,tempQk_1,head_len);
                    IFB_n++;
                }
            }
            else if(up_sys_len[j+bds_flag]==1)
            {
                int head_len = 4+clk_n+IFB_n;
                reduce_rclk_len(tempXk_1,tempPk_1,tempQk_1,head_len,opt_corr->sys_error_select);
                if(trans_para->pre_EIFB3_falg)
                {
                    head_len = 4+clk_n+IFB_n;
                    reduce_len(tempXk_1,tempPk_1,tempQk_1,head_len);
                }
                if(trans_para->pre_EIFB4_falg)
                {
                    head_len = 4+clk_n+IFB_n;
                    reduce_len(tempXk_1,tempPk_1,tempQk_1,head_len);
                }
                if(trans_para->pre_EIFB5_falg)
                {
                    head_len = 4+clk_n+IFB_n;
                    reduce_len(tempXk_1,tempPk_1,tempQk_1,head_len);
                }
            }
        }
        else if(m_sys_str[j]=='R')
        {
            if(sys_len[j+bds_flag]==up_sys_len[j+bds_flag]&&up_sys_len[j+bds_flag]==1)
                clk_n++;
            else if(sys_len[j+bds_flag]==1)
            {
                int head_len = 4+clk_n+IFB_n;
                inctrase_rclk_len(tempXk_1,tempPk_1,tempQk_1,head_len,opt_corr->sys_error_select);
                clk_n++;
            }
            else if(up_sys_len[j+bds_flag]==1)
            {
                int head_len = 4+clk_n+IFB_n;
                reduce_rclk_len(tempXk_1,tempPk_1,tempQk_1,head_len,opt_corr->sys_error_select);
            }
        }
    }
    wirte_debug="change_No_WL_Combination_Para 11111";
    write_qdebug.writeQdebug2Txt(wirte_debug);
    int lenVR = const_parm+preEpochLen;int delete_Num=0;
    for(int i = 0;i<preEpochLen;i++)
    {
        SatlitData preStalit = preEpoch.at(i);
        int prefre = preStalit.ObsLenfre;
        if(NewPrnFlag.at(i)!=-1)
        {
            lenVR=prefre+lenVR;
        }
        else
        {
            if(tempXk_1.size()-lenVR-prefre-1==0)
            {
                VectorXd head_Xn = tempXk_1.head(lenVR);
                tempXk_1.resize(head_Xn.size());
                tempXk_1=head_Xn;
                head_Xn = tempXk_1.head(const_parm+i-delete_Num);
                VectorXd tail_Xn = tempXk_1.tail(tempXk_1.size()-const_parm-i+delete_Num-1);
                tempXk_1.resize(head_Xn.size()+tail_Xn.size());
                tempXk_1<<head_Xn,tail_Xn;
            }
            else
            {
                VectorXd head_Xn = tempXk_1.head(lenVR);
                VectorXd tail_Xn = tempXk_1.tail(tempXk_1.size()-lenVR-prefre);
                tempXk_1.resize(head_Xn.size()+tail_Xn.size());//szg
                tempXk_1<<head_Xn,tail_Xn;
                head_Xn = tempXk_1.head(const_parm+i-delete_Num);
                tail_Xn = tempXk_1.tail(tempXk_1.size()-const_parm-i+delete_Num-1);
                tempXk_1.resize(head_Xn.size()+tail_Xn.size());
                tempXk_1<<head_Xn,tail_Xn;
            }

            for(int rem=0;rem<prefre;rem++)
            {
                m_mat_cal.RemoveRow(tempQk_1,lenVR);
                m_mat_cal.RemoveColumn(tempQk_1,lenVR);
                m_mat_cal.RemoveRow(tempPk_1,lenVR);
                m_mat_cal.RemoveColumn(tempPk_1,lenVR);
            }
            m_mat_cal.RemoveRow(tempPk_1,const_parm+i-delete_Num);
            m_mat_cal.RemoveColumn(tempPk_1,const_parm+i-delete_Num);
            m_mat_cal.RemoveRow(tempQk_1,const_parm+i-delete_Num);
            m_mat_cal.RemoveColumn(tempQk_1,const_parm+i-delete_Num);
            lenVR = lenVR-1;
            delete_Num++;
        }
    }
    wirte_debug.append(QString::number(const_parm+Total_sat_fre+Avai_satN));
    write_qdebug.writeQdebug2Txt(wirte_debug);
    wirte_debug="change_No_WL_Combination_Para 22222";
    write_qdebug.writeQdebug2Txt(wirte_debug);
    int freIn = const_parm+preEpochLen-delete_Num;
    for (int n = 0; n < Avai_satN;n++)
    {
        SatlitData oneStalit = current_Sat.at(n);
        int Lenfre = oneStalit.ObsLenfre;
        if (oldPrnFlag.at(n) == -1)
        {
            if(tempXk_1.size()-freIn==0)
            {
                VectorXd head_Xn = tempXk_1.head(const_parm+n);
                VectorXd tail_Xn = tempXk_1.tail(tempXk_1.size()-const_parm-n);
                tempXk_1.resize(tempXk_1.size()+1);
                tempXk_1.head(head_Xn.size()) = head_Xn;
                tempXk_1.tail(tail_Xn.size()) = tail_Xn;
                tempXk_1(const_parm+n) = 0;
                head_Xn = tempXk_1;
                tempXk_1.resize(tempXk_1.size()+Lenfre);
                tempXk_1.head(head_Xn.size()) = head_Xn;
                int frequetN = 0;
                for(int j=0;j<5;j++)
                {
                    if(oneStalit.ObsFreflag[j]==1)
                    {
                        if(opt_corr->Ambiguity_acquisition==poor_ambiguity)
                            tempXk_1(freIn+1+frequetN) = (oneStalit.CodeP[j] - oneStalit.PhaseL[j]*oneStalit.Lambda[j])/(M_C/oneStalit.Frq[j]);
                        else if(opt_corr->Ambiguity_acquisition==HMW_combinetion||opt_corr->Ambiguity_acquisition==filter_estimate||opt_corr->Ambiguity_acquisition==Do_not_consider_ionospheric)
                        {
                            if(j==0)
                                tempXk_1(freIn+1+frequetN) = (oneStalit.CodeP[j] - oneStalit.PhaseL[j]*oneStalit.Lambda[j])/(M_C/oneStalit.Frq[j]);
                            if(j==1)
                                tempXk_1(freIn+1+frequetN) = (oneStalit.CodeP[0] - oneStalit.PhaseL[0]*oneStalit.Lambda[0])/(M_C/oneStalit.Frq[0])
                                        -(oneStalit.CodeP[j] - oneStalit.PhaseL[j]*oneStalit.Lambda[j])/(M_C/oneStalit.Frq[j]);
                            if(j==2||j==3||j==4)
                                tempXk_1(freIn+1+frequetN) = (oneStalit.CodeP[1] - oneStalit.PhaseL[1]*oneStalit.Lambda[1])/(M_C/oneStalit.Frq[1])
                                        -(oneStalit.CodeP[j] - oneStalit.PhaseL[j]*oneStalit.Lambda[j])/(M_C/oneStalit.Frq[j]);
                        }

                        frequetN++;
                    }
                }
                wirte_debug="change_No_WL_Combination_Para 222221112";
                write_qdebug.writeQdebug2Txt(wirte_debug);
                MatrixXd tempPk_2;
                int Pkcols=tempPk_1.cols();
                tempPk_2.resize(Pkcols+1,Pkcols+1);
                tempPk_2.setZero(Pkcols+1,Pkcols+1);
                tempPk_2.block(0,0,const_parm+n,const_parm+n)=tempPk_1.block(0,0,const_parm+n,const_parm+n);
                tempPk_2(const_parm+n,const_parm+n) =  sat_ionP;
                tempPk_2.block(const_parm+n+1,const_parm+n+1,Pkcols-const_parm-n,Pkcols-const_parm-n)=tempPk_1.block(const_parm+n,const_parm+n,Pkcols-const_parm-n,Pkcols-const_parm-n);
                tempPk_2.block(0,const_parm+n+1,const_parm+n,Pkcols-const_parm-n)=tempPk_1.block(0,const_parm+n,const_parm+n,Pkcols-const_parm-n);
                tempPk_2.block(const_parm+n+1,0,Pkcols-const_parm-n,const_parm+n)=tempPk_1.block(const_parm+n,0,Pkcols-const_parm-n,const_parm+n);
                tempPk_1=tempPk_2;
                Pkcols = tempPk_1.cols();
                tempPk_2.resize(Pkcols+Lenfre,Pkcols+Lenfre);
                tempPk_2.setZero(Pkcols+Lenfre,Pkcols+Lenfre);
                tempPk_2.block(0,0,Pkcols,Pkcols) = tempPk_1.block(0,0,Pkcols,Pkcols);
                wirte_debug="change_No_WL_Combination_Para 222220000";
                write_qdebug.writeQdebug2Txt(wirte_debug);
                MatrixXd tempQk_2;
                int Qk1cols = tempQk_1.cols();
                tempQk_2.resize(Qk1cols+1,Qk1cols+1);
                tempQk_2.setZero(Qk1cols+1,Qk1cols+1);
                tempQk_2.block(0,0,const_parm+n,const_parm+n)=tempQk_1.block(0,0,const_parm+n,const_parm+n);
                tempQk_2(const_parm+n,const_parm+n) =  sat_ionQ;
                tempQk_2.block(const_parm+n+1,const_parm+n+1,Qk1cols-const_parm-n,Qk1cols-const_parm-n)=tempQk_1.block(const_parm+n,const_parm+n,Qk1cols-const_parm-n,Qk1cols-const_parm-n);
                tempQk_1=tempQk_2;
                Qk1cols = tempQk_1.cols();
                tempQk_2.resize(Qk1cols+Lenfre,Qk1cols+Lenfre);
                tempQk_2.setZero(Qk1cols+Lenfre,Qk1cols+Lenfre);
                tempQk_2.block(0,0,Qk1cols,Qk1cols) = tempQk_1.block(0,0,Qk1cols,Qk1cols);
                for(int j=0;j<Lenfre;j++)
                {
                    tempPk_2(Pkcols+j,Pkcols+j) =  sat_ambP;
                    tempQk_2(Qk1cols+j,Qk1cols+j) =  sat_ambQ;
                }
                int Pk2cols=tempPk_2.cols();
                tempPk_1.resize(Pk2cols,Pk2cols);
                tempPk_1 = tempPk_2;
                int Qk2cols = tempQk_2.cols();
                tempQk_1.resize(Qk2cols,Qk2cols);
                tempQk_1 = tempQk_2;
                wirte_debug="change_No_WL_Combination_Para 2222211";
                write_qdebug.writeQdebug2Txt(wirte_debug);
            }
            else
            {
                VectorXd head_Xn = tempXk_1.head(const_parm+n);
                VectorXd tail_Xn = tempXk_1.tail(tempXk_1.size()-const_parm-n);
                tempXk_1.resize(tempXk_1.size()+1);
                tempXk_1.head(head_Xn.size()) = head_Xn;
                tempXk_1.tail(tail_Xn.size()) = tail_Xn;
                tempXk_1(const_parm+n) = 0;
                head_Xn = tempXk_1.head(freIn+1);
                tail_Xn = tempXk_1.tail(tempXk_1.size()-freIn-1);
                tempXk_1.resize(tempXk_1.size()+Lenfre);
                tempXk_1.head(head_Xn.size()) = head_Xn;
                tempXk_1.tail(tail_Xn.size()) = tail_Xn;
                int frequetN = 0;
                for(int j=0;j<5;j++)
                {
                    if(oneStalit.ObsFreflag[j]==1)
                    {
                        if(opt_corr->Ambiguity_acquisition==poor_ambiguity)
                            tempXk_1(freIn+1+frequetN) = (oneStalit.CodeP[j] - oneStalit.PhaseL[j]*oneStalit.Lambda[j])/(M_C/oneStalit.Frq[j]);
                        else if(opt_corr->Ambiguity_acquisition==HMW_combinetion||opt_corr->Ambiguity_acquisition==filter_estimate||opt_corr->Ambiguity_acquisition==Do_not_consider_ionospheric)
                        {
                            if(j==0)
                                tempXk_1(freIn+1+frequetN) = (oneStalit.CodeP[j] - oneStalit.PhaseL[j]*oneStalit.Lambda[j])/(M_C/oneStalit.Frq[j]);
                            if(j==1)
                                tempXk_1(freIn+1+frequetN) = (oneStalit.CodeP[0] - oneStalit.PhaseL[0]*oneStalit.Lambda[0])/(M_C/oneStalit.Frq[0])
                                        -(oneStalit.CodeP[j] - oneStalit.PhaseL[j]*oneStalit.Lambda[j])/(M_C/oneStalit.Frq[j]);
                            if(j==2||j==3||j==4)
                                tempXk_1(freIn+1+frequetN) = (oneStalit.CodeP[1] - oneStalit.PhaseL[1]*oneStalit.Lambda[1])/(M_C/oneStalit.Frq[1])
                                        -(oneStalit.CodeP[j] - oneStalit.PhaseL[j]*oneStalit.Lambda[j])/(M_C/oneStalit.Frq[j]);
                        }
                        frequetN++;
                    }
                }
                wirte_debug="change_No_WL_Combination_Para 22222111111";
                write_qdebug.writeQdebug2Txt(wirte_debug);
                MatrixXd tempPk_2,transPCentre;
                int Pkcols=tempPk_1.cols();
                tempPk_2.resize(Pkcols+1,Pkcols+1);
                tempPk_2.setZero(Pkcols+1,Pkcols+1);
                tempPk_2.block(0,0,const_parm+n,const_parm+n)=tempPk_1.block(0,0,const_parm+n,const_parm+n);
                tempPk_2(const_parm+n,const_parm+n) =  sat_ionP;
                tempPk_2.block(const_parm+n+1,const_parm+n+1,Pkcols-const_parm-n,Pkcols-const_parm-n)=tempPk_1.block(const_parm+n,const_parm+n,Pkcols-const_parm-n,Pkcols-const_parm-n);
                tempPk_2.block(0,const_parm+n+1,const_parm+n,Pkcols-const_parm-n)=tempPk_1.block(0,const_parm+n,const_parm+n,Pkcols-const_parm-n);
                tempPk_2.block(const_parm+n+1,0,Pkcols-const_parm-n,const_parm+n)=tempPk_1.block(const_parm+n,0,Pkcols-const_parm-n,const_parm+n);
                tempPk_1=tempPk_2;

                Pkcols=tempPk_1.cols();
                tempPk_2.resize(Pkcols+Lenfre,Pkcols+Lenfre);
                tempPk_2.setZero(Pkcols+Lenfre,Pkcols+Lenfre);
                transPCentre.resize(Lenfre,Lenfre);
                transPCentre.setZero(Lenfre,Lenfre);
                wirte_debug="change_No_WL_Combination_Para 22222111111222";
                write_qdebug.writeQdebug2Txt(wirte_debug);
                MatrixXd tempQk_2,transQCentre;
                int Qk1cols = tempQk_1.cols();
                tempQk_2.resize(Qk1cols+1,Qk1cols+1);
                tempQk_2.setZero(Qk1cols+1,Qk1cols+1);
                tempQk_2.block(0,0,const_parm+n,const_parm+n)=tempQk_1.block(0,0,const_parm+n,const_parm+n);
                tempQk_2(const_parm+n,const_parm+n) =  sat_ionQ;
                tempQk_2.block(const_parm+n+1,const_parm+n+1,Qk1cols-const_parm-n,Qk1cols-const_parm-n)=tempQk_1.block(const_parm+n,const_parm+n,Qk1cols-const_parm-n,Qk1cols-const_parm-n);
                tempQk_1=tempQk_2;
                Qk1cols = tempQk_1.cols();
                tempQk_2.resize(Qk1cols+Lenfre,Qk1cols+Lenfre);
                tempQk_2.setZero(Qk1cols+Lenfre,Qk1cols+Lenfre);
                transQCentre.resize(Lenfre,Lenfre);
                transQCentre.setZero(Lenfre,Lenfre);
                for(int j=0;j<Lenfre;j++)
                {
                    transPCentre(j,j) =  sat_ambP;
                    transQCentre(j,j) =  sat_ambQ;
                }
                wirte_debug="change_No_WL_Combination_Para 2222211123333";
                write_qdebug.writeQdebug2Txt(wirte_debug);
                tempPk_2.block(0,0,freIn+1,freIn+1) = tempPk_1.block(0,0,freIn+1,freIn+1);
                tempPk_2.block(freIn+1+Lenfre,freIn+1+Lenfre,Pkcols-freIn-1,Pkcols-freIn-1) = tempPk_1.block(freIn+1,freIn+1,Pkcols-freIn-1,Pkcols-freIn-1);
                tempPk_2.block(0,(freIn+1)+Lenfre,(freIn+1),Pkcols-(freIn+1)) = tempPk_1.block(0,(freIn+1),(freIn+1),Pkcols-(freIn+1));
                tempPk_2.block((freIn+1)+Lenfre,0,Pkcols-(freIn+1),(freIn+1)) = tempPk_1.block((freIn+1),0,Pkcols-(freIn+1),(freIn+1));
                tempPk_2.block((freIn+1),(freIn+1),Lenfre,Lenfre) = transPCentre;
                int Pk2cols=tempPk_2.cols();
                tempPk_1.resize(Pk2cols,Pk2cols);
                tempPk_1 = tempPk_2;
                tempQk_2.block(0,0,(freIn+1),(freIn+1)) = tempQk_1.block(0,0,(freIn+1),(freIn+1));
                tempQk_2.block((freIn+1)+Lenfre,(freIn+1)+Lenfre,Qk1cols-(freIn+1),Qk1cols-(freIn+1)) = tempQk_1.block((freIn+1),(freIn+1),Qk1cols-(freIn+1),Qk1cols-(freIn+1));
                tempQk_2.block((freIn+1),(freIn+1),Lenfre,Lenfre) = transQCentre;
                int Qk2cols = tempQk_2.cols();
                tempQk_1.resize(Qk2cols,Qk2cols);
                tempQk_1 = tempQk_2;
            }
            freIn = freIn+1;
        }
        else if(oneStalit.cycly_slipF == 1)
        {
            int frequetN = 0;
            tempXk_1(const_parm+n) = 0;
            for(int j=0;j<5;j++)
            {
                wirte_debug="change_No_WL_Combination_Para 222221122344341";
                write_qdebug.writeQdebug2Txt(wirte_debug);
                if(oneStalit.ObsFreflag[j]==1)
                {
                    if(opt_corr->Ambiguity_acquisition==poor_ambiguity)
                        tempXk_1(freIn+frequetN) = (oneStalit.CodeP[j] - oneStalit.PhaseL[j]*oneStalit.Lambda[j])/(M_C/oneStalit.Frq[j]);
                    else if(opt_corr->Ambiguity_acquisition==HMW_combinetion||opt_corr->Ambiguity_acquisition==filter_estimate||opt_corr->Ambiguity_acquisition==Do_not_consider_ionospheric)
                    {
                        if(j==0)
                            tempXk_1(freIn+frequetN) = (oneStalit.CodeP[j] - oneStalit.PhaseL[j]*oneStalit.Lambda[j])/(M_C/oneStalit.Frq[j]);
                        if(j==1)
                            tempXk_1(freIn+frequetN) = (oneStalit.CodeP[0] - oneStalit.PhaseL[0]*oneStalit.Lambda[0])/(M_C/oneStalit.Frq[0])
                                    -(oneStalit.CodeP[j] - oneStalit.PhaseL[j]*oneStalit.Lambda[j])/(M_C/oneStalit.Frq[j]);
                        if(j==2||j==3||j==4)
                            tempXk_1(freIn+frequetN) = (oneStalit.CodeP[1] - oneStalit.PhaseL[1]*oneStalit.Lambda[1])/(M_C/oneStalit.Frq[1])
                                    -(oneStalit.CodeP[j] - oneStalit.PhaseL[j]*oneStalit.Lambda[j])/(M_C/oneStalit.Frq[j]);
                    }
                    frequetN++;
                }
            }
            wirte_debug="change_No_WL_Combination_Para 222221234123123";
            write_qdebug.writeQdebug2Txt(wirte_debug);
            for(int k=0;k<tempXk_1.size();k++)
            {
                if(k==n+const_parm)
                    tempPk_1(k,k) =  sat_ionP;
                else
                {
                    tempPk_1(k,n+const_parm) = 0;
                    tempPk_1(n+const_parm,k) = 0;
                }
                for(int j=0;j<Lenfre;j++)
                {
                    if(k == freIn+j)
                        tempPk_1(freIn+j,freIn+j) =  sat_ambP;
                    else
                    {
                        tempPk_1(freIn+j,k) = 0;
                        tempPk_1(k,freIn+j) = 0;
                    }
                }
            }
            wirte_debug="change_No_WL_Combination_Para 2222212312412";
            write_qdebug.writeQdebug2Txt(wirte_debug);
        }
        freIn = freIn + Lenfre;
    }
    wirte_debug="change_No_WL_Combination_Para 333333";
    write_qdebug.writeQdebug2Txt(wirte_debug);
    int Pkcols=tempPk_1.cols();
    if(Covar_P.cols()!=Pkcols)
        return;
    wirte_debug="change_No_WL_Combination_Para return";
    write_qdebug.writeQdebug2Txt(wirte_debug);
    Covar_P = tempPk_1;
    State_X = tempXk_1;
    Nois_Q = tempQk_1;
    trans_para->State_X = State_X;
    trans_para->Covar_P=Covar_P;
    trans_para->Nois_Q=Nois_Q;
    trans_para->state_Fi=state_Fi;
    trans_para->m_VarChang = true;//No longer initialized after
}
// update Rk(Observation Covariance)
void coefficient_matrix::update_IF_Rk(QVector< SatlitData > &current_Sat, int B_len,const corr_opt *opt_corr,para_trans *trans_para)
{
    int Avai_satN = current_Sat.length();
    MatrixXd Covar_Rk;
    Covar_Rk.resize(B_len, B_len);// this m_Rk_1 is for ISB
    Covar_Rk.setIdentity();// this m_Rk_1 is for ISB
    for (int i = 0;i < Avai_satN;i++)
    {
        SatlitData oneSatlit = current_Sat.at(i);
        /*Covariance of carrier equation Reciprocal (small noise)// 1/25000 =4e-4*/
        Covar_Rk(i*2,i*2) = 1.0/(oneSatlit.SatWight*oneSatlit.SatWight_L*oneSatlit.SatWight_IF[0]);
        Covar_Rk(i*2+1,i*2+1) = 1.0/(oneSatlit.SatWight*oneSatlit.SatWight_IF[1]);
    }
    trans_para->Covar_Rk = Covar_Rk;
}
// update Rk(Observation Covariance)
void coefficient_matrix::update_helmet_IF_Rk(QVector< SatlitData > &current_Sat, int B_len,const corr_opt *opt_corr,para_trans *trans_para)
{
    int Avai_satN = current_Sat.length();
    MatrixXd Covar_Rk;
    Covar_Rk.resize(B_len, B_len);// this m_Rk_1 is for ISB
    Covar_Rk.setIdentity();// this m_Rk_1 is for ISB
    for (int i = 0;i < Avai_satN;i++)
    {
        SatlitData oneSatlit = current_Sat.at(i);
        /*Covariance of carrier equation Reciprocal (small noise)// 1/25000 =4e-4*/
        Covar_Rk(i*2,i*2) = 1.0*oneSatlit.Helmert_Wight_L/(oneSatlit.SatWight*oneSatlit.SatWight_L*oneSatlit.SatWight_IF[0]);
        Covar_Rk(i*2+1,i*2+1) = 1.0*oneSatlit.Helmert_Wight_P/(oneSatlit.SatWight*oneSatlit.SatWight_IF[1]);
    }
    trans_para->Covar_Rk = Covar_Rk;
}
// update Rk(Observation Covariance)
void coefficient_matrix::update_NoCombination_Rk(QVector< SatlitData > &current_Sat, int B_len,const corr_opt *opt_corr,para_trans *trans_para)
{
    int Avai_satN = current_Sat.length();
    MatrixXd Covar_Rk;
    Covar_Rk.resize(B_len, B_len);// this m_Rk_1 is for ISB
    Covar_Rk.setIdentity();// this m_Rk_1 is for ISB
    int column_pos=0;
    for (int i = 0;i < Avai_satN;i++)
    {
        SatlitData oneSatlit = current_Sat.at(i);
        for(int mufre=0;mufre<5;mufre++)
        {
            if(oneSatlit.ObsFreflag[mufre]!=1)  continue;
            /*Covariance of carrier equation Reciprocal (small noise)// 1/25000 =4e-4*/
            Covar_Rk(column_pos*2,column_pos*2) = 1.0/(oneSatlit.SatWight*oneSatlit.SatWight_L*oneSatlit.SatWight_UD[mufre*2]);
            Covar_Rk(column_pos*2+1,column_pos*2+1) = 1.0/(oneSatlit.SatWight*oneSatlit.SatWight_UD[mufre*2+1]);
            column_pos++;
        }
    }
    trans_para->Covar_Rk = Covar_Rk;
}
// update Rk(Observation Covariance)
void coefficient_matrix::update_WLNoCombination_Rk(QVector< SatlitData > &current_Sat, int B_len,para_trans *trans_para)
{
    int Avai_satN = current_Sat.length();
    MatrixXd Covar_Rk;
    Covar_Rk.resize(B_len, B_len);// this m_Rk_1 is for ISB
    Covar_Rk.setIdentity();// this m_Rk_1 is for ISB
    int column_pos=0;
    for (int i = 0;i < Avai_satN;i++)
    {
        SatlitData oneSatlit = current_Sat.at(i);
        for(int mufre=0;mufre<5;mufre++)
        {
            if(oneSatlit.ObsFreflag[mufre]!=1)  continue;

            /*Covariance of carrier equation Reciprocal (small noise)// 1/25000 =4e-4*/
            Covar_Rk(column_pos*2,column_pos*2) = 1.0/(oneSatlit.SatWight*oneSatlit.SatWight_L*oneSatlit.SatWight_UD[mufre*2]);
            Covar_Rk(column_pos*2+1,column_pos*2+1) = 1.0/(oneSatlit.SatWight*oneSatlit.SatWight_UD[mufre*2+1]);
            column_pos++;
        }

    }
    trans_para->Covar_Rk = Covar_Rk;
}
/*计算状态和状态协方差阵的状态转移矩阵MatrixXd &pre_B,MatrixXd &curr_B*/
void coefficient_matrix::Calcu_CONV_PreB_CurB(MatrixXd &pre_B,MatrixXd &curr_B,QVector< int > &oldPrnFlag,QVector< int > &NewPrnFlag)
{
    /*先做删除，再做增加，同时要考虑钟差，电离层，多频的IFB*/
    int pre_B_len = NewPrnFlag.length();
    int cur_B_len = oldPrnFlag.length();

    //Determine whether the number of satellites has changed (comparison of two epochs before and after)
    int sys_pos_coffI[5];   for(int i=0;i<5;i++)    sys_pos_coffI[i] = -1;

    int remove_pre_B[pre_B_len];
    int remove_curr_B[cur_B_len];
    int pre_B_N = 0;int curr_B_N = 0;
    for(int i=0;i<pre_B_len;i++)
    {
        remove_pre_B[i] = 0;
    }
    for(int i=0;i<cur_B_len;i++)
    {
        remove_curr_B[i] = 0;
    }
//    qDebug()<<"4372";
    pre_B.resize(pre_B_len,pre_B_len);pre_B.setIdentity(pre_B_len,pre_B_len);/**/
    for (int i = 0;i < pre_B_len;i++)
    {//Whether the satellite inspections before and after the cycle are completely equal
        if(NewPrnFlag[i]==-1){
            remove_pre_B[pre_B_N] = i;
            pre_B_N++;
        }
    }
    curr_B.resize(cur_B_len,cur_B_len);curr_B.setIdentity(cur_B_len,cur_B_len);/**/
    for (int i = 0;i < cur_B_len;i++)
    {//Whether the satellite inspections before and after the cycle are completely equal
        if(oldPrnFlag[i]==-1||oldPrnFlag[i]==-2){
            remove_curr_B[curr_B_N] = i;
            curr_B_N++;
        }
    }


    sort_order(pre_B_N,remove_pre_B);
    sort_order(curr_B_N,remove_curr_B);
//    qDebug()<<"4534";
    for(int i =0;i<pre_B_N;i++)
    {
        m_mat_cal.RemoveRow(pre_B,remove_pre_B[i]);
    }

    for(int i =0;i<curr_B_N;i++)
    {
        m_mat_cal.RemoveColumn(curr_B,remove_curr_B[i]);
    }
}
/*对位置、钟差、IFB和对流层等*/
bool coefficient_matrix::is_clock_ifb_ion_Change(const corr_opt *opt_corr,para_trans *trans_para, QVector< int > &oldPrnFlag,QVector< int > &NewPrnFlag)
{
    VectorXd sys_len = trans_para->sys_len;
    VectorXd up_sys_len=trans_para->up_sys_len;
    QString m_sys_str=opt_corr->m_sys_str;
    /*XZY和对流层Tr*/
    for(int j=0;j<4;j++)
    {
        oldPrnFlag.append(0);
        NewPrnFlag.append(0);
    }
    int bds_flag=0;
    for(int j=0;j<m_sys_str.length();j++)
    {
        if(m_sys_str[j]=='C')
        {
            if(sys_len[j+bds_flag]==up_sys_len[j+bds_flag]&&up_sys_len[j+bds_flag]==1)
            {
                {
                    oldPrnFlag.append(0);
                    NewPrnFlag.append(0);
                }//对于钟差的操作
                if(trans_para->curr_C2IFB_falg&&trans_para->pre_C2IFB_falg)
                {
                    oldPrnFlag.append(0);
                    NewPrnFlag.append(0);
                }
                else if(trans_para->curr_C2IFB_falg&&!trans_para->pre_C2IFB_falg)
                {
                    oldPrnFlag.append(-2);//增加IFB或CLK参数
                }
                else if(!trans_para->curr_C2IFB_falg&&trans_para->pre_C2IFB_falg)
                {
                    NewPrnFlag.append(-1);//删除当前历元没有的IFB参数
                }
            }
            else if(sys_len[j+bds_flag]==1)
            {
                oldPrnFlag.append(-1);//增加IFB或CLK参数
                if(trans_para->curr_C2IFB_falg)
                {
                    oldPrnFlag.append(-2);//增加IFB或CLK参数
                }
            }
            else if(up_sys_len[j+bds_flag]==1)
            {
                NewPrnFlag.append(-1);//删除当前历元没有的IFB参数
                if(trans_para->pre_C2IFB_falg)
                {
                    NewPrnFlag.append(-1);//删除当前历元没有的IFB参数
                }
            }
            bds_flag++;
            if(sys_len[j+bds_flag]==up_sys_len[j+bds_flag]&&up_sys_len[j+bds_flag]==1)
            {
                {
                    oldPrnFlag.append(0);
                    NewPrnFlag.append(0);
                }//对于钟差的操作
                if(trans_para->curr_C3IFB3_falg&&trans_para->pre_C3IFB3_falg)
                {
                    oldPrnFlag.append(0);
                    NewPrnFlag.append(0);
                }
                else if(trans_para->curr_C3IFB3_falg&&!trans_para->pre_C3IFB3_falg)
                {
                    oldPrnFlag.append(-2);//增加IFB或CLK参数
                }
                else if(!trans_para->curr_C3IFB3_falg&&trans_para->pre_C3IFB3_falg)
                {
                    NewPrnFlag.append(-1);//删除当前历元没有的IFB参数
                }
                if(trans_para->curr_C3IFB4_falg&&trans_para->pre_C3IFB4_falg)
                {
                    oldPrnFlag.append(0);
                    NewPrnFlag.append(0);
                }
                else if(trans_para->curr_C3IFB4_falg&&!trans_para->pre_C3IFB4_falg)
                {
                    oldPrnFlag.append(-2);//增加IFB或CLK参数
                }
                else if(!trans_para->curr_C3IFB4_falg&&trans_para->pre_C3IFB4_falg)
                {
                    NewPrnFlag.append(-1);//删除当前历元没有的IFB参数
                }
                if(trans_para->curr_C3IFB5_falg&&trans_para->pre_C3IFB5_falg)
                {
                    oldPrnFlag.append(0);
                    NewPrnFlag.append(0);
                }
                else if(trans_para->curr_C3IFB5_falg&&!trans_para->pre_C3IFB5_falg)
                {
                    oldPrnFlag.append(-2);//增加IFB或CLK参数
                }
                else if(!trans_para->curr_C3IFB5_falg&&trans_para->pre_C3IFB5_falg)
                {
                    NewPrnFlag.append(-1);//删除当前历元没有的IFB参数
                }

            }
            else if(sys_len[j+bds_flag]==1)
            {
                oldPrnFlag.append(-1);//增加IFB或CLK参数
                if(trans_para->curr_C3IFB3_falg)
                {
                    oldPrnFlag.append(-2);//增加IFB或CLK参数
                }
                if(trans_para->curr_C3IFB4_falg)
                {
                    oldPrnFlag.append(-2);//增加IFB或CLK参数
                }
                if(trans_para->curr_C3IFB5_falg)
                {
                    oldPrnFlag.append(-2);//增加IFB或CLK参数
                }
            }
            else if(up_sys_len[j+bds_flag]==1)
            {
                NewPrnFlag.append(-1);//删除当前历元没有的IFB参数
                if(trans_para->pre_C3IFB3_falg)
                {
                    NewPrnFlag.append(-1);//删除当前历元没有的IFB参数
                }
                if(trans_para->pre_C3IFB4_falg)
                {
                    NewPrnFlag.append(-1);//删除当前历元没有的IFB参数
                }
                if(trans_para->pre_C3IFB5_falg)
                {
                    NewPrnFlag.append(-1);//删除当前历元没有的IFB参数
                }
            }
        }
        else if(m_sys_str[j]=='G')
        {
            if(sys_len[j+bds_flag]==up_sys_len[j+bds_flag]&&up_sys_len[j+bds_flag]==1)
            {
                {
                    oldPrnFlag.append(0);
                    NewPrnFlag.append(0);
                }
                if(trans_para->curr_GIFB_falg&&trans_para->pre_GIFB_falg)
                {
                    oldPrnFlag.append(0);
                    NewPrnFlag.append(0);
                }
                else if(trans_para->curr_GIFB_falg&&!trans_para->pre_GIFB_falg)
                {
                    oldPrnFlag.append(-2);//增加IFB或CLK参数
                }
                else if(!trans_para->curr_GIFB_falg&&trans_para->pre_GIFB_falg)
                {
                    NewPrnFlag.append(-1);//删除当前历元没有的IFB参数
                }
            }
            else if(sys_len[j+bds_flag]==1)
            {
                oldPrnFlag.append(-1);//增加IFB或CLK参数
                if(trans_para->curr_GIFB_falg)
                {
                    oldPrnFlag.append(-2);//增加IFB或CLK参数
                }
            }
            else if(up_sys_len[j+bds_flag]==1)
            {
                NewPrnFlag.append(-1);//删除当前历元没有的IFB参数
                if(trans_para->pre_GIFB_falg)
                {
                    NewPrnFlag.append(-1);//删除当前历元没有的IFB参数
                }
            }
        }
        else if(m_sys_str[j]=='E')
        {

            if(sys_len[j+bds_flag]==up_sys_len[j+bds_flag]&&up_sys_len[j+bds_flag]==1)
            {
                {
                    oldPrnFlag.append(0);
                    NewPrnFlag.append(0);
                }
                if(trans_para->curr_EIFB3_falg&&trans_para->pre_EIFB3_falg)
                {
                    oldPrnFlag.append(0);
                    NewPrnFlag.append(0);
                }
                else if(trans_para->curr_EIFB3_falg&&!trans_para->pre_EIFB3_falg)
                {
                    oldPrnFlag.append(-2);//增加IFB或CLK参数
                }
                else if(!trans_para->curr_EIFB3_falg&&trans_para->pre_EIFB3_falg)
                {
                    NewPrnFlag.append(-1);//删除当前历元没有的IFB参数
                }
                if(trans_para->curr_EIFB4_falg&&trans_para->pre_EIFB4_falg)
                {
                    oldPrnFlag.append(0);
                    NewPrnFlag.append(0);
                }
                else if(trans_para->curr_EIFB4_falg&&!trans_para->pre_EIFB4_falg)
                {
                    oldPrnFlag.append(-2);//增加IFB或CLK参数
                }
                else if(!trans_para->curr_EIFB4_falg&&trans_para->pre_EIFB4_falg)
                {
                    NewPrnFlag.append(-1);//删除当前历元没有的IFB参数
                }
                if(trans_para->curr_EIFB5_falg&&trans_para->pre_EIFB5_falg)
                {
                    oldPrnFlag.append(0);
                    NewPrnFlag.append(0);
                }
                else if(trans_para->curr_EIFB5_falg&&!trans_para->pre_EIFB5_falg)
                {
                    oldPrnFlag.append(-2);//增加IFB或CLK参数
                }
                else if(!trans_para->curr_EIFB5_falg&&trans_para->pre_EIFB5_falg)
                {
                    NewPrnFlag.append(-1);//删除当前历元没有的IFB参数
                }

            }
            else if(sys_len[j+bds_flag]==1)
            {
                oldPrnFlag.append(-1);//增加IFB或CLK参数
                if(trans_para->curr_EIFB3_falg)
                {
                    oldPrnFlag.append(-2);//增加IFB或CLK参数
                }
                if(trans_para->curr_EIFB4_falg)
                {
                    oldPrnFlag.append(-2);//增加IFB或CLK参数
                }
                if(trans_para->curr_EIFB5_falg)
                {
                    oldPrnFlag.append(-2);//增加IFB或CLK参数
                }
            }
            else if(up_sys_len[j+bds_flag]==1)
            {
                NewPrnFlag.append(-1);//删除当前历元没有的IFB参数
                if(trans_para->pre_EIFB3_falg)
                {
                    NewPrnFlag.append(-1);//删除当前历元没有的IFB参数
                }
                if(trans_para->pre_EIFB4_falg)
                {
                    NewPrnFlag.append(-1);//删除当前历元没有的IFB参数
                }
                if(trans_para->pre_EIFB5_falg)
                {
                    NewPrnFlag.append(-1);//删除当前历元没有的IFB参数
                }
            }
        }
        else if(m_sys_str[j]=='R')
        {
            if(sys_len[j+bds_flag]==up_sys_len[j+bds_flag]&&up_sys_len[j+bds_flag]==1)
            {
                oldPrnFlag.append(0);
                NewPrnFlag.append(0);
            }
            else if(sys_len[j+bds_flag]==1)
            {
                oldPrnFlag.append(-1);//增加IFB或CLK参数
            }
            else if(up_sys_len[j+bds_flag]==1)
            {
                NewPrnFlag.append(-1);//删除当前历元没有的IFB参数
            }
        }
    }

}


/*计算相比于上一历元新增卫星和频率的位置形成向量A、相比于上一历元缺失卫星和频率形成向量B*/
bool coefficient_matrix::isSatelliteChange_mixed_fre(QVector< SatlitData > &preEpoch,QVector< SatlitData > &currEpoch, QVector< int > &oldPrnFlag,QVector< int > &NewPrnFlag)
{
    int preEpochLen = preEpoch.length();
    int epochLenLB = currEpoch.length();
    //Determine whether the number of satellites has changed (comparison of two epochs before and after)
    int oldSatLen = 0;
    bool isNewSatlite = false;
    corr_opt *opt_corr = AGNSS_APPData::sGetAPPDataInstance()->mGetopt_corrData();
    if(opt_corr->Combination_mode==Comb_UD_mode)
    {
        for (int i = 0;i < epochLenLB;i++)
        {//Whether the satellite inspections before and after the cycle are completely equal
            SatlitData epochSatlit = currEpoch.at(i);
            bool Isfind = false;//Whether the tag finds the last epoch
            for (int j = 0;j < preEpochLen;j++)
            {
                SatlitData preEpochSatlit = preEpoch.at(j);
                if (epochSatlit.PRN == preEpochSatlit.PRN&&epochSatlit.SatType == preEpochSatlit.SatType)
                {
                    if(epochSatlit.cycly_slipF==1)
                    {
                        oldPrnFlag.append(-4);
                    }
                    else
                        oldPrnFlag.append(j);//Compared with the location of the same satellite in the previous epoch, it is not found with -1
                    Isfind = true;
                    oldSatLen++;
                    break;
                }
            }
            if (!Isfind)
            {
                oldPrnFlag.append(-1);//Compared with the location of the same satellite in the previous epoch, it is not found with -1
                isNewSatlite = true;
            }
        }
        for (int i = 0;i < preEpochLen;i++)
        {//Whether the satellite inspections before and after the cycle are completely equal
            SatlitData preEpochSatlit = preEpoch.at(i);
            bool Isfind = false;//Whether the tag finds the last epoch
            for (int j = 0;j < epochLenLB;j++)
            {
                SatlitData epochSatlit = currEpoch.at(j);
                if (epochSatlit.PRN == preEpochSatlit.PRN&&epochSatlit.SatType == preEpochSatlit.SatType)
                {
                    NewPrnFlag.append(j);//Compared with the location of the same satellite in the previous epoch, it is not found with -1
                    Isfind = true;
                    break;
                }
            }
            if (!Isfind)
            {
                NewPrnFlag.append(-1);//Compared with the location of the same satellite in the previous epoch, it is not found with -1
                isNewSatlite = true;
            }
        }

    }



    //oldPrnFlag   -1,代表可以需要增加的矩阵维数，
    //相比于前一历元，卫星和频率形成的向量oldPrnFlag,向量长度为卫星所有观测频率相加
    for (int i = 0;i < epochLenLB;i++)
    {//Whether the satellite inspections before and after the cycle are completely equal
        SatlitData epochSatlit = currEpoch.at(i);
        bool Isfind = false;//Whether the tag finds the last epoch
        for (int j = 0;j < preEpochLen;j++)
        {
            SatlitData preEpochSatlit = preEpoch.at(j);
            if (epochSatlit.PRN == preEpochSatlit.PRN&&epochSatlit.SatType == preEpochSatlit.SatType)
            {
                if(opt_corr->Combination_mode==Comb_UD_mode)
                {
                    for(int jj=0;jj<5;jj++)//对于当前历元与上一历元同时观测到的卫星，如果出现新增的频率，则对新增频率赋值-1以标志。
                    {
                        if(epochSatlit.ObsFreflag[jj]==1)
                        {
                            if(epochSatlit.ObsFreflag[jj]!=preEpochSatlit.ObsFreflag[jj])
                                oldPrnFlag.append(-1);
                            else if(epochSatlit.cycly_slipF==1)
                                oldPrnFlag.append(-3);//-3标记为周跳
                            else
                                oldPrnFlag.append(oldSatLen);
                            oldSatLen++;
                        }
                    }
                    Isfind = true;
                    break;
                }
                else
                {
                    oldPrnFlag.append(j);//Compared with the location of the same satellite in the previous epoch, it is not found with -1
                    Isfind = true;
                    oldSatLen++;
                    break;
                }
            }
        }
        if (!Isfind)
        {
            if(opt_corr->Combination_mode==Comb_UD_mode)
            {
                for(int jj=0;jj<5;jj++)
                {
                    if(epochSatlit.ObsFreflag[jj]==1)
                        oldPrnFlag.append(-1);//Compared with the location of the same satellite in the previous epoch, it is not found with -1
                }
            }
            else
            {
                oldPrnFlag.append(-1);
            }
            isNewSatlite = true;
        }
        if(epochSatlit.cycly_slipF==1)
            isNewSatlite = true;
    }
    //NewPrnFlag   -1,代表需要删除的矩阵维数，
    //相比于前一历元，卫星和频率形成的向量NewPrnFlag,向量长度为卫星所有观测频率相加
    int newSatlen = 0;
    for (int i = 0;i < preEpochLen;i++)
    {//Whether the satellite inspections before and after the cycle are completely equal
        SatlitData preEpochSatlit = preEpoch.at(i);
        bool Isfind = false;//Whether the tag finds the last epoch
        for (int j = 0;j < epochLenLB;j++)
        {
            SatlitData epochSatlit = currEpoch.at(j);
            if (epochSatlit.PRN == preEpochSatlit.PRN&&epochSatlit.SatType == preEpochSatlit.SatType)
            {
                if(opt_corr->Combination_mode==Comb_UD_mode)
                {
                    for(int jj=0;jj<5;jj++)
                    {
                        if(preEpochSatlit.ObsFreflag[jj]==1)
                        {
                            if(epochSatlit.ObsFreflag[jj]!=preEpochSatlit.ObsFreflag[jj])
                                NewPrnFlag.append(-1);
                            else
                                NewPrnFlag.append(newSatlen);
                            newSatlen++;
                        } 
                    }
                    Isfind = true;
                    break;
                }
                else
                {
                    NewPrnFlag.append(j);//Compared with the location of the same satellite in the previous epoch, it is not found with -1
                    Isfind = true;
                    break;
                }
            }
        }
        if (!Isfind)
        {
            if(opt_corr->Combination_mode==Comb_UD_mode)
            {
                for(int jj=0;jj<5;jj++)
                {
                    if(preEpochSatlit.ObsFreflag[jj]==1)
                        NewPrnFlag.append(-1);//Compared with the location of the same satellite in the previous epoch, it is not found with -1
                }
            }
            else
            {
                NewPrnFlag.append(-1);
            }
            isNewSatlite = true;
        }
    }
    return isNewSatlite;
}

//Determine whether the number of satellites has changed (comparison of two epochs before and after)   debug by xiaogongwei 2019.04.29
bool coefficient_matrix::isSatelliteChange(QVector< SatlitData > &preEpoch,QVector< SatlitData > &currEpoch, QVector< int > &oldPrnFlag,QVector< int > &NewPrnFlag)
{
    int preEpochLen = preEpoch.length();
    int epochLenLB = currEpoch.length();
    //Determine whether the number of satellites has changed (comparison of two epochs before and after)
    int oldSatLen = 0;
    bool isNewSatlite = false;
    corr_opt *opt_corr = AGNSS_APPData::sGetAPPDataInstance()->mGetopt_corrData();

    for (int i = 0;i < epochLenLB;i++)
    {//Whether the satellite inspections before and after the cycle are completely equal
        SatlitData epochSatlit = currEpoch.at(i);
        bool Isfind = false;//Whether the tag finds the last epoch
        for (int j = 0;j < preEpochLen;j++)
        {
            SatlitData preEpochSatlit = preEpoch.at(j);
            if (epochSatlit.PRN == preEpochSatlit.PRN&&epochSatlit.SatType == preEpochSatlit.SatType)
            {
                if(opt_corr->Combination_mode==Comb_UD_mode)
                {
                    bool UDisfind=false;
                    for(int jj=0;jj<5;jj++)
                    {
                        if(epochSatlit.ObsFreflag[jj]!=preEpochSatlit.ObsFreflag[jj])
                            UDisfind = true;

                    }
                    if(!UDisfind)
                    {
                        oldPrnFlag.append(j);//Compared with the location of the same satellite in the previous epoch, it is not found with -1
                        Isfind = true;
                        oldSatLen++;
                        break;
                    }
                }
                else
                {
                    oldPrnFlag.append(j);//Compared with the location of the same satellite in the previous epoch, it is not found with -1
                    Isfind = true;
                    oldSatLen++;
                    break;
                }
            }
        }
        if (!Isfind)
        {
            oldPrnFlag.append(-1);//Compared with the location of the same satellite in the previous epoch, it is not found with -1
            isNewSatlite = true;
        }
        if(epochSatlit.cycly_slipF==1)
            isNewSatlite = true;
    }
    for (int i = 0;i < preEpochLen;i++)
    {//Whether the satellite inspections before and after the cycle are completely equal
        SatlitData preEpochSatlit = preEpoch.at(i);
        bool Isfind = false;//Whether the tag finds the last epoch
        for (int j = 0;j < epochLenLB;j++)
        {
            SatlitData epochSatlit = currEpoch.at(j);
            if (epochSatlit.PRN == preEpochSatlit.PRN&&epochSatlit.SatType == preEpochSatlit.SatType)
            {
                if(opt_corr->Combination_mode==Comb_UD_mode)
                {
                    bool UDisfind=false;
                    for(int jj=0;jj<5;jj++)
                    {
                        if(epochSatlit.ObsFreflag[jj]!=preEpochSatlit.ObsFreflag[jj])
                            UDisfind = true;

                    }
                    if(!UDisfind)
                    {
                        NewPrnFlag.append(j);//Compared with the location of the same satellite in the previous epoch, it is not found with -1
                        Isfind = true;
                        break;
                    }
                }
                else
                {
                    NewPrnFlag.append(j);//Compared with the location of the same satellite in the previous epoch, it is not found with -1
                    Isfind = true;
                    break;
                }

            }
        }
        if (!Isfind)
        {
            NewPrnFlag.append(-1);//Compared with the location of the same satellite in the previous epoch, it is not found with -1
            isNewSatlite = true;
        }
    }
    return isNewSatlite;
}
// get matrix B and observer L for IF Combination
void coefficient_matrix::Obtaining_SPP_IF_equation(QVector< SatlitData > &Current_sat, double *m_ApproxRecPos, MatrixXd &mat_B, VectorXd &Vct_L,
                             MatrixXd &mat_P,const corr_opt *opt_corr,para_trans *trans_para)
{
    int Avai_satN = Current_sat.length();
    MatrixXd B, P;
    VectorXd L;
    B.resize(Avai_satN,3+trans_para->sys_num);
    P.resize(Avai_satN,Avai_satN);
    L.resize(Avai_satN);
    B.setZero(Avai_satN,3+trans_para->sys_num);
    L.setZero(Avai_satN);
    P.setIdentity();
    for (int i = 0; i < Avai_satN;i++)
    {
        SatlitData oneSatlit = Current_sat.at(i);
        double XYZ_coef[3],p0 = 0,satXYZ[3]={oneSatlit.X,oneSatlit.Y,oneSatlit.Z};
//        p0 = m_mat_cal.geodist(satXYZ,m_ApproxRecPos,XYZ_coef);
        double li = 0,mi = 0,ni = 0,dltaX = 0,dltaY = 0,dltaZ = 0;
        dltaX = oneSatlit.X - m_ApproxRecPos[0];
        dltaY = oneSatlit.Y - m_ApproxRecPos[1];
        dltaZ = oneSatlit.Z - m_ApproxRecPos[2];
        p0 = qSqrt(dltaX*dltaX+dltaY*dltaY+dltaZ*dltaZ);
        // compute li mi ni
        li = dltaX/p0;mi = dltaY/p0;ni = dltaZ/p0;
        //Correction of each
        double dlta = 0;
        dlta =  - oneSatlit.StaClock + oneSatlit.SatTrop - oneSatlit.Relativty -
            oneSatlit.Sagnac - oneSatlit.TideEffect - oneSatlit.AntHeight;
        // set B L P
//        for(int j=0;j<3;j++)
//        {
//            B(i,j) = XYZ_coef[j];
//        }
        B(i,0) = li;B(i,1) = mi;B(i,2) = ni;
//        B(i,3) = -oneSatlit.StaTropMap;
        int clkref=0,bds_flag=0;
        for(int j=0;j<opt_corr->m_sys_str.length();j++)
        {
            if(opt_corr->m_sys_str[j] == oneSatlit.SatType&&oneSatlit.SatType=='C'&&oneSatlit.BDS_2flag)
            {
                B(i,3+clkref) = -1;
            }
            else if(opt_corr->m_sys_str[j] == oneSatlit.SatType&&oneSatlit.SatType=='C')
            {
                if(trans_para->sys_len[j]==1)   clkref++;
                B(i,3+clkref) = -1;
            }
            else if(opt_corr->m_sys_str[j] == oneSatlit.SatType)
            {
                B(i,3+clkref) = -1;
            }
            if(opt_corr->m_sys_str[j]=='C')
            {
                if(trans_para->sys_len[j] == 1) clkref++;
                bds_flag++;
                if(trans_para->sys_len[j+bds_flag] == 1) clkref++;
            }
            else
                if(trans_para->sys_len[j+bds_flag] == 1) clkref++;
        }
        //Carrier L  pseudorange code L
        L(i) = p0 - oneSatlit.IF_P + dlta;
        // Computing weight matrix P
        P(i, i) = oneSatlit.SatWight;// Carrier L1 weight
    }
    mat_B = B;
    Vct_L = L;
    mat_P = P;
}
// get matrix B and observer L for No Combination
void coefficient_matrix::Obtaining_SPP_NoCombination_equation(QVector< SatlitData > &Current_sat, double *m_ApproxRecPos, MatrixXd &mat_B, VectorXd &Vct_L,
                             MatrixXd &mat_P,int Total_sat_fre,const corr_opt *opt_corr,para_trans *trans_para)
{
    int Avai_satN = Current_sat.length();
    MatrixXd B, P;
    VectorXd L;
    B.resize(Total_sat_fre,3+trans_para->sys_num);
    P.resize(Total_sat_fre,Total_sat_fre);
    L.resize(Total_sat_fre);
    B.setZero(Total_sat_fre,3+trans_para->sys_num);
    L.setZero(Total_sat_fre);
    P.setIdentity();
    int column_pos =0;
    for (int i = 0; i < Avai_satN;i++)
    {
        SatlitData oneSatlit = Current_sat.at(i);
        double XYZ_coef[3],p0 = 0,satXYZ[3]={oneSatlit.X,oneSatlit.Y,oneSatlit.Z};
//        p0 = m_mat_cal.geodist(satXYZ,m_ApproxRecPos,XYZ_coef);
        double li = 0,mi = 0,ni = 0,dltaX = 0,dltaY = 0,dltaZ = 0;
        dltaX = oneSatlit.X - m_ApproxRecPos[0];
        dltaY = oneSatlit.Y - m_ApproxRecPos[1];
        dltaZ = oneSatlit.Z - m_ApproxRecPos[2];
        p0 = qSqrt(dltaX*dltaX+dltaY*dltaY+dltaZ*dltaZ);
        // compute li mi ni
        li = dltaX/p0;mi = dltaY/p0;ni = dltaZ/p0;
        XYZ_coef[0]=li;XYZ_coef[1]=mi;XYZ_coef[2]=ni;
        //Correction of each
        double dlta = 0;
        dlta =  - oneSatlit.StaClock + oneSatlit.SatTrop - oneSatlit.Relativty -
            oneSatlit.Sagnac - oneSatlit.TideEffect - oneSatlit.AntHeight;
        // set B L P
        for(int mufre=0;mufre<5;mufre++)
        {
            if(oneSatlit.ObsFreflag[mufre]!=1)  continue;
            for(int j=0;j<3;j++)
            {
                B(column_pos,j) = XYZ_coef[j];
            }
//            B(column_pos,3) = -oneSatlit.StaTropMap;
            int clkref=0,bds_flag=0;
            for(int j=0;j<opt_corr->m_sys_str.length();j++)
            {
                if(opt_corr->m_sys_str[j] == oneSatlit.SatType&&oneSatlit.SatType=='C'&&oneSatlit.BDS_2flag)
                {
                    B(column_pos,3+clkref) = -1;
                }
                else if(opt_corr->m_sys_str[j] == oneSatlit.SatType&&oneSatlit.SatType=='C')
                {
                    if(trans_para->sys_len[j]==1)   clkref++;
                    B(column_pos,3+clkref) = -1;
                }
                else if(opt_corr->m_sys_str[j] == oneSatlit.SatType)
                {
                    B(column_pos,3+clkref) = -1;
                }
                if(opt_corr->m_sys_str[j]=='C')
                {
                    if(trans_para->sys_len[j] == 1) clkref++;
                    bds_flag++;
                    if(trans_para->sys_len[j+bds_flag] == 1) clkref++;
                }
                else
                    if(trans_para->sys_len[j+bds_flag] == 1) clkref++;
            }
            //Carrier L  pseudorange code L
            L(column_pos) = p0 - oneSatlit.CodePP[mufre] + dlta + oneSatlit.SatIon[mufre];//
            // Computing weight matrix P
            P(column_pos, column_pos) = oneSatlit.SatWight;// Carrier L1 weight
            column_pos++;
        }

    }//B, L is calculated
    // save data to mat_B
    mat_B = B;
    Vct_L = L;
    mat_P = P;

}
/*Initialize the satellites in the suspect domain*/
void coefficient_matrix::Domain_of_doubt_IF(QVector< SatlitData > &preEpoch,QVector< SatlitData > &current_Sat,const corr_opt *opt_corr,para_trans *trans_para)
{
    int preEpochLen = preEpoch.length();
    int epochLenLB = current_Sat.length();
    QVector< SatlitData > Domain_sat;
    //Determine whether the number of satellites has changed (comparison of two epochs before and after)
    for (int i = 0;i < epochLenLB;i++)
    {//Whether the satellite inspections before and after the cycle are completely equal
        SatlitData epochSatlit = current_Sat.at(i);
        for (int j = 0;j < preEpochLen;j++)
        {
            SatlitData preEpochSatlit = preEpoch.at(j);
            if (epochSatlit.PRN == preEpochSatlit.PRN&&epochSatlit.SatType == preEpochSatlit.SatType)
            {
                if(preEpochSatlit.Domain_of_doubt==-1)
                {
                    trans_para->State_X(i+trans_para->const_parm) = (epochSatlit.IF_P - epochSatlit.IF_L)/M_GetLamta3(epochSatlit.Frq[epochSatlit.IF_Frq[0]],epochSatlit.Frq[epochSatlit.IF_Frq[1]]);
                    epochSatlit.SatWight_IF[0]= preEpochSatlit.SatWight_IF[0];
                    epochSatlit.SatWight_IF[1]= preEpochSatlit.SatWight_IF[1];
                    for(int n=0;n<trans_para->Covar_P.cols();n++)
                    {
                        if(i+trans_para->const_parm!=n)
                            trans_para->Covar_P(i+trans_para->const_parm,n) = trans_para->Covar_P(n,i+trans_para->const_parm) = 0.0;
                        else
                            trans_para->Covar_P(n,n) = opt_corr->sat_ambP;
                    }
                }
                break;
            }
        }
        Domain_sat.append(epochSatlit);
    }
    if(Domain_sat.length() == current_Sat.length())
        current_Sat = Domain_sat;
    Domain_sat.clear();
}
/*Initialize the satellites in the suspect domain*/
void coefficient_matrix::Domain_of_doubt_NoCom(QVector< SatlitData > &preEpoch,QVector< SatlitData > &current_Sat,const corr_opt *opt_corr,para_trans *trans_para)
{
    int preEpochLen = preEpoch.length();
    int epochLenLB = current_Sat.length();
    //Determine whether the number of satellites has changed (comparison of two epochs before and after)
    int con_Num = 0;
    for (int i = 0;i < epochLenLB;i++)
    {//Whether the satellite inspections before and after the cycle are completely equal
        SatlitData epochSatlit = current_Sat.at(i);
        for (int j = 0;j < preEpochLen;j++)
        {
            SatlitData preEpochSatlit = preEpoch.at(j);
            if (epochSatlit.PRN == preEpochSatlit.PRN&&epochSatlit.SatType == preEpochSatlit.SatType)
            {
                if(preEpochSatlit.Domain_of_doubt==-1)
                {
                    int init_Pos = con_Num + 1;
//                    for(int n=0;n<Covar_P.cols();n++)
//                    {
//                        if(n!=con_Num)
//                            Covar_P(con_Num,n) = Covar_P(n,con_Num) = 0.0;
//                        else
//                            Covar_P(n,n) = sat_ionP;
//                    }
                    for(int mufre=0;mufre<5;mufre++)
                    {
                        if(epochSatlit.ObsFreflag[mufre]!=1)  continue;
                        epochSatlit.SatWight_UD[mufre*2]= preEpochSatlit.SatWight_UD[mufre*2];
                        epochSatlit.SatWight_UD[mufre*2+1]= preEpochSatlit.SatWight_UD[mufre*2+1];
//                        State_X(init_Pos) = (epochSatlit.CodeP[mufre] - epochSatlit.PhaseL[mufre])/(M_C/epochSatlit.Frq[mufre]);
//                        for(int n=0;n<Covar_P.cols();n++)
//                        {
//                            if(init_Pos+const_parm!=n)
//                                Covar_P(init_Pos+const_parm,n) = Covar_P(n,init_Pos+const_parm) = 0.0;
//                            else
//                                Covar_P(n,n) = sat_ambP;
//                        }
//                        init_Pos++;
                    }
                }
                break;
            }
        }
        con_Num++;
        for(int mufre=0;mufre<5;mufre++)
        {
            if(epochSatlit.ObsFreflag[mufre]!=1)  continue;
            con_Num++;
        }

    }
}
// get matrix B and observer L for No Combination
void coefficient_matrix::Obtaining_WL_constraint_equation(QVector< SatlitData > &Current_sat, double *m_ApproxRecPos, MatrixXd &mat_B, VectorXd &Vct_L,
                             MatrixXd &mat_P,int Total_sat_fre,const corr_opt *opt_corr,para_trans *trans_para)
{
    int Avai_satN = Current_sat.length();
    MatrixXd B, P;
    VectorXd L;
    VectorXd sys_len = trans_para->sys_len;
    QString m_sys_str=opt_corr->m_sys_str;
    int sys_num=trans_para->sys_num;
    int const_parm = 4+sys_num+trans_para->curr_IFB_n;
    trans_para->const_parm = const_parm;
    disply_textEdit *text_disp= AGNSS_APPData::sGetAPPDataInstance()->mGettrans_textEdit();
    QString aaa ;
    switch(opt_corr->filter_model)
    {
        case PPP_STATIC_SRIF:
        case PPP_KINEMATIC_SRIF:
        case PPP_STATIC_KALM:
        case PPP_KINEMATIC_KALM:
            B.resize(Total_sat_fre*2,const_parm+Avai_satN+Total_sat_fre);
            P.resize(Total_sat_fre*2,Total_sat_fre*2);
            L.resize(Total_sat_fre*2);
            B.setZero(Total_sat_fre*2,const_parm+Avai_satN+Total_sat_fre);
            L.setZero(Total_sat_fre*2);
            P.setIdentity();
            break;
        default:
            {
                aaa = "Obtaining_IF_equation you should use setModel().";
                text_disp->data_text.append(aaa);
                text_disp->data_lenth++;
            }
            break;
    }
    int column_pos =0;
    for (int i = 0; i < Avai_satN;i++)
    {
        SatlitData oneSatlit = Current_sat.at(i);


        double XYZ_coef[3],p0 = 0;

        double li = 0,mi = 0,ni = 0,dltaX = 0,dltaY = 0,dltaZ = 0;
        dltaX = oneSatlit.X - m_ApproxRecPos[0];
        dltaY = oneSatlit.Y - m_ApproxRecPos[1];
        dltaZ = oneSatlit.Z - m_ApproxRecPos[2];
        p0 = qSqrt(dltaX*dltaX+dltaY*dltaY+dltaZ*dltaZ);
        li = dltaX/p0;mi = dltaY/p0;ni = dltaZ/p0;
        XYZ_coef[0]=li;XYZ_coef[1]=mi;XYZ_coef[2]=ni;
        //Correction of each
        double dlta = 0;
        dlta =  - oneSatlit.StaClock + oneSatlit.SatTrop - oneSatlit.Relativty -
            oneSatlit.Sagnac - oneSatlit.TideEffect - oneSatlit.AntHeight;
        // set B L P
        int coeffI=0,one_fre = 0;
        double fre1=0,fre_n=0;int clk_ref=0,bds_flag=0,IFB_n=0,mult_fre_IFBn=0;
        fre1 = oneSatlit.Frq[0];//第一频率设置为所选第一频率，防止第一频率丢失导致电离层值变化
        for(int mufre=0;mufre<5;mufre++)
        {
            if(oneSatlit.ObsFreflag[mufre]!=1)  continue;
            for(int j=0;j<3;j++)
            {
                B(column_pos*2,j) = XYZ_coef[j];
                B(column_pos*2+1,j) = XYZ_coef[j];
            }
            B(column_pos*2,3) = -oneSatlit.StaTropMap;
            B(column_pos*2+1,3) = -oneSatlit.StaTropMap;
            clk_ref=0;bds_flag=0;IFB_n=0;
            for(int j=0;j<m_sys_str.length();j++)
            {
                if(m_sys_str[j] == oneSatlit.SatType&&oneSatlit.SatType=='C'&&oneSatlit.BDS_2flag)
                {
                    B(column_pos*2,4+clk_ref+IFB_n) = -1;
                    B(column_pos*2+1,4+clk_ref+IFB_n) = -1;
                    if(trans_para->curr_C2IFB_falg&&mufre==2)
                    {
                        IFB_n++;
                        B(column_pos*2+1,4+clk_ref+IFB_n) = -1;
                    }
                    if(clk_ref!=0&&opt_corr->sys_error_select==ISBs_mode)
                        B(column_pos*2+1,4) = B(column_pos*2,4) = -1;
                }
                else if(m_sys_str[j] == oneSatlit.SatType&&oneSatlit.SatType=='C')
                {
                    if(sys_len[j]==1)   clk_ref++;
                    if(trans_para->curr_C2IFB_falg)
                    {
                        IFB_n++;
                    }
                    B(column_pos*2,4+clk_ref+IFB_n) = -1;
                    B(column_pos*2+1,4+clk_ref+IFB_n) = -1;
                    if(trans_para->curr_C3IFB3_falg&&mufre==2)
                    {
                        IFB_n++;
                        B(column_pos*2+1,4+clk_ref+IFB_n+mult_fre_IFBn) = -1;
                        mult_fre_IFBn++;
                    }
                    if(trans_para->curr_C3IFB4_falg&&mufre==3)
                    {
                        IFB_n++;
                        B(column_pos*2+1,4+clk_ref+IFB_n+mult_fre_IFBn) = -1;
                        mult_fre_IFBn++;
                    }
                    if(trans_para->curr_C3IFB5_falg&&mufre==4)
                    {
                        IFB_n++;
                        B(column_pos*2+1,4+clk_ref+IFB_n+mult_fre_IFBn) = -1;
                        mult_fre_IFBn++;
                    }
                    if(clk_ref!=0&&opt_corr->sys_error_select==ISBs_mode)
                        B(column_pos*2+1,4) = B(column_pos*2,4) = -1;
                }
                else if(m_sys_str[j] == oneSatlit.SatType)
                {
                    B(column_pos*2,4+clk_ref+IFB_n) = -1;
                    B(column_pos*2+1,4+clk_ref+IFB_n) = -1;
                    if(oneSatlit.SatType=='G')
                    {
                        if(trans_para->curr_GIFB_falg&&mufre==2)
                        {
                            IFB_n++;
                            B(column_pos*2+1,4+clk_ref+IFB_n) = -1;
                        }
                    }
                    if(oneSatlit.SatType=='E')
                    {
                        if(trans_para->curr_EIFB3_falg&&mufre==2)
                        {
                            IFB_n++;
                            B(column_pos*2+1,4+clk_ref+IFB_n+mult_fre_IFBn) = -1;
                            mult_fre_IFBn++;
                        }
                        if(trans_para->curr_EIFB4_falg&&mufre==3)
                        {
                            IFB_n++;
                            B(column_pos*2+1,4+clk_ref+IFB_n+mult_fre_IFBn) = -1;
                            mult_fre_IFBn++;
                        }
                        if(trans_para->curr_EIFB5_falg&&mufre==4)
                        {
                            IFB_n++;
                            B(column_pos*2+1,4+clk_ref+IFB_n+mult_fre_IFBn) = -1;
                            mult_fre_IFBn++;
                        }
                    }
                    if(clk_ref!=0&&opt_corr->sys_error_select==ISBs_mode)
                        B(column_pos*2+1,4) = B(column_pos*2,4) = -1;
                }
                if(m_sys_str[j]=='C')
                {
                    if(sys_len[j] == 1)
                    {
                        clk_ref++;
                        if(trans_para->curr_C2IFB_falg)
                            IFB_n++;
                    }
                    bds_flag++;
                    if(sys_len[j+bds_flag] == 1)
                    {
                        clk_ref++;
                        if(trans_para->curr_C3IFB3_falg)
                            IFB_n++;
                        if(trans_para->curr_C3IFB4_falg)
                            IFB_n++;
                        if(trans_para->curr_C3IFB5_falg)
                            IFB_n++;
                    }
                }
                else if(m_sys_str[j]=='E')
                {
                    if(sys_len[j+bds_flag] == 1)
                    {
                        clk_ref++;
                        if(trans_para->curr_EIFB3_falg)
                            IFB_n++;
                        if(trans_para->curr_EIFB4_falg)
                            IFB_n++;
                        if(trans_para->curr_EIFB5_falg)
                            IFB_n++;
                    }
                }
                else if(m_sys_str[j]=='G')
                {
                    if(sys_len[j+bds_flag] == 1)
                    {
                        clk_ref++;
                        if(trans_para->curr_GIFB_falg)
                            IFB_n++;
                    }
                }
                else if(m_sys_str[j]=='R')
                {
                    if(sys_len[j+bds_flag] == 1)
                    {
                        clk_ref++;
                    }
                }

            }
            if(one_fre == 0)
            {
                one_fre = 1;
                coeffI=column_pos;
//                fre1 = oneSatlit.Frq[mufre];//第一频率设置为所选第一频率，防止第一频率丢失导致电离层值变化
            }
            fre_n = oneSatlit.Frq[mufre];
            B(column_pos*2,const_parm+i) = (fre1*fre1)/(fre_n*fre_n);
            B(column_pos*2,const_parm+Avai_satN+column_pos) = oneSatlit.Lambda[mufre];

            B(column_pos*2+1,const_parm+i) = -(fre1*fre1)/(fre_n*fre_n);
            //Carrier L  pseudorange code L
            L(column_pos*2) = p0 - oneSatlit.PhaseLL[mufre] + dlta;
            L(column_pos*2+1) = p0 - oneSatlit.CodePP[mufre] + dlta;
            // Computing weight matrix P
            P(column_pos*2, column_pos*2) = oneSatlit.SatWight * oneSatlit.SatWight_L *oneSatlit.SatWight_UD[2*mufre];// Carrier L1 weight
            P(column_pos*2+1, column_pos*2+1) = oneSatlit.SatWight*oneSatlit.SatWight_UD[2*mufre+1];// Carrier L2 weight
            column_pos++;
        }

    }//B, L is calculated
    column_pos = 0;
        // save data to mat_B
    mat_B = B;
    Vct_L = L;
    mat_P = P;

}
void coefficient_matrix::inctrase_len(VectorXd &tempXk_1,MatrixXd &tempPk_1,MatrixXd &tempQk_1,int head_len)
{
    VectorXd head_Xn = tempXk_1.head(head_len);
    VectorXd tail_Xn = tempXk_1.tail(tempXk_1.size()-head_len);
    tempXk_1.resize(head_Xn.size()+tail_Xn.size()+1);//szg
    tempXk_1.head(head_Xn.size()) = head_Xn;
    tempXk_1.tail(tail_Xn.size()) = tail_Xn;
    tempXk_1(head_len) = 0;
    MatrixXd tempPk_2;
    int Pkcols=tempPk_1.cols();
    tempPk_2.resize(Pkcols+1,Pkcols+1);
    tempPk_2.setZero(Pkcols+1,Pkcols+1);
    tempPk_2.block(0,0,head_len,head_len) = tempPk_1.block(0,0,head_len,head_len);
    tempPk_2.block(head_len+1,head_len+1,Pkcols-head_len,Pkcols-head_len) = tempPk_1.block(head_len,head_len,Pkcols-head_len,Pkcols-head_len);
    tempPk_2.block(head_len+1,0,Pkcols-head_len,head_len) = tempPk_1.block(head_len,0,Pkcols-head_len,head_len);
    tempPk_2.block(0,head_len+1,head_len,Pkcols-head_len) = tempPk_1.block(0,head_len,head_len,Pkcols-head_len);
    tempPk_2(head_len,head_len) = 1000;
    int Pk2cols=tempPk_2.cols();
    tempPk_1.resize(Pk2cols,Pk2cols);
    tempPk_1 = tempPk_2;
    MatrixXd tempQk_2;
    int Qk1cols = tempQk_1.cols();
    tempQk_2.resize(Qk1cols+1,Qk1cols+1);
    tempQk_2.setZero(Qk1cols+1,Qk1cols+1);
    tempQk_2.block(0,0,head_len,head_len) = tempQk_1.block(0,0,head_len,head_len);
    tempQk_2.block(head_len+1,head_len+1,Qk1cols-head_len,Qk1cols-head_len) = tempQk_1.block(head_len,head_len,Qk1cols-head_len,Qk1cols-head_len);
    tempQk_2(head_len,head_len)=100;
    int Qk2cols = tempQk_2.cols();
    tempQk_1.resize(Qk2cols,Qk2cols);
    tempQk_1 = tempQk_2;
}
void coefficient_matrix::reduce_len(VectorXd &tempXk_1,MatrixXd &tempPk_1,MatrixXd &tempQk_1,int head_len)
{
    VectorXd head_Xn = tempXk_1.head(head_len);
    VectorXd tail_Xn = tempXk_1.tail(tempXk_1.size()-head_len-1);
    tempXk_1.resize(head_Xn.size()+tail_Xn.size());//szg
    tempXk_1<<head_Xn,tail_Xn;
    m_mat_cal.RemoveRow(tempQk_1,head_len);
    m_mat_cal.RemoveColumn(tempQk_1,head_len);
    m_mat_cal.RemoveRow(tempPk_1,head_len);
    m_mat_cal.RemoveColumn(tempPk_1,head_len);
}
void coefficient_matrix::inctrase_rclk_len(VectorXd &tempXk_1,MatrixXd &tempPk_1,MatrixXd &tempQk_1,int head_len,int sys_error_select)
{
    VectorXd head_Xn = tempXk_1.head(head_len);
    VectorXd tail_Xn = tempXk_1.tail(tempXk_1.size()-head_len);
    tempXk_1.resize(head_Xn.size()+tail_Xn.size()+1);//szg
    tempXk_1.head(head_Xn.size()) = head_Xn;
    tempXk_1.tail(tail_Xn.size()) = tail_Xn;
    if(head_Xn.size()>4)
    {
        if(sys_error_select==ISBs_mode)
            tempXk_1(head_len) = 0;
        else
            tempXk_1(head_len) = head_Xn(4);
    }
    else
    {
        if(sys_error_select==ISBs_mode)
            tempXk_1(head_len) = 0;
        else
            tempXk_1(head_len) = tail_Xn(0);
    }
    MatrixXd tempPk_2;
    int Pkcols=tempPk_1.cols();
    tempPk_2.resize(Pkcols+1,Pkcols+1);
    tempPk_2.setZero(Pkcols+1,Pkcols+1);
    tempPk_2.block(0,0,head_len,head_len) = tempPk_1.block(0,0,head_len,head_len);
    tempPk_2.block(head_len+1,head_len+1,Pkcols-head_len,Pkcols-head_len) = tempPk_1.block(head_len,head_len,Pkcols-head_len,Pkcols-head_len);
    tempPk_2.block(head_len+1,0,Pkcols-head_len,head_len) = tempPk_1.block(head_len,0,Pkcols-head_len,head_len);
    tempPk_2.block(0,head_len+1,head_len,Pkcols-head_len) = tempPk_1.block(0,head_len,head_len,Pkcols-head_len);
    tempPk_2(head_len,head_len) = 10000;
    int Pk2cols=tempPk_2.cols();
    tempPk_1.resize(Pk2cols,Pk2cols);
    tempPk_1 = tempPk_2;
    MatrixXd tempQk_2;
    int Qk1cols = tempQk_1.cols();
    tempQk_2.resize(Qk1cols+1,Qk1cols+1);
    tempQk_2.setZero(Qk1cols+1,Qk1cols+1);
    tempQk_2.block(0,0,head_len,head_len) = tempQk_1.block(0,0,head_len,head_len);
    tempQk_2.block(head_len+1,head_len+1,Qk1cols-head_len,Qk1cols-head_len) = tempQk_1.block(head_len,head_len,Qk1cols-head_len,Qk1cols-head_len);
    if(head_len!=4&&sys_error_select==ISBs_mode)
        tempQk_2(head_len,head_len) = 100;
    else
        tempQk_2(head_len,head_len) = 10000;
    int Qk2cols = tempQk_2.cols();
    tempQk_1.resize(Qk2cols,Qk2cols);
    tempQk_1 = tempQk_2;
}
void coefficient_matrix::reduce_rclk_len(VectorXd &tempXk_1,MatrixXd &tempPk_1,MatrixXd &tempQk_1,int head_len,int sys_error_select)
{
    VectorXd head_Xn = tempXk_1.head(head_len);
    VectorXd tail_Xn = tempXk_1.tail(tempXk_1.size()-head_len-1);
    tempXk_1.resize(head_Xn.size()+tail_Xn.size());//szg
    tempXk_1<<head_Xn,tail_Xn;
    m_mat_cal.RemoveRow(tempQk_1,head_len);
    m_mat_cal.RemoveColumn(tempQk_1,head_len);
    m_mat_cal.RemoveRow(tempPk_1,head_len);
    m_mat_cal.RemoveColumn(tempPk_1,head_len);
    if(head_len==4&&sys_error_select==ISBs_mode)
    {
        tempPk_1(head_len,head_len) = 10000;
        tempQk_1(head_len,head_len) = 10000;
    }
}
void coefficient_matrix::sort_order(int de_sortN,int *sortMaxEpo)
{
    for(int i=0;i<de_sortN;i++) {
        for(int j=i+1;j<de_sortN;j++) {
            if(sortMaxEpo[i]<sortMaxEpo[j]) {
                int temp=sortMaxEpo[i];//,temp1 = sysTemFlag[i]
                sortMaxEpo[i]=sortMaxEpo[j];
                sortMaxEpo[j]=temp;
            }
        }
    }
}
