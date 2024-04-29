#ifndef SORTER_H
#define SORTER_H

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

    for (auto &it : df){
        pairs.append({it.first(), it["Stadium name"]});
    }

    sort(pairs.begin(), pairs.end(), [](auto& a, auto& b){
        return a.second < b.second;
    });

    for (auto &it : pairs){
        teams.push_back(it.first);
    }

    return teams;
}


#endif // SORTER_H
