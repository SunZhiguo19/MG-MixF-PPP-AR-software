#include "skyview.h"

skyview::skyview(QWidget *parent) : QWidget(parent)
{
    colorCircleRadius=this->width()>this->height()?this->height()*0.45:this->width()*0.45;
    outerCircleRadius=colorCircleRadius*0.95;
    middleCircleRadius=outerCircleRadius*0.92;
    innerCircleRadius=outerCircleRadius/3;
    satRadius=innerCircleRadius*0.2;
    center=this->rect().center();

    course=0;
    numberSat=0;
}

void skyview::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)

}

void skyview::paintEvent(QPaintEvent *)
{
    /*绘制背景颜色*/
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    /************/
    QPainter painter;
    painter.begin(this);
    painter.setRenderHints(QPainter::Antialiasing);
    resetVariables(&painter);
//    drawColorPies(&painter);
    drawOuterCircle(&painter);
    drawMiddleCircle(&painter);
    drawInnerCircle(&painter);
    drawCoordinate(&painter);
//    drawCourse(&painter,course);
    drawSat(&painter);
    drawline(&painter);//
    drawStatus(&painter);
    drawdop(&painter);
    painter.end();
}

void skyview::resetVariables(QPainter *painter)
{
    Q_UNUSED(painter);
    colorCircleRadius=this->width()>this->height()?this->height()*0.45:this->width()*0.45;
    outerCircleRadius=colorCircleRadius*0.95;
    middleCircleRadius=outerCircleRadius*0.92;
    innerCircleRadius=outerCircleRadius/3;
    satRadius=innerCircleRadius*0.15;
    center=this->rect().center();
}

void skyview::drawColorPies(QPainter* painter)
{
    painter->save();
    QPointF topLeftPointF(center.x()-colorCircleRadius,center.y()-colorCircleRadius);
    QPointF bottomRightPoointF(center.x()+colorCircleRadius,center.y()+colorCircleRadius);
    QRectF colorPieRect=QRectF(topLeftPointF,bottomRightPoointF);
    painter->setPen(Qt::black);
    painter->drawRect(this->rect());
    painter->drawEllipse(center,colorCircleRadius,colorCircleRadius);
    painter->setPen(Qt::NoPen);
    for(int i=0;i<36;i++)
    {
        if(i%2)
        {
            painter->setBrush(inner);
        }
        else
        {
            painter->setBrush(outer);
        }
        painter->drawPie(colorPieRect,(90+i*10)*16,10*16);
    }
    painter->restore();
}

void skyview::drawOuterCircle(QPainter *painter)
{
    painter->save();
    painter->setPen(line);
    painter->setBrush(outer);
    painter->drawEllipse(center,outerCircleRadius,outerCircleRadius);
    painter->restore();
}

void skyview::drawMiddleCircle(QPainter *painter)
{
    painter->save();
    painter->setPen(Qt::NoPen);
    painter->setBrush(inner);
    painter->drawEllipse(center,middleCircleRadius,middleCircleRadius);
    painter->restore();
}
void skyview::drawInnerCircle(QPainter *painter)
{
    painter->save();
    painter->setPen(line);
    for(int i=1;i<3;i++)
    {
        painter->drawEllipse(center,innerCircleRadius*i,innerCircleRadius*i);
    }
    painter->restore();
}

void skyview::drawCoordinate(QPainter *painter)
{
  painter->save();
  QLineF acrossLine(center.x()- 0.9*outerCircleRadius,center.y(),center.x()+0.9*outerCircleRadius,center.y());
  QLineF verticalLine(center.x(),center.y()-0.9*outerCircleRadius,center.x(),center.y()+0.9*outerCircleRadius);
  QLineF a45(center.x()- 0.65*outerCircleRadius,center.y()- 0.65*outerCircleRadius,center.x()+0.65*outerCircleRadius,center.y()+ 0.65*outerCircleRadius); //45
  QLineF a135(center.x()- 0.65*outerCircleRadius,center.y()+0.65*outerCircleRadius,center.x()+0.65*outerCircleRadius,center.y()- 0.65*outerCircleRadius); //135
  qreal rectWidth=colorCircleRadius-outerCircleRadius;
  painter->setPen(line);
  painter->drawLine(acrossLine);
  painter->drawLine(verticalLine);
  painter->drawLine(a45);
  painter->drawLine(a135);
  painter->translate(center);
  painter->setPen(QPen(line,1));
  QFont font=painter->font();
  font.setBold(true);
  //font.setItalic(true);    //设置字体为斜体
  font.setPointSize(rectWidth);
  qreal pointSize=font.pointSize();
  painter->setFont(font);
//    painter->drawText(textRectF(innerCircleRadius,-90,pointSize*1.4),Qt::AlignCenter,"60");
//    painter->drawText(textRectF(2*innerCircleRadius,-90,pointSize*1.4),Qt::AlignCenter,"30");
  painter->drawText(textRectF(0.92*outerCircleRadius,-90,pointSize*1.4),Qt::AlignCenter,"N");
  painter->drawText(textRectF(1.0*outerCircleRadius,90,pointSize*1.4),Qt::AlignCenter,"S");
  painter->drawText(textRectF(0.95*outerCircleRadius,0,pointSize*1.4),Qt::AlignCenter,"E");
  painter->drawText(textRectF(0.95*outerCircleRadius,180,pointSize*1.4),Qt::AlignCenter,"W");
  painter->restore();
}

