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

    if (n != order.size()) throw std::invalid_argument("permuteNodeIDs: order.size() does not match number of vertices");

    // Build inverse permutation: rank[oldID] = newID
    rank.resize(n);
    for (uint32_t newID = 0; newID < n; ++newID)
        rank[order[newID]] = newID;

    adj.resize(n);

    // Iterate over original edges (edges only at lower old ID)
    for (uint32_t oldU = 0; oldU < n; ++oldU) {
        for (auto it = G->beginNeighborhood(oldU); it != G->endNeighborhood(oldU); ++it) {
            uint32_t oldV = *it;
            uint32_t upWeight = G->getUpwardWeight(std::distance(G->beginNeighborhood(0), it));
            uint32_t downWeight = G->getDownwardWeight(std::distance(G->beginNeighborhood(0), it));

            // should not happen:
            if (oldU >= oldV) throw std::invalid_argument("permuteNodeIDs: duplicate or self loop discovered");

            // Map to new IDs
            uint32_t newU = rank[oldU];
            uint32_t newV = rank[oldV];

            if (newU > newV) {
                // edge is downward in new graph
                std::swap(upWeight, downWeight);
            }

            // Store only at lower new ID
            uint32_t lower = std::min(newU, newV);
            uint32_t upper = std::max(newU, newV);
            adj[lower].emplace_back(upper, upWeight, downWeight);
        }
    }

    for (auto& neigh : adj) {
        // Remove multi-edges by sorting and unique
        std::sort(neigh.begin(), neigh.end(), [](const auto& e1, const auto& e2) {
            return std::get<0>(e1) < std::get<0>(e2);
        });
    }
}

void ContractionBuilder::contractGraph() {
    uint32_t n = G->numVertices();
    std::vector<uint32_t> ET(n, Graph::INFINITY);
    std::vector<std::vector<uint32_t>> weightlessAdj(n);

    // build adjacency list only containing heads for contraction
    for (uint32_t u = 0; u < n; u++) {
        std::vector<uint32_t> neighborsU;
        for (auto& tuple : adj[u]) {
            uint32_t h = std::get<0>(tuple);
            neighborsU.push_back(h);
        }
        weightlessAdj[u] = neighborsU;
    }

    // Do contraction
    for (uint32_t u = 0; u < n; u++) {
        std::vector<uint32_t>& neighborsU = weightlessAdj[u];
        if (neighborsU.empty()) continue;

        std::sort(neighborsU.begin(), neighborsU.end());
        neighborsU.erase(unique(neighborsU.begin(), neighborsU.end()), neighborsU.end());

        uint32_t v = neighborsU[0];
        weightlessAdj[v].insert(weightlessAdj[v].end(), (neighborsU.begin() +1), neighborsU.end());

        // Build Elimination Tree
        ET[u] = v;
    }

    // Build Gplus
    GplusFirstOut.resize(n + 1);
    GplusHead.clear();
    uint32_t currentIndex = 0;

    for (uint32_t u = 0; u < n; u++) {
        GplusFirstOut[u] = currentIndex;

        uint32_t weightLessIndex = 0;
        uint32_t weightedIndex = 0;
        while (weightLessIndex < weightlessAdj[u].size()) {
            if (weightedIndex >= adj[u].size()) {
                // remaining edges only in weightlessAdj
                uint32_t h_weightless = weightlessAdj[u][weightLessIndex];
                GplusHead.push_back(h_weightless);
                GplusUpwardWeights.push_back(Graph::INFINITY);
                GplusDownwardWeights.push_back(Graph::INFINITY);
                currentIndex++;
                weightLessIndex++;
                continue;
            }

            uint32_t h_weightless = weightlessAdj[u][weightLessIndex];
            uint32_t h_weighted = std::get<0>(adj[u][weightedIndex]);

            if (h_weightless == h_weighted) {
                // edge exists in both, add to Gplus
                GplusHead.push_back(h_weighted);
                GplusUpwardWeights.push_back(std::get<1>(adj[u][weightedIndex]));
                GplusDownwardWeights.push_back(std::get<2>(adj[u][weightedIndex]));
                currentIndex++;
                weightLessIndex++;
                weightedIndex++;
            } else if (h_weightless < h_weighted) {
                // edge only in weightless, add to Gplus with infinite weights
                GplusHead.push_back(h_weightless);
                GplusUpwardWeights.push_back(Graph::INFINITY);
                GplusDownwardWeights.push_back(Graph::INFINITY);
                currentIndex++;
                weightLessIndex++;
            } else {
                throw std::runtime_error("contractGraph: inconsistency between weightless and weighted adjacency detected.");
            }
        }
    }

    GplusFirstOut[n] = GplusHead.size();
    Gplus = new Graph(GplusFirstOut, GplusHead, GplusUpwardWeights, GplusDownwardWeights, ET);
}