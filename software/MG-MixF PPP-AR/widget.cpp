#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    init();

}

void Widget::init()
{

}


void Widget::drawline(wigdet_transmit result)
{
//    int number;
//    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
//    number = qrand() % 9;

//    m_series->append(result.epoch,number);


       QVector<QPointF> list;
       QVector<QPointF> newlist;

       list = line->pointsVector();//获取现在图中列表
       if (list.size() < 100)
       {
           //保持原来

           newlist = list;
       }
       else
       {
           //错位移动
           for(int i =1 ; i< list.size();i++)
           {
               newlist.append(QPointF(i-1,list.at(i).y()));
           }
       }
       double value = qAbs(result.transmit_xyz[0] - -2279829.0952);
       newlist.append(QPointF(newlist.size(),value));//最后补上新的数据
       line->replace(newlist);//替换更新

       this->update();

}


Widget::~Widget()
{
    delete ui;
}
// The edit box automatically scrolls, adding one row or more lines at a time.
void Widget::autoScrollTextEdit(QTextEdit *textEdit,QString &add_text)
{
    if(textEdit == NULL) return ;
    int m_Display_Max_line = 99999;
    //Add line character and refresh edit box.
    QString insertText = add_text + ENDLINE;
    textEdit->insertPlainText(insertText);
    //Keep the editor in the last line of the cursor.
    QTextCursor cursor=textEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    textEdit->setTextCursor(cursor);
    textEdit->repaint();
    //If you exceed a certain number of lines, empty it.
    if(textEdit->document()->lineCount() > m_Display_Max_line)
    {
        textEdit->clear();
    }
}
