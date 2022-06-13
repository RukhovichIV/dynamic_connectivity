#include "level_graph.cpp"

class DynamicGraph {
public:
    DynamicGraph() = delete;
    DynamicGraph(size_t n_vertices) {
        size_t level = 0u;
        graphs_.emplace_back(std::make_shared<LevelGraph>(level++, n_vertices, nullptr));
        while (1u << level < n_vertices << 1u) {
            graphs_.emplace_back(std::make_shared<LevelGraph>(level++, n_vertices, graphs_.back()));
        }
    }

    void insert(size_t u, size_t v) {
        graphs_.back()->insert_to_level(u, v);
    }

    void erase(size_t u, size_t v) {
        auto it = graphs_.rbegin();
        while (it != graphs_.rend() &&
               (*it)->edges_at_level_[u].find(v) == (*it)->edges_at_level_[u].end()) {
            ++it;
        }
        if (it == graphs_.rend()) {
            throw std::runtime_error("No such edge in graph");
        }
        std::pair<size_t, size_t> replacement = (*it)->erase_from_level(u, v);
        if (replacement.first == 0 && replacement.second == 0) {
            return;
        }
        while (it++ != graphs_.rbegin()) {
            if (replacement.first != 1 || replacement.second != 1) {
                if (replacement.first == 0 && replacement.second == 0) {
                    throw std::logic_error("Impossible behaviour");
                }
                --it;
                while (it++ != graphs_.rbegin()) {
                    (*it)->erase_and_replace(u, v, replacement.first, replacement.second);
                }
                break;
            }
            replacement = (*it)->erase_from_level(u, v);
        }
    }

    bool is_connected(size_t u, size_t v) {
        return graphs_.back()->is_connected(u, v);
    }

private:
    std::vector<std::shared_ptr<LevelGraph>> graphs_;
};