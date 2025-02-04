#include "kalman_filter.h"

kalman_filter::kalman_filter()
{

}
void kalman_filter::KalmanforStaticOu(MatrixXd Bk,VectorXd Lk,MatrixXd F,MatrixXd Qwk,
                                      MatrixXd PBk,VectorXd &tXk_1,MatrixXd &tPk_1)
{
    //Time update
    VectorXd Xkk_1 = F*tXk_1,LVk;
    MatrixXd Pkk_1 = F*tPk_1*F.transpose() + Qwk;
    LVk = Lk - Bk*Xkk_1;

    //Filter update
    MatrixXd Mk, Nk , Pkk_1_inv;
    Nk = Bk.transpose()*PBk*Bk;
    Pkk_1_inv = Pkk_1.inverse();
    Mk = Nk + Pkk_1_inv;// Symmetry is not guaranteed
//    Mk = (Nk + Nk.transpose()) / 2 + (Pkk_1_inv + Pkk_1_inv.transpose()) / 2;// Symmetry is not guaranteed
    tPk_1 = Mk.inverse();
    VectorXd delatXkd = tPk_1*(Bk.transpose())*PBk*LVk;
    tXk_1 = Xkk_1 + delatXkd;
}

//Third version use to change Kalman
void kalman_filter::KalmanforStatic(MatrixXd Bk,VectorXd Lk,MatrixXd F,MatrixXd Qwk,
                                    MatrixXd Rk,VectorXd &tXk_1,MatrixXd &tPk_1,const corr_opt *opt_corr,para_trans *trans_para)
{
    //Time update
    VectorXd Xkk_1 = F*tXk_1,Vk;
    MatrixXd Pkk_1 = F*tPk_1*F.transpose() + Qwk,I,tempKB,Kk;
//    for(int i=0;i<3;i++)
//    {
//        for(int j = 0;j<trans_para->Nois_Q.cols();j++)
//        {
//            if(i==j)
//                Pkk_1(i,j) = Pkk_1(i,j)+trans_para->Nois_Q(i,j);
//            else
//            {
//                Pkk_1(i,j) =  trans_para->Nois_Q(i,j);
//                Pkk_1(j,i) =  trans_para->Nois_Q(j,i);
//            }
//        }
//    }

    //Calculated gain matrix
    Kk = (Pkk_1*Bk.transpose())*((Bk*Pkk_1*Bk.transpose() + Rk).inverse());
    //debug 0618

    //Filter update
    Vk = Lk - Bk*Xkk_1;

    //Update X
    tXk_1 = Xkk_1 + Kk*Vk;
    //Filtered residual, normal download wave is very small
    VectorXd posterior_V = Lk - Bk*tXk_1;
    tempKB = Kk*Bk;
    I.resize(tempKB.rows(),tempKB.cols());
    I.setIdentity();
    //Update P (Case I) (this update is extremely unstable)
    tPk_1 = (I - tempKB)*Pkk_1;
    if(Bk.rows() -Bk.cols()==0) return ;
    double sigma0=0;
    sigma0 = posterior_V.transpose() *Rk.inverse() *posterior_V;
    sigma0 = sqrt(sigma0/(Bk.rows() -Bk.cols()));
    MatrixXd PQvv = (Pkk_1.inverse()+Bk.transpose()*Rk.inverse()*Bk).inverse();
//    MatrixXd = Bk*tPk_1*Bk.transpose();Pxk_1.inverse() + B_k.transpose()*R_k.inverse()*B_k
    trans_para->posterior_V = posterior_V;
    trans_para->Residual_Qvv = Rk - Bk*PQvv*Bk.transpose();
    trans_para->sigma0 = sigma0;

//    Residual_Qvv = Rk + Bk*(Bk.transpose()*wightP*Bk).inverse()*Bk.transpose();
}

