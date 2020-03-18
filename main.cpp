#include "mainwindow.h"
#include <QApplication>
#include "./CV_Sources/camera.h"
#include <ctime>

int main(int argc, char *argv[])
{
    srand(time(NULL));
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}

