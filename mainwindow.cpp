
#include "mainwindow.h"
#include "parser.h"
#include "sorter.h"
#include "pathing.h"
#include "ui_mainwindow.h"
#include "algorithms.h"

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
#include <QThread>
#include <QSpinBox>

#include <iostream>
#include <string>

using namespace std;

/**
 * @brief MainWindow constructor.
 * @param parent The parent widget.
 *
 * The MainWindow constructor initializes the main window of the application. It sets up the user
 * interface, loads data from CSV files, and initializes various data structures used throughout
 * the application. The constructor also sets up connections for various signals and slots.
 */
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
    //cout << path << endl;
    csv_to_table(path, distTable);

    QDir souvenirPath;
    souvenirPath.cdUp();

    path = souvenirPath.path().toStdString() + "/" + projectFileName + "/Baseball Souvenirs.csv";
    //cout << path << endl;
    csv_to_table(path, souvenirTable);

    QDir infoPath;
    infoPath.cdUp();

    path = distPath.path().toStdString() + "/" + projectFileName + "/MLB Information.csv";
    //cout << path << endl;
    csv_to_df(path, infoDf);

    //QVector<QPair<QString, double>> shortestPath = dijkstra(distTable, "Dodger Stadium", "Fenway Park");

    //printPath(shortestPath);


}
/**
 * @brief MainWindow destructor.
 *
 * The MainWindow destructor is responsible for cleaning up and deallocating resources used by
 * the main window.
 */
MainWindow::~MainWindow()
{
    delete ui;
}
/**
 * @brief Function to change the page.
 *
 * This function is responsible for changing the currently displayed page or widget in the main
 * window. It uses a map to associate button names with the desired page names. When a button
 * designed to change the page is pressed, the function takes the button name and uses the map
 * to find the desired page name, and then switches to that page.
 */
map<QString, QString> buttonMap = {
        // buttons
        {"buttonDisplay", "display"},
        {"buttonPlan", "plan"},
        {"buttonAdmin", "adminlogin"},
        {"buttonStartTrip", "virtualtrip"},
        {"buttonAlgorithm", "algorithm"},

        // back buttons
        {"backDisplay", "home"},
        {"backPlan", "home"},
        {"backAdmin", "home"},
        {"backLogin", "home"},
        {"backVirtualTrip", "plan"},
        {"backAlgorithm", "home"},

        // special condition
        // allows us to pass an argument into the function, which dictates which page it wants to go to.
        {"loginSuccess", "adminpanel"},
        {"endTrip",     "plan"},
};

/**
 * @brief Function to change the page to a specific page.
 * @param specific The name of the specific page.
 *
 * This function changes the currently displayed page or widget in the main window to a specific
 * page. It takes a QString parameter `specific` that represents the name of the desired page.
 * The function uses a map to find the corresponding page name and then switches to that page.
 */
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

/**
 * @brief Function to change the page to a specific page.
 * @param specific The name of the specific page.
 */
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



const QString adminPassword = "admin";

/**
 * @brief Function to handle admin login.
 *
 * This function is responsible for handling the admin login functionality. It retrieves the
 * entered password from the input field, compares it with the stored admin password, and
 * either displays a success message and changes to the admin panel page, or displays a warning
 * message if the password is incorrect.
 */
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

/**
 * @brief Function to initialize the table.
 * @param tableUI The table widget to initialize.
 * @param totalUI The total UI label to initialize.
 * @param teams The teams to display in the table.
 * @param infoDf The information dataframe.
 */
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

/**
 * @brief Function to initialize the list.
 * @param listUI The list widget to initialize.
 * @param infoDf The information dataframe.
 */
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

/**
 * @brief Function to initialize the attributes.
 * @param comboUI The combo box UI to initialize.
 * @param teams The teams to display in the combo box.
 * @param infoDf The information dataframe.
 */
void initializeAttributes(QComboBox *comboUI, QVector<QString> teams, QMap<QString, QMap<QString, QString>> infoDf);

/**
 * @brief Function to initialize the purchase table.
 * @param tableUI The table widget to initialize.
 */
void initializePurchase(QTableWidget *tableUI) {
    tableUI->setColumnCount(3);
    tableUI->setColumnWidth(0, 265);
    tableUI->setColumnWidth(1, 225);
    tableUI->setColumnWidth(2, 250);
    tableUI->setHorizontalHeaderLabels({"Souvenir", "Cost", "Quantity"});
}

