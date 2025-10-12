//
// Created by Julius on 26.09.2025.
//
#include "Graph.hpp"

Graph::Graph(std::vector<uint32_t> &firstOut, std::vector<uint32_t> &head) : firstOut(firstOut), head(head) {
    n = firstOut.size() -1;
    m = head.size();
    initEliminationTree();
}

Graph::Graph(std::vector<uint32_t> &firstOut, std::vector<uint32_t> &head, std::vector<uint32_t>& eliminationTree) : firstOut(firstOut), head(head), eliminationTree(eliminationTree) {
    n = firstOut.size() -1;
    m = head.size();
}

uint32_t Graph::getUpwardWeight(uint32_t edgeID) const {
    if (edgeID >= n) throw std::out_of_range("Tried to get weight for Edge with ID " + std::to_string(edgeID) + " but this ID is out of bounds.");
    return upwardWeights[edgeID];
}

uint32_t Graph::getDownwardWeight(uint32_t edgeID) const {
    if (edgeID >= n) throw std::out_of_range("Tried to get weight for Edge with ID " + std::to_string(edgeID) + " but this ID is out of bounds.");
    return downwardWeights[edgeID];
}

void Graph::setUpwardWeight(uint32_t edgeID, uint32_t weight) {
    if (edgeID >= n) throw std::out_of_range("Tried to set weight for Edge with ID " + std::to_string(edgeID) + " but this ID is out of bounds.");
    upwardWeights[edgeID] = weight;
}

void Graph::setDownwardWeight(uint32_t edgeID, uint32_t weight) {
    if (edgeID >= n) throw std::out_of_range("Tried to set weight for Edge with ID " + std::to_string(edgeID) + " but this ID is out of bounds.");
    upwardWeights[edgeID] = weight;
}

void Graph::initEliminationTree() {
    eliminationTree.assign(n, MAX_UINT32);
}

uint32_t Graph::parentOf(uint32_t node) const {
    return eliminationTree[node];
}