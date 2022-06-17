#include "test_cases.cpp"

int main() {
    BenchmarksRunner runner = BenchmarksRunner();
    runner.AddBenchmark(std::make_shared<SimpleBenchmark>());
    runner.AddBenchmark(std::make_shared<RandomInsetion>());
    runner.AddBenchmark(std::make_shared<RandomErase>());
    runner.AddBenchmark(std::make_shared<RandomConnection>());

    runner.RunBenchmarks(BenchmarksRunner::Range(5, 100'000, 30, true), "../experiments/");
    return 0;
}
