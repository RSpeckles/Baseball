/**
 * @file main.cpp
 * @brief This file contains the main function to run the application.
 */
#include "mainwindow.h"

#include <QApplication>

/**
 * @brief The main function to run the application.
 * @param argc The number of command-line arguments.
 * @param argv The array of command-line arguments.
 * @return Returns the exit status of the application.
 */
int main(int argc, char *argv[])
{
    /**
     * @brief QApplication manages the GUI application's control flow and main settings.
     * @param argc The number of command-line arguments.
     * @param argv The array of command-line arguments.
     */
    QApplication a(argc, argv);

    /**
        * @brief Define the main window of the application.
        */
    MainWindow window;
    window.show();

    /**
        * @brief Initialize the main window.
        */
    window.initialize();

    /**
    * @brief Starts the event loop and waits until exit() is called.
    * @return Returns the value that was set to exit() (which is 0 if exit() is called via quit()).
    */
    return a.exec();
}

//Test test test
