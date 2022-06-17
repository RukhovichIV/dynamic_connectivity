#include <cassert>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <memory>
#include <random>
#include <vector>

#define NANOSEC_TO_MS_MULT 1e-6

class BenchmarksRunner {
public:
    class IBenchmark {
    public:
        virtual const std::string &Name() const = 0;

        virtual void OnInit(size_t n_vertices, std::shared_ptr<std::mt19937> gen) = 0;
        virtual void Run() = 0;
        virtual void OnEnd() = 0;

        virtual ~IBenchmark() = default;
    };

    struct Range {
        Range() = delete;
        Range(uint64_t begin, uint64_t end, uint64_t step = 1, bool log_scale = false,
              uint64_t n_runs = 5) {
            begin_ = begin;
            end_ = end;
            step_ = step;
            n_runs_ = n_runs;
            log_scale_ = log_scale;
        }
        uint64_t begin_;
        uint64_t end_;
        uint64_t step_;
        uint64_t n_runs_;
        bool log_scale_;
    };

    BenchmarksRunner() {
        std::random_device device;
        gen_ = std::make_shared<std::mt19937>(device());
    }

    void AddBenchmark(std::shared_ptr<IBenchmark> bench) {
        benchmarks_.emplace_back(bench);
    }

    void RunBenchmarks(const Range &nvertex_range, const std::string &out_path) {
        for (auto &bench : benchmarks_) {
            std::ofstream out(out_path + bench->Name() + ".csv");
            out.precision(3);
            out << "n_vertex";
            for (uint64_t i = 0; i < nvertex_range.n_runs_; ++i) {
                out << ", run_" << i;
            }
            if (nvertex_range.log_scale_) {
                assert(nvertex_range.step_ > 1);
                long double step =
                    std::pow((long double)(nvertex_range.end_) / nvertex_range.begin_,
                             1.0l / (nvertex_range.step_ - 1));
                long double cur_approx = nvertex_range.begin_;
                uint64_t prev = 0;
                for (uint64_t i = 1; i < nvertex_range.step_; ++i) {
                    uint64_t cur = std::floor(cur_approx);
                    if (cur == prev) {
                        cur_approx *= step;
                        continue;
                    }
                    out << '\n' << cur;
                    for (uint64_t run_number = 0; run_number < nvertex_range.n_runs_;
                         ++run_number) {
                        double time = RunWithCertainSize(bench, cur);
                        out << ", " << std::fixed << time;
                    }
                    cur_approx *= step;
                    prev = cur;
                }
                out << '\n' << nvertex_range.end_;
                for (uint64_t run_number = 0; run_number < nvertex_range.n_runs_; ++run_number) {
                    double time = RunWithCertainSize(bench, nvertex_range.end_);
                    out << ", " << std::fixed << time;
                }
            } else {
                for (uint64_t i = nvertex_range.begin_; i <= nvertex_range.end_;
                     i += nvertex_range.step_) {
                    out << '\n' << std::to_string(i);
                    for (uint64_t run_number = 0; run_number < nvertex_range.n_runs_;
                         ++run_number) {
                        double time = RunWithCertainSize(bench, i);
                        out << ", " << std::fixed << time;
                    }
                }
            }
            out.close();
        }
    }

private:
    std::vector<std::shared_ptr<IBenchmark>> benchmarks_;
    std::shared_ptr<std::mt19937> gen_;

    double RunWithCertainSize(std::shared_ptr<IBenchmark> bench, size_t n_vertex) {
        try {
            bench->OnInit(n_vertex, gen_);

            auto tbegin = std::chrono::high_resolution_clock::now();
            bench->Run();
            auto tend = std::chrono::high_resolution_clock::now();
            double time =
                std::chrono::duration_cast<std::chrono::nanoseconds>(tend - tbegin).count() *
                NANOSEC_TO_MS_MULT;

            bench->OnEnd();

            return time;
        } catch (std::exception &ex) {
            std::cout << bench->Name() << "\tfailure: " << ex.what() << '\n';
            bench->OnEnd();
        } catch (...) {
            std::cout << bench->Name() << "\tfailure: Unknown exception type";
            bench->OnEnd();
        }
        return -1;
    }
};