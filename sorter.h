#ifndef SORTER_H
#define SORTER_H

#include <QDebug>
#include <iostream>
#include <QString>
#include <QMap>
#include <algorithm>

#include <QRegularExpression>
QRegularExpression digits("\\d+"); // Matches one or more digits

using namespace std;

// since strings can be something like "407 feet (124m)" we need to convert the first num into an int
int stringToInt(QString stringToConvert) {
    QRegularExpressionMatch match = digits.match(stringToConvert);

    if (match.hasMatch()) {
        QString matchedText = match.captured();
        return matchedText.toInt();
    }

    return 0;
}

// function returns greatest and shortestC= CF
void getGreatestAndShortestCF(QMap<QString, QMap<QString, QString>> df, int &greatestCF, int &shortestCF)
{
    for (auto it = df.cbegin(), end = df.cend(); it != end; it++){
        int currentCF = stringToInt((*it)["Distance to center field"]);

        if (currentCF > greatestCF) {
            greatestCF = currentCF;
        }
        else if (currentCF < shortestCF) {
            shortestCF = currentCF;
        }
    }
}

// Sort by. First parameter is dataframe (should be infoDf)
// Second is aspect to sort by (must be spelled exactly as is in CSV) default is "Team name"
// Last is what league to include. "Major" (default) includes all teams.
// "American" only shows American league and "National" only shows National league.
// "Open" only shows leagues with open roof stadiums.
// "GreatestCF" only shows leagues with the greatest CF.
// "ShortestCF" only shows leagues with the shortest CF.
QVector<QString> sort_by_teams(QMap<QString, QMap<QString, QString>> df, QString sort_by = "Team name", string filter = "Major"){
    QVector<QString> teams;
    QVector<QPair<QString, QString>> pairs;

    // get greatest and shortestCF
    int greatestCF = 0;
    int shortestCF = 999999999;
    if (filter == "GreatestCF" || filter == "ShortestCF") {
        getGreatestAndShortestCF(df, greatestCF, shortestCF);
    }

    // iterate and append
    for (auto it = df.cbegin(), end = df.cend(); it != end; it++){
        // if filter is "American" or "National" or "Open
        if (filter == "Major" || filter == (*it)["League"].toStdString() || filter == (*it)["Roof Type"].toStdString()){
            pairs.append({it.key(), (*it)[sort_by]});
            // cout << (*it)["Stadium name"].toStdString() << endl;
        }

        // if filter is greatestCF or shortestCF
        int currentCF = stringToInt((*it)["Distance to center field"]);

        if ((filter == "GreatestCF" && greatestCF == currentCF) || (filter == "ShortestCF" && shortestCF == currentCF)) {
            pairs.append({it.key(), (*it)[sort_by]});
        }
    }

    sort(pairs.begin(), pairs.end(), [](auto& a, auto& b){
        // cout << a.second.toStdString() << " - " << b.second.toStdString() << endl;
        return a.second < b.second;
    });

    for (auto &it : pairs){
        // cout << it.first.toStdString() << " : " << it.second.toStdString() << endl;
        teams.push_back(it.first);
    }

    return teams;
}

#endif // SORTER_H
