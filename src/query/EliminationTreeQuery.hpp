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
    EliminationTreeQuery(const Graph& g);

    uint32_t query(int s, int t);

private:
    const Graph& Gplus;

    std::vector<uint32_t> distUp;
    std::vector<uint32_t> distDown;

    void ProcessVertexUp(int u, uint32_t d);
    void ProcessVertexDown(int u, uint32_t d);
};