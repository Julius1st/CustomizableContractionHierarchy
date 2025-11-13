//
// Created by Julius on 05.11.2025.
//

#include <chrono>
#include <iostream>
#include "DistancePreprocessing.hpp"

DistancePreprocessing::DistancePreprocessing(Graph* graph) : G(graph) {
    eliminationTree = G->getEliminationTree();
    precomputedNodes.resize(G->numVertices());
    precomputedDistancesUp.resize(G->numVertices());
    precomputedDistancesDown.resize(G->numVertices());
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

    return Gnew;
}

void DistancePreprocessing::precomputeDistances() {
    // used to calculate EdgeIDs
    auto headStart = G->beginNeighborhood(0);

    for (uint32_t node = G->numVertices(); node-- > 0;) {
        if (G->parentOf(node) == Graph::INFINITY) continue;

        // For computing Distances to the vertices with the highest IDs in the Graph
        std::vector<uint32_t> precomputeNodesIDs(precomputedNodes[G->parentOf(node)]);
        if (G->parentOf(node) > G->numVertices() - 100) precomputeNodesIDs.push_back(G->parentOf(node)); // TODO: add precomputation amount as program parameter
        precomputedNodes[node] = precomputeNodesIDs;

        precomputedDistancesUp[node].resize(precomputedNodes[node].size(), Graph::INFINITY);
        precomputedDistancesDown[node].resize(precomputedNodes[node].size(), Graph::INFINITY);

        for (auto itv = G->beginNeighborhood(node); itv != G->endNeighborhood(node); itv++) {
            uint32_t neighbor = *itv;
            uint32_t lowerPrecomputationID = 0;
            uint32_t upperPrecomputationID = 0;

            // Compute the distances to the parent node
            if (neighbor == G->parentOf(node)) {
                uint32_t distanceToParentUp = G->getDownwardWeight(std::distance(headStart, itv));
                uint32_t distanceToParentDown = G->getUpwardWeight(std::distance(headStart, itv));

                precomputedDistancesUp[node].back() = distanceToParentUp;
                precomputedDistancesDown[node].back() = distanceToParentDown;
            }

            while (lowerPrecomputationID < precomputedNodes[node].size() && upperPrecomputationID < precomputedNodes[neighbor].size()) {
                if (precomputedNodes[node][lowerPrecomputationID] < precomputedNodes[neighbor][upperPrecomputationID]) lowerPrecomputationID++;
                else if (precomputedNodes[node][lowerPrecomputationID] > precomputedNodes[neighbor][upperPrecomputationID]) upperPrecomputationID++; // This case should never happen
                else {
                    // Match found
                    uint32_t distanceViaNeighborUp = G->getDownwardWeight(std::distance(headStart, itv)) + precomputedDistancesUp[neighbor][upperPrecomputationID];
                    uint32_t distanceViaNeighborDown = G->getUpwardWeight(std::distance(headStart, itv)) + precomputedDistancesDown[neighbor][upperPrecomputationID];

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