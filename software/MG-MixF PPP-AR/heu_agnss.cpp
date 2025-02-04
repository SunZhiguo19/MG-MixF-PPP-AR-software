#include "heu_agnss.h"
#include "ui_heu_agnss.h"

HEU_AGNSS::HEU_AGNSS(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::HEU_AGNSS)
{
    ui->setupUi(this);
    init_config();

    //  DOP显示
    QBarSet *set0 = new QBarSet("Jane");


    set0->append( 10 );


    QBarSeries *series = new QBarSeries();
    series->append(set0);


    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Simple barchart example");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    m_autoResize_Handler=new AutoResize(this,this->rect().width(),this->rect().height());
    m_autoResize_Handler->setAutoResizeFlag(
            AutoResize::INCLUDE_BUTTON|AutoResize::INCLUDE_COMBOBOX|
            AutoResize::INCLUDE_EDITOR|AutoResize::INCLUDE_LABEL
            );

  //  m_autoResize_Handler->pushAllResizeItem();
    m_autoResize_Handler->addOtherItem(ui->menuBar);
    m_autoResize_Handler->addOtherItem(ui->centralWidget);
    m_autoResize_Handler->addOtherItem(ui->widget_2);
    m_autoResize_Handler->addOtherItem(ui->sky);
    m_autoResize_Handler->addOtherItem(ui->groupBox);

    m_autoResize_Handler->pushAllResizeItem();



}
void HEU_AGNSS::resizeEvent(QResizeEvent *event)//*****szg** start****//
{
    Q_UNUSED(event)
    //***** sunzg*****//
    int Zsize;


    Zsize = changeSize(13);
    QFont thdchangefont;


//QLineEdit



    thdchangefont=ui->lineTime->font();
    thdchangefont.setPointSize(Zsize);
    ui->lineTime->setFont(thdchangefont);//改变字体大小
    thdchangefont=ui->lineTime_2->font();
    thdchangefont.setPointSize(Zsize);
    ui->lineTime_2->setFont(thdchangefont);//改变字体大小



    thdchangefont=ui->lineEdit_x->font();
    thdchangefont.setPointSize(Zsize);
    ui->lineEdit_x->setFont(thdchangefont);//改变字体大小
    thdchangefont=ui->lineEdit_y->font();
    thdchangefont.setPointSize(Zsize);
    ui->lineEdit_y->setFont(thdchangefont);//改变字体大小
    thdchangefont=ui->lineEdit_z->font();
    thdchangefont.setPointSize(Zsize);
    ui->lineEdit_z->setFont(thdchangefont);//改变字体大小
}


HEU_AGNSS::~HEU_AGNSS()
{
    delete ui;
}
/**
 * @brief HSSTDP1MainWindow::showEvent  窗口显示事件
 * @param event 显示事件
 */
void HEU_AGNSS::showEvent(QShowEvent *event)
{
    this->update();
}
void HEU_AGNSS::init_config()
{
    // fix windows
    setFixedSize(this->width(), this->height());
//    setWindowIcon(QIcon(":/icon1.png"));
    setWindowTitle("MG-MixF PPP-AR v1.0.0");

    // menuBar
    m_otherMenu = menuBar()->addMenu("&Tools");

    // Set window
    QString SetPath = m_App_floder + "images/SetWidget.ico";
    QAction *SetAct = new QAction(tr("&SetWidget"),this);
    SetAct->setStatusTip(tr("SetWidget MG-MixF PPP-AR."));
    SetAct->setIcon(QIcon(SetPath));
    connect(SetAct,SIGNAL(triggered()), &m_SetWidget,SLOT(show()));
    m_otherMenu->addAction(SetAct);

    // Configure window
    QString cofPath = m_App_floder + "images/config.ico";
    QAction *configureAct = new QAction(tr("&Configure"),this);
    configureAct->setStatusTip(tr("Configure MG-MixF PPP-AR."));
    configureAct->setIcon(QIcon(cofPath));
    connect(configureAct,SIGNAL(triggered()), &m_ConfigWidget,SLOT(show()));
    m_otherMenu->addAction(configureAct);

    //开始
    sysPostpppRun=new post_ppp(this);
    connect(this, SIGNAL(run_postppp()), sysPostpppRun, SLOT(sysMain()));
    sysRealppparRun = new real_ppp_ar(this);
    connect(this, SIGNAL(run_real_pppar()), sysRealppparRun, SLOT(sysMain()));
    sysPostppparRun = new post_ppp_ar(this);
    connect(this, SIGNAL(run_postpppar()), sysPostppparRun, SLOT(sysMain()));
    sysUDUCppparRun = new uduc_ppp_ar(this);
    connect(this, SIGNAL(run_uducpppar()), sysUDUCppparRun, SLOT(sysMain()));
    //位置显示
    connect(sysPostpppRun,SIGNAL(changexyz(wigdet_transmit)), this,SLOT(changePos(wigdet_transmit)));
    connect(sysRealppparRun,SIGNAL(changexyz(wigdet_transmit)), this,SLOT(changePos(wigdet_transmit)));
    connect(sysPostppparRun,SIGNAL(changexyz(wigdet_transmit)), this,SLOT(changePos(wigdet_transmit)));
    connect(sysUDUCppparRun,SIGNAL(changexyz(wigdet_transmit)), this,SLOT(changePos(wigdet_transmit)));
    connect(ui->run_engineering, SIGNAL(clicked(bool)), this, SLOT(run_sys_caul()));


}
void HEU_AGNSS::run_sys_caul()
{
    corr_opt *opt_corr = AGNSS_APPData::sGetAPPDataInstance()->mGetopt_corrData();
    switch (opt_corr->treat_method) {
    case POS_PP_PPP:
        emit(run_postppp());
        break;
    case POS_PP_PPP_AR_1:
    case POS_PP_PPP_AR_2:
        emit(run_postpppar());
        break;
    case POS_Real_PPP_AR:
        emit(run_real_pppar());
        break;
    case POS_UDUC_PPP_AR:
        emit(run_uducpppar());
        break;
    default:
        break;
    }
}

