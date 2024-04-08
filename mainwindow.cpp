#include "mainwindow.h"
#include "ui_mainwindow.h"

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

void MainWindow::csv_to_table(std::string path, QMap<QString, QMap<QString, double>> &dataframe)
{
    ifstream csv(path);

    if (!csv) {
        cout << "Could not open file! :(" << endl;
    }
    char ch;
    string buffer;
    unsigned quotes = 0, count = 0;
    string row, col, val;

    csv.ignore(1000, '\n');
    while ((ch = csv.get()) != EOF) {
        // cout << ch << endl;
        if (ch == ',' && quotes % 2 == 0) {
            switch (count % 2) {
            case 0:
                // cout << "Case 0" << endl;
                row.assign(buffer);
                // cout << "ROW: " << row << endl;
                break;
            case 1:
                // cout << "Case 1" << endl;
                col.assign(buffer);
                // cout << "COL: " << col << endl;
                break;
            }
            count += 1;
            buffer.assign("");
        }

        else if (ch == '\n') {
            val.assign(buffer);
            // cout << "VAL: " << val << endl;
            buffer.assign("");
            cout << row << " - " << col << " - " << val << endl;
            dataframe[QString::fromStdString(row)][QString::fromStdString(col)] = std::stof(val);
        }

        else if (ch == '\"') {
            quotes += 1;
        }

        else {
            buffer += ch;
        }

    }
}

void MainWindow::csv_to_df(std::string path, QMap<QString, QMap<QString, QString>> &dataframe)
{
    cout << "AAA" << endl;
    ifstream csv(path);

    if (!csv) {
        cout << "Could not open file! :(" << endl;
    }

    char buffer[1024];
    int numCols = 0;
    QVector<QString> colNames;

    csv.getline(buffer, 1024);

    for (int i = 0; i < strlen(buffer); i++){
        if (buffer[i] == ','){
            numCols ++;
        }
    }

    char * temp = strtok(buffer, ",");
    cout << temp << endl;
    for (int i = 1; i <= numCols; i++){
        temp = strtok(NULL, ",");
        colNames.append(temp);
        cout << temp << endl;
    }

    char ch;
    string strBuffer, row;
    unsigned quotes = 0, count = 0;

    QVector<QString>::Iterator iter = colNames.begin();

    while ((ch = csv.get()) != EOF) {
        if ((ch == ',' && quotes % 2 == 0)) {
            if (iter == colNames.begin()){
                row = strBuffer;
                infoDf[QString::fromStdString(row)];
                cout << "AA" << endl;
            }
            // cout << infoDf[row].first().toStdString() << endl;
             cout << strBuffer << endl;
            infoDf[QString::fromStdString(row)][*iter] = QString::fromStdString(strBuffer);
            iter++;
            strBuffer = "";
        }
        else if (ch == '\n'){
            iter = colNames.begin();
        }
        else if (ch == '\"') {
            quotes += 1;
        }


        else {
            strBuffer += ch;
        }
    }
}
