#include <unordered_map>
#include <unordered_set>

#include "../../src/bst/bst_interface.h"
#include "../../src/bst/cartesian_bst.h"

struct TreeIterator {
public:
    TreeIterator() = delete;

    TreeIterator(std::shared_ptr<IBST<size_t>> tree, IBST<size_t>::iterator it)
        : tree_(tree), it_(it) {
    }

    std::shared_ptr<IBST<size_t>> tree_;
    IBST<size_t>::iterator it_;
};

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
            trees_.emplace(tree);
            vertices_[i] = TreeIterator(tree, tree->begin());
        }
    }

    void AddNewEdge(size_t u, size_t v) {
        if (u >= n_vertices_ || v >= n_vertices_) {
            throw std::runtime_error("No such vertices in graph");
        }
        auto first_part = vertices_[u].tree_;
        trees_.erase(first_part);
        bool got_one_vertex = ++(++first_part->begin()) == first_part->end();
        auto fourth_part = first_part->split(vertices_[u].it_);
        if (!got_one_vertex) {
            std::vector<size_t> add_vertex({u});
            auto add_tree =
                std::make_shared<CartesianBST<size_t>>(add_vertex.begin(), add_vertex.end());
            add_tree->merge(fourth_part);
            fourth_part = std::move(add_tree);
        }
        auto third_part = vertices_[v].tree_;
        trees_.erase(third_part);
        got_one_vertex = ++(++third_part->begin()) == third_part->end();
        bool third_part_is_empty = vertices_[v].it_ == third_part->begin();
        auto second_part =
            (third_part_is_empty ? std::move(third_part) : third_part->split(--vertices_[v].it_));
        second_part->split(--second_part->end());
        if (!got_one_vertex) {
            std::vector<size_t> add_vertex({v});
            if (third_part_is_empty) {
                third_part =
                    std::make_shared<CartesianBST<size_t>>(add_vertex.begin(), add_vertex.end());
            } else {
                third_part->merge(
                    std::make_shared<CartesianBST<size_t>>(add_vertex.begin(), add_vertex.end()));
            }

            third_part->merge(fourth_part);
        } else {
            third_part = std::move(fourth_part);
        }
        first_part->merge(second_part);
        first_part->merge(third_part);
    }

    void RemoveExistingEdge(size_t u, size_t v) {
        if (u >= n_vertices_ || v >= n_vertices_) {
            throw std::runtime_error("No such vertices in graph");
        }
    }
    bool IsConnected(size_t u, size_t v) {
        if (u >= n_vertices_ || v >= n_vertices_) {
            throw std::runtime_error("No such vertices in graph");
        }
        return false;
    }

private:
    std::unordered_set<std::shared_ptr<IBST<size_t>>> trees_;
    std::unordered_map<size_t, TreeIterator> vertices_;
    std::unordered_map<std::pair<size_t, size_t>, TreeIterator, EdgeHash> edges_;
    size_t n_vertices_;
};