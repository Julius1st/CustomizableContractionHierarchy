//
// Created by Julius on 05.11.2025.
//

#include <chrono>
#include <iostream>
#include <stdexcept>
#include "DistancePreprocessing.hpp"

DistancePreprocessing::DistancePreprocessing(Graph* graph) : G(graph) {
    eliminationTree = G->getEliminationTree();
    precomputedNodes.resize(G->numVertices());
    precomputedDistancesUp.resize(G->numVertices());
    precomputedDistancesDown.resize(G->numVertices());

    distanceToRoot.resize(G->numVertices(), Graph::INFINITY_VALUE);
}

Graph* DistancePreprocessing::run() {

    auto begin = std::chrono::steady_clock::now();
    precomputeDistances();
    auto end = std::chrono::steady_clock::now();
    std::cout << "Distance Preprocessing done, time in milliseconds: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << std::endl;
    begin = std::chrono::steady_clock::now();
    createGraphWithPrecomputedDistances();
    end = std::chrono::steady_clock::now();
    std::cout << "Graph Creation with Precomputed Distances done, time in milliseconds: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << std::endl;

    std::cout << "This many Edges have been deleted in Distance Preprocessing: " << G->numEdges() - Gnew->numEdges() << std::endl;

    // Gnew->printGraphInfo();
    return Gnew;
}

void DistancePreprocessing::precomputeDistances() {
    // used to calculate EdgeIDs
    auto headStart = G->beginNeighborhood(0);

    for (uint32_t node = G->numVertices(); node-- > 0;) {

        // TODO: Different options for selecting nodes for distance precomputation via program parameter
        // selectNodesWithHighestID(node, 100);
        selectNodesWithMaxDistanceToRoot(node, 100);

        precomputedDistancesUp[node].resize(precomputedNodes[node].size(), Graph::INFINITY_VALUE);
        precomputedDistancesDown[node].resize(precomputedNodes[node].size(), Graph::INFINITY_VALUE);

        if (precomputedNodes[node].empty()) continue;
        if (G->beginNeighborhood(node) == G->endNeighborhood(node)) continue; // No neighbors

        // Initialize distances to precomputed nodes that are direct neighbors
        uint32_t neighborPrecomputationID = precomputedNodes[node].size() -1; // Index for iterating over precomputed nodes to check if neighbor is also a precomputed node and if so initialize it
        uint32_t neighborID = 0;
        while (neighborID < std::distance(G->beginNeighborhood(node), G->endNeighborhood(node))) {
            if (neighborPrecomputationID >= precomputedNodes[node].size()) break; // Underflow

            uint32_t neighbor = *(G->beginNeighborhood(node) + neighborID);
            if (precomputedNodes[node][neighborPrecomputationID] == neighbor) {
                uint32_t edgeID = std::distance(headStart, G->beginNeighborhood(node) + neighborID);
                uint32_t distanceToNeighborUp = G->getUpwardWeight(edgeID);
                uint32_t distanceToNeighborDown = G->getDownwardWeight(edgeID);

                precomputedDistancesUp[node][neighborPrecomputationID] = distanceToNeighborUp;
                precomputedDistancesDown[node][neighborPrecomputationID] = distanceToNeighborDown;

                neighborPrecomputationID--;
            }
            else if (precomputedNodes[node][neighborPrecomputationID] < neighbor) {
                neighborPrecomputationID--;
            }
            else {
                neighborID++;
            }
        }

        for (auto itv = G->beginNeighborhood(node); itv != G->endNeighborhood(node); itv++) {
            uint32_t neighbor = *itv;
            uint32_t lowerPrecomputationID = 0;
            uint32_t upperPrecomputationID = 0;

            while (lowerPrecomputationID < precomputedNodes[node].size() && upperPrecomputationID < precomputedNodes[neighbor].size()) {

                if (precomputedNodes[node][lowerPrecomputationID] < precomputedNodes[neighbor][upperPrecomputationID]) lowerPrecomputationID++;

                else if (precomputedNodes[node][lowerPrecomputationID] > precomputedNodes[neighbor][upperPrecomputationID]) upperPrecomputationID++; // This case should never happen

                else {
                    // Match found
                    uint32_t distanceViaNeighborUp = G->getUpwardWeight(std::distance(headStart, itv)) + precomputedDistancesUp[neighbor][upperPrecomputationID];
                    uint32_t distanceViaNeighborDown = G->getDownwardWeight(std::distance(headStart, itv)) + precomputedDistancesDown[neighbor][upperPrecomputationID];

                    // Update distances if smaller via neighbor
                    if (distanceViaNeighborUp < precomputedDistancesUp[node][lowerPrecomputationID]) {
                        precomputedDistancesUp[node][lowerPrecomputationID] = distanceViaNeighborUp;
                    }
                    if (distanceViaNeighborDown < precomputedDistancesDown[node][lowerPrecomputationID]) {
                        precomputedDistancesDown[node][lowerPrecomputationID] = distanceViaNeighborDown;
                    }

                    lowerPrecomputationID++;
                    upperPrecomputationID++;
                }
            }
        }
    }

}