QRectF skyview::textRectF(double radius, int angle,qreal pointSize)
{
    QRectF rectf;
    rectf.setX(radius*qCos(angle*D2R)-2*pointSize);
    rectf.setY(radius*qSin(angle*D2R)-pointSize);
    rectf.setWidth(pointSize*4);
    rectf.setHeight(pointSize);
    return rectf;
}

QString skyview::satNo2Id(char sys_tem,int prn)
{
    QString id;
    switch (sys_tem)
    {
        case 'G':
            id=QString("G%01").arg(prn);
            return id;
        case 'C':
            id=QString("C%01").arg(prn);
            return id;
        case 'R':
            id=QString("R%01").arg(prn);
            return id;
        case 'E':
            id=QString("E%01").arg(prn);
            return id;
        default:
            id=" ";
            return id;
    }
}

void skyview::drawSat(QPainter *painter)
{
    painter->save();
    QFont font("Times New Roman");//Times New Roman
    //卫星在坐标系的位置
    QPointF satCenter;
    //卫星导卫星视图坐标系的距离
    qreal o2s;
    font.setPointSize(satRadius*0.7);   //卫星编号的字体大小
    font.setBold(true);
    //font.setItalic(true);    //设置卫星编号的字体为斜体
    //转换坐标系转换到卫星视图中心
    painter->translate(center);
    disply_satellite *sate_disp = AGNSS_APPData::sGetAPPDataInstance()->mGettrans_sate_dispData();
    for(int i=0;i<sate_disp->az.length();i++)
    {
        if(sate_disp->el[i]>90||sate_disp->el[i]<=0) continue;

        // Change By Xuyinglong 2021-05-31
        // 判断卫星显示状态
        QString sat_id = satNo2Id(sate_disp->sat_sys[i],sate_disp->sat_prn[i]);

        o2s=(1.0-(double)sate_disp->el[i]/90)*outerCircleRadius;
        satCenter.setX(o2s*qSin(sate_disp->az[i]));
        satCenter.setY(-o2s*qCos(sate_disp->az[i]));
        //将卫星视图坐标系装换到卫星坐标系(以卫星所在位置为原点)
        painter->save();
        painter->translate(satCenter);
        QPointF beginPointF(0,0);
        QPointF endPointF(-satRadius*0.5,-satRadius*0.5);
        //渐进填充
        QRadialGradient radialGradient(beginPointF,satRadius,endPointF);
        radialGradient.setColorAt(0.0,QColor(187,185,185));
        if(!sate_disp->sat_flag[i])
            radialGradient.setColorAt(1.0,satColor(sate_disp->sat_sys[i]));
        else
            radialGradient.setColorAt(1.0,QColor(128,138,135));
        painter->setPen(Qt::NoPen);
        painter->setBrush(radialGradient);
        painter->drawEllipse(beginPointF,satRadius,satRadius);
        //画卫星号
        QPointF topLeftPointF(-satRadius,-satRadius);
        QPointF bottomRightPointF(satRadius,satRadius);
        QRectF textRectF(topLeftPointF,bottomRightPointF);
        painter->setFont(font);
        painter->setPen(Qt::white);
        painter->drawText(textRectF,Qt::AlignCenter,sat_id);
        painter->restore();
    }
    painter->restore();
}

