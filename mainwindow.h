#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <string>

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

private:
    Ui::MainWindow *ui;

    QMap<QString, QMap<QString, double>> distTable;
    QMap<QString, QMap<QString, std::string>> infoDf;

    void csv_to_table(std::string path, QMap<QString, QMap<QString, double>> &dataframe);
    void csv_to_df(std::string path, QMap<QString, QMap<QString, std::string>> &dataframe);
};
#endif // MAINWINDOW_H