/**
 * @brief Function to initialize the main window.
 *
 * This function initializes various components of the main window, including the team display
 * table, admin list, attribute combo box, and souvenir purchase table. It also initializes the
 * stadium plan and algorithm components.
 */
void MainWindow::initialize() {
    // get list of current stadiums
    QVector<QString> teams;

    for (auto it = infoDf.cbegin(); it != infoDf.cend(); ++it  )
    {
        QString team = it.key();
        teams.push_back(team);
    }

    // init
    initializeTable(this->ui->tableDisplay, this->ui->totalDisplay, teams, infoDf);
    initializeList(this->ui->adminList, infoDf);
    initializeAttributes(this->ui->attributeCombo, stadiums, infoDf);
    initializePurchase(this->ui->tableSouvenir);
    this->initializeStadiumPlan();
    this->intializeAlgorithm();
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
/**
 * @brief Function to filter teams.
 *
 * This function is responsible for filtering the teams displayed in the table based on a
 * selected filter. It retrieves the filter from the combo box, maps it to a corresponding
 * filter string, sorts the teams based on the filter, and updates the table display with the
 * filtered teams.
 */
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
/**
 * @brief Function to view souvenirs.
 *
 * This function handles the "view souvenirs" button in the display page. It retrieves the
 * currently selected team, checks if souvenirs exist for that team, and displays a popup
 * window with the list of souvenirs and their corresponding costs.
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

void MainWindow::viewInfo() {
    int row = this->ui->tableDisplay->currentRow();

    // check if any row is selected
    if (row < 0) {
        QMessageBox::warning(this, "Display Teams", "Please select a team first!");
        return;
    }

    QString team = this->ui->tableDisplay->item(row, 0)->text();

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
    labelSouvenirs->setText(team + "\nInfo:");

    // table
    QTableWidget *table = new QTableWidget();
    table->setFrameShape(QAbstractScrollArea::Shape::Box);

    table->setColumnCount(2);
    table->setHorizontalHeaderLabels({"Attribute", "Value"});
    table->setRowCount(infoDf[team].size());

    int index = 0;
    for (auto it = infoDf[team].cbegin(); it != infoDf[team].cend(); ++it  )
    {
        QString attribute = it.key();
        QString value = it.value();

        QTableWidgetItem *attributeUI = new QTableWidgetItem(attribute);
        QTableWidgetItem *valueUI = new QTableWidgetItem(value);

        table->setItem(index, 0, attributeUI);
        table->setItem(index, 1, valueUI);

        index++;
    }

    // final
    formLayout->addWidget(labelSouvenirs);
    formLayout->addWidget(table);
    formLayout->addWidget(buttonBox);

    endPopup->setLayout(formLayout);

    // run
    table->setColumnWidth(0, 230);
    table->setColumnWidth(1, 230);
    endPopup->exec();
}

/*
 *  SOUVENIR DISPLAY FUNCTION
*/


map<QString, QString> souvenirMap = {
        // buttons
        {"adminList",   "adminSouvenirList"},
};

/**
 * @brief Function to update souvenirs.
 * @param senderName The name of the sender.
 *
 * This function updates the list of souvenirs displayed for a selected team. It takes a
 * `senderName` parameter that represents the name of the sender object (e.g., a list widget
 * or button). The function finds the corresponding souvenir list object, clears it, and
 * populates it with the souvenirs for the selected team.
 */
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


/**
 * @brief Function to initialize the attributes.
 * @param comboUI The combo box UI to initialize.
 * @param teams The teams to display in the combo box.
 * @param infoDf The information dataframe.
 *
 * This function initializes a combo box with team attributes from the provided information
 * dataframe. It populates the combo box with the attributes, excluding the team name and
 * stadium name.
 */
void initializeAttributes(QComboBox *comboUI, QVector<QString> teams, QMap<QString, QMap<QString, QString>> infoDf) {
    // disable so that "textChanged" connection doesnt loop forever
    comboUI->setEnabled(false);

    // iterate through teams
    //qDebug() << teams[1];
    for (auto it = infoDf[teams[1]].cbegin(); it != infoDf[teams[1]].cend(); ++it  )
    {
        QString attribute = it.key();
        if (attribute == "Team name" || attribute == "Stadium name") { continue; } // disables editing team names

        comboUI->addItem(attribute);
    }

    // reenable
    comboUI->setEnabled(true);
}

/**
 * @brief Function to confirm an admin edit.
 */
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
/**
 * @brief Function to handle when an admin team is selected.
 */
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
/**
 * @brief Function to handle an admin edit souvenir.
 */
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

/**
 * @brief Function to confirm an admin edit.
 */
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

/**
 * @brief Function to handle when an admin souvenir is selected.
 */
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

/**
 * @brief Converts a string to a double.
 *
 * This function attempts to convert a string to a double. If the conversion is successful,
 * the result is stored in the reference parameter and the function returns true. If the
 * conversion fails, the function returns false.
 *
 * @param inputString The string to convert.
 * @param result Reference to a double where the result will be stored if the conversion is successful.
 * @return true if the conversion is successful, false otherwise.
 */
bool stringToDouble(string inputString, double &result) {
    char* end;
    result = strtod(inputString.c_str(), &end);
    if (end == inputString.c_str() || *end != '\0') return false;
    return true;
}



/**
 * @brief Handles the editing of souvenirs in the admin panel.
 *
 * This function is called when the admin clicks on the "Edit Souvenir" button. It opens a dialog
 * where the admin can edit the name and cost of the souvenir. The changes are then applied to the
 * souvenir table. If the admin clicks on the "Add Souvenir" button, a new souvenir with a default
 * name and cost is added. If the admin clicks on the "Remove Souvenir" button, the selected souvenir
 * is removed from the table.
 */
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

/**
 * @brief Function to save data to CSV.
 *
 * This function saves the data from the application to CSV files. It saves the information
 * dataframe, souvenir table, and distance table to their respective CSV files.
 */
void MainWindow::saveToCSV() {
    QDir directory;
    directory.cdUp();

    // infoDF
    string path = directory.path().toStdString() + "/" + projectFileName + "/MLB Information.csv";
    QString columns = "Team name,Stadium name,Seating capacity,Location,Playing surface,League,Date opened,Distance to center field,Ballpark typology,Roof Type";

    df_to_csv(path, infoDf, columns);

    // save souvenirs
    path = directory.path().toStdString() + "/" + projectFileName + "/Baseball Souvenirs.csv";
    columns = "Team name,Souvenir,Cost";
    df_to_csv(path, souvenirTable, columns);

    // save distances
    path = directory.path().toStdString() + "/" + projectFileName + "/Distance between stadiums.csv";
    columns = "Originated Stadium,Destination Stadium,Distance";
    df_to_csv(path, distTable, columns);
}

// load new files from csv
bool alreadyRead = false;
/**
* @brief Function to load data from CSV.
*
* This function loads data from CSV files into the application. It reads the distance table,
* information dataframe, and souvenir table from their respective CSV files. It also updates
* a progress bar to indicate the loading progress and enables/disables certain UI elements
* based on the loaded data.
*/
void MainWindow::loadFromCSV() {
    if (alreadyRead) { return; }
    alreadyRead = true;

    // read in
    QDir directory;
    directory.cdUp();

    // dist
    string distPath = directory.path().toStdString() + "/" + projectFileName + "/Distance between expansion stadium.csv";
    csv_to_table(distPath, distTable);

    // infoDF
    string infoPath = directory.path().toStdString() + "/" + projectFileName + "/MLB Information Expansion.csv";
    csv_to_df(infoPath, infoDf);

    // progress bar
    for(int i = 0; i <101; i++)
    {
        QThread::msleep(3);
        ui->progressBar->setValue(i);
        qApp->processEvents(QEventLoop::AllEvents);
    }
    ui->progressBar->setValue(0);

    this->ui->buttonSouvenirAdd->setEnabled(false);
    this->ui->buttonSouvenirEdit->setEnabled(false);
    this->ui->buttonSouvenirRemove->setEnabled(false);

    // update everything lol
    this->initialize();
}

/*
 * PLAN TRIP STUFF
 */


/**
* @brief Function to initialize the stadium plan.
*
* This function initializes the components related to the stadium plan feature. It clears the
* stadium combo box and the path result list, disables the "Start Trip" button, and populates
* the stadium combo box with the list of stadiums and their corresponding teams.
*/
void MainWindow::initializeStadiumPlan() {
    this->ui->comboPlanStadiums->clear();
    this->ui->listPathResult->clear();
    this->ui->buttonStartTrip->setEnabled(false);

    // add stadiums to dropdown
    for (auto it = infoDf.cbegin(); it != infoDf.cend(); ++it  )
    {
        QString team = it.key();
        QString stadium = it.value()["Stadium name"];

        this->ui->comboPlanStadiums->addItem(stadium + " (" + team + ")");
    }
}

/**
* @brief Function to get destinations.
*
* This function retrieves the list of potential destinations based on the currently selected
* stadium. It clears the list of destinations, finds the team associated with the selected
* stadium, and populates the destination list with all other stadiums, excluding the selected
* one. Each destination is displayed as a checkable item in the list.
*/

QVector<QString> orderSpecified;

void MainWindow::addToOrder() {
    for(int i = 0; i < this->ui->listPlanStadiums->count(); i++)
    {
        // get name
        QListWidgetItem *item = this->ui->listPlanStadiums->item(i);

        // check if checked
        if (item->checkState() == Qt::Checked) {
            string currentText = item->text().toStdString();
            int endPos = currentText.find('(');
            string stadiumName = currentText.substr(0, endPos - 1);

            // add to list
            orderSpecified.push_back(QString::fromStdString(stadiumName));
            qDebug() << orderSpecified;
            return;
        };
    }
}

void MainWindow::getDestinations() {
    // get name of college selected
    string currentText = this->ui->comboPlanStadiums->currentText().toStdString();

    size_t startPos = currentText.find('(');
    size_t endPos = currentText.find(')', startPos);
    string currentTeam = currentText.substr(startPos + 1, endPos - startPos - 1);

    // clear list of destinations
    this->ui->listPlanStadiums->clear();

    // get college and list destinations
    for (auto it = infoDf.cbegin(); it != infoDf.cend(); ++it  )
    {
        QString team = it.key();
        if (currentTeam == team.toStdString()) {continue;}

        QString stadium = it.value()["Stadium name"];

        QListWidgetItem* item = new QListWidgetItem("item", this->ui->listPlanStadiums);
        item->setText(stadium + " (" + team + ")");
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable); // set checkable flag
        item->setCheckState(Qt::Unchecked); // AND initialize check state
        item->setFlags(item->flags() & ~Qt::ItemIsSelectable);

    }

    orderSpecified.clear();
}

