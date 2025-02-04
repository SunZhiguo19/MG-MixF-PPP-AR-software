/********************************************************************************
** Form generated from reading UI file 'heu_agnss.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_HEU_AGNSS_H
#define UI_HEU_AGNSS_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>
#include <widget.h>
#include "skyview.h"

QT_BEGIN_NAMESPACE

class Ui_HEU_AGNSS
{
public:
    QWidget *centralWidget;
    QLineEdit *lineEdit_x;
    QLineEdit *lineEdit_z;
    QLineEdit *lineEdit_y;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QPushButton *run_engineering;
    QGroupBox *groupBox;
    QLineEdit *lineTime;
    QLineEdit *lineTime_2;
    skyview *sky;
    Widget *widget_2;
    QTextEdit *info_textEdit;
    QGroupBox *groupBox_2;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *HEU_AGNSS)
    {
        if (HEU_AGNSS->objectName().isEmpty())
            HEU_AGNSS->setObjectName(QStringLiteral("HEU_AGNSS"));
        HEU_AGNSS->resize(1361, 914);
        QIcon icon;
        icon.addFile(QStringLiteral(":/resources/icon1.png"), QSize(), QIcon::Normal, QIcon::Off);
        HEU_AGNSS->setWindowIcon(icon);
        centralWidget = new QWidget(HEU_AGNSS);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        lineEdit_x = new QLineEdit(centralWidget);
        lineEdit_x->setObjectName(QStringLiteral("lineEdit_x"));
        lineEdit_x->setGeometry(QRect(120, 90, 331, 41));
        QFont font;
        font.setFamily(QStringLiteral("Times New Roman"));
        font.setPointSize(16);
        font.setBold(false);
        font.setWeight(50);
        lineEdit_x->setFont(font);
        lineEdit_x->setReadOnly(true);
        lineEdit_z = new QLineEdit(centralWidget);
        lineEdit_z->setObjectName(QStringLiteral("lineEdit_z"));
        lineEdit_z->setGeometry(QRect(120, 210, 331, 41));
        lineEdit_z->setFont(font);
        lineEdit_z->setReadOnly(true);
        lineEdit_y = new QLineEdit(centralWidget);
        lineEdit_y->setObjectName(QStringLiteral("lineEdit_y"));
        lineEdit_y->setGeometry(QRect(120, 150, 331, 41));
        lineEdit_y->setFont(font);
        lineEdit_y->setReadOnly(true);
        label = new QLabel(centralWidget);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(70, 90, 41, 31));
        QFont font1;
        font1.setFamily(QStringLiteral("Times New Roman"));
        font1.setPointSize(20);
        label->setFont(font1);
        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(70, 150, 41, 31));
        label_2->setFont(font1);
        label_3 = new QLabel(centralWidget);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(70, 210, 41, 31));
        label_3->setFont(font1);
        run_engineering = new QPushButton(centralWidget);
        run_engineering->setObjectName(QStringLiteral("run_engineering"));
        run_engineering->setGeometry(QRect(320, 490, 101, 31));
        QFont font2;
        font2.setFamily(QStringLiteral("Times New Roman"));
        font2.setPointSize(18);
        run_engineering->setFont(font2);
        groupBox = new QGroupBox(centralWidget);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setGeometry(QRect(40, 30, 451, 251));
        QFont font3;
        font3.setFamily(QStringLiteral("Times New Roman"));
        font3.setPointSize(14);
        groupBox->setFont(font3);
        lineTime = new QLineEdit(centralWidget);
        lineTime->setObjectName(QStringLiteral("lineTime"));
        lineTime->setGeometry(QRect(70, 350, 411, 41));
        lineTime->setFont(font);
        lineTime->setReadOnly(true);
        lineTime_2 = new QLineEdit(centralWidget);
        lineTime_2->setObjectName(QStringLiteral("lineTime_2"));
        lineTime_2->setGeometry(QRect(70, 400, 411, 41));
        lineTime_2->setFont(font);
        lineTime_2->setReadOnly(true);
        sky = new skyview(centralWidget);
        sky->setObjectName(QStringLiteral("sky"));
        sky->setGeometry(QRect(590, 40, 731, 361));
        widget_2 = new Widget(centralWidget);
        widget_2->setObjectName(QStringLiteral("widget_2"));
        widget_2->setGeometry(QRect(590, 440, 731, 331));
        info_textEdit = new QTextEdit(widget_2);
        info_textEdit->setObjectName(QStringLiteral("info_textEdit"));
        info_textEdit->setGeometry(QRect(0, 0, 731, 331));
        groupBox_2 = new QGroupBox(centralWidget);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        groupBox_2->setGeometry(QRect(40, 310, 451, 151));
        groupBox_2->setFont(font3);
        HEU_AGNSS->setCentralWidget(centralWidget);
        groupBox_2->raise();
        groupBox->raise();
        lineEdit_x->raise();
        lineEdit_z->raise();
        lineEdit_y->raise();
        label->raise();
        label_2->raise();
        label_3->raise();
        run_engineering->raise();
        lineTime->raise();
        lineTime_2->raise();
        sky->raise();
        widget_2->raise();
        menuBar = new QMenuBar(HEU_AGNSS);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1361, 21));
        HEU_AGNSS->setMenuBar(menuBar);
        mainToolBar = new QToolBar(HEU_AGNSS);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        HEU_AGNSS->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(HEU_AGNSS);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        HEU_AGNSS->setStatusBar(statusBar);

        retranslateUi(HEU_AGNSS);

        QMetaObject::connectSlotsByName(HEU_AGNSS);
    } // setupUi

    void retranslateUi(QMainWindow *HEU_AGNSS)
    {
        HEU_AGNSS->setWindowTitle(QApplication::translate("HEU_AGNSS", "HEU_AGNSS", 0));
        label->setText(QApplication::translate("HEU_AGNSS", "X", 0));
        label_2->setText(QApplication::translate("HEU_AGNSS", "Y", 0));
        label_3->setText(QApplication::translate("HEU_AGNSS", "Z", 0));
        run_engineering->setText(QApplication::translate("HEU_AGNSS", "run", 0));
        groupBox->setTitle(QApplication::translate("HEU_AGNSS", "WGS 84", 0));
        groupBox_2->setTitle(QApplication::translate("HEU_AGNSS", "GPS Time", 0));
    } // retranslateUi

};

namespace Ui {
    class HEU_AGNSS: public Ui_HEU_AGNSS {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_HEU_AGNSS_H
