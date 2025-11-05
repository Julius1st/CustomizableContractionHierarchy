//
// Created by Julius on 26.09.2025.
//
#pragma once
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <cstdint>

class Graph {
public:
    inline static const uint32_t INFINITY = 0xFFFFFFFF / 2 - 1;

    Graph(std::vector<uint32_t>& firstOut, std::vector<uint32_t>& head, std::vector<uint32_t>& upwardWeights, std::vector<uint32_t>& downwardWeights);
    Graph(std::vector<uint32_t>& firstOut, std::vector<uint32_t>& head, std::vector<uint32_t>& upwardWeights, std::vector<uint32_t>& downwardWeights, std::vector<uint32_t>& eliminationTree);

    uint32_t numVertices() const { return n;}
    uint32_t getHead(uint32_t edgeID) const;
    uint32_t getUpwardWeight(uint32_t edgeID) const; // the edgeID corresponds to the index the edge Head has in the head vector.
    uint32_t getDownwardWeight(uint32_t  edgeID) const; // the edgeID corresponds to the index the edge Head has in the head vector.
    void setUpwardWeight(uint32_t edgeID, uint32_t weight); // the edgeID corresponds to the index the edge Head has in the head vector.
    void setDownwardWeight(uint32_t edgeID, uint32_t weight); // the edgeID corresponds to the index the edge Head has in the head vector.

    std::vector<uint32_t>::const_iterator beginNeighborhood(uint32_t u) const; // first Element part of the neighborhood
    std::vector<uint32_t>::const_iterator endNeighborhood(uint32_t u) const; // first Element not part of the neighborhood

    void initEliminationTree();
    uint32_t parentOf(uint32_t node) const;

    // TODO: implement with throwing exceptions
    const std::vector<uint32_t>& getPrecomputedNodes(uint32_t node) const { return precomputedNodes[node]; } // nodes to which distances have been precomputed from node
    const std::vector<uint32_t>& getPrecomputedDistancesUp(uint32_t node) const {return precomputedDistancesUp[node];} // distances from 'node' to precomputed Nodes of node
    const std::vector<uint32_t>& getPrecomputedDistancesDown(uint32_t node) const {return precomputedDistancesDown[node];} // distances from 'node' to precomputed Nodes of node

private:
    uint32_t n; // Number of Vertices
    uint32_t m; // Number of Edges
    std::vector<uint32_t> firstOut;
    std::vector<uint32_t> head;
    std::vector<uint32_t> eliminationTree;
    // Weights are stored at the same Index as their respective Edge Head in head.
    // Weights are stored in upwardWeights if head node has higher ID than tail node. downwardWeights the other way around.
    std::vector<uint32_t> upwardWeights;
    std::vector<uint32_t> downwardWeights;

    // For query speed-ups
    std::vector<std::vector<uint32_t>> precomputedNodes; // nodes to which distances have been precomputed
    std::vector<std::vector<uint32_t>> precomputedDistancesUp; // Corresponding distances from all other nodes to each node in precomputedNodes (precomputedDistancesUp[node][i] = distance from node to precomputedNodes[node][i])
    std::vector<std::vector<uint32_t>> precomputedDistancesDown; // Corresponding distances from all other nodes to each node in precomputedNodes
};