// current path
QVector<QPair<QString, double>> currentPath;
int currentIndex = 0;


/**
* @brief Function to plan a trip.
*
* This function plans a trip based on the selected starting stadium and the chosen destinations.
* It retrieves the selected starting stadium, gets the list of destinations (either by selecting
* individual destinations or using the "closest" option), calculates the path using a graph
* algorithm, and displays the path with the total distance in the user interface.
*/

void MainWindow::planTrip() {
    // get name of stadium selected
    string currentText = this->ui->comboPlanStadiums->currentText().toStdString();
    size_t endPos = currentText.find('(');
    QString currentStadium = QString::fromStdString(currentText.substr(0, endPos - 1));

    // get destinations
    vector<string> destinations;
    //int closestNum = this->ui->spinClosest->value();
    bool isOrderSpecified = this->ui->orderSpecified->isChecked();

    // get path
    if (!isOrderSpecified) {
        // add other destinations
        for(int i = 0; i < this->ui->listPlanStadiums->count(); i++)
        {
            // get name
            QListWidgetItem *item = this->ui->listPlanStadiums->item(i);

            // check if checked
            if (item->checkState() == Qt::Unchecked) { continue; };
            currentText = item->text().toStdString();
            endPos = currentText.find('(');
            string stadiumName = currentText.substr(0, endPos - 1);

            // add to list
            destinations.push_back(stadiumName);
        }

        // ok finally its implemented
        QDir distPath;
        distPath.cdUp();
        string path = distPath.path().toStdString() + "/" + projectFileName + "/Distance between stadiums.csv";
        unordered_map<string, unordered_map<string, int>> graph = readStadiumDistances(path);

        string start_stadium = currentStadium.toStdString();
        int num_closest_stadiums = destinations.size();
        unordered_map<string, int> distances;
        string thisStadium = currentStadium.toStdString();
        QVector<string> visitedStadiums = {currentStadium.toStdString()};
        for (auto it = destinations.cbegin(); it != destinations.cend(); it++){
            distances = dijkstra(graph, thisStadium);
            double min = INT_MAX;
            string nearest = "";
            for (auto it = distances.cbegin(); it != distances.cend(); it++){
                if (it->second < min && visitedStadiums.contains(it->first) != true && destinations.contains(it->first)){
                    min = it->second;
                    nearest = it->first;
                }
            }
            thisStadium = nearest;
            visitedStadiums.push_back(nearest);
            currentPath.push_back({QString::fromStdString(nearest), min});
        }

        // currentPath = findClosestStadiums(distances, destinations, num_closest_stadiums);
        currentPath.push_front({currentStadium, 0});

    }
    else {
        // if closest lol
        //currentPath = visitNumClosest(distTable, currentStadium, closestNum);
    }

    // check
    if (currentPath.size() < 2) {
        QMessageBox::warning(this, "Plan Visit", "There must be at least one destination!");
        return;
    }

    // ui stuff
    this->ui->listPathResult->clear();
    this->ui->buttonStartTrip->setEnabled(true);

    double totalDist = 0;
    for (int i = 0; i < static_cast<int>(currentPath.size()); i++) {
        // get stadium
        QPair<QString, double> destination = currentPath[i];

        // get team
        bool foundTeam = false;
        QMap<QString, QString> team;

        for (auto it = infoDf.cbegin(); it != infoDf.cend(); ++it  )
        {
            if (it.value()["Stadium name"] == destination.first) {
                team = it.value();
                foundTeam = true;
                break;
            }
        }

        if (!foundTeam) { qDebug() << "Couldn't find team to set destination to!"; return; }

        QString pathString;
        pathString.append(destination.first + " (" + team["Team name"] + ")");
        totalDist += destination.second;

        if (i < static_cast<int>(currentPath.size()) - 1) { pathString.append("\n↓"); }

        QListWidgetItem* item = new QListWidgetItem("item", this->ui->listPathResult);
        item->setTextAlignment(Qt::AlignCenter);
        item->setText(pathString);
        item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
    }

    this->ui->labelTotalDistance->setText("Total Distance: " + QString::number(totalDist) + " miles");
}

