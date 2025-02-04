/********************************************************************************
** Form generated from reading UI file 'SetWidget.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETWIDGET_H
#define UI_SETWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SetWidget
{
public:
    QGroupBox *groupBox_2;
    QCheckBox *Bad_satellites;
    QCheckBox *ZTDW_Clock;
    QCheckBox *Satellite_info;
    QCheckBox *Az_El_coff;
    QCheckBox *dop_satN_AR;
    QCheckBox *position;
    QCheckBox *ambiguity_inf;
    QGroupBox *groupBox_3;
    QComboBox *positioning_method;
    QComboBox *PAR_subset;
    QLabel *label_8;
    QLabel *label_9;
    QLabel *label_3;
    QComboBox *Ref_satelite;
    QGroupBox *groupBox_7;
    QCheckBox *BDS2;
    QCheckBox *BDS3;
    QCheckBox *BDS2GEO;
    QCheckBox *BDS3GEO;
    QLineEdit *delete_satellite;
    QGroupBox *groupBox;
    QComboBox *Combination_mode;
    QComboBox *fre_mode;
    QLabel *label_2;
    QLabel *label_13;
    QComboBox *motion_state;
    QComboBox *filter;
    QLabel *label_24;
    QComboBox *data_process_state;
    QGroupBox *groupBox_4;
    QCheckBox *select_GLONASS;
    QCheckBox *select_Galileo;
    QCheckBox *select_GPS;
    QCheckBox *select_BDS;
    QGroupBox *groupBox_9;
    QStackedWidget *select_frequence;
    QWidget *page;
    QComboBox *comboBox_C_f1;
    QComboBox *comboBox_C_f2;
    QComboBox *comboBox_C_f3;
    QComboBox *comboBox_C_f4;
    QComboBox *comboBox_C_f5;
    QComboBox *comboBox_G_f1;
    QComboBox *comboBox_G_f2;
    QComboBox *comboBox_G_f3;
    QComboBox *comboBox_E_f5;
    QComboBox *comboBox_E_f1;
    QComboBox *comboBox_E_f2;
    QComboBox *comboBox_E_f4;
    QComboBox *comboBox_E_f3;
    QComboBox *comboBox_R_f3;
    QComboBox *comboBox_R_f2;
    QComboBox *comboBox_R_f1;
    QWidget *page_2;
    QComboBox *BDSIF_1fre;
    QComboBox *BDSIF_2fre;
    QComboBox *GPSIF_2fre;
    QComboBox *GPSIF_1fre;
    QComboBox *GLEIF_2fre;
    QComboBox *GLEIF_1fre;
    QComboBox *GLOIF_2fre;
    QComboBox *GLOIF_1fre;
    QLabel *label;
    QLabel *label_17;
    QLabel *label_18;
    QLabel *label_19;
    QLabel *label_20;
    QLabel *label_21;
    QLabel *label_22;
    QLabel *label_23;
    QGroupBox *groupBox_10;
    QLabel *label_6;
    QSpinBox *ELEspinBox;
    QSlider *angle_of_altitude;
    QLabel *label_5;
    QSpinBox *SNRspinBox_2;
    QSlider *SNR_of_min;
    QCheckBox *cycle_slip_detection;
    QComboBox *robust_estimation;
    QCheckBox *Robust_filter;
    QGroupBox *groupBox_5;
    QPushButton *select_path;
    QLineEdit *observation_file_path;
    QLineEdit *result_file_path;
    QPushButton *select_save_file_path;
    QGroupBox *groupBox_6;
    QComboBox *sys_error_select;
    QCheckBox *GNSS_AR_select;
    QCheckBox *SRC_Ps;
    QPushButton *run_engineering_2;
    QPushButton *save_config;
    QGroupBox *groupBox_8;
    QCheckBox *Reinitalization_timebox;
    QComboBox *Reinitalization_time;
    QCheckBox *mult_station;
    QCheckBox *mult_day;
    QGroupBox *groupBox_11;
    QCheckBox *WL_constraint;
    QComboBox *Ambiguity_acquisition;
    QGroupBox *groupBox_12;
    QComboBox *filter_dir;
    QComboBox *frequency_model;

    void setupUi(QWidget *SetWidget)
    {
        if (SetWidget->objectName().isEmpty())
            SetWidget->setObjectName(QStringLiteral("SetWidget"));
        SetWidget->resize(1376, 623);
        QFont font;
        font.setFamily(QStringLiteral("Times New Roman"));
        font.setPointSize(10);
        SetWidget->setFont(font);
        QIcon icon;
        icon.addFile(QStringLiteral(":/resources/icon2.png"), QSize(), QIcon::Normal, QIcon::Off);
        SetWidget->setWindowIcon(icon);
        groupBox_2 = new QGroupBox(SetWidget);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        groupBox_2->setGeometry(QRect(470, 260, 581, 151));
        Bad_satellites = new QCheckBox(groupBox_2);
        Bad_satellites->setObjectName(QStringLiteral("Bad_satellites"));
        Bad_satellites->setGeometry(QRect(120, 30, 101, 21));
        ZTDW_Clock = new QCheckBox(groupBox_2);
        ZTDW_Clock->setObjectName(QStringLiteral("ZTDW_Clock"));
        ZTDW_Clock->setGeometry(QRect(230, 30, 81, 21));
        ZTDW_Clock->setChecked(true);
        Satellite_info = new QCheckBox(groupBox_2);
        Satellite_info->setObjectName(QStringLiteral("Satellite_info"));
        Satellite_info->setGeometry(QRect(330, 30, 101, 21));
        Az_El_coff = new QCheckBox(groupBox_2);
        Az_El_coff->setObjectName(QStringLiteral("Az_El_coff"));
        Az_El_coff->setGeometry(QRect(30, 70, 111, 21));
        Az_El_coff->setChecked(true);
        dop_satN_AR = new QCheckBox(groupBox_2);
        dop_satN_AR->setObjectName(QStringLiteral("dop_satN_AR"));
        dop_satN_AR->setGeometry(QRect(450, 30, 121, 21));
        dop_satN_AR->setChecked(true);
        position = new QCheckBox(groupBox_2);
        position->setObjectName(QStringLiteral("position"));
        position->setGeometry(QRect(30, 30, 91, 21));
        position->setChecked(true);
        ambiguity_inf = new QCheckBox(groupBox_2);
        ambiguity_inf->setObjectName(QStringLiteral("ambiguity_inf"));
        ambiguity_inf->setEnabled(true);
        ambiguity_inf->setGeometry(QRect(150, 70, 91, 19));
        ambiguity_inf->setChecked(true);
        groupBox_3 = new QGroupBox(SetWidget);
        groupBox_3->setObjectName(QStringLiteral("groupBox_3"));
        groupBox_3->setGeometry(QRect(1090, 30, 251, 211));
        positioning_method = new QComboBox(groupBox_3);
        positioning_method->setObjectName(QStringLiteral("positioning_method"));
        positioning_method->setGeometry(QRect(10, 50, 191, 31));
        PAR_subset = new QComboBox(groupBox_3);
        PAR_subset->setObjectName(QStringLiteral("PAR_subset"));
        PAR_subset->setGeometry(QRect(10, 110, 221, 31));
        PAR_subset->setFrame(true);
        label_8 = new QLabel(groupBox_3);
        label_8->setObjectName(QStringLiteral("label_8"));
        label_8->setGeometry(QRect(10, 20, 221, 16));
        label_9 = new QLabel(groupBox_3);
        label_9->setObjectName(QStringLiteral("label_9"));
        label_9->setGeometry(QRect(10, 90, 231, 16));
        label_3 = new QLabel(groupBox_3);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(10, 150, 211, 16));
        Ref_satelite = new QComboBox(groupBox_3);
        Ref_satelite->setObjectName(QStringLiteral("Ref_satelite"));
        Ref_satelite->setGeometry(QRect(10, 170, 221, 31));
        groupBox_7 = new QGroupBox(SetWidget);
        groupBox_7->setObjectName(QStringLiteral("groupBox_7"));
        groupBox_7->setGeometry(QRect(20, 260, 311, 151));
        BDS2 = new QCheckBox(groupBox_7);
        BDS2->setObjectName(QStringLiteral("BDS2"));
        BDS2->setGeometry(QRect(20, 20, 91, 19));
        BDS3 = new QCheckBox(groupBox_7);
        BDS3->setObjectName(QStringLiteral("BDS3"));
        BDS3->setGeometry(QRect(170, 20, 91, 19));
        BDS2GEO = new QCheckBox(groupBox_7);
        BDS2GEO->setObjectName(QStringLiteral("BDS2GEO"));
        BDS2GEO->setGeometry(QRect(20, 40, 101, 19));
        BDS2GEO->setChecked(true);
        BDS3GEO = new QCheckBox(groupBox_7);
        BDS3GEO->setObjectName(QStringLiteral("BDS3GEO"));
        BDS3GEO->setGeometry(QRect(170, 40, 101, 19));
        BDS3GEO->setChecked(true);
        delete_satellite = new QLineEdit(groupBox_7);
        delete_satellite->setObjectName(QStringLiteral("delete_satellite"));
        delete_satellite->setGeometry(QRect(10, 70, 291, 71));
        groupBox = new QGroupBox(SetWidget);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setGeometry(QRect(20, 30, 241, 211));
        Combination_mode = new QComboBox(groupBox);
        Combination_mode->setObjectName(QStringLiteral("Combination_mode"));
        Combination_mode->setGeometry(QRect(20, 40, 71, 31));
        fre_mode = new QComboBox(groupBox);
        fre_mode->setObjectName(QStringLiteral("fre_mode"));
        fre_mode->setGeometry(QRect(96, 40, 111, 31));
        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(20, 20, 141, 21));
        label_13 = new QLabel(groupBox);
        label_13->setObjectName(QStringLiteral("label_13"));
        label_13->setGeometry(QRect(20, 80, 141, 21));
        motion_state = new QComboBox(groupBox);
        motion_state->setObjectName(QStringLiteral("motion_state"));
        motion_state->setGeometry(QRect(20, 100, 91, 31));
        filter = new QComboBox(groupBox);
        filter->setObjectName(QStringLiteral("filter"));
        filter->setGeometry(QRect(130, 100, 71, 31));
        label_24 = new QLabel(groupBox);
        label_24->setObjectName(QStringLiteral("label_24"));
        label_24->setGeometry(QRect(20, 140, 141, 21));
        data_process_state = new QComboBox(groupBox);
        data_process_state->setObjectName(QStringLiteral("data_process_state"));
        data_process_state->setGeometry(QRect(20, 170, 181, 31));
        groupBox_4 = new QGroupBox(SetWidget);
        groupBox_4->setObjectName(QStringLiteral("groupBox_4"));
        groupBox_4->setGeometry(QRect(270, 30, 120, 211));
        select_GLONASS = new QCheckBox(groupBox_4);
        select_GLONASS->setObjectName(QStringLiteral("select_GLONASS"));
        select_GLONASS->setGeometry(QRect(10, 180, 91, 21));
        select_Galileo = new QCheckBox(groupBox_4);
        select_Galileo->setObjectName(QStringLiteral("select_Galileo"));
        select_Galileo->setGeometry(QRect(10, 130, 91, 21));
        select_GPS = new QCheckBox(groupBox_4);
        select_GPS->setObjectName(QStringLiteral("select_GPS"));
        select_GPS->setEnabled(true);
        select_GPS->setGeometry(QRect(10, 80, 91, 21));
        select_GPS->setAcceptDrops(false);
        select_GPS->setChecked(false);
        select_BDS = new QCheckBox(groupBox_4);
        select_BDS->setObjectName(QStringLiteral("select_BDS"));
        select_BDS->setGeometry(QRect(10, 30, 81, 21));
        select_BDS->setChecked(true);
        groupBox_9 = new QGroupBox(SetWidget);
        groupBox_9->setObjectName(QStringLiteral("groupBox_9"));
        groupBox_9->setGeometry(QRect(410, 30, 381, 211));
        select_frequence = new QStackedWidget(groupBox_9);
        select_frequence->setObjectName(QStringLiteral("select_frequence"));
        select_frequence->setGeometry(QRect(20, 20, 341, 181));
        page = new QWidget();
        page->setObjectName(QStringLiteral("page"));
        comboBox_C_f1 = new QComboBox(page);
        comboBox_C_f1->setObjectName(QStringLiteral("comboBox_C_f1"));
        comboBox_C_f1->setGeometry(QRect(10, 10, 51, 22));
        comboBox_C_f2 = new QComboBox(page);
        comboBox_C_f2->setObjectName(QStringLiteral("comboBox_C_f2"));
        comboBox_C_f2->setGeometry(QRect(80, 10, 51, 22));
        comboBox_C_f3 = new QComboBox(page);
        comboBox_C_f3->setObjectName(QStringLiteral("comboBox_C_f3"));
        comboBox_C_f3->setGeometry(QRect(150, 10, 51, 22));
        comboBox_C_f4 = new QComboBox(page);
        comboBox_C_f4->setObjectName(QStringLiteral("comboBox_C_f4"));
        comboBox_C_f4->setGeometry(QRect(220, 10, 51, 22));
        comboBox_C_f5 = new QComboBox(page);
        comboBox_C_f5->setObjectName(QStringLiteral("comboBox_C_f5"));
        comboBox_C_f5->setGeometry(QRect(290, 10, 51, 22));
        comboBox_G_f1 = new QComboBox(page);
        comboBox_G_f1->setObjectName(QStringLiteral("comboBox_G_f1"));
        comboBox_G_f1->setGeometry(QRect(10, 60, 51, 22));
        comboBox_G_f2 = new QComboBox(page);
        comboBox_G_f2->setObjectName(QStringLiteral("comboBox_G_f2"));
        comboBox_G_f2->setGeometry(QRect(80, 60, 51, 22));
        comboBox_G_f3 = new QComboBox(page);
        comboBox_G_f3->setObjectName(QStringLiteral("comboBox_G_f3"));
        comboBox_G_f3->setGeometry(QRect(150, 60, 51, 22));
        comboBox_E_f5 = new QComboBox(page);
        comboBox_E_f5->setObjectName(QStringLiteral("comboBox_E_f5"));
        comboBox_E_f5->setGeometry(QRect(290, 110, 51, 22));
        comboBox_E_f1 = new QComboBox(page);
        comboBox_E_f1->setObjectName(QStringLiteral("comboBox_E_f1"));
        comboBox_E_f1->setGeometry(QRect(10, 110, 51, 22));
        comboBox_E_f2 = new QComboBox(page);
        comboBox_E_f2->setObjectName(QStringLiteral("comboBox_E_f2"));
        comboBox_E_f2->setGeometry(QRect(80, 110, 51, 22));
        comboBox_E_f4 = new QComboBox(page);
        comboBox_E_f4->setObjectName(QStringLiteral("comboBox_E_f4"));
        comboBox_E_f4->setGeometry(QRect(220, 110, 51, 22));
        comboBox_E_f3 = new QComboBox(page);
        comboBox_E_f3->setObjectName(QStringLiteral("comboBox_E_f3"));
        comboBox_E_f3->setGeometry(QRect(150, 110, 51, 22));
        comboBox_R_f3 = new QComboBox(page);
        comboBox_R_f3->setObjectName(QStringLiteral("comboBox_R_f3"));
        comboBox_R_f3->setGeometry(QRect(150, 155, 51, 22));
        comboBox_R_f2 = new QComboBox(page);
        comboBox_R_f2->setObjectName(QStringLiteral("comboBox_R_f2"));
        comboBox_R_f2->setGeometry(QRect(80, 155, 51, 22));
        comboBox_R_f1 = new QComboBox(page);
        comboBox_R_f1->setObjectName(QStringLiteral("comboBox_R_f1"));
        comboBox_R_f1->setGeometry(QRect(10, 155, 51, 22));
        select_frequence->addWidget(page);
        page_2 = new QWidget();
        page_2->setObjectName(QStringLiteral("page_2"));
        BDSIF_1fre = new QComboBox(page_2);
        BDSIF_1fre->setObjectName(QStringLiteral("BDSIF_1fre"));
        BDSIF_1fre->setGeometry(QRect(60, 10, 87, 31));
        BDSIF_2fre = new QComboBox(page_2);
        BDSIF_2fre->setObjectName(QStringLiteral("BDSIF_2fre"));
        BDSIF_2fre->setGeometry(QRect(230, 10, 87, 31));
        GPSIF_2fre = new QComboBox(page_2);
        GPSIF_2fre->setObjectName(QStringLiteral("GPSIF_2fre"));
        GPSIF_2fre->setGeometry(QRect(230, 60, 87, 31));
        GPSIF_1fre = new QComboBox(page_2);
        GPSIF_1fre->setObjectName(QStringLiteral("GPSIF_1fre"));
        GPSIF_1fre->setGeometry(QRect(60, 60, 87, 31));
        GLEIF_2fre = new QComboBox(page_2);
        GLEIF_2fre->setObjectName(QStringLiteral("GLEIF_2fre"));
        GLEIF_2fre->setGeometry(QRect(230, 100, 87, 31));
        GLEIF_1fre = new QComboBox(page_2);
        GLEIF_1fre->setObjectName(QStringLiteral("GLEIF_1fre"));
        GLEIF_1fre->setGeometry(QRect(60, 100, 87, 31));
        GLOIF_2fre = new QComboBox(page_2);
        GLOIF_2fre->setObjectName(QStringLiteral("GLOIF_2fre"));
        GLOIF_2fre->setGeometry(QRect(230, 150, 87, 31));
        GLOIF_1fre = new QComboBox(page_2);
        GLOIF_1fre->setObjectName(QStringLiteral("GLOIF_1fre"));
        GLOIF_1fre->setGeometry(QRect(60, 150, 87, 31));
        label = new QLabel(page_2);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(0, 10, 51, 31));
        label_17 = new QLabel(page_2);
        label_17->setObjectName(QStringLiteral("label_17"));
        label_17->setGeometry(QRect(0, 60, 51, 31));
        label_18 = new QLabel(page_2);
        label_18->setObjectName(QStringLiteral("label_18"));
        label_18->setGeometry(QRect(0, 100, 51, 31));
        label_19 = new QLabel(page_2);
        label_19->setObjectName(QStringLiteral("label_19"));
        label_19->setGeometry(QRect(0, 150, 51, 31));
        label_20 = new QLabel(page_2);
        label_20->setObjectName(QStringLiteral("label_20"));
        label_20->setGeometry(QRect(160, 150, 51, 31));
        label_21 = new QLabel(page_2);
        label_21->setObjectName(QStringLiteral("label_21"));
        label_21->setGeometry(QRect(160, 100, 51, 31));
        label_22 = new QLabel(page_2);
        label_22->setObjectName(QStringLiteral("label_22"));
        label_22->setGeometry(QRect(160, 10, 51, 31));
        label_23 = new QLabel(page_2);
        label_23->setObjectName(QStringLiteral("label_23"));
        label_23->setGeometry(QRect(160, 60, 51, 31));
        select_frequence->addWidget(page_2);
        groupBox_10 = new QGroupBox(SetWidget);
        groupBox_10->setObjectName(QStringLiteral("groupBox_10"));
        groupBox_10->setGeometry(QRect(800, 30, 251, 211));
        label_6 = new QLabel(groupBox_10);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(20, 30, 41, 16));
        ELEspinBox = new QSpinBox(groupBox_10);
        ELEspinBox->setObjectName(QStringLiteral("ELEspinBox"));
        ELEspinBox->setGeometry(QRect(190, 30, 46, 22));
        ELEspinBox->setMaximum(90);
        ELEspinBox->setValue(10);
        angle_of_altitude = new QSlider(groupBox_10);
        angle_of_altitude->setObjectName(QStringLiteral("angle_of_altitude"));
        angle_of_altitude->setGeometry(QRect(80, 30, 101, 22));
        angle_of_altitude->setMaximum(90);
        angle_of_altitude->setPageStep(30);
        angle_of_altitude->setValue(10);
        angle_of_altitude->setSliderPosition(10);
        angle_of_altitude->setOrientation(Qt::Horizontal);
        label_5 = new QLabel(groupBox_10);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(20, 70, 31, 21));
        SNRspinBox_2 = new QSpinBox(groupBox_10);
        SNRspinBox_2->setObjectName(QStringLiteral("SNRspinBox_2"));
        SNRspinBox_2->setGeometry(QRect(190, 70, 46, 22));
        SNRspinBox_2->setMaximum(90);
        SNRspinBox_2->setValue(30);
        SNR_of_min = new QSlider(groupBox_10);
        SNR_of_min->setObjectName(QStringLiteral("SNR_of_min"));
        SNR_of_min->setGeometry(QRect(80, 70, 101, 22));
        SNR_of_min->setMaximum(90);
        SNR_of_min->setPageStep(30);
        SNR_of_min->setValue(30);
        SNR_of_min->setSliderPosition(30);
        SNR_of_min->setOrientation(Qt::Horizontal);
        cycle_slip_detection = new QCheckBox(groupBox_10);
        cycle_slip_detection->setObjectName(QStringLiteral("cycle_slip_detection"));
        cycle_slip_detection->setGeometry(QRect(20, 100, 201, 31));
        cycle_slip_detection->setChecked(true);
        robust_estimation = new QComboBox(groupBox_10);
        robust_estimation->setObjectName(QStringLiteral("robust_estimation"));
        robust_estimation->setGeometry(QRect(20, 170, 191, 31));
        Robust_filter = new QCheckBox(groupBox_10);
        Robust_filter->setObjectName(QStringLiteral("Robust_filter"));
        Robust_filter->setGeometry(QRect(20, 140, 131, 19));
        groupBox_5 = new QGroupBox(SetWidget);
        groupBox_5->setObjectName(QStringLiteral("groupBox_5"));
        groupBox_5->setGeometry(QRect(20, 480, 1031, 121));
        select_path = new QPushButton(groupBox_5);
        select_path->setObjectName(QStringLiteral("select_path"));
        select_path->setGeometry(QRect(740, 30, 271, 31));
        observation_file_path = new QLineEdit(groupBox_5);
        observation_file_path->setObjectName(QStringLiteral("observation_file_path"));
        observation_file_path->setGeometry(QRect(10, 30, 711, 31));
        result_file_path = new QLineEdit(groupBox_5);
        result_file_path->setObjectName(QStringLiteral("result_file_path"));
        result_file_path->setGeometry(QRect(10, 80, 711, 31));
        select_save_file_path = new QPushButton(groupBox_5);
        select_save_file_path->setObjectName(QStringLiteral("select_save_file_path"));
        select_save_file_path->setGeometry(QRect(740, 80, 271, 31));
        groupBox_6 = new QGroupBox(SetWidget);
        groupBox_6->setObjectName(QStringLiteral("groupBox_6"));
        groupBox_6->setGeometry(QRect(340, 260, 121, 151));
        sys_error_select = new QComboBox(groupBox_6);
        sys_error_select->setObjectName(QStringLiteral("sys_error_select"));
        sys_error_select->setGeometry(QRect(10, 30, 87, 31));
        GNSS_AR_select = new QCheckBox(groupBox_6);
        GNSS_AR_select->setObjectName(QStringLiteral("GNSS_AR_select"));
        GNSS_AR_select->setGeometry(QRect(10, 70, 101, 19));
        SRC_Ps = new QCheckBox(groupBox_6);
        SRC_Ps->setObjectName(QStringLiteral("SRC_Ps"));
        SRC_Ps->setGeometry(QRect(10, 110, 81, 16));
        run_engineering_2 = new QPushButton(SetWidget);
        run_engineering_2->setObjectName(QStringLiteral("run_engineering_2"));
        run_engineering_2->setGeometry(QRect(1110, 510, 221, 31));
        save_config = new QPushButton(SetWidget);
        save_config->setObjectName(QStringLiteral("save_config"));
        save_config->setGeometry(QRect(1110, 560, 221, 31));
        groupBox_8 = new QGroupBox(SetWidget);
        groupBox_8->setObjectName(QStringLiteral("groupBox_8"));
        groupBox_8->setGeometry(QRect(1090, 260, 251, 151));
        Reinitalization_timebox = new QCheckBox(groupBox_8);
        Reinitalization_timebox->setObjectName(QStringLiteral("Reinitalization_timebox"));
        Reinitalization_timebox->setGeometry(QRect(10, 30, 111, 20));
        Reinitalization_time = new QComboBox(groupBox_8);
        Reinitalization_time->setObjectName(QStringLiteral("Reinitalization_time"));
        Reinitalization_time->setGeometry(QRect(120, 30, 111, 21));
        mult_station = new QCheckBox(groupBox_8);
        mult_station->setObjectName(QStringLiteral("mult_station"));
        mult_station->setGeometry(QRect(10, 70, 221, 19));
        mult_station->setChecked(true);
        mult_day = new QCheckBox(groupBox_8);
        mult_day->setObjectName(QStringLiteral("mult_day"));
        mult_day->setGeometry(QRect(10, 100, 211, 16));
        mult_day->setChecked(true);
        groupBox_11 = new QGroupBox(SetWidget);
        groupBox_11->setObjectName(QStringLiteral("groupBox_11"));
        groupBox_11->setGeometry(QRect(20, 420, 461, 61));
        WL_constraint = new QCheckBox(groupBox_11);
        WL_constraint->setObjectName(QStringLiteral("WL_constraint"));
        WL_constraint->setGeometry(QRect(20, 30, 181, 16));
        Ambiguity_acquisition = new QComboBox(groupBox_11);
        Ambiguity_acquisition->setObjectName(QStringLiteral("Ambiguity_acquisition"));
        Ambiguity_acquisition->setGeometry(QRect(220, 20, 211, 31));
        groupBox_12 = new QGroupBox(SetWidget);
        groupBox_12->setObjectName(QStringLiteral("groupBox_12"));
        groupBox_12->setGeometry(QRect(510, 420, 541, 61));
        filter_dir = new QComboBox(groupBox_12);
        filter_dir->setObjectName(QStringLiteral("filter_dir"));
        filter_dir->setGeometry(QRect(30, 20, 321, 31));
        frequency_model = new QComboBox(groupBox_12);
        frequency_model->setObjectName(QStringLiteral("frequency_model"));
        frequency_model->setGeometry(QRect(370, 20, 161, 31));
        groupBox_3->raise();
        groupBox_2->raise();
        groupBox_7->raise();
        groupBox->raise();
        groupBox_4->raise();
        groupBox_9->raise();
        groupBox_10->raise();
        groupBox_5->raise();
        groupBox_6->raise();
        run_engineering_2->raise();
        save_config->raise();
        groupBox_8->raise();
        groupBox_11->raise();
        groupBox_12->raise();

        retranslateUi(SetWidget);

        select_frequence->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(SetWidget);
    } // setupUi

    void retranslateUi(QWidget *SetWidget)
    {
        SetWidget->setWindowTitle(QApplication::translate("SetWidget", "Form", 0));
        groupBox_2->setTitle(QApplication::translate("SetWidget", "Results file storage", 0));
        Bad_satellites->setText(QApplication::translate("SetWidget", "Bad_satellites", 0));
        ZTDW_Clock->setText(QApplication::translate("SetWidget", "ZTDW_Clock", 0));
        Satellite_info->setText(QApplication::translate("SetWidget", "Satellite_info", 0));
        Az_El_coff->setText(QApplication::translate("SetWidget", "Az_El_coff", 0));
        dop_satN_AR->setText(QApplication::translate("SetWidget", "dop_satN_AR", 0));
        position->setText(QApplication::translate("SetWidget", "position", 0));
        ambiguity_inf->setText(QApplication::translate("SetWidget", "amb_inf", 0));
        groupBox_3->setTitle(QApplication::translate("SetWidget", "ambiguity resolution", 0));
        positioning_method->clear();
        positioning_method->insertItems(0, QStringList()
         << QApplication::translate("SetWidget", "Float solution", 0)
         << QApplication::translate("SetWidget", "Integer clock products", 0)
         << QApplication::translate("SetWidget", "OSBs products", 0)
         << QApplication::translate("SetWidget", "UPD/FCBs products", 0)
        );
        PAR_subset->clear();
        PAR_subset->insertItems(0, QStringList()
         << QApplication::translate("SetWidget", "success rate criterion", 0)
         << QApplication::translate("SetWidget", "elevation angle strategy", 0)
         << QApplication::translate("SetWidget", "ADOP strategy", 0)
         << QApplication::translate("SetWidget", "posterior residual sequence", 0)
         << QApplication::translate("SetWidget", "full ambiguity fixed", 0)
         << QApplication::translate("SetWidget", "Diagonal var", 0)
        );
        label_8->setText(QApplication::translate("SetWidget", "product corresponding model", 0));
        label_9->setText(QApplication::translate("SetWidget", "partial ambiguity resolution", 0));
        label_3->setText(QApplication::translate("SetWidget", "Reference satellite selection", 0));
        Ref_satelite->clear();
        Ref_satelite->insertItems(0, QStringList()
         << QApplication::translate("SetWidget", "Max elevation satellite", 0)
         << QApplication::translate("SetWidget", "Minimum variance satellite", 0)
        );
        groupBox_7->setTitle(QApplication::translate("SetWidget", "delete setllites, for example G04;et", 0));
        BDS2->setText(QApplication::translate("SetWidget", "BDS-2", 0));
        BDS3->setText(QApplication::translate("SetWidget", "BDS-3", 0));
        BDS2GEO->setText(QApplication::translate("SetWidget", "BDS-2 GEO", 0));
        BDS3GEO->setText(QApplication::translate("SetWidget", "BDS-3 GEO", 0));
        delete_satellite->setText(QString());
        groupBox->setTitle(QApplication::translate("SetWidget", "PPP model", 0));
        Combination_mode->clear();
        Combination_mode->insertItems(0, QStringList()
         << QApplication::translate("SetWidget", "UDUC", 0)
         << QApplication::translate("SetWidget", "IF", 0)
        );
        fre_mode->clear();
        fre_mode->insertItems(0, QStringList()
         << QApplication::translate("SetWidget", "double-fre", 0)
         << QApplication::translate("SetWidget", "triple-fre", 0)
        );
        label_2->setText(QApplication::translate("SetWidget", "combination mode", 0));
        label_13->setText(QApplication::translate("SetWidget", "filtering mode", 0));
        motion_state->clear();
        motion_state->insertItems(0, QStringList()
         << QApplication::translate("SetWidget", "dynamic", 0)
         << QApplication::translate("SetWidget", "static", 0)
        );
        filter->clear();
        filter->insertItems(0, QStringList()
         << QApplication::translate("SetWidget", "kalman", 0)
         << QApplication::translate("SetWidget", "slsq", 0)
        );
        label_24->setText(QApplication::translate("SetWidget", "ephemeris state", 0));
        data_process_state->clear();
        data_process_state->insertItems(0, QStringList()
         << QApplication::translate("SetWidget", "post precise", 0)
        );
        groupBox_4->setTitle(QApplication::translate("SetWidget", "select system", 0));
        select_GLONASS->setText(QApplication::translate("SetWidget", "GLONASS", 0));
        select_Galileo->setText(QApplication::translate("SetWidget", "Galileo", 0));
        select_GPS->setText(QApplication::translate("SetWidget", "GPS", 0));
        select_BDS->setText(QApplication::translate("SetWidget", "BDS", 0));
        groupBox_9->setTitle(QApplication::translate("SetWidget", "select frequence", 0));
        comboBox_C_f1->clear();
        comboBox_C_f1->insertItems(0, QStringList()
         << QApplication::translate("SetWidget", "B1C", 0)
         << QApplication::translate("SetWidget", "B1I", 0)
        );
        comboBox_C_f2->clear();
        comboBox_C_f2->insertItems(0, QStringList()
         << QApplication::translate("SetWidget", "B3I", 0)
         << QApplication::translate("SetWidget", "B1I", 0)
         << QApplication::translate("SetWidget", "B2a", 0)
        );
        comboBox_C_f3->clear();
        comboBox_C_f3->insertItems(0, QStringList()
         << QApplication::translate("SetWidget", "OFF", 0)
         << QApplication::translate("SetWidget", "B3I", 0)
         << QApplication::translate("SetWidget", "B2b/B2I", 0)
         << QApplication::translate("SetWidget", "B2a", 0)
        );
        comboBox_C_f4->clear();
        comboBox_C_f4->insertItems(0, QStringList()
         << QApplication::translate("SetWidget", "OFF", 0)
        );
        comboBox_C_f5->clear();
        comboBox_C_f5->insertItems(0, QStringList()
         << QApplication::translate("SetWidget", "OFF", 0)
        );
        comboBox_G_f1->clear();
        comboBox_G_f1->insertItems(0, QStringList()
         << QApplication::translate("SetWidget", "L1", 0)
         << QApplication::translate("SetWidget", "L2", 0)
        );
        comboBox_G_f2->clear();
        comboBox_G_f2->insertItems(0, QStringList()
         << QApplication::translate("SetWidget", "L2", 0)
         << QApplication::translate("SetWidget", "L5", 0)
        );
        comboBox_G_f3->clear();
        comboBox_G_f3->insertItems(0, QStringList()
         << QApplication::translate("SetWidget", "OFF", 0)
         << QApplication::translate("SetWidget", "L5", 0)
        );
        comboBox_E_f5->clear();
        comboBox_E_f5->insertItems(0, QStringList()
         << QApplication::translate("SetWidget", "OFF", 0)
        );
        comboBox_E_f1->clear();
        comboBox_E_f1->insertItems(0, QStringList()
         << QApplication::translate("SetWidget", "E1", 0)
         << QApplication::translate("SetWidget", "E5 boc", 0)
        );
        comboBox_E_f2->clear();
        comboBox_E_f2->insertItems(0, QStringList()
         << QApplication::translate("SetWidget", "E5a", 0)
         << QApplication::translate("SetWidget", "E5 boc", 0)
         << QApplication::translate("SetWidget", "E6", 0)
         << QApplication::translate("SetWidget", "E5b", 0)
        );
        comboBox_E_f4->clear();
        comboBox_E_f4->insertItems(0, QStringList()
         << QApplication::translate("SetWidget", "OFF", 0)
        );
        comboBox_E_f3->clear();
        comboBox_E_f3->insertItems(0, QStringList()
         << QApplication::translate("SetWidget", "OFF", 0)
         << QApplication::translate("SetWidget", "E5 boc", 0)
         << QApplication::translate("SetWidget", "E5b", 0)
         << QApplication::translate("SetWidget", "E5a", 0)
        );
        comboBox_R_f3->clear();
        comboBox_R_f3->insertItems(0, QStringList()
         << QApplication::translate("SetWidget", "OFF", 0)
         << QApplication::translate("SetWidget", "L5", 0)
        );
        comboBox_R_f2->clear();
        comboBox_R_f2->insertItems(0, QStringList()
         << QApplication::translate("SetWidget", "L2", 0)
        );
        comboBox_R_f1->clear();
        comboBox_R_f1->insertItems(0, QStringList()
         << QApplication::translate("SetWidget", "L1", 0)
        );
        BDSIF_1fre->clear();
        BDSIF_1fre->insertItems(0, QStringList()
         << QApplication::translate("SetWidget", "B1I", 0)
         << QApplication::translate("SetWidget", "B2I", 0)
         << QApplication::translate("SetWidget", "B3I", 0)
         << QApplication::translate("SetWidget", "B1C", 0)
         << QApplication::translate("SetWidget", "B2a", 0)
        );
        BDSIF_2fre->clear();
        BDSIF_2fre->insertItems(0, QStringList()
         << QApplication::translate("SetWidget", "B3I", 0)
         << QApplication::translate("SetWidget", "B1I", 0)
         << QApplication::translate("SetWidget", "B2I", 0)
         << QApplication::translate("SetWidget", "B1C", 0)
         << QApplication::translate("SetWidget", "B2a", 0)
        );
        GPSIF_2fre->clear();
        GPSIF_2fre->insertItems(0, QStringList()
         << QApplication::translate("SetWidget", "L2", 0)
         << QApplication::translate("SetWidget", "L1", 0)
         << QApplication::translate("SetWidget", "L5", 0)
        );
        GPSIF_1fre->clear();
        GPSIF_1fre->insertItems(0, QStringList()
         << QApplication::translate("SetWidget", "L1", 0)
         << QApplication::translate("SetWidget", "L2", 0)
         << QApplication::translate("SetWidget", "L5", 0)
        );
        GLEIF_2fre->clear();
        GLEIF_2fre->insertItems(0, QStringList()
         << QApplication::translate("SetWidget", "E5a", 0)
         << QApplication::translate("SetWidget", "E1", 0)
         << QApplication::translate("SetWidget", "E5b", 0)
         << QApplication::translate("SetWidget", "E5boc", 0)
         << QApplication::translate("SetWidget", "E6", 0)
        );
        GLEIF_1fre->clear();
        GLEIF_1fre->insertItems(0, QStringList()
         << QApplication::translate("SetWidget", "E1", 0)
         << QApplication::translate("SetWidget", "E5a", 0)
         << QApplication::translate("SetWidget", "E5b", 0)
         << QApplication::translate("SetWidget", "E5 boc", 0)
         << QApplication::translate("SetWidget", "E6", 0)
        );
        GLOIF_2fre->clear();
        GLOIF_2fre->insertItems(0, QStringList()
         << QApplication::translate("SetWidget", "L2", 0)
         << QApplication::translate("SetWidget", "L1", 0)
         << QApplication::translate("SetWidget", "L3", 0)
        );
        GLOIF_1fre->clear();
        GLOIF_1fre->insertItems(0, QStringList()
         << QApplication::translate("SetWidget", "L1", 0)
         << QApplication::translate("SetWidget", "L2", 0)
         << QApplication::translate("SetWidget", "L3", 0)
        );
        label->setText(QApplication::translate("SetWidget", "first", 0));
        label_17->setText(QApplication::translate("SetWidget", "first", 0));
        label_18->setText(QApplication::translate("SetWidget", "first", 0));
        label_19->setText(QApplication::translate("SetWidget", "first", 0));
        label_20->setText(QApplication::translate("SetWidget", "second", 0));
        label_21->setText(QApplication::translate("SetWidget", "second", 0));
        label_22->setText(QApplication::translate("SetWidget", "second", 0));
        label_23->setText(QApplication::translate("SetWidget", "second", 0));
        groupBox_10->setTitle(QApplication::translate("SetWidget", "quality control", 0));
        label_6->setText(QApplication::translate("SetWidget", "ELE", 0));
        label_5->setText(QApplication::translate("SetWidget", "SNR", 0));
        cycle_slip_detection->setText(QApplication::translate("SetWidget", "cycle slip detection", 0));
        robust_estimation->clear();
        robust_estimation->insertItems(0, QStringList()
         << QApplication::translate("SetWidget", "3_cigma_criteria", 0)
        );
        Robust_filter->setText(QApplication::translate("SetWidget", "Robust filtering", 0));
        groupBox_5->setTitle(QApplication::translate("SetWidget", "Data path and data processing", 0));
        select_path->setText(QApplication::translate("SetWidget", "Select the input data folder path", 0));
        select_save_file_path->setText(QApplication::translate("SetWidget", "Select the path to save the result file", 0));
        groupBox_6->setTitle(QApplication::translate("SetWidget", "system error", 0));
        sys_error_select->clear();
        sys_error_select->insertItems(0, QStringList()
         << QApplication::translate("SetWidget", "ISBs", 0)
         << QApplication::translate("SetWidget", "Only EST", 0)
        );
        GNSS_AR_select->setText(QApplication::translate("SetWidget", "unused", 0));
        SRC_Ps->setText(QApplication::translate("SetWidget", "SRC Ps", 0));
        run_engineering_2->setText(QApplication::translate("SetWidget", "Close the window", 0));
        save_config->setText(QApplication::translate("SetWidget", "Savesave configuration", 0));
        groupBox_8->setTitle(QApplication::translate("SetWidget", "Initialization time", 0));
        Reinitalization_timebox->setText(QApplication::translate("SetWidget", "Reinit_time", 0));
        Reinitalization_time->clear();
        Reinitalization_time->insertItems(0, QStringList()
         << QApplication::translate("SetWidget", "1hour", 0)
         << QApplication::translate("SetWidget", "2hour", 0)
         << QApplication::translate("SetWidget", "3hour", 0)
         << QApplication::translate("SetWidget", "4hour", 0)
         << QApplication::translate("SetWidget", "5hour", 0)
         << QApplication::translate("SetWidget", "6hour", 0)
        );
        mult_station->setText(QApplication::translate("SetWidget", "Multi-station data processing", 0));
        mult_day->setText(QApplication::translate("SetWidget", "Multi-day data processing", 0));
        groupBox_11->setTitle(QApplication::translate("SetWidget", "pattern configuration", 0));
        WL_constraint->setText(QApplication::translate("SetWidget", "unused", 0));
        Ambiguity_acquisition->clear();
        Ambiguity_acquisition->insertItems(0, QStringList()
         << QApplication::translate("SetWidget", "poor ambiguity", 0)
        );
        groupBox_12->setTitle(QApplication::translate("SetWidget", "forward and backward filtering", 0));
        filter_dir->clear();
        filter_dir->insertItems(0, QStringList()
         << QApplication::translate("SetWidget", "forward filter", 0)
         << QApplication::translate("SetWidget", "forward and backward filter", 0)
        );
        frequency_model->clear();
        frequency_model->insertItems(0, QStringList()
         << QApplication::translate("SetWidget", "fixed-fre", 0)
         << QApplication::translate("SetWidget", "mixed-fre", 0)
        );
    } // retranslateUi

};

namespace Ui {
    class SetWidget: public Ui_SetWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETWIDGET_H
