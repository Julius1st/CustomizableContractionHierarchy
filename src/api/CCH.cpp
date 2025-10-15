//
// Created by Julius on 26.09.2025.
//
#include "CCH.hpp"

CCH::CCH(Graph* baseGraph, std::vector<uint32_t> &rank) : G(baseGraph), rankOrder(rank) {
    builder = new ContractionBuilder(G, rankOrder);
}

void CCH::preprocess() {
    Gplus = builder->buildGplus();
    basicCustomizer = new BasicCustomizer(Gplus);
}

void CCH::customize(const std::vector<uint32_t> &upward_weights, const std::vector<uint32_t> &downward_weights) {
    std::vector<uint32_t> Gplus_upward_weights = builder->buildNewWeightsForGplus(
                            builder->permuteWeights(upward_weights));
    std::vector<uint32_t> Gplus_downward_weights = builder->buildNewWeightsForGplus(
                            builder->permuteWeights(downward_weights));

    Gplus->setUpwardWeights(Gplus_upward_weights);
    Gplus->setDownwardWeights(Gplus_downward_weights);

    basicCustomizer->run();

    queryEngine = new EliminationTreeQuery(Gplus);
}

uint32_t CCH::query(uint32_t s, uint32_t t) {
    return queryEngine->query(s, t);
}