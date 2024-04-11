#include "ddswindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ddsWindow w;
    w.show();
    return a.exec();
}
