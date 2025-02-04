#include "SetWidget.h"
#include "ui_SetWidget.h"

SetWidget::SetWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SetWidget),
    TranIni("MG-MixF Config.ini")
{
    ui->setupUi(this);
    init();
    save_config();
    // push select file path
    connect(ui->select_path, SIGNAL(clicked(bool)), this, SLOT(select_Path()));
    connect(ui->select_save_file_path, SIGNAL(clicked(bool)), this, SLOT(select_result_file_Path()));
    connect(ui->save_config, SIGNAL(clicked(bool)), this, SLOT(save_config()));
    connect(ui->Combination_mode,SIGNAL(currentIndexChanged(int)),this,SLOT(changeDisplay_fre()));
    connect(ui->angle_of_altitude,SIGNAL(valueChanged(int)),ui->ELEspinBox,SLOT(setValue(int)));
    connect(ui->SNR_of_min,SIGNAL(valueChanged(int)),ui->SNRspinBox_2,SLOT(setValue(int)));
    connect(ui->ELEspinBox,SIGNAL(valueChanged(int)),ui->angle_of_altitude,SLOT(setValue(int)));
    connect(ui->SNRspinBox_2,SIGNAL(valueChanged(int)),ui->SNR_of_min,SLOT(setValue(int)));
    m_autoResize_setwidget_Handler=new AutoResize(this,this->rect().width(),this->rect().height());
    m_autoResize_setwidget_Handler->setAutoResizeFlag(
            AutoResize::INCLUDE_BUTTON|AutoResize::INCLUDE_COMBOBOX|
            AutoResize::INCLUDE_EDITOR|AutoResize::INCLUDE_LABEL
            );

  //  m_autoResize_setwidget_Handler->pushAllResizeItem();
    m_autoResize_setwidget_Handler->addOtherItem(ui->groupBox_6);
    m_autoResize_setwidget_Handler->addOtherItem(ui->groupBox_5);
    m_autoResize_setwidget_Handler->addOtherItem(ui->groupBox_4);
    m_autoResize_setwidget_Handler->addOtherItem(ui->groupBox_3);
    m_autoResize_setwidget_Handler->addOtherItem(ui->groupBox_2);
    m_autoResize_setwidget_Handler->addOtherItem(ui->groupBox);
    m_autoResize_setwidget_Handler->addOtherItem(ui->groupBox_7);
    m_autoResize_setwidget_Handler->addOtherItem(ui->groupBox_8);
    m_autoResize_setwidget_Handler->addOtherItem(ui->groupBox_9);
    m_autoResize_setwidget_Handler->addOtherItem(ui->groupBox_10);
    m_autoResize_setwidget_Handler->addOtherItem(ui->groupBox_11);
    m_autoResize_setwidget_Handler->addOtherItem(ui->groupBox_12);
    m_autoResize_setwidget_Handler->addOtherItem(ui->select_frequence);

    m_autoResize_setwidget_Handler->pushAllResizeItem();

}
/**
 * @brief SetWidget::~SetWidget   析构函数
 */
SetWidget::~SetWidget()
{
    delete ui;
}
void SetWidget::resizeEvent(QResizeEvent *event)
{
    m_autoResize_setwidget_Handler->doAutoResize();
}
/**
 * @brief SetWidget::showEvent  窗口显示事件
 * @param event 显示事件
 */
void SetWidget::showEvent(QShowEvent *event)
{
    // Nav功能运行状态下，定位相关信息不可配置
    if (AGNSS_APPData::sGetAPPDataInstance()->mGetAppFunState())
    {
        this->setEnabled(false);
    }
    else
        this->setEnabled(true);
}

