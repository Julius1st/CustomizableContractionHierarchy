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

    Graph(std::vector<uint32_t>& firstOut, std::vector<uint32_t>& head);
    Graph(std::vector<uint32_t>& firstOut, std::vector<uint32_t>& head, std::vector<uint32_t>& eliminationTree);

    uint32_t test(uint32_t id) {return firstOut[id];}

    uint32_t numVertices() const { return n;}
    uint32_t numEdges() const {return m;}
    uint32_t getHead(uint32_t edgeID) const;
    uint32_t getUpwardWeight(uint32_t edgeID) const; // the edgeID corresponds to the index the edge Head has in the head vector.
    uint32_t getDownwardWeight(uint32_t  edgeID) const; // the edgeID corresponds to the index the edge Head has in the head vector.
    void setUpwardWeight(uint32_t edgeID, uint32_t weight); // the edgeID corresponds to the index the edge Head has in the head vector.
    void setDownwardWeight(uint32_t edgeID, uint32_t weight); // the edgeID corresponds to the index the edge Head has in the head vector.

    void setUpwardWeights(std::vector<uint32_t> newWeights) {upwardWeights = newWeights;}
    void setDownwardWeights(std::vector<uint32_t> newWeights) {downwardWeights = newWeights;}

    std::vector<uint32_t>::const_iterator beginNeighborhood(uint32_t u) const; // first Element part of the neighborhood
    std::vector<uint32_t>::const_iterator endNeighborhood(uint32_t u) const; // first Element not part of the neighborhood

    void initEliminationTree();
    uint32_t parentOf(uint32_t node) const;

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
};
