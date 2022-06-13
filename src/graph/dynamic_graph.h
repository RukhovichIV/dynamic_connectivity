#include <unordered_map>
#include <unordered_set>

#include "../../src/forest/simple_forest.h"

class LevelGraph {
public:
    LevelGraph() = delete;
    LevelGraph(size_t level, size_t n_vertices) : spanning_forest_(n_vertices), level_(level) {
    }

    void insert_to_level(size_t u, size_t v) {
        if (edges_at_level_.find(u) == edges_at_level_.end() || edges_at_level_[u].empty()) {
            spanning_forest_.vertices_[u].set_has_level_edges(true);
        }
        edges_at_level_[u].insert(v);

        if (edges_at_level_.find(v) == edges_at_level_.end() || edges_at_level_[v].empty()) {
            spanning_forest_.vertices_[v].set_has_level_edges(true);
        }
        edges_at_level_[v].insert(u);

        if (!spanning_forest_.is_connected(u, v)) {
            spanning_forest_.add_new_edge(u, v);
        }
    }

    void erase_from_level(size_t u, size_t v) {
        edges_at_level_[u].erase(v);
        if (edges_at_level_[u].empty()) {
            spanning_forest_.vertices_[u].set_has_level_edges(false);
        }

        edges_at_level_[v].erase(u);
        if (edges_at_level_[v].empty()) {
            spanning_forest_.vertices_[v].set_has_level_edges(false);
        }

        if (!spanning_forest_.is_connected(u, v)) {
            return;
        }
        auto tree_pair = spanning_forest_.erase_existing_edge(u, v);
        auto lesser_tree = (tree_pair.first->size() < tree_pair.second->size() ? tree_pair.first : tree_pair.second);
        
    }

    bool is_connected(size_t u, size_t v) {
        return spanning_forest_.is_connected(u, v);
    }

private:
    // void reduce_edge_level(size_t u, size_t v) {
    //     edges_at_level_[u].erase(v);
    //     if (edges_at_level_[u].empty()) {
    //         spanning_forest_.vertices_[u].set_has_level_edges(false);
    //     }

    //     edges_at_level_[v].erase(u);
    //     if (edges_at_level_[v].empty()) {
    //         spanning_forest_.vertices_[v].set_has_level_edges(false);
    //     }
    // }

    Forest spanning_forest_;
    std::unordered_map<size_t , std::unordered_set<size_t>> edges_at_level_;
    size_t level_;
};