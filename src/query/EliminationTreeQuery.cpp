//
// Created by Julius on 26.09.2025.
//

#include "EliminationTreeQuery.hpp"

EliminationTreeQuery::EliminationTreeQuery(Graph *g) : G(g){
    distUp = std::vector<uint32_t> (G->numVertices(), Graph::INFINITY_VALUE);
    distDown = std::vector<uint32_t> (G->numVertices(), Graph::INFINITY_VALUE);
    predecessorUp = std::vector<uint32_t> (G->numVertices(), Graph::INFINITY_VALUE);
    predecessorDown = std::vector<uint32_t> (G->numVertices(), Graph::INFINITY_VALUE);
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
            s = G->eliminationTree[s];
        } else {
            ProcessVertexDown(t, d);
            t = G->eliminationTree[t];
        }
    }

    uint32_t u = s;

    while (u != Graph::INFINITY_VALUE) {
        d = std::min(d, distUp[u] + distDown[u]);
        ProcessVertexUp(u, d);
        ProcessVertexDown(u, d);
        u = G->eliminationTree[u];
    }

    return d;
}

void EliminationTreeQuery::ProcessVertexUp(uint32_t u, uint32_t d) {
    if (distUp[u] < d) {
        for (uint32_t i = G->firstOut[u]; i < G->firstOut[u+1]; i++) {
            uint32_t v = G->head[i];
            if (distUp[u] + G->upwardWeights[i] < distUp[v]) {
                distUp[v] = distUp[u] + G->upwardWeights[i];
                predecessorUp[v] = u;
            }
            relaxedEdges++;
        }
    }
    distUp[u] = Graph::INFINITY_VALUE;
}

void EliminationTreeQuery::ProcessVertexDown(uint32_t u, uint32_t d) {
    if (distDown[u] < d) {
        for (uint32_t i = G->firstOut[u]; i < G->firstOut[u+1]; i++) {
            uint32_t v = G->head[i];
            if (distDown[u] + G->downwardWeights[i] < distDown[v]) {
                distDown[v] = distDown[u] + G->downwardWeights[i];
                predecessorDown[v] = u;
            }
            relaxedEdges++;
        }
    }
    distDown[u] = Graph::INFINITY_VALUE;
}

uint32_t EliminationTreeQuery::initializeDistances(uint32_t s, uint32_t t) {
    uint32_t d = Graph::INFINITY_VALUE;

    for (uint32_t sIndex = 0; sIndex < G->precomputedNodes[s].size(); sIndex++) {
        if (G->precomputedDistancesUp[s][sIndex] >= Graph::INFINITY_VALUE) continue;
        distUp[G->precomputedNodes[s][sIndex]] = G->precomputedDistancesUp[s][sIndex];
        uint32_t currentVertex = s;

        // The prerequisite for this to work is, that in every precomputedNodes[x][sIndex] the same sIndex corresponds to the same node in the elimination tree. (For this sub tree)
        while (currentVertex != G->precomputedNodes[s][sIndex]) {
            predecessorUp[G->successorUp[currentVertex][sIndex]] = currentVertex;
            currentVertex = G->successorUp[currentVertex][sIndex];
        }
    }

    for (uint32_t tIndex = 0; tIndex < G->precomputedNodes[t].size(); tIndex++) {
        if (G->precomputedDistancesDown[t][tIndex] >= Graph::INFINITY_VALUE) continue;
        distDown[G->precomputedNodes[t][tIndex]] = G->precomputedDistancesDown[t][tIndex];

        uint32_t currentVertex = t;
        while (currentVertex != G->precomputedNodes[t][tIndex]) {
            predecessorDown[G->successorDown[currentVertex][tIndex]] = currentVertex;
            currentVertex = G->successorDown[currentVertex][tIndex];
        }

        d = std::min(d, distUp[G->precomputedNodes[t][tIndex]] + distDown[G->precomputedNodes[t][tIndex]]);
    }

    return d;
}