//
// Created by Julius on 05.11.2025.
//

#include "DistancePreprocessing.hpp"

DistancePreprocessing::DistancePreprocessing(Graph* graph) : G(graph) {
    eliminationTree = G->getEliminationTree();
    precomputedNodes.resize(G->numVertices());
    precomputedDistancesUp.resize(G->numVertices());
    precomputedDistancesDown.resize(G->numVertices());
    successorUp.resize(G->numVertices());
    successorDown.resize(G->numVertices());
}

std::unique_ptr<Graph> DistancePreprocessing::run(uint32_t preprocessingParameter) {

    auto begin = std::chrono::steady_clock::now();
    precomputeDistances(preprocessingParameter);
    auto end = std::chrono::steady_clock::now();
    processingTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
    std::cout << "Distance Preprocessing done, time in milliseconds: " << processingTime << std::endl;

    begin = std::chrono::steady_clock::now();
    createGraphWithPrecomputedDistances();
    end = std::chrono::steady_clock::now();
    graphCreationTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
    std::cout << "Graph Creation with Precomputed Distances done, time in milliseconds: " << graphCreationTime << std::endl;

    numDeletedEdges = G->numEdges() - Gnew->numEdges();
    std::cout << "This many Edges have been deleted in Distance Preprocessing: " << numDeletedEdges << std::endl;

    // Gnew->printGraphInfo();
    return std::move(Gnew);
}

