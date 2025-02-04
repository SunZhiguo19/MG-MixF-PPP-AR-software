#include "srif_algorithm.h"

srif_algorithm::srif_algorithm()
{

}

void srif_algorithm::Sequential_Least_Squares(MatrixXd &B_k,VectorXd &L_k,VectorXd &X_k,MatrixXd &P_k,MatrixXd &R_k,MatrixXd Qwk,const corr_opt *opt_corr,para_trans *trans_para)
{
   MatrixXd Q_xk,J,Pxk_1 = P_k+Qwk;


   VectorXd X_kk1 = X_k,V_k,v_k1;

   V_k =L_k - B_k*X_kk1;

   Q_xk = Pxk_1.inverse() + B_k.transpose()*R_k.inverse()*B_k;
   J = Q_xk.inverse()*B_k.transpose()*R_k.inverse();
   v_k1 = J*V_k;
   X_k=X_kk1+v_k1;
   //Filtered residual, normal download wave is very small
   trans_para->posterior_V = L_k - B_k*X_k;
   P_k=Q_xk.inverse();
   if(B_k.rows() -B_k.cols()<2) return ;
   double sigma0 = trans_para->posterior_V.transpose() *R_k.inverse() *trans_para->posterior_V;
   sigma0 = sqrt(sigma0/(B_k.rows() -B_k.cols()));
   trans_para->sigma0 = sigma0;
//    MatrixXd = Bk*tPk_1*Bk.transpose();
   trans_para->Residual_Qvv = R_k - B_k*P_k*B_k.transpose();

}
void srif_algorithm::pos_srif_calculating(QVector< SatlitData > &preEpoch,QVector< SatlitData > &current_Sat,
                                    double *m_ApproxRecPos,VectorXd &X,MatrixXd &P,int Total_sat_fre,const corr_opt *opt_corr,para_trans *trans_para)
{
    if(opt_corr->filter_model==PPP_KINEMATIC_SRIF||!trans_para->isInitPara)
    {
        // we solver five parameter[dx,dy,dz,dTrop,dClock],so epochLenLB > 4
        for(int i=0;i<3;i++)
        {
            trans_para->m_SPP_Pos[i] = m_ApproxRecPos[i];
            trans_para->State_X(i) = 0;
        }
    }
    else
    {
        for(int i=0;i<3;i++)
        {
            trans_para->m_SPP_Pos[i] = m_ApproxRecPos[i];
            trans_para->State_X(i) = 0;
        }
    }
    //save filter sate for Quality Control
    MatrixXd temp_Q = trans_para->Nois_Q,
             temp_P = trans_para->Covar_P;
    MatrixXd pre_P,pre_Q;
    VectorXd temp_X = trans_para->State_X;VectorXd pre_X,pre_sys_len;
    int pre_sys_num;
    pre_sys_num = trans_para->sys_num;pre_sys_len = trans_para->sys_len;
    quality_control_flag =opt_corr->quality_flag;
    trans_para->cross_N=0;
    filter_srif(preEpoch,current_Sat,Total_sat_fre,opt_corr,trans_para);
    pre_X = trans_para->State_X;pre_P = trans_para->Covar_P;pre_Q = trans_para->Nois_Q;
    QVector< SatlitData > CrossE_current_Sat;
    QVector< SatlitData > CrossE;
    CrossE = current_Sat;
    while(!quality_control_flag&&CrossE.length()-trans_para->const_parm>=2&&trans_para->cross_N<20)
    {
        trans_para->cross_N++;
        quality_control_flag = true;
        CrossE_current_Sat.clear();
        switch (opt_corr->quality_control_method) {
        case Classic_3cigma_method:
            Classical_gross_error_detection(CrossE,CrossE_current_Sat,&Total_sat_fre,opt_corr,trans_para);
            break;
        default:
            break;
        }
        if(quality_control_flag)    break;
        trans_para->Nois_Q = temp_Q;
        trans_para->Covar_P = temp_P;
        trans_para->State_X = temp_X;
        system_num(CrossE_current_Sat);
        Availa_SysFlag(opt_corr->m_sys_str,trans_para->sys_len,&trans_para->sys_num);
        if(CrossE_current_Sat.length()>=trans_para->const_parm+1)
            filter_srif(preEpoch,CrossE_current_Sat,Total_sat_fre,opt_corr,trans_para);
        CrossE = CrossE_current_Sat;
    }
    if(current_Sat.length() - CrossE_current_Sat.length()>3||CrossE_current_Sat.length()<trans_para->const_parm+2)
    {
         trans_para->State_X = pre_X;
         trans_para->Covar_P = pre_P;
         trans_para->Nois_Q = pre_Q;
         trans_para->sys_num = pre_sys_num;
         trans_para->sys_len = pre_sys_len;
    }
    else
    {
        current_Sat = CrossE_current_Sat;
    }
    CrossE_current_Sat.clear();
    CrossE.clear();
    //Save the results of this epoch (does not contain initialization data)
    X = trans_para->State_X;
    P = trans_para->Covar_P;
    m_ApproxRecPos[0] = trans_para->m_SPP_Pos[0] + trans_para->State_X(0);
    m_ApproxRecPos[1] = trans_para->m_SPP_Pos[1] + trans_para->State_X(1);
    m_ApproxRecPos[2] = trans_para->m_SPP_Pos[2] + trans_para->State_X(2);

}
void srif_algorithm::filter_srif(QVector< SatlitData > &preEpoch,QVector< SatlitData > &current_Sat,
                                 int Total_sat_fre,const corr_opt *opt_corr,para_trans *trans_para)
{
    int preEpochLen = preEpoch.length();
    int Avai_satN = current_Sat.length();
    // get B, wightP ,L
    MatrixXd B, wightP;
    VectorXd L;
    //First epoch initialization  Filter init
    switch (opt_corr->Combination_mode) {
    case Comb_IF_mode:
        Obtaining_IF_equation(current_Sat,trans_para->m_SPP_Pos,B,L,wightP,opt_corr,trans_para);
        break;
    case Comb_UD_mode:
        Obtaining_NoCombination_equation(current_Sat,trans_para->m_SPP_Pos,B,L,wightP,Total_sat_fre,opt_corr,trans_para);
        break;
    default:
        break;
    }
    if(0 == preEpochLen)
    {
        switch (opt_corr->Combination_mode) {
        case Comb_IF_mode:
            init_IF_matrix(Avai_satN,B,L,opt_corr,trans_para);
            break;
        case Comb_UD_mode:
            init_NOcombina_matrix(current_Sat,B,L,Total_sat_fre,opt_corr,trans_para);
            break;
        default:
            break;
        }
    }


    //Determine whether the number of satellites has changed (comparison of two epochs before and after)
    QVector< int > oldPrnFlag;//Compared with the location of the same satellite in the previous epoch, it is not found with -1
    bool isNewSatlite = false;
    QVector< int > NewPrnFlag;
    isNewSatlite = isSatelliteChange(preEpoch, current_Sat, oldPrnFlag,NewPrnFlag);


    //Change filter parameters
    if (((preEpochLen != Avai_satN) || isNewSatlite ) && preEpochLen != 0)
    {
        switch (opt_corr->Combination_mode) {
        case Comb_IF_mode:
            change_IF_Paramatrix(preEpoch,current_Sat,oldPrnFlag,preEpochLen,NewPrnFlag,opt_corr,trans_para);
            break;
        case Comb_UD_mode:
            change_NoCombination_Para(preEpoch,current_Sat,oldPrnFlag,preEpochLen,NewPrnFlag,Total_sat_fre,opt_corr,trans_para);
            break;
        default:
            break;
        }
    }
    //Update Rk_1（There is no change in the number of satellites）
    switch (opt_corr->Combination_mode) {
    case Comb_IF_mode:
        update_IF_Rk(current_Sat,B.rows(),opt_corr,trans_para);
        break;
    case Comb_UD_mode:
        update_NoCombination_Rk(current_Sat,B.rows(),opt_corr,trans_para);
        break;
    default:
        break;
    }
    oldPrnFlag.clear();
    NewPrnFlag.clear();
    //Version Kalman filter
    Sequential_Least_Squares(B,L,trans_para->State_X,trans_para->Covar_P,trans_para->Covar_Rk,trans_para->Nois_Q,opt_corr,trans_para);
}
void srif_algorithm::pos_WL_srif_calculating(QVector< SatlitData > &preEpoch,QVector< SatlitData > &current_Sat,
                                    double *m_ApproxRecPos,VectorXd &X,MatrixXd &P,int Total_sat_fre,const corr_opt *opt_corr,para_trans *trans_para)
{
    for(int i=0;i<3;i++)
    {
        trans_para->m_SPP_Pos[i] = m_ApproxRecPos[i];
        trans_para->State_X(i) = 0;
    }
    //save filter sate for Quality Control
    MatrixXd temp_Q = trans_para->Nois_Q,
             temp_P = trans_para->Covar_P;
    MatrixXd pre_P,pre_Q;
    VectorXd temp_X = trans_para->State_X;VectorXd pre_X,pre_sys_len,temp_sys_len;
    int pre_sys_num,temp_sys_num;
    quality_control_flag = opt_corr->quality_flag;
    trans_para->cross_N=0;
    temp_sys_num = trans_para->sys_num;temp_sys_len = trans_para->sys_len;
    QString wirte_debug;
    bool Helmert_f = false;
    if(opt_corr->Helmert_flag)
    {
        Helmert_f = true;
        trans_para->Helmert_flag = false;
    }
    trans_para->fixed_WL_number=fixed_WL_ambiguity_N(current_Sat);
    filter_WL_srif(preEpoch,current_Sat,Total_sat_fre,opt_corr,trans_para);
    pre_X = trans_para->State_X;pre_P = trans_para->Covar_P;pre_Q = trans_para->Nois_Q;
    pre_sys_num = trans_para->sys_num;pre_sys_len = trans_para->sys_len;
    int pre_const_parm = trans_para->const_parm;
    QVector< SatlitData > CrossE_current_Sat;
    QVector< SatlitData > CrossE;
    CrossE = current_Sat;
    while(!quality_control_flag&&CrossE.length()-trans_para->const_parm>=1&&trans_para->cross_N<6)
    {
        trans_para->cross_N++;
        quality_control_flag = true;
        CrossE_current_Sat.clear();
        switch (opt_corr->quality_control_method) {
        case Classic_3cigma_method:
            Classical_gross_error_detection(CrossE,CrossE_current_Sat,&Total_sat_fre,opt_corr,trans_para);
            break;
        default:
            break;
        }
        if(quality_control_flag)    break;
        trans_para->Nois_Q = temp_Q;
        trans_para->Covar_P = temp_P;
        trans_para->State_X = temp_X;
        system_num(CrossE_current_Sat);
        Availa_SysFlag(opt_corr->m_sys_str,trans_para->sys_len,&trans_para->sys_num);
        trans_para->fixed_WL_number=fixed_WL_ambiguity_N(CrossE_current_Sat);
        if(CrossE_current_Sat.length()>=trans_para->const_parm+1)
            filter_WL_srif(preEpoch,CrossE_current_Sat,Total_sat_fre,opt_corr,trans_para);
        CrossE = CrossE_current_Sat;
    }
    if(!opt_corr->quality_flag)
    {
        if((current_Sat.length() - CrossE_current_Sat.length()>3||CrossE_current_Sat.length()<trans_para->const_parm+1))
        {
             trans_para->State_X = pre_X;
             trans_para->Covar_P = pre_P;
             trans_para->Nois_Q = pre_Q;
             trans_para->sys_num = pre_sys_num;
             trans_para->sys_len = pre_sys_len;
             trans_para->const_parm=pre_const_parm;
        }
        else
        {
    //        wirte_debug="CrossE_current_Sat 0000";
    //        writeQdebug2Txt(wirte_debug);
            current_Sat = CrossE_current_Sat;
        }
    }
    if(Helmert_f)
    {
        trans_para->Helmert_flag = true;;
        trans_para->Nois_Q = temp_Q;
        trans_para->Covar_P = temp_P;
        trans_para->State_X = temp_X;
        system_num(current_Sat);
        Availa_SysFlag(opt_corr->m_sys_str,trans_para->sys_len,&trans_para->sys_num);
        filter_WL_srif(preEpoch,current_Sat,Total_sat_fre,opt_corr,trans_para);
    }
    CrossE_current_Sat.clear();
    CrossE.clear();
    //Save the results of this epoch (does not contain initialization data)
    X = trans_para->State_X;
    P = trans_para->Covar_P;
    m_ApproxRecPos[0] = trans_para->m_SPP_Pos[0] + trans_para->State_X(0);
    m_ApproxRecPos[1] = trans_para->m_SPP_Pos[1] + trans_para->State_X(1);
    m_ApproxRecPos[2] = trans_para->m_SPP_Pos[2] + trans_para->State_X(2);
    trans_para->receiver_pos[0] = m_ApproxRecPos[0];
    trans_para->receiver_pos[1] = m_ApproxRecPos[1];
    trans_para->receiver_pos[2] = m_ApproxRecPos[2];

}
void srif_algorithm::filter_WL_srif(QVector< SatlitData > &preEpoch,QVector< SatlitData > &current_Sat,
                                 int Total_sat_fre,const corr_opt *opt_corr,para_trans *trans_para)
{
    int preEpochLen = preEpoch.length();
    int Avai_satN = current_Sat.length();
    GNSS_IFB_and_osbfre(current_Sat,trans_para);
    // get B, wightP ,L
    MatrixXd B, wightP;
    VectorXd L;
    //First epoch initialization  Filter init
    switch (opt_corr->Combination_mode) {
    case Comb_IF_mode:
        Obtaining_IF_equation(current_Sat,trans_para->m_SPP_Pos,B,L,wightP,opt_corr,trans_para);
        break;
    case Comb_UD_mode:
        Obtaining_WL_constraint_equation(current_Sat,trans_para->m_SPP_Pos,B,L,wightP,Total_sat_fre,opt_corr,trans_para);
        break;
    default:
        break;
    }
    if(0 == preEpochLen)
    {
        switch (opt_corr->Combination_mode) {
        case Comb_IF_mode:
            init_IF_matrix(Avai_satN,B,L,opt_corr,trans_para);
            break;
        case Comb_UD_mode:
            init_NO_WL_combina_matrix(current_Sat,B,L,Total_sat_fre,opt_corr,trans_para);
            break;
        default:
            break;
        }
    }


    //Determine whether the number of satellites has changed (comparison of two epochs before and after)
    QVector< int > oldPrnFlag;//Compared with the location of the same satellite in the previous epoch, it is not found with -1
    bool isNewSatlite = false;
    QVector< int > NewPrnFlag;
    if(opt_corr->frequency_model==mixed_frequency&& preEpochLen != 0)
        change_mixed_fre_Combination(preEpoch,current_Sat,oldPrnFlag,NewPrnFlag,Total_sat_fre,opt_corr,trans_para);
    else if(opt_corr->frequency_model==fixed_frequency&& preEpochLen != 0){
        //Determine whether the number of satellites has changed (comparison of two epochs before and after)

        isNewSatlite = isSatelliteChange(preEpoch, current_Sat, oldPrnFlag,NewPrnFlag);

        //Change filter parameters
        if (((preEpochLen != Avai_satN) || isNewSatlite ) && preEpochLen != 0)
        {
            switch (opt_corr->Combination_mode) {
            case Comb_IF_mode:
                change_IF_Paramatrix(preEpoch,current_Sat,oldPrnFlag,preEpochLen,NewPrnFlag,opt_corr,trans_para);
                break;
            case Comb_UD_mode:
                change_No_WL_Combination_Para(preEpoch,current_Sat,oldPrnFlag,preEpochLen,NewPrnFlag,Total_sat_fre,opt_corr,trans_para);
                break;
            default:
                break;
            }
        }
    }

    //Update Rk_1（There is no change in the number of satellites）
    switch (opt_corr->Combination_mode) {
    case Comb_IF_mode:
        update_IF_Rk(current_Sat,B.rows(),opt_corr,trans_para);
        break;
    case Comb_UD_mode:
        update_WLNoCombination_Rk(current_Sat,B.rows(),trans_para);
        break;
    default:
        break;
    }
    oldPrnFlag.clear();
    NewPrnFlag.clear();
    //Version Kalman filter
    Sequential_Least_Squares(B,L,trans_para->State_X,trans_para->Covar_P,trans_para->Covar_Rk,trans_para->Nois_Q,opt_corr,trans_para);
}
