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

    if (n != order.size()) throw std::invalid_argument("permuteNodeIDs: order.size() does not match number of vertices");

    // Build inverse permutation: rank[oldID] = newID
    rank.resize(n);
    for (uint32_t newID = 0; newID < n; ++newID)
        rank[order[newID]] = newID;

    // Temporary adjacency for new graph
    std::vector<std::vector<uint32_t>> adj(n);

    // Iterate over original edges (edges only at lower old ID)
    for (uint32_t oldU = 0; oldU < n; ++oldU) {
        for (auto it = G->beginNeighborhood(oldU); it != G->endNeighborhood(oldU); ++it) {
            uint32_t oldV = *it;
            // should not happen:
            if (oldU >= oldV) throw std::invalid_argument("permuteNodeIDs: duplicate or self loop discovered");

            // Map to new IDs
            uint32_t newU = rank[oldU];
            uint32_t newV = rank[oldV];

            // Store only at lower new ID
            uint32_t lower = std::min(newU, newV);
            uint32_t upper = std::max(newU, newV);
            adj[lower].push_back(upper);
        }
    }

    // Build firstOut and head arrays
    permutedFirstOut.assign(n + 1, 0);
    permutedHead.clear();

    // Count edges
    for (uint32_t u = 0; u < n; ++u)
        permutedFirstOut[u + 1] = permutedFirstOut[u] + adj[u].size();

    permutedHead.reserve(permutedFirstOut.back());
    for (uint32_t u = 0; u < n; ++u) {
        auto& neigh = adj[u];
        std::sort(neigh.begin(), neigh.end());
        for (uint32_t v : neigh)
            permutedHead.push_back(v);
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

uint32_t ContractionBuilder::findEdgeInOriginal(uint32_t lowerOld, uint32_t upperOld) {
    auto firstHead = G->beginNeighborhood(0);

    for (auto it = G->beginNeighborhood(lowerOld); it < G->endNeighborhood(lowerOld); it++) {
        if (*it == upperOld) return std::distance(firstHead, it);
    }

    throw std::runtime_error("findEdgeInOriginal: tried to find a non existing edge.");
}

std::vector<uint32_t> ContractionBuilder::permuteWeights(const std::vector<uint32_t>& weights) {
    const uint32_t n = G->numVertices();
    std::vector<uint32_t> permutedWeights(permutedHead.size(), Graph::MAX_UINT32);

    for (uint32_t newU = 0; newU < n; ++newU) {
        uint32_t begin = permutedFirstOut[newU];
        uint32_t end = permutedFirstOut[newU + 1];

        for (uint32_t idx = begin; idx < end; ++idx) {
            uint32_t newV = permutedHead[idx];

            // map both endpoints back to old IDs
            uint32_t oldU = order[newU];
            uint32_t oldV = order[newV];

            // edges in original graph are always stored at the lower old ID
            uint32_t lowerOld = (oldU < oldV) ? oldU : oldV;
            uint32_t upperOld = (oldU < oldV) ? oldV : oldU;

            // find edge position in original adjacency
            uint32_t pos = findEdgeInOriginal(lowerOld, upperOld);

            permutedWeights[idx] = weights[pos];  // copy original weight
        }
    }

    return permutedWeights;
}

std::vector<uint32_t> ContractionBuilder::buildNewWeightsForGplus(const std::vector<uint32_t>& permutedWeights) {
    std::vector<uint32_t> newWeights(GplusHead.size(), Graph::MAX_UINT32);

    for (uint32_t vertex = 0; vertex < GplusFirstOut.size()-1; vertex++) {
        uint32_t contractedIndex = GplusFirstOut[vertex];
        uint32_t oldIndex = permutedFirstOut[vertex];
        while (oldIndex != permutedFirstOut[vertex+1]) {
            if (GplusHead[contractedIndex] == permutedHead[oldIndex]) {
                newWeights[contractedIndex] = permutedWeights[oldIndex];
                contractedIndex++;
                oldIndex++;
            } else {
                contractedIndex++;
            }
        }

    }

    return newWeights;
}