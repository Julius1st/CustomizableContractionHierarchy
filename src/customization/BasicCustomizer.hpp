//
// Created by Julius on 26.09.2025.
//

#pragma once
#include "core/Graph.hpp"
#include <vector>

// Enforces lower triangle inequalities (bottom-up)
class BasicCustomizer {
public:
    BasicCustomizer(AugmentedGraph& g);

    void run();

private:
    AugmentedGraph& Gplus;
    std::vector<double> w_up; //forward weights
    std::vector<double> w_down; //backward weights
};