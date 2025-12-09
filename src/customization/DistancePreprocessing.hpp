//
// Created by Julius on 05.11.2025.
//

#pragma once
#include "core/Graph.hpp"
#include <memory>

class DistancePreprocessing {
public:
    explicit DistancePreprocessing(Graph* graph);

    std::unique_ptr<Graph> run(uint32_t preprocessingParameter);

private:
    Graph* G;
    std::unique_ptr<Graph> Gnew;
    std::vector<uint32_t> eliminationTree;

    std::vector<std::vector<uint32_t>> precomputedNodes; // nodes to which distances have been precomputed
    std::vector<std::vector<uint32_t>> precomputedDistancesUp; // Corresponding distances from all other nodes to each node in precomputedNodes (precomputedDistancesUp[node][i] = distance from node to precomputedNodes[node][i])
    std::vector<std::vector<uint32_t>> precomputedDistancesDown; // Corresponding distances from all other nodes to each node in precomputedNodes

    void precomputeDistances(uint32_t preprocessingParameter);
    void createGraphWithPrecomputedDistances();

    // Different options for Vertex selection for distance precomputation
    void selectNodesWithHighestID(uint32_t currentVertex, uint32_t numberOfNodesToSelect);
    void selectNodesWithMaxDistanceToRoot(uint32_t currentVertex, uint32_t maxDistance);
};