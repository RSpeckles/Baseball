#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    // idk what this does
    QApplication a(argc, argv);

    // define window
    MainWindow window;
    window.show();

    // initialize
    window.initializeTables();

    return a.exec();
}

//Test test test
