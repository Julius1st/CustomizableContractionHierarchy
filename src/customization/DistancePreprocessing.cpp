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
        /*
        if (node == 15215059 ) {
            std::cout << "Node 15215059 neighbors: ";
            for (auto it = G->beginNeighborhood(node); it != G->endNeighborhood(node); it++) {
                std::cout << *it << ", ";
            }
            std::cout << std::endl;
            std::cout << "Node 15215062 neighbors: ";
            for (auto it = G->beginNeighborhood(15215062); it != G->endNeighborhood(15215062); it++) {
                std::cout << *it << ", ";
            }
            std::cout << std::endl;
            std::cout << "Node 15215068 neighbors: ";
            for (auto it = G->beginNeighborhood(15215068); it != G->endNeighborhood(15215068); it++) {
                std::cout << *it << ", ";
            }
            std::cout << std::endl;
            std::cout << "Node 15215111 neighbors: ";
            for (auto it = G->beginNeighborhood(15215111); it != G->endNeighborhood(15215111); it++) {
                std::cout << *it << ", ";
            }
            std::cout << std::endl;
            std::cout << "Node 15215117 neighbors: ";
            for (auto it = G->beginNeighborhood(15215117); it != G->endNeighborhood(15215117); it++) {
                std::cout << *it << ", ";
            }
            std::cout << std::endl;
            std::cout << "Node 15215118 neighbors: ";
            for (auto it = G->beginNeighborhood(15215118); it != G->endNeighborhood(15215118); it++) {
                std::cout << *it << ", ";
            }
            std::cout << std::endl;
            std::cout << "Node 15215119 neighbors: ";
            for (auto it = G->beginNeighborhood(15215119); it != G->endNeighborhood(15215119); it++) {
                std::cout << *it << ", ";
            }
            std::cout << std::endl;
            std::cout << "Node 15222034 neighbors: ";
            for (auto it = G->beginNeighborhood(15222034); it != G->endNeighborhood(15222034); it++) {
                std::cout << *it << ", ";
            }
            std::cout << std::endl;
            std::cout << "Node 15222031 neighbors: ";
            for (auto it = G->beginNeighborhood(15222031); it != G->endNeighborhood(15222031); it++) {
                std::cout << *it << ", ";
            }
            std::cout << std::endl;
            std::cout << std::endl;

            std::cout << "Node 15215059 precomputed Nodes: ";
            for (uint32_t pn : precomputedNodes[node]) {
                std::cout << pn << ", ";
            }
            std::cout << std::endl;
            std::cout << "Node 15215062 precomputed Nodes: ";
            for (uint32_t pn : precomputedNodes[15215062]) {
                std::cout << pn << ", ";
            }
            std::cout << std::endl;
            std::cout << "Node 15215068 precomputed Nodes: ";
            for (uint32_t pn : precomputedNodes[15215068]) {
                std::cout << pn << ", ";
            }
            std::cout << std::endl;
            std::cout << "Node 15215111 precomputed Nodes: ";
            for (uint32_t pn : precomputedNodes[15215111]) {
                std::cout << pn << ", ";
            }
            std::cout << std::endl;
            std::cout << "Node 15215117 precomputed Nodes: ";
            for (uint32_t pn : precomputedNodes[15215117]) {
                std::cout << pn << ", ";
            }
            std::cout << std::endl;
            std::cout << "Node 15215118 precomputed Nodes: ";
            for (uint32_t pn : precomputedNodes[15215118]) {
                std::cout << pn << ", ";
            }
            std::cout << std::endl;
            std::cout << "Node 15215119 precomputed Nodes: ";
            for (uint32_t pn : precomputedNodes[15215119]) {
                std::cout << pn << ", ";
            }
            std::cout << std::endl;
            std::cout << "Node 15222034 precomputed Nodes: ";
            for (uint32_t pn : precomputedNodes[15222034]) {
                std::cout << pn << ", ";
            }
            std::cout << std::endl;
            std::cout << "Node 15222031 precomputed Nodes: ";
            for (uint32_t pn : precomputedNodes[15222031]) {
                std::cout << pn << ", ";
            }
            std::cout << std::endl;
            std::cout << std::endl;

            std::cout << "Node 15215059 precomputed distances up: ";
            for (uint32_t dist : precomputedDistancesUp[node]) {
                std::cout << dist << ", ";
            }
            std::cout << std::endl;
            std::cout << "Node 15215062 precomputed distances up: ";
            for (uint32_t dist : precomputedDistancesUp[15215062]) {
                std::cout << dist << ", ";
            }
            std::cout << std::endl;
            std::cout << "Node 15215068 precomputed distances up: ";
            for (uint32_t dist : precomputedDistancesUp[15215068]) {
                std::cout << dist << ", ";
            }
            std::cout << std::endl;
            std::cout << "Node 15215111 precomputed distances up: ";
            for (uint32_t dist : precomputedDistancesUp[15215111]) {
                std::cout << dist << ", ";
            }
            std::cout << std::endl;
            std::cout << "Node 15215117 precomputed distances up: ";
            for (uint32_t dist : precomputedDistancesUp[15215117]) {
                std::cout << dist << ", ";
            }
            std::cout << std::endl;
            std::cout << "Node 15215118 precomputed distances up: ";
            for (uint32_t dist : precomputedDistancesUp[15215118]) {
                std::cout << dist << ", ";
            }
            std::cout << std::endl;
            std::cout << "Node 15215119 precomputed distances up: ";
            for (uint32_t dist : precomputedDistancesUp[15215119]) {
                std::cout << dist << ", ";
            }
            std::cout << std::endl;
            std::cout << "Node 15222034 precomputed distances up: ";
            for (uint32_t dist : precomputedDistancesUp[15222034]) {
                std::cout << dist << ", ";
            }
            std::cout << std::endl;
            std::cout << "Node 15222031 precomputed distances up: ";
            for (uint32_t dist : precomputedDistancesUp[15222031]) {
                std::cout << dist << ", ";
            }
            std::cout << std::endl;
            std::cout << std::endl;

            std::cout << "Node 15215059 successorUp: ";
            for (uint32_t succ : successorUp[node]) {
                std::cout << succ << ", ";
            }
            std::cout << std::endl;
            std::cout << "Node 15215062 successorUp: ";
            for (uint32_t succ : successorUp[15215062]) {
                std::cout << succ << ", ";
            }
            std::cout << std::endl;
            std::cout << "Node 15215068 successorUp: ";
            for (uint32_t succ : successorUp[15215068]) {
                std::cout << succ << ", ";
            }
            std::cout << std::endl;
            std::cout << "Node 15215111 successorUp: ";
            for (uint32_t succ : successorUp[15215111]) {
                std::cout << succ << ", ";
            }
            std::cout << std::endl;
            std::cout << "Node 15215117 successorUp: ";
            for (uint32_t succ : successorUp[15215117]) {
                std::cout << succ << ", ";
            }
            std::cout << std::endl;
            std::cout << "Node 15215118 successorUp: ";
            for (uint32_t succ : successorUp[15215118]) {
                std::cout << succ << ", ";
            }
            std::cout << std::endl;
            std::cout << "Node 15215119 successorUp: ";
            for (uint32_t succ : successorUp[15215119]) {
                std::cout << succ << ", ";
            }
            std::cout << std::endl;
            std::cout << "Node 15222034 successorUp: ";
            for (uint32_t succ : successorUp[15222034]) {
                std::cout << succ << ", ";
            }
            std::cout << std::endl;
            std::cout << "Node 15222031 successorUp: ";
            for (uint32_t succ : successorUp[15222031]) {
                std::cout << succ << ", ";
            }
            std::cout << std::endl;
            std::cout << std::endl;

            std::cout << "Node 15215111 EdgeWeights: ";
            for (uint32_t i = G->firstOut[15215111]; i < G->firstOut[15215111 +1]; i++) {
                std::cout << G->getUpwardWeight(i) << ", ";
            }
            std::cout << std::endl;
            std::cout << "Node 15215117 EdgeWeights: ";
            for (uint32_t i = G->firstOut[15215117]; i < G->firstOut[15215117 +1]; i++) {
                std::cout << G->getUpwardWeight(i) << ", ";
            }
            std::cout << std::endl;
            std::cout << "Node 15215118 EdgeWeights: ";
            for (uint32_t i = G->firstOut[15215118]; i < G->firstOut[15215118 +1]; i++) {
                std::cout << G->getUpwardWeight(i) << ", ";
            }
            std::cout << std::endl;
            std::cout << "Node 15215119 EdgeWeights: ";
            for (uint32_t i = G->firstOut[15215119]; i < G->firstOut[15215119 +1]; i++) {
                std::cout << G->getUpwardWeight(i) << ", ";
            }
            std::cout << std::endl;
            std::cout << "Node 15222034 EdgeWeights: ";
            for (uint32_t i = G->firstOut[15222034]; i < G->firstOut[15222034 +1]; i++) {
                std::cout << G->getUpwardWeight(i) << ", ";
            }
            std::cout << std::endl;
            std::cout << "Node 15222031 EdgeWeights: ";
            for (uint32_t i = G->firstOut[15222031]; i < G->firstOut[15222031 +1]; i++) {
                std::cout << G->getUpwardWeight(i) << ", ";
            }
            std::cout << std::endl;
        }
        */
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
        // TODO replace added edges with overall added edges
        uint32_t addedEdges = 0;

        auto it = G->beginNeighborhood(node);
        while (it != G->endNeighborhood(node)) {
            uint32_t neighbor = *it;

            // If all precomputed nodes have been skipped or the neighbor is smaller than the current precomputed node, add the edge
            if (precomputedNodesID < 0 || neighbor < precomputedNodes[node][precomputedNodesID]) {
                newHead[overallAddedEdges] = neighbor;
                newUpwardWeights[overallAddedEdges] = G->getUpwardWeight(std::distance(headStart, it));
                newDownwardWeights[overallAddedEdges] = G->getDownwardWeight(std::distance(headStart, it));
                addedEdges++;
                overallAddedEdges++;
                it++;
            }
            else if (neighbor > precomputedNodes[node][precomputedNodesID]) precomputedNodesID--;

            // If neighbor == precomputedNodes[node][precomputedNodesID], skip adding the edge
            else {
                it++;
                precomputedNodesID--;
            }
        }
        /*
        for (auto it = G->beginNeighborhood(node); it != G->endNeighborhood(node); it++) {
            uint32_t neighbor = *it;

            if (neighbor == 16945555) {
                std::cout << "Test" << std::endl;
            }
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
        */
        newFirstOut[node +1] = newFirstOut[node] + addedEdges;
    }
    newHead.resize(overallAddedEdges);
    newUpwardWeights.resize(overallAddedEdges);
    newDownwardWeights.resize(overallAddedEdges);

    Gnew = std::make_unique<Graph>(newFirstOut, newHead, newUpwardWeights, newDownwardWeights, eliminationTree,
                     precomputedNodes, precomputedDistancesUp, precomputedDistancesDown, successorUp, successorDown);

    return;
    std::cout << "In new Graph:" << std::endl;
    for (uint32_t i = 0; i < Gnew->numVertices(); i++) {
        auto headBeginning = Gnew->beginNeighborhood(0);
        for (auto j = Gnew->beginNeighborhood(i); j != Gnew->endNeighborhood(i); j++) {
            uint32_t neighbor = *j;
            if (neighbor == 16945555) {
                uint32_t edgeID = std::distance(headBeginning, j);
                std::cout << "Edge from " << i << " to " << neighbor << " with upward weight " << Gnew->getUpwardWeight(edgeID) << " and parent: " << Gnew->parentOf(i) << std::endl;
            }
        }
    }
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