void DistancePreprocessing::precomputeDistances(uint32_t preprocessingParameter) {
    // used to calculate EdgeIDs
    const auto headStart = G->beginNeighborhood(0);

    // TODO: Different options for selecting nodes for distance precomputation via program parameter
    // selectNodesWithHighestID(100);
    // selectNodesWithMaxDistanceToRoot(preprocessingParameter);
    selectNodesWithHighestWeightedInDegree(preprocessingParameter);

    for (uint32_t node = G->numVertices(); node-- > 0;) {

        precomputedDistancesUp[node].resize(precomputedNodes[node].size(), Graph::INFINITY_VALUE);
        precomputedDistancesDown[node].resize(precomputedNodes[node].size(), Graph::INFINITY_VALUE);
        successorUp[node].resize(precomputedNodes[node].size(), Graph::INFINITY_VALUE);
        successorDown[node].resize(precomputedNodes[node].size(), Graph::INFINITY_VALUE);

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
                successorUp[node][neighborPrecomputationID] = neighbor;
                successorDown[node][neighborPrecomputationID] = neighbor;

                neighborPrecomputationID--;
            }
            else if (precomputedNodes[node][neighborPrecomputationID] < neighbor) {
                neighborPrecomputationID--;
            }
            else {
                neighborID++;
            }
        }

        // Update distances via neighbors that also have precomputed distances
        for (auto itv = G->beginNeighborhood(node); itv != G->endNeighborhood(node); itv++) {
            uint32_t neighbor = *itv;
            uint32_t nodePrecomputationID = 0;
            uint32_t neighborPrecomputationID = 0;

            // TODO: make this a for loop?
            while (nodePrecomputationID < precomputedNodes[node].size() && neighborPrecomputationID < precomputedNodes[neighbor].size()) {
                if (node == 16262286 && neighbor == 16262287 && neighborPrecomputationID == 9) {
                    std::cout << "Processing neighbor 16262337 of node 16262286" << std::endl;
                    uint32_t x = std::distance(headStart, itv);
                    uint32_t y = itv - headStart;
                    std::cout << x << ", " << y << ", " << std::endl;
                    std:: cout << std::addressof(*headStart) << std::addressof(*itv) << std::endl;
                }

                if (precomputedNodes[node][nodePrecomputationID] < precomputedNodes[neighbor][neighborPrecomputationID]) neighborPrecomputationID++;

                else if (precomputedNodes[node][nodePrecomputationID] > precomputedNodes[neighbor][neighborPrecomputationID]) nodePrecomputationID++;

                else {
                    // Match found
                    uint32_t distanceViaNeighborUp = G->getUpwardWeight(std::distance(headStart, itv)) + precomputedDistancesUp[neighbor][neighborPrecomputationID];
                    uint32_t distanceViaNeighborDown = G->getDownwardWeight(std::distance(headStart, itv)) + precomputedDistancesDown[neighbor][neighborPrecomputationID];

                    // Update distances if smaller via neighbor
                    if (distanceViaNeighborUp < precomputedDistancesUp[node][nodePrecomputationID]) {
                        precomputedDistancesUp[node][nodePrecomputationID] = distanceViaNeighborUp;
                        successorUp[node][nodePrecomputationID] = neighbor;
                    }
                    if (distanceViaNeighborDown < precomputedDistancesDown[node][nodePrecomputationID]) {
                        precomputedDistancesDown[node][nodePrecomputationID] = distanceViaNeighborDown;
                        successorDown[node][nodePrecomputationID] = neighbor;
                    }

                    nodePrecomputationID++;
                    neighborPrecomputationID++;
                }
            }
        }
        if (node == 8230481 ) {
            std::cout << "Node 8230481 neighbors: ";
            for (auto it = G->beginNeighborhood(node); it != G->endNeighborhood(node); it++) {
                std::cout << *it << ", ";
            }
            std::cout << std::endl;
            std::cout << "Node 16262286 neighbors: ";
            for (auto it = G->beginNeighborhood(16262286); it != G->endNeighborhood(16262286); it++) {
                std::cout << *it << ", ";
            }
            std::cout << std::endl;
            std::cout << "Node 16262287 neighbors: ";
            for (auto it = G->beginNeighborhood(16262287); it != G->endNeighborhood(16262287); it++) {
                std::cout << *it << ", ";
            }
            std::cout << std::endl;
            std::cout << "Node 16262288 neighbors: ";
            for (auto it = G->beginNeighborhood(16262288); it != G->endNeighborhood(16262288); it++) {
                std::cout << *it << ", ";
            }
            std::cout << std::endl;
            std::cout << "Node 16262336 neighbors: ";
            for (auto it = G->beginNeighborhood(16262336); it != G->endNeighborhood(16262336); it++) {
                std::cout << *it << ", ";
            }
            std::cout << std::endl;
            std::cout << "Node 16262337 neighbors: ";
            for (auto it = G->beginNeighborhood(16262337); it != G->endNeighborhood(16262337); it++) {
                std::cout << *it << ", ";
            }
            std::cout << std::endl;
            std::cout << std::endl;

            std::cout << "Node 8230481 precomputed Nodes: ";
            for (uint32_t pn : precomputedNodes[node]) {
                std::cout << pn << ", ";
            }
            std::cout << std::endl;
            std::cout << "Node 16262286 precomputed Nodes: ";
            for (uint32_t pn : precomputedNodes[16262286]) {
                std::cout << pn << ", ";
            }
            std::cout << std::endl;
            std::cout << "Node 16262287 precomputed Nodes: ";
            for (uint32_t pn : precomputedNodes[16262287]) {
                std::cout << pn << ", ";
            }
            std::cout << std::endl;
            std::cout << "Node 16262288 precomputed Nodes: ";
            for (uint32_t pn : precomputedNodes[16262288]) {
                std::cout << pn << ", ";
            }
            std::cout << std::endl;
            std::cout << "Node 16262336 precomputed Nodes: ";
            for (uint32_t pn : precomputedNodes[16262336]) {
                std::cout << pn << ", ";
            }
            std::cout << std::endl;
            std::cout << "Node 16262337 precomputed Nodes: ";
            for (uint32_t pn : precomputedNodes[16262337]) {
                std::cout << pn << ", ";
            }
            std::cout << std::endl;
            std::cout << std::endl;

            std::cout << "Node 8230481 precomputed distances up: ";
            for (uint32_t dist : precomputedDistancesUp[node]) {
                std::cout << dist << ", ";
            }
            std::cout << std::endl;
            std::cout << "Node 16262286 precomputed distances up: ";
            for (uint32_t dist : precomputedDistancesUp[16262286]) {
                std::cout << dist << ", ";
            }
            std::cout << std::endl;
            std::cout << "Node 16262287 precomputed distances up: ";
            for (uint32_t dist : precomputedDistancesUp[16262287]) {
                std::cout << dist << ", ";
            }
            std::cout << std::endl;
            std::cout << "Node 16262288 precomputed distances up: ";
            for (uint32_t dist : precomputedDistancesUp[16262288]) {
                std::cout << dist << ", ";
            }
            std::cout << std::endl;
            std::cout << "Node 16262336 precomputed distances up: ";
            for (uint32_t dist : precomputedDistancesUp[16262336]) {
                std::cout << dist << ", ";
            }
            std::cout << std::endl;
            std::cout << "Node 16262337 precomputed distances up: ";
            for (uint32_t dist : precomputedDistancesUp[16262337]) {
                std::cout << dist << ", ";
            }
            std::cout << std::endl;
            std::cout << std::endl;

            std::cout << "Node 8230481 successorUp: ";
            for (uint32_t succ : successorUp[node]) {
                std::cout << succ << ", ";
            }
            std::cout << std::endl;
            std::cout << "Node 16262286 successorUp: ";
            for (uint32_t succ : successorUp[16262286]) {
                std::cout << succ << ", ";
            }
            std::cout << std::endl;
            std::cout << "Node 16262287 successorUp: ";
            for (uint32_t succ : successorUp[16262287]) {
                std::cout << succ << ", ";
            }
            std::cout << std::endl;
            std::cout << "Node 16262288 successorUp: ";
            for (uint32_t succ : successorUp[16262288]) {
                std::cout << succ << ", ";
            }
            std::cout << std::endl;
            std::cout << "Node 16262336 successorUp: ";
            for (uint32_t succ : successorUp[16262336]) {
                std::cout << succ << ", ";
            }
            std::cout << std::endl;
            std::cout << "Node 16262337 successorUp: ";
            for (uint32_t succ : successorUp[16262337]) {
                std::cout << succ << ", ";
            }
            std::cout << std::endl;
            std::cout << std::endl;
        }
    }
}

