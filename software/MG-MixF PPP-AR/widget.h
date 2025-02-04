#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QtCharts/QChart>
#include <QtCharts\QChartView>   //两个基本模块
#include <QPointF>     //点类
#include <QList>         //列表
#include <QtCore/QTimer>   //定时器
#include <QWidget>
#include <QGraphicsItem>
#include <QSplineSeries>
#include <QValueAxis>
#include <QPen>
#include <QLegend>
#include <QLegendMarker>
#include <QBrush>
#include <QTime>
#include <QDebug>
#include <QTextEdit>
#include <QApplication>
#include <QTextCursor>
#include <QCoreApplication>
#include "pos_algorith.h"

QT_CHARTS_BEGIN_NAMESPACE
class QSplineSeries;
class QValueAxis;                 //引入这两个类而免于引入整个头文件的方法
QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE   //使用qtchart需要加入这条语句

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    QChartView *m_chartView;   //因为布局时其它函数会访问这个画布，所以设为public
    virtual ~Widget();
    void init();



public:
    void drawline(wigdet_transmit);
    void autoScrollTextEdit(QTextEdit *textEdit,QString &add_text);

//public:
//    void on_pushButton_clicked();

private:
//   // QTimer m_timer;     //定时器指针

//    QChart * m_chart;     //图表组件，可理解为画笔，用它画曲线
//    QList<QSplineSeries *> m_serieslist;   //曲线列表，splineseries为光滑曲线

//    QSplineSeries *m_series;       //曲线指针
//    QStringList m_titles;          //标题
//    QValueAxis *axisX;             //x坐标轴
//    QValueAxis *axisY;             //y坐标轴

//    qreal m_step;
//    qreal m_x;
//    qreal m_y;

    //绘图变量和坐标
    QTimer *timer_plot;
    QSplineSeries* line;
    QChart* chart;
    QValueAxis *axisX;
    QValueAxis *axisY;


private:
    Ui::Widget *ui;
};

#endif // WIDGET_H
