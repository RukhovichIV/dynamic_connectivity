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