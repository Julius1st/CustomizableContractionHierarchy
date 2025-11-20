//
// Created by Julius on 26.09.2025.
//

#include "EliminationTreeQuery.hpp"

EliminationTreeQuery::EliminationTreeQuery(Graph *g) : G(g){
    distUp = std::vector<uint32_t> (G->numVertices(), Graph::INFINITY_VALUE);
    distDown = std::vector<uint32_t> (G->numVertices(), Graph::INFINITY_VALUE);
}

uint32_t EliminationTreeQuery::query(uint32_t s, uint32_t t) {
    distUp[s] = 0;
    distDown[t] = 0;

    // Initialize distances from precomputed nodes (query speed-up)
    auto begin = std::chrono::steady_clock::now();
    uint32_t d = initializeDistances(s, t);
    auto end = std::chrono::steady_clock::now();
    initTime += std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
    initializedFields += G->precomputedNodes[s].size() + G->precomputedNodes[t].size();

    // Original Algorithm
    while (s != t) {
        if (s < t) {
            ProcessVertexUp(s, d);
            s = G->parentOf(s);
        } else {
            ProcessVertexDown(t, d);
            t = G->parentOf(t);
        }
    }

    uint32_t u = s;

    while (u != Graph::INFINITY_VALUE) {
        d = std::min(d, distUp[u] + distDown[u]);
        ProcessVertexUp(u, d);
        ProcessVertexDown(u, d);
        u = G->parentOf(u);
    }

    return d;
}

void EliminationTreeQuery::ProcessVertexUp(uint32_t u, uint32_t d) {
    auto headStart = G->beginNeighborhood(0);
    if (distUp[u] < d) {
        for (auto it = G->beginNeighborhood(u); it != G->endNeighborhood(u); it++) {
            uint32_t v = *it;
            uint32_t uvIndex = std::distance(headStart, it);
            distUp[v] = std::min(distUp[v], distUp[u] + G->getUpwardWeight(uvIndex));
        }
    }
    distUp[u] = Graph::INFINITY_VALUE;
}

void EliminationTreeQuery::ProcessVertexDown(uint32_t u, uint32_t d) {
    auto headStart = G->beginNeighborhood(0);
    if (distDown[u] < d) {
        for (auto it = G->beginNeighborhood(u); it != G->endNeighborhood(u); it++) {
            uint32_t v = *it;
            uint32_t uvIndex = std::distance(headStart, it);
            distDown[v] = std::min(distDown[v], distDown[u] + G->getDownwardWeight(uvIndex));
        }
    }
    distDown[u] = Graph::INFINITY_VALUE;
}

uint32_t EliminationTreeQuery::initializeDistances(uint32_t s, uint32_t t) {
    uint32_t d = Graph::INFINITY_VALUE;

    for (uint32_t sIndex = 0; sIndex < G->precomputedNodes[s].size(); sIndex++) {
        distUp[G->precomputedNodes[s][sIndex]] = G->precomputedDistancesUp[s][sIndex];
    }

    for (uint32_t tIndex = 0; tIndex < G->precomputedNodes[t].size(); tIndex++) {
        distDown[G->precomputedNodes[t][tIndex]] = G->precomputedDistancesDown[t][tIndex];
        if (distUp[G->precomputedNodes[t][tIndex]] != Graph::INFINITY_VALUE) {
            d = std::min(d, distUp[G->precomputedNodes[t][tIndex]] + distDown[G->precomputedNodes[t][tIndex]]);
        }
    }

    return d;
}