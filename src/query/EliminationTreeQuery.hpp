//
// Created by Julius on 26.09.2025.
//

#pragma once
#include "core/Graph.hpp"
#include "core/EliminationTree.hpp"
#include <vector>

// Elimination-tree-based query
class EliminationTreeQuery {
public:
    EliminationTreeQuery(const AugmentedGraph& g, const EliminationTree& et);

    double query(int s, int t);
    std::vector<int> unpackPath(int s, int t);

private:
    const AugmentedGraph& Gplus;
    const EliminationTree& ET;

    std::vector<double> distUp;
    std::vector<double> distDown;
};