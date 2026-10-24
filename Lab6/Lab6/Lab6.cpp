#include <iostream> 
#include <thread> 
int counter = 0;
const int ITERATIONS = 6 * 100000;
void increment()
{
    for (int i = 0; i < ITERATIONS; ++i)
    {
        ++counter;
    }
}
int main()
{
    std::thread t1(increment);
    std::thread t2(increment);
    t1.join();
    t2.join();
    const int expected = 2 * ITERATIONS;
    std::cout << "Expected value: " << expected << '\n';
    std::cout << "Actual value:   " << counter << '\n';
    return 0;
}

