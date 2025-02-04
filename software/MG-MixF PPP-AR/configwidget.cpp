#include "ConfigWidget.h"
#include "ui_ConfigWidget.h"

ConfigWidget::ConfigWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConfigWidget),
    myConfTranIni("MG-MixF PPP-AR.ini")
{
    ui->setupUi(this);
    initWidgt();
    clickOk();
    this->setFixedSize(this->size());// fix window size
    this->setWindowModality(Qt::ApplicationModal);
    m_autoResize_setwidget_Handler=new AutoResize(this,this->rect().width(),this->rect().height());
    m_autoResize_setwidget_Handler->setAutoResizeFlag(
            AutoResize::INCLUDE_BUTTON|AutoResize::INCLUDE_COMBOBOX|
            AutoResize::INCLUDE_EDITOR|AutoResize::INCLUDE_LABEL
            );

    m_autoResize_setwidget_Handler->addOtherItem(ui->groupBox_5);
    m_autoResize_setwidget_Handler->addOtherItem(ui->groupBox_4);
    m_autoResize_setwidget_Handler->addOtherItem(ui->groupBox_3);
    m_autoResize_setwidget_Handler->addOtherItem(ui->groupBox_2);
    m_autoResize_setwidget_Handler->addOtherItem(ui->groupBox);
    m_autoResize_setwidget_Handler->addOtherItem(ui->groupBox_del_2);
    m_autoResize_setwidget_Handler->pushAllResizeItem();
    // connect
    connect(ui->pushButton_ok, SIGNAL(clicked(bool)), this, SLOT(clickOk()));
    connect(ui->pushButton_cancel, SIGNAL(clicked(bool)), this, SLOT(clickCancel()));
    // init window

}

