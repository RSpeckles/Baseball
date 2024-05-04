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
#include <QListWidget>
#include <QMessageBox>
#include <QDebug>
#include <QFormLayout>
#include <QDialogButtonBox>

#include <iostream>
#include <string>

using namespace std;

// constructor/destructor (code inside will probably be moved once rowan is done)
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /* REMEMBER TO CHANGE "projectFileName" in mainwindow.h to your project file name.
     * We have different working project file names for whatever reason,
     * and this should be the easiest way to change the destination.
    */

    QDir distPath;

    cout <<distPath.path().toStdString();
    distPath.cdUp();

    string path = distPath.path().toStdString() + "/" + projectFileName + "/Distance between stadiums.csv";
    cout << path << endl;
    csv_to_table(path, distTable);

    QDir souvenirPath;
    souvenirPath.cdUp();

    path = souvenirPath.path().toStdString() + "/" + projectFileName + "/Baseball Souvenirs.csv";
    cout << path << endl;
    csv_to_table(path, souvenirTable);

    QDir infoPath;
    infoPath.cdUp();

    path = distPath.path().toStdString() + "/" + projectFileName + "/MLB Information.csv";
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

// initialize table
void initializeTable(QTableWidget *tableUI, QLabel *totalUI, QVector<QString> teams, QMap<QString, QMap<QString, QString>> infoDf)
{
    // set sorting to false so doesnt break
    tableUI->setSortingEnabled(false);

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
        qDebug() << name;

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

    // enable sorting again
    tableUI->setSortingEnabled(true);

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

// initalize lists
void initializeList(QListWidget *listUI, QMap<QString, QMap<QString, QString>> infoDf)
{
    listUI->clear();
    // iterate through teams
    for (auto it = infoDf.cbegin(); it != infoDf.cend(); ++it  )
    {
        QString team = it.key();
        listUI->addItem(it.value()["Team name"]);
    }
}

// define admin stuff
void initializeAttributes(QComboBox *comboUI, QVector<QString> teams, QMap<QString, QMap<QString, QString>> infoDf);

// init func
void MainWindow::initialize() {
    initializeTable(this->ui->tableDisplay, this->ui->totalDisplay, stadiums, infoDf);
    initializeList(this->ui->adminList, infoDf);
    initializeAttributes(this->ui->attributeCombo, stadiums, infoDf);
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

/*
 *  VIEW SOUVENIRS FUNCTION
 *
 *  Handles the "view souvenirs" button in the display page
*/

void MainWindow::viewSouvenirs() {
    int row = this->ui->tableDisplay->currentRow();

    // check if any row is selected
    if (row < 0) {
        QMessageBox::warning(this, "Display Teams", "Please select a team first!");
        return;
    }

    QString team = this->ui->tableDisplay->item(row, 0)->text();

    // check if souvenirs exist for team
    if (souvenirTable.find(team) == souvenirTable.end()) {
        QMessageBox::warning(this, "Display Teams", "No souvenirs found for " + team);
        return;
    }
    QMap<QString, double> souvenirs = souvenirTable[team];

    // popup
    QDialog *endPopup = new QDialog();
    QFormLayout *formLayout = new QFormLayout();
    endPopup->setStyleSheet("font: 500 13pt \"Bahnschrift\"");
    endPopup->resize(512, 512);

    // connect ok and cancel boxes
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
    QObject::connect(buttonBox, SIGNAL(accepted()), endPopup, SLOT(accept()));

    // labels
    QLabel *labelSouvenirs = new QLabel();
    labelSouvenirs->setText(team + "\nSouvenirs:");

    // table
    QTableWidget *table = new QTableWidget();
    table->setFrameShape(QAbstractScrollArea::Shape::Box);

    table->setColumnCount(2);
    table->setHorizontalHeaderLabels({"Souvenir", "Cost"});
    table->setRowCount(souvenirs.size());

    int index = 0;
    for (auto it = souvenirs.cbegin(); it != souvenirs.cend(); ++it  )
    {
        QString name = it.key();
        double cost = it.value();

        QTableWidgetItem *itemUI = new QTableWidgetItem(name);
        QTableWidgetItem *costUI = new QTableWidgetItem("$" + QString::number(cost, 'f', 2));

        table->setItem(index, 0, itemUI);
        table->setItem(index, 1, costUI);

        index++;
    }

    // final
    formLayout->addWidget(labelSouvenirs);
    formLayout->addWidget(table);
    formLayout->addWidget(buttonBox);

    endPopup->setLayout(formLayout);

    // run
    table->setColumnWidth(0, 332);
    table->setColumnWidth(1, 128);
    endPopup->exec();
}

/*
 *  SOUVENIR DISPLAY FUNCTION
*/

map<QString, QString> souvenirMap = {
    // buttons
    {"adminList",   "adminSouvenirList"},
    };

void MainWindow::updateSouvenirs(QString senderName) {
    if (souvenirMap.find(senderName) != souvenirMap.end()) {
        // get item selected
        QString team = this->findChild<QListWidget*>(senderName)->currentItem()->text();

        // find corresponding souvenir list object
        QListWidget* souvenirList = this->findChild<QListWidget*>(souvenirMap.at(senderName));
        souvenirList->clear();

        QMap<QString, double> souvenirs = souvenirTable[team];

        // add souvenirs
        for (auto it = souvenirs.cbegin(); it != souvenirs.cend(); ++it  )
        {
            QString name = it.key();
            double cost = it.value();

            QString finalString = name + "\n$" + QString::number(cost, 'f', 2);

            souvenirList->addItem(finalString);
        }
    } else {
        // if not found, print
        qDebug() << "Button was not found in the map.";
    }
}

/*
 *  ADMIN STUFF
 *
 *  Admin functions
*/

void initializeAttributes(QComboBox *comboUI, QVector<QString> teams, QMap<QString, QMap<QString, QString>> infoDf) {
    // disable so that "textChanged" connection doesnt loop forever
    comboUI->setEnabled(false);

    // iterate through teams
    //qDebug() << teams[1];
    for (auto it = infoDf[teams[1]].cbegin(); it != infoDf[teams[1]].cend(); ++it  )
    {
        QString attribute = it.key();
        if (attribute == "Team name") { continue; } // disables editing team names

        comboUI->addItem(attribute);
    }

    // reenable
    comboUI->setEnabled(true);
}

QString updateValue(QString attribute, QString team, QMap<QString, QMap<QString, QString>> infoDf) {
    // make sure team and attribute exist when updating value
    if (infoDf.find(team) != infoDf.end()) {
        if (infoDf[team].find(attribute) != infoDf[team].end()) {
            return infoDf[team][attribute];
        }
    }
    return "null";
}

QString currentTeam = "";
void MainWindow::adminTeamSelected()
{
    // get the button object and name that got pressed
    QObject* senderObject = sender();
    QString senderName = senderObject->objectName();

    // get current selected college
    this->updateSouvenirs(senderName);

    // checks
    if (!this->ui->attributeCombo->isEnabled()) { return; }
    if (this->ui->adminList->currentRow() == -1) { return; }

    // get name of team selected
    currentTeam = this->ui->adminList->currentItem()->text();

    // check if string is empty
    if (currentTeam.size() <= 0) { return; }

    // if currentTeam doesnt exist in infoDf, its possible that the name was changed in the UI.
    // to fix this, we need to iterate through all of infoDf to check the "Team name" value.
    // if theres a match, then we take the string of the key and set that to currentTeam
    if (infoDf.find(currentTeam) == infoDf.end()) {
        for (auto it = infoDf.cbegin(); it != infoDf.cend(); ++it  )
        {
            if (it.value()["Team name"] == currentTeam) {
                currentTeam = it.key();
            }
        }
    }

    // update value
    QString currentAttribute = this->ui->attributeCombo->currentText();
    this->ui->attributeEdit->setText(updateValue(currentAttribute, currentTeam, infoDf));

    // update souvenir ui
    this->ui->buttonSouvenirAdd->setEnabled(true);
    this->updateSouvenirs(senderName);
    this->adminSouvenirSelected();
}

// map that uses attributes as the key to check for errors
QMap<QString, function<bool(QString&, MainWindow*, QMap<QString, QMap<QString, QString>>)>> editChecks = {
    {"Seating capacity", [](QString& attribute, MainWindow* mainWindow, QMap<QString, QMap<QString, QString>> infoDf)
        {
            bool conversionOK;
            int value = attribute.toInt(&conversionOK);
            if (!conversionOK) {
                QMessageBox::warning(mainWindow, "Admin Edit", "Please enter an int!");
                return false;
            }
            if (value <= 0) {
                QMessageBox::warning(mainWindow, "Admin Edit", "Please enter a value greater than 0!");
                return false;
            }
            return true;
        }
    },
    {"League", [](QString& attribute, MainWindow* mainWindow, QMap<QString, QMap<QString, QString>> infoDf)
        {
        if (attribute != "National" && attribute != "American") {
            QMessageBox::warning(mainWindow, "Admin Edit", "Please enter National or American!");
            return false;
        }
        return true;
        }
    },
    {"Date opened", [](QString& attribute, MainWindow* mainWindow, QMap<QString, QMap<QString, QString>> infoDf)
        {
        bool conversionOK;
        int value = attribute.toInt(&conversionOK);
        if (!conversionOK) {
             QMessageBox::warning(mainWindow, "Admin Edit", "Please enter an int!");
             return false;
        }
        if (value <= 0) {
             QMessageBox::warning(mainWindow, "Admin Edit", "Please enter a value greater than 0!");
             return false;
        }
        return true;
        }
    },
    {"Distance to center field", [](QString& attribute, MainWindow* mainWindow, QMap<QString, QMap<QString, QString>> infoDf)
        {
         bool conversionOK;
         int value = attribute.toInt(&conversionOK);
         if (!conversionOK) {
             QMessageBox::warning(mainWindow, "Admin Edit", "Please enter an int!");
             return false;
         }
         if (value <= 0) {
             QMessageBox::warning(mainWindow, "Admin Edit", "Please enter a value greater than 0!");
             return false;
         }
         int meters = value / 3.281; // google told me this
         attribute = attribute + " feet (" + QString::number(meters) + " m)";

         return true;
     }
    },
    {"Team name", [](QString& attribute, MainWindow* mainWindow, QMap<QString, QMap<QString, QString>> infoDf)
        {
        for (auto it = infoDf.cbegin(); it != infoDf.cend(); ++it  )
        {
            if (it.value()["Team name"] == attribute) {
                QMessageBox::warning(mainWindow, "Admin Edit", "Team name already exists!");
                return false;
            }
        }

            return true;
        }
    },
};

void MainWindow::adminConfirmEdit(){
    QString newValue = this->ui->attributeEdit->text();
    QString currentAttribute = this->ui->attributeCombo->currentText();

    //qDebug() << newValue;

    // if there is an attribute checker
    if (editChecks.find(currentAttribute) != editChecks.end()) {
        bool isValid = editChecks[currentAttribute](newValue, this, infoDf);

        // if input is invalid, reset value and return
        if (isValid == false) {
            this->ui->attributeEdit->setText(updateValue(currentAttribute, currentTeam, infoDf));
            return;
        }
    }

    // finally, check if team and attribute exists in infoDf and make changes
    if (infoDf.find(currentTeam) == infoDf.end()) { return; }
    if (infoDf[currentTeam].find(currentAttribute) == infoDf[currentTeam].end()) { return; }

    infoDf[currentTeam][currentAttribute] = newValue;
    this->ui->attributeEdit->setText(updateValue(currentAttribute, currentTeam, infoDf));
    QMessageBox::information(this, "Admin Edit", "Edit success! \n" + currentAttribute + " set to " + newValue);

    // update everything lol
    this->initialize();
}

// souvenir stuff for admin
void MainWindow::adminSouvenirSelected() {
    if (this->ui->adminSouvenirList->currentItem()) {
        this->ui->buttonSouvenirEdit->setEnabled(true);
        this->ui->buttonSouvenirRemove->setEnabled(true);
    }
    else {
        this->ui->buttonSouvenirEdit->setEnabled(false);
        this->ui->buttonSouvenirRemove->setEnabled(false);
    }
}

bool stringToDouble(string inputString, double &result) {
    char* end;
    result = strtod(inputString.c_str(), &end);
    if (end == inputString.c_str() || *end != '\0') return false;
    return true;
}


void MainWindow::adminEditSouvenir() {
    // get the button object and name that got pressed
    QObject* senderObject = sender();
    QString senderName = senderObject->objectName();

    // get name of college selected
    QString team = this->ui->adminList->currentItem()->text();

    // check if souvenirs exist for team
    if (souvenirTable.find(team) == souvenirTable.end()) {
        QMessageBox::warning(this, "Display Teams", "No souvenirs found for " + team);
        return;
    }
    // ok get souvenirs now LOL!
    QMap<QString, double> &souvenirs = souvenirTable[team];

    if (senderName == "buttonSouvenirAdd") {
        QString name = "New Souvenir";
        double cost = 12.34;

        int count = 1;
        while (souvenirs.find(name + " " + QString::number(count)) != souvenirs.end()) {
            count += 1;
        };
        name += " " + QString::number(count);

        souvenirs[name] = cost;
    }
    else if (senderName == "buttonSouvenirEdit") {
        // get name of current souvenir selected
        QString currentSouvenir = this->ui->adminSouvenirList->currentItem()->text();
        long indexToCutOff = currentSouvenir.indexOf("\n");
        QString name = (currentSouvenir.left(indexToCutOff));
        double cost = souvenirs[name];

        // EDIT POPUP
        QDialog *editPopup = new QDialog();
        QFormLayout *formLayout = new QFormLayout();

        QLabel *labelName = new QLabel();
        QLabel *labelCost = new QLabel();
        labelName->setText("Name");
        labelCost->setText("Cost $");

        QLineEdit *editName = new QLineEdit();
        QLineEdit *editCost = new QLineEdit();

        editName->setText(name);
        editCost->setText(QString::number(cost));

        // connect ok and cancel boxes
        QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        QObject::connect(buttonBox, SIGNAL(accepted()), editPopup, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), editPopup, SLOT(reject()));

        formLayout->addRow(labelName, editName);
        formLayout->addRow(labelCost, editCost);
        formLayout->addWidget(buttonBox);

        editPopup->setLayout(formLayout);

        // get result after any button is clicked
        int result = editPopup->exec();
        if(result == QDialog::Accepted)
        {
            // apply changes to souvenir
            QString newName = editName->text();
            QString newCost = editCost->text();
            double cost;

            // checks
            if (!stringToDouble(newCost.toStdString(), cost)) {
                QMessageBox::warning(this, "Souvenir Edit", "Invalid cost input!");
                return;
            }

            if (souvenirs.find(newName) != souvenirs.end() && newName != name) {
                QMessageBox::warning(this, "Souvenir Edit", "Souvenir already exists! Choose a different name.");
                return;
            }

            souvenirs.remove(name);
            souvenirs[newName] = cost;
        }
    }
    else if (senderName == "buttonSouvenirRemove") {
        // get name of current souvenir selected
        QString currentSouvenir = this->ui->adminSouvenirList->currentItem()->text();
        long indexToCutOff = currentSouvenir.indexOf("\n");
        QString name = (currentSouvenir.left(indexToCutOff));

        souvenirs.remove(name);
    }

    // update souvenirs on screen
    this->updateSouvenirs("adminList");
    this->adminSouvenirSelected();
}

// save data to csv
void MainWindow::saveToCSV() {
    QDir directory;
    directory.cdUp();

    // infoDF weird so dont save that yet
    string path = directory.path().toStdString() + "/" + projectFileName + "/MLB Information.csv";
    QString columns = "Team name,Stadium name,Seating capacity,Location,Playing surface,League,Date opened,Distance to center field,Ballpark typology,Roof Type";

    df_to_csv(path, infoDf, columns);

    // save souvenirs
    path = directory.path().toStdString() + "/" + projectFileName + "/Baseball Souvenirs.csv";
    columns = "Team name,Souvenir,Cost";
    df_to_csv(path, souvenirTable, columns);
}
