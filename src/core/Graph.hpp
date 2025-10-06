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
    static const int NO_ET_PARENT = -1;

    explicit Graph(int n = 0);

    int numVertices() const { return n;}
    uint32_t getWeight(int u, int v) const; // u is considered tail and v is considered head of edge
    bool isEdge(int u, int v) const ; // u is considered tail and v is considered head of edge
    std::vector<std::vector<uint32_t>>& getUpwardWeights() {return upwardWeights;}
    std::vector<std::vector<uint32_t>>& getDownwardWeights() {return downwardWeights;}

    void addEdge(int u, int v); // u is considered tail and v is considered head of edge
    void setWeight(int u, int v, uint32_t weight); // u is considered tail and v is considered head of edge
    void removeEdge(int u, int v); // u is considered tail and v is considered head of edge

    const std::vector<int>& neighbors(int u) const; // Only returns upper neighborhood (neighbors with higher ID than u)
    std::vector<int>& neighbors(int u); // Only returns upper neighborhood (neighbors with higher ID than u)

    void initEliminationTree();
    void setParent(int node, int parent);
    int parentOf(int node) const;

private:
    int n;
    std::vector<std::vector<int>> adj; // Edges are undirected, but are only stored at the vertex with lower ID
    std::vector<int> eliminationTree;
    // Accessed with help of adj: weight of u to v = upwardWeights[u][index of v in adj[u]]
    // Weights are stored in upwardWeights if head node has higher ID than tail node and vice versa
    std::vector<std::vector<uint32_t>> upwardWeights;
    std::vector<std::vector<uint32_t>> downwardWeights;
};
