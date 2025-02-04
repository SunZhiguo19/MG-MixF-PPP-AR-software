#include "AGNSS_APPData.h"
AGNSS_APPData *AGNSS_APPData::m_APPData_instance = nullptr;

AGNSS_APPData::AGNSS_APPData()
{
    m_nav_fun_run=false;
}
AGNSS_APPData::~AGNSS_APPData()
{
    if (m_APPData_instance != nullptr)
    {
        delete m_APPData_instance;
        m_APPData_instance = nullptr;
    }
}
/**
 * @brief AGNSS_APPData::sGetAppInstance    获取App单例类指针
 * @return  App单例类指针
 */
AGNSS_APPData *AGNSS_APPData::sGetAPPDataInstance()
{
    // 创建App单例类
    if (m_APPData_instance == nullptr)
    {
        m_APPData_instance = new AGNSS_APPData;
    }
    return m_APPData_instance;
}
/**
 * @brief *AGNSS_APPData::mGetopt_corrData()    获取corr_opt配置数据
 * @return
 */
corr_opt *AGNSS_APPData::mGetopt_corrData()
{
    return &opt_corr;
}
/**
 * @brief *AGNSS_APPData::mGetGNSS_PAR_autoData()    获取corr_opt配置数据
 * @return
 */
GNSS_PAR_auto *AGNSS_APPData::mGetGNSS_PAR_autoData()
{
    return &auto_PAR;
}
/**
 * @brief *AGNSS_APPData::mGetGNSS_PAR_autoData()    获取Ambiguity_mat配置数据
 * @return
 */
Ambiguity_mat *AGNSS_APPData::mGetAmbiguity_matData()
{
    return &A_mat;
}
/**
 * @brief *AGNSS_APPData::mGetGNSS_PAR_autoData()    获取Ambiguity_mat配置数据
 * @return
 */
Helmert_mat *AGNSS_APPData::mGetHelmert_matData()
{
    return &Helmert_estimate;
}
/**
 * @brief *AGNSS_APPData::mGetopt_corrData()    获取corr_opt配置数据
 * @return
 */
para_trans *AGNSS_APPData::mGettrans_paraData()
{
    return &trans_para;
}
/**
 * @brief *AGNSS_APPData::mGettrans_pravide_obs()    obs_pravide
 * @return
 */
obs_pravide *AGNSS_APPData::mGettrans_pravide_obs()
{
    return &pravide_obs;
}
/**
 * @brief *AGNSS_APPData::mGetopt_corrData()    获取corr_opt配置数据
 * @return
 */
RTwriteFile *AGNSS_APPData::mGetRTwriteData()
{
    return &RTwritefile;
}
/**
 * @brief *AGNSS_APPData::mGetopt_corrData()    获取corr_opt配置数据
 * @return
 */
disply_satellite *AGNSS_APPData::mGettrans_sate_dispData()
{
    return &sate_disp;
}
/**
 * @brief *AGNSS_APPData::mGetopt_corrData()    获取corr_opt配置数据
 * @return
 */
disply_textEdit *AGNSS_APPData::mGettrans_textEdit()
{
    return &text_disp;
}
/**
 * @brief *AGNSS_APPData::mGetopt_corrData()    获取dat_fil配置数据
 * @return
 */
file_dat *AGNSS_APPData::mGetdat_filData()
{
    return &dat_fil;
}
/**
 * @brief AGNSS_APPData::mGetAppFunState   获取app功能运行状态
 * @return
 */
bool AGNSS_APPData::mGetAppFunState()
{
    return m_nav_fun_run;
}
/**
 * @brief file_dat::mSetAppFunState   设置app功能运行状态
 * @param state
 */
void AGNSS_APPData::mSetAppFunState(bool state)
{
    m_nav_fun_run = state;
}
