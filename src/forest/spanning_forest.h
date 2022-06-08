#include <unordered_map>

#include "../../src/bst/bst_interface.h"
#include "../../src/bst/cartesian_bst.h"

class Forest {
private:
    struct EdgeIterators {
        EdgeIterators() = default;
        EdgeIterators(const IBST<size_t>::iterator& straight, const IBST<size_t>::iterator& back)
            : straight_(straight), back_(back) {
        }

        IBST<size_t>::iterator straight_;
        IBST<size_t>::iterator back_;
    };

    class EdgeHash {
    public:
        size_t operator()(const std::pair<size_t, size_t>& pair) const noexcept {
            return pair.first ^
                   (pair.second + 0x9e3779b9 + (pair.first << 6ul) + (pair.first >> 2ul));
        }
    };

public:
    Forest() = delete;
    ~Forest() = default;

    explicit Forest(size_t n_vertices) : n_vertices_(n_vertices) {
        for (size_t i = 0; i < n_vertices_; ++i) {
            std::vector<size_t> tour({i});
            auto tree = std::make_shared<CartesianBST<size_t>>(tour.begin(), tour.end());
            vertices_[i] = tree->begin();
        }
    }

    void add_new_edge(size_t u, size_t v) {
        if (u == v) {
            throw std::runtime_error("Loop is not a valid edge");
        }
        if (u >= n_vertices_ || v >= n_vertices_) {
            throw std::runtime_error("No such vertices in graph");
        }
        auto first_pair = vertices_[u].split();
        std::vector<size_t> add_vertex({u});
        auto add_tree =
            std::make_shared<CartesianBST<size_t>>(add_vertex.begin(), add_vertex.end());
        if (first_pair.second->empty()) {
            last_vertices_keeper_[u] = add_tree->begin();
        }
        add_tree->merge(first_pair.second);
        first_pair.second = std::move(add_tree);
        auto edge_iterator = --first_pair.first->end();

        auto second_pair = vertices_[v].split();
        auto back_edge_iterator = --second_pair.first->end();
        if (!second_pair.second->empty()) {
            add_vertex = {v};
            add_tree = std::make_shared<CartesianBST<size_t>>(add_vertex.begin(), add_vertex.end());
            add_tree->merge(second_pair.second);
            second_pair.second = std::move(add_tree);
            vertices_[v] = second_pair.second->begin();
            auto iterator_to_remove = --second_pair.second->end();
            last_vertices_keeper_.erase(*iterator_to_remove);
            auto v_delta_pair = (--iterator_to_remove).split();
            first_pair.first->merge(v_delta_pair.first);
        }
        second_pair.first->merge(first_pair.second);
        first_pair.first->merge(second_pair.first);
        edges_[std::make_pair(u, v)] = EdgeIterators(edge_iterator, back_edge_iterator);
    }

    void remove_existing_edge(size_t u, size_t v) {
        if (u == v) {
            throw std::runtime_error("Loop is not a valid edge");
        }
        if (u >= n_vertices_ || v >= n_vertices_) {
            throw std::runtime_error("No such vertices in graph");
        }
        if (edges_.find(std::make_pair(u, v)) == edges_.end()) {
            std::swap(u, v);
        }
        auto edge = std::make_pair(u, v);
        if (edges_.find(edge) == edges_.end()) {
            throw std::runtime_error("No such edge in graph");
        }
        auto edge_iterators = edges_[edge];
        edges_.erase(edge);
        auto first_split = edge_iterators.straight_.split();
        auto second_split = edge_iterators.back_.split();
        if (first_split.second->begin() != --second_split.first->end()) {
            last_vertices_keeper_[v] = --second_split.first->end();
        }
        auto last_part = second_split.second->begin().split();
        last_vertices_keeper_.erase(*last_part.first->begin());
        first_split.first->merge(last_part.second);
    }

    bool is_connected(size_t u, size_t v) {
        if (u == v) {
            throw std::runtime_error("Loop is not a valid edge");
        }
        if (u >= n_vertices_ || v >= n_vertices_) {
            throw std::runtime_error("No such vertices in graph");
        }
        return vertices_[u].get_root() == vertices_[v].get_root();
    }

private:
    std::unordered_map<size_t, IBST<size_t>::iterator> vertices_;
    std::unordered_map<std::pair<size_t, size_t>, EdgeIterators, EdgeHash> edges_;
    std::unordered_map<size_t, IBST<size_t>::iterator> last_vertices_keeper_;
    size_t n_vertices_;
};