// files
#include "mainwindow.h"
#include "parser.h"
#include "sorter.h"
#include "ui_mainwindow.h"

// librarires
#include <QDir>
#include <QMapIterator>
#include <QMessageBox>
#include <QString>
#include <QTableWidget>
#include <QDebug>

#include <iostream>
#include <string>

using namespace std;

// constructor/destructor (code inside will probably be moved once rowan is done)
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // commented out for now so that i can focus on ui
    QDir distPath;

    cout <<distPath.path().toStdString();
    distPath.cdUp();

    string path = distPath.path().toStdString() + "/Baseball/Distance between stadiums.csv";
    cout << path << endl;
    csv_to_table(path, distTable);

    QDir infoPath;
    infoPath.cdUp();

    path = distPath.path().toStdString() + "/Baseball/MLB Information.csv";
    cout << path << endl;
    csv_to_df(path, infoDf);

    cout << infoDf["Arizona Diamondbacks"]["Team name"].toStdString() << endl;
    QVector<QString> teams = sort_by_typology(infoDf, "Major");

    // for (auto &it : teams){
    //     cout << infoDf[it]["Ballpark typology"].toStdString() << endl;
    // }
}

MainWindow::~MainWindow()
{
    delete ui;
}

/*
 *  CHANGE SCREEN FUNCTION
 *
 *  This map associates button names to their corresponding desired page in the stacked widget.
 *  When any button designed to change the page is pressed, the function takes the button name
 *  and uses the map to find the desired page name.
 *
 *  To add your own button, create a connection using the signal editor that goes into the
 *  "changePage" function.
*/

map<QString, QString> buttonMap = {
    // buttons
    {"buttonDisplay", "display"},
    {"buttonPlan", "plan"},
    {"buttonAdmin", "adminlogin"},
    {"buttonStartTrip", "virtualtrip"},

    // back buttons
    {"backDisplay", "home"},
    {"backPlan", "home"},
    {"backAdmin", "home"},
    {"backLogin", "home"},
    {"backVirtualTrip", "plan"},

    // special condition
    // allows us to pass an argument into the function, which dictates which page it wants to go to.
    {"loginSuccess", "adminpanel"},
};

// default changePage function
void MainWindow::changePage()
{
    // get the button object and name that got pressed
    QObject *senderObject = sender();
    QString senderName = senderObject->objectName();

    // check if button is in the map
    if (buttonMap.find(senderName) != buttonMap.end()) {
        // if found, get desired widget and change to it
        QString desiredName = buttonMap.at(senderName);
        QWidget *desiredWidget = this->findChild<QWidget *>(desiredName);

        // "main" is the stacked widget
        this->ui->main->setCurrentWidget(desiredWidget);
    } else {
        // if not found, print
        qDebug() << "Button was not found in the map.";
    }
}

// changePage function that allows an argument to be passed
void MainWindow::changePage(QString specific)
{
    // check if button is in the map
    if (buttonMap.find(specific) != buttonMap.end()) {
        // if found, get desired widget and change to it
        QString desiredName = buttonMap.at(specific);
        QWidget *desiredWidget = this->findChild<QWidget *>(desiredName);

        this->ui->main->setCurrentWidget(desiredWidget);
    } else {
        // if not found, print
        qDebug() << "Button was not found in the map.";
    }
}

/*
 *  ADMIN LOGIN FUNCTION
 *
 *  This function is connected to the "login" button in the admin login page.
*/

// admin password
const QString adminPassword = "admin";

void MainWindow::adminLogin()
{
    QString password = ui->inputPassword->text();
    if (password == adminPassword) {
        QMessageBox::information(this, "Admin Login", "Login success!");

        // change page using existing changePage function
        changePage("loginSuccess");
    } else {
        QMessageBox::warning(this, "Admin Login", "Password is incorrect.");
    }
    ui->inputPassword->clear();
    ui->inputName->setText("admin");
}

/*
 *  INITIALIZERS
 *
 *  These functions initialize the UI.
*/

void initializeTable(QTableWidget *tableUI, vector<std::string> stadiums, QMap<QString, QMap<QString, QString>> infoDf)
{
    // get headers so that we can insert items in order
    QMap<QString, int> headers;
    for(int i = 0; i < tableUI->model()->columnCount(); i++)
    {
        QString header = tableUI->model()->headerData(i, Qt::Horizontal).toString().toLower();
        headers[header] = i;

        //qDebug() << header << ": " << headers[header];
    }

    tableUI->setRowCount(stadiums.size());

    // iterate thru teams
    int index = 0;

    // iterate through infoDF map
    for (auto it = infoDf.cbegin(); it != infoDf.cend(); ++it  )
    {
        QString team = it.key();
        QMap<QString, QString> data = it.value();

        for (auto it = data.cbegin(); it != data.cend(); ++it  )
        {
            QString key = it.key().toLower();
            QString value = it.value();
            //qDebug() << key;

            // if key is empty then ignore
            //if (key == "") { continue; }


            QTableWidgetItem *data = new QTableWidgetItem(value);
            tableUI->setItem(index, headers[key], data);
            //count++;
        }
        index++;
    }
}

void MainWindow::initializeTables() {
    initializeTable(this->ui->tableDisplay, stadiums, infoDf);
}

/*
 *  SORT TEAMS FUNCTION
 *
 *  This function is connected to the columns of the team table widgets.
*/

void MainWindow::sortTeams()
{

}
