#include "heu_agnss.h"
#include <QApplication>
#include <QIcon>

QString current_date="yyyy-MM-dd_hh_mm_ss_zzz";
int flag =0;
void customMessageHandler(QtMsgType type, const QMessageLogContext &, const QString & str)
{
    QString txt=str;
    QString app_run_addr;
    //获取程序当前运行目录
    QString current_PathName = QCoreApplication::applicationDirPath();
    QDateTime current_date_time =QDateTime::currentDateTime();
    if (flag == 0) {
        current_date = current_date_time.toString("yyyy-MM-dd_hh_mm_ss_zzz");
        flag=1;
    }
    if(QFile::exists(current_PathName)==false)
    {
        app_run_addr="debug.log";
    }else
    {
        app_run_addr=current_PathName+"/"+"debug_"+current_date+".log";
    }
    QFile outFile(app_run_addr);
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&outFile);
    ts << txt << endl;
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    a.setWindowIcon(QIcon(":/icon1.png"));
    qInstallMessageHandler(customMessageHandler);
    HEU_AGNSS w;
    w.show();

    return a.exec();
}
