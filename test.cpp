//
// Created by Julius on 21.10.2025.
//
#include <iostream>
#include <stdexcept>
#include <queue>
#include "CCH.hpp"

using namespace std;

struct DirectedEdge {
    uint32_t a, b;
    uint32_t weight; // weight from a->b
};

uint32_t dijkstra(uint32_t s, uint32_t t, vector<vector<DirectedEdge>> adj) {
    uint32_t n = adj.size();
    uint32_t infinity = 0xFFFFFFFF/2 -1;
    vector<uint32_t> dist(n, infinity);
    vector<uint32_t> predecessor(n, infinity);
    dist[s] = 0;
    priority_queue<pair<uint32_t, uint32_t>, vector<pair<uint32_t, uint32_t>>, greater<pair<uint32_t, uint32_t>> > pq;

    pq.push({0, s});
    while (!pq.empty()) {
        uint32_t u = pq.top().second;
        pq.pop();

        if (u == t) return dist[t];

        for (auto it = adj[u].begin(); it != adj[u].end(); it++) {
            uint32_t v = it->b;
            uint32_t weight = it->weight;
            if (dist[v] > dist[u] + weight) {
                dist[v] = dist[u] + weight;
                pq.push({dist[v], v});
            }
        }
    }

    return dist[t];
}

int main() {

    vector<uint32_t> first_out = {0, 2, 3, 4, 5, 7, 8, 8, 8};
    vector<uint32_t> head = {2, 3, 3, 4, 4, 5, 6, 7};
    vector<uint32_t> order = {7, 6, 5, 4, 3, 2, 1, 0};
    vector<uint32_t> upward_weight = {2, 3, 13, 24, 34, 45, 46, 57};
    vector<uint32_t> downward_weight = {20, 30, 31, 42, 43, 54, 64, 75};
    vector<vector<DirectedEdge>> adj;
    adj.resize(8);
    adj[0] = {{0,2,2}, {0,3,3}};
    adj[1] = {{1,3,13}};
    adj[2] = {{2,4,24}, {2, 0, 20}};
    adj[3] = {{3,4,34}, {3,0,30}, {3,1,31}};
    adj[4] = {{4,5,45}, {4,2,42}, {4,3,43}, {4, 6, 46}};
    adj[5] = {{5, 4, 54}, {5, 7, 57}};
    adj[6] = {{6, 4, 64}};
    adj[7] = {{7, 5, 75}};

    Graph* G = new Graph(first_out, head, upward_weight, downward_weight);

    CCH* cch = new CCH(G, order);

    cch->preprocess();

    cch->customize();

    Graph* Gplus = cch->getGplus();
    cout << "Gplus FirstOut: ";
    for (auto fo : Gplus->getFirstOut()) {
        cout << fo << " ";
    }
    cout << endl;
    cout << "Gplus Head: ";
    for (auto h : Gplus->getHead()) {
        cout << h << " ";
    }
    cout << endl;
    cout << "Gplus Upward Weights: ";
    for (auto w : Gplus->getUpwardWeights()) {
        cout << w << " ";
    }
    cout << endl;
    cout << "Gplus Downward Weights: ";
    for (auto w : Gplus->getDownwardWeights()) {
        cout << w << " ";
    }
    cout << endl;
    cout << "Gplus Elimination Tree: ";
    for (auto et : Gplus->getEliminationTree()) {
        cout << et << " ";
    }
    cout << endl;

    uint32_t s = 6;
    uint32_t t = 0;

    uint32_t distance = cch->query(s, t);
    cout << "The computed distance is: " + std::to_string(distance) << endl;
    uint32_t dijkstra_distance = dijkstra(s, t, adj);
    cout << "The Dijkstra distance is: " + std::to_string(dijkstra_distance) << endl;
}