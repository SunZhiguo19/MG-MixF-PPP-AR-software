#ifndef APP_H
#define APP_H
#include <QSettings>
#include <QString>

class App
{
public:
    static QString appPath;
    static int AppFontSize;                 //应用程序字体大小
    static QString appFontName;             //应用程序字体名称

    static int IcoMain;                     //左上角图标
    static int IcoMenu;                     //下来菜单图标
    static int IcoMin;                      //最小化图标
    static int IcoMax;                      //最大化图标
    static int IcoNormal;                   //正常图标
    static int IcoClose;                    //关闭图标

    static int deskWidth;
    static int deskHeight;
    static QString appTitle;
    static QString appStyle;                //界面样式


};

#endif // APP_H
