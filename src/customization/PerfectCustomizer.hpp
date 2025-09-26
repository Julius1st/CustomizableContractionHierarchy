//
// Created by Julius on 26.09.2025.
//

#pragma once
#include "core/Graph.hpp"

// Refines weights using upper/intermediate triangle (top-down)
class PerfectCustomizer {
public:
    PerfectCustomizer(AugmentedGraph& g);

    void run();

private:
    AugmentedGraph& Gplus;
};