ConfigWidget::~ConfigWidget()
{
    delete ui;
}
void ConfigWidget::resizeEvent(QResizeEvent *event)
{
    m_autoResize_setwidget_Handler->doAutoResize();
}
void ConfigWidget::showEvent(QShowEvent *event)
{
    // Nav功能运行状态下，定位相关信息不可配置
    if (AGNSS_APPData::sGetAPPDataInstance()->mGetAppFunState())
    {
        this->setEnabled(false);
    }
    else
        this->setEnabled(true);
}
void ConfigWidget::initWidgt()
{

//title
    setWindowTitle("ConfigWidget");

// Set Parameters
    QString Qw_Str, Pk_Str,WL_Str,NL_cutoff_str;
    Qw_Str = myConfTranIni.getValue("/MG-MixF PPP-AR/Qw");
    Pk_Str = myConfTranIni.getValue("/MG-MixF PPP-AR/Pk");
    WL_Str = myConfTranIni.getValue("/MG-MixF PPP-AR/WL weight");
    NL_cutoff_str = myConfTranIni.getValue("/MG-MixF PPP-AR/NL cutoff");
    QStringList Qw_StrList =  Qw_Str.split(";"),
            Pk_StrList = Pk_Str.split(";"),
            WL_StrList = WL_Str.split(";"),
            NL_cutoff_StrList = NL_cutoff_str.split(";");
    Qw_StrList.removeAll(QString("")); Pk_StrList.removeAll(QString(""));
    WL_StrList.removeAll(QString(""));NL_cutoff_StrList.removeAll(QString(""));
    if(Qw_StrList.length() != 0)
    {
        while(Qw_StrList.length() < 5) Qw_StrList.append(QString(""));
        ui->Qw_pos->setPlainText(Qw_StrList.at(0)); ui->Qw_zwd->setPlainText(Qw_StrList.at(1));
        ui->Qw_clk->setPlainText(Qw_StrList.at(2)); ui->Qw_amb->setPlainText(Qw_StrList.at(3));
        ui->Qw_ion->setPlainText(Qw_StrList.at(4));
    }

    if(Pk_StrList.length() != 0)
    {
        while(Pk_StrList.length() < 5) Pk_StrList.append(QString(""));
        ui->Pk_pos->setPlainText(Pk_StrList.at(0)); ui->Pk_zwd->setPlainText(Pk_StrList.at(1));
        ui->Pk_clk->setPlainText(Pk_StrList.at(2)); ui->Pk_amb->setPlainText(Pk_StrList.at(3));
        ui->Pk_ion->setPlainText(Pk_StrList.at(4));
    }
    if(WL_StrList.length() != 0)
    {
        while(WL_StrList.length() < 1) WL_StrList.append(QString(""));
        ui->WL_weight->setPlainText(WL_StrList.at(0));
    }
    if(NL_cutoff_StrList.length() != 0)
    {
        while(NL_cutoff_StrList.length() < 1) NL_cutoff_StrList.append(QString(""));
        ui->NL_cutoff->setPlainText(NL_cutoff_StrList.at(0));
    }
    QString LP_precision = myConfTranIni.getValue("/MG-MixF PPP-AR/LP_precision");
    QStringList LP_StrList =  LP_precision.split(";");
    LP_StrList.removeAll(QString(""));
    if(LP_StrList.length() !=0)
    {
        while(LP_StrList.length() < 10) LP_StrList.append(QString(""));
        ui->L_precision->setPlainText(LP_StrList.at(0)); ui->P_precision->setPlainText(LP_StrList.at(1));
        ui->L_precision_2->setPlainText(LP_StrList.at(2)); ui->P_precision_2->setPlainText(LP_StrList.at(3));
        ui->L_precision_3->setPlainText(LP_StrList.at(4)); ui->P_precision_3->setPlainText(LP_StrList.at(5));
        ui->L_precision_4->setPlainText(LP_StrList.at(6)); ui->P_precision_4->setPlainText(LP_StrList.at(7));
        ui->L_precision_5->setPlainText(LP_StrList.at(8)); ui->P_precision_5->setPlainText(LP_StrList.at(9));
    }
    QString Helmert_VC = myConfTranIni.getValue("/MG-MixF PPP-AR/Helmert_VC");
    QStringList Helmet_StrList =  Helmert_VC.split(";");
    Helmet_StrList.removeAll(QString(""));
    if(Helmet_StrList.length() !=0)
    {
        if(Helmet_StrList[0]=="1")
            ui->Helmert_VC->setChecked(true);
        else
            ui->Helmert_VC->setChecked(false);
    }
    QString Stochastic_m = myConfTranIni.getValue("/MG-MixF PPP-AR/Stochastic_model");
    if(Stochastic_m.length() !=0)
    {
        ui->stochastic_model->setCurrentIndex(Stochastic_m.toInt());
    }
    QString Ratio_Str;
    Ratio_Str = myConfTranIni.getValue("/MG-MixF PPP-AR/Ratio value");
    if(Ratio_Str.length()!=0)
        ui->Ratio_value->setPlainText(Ratio_Str);
    corr_opt *opt_corr = AGNSS_APPData::sGetAPPDataInstance()->mGetopt_corrData();
    para_trans *trans_para = AGNSS_APPData::sGetAPPDataInstance()->mGettrans_paraData();
    trans_para->ratio_value = Ratio_Str.toDouble();

    if(LP_StrList.length()!=0)
    {
        opt_corr->G_code_whihet = LP_StrList.at(1).toDouble();
        opt_corr->G_whihet_diff = LP_StrList.at(1).toDouble()/LP_StrList.at(0).toDouble();
    }
    opt_corr->G_whihet_diff = opt_corr->G_whihet_diff*opt_corr->G_whihet_diff;
    if(LP_StrList.length()!=0)
    {
        opt_corr->C2_code_whihet = LP_StrList.at(3).toDouble();
        opt_corr->C2_whihet_diff = LP_StrList.at(3).toDouble()/LP_StrList.at(2).toDouble();
    }
    opt_corr->C2_whihet_diff = opt_corr->C2_whihet_diff*opt_corr->C2_whihet_diff;
    if(LP_StrList.length()!=0)
    {
        opt_corr->C3_code_whihet = LP_StrList.at(5).toDouble();
        opt_corr->C3_whihet_diff = LP_StrList.at(5).toDouble()/LP_StrList.at(4).toDouble();
    }
    opt_corr->C3_whihet_diff = opt_corr->C3_whihet_diff*opt_corr->C3_whihet_diff;
    if(LP_StrList.length()!=0)
    {
        opt_corr->E_code_whihet = LP_StrList.at(7).toDouble();
        opt_corr->E_whihet_diff = LP_StrList.at(7).toDouble()/LP_StrList.at(6).toDouble();
    }
    opt_corr->E_whihet_diff = opt_corr->E_whihet_diff*opt_corr->E_whihet_diff;
    if(LP_StrList.length()!=0)
    {
        opt_corr->R_code_whihet = LP_StrList.at(9).toDouble();
        opt_corr->R_whihet_diff = LP_StrList.at(9).toDouble()/LP_StrList.at(8).toDouble();
    }
    opt_corr->R_whihet_diff = opt_corr->R_whihet_diff*opt_corr->R_whihet_diff;
    if(Qw_StrList.length()!=0)
    {
        opt_corr->xyz_kinematic = Qw_StrList.at(0).toDouble();
        opt_corr->trop_covarQ = Qw_StrList.at(1).toDouble();
        opt_corr->rcv_clkQ = Qw_StrList.at(2).toDouble();
        opt_corr->sat_ambQ = Qw_StrList.at(3).toDouble();
        opt_corr->sat_ionQ = Qw_StrList.at(4).toDouble();
    }
    if(Qw_StrList.length()!=0)
    {
        opt_corr->xyz_static = Pk_StrList.at(0).toDouble();
        opt_corr->trop_covarP = Pk_StrList.at(1).toDouble();
        opt_corr->rcv_clkP = Pk_StrList.at(2).toDouble();
        opt_corr->sat_ambP = Pk_StrList.at(3).toDouble();
        opt_corr->sat_ionP = Pk_StrList.at(4).toDouble();
    }
    if(WL_StrList.length()!=0)
    {
        opt_corr->wl_fixed_weight = WL_StrList.at(0).toDouble();
    }
    if(NL_cutoff_StrList.length()!=0)
    {
        trans_para->bias_nl = NL_cutoff_StrList.at(0).toDouble();
    }
    if(ui->Helmert_VC->isChecked())
        opt_corr->Helmert_flag = true;
    else
        opt_corr->Helmert_flag = false;
    Stochastic_m = ui->stochastic_model->currentText();
    if(Stochastic_m=="Weighted sine elevation angle")
        opt_corr->sto_model = stochastic_model_1;
    else if(Stochastic_m=="Weighted cosine elevation angle")
        opt_corr->sto_model = stochastic_model_2;
    else if(Stochastic_m=="Sine  elevation angle 1")
        opt_corr->sto_model = stochastic_model_3;
    else if(Stochastic_m=="Panda stochastic model")
        opt_corr->sto_model = stochastic_model_4;

}


