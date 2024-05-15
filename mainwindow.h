#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
}
QT_END_NAMESPACE

/**
 * @brief The MainWindow class represents the main window of the application.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a MainWindow object.
     * @param parent The parent widget.
     */
    MainWindow(QWidget *parent = nullptr);

    /**
     * @brief Destroys the MainWindow object.
     */
    ~MainWindow();

public slots:
            /*
             * INITIALIZERS
            */

            /**
             * @brief Initializes the MainWindow.
             */
            void initialize();

    /*
     * CONNECTIONS
    */

    // general

    /**
     * @brief Changes the page displayed in the MainWindow.
     */
    void changePage();

    /**
     * @brief Changes the page to a specific one.
     * @param specific The specific page to change to.
     */
    void changePage(QString specific);

    /**
     * @brief Performs admin login.
     */
    void adminLogin();

    /**
     * @brief Filters teams.
     */
    void filterTeams();

    /**
     * @brief Views souvenirs.
     */
    void viewSouvenirs();

    /**
     * @brief Updates souvenirs.
     * @param senderName The name of the sender.
     */
    void updateSouvenirs(QString senderName);

    // admin

    /**
     * @brief Handles admin team selection.
     */
    void adminTeamSelected();

    /**
     * @brief Confirms admin edit.
     */
    void adminConfirmEdit();

    /**
     * @brief Handles admin souvenir selection.
     */
    void adminSouvenirSelected();

    /**
     * @brief Edits admin souvenir.
     */
    void adminEditSouvenir();

    /**
     * @brief Saves data to CSV file.
     */
    void saveToCSV();

    /**
     * @brief Loads data from CSV file.
     */
    void loadFromCSV();

    // planning

    /**
     * @brief Initializes stadium plan.
     */
    void initializeStadiumPlan();

    /**
     * @brief Gets destinations.
     */
    void getDestinations();

    /**
     * @brief Plans trip.
     */
    void planTrip();

    /**
     * @brief Starts trip.
     */
    void startTrip();

    /**
     * @brief Sets current destination.
     * @param index The index of the destination.
     */
    void setCurrentDestination(int index);

    /**
     * @brief Moves to next stop.
     */
    void nextStop();

    /**
     * @brief Purchases souvenirs.
     */
    void purchase();

    /**
     * @brief Shows end trip screen.
     * @param totalSouvenirs Vector containing pairs of souvenir names and prices.
     */
    void endTripScreen(QVector<QPair<QString, double>> &totalSouvenirs);

    // algorithm

    /**
     * @brief Initializes algorithm.
     */
    void intializeAlgorithm();

    /**
     * @brief Handles algorithm change.
     */
    void algorithmChanged();

    /**
     * @brief Runs algorithm.
     */
    void runAlgorithm();


private slots:

private:
    Ui::MainWindow *ui;

    std::string projectFileName = "Baseball";
    QMap<QString, QMap<QString, double>> distTable;
    QMap<QString, QMap<QString, double>> souvenirTable;
    QMap<QString, QMap<QString, QString>> infoDf;

    QVector<QString> stadiums = {"Arizona Diamondbacks",
                                 "Atlanta Braves",
                                 "Baltimore Orioles",
                                 "Boston Red Sox",
                                 "Chicago Cubs",
                                 "Chicago White Sox",
                                 "Cincinnati Reds",
                                 "Cleveland Indians",
                                 "Colorado Rockies",
                                 "Detroit Tigers",
                                 "Houston Astros",
                                 "Kansas City Royals",
                                 "Los Angeles Angels",
                                 "Los Angeles Dodgers",
                                 "Miami Marlins",
                                 "Milwaukee Brewers",
                                 "Minnesota Twins",
                                 "New York Mets",
                                 "New York Yankees",
                                 "Oakland Athletics",
                                 "Philadelphia Phillies",
                                 "Pittsburgh Pirates",
                                 "San Diego Padres",
                                 "San Francisco Giants",
                                 "Seattle Mariners",
                                 "St. Louis Cardinals",
                                 "Tampa Bay Rays",
                                 "Texas Rangers",
                                 "Toronto Blue Jays",
                                 "Washington Nationals"};
};
#endif // MAINWINDOW_H
