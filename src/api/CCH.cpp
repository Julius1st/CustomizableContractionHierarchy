//
// Created by Julius on 26.09.2025.
//
#include "CCH.hpp"
#include <iostream>

CCH::CCH(Graph* baseGraph, std::vector<uint32_t> &order) : G(baseGraph), rankOrder(order) {
    builder = new ContractionBuilder(G, rankOrder);
}

void CCH::preprocess() {
    Gplus = builder->buildGplus();
    basicCustomizer = new BasicCustomizer(Gplus);
}

void CCH::customize() {
    basicCustomizer->run();
    // Remove to use the distance-preprocessed graph for queries:
    queryEngine = new EliminationTreeQuery(Gplus);

    // TODO: create option to switch between normal and distance-preprocessed queries with program parameter

    // For query speed-up:
    distancePreprocessing = new DistancePreprocessing(Gplus);
    GwithPrecomputedDistances = distancePreprocessing->run();
    distancePreprocessedQueryEngine = new EliminationTreeQuery(GwithPrecomputedDistances);

    //printEliminationTreeInformation();
}

void CCH::printEliminationTreeInformation() {
    std::vector<uint32_t> distanceToRoot(GwithPrecomputedDistances->numVertices(), 0);
    std::vector<bool> hasChild(GwithPrecomputedDistances->numVertices(), false);
    for(uint32_t i = GwithPrecomputedDistances->numVertices(); i-- >0; ) {
        if (GwithPrecomputedDistances->parentOf(i) != Graph::INFINITY) {
            distanceToRoot[i] = distanceToRoot[GwithPrecomputedDistances->parentOf(i)] + 1;
            hasChild[GwithPrecomputedDistances->parentOf(i)] = true;
        }
    }
    std::cout << "Max Distance to root in elimination tree: " << *std::max_element(distanceToRoot.begin(), distanceToRoot.end()) << std::endl;
    uint32_t distanceToRootSum = 0;
    uint32_t numLeaves = 0;
    for (uint32_t i = 0; i < GwithPrecomputedDistances->numVertices(); i++) {
        if (!hasChild[i]) {
            distanceToRootSum += distanceToRoot[i];
            numLeaves++;
        }
    }
    std::cout << "Average Distance to root over all leaves in elimination tree: " << (double)distanceToRootSum / numLeaves << std::endl;
    std::cout << "Number of leaves in elimination tree: " << numLeaves << std::endl;
}

uint32_t CCH::query(uint32_t s, uint32_t t) {
    return queryEngine->query(builder->getRank(s), builder->getRank(t));
}

uint32_t CCH::queryWithDistancePreprocessing(uint32_t s, uint32_t t) {
    return distancePreprocessedQueryEngine->query(builder->getRank(s), builder->getRank(t));
}