#include "mainwindow.h"
#include <QApplication>
#include "comm/config/cfgsetting.h"
int main(int argc, char *argv[])
{
    CfgSetting::getInstance();
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
