#ifndef SORTER_H
#define SORTER_H

#include <iostream>
#include <QString>
#include <QMap>
#include <algorithm>

using namespace std;


// Sort by. First parameter is dataframe (should be infoDf)
// Second is aspect to sort by (must be spelled exactly as is in CSV) default is "Team name"
// Last is what league to include. "Major" (default) includes all teams.
// "American" only shows American league and "National" only shows National league.
QVector<QString> sort_by_teams(QMap<QString, QMap<QString, QString>> df, QString sort_by = "Team name", string league = "Major"){
    QVector<QString> teams;
    QVector<QPair<QString, QString>> pairs;

    for (auto it = df.cbegin(), end = df.cend(); it != end; it++){
        if (league == "Major" || league == (*it)["League"].toStdString()){
            pairs.append({it.key(), (*it)[sort_by]});
            // cout << (*it)["Stadium name"].toStdString() << endl;
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