/**
 * @brief Function to set the current destination.
 * @param index The index of the current destination.
 */
void MainWindow::setCurrentDestination(int index) {
    if (currentPath.size() < 2) { return; }

    // set some stuff
    QPair<QString, double> stadium = currentPath[index];
    currentIndex = index;                   // for replication purposes
    QString prefix = "";

    this->ui->buttonNextStop->setText("Next Stop");

    if (index == 0) {
        prefix = " (First Stop)";
    }
    else if (index == static_cast<int>(currentPath.size()) - 1) {
        prefix = " (Last Stop)";
        this->ui->buttonNextStop->setText("End Trip");
    }

    this->ui->groupDestination->setTitle("Destination " + QString::number(index) + prefix);

    // get team and souvenirs
    bool foundTeam = false;
    QMap<QString, QString> team;

    for (auto it = infoDf.cbegin(); it != infoDf.cend(); ++it  )
    {
        if (it.value()["Stadium name"] == stadium.first) {
            team = it.value();
            foundTeam = true;
            break;
        }
    }

    if (!foundTeam) { qDebug() << "Couldn't find team to set destination to!"; return; }

    this->ui->labelDestination->setText(stadium.first + " (" + team["Team name"] + ")");
    QMap<QString, double> souvenirs = souvenirTable[team["Team name"]];

    //clear
    this->ui->tableSouvenir->clearContents();
    this->ui->tableSouvenir->setRowCount(souvenirs.size());
    this->ui->labelPurchaseTotal->setText("Total Spent: $0.00");

    // create new item for each souvenir
    int souvenirIndex = 0;
    for (auto it = souvenirs.cbegin(); it != souvenirs.cend(); ++it  )
    {
        QString name = it.key();
        double cost = it.value();

        QTableWidgetItem *itemUI = new QTableWidgetItem(name);
        QTableWidgetItem *costUI = new QTableWidgetItem("$" + QString::number(cost, 'f', 2));

        // Assuming you have a QSpinBox for quantity in each row
        QSpinBox *quantitySpinBox = new QSpinBox;
        quantitySpinBox->setMinimum(0);
        quantitySpinBox->setMaximum(99); // Adjust max as needed
        quantitySpinBox->setValue(0); // Set initial quantity

        this->ui->tableSouvenir->setItem(souvenirIndex, 0, itemUI);
        this->ui->tableSouvenir->setItem(souvenirIndex, 1, costUI);
        this->ui->tableSouvenir->setCellWidget(souvenirIndex, 2, quantitySpinBox);

        souvenirIndex++;
    }
}

