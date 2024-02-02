#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

std::mutex m;
std::condition_variable cv;
bool ready = false;
int a = 0;

void A()
{
	std::unique_lock<std::mutex> lck(m);
	for (size_t i = 0; i < 10000; i++)
	{
		a++;
	}
}

void B()
{
	std::unique_lock<std::mutex> lck(m);
	for (size_t i = 0; i < 10000; i++)
	{
		a++;
	}
}

int main()
{
	std::thread t1(A);
	std::thread t2(B);

	t1.join();
	t2.join();

	std::cout << a;
}