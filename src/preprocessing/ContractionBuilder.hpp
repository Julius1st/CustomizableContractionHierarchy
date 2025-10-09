//
// Created by Julius on 26.09.2025.
//

#pragma once
#include "core/Graph.hpp"
#include <vector>

// Builds the Augmented Graph G+ and the elimination tree
class ContractionBuilder {
public:
    ContractionBuilder(const Graph& g, const std::vector<uint32_t>& rankOrder) : G(g), rank(rankOrder) {}

    Graph* buildGplus();
    std::vector<uint32_t> permuteWeights(std::vector<uint32_t> weights);

private:
    const Graph& G;
    Graph* Gplus;
    const std::vector<uint32_t> rank;
    std::vector<uint32_t> permutedFirstOut;
    std::vector<uint32_t> permutedHead;
    std::vector<uint32_t> reverseRank;

    void permuteNodeIDs();
    void contractGraph();
};