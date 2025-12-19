//
// Created by julius on 15.12.25.
//

#pragma once
#include "core/Graph.hpp"

class PerfectCustomizer {
public:
    explicit PerfectCustomizer(Graph* Graph);

    void run();

private:
    Graph* G;
    std::vector<bool> deleteUp;
    std::vector<bool> deleteDown;
};