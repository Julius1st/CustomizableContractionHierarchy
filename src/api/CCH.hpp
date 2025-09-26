//
// Created by Julius on 26.09.2025.
//

#pragma once
#include "core/Graph.hpp"
#include "core/EliminationTree.hpp"
#include "preprocessing/RankOrderer.hpp"
#include "preprocessing/ContractionBuilder.hpp"
#include "customization/BasicCustomizer.hpp"
#include "customization/PerfectCustomizer.hpp"
#include "query/EliminationTreeQuery.hpp"
#include <vector>

// Facade Class
class CCH {
public:
    explicit CCH(const Graph& baseGraph);

    void preprocess();
    void customize(const std::vector<double>& weights);
    double query(int s, int t);
    std::vector<int> getPath(int s, int t);

private:
    Graph G;
    AugmentedGraph Gplus;
    EliminationTree ET;

    std::vector<int> rankOrder;

    RankOrderer orderer;
    ContractionBuilder builder;
    BasicCustomizer* basicCustomizer;
    PerfectCustomizer* perfectCustomizer;
    EliminationTreeQuery* queryEngine;
};