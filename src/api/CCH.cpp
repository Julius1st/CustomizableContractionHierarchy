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

    //printEliminationTreeInformationOfGplus();
    //printEliminationTreeInformationOfNewGraph();
}

uint32_t CCH::query(uint32_t s, uint32_t t) {
    return queryEngine->query(builder->getRank(s), builder->getRank(t));
}

uint32_t CCH::queryWithDistancePreprocessing(uint32_t s, uint32_t t) {
    return distancePreprocessedQueryEngine->query(builder->getRank(s), builder->getRank(t));
}

void CCH::printEliminationTreeInformationOfGplus() {
    std::vector<uint32_t> numChildren(Gplus->numVertices(), 0);
    for (uint32_t i = 0; i < Gplus->numVertices(); i++) {
        uint32_t parent = Gplus->parentOf(i);
        if (parent != Graph::INFINITY) {
            numChildren[parent]++;
        }
    }
    std::vector<uint32_t> distanceToRoot(Gplus->numVertices(), 0);
    for(uint32_t i = Gplus->numVertices(); i-- >0; ) {
        if (Gplus->parentOf(i) != Graph::INFINITY) {
            distanceToRoot[i] = distanceToRoot[Gplus->parentOf(i)] + 1;
        }
    }

    for (uint32_t i = G->numVertices()-1; i-- >0; ) {
        if (numChildren[i] >= 2 && i < 18010153) {
            std::cout << "Node " << i << " has " << numChildren[i] << " in the elimination tree. This node is " << G->numVertices()-1-i << " steps from the root" << std::endl;
            break;
        }
    }

    // calculate graph component sizes
    std::vector<uint32_t> componentSizes(Gplus->numVertices(), 1);
    for (uint32_t i = 0; i < Gplus->numVertices(); i++) {
        uint32_t parent = Gplus->parentOf(i);
        if (parent != Graph::INFINITY) {
            componentSizes[parent] += componentSizes[i];
        }
        if (parent == Graph::INFINITY) continue;
        if (distanceToRoot[i] < 150 && distanceToRoot[i] > 70 && numChildren[parent] >= 2 && componentSizes[i] > 1000000) {
            std::cout << "Dist: " << distanceToRoot[i] << " - Component size of node " << i << " with parent " << parent << " is " << componentSizes[i] << std::endl;
        }
    }
}

void CCH::printEliminationTreeInformationOfNewGraph() {
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