#include <iostream>
#include <stdexcept>
#include <chrono>
#include <queue>
#include "CCH.hpp"
#include "VectorIO.h"

using namespace std;

struct UndirectedEdge {
    uint32_t a, b;
    uint32_t w_up;   // weight from a->b
    uint32_t w_down; // weight from b->a
};

struct DirectedEdge {
    uint32_t a, b;
    uint32_t weight; // weight from a->b
};

void cleanInputData(vector<uint32_t>& in_first_out,
                    vector<uint32_t>& in_head,
                    vector<uint32_t>& in_weight,
                    vector<uint32_t>& clean_first_out,
                    vector<uint32_t>& clean_head,
                    vector<uint32_t>& clean_upward_weight,
                    vector<uint32_t>& clean_downward_weight,
                    vector<vector<DirectedEdge>>& adj) {

    uint32_t missing_weight = 0xFFFFFFFF/2 -1;
    const size_t n = in_first_out.size() - 1;
    vector<UndirectedEdge> edges;

    // collect edges (loop-free, normalized to (min,max))
    for (uint32_t u = 0; u < n; ++u) {
        for (uint32_t e = in_first_out[u]; e < in_first_out[u + 1]; ++e) {
            uint32_t v = in_head[e];
            uint32_t w = in_weight[e];
            if (u == v) continue; // skip loops

            uint32_t a = min(u, v);
            uint32_t b = max(u, v);

            UndirectedEdge edge{a, b, missing_weight, missing_weight};
            if (u == a) edge.w_up = w;     // u->v means a->b
            else edge.w_down = w;           // v->u means b->a

            edges.push_back(edge);
        }
    }

    // sort by (a,b)
    sort(edges.begin(), edges.end(), [](const auto& e1, const auto& e2) {
        return (e1.a < e2.a) || (e1.a == e2.a && e1.b < e2.b);
    });

    // merge duplicates (multi-edges)
    vector<UndirectedEdge> merged;
    for (size_t i = 0; i < edges.size();) {
        size_t j = i + 1;
        uint32_t a = edges[i].a, b = edges[i].b;
        uint32_t best_up = edges[i].w_up;
        uint32_t best_down = edges[i].w_down;

        while (j < edges.size() && edges[j].a == a && edges[j].b == b) {
            if (edges[j].w_up != missing_weight)
                best_up = (best_up == missing_weight) ? edges[j].w_up : std::min(best_up, edges[j].w_up);
            if (edges[j].w_down != missing_weight)
                best_down = (best_down == missing_weight) ? edges[j].w_down : std::min(best_down, edges[j].w_down);
            ++j;
        }

        merged.push_back({a, b,best_up,best_down});

        i = j;
    }

    // Build vectors
    clean_first_out.assign(n + 1, 0);
    clean_head.reserve(merged.size());
    clean_upward_weight.reserve(merged.size());
    clean_downward_weight.reserve(merged.size());

    for (auto& e : merged) {
        ++clean_first_out[e.a + 1];
        clean_head.push_back(e.b);
        clean_upward_weight.push_back(e.w_up);
        clean_downward_weight.push_back(e.w_down);

        // Build bidirectional adj list used for Dijkstra
        adj[e.a].push_back({e.a, e.b, e.w_up});
        adj[e.b].push_back({e.b, e.a, e.w_down});
    }

    // Prefix-sum first_out
    for (size_t i = 1; i <= n; ++i)
        clean_first_out[i] += clean_first_out[i - 1];
}

uint32_t dijkstra(uint32_t s, uint32_t t, vector<vector<DirectedEdge>> adj, vector<uint32_t>& predecessor, vector<uint32_t>& hop_count) {
    uint32_t n = adj.size();
    uint32_t infinity = 0xFFFFFFFF/2 -1;
    vector<uint32_t> dist(n, infinity);
    dist[s] = 0;
    hop_count[s] = 0;
    priority_queue<pair<uint32_t, uint32_t>, vector<pair<uint32_t, uint32_t>>, greater<pair<uint32_t, uint32_t>> > pq;

    pq.push({0, s});
    while (!pq.empty()) {
        uint32_t u = pq.top().second;
        pq.pop();

        if (u == t) return dist[t];

        for (auto it = adj[u].begin(); it != adj[u].end(); it++) {
            uint32_t v = it->b;
            uint32_t weight = it->weight;
            if (dist[v] > dist[u] + weight) {
                dist[v] = dist[u] + weight;
                predecessor[v] = u;
                hop_count[v] = hop_count[u] + 1;
                pq.push({dist[v], v});
            }
        }
    }

    return dist[t];
}

