// files
#include "mainwindow.h"
#include "parser.h"
#include "sorter.h"
#include "pathing.h"
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

    string path = distPath.path().toStdString() + "/BaseballProject/Distance between stadiums.csv";
    cout << path << endl;
    csv_to_table(path, distTable);

    QDir souvenirPath;
    souvenirPath.cdUp();

    path = souvenirPath.path().toStdString() + "/BaseballProject/Baseball Souvenirs.csv";
    cout << path << endl;
    csv_to_table(path, souvenirTable);

    QDir infoPath;
    infoPath.cdUp();

    path = distPath.path().toStdString() + "/BaseballProject/MLB Information.csv";
    cout << path << endl;
    csv_to_df(path, infoDf);

    QVector<QPair<QString, double>> shortestPath = dijkstra(distTable, "Dodger Stadium", "Fenway Park");

    printPath(shortestPath);
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

void initializeTable(QTableWidget *tableUI, QLabel *totalUI, QVector<QString> teams, QMap<QString, QMap<QString, QString>> infoDf)
{
    // get headers so that we can insert items in order
    QMap<QString, int> headers;
    for(int i = 0; i < tableUI->model()->columnCount(); i++)
    {
        QString header = tableUI->model()->headerData(i, Qt::Horizontal).toString().toLower();
        headers[header] = i;

        //qDebug() << header << ": " << headers[header];
    }

    // reset
    tableUI->clearContents();
    tableUI->setRowCount(teams.size());
    tableUI->setSortingEnabled(true);

    // iterate thru teams
    int index = 0;
    int totalCapacity = 0;
    // iterate through infoDF map
    for (QString name : teams)
    {
        // if team doesnt exist in data, continue
        if (infoDf.find(name) == infoDf.end()) {
            qDebug() << "Missing in info dataframe: " << name;
            continue;
        }
        QMap<QString, QString> data = infoDf.value(name);
        //qDebug() << data;

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

            // get total capacity
            if (key == "seating capacity") {
                totalCapacity += value.toInt();
            }
        }
        index++;
    }

    totalUI->setText(
        "Total Teams: " + QString::number(teams.size())
        + "\nTotal Capacity: " + QString::number(totalCapacity)
        );

    /* OLD
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
    */
}

void MainWindow::initializeTables() {
    initializeTable(this->ui->tableDisplay, this->ui->totalDisplay, stadiums, infoDf);
}

/*
 *  SORT TEAMS FUNCTION
 *
 *  This function is connected to the columns of the team table widgets.
*/

// takes the text from the combo box and converts it to data for the function to read
map<QString, QString> filterMap = {
    {"Major League", "Major"},
    {"National League", "National"},
    {"American League", "American"},
    {"Open Roof", "Open"},
    {"Greatest Distance to Center Field", "GreatestCF"},
    {"Shortest Distance to Center Field", "ShortestCF"},
    };

void MainWindow::filterTeams()
{
    QString filter = ui->filterDisplay->currentText();
    //qDebug() << filter;

    // checks if filter exists, else return
    if (filterMap.find(filter) == filterMap.end()) { return; }

    // get filtered teams
    QVector<QString> filteredTeams = sort_by_teams(infoDf, "Team name", filterMap.at(filter).toStdString());

    // modify ui
    initializeTable(this->ui->tableDisplay, this->ui->totalDisplay, filteredTeams, infoDf);
}
