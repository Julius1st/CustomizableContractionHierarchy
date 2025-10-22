//
// Created by Julius on 21.10.2025.
//
#include <iostream>
#include <stdexcept>
#include <queue>
#include "CCH.hpp"

using namespace std;

int main() {

    vector<uint32_t> first_out = {0, 2, 3, 4, 5, 7, 8, 8, 8};
    vector<uint32_t> head = {2, 3, 3, 4, 4, 5, 6, 7};
    vector<uint32_t> order = {0, 1, 2, 3, 4, 5, 6, 7};
    vector<uint32_t> upward_weight = {2, 3, 13, 24, 34, 45, 46, 57};
    vector<uint32_t> downward_weight = {20, 30, 31, 42, 43, 54, 64, 75};

    Graph* G = new Graph(first_out, head);

    CCH* cch = new CCH(G, order);

    cch->preprocess();

    cch->customize(upward_weight, downward_weight);

    uint32_t s = 4;
    uint32_t t = 6;

    uint32_t distance = cch->query(s, t);
    cout << "The computed distance is: " + std::to_string(distance) << endl;
}