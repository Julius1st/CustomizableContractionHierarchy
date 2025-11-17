//
// Created by Julius on 26.09.2025.
//

#pragma once
#include "core/Graph.hpp"
#include "preprocessing/ContractionBuilder.hpp"
#include "customization/BasicCustomizer.hpp"
#include "query/EliminationTreeQuery.hpp"
#include "DistancePreprocessing.hpp"
#include <vector>

// Facade Class
class CCH {
public:
    explicit CCH(Graph* baseGraph, std::vector<uint32_t>& order);

    void preprocess();
    void customize();
    uint32_t query(uint32_t s, uint32_t t);
    uint32_t queryWithDistancePreprocessing(uint32_t s, uint32_t t);
    int getInitTime() const { return distancePreprocessedQueryEngine->getInitTime(); }
    uint32_t getInitializedFields() const { return distancePreprocessedQueryEngine->getInitializedFields(); }

    //For testing:
    void printEliminationTreeInformation();

private:
    Graph* G;
    Graph* Gplus;

    std::vector<uint32_t> rankOrder;

    ContractionBuilder* builder;
    BasicCustomizer* basicCustomizer;
    EliminationTreeQuery* queryEngine;
    EliminationTreeQuery* distancePreprocessedQueryEngine;

    // For query speed-up:
    DistancePreprocessing* distancePreprocessing;
    Graph* GwithPrecomputedDistances;
};