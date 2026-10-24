#include <iostream>
#include <vector>
#include <future>
#include <chrono>

long long sumSquares(const std::vector<int>& data, size_t begin, size_t end)
{
    long long sum = 0;
    for (size_t i = begin; i < end; ++i)
    {
        sum += (long long)data[i] * data[i];
    }
    return sum;
}

// Послідовна версія
long long sequentialSum(const std::vector<int>& data)
{
    return sumSquares(data, 0, data.size());
}

// Паралельна версія
long long parallelSum(const std::vector<int>& data, int tasks)
{
    std::vector<std::future<long long>> futures;
    size_t part = data.size() / tasks;

    for (int t = 0; t < tasks; ++t)
    {
        size_t begin = t * part;
        size_t end = (t == tasks - 1) ? data.size() : begin + part;

        futures.push_back(std::async(std::launch::async,
            sumSquares, std::cref(data), begin, end));
    }

    long long total = 0;
    for (auto& f : futures)
    {
        total += f.get();
    }
    return total;
}

int main()
{
    const size_t SIZE = 50000000;  
    const int TASKS = 2;           

    std::vector<int> data(SIZE);
    for (size_t i = 0; i < SIZE; ++i)
    {
        data[i] = i % 100;
    }

    auto t1 = std::chrono::steady_clock::now();
    long long seq = sequentialSum(data);
    auto t2 = std::chrono::steady_clock::now();
    long long par = parallelSum(data, TASKS);
    auto t3 = std::chrono::steady_clock::now();

    double tSeq = std::chrono::duration<double, std::milli>(t2 - t1).count();
    double tPar = std::chrono::duration<double, std::milli>(t3 - t2).count();

    std::cout << "Size: " << SIZE << ", tasks: " << TASKS << '\n';
    std::cout << "Sequential result: " << seq << ", time: " << tSeq << " ms\n";
    std::cout << "Parallel result:   " << par << ", time: " << tPar << " ms\n";
    std::cout << "Results equal: " << (seq == par ? "yes" : "NO") << '\n';
    std::cout << "Speedup: " << tSeq / tPar << '\n';
    return 0;
}