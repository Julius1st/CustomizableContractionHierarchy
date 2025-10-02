//
// Created by Julius on 26.09.2025.
//

#pragma once
#include "core/Graph.hpp"
#include "core/EliminationTree.hpp"
#include <vector>

// Builds the Augmented Graph G+ and the elimination tree
class ContractionBuilder {
public:
    ContractionBuilder(const Graph& g, Graph& Gplus, const std::vector<int>& rankOrder) : G(g), rank(rankOrder) {Gplus = Gplus;}

    // Expects that numVertices(Gplus) == numVertices(G)
    void buildGplus();

private:
    const Graph& G;
    Graph Gplus;
    const std::vector<int> rank;

    void permuteNodeIDs();
    void contractGraph();
};