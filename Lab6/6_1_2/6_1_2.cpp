#include <iostream> 
#include <thread> 
#include <mutex> 
#include <chrono>

int counter = 0;
std::mutex counterMutex;
const int ITERATIONS = 6 * 100000;
void increment()
{
    for (int i = 0; i < ITERATIONS; ++i)
    {
        std::lock_guard<std::mutex> lock(counterMutex);
        ++counter;
    }
}
int main()
{
    auto start = std::chrono::steady_clock::now();

    std::thread t1(increment);
    std::thread t2(increment);
    t1.join();
    t2.join();

    auto end = std::chrono::steady_clock::now();
    double timeMs = std::chrono::duration<double, std::milli>(end - start).count();

    const int expected = 2 * ITERATIONS;
    std::cout << "Expected value: " << expected << '\n';
    std::cout << "Actual value: " << counter << '\n';
    std::cout << "Time: " << timeMs << " ms\n";
    return 0;
}
