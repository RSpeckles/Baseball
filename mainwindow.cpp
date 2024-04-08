#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "parser.h"

#include <QDir>
#include <QMapIterator>
#include <QString>
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QDir distPath;
    distPath.cdUp();

    string path = distPath.path().toStdString()
                  + "/Baseball/Distance between stadiums.csv";
    cout << path << endl;
    csv_to_table(path, distTable);

    QDir infoPath;
    infoPath.cdUp();

    path = distPath.path().toStdString()
                  + "/Baseball/MLB Information.csv";
    cout << path << endl;
    csv_to_df(path, infoDf);
}

MainWindow::~MainWindow()
{
    delete ui;
}
