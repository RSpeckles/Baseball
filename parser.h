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
        // cout << buffer[i] << " ";
        if (buffer[i] == ',') {
            numCols++;
        }
    }
    cout << endl;

    char *temp = strtok(buffer, "¿");
    temp = strtok(NULL, ",");
    colNames.append(temp);
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
                // cout << row << endl;
            }
            dataframe[QString::fromStdString(row)][*iter] = QString::fromStdString(strBuffer);
            // cout << strBuffer << " - ";
            iter++;
            strBuffer = "";
        } else if (ch == '\n') {
            // cout << strBuffer << endl;
            dataframe[QString::fromStdString(row)][*iter] = QString::fromStdString(strBuffer);
            iter++;
            strBuffer = "";
            iter = colNames.begin();
        } else if (ch == '\"') {
            quotes += 1;
        }

        else {
            strBuffer += ch;
        }
    }
    // cout << dataframe["Arizona Diamondbacks"]["League"].toStdString() << endl;
}


//Write stuff to CSV file from dataframe
//First parameter is full path of file you want to write to
//Second parameter is dataframe map you're writing from
//Last parameter is title of rows. Write as one string, with titles separated by commas. No Spaces
//eg. "Team name,Souvenir,Cost"

// souvenirTable
void df_to_csv(string path,
               QMap<QString, QMap<QString, double>> const dataframe,
               QString label)
{
    ofstream csv;
    cout << path << endl;
    csv.open(path);

    if (!csv) {
        cout << "Could not open file! :(" << endl;
        return;
    }

    csv << label.toStdString() << "\n";

    for (auto i = dataframe.begin(); i != dataframe.end(); i++) {
        for (auto j = i->begin(); j != i->end(); j++) {
            bool flag = false;
            foreach (QChar c, i.key()) {
                if (c == ',') {
                    flag = true;
                }
            }

            if (flag) {
                csv << "\"" << i.key().toStdString() << "\""
                    << ",";
            } else {
                csv << i.key().toStdString() << ",";
            }

            flag = false;

            foreach (QChar c, j.key()) {
                if (c == ',') {
                    flag = true;
                }
            }

            if (flag)
                csv << "\"" << j.key().toStdString() << "\""
                    << ",";
            else
                csv << j.key().toStdString() << ",";

            csv << fixed << setprecision(2) << j.value() << "\n";
        }
    }
}

// infoDf
void df_to_csv(string path,
               QMap<QString, QMap<QString, QString>> const dataframe,
               QString label)
{
    ofstream csv;
    cout << path << endl;
    csv.open(path);

    if (!csv) {
        cout << "Could not open file! :(" << endl;
        return;
    }

    /* THIS IS UTTERLY FUCKING AUTISTIC
     * basically to save to the MLB Information.csv correctly we have to write it as UTF-8 BOM
     * i have no idea what this means but we need this so that it reads in correctly next time we open the app
     * I honestly have no fucking idea how other groups will figure this out SO THANKS THIS PROJECT FUCKING SUCKS
     * - Nico 5/3/2023 7:35pm
    */
    unsigned char bom[] = { 0xEF,0xBB,0xBF };
    csv.write((char*)bom, sizeof(bom));

    // add columns
    csv << label.toStdString() << "\n";

    /* split columns string so that we can find it in the dataframe rather than iterating thru each key
     * QMaps automatically order keys alphabetically, the provided CSV isnt sorted alphabetically
     * THIS IS THE ONLY WAY MUAHAHH!!!!!!
    */
    string s = label.toStdString();
    string delimiter = ",";

    size_t pos = 0;
    string token;
    vector<QString> keys;
    while ((pos = s.find(delimiter)) != std::string::npos) {
        token = s.substr(0, pos);
        keys.push_back((QString::fromStdString(token)));
        s.erase(0, pos + delimiter.length());
    }
    keys.push_back((QString::fromStdString(s)));

    for (auto i = dataframe.begin(); i != dataframe.end(); i++) {
        int index = 0;
        for (QString key : keys) {
            // check if it exists
            //qDebug() << key;
            if (i.value().find(key) == i->end()) {; continue; }

            bool flag = false;
            foreach (QChar c, i.value()[key]) {
                if (c == ',') {
                    flag = true;
                }
            }

            if (flag) {
                csv << "\"" << i.value()[key].toStdString() << "\""
                    << ",";
            } else {
                csv << i.value()[key].toStdString();
                if (keys.size()-1 > index) {
                    csv << ",";
                }
            }
            index++;
        }
        csv << "\n";
    }
}


#endif // PARSER_H
