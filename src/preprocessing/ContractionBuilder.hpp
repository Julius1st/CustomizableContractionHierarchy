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
    ContractionBuilder(const Graph& g, const std::vector<int>& rankOrder);

    AugmentedGraph build(AugmentedGraph& Gplus, EliminationTree& ET);

private:
    const Graph& G;
    std::vector<int> rank;
};