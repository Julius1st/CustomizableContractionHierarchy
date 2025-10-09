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

    // These Methods are used to get the proper weight vectors for the customization phase
    std::vector<uint32_t>* permuteWeights(std::vector<uint32_t> weights);
    std::vector<uint32_t>* buildNewWeightsForGplus(std::vector<uint32_t> permutedWeights);

private:
    const Graph& G;
    Graph* Gplus;
    const std::vector<uint32_t> rank;
    std::vector<uint32_t> permutedFirstOut;
    std::vector<uint32_t> permutedHead;
    std::vector<uint32_t> reverseRank;
    std::vector<uint32_t> GplusHead;
    std::vector<uint32_t> GplusFirstOut;

    void permuteNodeIDs();
    void contractGraph();
};