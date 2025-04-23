#include "mainwindow.h"

#include "color_scheme.hpp"

#include <QApplication>
#include <QFile>
#include <QPalette>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

#ifdef __linux__
    QCoreApplication::setAttribute(Qt::AA_DontUseNativeDialogs);
#endif

    QApplication::setStyle(QStyleFactory::create("fusion"));

    QPalette palette = create_palette();

    QApplication::setPalette(palette);

    MainWindow w;

    //QFile file(":/cameraviewer.qss");
    //file.open(QFile::ReadOnly);
    //QString styleSheet { QLatin1String(file.readAll()) };
    //a.setStyleSheet(styleSheet);

    w.show();
    return a.exec();
}
