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
    const uint32_t MAX_UINT32 = 0xFFFFFFFF;

    Graph(int n = 0);

    int numVertices() const { return n;}
    uint32_t getWeight(int u, int v) const {return weights[u][v];}

    void addEdge(int u, int v);
    void setWeight(int u, int v, uint32_t weight);
    void removeEdge(int u, int v);

    const std::vector<int>& neighbors(int u) const; // Only returns upper neighborhood (neighbors with higher ID than u)
    std::vector<int>& neighbors(int u); // Only returns upper neighborhood (neighbors with higher ID than u)

    void initEliminationTree();
    void setParent(int node, int parent);
    int parentOf(int node) const;

private:
    int n;
    std::vector<std::vector<int>> adj; // Edges are undirected, but are only stored at the vertex with lower ID
    std::vector<int> eliminationTree;
    std::vector<std::vector<uint32_t>> weights; // Weights are directional (weights[u][v] not necessarily equal to weights[v][u]
};
