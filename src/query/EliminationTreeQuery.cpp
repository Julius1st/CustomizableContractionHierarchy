//
// Created by Julius on 26.09.2025.
//

#include "EliminationTreeQuery.hpp"

EliminationTreeQuery::EliminationTreeQuery(Graph *gUp, Graph* gDown) : Gup(gUp), Gdown(gDown) {
    if (gUp->n != gDown->n) throw std::runtime_error("The two graphs used to initialize the query must have the same number of vertices.");
    distUp = std::vector<uint32_t> (Gup->numVertices(), Graph::INFINITY_VALUE);
    distDown = std::vector<uint32_t> (Gup->numVertices(), Graph::INFINITY_VALUE);
    predecessorUp = std::vector<uint32_t> (Gup->numVertices(), Graph::INFINITY_VALUE);
    predecessorDown = std::vector<uint32_t> (Gup->numVertices(), Graph::INFINITY_VALUE);
}

uint32_t EliminationTreeQuery::query(uint32_t s, uint32_t t) {
    distUp[s] = 0;
    distDown[t] = 0;

    // Initialize distances from precomputed nodes (query speed-up)
    auto begin = std::chrono::steady_clock::now();
    uint32_t d = initializeDistances(s, t);
    auto end = std::chrono::steady_clock::now();
    initTime += std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
    initializedFields += Gup->precomputedNodes[s].size() + Gup->precomputedNodes[t].size();

    // Original Algorithm
    while (s != t) {
        if (s < t) {
            ProcessVertexUp(s, d);
            s = Gup->eliminationTree[s];
        } else {
            ProcessVertexDown(t, d);
            t = Gdown->eliminationTree[t];
        }
    }

    uint32_t u = s;

    while (u != Graph::INFINITY_VALUE) {
        d = std::min(d, distUp[u] + distDown[u]);
        ProcessVertexUp(u, d);
        ProcessVertexDown(u, d);
        u = Gup->eliminationTree[u];
    }

    return d;
}

void EliminationTreeQuery::ProcessVertexUp(uint32_t u, uint32_t d) {
    if (distUp[u] < d) {
        for (uint32_t i = Gup->firstOut[u]; i < Gup->firstOut[u+1]; i++) {
            uint32_t v = Gup->head[i];
            if (distUp[u] + Gup->upwardWeights[i] < distUp[v]) {
                distUp[v] = distUp[u] + Gup->upwardWeights[i];
                predecessorUp[v] = u;
            }
            relaxedEdges++;
        }
    }
    distUp[u] = Graph::INFINITY_VALUE;
}

void EliminationTreeQuery::ProcessVertexDown(uint32_t u, uint32_t d) {
    if (distDown[u] < d) {
        for (uint32_t i = Gdown->firstOut[u]; i < Gdown->firstOut[u+1]; i++) {
            uint32_t v = Gdown->head[i];
            if (distDown[u] + Gdown->downwardWeights[i] < distDown[v]) {
                distDown[v] = distDown[u] + Gdown->downwardWeights[i];
                predecessorDown[v] = u;
            }
            relaxedEdges++;
        }
    }
    distDown[u] = Graph::INFINITY_VALUE;
}

uint32_t EliminationTreeQuery::initializeDistances(uint32_t s, uint32_t t) {
    uint32_t d = Graph::INFINITY_VALUE;

    for (uint32_t sIndex = 0; sIndex < Gup->precomputedNodes[s].size(); sIndex++) {
        if (Gup->precomputedDistancesUp[s][sIndex] >= Graph::INFINITY_VALUE) continue;
        distUp[Gup->precomputedNodes[s][sIndex]] = Gup->precomputedDistancesUp[s][sIndex];
        uint32_t currentVertex = s;

        // The prerequisite for this to work is, that in every precomputedNodes[x][sIndex] the same sIndex corresponds to the same node in the elimination tree. (For this sub tree)
        while (currentVertex != Gup->precomputedNodes[s][sIndex]) {
            predecessorUp[Gup->successorUp[currentVertex][sIndex]] = currentVertex;
            currentVertex = Gup->successorUp[currentVertex][sIndex];
        }
    }

    for (uint32_t tIndex = 0; tIndex < Gdown->precomputedNodes[t].size(); tIndex++) {
        if (Gdown->precomputedDistancesDown[t][tIndex] >= Graph::INFINITY_VALUE) continue;
        distDown[Gdown->precomputedNodes[t][tIndex]] = Gdown->precomputedDistancesDown[t][tIndex];

        uint32_t currentVertex = t;
        while (currentVertex != Gdown->precomputedNodes[t][tIndex]) {
            predecessorDown[Gdown->successorDown[currentVertex][tIndex]] = currentVertex;
            currentVertex = Gdown->successorDown[currentVertex][tIndex];
        }

        d = std::min(d, distUp[Gup->precomputedNodes[t][tIndex]] + distDown[Gdown->precomputedNodes[t][tIndex]]);
    }

    return d;
}