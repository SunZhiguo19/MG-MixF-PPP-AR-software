#ifndef POST_PPP_H
#define POST_PPP_H

#include <QObject>
#include <QThread>
#include "pos_algorith.h"
#include "pos_alg/search_file.h"
#include "filter_alg/srif_algorithm.h"
#include "pos_alg/trans_coor.h"
#include "filter_alg/quality_control.h"
#include "stand_pos.h"

#include "SetWidget.h"

class post_ppp: public QThread
{
    Q_OBJECT
public:
    explicit post_ppp(QObject *parent = 0);

    ~post_ppp();
    void post_ppp_calculating(const corr_opt *opt_corr, para_trans *trans_para, file_dat *dat_fil, disply_satellite *sate_disp, QStringList OFileNamesList, bool isDisplayEveryEpoch=false);



public slots:
    /*程序启动入口函数*/
    void sysMain();


private:

    void run();                                          // 处理数据线程执行主函数
    bool pritf_Widelane(SatlitData &oneSatlitData);
    void get_smooth_Wl(QVector< SatlitData > &prevEpochSatlitData,QVector< SatlitData > &epochSatlitData);
    Parameter_transfer *Para_transfer;
    QMutex mutex; //分配空间
signals:
    void changexyz(wigdet_transmit);
    void test(wigdet_transmit);



};

#endif // POST_PPP_H
