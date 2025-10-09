//
// Created by Julius on 26.09.2025.
//
#include "Graph.hpp"

Graph::Graph(uint32_t n, uint32_t m) : n(n), m(m){
    initEliminationTree();
}

Graph::Graph(std::vector<uint32_t> &firstOut, std::vector<uint32_t> &head, std::vector<uint32_t> &upwardWeights, std::vector<uint32_t> &downwardWeights) : firstOut(firstOut), head(head), upwardWeights(upwardWeights), downwardWeights(downwardWeights) {
    n = firstOut.size() -1;
    m = head.size();
    initEliminationTree();
}

Graph::Graph(std::vector<uint32_t> &firstOut, std::vector<uint32_t> &head, std::vector<uint32_t>& eliminationTree) : firstOut(firstOut), head(head), eliminationTree(eliminationTree) {
    n = firstOut.size() -1;
    m = head.size();
}

//TODO
void Graph::addEdge(uint32_t u, uint32_t v) {
    if (u >= n) throw std::out_of_range("Tried to add an Edge from node " + std::to_string(u) + " to " + std::to_string(v) + " but but the first ID is out of bounds.");
    if (v >= n) throw std::out_of_range("Tried to add an Edge from node " + std::to_string(u) + " to " + std::to_string(v) + " but but the second ID is out of bounds.");
    if (u == v) return; // We don't want selfloops

    if (u > v) std::swap(u, v);
    if (std::find(adj[u].begin(), adj[u].end(), v) != adj[u].end()) return; // If edge exists already, don't add another one
    adj[u].push_back(v);
}

void Graph::setWeight(uint32_t u, uint32_t v, uint32_t weight) {
    if (u == v) throw std::invalid_argument("Tried to set weight for Edge from node " + std::to_string(u) + " to " + std::to_string(v) + " but no self loops exist.");
    if (u >= n) throw std::out_of_range("Tried to set weight for Edge from node " + std::to_string(u) + " to " + std::to_string(v) + " but but the first ID is out of bounds.");
    if (v >= n) throw std::out_of_range("Tried to set weight for Edge from node " + std::to_string(u) + " to " + std::to_string(v) + " but but the second ID is out of bounds.");

    if (u < v) {
        auto it = std::find(head.begin() + firstOut[u], head.begin() + firstOut[(u+1)], v);

        if (it == head.begin() + firstOut[(u+1)]) throw std::invalid_argument("Tried to set weight for Edge from node " + std::to_string(u) + " to " + std::to_string(v) + " but they are not adjacent.");

        uint32_t index = std::distance(head.begin(), it);
        upwardWeights[index] = weight;
    } else {
        std::swap(u, v);
        auto it = std::find(head.begin() + firstOut[u], head.begin() + firstOut[(u+1)], v);

        if (it == head.begin() + firstOut[(u+1)]) throw std::invalid_argument("Tried to set weight for Edge from node " + std::to_string(u) + " to " + std::to_string(v) + " but they are not adjacent.");

        uint32_t index = std::distance(head.begin(), it);
        downwardWeights[index] = weight;
    }
}

uint32_t Graph::getWeight(uint32_t u, uint32_t v) const {
    if (u == v) throw std::invalid_argument("Tried to get weight for Edge from node " + std::to_string(u) + " to " + std::to_string(v) + " but no self loops exist.");
    if (u >= n) throw std::out_of_range("Tried to get weight for Edge from node " + std::to_string(u) + " to " + std::to_string(v) + " but but the first ID is out of bounds.");
    if (v >= n) throw std::out_of_range("Tried to get weight for Edge from node " + std::to_string(u) + " to " + std::to_string(v) + " but but the second ID is out of bounds.");

    if (u < v) {
        auto it = std::find(head.begin() + firstOut[u], head.begin() + firstOut[(u+1)], v);

        if (it == head.begin() + firstOut[(u+1)]) throw std::invalid_argument("Tried to get weight for Edge from node " + std::to_string(u) + " to " + std::to_string(v) + " but they are not adjacent.");

        uint32_t index = std::distance(head.begin(), it);
        return upwardWeights[index];
    } else {
        std::swap(u, v);
        auto it = std::find(head.begin() + firstOut[u], head.begin() + firstOut[(u+1)], v);

        if (it == head.begin() + firstOut[(u+1)]) throw std::invalid_argument("Tried to get weight for Edge from node " + std::to_string(u) + " to " + std::to_string(v) + " but they are not adjacent.");

        uint32_t index = std::distance(head.begin(), it);
        return downwardWeights[index];
    }
}

bool Graph::isEdge(uint32_t u, uint32_t v) const {
    if (std::find(head.begin() + firstOut[u], head.begin() + firstOut[(u+1)], v) == head.begin() + firstOut[(u+1)]) return false;
    else return true;
}

//TODO remove?
const std::vector<uint32_t>& Graph::neighbors(int u) const {
    return adj[u];
}

 std::vector<uint32_t>& Graph::neighbors(int u) {
    return adj[u];
}

void Graph::initEliminationTree() {
    eliminationTree.assign(n, MAX_UINT32);
}

void Graph::setParent(uint32_t node, uint32_t parent) {
    eliminationTree[node] = parent;
}

uint32_t Graph::parentOf(uint32_t node) const {
    return eliminationTree[node];
}