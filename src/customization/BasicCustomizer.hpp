//
// Created by Julius on 26.09.2025.
//

#pragma once
#include "core/Graph.hpp"
#include <vector>

// Enforces lower triangle inequalities (bottom-up)
class BasicCustomizer {
public:
    explicit BasicCustomizer(Graph* Graph);

    void run();

private:
    Graph* G;

    // stored edges, used for path reconstruction with (u, v, w) is a triangle with u < v < w
    // the names do not reflect the orientation of the edges, as they are all oriented upwards
    // these are used to reconstruct vw (upward):
    std::vector<uint32_t> vuEdgeForUp;
    std::vector<uint32_t> uwEdgeForUp;
    //these are used to reconstruct wv (downward):
    std::vector<uint32_t> wuEdgeForDown;
    std::vector<uint32_t> uvEdgeForDown;
};