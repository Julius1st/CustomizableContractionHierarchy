//
// Created by Julius on 05.11.2025.
//

#include "DistancePreprocessing.hpp"

Graph* DistancePreprocessing::run() {

    precomputeDistances();
    createGraphWithPrecomputedDistances();

    return Gnew;
}

void DistancePreprocessing::precomputeDistances() {

}

void DistancePreprocessing::createGraphWithPrecomputedDistances() {
    // TODO: create new Graph by deleting superfluous edges and setting the precomputed distances
}