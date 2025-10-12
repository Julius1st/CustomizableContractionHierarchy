//
// Created by Julius on 26.09.2025.
//

#include "EliminationTreeQuery.hpp"

EliminationTreeQuery::EliminationTreeQuery(const Graph &g) : Gplus(g){
    distUp = std::vector<uint32_t> (Gplus.numVertices(), Graph::MAX_UINT32);
    distDown = std::vector<uint32_t> (Gplus.numVertices(), Graph::MAX_UINT32);
}

uint32_t EliminationTreeQuery::query(uint32_t s, uint32_t t) {
    while (s != t) {
        if (s < t) {
            ProcessVertexUp(s, Graph::MAX_UINT32);
            s = Gplus.parentOf(s);
        } else {
            ProcessVertexDown(t, Graph::MAX_UINT32);
            t = Gplus.parentOf(t);
        }
    }

    uint32_t d = Graph::MAX_UINT32;
    uint32_t u = s;

    while (u != Graph::MAX_UINT32) {
        d = std::min(d, distUp[u] + distDown[u]);
        ProcessVertexUp(u, d);
        ProcessVertexDown(u, d);
        u = Gplus.parentOf(u);
    }

    return d;
}

void EliminationTreeQuery::ProcessVertexUp(uint32_t u, uint32_t d) {
    auto headStart = Gplus.beginNeighborhood(0);
    if (distUp[u] < d) {
        for (auto it = Gplus.beginNeighborhood(u); it != Gplus.endNeighborhood(u); it++) {
            uint32_t v = *it;
            uint32_t uvIndex = std::distance(headStart, it);
            distUp[v] = std::min(distUp[v], distUp[u] + Gplus.getUpwardWeight(uvIndex));
        }
    }
    distUp[u] = Graph::MAX_UINT32;
}

void EliminationTreeQuery::ProcessVertexDown(uint32_t u, uint32_t d) {
    auto headStart = Gplus.beginNeighborhood(0);
    if (distDown[u] < d) {
        for (auto it = Gplus.beginNeighborhood(u); it != Gplus.endNeighborhood(u); it++) {
            uint32_t v = *it;
            uint32_t uvIndex = std::distance(headStart, it);
            distDown[v] = std::min(distDown[v], distDown[u] + Gplus.getDownwardWeight(uvIndex));
        }
    }
    distDown[u] = Graph::MAX_UINT32;
}