void DistancePreprocessing::createGraphWithPrecomputedDistances() {
    uint32_t numVertices = G->numVertices();
    uint32_t numEdges = G->numEdges();

    std::vector<uint32_t> newFirstOut(numVertices + 1);
    std::vector<uint32_t> newHead(numEdges);
    std::vector<uint32_t> newUpwardWeights(numEdges);
    std::vector<uint32_t> newDownwardWeights(numEdges);

    newFirstOut[0] = 0;

    auto headStart = G->beginNeighborhood(0);

    // TODO: Improve this method
    uint32_t overallAddedEdges = 0;
    for (uint32_t node = 0; node < numVertices; node++) {
        // if no nodes are precomputed, add all edges to the graph
        if (precomputedNodes[node].empty()) {
            uint32_t addedEdges = 0;
            for (auto it = G->beginNeighborhood(node); it != G->endNeighborhood(node); it++) {
                uint32_t neighbor = *it;
                newHead[overallAddedEdges] = neighbor;
                newUpwardWeights[overallAddedEdges] = G->getUpwardWeight(std::distance(headStart, it));
                newDownwardWeights[overallAddedEdges] = G->getDownwardWeight(std::distance(headStart, it));
                addedEdges++;
                overallAddedEdges++;
            }
            newFirstOut[node +1] = newFirstOut[node] + addedEdges;
            continue;
        }

        int64_t precomputedNodesID = precomputedNodes[node].size()-1;
        uint32_t addedEdges = 0;
        for (auto it = G->beginNeighborhood(node); it != G->endNeighborhood(node); it++) {
            uint32_t neighbor = *it;

            // If all precomputed nodes have been skipped or the neighbor is smaller than the current precomputed node, add the edge
            if (precomputedNodesID < 0 || neighbor < precomputedNodes[node][precomputedNodesID]) {
                newHead[overallAddedEdges] = neighbor;
                newUpwardWeights[overallAddedEdges] = G->getUpwardWeight(std::distance(headStart, it));
                newDownwardWeights[overallAddedEdges] = G->getDownwardWeight(std::distance(headStart, it));
                addedEdges++;
                overallAddedEdges++;
            }
            else if (neighbor > precomputedNodes[node][precomputedNodesID]) precomputedNodesID--;

            // If neighbor == precomputedNodes[node][precomputedNodesID], skip adding the edge
        }
        newFirstOut[node +1] = newFirstOut[node] + addedEdges;
    }
    newHead.resize(overallAddedEdges);
    newUpwardWeights.resize(overallAddedEdges);
    newDownwardWeights.resize(overallAddedEdges);

    Gnew = std::make_unique<Graph>(newFirstOut, newHead, newUpwardWeights, newDownwardWeights, eliminationTree,
                     precomputedNodes, precomputedDistancesUp, precomputedDistancesDown, successorUp, successorDown);
}

