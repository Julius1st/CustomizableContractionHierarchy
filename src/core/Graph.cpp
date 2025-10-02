//
// Created by Julius on 26.09.2025.
//
#include "Graph.hpp"

Graph::Graph(int n) : n(n), adj(n){
    initEliminationTree();
}

void Graph::addEdge(int u, int v) {
    if (u < 0 || u >= n) throw std::out_of_range("Tried to add an Edge from node " + std::to_string(u) + " to " + std::to_string(v) + " but but the first ID is out of bounds.");
    if (v < 0 || v >= n) throw std::out_of_range("Tried to add an Edge from node " + std::to_string(u) + " to " + std::to_string(v) + " but but the second ID is out of bounds.");
    if (u == v) return; // We don't want selfloops

    if (u > v) std::swap(u, v);
    if (std::find(adj[u].begin(), adj[u].end(), v) != adj[u].end()) return; // If edge exists already, don't add another one
    adj[u].push_back(v);
}

void Graph::removeEdge(int u, int v) {
    if (u == v) throw std::invalid_argument("Tried to remove an Edge from node " + std::to_string(u) + " to " + std::to_string(v) + " but no self loops exist.");
    if (u < 0 || u >= n) throw std::out_of_range("Tried to remove an Edge from node " + std::to_string(u) + " to " + std::to_string(v) + " but but the first ID is out of bounds.");
    if (v < 0 || v >= n) throw std::out_of_range("Tried to remove an Edge from node " + std::to_string(u) + " to " + std::to_string(v) + " but but the second ID is out of bounds.");

    if (u > v) std::swap(u, v);
    std::vector<int>& nu = adj[u];
    auto it = std::find(nu.begin(), nu.end(), v);
    if (it != nu.end()) nu.erase(it);
}

const std::vector<int>& Graph::neighbors(int u) const {
    return adj[u];
}

 std::vector<int>& Graph::neighbors(int u) {
    return adj[u];
}

void Graph::initEliminationTree() {
    eliminationTree.assign(n, -1);
}

void Graph::setParent(int node, int parent) {
    eliminationTree[node] = parent;
}

int Graph::parentOf(int node) const {
    return eliminationTree[node];
}