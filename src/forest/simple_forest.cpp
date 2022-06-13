#include <unordered_map>

#include "../../src/bst/bst_interface.h"
#include "../../src/bst/cartesian_bst.h"

class LevelGraph;

class Forest {
public:
    class EdgeHash {
    public:
        size_t operator()(const std::pair<size_t, size_t>& pair) const noexcept {
            return pair.first ^
                   (pair.second + 0x9e3779b9 + (pair.first << 6ul) + (pair.first >> 2ul));
        }
    };

private:
    struct EdgeIterators {
        EdgeIterators() = default;
        EdgeIterators(const IBST<size_t>::iterator& straight, const IBST<size_t>::iterator& back)
            : straight_(straight), back_(back) {
        }

        IBST<size_t>::iterator straight_;
        IBST<size_t>::iterator back_;
    };

public:
    Forest() = delete;
    ~Forest() = default;

    explicit Forest(size_t n_vertices) : n_vertices_(n_vertices) {
        for (size_t i = 0; i < n_vertices_; ++i) {
            std::vector<size_t> tour({i});
            auto tree = std::make_shared<CartesianBST<size_t>>(tour.begin(), tour.end());
            vertices_[i] = tree->begin();
            tree->begin().set_is_vertex(true);
            last_vertices_keeper_[i] = tree->begin();
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
        auto edge_iterator = add_tree->begin();
        vertices_[u].set_is_vertex(false);
        vertices_[u] = edge_iterator;
        vertices_[u].set_is_vertex(true);
        first_pair.first->merge(add_tree);

        auto second_pair = vertices_[v].split();
        add_vertex = {v};
        add_tree = std::make_shared<CartesianBST<size_t>>(add_vertex.begin(), add_vertex.end());
        auto back_edge_iterator = add_tree->begin();
        second_pair.first->merge(add_tree);

        auto iterator_to_remove = --second_pair.second->end();
        last_vertices_keeper_.erase(*iterator_to_remove);
        auto v_delta_pair = iterator_to_remove.split();
        if (v_delta_pair.first->empty()) {
            vertices_[v].set_is_vertex(false);
            vertices_[v] = back_edge_iterator;
            vertices_[v].set_is_vertex(true);
        }

        first_pair.first->merge(v_delta_pair.first);
        second_pair.first->merge(first_pair.second);
        first_pair.first->merge(second_pair.first);
        edges_[std::make_pair(u, v)] = EdgeIterators(edge_iterator, back_edge_iterator);
    }

    std::pair<std::shared_ptr<IBST<size_t>>, std::shared_ptr<IBST<size_t>>> erase_existing_edge(
        size_t u, size_t v) {
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
        auto second_split = (++edge_iterators.back_).split();
        vertices_[u].set_is_vertex(false);
        vertices_[u] = second_split.second->begin();
        vertices_[u].set_is_vertex(true);
        first_split.first->merge(second_split.second);

        auto mid_part = (++first_split.second->begin()).split();
        last_vertices_keeper_[v] = --second_split.first->end();
        return std::make_pair(first_split.first, mid_part.second);
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

    friend class LevelGraph;
};