#ifndef AGNSS_APPDATA_H
#define AGNSS_APPDATA_H
#include "pos_algorith.h"

class AGNSS_APPData
{
public:
    AGNSS_APPData();
    ~AGNSS_APPData();
    para_trans *mGettrans_paraData();
    corr_opt *mGetopt_corrData();
    Helmert_mat *mGetHelmert_matData();
    GNSS_PAR_auto *mGetGNSS_PAR_autoData();
    Ambiguity_mat *mGetAmbiguity_matData();
    file_dat *mGetdat_filData();
    RTwriteFile *mGetRTwriteData();
    disply_satellite *mGettrans_sate_dispData();
    obs_pravide *mGettrans_pravide_obs();
    bool mGetAppFunState();
    static AGNSS_APPData *sGetAPPDataInstance();
    disply_textEdit *mGettrans_textEdit();
    void mSetAppFunState(bool state);
private:
    static AGNSS_APPData *m_APPData_instance;        // App单例类指针
    corr_opt opt_corr;
    para_trans trans_para;
    Helmert_mat Helmert_estimate;
    obs_pravide pravide_obs;
    GNSS_PAR_auto auto_PAR;
    file_dat dat_fil;
    disply_satellite sate_disp;
    RTwriteFile RTwritefile;
    disply_textEdit text_disp;
    Ambiguity_mat A_mat;
    bool m_nav_fun_run;

};

#endif // AGNSS_APPDATA_H
