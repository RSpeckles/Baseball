#include "qdebug.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <queue>
#include <limits>
#include <algorithm>
#include <QString>
#include <QMap>

// DOP DOP DOP
const QString START_STADIUM = "Dodger Stadium";
const QString END_STADIUM = "Fenway Park";

// Dijistra skibidi
QVector<QPair<QString, double>> dijkstra(QMap<QString, QMap<QString, double>>& distTable, QString start, QString end) {
    QMap<QString, double> distance;
    QMap<QString, QString> previous;
    std::priority_queue<QPair<double, QString>> pq;

    for (auto it = distTable.begin(); it != distTable.end(); ++it) {
        distance[it.key()] = std::numeric_limits<double>::max();
    }
    distance[start] = 0;

    pq.push({0, start});

    while (!pq.empty()) {
        QString current = pq.top().second;
        pq.pop();

        if (current == end) {
            break;
        }

        for (auto it = distTable[current].begin(); it != distTable[current].end(); ++it) {
            double newDist = distance[current] + *it;
            if (newDist < distance[it.key()]) {
                distance[it.key()] = newDist;
                previous[it.key()] = current;
                pq.push({newDist, it.key()});
            }
        }
    }

    QVector<QPair<QString, double>> path;
    QString current = end;
    while (previous.find(current) != previous.end()) {
        path.push_back({current, distance[current]});
        current = previous[current];
    }
    path.push_back({start, 0});
    std::reverse(path.begin(), path.end());

    return path;
}

// to print the shortest path
void printPath(const QVector<QPair<QString, double>>& path) {
    std::cout << "Shortest path from " << START_STADIUM.toStdString() << " to " << END_STADIUM.toStdString() << ":\n";
    for (const auto& stadium : path) {
        std::cout << stadium.first.toStdString() << " - Distance: " << stadium.second << " miles\n";
    }
}

QVector<QPair<QString, double>> visitNumClosest(QMap<QString, QMap<QString, double>>& distTable, QString start, int numToVisit) {
    QVector<QPair<QString, double>> path;
    QString currDestination = start;
    path.push_back({start, 0});

    for (int i = 0; i < numToVisit; i++) {
        qDebug() << currDestination;
        QMap<QString, double> currDistances = distTable[currDestination];
        QPair<QString, double> shortest = {"", 999999999999999};

        for (auto it = currDistances.cbegin(); it != currDistances.cend(); ++it  )
        {
            QString name = it.key();
            double dist = it.value();

            // check if already exists in path
            bool alreadyInPath = false;
            for (QPair<QString, double> stadiums : path) {
                if (stadiums.first == name) { alreadyInPath = true; }
            }
            if (alreadyInPath == true) { continue; }

            if (dist < shortest.second) {
                shortest = {name, dist};
            }
        }

        path.push_back(shortest);
        currDestination = shortest.first;
    }

    return path;
}

// int main() {
//     const QString FILENAME = "/Users/dimitri.chrysafis/CLionProjects/Dijij/Distance between stadiums.csv";
//     QMap<QString, QMap<QString, double>> distTable;
//     csv_to_table(FILENAME, distTable);
//     QVector<QPair<QString, double>> shortestPath = dijkstra(distTable, START_STADIUM, END_STADIUM);
//     printPath(shortestPath);
//     return 0;
// }
