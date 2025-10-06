//
// Created by Julius on 26.09.2025.
//

#include "EliminationTreeQuery.hpp"

EliminationTreeQuery::EliminationTreeQuery(const Graph &g) : Gplus(g){
    distUp = std::vector<uint32_t> (Gplus.numVertices(), Graph::MAX_UINT32);
    distDown = std::vector<uint32_t> (Gplus.numVertices(), Graph::MAX_UINT32);
}

uint32_t EliminationTreeQuery::query(int s, int t) {
    while (s != t) {
        // TODO: set s or to to infinity if out of bounds? (See Algo 3.5)
        if (s < t) {
            ProcessVertexUp(s, Graph::MAX_UINT32);
            s = Gplus.parentOf(s);
        } else {
            ProcessVertexDown(t, Graph::MAX_UINT32);
            t = Gplus.parentOf(t);
        }
    }

    uint32_t d = Graph::MAX_UINT32;
    int u = s;

    while (u != Graph::NO_ET_PARENT) {
        d = std::min(d, distUp[u] + distDown[u]);
        ProcessVertexUp(u, d);
        ProcessVertexDown(u, d);
        u = Gplus.parentOf(u);
    }

    return d;
}

void EliminationTreeQuery::ProcessVertexUp(int u, uint32_t d) {
    if (distUp[u] < d) {
        for (int v : Gplus.neighbors(u)) {
            distUp[v] = std::min(distUp[v], distUp[u] + Gplus.getWeight(u, v));
        }
    }
    distUp[u] = Graph::MAX_UINT32;
}

void EliminationTreeQuery::ProcessVertexDown(int u, uint32_t d) {
    if (distDown[u] < d) {
        for (int v : Gplus.neighbors(u)) {
            distDown[v] = std::min(distDown[v], distDown[u] + Gplus.getWeight(v, u));
        }
    }
    distDown[u] = Graph::MAX_UINT32;
}