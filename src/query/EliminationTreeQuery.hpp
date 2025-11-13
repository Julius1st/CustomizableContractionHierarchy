//
// Created by Julius on 26.09.2025.
//

#pragma once
#include "core/Graph.hpp"
#include <vector>
#include <chrono>

// Elimination-tree-based query
class EliminationTreeQuery {
public:
    EliminationTreeQuery(Graph* g);

    uint32_t query(uint32_t s, uint32_t t);

    int getInitTime() const { return initTime; }
    uint32_t getInitializedFields() const { return initializedFields; }

private:
    const Graph* G;

    std::vector<uint32_t> distUp;
    std::vector<uint32_t> distDown;

    void ProcessVertexUp(uint32_t u, uint32_t d);
    void ProcessVertexDown(uint32_t u, uint32_t d);

    int initTime = 0;
    uint32_t initializedFields = 0;
};