void DistancePreprocessing::selectNodesWithHighestID(uint32_t numberOfNodesToSelect) {
    for (uint32_t node = G->numVertices(); node-- > 0;) {
        if (G->parentOf(node) == Graph::INFINITY_VALUE) {
            precomputedNodes[node] = std::vector<uint32_t>();
            continue;
        }
        precomputedNodes[node] = precomputedNodes[G->parentOf(node)];
        if (G->parentOf(node) > G->numVertices() - numberOfNodesToSelect) precomputedNodes[node].push_back(G->parentOf(node));
    }
}

void DistancePreprocessing::selectNodesWithMaxDistanceToRoot(uint32_t maxDistance) {
    for (uint32_t node = G->numVertices(); node-- > 0;) {
        if (G->parentOf(node) == Graph::INFINITY_VALUE) {
            precomputedNodes[node] = std::vector<uint32_t>();
            continue;
        }
        precomputedNodes[node] = precomputedNodes[G->parentOf(node)];
        if (precomputedNodes[node].size() < maxDistance) precomputedNodes[node].push_back(G->parentOf(node));
    }
}

// This function inserts item into vec such that vec remains sorted according to pred
template< typename T, typename Pred >
typename std::vector<T>::iterator insert_sorted( std::vector<T> & vec, T const& item, Pred pred ) {
    return vec.insert(
           std::lower_bound( vec.begin(), vec.end(), item, pred ),
           item
        );
}
auto pred = [](const std::tuple<uint32_t, uint32_t>& a, const std::tuple<uint32_t, uint32_t>& b) {
    return std::get<1>(a) > std::get<1>(b);
};

void DistancePreprocessing::selectNodesWithHighestWeightedInDegree(uint32_t numberOfNodesToSelect) {
    if (numberOfNodesToSelect == 0) {
        precomputedNodes = std::vector<std::vector<uint32_t>> (G->numVertices());
        return;
    }
    size = std::vector<uint32_t> (G->numVertices(), 1);
    std::vector<std::vector<std::tuple<uint32_t, uint32_t>>> precomputedNodesTemp(G->numVertices());

    // computation of size vector, which is a weighted sum of the in-degrees of all descendants
    for (uint32_t node = 0; node < G->numVertices(); node++) {
        for (auto it = G->beginNeighborhood(node); it != G->endNeighborhood(node); it++) {
            uint32_t neighbor = *it;
            size[neighbor] += size[node];
        }
    }

    // choose vertices with the highest size value
    for (uint32_t node = G->numVertices(); node-- > 0;) {
        uint32_t parent = G->parentOf(node);
        if (parent == Graph::INFINITY_VALUE) {
            precomputedNodesTemp[node] = std::vector<std::tuple<uint32_t, uint32_t>>();
            continue;
        }
        precomputedNodesTemp[node] = precomputedNodesTemp[parent];

        // Fill precomputedNodesTemp as it is not yet at capacity
        if (precomputedNodesTemp[parent].size() < numberOfNodesToSelect) {
            insert_sorted(precomputedNodesTemp[node], std::make_tuple(parent, size[parent]), pred);
            continue;
        }
        // Replace smallest element if current node has larger size
        if (size[parent] > std::get<1>(precomputedNodesTemp[node].back())) {
            insert_sorted(precomputedNodesTemp[node], std::make_tuple(parent, size[parent]), pred);
            precomputedNodesTemp[node].pop_back();
        }
    }

    // Fill actual precomputedNodes vector, in descending order of node IDs
    for (uint32_t node = 0; node < G->numVertices(); node++) {
        for (auto it = precomputedNodesTemp[node].begin(); it != precomputedNodesTemp[node].end(); it++) {
            precomputedNodes[node].push_back(std::get<0>(*it));
        }
        std::sort(precomputedNodes[node].begin(), precomputedNodes[node].end(), std::greater<>());
    }
}