void DistancePreprocessing::createGraphWithPrecomputedDistances() {
    std::vector<uint32_t> newFirstOut;
    std::vector<uint32_t> newHead;
    std::vector<uint32_t> newUpwardWeights;
    std::vector<uint32_t> newDownwardWeights;

    newFirstOut.resize(G->numVertices() + 1);
    newFirstOut[0] = 0;

    auto headStart = G->beginNeighborhood(0);


    for (uint32_t node = 0; node < G->numVertices(); node++) {
        uint32_t precomputedNodesID = precomputedNodes[node].size()-1;
        uint32_t addedEdges = 0;
        for (auto it = G->beginNeighborhood(node); it != G->endNeighborhood(node); it++) {
            uint32_t neighbor = *it;

            // If all precomputed nodes have been skipped (underflow) or the neighbor is smaller than the current precomputed node, add the edge
            if (precomputedNodesID > precomputedNodes[node].size()-1 || neighbor < precomputedNodes[node][precomputedNodesID]) {
                newHead.push_back(neighbor);
                newUpwardWeights.push_back(G->getUpwardWeight(std::distance(headStart, it)));
                newDownwardWeights.push_back(G->getDownwardWeight(std::distance(headStart, it)));
                addedEdges++;
            }
            else if (neighbor > precomputedNodes[node][precomputedNodesID]) precomputedNodesID--;

            // If neighbor == precomputedNodes[node][precomputedNodesID], skip adding the edge
        }
        newFirstOut[node +1] = newFirstOut[node] + addedEdges;
    }

    Gnew = new Graph(newFirstOut, newHead, newUpwardWeights, newDownwardWeights, eliminationTree,
                     precomputedNodes, precomputedDistancesUp, precomputedDistancesDown);
}

void DistancePreprocessing::selectNodesWithHighestID(uint32_t currentVertex, uint32_t numberOfNodesToSelect) {
    if (numberOfNodesToSelect > G->numVertices()) {
        throw std::invalid_argument("selectNodesWithHighestID called with numberOfNodesToSelect larger than number of vertices in graph.");
    }
    if (currentVertex == Graph::INFINITY_VALUE) {
        throw std::invalid_argument("selectNodesWithHighestID called with INFINITY as currentVertex.");
    }
    if (G->parentOf(currentVertex) == Graph::INFINITY_VALUE) {
        precomputedNodes[currentVertex] = std::vector<uint32_t>();
        return;
    }
    std::vector<uint32_t> precomputeNodesIDs(precomputedNodes[G->parentOf(currentVertex)]);
    if (G->parentOf(currentVertex) > G->numVertices() - numberOfNodesToSelect) precomputeNodesIDs.push_back(G->parentOf(currentVertex));
    precomputedNodes[currentVertex] = precomputeNodesIDs;
}

void DistancePreprocessing::selectNodesWithMaxDistanceToRoot(uint32_t currentVertex, uint32_t maxDistance) {
    if (currentVertex >= G->numVertices()) {
        throw std::invalid_argument("selectNodesWithMaxDistanceToRoot called with out-of-bounds currentVertex: " + std::to_string(currentVertex));
    }
    if (G->parentOf(currentVertex) == Graph::INFINITY_VALUE) {
        precomputedNodes[currentVertex] = std::vector<uint32_t>();
        distanceToRoot[currentVertex] = 0;
    } else {
        distanceToRoot[currentVertex] = distanceToRoot[G->parentOf(currentVertex)] + 1;
        std::vector<uint32_t> precomputeNodesIDs(precomputedNodes[G->parentOf(currentVertex)]);
        if (distanceToRoot[currentVertex] <= maxDistance+1) precomputeNodesIDs.push_back(G->parentOf(currentVertex));
        precomputedNodes[currentVertex] = precomputeNodesIDs;
    }
}