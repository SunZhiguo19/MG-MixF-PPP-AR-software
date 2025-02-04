#ifndef UDUC_PPP_AR_H
#define UDUC_PPP_AR_H


#include <QObject>
#include <QThread>
#include <QMutex>
#include "pos_algorith.h"
#include "pos_alg/search_file.h"
#include "filter_alg/srif_algorithm.h"
#include "pos_alg/trans_coor.h"
#include "filter_alg/quality_control.h"
#include "stand_pos.h"
#include "ambiguity_resolution.h"
#include "uduc_ambiguity_resolution.h"
#include "SetWidget.h"
class uduc_ppp_ar: public QThread
{
    Q_OBJECT
public:
    explicit uduc_ppp_ar(QObject *parent = 0);

    ~uduc_ppp_ar();
    void uduc_ppp_ar_calculating(const corr_opt *opt_corr, para_trans *trans_para, file_dat *dat_fil, disply_satellite *sate_disp, QStringList OFileNamesList);
private:

    Ambiguity_resolution Amb_res;
    uduc_ambiguity_resolution UDUC_AR;
public slots:
    /*程序启动入口函数*/
    void sysMain();


private:
    QMutex mutex; //分配空间
    void run();                                          // 处理数据线程执行主函数
    Parameter_transfer *Para_transfer;
signals:
    void changexyz(wigdet_transmit);
    void test(wigdet_transmit);
};

#endif // UDUC_PPP_AR_H
