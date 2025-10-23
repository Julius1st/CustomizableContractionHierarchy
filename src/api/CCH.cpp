//
// Created by Julius on 26.09.2025.
//
#include "CCH.hpp"

CCH::CCH(Graph* baseGraph, std::vector<uint32_t> &order) : G(baseGraph), rankOrder(order) {
    builder = new ContractionBuilder(G, rankOrder);
}

void CCH::preprocess() {
    Gplus = builder->buildGplus();
    basicCustomizer = new BasicCustomizer(Gplus);
}

void CCH::customize() {
    basicCustomizer->run();
    queryEngine = new EliminationTreeQuery(Gplus);
}

uint32_t CCH::query(uint32_t s, uint32_t t) {
    return queryEngine->query(builder->getRank(s), builder->getRank(t));
}