void HEU_AGNSS::changePos(wigdet_transmit result)
{
    mutex.lock();
//  坐标显示
    QPalette px = ui->lineEdit_x->palette();
    QPalette py = ui->lineEdit_y->palette();
    QPalette pz = ui->lineEdit_z->palette();

    px.setColor(QPalette::Base,QColor(255,0,0,0));
    py.setColor(QPalette::Base,QColor(255,0,0,0));
    pz.setColor(QPalette::Base,QColor(255,0,0,0));

    ui->lineEdit_x->setPalette(px);
    ui->lineEdit_y->setPalette(py);
    ui->lineEdit_z->setPalette(pz);

    ui->lineEdit_x->setText(QString::number(result.transmit_xyz[0],'f',3));
    ui->lineEdit_y->setText(QString::number(result.transmit_xyz[1],'f',3));
    ui->lineEdit_z->setText(QString::number(result.transmit_xyz[2],'f',3));

//  时间显示
    QDateTime CurrentTime=QDateTime::currentDateTime();
    QString dateStr=CurrentTime.toString("yyyy/MM/dd"); //设置显示的格式
    QString timeStr=CurrentTime.toString("  hh:mm:ss"); //设置显示的格式
    ui->lineTime->setText(dateStr);
    ui->lineTime_2->setText(timeStr);

    double ep[6]={0,0,0,0,0,0};

    ep[0]= result.Year;
    ep[1]= result.Month;
    ep[2]= result.Day;
    ep[3]= result.Hours;
    ep[4]= result.Minutes;
    ep[5]= result.Seconds;

    result.data=QString("%1"" %2"" %3"" %4"" %5").arg(ep[0]).arg(tr("/")).arg((int)ep[1],2, 10,QChar('0')).arg(tr("/")).arg((int)ep[2]);
    result.time=QString("    %1"" %2"" %3"" %4"" %5").arg((int)ep[3],2, 10,QChar('0')).arg(tr(":"))
            .arg((int)ep[4],2, 10,QChar('0')).arg(tr(":")).arg((int)ep[5]);

    ui->lineTime->setText(result.data);
    ui->lineTime_2->setText(result.time);

//天空试图
    ui->sky->updataSky(result.sog);

//误差曲线图
//    QStringList data_text = result.data_text;

    int data_size = result.data_text.size();
    if (data_size > 0)
    {
        for(int i=0; i<data_size; i++)
        {
            QString aaa=result.data_text.at(i);
            ui->widget_2->autoScrollTextEdit(ui->info_textEdit,aaa);
        }
    }
    mutex.unlock();
//    if(result.epoch == 0)
//    ui->widget_2->on_pushButton_clicked();

//    ui->widget_2->drawline(result);
}

int HEU_AGNSS::changeSize( int data_radio)//szg
{
    /*****比例参数设定*****/
    int fontssize;
    int fontsize;
    fontssize = this->width();
    float latradio  = fontssize/914.0;
    fontsize  = data_radio*latradio;
    return fontsize;

}