void kalman_filter::pos_calculating(QVector< SatlitData > &preEpoch,QVector< SatlitData > &current_Sat,
                                    double *m_ApproxRecPos,VectorXd &X,MatrixXd &P,int Total_sat_fre,const corr_opt *opt_corr,para_trans *trans_para)
{
    if(opt_corr->filter_model==PPP_KINEMATIC_KALM||!trans_para->isInitPara)
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
    MatrixXd temp_Fi = trans_para->state_Fi, temp_Q = trans_para->Nois_Q,
             temp_P = trans_para->Covar_P;
    MatrixXd pre_P,pre_Q,pre_Fi;
    VectorXd temp_X = trans_para->State_X;VectorXd pre_X,pre_sys_len,temp_sys_len;
    int pre_sys_num,temp_sys_num;
    quality_control_flag = opt_corr->quality_flag;
    trans_para->cross_N=0;
    temp_sys_num = trans_para->sys_num;temp_sys_len = trans_para->sys_len;
//    QString wirte_debug="quality_control_flag 0000";
//    writeQdebug2Txt(wirte_debug);
    QString wirte_debug;

//    wirte_debug="quality_control_flag 000000  ";
//    wirte_debug.append(QString::number(trans_para->State_X(0),'f',4));
//    writeQdebug2Txt(wirte_debug);
    bool Helmert_f = false;
    if(opt_corr->Helmert_flag)
    {
        Helmert_f = true;
        trans_para->Helmert_flag = false;
    }

    filter_alogrithm(preEpoch,current_Sat,Total_sat_fre,opt_corr,trans_para);
    pre_X = trans_para->State_X;pre_P = trans_para->Covar_P;pre_Q = trans_para->Nois_Q;pre_Fi = trans_para->state_Fi;
    pre_sys_num = trans_para->sys_num;pre_sys_len = trans_para->sys_len;
    int pre_const_parm = trans_para->const_parm;
    QVector< SatlitData > CrossE_current_Sat;
    QVector< SatlitData > CrossE;
    CrossE = current_Sat;


    while(!quality_control_flag&&CrossE.length()-trans_para->const_parm>=2&&trans_para->cross_N<6)
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
        trans_para->state_Fi = temp_Fi ;
        trans_para->Nois_Q = temp_Q;
        trans_para->Covar_P = temp_P;
        trans_para->State_X = temp_X;
        system_num(CrossE_current_Sat);
        Availa_SysFlag(opt_corr->m_sys_str,trans_para->sys_len,&trans_para->sys_num);
        if(CrossE_current_Sat.length()>=trans_para->const_parm+1)
            filter_alogrithm(preEpoch,CrossE_current_Sat,Total_sat_fre,opt_corr,trans_para);
        CrossE = CrossE_current_Sat;
    }
    if(!opt_corr->quality_flag)
    {
        if((current_Sat.length() - CrossE_current_Sat.length()>3||CrossE_current_Sat.length()<trans_para->const_parm+1))
        {
             trans_para->State_X = pre_X;
             trans_para->Covar_P = pre_P;
             trans_para->Nois_Q = pre_Q;
             trans_para->state_Fi = pre_Fi;
             trans_para->sys_num = pre_sys_num;
             trans_para->sys_len = pre_sys_len;
             trans_para->const_parm=pre_const_parm;
             //Output year, month, day, hour, minute, second
             wirte_debug ="IGG III error";
             writeQdebug2Txt(wirte_debug);
    //         wirte_debug="CrossE_current_Sat 11111";
    //         writeQdebug2Txt(wirte_debug);
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
        trans_para->Helmert_flag = true;
        trans_para->state_Fi = temp_Fi ;
        trans_para->Nois_Q = temp_Q;
        trans_para->Covar_P = temp_P;
        trans_para->State_X = temp_X;
        system_num(current_Sat);
        Availa_SysFlag(opt_corr->m_sys_str,trans_para->sys_len,&trans_para->sys_num);
        filter_alogrithm(preEpoch,current_Sat,Total_sat_fre,opt_corr,trans_para);
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
void kalman_filter::filter_alogrithm(QVector< SatlitData > &preEpoch,QVector< SatlitData > &current_Sat,
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
//    QString wirte_debug="Obtaining_IF_equation 0000";
//    writeQdebug2Txt(wirte_debug);
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
//    wirte_debug="isSatelliteChange 0000";
//        writeQdebug2Txt(wirte_debug);

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

    if(trans_para->Helmert_flag)
        helmert_cate(current_Sat,B,L,trans_para->State_X,opt_corr);
    //Update Rk_1（There is no change in the number of satellites）
    switch (opt_corr->Combination_mode) {
    case Comb_IF_mode:
        if(!trans_para->Helmert_flag)
            update_IF_Rk(current_Sat,B.rows(),opt_corr,trans_para);
        else
            update_helmet_IF_Rk(current_Sat,B.rows(),opt_corr,trans_para);
        break;
    case Comb_UD_mode:
        update_NoCombination_Rk(current_Sat,B.rows(),opt_corr,trans_para);
        break;
    default:
        break;
    }

//    wirte_debug="update_IF_Rk 0000";
//    writeQdebug2Txt(wirte_debug);
    oldPrnFlag.clear();
    NewPrnFlag.clear();

    //Version Kalman filter
    KalmanforStatic(B,L,trans_para->state_Fi,trans_para->Nois_Q,trans_para->Covar_Rk,trans_para->State_X,trans_para->Covar_P,opt_corr,trans_para);

}

void kalman_filter::Classical_gross_error_detection(QVector< SatlitData > &current_Sat,QVector< SatlitData > &CrossE_current_Sat,int *Total_sat_fre,const corr_opt *opt_corr,para_trans *trans_para)
{
    int Avai_satN = current_Sat.length();
    int column_pos=0;
    double max_poster_Lv = 0,max_poster_Pv = 0;
    int max_n_Pv = -1,max_n_Lv = -1;
    double L_threshold=0.06,C_threshold=6.0;
    for(int i=0;i<Avai_satN;i++)
    {
        SatlitData GroEro_det = current_Sat.at(i);
        int GrossE_flag = 0;
        double poster_Lv=0.0,poster_Pv=0.0;
        switch (opt_corr->Combination_mode) {
        case Comb_IF_mode:
            poster_Lv = fabs(trans_para->posterior_V(2*i));
            poster_Pv = fabs(trans_para->posterior_V(2*i+1));
            if(poster_Lv>L_threshold||poster_Pv>C_threshold)
            {
                if(poster_Pv>max_poster_Pv)
                {
                    max_poster_Pv = poster_Pv;
                    max_n_Pv = i;
                }
                if(poster_Lv>max_poster_Lv)
                {
                    max_poster_Lv = poster_Lv;
                    max_n_Lv = i;
                }
                GrossE_flag=-1;
                quality_control_flag = false;
            }
            break;
        case Comb_UD_mode:
            for(int mufre=0;mufre<5;mufre++)
            {
                if(GroEro_det.ObsFreflag[mufre]!=1)  continue;
                poster_Lv = fabs(trans_para->posterior_V(column_pos*2));
                poster_Pv = fabs(trans_para->posterior_V(column_pos*2+1));
                if(poster_Lv>L_threshold||poster_Pv>C_threshold)
                {
                    if(poster_Pv>max_poster_Pv)
                    {
                        max_poster_Pv = poster_Pv;
                        max_n_Pv = i;
                    }
                    if(poster_Lv>max_poster_Lv)
                    {
                        max_poster_Lv = poster_Lv;
                        max_n_Lv = i;
                    }
                    GrossE_flag=-1;
                    quality_control_flag = false;
                }
                column_pos++;
            }
            break;
        default:
            break;
        }

    }
    if(!quality_control_flag)
    {
        for(int i=0;i<Avai_satN;i++)
        {
            SatlitData GroEro_det = current_Sat.at(i);
            if(i!=max_n_Pv||i!=max_n_Lv)
                CrossE_current_Sat.append(GroEro_det);
            else
            {
                if(opt_corr->Combination_mode == Comb_UD_mode)
                    *Total_sat_fre = *Total_sat_fre - GroEro_det.ObsLenfre;
                QString errorline;
                ErrorMsg(errorline);
                GroEro_det.badMsg.append("Classical gross error detection 3 cigma: poster_Pv " + QString::number(max_poster_Pv,'f',2)
                                         +" poster_Lv " + QString::number(max_poster_Lv,'f',2) +errorline);
                delet_BadSat.append(GroEro_det);
            }
        }
    }

}

void kalman_filter::helmert_cate(QVector< SatlitData > &current_Sat,MatrixXd Bk,VectorXd Lk,
                                 VectorXd &tXk_1,const corr_opt *opt_corr)
{
    int Avai_satN = current_Sat.length();
    double Variance_cate[Avai_satN];int Variance_sort[Avai_satN];
    int Vc_sort[Avai_satN];
    int VC_flag = 0;QString satellite_type;int X_len = tXk_1.size();
    VectorXd sita_x;MatrixXd wight_P;
    VectorXd state_X;MatrixXd coef_Nx;
    VectorXd conver_Wx;VectorXd weight_;

    wight_P.resize(Avai_satN*2,Avai_satN*2);
    wight_P.setIdentity(Avai_satN*2,Avai_satN*2);
    coef_Nx.resize(X_len,X_len);
    coef_Nx.setZero(X_len,X_len);
    weight_.resize(Avai_satN*2);weight_.setZero(Avai_satN*2);
    state_X = tXk_1;
    MatrixXd B_k;VectorXd L_k;
    B_k.resize(Avai_satN*2,X_len);
    B_k.setZero(Avai_satN*2,X_len);
    L_k.resize(Avai_satN*2);L_k.resize(Avai_satN*2);
    for(int i=0;i<Avai_satN;i++)
    {
        SatlitData GroEro_det = current_Sat.at(i);
        Variance_cate[i] = GroEro_det.SatWight;
        if(i==0)
        {
            satellite_type.append(GroEro_det.SatType);
            Variance_sort[i] = VC_flag;
            Vc_sort[VC_flag] = VC_flag;
            VC_flag++;
        }
        else
        {
            bool Vc_fla = false;
            for(int j=0;j<VC_flag;j++)
            {
                QChar SatType = satellite_type.at(j);
                if(SatType==GroEro_det.SatType)
                {
                    Variance_sort[i] = Vc_sort[j];
                    Vc_fla = true;
                    break;
                }
            }
            if(!Vc_fla)
            {
                satellite_type.append(GroEro_det.SatType);
                Variance_sort[i] = VC_flag;
                Vc_sort[VC_flag] = VC_flag;
                VC_flag++;
            }
        }
    }
    sita_x.resize(VC_flag);sita_x.setZero(VC_flag);
    int Variance_number[VC_flag];
    for(int i=0;i<VC_flag;i++)
    {
        int number_sat=0;
        for(int j=0;j<Avai_satN;j++)
        {
            if(Variance_sort[j]==i)
                number_sat++;
        }
        Variance_number[i] = number_sat;
    }
    Helmert_mat *Helmert_est = AGNSS_APPData::sGetAPPDataInstance()->mGetHelmert_matData();
    Helmert_mat Helmert_estimate[VC_flag];
    for(int i=0;i<VC_flag;i++)
    {
        Helmert_estimate[i].obs_Lx = Helmert_est->obs_Lx;
        Helmert_estimate[i].resi_vx = Helmert_est->resi_vx;
        Helmert_estimate[i].coef_Nx = Helmert_est->coef_Nx;
        Helmert_estimate[i].weight_Px = Helmert_est->weight_Px;
        Helmert_estimate[i].coef_Bx = Helmert_est->coef_Bx;
        Helmert_estimate[i].conver_Wx = Helmert_est->conver_Wx;
    }

    for(int i=0;i<VC_flag;i++)
    {
        int obs_len = Variance_number[i];
        Helmert_estimate[i].coef_Bx.resize(obs_len*2,X_len);
        Helmert_estimate[i].coef_Bx.setZero(obs_len*2,X_len);
        Helmert_estimate[i].weight_Px.resize(obs_len*2,obs_len*2);
        Helmert_estimate[i].weight_Px.setIdentity(obs_len*2,obs_len*2);
        Helmert_estimate[i].conver_Wx.resize(obs_len*2);
        Helmert_estimate[i].conver_Wx.setZero(obs_len*2);
        Helmert_estimate[i].coef_Nx.resize(X_len,X_len);
        Helmert_estimate[i].coef_Nx.setZero(X_len,X_len);
        Helmert_estimate[i].obs_Lx.resize(obs_len*2);
        Helmert_estimate[i].obs_Lx.setZero(obs_len*2);
        Helmert_estimate[i].resi_vx.resize(obs_len*2);
        Helmert_estimate[i].resi_vx.setZero(obs_len*2);
        int number_sat=0;
        for(int j=0;j<Avai_satN;j++)
        {
            if(Variance_sort[j]==i)
            {
                SatlitData GroEro_det = current_Sat.at(j);
                Helmert_estimate[i].coef_Bx.block(number_sat*2,0,1,X_len) = Bk.block(j*2,0,1,X_len);
                Helmert_estimate[i].coef_Bx.block(number_sat*2+1,0,1,X_len) = Bk.block(j*2+1,0,1,X_len);
                Helmert_estimate[i].obs_Lx(number_sat*2) = Lk(j*2);
                Helmert_estimate[i].obs_Lx(number_sat*2+1) = Lk(j*2+1);
                Helmert_estimate[i].weight_Px(number_sat*2) = Variance_cate[j]*GroEro_det.SatWight_L*GroEro_det.SatWight_IF[0];
                Helmert_estimate[i].weight_Px(number_sat*2+1) = Variance_cate[j]*GroEro_det.SatWight_IF[1];
                number_sat++;
            }
        }
        sita_x(i) = 1.0;
    }

    int n_len=0;
    int number_sat=0;
    for(int i=0;i<VC_flag;i++)
    {
        n_len = Variance_number[i]*2;
        B_k.block(number_sat,0,n_len,X_len) = Helmert_estimate[i].coef_Bx;
        wight_P.block(number_sat,number_sat,n_len,n_len) = Helmert_estimate[i].weight_Px;
        for(int j=0;j<n_len;j++)
            L_k(number_sat+j)=Helmert_estimate[i].obs_Lx(j);
        number_sat = number_sat +n_len;
    }
    bool frist_flag =true,H_flag=true;double wight_flag[VC_flag];
    for(int i=0;i<VC_flag;i++)  wight_flag[i]=1.0;
    while(H_flag)
    {
        double sita_N[VC_flag];int mat_num=0;
        for(int i=0;i<VC_flag;i++)
        {
            sita_N[i] = 1.0/sita_x(i);
            wight_flag[i] = sita_N[i]*wight_flag[i];
            Helmert_estimate[i].weight_Px = sita_N[i] * Helmert_estimate[i].weight_Px;
            int obs_len = Variance_number[i]*2;
            wight_P.block(mat_num,mat_num,obs_len,obs_len)= Helmert_estimate[i].weight_Px;
            mat_num = mat_num+obs_len;
            Helmert_estimate[i].coef_Nx = Helmert_estimate[i].coef_Bx.transpose() * Helmert_estimate[i].weight_Px*Helmert_estimate[i].coef_Bx;
            Helmert_estimate[i].conver_Wx = Helmert_estimate[i].coef_Bx.transpose() * Helmert_estimate[i].weight_Px*Helmert_estimate[i].obs_Lx;
        }
        coef_Nx = B_k.transpose() *wight_P*B_k;
        conver_Wx = B_k.transpose() *wight_P*L_k;
        if(!frist_flag)
        {
            state_X = coef_Nx.inverse()*conver_Wx;
        }
        frist_flag = false;
        VectorXd W;MatrixXd S;
        W.resize(VC_flag);
        S.resize(VC_flag,VC_flag);
        S.setZero(VC_flag,VC_flag);
        for(int i=0;i<VC_flag;i++)
        {
            Helmert_estimate[i].resi_vx = Helmert_estimate[i].obs_Lx - Helmert_estimate[i].coef_Bx * state_X;
            W(i) = Helmert_estimate[i].resi_vx.transpose() * Helmert_estimate[i].weight_Px*Helmert_estimate[i].resi_vx;
        }
        for(int i=0;i<VC_flag;i++)
        {
            int n_len = Variance_number[i]*2;
            for(int j=0;j<VC_flag;j++)
            {
                if(i==j)
                    S(i,j) = n_len - 2*(coef_Nx.inverse()*Helmert_estimate[i].coef_Nx).trace()
                            +(coef_Nx.inverse()*Helmert_estimate[i].coef_Nx*coef_Nx.inverse()*Helmert_estimate[i].coef_Nx).trace();
                else
                {
                    S(i,j) = (coef_Nx.inverse()*Helmert_estimate[i].coef_Nx*coef_Nx.inverse()*Helmert_estimate[j].coef_Nx).trace();
                    S(j,i) = (coef_Nx.inverse()*Helmert_estimate[i].coef_Nx*coef_Nx.inverse()*Helmert_estimate[j].coef_Nx).trace();
                }
            }
        }
        sita_x = S.inverse() * W;
        double max_sita=0,min_sita = 100;
        for(int i=0;i<VC_flag;i++)
        {
            if(i==0)
            {
                min_sita = sita_x(i);
                max_sita = sita_x(i);
            }
            if(sita_x(i)<min_sita)
                min_sita = sita_x(i);
            if(sita_x(i)>max_sita)
                max_sita = sita_x(i);
        }
        if(fabs(1-fabs(min_sita/max_sita))<0.01)
            break;
    }
    QVector< SatlitData > TransCurrent;
    for(int i=0;i<Avai_satN;i++)
    {
        for(int j=0;j<VC_flag;j++)
        {
            if(Variance_sort[i]==j)
            {
                SatlitData GroEro_det = current_Sat.at(i);
                GroEro_det.Helmert_Wight_L = wight_flag[j];
                GroEro_det.Helmert_Wight_P = wight_flag[j];
                TransCurrent.append(GroEro_det);
            }
        }
    }
    current_Sat = TransCurrent;


}
void kalman_filter::pos_WL_constraint_calculating(QVector< SatlitData > &preEpoch,QVector< SatlitData > &current_Sat,
                                    double *m_ApproxRecPos,VectorXd &X,MatrixXd &P,int Total_sat_fre,const corr_opt *opt_corr,para_trans *trans_para)
{
    for(int i=0;i<3;i++)
    {
        trans_para->m_SPP_Pos[i] = m_ApproxRecPos[i];
        trans_para->State_X(i) = 0;
    }
    //save filter sate for Quality Control
    MatrixXd temp_Fi = trans_para->state_Fi, temp_Q = trans_para->Nois_Q,
             temp_P = trans_para->Covar_P;
    MatrixXd pre_P,pre_Q,pre_Fi;
    VectorXd temp_X = trans_para->State_X;VectorXd pre_X,pre_sys_len,temp_sys_len;
    int pre_sys_num,temp_sys_num;
    quality_control_flag = opt_corr->quality_flag;
    trans_para->cross_N=0;
    temp_sys_num = trans_para->sys_num;temp_sys_len = trans_para->sys_len;
    bool Helmert_f = false;
    if(opt_corr->Helmert_flag)
    {
        Helmert_f = true;
        trans_para->Helmert_flag = false;
    }
    trans_para->fixed_WL_number=fixed_WL_ambiguity_N(current_Sat);
    filter_WL_constraint_alogrithm(preEpoch,current_Sat,Total_sat_fre,opt_corr,trans_para);
    pre_X = trans_para->State_X;pre_P = trans_para->Covar_P;pre_Q = trans_para->Nois_Q;pre_Fi = trans_para->state_Fi;
    pre_sys_num = trans_para->sys_num;pre_sys_len = trans_para->sys_len;
    int pre_const_parm = trans_para->const_parm;
    QVector< SatlitData > CrossE_current_Sat;
    QVector< SatlitData > CrossE;
    CrossE = current_Sat;


    while(!quality_control_flag&&CrossE.length()-trans_para->const_parm>=2&&trans_para->cross_N<6)
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
        trans_para->state_Fi = temp_Fi ;
        trans_para->Nois_Q = temp_Q;
        trans_para->Covar_P = temp_P;
        trans_para->State_X = temp_X;
        system_num(CrossE_current_Sat);
        Availa_SysFlag(opt_corr->m_sys_str,trans_para->sys_len,&trans_para->sys_num);
        trans_para->fixed_WL_number=fixed_WL_ambiguity_N(CrossE_current_Sat);
        if(CrossE_current_Sat.length()>=trans_para->const_parm+1)
            filter_WL_constraint_alogrithm(preEpoch,CrossE_current_Sat,Total_sat_fre,opt_corr,trans_para);
        CrossE = CrossE_current_Sat;
    }
    if(!opt_corr->quality_flag)
    {
        if((current_Sat.length() - CrossE_current_Sat.length()>3||CrossE_current_Sat.length()<trans_para->const_parm+1))
        {
             trans_para->State_X = pre_X;
             trans_para->Covar_P = pre_P;
             trans_para->Nois_Q = pre_Q;
             trans_para->state_Fi = pre_Fi;
             trans_para->sys_num = pre_sys_num;
             trans_para->sys_len = pre_sys_len;
             trans_para->const_parm=pre_const_parm;
             //Output year, month, day, hour, minute, second

        }
        else
        {
            current_Sat = CrossE_current_Sat;
        }
    }
    if(Helmert_f)
    {
        trans_para->Helmert_flag = true;
        trans_para->state_Fi = temp_Fi ;
        trans_para->Nois_Q = temp_Q;
        trans_para->Covar_P = temp_P;
        trans_para->State_X = temp_X;
        system_num(current_Sat);
        Availa_SysFlag(opt_corr->m_sys_str,trans_para->sys_len,&trans_para->sys_num);
        filter_alogrithm(preEpoch,current_Sat,Total_sat_fre,opt_corr,trans_para);
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
void kalman_filter::filter_WL_constraint_alogrithm(QVector< SatlitData > &preEpoch,QVector< SatlitData > &current_Sat,
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
    QVector< int > oldPrnFlag;//Compared with the location of the same satellite in the previous epoch, it is not found with -1
    bool isNewSatlite = false;
    QVector< int > NewPrnFlag;
    //////////qDebug()<<"Kalman 705";
    if(opt_corr->frequency_model==mixed_frequency&& preEpochLen != 0)
    {
       change_mixed_fre_Combination(preEpoch,current_Sat,oldPrnFlag,NewPrnFlag,Total_sat_fre,opt_corr,trans_para);

//        isNewSatlite = isSatelliteChange(preEpoch, current_Sat, oldPrnFlag,NewPrnFlag);
//        ////////qDebug()<<"Kalman 711";
//        //Change filter parameters
//        if (((preEpochLen != Avai_satN) || isNewSatlite ) && preEpochLen != 0)
//        {
//            switch (opt_corr->Combination_mode) {
//            case Comb_IF_mode:
//                change_IF_Paramatrix(preEpoch,current_Sat,oldPrnFlag,preEpochLen,NewPrnFlag,opt_corr,trans_para);
//                break;
//            case Comb_UD_mode:
//                change_No_WL_Combination_Para(preEpoch,current_Sat,oldPrnFlag,preEpochLen,NewPrnFlag,Total_sat_fre,opt_corr,trans_para);
//                break;
//            default:
//                break;
//            }
//        }
//        //////////qDebug()<<"Kalman 726";
    }
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
    //////////qDebug()<<"Kalman 762";
    //////////qDebug()<<"B"<<B.rows()<<"B"<<B.cols()<<"X"<<trans_para->State_X.rows()<<"P"<<trans_para->Covar_P.rows();
    //Version Kalman filter
    KalmanforStatic(B,L,trans_para->state_Fi,trans_para->Nois_Q,trans_para->Covar_Rk,trans_para->State_X,trans_para->Covar_P,opt_corr,trans_para);
}
