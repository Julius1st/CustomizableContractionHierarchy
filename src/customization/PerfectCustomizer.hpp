//
// Created by julius on 15.12.25.
//

#pragma once
#include "core/Graph.hpp"

class PerfectCustomizer {
public:
    explicit PerfectCustomizer(Graph* Graph);

    void run();

    Graph* getGperfectUp() const { return GperfectUp; }
    Graph* getGperfectDown() const { return GperfectDown; }

private:
    void customize();
    void generateGup();
    void generateGdown();

    Graph* G;
    Graph* GperfectUp;
    Graph* GperfectDown;

    std::vector<bool> deleteUp;
    std::vector<bool> deleteDown;
};