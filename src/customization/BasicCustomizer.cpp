//
// Created by Julius on 26.09.2025.
//

#include "BasicCustomizer.hpp"

// TODO Confirm with michael, my implementation is correct without using k
void BasicCustomizer::run() {
    for (uint32_t u = 0; u < Gplus.numVertices(); u++) {
        for (auto itv = Gplus.beginNeighborhood(u); itv != Gplus.endNeighborhood(u); itv++) {
            uint64_t v = *itv;
            for (auto itw = itv+1; itw != Gplus.endNeighborhood(u); itw++) {
                uint32_t w = *itw;
                // (u, v, w) is a triangle with u < v < w
                Gplus.setWeight(u, w, std::min(Gplus.getWeight(v, w), Gplus.getWeight(v, u) + Gplus.getWeight(u, w))); // forward
                Gplus.setWeight(w, v, std::min(Gplus.getWeight(w, v), Gplus.getWeight(u, v) + Gplus.getWeight(w, u))); // backward
            }
        }
    }
}