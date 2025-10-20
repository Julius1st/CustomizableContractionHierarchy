//
// Created by Julius on 26.09.2025.
//

#pragma once
#include "core/Graph.hpp"
#include <vector>

// Builds the Augmented Graph G+ and the elimination tree
class ContractionBuilder {
public:
    ContractionBuilder(Graph* g, std::vector<uint32_t>& rankOrder) : G(g), order(rankOrder) {}

    Graph* buildGplus();

    // These Methods are used to get the proper weight vectors for the customization phase
    std::vector<uint32_t> permuteWeights(const std::vector<uint32_t>& weights);
    std::vector<uint32_t> buildNewWeightsForGplus(const std::vector<uint32_t>& permutedWeights);
    uint32_t getRank(uint32_t u) {return rank[u];} // rank[oldID] = newID

private:
    const Graph* G;
    Graph* Gplus;
    std::vector<uint32_t> rank; // rank[oldID] = newID
    std::vector<uint32_t> permutedFirstOut;
    std::vector<uint32_t> permutedHead;
    std::vector<uint32_t> order; // order[newID] = oldID
    std::vector<uint32_t> GplusHead;
    std::vector<uint32_t> GplusFirstOut;

    void permuteNodeIDs();
    void contractGraph();
    uint32_t findEdgeInOriginal(uint32_t lowerOld, uint32_t upperOld);
};