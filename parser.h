#ifndef PARSER_H
#define PARSER_H

#include <QDir>
#include <QMapIterator>
#include <QString>
#include <fstream>
#include <iostream>
#include <string>

using namespace std;
void csv_to_table(std::string path, QMap<QString, QMap<QString, double>> &dataframe)
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
                row.assign(buffer);
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
            // cout << row << " - " << col << " - " << val << endl;
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

void csv_to_df(std::string path, QMap<QString, QMap<QString, QString>> &dataframe)
{
    // cout << "AAA" << endl;
    ifstream csv(path);

    if (!csv) {
        cout << "Could not open file! :(" << endl;
    }

    char buffer[1024];
    int numCols = 0;
    QVector<QString> colNames;

    csv.getline(buffer, 1024);

    for (int i = 0; i < strlen(buffer); i++) {
        if (buffer[i] == ',') {
            numCols++;
        }
    }

    char *temp = strtok(buffer, ",");
    // cout << temp << endl;
    for (int i = 1; i <= numCols; i++) {
        temp = strtok(NULL, ",");
        colNames.append(temp);
        // cout << temp << endl;
    }

    char ch;
    string strBuffer, row;
    unsigned quotes = 0, count = 0;

    QVector<QString>::Iterator iter = colNames.begin();

    while ((ch = csv.get()) != EOF) {
        if ((ch == ',' && quotes % 2 == 0)) {
            if (iter == colNames.begin()) {
                row = strBuffer;
                dataframe[QString::fromStdString(row)];
            }
            // cout << infoDf[row].first().toStdString() << endl;
            // cout << strBuffer << endl;
            dataframe[QString::fromStdString(row)][*iter] = QString::fromStdString(strBuffer);
            iter++;
            strBuffer = "";
        } else if (ch == '\n') {
            iter = colNames.begin();
        } else if (ch == '\"') {
            quotes += 1;
        }

        else {
            strBuffer += ch;
        }
    }
}

#endif // PARSER_H
