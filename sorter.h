#ifndef SORTER_H
#define SORTER_H

#include <iostream>
#include <QString>
#include <QMap>
#include <algorithm>

using namespace std;

QVector<QString> sort_by_team(QVector<QString> &teams, QMap<QString, QMap<QString, QString>> df){
    std::sort(teams.begin(), teams.end());

    return teams;
}

QVector<QString> sort_by_stadium(QMap<QString, QMap<QString, QString>> df){
    QVector<QString> teams;
    QVector<QPair<QString, QString>> pairs;

    for (auto it = df.cbegin(), end = df.cend(); it != end; it++){
        pairs.append({it.key(), (*it)["Stadium name"]});
        cout << (*it)["Stadium name"].toStdString() << endl;
    }

    sort(pairs.begin(), pairs.end(), [](auto& a, auto& b){
        // cout << a.second.toStdString() << " - " << b.second.toStdString() << endl;
        return a.second < b.second;
    });

    for (auto &it : pairs){
        cout << it.first.toStdString() << " : " << it.second.toStdString() << endl;
        teams.push_back(it.first);
    }

    return teams;
}


#endif // SORTER_H
