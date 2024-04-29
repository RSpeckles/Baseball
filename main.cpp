#include <iostream>
#include <set>
#include <map>
#include <unordered_set>
#include <queue>
#include <algorithm>
#include <functional>
using namespace std;
struct compare {
    bool operator()(const pair<string, int> &lhs,
                    const pair<string, int> &rhs) const {
        return lhs.second < rhs.second;
    }
};
map<string, multiset<pair<string, int>,compare>> graph;

void addEdge(string src, string dest, int weight) {
    graph[src].insert({dest, weight});
}
void dfs(string start) {
    unordered_set<string> visited;
    unordered_set<string> being_visited; // To track nodes being visited in current DFS traversal
    vector<pair<string, string>> discovery_edges, forward_edges, back_edges, cross_edges;
    int total_distance = 0;

    function<void(string)> dfs_helper = [&](string node) {
        visited.insert(node);
        being_visited.insert(node); // Mark the current node as being visited
        for (auto [neighbor, distance] : graph[node]) {
            if (being_visited.find(neighbor) != being_visited.end() && neighbor != node) {
                back_edges.push_back({node, neighbor}); // Found a back edge
            } else if (visited.find(neighbor) == visited.end()) {
                discovery_edges.push_back({node, neighbor});
                total_distance += distance;
                dfs_helper(neighbor);
                forward_edges.push_back({node, neighbor});
            } else {
                cross_edges.push_back({node, neighbor});
            }
        }
        being_visited.erase(node); // Mark the current node as finished
    };

    dfs_helper(start);

    cout << "DFS Discovery Edges:\n";
    for (auto edge : discovery_edges) {
        cout << edge.first << " -> " << edge.second << "\n";
    }
    cout << "\nDFS Forward Edges:\n";
    for (auto edge : forward_edges) {
        cout << edge.first << " -> " << edge.second << "\n";
    }
    cout << "\nDFS Back Edges:\n";
    for (auto edge : back_edges) {
        cout << edge.first << " -> " << edge.second << "\n";
    }
    cout << "\nDFS Cross Edges:\n";
    for (auto edge : cross_edges) {
        cout << edge.first << " -> " << edge.second << "\n";
    }
    cout << "\nTotal Distance Travelled (DFS): " << total_distance << endl;
}

void bfs(string start) {
    unordered_set<string> visited;
    vector<pair<string, string>> discovery_edges, forward_edges, back_edges, cross_edges;
    int total_distance = 0;

    queue<pair<string, string>> q;
    q.push({start, start}); // Start node with itself as the parent

    while (!q.empty()) {
        auto [node, parent] = q.front();
        q.pop();

        if (visited.find(node) == visited.end()) {
            visited.insert(node);
            for (auto [neighbor, weight] : graph[node]) {
                if (visited.find(neighbor) == visited.end()) {
                    discovery_edges.push_back({node, neighbor});
                    total_distance += weight;
                    q.push({neighbor, node}); // Pass the current node as the parent
                    forward_edges.push_back({node, neighbor});
                } else if (neighbor != parent) { // If the neighbor is not the parent, it's a cross edge
                    cross_edges.push_back({node, neighbor});
                }
            }
        }
    }

    cout << "\nBFS Discovery Edges:\n";
    for (auto edge : discovery_edges) {
        cout << edge.first << " -> " << edge.second << "\n";
    }
    cout << "\nBFS Forward Edges:\n";
    for (auto edge : forward_edges) {
        cout << edge.first << " -> " << edge.second << "\n";
    }
    cout << "\nBFS Cross Edges:\n";
    for (auto edge : cross_edges) {
        cout << edge.first << " -> " << edge.second << "\n";
    }
    cout << "\nTotal Distance Travelled (BFS): " << total_distance << endl;
}

int main() {
    addEdge("Seattle", "San Francisco", 807);
    addEdge("Seattle", "Denver", 1331);
    addEdge("San Francisco", "Los Angeles", 381);
    addEdge("Los Angeles", "Denver", 1015);
    addEdge("Dallas", "Los Angeles", 1435);
    addEdge("Dallas", "Kansas City", 496);
    addEdge("Dallas", "Atlanta", 781);
    addEdge("Houston", "Dallas", 239);
    addEdge("Houston", "Atlanta", 810);
    addEdge("Miami", "Houston", 1187);
    addEdge("Atlanta", "Miami", 661);
    addEdge("New York", "Atlanta", 1260);
    addEdge("New York", "Chicago", 787);
    addEdge("Boston", "New York", 214);
    addEdge("Chicago", "Boston", 983);
    addEdge("Chicago", "Seattle", 2097);
    addEdge("Denver", "San Francisco", 1267);
    addEdge("Denver", "Chicago", 1003);
    addEdge("Kansas City", "Chicago", 533);
    addEdge("Kansas City", "Denver", 599);
    addEdge("Kansas City", "Los Angeles", 1663);
    addEdge("Kansas City", "Atlanta", 864);
    addEdge("Kansas City", "New York", 1260);

    cout << "DFS Starting at Dallas\n";
    dfs("Dallas");

    cout << "\nBFS Starting at Dallas\n";
    bfs("Dallas");

    return 0;
}
