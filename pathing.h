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

using namespace std;

// DOP DOP DOP
const QString START_STADIUM = "Dodger Stadium";
const QString END_STADIUM = "Fenway Park";

// this is for when we only need to visit the closest stadiums given only the number and starting stadium
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

// read stadium distances
unordered_map<string, unordered_map<string, int>> readStadiumDistances(const string& filename) {
    ifstream file(filename);
    unordered_map<string, unordered_map<string, int>> graph;

    if (!file.is_open()) {
        cerr << "Error: Unable to open file " << filename << endl;
        exit(1);
    }

    string line;
    getline(file, line);

    while (getline(file, line)) {
        stringstream ss(line);
        string origin, destination, distance_str;
        getline(ss, origin, ',');
        getline(ss, destination, ',');
        getline(ss, distance_str, ',');
        int distance = stoi(distance_str);

        graph[origin][destination] = distance;
        graph[destination][origin] = distance;
    }

    file.close();
    return graph;
}

// stole this shit lmao
unordered_map<string, int> dijkstra(const unordered_map<string, unordered_map<string, int>>& graph, const string& start) {
    unordered_map<string, int> distances;
    priority_queue<pair<int, string>, vector<pair<int, string>>, greater<pair<int, string>>> pq;

    for (const auto& entry : graph) {
        distances[entry.first] = INT_MAX;
    }
    distances[start] = 0;
    pq.push({0, start});

    while (!pq.empty()) {
        auto [current_distance, current_node] = pq.top();
        pq.pop();

        if (current_distance > distances[current_node]) {
            continue;
        }

        for (const auto& neighbor : graph.at(current_node)) {
            int distance = current_distance + neighbor.second;
            if (distance < distances[neighbor.first]) {
                distances[neighbor.first] = distance;
                pq.push({distance, neighbor.first});
            }
        }
    }

    return distances;
}

// FUCK THIS SHIT https://www.youtube.com/watch?v=DRDvrqjTR7s&list=LL&index=1
// i listedned to that osng in loop this entire time
QVector<QPair<QString, double>> findClosestStadiums(const unordered_map<string, int>& distances, const vector<string>& stadiums, int num_closest) {
    vector<pair<string, int>> sorted_distances(distances.begin(), distances.end());
    sort(sorted_distances.begin(), sorted_distances.end(), [](const pair<string, int>& a, const pair<string, int>& b) {
        return a.second < b.second;
    });

    QVector<QPair<QString, double>> path;

    cout << "Closest " << num_closest << " stadiums:" << endl;
    int count = 0;
    for (const auto& [stadium, distance] : sorted_distances) {
        if (count >= num_closest) {
            break;
        }
        if (find(stadiums.begin(), stadiums.end(), stadium) != stadiums.end()) {
            path.push_back({QString::fromStdString(stadium), distance});

            //cout << "Stadium: " << stadium << ", Distance: " << distance << endl;
            count++;
        }
    }

    return path;
}
