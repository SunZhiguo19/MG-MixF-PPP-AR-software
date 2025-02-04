#include "stochastic_model.h"

stochastic_model::stochastic_model()
{

}
void stochastic_model::sine_Wight(SatlitData &tempSatlitData)
{
    corr_opt *opt_corr = AGNSS_APPData::sGetAPPDataInstance()->mGetopt_corrData();
    double E = tempSatlitData.EA[0]*MM_PI/180;
    double SatWight=1,SatWight_L=1;//Set the weight of the satellite
    switch (tempSatlitData.SatType) {
    case 'G':
        SatWight =  qSin(E)*qSin(E) /( SQR(opt_corr->G_code_whihet) + SQR(opt_corr->G_code_whihet)*qSin(E)*qSin(E));
        SatWight_L = opt_corr->G_whihet_diff;
        break;
    case 'R':
        SatWight =  qSin(E)*qSin(E) /( SQR(opt_corr->R_code_whihet) + SQR(opt_corr->R_code_whihet)*qSin(E)*qSin(E));
        SatWight_L = opt_corr->R_whihet_diff;
        break;
    case 'C':
    {
        if(tempSatlitData.PRN<=18)
        {
            SatWight =  qSin(E)*qSin(E) /( SQR(opt_corr->C2_code_whihet) + SQR(opt_corr->C2_code_whihet)*qSin(E)*qSin(E));
            SatWight_L = opt_corr->C2_whihet_diff;
        }
        else
        {
            SatWight =  qSin(E)*qSin(E) /( SQR(opt_corr->C3_code_whihet) + SQR(opt_corr->C3_code_whihet)*qSin(E)*qSin(E));
            SatWight_L = opt_corr->C3_whihet_diff;
        }
        break;
    }
    case 'E':
        SatWight =  qSin(E)*qSin(E) /( SQR(opt_corr->E_code_whihet) + SQR(opt_corr->E_code_whihet)*qSin(E)*qSin(E));
        SatWight_L = opt_corr->E_whihet_diff;
        break;//0.3*SatWight
    default:
        break;
    }
    //Five satellites with poor GEO orbit in front of Beidou
    if(tempSatlitData.SatType == 'C' && (tempSatlitData.PRN <= 5||tempSatlitData.PRN >= 59))
    {
        SatWight = 0.01*SatWight;SatWight_L = 0.01*SatWight_L;
    }
    tempSatlitData.SatWight = SatWight;//Set the weight of the satellite  debug
    tempSatlitData.SatWight_L = SatWight_L;//Set the weight of the satellite  debug
    tempSatlitData.WL_weight = opt_corr->wl_fixed_weight;
}
void stochastic_model::cosine_Wight(SatlitData &tempSatlitData)
{
    corr_opt *opt_corr = AGNSS_APPData::sGetAPPDataInstance()->mGetopt_corrData();
    double E = tempSatlitData.EA[0]*MM_PI/180;
    double SatWight=1,SatWight_L=1;//Set the weight of the satellite
    switch (tempSatlitData.SatType) {
    case 'G':
        SatWight =  1/( SQR(opt_corr->G_code_whihet) + SQR(opt_corr->G_code_whihet)*qCos(E)*qCos(E));
        SatWight_L = opt_corr->G_whihet_diff;
        break;
    case 'R':
        SatWight =  1/( SQR(opt_corr->R_code_whihet) + SQR(opt_corr->R_code_whihet)*qCos(E)*qCos(E));
        SatWight_L = opt_corr->R_whihet_diff;
        break;
    case 'C':
    {
        if(tempSatlitData.PRN<=18)
        {
            SatWight =  1/( SQR(opt_corr->C2_code_whihet) + SQR(opt_corr->C2_code_whihet)*qCos(E)*qCos(E));
            SatWight_L = opt_corr->C2_whihet_diff;
        }
        else
        {
            SatWight =  1/( SQR(opt_corr->C3_code_whihet) + SQR(opt_corr->C3_code_whihet)*qCos(E)*qCos(E));
            SatWight_L = opt_corr->C3_whihet_diff;
        }
        break;
    }
    case 'E':
        SatWight =  1/( SQR(opt_corr->E_code_whihet) + SQR(opt_corr->E_code_whihet)*qCos(E)*qCos(E));
        SatWight_L = opt_corr->E_whihet_diff;
        break;//0.3*SatWight
    default:
        break;
    }
    //Five satellites with poor GEO orbit in front of Beidou
    if(tempSatlitData.SatType == 'C' && (tempSatlitData.PRN <= 5||tempSatlitData.PRN >= 59))
    {
        SatWight = 0.01*SatWight;SatWight_L = 0.01*SatWight_L;
    }
    tempSatlitData.SatWight = SatWight;//Set the weight of the satellite  debug
    tempSatlitData.SatWight_L = SatWight_L;//Set the weight of the satellite  debug
    tempSatlitData.WL_weight = opt_corr->wl_fixed_weight;
}
void stochastic_model::sine1_Wight(SatlitData &tempSatlitData)
{
    corr_opt *opt_corr = AGNSS_APPData::sGetAPPDataInstance()->mGetopt_corrData();
    double E = tempSatlitData.EA[0]*MM_PI/180;
    double SatWight=1,SatWight_L=1 ;//Set the weight of the satellite
    switch (tempSatlitData.SatType) {
    case 'G':
        SatWight =  qSin(E)*qSin(E) /SQR(opt_corr->G_code_whihet);
        SatWight_L = opt_corr->G_whihet_diff;
        break;
    case 'R':
        SatWight =  qSin(E)*qSin(E) /SQR(opt_corr->R_code_whihet);
        SatWight_L = opt_corr->R_whihet_diff;
        break;
    case 'C':
    {
        if(tempSatlitData.PRN<=18)
        {
            SatWight =  qSin(E)*qSin(E) /SQR(opt_corr->C2_code_whihet);
            SatWight_L = opt_corr->C2_whihet_diff;
        }
        else
        {
            SatWight =  qSin(E)*qSin(E) /SQR(opt_corr->C3_code_whihet);
            SatWight_L = opt_corr->C3_whihet_diff;
        }
        break;
    }
    case 'E':
        SatWight =  qSin(E)*qSin(E) /SQR(opt_corr->E_code_whihet);
        SatWight_L = opt_corr->E_whihet_diff;
        break;//0.3*SatWight
    default:
        break;
    }
    //Five satellites with poor GEO orbit in front of Beidou
    if(tempSatlitData.SatType == 'C' && (tempSatlitData.PRN <= 5||tempSatlitData.PRN >= 59))
    {
        SatWight = 0.01*SatWight;SatWight_L = 0.01*SatWight_L;
    }
    tempSatlitData.SatWight = SatWight;//Set the weight of the satellite  debug
    tempSatlitData.SatWight_L = SatWight_L;//Set the weight of the satellite  debug
    tempSatlitData.WL_weight = opt_corr->wl_fixed_weight;
}
void stochastic_model::panda_Wight(SatlitData &tempSatlitData)
{
    corr_opt *opt_corr = AGNSS_APPData::sGetAPPDataInstance()->mGetopt_corrData();
    double E = tempSatlitData.EA[0]*MM_PI/180;
    double SatWight=1,SatWight_L=1,threshold_E =MM_PI/6;

    switch (tempSatlitData.SatType) {
    case 'G':
        if(E>=threshold_E)
            SatWight = 1/SQR(opt_corr->G_code_whihet);
        else
            SatWight = 4*qSin(E)*qSin(E)/ SQR(opt_corr->G_code_whihet);//Set the weight of the satellite
        SatWight_L = opt_corr->G_whihet_diff;
        break;
    case 'R':
        if(E>=threshold_E)
            SatWight = 1/SQR(opt_corr->R_code_whihet);
        else
            SatWight = 4*qSin(E)*qSin(E)/ SQR(opt_corr->R_code_whihet);//Set the weight of the satellite
        SatWight_L = opt_corr->R_whihet_diff;
        break;
    case 'C':
    {
        if(tempSatlitData.PRN<=18)
        {
            if(E>=threshold_E)
                SatWight = 1/SQR(opt_corr->C2_code_whihet);
            else
                SatWight = 4*qSin(E)*qSin(E)/ SQR(opt_corr->C2_code_whihet);//Set the weight of the satellite
            SatWight_L = opt_corr->C2_whihet_diff;
        }
        else
        {
            if(E>=threshold_E)
                SatWight = 1/SQR(opt_corr->C3_code_whihet);
            else
                SatWight = 4*qSin(E)*qSin(E)/ SQR(opt_corr->C3_code_whihet);//Set the weight of the satellite
            SatWight_L = opt_corr->C3_whihet_diff;
        }
        break;
    }
    case 'E':
        if(E>=threshold_E)
            SatWight = 1/SQR(opt_corr->E_code_whihet);
        else
            SatWight = 4*qSin(E)*qSin(E)/ SQR(opt_corr->E_code_whihet);//Set the weight of the satellite
        SatWight_L = opt_corr->E_whihet_diff;
        break;//0.3*SatWight
    default:
        break;
    }
    //Five satellites with poor GEO orbit in front of Beidou
    if(tempSatlitData.SatType == 'C' && (tempSatlitData.PRN <= 5||tempSatlitData.PRN >= 59))
    {
        SatWight = 0.01*SatWight;SatWight_L = 0.01*SatWight_L;
    }
    tempSatlitData.SatWight = SatWight;//Set the weight of the satellite  debug
    tempSatlitData.SatWight_L = SatWight_L;//Set the weight of the satellite  debug
    tempSatlitData.WL_weight = opt_corr->wl_fixed_weight;
}
