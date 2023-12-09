#include "mainwindow.h"

#include <QApplication>

//Для получения всех необходимых библиотек через windeployqt можно указать следующие аргументы:
//windeployqt.exe /path/to/proj/ --no-compiler-runtime --no-translations --no-system-d3d-compiler --no-opengl-sw --no-pdf --no-svg --no-network
//Инсталлер можно создать через QtInstallerFramework

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
