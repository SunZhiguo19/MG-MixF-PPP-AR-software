#ifndef MYHELPER_H
#define MYHELPER_H

#include <QObject>
#include <QtGui>
#include <QtCore>
#include <QFile>
#include <QApplication>
#include <QColor>
#include <QPalette>
#include <QTranslator>
#include <QDesktopWidget>
#include "app.h"
//#include "usercontrol/frmmessagebox.h"

//// Change By Xuyinglong 2021-05-31
//#include "usercontrol/frmPasswordBox.h"

#define TIME qPrintable (QTime::currentTime().toString("HH:mm:ss"))
#define STRTIME qPrintable (QTime::currentTime().toString("HH时mm分ss秒"))
#define DATE qPrintable (QDate::currentDate().toString("yyyy-MM-dd"))
#define STRDATE qPrintable (QDate::currentDate().toString("yyyy年MM月dd日"))
#define DATETIME qPrintable (QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"))
#define STRDATETIME qPrintable (QDateTime::currentDateTime().toString("yyyy年MM月dd日HH时mm分ss秒"))

class myHelper : public QObject
{
    Q_OBJECT
public:
    explicit myHelper(QObject *parent = 0);

    //设置为开机启动
    static void AutoRunWithSystem(bool IsAutoRun, QString AppName, QString AppPath) {
        QSettings *reg = new QSettings(
//            "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
            "HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
             QSettings::NativeFormat);

        if (IsAutoRun) {
            reg->setValue(AppName, AppPath);
        } else {
            reg->setValue(AppName, "");
        }
    }
    //设置指定样式
    static void SetStyle(const QString &qssFile)
    {
        QFile file(qssFile);
        if (file.open(QFile::ReadOnly))
        {
            QString qss = QLatin1String(file.readAll());
            qApp->setStyleSheet(qss);
            file.close();
        }
    }
    static void setChinese()
    {
        QTranslator *translator = new QTranslator(qApp);
        translator->load("");
        qApp->installTranslator(translator);
    }

    static void setFont()
    {
        qApp->setFont(QFont(App::appFontName,App::AppFontSize));
    }

    //窗体居中显示
    static void formInCenter(QWidget *frm)
    {
        int frmX = frm->width();
        int frmY = frm->height();
        QDesktopWidget w;
        int deskWidth = w.availableGeometry().width();
        int deskHeight = w.availableGeometry().height();
        QPoint movePoint(deskWidth / 2 - frmX / 2, deskHeight / 2 - frmY / 2);
        frm->move(movePoint);
    }


    //文件是否存在
    static bool fileIsExist(QString strFile) {
        QFile tempFile(strFile);
        return tempFile.exists();
    }


};

#endif // MYHELPER_H
