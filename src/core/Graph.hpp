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
    static const uint32_t MAX_UINT32 = 0xFFFFFFFF;

    explicit Graph(uint32_t n = 0, uint32_t m = 0);
    Graph(std::vector<uint32_t>& firstOut, std::vector<uint32_t>& head, std::vector<uint32_t>& upwardWeights, std::vector<uint32_t>& downwardWeights);
    Graph(std::vector<uint32_t>& firstOut, std::vector<uint32_t>& head, std::vector<uint32_t>& eliminationTree);

    uint32_t numVertices() const { return n;}
    uint32_t numEdges() const {return m;}
    uint32_t getWeight(uint32_t u, uint32_t v) const; // u is considered tail and v is considered head of edge
    bool isEdge(uint32_t u, uint32_t v) const ; // u is considered tail and v is considered head of edge
    std::vector<uint32_t>& getUpwardWeights() {return upwardWeights;}
    std::vector<uint32_t>& getDownwardWeights() {return downwardWeights;}

    void addEdge(uint32_t u, uint32_t v); // u is considered tail and v is considered head of edge
    void setWeight(uint32_t u, uint32_t v, uint32_t weight); // u is considered tail and v is considered head of edge

    auto beginNeighborhood(uint32_t u) const {return head.begin() + firstOut[u];} //first Element part of the neighborhood
    auto endNeighborhood(uint32_t u) const {return head.begin() + firstOut[(u+1)];} //first Element not part of the neighborhood
    //const std::vector<uint32_t>& neighbors(uint32_t u) const; // Only returns upper neighborhood (neighbors with higher ID than u)
    //std::vector<uint32_t>& neighbors(uint32_t u); // Only returns upper neighborhood (neighbors with higher ID than u)

    void initEliminationTree();
    void setParent(uint32_t node, uint32_t parent);
    uint32_t parentOf(uint32_t node) const;

private:
    uint32_t n;
    uint32_t m;
    std::vector<uint32_t> firstOut;
    std::vector<uint32_t> head;
    //std::vector<std::vector<int>> adj; // Edges are undirected, but are only stored at the vertex with lower ID
    std::vector<uint32_t> eliminationTree;
    // Accessed with help of adj: weight of u to v = upwardWeights[u][index of v in adj[u]]
    // Weights are stored in upwardWeights if head node has higher ID than tail node and vice versa
    std::vector<uint32_t> upwardWeights;
    std::vector<uint32_t> downwardWeights;
};