void SetWidget::init()
{
    //title
     setWindowTitle("SetWidget");

    // Set Parameters
        QString S_Combination_PPP, S_CutAngle ,S_SNR , S_Filename ,S_result_Filename, S_Filesave , S_UDUCFrequency , S_Partial_subset ,S_Reference_sta, S_Reinitalization_timebox , S_System ,
                S_delete , S_deletetxt, S_Reinitalization_time , S_muilt_stations , S_mult_day ,S_Combination_model,S_fre_model,S_motion_state,S_Filtering,S_data_process_state,
                S_position_method , S_quality_flag , S_robust_est,S_cycle_slip_detection,S_filter_direction,S_frequency_model,
                S_BDSIFFrequency1 ,S_BDSIFFrequency2,S_GPSIFFrequency1,S_GPSIFFrequency2,S_GLEIFFrequency1  ,S_GLEIFFrequency2,S_GLOIFFrequency1,S_GLOIFFrequency2 ,S_sys_error_select,S_GNSS_AR_select,
                S_comboBox_C_f1,S_comboBox_C_f2,S_comboBox_C_f3,S_comboBox_C_f4,S_comboBox_C_f5,
                S_comboBox_G_f1,S_comboBox_G_f2,S_comboBox_G_f3,
                S_comboBox_E_f1,S_comboBox_E_f2,S_comboBox_E_f3,S_comboBox_E_f4,S_comboBox_E_f5,
                S_comboBox_R_f1,S_comboBox_R_f2,S_comboBox_R_f3,
                S_Ambiguity_inf,S_SRC_Ps,S_WL_constraint,S_Ambiguity_acquisition;



        S_Combination_PPP = TranIni.getValue("/MG-MixF Config/Combination_PPP");
        S_CutAngle = TranIni.getValue("/MG-MixF Config/CutAngle");
        S_SNR = TranIni.getValue("/MG-MixF Config/SNR");
        S_Filename = TranIni.getValue("/MG-MixF Config/Filename");
        S_result_Filename = TranIni.getValue("/MG-MixF Config/result_Filename");
        S_Filesave = TranIni.getValue("/MG-MixF Config/Filesave");
        S_UDUCFrequency = TranIni.getValue("/MG-MixF Config/UDUCFrequency");
        S_BDSIFFrequency1 = TranIni.getValue("/MG-MixF Config/BDSIFFrequency1");
        S_BDSIFFrequency2 = TranIni.getValue("/MG-MixF Config/BDSIFFrequency2");
        S_GPSIFFrequency1 = TranIni.getValue("/MG-MixF Config/GPSIFFrequency1");
        S_GPSIFFrequency2 = TranIni.getValue("/MG-MixF Config/GPSIFFrequency2");
        S_GLEIFFrequency1 = TranIni.getValue("/MG-MixF Config/GLEIFFrequency1");
        S_GLEIFFrequency2 = TranIni.getValue("/MG-MixF Config/GLEIFFrequency2");
        S_GLOIFFrequency1 = TranIni.getValue("/MG-MixF Config/GLOIFFrequency1");
        S_GLOIFFrequency2 = TranIni.getValue("/MG-MixF Config/GLOIFFrequency2");
        S_Partial_subset = TranIni.getValue("/MG-MixF Config/Partial_subset");
        S_comboBox_C_f1 = TranIni.getValue("/MG-MixF Config/comboBox_C_f1");
        S_comboBox_C_f2 = TranIni.getValue("/MG-MixF Config/comboBox_C_f2");
        S_comboBox_C_f3 = TranIni.getValue("/MG-MixF Config/comboBox_C_f3");
        S_comboBox_C_f4 = TranIni.getValue("/MG-MixF Config/comboBox_C_f4");
        S_comboBox_C_f5 = TranIni.getValue("/MG-MixF Config/comboBox_C_f5");
        S_comboBox_G_f1 = TranIni.getValue("/MG-MixF Config/comboBox_G_f1");
        S_comboBox_G_f2 = TranIni.getValue("/MG-MixF Config/comboBox_G_f2");
        S_comboBox_G_f3 = TranIni.getValue("/MG-MixF Config/comboBox_G_f3");
        S_comboBox_E_f1 = TranIni.getValue("/MG-MixF Config/comboBox_E_f1");
        S_comboBox_E_f2 = TranIni.getValue("/MG-MixF Config/comboBox_E_f2");
        S_comboBox_E_f3 = TranIni.getValue("/MG-MixF Config/comboBox_E_f3");
        S_comboBox_E_f4 = TranIni.getValue("/MG-MixF Config/comboBox_E_f4");
        S_comboBox_E_f5 = TranIni.getValue("/MG-MixF Config/comboBox_E_f5");
        S_comboBox_R_f1 = TranIni.getValue("/MG-MixF Config/comboBox_R_f1");
        S_comboBox_R_f2 = TranIni.getValue("/MG-MixF Config/comboBox_R_f2");
        S_comboBox_R_f3 = TranIni.getValue("/MG-MixF Config/comboBox_R_f3");
        S_filter_direction = TranIni.getValue("/MG-MixF Config/filter_dir");
        S_frequency_model  = TranIni.getValue("/MG-MixF Config/frequency_model");

        S_Reference_sta = TranIni.getValue("/MG-MixF Config/Reference_sta");
        S_Reinitalization_timebox = TranIni.getValue("/MG-MixF Config/Reinitalization_timebox");

        S_GNSS_AR_select = TranIni.getValue("/MG-MixF Config/GNSS AR select");
        S_SRC_Ps = TranIni.getValue("/MG-MixF Config/SRC Ps");
        S_WL_constraint = TranIni.getValue("/MG-MixF Config/Wide lane constraint");
        S_Ambiguity_acquisition = TranIni.getValue("/MG-MixF Config/Ambiguity_acquisition");
        S_System = TranIni.getValue("/MG-MixF Config/System");
        S_delete = TranIni.getValue("/MG-MixF Config/delete");
        S_deletetxt = TranIni.getValue("/MG-MixF Config/deletetxt");
        S_Reinitalization_time = TranIni.getValue("/MG-MixF Config/Reinitalization_time");

        S_muilt_stations = TranIni.getValue("/MG-MixF Config/muilt_stations");
        S_mult_day =TranIni.getValue("/MG-MixF Config/mult_day");
        S_sys_error_select = TranIni.getValue("/MG-MixF Config/system error select");
        S_Combination_model = TranIni.getValue("/MG-MixF Config/Combination_model");
        S_fre_model = TranIni.getValue("/MG-MixF Config/fre_model");
        S_motion_state = TranIni.getValue("/MG-MixF Config/motion_state");
        S_Filtering = TranIni.getValue("/MG-MixF Config/Filtering");
        S_data_process_state = TranIni.getValue("/MG-MixF Config/data_process_state");
        S_position_method = TranIni.getValue("/MG-MixF Config/position_method");


        S_quality_flag = TranIni.getValue("/MG-MixF Config/quality_flag");
        S_robust_est = TranIni.getValue("/MG-MixF Config/robust_est");
        S_cycle_slip_detection = TranIni.getValue("/MG-MixF Config/cycle_slip_detection");
        S_Ambiguity_inf = TranIni.getValue("/MG-MixF Config/Ambioguity information");


        QStringList S_FilesaveList = S_Filesave.split(";"),
                    S_UDUCFrequencyList = S_UDUCFrequency.split(";"),
                    S_deleteList = S_delete.split(";"),
                    S_SystemList = S_System.split(";");

        S_FilesaveList.removeAll(QString("")); S_UDUCFrequencyList.removeAll(QString(""));
        S_deleteList.removeAll(QString("")); S_SystemList.removeAll(QString(""));


        if(S_Combination_PPP.length() != 0)
        {
           ui->Combination_mode->setCurrentIndex(S_Combination_PPP.toInt());
        }
        if(S_CutAngle.length() != 0)
        {
           ui->ELEspinBox->setValue(S_CutAngle.toInt());
        }
        if(S_SNR.length() != 0)
        {
           ui->SNRspinBox_2->setValue(S_SNR.toInt());
        }
        if(S_Filename.length() != 0)
        {
           ui->observation_file_path->setText(S_Filename);
        }
        if(S_result_Filename.length() != 0)
        {
           ui->result_file_path->setText(S_result_Filename);
        }
        if(S_Filesave.length() != 0)
        {
            if(S_FilesaveList[0]=="1")
                ui->position->setChecked(true);
            else
                ui->position->setChecked(false);
            if(S_FilesaveList[1]=="1")
                ui->Bad_satellites->setChecked(true);
            else
                ui->Bad_satellites->setChecked(false);
            if(S_FilesaveList[2]=="1")
                ui->ZTDW_Clock->setChecked(true);
            else
                ui->ZTDW_Clock->setChecked(false);
            if(S_FilesaveList[3]=="1")
                ui->Satellite_info->setChecked(true);
            else
                ui->Satellite_info->setChecked(false);

            if(S_FilesaveList[4]=="1")
                ui->dop_satN_AR->setChecked(true);
            else
                ui->dop_satN_AR->setChecked(false);
            if(S_FilesaveList[5]=="1")
                ui->Az_El_coff->setChecked(true);
            else
                ui->Az_El_coff->setChecked(false);

        }
        if(S_BDSIFFrequency1.length() != 0)
        {
            ui->BDSIF_1fre->setCurrentIndex(S_BDSIFFrequency1.toInt());
        }
        if(S_BDSIFFrequency2.length() != 0)
        {
            ui->BDSIF_2fre->setCurrentIndex(S_BDSIFFrequency2.toInt());
        }
        if(S_GPSIFFrequency1.length() != 0)
        {
            ui->GPSIF_1fre->setCurrentIndex(S_GPSIFFrequency1.toInt());
        }
        if(S_GPSIFFrequency2.length() != 0)
        {
            ui->GPSIF_2fre->setCurrentIndex(S_GPSIFFrequency2.toInt());
        }
        if(S_GLEIFFrequency1.length() != 0)
        {
            ui->GLEIF_1fre->setCurrentIndex(S_GLEIFFrequency1.toInt());
        }
        if(S_GLEIFFrequency2.length() != 0)
        {
            ui->GLEIF_2fre->setCurrentIndex(S_GLEIFFrequency2.toInt());
        }
        if(S_GLOIFFrequency1.length() != 0)
        {
            ui->GLOIF_1fre->setCurrentIndex(S_GLOIFFrequency1.toInt());
        }
        if(S_GLOIFFrequency2.length() != 0)
        {
            ui->GLOIF_2fre->setCurrentIndex(S_GLOIFFrequency2.toInt());
        }

        if(S_comboBox_C_f1.length() != 0)
        {
            ui->comboBox_C_f1->setCurrentIndex(S_comboBox_C_f1.toInt());
        }
        if(S_comboBox_C_f2.length() != 0)
        {
            ui->comboBox_C_f2->setCurrentIndex(S_comboBox_C_f2.toInt());
        }
        if(S_comboBox_C_f3.length() != 0)
        {
            ui->comboBox_C_f3->setCurrentIndex(S_comboBox_C_f3.toInt());
        }
        if(S_comboBox_C_f4.length() != 0)
        {
            ui->comboBox_C_f4->setCurrentIndex(S_comboBox_C_f4.toInt());
        }
        if(S_comboBox_C_f5.length() != 0)
        {
            ui->comboBox_C_f5->setCurrentIndex(S_comboBox_C_f5.toInt());
        }
        if(S_comboBox_G_f1.length() != 0)
        {
            ui->comboBox_G_f1->setCurrentIndex(S_comboBox_G_f1.toInt());
        }
        if(S_comboBox_G_f2.length() != 0)
        {
            ui->comboBox_G_f2->setCurrentIndex(S_comboBox_G_f2.toInt());
        }
        if(S_comboBox_G_f3.length() != 0)
        {
            ui->comboBox_G_f3->setCurrentIndex(S_comboBox_G_f3.toInt());
        }
        if(S_comboBox_E_f1.length() != 0)
        {
            ui->comboBox_E_f1->setCurrentIndex(S_comboBox_E_f1.toInt());
        }
        if(S_comboBox_E_f2.length() != 0)
        {
            ui->comboBox_E_f2->setCurrentIndex(S_comboBox_E_f2.toInt());
        }
        if(S_comboBox_E_f3.length() != 0)
        {
            ui->comboBox_E_f3->setCurrentIndex(S_comboBox_E_f3.toInt());
        }
        if(S_comboBox_E_f4.length() != 0)
        {
            ui->comboBox_E_f4->setCurrentIndex(S_comboBox_E_f4.toInt());
        }
        if(S_comboBox_E_f5.length() != 0)
        {
            ui->comboBox_E_f5->setCurrentIndex(S_comboBox_E_f5.toInt());
        }
        if(S_comboBox_R_f1.length() != 0)
        {
            ui->comboBox_R_f1->setCurrentIndex(S_comboBox_R_f1.toInt());
        }
        if(S_comboBox_R_f2.length() != 0)
        {
            ui->comboBox_R_f2->setCurrentIndex(S_comboBox_R_f2.toInt());
        }
        if(S_comboBox_R_f3.length() != 0)
        {
            ui->comboBox_R_f3->setCurrentIndex(S_comboBox_R_f3.toInt());
        }
        if(S_filter_direction.length() != 0)
        {
            ui->filter_dir->setCurrentIndex(S_filter_direction.toInt());
        }
        if(S_frequency_model.length() != 0)
        {
            ui->frequency_model->setCurrentIndex(S_frequency_model.toInt());
        }
        if(S_Partial_subset.length() != 0)
        {
           ui->PAR_subset->setCurrentIndex(S_Partial_subset.toInt());
        }
        if(S_Reference_sta.length() != 0)
        {
           ui->Ref_satelite->setCurrentIndex(S_Reference_sta.toInt());
        }
        if(S_Reinitalization_timebox.length() != 0)
        {
            if(S_Reinitalization_timebox == "1")
                ui->Reinitalization_timebox->setChecked(true);
            if(S_Reinitalization_timebox == "0")
                ui->Reinitalization_timebox->setChecked(false);
        }
        if(S_GNSS_AR_select.length()!=0)
        {
            if(S_GNSS_AR_select == "1")
                ui->GNSS_AR_select->setChecked(true);
            else if(S_GNSS_AR_select == "0")
                ui->GNSS_AR_select->setChecked(false);
        }
        if(S_SRC_Ps.length()!=0)
        {
            if(S_SRC_Ps == "1")
                ui->SRC_Ps->setChecked(true);
            else if(S_SRC_Ps == "0")
                ui->SRC_Ps->setChecked(false);
        }
        if(S_WL_constraint.length()!=0)
        {
            if(S_WL_constraint == "1")
                ui->WL_constraint->setChecked(true);
            else if(S_WL_constraint == "0")
                ui->WL_constraint->setChecked(false);
        }
        if(S_Ambiguity_acquisition.length() != 0)
        {
            ui->Ambiguity_acquisition->setCurrentIndex(S_Ambiguity_acquisition.toInt());
        }
        if(S_SystemList.length() != 0)
        {
            if(S_SystemList[0] == "1")
                ui->select_GPS->setChecked(true);
            else
                ui->select_GPS->setChecked(false);


            if(S_SystemList[1] == "1")
                ui->select_GLONASS->setChecked(true);
            else
                ui->select_GLONASS->setChecked(false);

            if(S_SystemList[2] == "1")
                ui->select_Galileo->setChecked(true);
            else
                ui->select_Galileo->setChecked(false);

            if(S_SystemList[3] == "1")
                ui->select_BDS->setChecked(true);
            else
                ui->select_BDS->setChecked(false);
        }
        if(S_deleteList.length() != 0)
        {
            if(S_deleteList[0] == "1")
                ui->BDS2->setChecked(true);
            else
                ui->BDS2->setChecked(false);
            if(S_deleteList[1] == "1")
                ui->BDS3->setChecked(true);
            else
                ui->BDS3->setChecked(false);
            if(S_deleteList[2] == "1")
                ui->BDS2GEO->setChecked(true);
            else
                ui->BDS2GEO->setChecked(false);
            if(S_deleteList[3] == "1")
                ui->BDS3GEO->setChecked(true);
            else
                ui->BDS3GEO->setChecked(false);
        }
        if(S_deletetxt.length() != 0)
        {
            ui->delete_satellite->setText(S_deletetxt);
        }
        if(S_Reinitalization_time.length() != 0)
        {
            ui->Reinitalization_time->setCurrentIndex(S_Reinitalization_time.toInt());
        }
        if(S_muilt_stations.length() != 0)
        {
            if(S_muilt_stations == "1")
            ui->mult_station->setChecked(true);
            else
             ui->mult_station->setChecked(false);
        }
        if(S_mult_day.length() != 0)
        {
            if(S_mult_day == "1")
            ui->mult_day->setChecked(true);
            else
             ui->mult_day->setChecked(false);
        }
        if(S_Combination_model.length() != 0)
        {
            ui->Combination_mode->setCurrentIndex(S_Combination_model.toInt());
        }
        ui->select_frequence->setCurrentIndex(ui->Combination_mode->currentIndex());
        if(S_sys_error_select.length()!=0)
        {
            ui->sys_error_select->setCurrentIndex(S_sys_error_select.toInt());
        }
        if(S_fre_model.length() != 0&&ui->Combination_mode->currentIndex()==0)
        {
            ui->fre_mode->setCurrentIndex(S_fre_model.toInt());
        }
        else
            ui->fre_mode->setCurrentIndex(0);
        if(S_motion_state.length() != 0)
        {
            ui->motion_state->setCurrentIndex(S_motion_state.toInt());
        }
        if(S_Filtering.length() != 0)
        {
            ui->filter->setCurrentIndex(S_Filtering.toInt());
        }
        if(S_data_process_state.length() != 0)
        {
            ui->data_process_state->setCurrentIndex(S_data_process_state.toInt());
        }
        if(S_position_method.length() != 0)
        {
            ui->positioning_method->setCurrentIndex(S_position_method.toInt());
        }

        if(S_quality_flag.length() != 0)
        {
            if(S_quality_flag == "1")
               ui->Robust_filter->setChecked(false);
            else
               ui->Robust_filter->setChecked(true);
        }
        if(S_robust_est.length() != 0)
        {
            ui->robust_estimation->setCurrentIndex(S_robust_est.toInt());
        }
        if(S_cycle_slip_detection.length() != 0)
        {
            if(S_cycle_slip_detection == "1")
                ui->cycle_slip_detection->setChecked(false);
            else
                ui->cycle_slip_detection->setChecked(true);
        }
        if(S_Ambiguity_inf.length() != 0)
        {
            if(S_Ambiguity_inf == "1")
                ui->ambiguity_inf->setChecked(false);
            else
                ui->ambiguity_inf->setChecked(true);
        }


}


