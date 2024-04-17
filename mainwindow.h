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
    // uhh no initialize functions yet lololol

    // connections
    void changePage();
    void changePage(QString specific);
    void adminLogin();

private slots:

private:
    Ui::MainWindow *ui;

    QMap<QString, QMap<QString, double>> distTable;
    QMap<QString, QMap<QString, QString>> infoDf;
};
#endif // MAINWINDOW_H