int currentDestination = 0;
/**
* @brief Function to start a trip.
*
* This function initiates the virtual trip based on the planned path. It sets the current
* destination to the first stop in the path, displays the corresponding information, and
* changes the page to the "Virtual Trip" view.
*/
void MainWindow::startTrip() {
    currentDestination = 0;
    this->setCurrentDestination(currentDestination);
    this->changePage("buttonStartTrip");
}

QVector<QPair<QString, double>> currentSouvenirs;
QVector<QPair<QString, double>> totalSouvenirs;

double getTotalOfSouvenirs(QVector<QPair<QString, double>> &souvenirs) {
    double total = 0;

    for (QPair<QString, double> souvenir : souvenirs) {
        total += souvenir.second;
    }

    return total;
}

/**
* @brief Function to go to the next stop.
*
* This function handles the transition to the next stop in the virtual trip. It adds the
* purchased souvenirs from the current stop to the total souvenirs list, clears the current
* souvenirs list, and either updates the UI to display the next stop or displays the end trip
* screen if it's the last stop.
*/
void MainWindow::nextStop() {
    //add currentSouvenirs to total
    for (QPair<QString, double> souvenir : currentSouvenirs) {
        totalSouvenirs.push_back(souvenir);
    }

    currentSouvenirs.clear();

    // update UI
    if (currentDestination == static_cast<int>(currentPath.size()) - 1) {
        this->endTripScreen(totalSouvenirs);
        this->changePage("endTrip");
        totalSouvenirs.clear();
    } else {
        currentDestination++;
        this->setCurrentDestination(currentDestination);
    }
}

