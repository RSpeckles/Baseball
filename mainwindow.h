#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    // initializers
    void initializeTables();

    // connections
    void changePage();
    void changePage(QString specific);
    void adminLogin();
    void filterTeams();

private slots:

private:
    Ui::MainWindow *ui;

    QMap<QString, QMap<QString, double>> distTable;
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
