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

void Graph::setWeight(int u, int v, uint32_t weight) {
    if (u == v) throw std::invalid_argument("Tried to set weight for Edge from node " + std::to_string(u) + " to " + std::to_string(v) + " but no self loops exist.");
    if (u < 0 || u >= n) throw std::out_of_range("Tried to set weight for Edge from node " + std::to_string(u) + " to " + std::to_string(v) + " but but the first ID is out of bounds.");
    if (v < 0 || v >= n) throw std::out_of_range("Tried to set weight for Edge from node " + std::to_string(u) + " to " + std::to_string(v) + " but but the second ID is out of bounds.");

    if (u < v) {
        auto it = std::find(adj[u].begin(), adj[u].end(), v);

        if (it == adj[u].end()) throw std::invalid_argument("Tried to set weight for Edge from node " + std::to_string(u) + " to " + std::to_string(v) + " but they are not adjacent.");

        int index = std::distance(adj[u].begin(), it);
        upwardWeights[u][index] = weight;
    } else {
        std::swap(u, v);
        auto it = std::find(adj[u].begin(), adj[u].end(), v);

        if (it == adj[u].end()) throw std::invalid_argument("Tried to set weight for Edge from node " + std::to_string(u) + " to " + std::to_string(v) + " but they are not adjacent.");

        int index = std::distance(adj[u].begin(), it);
        downwardWeights[u][index] = weight;
    }
}

uint32_t Graph::getWeight(int u, int v) const {
    if (u == v) throw std::invalid_argument("Tried to get weight for Edge from node " + std::to_string(u) + " to " + std::to_string(v) + " but no self loops exist.");
    if (u < 0 || u >= n) throw std::out_of_range("Tried to get weight for Edge from node " + std::to_string(u) + " to " + std::to_string(v) + " but but the first ID is out of bounds.");
    if (v < 0 || v >= n) throw std::out_of_range("Tried to get weight for Edge from node " + std::to_string(u) + " to " + std::to_string(v) + " but but the second ID is out of bounds.");

    if (u < v) {
        auto it = std::find(adj[u].begin(), adj[u].end(), v);

        if (it == adj[u].end()) throw std::invalid_argument("Tried to get weight for Edge from node " + std::to_string(u) + " to " + std::to_string(v) + " but they are not adjacent.");

        int index = std::distance(adj[u].begin(), it);
        return upwardWeights[u][index];
    } else {
        std::swap(u, v);
        auto it = std::find(adj[u].begin(), adj[u].end(), v);

        if (it == adj[u].end()) throw std::invalid_argument("Tried to get weight for Edge from node " + std::to_string(u) + " to " + std::to_string(v) + " but they are not adjacent.");

        int index = std::distance(adj[u].begin(), it);
        return downwardWeights[u][index];
    }
}

bool Graph::isEdge(int u, int v) const {
    if (std::find(neighbors(u).begin(), neighbors(u).end(), v) == neighbors(u).end()) return false;
    else return true;
}

const std::vector<int>& Graph::neighbors(int u) const {
    return adj[u];
}

 std::vector<int>& Graph::neighbors(int u) {
    return adj[u];
}

void Graph::initEliminationTree() {
    eliminationTree.assign(n, NO_ET_PARENT);
}

void Graph::setParent(int node, int parent) {
    eliminationTree[node] = parent;
}

int Graph::parentOf(int node) const {
    return eliminationTree[node];
}