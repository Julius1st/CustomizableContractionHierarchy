//
// Created by Julius on 26.09.2025.
//

#include "ContractionBuilder.hpp"

void ContractionBuilder::buildGplus() {
    permuteNodeIDs();

    // Do contraction of Graph
    contractGraph();
}


void ContractionBuilder::permuteNodeIDs() {
    int n = G.numVertices();
    for (int u = 0; u < n; u++) {
        for (int v : G.neighbors(u)) {
            int newU = rank[u];
            int newV = rank[v];
            Gplus.addEdge(newU, newV);
        }
    }
}

void ContractionBuilder::contractGraph() {
    for (int u = 0; u < Gplus.numVertices(); u++) {
        std::vector<int>& neighborsU = Gplus.neighbors(u);
        if (neighborsU.empty()) continue;
        std::sort(neighborsU.begin(), neighborsU.end());
        neighborsU.erase(unique(neighborsU.begin(), neighborsU.end()), neighborsU.end());
        int v = neighborsU[0];
        Gplus.neighbors(v).insert(Gplus.neighbors(v).end(), (neighborsU.begin() +1), neighborsU.end());

        // Build Elimination Tree
        Gplus.setParent(u, v);
    }
}