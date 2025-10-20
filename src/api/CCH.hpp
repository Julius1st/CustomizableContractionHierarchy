//
// Created by Julius on 26.09.2025.
//

#pragma once
#include "core/Graph.hpp"
#include "preprocessing/RankOrderer.hpp"
#include "preprocessing/ContractionBuilder.hpp"
#include "customization/BasicCustomizer.hpp"
#include "query/EliminationTreeQuery.hpp"
#include <vector>

// Facade Class
class CCH {
public:
    explicit CCH(Graph* baseGraph, std::vector<uint32_t>& order);

    void preprocess();
    void customize(const std::vector<uint32_t>& upward_weights, const std::vector<uint32_t>& downward_weights);
    uint32_t query(uint32_t s, uint32_t t);

private:
    Graph* G;
    Graph* Gplus;

    std::vector<uint32_t> rankOrder;

    ContractionBuilder* builder;
    BasicCustomizer* basicCustomizer;
    EliminationTreeQuery* queryEngine;
};