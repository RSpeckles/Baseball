#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include <QDebug>
#include <QString>
#include <QMap>
#include <QIODevice>
#include <QFile>
#include <queue>
#include <QQueue>
#include <unordered_set>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>

struct Distance {
    QString srcStadium;
    QString destStadium;
    int distance;

    Distance(const QString& src, const QString& dest, int dist) : srcStadium(src), destStadium(dest), distance(dist) {}
};

struct Stadium {
    QString name;
    std::vector<std::pair<QString, int>> connections;

    Stadium() {} // Default constructor

    Stadium(const QString& n) : name(n) {} // Constructor with name argument
};

struct Edge {
    std::string src;
    std::string dest;
    int weight;
    Edge(std::string s, std::string d, int w) : src(s), dest(d), weight(w) {}
};

struct CompareEdge {
    bool operator()(const Edge& e1, const Edge& e2) const {
        return e1.weight > e2.weight;
    }
};

std::vector<Distance> parseDistanceCSV(const QString& filename) {
    std::vector<Distance> distances;
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Error opening file" << filename;
        return distances;
    }

    QTextStream in(&file);
    in.readLine();

    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList parts = line.split(",");
        if (parts.size() < 3) {
            qWarning() << "Invalid line:" << line;
            continue;
        }
        QString src = parts[0];
        QString dest = parts[1];
        int distance = parts[2].toInt();

        distances.emplace_back(src, dest, distance);
    }

    file.close();
    return distances;
}

std::vector<Stadium> buildGraph(const std::vector<Distance>& distances) {
    std::unordered_set<QString> uniqueStadiums;
    std::vector<Stadium> graph;

    for (const auto& distance : distances) {
        uniqueStadiums.insert(distance.srcStadium);
        uniqueStadiums.insert(distance.destStadium);
    }

    for (const auto& stadiumName : uniqueStadiums) {
        graph.emplace_back(stadiumName);
    }

    for (const auto& distance : distances) {
        for (auto& stadium : graph) {
            if (stadium.name == distance.srcStadium) {
                stadium.connections.emplace_back(distance.destStadium, distance.distance);
            }
        }
    }

    return graph;
}

void printMSTPath(const std::unordered_map<QString, QString>& parent) {
    qDebug() << "Minimum Spanning Tree Path:";
    for (const auto& [child, parent] : parent) {
        qDebug() << parent << " -> " << child;
    }
}

struct StadiumQ {
    QString name;
    QMap<QString, int> connections;
};

QMap<QString, StadiumQ> parseCSV(const QString& filename) {
    QMap<QString, StadiumQ> graph;
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Error opening file";
        return graph;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList parts = line.split(",");
        QString origin = parts[0];
        QString destination = parts[1];
        int distance = parts[2].toDouble();

        if (!graph.contains(origin))
            graph[origin] = {origin, {}};

        if (!graph.contains(destination))
            graph[destination] = {destination, {}};

        graph[origin].connections[destination] = distance;
        graph[destination].connections[origin] = distance;
    }

    file.close();
    return graph;
}

std::unordered_map<std::string, std::vector<std::pair<std::string, int>>> parseCSV2(const QString& file_path) {
    std::ifstream file(file_path.toStdString());
    std::unordered_map<std::string, std::vector<std::pair<std::string, int>>> graph;
    std::string line;

    if (file.is_open()) {
        getline(file, line);

        while (getline(file, line)) {
            std::stringstream ss(line);
            std::string src, dest, weight_str;

            getline(ss, src, ',');
            getline(ss, dest, ',');
            getline(ss, weight_str, ',');
            int weight = stoi(weight_str);

            graph[src].push_back({dest, weight});
            graph[dest].push_back({src, weight});
        }
        file.close();
    }

    return graph;
}

int dfs(const QMap<QString, StadiumQ>& graph, const QString& startStadium, QMap<QString, bool>& visited) {
    visited[startStadium] = true;
    int totalDistance = 0;

    //qDebug() << "Visited:" << startStadium;

    auto it = graph.find(startStadium);
    if (it == graph.end()) {
        qWarning() << "Error: Starting stadium not found in the graph.";
        return totalDistance;
    }

    for (const auto& adjacentStadium : it.value().connections.keys()) {
        if (!visited[adjacentStadium]) {
            int distance = it.value().connections.value(adjacentStadium);
            //qDebug() << "Traversing from" << startStadium << "to" << adjacentStadium << "(Distance:" << distance << "miles)";
            totalDistance += distance + dfs(graph, adjacentStadium, visited);
        }
    }

    return totalDistance;
}

int bfs(const QMap<QString, StadiumQ>& graph, const QString& startStadium) {
    QMap<QString, int> mileage;
    QQueue<QString> q;
    for (const auto& key : graph.keys()) {
        mileage[key] = INT_MAX;
    }
    q.enqueue(startStadium);
    mileage[startStadium] = 0;
    int totalDistance = 0;
    while (!q.isEmpty()) {
        QString currentStadium = q.dequeue();
        for (const auto& adjacentStadium : graph[currentStadium].connections.keys()) {
            if (mileage[currentStadium] + graph[currentStadium].connections[adjacentStadium] < mileage[adjacentStadium]) {
                mileage[adjacentStadium] = mileage[currentStadium] + graph[currentStadium].connections[adjacentStadium];
                q.enqueue(adjacentStadium);
                totalDistance += graph[currentStadium].connections[adjacentStadium];
            }
        }
    }

    return totalDistance;
}

std::vector<Edge> primMST(const std::unordered_map<std::string, std::vector<std::pair<std::string, int>>>& graph) {
    std::vector<Edge> mst;
    std::unordered_map<std::string, bool> visited;
    std::priority_queue<Edge, std::vector<Edge>, CompareEdge> pq;

    for (const auto& edge : graph.begin()->second) {
        pq.push({graph.begin()->first, edge.first, edge.second});
    }
    visited[graph.begin()->first] = true;

    while (!pq.empty()) {
        Edge min_edge = pq.top();
        pq.pop();

        if (visited[min_edge.dest]) continue;

        mst.push_back(min_edge);
        visited[min_edge.dest] = true;

        for (const auto& edge : graph.at(min_edge.dest)) {
            if (!visited[edge.first]) {
                pq.push({min_edge.dest, edge.first, edge.second});
            }
        }
    }

    return mst;
}

#endif // ALGORITHMS_H