//slots
void ConfigWidget::clickOk()
{
// only insert string
    QJsonArray myconfArryJson;
    QJsonObject allConfig;
// Set Parameters
    QString Qw_List = ui->Qw_pos->toPlainText() + ";" + ui->Qw_zwd->toPlainText() + ";"
            + ui->Qw_clk->toPlainText() + ";" + ui->Qw_amb->toPlainText() + ";"
            + ui->Qw_ion->toPlainText();// Qw String List
    Qw_List = Qw_List.replace(";", ";"); Qw_List = Qw_List.replace(" ", ";");

    QString Pk_List = ui->Pk_pos->toPlainText() + ";" + ui->Pk_zwd->toPlainText() + ";"
            + ui->Pk_clk->toPlainText() + ";" + ui->Pk_amb->toPlainText() + ";"
            + ui->Pk_ion->toPlainText();// Qw String List
    Pk_List = Pk_List.replace(";", ";"); Pk_List = Pk_List.replace(" ", ";");
    QString WL_List = ui->WL_weight->toPlainText() + ";";// Qw String List
    WL_List = WL_List.replace(";", ";"); WL_List = WL_List.replace(" ", ";");
    QString NL_cutoff_List = ui->NL_cutoff->toPlainText() + ";";// Qw String List
    NL_cutoff_List = NL_cutoff_List.replace(";", ";"); NL_cutoff_List = NL_cutoff_List.replace(" ", ";");
    QString LP_precision = ui->L_precision->toPlainText() + ";" +ui->P_precision->toPlainText()+
            ";"+ui->L_precision_2->toPlainText() + ";" +ui->P_precision_2->toPlainText()+
            ";"+ui->L_precision_3->toPlainText() + ";" +ui->P_precision_3->toPlainText()+
            ";"+ui->L_precision_4->toPlainText() + ";" +ui->P_precision_4->toPlainText()+
            ";"+ui->L_precision_5->toPlainText() + ";" +ui->P_precision_5->toPlainText();
    QString  Helmert_VC;
    QString Stochastic_model = QString::number(ui->stochastic_model->currentIndex());
    QString Ratio_value;
    Ratio_value= ui->Ratio_value->toPlainText();
    if(ui->Helmert_VC->isChecked())
        Helmert_VC.append("1");
    else
        Helmert_VC.append("0");

    allConfig.insert("Qw", Qw_List.toUtf8().data());
    allConfig.insert("Pk", Pk_List.toUtf8().data());
    allConfig.insert("WL weight", WL_List.toUtf8().data());
    allConfig.insert("NL cutoff", NL_cutoff_List.toUtf8().data());
    allConfig.insert("LP_precision", LP_precision.toUtf8().data());
    allConfig.insert("Helmert_VC", Helmert_VC.toUtf8().data());
    allConfig.insert("Stochastic_model", Stochastic_model.toUtf8().data());
    allConfig.insert("Ratio value", Ratio_value.toUtf8().data());
// add all config to array
    myconfArryJson.append(allConfig);
    QString iniFileName = myConfTranIni.getFileName(), jsonFileName = "MG-MixF PPP-AR.json";
    QStringList iniList = iniFileName.split(".");
    if(iniList.length() > 1) jsonFileName = iniList.at(0) + ".json";
    ConfTranIni::writeJson2Ini(myConfTranIni.getFileName(), myconfArryJson);
    ConfTranIni::writeJson2File(jsonFileName, myconfArryJson);
    initWidgt();
    this->close();
// write to ini file
//    QMessageBox:: StandardButton result = QMessageBox::information(this, "Configure", "Are you sure about the reconfiguration?",
//                                                                   QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
//    if(result == QMessageBox::Yes)
//    {

//    }
}

void ConfigWidget::clickCancel()
{
    initWidgt();
    this->close();
}
