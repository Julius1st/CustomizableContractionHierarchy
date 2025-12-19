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
    auto headStart = G->beginNeighborhood(0);
    // reverse rank order:
    for (uint32_t u = G->numVertices(); u-- > 0;) {
        for (auto itv = G->beginNeighborhood(u); itv != G->endNeighborhood(u); itv++) {
            uint32_t v = *itv;
            uint32_t vIndex = std::distance(headStart, G->beginNeighborhood(v));
            uint32_t uvIndex = std::distance(headStart, itv);
            uint32_t k = 0;
            for (auto itw = itv+1; itw != G->endNeighborhood(u); itw++) {
                uint32_t w = *itw;
                uint32_t uwIndex = std::distance(headStart, itw);
                while (G->getHead(vIndex + k) != w) k++;

                // (u, v, w) is an upper triangle of uv
                if (G->getUpwardWeight(vIndex + k) + G->getDownwardWeight(uwIndex) < G->getUpwardWeight(uvIndex)) { // forward
                    G->setUpwardWeight(uvIndex, G->getDownwardWeight(uwIndex) < G->getUpwardWeight(uvIndex));
                    deleteUp[uvIndex] = true;
                }
                if (G->getDownwardWeight(vIndex + k) + G->getUpwardWeight(uwIndex) < G->getDownwardWeight(uvIndex)) { // backward
                    G->setDownwardWeight(uvIndex, G->getDownwardWeight(vIndex + k) + G->getUpwardWeight(uwIndex));
                    deleteDown[uvIndex] = true;
                }

                // (u, v, w) is an intermediate triangle of uw
                if (G->getUpwardWeight(uvIndex) + G->getUpwardWeight(vIndex + k) < G->getUpwardWeight(uwIndex)) { // forward
                    G->setUpwardWeight(uwIndex, G->getUpwardWeight(uvIndex) + G->getUpwardWeight(vIndex + k));
                    deleteUp[uwIndex] = true;
                }
                if (G->getDownwardWeight(uvIndex) + G->getDownwardWeight(vIndex + k) < G->getDownwardWeight(uwIndex)) { // backward
                    G->setDownwardWeight(uwIndex, G->getDownwardWeight(uvIndex) + G->getDownwardWeight(vIndex + k));
                    deleteDown[uwIndex] = true;
                }
            }
        }
    }
}