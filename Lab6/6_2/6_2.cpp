#include <iostream> 
#include <thread> 
#include <mutex> 
#include <barrier> 
std::mutex mutexA;
std::mutex mutexB;
std::barrier syncPoint(2);
void task1()
{
	std::cout << "Thread 1: locking mutexA\n";
	std::lock_guard<std::mutex> lockA(mutexA);
	std::cout << "Thread 1: mutexA locked\n";
	syncPoint.arrive_and_wait();
	std::cout << "Thread 1: waiting for mutexB\n";
	std::lock_guard<std::mutex> lockB(mutexB);
	std::cout << "Thread 1: both mutexes locked\n";
}
void task2()
{
	std::cout << "Thread 2: locking mutexB\n";
	std::lock_guard<std::mutex> lockB(mutexB);
	std::cout << "Thread 2: mutexB locked\n";
	syncPoint.arrive_and_wait();
	std::cout << "Thread 2: waiting for mutexA\n";
	std::lock_guard<std::mutex> lockA(mutexA);
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