/**
* @brief Function to handle a purchase.
*
* This function allows the user to purchase souvenirs at the current stop during the virtual
* trip. It iterates through the souvenir table, retrieves the selected quantities for each
* souvenir, adds the purchased souvenirs to the current souvenirs list, and updates the total
* spent on souvenirs.
*/

void MainWindow::purchase() {
    bool itemsSelected = false;

    // iterates thru each row of table
    for (int row = 0; row < this->ui->tableSouvenir->rowCount(); ++row) {
        QSpinBox *quantityBox = qobject_cast<QSpinBox *>(this->ui->tableSouvenir->cellWidget(row, 2));

        // check if theres a quantity box and its value is greater than 0
        if (!quantityBox || quantityBox->value() <= 0) { continue; }
        itemsSelected = true;

        // get souvenir, and add it to current purchased souvenirs quantity times
        QString name = this->ui->tableSouvenir->item(row, 0)->text();
        string stringcost = this->ui->tableSouvenir->item(row, 1)->text().toStdString().substr(1);
        double cost = QString::fromStdString(stringcost).toDouble();

        int quantity = quantityBox->value();

        for (int j = 0; j < quantity; j++) {
            currentSouvenirs.push_back({name, cost});
        }
    }

    if (!itemsSelected) {
        QMessageBox::information(this, "Purchase Failed", "You have no souvenirs selected!");
    }
    else {
        QMessageBox::information(this, "Purchase Confirmation", "Your purchase has been confirmed.");
    }

    // update total
    double total = getTotalOfSouvenirs(currentSouvenirs);
    QString totalString = "Total Spent: $" + QString::number(total, 'f', 2);

    this->ui->labelPurchaseTotal->setText(totalString);
}

/**
 * @brief Function to end the trip and display the screen.
 * @param totalSouvenirs The total souvenirs.
 */
