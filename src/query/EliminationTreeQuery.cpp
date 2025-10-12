//
// Created by Julius on 26.09.2025.
//

#include "EliminationTreeQuery.hpp"

EliminationTreeQuery::EliminationTreeQuery(Graph *g) : G(g){
    distUp = std::vector<uint32_t> (G->numVertices(), Graph::MAX_UINT32);
    distDown = std::vector<uint32_t> (G->numVertices(), Graph::MAX_UINT32);
}

uint32_t EliminationTreeQuery::query(uint32_t s, uint32_t t) {
    while (s != t) {
        if (s < t) {
            ProcessVertexUp(s, Graph::MAX_UINT32);
            s = G->parentOf(s);
        } else {
            ProcessVertexDown(t, Graph::MAX_UINT32);
            t = G->parentOf(t);
        }
    }

    uint32_t d = Graph::MAX_UINT32;
    uint32_t u = s;

    while (u != Graph::MAX_UINT32) {
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
    distUp[u] = Graph::MAX_UINT32;
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
    distDown[u] = Graph::MAX_UINT32;
}