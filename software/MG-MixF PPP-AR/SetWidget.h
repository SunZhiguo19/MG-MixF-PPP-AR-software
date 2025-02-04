#ifndef SETWIDGET_H
#define SETWIDGET_H

#include <QWidget>
#include <QMainWindow>
#include <qmath.h>
#include <QDialog>
#include <QFileDialog>
#include <QIcon>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include "filter_alg/coefficient_matrix.h"
#include "pos_alg/coordinate_trans.h"
#include "pos_alg/search_file.h"
#include "ppp_model/real_ppp_ar.h"
#include "ppp_model/post_ppp_ar.h"
#include "ppp_model/post_ppp.h"
#include "autoresize/autoresize.h"
#include "AGNSS_APPData.h"
#include "ConfTranIni.h"


namespace Ui {
class SetWidget;
}

class SetWidget : public QWidget
{
   Q_OBJECT

public:
    explicit SetWidget(QWidget *parent = 0);
    ~SetWidget();
    void parameter_setting(corr_opt *opt_corr, para_trans *trans_para, file_dat *dat_fil);
    ConfTranIni TranIni;

private:
    Ui::SetWidget *ui;
    AutoResize *m_autoResize_setwidget_Handler;
private:
    void resizeEvent(QResizeEvent *event);
    void showEvent(QShowEvent *event);
    QString files_path;
    QString result_files_path;
    QString m_sys_str;
    corr_opt *opt_corr;
    para_trans *trans_para;
    file_dat *dat_fil;
    int treat_method;
    void init();

public slots:
    void save_config();
    void select_Path();
    void select_result_file_Path();
private slots:
    void changeDisplay_fre();


};

#endif // SETWIDGET_H
