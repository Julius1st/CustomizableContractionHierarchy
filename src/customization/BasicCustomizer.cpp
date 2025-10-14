//
// Created by Julius on 26.09.2025.
//
#include <iostream>
#include "BasicCustomizer.hpp"

void BasicCustomizer::run() {
    auto headStart = G->beginNeighborhood(0);
    for (uint32_t u = 0; u < G->numVertices(); u++) {
        std::cout << "u: " << std::to_string(u) << std::endl << std::flush;
        for (auto itv = G->beginNeighborhood(u); itv != G->endNeighborhood(u); itv++) {
            uint32_t v = *itv;
            uint32_t vIndex = std::distance(headStart, G->beginNeighborhood(v));
            uint32_t uvIndex = std::distance(headStart, itv);
            uint32_t k = 0;
            std::cout << "u: " << std::to_string(u) << " v: " << std::to_string(v) << std::endl << std::flush;
            for (auto itw = itv+1; itw != G->endNeighborhood(u); itw++) {
                uint32_t w = *itw;
                uint32_t uwIndex = std::distance(headStart, itw);
                while (G->getHead(vIndex + k) != w) {k++; //std::cout << "k: " << std::to_string(k) << std::endl;
                 }
                std::cout << "u: " << std::to_string(u) << " v: " << std::to_string(v) << " w: " << std::to_string(w) << std::endl << std::flush;
                // (u, v, w) is a triangle with u < v < w
                G->setUpwardWeight(vIndex + k,
                std::min(G->getUpwardWeight(vIndex + k), G->getDownwardWeight(uvIndex) + G->getUpwardWeight(uwIndex)));
                G->setDownwardWeight(vIndex + k,
                std::min(G->getDownwardWeight(vIndex + k), G->getUpwardWeight(uvIndex) + G->getDownwardWeight(uwIndex)));
            }
        }
    }
}