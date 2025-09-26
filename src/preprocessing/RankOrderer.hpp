//
// Created by Julius on 26.09.2025.
//

#pragma once
#include "core/Graph.hpp"
#include <vector>

class RankOrderer {
public:
    RankOrderer(const Graph& g);

    std::vector<int> computeOrder();

private:
    const Graph& G;
};