void skyview::drawStatus(QPainter *painter)
{
    // Change By Xuyinglong 2021-05-31
    // 卫星数无效状态
    disply_satellite *sate_disp = AGNSS_APPData::sGetAPPDataInstance()->mGettrans_sate_dispData();
    QString bdsSatCount = QString::number(sate_disp->bdsSatNum[1]);
    QString gpsSatCount = QString::number(sate_disp->gpsSatNum[1]);
    QString gloSatCount = QString::number(sate_disp->gloSatNum[1]);
    QString galSatCount = QString::number(sate_disp->galSatNum[1]);
    QFont font(tr("Times New Roman"),20);
    int FontSize;
    FontSize = changeSize(20);
    font.setPointSize(FontSize);
    font.setBold(true);
    painter->save();
    painter->setPen(number);
    painter->setFont(font);


    painter->drawText(5,30,"BDS ");
    painter->drawText(70,30,bdsSatCount+"/"+QString::number(sate_disp->bdsSatNum[0]));
    painter->drawText(5,60,"GPS ");
    painter->drawText(70,60,gpsSatCount+"/"+QString::number(sate_disp->gpsSatNum[0]));
    painter->drawText(5,90,"GLO ");
    painter->drawText(70,90,gloSatCount+"/"+QString::number(sate_disp->gloSatNum[0]));
    painter->drawText(5,120,"GAL ");
    painter->drawText(70,120,galSatCount+"/"+QString::number(sate_disp->galSatNum[0]));

    painter->restore();
}
void skyview::drawdop(QPainter *painter)
{

    disply_satellite *sate_disp = AGNSS_APPData::sGetAPPDataInstance()->mGettrans_sate_dispData();
    QFont font(tr("Times New Roman"),20);
    int FontSize;
    FontSize = changeSize(20);
    font.setPointSize(FontSize);
    font.setBold(true);
    painter->save();
    painter->setPen(number);
    painter->setFont(font);
    painter->drawText(5,320,"HDOP:");
    painter->drawText(120,320,QString::number(sate_disp->HDOP, 'f', 2));
    painter->drawText(500,30,"GDOP:");
    painter->drawText(620,30,QString::number(sate_disp->GDOP, 'f', 2));
    painter->drawText(500,90,"PDOP:");
    painter->drawText(620,90,QString::number(sate_disp->PDOP, 'f', 2));
    painter->drawText(500,320,"VDOP:");
    painter->drawText(620,320,QString::number(sate_disp->VDOP, 'f', 2));

    painter->restore();
}
void skyview::drawCourse(QPainter *painter, qreal course)
{
    // Change By Xuyinglong 2021-04-26
    painter->save();

    float scale = 15.0;                 // 缩放比例

    int origin_ship_width = 8;          // 8米
    int origin_ship_height = 25;        // 25米

    // 缩放后船宽高
    float vessel_width = origin_ship_width*scale;
    float vessel_height = origin_ship_height*scale;

    float ship_width = vessel_width;
    float ship_top_height = vessel_height * 0.2;
    float ship_bottom_height = vessel_height * 0.8;
    float ship_arc = 16;

    /*船航向线*/
    painter->translate(center);
    painter->rotate(course);

    // 船半身矩形
    // 船上半身圆弧
    QRectF ship_pie(-(ship_width / 2), -(vessel_height / 2), ship_width, ship_top_height * 2);

    // 船半身矩形
    QRectF ship_rect(-(ship_width / 2), -(vessel_height / 2 - ship_top_height) - 0.5, ship_width, ship_bottom_height + 0.5);

    painter->setBrush(QColor(120, 120, 120, 200));
    painter->setPen(Qt::NoPen);
    painter->drawPie(ship_pie, 0, 180 * ship_arc);
    painter->drawRect(ship_rect);

    /**********/
    // Change By Xuyinglong 2021-05-31
    // 绘制方向
    painter->restore();

    // 标注航向角度
    painter->save();
    painter->translate(center);
    painter->rotate(course);

    qreal radius = colorCircleRadius - outerCircleRadius;
    QPointF trianglePoint[3] =
    {
        QPointF(0, -outerCircleRadius),
        QPointF(radius*0.5, -colorCircleRadius),
        QPointF(-radius*0.5, -colorCircleRadius)
    };

    painter->setBrush(guide);
    painter->drawPolygon(trianglePoint, 3);

    QFont font("Times New Roman");
    font.setBold(true);
    font.setPointSize(radius);
    painter->setPen(number);
    painter->setFont(font);

    painter->save();

    // 调节航向显示框位置
    {
        qreal unit = radius/9;

        if (course >= 0 && course < 10)
        {
            painter->translate(-3 * unit, -colorCircleRadius - 3 * unit);
        }
        if (course >= 10 && course < 20)
        {
            painter->translate(-5 * unit, -colorCircleRadius - 2 * unit);
        }
        if (course >= 20 && course < 30)
        {
            painter->translate(-7 * unit, -colorCircleRadius - 2 * unit);
        }
        if (course >= 30 && course < 40)
        {
            painter->translate(-6 * unit, -colorCircleRadius - 2 * unit);
        }
        if (course >= 40 && course < 50)
        {
            painter->translate(-4 * unit, -colorCircleRadius - 1 * unit);
        }
        if (course >= 50 && course < 60)
        {
            painter->translate(-1 * unit, -colorCircleRadius - 2 * unit);
        }
        if (course >= 60 && course < 70)
        {
            painter->translate(2 * unit, -colorCircleRadius - 2 * unit);
        }
        if (course >= 70 && course < 80)
        {
            painter->translate(4 * unit, -colorCircleRadius - 3 * unit);
        }
        if (course >= 80 && course < 90)
        {
            painter->translate(5 * unit, -colorCircleRadius - 3.5 * unit);
        }
        if (course >= 90 && course < 100)
        {
            painter->translate(5 * unit, -colorCircleRadius - 4 * unit);
        }
        if (course >= 100 && course < 110)
        {
            painter->translate(5 * unit, -colorCircleRadius - 4 * unit);
        }
        if (course >= 110 && course < 120)
        {
            painter->translate(6 * unit, -colorCircleRadius - 4.5 * unit);
        }
        if (course >= 120 && course < 130)
        {
            painter->translate(6 * unit, -colorCircleRadius - 6 * unit);
        }
        if (course >= 130 && course < 140)
        {
            painter->translate(7 * unit, -colorCircleRadius - 8 * unit);
        }
        if (course >= 140 && course < 150)
        {
            painter->translate(10 * unit, -colorCircleRadius - 10 * unit);
        }
        if (course >= 150 && course < 160)
        {
            painter->translate(13 * unit, -colorCircleRadius - 10 * unit);
        }
        if (course >= 160 && course < 170)
        {
            painter->translate(13 * unit, -colorCircleRadius - 11 * unit);
        }
        if ( course >= 170 && course < 180)
        {
            painter->translate(13 * unit, -colorCircleRadius - 12 * unit);
        }
        if (course >= 180 && course < 185)
        {
            painter->translate(12 * unit, -colorCircleRadius - 14 * unit);
        }
        if (course >= 185 && course < 190)
        {
            painter->translate(12 * unit, -colorCircleRadius - 15 * unit);
        }
        if (course >= 190 && course < 195)
        {
            painter->translate(11 * unit, -colorCircleRadius - 16 * unit);
        }
        if (course >= 195 && course < 200)
        {
            painter->translate(10 * unit, -colorCircleRadius - 17 * unit);
        }
        if (course >= 200 && course < 205)
        {
            painter->translate(10 * unit, -colorCircleRadius - 19 * unit);
        }
        if (course >= 205 && course < 210)
        {
            painter->translate(8 * unit, -colorCircleRadius - 21 * unit);
        }
        if (course >= 210 && course < 215)
        {
            painter->translate(8 * unit, -colorCircleRadius - 21 * unit);
        }
        if (course >= 215 && course < 220)
        {
            painter->translate(6 * unit, -colorCircleRadius - 23 * unit);
        }
        if (course >= 220 && course < 225)
        {
            painter->translate(6 * unit, -colorCircleRadius - 24 * unit);
        }
        if (course >= 225 && course < 230)
        {
            painter->translate(6 * unit, -colorCircleRadius - 26 * unit);
        }
        if (course >= 230 && course < 235)
        {
            painter->translate(6 * unit, -colorCircleRadius - 26 * unit);
        }
        if (course >= 235 && course < 240)
        {
            painter->translate(6 * unit, -colorCircleRadius - 27 * unit);
        }
        if (course >= 240 && course < 245)
        {
            painter->translate(4 * unit, -colorCircleRadius - 27 * unit);
        }
        if (course >= 245 && course < 250)
        {
            painter->translate(4 * unit, -colorCircleRadius - 27 * unit);
        }
        if (course >= 250 && course < 255)
        {
            painter->translate(2 * unit, -colorCircleRadius - 27 * unit);
        }
        if (course >= 255 && course < 260)
        {
            painter->translate(-1 * unit, -colorCircleRadius - 27 * unit);
        }
        if (course >= 260 && course < 265)
        {
            painter->translate(-2 * unit, -colorCircleRadius - 27 * unit);
        }
        if (course >= 265 && course < 270)
        {
            painter->translate(-4 * unit, -colorCircleRadius - 27 * unit);
        }
        if (course >= 270 && course < 275)
        {
            painter->translate(-6 * unit, -colorCircleRadius - 27 * unit);
        }
        if (course >= 275 && course < 280)
        {
            painter->translate(-8 * unit, -colorCircleRadius - 27 * unit);
        }
        if (course >= 280 && course < 285)
        {
            painter->translate(-10 * unit, -colorCircleRadius - 26 * unit);
        }
        if (course >= 285 && course < 290)
        {
            painter->translate(-11 * unit, -colorCircleRadius - 25 * unit);
        }
        if (course >= 290 && course < 295)
        {
            painter->translate(-12 * unit, -colorCircleRadius - 24 * unit);
        }
        if (course >= 295 && course < 300)
        {
            painter->translate(-10 * unit, -colorCircleRadius - 23 * unit);
        }
        if (course >= 300 && course < 305)
        {
            painter->translate(-9 * unit, -colorCircleRadius - 25 * unit);
        }
        if (course >= 305 && course < 310)
        {
            painter->translate(-9 * unit, -colorCircleRadius - 21 * unit);
        }
        if (course >= 310 && course < 315)
        {
            painter->translate(-11 * unit, -colorCircleRadius - 19 * unit);
        }
        if (course >= 315 && course < 320)
        {
            painter->translate(-12 * unit, -colorCircleRadius - 18 * unit);
        }
        if (course >= 320 && course < 325)
        {
            painter->translate(-13 * unit, -colorCircleRadius - 17 * unit);
        }
        if (course >= 325 && course < 330)
        {
            painter->translate(-14 * unit, -colorCircleRadius - 16 * unit);
        }
        if (course >= 330 && course < 335)
        {
            painter->translate(-14 * unit, -colorCircleRadius - 15 * unit);
        }
        if (course >= 335 && course < 340)
        {
            painter->translate(-14 * unit, -colorCircleRadius - 13 * unit);
        }
        if (course >= 340 && course < 345)
        {
            painter->translate(-15 * unit, -colorCircleRadius - 11 * unit);
        }
        if (course >= 345 && course < 350)
        {
            painter->translate(-14 * unit, -colorCircleRadius - 8 * unit);
        }
        if (course >= 350 && course < 355)
        {
            painter->translate(-14 * unit, -colorCircleRadius - 6 * unit);
        }
        if (course >= 355 && course < 360)
        {
            painter->translate(-14 * unit, -colorCircleRadius - 4 * unit);
        }

        painter->rotate(-course);
        painter->drawText(0,0, QString::number(course));
        painter->restore();
    }
    // 恢复中心点
    painter->translate(-center);

    painter->restore();
}