void MainWindow::endTripScreen(QVector<QPair<QString, double>> &totalSouvenirs) {
    // EDIT POPUP
    QDialog *endPopup = new QDialog();
    QFormLayout *formLayout = new QFormLayout();
    endPopup->setStyleSheet("font: 500 13pt \"Bahnschrift\"");
    endPopup->resize(512, 512);

    // connect ok and cancel boxes
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
    QObject::connect(buttonBox, SIGNAL(accepted()), endPopup, SLOT(accept()));

    // labels
    QLabel *labelSouvenirs = new QLabel();
    labelSouvenirs->setText("Reciept");

    QLabel *labelTotal = new QLabel();
    double total = getTotalOfSouvenirs(totalSouvenirs);
    QString totalString = "Total: $" + QString::number(total, 'f', 2);
    labelTotal->setText(totalString);

    // table
    QTableWidget *table = new QTableWidget();
    table->setFrameShape(QAbstractScrollArea::Shape::Box);

    table->setColumnCount(2);
    table->setHorizontalHeaderLabels({"Souvenir", "Cost"});
    table->setRowCount(totalSouvenirs.size());


    for (int i = 0; i < table->rowCount(); i++) {
        QPair<QString, double> souvenir = totalSouvenirs[i];
        QTableWidgetItem *item = new QTableWidgetItem(souvenir.first);
        QTableWidgetItem *cost = new QTableWidgetItem("$" + QString::number(souvenir.second, 'f', 2));

        table->setItem(i, 0, item);
        table->setItem(i, 1, cost);
    }

    // final
    formLayout->addWidget(labelSouvenirs);
    formLayout->addWidget(table);
    formLayout->addWidget(labelTotal);
    formLayout->addWidget(buttonBox);

    endPopup->setLayout(formLayout);
    table->setColumnWidth(0, 332);
    table->setColumnWidth(1, 128);

    // run
    endPopup->exec();
}

/*
 * Algorithm stuff
*/

/*
 * PLAN TRIP STUFF
 */
/**
* @brief Function to initialize the algorithm UI.
*
* This function initializes the user interface components related to the algorithm feature.
* It populates the stadium combo box with the list of stadiums and their corresponding teams.
*/
void MainWindow::intializeAlgorithm() {
    this->ui->comboStadium->clear();

    // add stadiums to dropdown
    for (auto it = infoDf.cbegin(); it != infoDf.cend(); ++it  )
    {
        QString team = it.key();
        QString stadium = it.value()["Stadium name"];

        this->ui->comboStadium->addItem(stadium + " (" + team + ")");
    }

    this->algorithmChanged();
}
/**
* @brief Function to handle algorithm selection.
*
* This function is called when the user selects a different algorithm from the combo box. It
* enables or disables the stadium combo box based on whether the selected algorithm requires
* a starting stadium or not.
*/
void MainWindow::algorithmChanged() {
    QString currentAlg = this->ui->comboAlgorithm->currentText();

    this->ui->comboStadium->setEnabled((currentAlg == "BFS" || currentAlg == "DFS"));
}

QMap<QString, QString> algorithmStrings = {
        {"MST", "Minimum Spanning Tree (Prim's)"},
        {"BFS", "Breadth First Search"},
        {"DFS", "Depth First Search"},
};

/**
 * @brief Executes the selected algorithm.
 *
 * This function runs the algorithm selected in the combo box on the graph.
 * The graph is loaded from a CSV file. The function handles three types of algorithms:
 * Minimum Spanning Tree (Prim's), Breadth First Search, and Depth First Search.
 * Depending on the algorithm, it may also use the selected stadium as a starting point.
 */
void MainWindow::runAlgorithm() {
    // load file
    QDir distPath;
    distPath.cdUp();

    string path = distPath.path().toStdString() + "/" + projectFileName + "/Distance between stadiums.csv";
    QMap<QString, StadiumQ> graph = parseCSV(QString::fromStdString(path));
    unordered_map<string, vector<pair<string, int>>> graph2 = parseCSV2(QString::fromStdString(path));

    // get curr algorithm
    QString currentAlg = this->ui->comboAlgorithm->currentText();

    // get curr stadium
    string currentStadium = this->ui->comboStadium->currentText().toStdString();
    auto endPos = currentStadium.find('(');
    QString stadiumName = QString::fromStdString(currentStadium.substr(0, endPos - 1));

    // declare total dist
    int totalDistance = 0;

    // idk what this is but it was given to me
    QMap<QString, bool> visited;
    for (const auto& key : graph.keys()) {
        visited[key] = false;
    }

    if (currentAlg == "MST") {
        vector<Edge> mst = primMST(graph2);

        for (const auto& edge : mst) {
            totalDistance += edge.weight;
        }
    }
    else if (currentAlg == "BFS") {
        totalDistance = bfs(graph, stadiumName);
        //qDebug() << totalDistance;
    }
    else if (currentAlg == "DFS") {
        totalDistance = dfs(graph, stadiumName, visited);
        //qDebug() << totalDistance;
    }

    QString finalText = "Total Distance: " + QString::number(totalDistance);
    QString poopText = "Algorithm: " + algorithmStrings[currentAlg];

    this->ui->labelMileage->setText(finalText);
    this->ui->labelAlgSelected->setText(poopText);