void SetWidget::select_Path()
{
    QFileDialog fileDialog;
    fileDialog.setFileMode(QFileDialog::Directory);
    files_path = fileDialog.getExistingDirectory(this, "Open Directory", "", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if(files_path.isEmpty())
    {
        ui->observation_file_path->setText("");
    }
    else
    {
        ui->observation_file_path->setText(files_path);
    }
}
void SetWidget::select_result_file_Path()
{
    QFileDialog fileDialog;
    fileDialog.setFileMode(QFileDialog::Directory);
    result_files_path = fileDialog.getExistingDirectory(this, "Open Directory", "", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if(result_files_path.isEmpty())
    {
        ui->result_file_path->setText("");
    }
    else
    {
        ui->result_file_path->setText(result_files_path);
    }
}


void SetWidget::save_config()
{
    //实例化
    opt_corr = AGNSS_APPData::sGetAPPDataInstance()->mGetopt_corrData();
    trans_para = AGNSS_APPData::sGetAPPDataInstance()->mGettrans_paraData();
    dat_fil = AGNSS_APPData::sGetAPPDataInstance()->mGetdat_filData();
        opt_corr->m_sys_str.clear();


// 系统选择
        //系统
        if(ui->select_GPS->isChecked())
            opt_corr->m_sys_str.append("G");
        if(ui->select_GLONASS->isChecked())
            opt_corr->m_sys_str.append("R");
        if(ui->select_Galileo->isChecked())
            opt_corr->m_sys_str.append("E");
        if(ui->select_BDS->isChecked())
            opt_corr->m_sys_str.append("C");
        if(ui->GPSIF_1fre->currentText()=="L1")
            opt_corr->IF0_combination[0]=0;
        else if(ui->GPSIF_1fre->currentText()=="L2")
            opt_corr->IF0_combination[0]=1;
        else if(ui->GPSIF_1fre->currentText()=="L5")
            opt_corr->IF0_combination[0]=2;
        if(ui->GPSIF_2fre->currentText()=="L1")
            opt_corr->IF1_combination[0]=0;
        else if(ui->GPSIF_2fre->currentText()=="L2")
            opt_corr->IF1_combination[0]=1;
        else if(ui->GPSIF_2fre->currentText()=="L5")
            opt_corr->IF1_combination[0]=2;

        if(ui->BDSIF_1fre->currentText()=="B1I")
            opt_corr->IF0_combination[1]=0;
        else if(ui->BDSIF_1fre->currentText()=="B2I")
            opt_corr->IF0_combination[1]=2;
        else if(ui->BDSIF_1fre->currentText()=="B3I")
            opt_corr->IF0_combination[1]=1;
        else if(ui->BDSIF_1fre->currentText()=="B1C")
            opt_corr->IF0_combination[1]=3;
        else if(ui->BDSIF_1fre->currentText()=="B2a")
            opt_corr->IF0_combination[1]=4;
        if(ui->BDSIF_2fre->currentText()=="B1I")
            opt_corr->IF1_combination[1]=0;
        else if(ui->BDSIF_2fre->currentText()=="B2I")
            opt_corr->IF1_combination[1]=2;
        else if(ui->BDSIF_2fre->currentText()=="B3I")
            opt_corr->IF1_combination[1]=1;
        else if(ui->BDSIF_2fre->currentText()=="B1C")
            opt_corr->IF1_combination[1]=3;
        else if(ui->BDSIF_2fre->currentText()=="B2a")
            opt_corr->IF1_combination[1]=4;
        opt_corr->IF1_combination[2] = opt_corr->IF1_combination[1];
        opt_corr->IF0_combination[2] = opt_corr->IF0_combination[1];

        if(ui->GLOIF_1fre->currentText()=="L1")
            opt_corr->IF0_combination[3]=0;
        else if(ui->GLOIF_1fre->currentText()=="L2")
            opt_corr->IF0_combination[3]=1;
        else if(ui->GLOIF_1fre->currentText()=="L3")
            opt_corr->IF0_combination[3]=2;
        if(ui->GLOIF_2fre->currentText()=="L1")
            opt_corr->IF1_combination[3]=0;
        else if(ui->GLOIF_2fre->currentText()=="L2")
            opt_corr->IF1_combination[3]=1;
        else if(ui->GLOIF_2fre->currentText()=="L3")
            opt_corr->IF1_combination[3]=2;

        if(ui->GLEIF_1fre->currentText()=="E1")
            opt_corr->IF0_combination[4]=0;
        else if(ui->GLEIF_1fre->currentText()=="E5a")
            opt_corr->IF0_combination[4]=1;
        else if(ui->GLEIF_1fre->currentText()=="E5b")
            opt_corr->IF0_combination[4]=2;
        else if(ui->GLEIF_1fre->currentText()=="E5 boc")
            opt_corr->IF0_combination[4]=3;
        else if(ui->GLEIF_1fre->currentText()=="E6")
            opt_corr->IF0_combination[4]=4;
        if(ui->GLEIF_2fre->currentText()=="E1")
            opt_corr->IF1_combination[4]=0;
        else if(ui->GLEIF_2fre->currentText()=="E5a")
            opt_corr->IF1_combination[4]=1;
        else if(ui->GLEIF_2fre->currentText()=="E5b")
            opt_corr->IF1_combination[4]=2;
        else if(ui->GLEIF_2fre->currentText()=="E5 boc")
            opt_corr->IF1_combination[4]=3;
        else if(ui->GLEIF_2fre->currentText()=="E6")
            opt_corr->IF1_combination[4]=4;


        if(ui->comboBox_C_f1->currentText()=="B1I")
            opt_corr->Frequently_BDS[0] = 1;
        else if(ui->comboBox_C_f1->currentText()=="B1C")
            opt_corr->Frequently_BDS[0] = 4;
        if(ui->comboBox_C_f2->currentText()=="B1I")
            opt_corr->Frequently_BDS[1] = 1;
        else if(ui->comboBox_C_f2->currentText()=="B2a")
            opt_corr->Frequently_BDS[1] = 5;
        else if(ui->comboBox_C_f2->currentText()=="B3I")
            opt_corr->Frequently_BDS[1] = 2;
        if(ui->comboBox_C_f3->currentText()=="OFF")
            opt_corr->Frequently_BDS[2] = 0;
        else if(ui->comboBox_C_f3->currentText()=="B2b/B2I")
            opt_corr->Frequently_BDS[2] = 3;
        else if(ui->comboBox_C_f3->currentText()=="B3I")
            opt_corr->Frequently_BDS[2] = 2;
        else if(ui->comboBox_C_f3->currentText()=="B2a")
            opt_corr->Frequently_BDS[2] = 5;
        if(ui->comboBox_C_f4->currentText()=="OFF")
            opt_corr->Frequently_BDS[3] = 0;
        else if(ui->comboBox_C_f4->currentText()=="B2b/B2I")
            opt_corr->Frequently_BDS[3] = 3;
        else if(ui->comboBox_C_f4->currentText()=="B2a")
            opt_corr->Frequently_BDS[3] = 5;
        if(ui->comboBox_C_f5->currentText()=="OFF")
            opt_corr->Frequently_BDS[4] = 0;
        else if(ui->comboBox_C_f5->currentText()=="B2a")
            opt_corr->Frequently_BDS[4] = 5;

        if(ui->comboBox_G_f1->currentText()=="L1")
            opt_corr->Frequently_GPS[0] = 1;
        else if(ui->comboBox_G_f1->currentText()=="L2")
            opt_corr->Frequently_GPS[0] = 2;
        if(ui->comboBox_G_f2->currentText()=="L2")
            opt_corr->Frequently_GPS[1] = 2;
        else if(ui->comboBox_G_f2->currentText()=="L5")
            opt_corr->Frequently_GPS[1] = 3;
        if(ui->comboBox_G_f3->currentText()=="OFF")
            opt_corr->Frequently_GPS[2] = 0;
        else if(ui->comboBox_G_f3->currentText()=="L5")
            opt_corr->Frequently_GPS[2] = 3;

        if(ui->comboBox_E_f1->currentText()=="E1")
            opt_corr->Frequently_GAL[0] = 1;
        else if(ui->comboBox_E_f1->currentText()=="E6")
            opt_corr->Frequently_GAL[0] = 3;
        if(ui->comboBox_E_f2->currentText()=="E6")
            opt_corr->Frequently_GAL[1] = 3;
        else if(ui->comboBox_E_f2->currentText()=="E5 boc")
            opt_corr->Frequently_GAL[1] = 5;
        else if(ui->comboBox_E_f2->currentText()=="E5a")
            opt_corr->Frequently_GAL[1] = 2;
        else if(ui->comboBox_E_f2->currentText()=="E5b")
            opt_corr->Frequently_GAL[1] = 4;
        if(ui->comboBox_E_f3->currentText()=="OFF")
            opt_corr->Frequently_GAL[2] = 0;
        else if(ui->comboBox_E_f3->currentText()=="E5a")
            opt_corr->Frequently_GAL[2] = 2;
        else if(ui->comboBox_E_f3->currentText()=="E5b")
            opt_corr->Frequently_GAL[2] = 4;
        else if(ui->comboBox_E_f3->currentText()=="E5 boc")
            opt_corr->Frequently_GAL[2] = 5;
        if(ui->comboBox_E_f4->currentText()=="OFF")
            opt_corr->Frequently_GAL[3] = 0;
        else if(ui->comboBox_E_f4->currentText()=="E5b")
            opt_corr->Frequently_GAL[3] = 4;
        else if(ui->comboBox_E_f4->currentText()=="E5a")
            opt_corr->Frequently_GAL[3] = 2;
        if(ui->comboBox_E_f5->currentText()=="OFF")
            opt_corr->Frequently_GAL[4] = 0;
        else if(ui->comboBox_E_f5->currentText()=="E5a")
            opt_corr->Frequently_GAL[4] = 2;

        if(ui->comboBox_R_f1->currentText()=="L1")
            opt_corr->Frequently_GLO[0] = 1;
        else if(ui->comboBox_R_f1->currentText()=="L2")
            opt_corr->Frequently_GLO[0] = 2;
        if(ui->comboBox_R_f2->currentText()=="L2")
            opt_corr->Frequently_GLO[1] = 2;
        else if(ui->comboBox_R_f2->currentText()=="L5")
            opt_corr->Frequently_GLO[1] = 3;
        if(ui->comboBox_R_f3->currentText()=="OFF")
            opt_corr->Frequently_GLO[2] = 0;
        else if(ui->comboBox_R_f3->currentText()=="L5")
            opt_corr->Frequently_GLO[2] = 3;
        if(ui->filter_dir->currentText()=="forward filter")
            trans_para->for_and_backward_filter = false;
        else if(ui->filter_dir->currentText()=="forward and backward filter")
            trans_para->for_and_backward_filter = true;
        if(ui->frequency_model->currentText()=="mixed-fre")
            opt_corr->frequency_model = mixed_frequency;
        else if(ui->frequency_model->currentText()=="fixed-fre")
            opt_corr->frequency_model = fixed_frequency;

        int HMW_basd_fre=0;
        opt_corr->BDS_fre_Num=opt_corr->GPS_fre_Num=opt_corr->GLE_fre_Num=opt_corr->GLO_fre_Num = 0;

        HMW_basd_fre = 0;
        for(int i=0;i<5;i++)    {

            if(HMW_basd_fre==1)
                trans_para->basd_HMWC3 = 1;
            if(opt_corr->Frequently_BDS[i]==1)
                HMW_basd_fre++;
            opt_corr->BDS_fre_Num = opt_corr->BDS_fre_Num+opt_corr->Frequently_BDS[i];
        }
        HMW_basd_fre = 0;
        for(int i=0;i<3;i++)    {

            if(HMW_basd_fre==1)
                trans_para->basd_HMWG = 1;
            if(opt_corr->Frequently_GPS[i]==1)
                HMW_basd_fre++;
            opt_corr->GPS_fre_Num = opt_corr->GPS_fre_Num+opt_corr->Frequently_GPS[i];
        }
        HMW_basd_fre = 0;
        for(int i=0;i<5;i++)    {

            if(HMW_basd_fre==1)
                trans_para->basd_HMWE = 1;
            if(opt_corr->Frequently_GAL[i]==1)
                HMW_basd_fre++;
            opt_corr->GLE_fre_Num = opt_corr->GLE_fre_Num+opt_corr->Frequently_GAL[i];
        }
        HMW_basd_fre = 0;
        for(int i=0;i<3;i++)    {

            if(HMW_basd_fre==1)
                trans_para->basd_HMWR = 1;
            if(opt_corr->Frequently_GLO[i]==1)
                HMW_basd_fre++;
            opt_corr->GLO_fre_Num = opt_corr->GLO_fre_Num+opt_corr->Frequently_GLO[i];
        }
        QString Partial_subset = ui->PAR_subset->currentText();
        // 部分模糊度子集选取
        if(Partial_subset=="stand posterior residual")
            trans_para->Partial_subset = stand_post_res;
        else if(Partial_subset=="posterior residual sequence")
            trans_para->Partial_subset = post_resudual;
        else if(Partial_subset=="elevation angle strategy")
            trans_para->Partial_subset = angle_screen;
        else if(Partial_subset=="ADOP strategy")
            trans_para->Partial_subset = Adop_demet_secreen;
        else if(Partial_subset=="success rate criterion")
            trans_para->Partial_subset = Boost_Par;
        else if(Partial_subset=="full ambiguity fixed")
            trans_para->Partial_subset = full_ambiguity_fixed;
        else if(Partial_subset=="Diagonal var")
            trans_para->Partial_subset = Diagonal_var;

        QString Reference_sta = ui->Ref_satelite->currentText();
        if(Reference_sta=="Max elevation satellite")
            trans_para->Reference_sta = Max_el_sat;
        else if(Reference_sta=="Minimum variance satellite")
            trans_para->Reference_sta = Min_vc_sat;
        else if(Reference_sta=="Maximum success rate")
            trans_para->Reference_sta = Max_su_sat;
        else if(Reference_sta=="Min Adop value")
            trans_para->Reference_sta = Min_adop_sat;

        if(ui->Reinitalization_timebox->isChecked())
            opt_corr->Reinitalization_timebox = true;
        else
            opt_corr->Reinitalization_timebox = false;
        if(ui->GNSS_AR_select->isChecked())
            opt_corr->GNSS_AR_Select = true;
        else
            opt_corr->GNSS_AR_Select = false;
        if(ui->SRC_Ps->isChecked())
            trans_para->Sosr_flag = true;
        else
            trans_para->Sosr_flag = false;
        QString Ambiguity_acquisition = ui->Ambiguity_acquisition->currentText();
        if(Ambiguity_acquisition=="poor ambiguity")
            opt_corr->Ambiguity_acquisition = poor_ambiguity;

        QString Reinitalization_time = ui->Reinitalization_time->currentText();
        if("1hour"==Reinitalization_time)
            opt_corr->Reinitalization_time = 3600;
        else if("2hour"==Reinitalization_time)
            opt_corr->Reinitalization_time = 7200;
        else if("3hour"==Reinitalization_time)
            opt_corr->Reinitalization_time = 10800;
        else if("4hour"==Reinitalization_time)
            opt_corr->Reinitalization_time = 14400;
        else if("5hour"==Reinitalization_time)
            opt_corr->Reinitalization_time = 18000;
        else if("6hour"==Reinitalization_time)
            opt_corr->Reinitalization_time = 21600;
        QString select_frequence = ui->fre_mode->currentText();
        if(select_frequence == "double-fre")
            opt_corr->fre_model = double_fre;
        else if(select_frequence == "triple-fre")
            opt_corr->fre_model = triple_fre;
        else if(select_frequence == "four-fre")
            opt_corr->fre_model = four_fre;
        else if(select_frequence == "hybrid-fre")
            opt_corr->fre_model = hybrid_fre;
        opt_corr->UDUC_fre = opt_corr->fre_model;


        QString robust_est = ui->robust_estimation->currentText();
        if(ui->Robust_filter->isChecked())
            opt_corr->quality_flag = false;
        else
            opt_corr->quality_flag = true;


        // 抗差滤波选择
        if("3_cigma_criteria"==robust_est)
            opt_corr->quality_control_method = Classic_3cigma_method;


        // 高度角
        opt_corr->m_CutAngle = ui->angle_of_altitude->value();
        opt_corr->SNR = ui->SNR_of_min->value();
        QString position_method = ui->positioning_method->currentText();
        QString data_process_state=ui->data_process_state->currentText();
        QString motion_state = ui->motion_state->currentText();
        QString filter = ui->filter->currentText();

        QString Combination_PPP = ui->Combination_mode->currentText();
        ui->select_frequence->setCurrentIndex(ui->Combination_mode->currentIndex());
        if(Combination_PPP=="IF")
            ui->fre_mode->setCurrentIndex(0);
//定位模型
        // 模糊度固定模式

        if("Float solution" == position_method&&data_process_state=="post precise")
            opt_corr->treat_method = POS_PP_PPP;
        else if("Integer clock products" == position_method&&data_process_state=="post precise"&&Combination_PPP=="IF")
            opt_corr->treat_method = POS_PP_PPP_AR_1;
        else if("OSBs products" == position_method&&data_process_state=="post precise"&&Combination_PPP=="IF")
            opt_corr->treat_method = POS_Real_PPP_AR;
        else if("UPD/FCBs products" == position_method&&data_process_state=="post precise"&&Combination_PPP=="IF")
            opt_corr->treat_method = POS_PP_PPP_AR_2;
        else if("OSBs products" == position_method&&data_process_state=="post precise"&&Combination_PPP=="UDUC")
            opt_corr->treat_method = POS_UDUC_PPP_AR;


        //滤波模型
        if("dynamic" == motion_state&&filter=="kalman")
            opt_corr->filter_model = PPP_KINEMATIC_KALM;
        else if("static" == motion_state&&filter=="kalman")
            opt_corr->filter_model = PPP_STATIC_KALM;
        else if("dynamic" == motion_state&&filter=="slsq")
            opt_corr->filter_model = PPP_KINEMATIC_SRIF;
        else if("static" == motion_state&&filter=="slsq")
            opt_corr->filter_model = PPP_STATIC_SRIF;
        QString sys_error_select = ui->sys_error_select->currentText();
        if("ISBs" == sys_error_select)
            opt_corr->sys_error_select = ISBs_mode;
        else if("Only EST" == sys_error_select)
            opt_corr->sys_error_select = only_est;
        //组合方法
        if("IF"==Combination_PPP)
            opt_corr->Combination_mode=Comb_IF_mode;
        else if("UDUC"==Combination_PPP)
            opt_corr->Combination_mode=Comb_UD_mode;
        //周跳探测按钮
        if(ui->cycle_slip_detection->isChecked())
            opt_corr->cycle_slip_detection = true;
        else
            opt_corr->cycle_slip_detection = false;
        if(ui->ambiguity_inf->isChecked())
            opt_corr->ambiguity_inf = true;
        else
            opt_corr->ambiguity_inf = false;
//文件存储
        if(ui->position->isChecked())
            opt_corr->position = true;
        else
            opt_corr->position = false;
        if(ui->Bad_satellites->isChecked())
            opt_corr->bad_satellites = true;
        else
            opt_corr->bad_satellites = false;
        if(ui->ZTDW_Clock->isChecked())
            opt_corr->ZTDW_Clock = true;
        else
            opt_corr->ZTDW_Clock = false;
        if(ui->Satellite_info->isChecked())
            opt_corr->Satellite_info = true;
        else
            opt_corr->Satellite_info = false;
        if(ui->dop_satN_AR->isChecked())
            opt_corr->dop_satN_AR = true;
        else
            opt_corr->dop_satN_AR = false;
        if(ui->Az_El_coff->isChecked())
            opt_corr->Az_El_coff = true;
        else
            opt_corr->Az_El_coff = false;


        if(ui->mult_station->isChecked())
            opt_corr->muilt_stations = true;
        else
            opt_corr->muilt_stations = false;
        if(ui->mult_day->isChecked())
            opt_corr->mult_day = true;
        else
            opt_corr->mult_day = false;

// parameter_setting
        //    treat_method = POS_Real_PPP_AR;/*Positioning method*/
            opt_corr->flag_AR = 1;/*Partial ambiguity fixed*/
            opt_corr->ambig_out=0;
            if(opt_corr->treat_method==POS_RT_PPP_AR||opt_corr->treat_method==POS_PP_PPP_AR_1
                    ||opt_corr->treat_method==POS_PP_PPP_AR_2||opt_corr->treat_method==POS_Real_PPP_AR||opt_corr->treat_method==POS_UDUC_PPP_AR)
                opt_corr->ambig_out = 1;
            opt_corr->trop_selec = trop_UNB3m;
//            opt_corr->sto_model = stochastic_model_4;
            dat_fil->Output_system = opt_corr->m_sys_str;
            if(opt_corr->filter_model==PPP_KINEMATIC_KALM||opt_corr->filter_model==PPP_KINEMATIC_SRIF)
                dat_fil->Out_mod = "KINEMATIC";
            else if(opt_corr->filter_model==PPP_STATIC_KALM||opt_corr->filter_model==PPP_STATIC_SRIF)
                dat_fil->Out_mod = "STATIC";
            trans_para->m_minSatFlag = trans_para->const_parm;
        //    opt_corr->Combination_mode;
            opt_corr->WLUPD_corr = 0;
            dat_fil->OBX_flie_flag = 0;
            dat_fil->bia_file_flag = 0;
            dat_fil->int_clk_ar_flag = 0;
        //    opt_corr->m_sys_str;
            files_path = ui->observation_file_path->text();
            dat_fil->files_path = files_path;
            dat_fil->dat_path = dat_fil->files_path + PATHSEG;
            result_files_path = ui->result_file_path->text();
            dat_fil->result_files_path = result_files_path;
            dat_fil->out_dat_path = dat_fil->result_files_path + PATHSEG;
            opt_corr->m_deleteSats.clear();
            if(ui->BDS2->isChecked())
                opt_corr->m_deleteSats.append("C01,C02,C03,C04,C05,C06,C07,C08,C09,C10,C11,C12,C13,C14,C16,C17,C18");
            if(ui->BDS3->isChecked())
            {
                opt_corr->m_deleteSats.append("C19,C20,C21,C22,C23,C24,C25,C26,C27,C28,C29,C30,C31,C32");
                opt_corr->m_deleteSats.append("C33,C34,C35,C36,C37,C38,C39,C40,C41,C42,C43,C44,C45,C46,C47,C48,C49,C50,C51,C52,C53,C54,C55,C56,C57,58,C59,C60,C61,C62,C63");
            }
            if(ui->BDS2GEO->isChecked())
                opt_corr->m_deleteSats.append("C01,C02,C03,C04,C05,");
            if(ui->BDS3GEO->isChecked())
                opt_corr->m_deleteSats.append("C58,C59,C60,C61");
            QString delete_sat = ui->delete_satellite->text();
            opt_corr->m_deleteSats.append(delete_sat);
            opt_corr->multi_file_jia.clear();
            opt_corr->multi_day_OFileName.clear();
            search_file sea_f;
            QPair<QStringList, QStringList> list=sea_f.getFileList(dat_fil->dat_path);
            for(int i=1;i<list.second.length();i++)
            {
                QString file_list = dat_fil->files_path + list.second.at(i);
                opt_corr->multi_file_jia.append(file_list);
            }
            for(int i=1;i<list.second.length();i++)
            {

                QString file_path_obs=opt_corr->multi_file_jia.at(i-1);
                if(opt_corr->mult_day)
                {
                    QVector< QStringList > multi_sta_OFileName;
                    sea_f.multiple_stations(multi_sta_OFileName,file_path_obs);
                    opt_corr->multi_day_OFileName.append(multi_sta_OFileName);
                }
                else
                {
                    QVector< QStringList > multi_sta_OFileName;
                    sea_f.multiple_stations(multi_sta_OFileName,dat_fil->dat_path);
                    opt_corr->multi_sta_OFileName= multi_sta_OFileName;
                }
            }

            if(list.second.length()==1)
            {
                QVector< QStringList > multi_sta_OFileName;
                sea_f.multiple_stations(multi_sta_OFileName,dat_fil->dat_path);
                opt_corr->multi_sta_OFileName= multi_sta_OFileName;
            }

            coefficient_matrix coe_mat;
            coe_mat.init_CoeffVar(opt_corr,trans_para);
            opt_corr->WLUPD_corr =dat_fil->WLUPD_corr;


// save files
        // only insert string
            QJsonArray myconfArryJson;
            QJsonObject allConfig;

        // Set Parameters


            QString  Q_Combination_PPP = QString::number(ui->Combination_mode->currentIndex()) ;
            QString  Q_CutAngle = QString::number(ui->angle_of_altitude->value());
            QString  Q_SNR1 = QString::number(ui->SNR_of_min->value());
            QString  Q_Filename = ui->observation_file_path->text();
            QString  Q_result_Filename = ui->result_file_path->text();
            QString  Q_Filesave =
                     QString::number(opt_corr->position) + ";" +QString::number(opt_corr->bad_satellites) + ";" +
                     QString::number(opt_corr->ZTDW_Clock) + ";" +QString::number(opt_corr->Satellite_info) + ";" +
                     QString::number(opt_corr->dop_satN_AR) + ";" +QString::number(opt_corr->Az_El_coff) + ";" ;

            QString  Q_UDUCfrequency =
                     QString::number(ui->comboBox_C_f1->currentIndex())+ ";" +QString::number(ui->comboBox_C_f2->currentIndex())+ ";" +QString::number(ui->comboBox_C_f3->currentIndex())+";" +QString::number(ui->comboBox_C_f4->currentIndex())+ ";" +QString::number(ui->comboBox_C_f5->currentIndex())+";"+
                     QString::number(ui->comboBox_G_f1->currentIndex())+ ";" +QString::number(ui->comboBox_G_f2->currentIndex())+ ";" +QString::number(ui->comboBox_G_f3->currentIndex())+";" +
                     QString::number(ui->comboBox_E_f1->currentIndex())+ ";" +QString::number(ui->comboBox_E_f2->currentIndex())+ ";" +QString::number(ui->comboBox_E_f3->currentIndex())+ ";" +QString::number(ui->comboBox_E_f4->currentIndex())+ ";" +QString::number(ui->comboBox_E_f5->currentIndex())+";"+
                     QString::number(ui->comboBox_R_f1->currentIndex())+ ";" +QString::number(ui->comboBox_R_f2->currentIndex())+ ";" +QString::number(ui->comboBox_R_f3->currentIndex());
            QString  Q_filter_dir =QString::number(ui->filter_dir->currentIndex());
            QString  Q_frequency_model =QString::number(ui->frequency_model->currentIndex());
            QString  Q_BDSIFFrequency1 = QString::number(ui->BDSIF_1fre->currentIndex());
            QString  Q_BDSIFFrequency2 = QString::number(ui->BDSIF_2fre->currentIndex());
            QString  Q_GPSIFFrequency1 = QString::number(ui->GPSIF_1fre->currentIndex());
            QString  Q_GPSIFFrequency2 = QString::number(ui->GPSIF_2fre->currentIndex());
            QString  Q_GLEIFFrequency1 = QString::number(ui->GLEIF_1fre->currentIndex());
            QString  Q_GLEIFFrequency2 = QString::number(ui->GLEIF_2fre->currentIndex());
            QString  Q_GLOIFFrequency1 = QString::number(ui->GLOIF_1fre->currentIndex());

            QString  Q_GLOIFFrequency2 = QString::number(ui->GLOIF_2fre->currentIndex());
            QString  Q_Partial_subset = QString::number(ui->PAR_subset->currentIndex());
            QString  Q_Reference_sta = QString::number(ui->Ref_satelite->currentIndex());

            QString  Q_comboBox_C_f1 = QString::number(ui->comboBox_C_f1->currentIndex());
            QString  Q_comboBox_C_f2 = QString::number(ui->comboBox_C_f2->currentIndex());
            QString  Q_comboBox_C_f3 = QString::number(ui->comboBox_C_f3->currentIndex());
            QString  Q_comboBox_C_f4 = QString::number(ui->comboBox_C_f4->currentIndex());
            QString  Q_comboBox_C_f5 = QString::number(ui->comboBox_C_f5->currentIndex());
            QString  Q_comboBox_G_f1 = QString::number(ui->comboBox_G_f1->currentIndex());
            QString  Q_comboBox_G_f2 = QString::number(ui->comboBox_G_f2->currentIndex());
            QString  Q_comboBox_G_f3 = QString::number(ui->comboBox_G_f3->currentIndex());
            QString  Q_comboBox_E_f1 = QString::number(ui->comboBox_E_f1->currentIndex());
            QString  Q_comboBox_E_f2 = QString::number(ui->comboBox_E_f2->currentIndex());
            QString  Q_comboBox_E_f3 = QString::number(ui->comboBox_E_f3->currentIndex());
            QString  Q_comboBox_E_f4 = QString::number(ui->comboBox_E_f4->currentIndex());
            QString  Q_comboBox_E_f5 = QString::number(ui->comboBox_E_f5->currentIndex());
            QString  Q_comboBox_R_f1 = QString::number(ui->comboBox_R_f1->currentIndex());
            QString  Q_comboBox_R_f2 = QString::number(ui->comboBox_R_f2->currentIndex());
            QString  Q_comboBox_R_f3 = QString::number(ui->comboBox_R_f3->currentIndex());
            QString  Q_Reinitalization_timebox;
            if(ui->Reinitalization_timebox->isChecked())
                Q_Reinitalization_timebox.append("1");
            else
                Q_Reinitalization_timebox.append("0");
            QString  Q_GNSS_AR_select;
            if(ui->GNSS_AR_select->isChecked())
                Q_GNSS_AR_select.append("1");
            else
                Q_GNSS_AR_select.append("0");
            QString  Q_SRC_Ps;
            if(ui->SRC_Ps->isChecked())
                Q_SRC_Ps.append("1");
            else
                Q_SRC_Ps.append("0");
            QString  Q_WL_constraint;
            if(ui->WL_constraint->isChecked())
                Q_WL_constraint.append("1");
            else
                Q_WL_constraint.append("0");
            QString  Q_Reinitalization_time = QString::number( ui->Reinitalization_time->currentIndex());
            QString Q_system;
            if(ui->select_GPS->isChecked())
                Q_system.append("1;");
            else
                Q_system.append("0;");
            if(ui->select_GLONASS->isChecked())
                Q_system.append("1;");
            else
                Q_system.append("0;");
            if(ui->select_Galileo->isChecked())
                Q_system.append("1;");
            else
                Q_system.append("0;");
            if(ui->select_BDS->isChecked())
                Q_system.append("1");
            else
                Q_system.append("0");
            QString  Q_delete;
            if(ui->BDS2->isChecked())
                Q_delete.append("1;");
            else
                Q_delete.append("0;");
            if(ui->BDS3->isChecked())
                Q_delete.append("1;");
            else
                Q_delete.append("0;");
            if(ui->BDS2GEO->isChecked())
                Q_delete.append("1;");
            else
                Q_delete.append("0;");
            if(ui->BDS3GEO->isChecked())
                Q_delete.append("1");
            else
                Q_delete.append("0");
            QString  Q_deletetxt = ui->delete_satellite->text();
            QString  Q_muilt_stations,Q_mult_day;
            if(ui->mult_station->isChecked())
                Q_muilt_stations = "1";
            else
                Q_muilt_stations = "0";
            if(ui->mult_day->isChecked())
                Q_mult_day = "1";
            else
                Q_mult_day = "0";
            QString  Q_sys_error_select = QString::number(ui->sys_error_select->currentIndex());
            QString  Q_Combination_model = QString::number(ui->Combination_mode->currentIndex());
            QString  Q_fre_model = QString::number(ui->fre_mode->currentIndex());
            QString  Q_motion_state = QString::number(ui->motion_state->currentIndex());
            QString  Q_Filtering = QString::number(ui->filter->currentIndex());
            QString  Q_data_process_state = QString::number(ui->data_process_state->currentIndex());

            QString  Q_position_method = QString::number(ui->positioning_method->currentIndex());
            QString  Q_Ambiguity_acquisition = QString::number(ui->Ambiguity_acquisition->currentIndex());
            QString  Q_robust_est = QString::number(ui->robust_estimation->currentIndex());
            QString  Q_cycle_slip_detection,Q_quality_flag,Q_ambiguity_inf;
            if(ui->Robust_filter->isChecked())
                Q_quality_flag = "0";
            else
                Q_quality_flag = "1";
            if(ui->cycle_slip_detection->isChecked())
                Q_cycle_slip_detection = "0";
            else
                Q_cycle_slip_detection = "1";
            if(ui->ambiguity_inf->isChecked())
                Q_ambiguity_inf = "0";
            else
                Q_ambiguity_inf = "1";

            allConfig.insert("Combination_PPP", Q_Combination_PPP.toUtf8().data());
            allConfig.insert("CutAngle", Q_CutAngle.toUtf8().data());
            allConfig.insert("SNR", Q_SNR1.toUtf8().data());
            allConfig.insert("Filename", Q_Filename.toUtf8().data());
            allConfig.insert("result_Filename", Q_result_Filename.toUtf8().data());
            allConfig.insert("Filesave", Q_Filesave.toUtf8().data());
            allConfig.insert("UDUCFrequency", Q_UDUCfrequency.toUtf8().data());
            allConfig.insert("BDSIFFrequency1", Q_BDSIFFrequency1.toUtf8().data());
            allConfig.insert("BDSIFFrequency2", Q_BDSIFFrequency2.toUtf8().data());
            allConfig.insert("GPSIFFrequency1", Q_GPSIFFrequency1.toUtf8().data());
            allConfig.insert("GPSIFFrequency2", Q_GPSIFFrequency2.toUtf8().data());
            allConfig.insert("GLEIFFrequency1", Q_GLEIFFrequency1.toUtf8().data());
            allConfig.insert("GLEIFFrequency2", Q_GLEIFFrequency2.toUtf8().data());
            allConfig.insert("GLOIFFrequency1", Q_GLOIFFrequency1.toUtf8().data());
            allConfig.insert("GLOIFFrequency2", Q_GLOIFFrequency2.toUtf8().data());

            allConfig.insert("comboBox_C_f1", Q_comboBox_C_f1.toUtf8().data());
            allConfig.insert("comboBox_C_f2", Q_comboBox_C_f2.toUtf8().data());
            allConfig.insert("comboBox_C_f3", Q_comboBox_C_f3.toUtf8().data());
            allConfig.insert("comboBox_C_f4", Q_comboBox_C_f4.toUtf8().data());
            allConfig.insert("comboBox_C_f5", Q_comboBox_C_f5.toUtf8().data());
            allConfig.insert("comboBox_G_f1", Q_comboBox_G_f1.toUtf8().data());
            allConfig.insert("comboBox_G_f2", Q_comboBox_G_f2.toUtf8().data());
            allConfig.insert("comboBox_G_f3", Q_comboBox_G_f3.toUtf8().data());
            allConfig.insert("comboBox_E_f1", Q_comboBox_E_f1.toUtf8().data());
            allConfig.insert("comboBox_E_f2", Q_comboBox_E_f2.toUtf8().data());
            allConfig.insert("comboBox_E_f3", Q_comboBox_E_f3.toUtf8().data());
            allConfig.insert("comboBox_E_f4", Q_comboBox_E_f4.toUtf8().data());
            allConfig.insert("comboBox_E_f5", Q_comboBox_E_f5.toUtf8().data());
            allConfig.insert("comboBox_R_f1", Q_comboBox_R_f1.toUtf8().data());
            allConfig.insert("comboBox_R_f2", Q_comboBox_R_f2.toUtf8().data());
            allConfig.insert("comboBox_R_f3", Q_comboBox_R_f3.toUtf8().data());
            allConfig.insert("filter_dir", Q_filter_dir.toUtf8().data());
            allConfig.insert("frequency_model", Q_frequency_model.toUtf8().data());
            allConfig.insert("Partial_subset", Q_Partial_subset.toUtf8().data());
            allConfig.insert("Reference_sta", Q_Reference_sta.toUtf8().data());
            allConfig.insert("Reinitalization_timebox", Q_Reinitalization_timebox.toUtf8().data());
            allConfig.insert("GNSS AR select", Q_GNSS_AR_select.toUtf8().data());
            allConfig.insert("SRC Ps", Q_SRC_Ps.toUtf8().data());
            allConfig.insert("Wide lane constraint", Q_WL_constraint.toUtf8().data());
            allConfig.insert("Reinitalization_time", Q_Reinitalization_time.toUtf8().data());
            allConfig.insert("System", Q_system.toUtf8().data());
            allConfig.insert("delete", Q_delete.toUtf8().data());
            allConfig.insert("deletetxt", Q_deletetxt.toUtf8().data());
            allConfig.insert("muilt_stations", Q_muilt_stations.toUtf8().data());
            allConfig.insert("mult_day", Q_mult_day.toUtf8().data());
            allConfig.insert("Combination_model", Q_Combination_model.toUtf8().data());
            allConfig.insert("system error select", Q_sys_error_select.toUtf8().data());
            allConfig.insert("fre_model", Q_fre_model.toUtf8().data());
            allConfig.insert("motion_state", Q_motion_state.toUtf8().data());
            allConfig.insert("Filtering", Q_Filtering.toUtf8().data());
            allConfig.insert("data_process_state", Q_data_process_state.toUtf8().data());
            allConfig.insert("position_method", Q_position_method.toUtf8().data());
            allConfig.insert("quality_flag", Q_quality_flag.toUtf8().data());

            allConfig.insert("robust_est", Q_robust_est.toUtf8().data());



            allConfig.insert("cycle_slip_detection", Q_cycle_slip_detection.toUtf8().data());
            allConfig.insert("Ambioguity information", Q_ambiguity_inf.toUtf8().data());
            allConfig.insert("Ambiguity_acquisition", Q_Ambiguity_acquisition.toUtf8().data());








        // add all config to array
        myconfArryJson.append(allConfig);
        QString iniFileName = TranIni.getFileName(), jsonFileName = "MG-MixF Config.json";
        QStringList iniList = iniFileName.split(".");
        if(iniList.length() > 1) jsonFileName = iniList.at(0) + ".json";
        ConfTranIni::writeJson2IniS(TranIni.getFileName(), myconfArryJson);
        ConfTranIni::writeJson2File(jsonFileName, myconfArryJson);
        init();
        this->close();
        // write to ini file
//            QMessageBox:: StandardButton result = QMessageBox::information(this, "Configure", "Are you sure about the reconfiguration?",
//                                                                           QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
//            if(result == QMessageBox::Yes)
//            {

//            }
}
void SetWidget::changeDisplay_fre()
{
    ui->select_frequence->setCurrentIndex(ui->Combination_mode->currentIndex());
    if(ui->Combination_mode->currentText()=="IF")
        ui->fre_mode->setCurrentIndex(0);
}

