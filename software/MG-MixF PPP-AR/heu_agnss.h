#ifndef HEU_AGNSS_H
#define HEU_AGNSS_H
#include <QMainWindow>
#include <qmath.h>
#include <QDialog>
#include <QBarSet>
#include <QBarSeries>
#include <QChart>
#include <QChartView>
#include <QLineSeries>
#include <QValueAxis>
#include <QMutex>

#include <QFileDialog>
#include "filter_alg/coefficient_matrix.h"
#include "pos_alg/coordinate_trans.h"
#include "pos_alg/search_file.h"
#include "ppp_model/real_ppp_ar.h"
#include "ppp_model/post_ppp_ar.h"
#include "ppp_model/post_ppp.h"
#include "ppp_model/uduc_ppp_ar.h"
#include "ConfigWidget.h"
#include "SetWidget.h"
#include "ppp_model/post_ppp.h"
#include <QTimer>
#include <QThread>
#include <QThreadPool>
#include "skyview.h"
#include "widget.h"
#include "autoresize/autoresize.h"
#define REPAINT_TIME 20
QT_CHARTS_USE_NAMESPACE


namespace Ui {
class HEU_AGNSS;
}

class HEU_AGNSS : public QMainWindow
{
    Q_OBJECT

public:
    explicit HEU_AGNSS(QWidget *parent = 0);
    ~HEU_AGNSS();
    SetWidget m_SetWidget;


public:
      post_ppp *sysPostpppRun;
      post_ppp_ar *sysPostppparRun;
      real_ppp_ar *sysRealppparRun;
      uduc_ppp_ar *sysUDUCppparRun;
      QList<QPointF> mydata1;


private:
    Ui::HEU_AGNSS *ui;
    QMutex mutex; //分配空间
    // menu bar
    QMenu *m_otherMenu;
    QString m_station_path, m_App_floder;// obsvertion path
    ConfigWidget m_ConfigWidget;
    void showEvent(QShowEvent *event);
    void init_config();
    void resizeEvent(QResizeEvent *event);
    QChartView * MyChartView;   // 画布对象
    QChart MyChart;             // 图表对象



public slots:
    void changePos(wigdet_transmit);
    void run_sys_caul();
signals:
    void run_postppp();
    void run_postpppar();
    void run_real_pppar();
    void run_uducpppar();


private:
    QString files_path;
    AutoResize *m_autoResize_Handler;
    //比例参数计算
    int changeSize( int data_radio);//szg

};

#endif // HEU_AGNSS_H