//TODO: delete this:

// This function inserts item into vec such that vec remains sorted according to pred
template< typename T, typename Pred >
typename std::vector<T>::iterator insert_sorted( std::vector<T> & vec, T const& item, Pred pred ) {
    return vec.insert(
           std::lower_bound( vec.begin(), vec.end(), item, pred ),
           item
        );
}
// Predicate for sorting tuples based on the second element in descending order
auto predDescSecondElement = [](const std::tuple<uint32_t, uint32_t>& a, const std::tuple<uint32_t, uint32_t>& b) {
    return std::get<1>(a) > std::get<1>(b);
};

void DistancePreprocessing::selectNodesWithHighestWeightedInDegree(uint32_t numberOfNodesToSelect) {
    if (numberOfNodesToSelect == 0) {
        precomputedNodes = std::vector<std::vector<uint32_t>> (G->numVertices());
        return;
    }
    // the first Element in the tuple is the node ID, the second element is the size value
    size = std::vector<std::tuple<uint32_t, uint32_t>> (G->numVertices());
    for (uint32_t i = 0; i < G->numVertices(); i++) {
        size[i] = std::make_tuple(i, 1);
    }

    // computation of size vector, which is a weighted sum of the in-degrees of all descendants
    for (uint32_t node = 0; node < G->numVertices(); node++) {
        for (auto it = G->beginNeighborhood(node); it != G->endNeighborhood(node); it++) {
            uint32_t neighbor = *it;
            std::get<1>(size[neighbor]) += std::get<1>(size[node]);
        }
    }

    // TODO: delete nodeIDsOnly vector
    std::vector<bool> isInTopNodes(G->numVertices(), false);
    std::sort(size.begin(), size.end(), predDescSecondElement);
    std::vector<uint32_t> nodeIDsOnly(numberOfNodesToSelect);
    for (uint32_t i = 0; i < numberOfNodesToSelect; i++) {
        nodeIDsOnly[i] = std::get<0>(size[i]);
        isInTopNodes[nodeIDsOnly[i]] = true;
    }
    std::sort(nodeIDsOnly.begin(), nodeIDsOnly.end(), std::greater<uint32_t>());
    // print nodeIDsOnly for debugging
    /*
    std::cout << "Node IDs with highest weighted in-degree: ";
    for (uint32_t id : nodeIDsOnly) {
        std::cout << id << ", ";
    }
    std::cout << std::endl;
    */

    // TODO store the ID of the last found to shorten the search range in lower_bound? Stuff is commented out with: THIS:

    // choose vertices with highest size value, in such a way that the chosen nodes for a parent are a subset of the chosen nodes of its children

    //THIS: std::vector<uint32_t> indexLastInsertedNode(G->numVertices(),  0);
    for (uint32_t node = G->numVertices(); node-- > 0;) {
        if (G->parentOf(node) == Graph::INFINITY_VALUE) {
            precomputedNodes[node] = std::vector<uint32_t>();
            continue;
        }
        precomputedNodes[node] = precomputedNodes[G->parentOf(node)];
        //THIS: indexLastInsertedNode[node] = indexLastInsertedNode[G->parentOf(node)];
        if (isInTopNodes[G->parentOf(node)]) {
            precomputedNodes[node].push_back(G->parentOf(node));
        }
    }

    return;

    for (uint32_t i = 0; i < G->numVertices(); i++) {
        auto headBeginning = G->beginNeighborhood(0);
        for (auto j = G->beginNeighborhood(i); j != G->endNeighborhood(i); j++) {
            uint32_t neighbor = *j;
            if (neighbor == 16945555) {
                uint32_t edgeID = std::distance(headBeginning, j);
                std::cout << "Edge from " << i << " to " << neighbor << " with upward weight " << G->getUpwardWeight(edgeID) << " and parent: " << G->parentOf(i) << std::endl;
            }
        }
    }

    //print precomputed nodes for debugging of node 16945554
    std::cout << "Children of 12145846: ";
    for (uint32_t node = 0; node < G->numVertices(); node++) {
        if (G->parentOf(node) == 12145846) {
            std::cout << node << ", ";
        }
    }
    std::cout << std::endl;
    std::cout << "Precomputed nodes of node 16945554: ";
    for (uint32_t pn : precomputedNodes[16945554]) {
        std::cout << pn << ", ";
    }
    std::cout << std::endl;
    std::cout << "Precomputed nodes of node 12145846: ";
    for (uint32_t pn : precomputedNodes[12145846]) {
        std::cout << pn << ", ";
    }
    std::cout << std::endl;
    std::cout << "Precomputed nodes of node 12145845: ";
    for (uint32_t pn : precomputedNodes[12145845]) {
        std::cout << pn << ", ";
    }
    std::cout << std::endl;
    std::cout << "Precomputed nodes of node 1: ";
    for (uint32_t pn : precomputedNodes[1]) {
        std::cout << pn << ", ";
    }
    std::cout << std::endl;
    std::cout << std::endl;

    /*
    for (uint32_t i = 0; i < G->numVertices(); i++) {
        auto headBeginning = G->beginNeighborhood(0);
        for (auto j = G->beginNeighborhood(i); j != G->endNeighborhood(i); j++) {
            uint32_t neighbor = *j;
            if (neighbor == 16945555) {
                uint32_t edgeID = std::distance(headBeginning, j);
                std::cout << "Edge from " << i << " to " << neighbor << " with upward weight " << G->getUpwardWeight(edgeID) << " and parent: " << G->parentOf(i) << std::endl;
            }
        }
    }
    std::cout << std::endl;
    */

}
