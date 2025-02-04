#ifndef CONFIGWIDGET_H
#define CONFIGWIDGET_H

#include <QWidget>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>
#include "ConfTranIni.h"
#include "filter_alg/coefficient_matrix.h"
#include "pos_alg/coordinate_trans.h"
#include "pos_alg/search_file.h"
#include "autoresize/autoresize.h"
#include "ppp_model/real_ppp_ar.h"
#include "ppp_model/post_ppp_ar.h"
#include "ppp_model/post_ppp.h"

namespace Ui {
class ConfigWidget;
}

class ConfigWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ConfigWidget(QWidget *parent = 0);
    ~ConfigWidget();
    ConfTranIni myConfTranIni;


private:
    void showEvent(QShowEvent *event);
    void initWidgt();
    void resizeEvent(QResizeEvent *event);
private slots:
    void clickOk();
    void clickCancel();

private:
    Ui::ConfigWidget *ui;
    AutoResize *m_autoResize_setwidget_Handler;
};

#endif // CONFIGWIDGET_H
