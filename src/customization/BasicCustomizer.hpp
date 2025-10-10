//
// Created by Julius on 26.09.2025.
//

#pragma once
#include "core/Graph.hpp"
#include <vector>

// Enforces lower triangle inequalities (bottom-up)
class BasicCustomizer {
public:
    explicit BasicCustomizer(Graph& Gplus) {Gplus = Gplus;}

    void run();

private:
    Graph Gplus;
};