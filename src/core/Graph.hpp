//
// Created by Julius on 26.09.2025.
//
#pragma once
#include <vector>
#include <stdexcept>
#include <algorithm>

class Graph {
public:
    Graph(int n = 0);

    int numVertices() const { return n;}

    void addEdge(int u, int v);
    void removeEdge(int u, int v);

    const std::vector<int>& neighbors(int u) const;
    std::vector<int>& neighbors(int u);

    void initEliminationTree();
    void setParent(int node, int parent);
    int parentOf(int node) const;

private:
    int n;
    std::vector<std::vector<int>> adj;
    std::vector<int> eliminationTree;
};
