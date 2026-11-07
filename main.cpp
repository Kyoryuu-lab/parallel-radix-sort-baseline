#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <algorithm>
#include <iomanip>
#include <tbb/tbb.h>

constexpr int RADIX_BITS = 8;
constexpr int NUM_BUCKETS = 1 << RADIX_BITS;
constexpr uint32_t RADIX_MASK = NUM_BUCKETS - 1;

std::vector<uint32_t> generate_data(size_t n) {
    std::vector<uint32_t> data(n);
    std::mt19937 rng(1337);
    for (size_t i = 0; i < n; ++i) {
        data[i] = rng();
    }
    return data;
}

int compare_uint(const void* a, const void* b) {
    uint32_t arg1 = *static_cast<const uint32_t*>(a);
    uint32_t arg2 = *static_cast<const uint32_t*>(b);
    return (arg1 > arg2) - (arg1 < arg2);
}

void radix_sort_parallel_v1(std::vector<uint32_t>& input) {
    const size_t n = input.size();
    std::vector<uint32_t> output(n);
    const size_t p = tbb::info::default_concurrency();
    const size_t block_size = (n + p - 1) / p;

    for (int shift = 0; shift < 32; shift += RADIX_BITS) {
        // 1. Phase: Histogram-Keys
        std::vector<std::vector<size_t>> hist(p, std::vector<size_t>(NUM_BUCKETS, 0));
        tbb::parallel_for(size_t(0), p, [&](size_t t) {
            size_t start = t * block_size;
            size_t end = std::min(start + block_size, n);
            for (size_t i = start; i < end; ++i) {
                hist[t][(input[i] >> shift) & RADIX_MASK]++;
            }
        });

        // 2. Phase: Scan-Buckets
        std::vector<std::vector<size_t>> offsets(p, std::vector<size_t>(NUM_BUCKETS, 0));
        size_t running_sum = 0;
        for (size_t b = 0; b < NUM_BUCKETS; ++b) {
            for (size_t t = 0; t < p; ++t) {
                offsets[t][b] = running_sum;
                running_sum += hist[t][b];
            }
        }

        // 3. Phase: Rank-and-Permute (Scatter)
        tbb::parallel_for(size_t(0), p, [&](size_t t) {
            size_t start = t * block_size;
            size_t end = std::min(start + block_size, n);
            auto local_offsets = offsets[t];
            for (size_t i = start; i < end; ++i) {
                uint32_t digit = (input[i] >> shift) & RADIX_MASK;
                output[local_offsets[digit]++] = input[i];
            }
        });

        input = output; // Простое копирование для базовой версии
    }
}

int main() {
    const size_t N = 10'000'000;
    const unsigned int p = tbb::info::default_concurrency();

    auto orig_data = generate_data(N);

    auto qsort_data = orig_data;
    auto t1 = std::chrono::high_resolution_clock::now();
    std::qsort(qsort_data.data(), qsort_data.size(), sizeof(uint32_t), compare_uint);
    auto t2 = std::chrono::high_resolution_clock::now();
    double t_seq = std::chrono::duration<double, std::milli>(t2 - t1).count();

    auto v1_data = orig_data;
    auto t3 = std::chrono::high_resolution_clock::now();
    radix_sort_parallel_v1(v1_data);
    auto t4 = std::chrono::high_resolution_clock::now();
    double t_v1 = std::chrono::duration<double, std::milli>(t4 - t3).count();

    std::cout << "std::qsort:            " << std::fixed << std::setprecision(2) << t_seq << " ms\n";
    std::cout << "Parallel RadixSort V1: " << t_v1 << " ms\n";
    std::cout << "Validierung:           " << (std::is_sorted(v1_data.begin(), v1_data.end()) ? "OK" : "FEHLER") << "\n";
    std::cout << "Speedup:               " << (t_seq / t_v1) << "x\n";

    return 0;
}