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

    uint32_t getRank(uint32_t u) {return rank[u];} // rank[oldID] = newID

    // just for testing
    std::vector<uint32_t> getRank(){ return rank; }

private:
    const Graph* G;
    Graph* Gplus;
    std::vector<uint32_t> rank; // rank[oldID] = newID
    std::vector<uint32_t> permutedFirstOut;
    std::vector<uint32_t> permutedHead;
    std::vector<uint32_t> order; // order[newID] = oldID
    std::vector<uint32_t> GplusHead;
    std::vector<uint32_t> GplusFirstOut;
    std::vector<uint32_t> GplusUpwardWeights;
    std::vector<uint32_t> GplusDownwardWeights;
    // Temporary adjacency for new graph tuple: (head, upWeight, downWeight)
    std::vector<std::vector<std::tuple<uint32_t, uint32_t, uint32_t>>> adj;

    void permuteNodeIDs();
    void contractGraph();
};