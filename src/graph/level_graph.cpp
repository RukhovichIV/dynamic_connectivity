#include <unordered_map>
#include <unordered_set>

#include "../../src/forest/simple_forest.cpp"

class LevelGraph {
public:
    LevelGraph() = delete;
    LevelGraph(size_t level, size_t n_vertices, std::shared_ptr<LevelGraph> lower_graph)
        : spanning_forest_(n_vertices), lower_graph_(lower_graph), level_(level) {
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

    std::pair<size_t, size_t> erase_from_level(size_t u, size_t v) {
        edges_at_level_[u].erase(v);
        if (edges_at_level_[u].empty()) {
            spanning_forest_.vertices_[u].set_has_level_edges(false);
        }

        edges_at_level_[v].erase(u);
        if (edges_at_level_[v].empty()) {
            spanning_forest_.vertices_[v].set_has_level_edges(false);
        }

        if (!spanning_forest_.is_connected(u, v)) {
            return std::make_pair(0, 0);
        }
        auto tree_pair = spanning_forest_.erase_existing_edge(u, v);
        if (tree_pair.second->size() < tree_pair.first->size()) {
            std::swap(tree_pair.first, tree_pair.second);
        }

        auto it = tree_pair.first->begin(true);
        while (it != tree_pair.first->end()) {
            std::unordered_set<size_t> remaining_edges;
            for (const auto& to : edges_at_level_[*it]) {
                if (spanning_forest_.is_connected(*it, to)) {
                    lower_graph_->insert_to_level(*it, to);
                    edges_at_level_[to].erase(*it);
                    if (edges_at_level_[to].empty()) {
                        spanning_forest_.vertices_[to].set_has_level_edges(false);
                    }
                } else {
                    remaining_edges.insert(to);
                }
            }
            edges_at_level_[*it] = std::move(remaining_edges);
            if (edges_at_level_[*it].empty()) {
                spanning_forest_.vertices_[*it].set_has_level_edges(false);
            }
            it.next_with_level_edges();
        }

        std::pair<size_t, size_t> replacement = std::make_pair(1, 1);
        it = tree_pair.first->begin(true);
        while (it != tree_pair.first->end()) {
            std::unordered_set<size_t> remaining_edges;
            bool found_replacement = false;
            for (const auto& to : edges_at_level_[*it]) {
                if (found_replacement) {
                    remaining_edges.insert(to);
                    continue;
                }
                if (spanning_forest_.is_connected(to, *tree_pair.second->begin())) {
                    found_replacement = true;
                    replacement = std::make_pair(*it, to);
                    spanning_forest_.add_new_edge(*it, to);
                } else {
                    lower_graph_->insert_to_level(*it, to);
                    edges_at_level_[to].erase(*it);
                    if (edges_at_level_[to].empty()) {
                        spanning_forest_.vertices_[to].set_has_level_edges(false);
                    }
                }
            }
            edges_at_level_[*it] = std::move(remaining_edges);
            if (edges_at_level_[*it].empty()) {
                spanning_forest_.vertices_[*it].set_has_level_edges(false);
            }
            if (found_replacement) {
                break;
            }
            it.next_with_level_edges();
        }
        return replacement;
    }

    void erase_and_replace(size_t u, size_t v, size_t new_u, size_t new_v) {
    }

    bool is_connected(size_t u, size_t v) {
        return spanning_forest_.is_connected(u, v);
    }

private:
    Forest spanning_forest_;
    std::shared_ptr<LevelGraph> lower_graph_;
    std::unordered_map<size_t, std::unordered_set<size_t>> edges_at_level_;
    size_t level_;

    friend class DynamicGraph;
};