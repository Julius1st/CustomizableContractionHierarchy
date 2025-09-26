//
// Created by Julius on 26.09.2025.
//
#pragma once
#include <vector>

// A directed edge
struct Edge {
    int tail;
    int head;
    double weight;
    bool isShortcut;
    int via; // used for unpacking, -1 if none
};

class Graph {
public:
    Graph(int n = 0);

    void addEdge(int u, int v, double w);
    const std::vector<Edge>& neighbors(int u) const;

    int numVertices() const;
    int numEdges() const;

private:
    int n;
    std::vector<std::vector<Edge>> adj;
    std::vector<Edge> edges;
};

class AugmentedGraph {
public:
    AugmentedGraph(int n = 0);

    void addEdge(int u, int v, int w, bool shortcut = false, int via = -1);
    const std::vector<Edge>& upwardNeighbors(int u) const;

    int numVertices() const;
    int numEdges() const;

private:
    int n;
    std::vector<std::vector<Edge>> upAdj;
    std::vector<Edge> edges;
};