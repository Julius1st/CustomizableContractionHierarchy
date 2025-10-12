//
// Created by Julius on 26.09.2025.
//

#include "ContractionBuilder.hpp"

Graph* ContractionBuilder::buildGplus() {
    permuteNodeIDs();
    contractGraph();

    return Gplus;
}


void ContractionBuilder::permuteNodeIDs() {
    uint32_t n = G->numVertices();
    uint32_t m = G->numEdges();
    if(n != rank.size()) throw std::invalid_argument("When permuting the node IDs in the ContractionBuilder, the number of nodes did not match the size of the rank vector.");
    permutedFirstOut.resize(n+1);
    permutedFirstOut[n] = m;
    permutedHead.resize(m);

    // Build a reverse mapping of rank
    for (uint32_t u = 0; u < rank.size(); u++) {
        reverseRank[rank[u]] = u;
    }

    uint32_t currentIndex = 0;
    for (uint32_t u = 0; u < n; u++) {
        permutedFirstOut[u] = currentIndex;
        for (auto it = G->beginNeighborhood(reverseRank[u]); it != G->endNeighborhood(reverseRank[u]); it++) {
            permutedHead[currentIndex] = *it;
            currentIndex++;
        }
    }

    // sort neighborhoods in Head
    for (uint32_t u = 0; u < n; u++) {
        std::sort(permutedHead.begin() + permutedFirstOut[u], permutedHead.begin() + permutedFirstOut[u+1]);
    }
}

void ContractionBuilder::contractGraph() {
    std::vector<uint32_t> contractedHead;
    std::vector<std::vector<uint32_t>> adj(G->numVertices());
    std::vector<uint32_t> ET(G->numVertices(), Graph::MAX_UINT32);

    // Build Adjacency list
    for (uint32_t u = 0; u < G->numVertices(); u++) {
        for (uint32_t i = permutedFirstOut[u]; i < permutedFirstOut[u+1]; i++) {
            adj[u].push_back(permutedHead[i]);
        }
    }

    // Do contraction
    for (uint32_t u = 0; u < G->numVertices(); u++) {
        std::vector<uint32_t>& neighborsU = adj[u];
        if (neighborsU.empty()) continue;

        std::sort(neighborsU.begin(), neighborsU.end());
        neighborsU.erase(unique(neighborsU.begin(), neighborsU.end()), neighborsU.end());

        uint32_t v = neighborsU[0];
        adj[v].insert(adj[v].end(), (neighborsU.begin() +1), neighborsU.end());

        // Build Elimination Tree
        ET[u] = v;
    }

    // Build G
    GplusFirstOut.resize(G->numVertices() + 1);
    uint32_t currentIndex = 0;

    for (uint32_t u = 0; u < G->numVertices(); u++) {
        GplusFirstOut[u] = currentIndex;
        for (uint32_t h : adj[u]) {
            GplusHead.push_back(h);
            currentIndex++;
        }
    }

    GplusFirstOut[G->numVertices()] = GplusHead.size();
    Gplus = new Graph(GplusFirstOut, GplusHead, ET);
}

std::vector<uint32_t>* ContractionBuilder::permuteWeights(std::vector<uint32_t> weights) {
    uint32_t n = G->numVertices();
    uint32_t m = G->numEdges();
    if(m != weights.size()) throw std::invalid_argument("When permuting the weights in the ContractionBuilder, the number of weights did not match the number of edges in the original Graph.");
    std::vector<uint32_t> permutedWeights(m);

    uint32_t currentIndex = 0;
    for (uint32_t u = 0; u < n; u++) {
        for (auto it = G->beginNeighborhood(reverseRank[u]); it != G->endNeighborhood(reverseRank[u]); it++) {
            uint32_t index = std::distance(G->beginNeighborhood(0), it);
            permutedWeights[currentIndex] = weights[index];
            currentIndex++;
        }
    }

    return &permutedWeights;
}

std::vector<uint32_t>* ContractionBuilder::buildNewWeightsForGplus(std::vector<uint32_t> permutedWeights) {
    std::vector<uint32_t> newWeights(GplusHead.size(), Graph::MAX_UINT32);

    for (uint32_t vertex = 0; vertex < GplusFirstOut.size()-1; vertex++) {
        uint32_t contractedIndex = GplusFirstOut[vertex];
        uint32_t oldIndex = permutedFirstOut[vertex];
        while (oldIndex != permutedFirstOut[vertex+1]) {
            if (GplusHead[contractedIndex] == permutedHead[oldIndex]) {
                newWeights[contractedIndex] = permutedWeights[oldIndex];
                contractedIndex++;
                oldIndex++;
                continue;
            } else {
                contractedIndex++;
            }
        }

    }

    return &newWeights;
}