Graph* buildTEstGraph() {
    // Build a small test graph
    vector<uint32_t> first_out = {0, 2, 3, 4, 5, 7, 8, 8, 8};
    vector<uint32_t> head = {2, 3, 3, 4, 4, 5, 6, 7};
    vector<uint32_t> upward_weight = {2, 3, 13, 24, 34, 45, 46, 57};
    vector<uint32_t> downward_weight = {20, 30, 31, 42, 43, 54, 64, 75};

    return new Graph(first_out, head, upward_weight, downward_weight);
}

int main(int argc, char *argv[]) {
    try{
        string graph_first_out;
        string graph_head;
        string graph_weight;

        string ch_order;
        string ch_first_out;
        string ch_head;
        string ch_forward_weight;
        string ch_backward_weight;

        if(argc != 5){
            cerr << argv[0] << " graph_first_out graph_head graph_weight ch_order" << endl;
            return 1;
        }else{
            graph_first_out = argv[1];
            graph_head = argv[2];
            graph_weight = argv[3];
            ch_order = argv[4];
        }


        cout << "Loading graph ... " << flush;

        vector<uint32_t>first_out = load_vector<uint32_t>(graph_first_out);
        vector<uint32_t>head = load_vector<uint32_t>(graph_head);
        vector<uint32_t>weight = load_vector<uint32_t>(graph_weight);
        vector<uint32_t>order = load_vector<uint32_t >(ch_order);

        cout << "done" << endl;

        unsigned node_count = first_out.size()-1;
        const unsigned arc_count = head.size();

        cout << "Processing graph data ... " << flush;

        if(first_out.front() != 0)
            throw runtime_error("The first element of first out must be 0.");
        if(first_out.back() != arc_count)
            throw runtime_error("The last element of first out must be the arc count.");
        if(head.empty())
            throw runtime_error("The head vector must not be empty.");
        for (unsigned int i : head) {
            if (i >= node_count) throw runtime_error("The head vector contains an out-of-bounds node id.");
        }
        if(weight.size() != arc_count)
            throw runtime_error("The weight vector must be as long as the number of arcs");

        vector<uint32_t> clean_first_out;
        vector<uint32_t> clean_head;
        vector<uint32_t> clean_upward_weight;
        vector<uint32_t> clean_downward_weight;
        vector<vector<DirectedEdge>> adj; // only used for Dijkstra, may be discarded when only querying CCH
        adj.resize(node_count);

        cleanInputData(first_out, head, weight, clean_first_out, clean_head, clean_upward_weight, clean_downward_weight, adj);

        cout << "done" << endl;

        Graph* G = new Graph(clean_first_out, clean_head, clean_upward_weight, clean_downward_weight);

        // activate to use test graph:
        //G = buildTEstGraph();
        //order = {0, 1, 2, 3, 4, 5, 6, 7};
        //node_count = G->numVertices();

        CCH* cch = new CCH(G, order);
        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        cout << "Preprocessing Graph ... " << flush;
        cch->preprocess();
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        cout << "done, time in milliseconds: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << endl;

        cout << "Customizing Graph ... " << std::endl << flush;
        begin = std::chrono::steady_clock::now();
        cch->customize();
        end = std::chrono::steady_clock::now();
        cout << "done, time in milliseconds: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << endl;

        cout << "Querying distances ... " << endl << flush;
        int norm_time = 0;
        int preproc_time = 0;

        // 1000 random queries from which the average query runtime is computed
        for (uint32_t i = 0; i < 1000; ++i) {
            uint32_t s = rand() % node_count;
            uint32_t t = rand() % node_count;

            begin = std::chrono::steady_clock::now();
            uint32_t cch_distance = cch->query(s, t);
            end = std::chrono::steady_clock::now();
            norm_time += std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();

            begin = std::chrono::steady_clock::now();
            uint32_t cch_precomputed_distance = cch->queryWithDistancePreprocessing(s, t);
            end = std::chrono::steady_clock::now();
            preproc_time += std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();

            if(cch_distance != cch_precomputed_distance) {
                cout << "Distances from CCH query and distance-preprocessed CCH query do not match. Distance from normal query: "
                                    + std::to_string(cch_distance) + ", distance from distance-preprocessed query: "
                                    + std::to_string(cch_precomputed_distance) + " for query from "
                                    + std::to_string(s) + " to " + std::to_string(t) + ". This was query number: " + std::to_string(i) << endl;
            }

        }

        cout << "done, time in nanoseconds (normal query): " << norm_time << " average per query: " << norm_time /1000 << endl;
        cout << "done, time in nanoseconds (distance-preprocessed query): " << preproc_time << " average per query: " << preproc_time /1000 << endl;
        std::cout << "Query init time in nanoseconds (avg): " << cch->getInitTime() /1000 << std::endl;
        std::cout << "Initialized fields (avg): " << cch->getInitializedFields() /1000 << std::endl;

    }catch(exception&err){
        cerr << "Stopped on exception : " << err.what() << endl;
    }
}


