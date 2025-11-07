//
// Created by Julius on 26.09.2025.
//
#include "Graph.hpp"

Graph::Graph(std::vector<uint32_t> &firstOut, std::vector<uint32_t> &head, std::vector<uint32_t>& upwardWeightsVec, std::vector<uint32_t>& downwardWeightsVec) :
firstOut(firstOut), head(head), upwardWeights(upwardWeightsVec), downwardWeights(downwardWeightsVec) {
    if (upwardWeights.size() != head.size())
        throw std::invalid_argument("Graph constructor: upwardWeights size does not match head size.");
    if (downwardWeights.size() != head.size())
        throw std::invalid_argument("Graph constructor: downwardWeights size does not match head size.");
    n = firstOut.size() -1;
    m = head.size();
    initEliminationTree();
}

Graph::Graph(std::vector<uint32_t> &firstOut, std::vector<uint32_t> &head, std::vector<uint32_t>& upwardWeightsVec, std::vector<uint32_t>& downwardWeightsVec, std::vector<uint32_t>& eliminationTree) :
    firstOut(firstOut), head(head), upwardWeights(upwardWeightsVec), downwardWeights(downwardWeightsVec), eliminationTree(eliminationTree) {
    if (upwardWeights.size() != head.size())
        throw std::invalid_argument("Graph constructor: upwardWeights size does not match head size.");
    if (downwardWeights.size() != head.size())
        throw std::invalid_argument("Graph constructor: downwardWeights size does not match head size.");
    n = firstOut.size() -1;
    m = head.size();
}

Graph::Graph(std::vector<uint32_t> &firstOut, std::vector<uint32_t> &head, std::vector<uint32_t>& upwardWeightsVec, std::vector<uint32_t>& downwardWeightsVec, std::vector<uint32_t>& eliminationTree,
             std::vector<std::vector<uint32_t>> &precomputedNodes, std::vector<std::vector<uint32_t>> &precomputedDistancesUp, std::vector<std::vector<uint32_t>> &precomputedDistancesDown) :
        firstOut(firstOut), head(head), upwardWeights(upwardWeightsVec), downwardWeights(downwardWeightsVec), eliminationTree(eliminationTree),
        precomputedNodes(precomputedNodes), precomputedDistancesUp(precomputedDistancesUp), precomputedDistancesDown(precomputedDistancesDown) {

    if (upwardWeights.size() != head.size())
        throw std::invalid_argument("Graph constructor: upwardWeights size does not match head size.");
    if (downwardWeights.size() != head.size())
        throw std::invalid_argument("Graph constructor: downwardWeights size does not match head size.");
    n = firstOut.size() -1;
    m = head.size();
}

uint32_t Graph::getHead(uint32_t edgeID) const {
    if (edgeID >= m) throw std::out_of_range("Tried to get head for Edge with ID " + std::to_string(edgeID) + " but this ID is out of bounds.");
    return head[edgeID];
}

uint32_t Graph::getUpwardWeight(uint32_t edgeID) const {
    if (edgeID >= m) throw std::out_of_range("Tried to get weight for Edge with ID " + std::to_string(edgeID) + " but this ID is out of bounds.");
    return upwardWeights[edgeID];
}

uint32_t Graph::getDownwardWeight(uint32_t edgeID) const {
    if (edgeID >= m) throw std::out_of_range("Tried to get weight for Edge with ID " + std::to_string(edgeID) + " but this ID is out of bounds.");
    return downwardWeights[edgeID];
}

void Graph::setUpwardWeight(uint32_t edgeID, uint32_t weight) {
    if (edgeID >= m) throw std::out_of_range("Tried to set weight for Edge with ID " + std::to_string(edgeID) + " but this ID is out of bounds.");
    upwardWeights[edgeID] = weight;
}

void Graph::setDownwardWeight(uint32_t edgeID, uint32_t weight) {
    if (edgeID >= m) throw std::out_of_range("Tried to set weight for Edge with ID " + std::to_string(edgeID) + " but this ID is out of bounds.");
    downwardWeights[edgeID] = weight;
}

std::vector<uint32_t>::const_iterator Graph::beginNeighborhood(uint32_t u) const {
    if (u >= n) throw std::out_of_range("Tried to get begin of neighborhood of node " + std::to_string(u) + " but this ID is out of bounds.");
    return (head.begin() + firstOut[u]);
}

std::vector<uint32_t>::const_iterator Graph::endNeighborhood(uint32_t u) const {
    if (u >= n) throw std::out_of_range("Tried to get end of neighborhood of node " + std::to_string(u) + " but this ID is out of bounds.");
    return (head.begin() + firstOut[(u+1)]);
}

void Graph::initEliminationTree() {
    eliminationTree.assign(n, INFINITY);
}

uint32_t Graph::parentOf(uint32_t node) const {
    if (node >= n) throw std::out_of_range("Tried to get parent of node " + std::to_string(node) + " but this ID is out of bounds.");
    return eliminationTree[node];
}