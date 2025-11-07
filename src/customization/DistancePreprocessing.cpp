//
// Created by Julius on 05.11.2025.
//

#include "DistancePreprocessing.hpp"

Graph* DistancePreprocessing::run() {

    precomputeDistances();
    createGraphWithPrecomputedDistances();

    return Gnew;
}

void DistancePreprocessing::precomputeDistances() {
    // used to calculate EdgeIDs
    auto headStart = G->beginNeighborhood(0);

    for (uint32_t node = G->numVertices(); node-- > 0;) {
        // For computing Distances to the vertices with the highest IDs in the Graph
        std::vector<uint32_t> precomputeNodesIDs(precomputedNodes[G->parentOf(node)]);
        if (G->parentOf(node) < 1000) precomputeNodesIDs.push_back(G->parentOf(node)); // TODO: add precomputation amount as program parameter
        precomputedNodes[node] = precomputeNodesIDs;

        for (auto itv = G->beginNeighborhood(node); itv != G->endNeighborhood(node); itv++) {
            uint32_t neighbor = *itv;
            uint32_t lowerPrecomputationID = 0;
            uint32_t upperPrecomputationID = 0;

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
    // TODO: create new Graph by deleting superfluous edges and setting the precomputed distances
}