#include "test_cases.cpp"

int main() {
    BenchmarksRunner runner = BenchmarksRunner();
    runner.AddBenchmark(std::make_shared<SimpleBenchmark>());

    runner.RunBenchmarks(BenchmarksRunner::Range(1, 1000, 5, true), "../experiments/");
    return 0;
}
