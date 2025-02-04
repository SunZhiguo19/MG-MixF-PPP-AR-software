#ifndef SKYVIEW_H
#define SKYVIEW_H

#include <QWidget>
#include <QObject>
#include <QDebug>

#include <QPaintEvent>
#include <QPainter>
#include <QtMath>
#include <QTimer>
#include <QTime>
#include <QList>
#include <QLabel>
#include <QLineEdit>
#include <QStyleOption>
#include <QtGlobal>
#include <QAction>
#include "AGNSS_APPData.h"



class skyview: public QWidget
{
    Q_OBJECT

public:
     explicit skyview (QWidget *parent = 0);

protected:
    void paintEvent(QPaintEvent*);
    void resizeEvent(QResizeEvent *event);

public:
    void updataSky(QString sog);
    QString satNo2Id(char sys_tem, int prn);

//public slots:
//    void changeStyle();


private:
    QColor outer=QColor(227,227,227);
    QColor inner=QColor(235,235,235);
    QColor line=QColor(139,139,139);
    QColor number=Qt::black;
    QColor guide=QColor(64,64,64);


private:
    //最外层颜色环的半径
    qreal colorCircleRadius;
    //外圆半径
    qreal outerCircleRadius;
    //中间圆环半径
    qreal middleCircleRadius;
    //内圆半径
    qreal innerCircleRadius;
    //卫星半径
    qreal satRadius;

    //坐标系中心
    QPointF center;

    //复位所有变量
    void resetVariables(QPainter*);
    //画外环的饼状图
    void drawColorPies(QPainter*);
    //画外圆
    void drawOuterCircle(QPainter*);
    //画中间圆形
    void drawMiddleCircle(QPainter*);
    //画内部两圆
    void drawInnerCircle(QPainter*);
    //画坐标系
    void drawCoordinate(QPainter*);
    //画航向
    void drawCourse(QPainter*,qreal angle);
    void drawline(QPainter*);//画小圆的两条线
    //画卫星
    void drawSat(QPainter*);
    void drawdop(QPainter *painter);
    //画状态信息
    void drawStatus(QPainter*);

    //返回文本框矩形的大小
    QRectF textRectF(double radius, int angle,qreal pointSize);

    int changeSize( int data_radio);//szg

private:
    qreal course;
    //接收到的卫星数
    int numberSat;
    //返回卫星的颜色
    QColor satColor(char system_sat);
    /*数组第一位有效卫星数，第二位总共卫星数*/
    int gpsSatNum[2];
    int bdsSatNum[2];
    int gloSatNum[2];
    /************************/




};

#endif // SKYVIEW_H
