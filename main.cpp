#include <QtGui/QApplication>
#include "qjd_show_mainwindow.h"
#include <QTranslator>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    QTranslator translator;
//    translator.load("paintTest_zh.qm");       //中文
//    //translator.load("paintTest_en.qm");          //英文
//    a.installTranslator(&translator);

    qDebug()<<argc<<argv[1];
    if(argc==1) return 0;
    if(argc==3)
    {

          qjdMainWindow windows(argv[1],argv[2]);

          windows.show();
           return a.exec();
      }

    else
        return 0;


//    qjdMainWindow windows;
//    windows.show();
//    return a.exec();
}
