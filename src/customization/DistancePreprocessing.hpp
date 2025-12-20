//
// Created by Julius on 05.11.2025.
//

#pragma once
#include "core/Graph.hpp"
#include <memory>
#include <chrono>

class DistancePreprocessing {
public:
    explicit DistancePreprocessing(Graph* graph);

    std::unique_ptr<Graph> run(uint32_t preprocessingParameter);

    uint32_t getNumDeletedEdges() const { return numDeletedEdges; }
    long getProcessingTime() const { return processingTime; }
    long getGraphCreationTime() const { return graphCreationTime; }

private:
    Graph* G;
    std::unique_ptr<Graph> Gnew;
    std::vector<uint32_t> eliminationTree;

    std::vector<std::vector<uint32_t>> precomputedNodes; // nodes to which distances have been precomputed
    std::vector<std::vector<uint32_t>> precomputedDistancesUp; // Corresponding distances from all other nodes to each node in precomputedNodes (precomputedDistancesUp[node][i] = distance from node to precomputedNodes[node][i])
    std::vector<std::vector<uint32_t>> precomputedDistancesDown; // Corresponding distances from all other nodes to each node in precomputedNodes

    // The next node on the path to each precomputed node. I.e., successor[node][i] is the next node on the shortest path from node to precomputedNodes[node][i].
    // If successor[node][i] == precomputedNodes[node][i], then node is the predecessor of precomputedNodes[node][i].
    // Used for reconstructing paths. successorUp is for the upward direction, successorDown for the downward direction but in both, the successor is higher in the tree.
    std::vector<std::vector<uint32_t>> successorUp;
    std::vector<std::vector<uint32_t>> successorDown;

    long processingTime = 0;
    long graphCreationTime = 0;
    uint32_t numDeletedEdges = 0;

    void precomputeDistances(uint32_t preprocessingParameter);
    void createGraphWithPrecomputedDistances();

    // Different options for Vertex selection for distance precomputation
    void selectNodesWithHighestID(uint32_t numberOfNodesToSelect);
    void selectNodesWithMaxDistanceToRoot(uint32_t maxDistance);
    void selectNodesWithHighestWeightedInDegree(uint32_t numberOfNodesToSelect);
    std::vector<uint32_t> size;
};