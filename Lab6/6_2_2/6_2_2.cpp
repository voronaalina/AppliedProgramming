#include <iostream> 
#include <thread> 
#include <mutex> 
#include <barrier> 
std::mutex mutexA;
std::mutex mutexB;
std::barrier syncPoint(2);
void task1()
{
	syncPoint.arrive_and_wait();
	std::lock_guard<std::mutex> lockA(mutexA);
	std::lock_guard<std::mutex> lockB(mutexB);
	std::cout << "Thread 1: both mutexes locked\n";
}
void task2()
{
	syncPoint.arrive_and_wait();
	std::lock_guard<std::mutex> lockA(mutexA);
	std::lock_guard<std::mutex> lockB(mutexB);
	std::cout << "Thread 2: both mutexes locked\n";
}
int main()
{
	std::thread t1(task1);
	std::thread t2(task2);
	t1.join();
	t2.join();
	std::cout << "Program finished\n";
	return 0;
}
