#include "ddswindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ddsWindow w;


    w.setGeometry(100,100,650,300);//设置大小
    w.show();
    return a.exec();
}