void skyview::drawline(QPainter*painter)//1
{
    painter->save();
    //平台宽
    qreal shipWidth=innerCircleRadius*0.2;
    //平台高
    qreal shipHeight=shipWidth*3;
    painter->translate(center);
    painter->setPen(number);
//    painter->drawLine(0,-shipHeight*4.6,0,shipHeight*4.6);
//    painter->drawLine(-shipHeight*4.6,0,shipHeight*4.6,0);
    painter->setBrush(QColor(255,255,255,0));
//    painter->drawEllipse(-shipHeight*5/3,-shipHeight*5/3,shipHeight*10/3,shipHeight*10/3);
//    painter->drawEllipse(-shipHeight*4.6,-shipHeight*4.6,shipHeight*9.2,shipHeight*9.2);
//    painter->drawEllipse(-shipHeight*4.6*0.725,-shipHeight*4.6*0.725,shipHeight*9.2*0.725,shipHeight*9.2*0.725);
    painter->restore();
}
QColor skyview::satColor(char system_sat)
{
    QColor color;
    switch(system_sat)
    {
        case 'G':
            color.setRgb(30,144,255);
            return color;
        case 'C':
            color.setRgb(27,130,139);
            return color;
        case 'R':
            color.setRgb(0,100,0);
            return color;
        case 'E':
            color.setRgb(255,0,0);
            return color;
        default:
            color.setRgb(0,0,128);
            return color;
    }
}




void skyview::updataSky(QString sog)
{
    course=sog.toDouble();
//    txtSog->setText(sog+QString("°"));
//    txtCog->setText(cog);
//    txtHdop->setText(hdop);
    this->update();
}
int skyview::changeSize( int data_radio)//szg
{
    /*****比例参数设定*****/
    int fontssize;
    int fontsize;
    fontssize = this->width();
    float latradio  = fontssize/914.0;
    fontsize  = data_radio*latradio;
    return fontsize;

}
