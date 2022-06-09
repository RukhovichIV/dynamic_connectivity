#include <unordered_map>

#include "../../src/forest/simple_forest.h"

class SpanningForest {
public:
    void insert(size_t u, size_t v) {
        if (u == v) {
            return;
        }
        if (u >= n_vertices_ || v >= n_vertices_) {
            throw std::runtime_error("No such vertices in graph");
        }
        if (edges_.find(std::make_pair(u, v)) == edges_.end()) {
            std::swap(u, v);
        }
        auto edge = std::make_pair(u, v);
        if (edges_.find(edge) != edges_.end()) {
            throw std::runtime_error("Multiple edges are forbidden");
        }
        edges_[edge] = level_;
        if (!simple_forest_->is_connected(u, v)) {
            simple_forest_->add_new_edge(u, v);
        }
    }

    void erase(size_t u, size_t v) {
    }

    bool is_connected(size_t u, size_t v) {
        return simple_forest_->is_connected(u, v);
    }

private:
    std::shared_ptr<Forest> simple_forest_;
    std::unordered_map<std::pair<size_t, size_t>, size_t, Forest::EdgeHash> edges_;
    size_t n_vertices_;
    size_t level_;
}