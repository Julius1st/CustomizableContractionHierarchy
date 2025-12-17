//
// Created by Julius on 26.09.2025.
//
#include "BasicCustomizer.hpp"

BasicCustomizer::BasicCustomizer(Graph* Graph) {
    G = Graph;
    vuEdgeForUp = std::vector<uint32_t> (G->numEdges(), Graph::INFINITY_VALUE);
    uwEdgeForUp = std::vector<uint32_t> (G->numEdges(), Graph::INFINITY_VALUE);
    wuEdgeForDown = std::vector<uint32_t> (G->numEdges(), Graph::INFINITY_VALUE);
    uvEdgeForDown = std::vector<uint32_t> (G->numEdges(), Graph::INFINITY_VALUE);
}

void BasicCustomizer::run() {
    auto headStart = G->beginNeighborhood(0);
    for (uint32_t u = 0; u < G->numVertices(); u++) {
        for (auto itv = G->beginNeighborhood(u); itv != G->endNeighborhood(u); itv++) {
            uint32_t v = *itv;
            uint32_t vIndex = std::distance(headStart, G->beginNeighborhood(v));
            uint32_t uvIndex = std::distance(headStart, itv);
            uint32_t k = 0;
            for (auto itw = itv+1; itw != G->endNeighborhood(u); itw++) {
                uint32_t w = *itw;
                uint32_t uwIndex = std::distance(headStart, itw);
                while (G->getHead(vIndex + k) != w) k++;

                // (u, v, w) is a triangle with u < v < w
                if (G->getDownwardWeight(uvIndex) + G->getUpwardWeight(uwIndex) < G->getUpwardWeight(vIndex + k)) {
                    G->setUpwardWeight(vIndex + k, G->getDownwardWeight(uvIndex) + G->getUpwardWeight(uwIndex));
                    vuEdgeForUp[vIndex + k] = uvIndex;
                    uwEdgeForUp[vIndex + k] = uwIndex;
                }
                if (G->getUpwardWeight(uvIndex) + G->getDownwardWeight(uwIndex) < G->getDownwardWeight(vIndex + k)) {
                    G->setDownwardWeight(vIndex + k, G->getUpwardWeight(uvIndex) + G->getDownwardWeight(uwIndex));
                    wuEdgeForDown[vIndex + k] = uwIndex;
                    uvEdgeForDown[vIndex + k] = uvIndex;
                }
            }
        }
    }
}