/********************************************************************************
** Form generated from reading UI file 'ConfigWidget.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONFIGWIDGET_H
#define UI_CONFIGWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ConfigWidget
{
public:
    QGroupBox *groupBox_del_2;
    QPlainTextEdit *Qw_pos;
    QLabel *Qw_label;
    QPlainTextEdit *Qw_zwd;
    QLabel *Pk_label;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QPlainTextEdit *Qw_clk;
    QLabel *label_4;
    QPlainTextEdit *Qw_amb;
    QLabel *label_5;
    QPlainTextEdit *Qw_ion;
    QPlainTextEdit *Pk_amb;
    QPlainTextEdit *Pk_pos;
    QPlainTextEdit *Pk_ion;
    QPlainTextEdit *Pk_clk;
    QPlainTextEdit *Pk_zwd;
    QLabel *label_6;
    QPlainTextEdit *L_precision;
    QPlainTextEdit *P_precision;
    QPlainTextEdit *P_precision_2;
    QLabel *label_8;
    QPlainTextEdit *L_precision_2;
    QPlainTextEdit *P_precision_3;
    QPlainTextEdit *L_precision_3;
    QLabel *label_9;
    QPlainTextEdit *P_precision_4;
    QPlainTextEdit *L_precision_4;
    QPlainTextEdit *P_precision_5;
    QPlainTextEdit *L_precision_5;
    QLabel *label_10;
    QLabel *label_11;
    QPushButton *pushButton_cancel;
    QPushButton *pushButton_ok;
    QGroupBox *groupBox;
    QCheckBox *Helmert_VC;
    QGroupBox *groupBox_2;
    QComboBox *stochastic_model;
    QGroupBox *groupBox_3;
    QLabel *label_7;
    QPlainTextEdit *Ratio_value;
    QGroupBox *groupBox_4;
    QPlainTextEdit *WL_weight;
    QGroupBox *groupBox_5;
    QPlainTextEdit *NL_cutoff;

    void setupUi(QWidget *ConfigWidget)
    {
        if (ConfigWidget->objectName().isEmpty())
            ConfigWidget->setObjectName(QString::fromUtf8("ConfigWidget"));
        ConfigWidget->resize(782, 613);
        QFont font;
        font.setFamily(QString::fromUtf8("Times New Roman"));
        font.setPointSize(10);
        ConfigWidget->setFont(font);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/resources/icon3.png"), QSize(), QIcon::Normal, QIcon::Off);
        ConfigWidget->setWindowIcon(icon);
        groupBox_del_2 = new QGroupBox(ConfigWidget);
        groupBox_del_2->setObjectName(QString::fromUtf8("groupBox_del_2"));
        groupBox_del_2->setGeometry(QRect(20, 20, 721, 241));
        QFont font1;
        font1.setFamily(QString::fromUtf8("Times New Roman"));
        font1.setPointSize(10);
        font1.setBold(true);
        font1.setWeight(75);
        groupBox_del_2->setFont(font1);
        Qw_pos = new QPlainTextEdit(groupBox_del_2);
        Qw_pos->setObjectName(QString::fromUtf8("Qw_pos"));
        Qw_pos->setGeometry(QRect(255, 40, 85, 31));
        QFont font2;
        font2.setFamily(QString::fromUtf8("Times New Roman"));
        font2.setPointSize(10);
        font2.setBold(false);
        font2.setWeight(50);
        Qw_pos->setFont(font2);
        Qw_label = new QLabel(groupBox_del_2);
        Qw_label->setObjectName(QString::fromUtf8("Qw_label"));
        Qw_label->setGeometry(QRect(50, 45, 201, 16));
        Qw_zwd = new QPlainTextEdit(groupBox_del_2);
        Qw_zwd->setObjectName(QString::fromUtf8("Qw_zwd"));
        Qw_zwd->setGeometry(QRect(352, 40, 85, 30));
        Qw_zwd->setFont(font2);
        Pk_label = new QLabel(groupBox_del_2);
        Pk_label->setObjectName(QString::fromUtf8("Pk_label"));
        Pk_label->setGeometry(QRect(10, 85, 251, 16));
        label = new QLabel(groupBox_del_2);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(252, 20, 91, 20));
        QFont font3;
        font3.setFamily(QString::fromUtf8("Times New Roman"));
        font3.setPointSize(9);
        font3.setBold(false);
        font3.setWeight(50);
        label->setFont(font3);
        label_2 = new QLabel(groupBox_del_2);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(377, 20, 31, 16));
        label_2->setFont(font3);
        label_3 = new QLabel(groupBox_del_2);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(470, 20, 31, 16));
        label_3->setFont(font3);
        Qw_clk = new QPlainTextEdit(groupBox_del_2);
        Qw_clk->setObjectName(QString::fromUtf8("Qw_clk"));
        Qw_clk->setGeometry(QRect(445, 40, 85, 30));
        Qw_clk->setFont(font2);
        label_4 = new QLabel(groupBox_del_2);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(568, 20, 31, 16));
        label_4->setFont(font3);
        Qw_amb = new QPlainTextEdit(groupBox_del_2);
        Qw_amb->setObjectName(QString::fromUtf8("Qw_amb"));
        Qw_amb->setGeometry(QRect(539, 40, 85, 30));
        Qw_amb->setFont(font2);
        label_5 = new QLabel(groupBox_del_2);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(658, 20, 31, 16));
        label_5->setFont(font3);
        Qw_ion = new QPlainTextEdit(groupBox_del_2);
        Qw_ion->setObjectName(QString::fromUtf8("Qw_ion"));
        Qw_ion->setGeometry(QRect(632, 40, 85, 30));
        Qw_ion->setFont(font2);
        Pk_amb = new QPlainTextEdit(groupBox_del_2);
        Pk_amb->setObjectName(QString::fromUtf8("Pk_amb"));
        Pk_amb->setGeometry(QRect(539, 80, 85, 30));
        Pk_amb->setFont(font2);
        Pk_pos = new QPlainTextEdit(groupBox_del_2);
        Pk_pos->setObjectName(QString::fromUtf8("Pk_pos"));
        Pk_pos->setGeometry(QRect(255, 80, 85, 31));
        Pk_pos->setFont(font2);
        Pk_ion = new QPlainTextEdit(groupBox_del_2);
        Pk_ion->setObjectName(QString::fromUtf8("Pk_ion"));
        Pk_ion->setGeometry(QRect(632, 80, 85, 30));
        Pk_ion->setFont(font2);
        Pk_clk = new QPlainTextEdit(groupBox_del_2);
        Pk_clk->setObjectName(QString::fromUtf8("Pk_clk"));
        Pk_clk->setGeometry(QRect(445, 80, 85, 30));
        Pk_clk->setFont(font2);
        Pk_zwd = new QPlainTextEdit(groupBox_del_2);
        Pk_zwd->setObjectName(QString::fromUtf8("Pk_zwd"));
        Pk_zwd->setGeometry(QRect(352, 80, 85, 30));
        Pk_zwd->setFont(font2);
        label_6 = new QLabel(groupBox_del_2);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(20, 123, 341, 21));
        L_precision = new QPlainTextEdit(groupBox_del_2);
        L_precision->setObjectName(QString::fromUtf8("L_precision"));
        L_precision->setGeometry(QRect(360, 120, 71, 31));
        L_precision->setFont(font2);
        P_precision = new QPlainTextEdit(groupBox_del_2);
        P_precision->setObjectName(QString::fromUtf8("P_precision"));
        P_precision->setGeometry(QRect(440, 120, 71, 31));
        P_precision->setFont(font2);
        P_precision_2 = new QPlainTextEdit(groupBox_del_2);
        P_precision_2->setObjectName(QString::fromUtf8("P_precision_2"));
        P_precision_2->setGeometry(QRect(440, 150, 71, 31));
        P_precision_2->setFont(font2);
        label_8 = new QLabel(groupBox_del_2);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setGeometry(QRect(20, 160, 341, 21));
        L_precision_2 = new QPlainTextEdit(groupBox_del_2);
        L_precision_2->setObjectName(QString::fromUtf8("L_precision_2"));
        L_precision_2->setGeometry(QRect(360, 150, 71, 31));
        L_precision_2->setFont(font2);
        P_precision_3 = new QPlainTextEdit(groupBox_del_2);
        P_precision_3->setObjectName(QString::fromUtf8("P_precision_3"));
        P_precision_3->setGeometry(QRect(630, 150, 61, 31));
        P_precision_3->setFont(font2);
        L_precision_3 = new QPlainTextEdit(groupBox_del_2);
        L_precision_3->setObjectName(QString::fromUtf8("L_precision_3"));
        L_precision_3->setGeometry(QRect(560, 150, 61, 31));
        L_precision_3->setFont(font2);
        label_9 = new QLabel(groupBox_del_2);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setGeometry(QRect(20, 190, 341, 21));
        P_precision_4 = new QPlainTextEdit(groupBox_del_2);
        P_precision_4->setObjectName(QString::fromUtf8("P_precision_4"));
        P_precision_4->setGeometry(QRect(440, 180, 71, 31));
        P_precision_4->setFont(font2);
        L_precision_4 = new QPlainTextEdit(groupBox_del_2);
        L_precision_4->setObjectName(QString::fromUtf8("L_precision_4"));
        L_precision_4->setGeometry(QRect(360, 180, 71, 31));
        L_precision_4->setFont(font2);
        P_precision_5 = new QPlainTextEdit(groupBox_del_2);
        P_precision_5->setObjectName(QString::fromUtf8("P_precision_5"));
        P_precision_5->setGeometry(QRect(630, 180, 61, 31));
        P_precision_5->setFont(font2);
        L_precision_5 = new QPlainTextEdit(groupBox_del_2);
        L_precision_5->setObjectName(QString::fromUtf8("L_precision_5"));
        L_precision_5->setGeometry(QRect(560, 180, 61, 31));
        L_precision_5->setFont(font2);
        label_10 = new QLabel(groupBox_del_2);
        label_10->setObjectName(QString::fromUtf8("label_10"));
        label_10->setGeometry(QRect(520, 160, 31, 21));
        label_11 = new QLabel(groupBox_del_2);
        label_11->setObjectName(QString::fromUtf8("label_11"));
        label_11->setGeometry(QRect(520, 190, 31, 21));
        pushButton_cancel = new QPushButton(ConfigWidget);
        pushButton_cancel->setObjectName(QString::fromUtf8("pushButton_cancel"));
        pushButton_cancel->setGeometry(QRect(640, 560, 93, 28));
        pushButton_ok = new QPushButton(ConfigWidget);
        pushButton_ok->setObjectName(QString::fromUtf8("pushButton_ok"));
        pushButton_ok->setGeometry(QRect(510, 560, 93, 28));
        groupBox = new QGroupBox(ConfigWidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(20, 270, 281, 80));
        Helmert_VC = new QCheckBox(groupBox);
        Helmert_VC->setObjectName(QString::fromUtf8("Helmert_VC"));
        Helmert_VC->setGeometry(QRect(10, 30, 171, 19));
        groupBox_2 = new QGroupBox(ConfigWidget);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setGeometry(QRect(20, 360, 721, 80));
        stochastic_model = new QComboBox(groupBox_2);
        stochastic_model->addItem(QString());
        stochastic_model->addItem(QString());
        stochastic_model->addItem(QString());
        stochastic_model->addItem(QString());
        stochastic_model->setObjectName(QString::fromUtf8("stochastic_model"));
        stochastic_model->setGeometry(QRect(30, 30, 301, 31));
        groupBox_3 = new QGroupBox(ConfigWidget);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        groupBox_3->setGeometry(QRect(20, 460, 341, 80));
        label_7 = new QLabel(groupBox_3);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(20, 40, 101, 21));
        Ratio_value = new QPlainTextEdit(groupBox_3);
        Ratio_value->setObjectName(QString::fromUtf8("Ratio_value"));
        Ratio_value->setGeometry(QRect(120, 40, 104, 31));
        groupBox_4 = new QGroupBox(ConfigWidget);
        groupBox_4->setObjectName(QString::fromUtf8("groupBox_4"));
        groupBox_4->setGeometry(QRect(320, 270, 201, 80));
        WL_weight = new QPlainTextEdit(groupBox_4);
        WL_weight->setObjectName(QString::fromUtf8("WL_weight"));
        WL_weight->setGeometry(QRect(50, 30, 85, 31));
        WL_weight->setFont(font2);
        groupBox_5 = new QGroupBox(ConfigWidget);
        groupBox_5->setObjectName(QString::fromUtf8("groupBox_5"));
        groupBox_5->setGeometry(QRect(540, 270, 201, 80));
        NL_cutoff = new QPlainTextEdit(groupBox_5);
        NL_cutoff->setObjectName(QString::fromUtf8("NL_cutoff"));
        NL_cutoff->setGeometry(QRect(50, 30, 85, 31));
        NL_cutoff->setFont(font2);

        retranslateUi(ConfigWidget);

        QMetaObject::connectSlotsByName(ConfigWidget);
    } // setupUi

    void retranslateUi(QWidget *ConfigWidget)
    {
        ConfigWidget->setWindowTitle(QCoreApplication::translate("ConfigWidget", "ConfigWidget", nullptr));
        groupBox_del_2->setTitle(QCoreApplication::translate("ConfigWidget", "Set Parameters(m^2/30s)", nullptr));
        Qw_pos->setPlainText(QCoreApplication::translate("ConfigWidget", "1e4", nullptr));
        Qw_label->setText(QCoreApplication::translate("ConfigWidget", "Transfer noise (Qw):", nullptr));
        Qw_zwd->setPlainText(QCoreApplication::translate("ConfigWidget", "3e-8", nullptr));
        Pk_label->setText(QCoreApplication::translate("ConfigWidget", "Initial covariance (Pk):", nullptr));
        label->setText(QCoreApplication::translate("ConfigWidget", "dynamic_pos", nullptr));
        label_2->setText(QCoreApplication::translate("ConfigWidget", "zwd", nullptr));
        label_3->setText(QCoreApplication::translate("ConfigWidget", "clk", nullptr));
        Qw_clk->setPlainText(QCoreApplication::translate("ConfigWidget", "1e6", nullptr));
        label_4->setText(QCoreApplication::translate("ConfigWidget", "amb", nullptr));
        Qw_amb->setPlainText(QCoreApplication::translate("ConfigWidget", "1e-16", nullptr));
        label_5->setText(QCoreApplication::translate("ConfigWidget", "ion", nullptr));
        Qw_ion->setPlainText(QCoreApplication::translate("ConfigWidget", "0.1", nullptr));
        Pk_amb->setPlainText(QCoreApplication::translate("ConfigWidget", "1e6", nullptr));
        Pk_pos->setPlainText(QCoreApplication::translate("ConfigWidget", "1e3", nullptr));
        Pk_ion->setPlainText(QCoreApplication::translate("ConfigWidget", "10", nullptr));
        Pk_clk->setPlainText(QCoreApplication::translate("ConfigWidget", "1e6", nullptr));
        Pk_zwd->setPlainText(QCoreApplication::translate("ConfigWidget", "10", nullptr));
        label_6->setText(QCoreApplication::translate("ConfigWidget", "Carrier/Pseudorange precision (m):G", nullptr));
        L_precision->setPlainText(QCoreApplication::translate("ConfigWidget", "0.03", nullptr));
        P_precision->setPlainText(QCoreApplication::translate("ConfigWidget", "3.0", nullptr));
        P_precision_2->setPlainText(QCoreApplication::translate("ConfigWidget", "3.0", nullptr));
        label_8->setText(QCoreApplication::translate("ConfigWidget", "Carrier/Pseudorange precision (m):C2", nullptr));
        L_precision_2->setPlainText(QCoreApplication::translate("ConfigWidget", "0.03", nullptr));
        P_precision_3->setPlainText(QCoreApplication::translate("ConfigWidget", "3.0", nullptr));
        L_precision_3->setPlainText(QCoreApplication::translate("ConfigWidget", "0.03", nullptr));
        label_9->setText(QCoreApplication::translate("ConfigWidget", "Carrier/Pseudorange precision (m):E", nullptr));
        P_precision_4->setPlainText(QCoreApplication::translate("ConfigWidget", "3.0", nullptr));
        L_precision_4->setPlainText(QCoreApplication::translate("ConfigWidget", "0.03", nullptr));
        P_precision_5->setPlainText(QCoreApplication::translate("ConfigWidget", "3.0", nullptr));
        L_precision_5->setPlainText(QCoreApplication::translate("ConfigWidget", "0.03", nullptr));
        label_10->setText(QCoreApplication::translate("ConfigWidget", "C3", nullptr));
        label_11->setText(QCoreApplication::translate("ConfigWidget", "R", nullptr));
        pushButton_cancel->setText(QCoreApplication::translate("ConfigWidget", "Cancel", nullptr));
        pushButton_ok->setText(QCoreApplication::translate("ConfigWidget", "Ok", nullptr));
        groupBox->setTitle(QCoreApplication::translate("ConfigWidget", "Helmert variance covariance estimation", nullptr));
        Helmert_VC->setText(QCoreApplication::translate("ConfigWidget", "Helmert VC select", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("ConfigWidget", "stochastic model", nullptr));
        stochastic_model->setItemText(0, QCoreApplication::translate("ConfigWidget", "Weighted sine elevation angle", nullptr));
        stochastic_model->setItemText(1, QCoreApplication::translate("ConfigWidget", "Weighted cosine elevation angle", nullptr));
        stochastic_model->setItemText(2, QCoreApplication::translate("ConfigWidget", "Sine  elevation angle 1", nullptr));
        stochastic_model->setItemText(3, QCoreApplication::translate("ConfigWidget", "Panda stochastic model", nullptr));

        groupBox_3->setTitle(QCoreApplication::translate("ConfigWidget", "Ratio Test", nullptr));
        label_7->setText(QCoreApplication::translate("ConfigWidget", "Ratio value:", nullptr));
        Ratio_value->setPlainText(QCoreApplication::translate("ConfigWidget", "2.0", nullptr));
        groupBox_4->setTitle(QCoreApplication::translate("ConfigWidget", "WL/EWL fixed ambiguity weight", nullptr));
        WL_weight->setPlainText(QCoreApplication::translate("ConfigWidget", "1e6", nullptr));
        groupBox_5->setTitle(QCoreApplication::translate("ConfigWidget", "NL ambiguity cutoff", nullptr));
        NL_cutoff->setPlainText(QCoreApplication::translate("ConfigWidget", "0.5", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ConfigWidget: public Ui_ConfigWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONFIGWIDGET_H
