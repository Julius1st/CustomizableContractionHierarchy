//
// Created by julius on 15.12.25.
//

#include "PerfectCustomizer.hpp"

PerfectCustomizer::PerfectCustomizer(Graph* Graph) {
    G = Graph;
    deleteUp = std::vector<bool> (G->numEdges(), false);
    deleteDown = std::vector<bool> (G->numEdges(), false);
}

void PerfectCustomizer::run() {
    customize();
    generateGup();
    generateGdown();
}

void PerfectCustomizer::customize() {
    auto headStart = G->beginNeighborhood(0);
    // reverse rank order:
    for (uint32_t u = G->numVertices(); u-- > 0;) {
        for (uint32_t uv = G->firstOut[u]; uv < G->firstOut[u+1]; uv++) {
            uint32_t v = G->head[uv];
            uint32_t vStart = G->firstOut[v];
            uint32_t k = 0;
            for (auto uw = uv+1; uw < G->firstOut[u+1]; uw++) {
                uint32_t w = G->head[uw];
                while (G->head[vStart + k] != w) k++;

                // (u, v, w) is an upper triangle of uv
                if (G->downwardWeights[vStart + k] + G->upwardWeights[uw] < G->upwardWeights[uv]) { // forward
                    G->upwardWeights[uv] = G->downwardWeights[vStart + k] + G->upwardWeights[uw];
                    deleteUp[uv] = true;
                }
                if (G->upwardWeights[vStart + k] + G->downwardWeights[uw] < G->downwardWeights[uv]) { // backward
                    G->downwardWeights[uv] = G->upwardWeights[vStart + k] + G->downwardWeights[uw];
                    deleteDown[uv] = true;
                }

                /*
                // (u, v, w) is an upper triangle of uv
                if (G->upwardWeights[vStart + k] + G->downwardWeights[uw] < G->upwardWeights[uv]) { // forward
                    G->upwardWeights[uv] = G->upwardWeights[vStart + k] + G->downwardWeights[uw];
                    deleteUp[uv] = true;
                }
                if (G->downwardWeights[vStart + k] + G->upwardWeights[uw] < G->downwardWeights[uv]) { // backward
                    G->downwardWeights[uv] = G->downwardWeights[vStart + k] + G->upwardWeights[uw];
                    deleteDown[uv] = true;
                    if (G->downwardWeights[uv] == 35) {
                        std::cout << "Edge (" << u << ", " << v << ") has new downward weight 35" << std::endl;
                    }
                }
                 */

                // (u, v, w) is an intermediate triangle of uw
                if (G->upwardWeights[uv] + G->upwardWeights[vStart + k] < G->upwardWeights[uw]) { // forward
                    G->upwardWeights[uw] = G->upwardWeights[uv] + G->upwardWeights[vStart + k];
                    deleteUp[uw] = true;
                }
                if (G->downwardWeights[uv] + G->downwardWeights[vStart + k] < G->downwardWeights[uw]) { // backward
                    G->downwardWeights[uw] = G->downwardWeights[uv] + G->downwardWeights[vStart + k];
                    deleteDown[uw] = true;
                }
            }
        }
    }
}

void PerfectCustomizer::generateGup() {
    uint32_t numVertices = G->numVertices();
    uint32_t numEdges = G->numEdges();

    std::vector<uint32_t> newFirstOut(numVertices + 1);
    std::vector<uint32_t> newHead(numEdges);
    std::vector<uint32_t> newUpwardWeights(numEdges);
    std::vector<uint32_t> newDownwardWeights(numEdges);

    newFirstOut[0] = 0;
    uint32_t overallAddedEdges = 0;
    for (uint32_t node = 0; node < numVertices; node++) {
        for (uint32_t edge = G->firstOut[node]; edge < G->firstOut[node+1]; edge++) {
            if (!deleteUp[edge]) {
                newHead[overallAddedEdges] = G->head[edge];
                newUpwardWeights[overallAddedEdges] = G->upwardWeights[edge];
                newDownwardWeights[overallAddedEdges] = G->downwardWeights[edge];
                overallAddedEdges++;
            }
        }
        newFirstOut[node +1] = overallAddedEdges;
    }

    newHead.resize(overallAddedEdges);
    newUpwardWeights.resize(overallAddedEdges);
    newDownwardWeights.resize(overallAddedEdges);

    GperfectUp = new Graph(newFirstOut, newHead, newUpwardWeights, newDownwardWeights, G->eliminationTree);
}

void PerfectCustomizer::generateGdown() {
    uint32_t numVertices = G->numVertices();
    uint32_t numEdges = G->numEdges();

    std::vector<uint32_t> newFirstOut(numVertices + 1);
    std::vector<uint32_t> newHead(numEdges);
    std::vector<uint32_t> newUpwardWeights(numEdges);
    std::vector<uint32_t> newDownwardWeights(numEdges);

    newFirstOut[0] = 0;
    uint32_t overallAddedEdges = 0;
    for (uint32_t node = 0; node < numVertices; node++) {
        for (uint32_t edge = G->firstOut[node]; edge < G->firstOut[node+1]; edge++) {
            if (!deleteDown[edge]) {
                newHead[overallAddedEdges] = G->head[edge];
                newUpwardWeights[overallAddedEdges] = G->upwardWeights[edge];
                newDownwardWeights[overallAddedEdges] = G->downwardWeights[edge];
                overallAddedEdges++;
            }
        }
        newFirstOut[node +1] = overallAddedEdges;
    }

    newHead.resize(overallAddedEdges);
    newUpwardWeights.resize(overallAddedEdges);
    newDownwardWeights.resize(overallAddedEdges);

    GperfectDown = new Graph(newFirstOut, newHead, newUpwardWeights, newDownwardWeights, G->eliminationTree);
}