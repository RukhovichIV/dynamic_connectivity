#include <unordered_map>
#include <unordered_set>

#include "../../src/bst/bst_interface.h"
#include "../../src/bst/cartesian_bst.h"

class EdgeHash {
public:
    size_t operator()(const std::pair<size_t, size_t> &pair) const noexcept {
        return pair.first ^ (pair.second + 0x9e3779b9 + (pair.first << 6ul) + (pair.first >> 2ul));
    }
};

class Forest {
public:
    Forest() = delete;
    ~Forest() = default;

    explicit Forest(size_t n_vertices) : n_vertices_(n_vertices) {
        for (size_t i = 0; i < n_vertices_; ++i) {
            std::vector<size_t> tour({i, i});
            auto tree = std::make_shared<CartesianBST<size_t>>(tour.begin(), tour.end());
            vertices_[i] = tree->begin();
        }
    }

    void AddNewEdge(size_t u, size_t v) {
        if (u >= n_vertices_ || v >= n_vertices_) {
            throw std::runtime_error("No such vertices in graph");
        }
        auto first_pair = vertices_[u].split();
        if (!(++first_pair.first->begin() == first_pair.first->end() &&
              ++first_pair.second->begin() == first_pair.second->end())) {
            std::vector<size_t> add_vertex({u});
            auto add_tree =
                std::make_shared<CartesianBST<size_t>>(add_vertex.begin(), add_vertex.end());
            add_tree->merge(first_pair.second);
            first_pair.second = std::move(add_tree);
        }
        auto edge_iterator = --first_pair.first->end();
        auto second_pair = vertices_[v].split(false);
        auto back_edge_iterator = second_pair.second->begin();
        if (!(++(++second_pair.second->begin()) == second_pair.second->end() &&
              second_pair.first->empty())) {
            std::vector<size_t> add_vertex({v});
            auto add_tree =
                std::make_shared<CartesianBST<size_t>>(add_vertex.begin(), add_vertex.end());
            back_edge_iterator = add_tree->begin();
            second_pair.first->merge(add_tree);
        }
        auto v_delta_pair = (--second_pair.second->end()).split();
        first_pair.first->merge(v_delta_pair.first);
        second_pair.first->merge(first_pair.second);
        first_pair.first->merge(second_pair.first);
        edges_[std::make_pair(u, v)] = std::make_pair(edge_iterator, back_edge_iterator);
    }

    void RemoveExistingEdge(size_t u, size_t v) {
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
        auto first_split = edge_iterators.first.split();
        auto second_split = edge_iterators.second.split();
        auto last_part = second_split.second->begin().split();
        first_split.first->merge(last_part.second);
    }

    bool IsConnected(size_t u, size_t v) {
        if (u >= n_vertices_ || v >= n_vertices_) {
            throw std::runtime_error("No such vertices in graph");
        }
        return vertices_[u].get_root() == vertices_[v].get_root();
    }

private:
    std::unordered_map<size_t, IBST<size_t>::iterator> vertices_;
    std::unordered_map<std::pair<size_t, size_t>,
                       std::pair<IBST<size_t>::iterator, IBST<size_t>::iterator>, EdgeHash>
        edges_;
    size_t n_vertices_;
};