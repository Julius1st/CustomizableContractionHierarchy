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
#include <memory>

// Facade Class
class CCH {
public:
    explicit CCH(Graph* baseGraph, std::vector<uint32_t>& order);

    void preprocess();
    void customize(uint32_t preprocessingParameter);
    uint32_t query(uint32_t s, uint32_t t);
    uint32_t queryWithDistancePreprocessing(uint32_t s, uint32_t t);
    int getInitTimeNormalEngine() const { return queryEngine->getInitTime(); }
    int getInitTimePreprocessedEngine() const { return distancePreprocessedQueryEngine->getInitTime(); }
    uint32_t getInitializedFields() const { return distancePreprocessedQueryEngine->getInitializedFields(); }

    //For testing:
    void printEliminationTreeInformationOfGplus();
    void printEliminationTreeInformationOfNewGraph();
    Graph* getGplus() const { return Gplus.get(); }

private:
    Graph* G;
    std::unique_ptr<Graph> Gplus;

    std::vector<uint32_t> rankOrder;

    std::unique_ptr<ContractionBuilder> builder;
    std::unique_ptr<BasicCustomizer> basicCustomizer;
    std::unique_ptr<EliminationTreeQuery> queryEngine;
    std::unique_ptr<EliminationTreeQuery> distancePreprocessedQueryEngine;

    // For query speed-up:
    std::unique_ptr<DistancePreprocessing> distancePreprocessing;
    std::unique_ptr<Graph> GwithPrecomputedDistances;
};