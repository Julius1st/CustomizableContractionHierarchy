//
// Created by Julius on 26.09.2025.
//

#pragma once
#include <vector>

class EliminationTree {
public:
    EliminationTree(int n = 0);

    void setParent(int v, int parent);
    int parentOf(int v) const;

private:
    std::vector<int> parent;
};