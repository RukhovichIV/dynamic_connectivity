#include "../src/graph/dynamic_graph.cpp"
#include "benchmarking_utils.cpp"

class SimpleBenchmark : public BenchmarksRunner::IBenchmark {
public:
    SimpleBenchmark() {
        name_ = "simple_benchmark";
    }
    const std::string& Name() const override {
        return name_;
    }

    void OnInit(size_t n_vertices, std::shared_ptr<std::mt19937> gen) override {
        g_ = std::make_shared<DynamicGraph>(n_vertices);
        gen_ = gen;
        n_vertices_ = n_vertices;
    }

    void Run() override {
        for (size_t i = 1; i < n_vertices_; ++i) {
            g_->insert(i - 1, i);
        }
    }

    void OnEnd() override {
    }

private:
    std::string name_;
    std::shared_ptr<DynamicGraph> g_;
    std::shared_ptr<std::mt19937> gen_;
    size_t n_vertices_;
};

class RandomInsetion : public BenchmarksRunner::IBenchmark {
public:
    RandomInsetion() {
        name_ = "random_insertion";
    }
    const std::string& Name() const override {
        return name_;
    }

    void OnInit(size_t n_vertices, std::shared_ptr<std::mt19937> gen) override {
        g_ = std::make_shared<DynamicGraph>(n_vertices);
        gen_ = gen;
        dist_ = std::uniform_int_distribution<size_t>(0, n_vertices - 1);
        n_vertices_ = n_vertices;

        for (size_t i = 0; i < n_vertices_; ++i) {
            size_t a = dist_(*gen_), b = dist_(*gen_);
            while (a == b) {
                a = dist_(*gen_);
                b = dist_(*gen_);
            }
            edges_.insert(std::make_pair(a, b));
        }
    }

    void Run() override {
        for (const auto& edge : edges_) {
            g_->insert(edge.first, edge.second);
        }
    }

    void OnEnd() override {
    }

private:
    std::string name_;
    std::shared_ptr<DynamicGraph> g_;
    std::shared_ptr<std::mt19937> gen_;
    std::uniform_int_distribution<size_t> dist_;
    size_t n_vertices_;
    std::unordered_set<std::pair<size_t, size_t>, Forest::EdgeHash> edges_;
};

class RandomErase : public BenchmarksRunner::IBenchmark {
public:
    RandomErase() {
        name_ = "random_erase";
    }
    const std::string& Name() const override {
        return name_;
    }

    void OnInit(size_t n_vertices, std::shared_ptr<std::mt19937> gen) override {
        g_ = std::make_shared<DynamicGraph>(n_vertices);
        gen_ = gen;
        dist_ = std::uniform_int_distribution<size_t>(0, n_vertices - 1);
        n_vertices_ = n_vertices;
        for (size_t i = 0; i < n_vertices_; ++i) {
            size_t a = dist_(*gen_), b = dist_(*gen_);
            while (a == b) {
                a = dist_(*gen_);
                b = dist_(*gen_);
            }
            g_->insert(a, b);
            edges_.insert(std::make_pair(a, b));
        }
    }

    void Run() override {
        for (const auto& edge : edges_) {
            g_->erase(edge.first, edge.second);
        }
    }

    void OnEnd() override {
    }

private:
    std::string name_;
    std::shared_ptr<DynamicGraph> g_;
    std::shared_ptr<std::mt19937> gen_;
    std::uniform_int_distribution<size_t> dist_;
    size_t n_vertices_;
    std::unordered_set<std::pair<size_t, size_t>, Forest::EdgeHash> edges_;
};

class RandomConnection : public BenchmarksRunner::IBenchmark {
public:
    RandomConnection() {
        name_ = "random_connection";
    }
    const std::string& Name() const override {
        return name_;
    }

    void OnInit(size_t n_vertices, std::shared_ptr<std::mt19937> gen) override {
        g_ = std::make_shared<DynamicGraph>(n_vertices);
        gen_ = gen;
        dist_ = std::uniform_int_distribution<size_t>(0, n_vertices - 1);
        n_vertices_ = n_vertices;
        for (size_t i = 0; i < n_vertices_; ++i) {
            size_t a = dist_(*gen_), b = dist_(*gen_);
            while (a == b) {
                a = dist_(*gen_);
                b = dist_(*gen_);
            }
            g_->insert(a, b);
            edges_.insert(std::make_pair(a, b));
        }
    }

    void Run() override {
        for (const auto& edge : edges_) {
            g_->is_connected(edge.first, edge.second);
        }
    }

    void OnEnd() override {
    }

private:
    std::string name_;
    std::shared_ptr<DynamicGraph> g_;
    std::shared_ptr<std::mt19937> gen_;
    std::uniform_int_distribution<size_t> dist_;
    size_t n_vertices_;
    std::unordered_set<std::pair<size_t, size_t>, Forest::EdgeHash> edges_;
};